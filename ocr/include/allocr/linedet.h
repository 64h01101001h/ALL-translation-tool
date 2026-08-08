// allocr/linedet.h — OCR stage 2, increment A: line detection.
//
// C++ port of BDRC's tibetan-ocr-app line-detection pipeline (MIT code;
// canonical sources: BDRC/Inference.py class LineDetection + BDRC/Utils.py
// preprocess_image/tile_image/binarize/normalize/stitch_predictions —
// ported from the files, proven against the canonical python running the
// same model on the same fixtures; see ocr/tests + tools/
// build_ocr_reference.py). Models: BDRC on Hugging Face, CC BY-NC 4.0,
// with BDRC's direct permission on record (docs/licenses/
// BDRC_OCR_permission.md). Credit BDRC wherever output appears.
//
// allocr is a SEPARATE target linking onnxruntime — ML never enters
// allcore (OCR_DESIGN.md). Output is review material by project rule:
// tagged ocr-derived, never corpus-bound without verification.
#pragma once

#include <cstdint>
#include <memory>
#include <string>
#include <vector>

namespace allocr {

struct Mask {
    int w = 0, h = 0;
    std::vector<uint8_t> data;  // 0 or 255, row-major
};

struct LineBand {
    int top = 0, bottom = 0;  // inclusive row range of a detected line
};

class LineDetector {
public:
    // modelPath: PhotiLines.onnx (or compatible line model)
    explicit LineDetector(const std::string& modelPath, int patchSize = 512);
    ~LineDetector();

    // rgb: interleaved 8-bit RGB, w*h*3 bytes. threshold as in the
    // canonical LineDetection.predict (default 0.9).
    Mask detect(const uint8_t* rgb, int w, int h,
                float threshold = 0.9f) const;

    // simple row-band extraction over the mask: maximal runs of rows
    // whose foreground count >= minPixels, runs shorter than minRows
    // dropped. (The canonical app's contour-based line building comes
    // with the recognition increment.)
    static std::vector<LineBand> rowBands(const Mask& m, int minPixels = 8,
                                          int minRows = 3);

private:
    struct Impl;
    std::unique_ptr<Impl> impl_;
};

}  // namespace allocr
