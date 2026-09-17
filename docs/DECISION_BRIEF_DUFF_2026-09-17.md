# Five decisions that close 37 findings

The Duff scan read both volumes and confirmed 41 gaps. Four were defects and
are fixed. **The remaining 37 are not defects** — they are places where Duff
describes Tibetan one way and our software models it another. Nothing is
broken; something is unmodelled, and whether to model it is a product
decision, not a battery's.

They cluster tightly. Answering the five questions below resolves all 37.
Where I have a recommendation I give it and say why, but each of these is
Adam's call. Nothing has been implemented.

---

## 1. Do we model the Tibetan letter-gender system? — 9 findings

**What Duff says.** The whole of Volume II is *The Application of Gender
Signs*, and the gender of a letter is the native explanatory engine for a
great deal. V2 p.74 classifies all thirty consonants and the five prefixes as
male, neutral, female, extremely female or barren. From that classification
Thumi derives, on V2 pp.20–21, **which tense a word is in, whether it is
transitive, and whether the action falls on the self side or the other side**.
V2 p.589 says the choice between the phrase helpers `pa` and `ba` is made by
letter gender and is not free. V2 p.37 opens the verb chapter by saying verbs
cannot be presented natively without it.

**What we do.** Nothing of this. `core/include/allcore/verbclass.h` groups
verbs into Wilson's **eight syntactic classes** keyed on the case of the
subject — Linking, Existence, Motion, AgentiveNom, Necessity and so on. That
is a different axis, imported from a Western description, and it is not worse;
it is answering a different question. We implement one *consequence* of the
gender rule without the rule: `searchnorm.cpp` folds `ba`→`pa` for search,
which is right, but it is a fold, not a derivation.

**The decision.** Three real options.
- **Model it.** A new native layer: gender on every letter, and the
  derivations that follow. Large, and it would let the tool explain *why* a
  form is what it is in the terms a Tibetan grammarian uses.
- **Do not, and say so.** Keep Wilson's scheme, and record in the docs that
  the native framework is deliberately not modelled. Cheap, honest, and it
  stops the question being re-raised every audit.
- **Take one piece.** Implement `pa`/`ba` selection by gender (V2 p.589) as a
  derivation rather than a fold, and leave the rest.

*My recommendation:* the third, then reconsider. It is the one piece with a
clear right answer and a testable output, and building it would show what the
full layer would cost before committing to it.

---

## 2. Is "particle" the wrong word? — 1 finding, wide reach

**What Duff says.** V1 pp.451–453 argues that "particle" is a mistranslation.
The Tibetan is `tshig.gi.phrad`, "linkers of phrases", abbreviated `tshig.phrad`
then `phrad`, and Duff's case is that "particle" was borrowed from Western
grammar and does not mean what the Tibetan means.

**What we do.** We say "particle" everywhere — `allcore/particles.h`,
`ParticleInfo`, `classifyParticle`, `checkAgreement`'s failure text, and dozens
of user-facing strings in `app/walkthrough_pane.inc`.

**The decision.** Rename to "phrase linker" in the interface, or keep
"particle" because it is what every English-language Tibetan course the
students have taken already uses — including, and this matters, Geshe
Michael's own courses.

*My recommendation:* ask what Geshe Michael's courses say before changing
anything. If they say "particle", the students' vocabulary outranks Duff's
argument here, and the right move is a note in the Walkthrough rather than a
rename. This is exactly the kind of question where being right about the
Tibetan and wrong about the reader is a bad trade.

---

## 3. Do we adopt Duff's accessory taxonomy? — 7 findings

**What Duff says.** V1 p.449: accessories fall into exactly three categories —
phrase linkers (`tshig.phrad`), phrase ornaments (`tshig.rgyan`) and phrase
helpers (`tshig.grogs`). Several later findings hang off this: what a boundary
continuative can correspond to in English (p.471), the imperative never taking
an ending linker (V2 p.40), and the continuous-present linkers
`kyin`/`gyin`/`gin` and `bzhin` (p.571), which are absent from our particle
layer entirely.

**What we do.** `core/src/particles.cpp` groups by paradigm row and by Wilson
case family — Genitive, Agentive, Concessive, Terminative and so on. A flat
functional list, not a taxonomy.

**The decision.** Restructure to the three categories, or add them as a second
label alongside what we have, or leave it.

*My recommendation:* the middle one, and take `kyin`/`gyin`/`gin`/`bzhin`
first regardless of what is decided about the taxonomy — those are simply
missing, and the agreement rule for them parallels `kyi`/`gyi`/`gi`, which we
already implement.

---

## 4. Where Duff gives two readings and we give one, do we show both? — 5 findings

Each of these is a place where our layer commits to a single value:

| Duff | We say |
|---|---|
| The genitive also marks **contradiction** — "this is a Buddhist, **not** an outsider" (V1 p.746) | `GI/KYI/GYI/YI` = "genitive — of, connective", one case, `{6}` |
| `'am` also carries **question or doubt** — "permanent **or** impermanent?" (V1 p.751) | "alternative — or" |
| A time noun in a `la` slot is **seventh** case, not second (V1 p.505) | `narrowLaDon()` narrows to case 2 whenever the verb is Motion or NomAction |
| The **agentive** can mark comparison as well as `las` (V1 p.511) | `las` carries the comparative reading, the agentive rows do not |
| A boundary continuative spans conjunction, participle, comma and full stop (V1 p.471) | one function string per connective |

**The decision.** This is really one question asked five times: **when the
grammar admits two readings, does the pane commit to one or show both?**

*My recommendation:* show both, and this is the one where I would argue
hardest. Committing silently to one reading is the same shape as every defect
fixed this week — a confident answer where the honest output is "it could be
either". The pane already knows how to say a thing is unsettled; step 4 does
it for reading order in as many words. Extending that to case readings costs
nothing in trust and buys a translator the information they actually need.

---

## 5. Two smaller Sanskrit questions — 2 findings

- **The reversed gi-gu never appears in the Sanskrit path.**
  `iast_to_tibetan('kṛṣṇa')` goes IAST → ACIP `KRish nA` → EWTS `kriSh Na` and
  renders `ཀྲིཥ་ཎ` with an ordinary gi-gu, where V1 p.301 wants the reversed
  one for vocalic ṛ. Distinct from the regression fixed on the 17th, which was
  the `r-i` notation being unreachable; this path never used that notation.
- **Sanskrit numerals** (V1 p.312) are written with Tibetan digits, the two
  sets corresponding in form. Unmodelled; may not matter.

*My recommendation:* fix the first, file the second. The first is a real
wrong-character output in a path we ship.

---

## What is NOT on this list, and why

Three of Duff's rules were measured and deliberately **not** implemented,
because acting on them would make the tool worse. They are recorded with their
measurements in `docs/AUDIT_2026-09-17_DUFF_GRAMMAR.md` and should not be
re-raised without new evidence: enforcing the ten native suffixes (refuses 175
correct Sanskrit headwords), `la` as a boundary continuative (the verb signal
is contaminated by homographs — `dag`, `zhig`, `bo`), and searching every
chunk for a verb rather than the last (multiplies false verbs against the same
homographs).

One further item needs a reader rather than a decision: our `stva` cluster
disagrees with the Illuminator, and **zero of the 105,634 master entries touch
it**, so our own ground truth cannot settle it.
