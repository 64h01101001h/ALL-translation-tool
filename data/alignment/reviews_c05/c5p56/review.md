# Independent semantic review — C05:166–168

**Verdict: APPROVE 166, APPROVE 167, APPROVE 168.** No blocking semantic findings; no spec changes requested. All remain provisional attestations.

Reviewed all **85 spans**, including **62 non-null d5/d7 dictionary pairs**, **14 null claims**, **2 d3 phrases**, and **7 non-null d6 particles**. Each individual span has a verdict, semantic rationale, verbatim source context, and zero-based half-open Tibetan/English ranges in `review.json`. The nine focused challenges below survived review; they are informational dispositions, not defects.

| Segment | Verdict | Spans | Kept d5/d7 pairs | Nulls | Generator | Body bytes | Errata |
|---|---|---:|---:|---:|---:|---:|---:|
| C05:166 | APPROVE | 32 | 23 | 7 | EXIT=0 | 5650 | 0 |
| C05:167 | APPROVE | 35 | 26 | 5 | EXIT=0 | 6992 | 0 |
| C05:168 | APPROVE | 18 | 13 | 2 | EXIT=0 | 3854 | 0 |

Sources and mechanical evidence

- Read `AGENTS.md`, the alignment specification, reconciliation brief, Rule 10, and C05 conventions. Required reference roots were readable.
- Obtained wylie, English, and source-of-record ACIP directly from `campaign-worktree/build/hgm_spine_v27_2.db` with SQLite `mode=ro`; did not substitute proposal quotations. Exact reads are preserved in `source-read.json`.
- Reran the actual `tools/gen_alignment_page.py` on each reconciled `spec.json`; all exited 0, stderr was empty, and all three generated bodies match the reconciled bodies byte-for-byte. Spec SHA-256 values and run outcomes are in `generator-checks.json`.
- Resolved every span with the generator’s actual `resolve`/`with_members` functions and inspected the resulting source ranges. Output is in `resolved-spans.json`. Generator success proves mechanical admissibility only.
- Queried the HGM glossary and relevant corpus context directly. Exact parallel rows are only C16:753/754/755, each identical in wylie, English and ACIP; these are same-lineage re-ingests, not independent witnesses. Context and glossary records are in `witness-check.json`.

Focused semantic challenges

1. **C05:166 w21 — APPROVE (severity: INFO).** Does rigs own the full nominal discrete being, or is being supplied?

   Evidence: Contextual whole nominal correspondence is supported by C05:170, whose expansion of the same gsum gyis ... ni phen par byed line uses ris mthun and repeats Three of them act to project a discrete being. The verse preserves rigs/RIGS; no spelling correction is assumed. Narrowing to discrete would retain only an adjective for the nominal object and is not required by Rule 10.

   Recommendation: Retain the contextual whole mapping and its C05:170 note. Do not correct RIGS to RIS MTHUN or present a general definition beyond this citation.

2. **C05:166 w5, m1, m2, m3 — APPROVE (severity: INFO).** Are the mthong/chos members honestly compositional, and is the genitive null true?

   Evidence: Parent [34,50) maps English [93,107); mthong maps seen [103,107), chos maps something [93,102). The technical term is in hgm_gloss; curated chos records things / objects. Genitive is structural with no separate English word.

   Recommendation: Retain parent and current tighter members; do not split something or invent an independent ba equivalent.

3. **C05:166 w6 — APPROVE_WITH_LANDING_ACTION (severity: INFO).** Function-word head in and such needs an exact licensor.

   Evidence: la sogs pa [51,61) owns and such [108,116), only the conventional list-closing gesture. C05 convention and c5p33/s99w20 already recognize the same closure. Target allowlist key is currently absent because the page is unlanded.

   Recommendation: During the planned landing, add pages_c05 key c5p56/s166w6 with exact Tibetan licensor sogs. Do not widen to a supplied noun.

4. **C05:167 w29, (omitted bzhi) — APPROVE (severity: INFO).** Could gsum silently attach to the conflicting opening three, or could a null hide the numeral discrepancy?

   Evidence: Final gsum owns later three at [215,220), in the definite subgroup. Opening English three at [14,19) and Tibetan bzhi/BZHI at [49,53) are both unassigned; their discrepancy is preserved.

   Recommendation: Retain later gsum/three only; keep opening bzhi and opening three verbatim and unmapped. Leave existing TODO issue pending without declaring a correction established.

5. **C05:167 w11, w13, w16a, w18, w22a, w25 — APPROVE (severity: INFO).** Repeated myong ownership and false absence in explanatory clauses.

   Evidence: Item 1 has both label experienced [296,307) and explanatory experience [364,374), owned by label [169,174) and explanation [135,140). Items 2 and 3 have one experienced each, in their labels [426,437)/[527,538); their explanations contain only temporal phrases after that is/which is to say. Nulls select the preceding explanatory myong [206,211)/[292,297), never the overt-label occurrences.

   Recommendation: Retain the occurrence choices and narrowly worded lexical-ellipsis nulls. Continue to omit, rather than null, auxiliaries whose meanings are distributed across lead to/future/ripen.

6. **C05:167 p3, p4 — APPROVE (severity: INFO).** Temporal paraphrases could smuggle false dictionary pairs for phyi ma or lan grangs.

   Evidence: The complete Tibetan temporal phrase says a life beyond the previously named next life. Its d3 equivalent in any life after the next is a coherent paraphrase, kept outside dictionary harvesting. de'i is anaphoric to next life; later phyi ma is not independently paired with next. lan grangs gzhan owns the contextual phrase some other life at d3. lan grangs alone is an occasion/count expression, so no false lan grangs/life dictionary entry or speculative member is minted. C05:174 and C08:163 corroborate the further-lifetime referent.

   Recommendation: Retain both complete phrases at d3 without d7 members; do not bank later phyi ma/next or lan grangs/life.

7. **C05:167 w12, m1, m2 — APPROVE (severity: INFO).** Technical compound members might replace the whole present-life category with generic seeing or phenomena.

   Evidence: mthong chos is itself in hgm_gloss with something seen; the source English deliberately quotes that expression and explains it as this very life. seen and something are internal lexical exponents, not alternative translations of the complete karmic category.

   Recommendation: Retain the whole compound and compositional members only within that parent.

8. **C05:168 w1 — APPROVE (severity: INFO).** Is people supplied English beyond kha cig?

   Evidence: The subject makes the zer/claim. hgm_gloss for the whole indefinite pronoun kha cig explicitly includes some people, some thinkers, someone and some. Some people is therefore a contextual whole-unit exponent here.

   Recommendation: Retain Some people as the whole pronoun; add no speculative kha/cig members.

9. **C05:168 w12, (omitted de gnyis ka ma) — APPROVE (severity: INFO).** A positive definite mapping might reverse the source negation, or attach inside indefinite.

   Evidence: Final positive nges pa at Tibetan [120,127) owns full definite at English [249,257). It is within the scope of neither ripening itself nor its timing, corresponding to de gnyis ka ma; omitting a distributed negation is not asserting a positive proposition. No substring of either indefinite is taken.

   Recommendation: Retain the complete positive lexical predicate under the explicitly noted negative scope. Keep de gnyis ka ma omitted as distributed correspondence, not falsely labelled unrendered.

Null-claim audit

- **166:** `w4` is the gapped positive predicate after “or not”; `w19` is the gapped second “claim.” `w7a` has no separate future/modal word in the verse label. `m2` genitive, `p3/p6` topic markers, and `p5` subject-marking instrumental have grammatical realization but no separate English lexical words. Their notes make this distinction; none says the conveyed relationship is absent.
- **167:** `w6` is the gapped positive predicate in the binary contrast. `w16a/w22a` are explanatory repetitions omitted as lexical words after the respective “that is” / “which is to say” hinges. `p0/p5` are continuation particles with no separate lexical exponents. Auxiliaries and recast outline material are omitted without false-null labels.
- **168:** `p5` continuation is expressed through the explanatory discourse and colon; `p4` is final assertion. Both notes claim no separate English word. Distributed `de gnyis ka ma`, the enumeration’s final `gnyis`, and recast `zhes/bas` remain omissions, never claims of absent meaning.

Depth, occurrence, and supplied-English audit

- Every retained d5 is a word, established compound, or whole pronoun; no numerical clause or free temporal explanation is promoted to dictionary depth. The four non-null d7 members belong to their immediate technical d5 parents and own strictly smaller internal English spans.
- Both C05:167 temporal paraphrases remain d3. These phrase-level records permit contextual recasting without falsely claiming `lan grangs` independently means “life.”
- Repeated `las`, `nges`, `myong`, `gsum`, `tshe`, “definite,” “deeds,” and “experience/experienced” resolve to the intended clauses. The English offset checks in the focused challenges are from the actual generator resolution.
- Articles, “one’s,” “very,” “different,” “with results,” and recast connective material stay uncovered where no independent lexical licensor is asserted. C05:168 `kha cig → Some people` is licensed as a whole indefinite pronoun, not an invented member pairing.
- **Required planned landing action:** add `pages_c05` allowlist key **`c5p56/s166w6`** with exact licensor **`sogs`**. This is the only new gated d5/d7 function-word head. Its absence in the currently unlanded allowlist is not a spec defect; approval does not waive this landing gate.

Errata and provenance

All three errata lists are empty. C05:167 preserves both `BZHI`/`bzhi` and published “three”; neither is silently corrected or linked to the other. The note reports the observable mismatch and existing pending TODO, without declaring which source should be changed. C05:166 preserves `RIGS`/`rigs`; C05:170 is contextual corroboration, not proof of a spelling correction. No correction is smuggled into the reviewed notes as certainty.

Review produced by Codex from local read-only evidence. No paid API calls were made; historical Claude producer identities were not claimed. No reconciled specs, landed pages, campaign registry, allowlist, or errata register were edited.
