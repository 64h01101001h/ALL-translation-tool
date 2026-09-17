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

**Top 2,000 lemmas cover about 73–76% of running text. Top 4,000 cover about
82–86%.**

| top N lemmas | dictionary only | + Monlam |
|---|---|---|
| 500 | 54.4% | 52.4% |
| 1,000 | 65.7% | 62.8% |
| **2,000** | **76.4%** | **72.8%** |
| **4,000** | **85.5%** | **81.7%** |
| 8,000 | 92.7% | 89.3% |

| | dictionary only | + Monlam |
|---|---|---|
| lexicon forms | 99,588 | 549,419 |
| word tokens counted | 708,729 | 676,166 |
| tokens the lexicon did not know | 6,542 | 3,144 |
| distinct lemmas | 27,496 | 36,677 |
| lemmas for 50% coverage | 379 | 425 |
| lemmas for 95% coverage | 10,555 | 15,335 |

### Corrected 2026-09-17, a few hours after first publication

The first version of this page printed 73.2% and 82.7%, and those were wrong
— **understated by about three points**, for exactly the kind of reason this
whole item warns about.

The segmenter returns a token's surface text *including the separator that
followed it*, so `དང` and `དང་` came back as two different lemmas. They were
ranks 2 and 3 of the first list: the same word, its 21,707 uses split into
11,131 and 10,576, with the type count inflated by 5,625 phantom lemmas. The
tsheg is a separator, not part of the word.

It was visible on the first page of output and I published anyway. That is the
counting trap in `docs/LEARN_TAB_VISION.md` §2 and in the
`gmr-translation-style-findings` note, met for the third time in this project,
and the lesson holds: **read the top of the list before believing the bottom
of it.**

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

## Stage 4: spread, and why the total alone misleads

Adam's framing: *"A term used 400 times in one text is not the same learning
priority as one used 400 times across forty texts."* The list carries a
`sources` column — how many of the 75 sources a lemma appears in — and it
earns its place immediately.

Of the top 2,000 lemmas, **6 appear in three or fewer sources**:

| rank | lemma | uses | sources |
|---|---|---|---|
| 1044 | `ཁྱབ་པ་ཁས` | 110 | 3 |
| 1084 | `རྩ་བར་འདོད་ན` | 106 | 3 |
| 1543 | `འཇམ་དཔལ` | 72 | 2 |
| 1686 | `མཐའ་དཔྱོད་` | 65 | 3 |
| 1733 | `ཟིན་བྲིས` | 63 | 2 |
| 1907 | `སྨན་གྱི་བླ་` | 56 | 2 |

These look like vocabulary and are genre. `ཁྱབ་པ་ཁས` is debate-logic
apparatus, `ཟིན་བྲིས` is "notes", `སྨན་གྱི་བླ་` is the Medicine Buddha — each
frequent because one or two texts use it constantly, not because a reader
meets it everywhere. 72 of the top 2,000 appear in 60 or more sources, and
those are the ones a memorised core should be built from.

**The spread column is not decoration.** Ranking by total alone would put all
six of those in a beginner's first 2,000 words.

## Stage 5: the list

`FREQ_LIST_OUT=<path>` writes it: rank, lemma, count, cumulative share, and
the source count. 27,496 lemmas under the dictionary policy. It is ordered by
frequency with ties broken by lemma so that reruns are identical.

**It is not yet a learner's list**, and one thing is deliberately missing: the
English. Joining these lemmas to `hgm_gloss` is the obvious next step and it
is also where Rule 1 bites — the list may only ORDER entries that exist, and
must never manufacture an equivalent for a lemma the dictionary does not
gloss. A frequent lemma with no `hgm_gloss` has to show as unglossed, not as
something a machine filled in.

## The finding that matters most: 32 particles are a fifth of everything

Classifying the top 2,000 lemmas with our own `classifyParticle` — a closed,
documented list, not a guess — splits them sharply:

| | count | share of ALL running text |
|---|---|---|
| **particles** | **32** | **19.1%** |
| everything else | 1,968 | 57.2% |

Thirty-two items buy a fifth of every text we hold. The top ten by use are
`དང` (21,707), `ལ` (16,960), `ནི` (11,375), `ཀྱི` (9,117), `དེ` (7,188),
`གྱི` (7,182), `ན` (5,054), `དུ` (4,771), `ལས` (4,737), `ཞེས` (4,681).

**This is Adam's own pedagogy falling out of the measurement.** His hike
framing was that word identification comes first — *"when you can look at any
given paragraph… and you can tell the individual words, well, then you can
start to decipher connecting particles and reading order"*. These 32 are
precisely the boundary markers that let a reader do that, and they are the
cheapest thing on the list by a wide margin.

It also sharpens the caution below. A list ordered by raw frequency puts these
32 at the very top, which is right for identification and wrong for
vocabulary: knowing `དང` is not knowing a word in the sense a learner means.
The two should be taught as two things, and the app must not present one
ordering as though it served both purposes.

## Gloss coverage of the top 2,000

| | count |
|---|---|
| matched a master headword AND carry `hgm_gloss` | 1,799 |
| matched a headword but carry no gloss | 157 |
| no master headword at all | 44 |

The 44 are mostly grammatical: `པའི`, `བ`, `པར`, `བའི` are nominaliser and
case forms the segmenter matched as standalone tokens, plus `ཨོཾ`. They argue
for the same split as above rather than for new dictionary entries.

**Rule 1 governs what happens next.** The join may only ORDER entries that
exist. A frequent lemma the dictionary does not gloss shows as unglossed — 157
of the top 2,000 do — and nothing fills that in.

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
