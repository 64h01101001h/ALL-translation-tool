// recognize.cpp — port of BDRC's OCRInference (see recognize.h). Image
// preparation via OpenCV C++ (same native code as the canonical cv2);
// CTC decoding is a faithful port of pyctcdecode's no-LM beam search
// (Kensho pyctcdecode, Apache-2.0 — the canonical app's decoder),
// float32 arithmetic matching numpy-2 weak promotion.
#include "allocr/recognize.h"

#include <onnxruntime_cxx_api.h>
#include <opencv2/imgproc.hpp>

#include <algorithm>
#include <cmath>
#include <fstream>
#include <map>
#include <sstream>
#include <stdexcept>
#include <tuple>

namespace allocr {

namespace {

// minimal parser for model_config.json's flat shape: string / int values
// and one array of strings ("charset")
struct OcrConfig {
    std::string onnxModel, inputLayer = "input", outputLayer = "output";
    std::string encoder = "wylie";
    int inputWidth = 3200, inputHeight = 100;
    bool squeeze = true, swapHw = false, addBlank = true;
    std::vector<std::string> charset;
};

std::string jsonString(const std::string& s, size_t& i) {
    std::string out;
    ++i;  // opening quote
    while (i < s.size() && s[i] != '"') {
        if (s[i] == '\\' && i + 1 < s.size()) {
            ++i;
            switch (s[i]) {
                case 'n': out += '\n'; break;
                case 't': out += '\t'; break;
                case 'u': {
                    unsigned cp = std::stoul(s.substr(i + 1, 4), nullptr, 16);
                    if (cp < 0x80) out += static_cast<char>(cp);
                    else if (cp < 0x800) {
                        out += static_cast<char>(0xC0 | (cp >> 6));
                        out += static_cast<char>(0x80 | (cp & 0x3F));
                    } else {
                        out += static_cast<char>(0xE0 | (cp >> 12));
                        out += static_cast<char>(0x80 | ((cp >> 6) & 0x3F));
                        out += static_cast<char>(0x80 | (cp & 0x3F));
                    }
                    i += 4;
                    break;
                }
                default: out += s[i];
            }
        } else {
            out += s[i];
        }
        ++i;
    }
    ++i;  // closing quote
    return out;
}

OcrConfig loadConfig(const std::string& path) {
    std::ifstream f(path, std::ios::binary);
    if (!f) throw std::runtime_error("cannot open " + path);
    std::stringstream buf;
    buf << f.rdbuf();
    const std::string s = buf.str();
    OcrConfig c;
    size_t i = 0;
    auto skip = [&] {
        while (i < s.size() && (s[i] == ' ' || s[i] == '\n' || s[i] == '\r' ||
                                s[i] == '\t' || s[i] == ',' || s[i] == ':'))
            ++i;
    };
    while (i < s.size()) {
        if (s[i] != '"') { ++i; continue; }
        std::string key = jsonString(s, i);
        skip();
        if (key == "charset") {
            while (i < s.size() && s[i] != '[') ++i;
            ++i;
            while (true) {
                skip();
                if (i >= s.size() || s[i] == ']') { ++i; break; }
                if (s[i] == '"') c.charset.push_back(jsonString(s, i));
                else ++i;
            }
            continue;
        }
        std::string val;
        if (i < s.size() && s[i] == '"') val = jsonString(s, i);
        else {
            while (i < s.size() && (isdigit(static_cast<unsigned char>(s[i]))))
                val += s[i++];
        }
        if (key == "onnx-model") c.onnxModel = val;
        else if (key == "input_layer") c.inputLayer = val;
        else if (key == "output_layer") c.outputLayer = val;
        else if (key == "encoder") c.encoder = val;
        else if (key == "input_width") c.inputWidth = std::stoi(val);
        else if (key == "input_height") c.inputHeight = std::stoi(val);
        else if (key == "squeeze_channel_dim") c.squeeze = (val == "yes");
        else if (key == "swap_hw") c.swapHw = (val == "yes");
        else if (key == "add_blank") c.addBlank = (val == "yes");
    }
    if (c.onnxModel.empty() || c.charset.empty())
        throw std::runtime_error("bad OCR model config: " + path);
    return c;
}

}  // namespace

struct TextRecognizer::Impl {
    OcrConfig cfg;
    Ort::Env env{ORT_LOGGING_LEVEL_ERROR, "allocr-ocr"};
    Ort::SessionOptions opts;
    std::unique_ptr<Ort::Session> session;
};

TextRecognizer::TextRecognizer(const std::string& modelDir)
    : impl_(new Impl) {
    impl_->cfg = loadConfig(modelDir + "/model_config.json");
    impl_->opts.SetIntraOpNumThreads(4);
    const std::string model = modelDir + "/" + impl_->cfg.onnxModel;
    impl_->session =
        std::make_unique<Ort::Session>(impl_->env, model.c_str(), impl_->opts);
}

TextRecognizer::~TextRecognizer() = default;

const std::string& TextRecognizer::encoder() const { return impl_->cfg.encoder; }

std::string TextRecognizer::recognize(const LineImage& line, bool prePad) const {
    const auto& cfg = impl_->cfg;
    if (line.w <= 0 || line.h <= 0) return "";
    cv::Mat img(line.h, line.w, CV_8UC3,
                const_cast<uint8_t*>(line.rgb.data()));
    img = img.clone();

    // _pre_pad: white HxH patches left and right
    if (prePad) {
        cv::Mat patch(img.rows, img.rows, CV_8UC3, cv::Scalar(255, 255, 255));
        cv::Mat joined;
        cv::hconcat(std::vector<cv::Mat>{patch, img, patch}, joined);
        img = joined;
    }

    // _pad_ocr_line: aspect-preserving pad to input_width x input_height
    // ("black"), then resize INTER_LINEAR to the exact input size
    const double wr = static_cast<double>(cfg.inputWidth) / img.cols;
    const double hr = static_cast<double>(cfg.inputHeight) / img.rows;
    cv::Mat padded;
    if (wr < hr || wr == hr) {  // pad_to_width
        double ratio = static_cast<double>(cfg.inputWidth) / img.cols;
        cv::Mat tmp;
        cv::resize(img, tmp,
                   cv::Size(cfg.inputWidth,
                            static_cast<int>(img.rows * ratio)),
                   0, 0, cv::INTER_LINEAR);
        const int middle = (cfg.inputHeight - tmp.rows) / 2;
        cv::Mat up = cv::Mat::zeros(std::max(middle, 0), cfg.inputWidth,
                                    CV_8UC3);
        cv::Mat down = cv::Mat::zeros(
            std::max(cfg.inputHeight - tmp.rows - middle, 0), cfg.inputWidth,
            CV_8UC3);
        cv::vconcat(std::vector<cv::Mat>{up, tmp, down}, padded);
    } else {  // pad_to_height
        double ratio = static_cast<double>(cfg.inputHeight) / img.rows;
        cv::Mat tmp;
        cv::resize(img, tmp,
                   cv::Size(static_cast<int>(img.cols * ratio),
                            cfg.inputHeight),
                   0, 0, cv::INTER_LINEAR);
        const int middle = (cfg.inputWidth - tmp.cols) / 2;
        cv::Mat left = cv::Mat::zeros(cfg.inputHeight, std::max(middle, 0),
                                      CV_8UC3);
        cv::Mat right = cv::Mat::zeros(
            cfg.inputHeight, std::max(cfg.inputWidth - tmp.cols - middle, 0),
            CV_8UC3);
        cv::hconcat(std::vector<cv::Mat>{left, tmp, right}, padded);
    }
    cv::Mat sized;
    cv::resize(padded, sized, cv::Size(cfg.inputWidth, cfg.inputHeight), 0, 0,
               cv::INTER_LINEAR);

    // _prepare_ocr_line: binarize (canonical adaptive Gaussian 51/13),
    // gray, normalize to [-1, 1]
    cv::Mat gray;
    cv::cvtColor(sized, gray, cv::COLOR_RGB2GRAY);
    cv::Mat bw;
    cv::adaptiveThreshold(gray, bw, 255, cv::ADAPTIVE_THRESH_GAUSSIAN_C,
                          cv::THRESH_BINARY, 51, 13);
    // canonical normalize: float64 arithmetic, then cast to float32
    std::vector<float> input(static_cast<size_t>(cfg.inputHeight) *
                             cfg.inputWidth);
    for (int y = 0; y < cfg.inputHeight; ++y)
        for (int x = 0; x < cfg.inputWidth; ++x)
            input[static_cast<size_t>(y) * cfg.inputWidth + x] =
                static_cast<float>(bw.ptr<uint8_t>(y)[x] / 127.5 - 1.0);

    std::array<int64_t, 3> shape{1, cfg.inputHeight, cfg.inputWidth};
    Ort::MemoryInfo mem =
        Ort::MemoryInfo::CreateCpu(OrtArenaAllocator, OrtMemTypeDefault);
    Ort::Value in = Ort::Value::CreateTensor<float>(
        mem, input.data(), input.size(), shape.data(), shape.size());
    const char* inNames[] = {cfg.inputLayer.c_str()};
    const char* outNames[] = {cfg.outputLayer.c_str()};
    auto out = impl_->session->Run(Ort::RunOptions{nullptr}, inNames, &in, 1,
                                   outNames, 1);
    auto info = out[0].GetTensorTypeAndShapeInfo();
    auto dims = info.GetShape();
    const float* logits = out[0].GetTensorData<float>();
    // squeeze to (time, vocab); transpose if (vocab, time)
    std::vector<int64_t> sq;
    for (int64_t d : dims)
        if (d != 1) sq.push_back(d);
    if (sq.size() != 2) return "";
    const int vocabSize = static_cast<int>(cfg.charset.size()) +
                          (cfg.addBlank ? 1 : 0);
    int64_t T = sq[0], V = sq[1];
    bool transposed = false;
    if (static_cast<int>(sq[0]) == vocabSize && sq[1] != vocabSize) {
        T = sq[1];
        V = sq[0];
        transposed = true;
    }
    // CTC decode: faithful port of pyctcdecode's no-LM beam search
    // (Kensho pyctcdecode, Apache-2.0 — the canonical app's decoder).
    // Vocab as the canonical CTCDecoder builds it: [" "] + charset — the
    // model's blank column decodes as " " and is stripped at the end
    // (pyctcdecode has no ""-blank here, so CTC collapse happens through
    // the space channel exactly as in the canonical app). Constants:
    // beam 100, prune_logp -10, token_min_logp -5; per-frame log-softmax
    // clipped at ln(1e-15); duplicate beams merged by log-sum-exp.
    std::vector<std::string> vocab;
    vocab.reserve(cfg.charset.size() + 1);
    vocab.push_back(" ");
    for (const auto& ch : cfg.charset) vocab.push_back(ch);

    struct Beam {
        std::string text, wordPart, lastChar;
        float score = 0.0f;   // numpy-2 weak promotion keeps float32
    };
    auto mergeTokens = [](const std::string& a, const std::string& b) {
        if (b.empty()) return a;
        if (a.empty()) return b;
        return a + " " + b;
    };
    std::vector<Beam> beams{Beam{}};
    std::vector<float> lp(V);
    for (int64_t t = 0; t < T; ++t) {
        // log_softmax over the frame in float32 (scipy-style, as
        // pyctcdecode's _log_softmax runs on the float32 logits),
        // clipped like pyctcdecode
        float maxL = -1e30f;
        for (int64_t v = 0; v < V; ++v) {
            const float x = transposed ? logits[v * T + t] : logits[t * V + v];
            lp[v] = x;
            maxL = std::max(maxL, x);
        }
        float sum = 0;
        for (int64_t v = 0; v < V; ++v) sum += std::exp(lp[v] - maxL);
        const float lse = std::log(sum);
        int maxIdx = 0;
        for (int64_t v = 0; v < V; ++v) {
            lp[v] = std::max((lp[v] - maxL) - lse,
                             static_cast<float>(std::log(1e-15)));
            if (lp[v] > lp[maxIdx]) maxIdx = static_cast<int>(v);
        }
        std::map<std::tuple<std::string, std::string, std::string>, Beam> merged;
        auto addBeam = [&](Beam b) {
            auto key = std::make_tuple(b.text, b.wordPart, b.lastChar);
            auto it = merged.find(key);
            if (it == merged.end()) {
                merged.emplace(key, std::move(b));
            } else {  // _sum_log_scores (math.log/exp on float32 inputs)
                float s1 = it->second.score, s2 = b.score;
                it->second.score = static_cast<float>(
                    s1 >= s2 ? s1 + std::log(1 + std::exp(double(s2) - s1))
                             : s2 + std::log(1 + std::exp(double(s1) - s2)));
            }
        };
        for (int64_t v = 0; v < V; ++v) {
            if (lp[v] < -5.0f && static_cast<int>(v) != maxIdx) continue;
            const std::string& ch = vocab[v];
            for (const Beam& b : beams) {
                Beam nb = b;
                nb.score += lp[v];
                if (b.lastChar == ch) {
                    nb.lastChar = ch;                 // same token: collapse
                } else if (ch == " ") {
                    nb.text = mergeTokens(b.text, b.wordPart);
                    nb.wordPart.clear();              // word boundary
                    nb.lastChar = ch;
                } else {
                    nb.wordPart += ch;                // continue the word
                    nb.lastChar = ch;
                }
                addBeam(std::move(nb));
            }
        }
        std::vector<Beam> next;
        next.reserve(merged.size());
        float best = -1e30f;
        for (auto& [k, b] : merged) best = std::max(best, b.score);
        for (auto& [k, b] : merged)
            if (b.score >= best - 10.0f) next.push_back(std::move(b));
        std::sort(next.begin(), next.end(),
                  [](const Beam& a, const Beam& b) { return a.score > b.score; });
        if (next.size() > 100) next.resize(100);
        beams = std::move(next);
    }
    // finalize: flush word parts, merge, take the best
    std::map<std::string, float> finals;
    for (const Beam& b : beams) {
        const std::string full = mergeTokens(b.text, b.wordPart);
        auto it = finals.find(full);
        if (it == finals.end()) {
            finals.emplace(full, b.score);
        } else {
            float s1 = it->second, s2 = b.score;
            it->second = static_cast<float>(
                s1 >= s2 ? s1 + std::log(1 + std::exp(double(s2) - s1))
                         : s2 + std::log(1 + std::exp(double(s1) - s2)));
        }
    }
    std::string text;
    float bestScore = -1e30f;
    for (const auto& [t2, s] : finals)
        if (s > bestScore) {
            bestScore = s;
            text = t2;
        }
    // canonical ctc_decode(): strip the vocab-space channel entirely
    {
        std::string noSpace;
        for (char c2 : text)
            if (c2 != ' ') noSpace += c2;
        text = noSpace;
    }
    // canonical post-processing: strip + '§' -> ' '
    size_t b = text.find_first_not_of(" \t\n");
    size_t e = text.find_last_not_of(" \t\n");
    text = b == std::string::npos ? "" : text.substr(b, e - b + 1);
    std::string outText;
    for (size_t i = 0; i < text.size();) {
        if (text.compare(i, 2, "\xC2\xA7") == 0) {  // '§'
            outText += ' ';
            i += 2;
        } else {
            outText += text[i++];
        }
    }
    return outText;
}

}  // namespace allocr
