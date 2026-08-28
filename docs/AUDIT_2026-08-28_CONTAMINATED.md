# The adversarial mapping audit — and how I contaminated it

*2026-08-28. Ten agents adversarially audited 60 high-risk mappings from the
alignment layer, aimed at risk R10 (a wrong pairing passes every mechanical
gate). The report came back alarming. Most of it was my fault, not the
layer's. This document records what was real, what was an artifact, and the
methodology error that produced the difference — because the artifact looked
exactly like a catastrophic quality failure.*

## What the audit reported

9 WRONG, 15 SUSPECT, 30 SOUND of 54 verdicts returned. Headline: **37.5% of
evidence-backed adjudications carried a demonstrated defect.** Dominant
cause, 10 of 18: **"broken citation integrity" — the cited segment does not
contain the Tibetan key, or the English span, or either.** Second cause:
**two English spans that "exist nowhere in the corpus"**, which if true would
breach inviolable rule 1 (machine-MATCHED, never composed).

## My error

**I hand-typed the candidate list into the workflow arguments instead of
passing the triaged file I had just written.** Checking ten of the sixty
against the live layer:

- **4 of 10 citations were invented.** `tshad` cited C03:119; the layer says
  C01:260. `'bu` cited C01:400; really C01:177. `'byed` cited C01:380; really
  C02:24. `'bad` cited C03:129; really C01:147.
- **3 of 10 English strings were paraphrased or truncated by me.** The layer
  holds `chad` → "and thinking things have sto…"; I typed "the extreme that
  we call \"thinking things stop\"". Same for `rtag` and for `zin` (layer:
  "fill" / "filled with"; I typed "filled").
- Only 3 of 10 were faithful.

So the auditors were right about the material I gave them and wrong about the
layer. When they reported "the cited segment does not contain the key", they
had correctly checked a citation **I fabricated**. When they reported "this
English appears nowhere in the corpus", they had correctly searched for a
string **I paraphrased into existence**.

## The decisive check

Rather than argue, I ran citation integrity over the **entire live layer**:

    (key, ref) pairs checked                 : 9,459
    Tibetan key absent from cited wylie      : 0
    English span absent from cited english   : 7

All seven English "failures" are the multi-occurrence join format — where one
label is wrapped twice in a segment the builder stores `"directing … within"`,
which is not a contiguous substring by design. **There are zero real
citation-integrity defects.** The audit's dominant finding does not exist.

This is what the existing battery already guaranteed; the audit could not have
found otherwise, because `build_alignment_layer.py` refuses to write any pair
whose Tibetan is not letter-exact in the cited wylie and whose English is not
letter-exact in the cited English.

## What survived — two real defects

Where my input *was* faithful, two verdicts hold up and are genuine errors in
my alignment work:

**1. `'gro don` → "all living beings" (C01:482).** The segment reads
`'gro don 'phel` → "May it help all living beings!". `'gro don` is a unit
meaning *benefit of beings*; mapping the whole compound to "all living beings"
silently drops `don`, which is what "help" renders. The span is too small for
its key.

**2. `'phel` → "the higher good" (C01:496).** `'phel` is a verb (*increase,
flourish*). "The higher good" far more plausibly renders `don` in
`'gro don`. A noun gloss hung on a verb is a category error, and this is the
same colophon formula as #1 — the two defects are one bad analysis of
`'gro don 'phel`, split across two segments.

**Rejected: `chos brgyad` → "eight different features" (C03:135).** The
auditor argued 11 of 12 corpus instances render it "eight worldly thoughts",
so this must be wrong. But C03:135 reads `lus kyi kun spyod chos brgyad` —
the eight features of *bodily conduct* — where the others read `'jig rten
chos brgyad`, *worldly*. Different compounds sharing a bigram. The verdict
over-applies a frequency argument. **The auditor's underlying concern is
still valid**: banking bare `chos brgyad` creates a headword that collides
with the worldly-thoughts sense, and the key should carry more of the
compound.

## The lesson, which is the point of this document

An audit is only as good as the material it audits. I built a careful
adversarial harness, gave it fabricated inputs, and got back a confident,
well-argued, internally consistent report of a catastrophe that had not
happened. Every individual agent reasoned correctly. The aggregate was false.

This is risk R6 from the register — "the measurement manufactured the pass" —
running in reverse: **the measurement manufactured the failure.** It belongs
beside R10 rather than replacing it, because R10 remains true and untested:
this audit still has not told us the real mapping defect rate.

**Standing rule from here: audit inputs are generated from the artifact by
script, never transcribed by hand.** The triaged file existed at
`/tmp/verify_candidates.json` and I did not use it.

## What is still owed

- Re-run the adversarial audit passing the **actual** triaged records.
- Fix the two real defects in the `'gro don 'phel` colophon analysis.
- The re-run is the only thing that can give an honest mapping defect rate;
  nothing in this document does.
