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
| segment → clauses | 425 | 83% | **90%** |
| clause → phrases | 112 | 51% | **59%** |
| phrase → sub-phrases | 18 | **61%** | 44% |
| sub-phrase → words | 169 | **36%** | 23% |
| **all** | **724** | 67% | 68% |

### Correction, same day

The first pass of this table read 16 / 57 / 14 / 120 spans and 49% against
44% overall, and it was wrong — not in shape, in size. The probe walked only
the FIRST clause of each span, so any child sitting in a second clause had no
chunk to map onto and the comparison silently dropped it: **523 of 730 spans
were unmappable**. Fixing the probe to walk every clause brings that to 6, and
the sample from 207 to 724.

What survived: the engine is worse than doing nothing at the top two levels
and better at the bottom two. What did not: the claim that it beats the
baseline overall. At 67% against 68% it does not — that 5-point lead was an
artefact of which spans happened to map.

The lesson is the one this file is about. A measurement that has never been
checked is not evidence, and mine had not been. The `unmappable` count is
printed first in the script's output for that reason.

The engine's verb-first/genitive-rightward rules are earned at the bottom two
levels and actively harmful at the top two. At the clause level, reordering can
only break what was already right. Finding 1 corroborates independently and
without going near the probe: 5% of 972 clause pairs come out backwards.

## Finding 3 — which of these gaps are real

Whole-permutation percentages hide how few spans actually separate two
orderings. McNemar on the paired comparison — only the spans where the engine
and the written order *disagree* carry any information:

| level | n | engine right, written wrong | written right, engine wrong | p (two-sided) |
|---|---:|---:|---:|---:|
| segment → clauses | 425 | 1 | 30 | **<0.0001** |
| clause → phrases | 112 | 6 | 15 | 0.078 |
| phrase → sub-phrases | 18 | 3 | 0 | 0.25 |
| sub-phrase → words | 169 | 37 | 15 | **0.0032** |
| **all** | **724** | 47 | 60 | 0.25 |

Two results are established and two are not:

- **Established: do not reorder clauses.** 30 spans where the written order is
  right and the engine wrong, against 1 the other way. This is as clean as
  this data gets.
- **Established: the ruling earns its keep at the word level.** 37 against 15,
  p=0.003.
- **NOT established: the middle level**, which is exactly where the Walkthrough
  pane's step 4 operates. Only 21 spans separate the two orderings and p=0.078.
  The 51-vs-59 split is a trend. The pane said "the better bet" and has been
  corrected to say the difference does not hold up — overstating in the
  direction of humility is still overstating.
- **NOT established: overall.** 47 against 60, p=0.25. A tie, as the
  percentages suggested.

**What would settle the middle level.** Significance turns on the discordant
count, which is currently 21. At the same 6:15 ratio it would take roughly 40
discordant spans to reach p<0.05 — about **200 aligned spans at clause →
phrases**, against today's 112. Slightly under double the present coverage at
that depth.

## Finding 4 — the verb-first hypothesis is false

The obvious explanation for the engine's deficit above the word level was that
`planReading` fronts the clause-final verb and Geshe Michael often does not.
Tested by scoring a variant that keeps every other rule and leaves the verb in
its written position — no change to the engine, the probe simply reports which
chunk it fronted:

| level | engine | engine without verb-fronting | written |
|---|---:|---:|---:|
| segment → clauses | 83% | 83% | 90% |
| clause → phrases | 51% | 50% | 59% |
| phrase → sub-phrases | 61% | 61% | 44% |
| sub-phrase → words | 36% | 35% | 23% |
| **all** | **67%** | **66%** | **68%** |

Verb-fronting costs one point overall and nothing at any single level. It is
not the problem. What remains is the *role* reordering — topic, agent, unmarked
object, la don, source — since the written order is what you get when the
genitive rule is applied and no role reordering happens at all. That is the
hypothesis to test next, and it needs the sample above before it can be acted
on.

## What is NOT claimed

- **Not every disagreement is the engine's error.** GMR translates
  idiomatically. He renders a title appositive as "Pabongka Rinpoche, the
  Holder of the Diamond" because that is English word order, not because the
  Tibetan reverses. An unknown share of the gap is idiom.
- **One row is still thin.** 18 spans at phrase → sub-phrases. Everything else
  now rests on 112 to 425 spans. The 18 is the only figure in the table that
  should not be quoted on its own.
- **3.7% of the corpus.** 1,550 aligned segments of 42,199.

## What was done about it

The Walkthrough pane (`app/walkthrough_pane.inc`) does not present step 4 as an
answer. It shows **both** orders side by side and quotes the figures for its own
level — 51% against 59% over 112 aligned spans, with only 21 of those separating
the two — and says **neither column is established**, offering the written order
as the simpler bet rather than the better one.

*(Corrected 2026-09-13: this paragraph used to say the pane "says which one the
evidence favours — currently the written order". It does not, and a gate holds
it to not doing so. Overstating in the direction of humility is still
overstating, and describing the pane as doing that was a third thing again.)*

Gates in the pane's `selfTest` fail if a later edit reduces step 4 to the
engine's ruling alone, lets the quoted figures drift from the constants, or
drops the sample size.

## The drill that came out of it

A fourteenth Drills mode, **"Reading order — which chunk does his English take
first?"** The chunks are presented in the order they are WRITTEN and the
question is what order his English takes them in.

It is deliberately not scored against `planReading`. The key is recovered from
the alignment bank — where each child span's English sits inside its parent's —
so the answer is **attested**, and the 51%-against-59% problem never touches
it. The pool skews towards spans that actually reorder, because a card whose
answer is "1 2 3" teaches a learner to type 1 2 3, and a gate fails if typing
the written order is ever accepted when his order differs.

This is the complement to the existing **Chunk order** drill, which scrambles
the Tibetan and asks for the Tibetan back. That one trains Tibetan word order
and says nothing about the mapping into English, which is the difficulty Adam
actually named. Both are worth having; only one of them was there.

**The engine itself was deliberately NOT changed.** Retuning `planReading` to
fit 112 spans would be overfitting. *(Corrected 2026-09-13: this said 57, which
was the first pass's count — the probe that walked only the first clause of each
segment and dropped 523 of 730 spans. The corrected figure is 112, and it is
`kClauseSpans`.)* The correction waits for alignment data that
can carry it; labelling it honestly does not wait. (Adam's ruling, 2026-09-12:
"do the development right now, but the refinement later after we've got the
entire corpus aligned.")

## Recalibrating after more alignment

1. Re-run the two measurements — `tools/reading_order/`, one command each, and
   check the `unmappable` count before believing the rest.
2. Update the constants in `namespace walk`. **Four of them are load-bearing**
   — the pane's prose quotes `kClauseEngine`, `kClausePlain`, `kClausePairs`
   and `kClauseBackPct`, and gates pin all four, so a stale one fails the
   build. `kClauseSpans` and `kClauseDiscordant` joined them on 2026-09-13,
   when step 4 started printing its sample size.
   **The rest are a record, not a mechanism**: `kOrderSpans`, `kOrderAgree`,
   `kOrderPlain`, `kWordEngine` and `kWordPlain` are referenced by nothing at
   all. Updating them changes no output and breaks no gate — which is worth
   knowing before trusting this step. Seven of eleven were in that state when
   it was checked, and that is exactly how the pane's own header came to be
   quoting a retracted 40%/60% months after the correction.
3. Re-run the app selftest. The gates quote the constants, so stale text in the
   UI fails rather than shipping.
4. When the middle level carries enough spans, revisit `planReading` itself —
   the open question is whether verb-first is over-applied above the word level.
