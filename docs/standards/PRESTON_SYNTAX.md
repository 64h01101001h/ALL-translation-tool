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
| complement to the object | 2nd |
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
