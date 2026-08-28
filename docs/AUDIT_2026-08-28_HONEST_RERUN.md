# Adversarial mapping audit — the honest re-run

*2026-08-28. The first run of this audit was invalidated because I had
transcribed its inputs by hand and partly fabricated them
(`AUDIT_2026-08-28_CONTAMINATED.md`). This re-run generated the candidate
records from the live artifact by script, attached the verbatim source
segment to each, and had every agent read its own slice from the file. No
data passed through my hands.*

## Result

60 mappings from the highest-risk stratum, 10 of 10 slices read, no records
lost.

| Verdict | n | share |
|---|---|---|
| WRONG — no salvageable core | 1 | 1.7% |
| SUSPECT — right key, wrong span boundary | 10 | 16.7% |
| **any defect** | **11** | **18.3%** (Wilson CI 10.6–29.9%) |
| SOUND — survived refutation | 49 | 81.7% |

The contaminated run had claimed 37.5%. **The honest figure is roughly half
that, and the character of the defects is different**: almost none are wrong
*pairings*, they are correct pairings with over-wide English spans.

Three cautions the auditors stated and I am keeping: the 11 defects fall on
9 distinct keys so draws are not independent; each item carries one auditor's
verdict, so there is no inter-rater figure; and SOUND means "an auditor tried
to break it and failed", not "verified correct".

## The one WRONG — confirmed against the spine

**`'dod` → "make"** (C01:123). The segment reads
`mar 'dod nas chu bsrub pa dang 'dra ba yin` → *"It's like thrashing water
to make butter."* The mapping is `mar`→butter, `bsrub pa`→thrashing,
`chu`→water — and then "make" is left over. `'dod` means *want*; "to make"
comes from the English idiom that butter is *made* by churning. Geshe
Michael has rendered the purposive `'dod nas` ("wanting butter") as "to make
butter", which is good English, but keying the bare verb `'dod` to "make"
would teach a reader that `'dod` means *make*. It does not.

`'dod` carries eleven other renderings in this layer — desire, wish, crave,
hold, assert, teach, says, Hope, believes, the urge — and "make" is the lone
outlier. That distributional oddity is itself the strongest signal.

## The dominant mechanism: span over-capture

Eight of the eleven flags are one mechanism — the English span banked against
a short key extends onto material an *adjacent* term licenses. `chad` and
`rtag` swallow `mtha'` ("the extreme"); both `tshad` items swallow `skyes pa`;
`'gyur` → "bring" absorbs `rgyu` and flips the diathesis. The single WRONG is
the terminal case: once the neighbours take their share, nothing licenses the
residue.

## A proposed gate that I tested and rejected

The auditors proposed three mechanical signals. One looked excellent:
**unbalanced quotation marks in the English span** — 3 of 3 such items in
their sample were defective, and none of the 49 SOUND items contained a quote
character. They correctly flagged that the rule was chosen after seeing the
data and needed validating on a fresh draw.

**I validated it against the whole layer, and it fails.**

    pairs with an unbalanced quote : 110 of 7,820 (1.4%)
      span OPENS a quotation       :  64
      quote interior or trailing   :  46

Inspecting them: the overwhelming majority are correct. `bdag 'dzin` →
`grasping for a "self`, `bdag med pa` → `what we call "no-self`,
`dbu ma'i lta ba` → `the "view of the middle way` — these stop before the
closing quote **deliberately**, because Geshe Michael puts commas inside his
closing quotes (`"wisdom,"`), so a span that included the closing quote would
drag the comma with it. That trap has been navigated on purpose in every
batch since C01:336.

So the quote signal is not a defect indicator; it is a fingerprint of correct
comma-trap handling. Wiring it to auto-reject would have thrown out 110
mostly-good pairs. **Recorded as rejected, with the measurement, so nobody
re-proposes it.**

A narrow residue survives: a span consisting of a leading quote plus one or
two words, such as `'chag` → `"press`, surfaces to a reader as a token with a
stray quotation mark. That is a **display** defect even where the mapping is
right, and it is worth a lint — not a correctness gate.

## The data-model gap worth fixing

The auditors could not determine which token a key attached to when the
segment repeats that token — `'gro` occurs twice in `ngan 'gror 'gro`, three
times in C01:53; `'ong` three times. The record carries the key *string* and
the segment, but **no character offset or token index**, so the attachment is
not recoverable after the fact. This is a data-model gap rather than an
auditor failure, and it is the cheapest thing on the list to fix: the page
generator knows the offset at wrap time and simply does not persist it.

## What this does and does not establish

The 60 were drawn from the highest-risk stratum **by construction**. So
18.3% is an **upper bound** on the layer, not an estimate of it. Saying "the
layer is 18% defective" would be wrong, and wrong in the direction that
overstates the damage.

The honest statement to any downstream consumer is: *the highest-risk stratum
audits at 18.3% defective (10.6–29.9%); the layer-wide rate is unmeasured and
lower.*

**Next measurement, and the highest-value one available:** an unstratified
random sample of 60–100 pairs from the full layer. That would give an actual
estimate rather than a ceiling. Nothing in this document substitutes for it.
