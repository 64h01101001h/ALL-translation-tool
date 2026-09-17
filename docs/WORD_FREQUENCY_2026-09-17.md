# Word frequency, and the policy it is counted under

Adam, on a hike, 2026-09-11: word identification is *"the first step for
people trying to learn how to translate Tibetan"* — before particles, before
chunk order, before anything the Learn tab already does. This is stage 2 of
that item, and stage 2 had a lock on it.

## The lock, and why it held

`docs/LEARN_TAB_VISION.md` records that two independent passes over this same
database, using two defensible tokenizations, produced coverage figures **about
2.5× apart** — 199 against 380 items for 50% coverage, 3,266 against 9,101 for
95% — and rules:

> No frequency-coverage figure may be printed as fact until a disambiguating
> segmenter with a documented tie-breaking policy exists and is battery-proven.

The dictionary lattice cannot be that segmenter. By design it keeps nested
matches and makes no segmentation decision at all.

## The policy

All three parts are stated because all three move the number.

| | choice | why it is defensible |
|---|---|---|
| **segmenter** | `SegTrie`'s maximal-match walk | Tie-break is longest-match. `botok_tok_smoke` proves `SegTrie::segment == Segmenter::segment` word for word at corpus scale, so the ported botok path is the anchor. |
| **lexicon** | named per run, both measured | It decides how long a maximal match can get. Not assumed — run both and compare. |
| **unit** | **lemma**, not surface form | A learner memorises a word, not its affixed shapes, and **21% of corpus tokens carry an affix** (148,703 of 708,729). `SegWord` gives `affixType` and `affixAa`, so the lemma is recovered exactly rather than guessed. |

## The answer to Adam's question

**Top 2,000 lemmas cover about 70–73% of running text. Top 4,000 cover about
79–83%.**

| top N lemmas | dictionary only | + Monlam |
|---|---|---|
| 500 | 51.0% | 49.1% |
| 1,000 | 62.4% | 59.5% |
| **2,000** | **73.2%** | **69.8%** |
| **4,000** | **82.7%** | **79.0%** |
| 8,000 | 90.5% | 87.1% |

| | dictionary only | + Monlam |
|---|---|---|
| lexicon forms | 99,588 | 549,419 |
| word tokens counted | 708,729 | 676,166 |
| tokens the lexicon did not know | 6,542 | 3,144 |
| distinct lemmas | 33,121 | 42,681 |
| lemmas for 50% coverage | 471 | 533 |
| lemmas for 95% coverage | 13,365 | 18,914 |

**The lexicon choice moves the answer by 3–4 points, not by 2.5×.** That is the
useful result: at Adam's numbers the figure is robust to the one policy choice
that was feared, so the list can be built without settling which lexicon is
"right" first.

## What these figures do NOT cover, and it belongs beside every one of them

- **6,259 of the 42,199 corpus segments (14.8%) do not render to Tibetan at
  all** and are absent from the input. A flagged rendering is not Tibetan to
  count. This is the largest single caveat and it is not small.
- **Tokens the lexicon does not know are reported, never folded in** — 6,542
  and 3,144 above. They are counted as unmatched and excluded from frequency.
- **Lemma recovery strips the affix syllable** the segmenter identified,
  restoring a dropped `འ` where `affixAa` says one was lost. A single-syllable
  token carrying an affix is left as it stands rather than reduced to nothing.
- **This is the ALIGNED CORPUS, which is a subset.** Stage 1 of the TODO item
  asks what "all of Geshe Michael Roach's known translations" actually means,
  and that is **not answered**. No figure here describes the whole body of that
  work, only the 42,199 aligned segments we hold.
- **The old 2.5× is not explained, only bypassed.** These numbers are larger in
  absolute terms than the ones on record and were produced by a different
  method, so nothing here reproduces or refutes that pair. What is established
  is that *under a stated policy* the figures are these, and that the lexicon
  is not the 2.5× lever.

## What must not be read into it

**The list is a recommendation, not a syllabus.** It says what is common.
Common is not the same as worth learning first, and the app must never present
the two as identical — a high-frequency particle and a high-frequency doctrinal
term are different learning problems, which is what stage 3's per-depth profile
is for.

**`hgm_gloss` stays binding.** A frequency list orders existing entries. It
does not create an equivalent and it does not promote one.

## Reproducing it

    build/core/word_frequency data/botok build/hgm_spine_v27_2.db <corpus.txt> dict
    build/core/word_frequency data/botok build/hgm_spine_v27_2.db <corpus.txt> \
        dict+monlam data/extracted/monlam_lexicon_1.txt data/extracted/monlam_lexicon_2.txt

`<corpus.txt>` is one Tibetan segment per line, produced by running the
spine's `wylie` column through `engines/ewts_unicode.py` and keeping only the
renderings that do not flag.
