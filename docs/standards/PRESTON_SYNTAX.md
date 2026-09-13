# The eight syntactic elements, and what they changed in the engine

**Source.** Craig Preston, *How to Read Classical Tibetan, Volume One: Summary
of the General Path* (Snow Lion). Front matter pp. xi–xviii and the grammar
review, p. 59. Preston works in the Hopkins → Napper → Wilson line and names
his departures from Wilson explicitly. Adam supplied the pages 2026-09-12.

This file records the **grammar** and what we did with it. It does not
reproduce the book; anyone working on this should read Preston directly.

## The eight elements

On the most basic level of syntactic function, Classical Tibetan has eight:

| element | case(s) |
|---|---|
| agent of a transitive verb | 3rd |
| object of a transitive verb | 1st or 2nd |
| complement to the object | 1st or 2nd |
| subject of an intransitive verb | 1st, 4th, 7th |
| complement to the subject | 1st or 2nd |
| qualifier of the verb | 2nd, 3rd, 4th, 5th, 7th, s.p. |
| transitive verb | — ends the sentence |
| intransitive verb | — ends the sentence |

Typical written order, transitive: agent → object → complement to object →
qualifier → **verb**. Intransitive: subject → complement to subject →
qualifier → **verb**. Any function may be filled by a word, a phrase or a
clause. Qualifiers answer *how, when, where, for whom, why, because of what,
for what purpose*.

Preston's departure from Wilson, which we adopt: **"subject" belongs to
intransitive constructions only and "agent" to transitive ones only.** Against
ordinary English usage, deliberately — it makes transitivity visible in the
label itself.

## Why this mattered here

I had spent the evening proving `planReading` performs worse than doing nothing
above the word level (docs/READING_ORDER_MEASUREMENT.md) and could not say why.
I tested verb-fronting as the cause and falsified it.

Preston's system explains the shape of the problem. He never teaches a
permutation. He teaches the student to identify **which of eight functions**
each chunk fills and **what relationship it bears to the verb**; the English
order falls out of the relationships. Our engine applies a fixed role-ordering.
A fixed permutation scoring 51% against a relational system is what you would
expect.

## What was implemented (2026-09-12)

`core/include/allcore/cases.h`, `core/src/cases.cpp`, battery `cases_smoke`.

1. **Case numbers.** `caseOf(marker)` maps a chunk's particle to the case(s) it
   declines into. The unambiguous families — *byed sgra* (3rd), *'brel sgra*
   (6th), *'byung khungs* (5th) — resolve to one number.
2. **The la don refusal.** `la na ru su tu du r` decline into the **2nd, 4th OR
   7th** and nothing in the particle distinguishes them — the relationship
   does. `caseOf` returns all three, carries no gloss, and the pane says the
   particle cannot choose (rule 3). Preston reads his own example's `la` as
   2nd, and he does it from the relationship, not the particle. A gate fails if
   the table ever resolves a la don particle to a single case.
3. **The first case is a fact, not an absence.** An unmarked noun returns case
   1, and the pane says so rather than printing "no particle".
4. **Transitivity, read off Wilson's own class names.** The `agentive-` classes
   (V, VI) are transitive; `nominative-`, `purposive-` and `locative-nominative`
   are not. Nothing new is decided — the split was already in the names.
5. **The unstated-agent caution.** Preston: a Tibetan sentence is *not*
   conclusively intransitive because no agent appears in it. When a transitive
   verb has no 3rd-case chunk, the pane says the agent is unstated and that
   this is not evidence of an intransitive construction.
6. **The predicate chunk is split for display.** `chunkClause` closes a chunk
   at each case particle, so the final chunk swallows the object *and* the
   verb: on Preston's own sentence it produced one row, "CHOS BSTAN —
   predicate slot", where his analysis has two. `spotVerb` already knows where
   the verb starts, so the row is split in the Walkthrough. **The engine is
   unchanged** — see below.

Checked against Preston's own example (p. xvii), element for element:

    [1] SANGS RGYAS KYIS  kyis  3   agent of the transitive verb
    [2] RGYA GAR LA       la    2/4/7  qualifier — place of activity
    [3] CHOS              —     1   object of the transitive verb
    [3b] BSTAN            —     —   the transitive verb, ending the clause

## Errata in this file

**Corrected 2026-09-13.** The table above recorded "complement to the object |
2nd". Preston's p.xv chart puts complements to objects *and* subjects in the
**1st or 2nd**. `cases.cpp` had it right — case 1 includes `ComplementToObject`
— so this file contradicted both the source and our own implementation, and
would have sent the next implementer to "fix" correct code.

A misquotation of a source document banked in the repository is exactly what
the errata register exists for, and this one was mine. Found by the adversarial
pass over the chart inversion, not by re-reading.

## A second tradition, and what it corroborates

Everything above descends from one lineage: Hopkins → Napper → Wilson →
Preston. Five hand-kept particle/case tables in `allcore` all trace to it and
nothing checks them against anything outside, so they can be perfectly
self-consistent and wrong together.

Two grammars outside that line are now local (gitignored,
`data/reference/grammars/`), both public domain by age:

- **Jäschke, *Tibetan Grammar* (1883)** — the 19th-century European
  philological tradition.
- **Hannah, *A Grammar of the Tibetan Language* (1912)**, Baptist Mission
  Press, Calcutta.

**First use, and it corroborates the correction made yesterday.** Jäschke
reckons *seven* cases, but the inventory he lists includes the vocative, and he
states that the unaltered form of the noun carries the functions of the
nominative, the accusative **and** the vocative. He counts seven only because
nominative and accusative share one form.

So two traditions, 120 years apart, agree on the fact that matters to us: **a
bare noun is not proof of the nominative.** That is the caveat `caseOf("")`
began carrying on 2026-09-12 on Preston's authority alone. It now has
independent support, which is a different and stronger thing than one more
voice from the same school.

His naming differs — instrumental/agent, terminative — and that difference is
itself useful: it is a reminder that "3rd case" is a label from one system, not
a fact about Tibetan.

**Not yet checked against the external grammars**, and each is a place our
tables may be self-consistently wrong: whether the gis-family has non-agentive
uses (p.xxv says three, and our own `verbclass.h` already knows one of them);
whether fused `r` is a case particle at all; and the four irregular syntaxes.

## Corrections from pp. 217–220 (Adam's scan, 2026-09-12)

Two things implemented earlier the same day were wrong, and his own
abbreviation tables say so.

**There are EIGHT cases, not seven.** p.218 lists nineteen case-marking
particles across eight cases, and the eighth — the **vocative** — takes no
particle, the same surface as the first. `caseOf("")` said "first case" flatly.
It now carries the caveat that a bare noun is not by itself *proof* of the
nominative. The caveat is carried rather than folded into the case list,
because marking every unmarked chunk `1/8` would drown the la don ambiguity
that actually bites.

The eight: 1 nominative (no particle) · 2 objective (la don) · 3 agentive ·
4 beneficial/purposive (la don) · 5 originative · 6 connective · 7 locative
(la don) · 8 vocative (no particle).

**Wilson's eight verb classes fall in THREE groups, not two** (p.218):

| group | classes | shape |
|---|---|---|
| 4 intransitive | nom-nom, nom-loc, nom-obj, nom-s.p. | subject in the 1st |
| 2 transitive | ag-nom, ag-obj | agent in the 3rd, object in the 1st or 2nd |
| **2 specialized** | b/p-nom, loc-nom | **subject** in the 4th or 7th, **object** in the 1st |

The specialized pair breaks the clean mapping. They carry an object like a
transitive verb and call their actor a *subject* like an intransitive one, and
p.219's entry for loc-nom says outright "class of **transitive** verbs whose
subject is in the locative (7th) case". `isTransitive` answered false for both;
`transitivityOf` is now three-valued and a gate fails if either is ever
collapsed into a bucket to tidy the enum.

**Still to act on, from the same pages.** p.219–220 record four *irregular*
syntaxes — `attrib. loc-nom`, `attrib. nom-loc`, `irreg. nom-s.p`,
`irreg. s.p.-nom` — each described as "syntactically irregular but frequently
seen alternative syntax with **normally agentive** verbs". That is a
cannot-infer of the first importance: a verb's class does **not** determine its
syntax in a given instance. Nothing in the engine knows this yet.

## Volume two: attributive syntax, and the assumption it breaks

**Adam scanned pp. 60, 70, 162–163 and 200 on 2026-09-13**, the four pages the
Index of Diagrammed Syntactic Elements lists under *attributive syntax*.

Everything above this section treats a verb's Wilson class as settling the
shape of its clause. Volume two says otherwise, on four pages, in four
wordings: a **normally transitive verb can be used with a nom-loc syntax**. In
that reading its actor is a **subject**, not an agent, and what is attributed
sits in the **7th case**. Page 219's abbreviation list already carried the
names — `attrib. loc-nom` (attributive subject in the 7th, object in the 1st)
and `attrib. nom-loc` — and I had read that page without seeing what it implied.

The pane had been announcing "this is a transitive verb, so the clause has an
agent, not a subject" as though the class decided it.

**Implemented** as `attributiveSyntaxOpen` — which **reports** the reading and
does not detect it. Nothing in the engine can detect it, and a detector would
be the guess rule 3 forbids. It fires only on the shape that admits the reading
(a normally transitive verb, no third-case chunk, and a la don chunk for a
seventh case to sit in) and says outright that both readings are live. A gate
asserts that last sentence stays, so the day someone makes it choose, the suite
objects.

Our own tables had part of this already: `verbclass.cpp`'s entry for *zer*
notes that as "is called" it shifts to Class VIII. The prose knew; the code
ignored it.

### Implied verbs

Page 60's upper diagram shows an implied verb of existence carrying a class
(nom-loc) it is not present to declare, and the volume two index lists sixteen
page references for implied verbs — twelve for the implied linking verb, four
for existence and possession.

`spotVerb` always returns something: the last dictionary word in the final
chunk. So a verbless clause got a named candidate reading exactly like a real
verb the glossary happens not to mark. Step 2 now says the clause may have no
verb at all, and calls the candidate the engine's last resort.

**Still wanted:** pp. 44, 53, 64, 65, 71 — the implied-verb examples
themselves. One diagram is enough to justify a caution; it is not enough to
build recognition on.

### A fix reverted, then found — the clause-end test

**Superseded 2026-09-13, later the same day.** The reverted attempt below was
right about the defect and wrong about the test. What separates the three
straddling spans is whether the glossed span runs to the **end of the clause**:

    dang po                 0..2 of a 0..4 clause   stops short -> a WORD
    dang bral               1..3 of a 0..3 clause   runs to end -> verb idiom
    rang bzhin gyis stong   3..7 of a 0..7 clause   runs to end -> verb idiom

A span reaching the clause end contains the predicate, so its particle is
doing particle work and needs its own chunk. A span stopping short is a
lexical word and a boundary inside it is simply wrong. `chunkClause` already
has the clause, so no reordering of `spotVerb` was needed after all.

Mutation-proved in both directions: dropping the clause-end test fires the two
guard rails and the two original Wilson gates; disabling the anchor fires the
`dang po` gate.

### The attempt that was reverted, kept for the reasoning

On p.60's passage the chunker returns `dang po gnyis la` as `DANG` +
`PO GNYIS LA` — a chunk boundary inside a word, since *dang po* is a dictionary
entry. The dictionary anchor that fixed the quotative *shes* one layer up does
not work here:

    DANG PO GNYIS LA           span 0..2  dang po
    CHOS DANG BRAL             span 1..3  dang bral
    ... RANG BZHIN GYIS STONG  span 3..7  rang bzhin gyis stong

All three straddle the boundary with the particle as the span's first syllable,
so no test on span shape separates them — yet *dang po* is a word in which
*dang* is not a particle, while the other two are verb idioms in which it is
doing its job, and Wilson is right that those need their boundaries. The anchor
broke both of those gates.

The distinction that would work is whether the straddling span contains the
clause verb, and `chunkClause` cannot ask: `spotVerb` runs after it, over the
chunks it produces. Fixing it means reordering that, which is not a change to
make on two examples.

## How often do the new cautions fire? Measured, 2026-09-13

A caution that fires on everything is wallpaper. Measured over 40 real corpus
segments drawn from `corpus_segments`, 25–80 characters, deduplicated:

| caution | fires on |
|---|---:|
| implied LINKING verb | 32% |
| implied EXISTENCE verb | 7% |
| ATTRIBUTIVE syntax open | 2% |
| agent UNSTATED | 5% |
| **verb UNVERIFIED** | **52%** |

The four cautions are proportionate. The attributive one at 2% is properly
rare, which is what you want from a reading that needs a specific shape.

**The 52% is the finding.** `spotVerb` cannot confidently identify a verb in
more than half of Geshe Michael's own corpus. That is not a display problem and
no amount of careful wording fixes it — it is the size of the gap that the
implied-verb work sits inside. Two things could be true in any mixture and this
measurement does not separate them: the verb tables and the spine's `tenses`
coverage are thin, or a large share of these segments genuinely have no finite
verb, which is what Preston's five pages would predict.

**Caveat on the 32%.** Corpus segments are lines, not always complete
sentences. A fragment with no verb and only first-case nouns looks exactly like
an implied-linking-verb clause, so this figure is an upper bound. Separating
them needs sentence-complete input, which the alignment bank has and this
sample did not use.

## Deferred, with the reason: unifying the la don resolver

`narrowLaDon` in cases.cpp and `caseLabel` in wilsonparse.cpp both resolve a la
don particle from the verb class. That is a duplicate table of exactly the kind
`table_conformance` exists to police, and it was written knowingly, held to the
older one by a gate.

**Not unified, deliberately.** Checked what it would cost:

- The gates on `caseLabel` match substrings (`.find("2nd")`, `.find("NOT 3rd")`),
  so they would survive the change. That is not the obstacle.
- The obstacle is the class-specific PHRASING. `caseLabel` says "2nd (objective:
  destination/place of activity)" under a motion verb and "7th (referential
  locative, about/toward)" under one of attitude. Rebuilding those from
  `narrowLaDon` means either changing a shipped pane's output, or moving
  display text into `allcore`, which is worse.
- The actual risk — the two drifting apart and the panes contradicting each
  other in front of a reader — is already caught by the conformance gate.

So the remaining benefit is tidiness, and the cost is touching a shipped pane's
strings. Revisit if `caseLabel` needs changing for another reason, and do it
then rather than for its own sake.

## The hypothesis this produced

The predicate chunk conflating object and verb is a better candidate for
`planReading`'s deficit than verb-fronting was: **fronting the verb silently
fronts its object too.** That would explain why removing verb-fronting changed
nothing (the conflation survives either way) and why the engine does worst
where objects are in play.

Not acted on. Per Adam's ruling of 2026-09-12 the engine correction waits for
alignment data that can carry it, and the display split above is a rendering,
not a change to `chunkClause`. Testing it properly means re-running the
measurement with a split predicate chunk — recorded here so it is not lost.

## What would help most from the rest of the book

**Appendix Four** (the eight verb classes, with paradigm sentences) and
**Appendix Five** (the eight cases of declension, with paradigm sentences).

Those are the highest-value pages by a distance, and the reason is specific:
nothing in this tool can currently check whether `chunkClause` assigns the
*right* role. The alignment bank gives Tibetan↔English correspondence and
carries no syntactic labels at all. Preston's paradigm sentences are a
**role-labelled corpus** — ground truth for the one layer that has never been
validated. They would convert the role assignment from an unchecked rule into
something with a battery behind it, which is what happened to every other
engine in this repo and has not yet happened to this one.

Also wanted: the transitive/intransitive verb-form chart Preston cites at the
end of the *Great Tibetan-Chinese Dictionary*, which would let transitivity be
looked up rather than inferred from Wilson's class names.
