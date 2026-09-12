# C05:169 — fresh Codex reconciliation
Historical inputs: handoff-check/recovered-c05/b19/prop_169_tib and prop_169_eng, each containing spec.json, body.html, errata.json and report.md; both independently oriented proposals were usable.
Provenance: the C05 ledger assigns the old PROPOSE work to Claude Opus 5; the recovered specs themselves carry no provider metadata. This reconciliation and the new checks are fresh Codex work, under the user-authorized continuation; no paid external model API was called.
Source: queried independently with SQLite mode=ro from campaign-worktree/build/hgm_spine_v27_2.db; all three original rows are preserved in ../../source.json. No source, hgm_gloss, registry, or landed page was edited.
Old proposal reruns with the CURRENT generator: Tibetan-first EXIT=0, 5038 bytes; English-first EXIT=0, 4944 bytes. Historical rec_169 was read as an unverified third artifact, not adopted as authority.
Boundary decisions: kept dictionary words nges pa (bare nges where actually written), sde pa → group, gzhan → Others; the plural marker dag has no separate English word and is the only morphological null.
Correspondence decisions: zhes → that is the explicit quotative complement; both la → but are adversative. The older reconciliation’s reliance on frequency of nulls does not override these visible exponents.
Hardest ruling: gnyis ka ma nges pa → definite in neither respect is ONE d=3 phrase. Neither gnyis ka nor ma is falsely equated with neither or nulled as if absent.
The second rnam smin is linked to ripen in not definite to ripen at all; the earlier should ripen is the English expansion of the occasion. Explicit English order resolves three English ripen occurrences against two Tibetan instances.
Independent omission check: only genitive 'i, final 'o and punctuation remain outside Tibetan spans. All content words have a non-null correspondence at d=5 or within the one honest d=3 phrase.
Supplied English stays free: school, the numbered deeds/respects framing, the extra should ripen, and They say there are. No arbitrary d=5 phrase or composed equivalent was added.
Function-word licensors: no d=5/d=7 span begins with the/a/an/and/or/his/our/your/I/you. Others is licensed by gzhan, both by gnyis ka; d=6 that by zhes and but by each la.
Errata: empty. Current-corpus searches by the Exemplist group and the matching Tibetan identify only C05:169 and C16:756; C16:756 is byte-identical in both columns and is a same-source re-ingest, not independent testimony.
Existing-register check: docs/errata_register.json, docs/ERRATA_REGISTER.md, docs/BACKLOG.md and TODO.md; no matching new-locus record. Known converter, footnote/folio, heading-splice, row-spill and 1,500-character-cap classes were considered.
Final counts: 26 spans; 23 linked, 3 grammatical nulls; depths {3: 1, 5: 17, 6: 8}; 0 candidate errata.
Final command: PYTHONDONTWRITEBYTECODE=1 python3 tools/gen_alignment_page.py < spec.json (cwd campaign-worktree); actual stdout is retained identically in body.html and generator.stdout; stderr is empty.
```
EXIT=0
body.html = 5155 bytes
```
Verification scope: current generator acceptance and an independent cursor/omission check; semantic approval remains pending the separate skeptical reviewer.
