# Reading order: what the engine claims, and what Geshe Michael actually did

**Measured 2026-09-12.** Adam: *"my main problem in reading/translating Tibetan
is understanding in what order to read the chunks and which direction to read
the chunks"* — and then, *"we need a tool that will take input, say a paragraph
or so of text, and our tool should break down how to read/translate the
paragraph step by step."*

This file records the measurement the Walkthrough pane is built on, so the
numbers in the UI have one source and a later batch of alignment can
recalibrate them without archaeology.

## What was measured

`core/src/reader.cpp: planReading()` has been shipping reading-order guidance
since the Trainer's increment 2 — verb first, then topic, agent, unmarked
object, la don, source; genitive and `dang` chunks attach rightward. It is
labelled as guidance. It had **never been checked against a translation.**

The alignment bank (`data/alignment/alignment_full_v1.json`, 41,759 links over
1,550 segments) records, at seven depths, which Tibetan span corresponds to
which English. The links sit in Tibetan order and each carries its English, so
for any parent span the *English* order of its children is recoverable by
finding each child's English inside the parent's. That gives what GMR actually
did, against which a predicted order can be scored.

The probe (`orderprobe`) runs the **shipping** engine — `buildOverlay`,
`splitClauses`, `chunkClause`, `spotVerb`, `planReading`. Nothing was
reimplemented; the rules are only exposed. Wylie reaches it through the proven
`ewtsToAcip`.

## Finding 1 — reordering gets worse the deeper you go

Adjacent child pairs whose English comes out in the opposite order:

| level (parent → children) | spans | pairs | read backwards |
|---|---:|---:|---:|
| segment → clauses | 426 | 972 | **5%** |
| clause → phrases | 113 | 171 | 34% |
| phrase → sub-phrases | 18 | 20 | 50% |
| sub-phrase → words | 173 | 793 | **54%** |

Clause order survives into English almost intact. Word order inside a phrase
carries essentially no information about English order — 54% is worse than a
coin flip. **This is why clause-first is the right order to work in:** you are
descending from the level where sequence is reliable to the level where the
particles have to take over.

## Finding 2 — the engine is worse than doing nothing at the top

Whole-permutation match, engine against the only baseline that matters (read
them left to right and change nothing):

| level | spans | engine | left-to-right |
|---|---:|---:|---:|
| segment → clauses | 16 | 69% | **100%** |
| clause → phrases | 57 | 40% | **60%** |
| phrase → sub-phrases | 14 | **57%** | 36% |
| sub-phrase → words | 120 | **50%** | 31% |
| **all** | **207** | **49%** | **44%** |

The engine's verb-first/genitive-rightward rules are earned at the bottom two
levels and actively harmful at the top two. At the clause level, reordering can
only break what was already right, and it does so 31% of the time. Finding 1
corroborates independently and with far more data: 5% of 972 clause pairs.

## What is NOT claimed

- **Not every disagreement is the engine's error.** GMR translates
  idiomatically. He renders a title appositive as "Pabongka Rinpoche, the
  Holder of the Diamond" because that is English word order, not because the
  Tibetan reverses. An unknown share of the gap is idiom.
- **The middle rows are thin.** 57 spans at clause → phrases, 14 at phrase →
  sub-phrases. The headline "engine worse than baseline in the middle" rests on
  the 57. Finding 1's clause row and the word row are solid; the middle is not.
- **3.7% of the corpus.** 1,550 aligned segments of 42,199.

## What was done about it

The Walkthrough pane (`app/walkthrough_pane.inc`) does not present step 4 as an
answer. It shows **both** orders side by side, quotes the figures for its own
level, and says which one the evidence favours — currently the written order.
Gates in the pane's `selfTest` fail if a later edit reduces step 4 to the
engine's ruling alone, or lets the quoted figures drift from the constants.

**The engine itself was deliberately NOT changed.** Retuning `planReading` to
fit 57 spans would be overfitting. The correction waits for alignment data that
can carry it; labelling it honestly does not wait. (Adam's ruling, 2026-09-12:
"do the development right now, but the refinement later after we've got the
entire corpus aligned.")

## Recalibrating after more alignment

1. Re-run the two measurements against the grown `alignment_full_v1.json`.
2. Update the constants in `namespace walk` — `kOrderSpans`, `kOrderAgree`,
   `kOrderPlain`, `kClauseSpans`, `kClauseEngine`, `kClausePlain`,
   `kClausePairs`, `kClauseBackPct`. The pane's prose reads from them.
3. Re-run the app selftest. The gates quote the constants, so stale text in the
   UI fails rather than shipping.
4. When the middle level carries enough spans, revisit `planReading` itself —
   the open question is whether verb-first is over-applied above the word level.
