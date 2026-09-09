# The ALL Sanskrit Pronunciation Standard

*Tibetan-tradition recitation of Sanskrit, as Geshe Michael Roach prints it.*
Version 0.1, 2026-09-09. Founding rulings by Adam, same date.

---

## 1. What this is, and what it is not

**It is an additional line.** Adam, 2026-09-09: *"This should be a separate
line/entry of the standards and isn't supposed to supplant or replace any
existing pronunciation schemes we've already lined out."* The classical IPA
line and the plain-letter simplified line are both unchanged and both still
shown.

**Its base is FPMT.** Adam: *"All other pronunciation standards founded by
FPMT will stand as a base for the ALL Sanskrit pronunciation standards while I
will then make a hybridized version by asking that we modify bits and pieces of
the FPMT standard to create an improved one."* So the standard has three
layers, applied in that order, each later layer winning:

| Layer | What it is | Authority |
|---|---|---|
| 1. FPMT base | The FPMT Translation Services letter values (November 2020), the same guide the app's letter-by-letter help already follows | External standard |
| 2. Matched from his courses | Readings taken from Geshe Michael Roach's own published English. Matched, never composed | His own published text |
| 3. Adam's amendments | Adam's rulings, each dated. May contradict either layer above; where they do, it is recorded | Adam, interim |

**It is not Classical Sanskrit.** IPA answers "how did this sound in
classical India". This answers "how do I say it aloud in this tradition".
Different questions, both worth answering, neither replacing the other.

**It is not the full Tibetan chanting convention.** The best-known marker of
that convention is *vajra* recited "benza". Measured against the whole corpus
on 2026-09-09: **"benza" appears in 0 of 42,199 segments; "vajra" appears in
513.** It is not in this standard because it is not in his text. If Adam wants
it, it becomes an amendment with his name on it.

---

## 2. Adam's founding rules

> **Rule 1 (2026-09-09).** `hūṁ` → **hung**
> **Rule 2 (2026-09-09).** `svāhā` → **soha**

Both are implemented. Both are whole-syllable rules, applied before any letter
rule, because a seed syllable is a word in its own right.

**What the corpus says about each — this is the part that matters.**

| | Adam's rule | In his published courses |
|---|---|---|
| Rule 1 | hūṁ → "hung" | **"hung" in 1,073 segments, "hum" in 37.** The rule is his own overwhelming practice. Word-aligned, `hu#m%` reads "hung" 476 times. |
| Rule 2 | svāhā → "soha" | **"soha" in 0 segments. "sva ha" in 380.** The rule appears nowhere in his published English. |

Rule 2 therefore changes what his own courses print. That is Adam's to decide
and the ruling is implemented as given — but it is recorded here, in the code
comment beside the rule, and in the workbench itself, rather than quietly
shipped.

**Provenance is owed.** `docs/standards/HGM_TRANSLATION_STANDARDS.md` binds:
*"Nothing here is invented: every entry must be sourced — who said it, when,
where"*, and *"we compile his guidance; we never compose it in his name."*
`docs/standards/SOURCE_TRUST_HIERARCHY.md` reserves its top tier for Geshe
Michael Roach's own live rulings, and its precedent for a ruling of Adam's is
recorded as *"Adam's interim ruling, awaiting his word."* These two rules are
labelled the same way until their source is recorded: **who said it, when, and
in what medium.**

---

## 3. The evidence base

The corpus contains its own parallel text for exactly this question. In
`data/full_parallel_corpus_v32.json.gz` (42,199 aligned segments), 821 lines
print a mantra as ACIP input code in braces followed immediately by Geshe
Michael Roach's own reading of it:

```
{om%^ hri#: s%t%ri#: vikr%ta#nana hu#m% phat%}  Om hrih shtrih vikirta-anana hung phet
```

681 of those align word for word, giving **346 distinct (code, reading) pairs
over 5,875 weighted occurrences**. Banked at
`data/pronunciation/gmr_mantra_evidence.csv`, regenerable, and used directly as
the acceptance battery.

### What the marks do, measured over the whole set

| Mark | Reading | Holds in |
|---|---|---|
| `s*`, `s#` (ś) | sh | 309/309 = 100% |
| `%` (retroflex ṭ ḍ ṇ ṣ) | the plain letter | 503/503 = 100% |
| `#` (long vowel ā ī ū) | the short letter, unmarked | 2,900/2,900 = 100% |
| `kr%` (ṛ) | ir | 65/65 = 100% |
| `cch` | ch | 205/205 = 100% |
| `:` (visarga ḥ) | h | 572/644 = 89% |

The visarga is the only mark he is not consistent about: `dharmāḥ` reads
"dharmah" 33 times and "dharma" 15; `hoḥ` reads "ho" 16 and "hoh" 7. The
standard follows the majority and writes the h. **Open question for Adam.**

---

## 4. The rule table

Applied as a **single left-to-right longest-match scan per word**, so no rule
can ever act on another rule's output. Whole-syllable idioms are tried first.

### Whole-syllable idioms (tried first, longest first)

| Input | Output | Basis |
|---|---|---|
| svāhā | soha | Adam, rule 2 |
| hūṁ / hūṃ | hung | Adam, rule 1 — and 476 attested |
| oṃ / oṁ | om | 718 attested |
| āḥ | ah | 185 attested |
| phaṭ | phet | 168 attested |
| hrīḥ | hrih | 71 attested |

### Letter rules

| Input | Output | Layer | Evidence |
|---|---|---|---|
| ā ī ū | a i u | FPMT + attested | 2,900 |
| ai au | ai au | FPMT | — |
| ṛ ṝ | ir / ri | **attested overrides FPMT** | FPMT's anchor is "cringe" (ri); he writes vikṛtānana as "vikirta-anana", 65/65 |
| ḷ ḹ | li | FPMT | — |
| ṅ | ng | FPMT | — |
| ñ | ny | FPMT | — |
| ñc ñch ñj | nch nch nj | attested | pañca → "pancha", 25 |
| ṭ ṭh ḍ ḍh ṇ | t th d dh n | FPMT | 503 |
| ś ṣ | sh | FPMT + attested | 309 |
| c, ch | ch | FPMT | — |
| cch | ch | attested | 205 |
| ṃ ṁ | m | FPMT | om, argham, padyam, pushtim |
| ṃk ṃkh ṃg ṃgh | nk nkh ng ngh | attested | vaśaṃkarama → "vashankarama", 47 |
| ḥ | h | attested | 572/644 |
| ' ’ ʼ (avagraha) | dropped | attested | ātmako'ham → "atmakoham" |

---

## 5. Worked examples

| IAST | ALL standard |
|---|---|
| oṃ maṇi padme hūṁ | om mani padme hung |
| oṃ tāre tuttāre ture svāhā | om tare tuttare ture soha |
| oṃ āḥ hūṁ | om ah hung |
| oṃ vajrasattva samaya hūṁ | om vajrasattva samaya hung |
| vikṛtānana | vikirtanana |
| praticcha | praticha |
| śuddhāḥ | shuddhah |
| pañcāmṛta | panchamirta |
| phaṭ | phet |

Entered as Tibetan script, ཨོཾ་མ་ཎི་པདྨེ་ཧཱུྃ gives **om ma ni padme hung**,
syllable by syllable, which is what a reciter reads.

---

## 6. How well it does

**94.4% weighted agreement with his own published readings** — 5,546 of 5,875
aligned mantra words. Held by battery I in `core/tests/engines_battery.cpp`,
with the floor set at 94% and marked ratchet-upward-only.

The Python oracle and the C++ port agree exactly: 79,452 of 79,452 Lokesh
Chandra terms, including the new column.

### The remaining 5.6%, every class of it

| He writes | We produce | n | What it is |
|---|---|---|---|
| rakmo, rak | ragmo, rag | 86 | g → k before a consonant. Possibly a Tibetan element inside a Sanskrit mantra. **Unruled.** |
| vikirta-anana | vikirtanana | 45 | His hyphen. Typography, not pronunciation. |
| shuddhohang | shuddhoham | 41 | Final -m of `-o'ham` read as -ng. But ātmako'ham reads "atmakoham" (7). **He does both. Unruled.** |
| ho, shuddha, ha, ja | hoh, shuddhah, hah, jah | 66 | The visarga optionally dropped. We follow the majority. **Unruled.** |
| mo | ma | 17 | A vowel difference in what may be a Tibetan element. **Unruled.** |
| jnana | jnyana | 5 | FPMT's own guide says jñ "may be pronounced jñ, gy, or gñ". Documented as variable. |

None of these are papered over. Each is a question for Adam, and each would be
a one-line amendment.

---

## 7. What this standard is silent on

- Whether "benza" for vajra belongs in it (0 corpus occurrences; his text says vajra).
- Whether the visarga is written or dropped at the end of a word.
- Whether `-o'ham` ends in -ng or -m.
- Stress, vowel length in delivery, and chant melody. It gives letters, not music.
- Sanskrit that is not mantra. The evidence base is mantra lines; ordinary
  Sanskrit prose is covered only by the FPMT base layer.

---

## 8. Where it lives

| Piece | File |
|---|---|
| Canonical engine | `engines/sanskrit_convert.py` — `FPMT_BASE`, `ALL_ATTESTED`, `ALL_RULINGS`, `ALL_IDIOMS`, `iast_to_all_pronunciation` |
| C++ port | `core/src/sanskrit.cpp` — `allTable()`, `allIdioms()`, `iastToAllPronunciation` |
| Declaration | `core/include/allcore/sanskrit.h` |
| Evidence | `data/pronunciation/gmr_mantra_evidence.csv` |
| Acceptance battery | `core/tests/engines_battery.cpp`, battery I |
| Port-parity fixture | `tools/build_sanskrit_reference.py`, column 10 |
| The workbench row | `app/main.cpp`, `SanskritPane` — To option `allpron` |

Adding an amendment is one line in `ALL_RULINGS` in the Python and one entry in
`allTable()` in the C++, plus its row here with a date and a source. The
battery will say immediately what the amendment did to the agreement figure.

---

## 9. Two gaps in the converter this work found

Running his own mantra lines back through `inputcode_to_iast` refused a fifth
of them. Two forms appear throughout his courses that the forward table never
emits, so they had never needed reading:

- the visarga written `:` — 644 occurrences
- ś written `s*` or `s#` — 327 occurrences

Both are now read, in the Python oracle and the C++ port together. Agreement
went from 80.1% to 92.6% on that change alone.
