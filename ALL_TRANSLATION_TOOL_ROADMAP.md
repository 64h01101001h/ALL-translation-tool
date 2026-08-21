# ALL Translation Tool

> **2026-08-14 note:** this roadmap's to-do ledger is historical — the
> numbered build items have shipped (see docs/FEATURE_STATUS.md).
> The LIVE list is TODO.md (Master Status Board at top). — Integration Roadmap
### v2 · 2026-08-04 · living document (update as tools land)

**ALL = Asian Legacy Library · ACIP = Asian Classics Input Project**

The ALL Translation Tool is the multifunctional desktop suite for working with Tibetan digital texts and translations: search, translation oversight, conversion (wylie ⇄ ACIP ⇄ unicode), phonetics, and linguistic overlays — one shared core (the HGM release package compiled to SQLite + the canonical engines), multiple panes.

**Stack (decided 2026-08-06): C++20 core library (`allcore`) + Qt 6 UI.** See CLAUDE.md §Tech stack.
**Item 1 SHIPPED (2026-08-05):** `tools/build_spine.py` → `build/hgm_spine_v27_2.db` (232 MB; FTS5 headword/english/corpus indexes; provenance preserved verbatim in `entries.raw`); smoke CLI `tools/spine_query.py`; batteries re-run via `tools/validate_engines.py` (ewts 99.03% on 26,318 GT pairs · pron 895/1,308 exact vs card-attested, differences classified · sanskrit 7/7 standard-doc examples — the 4,582-pair LC battery script is NOT banked in the Handoff; Adam to request it from the data project before the sanskrit_convert port).

## Core assets already in hand (HGM DICTIONARY - TRANSLATION APP/Handoff)
- **hgm_dictionary v0.27.1** — 105,638 entries (12,004 HGM-glossed; {C}/{PH}/{GD}/{LC} reference layers; 105,637 with Tibetan unicode; tiered evidence).
- **full_parallel_corpus** — 35,601 aligned Tibetan–English segments across 60+ HGM sources (the concordance/oversight evidence base).
- **Canonical engines (Handoff/tools/)** — pron_engine.py (phonetics, 42/42 battery), hgm_tools.py (ACIP⇄EWTS), ewts_unicode.py (wylie→unicode, 98.88% on 26,318 ground-truth pairs), glyph_decoder.py, hopkins_parser.py, practice_parser.py, generate_package.py.

## Newly scanned tool folders → functionality to integrate

### 1. Gofer (Gofer Developmetn Files) — proximity search semantics
Ted Lemon's rewrite of the Microlytics GOFER used by ACIP for decades (C core + Cocoa UI, 2015). Eight search terms combined with OR and **NEAR-within-N-lines** over directory trees; the TODO notes it was tuned to "the same search GMR is doing."
**Integrate:** the search pane's query model. SQLite FTS5 gives literal/OR/NEAR natively; replicate Gofer's expression grammar (strings, parentheses, "within N lines of") over both the corpus and raw ACIP text files. Port the *semantics*, not the C code. Files of interest: `lex.c`, `expr.c`, `near.c` (the NEAR algorithm), `SearchState.cpp`.

### 2. ACIPHypercontext Tool (Ven. Phil, ObjC, 2012–14) — the linguistic-overlay prototype
Interactive text viewer: parses ACIP texts with embedded markup, per-word dictionary lookup in context, glossary tables (TblGlossary/TblMergedGlossary/PhraseTable), its own pronunciation engine (TibPronunciation.m), the largest formattibetan.m (63KB), Unicode font handling.
**Integrate:** this is the reference UX for the "linguistic overlays" pane — click any word in a text, see stacked definitions/glossary/pronunciation in context. Rebuild on the HGM SQLite spine (tier-ranked HGM equivalents + corpus concordance instead of static glossaries).
**Extraction TODO:** `en.lproj/TibetanDictionary.dic` (4.3MB, .dic tagged format, rich English prose definitions — appears Illuminator-style). Extractable with the LkCh parser approach. ⚠ LICENSING: identify the dictionary before merging into releases (if Illuminator/Tony Duff, in-house-tool use vs redistribution differ).

### 3. tibetan-spellchecker (Elie Roux, CC0) — syllable-legality data
Curated legal-syllable inventories (root.txt, rare.txt, wasurs, proper names, suffixes.json) + excellent docs (standard syllable structure, particle morphology, second-suffix da).
**Integrate (CC0 = no restrictions):**
- Spellcheck pane for input ACIP/unicode texts (hunspell-style syllable validation).
- Validation layer for ewts_unicode.py and the segmenter (legal-syllable check catches wylie typos like the dkhyil/skhyil class found v0.27.1).
- particle-morphology.md cross-checks pron_engine's particle rules.

### 4. Tibetan Translation Tool (THL) — the reference UX + multi-dictionary bundle
`DictionarySearchStandalone.jar` (the classic THL Translation Tool: type Tibetan, get stacked entries from many dictionaries) + `thl-dicts` (thl.wrd 6.3MB wordlist, thl.def 22.4MB definitions — the THL merged bundle: Rangjung Yeshe / Valby / Martin / Hopkins-style entries visible in sample).
**Integrate:** (a) UX reference — the stacked multi-dictionary lookup is exactly the ALL tool's lookup pane; (b) **extraction TODO:** parse thl.wrd/thl.def into per-dictionary reference layers (each tagged by source like {LC}). ⚠ LICENSING: per-dictionary terms vary; fine for in-house lookup, review before shipping in releases.

## New functional requirements (Adam, 2026-08-04)

### A. Input-center document formatter (build from scratch)
Takes raw input data from ACIP input centers → produces a translator-ready formatted document per the formatting guidelines established by Holy Geshe Michael and the publishing/editing team.
**Needed before building:** (1) sample raw input files from the input centers; (2) the formatting guidelines document (or 2–3 finished exemplar documents to reverse-engineer); (3) target output format (docx? InDesign-ready? both?).

### B. Sanskrit multi-converter — ✅ ENGINE BUILT (sanskrit_convert.py, 2026-08-04)
Converts between all working Sanskrit representations: IAST transliteration · ACIP Sanskrit code · EWTS · Tibetan-script unicode · Devanagari · simplified reader pronunciation.
Validated: **92.2% structural match against 4,582 (IAST, ACIP) pairs** embedded in the Lokesh Chandra dictionary; residuals are mostly LC source noise plus rare conventions (cch class). Chain demo: oṃ maṇi padme hūṃ → AOm MA nI PAD ME H'Um → ཨོཾ་མ་ཎི་པད་མེ་ཧཱུཾ → ओं मणि पद्मे हूं.
**Pending:** validate/replace the pronunciation style against the official ACIP Sanskrit pronunciation guide (Adam to supply); vowel-only visarga syllables (āḥ) in the Tibetan chain.

## To-do list (ordered)
0. **Analysis pane (IN PROGRESS — pulled to front, Adam 2026-08-06):** passage-level
   translator-grade analysis per `docs/analysis/PASSAGE_ANALYSIS_TEMPLATE.md` (Adam's
   18-section format). Architecture: engine pre-pass (segmentation + HGM anchors from the
   spine + corpus hits) → prompt assembly → in-app Claude API call (Adam's key) →
   machine QC of the output (items 6/7 reconstruction check, item 8 table diffed against
   spine, ACIP-integrity diff) → rendered report with provenance labels (engine-verified /
   AI-drafted / stylistic-imitation banner on item 17). Wilson's *Translating Buddhism from
   Tibetan* is the grammar framework for the deterministic layer (rule tables authored from
   its case/particle/verb-class system; the book's text is copyrighted — not shipped).
   Reference-library indexing for item 14 DEFERRED (retrieval infra later; until then 14
   renders as "library not indexed").
1. **SQLite spine builder** — compile the v0.27.1 package + corpus into the app database (FTS5). Foundation for every pane. **✅ SHIPPED 2026-08-05** (see header note).
2. **Gofer-style query grammar** over corpus + ACIP files (OR / NEAR-within-N-lines).
3. **Extract TibetanDictionary.dic** (Hypercontext) → identify source, licensing decision, then reference layer.
4. **Extract thl.wrd/thl.def** → per-dictionary reference layers, licensing review.
5. **Spellcheck layer** from tibetan-spellchecker data (CC0) + wylie-typo audit of the master (dkhyil class).
6. **Overlay pane** per Hypercontext UX on the HGM spine (segmentation → tier-ranked equivalents → concordance).
7. **C++ ports** of pron_engine / hgm_tools / ewts_unicode with their validation batteries as unit tests (CTest). *(Was "Swift ports" — superseded 2026-08-06 by the C++20 core + Qt 6 stack decision; see CLAUDE.md §Tech stack.)*
8. **Input-center formatter** (blocked on samples + guidelines — see A above).
9. Standing items: mine the 15,151 corpus-attested LC entries; tier review (~3,900); 4 phantom entries; ~300 wylie/GT defects.

## Sources inventory (device paths)
- Gofer: `ACIP Software Developmeent/Gofer Developmetn Files`
- Hypercontext: `ACIP Software Developmeent/Ven. Phil's Development Folder/Mac_OSX/ACIPHypercontext Tool`
- Spellchecker: `ACIP Software Developmeent/tibetan-spellchecker`
- THL: `Tibetan Translation Tool/`
- LkCh trio: `Ven. Phil's Development Folder/Mac_OSX/{Create,Fix,}LokeshChandra*` (extracted v0.27 → Input/LokeshChandra/)
