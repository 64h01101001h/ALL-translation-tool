// linedet.cpp — port of BDRC's line-detection pipeline (see linedet.h).
// Canonical chain (Inference.py LineDetection.predict + Utils.py):
//   preprocess_image: clamp (width>4096 landscape -> resize to 4096;
//     height>2048 portrait -> resize to 2048; height<512 -> resize to 512)
//     then pad right/bottom with 255 to patch multiples
//   tile_image: row-major 512x512 tiles
//   binarize: RGB->GRAY, cv2.adaptiveThreshold(GAUSSIAN, block=51, C=13)
//   normalize: /255 float32
//   NHWC->NCHW -> ONNX ["input"]->["output"] -> squeeze -> sigmoid ->
//   threshold -> stitch tiles -> crop padding -> resize to original size
// cv2-matching details: RGB2GRAY uses OpenCV's fixed-point luma;
// adaptive threshold uses a replicate-border separable Gaussian
// (ksize 51 => sigma 8.0 per cv2's formula) rounded to uint8, compare
// src > mean - C. Resizes are float bilinear (cv2 INTER_LINEAR is
// fixed-point; sub-level differences are absorbed by the tolerance
// battery, which reports exact agreement).
#include "allocr/linedet.h"

#include <onnxruntime_cxx_api.h>

#include <algorithm>
#include <cmath>
#include <stdexcept>

namespace allocr {

namespace {

struct Gray {
    int w = 0, h = 0;
    std::vector<uint8_t> px;
};

struct Rgb {
    int w = 0, h = 0;
    std::vector<uint8_t> px;  // interleaved
};

// cv2 fixed-point luma: (R*4899 + G*9617 + B*1868 + 8192) >> 14
Gray toGray(const Rgb& img) {
    Gray g;
    g.w = img.w;
    g.h = img.h;
    g.px.resize(static_cast<size_t>(img.w) * img.h);
    for (size_t i = 0, n = g.px.size(); i < n; ++i) {
        const uint8_t* p = &img.px[i * 3];
        g.px[i] = static_cast<uint8_t>(
            (p[0] * 4899 + p[1] * 9617 + p[2] * 1868 + 8192) >> 14);
    }
    return g;
}

// float bilinear resize (align cv2's pixel-center convention)
Rgb resizeRgb(const Rgb& src, int nw, int nh) {
    Rgb out;
    out.w = nw;
    out.h = nh;
    out.px.resize(static_cast<size_t>(nw) * nh * 3);
    const double sx = static_cast<double>(src.w) / nw;
    const double sy = static_cast<double>(src.h) / nh;
    for (int y = 0; y < nh; ++y) {
        double fy = (y + 0.5) * sy - 0.5;
        int y0 = static_cast<int>(std::floor(fy));
        double wy = fy - y0;
        int y1 = std::min(y0 + 1, src.h - 1);
        y0 = std::max(y0, 0);
        for (int x = 0; x < nw; ++x) {
            double fx = (x + 0.5) * sx - 0.5;
            int x0 = static_cast<int>(std::floor(fx));
            double wx = fx - x0;
            int x1 = std::min(x0 + 1, src.w - 1);
            x0 = std::max(x0, 0);
            for (int c = 0; c < 3; ++c) {
                double v =
                    (1 - wy) * ((1 - wx) * src.px[(y0 * src.w + x0) * 3 + c] +
                                wx * src.px[(y0 * src.w + x1) * 3 + c]) +
                    wy * ((1 - wx) * src.px[(y1 * src.w + x0) * 3 + c] +
                          wx * src.px[(y1 * src.w + x1) * 3 + c]);
                out.px[(static_cast<size_t>(y) * nw + x) * 3 + c] =
                    static_cast<uint8_t>(std::lround(std::clamp(v, 0.0, 255.0)));
            }
        }
    }
    return out;
}

std::vector<float> resizeMaskF(const std::vector<float>& src, int sw, int sh,
                               int nw, int nh) {
    std::vector<float> out(static_cast<size_t>(nw) * nh);
    const double sx = static_cast<double>(sw) / nw;
    const double sy = static_cast<double>(sh) / nh;
    for (int y = 0; y < nh; ++y) {
        double fy = (y + 0.5) * sy - 0.5;
        int y0 = static_cast<int>(std::floor(fy));
        double wy = fy - y0;
        int y1 = std::min(y0 + 1, sh - 1);
        y0 = std::max(y0, 0);
        for (int x = 0; x < nw; ++x) {
            double fx = (x + 0.5) * sx - 0.5;
            int x0 = static_cast<int>(std::floor(fx));
            double wx = fx - x0;
            int x1 = std::min(x0 + 1, sw - 1);
            x0 = std::max(x0, 0);
            out[static_cast<size_t>(y) * nw + x] = static_cast<float>(
                (1 - wy) * ((1 - wx) * src[y0 * sw + x0] +
                            wx * src[y0 * sw + x1]) +
                wy * ((1 - wx) * src[y1 * sw + x0] +
                      wx * src[y1 * sw + x1]));
        }
    }
    return out;
}

// cv2 getGaussianKernel: sigma = 0.3*((ksize-1)*0.5 - 1) + 0.8
std::vector<double> gaussKernel(int ksize) {
    const double sigma = 0.3 * ((ksize - 1) * 0.5 - 1) + 0.8;
    std::vector<double> k(ksize);
    double sum = 0;
    const double c = (ksize - 1) / 2.0;
    for (int i = 0; i < ksize; ++i) {
        k[i] = std::exp(-((i - c) * (i - c)) / (2 * sigma * sigma));
        sum += k[i];
    }
    for (double& v : k) v /= sum;
    return k;
}

// adaptiveThreshold(GAUSSIAN_C, THRESH_BINARY, block, C):
// dst = src > round(gaussBlur_replicate(src)) - C ? 255 : 0
Gray adaptiveGaussianBinarize(const Gray& g, int block, int C) {
    const auto k = gaussKernel(block);
    const int r = block / 2;
    std::vector<double> tmp(static_cast<size_t>(g.w) * g.h);
    // horizontal pass, replicate border
    for (int y = 0; y < g.h; ++y)
        for (int x = 0; x < g.w; ++x) {
            double s = 0;
            for (int i = -r; i <= r; ++i) {
                int xx = std::clamp(x + i, 0, g.w - 1);
                s += k[i + r] * g.px[static_cast<size_t>(y) * g.w + xx];
            }
            tmp[static_cast<size_t>(y) * g.w + x] = s;
        }
    Gray out;
    out.w = g.w;
    out.h = g.h;
    out.px.resize(g.px.size());
    // vertical pass + compare
    for (int y = 0; y < g.h; ++y)
        for (int x = 0; x < g.w; ++x) {
            double s = 0;
            for (int i = -r; i <= r; ++i) {
                int yy = std::clamp(y + i, 0, g.h - 1);
                s += k[i + r] * tmp[static_cast<size_t>(yy) * g.w + x];
            }
            const int mean = static_cast<int>(std::lround(s));
            const size_t ix = static_cast<size_t>(y) * g.w + x;
            out.px[ix] = g.px[ix] > mean - C ? 255 : 0;
        }
    return out;
}

}  // namespace

struct LineDetector::Impl {
    Ort::Env env{ORT_LOGGING_LEVEL_ERROR, "allocr"};
    Ort::SessionOptions opts;
    std::unique_ptr<Ort::Session> session;
    int patch = 512;
};

LineDetector::LineDetector(const std::string& modelPath, int patchSize)
    : impl_(new Impl) {
    impl_->patch = patchSize;
    impl_->opts.SetIntraOpNumThreads(4);
    impl_->session = std::make_unique<Ort::Session>(impl_->env,
                                                    modelPath.c_str(),
                                                    impl_->opts);
}

LineDetector::~LineDetector() = default;

Mask LineDetector::detect(const uint8_t* rgb, int w, int h,
                          float threshold) const {
    if (w <= 0 || h <= 0) throw std::runtime_error("empty image");
    const int patch = impl_->patch;
    Rgb img;
    img.w = w;
    img.h = h;
    img.px.assign(rgb, rgb + static_cast<size_t>(w) * h * 3);

    // preprocess_image: clamp / minimum height, then pad with 255
    if (img.w > img.h && img.w > 4096) {
        double ratio = 4096.0 / img.w;
        img = resizeRgb(img, 4096,
                        static_cast<int>(img.h * ratio));
    } else if (img.h > img.w && img.h > 2048) {
        double ratio = 2048.0 / img.h;
        img = resizeRgb(img, static_cast<int>(img.w * ratio), 2048);
    } else if (img.h < patch) {
        double ratio = static_cast<double>(patch) / img.h;
        img = resizeRgb(img, static_cast<int>(img.w * ratio), patch);
    }
    const int padX =
        (img.w + patch - 1) / patch * patch - img.w;
    const int padY =
        (img.h + patch - 1) / patch * patch - img.h;
    Rgb padded;
    padded.w = img.w + padX;
    padded.h = img.h + padY;
    padded.px.assign(static_cast<size_t>(padded.w) * padded.h * 3, 255);
    for (int y = 0; y < img.h; ++y)
        std::copy(img.px.begin() + static_cast<size_t>(y) * img.w * 3,
                  img.px.begin() + static_cast<size_t>(y + 1) * img.w * 3,
                  padded.px.begin() + static_cast<size_t>(y) * padded.w * 3);

    const int xSteps = padded.w / patch;
    const int ySteps = padded.h / patch;
    const int tiles = xSteps * ySteps;

    // canonical order: tile FIRST, then binarize each tile independently
    // (adaptive-threshold border effects live at tile edges, as upstream),
    // then normalize; channels identical (binarized gray replicated to RGB)
    std::vector<float> input(static_cast<size_t>(tiles) * 3 * patch * patch);
    for (int ty = 0; ty < ySteps; ++ty)
        for (int tx = 0; tx < xSteps; ++tx) {
            const int t = ty * xSteps + tx;
            Rgb tile;
            tile.w = patch;
            tile.h = patch;
            tile.px.resize(static_cast<size_t>(patch) * patch * 3);
            for (int y = 0; y < patch; ++y)
                std::copy(padded.px.begin() +
                              (static_cast<size_t>(ty * patch + y) * padded.w +
                               tx * patch) * 3,
                          padded.px.begin() +
                              (static_cast<size_t>(ty * patch + y) * padded.w +
                               tx * patch + patch) * 3,
                          tile.px.begin() + static_cast<size_t>(y) * patch * 3);
            Gray bw = adaptiveGaussianBinarize(toGray(tile), 51, 13);
            for (int y = 0; y < patch; ++y)
                for (int x = 0; x < patch; ++x) {
                    const float v =
                        bw.px[static_cast<size_t>(y) * patch + x] / 255.0f;
                    const size_t base =
                        static_cast<size_t>(t) * 3 * patch * patch;
                    const size_t off = static_cast<size_t>(y) * patch + x;
                    input[base + off] = v;
                    input[base + static_cast<size_t>(patch) * patch + off] = v;
                    input[base + 2 * static_cast<size_t>(patch) * patch + off] =
                        v;
                }
        }

    Ort::MemoryInfo mem =
        Ort::MemoryInfo::CreateCpu(OrtArenaAllocator, OrtMemTypeDefault);
    std::array<int64_t, 4> shape{tiles, 3, patch, patch};
    Ort::Value in = Ort::Value::CreateTensor<float>(
        mem, input.data(), input.size(), shape.data(), shape.size());
    const char* inNames[] = {"input"};
    const char* outNames[] = {"output"};
    auto out = impl_->session->Run(Ort::RunOptions{nullptr}, inNames, &in, 1,
                                   outNames, 1);
    const float* logits = out[0].GetTensorData<float>();
    // output [tiles, 1, patch, patch] -> sigmoid -> threshold -> stitch
    std::vector<float> stitched(static_cast<size_t>(padded.w) * padded.h);
    for (int t = 0; t < tiles; ++t) {
        const int ty = t / xSteps, tx = t % xSteps;
        for (int y = 0; y < patch; ++y)
            for (int x = 0; x < patch; ++x) {
                const float s =
                    1.0f / (1.0f + std::exp(-logits[
                        static_cast<size_t>(t) * patch * patch +
                        static_cast<size_t>(y) * patch + x]));
                stitched[static_cast<size_t>(ty * patch + y) * padded.w +
                         tx * patch + x] = s > threshold ? 1.0f : 0.0f;
            }
    }
    // crop padding, resize to ORIGINAL input size
    std::vector<float> cropped(static_cast<size_t>(img.w) * img.h);
    for (int y = 0; y < img.h; ++y)
        std::copy(stitched.begin() + static_cast<size_t>(y) * padded.w,
                  stitched.begin() + static_cast<size_t>(y) * padded.w + img.w,
                  cropped.begin() + static_cast<size_t>(y) * img.w);
    std::vector<float> final_ = resizeMaskF(cropped, img.w, img.h, w, h);

    Mask m;
    m.w = w;
    m.h = h;
    m.data.resize(static_cast<size_t>(w) * h);
    for (size_t i = 0; i < m.data.size(); ++i)
        m.data[i] = final_[i] >= 0.5f ? 255 : 0;
    return m;
}

std::vector<LineBand> LineDetector::rowBands(const Mask& m, int minPixels,
                                             int minRows) {
    std::vector<LineBand> bands;
    int start = -1;
    for (int y = 0; y <= m.h; ++y) {
        int count = 0;
        if (y < m.h)
            for (int x = 0; x < m.w; ++x)
                count += m.data[static_cast<size_t>(y) * m.w + x] ? 1 : 0;
        const bool on = y < m.h && count >= minPixels;
        if (on && start < 0) start = y;
        if (!on && start >= 0) {
            if (y - start >= minRows) bands.push_back({start, y - 1});
            start = -1;
        }
    }
    return bands;
}

}  // namespace allocr
