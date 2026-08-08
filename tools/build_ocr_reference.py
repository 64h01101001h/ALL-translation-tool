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
    from BDRC.Utils import (binarize, normalize, preprocess_image,
                            sigmoid, stitch_predictions, tile_image)

    model = os.path.join(REPO,
                         "library/ocr_models/BDRC_PhotiLines/PhotiLines.onnx")
    session = ort.InferenceSession(model,
                                   providers=["CPUExecutionProvider"])
    out_dir = os.path.join(REPO, "build/ocr_ref")
    os.makedirs(out_dir, exist_ok=True)

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
    return 0


if __name__ == "__main__":
    sys.exit(main())
