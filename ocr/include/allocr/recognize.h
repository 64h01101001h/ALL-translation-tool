// allocr/recognize.h — OCR stage 2, increment C: CTC text recognition.
//
// C++ port of BDRC's OCRInference (Inference.py: _pre_pad, _pad_ocr_line,
// _prepare_ocr_line, _predict, _decode with the model's own charset from
// model_config.json). Decoding is a faithful C++ port of pyctcdecode's
// no-LM beam search (the canonical app's decoder; Apache-2.0): beam 100,
// prune -10, token-min -5, per-frame log-softmax, log-sum-exp beam
// merging — proven line-for-line against the canonical on the fixtures
// (residue: <=1 near-tie line per fixture from cross-runtime logit
// low-bit differences, printed by the battery).
// Models: BDRC on Hugging Face (CC BY-NC 4.0 + BDRC's permission on
// record). Output is Wylie (encoder "wylie"); conversion to Unicode goes
// through OUR proven chain, never theirs. Review material by rule.
#pragma once

#include <memory>
#include <string>
#include <vector>

#include "allocr/linebuild.h"

namespace allocr {

// A decoded word with its CTC frame span and its x-range on the line
// strip (pyctcdecode's own text_frames tracking — the canonical decoder
// computes these; the BDRC app just discards them). x is in LINE-STRIP
// pixels; LineImage::colMap maps strip x back to deskewed-page x.
struct WordSpan {
    std::string text;                 // word as decoded ('§' -> ' ' applied)
    int frameBeg = -1, frameEnd = -1; // CTC frame indices [beg, end)
    double x0 = 0.0, x1 = 0.0;        // strip-x range
};

// test hook: charset entry count parsed from a model config (-1 on
// error) — exercises both the array and BDRC string charset shapes
int ocrConfigCharsetCount(const std::string& path);

class TextRecognizer {
public:
    // modelDir must hold model_config.json + the ONNX file it names
    explicit TextRecognizer(const std::string& modelDir);
    ~TextRecognizer();

    // line: an extracted line image (interleaved RGB). Returns the
    // recognized text in the model's encoding (Wylie), stripped, with
    // the canonical '§' -> ' ' replacement applied. wordsOut (optional)
    // receives the best beam's word spans (see WordSpan).
    std::string recognize(const LineImage& line, bool prePad = true,
                          std::vector<WordSpan>* wordsOut = nullptr) const;

    const std::string& encoder() const;  // e.g. "wylie"

private:
    struct Impl;
    std::unique_ptr<Impl> impl_;
};

}  // namespace allocr
