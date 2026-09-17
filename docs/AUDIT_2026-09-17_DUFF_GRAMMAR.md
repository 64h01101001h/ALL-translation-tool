# Reading Tony Duff's *Standard Tibetan Grammar* against our engines

**What this is.** Adam owns Volumes I and II (Padma Karpo Translation
Committee) and asked that their entire contents be read and checked against
what our software actually does — *"fully scan the entire book… let's make
sure that we're not skipping over anything or missing anything."*

**Method, set by Adam:** *"if you write your findings in your own words, then
I need you to be 100% certain that you are accurate where you take liberty in
doing so."* So every claim about what Duff says carries a volume and printed
page, and rests on a verbatim quote that a separate reader re-opened the cited
page to confirm word for word. A finding whose quote could not be located on
its cited page was dropped rather than softened. Every claim about our code
was tested by running the engines, not by reading them.

**Coverage.** All 1,082 pages of both volumes, in 27 contiguous ranges, each
range read page by page. 893 agents; one verification task was lost to a
safeguard flag, and its finding was **recovered from the run journal and
verified by hand instead**, so coverage is complete. That finding was V1 p.759
on the generality term `gang`: the quote is on the page —

> "Initial to the expression of some name / The first having the second
> connected / Becomes the encompassing generality itself."

— and `isPronoun` in `wilsonparse.cpp` does contain `GANG`. The concern was
that Duff's two compound forms would be parsed as two tokens "unless the spine
happens to carry the compound as an entry". It does, for both: `gang zhig` is
glossed "a person / something / who…" and `gang dag` "who / that / those who",
with 535 and 323 corpus occurrences. They resolve lexically, which is the
right outcome and the same one reached for `shin tu` at V1 p.454. Downgraded
from PARTIAL to resolved.

**Outcome.** 1,242 findings. **177 were refuted by verification and
discarded.** 647 confirmed that our code already does the thing correctly.
377 were context — history, biography, doctrine — with no software bearing.
**41 confirmed gaps**, of which four were acted on and the rest are recorded
below with their dispositions.

**The Tibetan script does not survive extraction.** PKTC uses a legacy font,
so Tibetan appears as mangled Latin. No reader guessed at any of it; every
Tibetan-derived claim rests on Duff's own parenthetical transliteration, which
extracts cleanly. Where a point depended on script that could not be read, the
reader said so instead of inferring.

---

## Fixed

| | Finding | Commit |
|---|---|---|
| **V1 p.301** | The reversed gi-gu (U+0F80) for Sanskrit vocalic ṛ and ḷ was in the table and **unreachable**, because teaching the tokenizer to accept `-` as the prefix disambiguator consumed it first. `r-i` came back `རི` with the ordinary gi-gu, silently, `ok=true`. **My own regression**, from `5c3d77f8` the day before. | `4f21efd0` |
| **V1 p.306** | Sanskrit `cha` rendered as `ཚྷ` — tsha with a subjoined ha, which is not a letter. ACIP already spells tsha `TS`, so mapping `ch` to `TSH` sent it back as tsh plus a stray h. 1,267 reference rows. | `7b864956` |
| **V1 p.284/291** | `SUFF`, the ten native suffixes, was declared and never used — see *Measured and deliberately not done* below for why enforcing it would have been wrong. The record was corrected and both halves pinned by battery E2. | `829ee180` |
| **V1 p.79** | The da-drag rule: `kun` carries an invisible `da` and therefore takes `tu`, not `du`. We implement it correctly for tokenization; the corpus is internally inconsistent about it, and 23 segments were filed as errata. | in `255fed25`'s register |

---

## Measured, and deliberately NOT done

These are the ones worth reading. In each, Duff's rule is real, our omission
is real, and **implementing it with the evidence available would make the tool
worse.** Each was measured before the decision, not argued into.

### The ten suffixes should not be enforced by the converter — V1 p.291

Enforcing native name-formation rules in `wylie_to_unicode` newly refuses
**175 master headwords**, and they are `paṇ`, `tīk`, `utp la` — Sanskrit
written in Tibetan letters, where native rules do not apply, every one a
correct transliteration. Refusing them would be the defect.

The rule already lives where it belongs: `allcore::SyllableChecker` judges by
per-stack suffix *classes* rather than a flat ten, and correctly calls `kak`,
`kat`, `kap`, `kach`, `kazh` illegal while the converter renders them.

### `la` after a verb is a boundary continuative — V1 p.532

> "It is also one of a few linkers that are placed following a verb to show
> that the verb is at the end of one expression and that there is another
> expression leading on from it."

True, and `LA` is genuinely absent from `reader.cpp`'s `kConnectives`. But
that table closes a clause on **any** match, with no context test, and `la`
occurs **35,858 times** — nearly double `na`, the next most common.

Gating it on "preceded by a verb" does not work either. Of the 35,138 `la`
tokens, 4,284 (12.2%) follow a token the master records tense forms for — and
the three commonest are **homographs**: `dag`'s tenses belong to the verb
`'dag` "to purify" while `dag la` is the *plural marker*; `zhig`'s belong to
`'jig` "to destroy" while `zhig` is the *indefinite article*; `bo` is a
nominal suffix whose only gloss is ".". `verbEvidenceTok` already carries a
guard against exactly this, added after the same trap was hit three times.

**A false clause boundary is worse than a missing one** — it splits a sentence
that is not split. Not implemented.

### A complete sentence can have its verb at the front — V1 p.552

> "unlike most Tibetan sentences, the verb is at the beginning. If you were to
> follow the formula that is posted in some books, of always looking for the
> verb at the end of an expression, you would overlook the fact that this was
> a complete [sentence]"

`reader.cpp` searches only the final chunk, right-to-left, and says so in its
own comment. Searching every chunk would multiply false verbs against the same
homograph problem. The behaviour is a deliberate precision/recall trade; what
was missing is any statement of its limit, which is what this entry is.

---

## Open, and genuinely design questions

Not defects. Each would change what our grammar layer *models*, and that is
Adam's call rather than a battery's.

- **The letter-gender system** (V2 pp.20–21, 24–25, 37, 43, 74). Male /
  neutral / female / extremely female / barren, and Thumi's derivation of
  tense and transitivity from prefix gender. Entirely absent from the
  codebase. What we call verb "classes" is Wilson's eight syntactic classes
  keyed on subject case — a different axis, not a worse one.
- **"Particle" mistranslates `tshig.phrad`** (V1 pp.451–453). Duff argues the
  correct reading is "phrase linker". We say "particle" everywhere. A
  vocabulary change with wide UI reach.
- **The three categories of accessory** (V1 p.449): phrase linkers, phrase
  ornaments, phrase helpers. We group by paradigm row and Wilson case family
  instead.
- **Second readings our layer gives one value to**: the genitive also marks
  contradiction (V1 p.746); `'am` also carries question/doubt (V1 p.751);
  time nouns in a `la` slot are seventh case, not second (V1 p.505).
- **`kyin`/`gyin`/`gin` and `bzhin`**, the continuous present (V1 p.571) —
  absent from the particle layer.
- **Sanskrit vocalic ṛ never uses the reversed gi-gu in our Sanskrit path.**
  `iast_to_tibetan('kṛṣṇa')` goes through ACIP `KRish nA` to EWTS `kriSh Na`
  and renders `ཀྲིཥ་ཎ` with an ordinary gi-gu. Pre-existing and verified
  against the engine as it stood *before* `5c3d77f8`, so distinct from the
  regression fixed above.

---

## What the audit says about our own habits

Three of the four things fixed here were **silent** — `ok=true`, a plausible
Tibetan string, nothing flagged. None of 123 tests noticed, because the
batteries asked whether the engine produced something and not *what*. Battery
D (refusal), E (reachability) and E2 (converter renders / checker judges) exist
now because of that, and each was proved by reverting the fix it guards.
