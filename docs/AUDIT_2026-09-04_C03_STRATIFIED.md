<!-- Written 2026-09-04 from the on-disk verdict files of a 285-agent audit
     (100 first readers, 81 second readers, 57 + 9 skeptics). Every count
     below was recomputed from data/alignment/audit_verdicts_20260904.json,
     which is the primary record; this document is the reading of it. -->

# Correctness of the C03 alignment layer: the second measured rate

**Run:** 100 (headword, English rendering, citation) pairs, seed 20260904,
drawn from the C03 stratum of `alignment_evidence_v1.json` **headword-level**
(uniform over the headwords the layer cites in C03, then uniform over that
headword's C03 occurrences). Sample: `data/alignment/audit_sample_20260904.json`.
Verdicts, skeptic files and second-reader files, all 100 items:
`data/alignment/audit_verdicts_20260904.json`.

**Method (ADR-012, pre-registered in the 2026-09-04 gate addendum):** every
pair read once by an independent auditor (Claude Opus 5) with its layer
siblings and full source segment attached, under
`docs/alignment_briefs/AUDIT_BRIEF.md`. Every DEFECTIVE call went to three
skeptics (Claude Fable 5.1; lenses LICENCE, STYLE, EVIDENCE) instructed to
refute and to default to refuted. **New this run:** every SOUND call went to a
second reader (Fable 5.1) instructed to find the defect the first reader
missed; every CHALLENGE it raised went to the same three skeptics. A defect
counts only if at least two of three skeptics say it STANDS.

## 1. The headline

**19.0% confirmed intra-segment defect rate, Wilson 95% CI [12.5%, 27.8%].**
78 sound (concurred by a second reader), 19 confirmed defective, 3 refuted,
0 undecidable.

| stage | count |
|---|---|
| first-read SOUND | 81 |
| first-read DEFECTIVE | 19 |
| DEFECTIVE refuted by skeptic majority | 3 (items 12, 62, 65) |
| DEFECTIVE confirmed | 16 |
| second-reader CHALLENGE on a SOUND call | 3 (items 37, 39, 85) |
| CHALLENGE confirmed by skeptic majority | 3 |
| **confirmed defects, total** | **19** |
| of which unanimous (3/3 skeptics) | 11 |

Under a unanimity rule the rate is 11/100, CI [6.3%, 18.6%]. I report the
majority rule because it was the pre-registered method.

## 2. This is not the 4% of 2026-08-29, and the two numbers do not disagree

The first audit reported 4.0% [1.6%, 9.8%] on an **unstratified, triple-level**
draw across the whole layer with **no second reader**. Three things changed
here, and each moves the number in the direction it moved:

1. **Stratum.** C03 only. C03 is the course landed fastest (620 segments in
   two weeks under the batch discipline) and is the course whose rules the
   campaign was still tightening as it went (the article rule, the hinge
   rule, the negation ruling still open). The first audit's sample was
   dominated by C01, landed slowly and reviewed page by page.
2. **Unit.** Headword-level sampling weights every cited headword equally,
   so it over-represents rare, long, and awkward headwords relative to a
   triple-level draw, which is dominated by the frequent short words the
   layer gets right. Twelve of the 19 confirmed defects are on headwords
   with a single C03 citation.
3. **The second reader.** The first audit's §4 said its 4% was a floor
   because no SOUND call was ever challenged. This run challenged all 81 and
   found 3 more defects that survived three skeptics each. That is the
   measured size of the blind spot the first audit named: about 3.7% of
   SOUND calls (3/81) were wrong.

So the 4% is a layer-wide, triple-weighted floor; the 19% is a C03,
headword-weighted rate with both directions measured. Neither replaces the
other. A translator opening a C03 page and clicking on an arbitrary
*headword* has roughly a one-in-five chance that the span is mis-cut.
A translator reading C03 *text* meets far fewer, because the mis-cut spans
cluster on rare words.

## 3. Breakdown by kind

| kind | confirmed | what it looks like |
|---|---|---|
| OVER_CAPTURE | 13 | a supplied article, hinge, pronoun or GMR frame wrapped inside the span |
| UNDER_CAPTURE | 4 | the exponent of part of the headword sits outside the span |
| WRONG_MAPPING | 2 | positional, not lexical (15 `tshon`, 66 `mnga' ba`) |
| INVENTED | 0 | |

**Eight of the thirteen over-captures are one word wide**: "the listeners",
"The sea", "the mountains", "his student", "and supplication", "and sit
there", "like business", "our Lord Lama". Rule 7 of the spec forbids every
one of them. I first wrote here that this was "the rule not being applied
on the day"; a scan of the whole shipped layer, run while building the gate
for it, says otherwise. **A word-level English span beginning with a
supplied function word is the layer's habit, not its slip:**

| directory | d=5 English spans | begin with the/a/an/and/or/his/our/your/I/you |
|---|---|---|
| pages_c01 | 9,168 | 1,640 (17.9%) |
| pages (C02) | 680 | 49 (7.2%) |
| pages_c03 | 8,192 | 577 (7.0%), plus 36 at d=7 |

Some of those are licensed (a demonstrative `de`, a possessive `rang gi`, a
genitive), and this scan cannot tell which. But C01 at 18% means that when
C01 was landed the article was routinely banked inside the noun span
("the three principal paths", "the east", "the Noble Path"), and the
practice was only partly stopped by C03. The campaign log does show the
rule applied on the same C03 pages where it was broken (articles stripped at
C03:564, span nulled at C03:490), so C03 is inconsistent where C01 was
consistent in the other direction. The 2026-08-29 audit's 4% could not see
this because a triple-level draw is dominated by C01 spans and its single
auditor accepted the article as convention. The gate `no_supplied_span_head`
(added the same day) freezes these counts as a ratchet and forbids the
pattern in every new course directory unless the reconciler names the
licensor.

The three refutations were all "the auditor could not see the licensor":
item 12 (a misquoted sibling), item 62 (the infinitive "to" is the layer's
standing convention for verb spans, 85 instances), item 65 (`blang dor` is a
verbal compound and "observing" is its own exponent). That is the same
failure mode the first audit documented, at a lower rate (3/19 vs 5/9).

## 4. What this run adds to the method

- **The second reader is worth its cost.** 81 agents found 3 confirmed
  defects the first reader waved through, two of them unanimous. Without it
  this report would say 16%, and it would be wrong in the direction the
  first audit predicted.
- **Skeptics disagree with each other for reasons, and the reasons are
  informative.** In items 40 and 77 the STYLE lens refuted (a standing GMR
  epithet or idiom) while LICENCE and EVIDENCE stood. In items 5 and 66 the
  LICENCE lens refuted while STYLE and EVIDENCE stood. These are the cases
  where the spec has two rules pulling opposite ways (bank epithets whole vs
  no supplied pronoun inside a span) and the tie-break is Adam's, not the
  machine's. They are listed for ruling below.
- **The EVIDENCE lens never refuted alone.** All 22 evidence-lens skeptics
  found the auditor's quotations byte-exact. The first readers do not
  misquote; when they are wrong, they are wrong about licensing.

## 5. What this number does not cover

Everything in §4 of the 2026-08-29 report still applies: this is a rate of
intra-segment mis-pairing only. A row whose Tibetan and English belong to
different passages would pass every check here. The C13:63–82 offset remains
the standing example, and the mechanical gates for that class are closed
(ADR-012).

## 6. Items for Adam's ruling (spec conflicts, not machine errors)

- **40** `rje bla mas` → "our Lord Lama" (C03:337). All 25 corpus
  occurrences of "Lord Lama" carry "our". Bank the epithet whole (spec §3,
  standing epithets) or strip the pronoun (rule 7)? The STYLE skeptic says
  whole; two lenses say strip.
- **77** `de 'dra'i` → "that we are describing here" (C03:356). GMR's
  consistent idiom for anaphoric `de 'dra'i X` (TCS10:342, TCS15:58). Idiom
  or frame?
- **5** `dngos sam rjes mthun` (C03:152). The headword is cut across the
  disjunctive `sam`. The skeptics agree it is not a lexeme; whether to
  re-key it as two headwords or null it is a spec question.
- **15** `tshon` → "typical example" (C03:428). The token is prefix-dropped
  `mtshon te` (77 corpus hits vs 1). Re-key the headword to `mtshon`? This is
  also an input-variant flag for the spine (E-class to be assigned).

## 7. Repairs

Nineteen spans on fourteen C03 pages need their `eng` trimmed, widened, or
nulled. Behind them stands the layer-wide article habit in §3 (about 2,300
d=5 spans), which is a separate, larger repair that needs its own ruling:
strip mechanically where no demonstrative/possessive/genitive precedes the
noun in the wylie, or leave C01 as landed and label it. Not this audit's
call. The list with the exact edit for each is in
`data/alignment/audit_verdicts_20260904.json` (field `final ==
"CONFIRMED_DEFECT"`, with the skeptics' `argument` naming the fix). These
are page-spec edits that go through `tools/gen_alignment_page.py` and the
full gate set like any other landing; they are queued in TODO.md and are
**not** applied by this audit, which measures and does not repair.

## 8. Disposition for the Phase 3 gate

§5b of `docs/PHASE3_GATE_ALIGNMENT_V1.md` asked for this audit before C04
opens. It is done and banked. The number it produced is worse than the gate
document assumed when it was written, and the honest reading is that C04
should not be aligned under the C03 briefs unchanged. The one-line change
that would have prevented eight of the nineteen defects is a hard check in
the reconcile brief: **no span may begin with `the`, `a`, `an`, `and`, `or`,
`his`, `our`, `your`, `I`, `you` unless the reconciler names the Tibetan
word under it.** That is a brief edit and a gate, not a rule change, and it
is proposed as the precondition for opening C04.
