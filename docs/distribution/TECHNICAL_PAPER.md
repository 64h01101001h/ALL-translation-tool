# The ALL Translation Tool — Technical Paper

*Asian Legacy Library · August 2026*

*For developers, digital-humanities scholars, and technically-minded
readers. Companion documents: the white paper (`docs/whitepaper/`,
the non-technical story), `docs/TECHNICAL_OVERVIEW.md` (the
developer's under-the-hood reference), and the design documents cited
per subsystem.*

---

## 1. Architecture

The ALL Translation Tool is a desktop application built from two
layers with a hard boundary between them.

**`allcore`** is a C++20 static library that owns everything
deterministic: SQLite/FTS5 data access, the ported conversion
engines, the grammar and reader layer, search, quotation detection,
the Tibetan calendar, OCR post-processing, and every algorithm in the
product. It has no UI dependency and no network access.

**The application** is a single-file Qt 6 Widgets program
(`app/main.cpp`) — sixteen panes organized in six workflow groups
(§8), containing no logic that is not presentation. Qt 6 was chosen
deliberately: it bundles HarfBuzz, giving correct complex-script
shaping for Tibetan stacks out of the box, and it keeps the code
cross-platform-clean for future Windows and Linux builds for the
input centers.

Python appears only in `tools/` — data preparation and validation
harnesses. Nothing Python runs at application runtime; instead, the
Python engines of the HGM Dictionary data project remain the
**canonical oracles** against which every C++ port is proven (§3).

The build is CMake. `ctest` runs 38 automated suites (§9). The
product is `ALL Translation Tool.app`, distributed as a DMG (§11).

```
data releases (HGM Dictionary project, versioned)     external APIs
        │                                       (Claude, BDRC only)
        ▼                                                  │
tools/build_spine.py ──► build/hgm_spine_v27_2.db          │
        │                     ▲                            │
        │              ┌──────┴──────┐                     ▼
        └──────────►   │   allcore   │  ◄──── app/main.cpp (Qt UI)
 data/extracted/*.json │  (C++20)    │
 (banks, joins, TSVs)  └─────────────┘
```

### Four inviolable rules

The codebase enforces four rules inherited from the dictionary
project, and they shape every subsystem described below:

1. **The HGM gloss layer is binding; everything else is reference.**
   Reference layers (Monlam word lists, the CC0 verbs database,
   Hopkins, Lokesh Chandra, catalog English) are display-labeled and
   never feed lookup ranking or parse decisions.
2. **Never reconstruct an engine from its specification.** Every port
   names its source file and ships with a battery derived from the
   source's own outputs.
3. **Never guess.** Unconvertible input returns a flagged
   `⟨token⟩` marker and a failure code — never a silent
   approximation. Quotation detection below threshold is silence.
4. **Provenance is sacred.** Every corpus segment carries its source
   wave; every machine-derived value says so; upstream sources are
   banked with license and provenance records.

## 2. The data spine

The application never owns its data. It **imports versioned
releases** from the HGM Dictionary data project — a separately
maintained corpus-and-dictionary effort — and compiles them into a
local SQLite database, the *spine*
(`build/hgm_spine_v27_2.db`, ~240 MB, fully regenerable by
`tools/build_spine.py`).

Contents, at the current corpus release (v32):

- **`entries`** — 105,634 unified dictionary rows: 12,004 anchored to
  Geshe Michael Roach's English equivalents (tiered: curated >
  glossary > auto-aligned provisional), the Lokesh Chandra
  Tibetan–Sanskrit layer, Tibetan Unicode on every entry,
  pronunciations, ACIP forms, variants, and evidence. Provenance
  fields are preserved verbatim.
- **`corpus_segments`** — 42,199 aligned Tibetan–English segments
  from 60+ published sources, the concordance and evidence base,
  including catalog title pairs, bilingual subject headings, and
  join-verified author names, each wave separately tagged.
- **Full-text indexes** (FTS5) over headwords, English, and the
  corpus. One detail matters throughout: the tokenizer declares the
  apostrophe a token character, so *pa'i* is one token — and a shadow
  column `wylie_norm` stores the affix-stripped form wherever it
  differs (30,046 rows), produced at build time by piping through the
  C++ `wynorm` CLI so that the C++ normalization rules are the single
  authority. The result: every search is affix-tolerant with no
  query rewriting.

A second index, the **library index**, applies the same FTS5 schema
(same tokenizer, per-line granularity, incremental by path/mtime/
size) to everything under the user's `library/` folder — the 2,716
texts of ACIP Release 6 plus the user's own materials; a full build
indexes 8.68 million lines in about 97 seconds.

## 3. The engine ports and their validation batteries

The conversion engines — transliteration, script, pronunciation,
Sanskrit — were perfected as Python reference implementations during
the dictionary project. The application needed them in C++. The
discipline for every port is fixed:

1. **Bank the source** with its license.
2. **Capture oracle outputs from the source itself** — run the
   original over known inputs and save the fixtures in-repo.
3. **Port from the file, never from the spec.**
4. **The battery is a diff against the oracle**, at intermediate
   stages where possible — and a port that fails its battery does
   not ship.

Current results, all enforced as `ctest` suites on every build:

| Engine (C++ port) | Oracle | Battery result |
|---|---|---|
| `acipToEwts` (ACIP → scholarly EWTS) | `hgm_tools.py` | 100.000% identical on all 42,199 corpus conversions |
| `wylieToUnicode` (EWTS → Tibetan script) | `ewts_unicode.py` | 99.03% variant-aware exact on 26,318 source-attested ground-truth pairs — the canonical engine's own score; every miss traced to a documented source defect |
| `pronounce` (GMR-convention phonetics) | `pron_engine.py` | character-for-character identical on all 105,576 distinct dictionary headwords; 2,118 printed-card pronunciations as ground truth |
| Sanskrit converter (eight linked representations: IAST ⇄ Tibetanized ACIP ⇄ both official ACIP input-code styles ⇄ EWTS ⇄ Tibetan script ⇄ Devanagari ⇄ pronunciation) | `sanskrit_convert.py` | 100.000% identical on all 79,452 Lokesh Chandra terms — *including failing on exactly the same malformed inputs* |
| Devanagari → IAST reader | round-trip | exact on all 73,437 LC Devanagari renderings |
| Botok segmentation port (chunker → trie → tokenizer) | live Python botok | token-identical on the upstream test battery and on every distinct spine Tibetan string (105,546/105,546); field-for-field identical token streams at corpus scale |
| Tibetan calendar, day level (Modern Karana) | Henning's KCK program, compiled and run beside the port | 76/76 spot fixtures; 25/25 month headers and 721/721 days of two complete years; all 17 doubled lunar days exact pairs — with intermediate radix places (zla-dag, gza-dag, nyi-dag) diffed, not just final dates |

Two features of this table are worth underlining. First, the
Sanskrit battery counts *identical failures* as passes: rule 3 means
the C++ port must refuse exactly what the Python refuses. Second,
the wylie→Unicode target is 99.03% because that is the canonical
engine's own ceiling against imperfect historical ground truth — the
port is measured as *identical to the oracle*, not as approximately
correct.

Beyond the ports' own oracles, the independent `pyewts` library is
run as an **external cross-oracle** (91.4% exact agreement, with
every divergence classified and 13 defect candidates filed
upstream) — a check that the canonical engines themselves are sound,
not merely self-consistent.

## 4. The grammar and reader layer

Above the converters sits a grammar layer serving the pedagogy and
translation panes:

- **Clause and phrase chunking** (`reader.h`) splits a passage into
  clause units for the Trainer's progressive-reveal pedagogy and the
  Draft pane's clause-by-clause anchors.
- **Particle grammar** (`particles.h`) implements the classical
  particle tables, including fused endings — `splitFusedEnding`
  accepts both ACIP and wylie suffix forms, disjoint by case — and
  powers the Overlay's suffix-agreement diagnostics (a dotted-amber
  review hint, never an auto-correction).
- **Wilson parsing** (`verbclass.h`, `wilsonparse.h`) assigns case
  roles from verb classes (~70 verbs banked) and produces the full
  parse with all twelve "Science of the Dots" syllable designations.
  Verb-first reading order comes from the dictionary's own tense
  evidence, or is honestly marked unverified.
- **Structural extraction** parses explicit *bam po* and numbered
  chapter markers (including compound and contracted ordinals),
  flags out-of-sequence numbering, and estimates shloka counts —
  estimates labeled as estimates.

One normalization rule threads through all of it: ACIP is *defined*
uppercase and scholarly wylie lowercase, so any uppercase-containing
token is converted via `acipToEwts` and everything else passes
through — an exact rule, not a heuristic, applied identically in the
Overlay lattice, the headword index, and the library index.

## 5. Search: the Gofer grammar, ported

Geshe Michael Roach's daily search tool for thirty years was Gofer, a
C program with a distinctive query grammar. The Search pane is a
faithful port — same grammar semantics (verified against the original
`lex.c`/`expr.c`/`near.c`), original eight-box layout — over modern
indexes:

- Combiners: **OR** (any term), **AND** (all terms in the same
  file), **NEAR/n** (all terms within *n* lines).
- Targets, freely combined: the aligned corpus, the published
  apparatus (all 2,234 footnotes and 705 bibliography entries
  extracted from the published volumes), and arbitrary folder trees
  — with the prebuilt library index answering folder queries
  instantly.
- Pronunciation search ('jangchub' finds byang chub), affix-tolerant
  matching via the `wylie_norm` shadow column, saved searches, and a
  federated "Search this Mac" mode that probes Spotlight with the
  query as typed plus its Tibetan-Unicode conversions.

## 6. OCR: line detection, CTC recognition, and the review-only rule

The Scan pane embeds a full Tibetan OCR pipeline in-process, ported
from BDRC's published models and tooling: line detection (ONNX
segmentation model — sigmoid mask, threshold, stitch, crop), line
building, then per-line **CTC recognition** with each model's own
charset, running through ONNX Runtime in an isolated `allocr`
component.

The port is proven the same way as every engine: against BDRC's own
application running the same models on the same folios — masks agree
at ≥98.6%, line geometry is exact to the millidegree, and recognized
text is byte-identical up to cross-runtime floating-point near-ties,
with every diff printed. The word-coordinate decoder
(pyctcdecode's `text_frames`) is ported byte-exact against its
oracle, which enables per-word bounding boxes on the page image.

Policy is as important as capability, and it is structural: **OCR
output is review material, always.** Recognized text arrives
banner-marked, first-pass checked against the syllable-legality
rules, and is never trusted as text. In the Input pane, OCR pre-fill
works only onto an empty page — it cannot overwrite typing (a
guard exercised by the self-test in both directions). In the
Overlay's scan follow-along, recognition is used *only as a
locator*: "locate word" draws a box around the word under the cursor
on the woodblock image; the recognized text itself is never
surfaced.

## 7. Hybrid connectivity

The core is fully offline: dictionary, corpus, all engines, search,
OCR, calendar, pedagogy — everything runs from local data with no
network. Exactly two features touch the network, both clearly marked
in the UI:

- **The Analysis pane**, which sends a passage's deterministic
  engine pre-pass to the Claude API for an eighteen-section
  grammatical report — banner-labeled AI, always, and re-checked by
  machine (§10).
- **BDRC scan fetch**, which retrieves public-domain woodblock
  images over IIIF, with attribution and license shown.

All network calls live in the app layer; `allcore` cannot make one.

## 8. The UI layer: sixteen panes, six workflows

The single window presents six workflow tabs, each holding its
panes — sixteen working areas in all:

- **Read** — *Overlay* (the flagship reader: dictionary-shaded text
  in ACIP, wylie, or Tibetan script, click-for-card, BDRC scan
  follow-along) and *Library* (the canon on disk, catalog identities
  decoded from filenames).
- **Translate** — *Draft* (the translator's bench: color-linked
  clause anchors, phrase memory, terminology check, apparatus
  tools, RTF export), *Review* (advisory-only checking of a
  finished draft), *Align* (harvesting translator-authored pairs,
  including `.hyp` imports from the original Hypercontext era).
- **Research** — *Search* (Gofer), *Lookup* (stacked
  multi-dictionary, HGM layer first and binding), *Sanskrit*
  (the full cascade plus Whitney root analysis and the
  Mahavyutpatti bridge), *Convert* (every writing system, plus the
  calendar), *Analysis* (the AI report).
- **Learn** — *Trainer* (six-layer progressive reveal down to the
  full Wilson parse) and *Drills* (exercises generated from the
  corpus — every answer is attested, never invented).
- **Input** — *Input* (the input-center workstation: scan-follows-
  typing, double-keying diff, guarded OCR pre-fill) and *Scan*
  (batch recognition, review-only).
- **Community** — *Propose* (the team's channel to the authority)
  and *Approval* (the authority's queue; nothing enters the
  registers without a ruling).

Every pane's functions are also reachable from the menu bar, a
searchable in-app Help carries a tutorial per pane plus suggested
multi-pane workflows, and a Settings dialog centralizes appearance,
reading defaults, team identity, and data location.

## 9. Testing: 38 suites and a self-testing application

Automated verification runs at two levels.

**The `ctest` battery — 38 suites, ~600k verified data points.**
Thirty-seven suites exercise `allcore` directly: the engine
batteries of §3, plus suites for the spine, lattice, reader and
Wilson parse, Gofer, library index, quotation detection, calendar,
proposals store, exports, fonts (bundled faces must fully cover a
reference syllable corpus), and more. These are not smoke tests in
the pejorative sense — the engines are diffed against their oracles
across the full data on every run.

**The 38th suite is the application testing itself.** Run with
`--selftest`, the app constructs its real panes offscreen against
the real spine and exercises them programmatically — 71 checks
asserting actual pane state: the Overlay loads and shades a
document, a known headword's card contains its gloss and tier, the
Review pane raises the register warning on a crafted input, the
Approval queue lists a seeded proposal, hostile inputs (emoji,
punctuation-only, a 64k single line) survive, and a full-volume
engine pass stays under an enforced regression ceiling. Each check
prints PASS/FAIL and the process exits nonzero on any failure — so
the self-test runs in the packaging gate (§11), and a user can run
it from Terminal to localize a problem.

The written test plan (`docs/TEST_PLAN.md`) classifies every feature
of every pane into three tiers — core battery, self-test, or
manual — with the manual tier scripted as a pre-release checklist.
As of this writing the automation queue is empty: everything
automatable is automated; what remains manual (rendering quality,
network, real scans) is manual by nature.

## 10. The honesty architecture

Several mechanisms exist solely to keep the tool honest about what
it knows:

- **Tier labels everywhere.** Auto-aligned glosses are PROVISIONAL
  and *look* provisional — an amber-leaning tint wherever they
  appear, preserved even through the performance rewrite (§11's
  format-run merging was designed around keeping the tint).
- **Register warnings, never auto-resolution.** Single-register
  terms carry warnings; honorific terms wear badges with the
  ordinary counterpart alongside.
- **AI is banner-labeled and machine-checked.** The Analysis pane's
  report opens under an AI banner, and a QC validator re-checks
  every quotation the model makes against the deterministic
  extraction — the machine verifies the machine.
- **Mutations are approval-gated.** Community proposals wait in the
  Approval queue; a register change applies only on an authority's
  ruling, stamped with name and date; dictionary candidates export
  to the data project rather than editing the corpus — the app never
  owns the data it serves.
- **Spelling doubts are ruled, not corrected.** The legality
  layer's doubts gather into a working list any reader can file to
  the Approval queue. An approved flag exports as a confirmed input
  error — the app never edits the texts — while a declined flag
  enters a team-wide allowlist read from the shared store at launch:
  the software stops doubting a form the authority has ruled valid.
- **Two apparatus tiers, structurally enforced.** OFFICIAL notes and
  bibliography are extracted from published volumes only and are
  never hand-edited (there is no in-app approve button *by design*);
  everything else is PENDING and labeled so.

## 11. Performance engineering: the Overlay as a case study

The Overlay must open a complete canonical volume — a 1.2 MB ACIP
file, 344,940 tokens producing 472,112 dictionary spans — and stay
interactive. The first attempt hung indefinitely. The fix is a
worked example of measurement-driven engineering, and every step is
recorded in the repository history; the probe lineage on that volume
reads **infinite → 15.9 s → 10.8 s → 4.3 s → 3.1 s → 1.9 s**
end-to-end, with the pure engine pass now at **424 ms** (a 15-second
regression ceiling is enforced by the self-test).

The layers, in the order they were found by stack sampling and
scaling curves:

1. **Algorithmic: precomputed span vectors.** The loader called a
   per-token span scan — O(tokens × spans), on the order of 10¹¹
   operations for a full volume. Provisional-status and
   phrase-membership are now precomputed in the same single pass
   that builds the color map.
2. **Quadratic depth nesting, twice.** Overlap depth (which drives
   the nested wash) was a pairwise pass — quadratic, and present in
   *both* build paths, the second hiding behind the first fix. It is
   now O(n log n) via a Fenwick tree over span ends: 472,112 spans
   in 0.42 s, down from 167 s.
3. **Merged format runs.** ~10⁵ per-token character-format calls
   each paid a Qt layout invalidation. The pass now emits merged
   runs — identical adjacent formats become one call — inside a
   single edit block.
4. **Progressive apply.** The first 4,000 format runs land before
   the text appears; the rest apply in background slices,
   generation-guarded against reloads — opening feels instant at
   any size.
5. **Lazy layout.** The view switched from QTextEdit to
   QPlainTextEdit, whose per-block lazy layout eliminated a
   whole-document layout pass on load.
6. **O(log n) interaction.** Click and cursor hit-testing was a
   linear scan over 300k tokens; `tokenAt` is now a binary search,
   and per-token display conversion and spellcheck legality are
   cached per *distinct* token (Tibetan repeats heavily).

The matching index itself is tiered by first-two-token keys so
common syllables cannot cause thousand-candidate scans, while
preserving the original matcher's semantics (first exact wins, else
first clitic) — a property the lattice battery checks.

## 12. Packaging and distribution

Distribution is a macOS DMG, produced by `tools/package_macos.sh` —
a script that **refuses to package anything that fails the release
gate or the full test battery**: a bad build or a bad data drop
cannot become a DMG. The image contains the Release-built app with
Qt frameworks bundled by `macdeployqt`, an "ALL Tool Data" folder
(the spine and all reference payloads — drag both anywhere
together), and a three-step README for non-technical users.

Two hardening details are worth recording. First, `macdeployqt`
leaves escape hatches — surviving Homebrew rpaths and unrewritten
inter-dylib `@rpath` references — that on the build machine silently
load duplicate runtimes and on a clean machine fail outright; the
script strips and rewrites every such reference, verifies none
remain, re-signs, and launch-tests the *staged* layout rather than
the dev build. Second, the app finds its data by a deterministic
search (remembered setting → dev checkout → folder beside the
app → Application Support → a one-time remembered picker) — and if
the user cancels, it reports the missing spine rather than guessing.
Signing is currently ad-hoc (first launch is right-click → Open); a
Developer ID and notarization slot into the same script when ALL
provides them.

Windows and Linux are the second half, and the codebase is ready for
them by construction: C++20, Qt 6, CMake, no platform-only APIs in
`allcore`, SQLite vendored, fonts bundled, HarfBuzz shaping bundled
with Qt. The blockers are purely infrastructural — build machines or
CI runners for those platforms, and a round of font-rendering QA on
each (shaping *should* be identical; the project's habit is to
verify, not assume).

## 13. Honest edges

In keeping with the project's character, the known limits are
documented rather than hidden: the day-level calendar implements the
Modern Karana (correspondence with published Phugpa almanacs is a
documented open question); several reference payloads await license
resolution before they can ship; Old-Tibetan affix rules are
unported; and a small set of features is gated on external inputs
(the input-center formatting guidelines, upstream license
confirmations). The full register, item by item, is
`docs/FEATURE_STATUS.md`.

---

*Repository documents cited: `docs/TECHNICAL_OVERVIEW.md`,
`docs/TEST_PLAN.md`, `docs/FEATURE_STATUS.md`, `docs/PACKAGING.md`,
`docs/OCR_DESIGN.md`, `docs/RELEASE_READINESS.md`,
`data/help/tutorials.md`, and the project build history.*
