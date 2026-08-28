# Mechanical gates: what was proposed, tested, and rejected

*A running record. Every gate proposed for the alignment layer gets tested
against the whole layer before adoption, with a falsification criterion fixed
IN ADVANCE. Two have been proposed. Both were falsified. This file exists so
nobody re-proposes them on the strength of the sample that suggested them.*

The pattern both failures share: a signal that separates cleanly on 12–60
hand-inspected items saturates at 9,486. **A gate is a claim about the whole
layer and must be tested on the whole layer.**

---

## Gate 1 — unbalanced quotation marks · REJECTED

**Proposed by:** the stratified audit (2026-08-28). 3 of 3 flagged items were
defective; none of the 49 sound items contained a quote character.

**Criterion set in advance:** must hold at layer scale.

**Result:** fires on **110 of 7,820 pairs**. Inspection showed the majority
are correct — spans stop before a closing quote *deliberately*, because Geshe
Michael puts commas inside closing quotes (`"wisdom,"`) and a span including
the quote would drag the comma along.

**Verdict: the signal is a fingerprint of correct comma-trap handling, not of
error.** Adopting it would have discarded 110 mostly-good pairs.

**Salvage:** a span consisting of a leading quote plus one or two words
(`'chag` → `"press`) is a *display* defect even when the mapping is right.
Worth a lint. Not a correctness gate.

---

## Gate 2 — numeral leakage · REJECTED ON PRECISION

**Proposed by:** the uniform random-sample audit (2026-08-28), after 3 of its
12 defects were English cardinals with no Tibetan numeral in the key.

**Rule:** fire when the English span contains a cardinal ≥ "two" and the
Tibetan key contains no numeral morpheme (`gcig gnyis gsum bzhi lnga drug
bdun brgyad dgu bcu brgya stong khri 'bum phrag`).

**Criteria set in advance, before running:**
- falsified if it fires on more than ~400 triples
- falsified if precision < 50%

**Result:**

    fires        : 23 of 9,486 triples (0.24%)   <- PASSES the volume test, easily
    precision    : 10/23 = 43.5%                 <- FAILS the precision test

**Verdict: falsified.** It passed the criterion I expected it to fail and
failed the one I expected it to pass.

The 13 false positives are not random. They fall into four clean classes:

| class | n | example |
|---|---|---|
| the number is inside a **title** | 5 | `lam gtso` → "the Three Principal Paths" (short for *lam gtso rnam gsum*) |
| **lexicalised** term whose standard English name carries the count | 4 | `ngan song` → "the three lower realms"; `khams gong ma` → "the higher two" |
| number-meaning word **outside the morpheme list** | 3 | `zung` ("a pair"), `gnyi ga` ("both"), `skag` (the twelve-year-cycle obstacle year) |
| English idiom the key genuinely heads | 1 | `rang lag na yod` → "in our own two hands" |

**The 10 true positives were all real and are all now fixed** (see the commit
following this file). So the rule has value as a *triage queue* — 23 items to
read is cheap and it found 10 genuine defects. What it cannot be is an
auto-reject.

---

## The refinement Gate 2 points to — NOT YET VALIDATED

The auditors converged on one discriminator without being told it:

> the number belongs to a neighbour **only if that neighbour is actually
> present in this segment and already carries its own span.**

It separates the `lam gtso` cases perfectly. C01:73 and C01:53 spell out
`rnam gsum`, which has its own span — those were true positives. C01:209,
253, 351, 469, 484 have no `gsum` anywhere in the segment — those were the
title uses, all false positives.

**This refinement was derived from the 23 items it would be scored on.**
That is exactly the error that killed Gate 1: a rule chosen after seeing the
data describes that data and predicts nothing. It is a HYPOTHESIS. It does
not get adopted on this evidence, and its falsification criteria must be
fixed before it is next tested.

**But the refinement generalises past numerals**, and that is the part worth
pursuing:

> An English span over-captures whenever it contains material whose Tibetan
> licensor **is present in the same segment and already has its own span.**

That is not a numeral rule. It is a statement about span disjointness, and it
is checkable mechanically against the page structure for every span in the
layer, not just the 23 with numbers in them. It also explains the dominant
defect mechanism both audits found independently (span over-capture, 7/12 and
8/11), which is the first time a proposed gate has matched the measured
failure mode rather than a surface feature.

**Status: to be built and tested with its criteria fixed in advance.**

---

## Standing protocol for any future gate

1. State the rule and the falsification criteria **before** running it.
2. Run it against the whole layer. Report the fire count.
3. Hand-check a random draw of fires with **sibling spans attached** — an
   auditor that cannot see the other spans on the page will invent licensors.
   (Two false accusations in the random-sample audit came from exactly this.)
4. Report precision honestly against the pre-set threshold.
5. A gate that fails becomes a triage queue at best. Record it here either way.

---

## Gate 3 — self-referential over-capture · FALSIFIED ON VOLUME

**The rule** (stated in `tools/detect_overcapture.py` before it was run):

> span A on segment S has English E_A; span B on the same segment has Tibetan
> key K_B. If E_A contains a phrase the layer records ELSEWHERE as a rendering
> of K_B, then A has probably swallowed B's material.

The appeal: it needs no dictionary and no model. **The layer is its own
lexicon.** It improves as the layer grows, it can only fire on evidence the
project itself banked, and — unlike Gates 1 and 2 — it targets the mechanism
both audits independently measured as dominant (span over-capture: 7 of 12,
and 8 of 11) rather than a surface feature.

**Criteria set before the run:**
- falsified above 1,000 fires (the layer-wide defect estimate is ~1,423 pairs,
  of which over-capture is ~58%, so ~825; a detector aimed at one mechanism
  that fires more often than that mechanism is estimated to occur is finding
  something else)
- falsified below 50% precision

**Result: 2,322 fires — 24.5% of the layer. FALSIFIED on volume**, before
precision was ever measured.

**Why**, and it is worth stating because it is a fact about Tibetan rather
than about this code: **2,155 of the 2,322 fires are single-word phrases.**
Tibetan has many near-synonyms that Geshe Michael renders with the same
English word. `sdug` and `kun 'byung` both attract "suffering"; `ldog pa` and
`dran pa` both attract "back". A shared single English content word between
two Tibetan keys on one segment is the *expected* case, not a defect signal.

    stolen-phrase length : 1 word 2155 | 2 words 137 | 3 words 29 | 4 words 1

---

## Gate 3b — the same rule, restricted to multi-word evidence · UNDER TEST

**Rule:** Gate 3, but the shared phrase must be **at least two words** and
attested at **two or more other citations**.

**Provenance of each threshold, stated plainly:**
- The two-word floor has a justification independent of this data: single
  shared content words are expected between near-synonyms, as above. This is
  a linguistic argument, not a curve fit.
- The two-attestation floor is **data-derived** — I chose it after seeing the
  attestation histogram. It is the weaker half of this rule and should be the
  first thing re-examined if 3b survives.

**Fires: 52 of 9,486 (0.55%).** Volume is not at issue.

**Criterion set before the run: falsified below 50% precision** — the same
threshold Gate 2 was held to, so the two are comparable.

**Protocol, tightened over Gate 2's:** all 52 fires are checked with sibling
spans attached, and then **every claimed defect is handed to a separate
skeptic whose instruction is to refute it**, defaulting to refuted when
uncertain. Only survivors count toward precision. Gate 2's 43.5% was measured
without that second pass, so if 3b clears 50% it clears a strictly harder bar.

**Status: running.**

**Result: 52 fires, 1 confirmed defect. Precision 1.9%. FALSIFIED.**

Not merely below threshold — **below the base rate.** The layer's measured
any-defect rate is 15.0%, so a *random* draw of 52 pairs would be expected to
contain about 8 defects. Gate 3b found 1. It is worse than random: reading its
output is a worse use of a reviewer's time than reading arbitrary pairs.

The 51 false positives share one shape. The gate fires on generic nouns in
passages whose subject *is* that noun:

| segment | accused | "stolen" | supposed owner |
|---|---|---|---|
| C01:377 | `gdags gzhi` → "The basis to be given the name" | "the name" | `'dogs byed` |
| C01:377 | `'dogs byed` → "that applies the name" | "the name" | `gdags gzhi` |
| C01:349 | `gzung` → "and the object which it holds" | "the object" | `dgag bya` |

The first two are the **same segment accusing each other in both directions**
— a signature of pure string matching with no semantics. In a passage about
naming, every span contains "name", and both spans are correct.

**The one true positive is genuine and is fixed:** at C02:48 the root-text
incipit `sems bskyed pa ni gzhan don phyir` had `phyir` keyed to "for the
benefit of" while the compound `gzhan don` was reduced to "others". `don` *is*
the benefit; `phyir` is only the purposive postposition. The refuting agent
confirmed it against the Sanskrit — Abhisamayalamkara I.18, *cittotpadah
pararthaya*, where "benefit" is *artha* = `don` — and against the layer's own
settled convention at C02:50 and C02:51, where `gzhan don` carries the noun.
`phyir` now holds purposive senses only.

---

## Conclusion for the whole family — three gates, three failures

| gate | signal | fires | precision | verdict |
|---|---|---|---|---|
| 1 · unbalanced quote | surface punctuation | 110 | low | rejected |
| 2 · numeral leakage | surface lexical class | 23 | 43.5% | rejected |
| 3 · self-referential over-capture | layer's own lexicon | 2,322 | not reached | rejected on volume |
| 3b · same, multi-word only | layer's own lexicon | 52 | **1.9%** | rejected, below base rate |

**Stop building mechanical correctness gates for this layer.** The reason is
structural, not a matter of finding a better signal. What makes a mapping
wrong here is *semantic* — whether this English renders this Tibetan — and
every mechanically available signal is *surface*: punctuation, lexical class,
string co-occurrence. Surface signals cannot separate "B licenses this phrase"
from "A and B are both about the same topic." Gate 3b is the proof: given the
strongest self-referential evidence the project can construct, it performed
worse than chance.

This is consistent with **R10** in the risk register — the gates prove the
text is verbatim, never that the correspondence is correct — and it now has a
measurement behind it rather than only an argument.

### What does work, and what to spend effort on instead

**The model-read audit works.** Both audits found real defects at useful rates
(12 in 80 uniform, 10 in 23 triaged), and every confirmed defect in this
project has come from an agent reading the segment, not from a string rule.
Two design choices earned their keep and are now standing practice:

1. **Attach sibling spans.** An auditor that sees only `(key, rendering,
   segment)` invents licensors — it produced two false accusations before
   sibling spans were added, and used them correctly afterward to separate
   `lam gtso`-as-title from `lam gtso`-with-`gsum`-present.
2. **Run an adversarial refute pass.** Give every claimed defect to a separate
   skeptic instructed to refute it and to default to refuted when uncertain.
   Gate 3b's honest 1.9% exists *because* of that pass.

**Gate 2 survives as a triage queue, and only that.** 43.5% yield against a
15% base rate is a 2.9× enrichment — genuinely useful for ordering a review
backlog, useless as an auto-reject. Gate 3b earns not even that.

---

## Gate 4 — ordinal displacement in parallel lists · FALSIFIED ON PRECISION

**Proposed by:** the C03 adversarial audit (2026-08-28), from the one defect
it confirmed. Unlike gates 1–3 this targeted a **mechanism actually observed**
rather than a surface feature, which is why it was worth a run.

**Rule:** a span whose Tibetan key contains an ordinal morpheme (`dang po`,
`gnyis pa`, `gsum pa` …) should hold that ordinal's English word. If the
English ordinal is present in the segment but held by a *different* span, the
clause has been displaced — the exact signature of the C03:158 defect.

**Criteria fixed before the run:** falsified above 60 fires; falsified below
50% precision.

**Result: 2 fires. Volume passes easily. Precision 0 of 2. FALSIFIED.**

Both failures are instructive:

- **C03:153** — the "first" held elsewhere is a *temporal adverb* ("must first
  seek out"), not the ordinal. The rule cannot tell an English ordinal from
  its homonym.
- **C03:24** — the segment contains **two separate `dang po`**, and both spans
  are correct. The rule cannot tell one occurrence from another.

Four gates proposed, four falsified. The pattern across all four is now
unmistakable: **every mechanically available signal is a surface feature that
correlates with correctness only until it is measured at scale.**

### But the gate paid for itself anyway, and that is worth recording

Fire 1 was a false positive *on the gate's own terms* and still surfaced a
**real defect the audit had declared unmeasured**. `dang po'i gnyen por` ("as
the antidote to the first") had been keyed to *"Then they should acquaint
themselves with"* — English that actually renders `rig nas`, "having
recognised" — while its true rendering, *"The corrections for the first of
these obstacles"*, sat unwrapped in the last sentence of the segment.

That is a **wrong-pairing**, not an over-capture. Every audit so far has found
over-capture dominant, and the C03 audit explicitly flagged that mispairings
"remain effectively unmeasured". Here is one, found by a rule that was wrong
about why it fired.

**The lesson for future gates:** a falsified gate can still be a useful
*triage queue* if its fires are few enough to read by hand. Two fires cost
minutes and returned one real defect. That is a far better yield per minute
than the 15% base rate — but it is a claim about triage economics, never about
the rule's validity. **Do not let a lucky find rescue a falsified rule.**

---

## Gate 5 — semantic singleton against a well-attested key · FALSIFIED ON VOLUME

**Proposed by:** the first honest audit (2026-08-28), which named it as *the
only* signal that would have caught its one confirmed WRONG — `'dod` keyed to
"make", sitting among eleven renderings all meaning *want*. It was never built.
This built it.

**Rule:** for a key with ≥5 renderings, flag any rendering whose content words
are **disjoint from every other rendering of that key**. Unlike gates 1–4 this
is not a surface feature of one pair; it is a claim about a pair's relationship
to its siblings — which is the shape both confirmed WRONGs actually had
(`'dod`→"make" among want-words, `gnas`→"really" among place-words).

**Criteria fixed before the run:** falsified above 150 fires (1.8% of the
population); falsified below 50% precision.

**Result: 975 fires. FALSIFIED on volume, by more than six times.**

    keys with >=5 renderings          316
    renderings they hold            2,556
    disjoint from every sibling       975  (38.1%)

**Why it fails: the test conflates "different word" with "different meaning."**
It cannot see synonymy. A sample of its fires:

| key | flagged rendering | its siblings |
|---|---|---|
| `dgos` | "necessary" | must, have to, has to |
| `go ba` | "realize" | understand, understanding, idea |
| `med` | "Neither" | no, never, non-existence |
| `bka'` | "the words" | instruction, instructions, word |
| `zin pa` | "steeped" | filled, bound, under the influence |

Every one is *correct*. They share no stem with their siblings because English
has many words for one sense, not because the mapping is wrong.

### The symmetry with Gate 3 is the real lesson

Both were lexical-overlap tests, and they failed as exact mirror images:

- **Gate 3** fired because **different Tibetan keys share the same English** —
  Tibetan near-synonymy (`sdug` and `kun 'byung` both attract "suffering").
- **Gate 5** fires because **the same Tibetan key takes different English** —
  English synonymy (`go ba` is both "realize" and "understand").

Meaning is not words. A test built on word overlap is defeated from both
directions at once, and no amount of threshold tuning reaches across that gap.
**Five gates proposed, five falsified.** The mechanical-gate programme is
closed on the evidence, not on fatigue.

### What the 38.1% is worth on its own

It is a **measurement of Geshe Michael's range**, and the strongest quantitative
support the preserve-all-renderings ruling has:

> Across his most-rendered terms, **38.1% of renderings share no content word
> with any other rendering of the same term.**

Not a majority — but far too large a minority to be noise. A dictionary that
collapsed each headword to a single equivalent would not merely lose nuance;
for well over a third of these renderings it would discard English that has
**no lexical overlap at all** with what it kept. `go ba` would become
"understand" and lose "realize"; `bka'` would become "instruction" and lose
"the words". Those are not shades of one gloss. They are different words a
translator would actually reach for.
