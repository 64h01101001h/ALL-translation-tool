# ALL Translation Tool — Technical Overview
*Under the hood, for developers. Current as of 2026-08-11 (38 test
suites, corpus v32). Companion documents: `README.md` (user-facing
pane guide), `ALL_TRANSLATION_TOOL_ROADMAP.md` (master plan),
`TODO.md` (live ledger), `docs/whitepaper/` (the non-technical story),
and the design docs referenced per subsystem below.*

## 1. Architecture in one paragraph

A C++20 static library (**`allcore`**, `core/`) owns everything
deterministic: SQLite/FTS5 data access, the ported conversion engines,
the grammar layer, search, and every algorithm. A single-file **Qt 6
Widgets** app (`app/main.cpp`) is a thin UI over it — seventeen panes
in six workflow tabs (Read / Translate / Research / Learn / Input /
Community, mirrored by the menu bar), no
logic that isn't presentation, permission-gated network calls only in
the app layer. Python lives in `tools/` for data preparation and
validation harnesses only; nothing Python runs at app runtime. Build
is CMake; `ctest` runs 38 suites; the app is
`cmake-build/app/ALLTranslationTool.app`.

```
data releases (HGM Dictionary project, versioned)     external APIs
        │                                              (BDRC only)
        ▼                                                  │
tools/build_spine.py ──► build/hgm_spine_v27_2.db          │
        │                     ▲                            │
        │              ┌──────┴──────┐                     ▼
        └──────────►   │   allcore   │  ◄──── app/main.cpp (Qt UI)
 data/extracted/*.json │  (C++20)    │
 (banks, joins, TSVs)  └─────────────┘
```

## 2. The four inviolable rules, as they appear in code

1. **hgm_gloss is binding** — reference layers (Monlam, verbs DB,
   Hopkins, LC, catalog English, R6 subjects) are *display-labeled*
   and never feed lookup ranking, parse decisions, or the dictionary.
   Grep for the word "reference" in UI strings; every external layer
   carries its source and license inline.
2. **Never reconstruct engines from spec** — every port names its
   source file and ships with a battery derived from the source's own
   outputs (§6). Two same-day case studies: the calendar's `nyifac`
   (a zero assumption broke 76 fixtures; the value was in T3.C:111)
   and the month-name male-year exception (underivable; explicitly
   coded in T4.C).
3. **Never guess** — unconvertible input returns `⟨token⟩ + false`
   (`wylieToUnicode` is contractually no-throw since c29cca8's sibling
   fix); quotation detection below threshold is silence; author/subject
   pairs require join support; pre-epoch dates return invalid.
4. **Provenance is sacred** — every corpus segment carries
   `source_wave` (+ `catalog_no`, `join_support`, `subject_bo` where
   applicable); HANDOFF.md in the data project logs every wave;
   `third_party/` banks upstream sources with license + PROVENANCE.

## 3. Repository layout

```
core/include/allcore/*.h   public API, one header per subsystem
core/src/*.cpp             implementations
core/tests/*_smoke.cpp     the 38 batteries (see §6)
core/tools_*.cpp           headless CLIs: wynorm, libindex_cli
app/main.cpp               the whole UI (~11k lines, pane classes)
tools/*.py                 data prep + oracles (never runtime)
data/                      gitignored payloads; data/extracted/ =
                           regenerable banks (regeneration commands
                           in README + each tool's docstring)
third_party/kck_henning/   Henning KCK sources (MIT) + shims +
                           build_macos.sh + oracle fixtures
docs/                      designs, standards, apparatus, whitepaper
library/                   user data (texts, caches, .index.db)
build/hgm_spine_v27_2.db   compiled spine (238–244MB, regenerable)
```

## 4. The spine (SQLite/FTS5)

Built by `tools/build_spine.py` from the release package + corpus
(currently `full_parallel_corpus_v32.json.gz`, 42,199 segments) +
reverse index. Key tables:

- `entries` — 105,634 dictionary rows. **Gotcha**: physical column
  order matters; all reads go through `kEntryCols` (a `SELECT *`
  ordering bug once corrupted tense data — see memory/git 2026-08-06).
- `entries_fts`, `reverse_index`, `meta`.
- `corpus_segments(id, course, seq, acip, wylie, english, raw)` with
  `corpus_fts(wylie, english, acip, wylie_norm)` —
  `tokenize="unicode61 tokenchars ''''"` (apostrophe is a token char:
  `pa'i` is ONE token; keep the library index identical, §9).
  `wylie_norm` is the affix-stripped shadow (only stored where it
  differs; 30,046 rows), produced at build time by piping through the
  **`wynorm` CLI** so the C++ rules are the single authority. FTS5
  matches all columns by default → every search is affix-tolerant
  with no query rewriting.

Courses of special provenance: `TITLK/TITLT/TITLS/TITLR` (catalog
title pairs, v29–30), `SUBJ` (bilingual subject headings, v31),
`AUTH` (join-verified author names, v32).

## 5. allcore subsystems (header → what it does → battery)

| Header | Purpose | Battery |
|---|---|---|
| `spine.h` | DB access; `lookup` tries wylie → variants → Tibetan → ACIP | spine_smoke |
| `engines.h` | ported converters: `acipToEwts`, `wylieToUnicode`, `pronounce`, Sanskrit chain | engines_battery (full-dictionary parity vs Python oracles) |
| `lattice.h` | Overlay construction: tokenize → match → spans + nesting depth (§8) | lattice_smoke |
| `particles.h` | Wilson particle tables, fused endings (`splitFusedEnding` accepts ACIP *and* wylie suffixes — case-disjoint) | via lattice/reader |
| `reader.h`, `verbclass.h`, `wilsonparse.h` | trainer chunking, verb classes, full Wilson parse + dots | reader_smoke |
| `gofer.h` + `src/gofer_ast.h` | Gofer grammar (phrases/OR/NEAR) over spine or file trees | gofer_smoke |
| `libindex.h` | Library file index (§9) | libindex_smoke |
| `quotation.h` | attested quotation detection (§10) | quotation_smoke |
| `verbstems.h` | CC0 verbs DB reference layer (unicode-keyed) | verbstems_smoke |
| `lexicon.h` | Monlam word-list attestation (449k forms) | lexicon_smoke |
| `affixnorm.h` | BDRC lucene-bo affix stripping; wylie projection is *decided by* the unicode port | affixnorm_smoke |
| `tibcal.h` | year-level 60-cycle (rabjung/element-animal, both directions) | tibcal_smoke |
| `tibcal_day.h` | day-level Modern Karana + months (§11) | tibcal_day_smoke |
| `tibexport.h` | print export, Mixed Nuts prep, filename decoding, BDRC URLs, STD-002/007 formatters | tibexport_smoke |
| `spellcheck.h`, `refdict.h`, `outline.h`, `verse.h`, `terminology.h`, `analysis.h`, `drills.h`, `progress.h`, `qc.h` | per their names | respective smokes |
| `botok.h` (`botok_*.cpp`) | botok segmentation port: chunker → trie → tokenizer → Segmenter facade (built by the parallel session; oracle-proven vs live Python at corpus scale) | botok_smoke, botok_tok_smoke |

## 6. The port-and-prove discipline (how engines earn trust)

Pattern used by every port, in order:
1. **Bank the source** (with license) — `third_party/` or the data
   project's `Handoff/tools`.
2. **Capture oracle outputs from the source itself** — run the
   original (Python engines; Henning's DOS-era C compiled via
   `third_party/kck_henning/build_macos.sh` + header shims; botok via
   live pytest) over known inputs; save fixtures (TSV/JSON in-repo).
3. **Port from the file, never the spec.**
4. **Battery = diff against the fixtures**, at every intermediate
   where possible (the calendar checks zla-dag/gza-dag/nyi-dag places,
   not just final dates).
Current headline batteries: converters at full-dictionary parity;
calendar 76/76 days + 25/25 month headers + 721/721 fixture days
(17 lhag pairs); botok token-identical at corpus scale; affix rules
via our own converters; pyewts as an *external* cross-oracle
(91.4% exact; divergences classified, 13 defects filed upstream).

## 7. Canonical-space matching (the ACIP/wylie duality)

ACIP is *defined* uppercase; scholarly wylie is lowercase. That exact
rule (not a heuristic) drives normalization wherever both scripts can
appear:
- `lattice.cpp normTokens()`: uppercase-containing token →
  `acipToEwts(token)`, else pass through.
- `HeadwordIndex` keys are wylie (ACIP headwords converted once at
  build).
- `libindex` normalizes the same way before affix-stripping.
**EWTS trap** (battery-documented): wylie consonant+`'a` (`yin'am`)
parses as a *subjoined* a-chung stack, not an affixed particle — build
codepoint-level tests for affixed forms after consonants.

## 8. Overlay matching + canon-scale performance (lattice.cpp)

Pipeline: `tokenizeDocument` → `normTokens` → match → sort spans
(beg asc, longer first) → `computeDepths`.

Matching (indexed path, the app's): per position `i`
- singles: hash `single_[norm[i]]`; fused fallback via
  `splitFusedEnding(norm[i]).base`.
- phrases: `multi_[norm[i]][norm[i+1]]` (first-TWO-token tier —
  common syllables like *pa* no longer cause thousand-candidate
  scans); only the LAST token may differ, by a fused ending
  (`fusedEndingIf`); a len-2 phrase whose second token carries the
  ending is probed via the stripped base.
- semantics preserved from the original SQL path: per (i,len) first
  exact wins, else first clitic.

`computeDepths` is O(n log n) via a Fenwick tree over span ends:
after the sort, every earlier span has `beg <= s.beg`, and an earlier
span with `end >= s.end` is strictly longer *unless* it is the
identical range — identical ranges are adjacent in the sort, so
subtract the run length. **History**: the old pairwise pass was
quadratic in BOTH buildOverlay paths; the second instance hid behind
the first fix (probe timings unmoved — diagnose with a scaling curve,
4× time per 2× size, and `sample <pid>`). Result: 1.6MB canon file =
472,112 spans in 0.42s (was 167s).

## 9. Library index (libindex.cpp)

External-content FTS5 over per-line text of everything under
`library/`: `lines(id,file_id,line_no,text,text_norm)` +
`lines_fts(text, text_norm, content='lines')`, tokenizer identical to
the corpus (`tokenchars ''''` — schema v2; `PRAGMA user_version`
gates a one-time rebuild of older indexes). Incremental by
`(path, mtime, size)`. `text_norm` = per-token normalize→strip (only
when changed). **Traps learned**: external-content delete commands
must feed *every* column; an unfinalized `PRAGMA` statement self-locks
SQLite during the migration DROP (scope the probe `Stmt`). Headless
builds: `libindex_cli <index.db> <root>` (8.68M lines ≈ 97s).

## 10. Quotation detection (quotation.cpp)

A passage span that string-matches a corpus segment's wylie over ≥7
consecutive syllables (identical tokenization both sides) IS that
work quoted — FTS5 phrase query on the minimum window, then verify +
greedy-longest extend against the segment's own syllables;
non-overlapping; consecutive same-course lines merge into one
whole-span match. **Corpus property**: duplicated text across courses
is real (a mandala verse = two lines in one course, one segment in
another) — the battery seeds a boundary that exists in NO single
segment to force the merge path. UI joins matches to the bibliography
bank via ACIP refs and to footnotes via lemma-in-English.

## 11. The calendar (tibcal.cpp / tibcal_day.cpp)

- Year level: independent 60-cycle arithmetic (epoch 1027; element
  pairs advance from 1026 fire-male-tiger). Anchored battery.
- Day level: faithful KCK Modern Karana port — radix arithmetic in
  places (units;60;60;6;13), zla-dag / ril-cha / means / nyi-dag
  (subtract `nyifac1 = 7;44` before the correction lookup!) /
  gza-dag / spyi-zhag → JD. `kckDayForTrueMonth` is the primitive;
  `(year,month)` always resolves to the *regular* month, so
  intercalary days are reachable only by true-month count.
- Months: `kckYearMonths` walks tm 1..12; `zla1 ∈ {0,1}` emits the
  intercalary sibling first (display m−1). Names from T4.C
  (tsurlug==1): fixed animals (`(m+10)%12`, index 11 = Tiger),
  element base from (year-element, gender) switch + `(m−1)/2`, and
  the **male-year exception** for months 11–12. Weekday convention:
  gza 0 = Saturday. lhag = same tshes on two civil days (engine's JD
  is the pair's second); chad = two tshes sharing one JD.
- Epoch constants: comments in set_epoch mislabel spz_b/spz_c radices
  — **the code is the authority** (c pairs with 707, b with 64).

## 12. The apparatus program (STD-008)

Two tiers, structurally enforced: OFFICIAL = extracted from published
volumes only (`tools/extract_mixed_nuts_{notes,bibliography}.py` →
`tools/build_main_apparatus.py` → `docs/apparatus/MAIN_*.md`;
never hand-edited; no in-app approve button *by design*) vs PENDING
(`data/candidate_{notes,bib}.json` from the Draft pane, formatted for
review sessions by `tools/build_pending_review.py`). Insertions always
carry citations; pending insertions carry the PENDING label.

## 13. BDRC integration (app layer only; allcore stays offline)

- **Transliteration boundary**: anything sent to BDRC goes through
  `acipToEwts` first (`bo-x-ewts`).
- Scans: `bdrcScanUrl` maps KD/KL/TD via Tohoku (5-digit catalog refs
  normalize to BUDA's 4). IIIF chain:
  `iiifpres.bdrc.io/collection/wio:bdr:MW…` → manifests → canvases
  whose **labels are folio sides** ("94a") → image URLs; no
  calibration needed. Follow-along tracks `@NNN[A|B]` at the cursor;
  line band = newline count within the folio (exact) over uniform
  image division (labeled approximate). Async safety: every handler
  carries a `fetchEpoch_` and bails if a newer file took over;
  dialogs use themselves as the Qt context object so late replies
  can't touch dead stacks.
- Title search (Sungbum etc.): lds-pdi template **BLMP**
  (`/query/table/BLMP?L_NAME="…"&LG_NAME=bo-x-ewts&I_LIM=n&format=json`)
  — user confirms the edition; link persisted in
  `library/bdrc_links.json`. (Res_name doesn't exist; autosuggest
  host doesn't resolve.)

## 14. Data waves (corpus lineage since v27)

| Wave | Source | Added | Corpus |
|---|---|---|---|
| v28 | three GMR volumes (docx → aligned) | +2,206 | 37,807 |
| v29 | ACIP Brief Catalog TXT + Kangyur xlsx + Master Catalog Sheet2 | +2,955 title pairs | 40,762 |
| v30 | ༠༠ Sungbum Catalog xlsx (double-row header!) | +1,055 pairs + 2,416 subjects | 41,817 |
| v31 | R6 web subject browser (`Name--ACIP` bilingual by construction) | +367 subject pairs | 42,184 |
| v32 | R6 by-author tree × Sungbum catalog, joined on catalog number (≥2 works, ≥80%) | +15 author pairs | 42,199 |

All ingest scripts live in the data project (`Handoff/tools/`), each
wave has a HANDOFF.md note, and QC side-products are filed there too
(pyewts defect candidates; subject set-correspondence validation;
`…`/`@` truncation/uncertainty flags per Release IV conventions).

## 15. Reference layers & licenses (shipping posture)

Apache-2.0: Monlam word lists, lucene-bo rules, botok. CC0: verbs DB.
MIT: KCK, pyewts. OFL: Noto Serif Tibetan (bundled with license).
PD: BDRC scan images (shown with attribution). CC BY-NC 4.0: BDRC OCR
models (bundled in the DMG, used with BDRC's permission). **Never ship
without resolution**: 84000 TM (license discrepancy — email drafted),
Steinert dictionary payloads (per-dictionary), RY/Waldo/Valby, rKTs
dump. Full survey with per-item status:
`docs/TIBETAN_TOOLS_SURVEY.md`.

## 16. Developer workflows

- Build & test: `cmake -B cmake-build && cmake --build cmake-build -j8
  && ctest --test-dir cmake-build` (spine must exist:
  `python3 tools/build_spine.py`).
- Debug harness link recipe: `c++ -std=c++20 -I core/include X.cpp
  cmake-build/core/liballcore.a -lsqlite3` (system SQLite has FTS5).
- KCK oracle: `third_party/kck_henning/build_macos.sh` then drive via
  stdin (`3\n3\nD M Y\n` + `t`/`m`, or menu 2 + two year lines for
  print mode; **compile with `-x c -std=gnu89`** — capital `.C` reads
  as C++ otherwise).
- Regenerate banks: each `tools/*.py` documents its own command; the
  apparatus trio reruns per published volume; `pyewts_oracle.py`
  reruns per release.
- Two agent sessions may share this tree: **stage surgically, never
  `git add -A`**; check `git status` for the other session's in-flight
  files; shared-file hunks via `git apply --cached` with a crafted
  patch.

## 17. Known limits (honest edges)

Day-level calendar is Modern Karana (epoch 2009+; KCK computes the
Kalacakra karana — correspondence with published Phugpa almanacs is a
documented open question). Old-Tibetan affix rules unported. Wilson
dots OM/ADV/UP remainder is data-gated. The tier ruling on catalog
English (binding vs reference) is Adam's to make.
