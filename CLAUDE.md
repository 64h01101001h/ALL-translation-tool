# Diamond Cutter Translation Tool — Claude Code Project Instructions
**ALL = Asian Legacy Library · ACIP = Asian Classics Input Project**

You are building the Diamond Cutter Translation Tool: a multifunctional desktop suite for Tibetan digital texts and translations.

## Tech stack (as-built decision, 2026-08-06 — supersedes "Swift ports" wording below/in the roadmap)
**C++20 core (`allcore` static library) + Qt 6 UI.** Adam's call: C++ for the final product; Qt 6 chosen for proper Tibetan complex-script shaping (bundled HarfBuzz) and future Windows/Linux builds for the input centers. macOS-first, cross-platform-clean.
- `allcore` owns: SQLite (FTS5) spine access, engine ports, the Gofer query grammar. The UI layer only calls `allcore`.
- Engine ports are **C++ ports** (not Swift), same inviolable rules: port from the canonical Python file, prove with its validation battery as unit tests (CTest), never reconstruct from spec. The Python engines in `engines/` stay the canonical oracles — diff-test C++ vs Python across the full dictionary where practical.
- Build: CMake; SQLite via vendored amalgamation (FTS5 compiled in); batteries run via `ctest`.
- Validation-battery harness (Python side): `tools/validate_engines.py`. Spine builder: `tools/build_spine.py` → `build/hgm_spine_v27_2.db`; smoke CLI `tools/spine_query.py`. Read `DIAMOND_CUTTER_TRANSLATION_TOOL_ROADMAP.md` (project root) first — it is the master plan. Read `Handoff/HANDOFF.md` + `Handoff/README.md` for the data project's full history.

## Inviolable rules (inherited from the Geshe Michael Roach Dictionary project)
1. **hgm_gloss is binding; everything else is reference.** The dictionary's `hgm_gloss` field carries Geshe Michael Roach's (HGM's) English equivalents — the whole point of the project. Hopkins, {C}, {PH}, {GD}, {LC} material is reference comparanda only and must never be presented as, or promoted into, HGM equivalents. Machine work may MATCH HGM's English from corpus evidence, never COMPOSE it.
2. **Never reconstruct canonical engines from spec.** The Python engines in `Handoff/tools/` are the authority. When porting to Swift, port from the file and prove the port with its validation battery (below). A port that fails its battery does not ship.
3. **Never guess.** Failed conversions/parses are flagged and surfaced, not silently approximated. Follow the existing flag conventions (`pronunciation-review`, `tibetan-review`, `⟨wylie⟩` markers).
4. **Provenance is sacred.** Generated data is tagged (`tibetan_source: generated-ewts-v27-1`, `scaffold_added`, `source_wave`, `{LC}` etc.). Preserve these fields through any schema work.


## REQUIRED ACCESS — verify at session start, prompt Adam for anything missing
At the start of a session, check that each path below is readable. **If any path is not accessible, stop and ask Adam to grant access to it before proceeding** (he wants to be prompted rather than trying to remember the list). `~` = /Users/adamderickandrade.

1. `~/Desktop/HGM DICTIONARY - TRANSLATION APP` — THE data project: Handoff/ (release packages, corpus, canonical engines, HANDOFF.md, README), Input/LokeshChandra/, Guides/, roadmap, this CLAUDE.md.
2. `~/ACIP Software Developmeent/Gofer Developmetn Files` — Gofer search tool (query-grammar reference: lex.c, expr.c, near.c).
3. `~/ACIP Software Developmeent/Ven. Phil's Development Folder/Mac_OSX/ACIPHypercontext Tool` — overlay-pane UX reference + TibetanDictionary.dic payload.
4. `~/ACIP Software Developmeent/Ven. Phil's Development Folder/Mac_OSX/LokeshChandraDictionary` — LkCh reader app + LkChDictionary.dic/.ndx payload.
5. `~/ACIP Software Developmeent/Ven. Phil's Development Folder/Mac_OSX/CreateLokeshChandraDIctionary` — LkCh builder (format reference; git history holds ~110MB source blobs).
6. `~/ACIP Software Developmeent/Ven. Phil's Development Folder/Mac_OSX/FixLokeshChandra` — LkCh data fixer (format reference).
7. `~/ACIP Software Developmeent/tibetan-spellchecker` — CC0 syllable-legality data (spellcheck pane + validation).
8. `~/Tibetan Translation Tool` — THL DictionarySearchStandalone.jar + thl-dicts (thl.wrd/thl.def) lookup-pane reference + extraction target.

Reference documents (inside folder 1): `Guides/ACIP_Sanskrit_Input_Code_Standards.pdf` (the official Sanskrit code standard behind sanskrit_convert.py's three ACIP styles).

**Still to be supplied by Adam (ask when the relevant task starts):**
- Input-center raw data samples + the GMR/publishing-team formatting guidelines (for the input-center formatter — roadmap item A).
- ~~The "ACIP Standards for Simplified Tibetan and Sanskrit Pronunciation" document~~ — FOUND 2026-08-06 inside `~/Desktop/Thursday Database-Cataloging 6 4 26 Meeting/ACIP Catalog Release IV.pdf` (manual pp. 173–178); banked at `docs/standards/ACIP_PRONUNCIATION_STANDARD.md` with engine-validation findings (51/52 pure-Tibetan agreement). NOTE: the same PDF's "ACIP Tibetan Input Code Standards" chart pages (153–160) are EMPTY image placeholders in this copy — ask Adam for an intact copy if that chart is ever needed.
- Adam's own reference Xcode projects for the overall tool vision (mentioned 2026-08-04, not yet shared).

## Data assets (source of truth: `Handoff/release/`, current v0.27.2)
- `hgm_dictionary_v27_2.json.gz` — THE MASTER. 105,634 unified entries: 12,004 HGM-glossed (tiered: curated > glossary > auto-aligned provisional), 79,305 Lokesh Chandra layer entries (awaiting), Tibetan unicode on all entries, pronunciations, ACIP, variants, evidence, corpus occurrences.
- `full_parallel_corpus` (Handoff/data/, .gz) — **42,199** aligned
  Tibetan–English segments across HGM's courses. The concordance/
  oversight evidence base. (Corrected 2026-08-23: this line said
  35,601, which was the pre-v32 corpus. The SHIPPED spine records its
  own provenance — `source_corpus=full_parallel_corpus_v32.json.gz`,
  `n_corpus_segments=42199` — and that is the number the app can
  demonstrate live, so it is the one to quote.)
- `hgm_reverse_index_v27_2.json` — English → entries.
- `Input/LokeshChandra/` — standalone LC extract (88,973 headwords with IAST Sanskrit + citations).
- Release packages are versioned; the app should IMPORT a release, never own the data.

## Canonical engines (`Handoff/tools/`) and their acceptance tests
- `pron_engine.py` — wylie → GMR-convention phonetics. Battery: 42/42 spot battery + 2,118 card lines in `Handoff/data/card_phonetics.json`.
- `hgm_tools.py` — ACIP ⇄ EWTS (`acip_to_ewts`), aligned-text parser.
- `ewts_unicode.py` — wylie → Tibetan unicode. Battery: 98.88% variant-aware exact match on the master's 26,318 (wylie, tibetan) ground-truth pairs; regression-test any change against this.
- `sanskrit_convert.py` — IAST ⇄ Tibetanized ACIP ⇄ ACIP Sanskrit Input Code (a#/r%/n~ style) ⇄ next-letter style (a-/n.) ⇄ EWTS ⇄ Tibetan unicode ⇄ Devanagari ⇄ pronunciation. Batteries: 92.2% on 4,582 LC (IAST, ACIP) pairs; input-code/next-letter styles match the official standard doc examples (prama#n%a / prama-n.a / {PRA M'A nA}). Pronunciation style still provisional pending the separate ACIP pronunciation standards doc.
- Also: `glyph_decoder.py`, `hopkins_parser.py`, `practice_parser.py` (ingest-side; not needed in the app), `generate_package.py` (release generator — do not reimplement its format rules from memory).

## Architecture (agreed)
One app, one shared core, multiple panes. Core = SQLite (FTS5) database compiled from the release package + corpus, plus Swift ports of the engines. Panes: Search (Gofer-grammar: OR / NEAR-within-N-lines), Lookup (THL-style stacked multi-dictionary), Overlay (Hypercontext-style click-word-in-context: segmentation → tier-ranked HGM equivalents → corpus concordance), Convert (wylie/ACIP/unicode + full Sanskrit converter), Phonetics, translator oversight (side-by-side with provisional-tier and register warnings).
Build order = the numbered to-do in the roadmap: SQLite spine first.

## Legacy reference projects (read-only inspiration; paths in roadmap §Sources)
Gofer (search grammar: lex.c/expr.c/near.c) · ACIPHypercontext (overlay UX; contains TibetanDictionary.dic — extract but CHECK LICENSING before shipping) · tibetan-spellchecker (CC0 syllable data — use freely) · THL tool (thl.wrd/thl.def — extract, per-dictionary licensing review) · LokeshChandra trio (already extracted v0.27).

## Working conventions
- UI displays tier labels honestly: auto-aligned glosses are PROVISIONAL and must look provisional.
- Single-register terms (e.g. bsod nams: "goodness" in prayers vs "merit/good deeds" in prose) carry register warnings — never auto-resolve.
- File-size notes: master json is 63MB+ plain (ship/store gz); corpus 27MB.
- The HGM data project continues in Claude Cowork sessions (ingests, releases, mining); this repo consumes its releases. Don't fork the data.

## The engineering-firm skill set (installed 2026-08-21, Adam's directive)
`.claude/skills/` carries the vetted team-workflow skills (sources:
obra/superpowers · anthropics/skills · levnikolaevich/claude-code-skills;
every file reviewed before install). They govern how nontrivial work runs:
- **brainstorming** before any creative/feature work (intent before code)
- **writing-plans** for multi-step specs → **subagent-driven-development**
  / **dispatching-parallel-agents** to execute · **executing-plans**
- **test-driven-development** for features/bugfixes ·
  **systematic-debugging** for defects
- **requesting-code-review** / **receiving-code-review** /
  **ln-11/ln-12 reviewers** before merges (the §10 adversarial-review
  habit, now standing)
- **verification-before-completion**: evidence before assertions, always
  (the sweep/battery/press culture, codified)
- **using-git-worktrees** · **finishing-a-development-branch** ·
  **ln-41/ln-42 test planning** · **skill-creator/writing-skills** for
  minting house skills (candidates: the press ritual, the block-edit
  discipline, the constitution).

**The design wing** (installed 2026-08-21, same vetting): **design-critique**
(Nielsen + UX laws + IA + UX-writing review) · **nielsen-heuristics-audit** ·
**don-norman-principles-audit** · **accessibility-audit** (WCAG 2.2) ·
**interaction-design** · **design-systems** · **journey-mapping** ·
**ux-designer** (encyclopedic references). Composition rules:
- GUI-touching work opens with **brainstorming + design-critique**;
  ships through **accessibility-audit + verification-before-completion**.
- The Master UX Oversight governance (§63/§53) and the honesty rules
  REMAIN SUPREME — these skills inform, Adam's rulings decide, and no
  heuristic ever overrides a provenance/honesty label.
- New surfaces get a heuristic pass BEFORE Adam sees them; findings
  file into docs/FINDINGS.md with dispositions.
