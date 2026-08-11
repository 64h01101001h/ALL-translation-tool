# ALL Translation Tool — Open-Source & Data Notices

*Ships with every release. This document names every third-party
component and data source the application uses, with its license
and how it appears in the tool. Maintained per release; if a
component enters or leaves the tool, this document changes in the
same commit.*

Version: 0.27.2 · August 2026

---

## Software components

| Component | License | Use |
|---|---|---|
| Qt 6 (Widgets, Gui, Core, Network) | LGPL v3 | the user interface toolkit, dynamically linked; Qt frameworks ship inside the app bundle unmodified |
| HarfBuzz (bundled with Qt) | MIT-style | Tibetan complex-script shaping |
| SQLite + FTS5 | Public domain | the data spine and all full-text indexes |
| ONNX Runtime | MIT | runs the text-recognition models locally |
| Botok (ported to C++ from the Python original by Esukhia/OpenPecha) | Apache 2.0 | Tibetan word segmentation (reference layer) |
| tesseract (optional, user-installed) | Apache 2.0 | Devanagari OCR in the Convert/Sanskrit panes |

## Recognition models

| Source | License | Use |
|---|---|---|
| BDRC line-detection model (PhotiLines) and Woodblock recognition model | CC BY-NC 4.0, used with BDRC's permission (docs/licenses/BDRC_OCR_permission.md) | the Scan pane and Input-pane OCR; credited in the interface wherever output appears; all output marked review material |

## Reference data layers

| Layer | License / status | Use |
|---|---|---|
| Mahāvyutpatti (DILA TEI edition) | CC BY-SA 3.0 | Sanskrit–Tibetan equivalences on entry cards |
| Monlam word lists (via openpecha) | Apache 2.0 | attestation layer ("this is a real word") + segmenter lexicon |
| tibetan-spellchecker syllable data | CC0 | the classical syllable-legality check (red waves, spelling doubts) |
| TibSchol abbreviations (rKTs) | CC0 | bskungs-yig expansion on cards |
| Tibetan verbs database | CC0 | verb reference layer |
| SOAS POS lexicon | research use, banked with provenance | Wilson-dots disambiguation |
| Whitney, *Roots* (1885) and *Sanskrit Grammar* (1879), via CDSL | public domain (texts); CDSL encoding terms recorded at bank time | Sanskrit root analyses and grammar link-outs |
| THL dictionary payload (Waldo, Rangjung Yeshe, Valby, Dan Martin, Hopkins layers, etc.) | per-dictionary terms, LOCAL-ONLY display pending review — marked "(local only)" in the interface and NOT redistributed | Lookup/Overlay reference definitions |
| Lokesh Chandra Tibetan–Sanskrit layer | extracted from ACIP's own materials | Sanskrit reference on cards |
| BDRC woodblock scans (IIIF) | public-domain images, BDRC terms; fetched live, cached locally, attributed in-pane | scan follow-along |
| Buddhist canonical texts (ACIP Releases) | ACIP's own preserved corpus | the Library |
| Fonts (bundled Tibetan faces) | SIL OFL (each face's license banked in data/fonts/FONTS.md) | Tibetan script display |
| Teaching-moments index (Diamond Cutter Classics + The Knowledge Base YouTube channels) | Geshe Michael Roach's own organizations' teachings, harvested captions-only with ALL's authorization (data/teaching/PROVENANCE.md); links open the original recordings | timecoded "teaching this term" links on entry cards; machine-located, labeled |

## The dictionary and corpus

The HGM dictionary, aligned corpus, registers, and published
apparatus are the Asian Legacy Library's own data, © their
respective authors, distributed only within the ALL/ACIP team.
Geshe Michael Roach's English is the binding layer; nothing in this
tool relicenses or republishes it.

## Cross-validation acknowledgments

The engines were independently cross-checked against pyewts,
THL's wylie.js and Wylie.java, and Svante Janson's calendar work as
implemented in Henning's KCK — with thanks to those authors; none
of their code ships in the application.

## A note on what this tool never does

Reference layers are displayed with their source labels and are
never merged into, or presented as, the ALL's own dictionary
content. Layers marked "local only" above await licensing review
before any redistribution; they render only from the user's local
data folder.

---

*Corrections welcome: if you are the author of a component listed
here and want the attribution changed, contact the Asian Legacy
Library and it will be changed in the next release.*
