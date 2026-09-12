# C05:229–231 / c5p77 — independent semantic review

**SPEC: PASS. QUALITY: APPROVE.** No open findings or requested span corrections.
Producer: Codex, independent adversarial semantic reviewer. This approves the
frozen machine alignment for the next pipeline stage; it is not human editorial
acceptance, dictionary promotion, or approval of every source translation.

| Seq | Spans | Nulls | Non-null d5 pairs | d7 | Generator | Exact body bytes |
|---|---:|---:|---:|---:|---:|---:|
| 229 | 48 | 2 | 38 | 3 | 0 | 8368 |
| 230 | 26 | 0 | 14 | 3 | 0 | 5100 |
| 231 | 33 | 0 | 22 | 2 | 0 | 6393 |
| Total | 107 | 2 | 74 | 8 | | 19861 |

All three saved bodies equal fresh complete output from the actual unmodified
generator. Each run has empty stderr. The generator's own resolver and
`with_members` produced all 107 ordered records in `resolved-spans.json`; each
has exactly seq,id,d,tib,eng,tib_range,eng_range. Every range was checked against
the reopened original source field. All eight members immediately follow their
d5 parent, are unique within that parent on both sides, and are strictly tighter.
`review.json` gives the individual final hashes and bytes, not aggregate-spec
hashes. `span-semantic-decisions.json` records a separate semantic disposition
for every span.

## Evidence and scope

Read the entire 163,143-byte semantic-input package and appended review brief,
the shared protocol, AGENTS, alignment specification, RECONCILE/REFUTE briefs,
course-shape notes, all final specs/reports/detailed decisions and omission
inventories. Compared both original proposals, all superseded specs, generation
attempts and failed verification/extraction reports. The historical wrong-of
choice is a real semantic range defect despite EXIT 0; the extraction failures
were regex/null-apparatus problems and not changes to the source. Replayed all
21 current/historical spec files, including the wrong-of and overcaptured
Buddhas versions, rather than accepting their self-reported status.

Read-only original-spine queries reopened C05:227–233, 104 exact glossary terms,
and 261 distinct full source witness rows. Replayed 117 unique saved SQL queries;
all 252 saved count assertions match. Those figures count this audit's workload,
not independent publications or exhaustive witnesses. Full rows, query strings,
parameters, result counts, raw fields and evidence provenance are preserved.

Independently opened the physical v27.2 master archive and compared every full
spine glossary raw record: 95/95 are identical to the corresponding master
entry. Independently opened the physical v32 parallel-corpus archive and checked
261/261 complete witness raw records against it. Their original full records and
archive hashes are in `physical-master-original-entries.json` and
`physical-corpus-original-witnesses.json`. Neither equality establishes a
publication genealogy. No Hopkins, LC or other comparative wording supplied a
GMR English equivalent. No glossary field or source text was modified.

`original-to-copy-manifest.json` maps 317 preserved input/governing versions to
exact-byte copies, with SHA-256 and length. Both proposal angles, every historical
attempt, root evidence and the input package are preserved under
`proposal-inputs/`. The final individual specs, bodies and all reconciliation
evidence are under `reviewed-final/<seq>/`. Python/SQL helpers retain their exact
bytes with an explicit `.txt` destination suffix. No Python cache was copied.

The closing audit detected the concurrent c5p76 registry addition in the builder.
Both exact builder versions are preserved and verified against immutable Git
commits: initial 11a388a709b8c1adb5a67c151e8076806bafee15 and final
9f9edc6b8c15a0828d5ea55658fe57bd45063567. Each manifest record names its commit
and repository-relative path. The generator/resolver and SPINE constant did not
change. A fresh final replay after that addition still reproduces all three
bodies exactly; `final-audit-proof.json` records it, while the initial audit's
detected-change record is retained as `attempt-1-final-audit-proof.json`.

## C05:229

The outline atoms are sound and distinguish the two object/process/establishment
sets. `bstan pa'o` is used once for “describe,” not also for the earlier supplied
“First we will show.” The first and second `rnams` nulls are genuine grammatical
fusion into “objects” at English 88:95 and 603:610, not unresolved alignments.
The true-grasp compound and Upali name member have the correct local ranges.
The Upali work title is supported by full original ILL:1104, with no invented
Sutra morpheme. Its absent exact HGM title gloss is not concealed.

The most tempting overcapture challenges survive scrutiny. `kha bye` owns the
attested complete flower-opening idiom “open their petals” (HGM 1063): the
opening/body aspect belongs to the flowers and the possessive is internal
coreference. No independent pronoun or universal petals gloss is asserted.
`chos thams cad` owns the attested all-phenomena unit “every single object in the
universe” (HGM 4747); the unrestricted scope licenses the whole unit, and its
`chos/object` member is literal. These are lexical attestations with local
semantic support, not arbitrary phrases justified solely by glossary presence.

The palace ornament remains unaligned. HGM 4925 actually includes “breathtaking
beauty” for `mchog`; HGM 25091 gives “pleasant” for `yid 'ong ba`. That discrepancy
was actively considered. In the current verse both the excellence of the palace
and its pleasing appearance are compressed into the English ornament. ILL:1104
separates “Perfect palaces” and “lovely light.” It does not license assigning the
whole ornament solely to `yid 'ong ba`, nor prove the other modifier absent.
Keeping Golden/palaces/blaze while omitting both modifiers is defensible.

The maker passage retains only `byed pa/maker`. HGM's noisy `'ga'/find`
alternative does not make an existential indefinite the lexical search verb in
this recast. No Look-for/you/him frame is banked. The byed-pa/byed-po variation in
ILL:1104 versus C14:129 and C18:871 establishes no spelling error in C05.

The five selected conception-family occurrences point respectively to the
outline, two verse lines, commentary explanation, and creation expression.
The final `rtog pa'i dbang gis bzhag par` at Tibetan 478 onward is deliberately
omitted as compressed repetition. It is not silently attached to the final
English conceptions. The rejected English-first p10 lacked this conceptual
predicate in its claimed English; broader depth would not cure that mismatch.
The final statements/other/as-well/many atoms retain their reordered positions.
All remaining omission runs were checked: case, quotation, copula and supplied
apparatus are not falsely presented as lexical equivalents or nulls.

## C05:230

The ignorance clause keeps correct semantic roles: the world has ignorance as
condition, and ignorance causes the world. The isolated rkyen/causes plus
can/null proposal is not required by an HGM list containing “causes”; the local
possession-of-condition relation is distributed in English. Omitting that
relation while preserving world and ignorance is honest. The superseded
`rdzogs pa'i sangs rgyas/Buddhas` overcaptured an unexpressed qualifier; final
sangs-rgyas/Buddhas is sound.

`gang phyir/For` is the causal introduction to Buddha speech in this occurrence;
the dictionary's alternative “Why?” does not override the source syntax. The
separately supplied Why? stays unwrapped. `de yi phyir na/then` and `cis/why`
correctly divide the consequent and interrogative functions. `mi 'thad/wrong`
contains the negation, `zhes/to say` supplies the quotation function, and the
second world/is/conceptions set resolves to the verse conclusion. The selected
gsungs/say is English 110:113, not title say at 30.

HGM 22903 directly attests `'jig rten rnams/many worlds`; its literal worlds
member is sound and rnams must not be nulled here. HGM 18277 directly attests
the complete `rang gi ngo bos/through some essence of its own` unit. The source
supplies rang reflexive ownership, gi possession, ngo bo essence and instrumental
final -s. “Some” is internal indefinite English within the attested unit; no
additional object or participant is imported. Its essence member excludes -s.
This is the actual essence relation at English 306:337, not the superseded
gi/of at “meaning of.”

`ma/none` is the negative exponent in the first negative subject construction,
and does not assert that ma names a person. `grub pa/does so` resumes the
existential predicate “exist”; its d3 placement appropriately records the
pro-verb recast. They remain correct when the whole clause is read. Final
rtog-pas/conception and btags-pa/products are supported lexical contributions.
The restrictive tsam construction and supplied “our” are omitted rather than
hidden inside the rejected whole products-of-our-conception phrase. These
omissions do not assert absent meaning.

## C05:231

`brgya pa/400 Verses` is the abbreviated title; C01:335 has the full bzhi-brgya-pa
title. There is no brgya/400 member or numerical erratum. `sogs/and such` is the
complete attested closing gesture (HGM 20919). Sogs itself licenses the initial
and; the single necessary head allowance is recorded in
`span-head-allow-needed.json` for `c5p77/s231w6`.

The verse existential negative is a complete phrase at d3. Removing conditional
na from the span avoids pretending that its entire consequent “Why then would”
is present in the claimed English. The positive commentary premise at Tibetan
197 is explicitly not translated as a separate positive proposition in C05.
EM:264 articulates it; ILL:1109 gives a different rendering of the positive and
negative conditions. Neither permits concealing it in C05's negative-only
English phrase. Final w15 resolves to the later negative occurrence at 239:254;
w16 keeps the whole negative predicate. No absent/uncertain-content null or
unsupported negative subword is introduced.

The first retrospective gsungs supplies title “says” and the second supplies
commentary “states”; both are reordered explicitly and correctly. With-a-brain
is an established HGM unit. The rope/coiling construction remains compositional
d3, supported by the actual C05 wording and EM:264's coiled-rope phrase; bsdogs
has no HGM gloss, and bsdogs-pa has no exact entry, neither of which is a
spelling-error proof. Label/snake/like preserve their real local functions while
the supplied “you” remains unwrapped.

The first essence parent is HGM 18277's exact “through any essence of their
own”; its internal words have the same instrumental/reflexive/possessive
licensors as 230, with any as negative-polarity indefinite realization. The
second occurrence “through its own essence” is the same established expression
in reordered English, with every source component present. Both essence members
are unique and strictly tighter. The existential negative, certainty modifier
and final affirmative exist are correctly isolated.

The final definition's complete subject and dependence relation are not
approved: Tibetan describes conception arising in dependence on that, whereas
the English introduces things occurring through conceptualization. The local
Conception/occur atoms are secure; they do not endorse that complete relation.
P7:9 stops at “process of,” and ILL:1109 does not translate these definitions.
C16:808 substitutes projection vocabulary. None supplies missing English or
proves publication history. The omission note correctly exposes this limit.

## Erratum disposition and final limits

The sole English-first punctuation candidate is **REFUTED**; bank-ready errata
is `[]`. Independently applied all five grounds, with complete details in
`independent-errata-refutation.json` and full original witness/query proof:

1. The exact found text holds in C05:230 English, not in ACIP or Wylie; this
   ground does not refute it.
2. C05:235 and C16:807/812 retain the period; ILL:1106/2675 and SVN:555/936
   use question marks in different translations. Independence is unproved.
3. No row-specific registered duplicate was found; the known converter,
   footnote, folio, heading, marker, spill and cap classes do not apply.
4. Interrogative meaning supports a question mark as a plausible editorial
   choice, but does not establish this literary period as an error. Under the
   required default, unresolved publication evidence refutes the claim.
5. Full witness fields and all saved counts were rechecked. Relevant query
   results are 2 same-punctuation, 9 related-question-verse, and 2 targeted
   C16/P1/TCS hits. These are not counts of independent publications.

No source/master/repository edit, commit, delegation, paid API, web, browser,
native app or device action was performed. Writes are confined to this review
directory; SQLite connections used URI mode=ro and Python bytecode was disabled.
All final PROVISIONAL labels remain. There are no outstanding corrections for
the frozen reviewed files.
