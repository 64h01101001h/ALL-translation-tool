# Diamond Cutter Translation Tool — Developer Handbook

*Under the hood, for the development team. Current as of
**2026-08-22, v1.0.0-rc.1** (70 test suites green, corpus v32).
Since the v0.12.0 revision below: the Word-style ribbon band across
every pane; Translation Dossiers (a resumable desk per text); the
Glossary Workbench; team comments; conflict-safe shared stores with
Dropbox "conflicted copy" absorption; a constitution suite that
enforces process rules as tests; a geometry inquisition and
visual-regression gate in the press; stateful sequence fuzzing
(20,000 randomized real actions survived); and honesty invariants
swept across the whole dictionary rather than sampled. The sections
below remain accurate on architecture; version-specific counts may
lag. Companion documents:
`README.md` (user-facing pane guide),
`DIAMOND_CUTTER_TRANSLATION_TOOL_ROADMAP.md` (master plan), `TODO.md` (the
single live ledger — includes the merged attack queue as of
2026-08-14; `docs/LOOSE_ENDS_AUDIT.md` is now a historical pointer),
`docs/whitepaper/` (the non-technical story), and the design docs
referenced per subsystem below. The user-side counterpart is
`data/help/USER_MANUAL.md`, which ships inside the app's Help.*

## 1. Architecture in one page

A C++20 static library (**`allcore`**, `core/`) owns everything
deterministic: SQLite/FTS5 data access, the ported conversion
engines, the grammar layer, search, and every algorithm. A
single-file **Qt 6 Widgets** app (`app/main.cpp`, ~34,600 lines)
is the UI over it — **21 panes in six workflow groups** (Read /
Translate / Research / Learn / Input / Community, mirrored by the
menu bar), no logic that isn't presentation, network calls only in
the app layer (BDRC IIIF/scans, the Analysis pane's Anthropic
call, collection updates, and the Files pane's remote
connections — everything else runs fully offline). A second static
library (**`allocr`**, `ocr/`) carries line detection/recognition
(onnxruntime + OpenCV); the app builds without it (`ALL_HAVE_OCR`
gates the OCR pane). `engines/` holds the canonical Python engine
copies the C++ ports were transcribed from (the data project owns
their evolution). Python lives in `tools/` (72 scripts) for data
preparation and validation only; nothing Python runs at app
runtime. Build is CMake (Qt 6.11.1, C++20, system SQLite with
FTS5 asserted at runtime); `ctest` runs 70 suites (62 core + 7
app-level + 1 ocr); a headless `allanalyze` CLI mirrors the
Analysis pipeline. The packaged app installs to
`/Applications/Diamond Cutter Translation Tool/`.

```
data releases (Geshe Michael Roach Dictionary project, versioned)     external APIs
        │                                       (BDRC · Anthropic ·
        ▼                                        user's own servers)
tools/build_spine.py ──► build/hgm_spine_v27_2.db          │
        │                     ▲                            │
        │              ┌──────┴──────┐                     ▼
        └──────────►   │   allcore   │  ◄──── app/main.cpp (Qt UI)
 data/extracted/*.json │  (C++20)    │
 (banks, joins, TSVs)  └─────────────┘
```

**The prime directive of the data flow**: this repo *consumes*
versioned releases from the Geshe Michael Roach Dictionary project (a separate
Cowork effort); it never forks or edits the data. Engine defects
found here are *filed back* to the data project, which fixes the
canonical Python engines and re-releases; we then re-port and
re-prove.

## 2. The four inviolable rules, as they appear in code

1. **hgm_gloss is binding** — reference layers (Monlam, verbs DB,
   Hopkins, LC, 84000, Das/Jäschke, catalog English, R6 subjects)
   are *display-labeled* and never feed lookup ranking, parse
   decisions, or the dictionary. Every external layer carries its
   source and license inline. Machine work may MATCH the master's
   English from corpus evidence; it may never COMPOSE it (the
   pecha exporter's English interlinear appears only where the
   corpus attests the exact segment).
2. **Never reconstruct engines from spec** — every port names its
   source file and ships with a battery derived from the source's
   own outputs (§7). Case studies: the calendar's `nyifac` (a
   zero assumption broke 76 fixtures; the value was in T3.C:111);
   the month-name male-year exception (underivable; explicitly
   coded in T4.C); botok's dead placeholder loop (documented and
   deliberately not ported).
3. **Never guess** — unconvertible input returns `⟨token⟩ +
   false`; quotation detection below threshold is silence;
   author/subject pairs require join support; pre-epoch calendar
   dates return invalid and the epoch year 2009 is *refused* by
   `kckYearMonths` (its own oracle prints it corrupted); 4-syllable
   "meters" are excluded from verse grouping because they are
   arithmetically indistinguishable from enumeration lists; the
   style and typography checkers flag and never auto-fix.
4. **Provenance is sacred** — every corpus segment carries
   `source_wave` (+ `catalog_no`, `join_support`, `subject_bo`
   where applicable); HANDOFF.md in the data project logs every
   wave; `third_party/` banks upstream sources with license +
   PROVENANCE; generated data is tagged
   (`tibetan_source: generated-ewts-v27-1` etc.).

## 3. Repository layout

```
core/include/allcore/*.h   public API, one header per subsystem
core/src/*.cpp             implementations
core/tests/*_smoke.cpp     the 62 batteries (see §7)
core/tools_*.cpp           headless CLIs: wynorm, libindex_cli
app/main.cpp               the whole UI (~34.6k lines, pane classes)
ocr/                       allocr line-detect/recognize (onnx+OpenCV)
engines/                   canonical Python engines (port sources)
tools/*.py, *.sh           72 prep/oracle scripts + the press script
data/                      shipped payloads: fonts, extracted banks,
                           help (in-app manual/guides), das, teaching,
                           botok, spellcheck, soas_pos, whitney,
                           abbreviations, honorifics, standards…
third_party/kck_henning/   Henning KCK sources (MIT) + shims +
                           build_macos.sh + 26 oracle year fixtures
docs/                      designs, research, standards, apparatus,
                           distribution, whitepaper, licenses, drafts
library/                   user data (collections, caches, .index.db)
editions/                  gitignored: Esukhia eKangyur/eTengyur (PD)
build/                     spine (238MB) + reference.db (77MB) +
                           oracle reference dumps (botok 165MB,
                           sanskrit, pron) + whisper models
library/.index.db          the 2.36GB FTS index over the collections
VERSION                    single version source (CMake configure dep)
dist/, cmake-build-release/  gitignored FOREVER (disk postmortem)
```

## 4. The spine (SQLite/FTS5)

Built by `tools/build_spine.py` from the release package + corpus
(currently `full_parallel_corpus_v32.json.gz`, 42,199 segments) +
reverse index. Key tables:

- `entries` — 105,634 dictionary rows. **Gotcha**: physical column
  order matters; all reads go through `kEntryCols` (a `SELECT *`
  ordering bug once corrupted tense data).
- `entries_fts`, `reverse_index`, `meta`.
- `corpus_segments(id, course, seq, acip, wylie, english, raw)`
  with `corpus_fts(wylie, english, acip, wylie_norm)` —
  `tokenize="unicode61 tokenchars ''''"` (apostrophe is a token
  char: `pa'i` is ONE token; the library index is kept identical,
  §10). `wylie_norm` is the affix-stripped shadow (only stored
  where it differs; 30,046 rows), produced at build time through
  the **`wynorm` CLI** so the C++ rules are the single authority.
  FTS5 matches all columns by default → every search is
  affix-tolerant with no query rewriting.

Courses of special provenance: `TITLK/TITLT/TITLS/TITLR` (catalog
title pairs, v29–30), `SUBJ` (bilingual subject headings, v31),
`AUTH` (join-verified author names, v32).

## 5. allcore subsystems (header → what it does → battery)

| Header | Purpose | Battery |
|---|---|---|
| `spine.h` | DB access; `lookup` tries wylie → variants → Tibetan → ACIP | spine_smoke |
| `engines.h` | ported converters: `acipToEwts`, `wylieToUnicode`, `pronounce`, Sanskrit chain | engines_battery (full-dictionary parity vs Python oracles) |
| `lattice.h` | Overlay construction: tokenize → match → spans + nesting depth (§9) | lattice_smoke |
| `particles.h` | Wilson particle tables, fused endings (ACIP *and* wylie suffixes, case-disjoint) | via lattice/reader |
| `reader.h`, `verbclass.h`, `wilsonparse.h` | trainer chunking, verb classes, full Wilson parse + **all twelve dot designations** (S·OM·NA·NN·APP·VB·V·ADV·NOM·VOC·C-num·UP·SP) | reader_smoke |
| `gofer.h` + `src/gofer_ast.h` | Gofer grammar (phrases/OR/NEAR) over spine or file trees | gofer_smoke |
| `libindex.h` | Library full-text index (§10); now also feeds Analysis item 14 citations | libindex_smoke |
| `quotation.h` | attested quotation detection (§11) | quotation_smoke (silent skip now fails loudly) |
| `verbstems.h` | CC0 verbs DB reference layer (unicode-keyed) | verbstems_smoke |
| `lexicon.h` | Monlam word-list attestation (449k forms) | lexicon_smoke |
| `affixnorm.h` | BDRC lucene-bo affix stripping; wylie projection decided by the unicode port | affixnorm_smoke |
| `tibcal.h` | year-level 60-cycle (rabjung/element-animal, both directions) | tibcal_smoke |
| `tibcal_day.h` | day-level Modern Karana + month enumeration (§12) | tibcal_day_smoke (26 oracle years) |
| `tibexport.h` | print export, Mixed Nuts prep, filename decoding, BDRC URLs, STD-002/007 + **DCC-guide bibliography templates (Tibetan + full Sanskrit canonical)** | tibexport_smoke (pins the guide's own examples) |
| `thl_phonetics.h` | THL Simplified Phonetics (Germano & Tournadre), word-scoped; the app's `thlPhrase()` wrapper segments phrases via Botok | 139/139 on the standard's own examples |
| `botok.h` (`botok_*.cpp`) | botok segmentation port: chunker → trie → tokenizer → Segmenter facade + memory-lean `SegTrie` (HGM + Monlam lexicon, proven segmentation-identical) | botok_smoke, botok_tok_smoke |
| `analysis.h` | AI analysis prompt assembly: engine pre-pass (anchors + corpus + **library citations**) + template; back-translation QC | analysis_smoke |
| `abbr.h`, `contractions.h` | TibSchol abbreviation table (CC0) + the HGM-derived bsdus-tshig register (182 pairs) | abbr/contractions suites |
| `poslex.h` | SOAS hand-tagged POS lexicon (CC BY 4.0, 13,107 forms) as Wilson-parse evidence | poslex suite |
| `colloquial.h`, `honorifics` data | colloquial pronunciation register; zhe-sa rows | respective suites |
| `spellcheck.h`, `refdict.h`, `outline.h`, `verse.h`, `terminology.h`, `drills.h`, `progress.h`, `qc.h`, `hypfile.h`, `mvp.h`, `libindex.h` | per their names (Mahāvyutpatti = `mvp`) | respective smokes |

## 6. What was added in the August wave (app-layer subsystems)

These live in `app/main.cpp` (pane classes and free functions),
each with in-app selftest pins:

- **Citation & quotation locator** (`extractCitations`,
  `libTitleIndex`, `locateQuoteInFile`, `showCitations`,
  `exportCitationsDoc`): scans a loaded ACIP document for the
  classical quotation formulas (…ZHES/CES + speech-verb set;
  "zhes bya ba" naming excluded), resolves "…LAS"-announced
  titles against library filenames, locates each quote *inside*
  the cited source by first-8-syllable regex, reports "TEXTS YOU
  WILL ENCOUNTER" with open/jump links, and exports a compiled
  quotations-and-bibliography side document. Load-time hint on
  every open.
- **Prose & prayer reader + meter machinery** (`meterUnits`,
  `meterBlocks`, `meterReaderHtml`): every shad-delimited unit
  carries its syllable count; verse blocks group under their
  meter (odd ≥5 classical; even ≥6 song-meter rule), deviants
  show their arithmetic (8 ≠ 7), prose stays plain; live
  re-count on edit. **Canon meter census**
  (`tools/meter_census.py` → `data/extracted/meter_census.json`,
  21.3M units over all three collections): 333,802 verse lines —
  7-syllable 61.1%, 5-syl 15.4%, 9-syl 8.5%, 6-syl mgur 6.2% —
  wired into the reader's headings ("% OF ALL CANON VERSE").
- **House style engine** (`DraftPane::styleCheck` +
  `data/help/STYLE.md` + `docs/standards/DCC_Style_Guide…docx`):
  the editor's Diamond Cutter Classics guide (2023-08-25) as six
  Help chapters + a mechanical checker (quotes, dashes, ranges,
  serial ampersand, era style, the dated word-use rulings).
  Flags only — the guide itself forbids blind Replace All.
- **Classical typography engine**
  (`OverlayPane::typographyFindings`, `pechaBreakProtect`): the
  W3C Tibetan Layout Requirements digest (via DigitalTibetan) —
  flags shad-after-bare-ka/ga, tsheg-after-visarga, solid ।।,
  breaking-tsheg-in-ང་།; the pecha exporter break-protects ། །
  and ང་། at render time (display-layer only).
- **Pecha Maker** (in OverlayPane): print-faithful long-leaf
  export — title folio, ya-yig, volume letter, yig-mgo head
  marks, classical 5-line opening, measured woodblock preset
  (54-syllable autofit against 47,728 measured eKangyur lines),
  verse lineation, stanza double-shad normalization (opt-out),
  recto yig-mgo furniture, 2-up imposition + cover sheet, batch
  folder→PDFs.
- **Files pane** (FilesPane + FilesDirProxy): the file-browser
  programme (Path Finder × ForkLift × Commander One study —
  `docs/design/FILE_BROWSER_PLAN.md`). Dual panes, per-pane
  tabs (persistent), breadcrumbs, folders-first sort, Drop Stack
  shelf (QSettings), batch rename (pure `computeRenames`,
  collision-revert), quick select, pane compare, folder sync
  (pure `computeSyncPlan`; preview-then-Apply; never deletes),
  opt-in Commander keys (F3–F8), workspaces, favorites, Finder
  tags (read-only via `mdls`), ZIP/TAR browsing (never-overwrite
  extract), and **remote connections**: FTP/FTPS/WebDAV(S) via
  system curl (credentials over stdin `-K -`, never argv; never
  stored), SFTP via system `sftp -oBatchMode=yes` (key auth);
  recursive folder download/upload with per-batch conflict rules
  (skip default / keep-both / explicit overwrite); listing
  parsers (`parseFtpList`, `parseDavList`) pinned on output
  captured from live local pyftpdlib/wsgidav servers.
- **Hunt palette (⌘K)**: one query answers from the dictionary,
  English reverse index, phonetics fold, corpus, teaching index,
  and **Library filenames** (open with smart routing).
- **Scan sync + BUDA-style viewer**: select text → folio/line →
  IIIF scan viewer (thumbnail rail, click-zoom, drag-pan, pinch,
  whole-work overview with e-text). Routing is
  concordance-verified: KD/TD numeric via Tohoku; KL via the
  KL→MW26071 concordance (built title-matched, inversion-pruned,
  cross-checked 244/244 vs THL); Sungbum via
  `sungbum_scan_links.json` (520 links, tiered, other-edition
  warned) with ordinal folio fallback when manifests lack labels
  (POSITION-MAPPED caption).
- **Miss taxonomy** (`DrillsPane::missReportHtml`): every wrong
  drill answer files under the skill it reveals (particle
  family, cloze role, order, vocab); "My weak spots…" reports
  with per-family training prescriptions. Local progress.db only.
- **Help system**: `data/help/USER_MANUAL.md` + GUIDELINES +
  SOURCES + STYLE load as browsable chapters; Troubleshooting
  menu (logs, diagnostics, storage, reset).

## 7. The port-and-prove discipline (how engines earn trust)

Pattern used by every port, in order:
1. **Bank the source** (with license) — `third_party/` or the
   data project's `Handoff/tools`.
2. **Capture oracle outputs from the source itself** — run the
   original (Python engines; Henning's DOS-era C compiled via
   `build_macos.sh` + header shims; botok via live pytest) over
   known inputs; save fixtures in-repo.
3. **Port from the file, never the spec.**
4. **Battery = diff against the fixtures**, at every
   intermediate where possible.

Current headline batteries: converters at full-dictionary parity
(+ **jsewts as a third independent oracle: 98.84% mutual
agreement over all 26,318 ground-truth pairs**, its divergences
being its own Latin-passthrough defect); calendar **proven over
26 complete oracle years** (321 month headers, 9,479 days, 210
duplicated-day pairs, every intercalary month — 2010–2035; the
epoch year and everything earlier is refused, and the oracle
drive sequence is documented: epoch `3`, menu `2`, two years,
`9`); botok token-identical at corpus scale; THL phonetics
139/139 on the standard's own examples; DCC bibliography
composer pins the style guide's own printed examples; pyewts
externally cross-checked (13 defects filed to the data project).

## 8. Canonical-space matching (the ACIP/wylie duality)

ACIP is *defined* uppercase; scholarly wylie is lowercase. That
exact rule (not a heuristic) drives normalization wherever both
scripts can appear: `lattice.cpp normTokens()`
(uppercase-containing token → `acipToEwts`), `HeadwordIndex`
keys, `libindex` normalization. **EWTS trap**
(battery-documented): wylie consonant+`'a` (`yin'am`) parses as a
*subjoined* a-chung stack, not an affixed particle — build
codepoint-level tests for affixed forms after consonants.

## 9. Overlay matching + canon-scale performance (lattice.cpp)

Pipeline: `tokenizeDocument` → `normTokens` → match → sort spans
→ `computeDepths`. Matching: per position, singles via hash;
phrases via a first-TWO-token tier (common syllables no longer
cause thousand-candidate scans); only the LAST token may differ,
by a fused ending. `computeDepths` is O(n log n) via a Fenwick
tree. **History**: the old pairwise pass was quadratic in BOTH
buildOverlay paths; the second instance hid behind the first fix
— diagnose with a scaling curve and `sample <pid>`. Result: a
1.6MB canon file = 472,112 spans in 0.42s (was 167s).

## 10. Library index (libindex.cpp)

External-content FTS5 over per-line text of everything under
`library/`: tokenizer identical to the corpus. Incremental by
`(path, mtime, size)`. **Traps learned**: external-content delete
commands must feed *every* column; an unfinalized `PRAGMA`
statement self-locks SQLite during a migration DROP. Headless:
`libindex_cli <index.db> <root>` (8.68M lines ≈ 97s). Consumers:
the Search pane's per-folder rows, per-file hit rollups, and the
Analysis pane's item-14 citations (top phrase anchors × 3 hits,
cited as file:line).

## 11. Quotation detection (quotation.cpp)

≥7 consecutive syllables string-matching a corpus segment IS that
work quoted — FTS5 phrase query on the minimum window, verify +
greedy-longest extend, cross-segment merge for spans no single
segment contains. UI joins matches to the bibliography bank via
ACIP refs and to footnotes via lemma-in-English. (The app-layer
citation *locator* of §6 is the complementary instrument: it
reads the *document's own* announcements rather than matching
corpus text.)

## 12. The calendar (tibcal.cpp / tibcal_day.cpp)

- Year level: independent 60-cycle arithmetic (epoch 1027).
- Day level: faithful KCK Modern Karana port — radix arithmetic
  in places (units;60;60;6;13); `kckDayForTrueMonth` is the
  primitive; `(year,month)` always resolves to the *regular*
  month, so intercalary days are reachable only by true-month
  count. `kckYearMonths` enumerates a year's months with the
  original's display convention.
- **Validity is honest**: epoch 3 (Modern Karana, 2009) does not
  back-project. 1990–2008 oracle prints are corrupt (every month
  doubled); 2009 itself is boundary-corrupted; `kckYearMonths`
  refuses year < 2010. The 26-year fixture battery (§7) proves
  everything in range.
- Oracle quirks: `main()` calls `set_epoch()` FIRST, then the
  menu (the "day loop" is menu option 3 — a mis-read of this
  cost three runaway fixture runs, documented in memory);
  epoch-constant comments mislabel spz_b/spz_c radices — **the
  code is the authority**.

## 13. BDRC integration (app layer only; allcore stays offline)

- **Transliteration boundary**: anything sent to BDRC goes
  through `acipToEwts` first (`bo-x-ewts`).
- Scans: `bdrcScanUrlChecked` routes KD/TD numerically via
  Tohoku (5-digit refs normalize to BUDA's 4), **KL through the
  title-matched concordance** (`kl_bdrc_concordance.json` — KL
  numbers are NOT Lhasa numbers; proven live), Sungbum through
  `sungbum_scan_links.json`. IIIF chain: collection → manifests
  → canvases whose labels are folio sides → image URLs; ordinal
  fallback (docFolioSequence) when a manifest lacks folio
  labels, always labeled POSITION-MAPPED. Async safety: every
  handler carries a `fetchEpoch_`; dialogs use themselves as the
  Qt context object so late replies can't touch dead stacks.
- The viewer (`openWoodblockViewer`) is the BUDA-style two-level
  browser (rail + canvas + overview with e-text), all
  Qt-native.
- Person layer: `acip_person_links.json` (1,768 links) from
  Élie's authoritative catalog, cross-checked.

## 14. Data waves (corpus lineage since v27)

| Wave | Source | Added | Corpus |
|---|---|---|---|
| v28 | three GMR volumes (docx → aligned) | +2,206 | 37,807 |
| v29 | ACIP Brief Catalog TXT + Kangyur xlsx + Master Catalog | +2,955 title pairs | 40,762 |
| v30 | ༠༠ Sungbum Catalog xlsx (double-row header!) | +1,055 pairs + 2,416 subjects | 41,817 |
| v31 | R6 web subject browser | +367 subject pairs | 42,184 |
| v32 | R6 by-author tree × Sungbum catalog, joined | +15 author pairs | 42,199 |

All ingest scripts live in the data project (`Handoff/tools/`);
each wave has a HANDOFF.md note.

## 15. Reference layers & licenses (shipping posture)

Apache-2.0: Monlam word lists, lucene-bo rules, botok, jsewts
(oracle only). CC0: verbs DB, TibSchol abbreviations. MIT: KCK,
pyewts. OFL/SIL: bundled fonts — Noto Serif
Tibetan, Jomolhari, Monlam Bodyig, BabelStone Tibetan Slim (all
with license files beside them in `data/fonts/`; `font_smoke`
proves they render the syllable tables). PD: BDRC scan images (attributed), Esukhia
e-editions, Das 1902 + Jäschke 1881 scans (archive.org). CC BY
4.0: 84000 glossary (shipped as a labeled layer), SOAS POS
lexicon. CC BY-NC 4.0: BDRC OCR models (bundled with BDRC's
permission). CC BY-SA 4.0 (pending export): rKTs abbreviations.
Steinert grant: Das headword index ONLY. **Never ship without
resolution**: 84000 TM, Steinert dictionary payloads,
RY/Waldo/Valby, Qomolangma fonts (non-commercial), THL payload
(local-only display). Full survey:
`docs/TIBETAN_TOOLS_SURVEY.md`; per-source trust tiers (Adam/GMR
programme): `docs/standards/SOURCE_TRUST_HIERARCHY.md` +
`data/help/SOURCES.md`.

## 16. The app layer: structure and conventions

- **Panes** (21, grouped): Read = Overlay · Library · Files ·
  Scans · Export; Translate = Draft · Manuscript · Apparatus ·
  Review · Align; Research = Search · Lookup · Sanskrit ·
  Convert · Analysis; Learn = Trainer · Drills; Input = Input ·
  OCR; Community = Propose · Approval. Registration: panes are
  `tabs.addTab(...)`d flat, then regrouped by `mkGroup(name,
  {titles})` into inner tab rows; the menu bar mirrors the
  groups generically from each pane's own buttons/toggles.
- **Global hooks** (file-scope `std::function`s) decouple panes:
  `g_raisePane`, `g_lookupQuery`, `g_goferQuery`, `g_openAtLine`,
  `g_openScanInInput`, `g_segSylCounts` (Botok word boundaries
  for other panes), `g_teachingTib` (spoken-Tibetan index),
  `g_dasHeadwords`, `g_jaeSections`… Panes set them at
  construction; consumers null-check.
- **Design system**: ink `#2B2620`; green `#1E6B4E` reserved for
  HGM authority; warm-grey reference family `#6E675D`/`#F0E9DC`;
  gold `#9A7A33` micro-caps zone labels; Palatino app-wide
  (Adam's standing preference — matches the publishing house's
  own body face). Tier honesty is visual: provisional looks
  provisional, reference looks reference.
- **Dialog conventions**: non-modal `show()` with
  `WA_DeleteOnClose`; report browsers use anchor schemes
  (`jumppos:`, `citefile:`, `citeopen:`…) with base64 payloads
  for paths; live dialogs re-render on `textChanged` via
  debounced single-shot QTimers scoped to the dialog.
- **Persistence**: QSettings("ALL", "TranslationTool")
  throughout (toggle states, Files tabs/workspaces/favorites,
  session restore).

## 17. Testing: three harnesses

1. **ctest (70 suites = 62 core + 7 app + 1 ocr)** — the
   allcore batteries (§5/§7) plus app-level `dmp_smoke` /
   `collation_smoke` / `font_smoke` / `app_selftest` (which runs
   harness #2 offscreen under CTest) and `ocr_smoke`. Run:
   `ctest --test-dir cmake-build-release`. Batteries hard-fail on
   missing oracles; skips are loud.
2. **In-app selftest (225 checks)** — `DiamondCutterTranslationTool
   --selftest` builds the real UI offscreen (16 per-pane
   `selfTest` members + the main driver's inline checks) and
   checks pane behavior end-to-end: pins for every August subsystem
   (citations, meters, mgur, style, typography, THL phrase, sync
   plans, remote parsers, rename plans, Drop Stack persistence,
   Hunt lanes, miss taxonomy…), plus structural checks (every
   pane fits an 800px window; menu bar mirrors groups).
3. **Control sweeps** — `--sweep <PaneName>` exercises every
   button/toggle of one pane against a stall guard (Overlay 31 ·
   Files 12 · Draft 13 · …). `--screenshots <dir>` drives all 21
   panes with populated demo fields and captures PNGs (the
   basis of the Live Screens gallery).
   `--sanskritcheck` renders the attested Sanskrit-stack sheet.

Human-gate documents: `docs/TESTING_CAMPAIGN.md` (K/F columns are
Adam's), `docs/RELEASE_READINESS.md` (the five-step release gate;
clean-Mac install is the most important step).

## 18. Press pipeline & versioning

`bash tools/package_macos.sh`:
1. builds nothing itself — **build first** (`cmake --build
   cmake-build-release --target DiamondCutterTranslationTool -j8`);
2. stages the .app, runs `macdeployqt`, fixes rpaths, ad-hoc
   codesigns;
3. assembles "Diamond Cutter Tool Data" (spine, OCR models, runtime data
   folders — enumerated in the script);
4. launch-tests the staged layout (3 attempts);
5. **installs to `/Applications/Diamond Cutter Translation Tool/`** — app
   rsync with `--delete`, data rsync WITHOUT delete (user
   materials safe), then verifies the installed binary is
   **byte-identical** to the staged one (hard fail otherwise).
   *Postmortem 2026-08-14: this step was lost in the 08-13
   disk-emergency rewrite; presses built DMGs while the desktop
   ran stale. Never report "relaunched" without artifact
   verification.*
6. builds the DMG (`dist/Diamond-Cutter-Translation-Tool-<v>.dmg`).

Versioning: `VERSION` file is the single source (CMake
configure-dependency — a bump re-configures; this was the root of
the frozen-0.9.0 About pane). Minor bump per feature wave, patch
per fix-press, 1.0.0 at first team install. The About pane stamps
`RELEASED <build date>` from `__DATE__`. **Git discipline**:
sources only — `dist/` and `cmake-build-release/` are gitignored
forever (the .git once reached 21GB and ENOSPC'd the machine;
history rewrite approval pending). `data/extracted/` is
gitignored but its banked artifacts are force-added by
convention.

## 19. Developer workflows & hard-won gotchas

- Build & test: §17/18. Spine must exist
  (`python3 tools/build_spine.py`).
- Debug harness link: `c++ -std=c++20 -I core/include X.cpp
  cmake-build-release/core/liballcore.a -lsqlite3`.
- KCK oracle: `third_party/kck_henning/build_macos.sh` then
  `printf '3\n2\n<start>\n<end>\n9\n' | ./kck` (epoch first!);
  compile with `-x c -std=gnu89`.
- **Editing app/main.cpp at scale**: the working method is
  python edit scripts written to a scratch file (never heredoc
  one-liners), `assert s.count(old) == 1` on every anchor, then
  run. Gotchas that have each bitten more than once: (a) the
  file stores some em-dashes as the literal 6-char `—` —
  match with `\\u2014`; (b) `'\''` inside a triple-quoted python
  string collapses to `'''` — write C++ char literals as
  `QChar('\\'')` or avoid; (c) anchors like
  `allcore::Spine& spine_;` appear in ELEVEN classes — anchor on
  a unique neighbor; (d) missing `, 1)` on `str.replace` spans
  multiple lines silently.
- **Sweep-safe UI**: the sweep clicks every QPushButton/QCheckBox
  — new modal flows must tolerate an automated click
  (QMessageBox is auto-dismissed; QToolButtons are not swept, so
  menus hang nothing).
- Two agent sessions may share this tree: stage surgically,
  never `git add -A` blindly; check `git status` for the other
  session's files.
- Memory: the session-persistent project log lives at
  `~/.claude/projects/...-ALL-translation-tool/memory/` — the
  state file is the institutional memory (postmortems, drive
  sequences, gotchas); read it before repeating an experiment.

## 20. External gates & known limits (current, honest)

Waiting on Adam: ~14 finished Gmail drafts (sends), GMR's
address + rulings (LE'UR class, source-trust hierarchy,
STD-008/9), git history rewrite approval, API credits (glossary
stalled at 1,395 entries), ElevenLabs pilot, MITRA download,
Sungbum review workbook (963 decisions), K/F walkthrough,
clean-Mac install, several uploads (input-center standards,
Master Catalog, honorific books). Waiting on third parties: BDRC
OCR release (~end Aug), Lainé (rKTs export), THL, 84000, PKTC,
CIHTS. Known limits: day-level calendar is Modern Karana
2010–2035 (Phugpa correspondence is a documented open
question); Old-Tibetan affix rules deliberately unported until
Old-Tibetan material enters the library; letter-level
contraction fusions (sher phyin class) are a research item; the
Files pane's share sheet / 7z / tag-writing are logged
deferrals. The complete ledger: `TODO.md`'s ATTACK QUEUE section.

## 21. Where knowledge lives

- `docs/CLOSER.md` — **the single open-work backlog**. An item
  not on it does not exist (this supersedes TODO.md as the live
  ledger; TODO.md remains the historical attack queue, merged
  2026-08-14).
- `docs/LOOSE_ENDS_AUDIT.md` — historical record of that audit
  only; no longer a second tracked list.
- `docs/design/` — per-feature designs (file browser, scan sync,
  BUDA viewer, DigitalTibetan integration…).
- `docs/research/` — evidence documents (Sanskrit-in-Tibetan
  glyphs, meter, Sungbum lane, jsewts/pyewts reviews, THL sweep,
  transcription survey, HF MT survey…).
- `docs/standards/` — STD-001…011, the DCC style guide (original
  + distilled), pronunciation standard, source-trust hierarchy.
- `docs/distribution/` + `docs/whitepaper/` — release notes,
  feature guide, white papers, the user manual mirror.
- `data/help/` — everything the app's Help ships (manual,
  guidelines, sources, style).
- Session memory (see §19) — the running institutional log.


## 6½. The DigitalTibetan integration (slot-0 programme, same day)

docs/design/DIGITALTIBETAN_INTEGRATION_PLAN.md — all core phases
shipped 2026-08-14: **P1** typography rule engine (W3C digest
lint + pecha break protection) · **P3** Espel phonetics
cross-oracle (banked third_party, Perl dual license — found and
fixed our monosyllabic ba→wa defect; GMR pron_engine fenced
untouchable by Adam's ruling) · **P4** py-tiblegenc legacy
rescue (Apache-2.0; venv + wrapper, 70 font tables, Library
Maintenance lane) · **P5** StarDict reader (allcore module,
battery #40, Lookup user-supplied layer) · **P6** pecha
three-up + A5 saddle booklet imposition (pure ordering fn,
selftested) · **P8** TISE input keys in Convert (display-layer
joins, engine untouched). W3C full-document pass DONE 2026-08-14 (docs/research/W3C_TIBETAN_LAYOUT_FULL.md; three lint rules + pecha v4 lane shipped);
extra phonetics display conventions.

## Appendix A — exact inventory (captured 2026-08-14)

- **Classes in app/main.cpp (31)**: LookupPopup, AnalysisPane,
  ScanCanvasLabel, OverlayPane (the largest — Overlay + card +
  scan sync + pecha + citations + meters + typography),
  ApparatusPane, ScansPane, ExportPane, GoferPane, SanskritPane,
  TrainerPane, DrillsPane, DraftPane, FilesDirProxy, FilesPane,
  LibraryPane, ReviewPane, AlignPane, InputPane, ScanPane (OCR;
  compiled under `ALL_HAVE_OCR`), ProposePane, ApprovalPane,
  ManuscriptPane, SettingsDialog, HelpWindow, HuntPalette.
- **Global hooks (10)**: `g_raisePane`, `g_openAtLine`,
  `g_segSylCounts`, `g_openScanInInput`, `g_lookupQuery`,
  `g_goferQuery`, `g_surveyFile`, `g_sendToManuscript`,
  `g_mssComposeBib`, `g_mssProposeNote`.
- **CMake targets**: `allcore` (44 sources), `allocr`,
  `DiamondCutterTranslationTool` (MACOSX_BUNDLE), `allanalyze`,
  `libindex_cli`, `wynorm`, + 70 test executables. Root project
  version reads the `VERSION` file (wired 2026-08-14 — it had
  been a stale hardcoded 0.1.0); `USE_SYSTEM_SQLITE=OFF` now
  FATAL_ERRORs with instructions if the amalgamation isn't
  vendored.
- **data/ (24 dirs, 1.6GB)**: teaching 1.2GB (DCC+TKB caption
  corpora + teaching index) · extracted 168MB (53 banked
  artifacts: concordances, apparatus, lexica, registers,
  people/subjects, oracle reports, censuses) · das 142MB (Das
  1902 + Jäschke 1881 scans) · 84000 18MB (CSV mirror) · fonts
  7.7MB · ai_glossary · whitney · goldstein · hackett ·
  mahavyutpatti · soas_pos · manual_authentic · help ·
  proposals · hypercontext_legacy · botok · pron_colloquial ·
  spellcheck · abbreviations · acip_codes · honorifics · idioms
  · mst · reports.
- **library/ (2.9GB)**: sungbum 4,550 files · tengyur 3,439 ·
  kangyur 1,015 · ocr_models (4, BDRC) · the 2.36GB `.index.db`.
- **third_party/**: `kck_henning/` (MIT, PROVENANCE.md, DOS-shim
  build, 26 oracle year prints 2010–2035 + fixture TSVs) — the
  *oracle*, not compiled into allcore; plus
  `app/thirdparty/diff_match_patch.cpp` (compiled in, dmp_smoke).
- **Full test-suite list**: spine, analysis, engines_battery,
  thl_battery, spellcheck, gofer, lattice, reader, drills,
  progress, terminology, outline, hyp, whitney, colloquial,
  proposals, verse, quotation, verbstems, lexicon, tibcal,
  searchnorm, affixnorm, tibcal_day, tibexport, libindex, qc,
  botok, botok_tok, mvp, poslex, abbr, contractions, acipcodes
  (core); dmp, collation, font, app_selftest (app); ocr (ocr).
