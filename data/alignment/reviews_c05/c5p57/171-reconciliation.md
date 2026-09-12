# C05:171 — fresh Codex reconciliation
Historical inputs: handoff-check/recovered-c05/b19/prop_171_tib and prop_171_eng, each containing spec.json, body.html, errata.json and report.md; both independently oriented proposals were usable.
Provenance: the C05 ledger assigns the old PROPOSE work to Claude Opus 5; the recovered specs themselves carry no provider metadata. This reconciliation and the new checks are fresh Codex work, under the user-authorized continuation; no paid external model API was called.
Source: queried independently with SQLite mode=ro from campaign-worktree/build/hgm_spine_v27_2.db; all three original rows are preserved in ../../source.json. No source, hgm_gloss, registry, or landed page was edited.
Old proposal reruns with the CURRENT generator: Tibetan-first EXIT=0, 5691 bytes; English-first EXIT=0, 5522 bytes.
Hardest ruling: mi byed → does none and the elliptically resumed myong mi byed → Neither are d=3 predicate correspondences; yang min → Neither is also d=3. This avoids false standalone neither equivalents and avoids marking expressed negative predicates as null.
Relative gang las → for which and locative/emphatic gzhan du'ang → in others even are honest d=3 phrases, not d=5 dictionary entries or a bound 'ang mislabelled standalone.
Kept inflected dge ba'i → of virtuous with d=7 dge ba → virtuous and 'i → of; der → At that with d=7 de → that and r → At. Every member is unique and strictly contained on both sides.
Kept dictionary compound mi brtan pa → unstable (current HGM glossary attested) with negative member mi → un and positive brtan pa → stable, both explicitly subword. The first brtan pa owns the independent stable.
myod → experience quotes the original erroneous-looking token rather than silently correcting it; the adjacent myong mi byed and commentary C05:173 establish the contextual correspondence. The spelling question is a separate probable erratum.
Independent omission check: only two genitive 'i suffixes and punctuation remain outside Tibetan spans. All content words are linked, individually or in the five d=3 phrases; the three nulls are the first locative na and two topic ni markers.
Function-word licensors: no d=5/d=7 span has a prohibited supplied head. Every one is thams cad; of is bound 'i; At is bound r in der; member that is de; un is negative mi. Articles, has, each, to, he's and and in desire and peak remain unwrapped.
Errata: one PROBABLE/LOW candidate, skyes myod mi byed → skyes myong mi byed; retained for separate skeptical review. ACIP has MYOD, the next verse line has MYONG, and C05:173 explains this exact line with myong 'gyur and experience. No blockprint was consulted.
Corrected historical evidence: myod has TWO corpus rows, C05:171 and TCS10:338; the latter has myod yul and later myong yul in an unrelated passage. Current dictionary has no myod entry and lists myong tenses myong / myong / myangs/ myongs.
Witness queries for byis pa der skyes, myod mi byed, myong mi byed and A stable child does none return C05:171 only; no independent witness to this verse was found. Mid-sentence capitals are verse lineation; ACIP RTZ→wylie rts is normal conversion.
Existing-register check: docs/errata_register.json, docs/ERRATA_REGISTER.md, docs/BACKLOG.md and TODO.md; no matching new-locus record. Known converter, footnote/folio, heading-splice, row-spill and 1,500-character-cap classes were considered.
Final counts: 32 spans; 29 linked, 3 grammatical nulls; depths {3: 5, 5: 17, 6: 4, 7: 6}; 1 candidate errata.
Final command: PYTHONDONTWRITEBYTECODE=1 python3 tools/gen_alignment_page.py < spec.json (cwd campaign-worktree); actual stdout is retained identically in body.html and generator.stdout; stderr is empty.
```
EXIT=0
body.html = 6131 bytes
```
Verification scope: current generator acceptance and an independent cursor/omission check; semantic approval remains pending the separate skeptical reviewer.
