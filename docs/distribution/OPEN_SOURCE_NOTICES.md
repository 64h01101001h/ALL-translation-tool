# Diamond Cutter Translation Tool — Open-Source & Data Notices

*Ships with every release. This document names every third-party
component and data source the application uses, with its license
and how it appears in the tool. Maintained per release; if a
component enters or leaves the tool, this document changes in the
same commit.*

Version: 0.27.2 · August 2026

**Where the machine-readable version lives.** Every path in the DMG is
enumerated, with its licence and its anchor in this document, in
`docs/distribution/PAYLOAD_MANIFEST.txt`. `tools/manifest_check.py`
fails the press on any staged file that manifest does not name — this
prose and that table are checked against each other on every build.
Every binary inside the app bundle is enumerated, with its version and
SPDX licence id, into `Contents/Resources/licenses/BUNDLED_COMPONENTS.tsv`
at press time, beside the licence texts themselves.

---

## Software components

### Bundled binary components

These ship *inside* `Diamond Cutter Translation Tool.app`. The list is
generated from the staged bundle by `tools/bundle_licenses.py`, which
maps every Mach-O object to the Homebrew formula that produced it and
fails the press on any object it cannot map. Versions below are those
of the 1.0.0-rc.1 press; the exact versions of any given build are in
that build's `BUNDLED_COMPONENTS.tsv`.

| Component | Version | License (SPDX) | Use |
|---|---|---|---|
| Qt 6 (Core, Gui, Widgets, Network, DBus, OpenGL, Pdf, PdfWidgets, Qml, QmlMeta, QmlModels, QmlWorkerScript, Quick, and the cocoa/offscreen/imageformats/iconengines/tls/styles plugins) | 6.11.1 | LGPL-3.0-only (with BSD-3-Clause, GPL-2.0-only and GPL-3.0-only-with-Qt-GPL-exception-1.0 parts) | the user interface toolkit, dynamically linked and shipped unmodified inside the app bundle |
| HarfBuzz | 14.3.0 | MIT | Tibetan complex-script shaping |
| Graphite2 | 1.3.15 | MIT OR MPL-2.0 OR LGPL-2.1-or-later OR GPL-2.0-or-later | smart-font shaping under HarfBuzz |
| FreeType | 2.14.3 | FTL (BSD-style) | font rasterisation |
| ICU4C | 78.3 | Unicode-3.0 | Unicode tables and collation under Qt |
| GLib / GObject / GIO / GModule / GThread | 2.88.2 | LGPL-2.1-or-later | Qt's glib event-loop and network-information backends |
| gettext (libintl) | 1.0 | LGPL-2.1-or-later (runtime library) | message catalogues under glib |
| PCRE2 | 10.47 | BSD-3-Clause | regular expressions under Qt and glib |
| D-Bus | 1.16.2 | AFL-2.1 OR GPL-2.0-or-later | Qt's DBus module |
| double-conversion | 3.4.0 | BSD-3-Clause | number formatting under Qt |
| md4c | 0.5.3 | MIT | Markdown rendering under Qt |
| libpng | 1.6.58 | libpng-2.0 | PNG images |
| libjpeg-turbo | 3.2.0 | IJG AND Zlib AND BSD-3-Clause | JPEG images |
| libtiff | 4.7.2 | libtiff (BSD-style) | TIFF images |
| libwebp / sharpyuv | 1.6.0 | BSD-3-Clause | WebP images |
| libmng | 2.0.3 | Zlib | MNG images |
| JasPer | 4.2.9 | JasPer-2.0 | JPEG-2000 images |
| Little CMS (lcms2) | 2.19 | MIT | colour management under the image plugins |
| Brotli | 1.2.0 | MIT | content decompression under Qt Network |
| zstd | 1.5.7 | (BSD-3-Clause OR GPL-2.0-only) AND BSD-2-Clause AND MIT | compression under Qt |
| xz (liblzma) | 5.8.3 | 0BSD AND GPL-2.0-or-later | compression under Qt |
| libb2 | 0.98.1 | CC0-1.0 | BLAKE2 hashing under Qt |
| OpenSSL (libssl, libcrypto) | 3.6.3 | Apache-2.0 | TLS for the Analysis pane's report call and the BDRC image fetches |
| ONNX Runtime | 1.28.0 | MIT | runs the text-recognition models locally |
| ONNX | 1.22.0 | Apache-2.0 | the model format ONNX Runtime loads |
| Abseil (76 libraries) | 20260107.1 | Apache-2.0 | ONNX Runtime's base library |
| Protocol Buffers (protobuf-lite, utf8_range, utf8_validity) | 35.1 | BSD-3-Clause | ONNX Runtime's model deserialisation |
| RE2 | 2025-11-05 | BSD-3-Clause | regular expressions under ONNX Runtime |
| OpenCV (core, flann, geometry, imgproc) | 5.0.0 | Apache-2.0 | page/line image preparation for the Scan pane |
| OpenBLAS | 0.3.34 | BSD-3-Clause AND BSD-2-Clause-Views AND BSD-3-Clause-Open-MPI AND BSD-2-Clause | linear algebra under OpenCV and ONNX Runtime |
| Intel oneTBB | 2023.1.0 | Apache-2.0 | parallelism under OpenCV |
| LLVM OpenMP (libomp) | 22.1.8 | MIT (Apache-2.0-with-LLVM-exception upstream) | parallelism under ONNX Runtime |
| GCC runtime (libgfortran, libquadmath, libgcc_s) | 16.1.0 | GPL-3.0-or-later **WITH GCC-exception-3.1** (the Runtime Library Exception) | pulled in by OpenBLAS's Fortran kernels. The Runtime Library Exception is what permits this application to ship these libraries without itself becoming GPL |

**LGPL notice (Qt, GLib, gettext).** These libraries are used unmodified
and are dynamically linked. You may replace them with your own build:
the bundle is not signed with a Developer ID, and the libraries live in
`Contents/Frameworks/` where they can be substituted directly. Their
complete licence texts are in `Contents/Resources/licenses/`; where a
text is missing from a build, `BUNDLED_COMPONENTS.tsv` says so on that
component's row rather than leaving the gap silent, and the upstream
texts are at <https://www.gnu.org/licenses/>. Source for these
libraries is Homebrew's, at the formula and version named in the table.

### Used but not bundled

| Component | License | Use |
|---|---|---|
| SQLite + FTS5 | Public domain | the data spine and all full-text indexes. **Not shipped**: the app links macOS's own `/usr/lib/libsqlite3.dylib` |
| zlib | Zlib | via macOS's own `/usr/lib/libz.1.dylib` |
| Apple system frameworks (AppKit, Metal, ImageIO, IOKit, OpenGL, DiskArbitration, UniformTypeIdentifiers) | Apple SDK terms | the macOS platform |
| tesseract (optional, user-installed) | Apache-2.0 | Devanagari OCR in the Convert/Sanskrit panes; the app calls it if the user has installed it and says so plainly if not |

### Ported, not linked

| Component | License | Use |
|---|---|---|
| Botok (ported to C++ from the Python original by Esukhia/OpenPecha) | Apache-2.0 | Tibetan word segmentation (reference layer). The port is this project's code; Botok's data tables ship under `data/botok/` with their licence |

## Recognition models

| Source | License | Use |
|---|---|---|
| BDRC line-detection model (PhotiLines) and Woodblock recognition model | CC BY-NC 4.0, used with BDRC's permission (docs/licenses/BDRC_OCR_permission.md) | the Scan pane and Input-pane OCR; credited in the interface wherever output appears; all output marked review material. The tool is distributed free of charge and never sold, which is what places this bundling inside the NC terms (Adam's ruling, 2026-08-22, docs/SHIP.md) |

## Reference data layers

| Layer | License / status | Use |
|---|---|---|
| Mahāvyutpatti (DILA TEI edition) | CC BY-SA 3.0 | Sanskrit–Tibetan equivalences on entry cards |
| Monlam word lists (via openpecha) | Apache 2.0 | attestation layer ("this is a real word") + segmenter lexicon |
| tibetan-spellchecker syllable data | CC0 | the classical syllable-legality check (red waves, spelling doubts) |
| TibSchol abbreviations (rKTs) | CC0 | bskungs-yig expansion on cards |
| Tibetan verbs database | CC0 | verb reference layer |
| SOAS POS lexicon (Tibetan in Digital Communication, SOAS University of London) | CC BY 4.0 — Zenodo DOI 10.5281/zenodo.574876, banked in data/soas_pos/README.md. **The original terms have not yet been located** (docs/SHIP.md open item 4); the layer ships in team builds and blocks a market press until they are | Wilson-dots disambiguation |
| Treasury of Lives biography ids (data/extracted/tol_links.json) | identifiers only, no Treasury of Lives content is copied or redistributed — the app links out to their site. Sourced from BDRC's own owl:sameAs and from Wikidata property P4138 joined on P2477 (Wikidata data is CC0); URL pattern per Wikidata's P1630 formatter. Built by tools/widen_tol_links.py | the "Treasury of Lives biography" link on person cards and in the ⌘K palette; Wikidata-sourced matches are labeled as such in the interface |
| Whitney, *Roots* (1885) and *Sanskrit Grammar* (1879), via CDSL | public domain (texts); CDSL encoding terms recorded at bank time | Sanskrit root analyses and grammar link-outs |
| Das, *Tibetan-English Dictionary* (1902) | public domain (published 1902) | the bookmarked scan in the Lookup pane, plus the derived headword and page indexes |
| Jäschke, *Tibetan-English Dictionary* (1881) | public domain (published 1881) | page index for link-outs; the scan itself is not redistributed |
| BDRC catalog metadata (person records, part numbers, scan identifiers) | BDRC's open catalogue data, harvested via their public BLMP/SPARQL endpoints; identifiers, names, dates and part numbers only — no BDRC images or texts are copied | person cards, scan follow-along routing, subject headings |
| THL catalog numbers (Degé number ⇄ Tohoku number concordance) | **numbers only.** No THL prose, catalogue record or dictionary text is redistributed; the raw THL catalogue records used to build the concordance are held locally and are stripped from every DMG by the press | safe deep-linking to THL by title-matched number |
| Esukhia derge-kangyur e-text | public domain per that project's README §License; the e-text itself is **not** in the DMG — the shipped index carries byte offsets into a corpus the user supplies | Toh-number navigation when the user has the e-text |
| THL dictionary payload (Waldo, Rangjung Yeshe, Valby, Dan Martin, Hopkins layers, etc.) and the ACIPHypercontext TibetanDictionary.dic extraction | per-dictionary terms, **licensing review still pending**. These are unlicensed compilations for in-house lookup. They are compiled into build/reference.db, which is included in **TEAM builds only** — labelled "(local only)" in the interface, excluded from market builds by the press, and **not for redistribution**: a team DMG must not be forwarded outside the ALL/ACIP team. The source extractions (`thl_dicts.jsonl`, `tibetan_dictionary_dic.jsonl`) and the raw THL catalogue record trees are stripped from every DMG | Lookup/Overlay reference definitions |
| Lokesh Chandra Tibetan–Sanskrit layer | extracted from ACIP's own materials | Sanskrit reference on cards |
| BDRC woodblock scans (IIIF) | public-domain images, BDRC terms; fetched live, cached locally, attributed in-pane | scan follow-along |
| Buddhist canonical texts (ACIP Releases) | ACIP's own preserved corpus | the Library |
| 84000 glossary (Steinert cumulative export + official live harvest 2026-08-20, 29,383 terms) | CC BY 4.0 — 84000's canonical Terms of Use table (all-data/Terms_of_Use.md rev. 2023-06-14) | the labeled 84000 block on entry cards, with links to the original entries and "attested in" work links |
| 84000 Translation Memory (400,745 aligned Ti–En segments, 388 canonical texts; ingested 2026-08-20) | CC BY 4.0 — same Terms table ("Translation memory … CC-BY"); the TM repo README's BY-NC-ND boilerplate defers to those Terms | published-translation comparanda shown beside (never inside) the HGM corpus concordance. The cache is rebuilt locally from the CC BY source on first use; no TM database ships |
| Fonts (bundled Tibetan faces) | SIL OFL (each face's license banked in data/fonts/FONTS.md) | Tibetan script display |
| Honorific register table | the ALL's own data (HGM's register work), data/honorifics/README.md | honorific register warnings on cards |
| Colloquial pronunciation table | the ALL's own data (HGM's pronunciation conventions), data/pron_colloquial/README.md | the colloquial pronunciation line in the Phonetics pane |
| Idiom list | the ALL's own data | idiom detection in the Overlay pane |
| Teaching-moments index (Diamond Cutter Classics + The Knowledge Base YouTube channels) | Geshe Michael Roach's own organizations' teachings, harvested captions-only with ALL's authorization (data/teaching/PROVENANCE.md); links open the original recordings | timecoded "teaching this term" links on entry cards; machine-located, labeled |

## The dictionary and corpus

The Geshe Michael Roach Dictionary, aligned corpus, registers, and published
apparatus are the Asian Legacy Library's own data, © their
respective authors, distributed only within the ALL/ACIP team.
Geshe Michael Roach's English is the binding layer; nothing in this
tool relicenses or republishes it.

## Redistribution: what a team DMG is, and is not

A DMG pressed in **team** mode is an internal ALL/ACIP artifact. It
carries `build/reference.db` — unlicensed third-party compilations kept
for in-house lookup — and the ALL's own dictionary and corpus. **Do not
forward a team DMG outside the ALL/ACIP team.** The `README.txt` inside
every team DMG says the same thing in the same words.

A DMG pressed in **market** mode omits `reference.db` and refuses to
build at all while any shipped layer's terms are unresolved; the press
prints the exact rows that block it. The open items are listed in
`docs/SHIP.md` and are, today: the redistribution stance for the HGM
release package, the teachings-index consent line, the SOAS POS terms,
and the per-file itemisation of `data/extracted`.

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
