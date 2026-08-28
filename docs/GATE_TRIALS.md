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
