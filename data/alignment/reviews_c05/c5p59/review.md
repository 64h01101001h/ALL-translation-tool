# Independent skeptical semantic review — C05:175–177

**APPROVE 175, APPROVE 176, APPROVE 177.** No required spec changes and no blocking semantic findings. This is machine review of **PROVISIONAL** attestations, not human acceptance or promotion into `hgm_gloss`.

All **97 spans** were reviewed against original source: **53 linked d5/d7 dictionary pairs**, **14 null claims**, **18 linked d6 particles**, and **12 linked d2/d3 clauses or phrases**. `review.json` records a verdict, exact zero-based half-open source ranges, surrounding text, and a semantic reason for every span.

| Segment | Verdict | Spans | Linked dictionary pairs | Nulls | Actual generator | Bytes |
|---|---|---:|---:|---:|---|---:|
| C05:175 | APPROVE | 44 | 23 | 7 | EXIT=0 | 8869 |
| C05:176 | APPROVE | 18 | 12 | 1 | EXIT=0 | 4120 |
| C05:177 | APPROVE | 35 | 18 | 6 | EXIT=0 | 7657 |

Evidence and limits

Read the current alignment specification, reconciliation/refutation briefs, Rule 10, campaign conventions, current specs and reconciliation reports. Original wylie, English and source-of-record ACIP came directly from read-only SQLite; C05:174–180 context is retained in `source-read.json`. The actual generator was rerun separately for each current spec, with empty stderr and bodies byte-identical to the reconciled outputs. `generator-checks.json` records the input hashes and outcomes. `resolved-spans.json` records the actual generator’s Tibetan and English occurrence resolutions. None of these mechanical checks was treated as proof of semantic correspondence.

The historical proposal attribution for 175 remains a ledger attribution; no fresh historical-model execution is claimed. This review used Codex and local reads only. No paid API, browser workaround, extra agents, database writes, or changes to proposals, reconciliations, project files, or existing errata were used.

Focused challenges and decisions

1. **C05:175 f4 — APPROVE (severity INFO).** Does the negative until-restoration source justify the positive gaining wording?

   The whole interval de las nyams pa sor ma chud gyi bar du means from loss of that result until its restoration. English during the period between losing his result and gaining it back again has the same interval endpoints. The negative ma belongs to until-restoration syntax, not an isolated denial of gaining. At d3 this retains the complete temporal relation and anaphoric result without falsely nulling sor ma chud or mapping it alone to gaining.

   Recommendation: Retain the complete interval at d3 and its explanatory note. Do not isolate sor ma chud as gaining or mark it unrendered.

2. **C05:175 f1, f2, f3 — APPROVE (severity INFO).** Could locative contraction or shared experience wording become a false dictionary mapping?

   The contracted coordination dod rtser denotes the desire realm or peak level with a locative ending. The whole locative coordination maps the complete English temporal-domain modifier at d3; no invented single dictionary headword is created. skyes nas myong gyur is the whole next-life experience category. English first describes experience subsequent to rebirth and then restates that category as for either his next life. This is a complete contextual phrasal paraphrase at d3, with no lexical member capturing lead to, his, or life in isolation. The following or is separately owned by dang. The further-life experience category is rendered as for any life beyond under the shared earlier will lead to an experience predicate. A d3 alignment records that elliptical coordinate phrase; it does not claim myong means life or is semantically absent.

   Recommendation: Retain these complete contextual phrases at d3, without adding speculative lexical members.

3. **C05:175 p4, p7 — APPROVE (severity INFO).** Does assigning Neither to min falsely null the additive yang?

   yang contributes additive continuation to Neither rather than a separate also/either word. The null explicitly says fusion and no separately alignable word, so it does not infer absent meaning. English ever is temporal emphasis, whereas de yang adds this type of person to the previous case. Negative min owns Neither at grammatical depth. The HGM glossary attests neither for min, and the note explicitly retains yang as an additive co-contributor without claiming a separate word for it. No d5 lexical equation or English negative-affix invention is made.

   Recommendation: Retain the explicit fusion/no-separate-word note. Neither is the negative-additive continuation; do not rewrite the null as absent meaning.

4. **C05:176 c1 — APPROVE (severity INFO).** Could d2 conceal a false first de → One assertion?

   de ni mthong chos bras bu yin is a complete subject-predicate clause. English One is the type with results that you see identifies the relevant class with a present-life-result predicate. C05:178 explains the referent as the one group of eleven definite cases. The d2 record therefore has contextual clause correspondence, while making no lexical equation de/One and no independent mthong chos/you see or bras bu/type claim. The first de, ni and yin remain represented within the clause rather than falsely null.

   Recommendation: Retain the complete clause at d2, its C05:178 referent explanation, and no dictionary members.

5. **C05:176 p3, w5, w6 — APPROVE (severity INFO).** Exclusive but and the division of single/discrete type may be misassigned.

   kho na owns but in the restrictive expression but a single, meaning only. This is not adversative but, and d6 honestly records it as the exclusive particle. rigs is the category noun in discrete type; gcig supplies single separately. This contextual nominal equivalent is present in this very row and is supported by C05:178 expanding the same explanation through ris mthun gcig kho na and a single discrete being. The HGM rigs entry lists discrete and type as supporting vocabulary, not a license to compose or promote a new binding gloss. gcig owns single, independently from kho na exclusivity and rigs category.

   Recommendation: Retain kho na/but at d6, gcig/single, and contextual rigs/discrete type; preserve RIGS verbatim.

6. **C05:177 w1, f2 — APPROVE (severity INFO).** State-to-person recast might become a false bar do/person dictionary entry.

   First bar do owns state between death and rebirth, an attested HGM whole-term equivalent. Supplied beings of the remains outside this dictionary unit. bar do de nyid identifies the intermediate stage itself as one of the eleven periods. English names its bearer as inbetween being himself. The whole deictic/emphatic expression has contextual phrasal correspondence, while d3 prevents either bar do/person or de nyid/himself from being manufactured as dictionary equivalents. The distinct earlier bar do/state term is unchanged.

   Recommendation: Retain the first supported state term at d5 and second complete emphatic phrase at d3, with no false de nyid/himself member.

7. **C05:177 f4 — APPROVE (severity INFO).** Does the ambiguous English ellipsis support definite/indefinite correspondence, or require omission?

   The whole certainty opposition myong bar nges ma nges corresponds contextually to those which will definitely be experienced and those that will not. After the first will, the VP is definitely be experienced; the second will not can grammatically elide and negate that full VP. This preserves both nges occurrences. C05:178 immediately resumes the eleven definite cases, supporting definite-versus-indefinite classification rather than guaranteed non-experience. The English also admits a stronger everyday reading that denies experience alone; the current note states that limit. Retain d3 because the contextual whole-phrase correspondence is supported, not because greater depth excuses a contradiction. No dictionary pair ma nges/not experienced is endorsed and no source correction is asserted.

   Recommendation: Retain at d3 with the current ambiguity note. The whole contextual opposition is supported; omission is not required. Do not promote a word-level not-experienced mapping or assert that indefinite means guaranteed never experienced.

8. **C05:177 p3, p4, w12 — APPROVE (severity INFO).** Nominal-to-finite recast could conceal false existential or genitive nulls.

   The nominal genitive relation las kyi phen pa is recast as subject plus verb deeds project. No separate English genitive word appears; the note explicitly describes the structural realization. First yod is the existential in the nominal Tibetan question about whether projections exist. English asks whether deeds project; there is no separate existential word. This null is grounded in the identified nominal-to-finite recast, not inability to find a match. Final yod owns are in There are twenty-two, whose answer has been fronted in English. It is distinct from the first existential question yod, which has no separate word.

   Recommendation: Keep the first grammatical nulls limited to absence of separate words; retain final yod/are at its proven occurrence.

Nulls, omitted meaning, and occurrence selection

- **175:** `p3/p11` are governed comitatives recast as direct objects; `p14` is the loss ablative likewise recast; `p13` is topic; `p8` continuation; `p17` gives the explanatory reason with no separate “because.” `p4` expressly records yang’s additive contribution fused with “Neither,” not semantic absence. The genuine negative compounds retain all four supported affix/stem members. The first `de → he`, overt `yin → is`, and complete restoration interval remain represented. The three unwrapped genitive suffixes are omitted without false-null claims. Repeated desire/realm/peak/result/loss strings attach to their intended sentences.

- **176:** only the second `ni` is null, because there is no separate English topic word. First `de/ni/yin` stay inside their complete clause; second `de → It` and final `yin → is` are independently located. All six compound/numeral members are strictly internal and semantically compositional. The spelling `nyi shu gnyis` is preserved rather than normalized.

- **177:** first `kyi/yod` have identified nominal-to-finite recasts without separate English words; final `yod → are` attaches to the fronted total. `ste/te/la` have no separate continuation words; numeral `rtsa` is typographic linkage rather than a separate English word. The second certainty predicate is inside `f4`, not falsely nulled. The state/person recast is a phrase; the prenatal/postnatal five counts and the final list conjunction are located correctly. Apparatus is unwrapped.

The omission scan finds only three genitive suffixes plus punctuation at 175, punctuation at 176, and `[iv.209-12]` apparatus plus punctuation at 177. This scan is descriptive and was not used as semantic evidence or as a reason to force coverage.

Function-head licensors

There are **zero gated d5/d7 supplied heads** and **no required allowlist additions**. `gated-head-check.json` contains the independently computed empty list. Full explicit function-link evidence is in `review.json → function_licensors`. The salient grammatical licensors are:

| Segment | Span | English | Tibetan licensor / scope |
|---|---|---|---|
| 175 | w7 | he | de, resumptive subject |
| 175 | p7 / p4 | Neither | min negative; yang additive contribution is explicitly noted as fused |
| 175 | p6 | which | gyi, adnominal relative link |
| 175 | p15 / p16 | is / But | yin / connective la |
| 176 | w4 | It | second de |
| 176 | p3 | but | kho na, restrictive only in but a single |
| 176 | p4 | is | final yin |
| 177 | p1 / p2 | with / as well | na locative recast / yang additive |
| 177 | p7 / p6 | in / and | womb genitive gyi / list connective dang |
| 177 | w12 | are | final yod |

English “One” at 176 is contained only in a complete d2 clause whose referent is identified by 178; it is not licensed as a standalone de/One word pair. Supplied articles and framing stay outside dictionary spans.

Errata

No new erratum emerges; `errata.json` is empty. Direct exact-source queries find no other passage witnesses for these three rows; that does not prove a source error. The appended 175 heading is the already registered following-heading class. “Projections had with” occurs at both 176 and 177, in verse and explanation, and is defensible compressed wording. At 177 the stronger non-experience reading remains possible, but the syntax and 178 context support the certainty-contrast interpretation; therefore no factual correction is asserted. E-183/C05:171 and E-184/C05:172 are already filed and are not duplicated. No candidate required a new five-refutation erratum submission.

Required changes: **none**. Retain the present contextual/ambiguity notes with these provisional alignments; approval does not extend to isolated dictionary equations that those notes expressly reject.
