# Independent skeptical semantic review — C05:178–180

**APPROVE all three final reconciliations. Blocking findings: 0. Required changes: none.**

The review covers all **104 explicit spans**: **92 linked**, including **69 dictionary pairs**, **16 linked grammatical particles**, **7 linked phrases**, and **12 null claims**. Every retained tuple has its own semantic verdict, reason and independently resolved zero-based, half-open source ranges in `review.json` and `resolved-spans.json`. There are no d7 members. These are **PROVISIONAL machine attestations**, not human acceptance or promotion into `hgm_gloss`.

| Segment | Explicit spans | Linked | Nulls | Dictionary pairs | Actual generator | Final body bytes |
|---|---:|---:|---:|---:|---|---:|
| C05:178 | 32 | 29 | 3 | 20 | EXIT=0 | 6232 |
| C05:179 | 37 | 32 | 5 | 26 | EXIT=0 | 6909 |
| C05:180 | 35 | 31 | 4 | 23 | EXIT=0 | 7353 |

The originals and C05:176–183 context were read directly from SQLite using `mode=ro` before the final reconciliations were opened. Current AGENTS, alignment spec including the new null clarification, reconciliation/refutation briefs, Rule 10 and C05 course conventions were read. After the root's explicit ready signal, every final spec, note, report and errata claim was inspected. The root subsequently changed only the stale “awaiting separate semantic review” prefix to “PROVISIONAL machine alignment”; the final files were reread and rerun. All final stdout bodies match their reconciled bodies byte for byte, all stderr files are empty, and the actual generator used `PYTHONDONTWRITEBYTECODE=1`.

The semantic judgment is independent of the generator's success. A separate positional resolver rederived all Tibetan and English occurrence ranges and agreed with the actual generator. No mechanical pass was treated as evidence that a Tibetan–English correspondence was true.

## Main challenges and outcomes

- **178: technical term and temporal phrases.** `mthong chos` is the established present-life technical unit and supports “see in the same life” without an invented `chos → life` member. Each `gnas skabs kyi` phrase correctly owns its own “when” for collection or experiencing ripening. These are defensible complete temporal phrases, not phrasal camouflage for false atoms.
- **178: repetition and grammatical scope.** The three `las/deeds` pairs attach to the three body occurrences, not the later outline. The first `gcig` owns “same” in the shared projecting cause; the second owns “single” in the discrete-being predicate. The opening `yin` owns “is considered,” restricted to “is”; later `yin pa'i` owns “is counted,” also restricted to “is.” The intervening “This is because” copula is not mistakenly selected. `phyir` owns “because”; `kho na` owns exclusive “but.” Whole `gis` correctly marks the agent expressed by passive “by” at d6. The historical policy disagreement about agentive “by” remains open and this page does not resolve it.
- **179: compounds, relatives and copulas.** `rab dang` is the faith compound, supported by the glossary and the expanded form in 180; its `dang` is not a conjunction. The `gang` after `gsod` licenses the opening relative “which” across the complete list, `de` owns “Those,” and the last `gang` owns “Anything.” Final `yin pa'o` correctly owns “was” in that last relative clause. The null `yin pa` belongs to the earlier reduced “anything done” construction.
- **179: genitive and causal recasts.** `sa de'i → the level's` is a complete deictic/genitive phrase, with a real determiner and possession licensor. `gtan/completely` and `bral/free` remain true lexical links. `phyir/Where` is not banked as an isolated equation; its causal/conditional recast is explicitly noted as omitted rather than absent.
- **180: complete phrases.** `zhen → one may ask` is supported by the directly checked interrogative contexts C08:43/205, without inventing a dictionary definition of “ask.” `shugs drag po → fierce emotions` has contextual and HGM glossary support as a whole phrase; no `shugs/emotions` member is invented. `la brten pa/with respect to` and `kyang rung ba/even when` preserve the reference and concessive relations.
- **180: true fusion and repetition.** `gyi/of` correctly selects “of” after “object,” not “of either.” The `kyi` before `blos` has its qualification expressed through “meritorious intent,” so its explicit adjectival-fusion null is justified. Whole `blos` owns “intent”; its instrumental contribution to “with” is not split into a forbidden `-s` span. The final source conclusion is fronted as “Those deeds are definite”: the correct later `las`, `de`, `gang`, `rnam smin`, `myong bar` and `nges pa'o` own the intended conclusion/list occurrences. “Definitely” is not counted twice; “future” stays unwrapped.

## Every null and omission

The per-span decisions and complete omission inventory are recorded in `review.json`, with raw source gaps in `omission-check.json`. The gap scan is descriptive; it was not used to force coverage.

- **178's three nulls** are topic markers, including the final outline topic. The overt `yin/is`, both `when` relations, results and `can/with` are retained. Omitted `la`, `gyi`, `byed pa`, `te` and the projecting `kyi` have identified temporal, attributive or classificatory recasts. None is falsely labelled semantically absent or attached to an English phrase of another scope. Energy, presence, passive and explanatory additions, the expanded outline and the heading remain unwrapped.
- **179's five nulls** are three punctuation-rendered enumeration connectives, the copula reduced in “anything done,” and declarative `so`. Omitted `gyi` and `phyir` have explicit attributive and causal/conditional recast reasons. Parent-coordinating “or” and affliction/faith “or” are not reassigned to later list connectives. The chosen `yon tan/Qualities` boundary is the tighter attested lexical core; the wider “special qualities” is also attested and is not declared false.
- **180's four nulls** are two punctuation/numbering-rendered enumeration connectives, the copula reduced in “anything done,” and the proven genitive-to-adjective fusion. Initial `myong` is distributed within the compressed ripening category and is omitted with a note; initial `kyi/de` and final relative `yin pa` are structurally recast. Their omission is not a claim of absent meaning. Source `byas pa` belongs to the continual third item, not the later explanatory “this is done” in the fourth item. Inflected words are retained whole, without invented members. Citation apparatus, supplied numbering, framing, repeated certainty and “future” remain unwrapped.

No omitted lexical or grammatical material requires an additional sound separate link beyond those retained. There are **zero gated d5/d7 supplied heads** and **no required allowlist additions**. Every function link has an explicit licensor in `review.json`. No d7 parent/member or agentive-splitting defect exists.

## Errata refutation

**One claim survives: C05:180 `MYON MONGS PA → NYON MONGS PA`, LOW/PROBABLE.** The source remains unchanged. The independent five-check record is `errata-five-checks.json`; the ordered verdict list is `errata-verdicts.json`; the bank-ready list was written only after all five checks in `bank-ready-errata.json`.

1. **Verbatim source:** `MYON MONGS PA` appears once in ACIP; Wylie agrees with `myon mongs pa`. The English expresses the same first mental-affliction condition as root 179.
2. **Parallel witnesses:** direct comparison finds C16:759 identical in ACIP, Wylie and English, with no differences. It is the recorded same-source reingestion; no P1/TCS same-passage witness was found. Directly read local `SE5525M` and `SE05525M` files have identical bytes and preserve `MYON` at `@124B`. The distinct local `SL05525N` file reads `NYON` at the same passage. These digital facts do not establish independent publication, and no blockprint or manuscript was inspected.
3. **Register/classes:** no C05:178–180/MYON duplicate is present. This lexical spelling is not a registered converter escape, heading, folio, footnote, row-spill or field-cap class. The 178 following heading is already registered and is not a new claim.
4. **Defensible reading/correction:** exact `myon mongs pa` and `myon` dictionary headwords are absent, as are matching entry variants. `nyon mongs pa` has the HGM “mental affliction” equivalent; adjacent root 179 has `NYON MONGS`, and the directly checked SL passage supports `NYON MONGS PA`. A targeted library search finds `MYON MONGS` only in the two byte-identical SE aliases. No local evidence establishes an intentional lexical MYON variant here. This supports probable restoration, not a confirmed source correction.
5. **Counts/evidence:** the non-C13 disputed-form query returns exactly two rows, C05:180/C16:759; the corrected prose query returns zero; there are zero exact disputed headwords and one exact corrected headword; all three source fields match across C05/C16; all stated local file hashes recheck. No uniqueness or independence claim has been added beyond these explicit measurements.

`phung bo` at 178 is not filed: its HGM “heaps” entry and TCS17:366 show its lexical use, and the digital spelling difference alone does not prove a defect. Root-verse capitals at 179 are lineation. No other erratum is asserted.

## Final reviewed identities

| Segment | Final spec SHA-256 |
|---|---|
| 178 | `b2b509a69e529db8edbd924fce3788259ed007567f9ad8beaa9701c598cd64b5` |
| 179 | `0f76d7795696abd5f9620cd25c69b763443ddae61dad17e6c3d7e6343fd708bc` |
| 180 | `94333c74a9ff01cbb5c0a6cfeb7c0a86cbb550bab1e2aafeebc1257819078846` |

Ordered reviewed-tuple SHA-256: `29b8b7b4149423640c83961d782efea5585a0a38ef80c29a3bba248254b28d74`. The exact canonical encoding is documented in `review.json`. `generator-checks.json` contains final body hashes, byte counts and comparisons. `source-read.json`, `glossary-check.json`, `semantic-context-check.json`, `local-digital-witnesses.json` and `commentary-local-witnesses.json` preserve the independent source and support reads.

All writes were confined to this semantic-review directory. No reconciled spec, source, master, glossary, registry, browser, device, API or git state was changed by this reviewer. No C13 alignment scan or delegation occurred. Keep the present scope, omission and provenance notes with the provisional alignments.
