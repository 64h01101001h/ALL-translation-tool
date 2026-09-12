# C05:169–171 independent semantic and errata review

**APPROVED for provisional landing: 169, 170 and 171. No required spec changes and no function-word allowlist additions.** The single erratum survives as **refuted=false, LOW/PROBABLE**. This is a machine review, not human approval or proof from generator acceptance.

All 87 spans were reviewed: 50 d5 dictionary links, 6 d7 members, 8 d3 phrases and 23 d6 particles; all 13 null claims were examined. SQLite was opened with `mode=ro`. Reconciled specs and original sources were not edited. Independent generator reruns used `PYTHONDONTWRITEBYTECODE=1`, exited 0, and reproduced the prior outputs exactly (169: 5,155 bytes; 170: 6,288; 171: 6,131).

## Decisions requiring semantic judgment

- **169:** The four cases are maturation definite/time indefinite; time definite/maturation indefinite; both definite; neither definite. `gnyis ka ma nges pa → definite in neither respect` preserves the whole negative quantifier at d3. The second Tibetan `rnam smin` resolves to the third English `ripen`, in `not definite to ripen at all`; the extra `should ripen` belongs to GMR’s timing expansion. All six English `definite` occurrences, the two `time` occurrences and both `but` relations resolve to their correct cases.
- **170:** `mthong chos la myong 'gyur gyi las` is the complete experienced-in-this-life category at d3. `ris mthun mi 'phen → do not` is the repeated predicate represented by an English pro-verb. `gnyis pa → Next` is a real ordinal correspondence, expressly supported by HTG2016; the longer outline is supplied. `'phan → projects` remains valid as recorded by HTG2016. The rejected spelling erratum is not revived.
- **171:** `skyes myod` is the verse’s compressed next-life category, explained by `skyes nas myong 'gyur` at 173. The individual `born` and `experience` links remain contextual source attestations. `gzhan du'ang → in others even` refers to later lives, as 174 shows, not to other persons or other realms. The first `Neither` represents the repeated negative experience/deed predicate; the second belongs to `yang min`. Neither is banked as a false standalone word. `'dod` and `rtse` name the desire realm and peak level, confirmed by 175.

## Function-word licensors

No d5/d7 span starts with a prohibited article/pronoun/hinge. Beyond that mechanical fact, the apparent function words have actual licensors: 171 `of` is bound `'i` in `dge ba'i`; `At` is the final locative `r` in `der`; `that` is `de`; `un` is negative `mi`. 170 `may ask` is the fixed `zhe na` hinge and excludes supplied `One`; `very same` is emphatic `de nyid` and excludes `the`. 169 `Others` is `gzhan` with plural `dag` absorbed in the word, and `both` is `gnyis ka`. Particle-level `that/but` at 169 belong to `zhes/la`; `to/but/upon/because` at 170 belong to `par/kyi/la/phyir`. No allowlisting is needed.

## Erratum: C05:171 `skyes myod mi byed` → `skyes myong mi byed`

**refuted=false; retain LOW/PROBABLE.** All five checks were independently performed:

1. The exact found text occurs in Wylie, and ACIP reads `SKYES MYOD MI BYED`. This is not an ACIP-to-Wylie converter discrepancy.
2. No independent publication of this verse was found in C16, P1 or TCS. C16:758 starts `nyon mongs rab dang drag po dang` / `Those are definite which involve fierce` and corresponds to C05:179, not 171. TCS10:338 is the separate Lama-practice/death-as-path passage: `yang myod yul bde ba de gas chog cing, myong yul bde ba zhes pa`, with English `the object that you experience`. Its two spellings disprove uniqueness and do not establish the reading of this verse. C16:756 and 757 reuse 169 and 170; 757 changes only the appended heading, from `55 Deeds Projected for Different Realms and Beings` to `19 Deeds that Definitely Ripen`.
3. No `C05:169–171`, `myod` or `skyes myong` locus record occurs in the current JSON register, generated register, backlog or TODO. The brace-converter, footnote, folio-caption, following-heading, row-spill and 1,500-character-cap classes do not explain MYOD.
4. No exact or partial `myod` dictionary entry or variant was found. `myong` entry 14665 has HGM `experience` and tenses `myong / myong / myangs/ myongs`. The same verse has `myong mi byed`; C05:173 has `skyes nas myong 'gyur gyi las mi byed` and `does no deeds of the type that will lead to an experience there after being reborn`. Dictionary absence alone would be insufficient; these contextual supports make the restoration probable.
5. Counts recheck: `myod` has 2 rows (C05:171, TCS10:338); `byis pa der skyes`, `myod mi byed`, `myong mi byed`, and `A stable child does none` each have 1 row (C05:171); exact `myod` headwords and variants each have 0. All claimed counts and quoted source fragments hold. Broad substring searches also found unrelated `bzhin` substrings in ILL:2169/P3:125; direct inspection excluded them.

No blockprint was consulted, so the candidate must not be upgraded to CONFIRMED. The exact-source spelling remains unchanged in the alignment.

## Complete span audit

Ranges below are zero-based half-open character offsets into the original source fields. `∅` means no separately spanned English exponent. Each row was judged semantically; SOUND is not inferred from the generator.

### C05:169 — APPROVED_PROVISIONAL

| Span | d | Tibetan → English | Source ranges (T / E) | Judgment and evidence |
|---|---:|---|---|---|
| s169w1 | 5 | `dpe ston pa` → `Exemplist` | [0, 11] / [20, 29] | SOUND. The named Exemplist subgroup; HTG2016 dpe ston pa includes exemplist. No supplied article captured. |
| s169w2 | 5 | `mdo sde pa` → `Sutrist` | [12, 22] / [43, 50] | SOUND. The parent Sutrist school; the complete school name mdo sde pa is one compound. The extra English school stays free. |
| s169w3 | 5 | `sde pa` → `group` | [25, 31] / [30, 35] | SOUND. The independent sde pa after the school name owns group, not the earlier syllables inside mdo sde pa. HTG2016 supports group. |
| s169w4 | 5 | `gzhan` → `Others` | [32, 37] / [0, 6] | SOUND. gzhan supplies Others; dag supplies plurality within the same English word and has no separate word to claim. |
| s169p1 | 6 | `dag` → `∅` | [38, 41] / None | SOUND. Valid morphological null: plural dag is realized within Others, without a separate English span. This does not mean plurality is absent. |
| s169w5 | 5 | `mu` → `combinations` | [42, 44] / [83, 95] | SOUND. mu denotes the logical alternatives/combinations, the HTG2016 gloss. The surrounding possible/which hold true/in this regard frame stays free. |
| s169p2 | 6 | `ni` → `∅` | [45, 47] / None | SOUND. Valid topic null; the English predication does not supply a separate topic-marker word. |
| s169w6 | 5 | `bzhi` → `four` | [48, 52] / [131, 135] | SOUND. Four enumerated cases directly confirm bzhi. The unrelated auto-aligned dictionary gloss for bzhi is not used as evidence. |
| s169p3 | 6 | `zhes` → `that` | [53, 57] / [65, 69] | SOUND. zhes marks the quoted content of zer; that is the complementizer introducing precisely that claim. |
| s169w7 | 5 | `zer` → `claim` | [58, 61] / [59, 64] | SOUND. zer is claim in the initial sentence; the later They say is a resumptive English expansion, not a second zer occurrence. |
| s169p4 | 6 | `te` → `∅` | [62, 64] / None | SOUND. te continues into the enumeration; sentence restart and They say there are supply no separate connective equivalent. |
| s169w8 | 5 | `rnam smin` → `ripen` | [66, 75] / [188, 193] | SOUND. First rnam smin owns ripen in case 1 (definite to ripen), supported by HTG2016 ripen/ripening. |
| s169w9 | 5 | `nges pa` → `definite` | [76, 83] / [176, 184] | SOUND. First nges pa owns first definite, asserting certainty of maturation in case 1. |
| s169p5 | 6 | `la` → `but` | [84, 86] / [194, 197] | SOUND. First la contrasts certainty of maturation with uncertainty of time, licensing the first but. |
| s169w10 | 5 | `gnas skabs` → `time` | [87, 97] / [213, 217] | SOUND. First gnas skabs owns time in case 1, after no definite; this is the temporal dimension. |
| s169p6 | 6 | `ma` → `no` | [98, 100] / [201, 203] | SOUND. First ma owns no in no definite time, expressing negation of certainty without manufacturing neither. |
| s169w11 | 5 | `nges pa` → `definite` | [101, 108] / [204, 212] | SOUND. Second nges pa owns second definite in no definite time; it remains positive while p6 supplies negation. |
| s169w12 | 5 | `gnas skabs` → `time` | [110, 120] / [262, 266] | SOUND. Second gnas skabs owns time in case 2 (at a definite time), not the case-1 occurrence. |
| s169w13 | 5 | `nges` → `definite` | [121, 125] / [253, 261] | SOUND. Bare nges owns third definite, the positive timing half of case 2. |
| s169p7 | 6 | `la` → `but` | [126, 128] / [267, 270] | SOUND. Second la licenses the second but, contrasting definite occasion with uncertain maturation. |
| s169w14 | 5 | `rnam smin` → `ripen` | [129, 138] / [297, 302] | SOUND. Second rnam smin owns the THIRD English ripen, in not definite to ripen at all. The second English ripen, in should ripen at a definite time, expands gnas skabs nges and is correctly unwrapped. |
| s169p8 | 6 | `ma` → `not` | [139, 141] / [281, 284] | SOUND. Second ma owns not in case 2, preserving uncertain maturation rather than negating time. |
| s169w15 | 5 | `nges pa` → `definite` | [142, 149] / [285, 293] | SOUND. Third nges pa owns fourth definite, negated by p8 in case 2. |
| s169w16 | 5 | `gnyis ka` → `both` | [151, 159] / [343, 347] | SOUND. First gnyis ka owns both in case 3; the corresponding two dimensions are maturation and timing. |
| s169w17 | 5 | `nges pa` → `definite` | [160, 167] / [331, 339] | SOUND. Fourth nges pa owns fifth definite in case 3, ahead of both in English order. |
| s169f1 | 3 | `gnyis ka ma nges pa` → `definite in neither respect` | [169, 188] / [382, 409] | SOUND. Whole negative-quantifier phrase means certainty in neither of the two respects in this four-case context. Definite in neither respect is the entire sixth-definite phrase; splitting neither onto gnyis ka or ma alone would lose its quantifier scope. d3 is appropriate. |

Unwrapped Tibetan with letters: `'i `; `'o`. These are genitive/final suffixes; no content word was falsely omitted. Full uncovered English and Tibetan ranges are preserved in `review.json`.

### C05:170 — APPROVED_PROVISIONAL

| Span | d | Tibetan → English | Source ranges (T / E) | Judgment and evidence |
|---|---:|---|---|---|
| s170p1 | 6 | `'o na` → `∅` | [0, 5] / None | SOUND. Opening discourse hinge is implicit in the English question; there is no separate well then/in that case exponent. |
| s170w1 | 5 | `las` → `deeds` | [7, 10] / [27, 32] | SOUND. First las owns deeds in which of these deeds; additional outline deeds are English expansion. |
| s170w2 | 5 | `gang` → `which` | [11, 15] / [12, 17] | SOUND. gang supplies the interrogative which. of these is English framing and remains free. |
| s170p2 | 6 | `gis` → `∅` | [16, 19] / None | SOUND. gis marks deeds as the projecting agent; English subject position supplies no separate case word. |
| s170w3 | 5 | `ris mthun` → `discrete being` | [20, 29] / [44, 58] | SOUND. First ris mthun owns first discrete being; exact HTG2016 glossary support, without article a or in the future. |
| s170w4 | 5 | `'phan` → `projects` | [30, 35] / [33, 41] | SOUND. Written form is exactly phan with initial apostrophe; HTG2016 explicitly glosses it projects. Retain the source spelling and do not revive the dropped orthographic claim. |
| s170w5 | 5 | `zhe na` → `may ask` | [36, 42] / [4, 11] | SOUND. Fixed zhe na question hinge owns may ask under C05 convention; One is supplied. It is not justified by the unrelated auto-aligned dictionary gloss. |
| s170w6 | 5 | `gsum` → `Three` | [44, 48] / [74, 79] | SOUND. gsum supplies Three; of them remains free. |
| s170p3 | 6 | `gyis` → `∅` | [49, 53] / None | SOUND. gyis marks the three as agents; no separate English case exponent. |
| s170w7 | 5 | `ris mthun` → `discrete being` | [54, 63] / [105, 119] | SOUND. Second ris mthun owns the second discrete being in the affirmative answer. |
| s170p4 | 6 | `ni` → `∅` | [64, 66] / None | SOUND. Topic ni has no separate English exponent. |
| s170w8 | 5 | `'phen` → `project` | [67, 72] / [95, 102] | SOUND. phen with initial apostrophe owns project in act to project; HTG2016 supports this verb. |
| s170p5 | 6 | `par` → `to` | [73, 76] / [92, 94] | SOUND. par supplies the complement/purpose relation represented by to after act; the relation is licensed even though the Tibetan verbal construction is conventional. |
| s170w9 | 5 | `byed` → `act` | [77, 81] / [88, 91] | SOUND. byed has a visible act exponent here, unlike campaign cases where an unrelated English auxiliary was falsely pinned to it. The dictionary also includes act. |
| s170p6 | 6 | `kyi` → `but` | [82, 85] / [121, 124] | SOUND. kyi is adversative between the three projecting kinds and the excluded same-life kind; it owns but. |
| s170f1 | 3 | `mthong chos la myong 'gyur gyi las` → `deeds with results that you see in this very life` | [87, 121] / [125, 174] | SOUND. The full deed-category phrase identifies deeds experienced in the current life; GMR expresses this through deeds with results that you see in this very life. d3 preserves the category and its experiential predicate; it does not bank myong as see or gyur as an invented dictionary equivalent. |
| s170p7 | 6 | `kyis` → `∅` | [122, 126] / None | SOUND. kyis marks the excluded deed category as agent; there is no separate English case word. |
| s170f2 | 3 | `ris mthun mi 'phen` → `do not` | [127, 145] / [175, 181] | SOUND. do not is a pro-verb ellipsis for do not project a discrete being, whose antecedent is the immediately preceding affirmative clause. d3 is justified for the whole repeated predicate, with no ris mthun→do dictionary claim. |
| s170p8 | 6 | `te` → `∅` | [146, 148] / None | SOUND. te continues the explanation across an English sentence boundary; the explicit because belongs to final phyir. |
| s170w10 | 5 | `las` → `deed` | [150, 153] / [274, 278] | SOUND. Second independent las owns deed in original deed, not a later plural deeds in the supplied outline. |
| s170w11 | 5 | `byed pa` → `performed` | [154, 161] / [251, 260] | SOUND. byed pa supplies performed in the relative clause of the heaps; the tense adjustment is ordinary English translation, not an added predicate. |
| s170w12 | 5 | `phung po` → `heaps` | [164, 172] / [239, 244] | SOUND. phung po owns heaps (HTG2016), not the full explanatory description of the actor. |
| s170w13 | 5 | `rgyun` → `stream` | [175, 180] / [229, 235] | SOUND. rgyun owns stream (HTG2016), excluding the preceding the and following of. |
| s170w14 | 5 | `de nyid` → `very same` | [181, 188] / [219, 228] | SOUND. de nyid is the emphatic anaphor very same, pointing back to the acting continuum. Both words are licensed; the article the is outside the span. |
| s170p9 | 6 | `la` → `upon` | [189, 191] / [210, 214] | SOUND. la is the locus where the maturation occurs; upon directly renders this case relation. |
| s170w15 | 5 | `rnam par smin pa` → `ripen` | [192, 208] / [204, 209] | SOUND. rnam par smin pa owns ripen in the explanation, supported by the exact HTG2016 entry. |
| s170p10 | 6 | `phyir` → `because` | [211, 216] / [191, 198] | SOUND. phyir supplies because for the explanatory cause clause, correctly reordered to the English clause head. |
| s170w16 | 5 | `gnyis pa` → `Next` | [218, 226] / [280, 284] | SOUND. gnyis pa introduces the second outline item, rendered Next. HTG2016 explicitly includes next/here next; this differs from falsely reading the ordinal as cardinal two. The remaining outline explanation is supplied. |
| s170p11 | 6 | `ni` → `∅` | [227, 229] / None | SOUND. Final ni marks the outline topic; Next already belongs to gnyis pa and there is no separate ni exponent. |

Unwrapped Tibetan with letters: `'i `; `'i `; `'i `. These are genitive/final suffixes; no content word was falsely omitted. Full uncovered English and Tibetan ranges are preserved in `review.json`.

### C05:171 — APPROVED_PROVISIONAL

| Span | d | Tibetan → English | Source ranges (T / E) | Judgment and evidence |
|---|---:|---|---|---|
| s171w1 | 5 | `thams cad` → `Every one` | [0, 9] / [0, 9] | SOUND. thams cad owns Every one as the exhaustive quantifier. has and the later each are supplied/repeated English framing; there is only one Tibetan quantifier. |
| s171p1 | 6 | `na` → `∅` | [10, 12] / None | SOUND. First locative na is recast through the Every one has subject construction; no separate locative word exists. |
| s171p2 | 6 | `ni` → `∅` | [13, 15] / None | SOUND. First ni is an unexpressed topic marker. |
| s171w2 | 5 | `'phen pa` → `projections` | [16, 24] / [19, 30] | SOUND. phen pa with initial apostrophe owns projections, a glossary-supported nominal use. |
| s171w3 | 5 | `bzhi` → `four` | [25, 29] / [14, 18] | SOUND. bzhi owns four. C05:172 explicitly explains the four deed projections; no erroneous auto-aligned gloss is used. |
| s171w4 | 5 | `dmyal ba` → `hells` | [32, 40] / [44, 49] | SOUND. dmyal ba owns hells. the remains outside the span. |
| s171p3 | 6 | `na` → `In` | [41, 43] / [37, 39] | SOUND. Second na owns In for the hells; this is distinct from the first na, which lacks a separate exponent. |
| s171p4 | 6 | `ni` → `∅` | [44, 46] / None | SOUND. Second ni is an unexpressed topic marker. |
| s171w5 | 5 | `dge ba'i` → `of virtuous` | [47, 55] / [57, 68] | SOUND. Inflected dge bai with apostrophe owns of virtuous. C05:172 expands this as projections of virtuous types of deeds; of is licensed by the actual genitive, and no head noun is fabricated. |
| s171m1 | 7 | `dge ba` → `virtuous` | [47, 53] / [60, 68] | SOUND. dge ba owns virtuous inside its inflected parent; the genitive is excluded. This is a contextual adjective corresponding to GMR wording, not a change to the curated dictionary gloss. |
| s171m2 | 7 | `'i` → `of` | [53, 55] / [57, 59] | SOUND. Bound apostrophe-i is the actual genitive and owns of. Its English position precedes virtuous while its Tibetan position follows the stem. |
| s171w6 | 5 | `gsum` → `three` | [56, 60] / [51, 56] | SOUND. gsum owns three in the hell-realm exception, explained again at C05:172. |
| s171f1 | 3 | `gang las` → `for which` | [63, 71] / [122, 131] | SOUND. Relative/ablative gang las qualifies the levels from whose desire the subject has become free. GMR uses for which; the whole relation at d3 is sound and creates no gang las dictionary entry. |
| s171w7 | 5 | `'dod chags` → `desire` | [72, 82] / [146, 152] | SOUND. First dod chags with initial apostrophe owns final desire in the relative clause, supported by HTG2016. |
| s171w8 | 5 | `bral` → `overcome` | [83, 87] / [137, 145] | SOUND. bral owns overcome in the freedom-from-desire construction; exact HGM glossary support. he is supplied by GMR, not part of this span. |
| s171w9 | 5 | `brtan pa` → `stable` | [88, 96] / [72, 78] | SOUND. First brtan pa owns the independent stable modifying child, not the embedded stable inside unstable. |
| s171w10 | 5 | `byis pa` → `child` | [101, 108] / [79, 84] | SOUND. byis pa owns child in the technical non-Arya sense. C05:173 explicitly supplies child [non-Arya]. A remains free. |
| s171w11 | 5 | `der` → `At that` | [109, 112] / [114, 121] | SOUND. der is de plus terminative/locative r and owns At that; C05:173 explains birth at the level for which desire was overcome. Both English words have actual licensors. |
| s171m3 | 7 | `de` → `that` | [109, 111] / [117, 121] | SOUND. de licenses that within At that; this is the level-anaphor, not an added English pronoun. |
| s171m4 | 7 | `r` → `At` | [111, 112] / [114, 116] | SOUND. The final r of der licenses At. It is a bound locative member, correctly d7 rather than a standalone particle. |
| s171w12 | 5 | `skyes` → `born` | [113, 118] / [109, 113] | SOUND. skyes owns born within the compressed next-life category skyes myod. C05:173 expands skyes nas myong gyur with apostrophe to experience there after being reborn. This is not a mapping to a child who is currently being born. |
| s171w13 | 5 | `myod` → `experience` | [119, 123] / [98, 108] | SOUND. Source myod owns experience in that same next-life category. Adjacent myong and commentary C05:173 establish the contextual correspondence. Keep the exact misspelling-looking token with its PROBABLE erratum note; this is attestation of this source reading, not recognition of myod as a standard dictionary lemma. |
| s171f2 | 3 | `mi byed` → `does none` | [124, 131] / [85, 94] | SOUND. mi byed owns does none as the negative deed-performance predicate. none picks up understood deeds, not a separate Tibetan numeral. Keeping the construction at d3 avoids mi→none or byed→none dictionary claims. |
| s171w14 | 5 | `'phags pa` → `realized` | [134, 143] / [162, 170] | SOUND. phags pa with initial apostrophe owns realized, the technical Arya category. C05:174 supplies realized being; the verse has no separate being to bank. |
| s171f3 | 3 | `gzhan du'ang` → `in others even` | [144, 156] / [172, 186] | SOUND. gzhan duang with apostrophe is the locative/additive other-life phrase in others even. C05:174 expands it to any other life beyond the next. It does not mean other persons or other realms, and it stays d3. |
| s171f4 | 3 | `myong mi byed` → `Neither` | [157, 170] / [154, 161] | SOUND. The FIRST Neither elliptically resumes the negative deed-performance/experience predicate myong mi byed, with the temporal category provided by f3. It is a whole-phrase correspondence, not myong→Neither or a null claim for myong. |
| s171w15 | 5 | `'dod` → `desire` | [173, 177] / [210, 216] | SOUND. Abbreviated dod with initial apostrophe owns desire in desire and peak, here the desire realm. It is the second English desire occurrence, separate from the emotional desire of w7. |
| s171w16 | 5 | `rtse` → `peak` | [178, 182] / [221, 225] | SOUND. rtse owns peak, the abbreviated peak-of-existence level. C05:175 explicitly expands srid rtse and peak level. |
| s171w17 | 5 | `mi brtan pa` → `unstable` | [185, 196] / [200, 208] | SOUND. mi brtan pa owns unstable as an established compound, independently listed in HTG2016. The article the is excluded. |
| s171m5 | 7 | `mi` → `un` | [185, 187] / [200, 202] | SOUND. Negative mi licenses only un inside unstable, with the approved negative subword flag. |
| s171m6 | 7 | `brtan pa` → `stable` | [188, 196] / [202, 208] | SOUND. Positive brtan pa licenses stable inside its unstable parent, distinct from first brtan pa→stable. The member is tighter than its parent on both sides. |
| s171f5 | 3 | `yang min` → `Neither` | [197, 205] / [188, 195] | SOUND. yang min owns the SECOND Neither as the additive negative concluding the third subject case. It remains a construction at d3, not a free-standing neither dictionary equivalent. |

Unwrapped Tibetan with letters: `'i, ,`; `'i `. These are genitive/final suffixes; no content word was falsely omitted. Full uncovered English and Tibetan ranges are preserved in `review.json`.

## Evidence files

`source-review.json` holds the independently queried original passages; `dictionary-evidence.json` holds inspected entries; `refutation-evidence.json` holds rerun SQL, parameters and results; `169/`, `170/`, `171/` hold fresh generator outputs. `review.json` contains all 87 individual judgments and exact ranges. `../errata-verdicts.json` is the requested ordered verdict list, also copied beside this report.
