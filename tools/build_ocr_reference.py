#!/usr/bin/env python3
"""Generate the OCR line-detection oracle references by running the
CANONICAL BDRC pipeline (tibetan-ocr-app, MIT) on the banked fixtures.

    python3 tools/build_ocr_reference.py <path-to-tibetan-ocr-app-clone>

Needs: pip install onnxruntime opencv-python-headless numpy
Clone:  git clone https://github.com/buda-base/tibetan-ocr-app

For each fixture in ocr/tests/fixtures/*.png, writes into build/ocr_ref/:
    <name>.ppm       the decoded RGB input (what the C++ battery reads)
    <name>.mask.pgm  the canonical pipeline's line mask on that input
The mask is produced by the canonical functions themselves
(preprocess_image / tile_image / binarize / normalize, the ONNX session,
sigmoid/threshold/stitch/crop/resize) — mirroring LineDetection.predict
verbatim. The C++ battery (ocr_smoke) diffs against these.
"""
import glob
import os
import sys

import cv2
import numpy as np
import onnxruntime as ort

REPO = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))


def main() -> int:
    if len(sys.argv) < 2:
        print(__doc__)
        return 2
    sys.path.insert(0, sys.argv[1])
    # BDRC.Data imports PySide6.QtGui.QImage purely as a dataclass type;
    # none of the functions we call touch it. Shim it so the oracle needs
    # no Qt bindings (zero behavior change for the ported chain).
    import types
    if "PySide6" not in sys.modules:
        pyside = types.ModuleType("PySide6")
        pyside.__path__ = []  # mark as package
        for sub, names in {"QtGui": ["QImage", "Qt", "QPixmap", "QColor"],
                           "QtWidgets": ["QApplication", "QWidget"],
                           "QtCore": ["QObject", "Signal", "Qt",
                                      "QThreadPool", "QRunnable"]}.items():
            mod = types.ModuleType("PySide6." + sub)
            for n in names:
                setattr(mod, n, object)
            setattr(pyside, sub, mod)
            sys.modules["PySide6." + sub] = mod
        sys.modules["PySide6"] = pyside
    # pyewts is imported at module level by Inference.py but used only by
    # OCRPipeline's converter (we convert with OUR OWN proven chain);
    # it fails to build on py3.12 — shim it the same way.
    if "pyewts" not in sys.modules:
        pw = types.ModuleType("pyewts")
        pw.pyewts = object
        sys.modules["pyewts"] = pw
    from BDRC.Utils import (binarize, normalize, preprocess_image,
                            sigmoid, stitch_predictions, tile_image)
    from BDRC.line_detection import (build_line_data, build_raw_line_data,
                                     extract_line_images,
                                     filter_line_contours,
                                     sort_lines_by_threshold2)
    from BDRC.Inference import CTCDecoder
    from BDRC.Utils import pad_to_height, pad_to_width

    model = os.path.join(REPO,
                         "library/ocr_models/BDRC_PhotiLines/PhotiLines.onnx")
    session = ort.InferenceSession(model,
                                   providers=["CPUExecutionProvider"])
    out_dir = os.path.join(REPO, "build/ocr_ref")
    os.makedirs(out_dir, exist_ok=True)

    # canonical OCR recognition (OCRInference.run mirrored verbatim,
    # config-driven), when the Woodblock model is banked
    import json as _json
    ocr_dir = os.path.join(REPO, "library/ocr_models/BDRC_Woodblock")
    ocr_session = None
    if os.path.exists(os.path.join(ocr_dir, "OCRModel.onnx")):
        ocr_cfg = _json.load(open(os.path.join(ocr_dir, "model_config.json")))
        ocr_session = ort.InferenceSession(
            os.path.join(ocr_dir, ocr_cfg["onnx-model"]),
            providers=["CPUExecutionProvider"])
        decoder = CTCDecoder(ocr_cfg["charset"],
                             ocr_cfg["add_blank"] == "yes")
        IW, IH = ocr_cfg["input_width"], ocr_cfg["input_height"]

        def run_ocr_line(line_image):
            h, _, c = line_image.shape          # _pre_pad
            patch = np.ones((h, h, c), dtype=np.uint8) * 255
            line_image = np.hstack([patch, line_image, patch])
            wr = IW / line_image.shape[1]        # _pad_ocr_line
            hr = IH / line_image.shape[0]
            if wr < hr:
                out = pad_to_width(line_image, IW, IH, "black")
            elif wr > hr:
                out = pad_to_height(line_image, IW, IH, "black")
            else:
                out = pad_to_width(line_image, IW, IH, "black")
            out = cv2.resize(out, (IW, IH), interpolation=cv2.INTER_LINEAR)
            out = binarize(out)                  # _prepare_ocr_line
            out = cv2.cvtColor(out, cv2.COLOR_RGB2GRAY)
            out = out.reshape((1, IH, IW))
            out = (out / 127.5) - 1.0
            out = out.astype(np.float32)
            logits = ocr_session.run([ocr_cfg["output_layer"]],
                                     {ocr_cfg["input_layer"]: out})[0]
            logits = np.squeeze(logits)
            if logits.shape[0] == len(decoder.ctc_vocab):
                logits = np.transpose(logits, axes=[1, 0])
            return decoder.ctc_decode(logits)

    for path in sorted(glob.glob(os.path.join(REPO,
                                              "ocr/tests/fixtures/*.png"))):
        name = os.path.splitext(os.path.basename(path))[0]
        bgr = cv2.imread(path, cv2.IMREAD_COLOR)
        image = cv2.cvtColor(bgr, cv2.COLOR_BGR2RGB)

        # LineDetection.predict, verbatim chain
        padded_img, pad_x, pad_y = preprocess_image(image, 512)
        tiles, y_steps = tile_image(padded_img, 512)
        tiles = [binarize(x) for x in tiles]
        tiles = [normalize(x) for x in tiles]
        batch = np.transpose(np.array(tiles), axes=[0, 3, 1, 2])
        pred = session.run(["output"], {"input": batch})[0]
        pred = np.squeeze(pred, axis=1)
        pred = sigmoid(pred)
        pred = np.where(pred > 0.9, 1.0, 0.0)
        merged = stitch_predictions(pred, y_steps=y_steps)
        x_lim = merged.shape[1] - pad_x
        y_lim = merged.shape[0] - pad_y
        merged = merged[:y_lim, :x_lim]
        merged = cv2.resize(merged,
                            dsize=(image.shape[1], image.shape[0]))
        mask = (merged.astype(np.uint8)) * 255

        # write PPM (P6) + PGM (P5)
        h, w = image.shape[:2]
        with open(os.path.join(out_dir, name + ".ppm"), "wb") as f:
            f.write(b"P6\n%d %d\n255\n" % (w, h))
            f.write(image.astype(np.uint8).tobytes())
        with open(os.path.join(out_dir, name + ".mask.pgm"), "wb") as f:
            f.write(b"P5\n%d %d\n255\n" % (w, h))
            f.write(mask.tobytes())
        fg = int((mask > 0).sum())
        print(f"{name}: {w}x{h}, mask foreground {fg} px "
              f"({100.0 * fg / (w * h):.1f}%)")

        # ---- increment B: canonical line building on the SAME mask ------
        # (run_ocr defaults: k_factor 2.5, bbox_tolerance 4.0, merge_lines)
        rot_img, rot_mask, contours, angle = build_raw_line_data(image, mask)
        gray_rot_mask = cv2.cvtColor(rot_mask, cv2.COLOR_RGB2GRAY)
        filtered = filter_line_contours(rot_mask, contours)
        line_data = [build_line_data(x) for x in filtered]
        sorted_lines, thr = sort_lines_by_threshold2(rot_mask, line_data)
        line_images = extract_line_images(rot_img, sorted_lines, 2.5, 4.0)
        with open(os.path.join(out_dir, name + ".lines.tsv"), "w") as f:
            f.write(f"angle\t{angle}\nthreshold\t{thr}\n")
            for ln in sorted_lines:
                f.write(f"line\t{ln.bbox.x}\t{ln.bbox.y}\t{ln.bbox.w}\t"
                        f"{ln.bbox.h}\t{ln.center[0]}\t{ln.center[1]}\n")
        for i, li in enumerate(line_images):
            lh, lw = li.shape[:2]
            with open(os.path.join(out_dir, f"{name}.line{i}.ppm"),
                      "wb") as f:
                f.write(b"P6\n%d %d\n255\n" % (lw, lh))
                f.write(li.astype(np.uint8).tobytes())
        print(f"  lines: {len(sorted_lines)} (angle {angle:.3f}, "
              f"threshold {thr})")

        if ocr_session is not None:
            with open(os.path.join(out_dir, name + ".ocr.tsv"), "w",
                      encoding="utf-8") as f:
                for i, li in enumerate(line_images):
                    text = run_ocr_line(li).strip().replace("§", " ")
                    f.write(f"{i}\t{text}\n")
                    if i < 2:
                        print(f"    ocr[{i}]: {text[:60]}")
    return 0


if __name__ == "__main__":
    sys.exit(main())
