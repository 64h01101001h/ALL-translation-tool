# Orientation

## What this is

**The Diamond Cutter Translation Tool** — a desktop application (C++20 + Qt 6,
macOS-first, cross-platform-clean) with an iOS companion, built for the Asian
Legacy Library. It reads, searches, converts, aligns and teaches classical
Tibetan, and it runs fully offline.

Adam Derick Andrade builds it. Geshe Michael Roach (**GMR**) is the translator
whose English the whole system is organised around. **ACIP** is the Asian
Classics Input Project; **ALL** is the Asian Legacy Library.

## Why it exists, and what makes it unlike other tools

Every other Tibetan tool teaches *a language*. This one teaches *a library*:
42,199 aligned Tibetan–English segments in which **one hand wrote all the
English**. That closure buys three things nothing else can offer.

- **Every answer key is attested.** When the tool says "this means X here", it
  is quoting GMR's own published rendering of that exact line, with a citation
  — not a dictionary's opinion and not a machine's paraphrase.
- **Ambiguity is visible instead of hidden.** Where the same Tibetan is
  rendered two different ways, that is not noise between translators; it is one
  translator's judgement, and it can be shown as the lesson.
- **The silences are recorded.** The alignment layer marks spans where a
  Tibetan morpheme has no English exponent at all. No glossary can represent
  that, and it is the hardest fact for a beginner to accept.

## The architecture, in one screen

```
  data (external project)          this repository
  ─────────────────────            ───────────────
  hgm_dictionary_v27_2.json.gz  ┐
  full_parallel_corpus_v32.gz   ├─► tools/build_spine.py
  hgm_reverse_index_v27_2.json  ┘        │
                                         ▼
                            build/hgm_spine_v27_2.db   (SQLite + FTS5)
                                         │
                                         ▼
                     core/  =  "allcore"  (C++20 static library)
                        · owns ALL spine access
                        · owns the engine ports
                        · Qt-free, ML-free
                                         │
                                         ▼
                     app/   =  Qt 6 UI, ~22 panes
                        · app/main.cpp is ONE translation unit
                        · panes are .inc files included into it
                        · the UI only ever calls allcore
                                         │
                                         ▼
                     ios/   =  Swift companion
                        · generates NOTHING; consumes a pack
                        · the pack is built on the Mac by
                          tools/build_drill_pack.cpp
```

Two consequences you need immediately:

- **`app/main.cpp` is enormous** (tens of thousands of lines) because the panes
  are `#include`d into it. This is deliberate and is recorded in `docs/ADR.md`.
  Do not "fix" it. Editing means locating the right region, not restructuring.
- **The phone generates nothing.** Every drill it shows was built on the Mac by
  `allcore`. If the phone is wrong, the pack builder is wrong.

## The engines

Four canonical Python engines in the external data project are the **authority**
for conversion: wylie↔Tibetan unicode, ACIP↔EWTS, pronunciation, and Sanskrit.
Each has been ported to C++ in `core/`, and each port is proved by the Python
engine's own validation battery run as a ctest suite.

**You may not reconstruct an engine from its specification.** Port from the
file; prove with the battery. A port that fails its battery does not ship. This
is Inviolable Rule 2 and it is not negotiable — see `01_INVIOLABLE_RULES.md`.

## Two script systems, and the trap between them

Tibetan is written here three ways, and they are not interchangeable:

| | what it is | example |
|---|---|---|
| **Unicode** | actual Tibetan script | སེམས་ཅན |
| **Wylie (EWTS)** | lowercase transliteration | `sems can` |
| **ACIP** | uppercase transliteration | `SEMS CAN` |

**The trap that has bitten this project twice:** ACIP lowercase `d` is retroflex
ཌ, while wylie `d` is dental ད. Feeding a wylie string into an ACIP converter
produces valid-looking, *wrong* Tibetan. The alignment layer carries both a
`tib` field (**wylie**) and a `tib_acip` field (**ACIP**) for exactly this
reason. Check which one you have before converting.

EWTS is also a live grammar, not a lookup table: `.` is a disjunct operator
(`g.ya` = གཡ but `gya` = གྱ), and `-` fails loudly as `⟨…⟩` rather than
guessing.

## Where to look for things

| you want | go to |
|---|---|
| the master plan | `DIAMOND_CUTTER_TRANSLATION_TOOL_ROADMAP.md` |
| project rules and access list | `CLAUDE.md` |
| why a decision was made | `docs/ADR.md` |
| the alignment layer's schema and rules | `docs/ALIGNMENT_LAYER_SPEC.md` |
| what is open, by blocker | `docs/RELEASE_PUNCHLIST.md` |
| the raw backlog | `TODO.md` (large; the punchlist is its working face) |
| what shipped each day | `docs/digests/` |
| the learning half of the app | `docs/LEARN_TAB_VISION.md` |
