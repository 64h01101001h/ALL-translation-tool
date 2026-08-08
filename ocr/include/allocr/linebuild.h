// allocr/linebuild.h — OCR stage 2, increment B: line building.
//
// C++ port of BDRC's line_detection.py chain (MIT):
//   build_raw_line_data: mask -> page rotation angle (minAreaRect angle
//     statistics) -> deskew image+mask -> contours (area > 10)
//   filter_line_contours: w > 1% of image width, h > 10
//   build_line_data: approxPolyDP-optimized contour + bbox + center
//   sort_lines_by_threshold2: slice-based threshold estimate ->
//     mean-y grouping -> optional convex-hull merge of chunks -> reading
//     order
//   extract_line_images: per-line filled-contour mask, rect dilation with
//     adaptive k, mask-and-crop (every all-zero row/column removed, as
//     upstream)
// Geometry runs on OpenCV C++ — the same native code the canonical cv2
// calls — so this stage is bit-identical by construction; the battery
// still diffs everything against the canonical python on the fixtures.
#pragma once

#include <cstdint>
#include <string>
#include <vector>

#include "allocr/linedet.h"

namespace allocr {

struct OcrLine {
    int x = 0, y = 0, w = 0, h = 0;   // bbox in deskewed-image coordinates
    int cx = 0, cy = 0;               // center
};

struct LineImage {
    int w = 0, h = 0;
    std::vector<uint8_t> rgb;         // interleaved, extracted line strip
};

struct PageLines {
    double angle = 0.0;               // estimated page rotation (degrees)
    int width = 0, height = 0;        // deskewed page size (== input size)
    std::vector<uint8_t> deskewedRgb; // rotated page the lines index into
    std::vector<OcrLine> lines;       // reading order
    std::vector<LineImage> images;    // one per line, same order
};

// image: interleaved RGB; mask: the LineDetector output (same w/h).
// kFactor/bboxTolerance/mergeLines follow run_ocr's defaults (2.5 / 4.0 /
// true). forceAngle (optional): override the estimated page rotation —
// an OPT-IN deviation from the canonical pipeline (whose deskew math
// mis-fires under modern OpenCV; see OCR_DESIGN.md) — callers must label
// its use; nullptr = canonical behavior.
PageLines buildLines(const uint8_t* rgb, int w, int h, const Mask& mask,
                     double kFactor = 2.5, double bboxTolerance = 4.0,
                     bool mergeLines = true, const double* forceAngle = nullptr);

}  // namespace allocr
