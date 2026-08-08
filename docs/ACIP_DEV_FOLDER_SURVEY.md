# ACIP Software Development folder — full survey (2026-08-07)

Swept `~/ACIP Software Developmeent` top to bottom at Adam's request.
Beyond the five tools already integrated (Gofer grammar, Hypercontext
UX + TibetanDictionary.dic, LokeshChandra trio, tibetan-spellchecker,
THL dicts), the folder holds substantially more. Ranked by value.

## Tier 1 — directly actionable

1. **`Acipcode.txt`** — a complete ACIP stack-code inventory (~430
   codes), INCLUDING a separate "ADDITIONAL CODES SUPPORTED BY
   SAMBHOTA" section. This is the substance of the missing Release IV
   chart pages (153–160, empty in our PDF). → Bank; battery: every
   listed code must convert through acipToEwts→wylieToUnicode or be
   flagged as a Sanskrit stack — a completeness proof for the
   converters, and the reference for the input-center formatter.

2. **`UTFC-master` — the Universal Tibetan Font Converter** (Tashi
   Tsering / Trace Foundation, GPL v3): 39 bidirectional `.tbl`
   conversion tables covering 17 encoding schemes — 12 LEGACY FONTS
   (Sambhota 1.0 "Sama" + 2.0 "Dedris", Bandrida, Tongyuan, Beida
   Founder, Huanguang, LTibetan, Jamyang, TCRC BodYig, TibetanMachine,
   TibetanMachineWeb) + 4 transliterations + Unicode. THE
   input-center legacy-document rescue kit, already on disk — broader
   than the py-tiblegenc plan. GPL v3 → integrate as an EXTERNAL TOOL
   (the BDRC-app stage-1 pattern), never linked into allcore.

3. **Ven. Phil's own "Translation Tool" (2012, Obj-C)** — the direct
   ancestor of this whole project: three synchronized panes
   (TibDisplay / XLateDisplay / DictDisplay), a WordMap linking
   Unicode ranges to transliterations, and **per-text glossary
   import** (`fileImportGlossary`, TGlossary with sort keys). Feature
   to adopt: a per-text glossary the translator imports/builds for THE
   OPEN TEXT, outranking the general dictionary in the Overlay card
   (the Hypercontext design's "FROM THIS GLOSSARY:" idea, never yet
   built by us).

4. **Adam's own `hypercontext-master - MY DEVELOPMENT WORK`** —
   includes `syllable-table-with-unicode.txt` (1,973 ACIP↔Unicode
   syllable pairs — an independent battery fixture for the converter)
   and `syllablesThatFailToRender_testFile_HyperContext.txt` (a
   ready-made FONT RENDERING test list — run against
   Noto/BabelStone/SambhotaDege).

5. **Gofer full application source** (not just the grammar): includes
   `SavedSearches.cpp` — Gofer had NAMED SAVED SEARCHES. Feature: save
   named Gofer queries in our Search pane (trivial, persisted in
   QSettings or a small file).

6. **`critical-kangyur-tei-tags/segmentation-units.md`** — scholarly
   definitions of canonical structural units: shloka (4 verses, or 30
   syllables in prose), bampo (300 shlokas), chapter/le'u, etc.
   Feature: a structural-units layer — ACIP canon texts carry BAM PO
   markers; a bampo/shloka counter + outline alongside the sa-bcad
   extractor would be another exists-nowhere-else capability.

## Tier 2 — mine for knowledge

7. **Ven. Phil's catalog toolchain** — `Build ACIP Catalog`,
   `MakeGMRCatalog`, `MakeDergeCatalog`, `MakeKangyurDropBoxCatalog`,
   `FixTengyurCatalogInDropBox`, `AddTGAuthorTitle`,
   `ScanKangyurVolumes` (all Xcode/MySQL): the pipeline that BUILT the
   catalogs we've been consuming. Mine for schema/format knowledge to
   strengthen the titles/bibliography work; `MakeGMRCatalog`
   especially (the GMR catalog's own structure).

8. **TibDict toolchain** — `Tibetan-English Dictionary` (the TibDict
   app itself), `MakeTibDictMarkup`, `CheckTibDictUsedPhrases`:
   provenance chain for the TD reference layer we extracted; the
   used-phrases checker is a nice idea (which dictionary entries
   actually occur in a corpus).

9. **`wysearch`** — Ven. Phil's federated search (MySQL dictionaries +
   FileMaker + LOCAL SPOTLIGHT + TBRC/JSTOR/Google web sources in one
   query). Ancestor of our link-out tier; the Spotlight idea (search
   the user's own machine's materials from the app) is worth stealing.

10. **`OCRProcessing`** (Than Grove, 2013) — NGB OCR-XML batch
    pipeline: catalog-driven splitting of whole OCR'd VOLUMES into
    per-text files with TEI bibl records. The pattern for a future
    batch mode of our Scan pane (volume in, texts out, catalog-keyed).

11. **`tibetan-ocr`** — 2013-era handwritten-manuscript OCR (character
    segmentation, OpenCV) for the Tempangma Kangyur. Historical; our
    BDRC pipeline supersedes it, but its char-width heuristics may
    help if we ever OCR dbu-med manuscripts.

## Tier 3 — additional oracles & references

12. **`NetWylie`** (C#/.NET port incl. THL's `Wylie.java`),
    **`wylie.js`**, **`App-Lingua-BO-Wylie-Transliteration`** (Perl) —
    three more independent Wylie converters for pyewts-style
    diff-oracle cross-checks.
13. **`tip-las`** — Tibetan word segmentation + POS tagging toolkit;
    potential cross-check for Botok/SOAS layers.
14. **`hunspell-bo`** — same Elie Roux syllable data we already ship,
    in hunspell form; nothing new needed.
15. **`tibetan/DCT_format.txt`** — the DCT dictionary file-format spec
    (block-structured); documents the family the TibDict .dic belongs
    to (we reverse-engineered ours; keep as documentation).
16. **`little-lotsawa`** (Django study app), **`tibetanlistings`**
    (Rails), **`ConvertToUTF8`** (Sublime plugin), **`UniView`**,
    **`unihan`** — low value for us.
17. **`Lucene.pptx`**, **`naaclhlt2015.pdf`** (NLP paper),
    **`ascii character set graphic.pdf`**, **`keymap.txt`** — context
    documents.

## Recommended build order

① Acipcode.txt bank + converter-completeness battery (small, closes a
   standing gap) → ② per-text glossary (Overlay/Draft; the ancestor
   feature both Phil's tools had and we lack) → ③ saved searches →
   ④ font render-test from Adam's failure list → ⑤ UTFC external-tool
   lane for legacy documents → ⑥ bampo/shloka structural layer →
   ⑦ catalog-toolchain mining (with the data project).
