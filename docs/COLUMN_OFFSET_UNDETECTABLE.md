# Column offsets cannot be detected automatically — and what follows from that

*2026-08-28. Five approaches tried, all five failed against a known positive.
The negative result matters more than another gate would have.*

## The defect this was meant to catch

`C13:63–82` carries its `wylie` column **one row below** the English it belongs
to (`docs/upstream/C13_COLUMN_OFFSET.md`). Every segment reads as coherent
Tibetan beside coherent English; only someone who knows both texts sees they
are not the same text. **No verbatim gate can catch it** — the wylie is genuine
wylie, the English genuine English, both byte-exact against the spine.

It was found **by accident**: an alignment agent working on C03:299 happened to
cross-reference C13 and noticed. Nothing was looking for it.

That is the whole reason this detector was attempted.

## What was tried, and how each failed

| # | approach | result |
|---|---|---|
| 1 | Hand-rolled phonetic folding of wylie, matched against parentheticals in the English | Flagged **13 courses** on noise (folio citations, English asides) **and missed C13**. Wrong in both directions at once. |
| 2 | Filter anchors to plausible transliterations (reject English words, digits, Wylie hyphenation) | Noise dropped from 13 courses to 1 — **and the C13 signal went with it**. |
| 3 | Replace the hand-rolled fold with `pron_engine.py`, the project's own validated wylie→phonetics converter | Genuinely better: C13's offsets became visible (0 → 2 detected). But the verdict was course-level, and a 20-segment run inside a 1,041-segment course scores 5 aligned against 2 offset and trips nothing. |
| 4 | Detect **contiguous runs** rather than course totals — an offset is contiguous by nature | Correct in principle. But after filtering, only **34 usable anchors survive across all 42,199 segments**. Too sparse to see a 20-segment run anywhere. 0 runs found. |
| 5 | Abandon anchors for **length correlation** — wylie length against English length, present in *every* segment. An offset should drop the lag-0 correlation and raise lag ±1 | Failed on the known case. Over C13:63–82: **lag 0 = 0.088, lag −1 = 0.141**. The lag −1 correlation is indeed higher, in the right direction — but by 0.05, far inside noise, on a run of short title lines where length carries almost no information. |

Approach 3 is worth singling out: using the project's own engine instead of a
hand-rolled approximation was a real improvement and the right instinct.
`dga' ldan lha brgya ma` really does convert to "ganden hlagya ma", which is
exactly how Geshe Michael writes it. **The engine was not the problem. The
sparsity of the signal was.**

## Why it fails, structurally

An automated check needs a witness to the pairing that is **independent of the
columns being checked**. Only two candidates exist in this data:

- **Phonetic parentheticals** — genuinely independent, and genuinely rare.
  34 usable instances corpus-wide is roughly one per 1,200 segments. A defect
  run can sit anywhere between them.
- **Length** — universally present, but it carries almost no information about
  *which* text a row holds. It is a property of any Tibetan-English pair, not
  of this one.

There is no third signal in the corpus. Anything richer — does this English
describe this Tibetan? — is the semantic question, and five falsified gates in
`GATE_TRIALS.md` already establish that mechanical proxies for semantic
questions do not survive contact with scale.

## The consequence, which is the point

**The C13 offset was found by luck, and there may be others that no sweep will
find.**

Three things follow:

1. **Do not treat "no offset detected" as reassurance.** It has never been
   demonstrated for any course, including the ones already scanned.
2. **Reading is the detection method.** C13 was caught by an agent that knew
   what `shes rab snying po` means and noticed the facing English said
   something else. Every course this campaign reads at full depth gets that
   scrutiny as a by-product — which is an argument for finishing the eighteen
   courses that has nothing to do with the dictionary.
3. **The cheap check belongs upstream, at ingest, not downstream here.** At
   build time the corpus builder knows how many rows each column had. A row
   count that differs between the Tibetan and English sources for one text is
   trivially detectable *there* and impossible to reconstruct *here*. That is
   the recommendation in `docs/upstream/C13_COLUMN_OFFSET.md` and it stands as
   the only reliable answer.

`tools/detect_column_offset.py` is kept, marked non-working at the top, so the
next person does not spend the same afternoon rediscovering these five dead
ends.
