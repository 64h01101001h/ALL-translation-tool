# C05:292–294 — independent semantic review

**SPEC: APPROVE. QUALITY: APPROVE.** The 111 proposed spans are sound at the
exact ranges recorded in `resolved-spans.json`: 65 non-null dictionary pairs,
16 compound members and four grammatical nulls. No semantic finding remains
open. This is Codex machine review of provisional alignments, not human
editorial acceptance, a change to hgm_gloss, or acceptance of a desktop/iOS build.

| Segment | Spans | Non-null d5 | d7 | Nulls | Native generator |
|---|---:|---:|---:|---:|---|
| 292 | 32 | 21 | 0 | 2 | EXIT 0; 5,940-byte exact body |
| 293 | 34 | 20 | 8 | 0 | EXIT 0; 5,601-byte exact body |
| 294 | 45 | 24 | 8 | 2 | EXIT 0; 7,308-byte exact body |

The reviewer did not author either proposal or the reconciliation. I reopened
the SQLite source in read-only mode, verified every final range through the
canonical resolver, ran each real generator once, and compared every output
byte to the frozen final body. All three stderr streams are empty. Removing
only markup and explicit null annotations reproduces all six complete source
columns exactly. Native commands, actual input/output/error/exit and exact
spec/body hashes are in `generator/`, `generator-results.json` and
`plain-source-fidelity.json`.

The proposal/reconciliation source origin is immutable commit
`129e86afae8431aacfb1ee1cee65d8635dd265b8` through 285. Actual acceptance baseline
is `c107f1ecd9cfb68968245870ad81ba5d9a070721` through 291. The review's governing
snapshots are taken from that latter commit. Each preserved historical snapshot
retains its own actual earlier Git pin. No historical builder snapshot was
compared against a changed live builder. All baseline generated outputs match
their immutable Git objects; the externally supplied master/corpus match their
actual byte hashes. `acceptance-baseline-binding.json` records the distinction.

## Evidence and historical decisions

I read the complete current source and surrounding debate, both frozen original
spec/report/decision sets, all final specs/reports, all 210 original tuple
dispositions, all 111 final decisions, the material omission decisions and their
complete residual intervals, and the source-suspicion record. The 507,436-byte
semantic input package is decomposed into 47 verified exact sections, including
all six complete proposal-to-final diffs. Identical repeated tuples, reasons,
source rows and glossary material were read once with explicit references;
their full original bytes are preserved. No omitted field was replaced with
an inferred original rationale. In particular, 14 English member records have
`original_reason: null`; their parent's/shared original rationale and the
reconciler's explicit member decision remain distinct evidence.

The independent source check reopened 267 distinct SELECT queries, including
392 exact original/reconciliation query references and new exact/variant
lookups for every final Tibetan unit. All recorded complete query rows and
counts agree with SQLite. All 95 returned lexical entries were also checked
against their complete original master records. The selected three source rows,
three C15 parallels and P7:35 exactly match seven original corpus records.
Full HGM variants were considered, including broader and competing positive
boundaries. Reference dictionary material was not used to manufacture English.
Complete C05 and C15 physical source files are preserved once; all three C05
ACIP/English passages were independently reopened and compared in full after
whitespace folding for the source's line breaks. Actual English remains
byte-exact in the alignments. P7 evidence is the complete original corpus row;
no separately inspected physical P7 publication is claimed.

The original failed/superseded work remains available, including the Tibetan
293 initial `meaning` occurrence error, the English 293 wrong `as`/`of`
occurrence plan, the English 294 pre-generation third-occurrence failure, the
physical-source UTF-8 decoding failure, and the reconciler's superseded 293
occurrence plan. Passing generation never established those earlier placements
as semantically correct. Final 293 places `point` at English65:70, the later
`meaning` at302:309, `statement` at326:335, `as` at183:185 and441:443, and the
drop relation `of` at411:413. All were checked against the actual argument.

One evidence attribution is corrected explicitly here while preserving its
history. The English original's 292 drang don rationale calls the expanded
English an exact glossary attestation, and the 293 final decision calls
`figuratively` an exact HGM equivalent. The actual curated record HGM8829 says
`the figurative`. These source passages support the expanded predicate and
adverb as **contextual corpus attestations**; they are not the literal stored
glossary strings. The technical unit and local meanings are sound, so no span
change is required. Neither original claim is silently rewritten.

## C05:292

The opening conditional and negative existential are complete phrases.
`med par` owns `do not even exist at all` as a grammatical predicate with GMR's
emphasis; it does not create an isolated auxiliary dictionary entry. `thal`
owns the consequence `would have to say`, excluding `one` and `that`. The
following `de` is the doorways anaphor `they`, and the two positive `yod`
occurrences own their respective positive `exist` expressions.

Both `skye mched` renderings are established whole technical equivalents in
HGM692, including `doorways through which perceptions grow`. No etymological
grow/perception member is guessed. `drang don` is the established figurative
classification and is explained locally as `do not mean what they say`.
`dgongs pa can` has exact HGM2403 variants `have to interpret` and the complete
`must be interpreted to establish their real meaning`. These remain d5 despite
their English length. The first expression's further purpose explanation and
the second expression's later figurative restatement remain unbanked reprises.
There are two Tibetan `mdo` tokens and three English `sutras`: first and second
are banked, and the third repeated noun is not made into an extra source token.

The established `nyi shu pa` title, `rang 'grel` autocommentary, and
`bcom ldan 'das` epithet remain whole. Inner `gsungs pa` owns `says` in the
Buddha quotation; outer `zhes gsungs pa` owns `states that` before the quotation.
An isolated null for `zhes` would obscure the complementizer and is correctly
rejected. The two final `phyir` relations are `Because` and the introductory
`for`; the latter is not the beneficiary `for` later in English.

I challenged the broad recipient phrase against the tight HGM9569
`'dul ba'i skye bo`/`disciples` unit. The tight established unit is preferable
here: teaching and educability jointly inform `who might require such
explanations`, and no stable independent `ston pas`/`require` correspondence
is established. That remainder is deliberately unbanked, not absent. The
beneficiary relation `ched du`/`for the benefit of` stays complete at d3.
`bka'` owns `word`, excluding `His`.

The two nulls are justified: `rnams` is plural morphology fused into `doorways`,
and `ni` has topic function with no separately assignable English exponent.
Neither null asserts the grammatical meaning is absent. Other copular,
possessive, citation and additive material remains explicitly unassigned where
the English recast distributes its contribution. No title member is fabricated.

## C05:293

The two response/claim formulas are defensible complete phrases. The established
`skyon med`/`no problem` retains the strictly smaller problem/no members while
excluding `There's`. Both `phyi rol don` compounds retain outer/object members,
with plural and singular occurrences distinguished. The `du` particles select
the respective relations immediately before outer objects/object. First
`grub pa` gives `existing`; second gives `exists`. The first `yod` contributes
to the same distributed existential construction and is omitted without an
absence claim. `actually` is not silently assigned to `yod`.

The full `dgongs pa can` interpretation locution is explicitly in HGM2403 and
receives no speculative interpretation/meaning members. Independent `don`
owns `point` in the initial explanation and `meaning` in the original-statement
rebuttal. `lung` is that later original `statement`, not the earlier explanatory
`this statement`. Final `ces pa'i don yin pa'i phyir`, the conditional bridge
and additional copular/reason framing remain unbanked because the wording is
redistributed into the saying/meaning argument. A false `don`/`saying` atom
would not improve coverage.

The negative indefinite retains `thigs pa`/`drop`, `yang`/`even` and
`med`/`not exist`. The final negative includes both nonexistence and negation;
`does` and the article `a` remain outside. `'ga'` contributes to the complete
negative-minimizer structure rather than an independently supported English
atom here. The wider positive `mtshungs`/`just the same` was considered; the
tight `same` avoids owning an article or an unneeded intensity boundary.
`rnag khrag` retains locally justified pus/blood members; reciprocal dictionary
variants do not force the wrong member at this occurrence. There are no nulls.

## C05:294

`'bras bu med pa`/`bare` is the warranted recovery between the original broad
`nothing but bare limbs` and the other angle's complete omission. HGM13452
explicitly contains `bare`, `has no fruit` and `no fruit`; the local tree
context supports fruitlessness. Limbs is supplied imagery and receives no
Tibetan member. The fruit-bearing `'bras bu can` compound is present as a full
master unit with no hgm_gloss. Its actual corpus wording is provisionally
`loaded with fruit`, with well-supported `'bras bu`/`fruit` and
`can`/`loaded with` members. The broader positive `'bras bu`/`loaded with fruit`
variant does not erase the explicit local `can`.

The first two `mig shes` occurrences give the two local `sees` verbs, an
HGM14365 variant justified by the river and tree clauses. The collective
`visual consciousness of both` recaps those same cases and stays unbanked.
This plan preserves both local occurrences without double-banking one shared
English phrase. Final `mig shes` gives the later `visual consciousness`
at English598:618 with its strictly tighter visual/consciousness members.

All three `tshad mar song ba` validity-result constructions are retained as
complete phrases: the first and second `genuinely so`, then `genuine`.
The positive HGM20922 `song ba`/`so` was considered, together with the
HGM15288 genuine/genuinely variants and HGM24806 inflected `tshad mar`.
This supports the full grammatical construction without asserting that it is
an established d5 dictionary compound. Two separate `tshad ma` nouns own the
two `valid perception` conclusions. `dngos gnas pa` owns `actual`, not the
validity adjective `genuine`.

Each looks/looking and its `la`/`at` relation stays in the proper river, tree
or glass example. `chu klung` has both river-of-water and riverbed equivalents;
no broken river member is taken from riverbed. The wet/and/flowing/glass/full
units are sound and keep supplied `of something` outside. The `zhing`
conjunction is not the homographic curated paradise/world noun. One
`rnag khrag` owns the first English `pus and blood`; the predicate repetition
is deliberately unwrapped. `thal` gives consequence `must` and final `phyir`
gives the causal `Why`, with `Remember` as extra English prompting.

I challenged all three temporal clauses and the possessors/validity relations.
The three `de'i tshe` occurrences are reorganized into scenario descriptions,
`case`, `consider` and `Remember`; none requires a forced `tshe`/`case` entry.
The `de'i` by the early visual perceptions is a perceiver relation, while
English `it` is the perceived object; they are not interchangeable. Remaining
copulas and genitives are distributed in the two predicates and collective
summary and remain unbanked with reasons. The two case nulls, `kyis` and
`gyis`, are justified only by active English subject syntax with no separate
agentive word at their positions.

## Head allowances and source suspicions

Three head allowances are independently justified: 292/w3 and292/w22
`la sogs pa`/`and other such`, and293/w12 `sogs`/`and similar`. The complete
enumerative units own the conjunction, supported by HGM19208 and20919 and
the local form-list construction. Their following nouns are excluded. The
exact bank-ready map is `span-head-allow-needed.json`; no production allowlist
was edited by this reviewer.

All seven source suspicions are refuted on defensible-reading ground d, after
checking all five grounds independently. Quote locations, word counts, full
field-equality queries, C16/P1/TCS search results, complete original digital
witness rows and exact field differences are preserved in
`source-suspicion-independent-review.json`. Short searches for `sutras` and
`valid perception` each find eight C16/TCS overlaps, but those are different
passages, not independent publications of these exact paragraphs. Longer
target phrases have no hits in that targeted scope. No publication or
ingestion lineage is inferred from any digital match.

C15:437 has identical ACIP/Wylie to292 and English lengths756 versus752,
with `do`/`did` and one inserted `do`. Both C05 present-tense clauses are
physically preserved and defensible. C15:438 and439 match the three source
fields of293/294. P7:35 shares exactly the first624 of294's642 English
characters, ending before ` valid perception.`; it cannot corroborate the
missing ending. The full C05 physical text and final Tibetan `tshad ma` do
support that ending. No new registered-class defect or duplicate is found
among the192 existing records. `bank-ready-errata.json` is an empty list.

## Preservation and limits

All source specs, original/reconciled decisions, substantive attempts and
generator results remain exact. Identical immutable evidence is stored once
and every original path is mapped to it; current and changed historical core
spec/body/report/errata are readable by role and segment. The manifest verifies
each copied byte and all historical Git pins before freezing. Unsupported
helper extensions are retained as exact-byte `.py.txt` files.

One review bookkeeping attempt failed because it assumed the externally
supplied master was Git-tracked. That happened after the package, source,
disposition and five-ground checks had passed, and before approval. The actual
script/error/exit are preserved in `history/supplement-attempt01`. Only the
remaining storage/baseline checks resumed with the correct Git-versus-external
distinction and passed. No generator was rerun, no semantic evidence was
overwritten, and no source or production file was changed.

Production registration, complete fidelity suites, measured generated coverage,
desktop/iOS consumption and the independent outer-checkpoint integration
disposition remain root's separate acceptance work. This review asserts none
of those outcomes. All accepted correspondences remain PROVISIONAL.
