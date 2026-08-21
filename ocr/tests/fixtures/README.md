# OCR battery fixtures

Two folios of the Derge Pramanavarttika (Tohoku 4210, BDRC scan volume
bdr:I1490), retrieved 2026-08-07 from BDRC's public IIIF service
(license: public domain mark, as shown by the app's scan follow-along):

- `pv_folio_99a.png` — dense text folio (2002×310),
  `https://iiif.bdrc.io/bdr:I1490::14900199.tif/full/max/0/default.png`
- `pv_folio_98b.png` — dense text folio (2002×314) whose mask deskews
  sanely (−0.23°) — the real recognition case,
  `https://iiif.bdrc.io/bdr:I1490::14900198.tif/full/max/0/default.png`
- `pv_folio_94a.png` — sparse microfilm title folio (676×119), the
  stage-2 spike's "hardest case",
  `https://iiif.bdrc.io/bdr:I1490::14900189.tif/full/max/0/default.png`

Recognition additionally needs the Woodblock model downloaded to
`library/ocr_models/BDRC_Woodblock/` (OCRModel.onnx + model_config.json
from huggingface.co/BDRC/Woodblock — CC BY-NC 4.0, BDRC's permission on
record).

The battery (`ocr_smoke`) needs the oracle references in `build/ocr_ref/`
(gitignored): run `tools/build_ocr_reference.py` with a clone of BDRC's
tibetan-ocr-app — it executes the CANONICAL pipeline functions on these
fixtures and dumps the inputs (PPM) and line masks (PGM) the C++ port is
diffed against.
