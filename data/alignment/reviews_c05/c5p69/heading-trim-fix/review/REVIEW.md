SPEC: APPROVE
QUALITY: APPROVE

Checklist: 54/54 complete  
Incomplete: None

This is approval of the completed four-file engineering snapshot against base `4aa4aa6cce64fe2b5129ad666e8753a2947ea64e`. It is not approval of a later data commit, actual export regeneration, or whole-branch integration. No material findings survived review.

## Scope and evidence

The protected outcome is faithful dictionary display as the provisional bank grows: remove the clearly appended numbered course headings, allow only exact source-justified shortening, preserve existing phonetic extraction and the global exact-pair contract, and disclose both shortening paths accurately. Read the supplied complete U10 diff once; inspected supporting import initialization, old pronunciation AST, original committed bank/view, gate entrypoint and CTest registration to evaluate specific oracle, provenance, migration and wiring risks. No source, repository, index or generated output was edited; only this review directory was written. No delegates, builders, browser/device, native builds or paid tooling were used.

Read AGENTS and alignment spec; the two reviewer/verification skills in MAIN are byte-identical to the worktree copies read. All eight AGENTS reference roots were readable. The relevant policy is verbatim source preservation and honest display/provisional labeling; semantic alignment and unrelated existing exact-pair provenance behavior remain outside this task.

## Acceptance evidence

| Requirement | Implementation | Independent evidence | Result |
|---|---|---|---|
| Numbered heading boundary requires sentence punctuation, page, full ACI Roman-course heading and recognized Reading; preserve substantial content | `tools/build_dictionary_view.py:119-140` | Inspected all 11 complete migration source strings. Independent literal cut oracle locates the heading and preceding numeric page, preserving punctuation. | PASS |
| Only the 11 intended outputs change | Same transform and existing builder dispatch | Swept 30,964 valid links using original 4aa4 implementation against the fixed transform. Exactly 11 link changes and 11 removed/added display keys; 21,436 remaining keys retain all references. | PASS |
| Preserve prior behavior outside scope | Existing GLUED retained, earliest boundary selected | Original committed view yields 40 prior shortened rows: 10 heading outputs change, 12 other heading outputs remain, all 18 phonetic outputs remain. New C05:206 is the eleventh migration. | PASS |
| Preserve all phonetic output | `tools/build_dictionary_view.py:84-99,171-177` | Extracted original inline block directly by AST from base, avoiding a handwritten duplicate oracle; all 947 phonetic link outputs equal the new helper. | PASS |
| Every shortening needs nonempty refs and a valid same-Tibetan full source at each citation; heading depth and phonetic band must match | `tools/test_view_matches_layer.py:55-97` | Actual-payload corruptions reject empty/arbitrary/shorter/extended/invented English, no refs, wrong refs, extra wrong ref, wrong depth and translation placed in phonetics. Literal suite rejects phonetic output in a translation band. | PASS |
| Eliminate 40-row allowance without allowing arbitrary prefixes | `tools/test_view_matches_layer.py:36-39,87-95` | Original gate classifies 41 short rows and exceeds its cap. Fixed gate accepts 21,406 exact + 41 verified rows, zero invented, zero overlap. Fixed gate rejects exactly 11 old partial trims. | PASS |
| Preserve exact-banked path and pairwise phonetics/d5 disjointness | `tools/test_view_matches_layer.py:87-89,99-103` | Explicit independent fixture retains exact acceptance even without citations and counts identical phonetics/d5 pair overlap. New citation/depth enforcement is limited to shortening. | PASS |
| Guard shared transformation against correlated builder/gate acceptance | New literal test file; existing gate `__main__` | Fresh GREEN: 10 tests, zero failures/errors. Same final suite against original implementations: 11 assertion failures, zero errors. Removing shared numbered recognition or replacing shared phonetic helper with arbitrary first-word extraction causes assertion failures. | PASS |
| Existing CTest runs literal suite and real gate | `core/CMakeLists.txt:687-692`; `tools/test_view_matches_layer.py:128-136` | Registration points to unchanged CLI, which invokes the suite and refuses to proceed if it fails; successful suite invokes the real payload gate. No registration or test-count amendment needed. | PASS |
| Narrow rather than ship the rejected 16-row pattern | Numbered recognizer Reading witness | Inspected broad artifact: 16 changes. Independently proved all five excluded C03:151/C04:27,62,86,93 retain old output. Preserved RED/GREEN narrowing evidence is consistent. | PASS |
| Accurate visible disclosure without layout/schema drift | `tools/dictionary_view_shell.html:257-264`; builder disclosure comments | Diff changes only the authorized sentence: following headings or appended translations on phonetic lines are cut, full source retained, no composition/rewording. Count remains generated and covers both paths. | PASS |

Changed references: C03:131, C03:199; C04:37, C04:49, C04:117, C04:133, C04:168; C05:96, C05:118, C05:145, C05:206. All are depth 1. Full source, old/new text and exact independent cut offsets are recorded in `independent-migrations.json`.

## Quality and test decisions

The fix removes the count-based permission and arbitrary-prefix acceptance at their owning boundary. It extracts the existing pronunciation block into one helper without changing the canonical engine. Standard-library regex, mappings and unittest fit this bounded corpus policy; no generic mechanism or dependency was introduced. The original global exact-pair acceptance is a deliberate preserved contract, not a loophole newly introduced here. `any` source within each citation is appropriate because the reference is course/segment, which can contain multiple same-Tibetan links; `all` citations prevents an unlicensed second citation from being hidden by a valid first one.

ADD the ten literal regression tests: they exercise material isolated transformation and gate-policy behavior and remain in the normal CTest path. UPDATE the existing fidelity gate and disclosed copy. KEEP the existing exact-pair/disjointness checks and conservative GLUED behavior. No tests were deleted or quarantined. The shared transform risk is directly tested by mutation, not merely by running builder and gate with the same implementation. No browser check is needed for the static sentence-only shell change: selectors, markup, styles, behavior and layout structure are unchanged.

Independent review panel: None within this review seat, as expressly prohibited by the frozen task. This reviewer independently covered specification, source fidelity, oracle quality, failure behavior and scope; the controller's final whole-branch review is a separate integration boundary.

## Commands and artifacts

- `PYTHONDONTWRITEBYTECODE=1 python3 campaign-artifacts/integration/heading-trim-fix/review/verify.py`: exit 0; empty stderr. It reads original code and data with `git show 4aa4…:path`, imports the fixed snapshot without executing builders, compares transformations and grouping in memory, runs literal tests and twelve mutation checks, and writes only review artifacts.
- Additional focused in-memory command ran the final ten-test suite with original implementations: expected RED with 11 assertion failures and zero errors; verified the five broad-pattern exclusions. Results: `red-against-base.txt`, `additional-proof.json`.
- `verification.json`, `verify.stdout`, `verify.stderr`, `green.txt`, `mutations.json`, `independent-migrations.json` contain the directly generated evidence. Earlier implementer logs were inspected as supporting evidence, not accepted in place of these independent checks.
- `coverage-ledger.json` maps every required checklist item to evidence or an absent conditional trigger. One final closure pass checked verdict, scope, counts, evidence and hashes; no unresolved item remains.

## Reviewed SHA-256 fingerprints

| File | SHA-256 |
|---|---|
| `tools/build_dictionary_view.py` | `dee3188a3b490cf4aa1845a04b7dee0ccdd517470aff313f9a0d507454a1ebca` |
| `tools/test_view_matches_layer.py` | `4a277de95e65392b986dc57d3247d0cd230e55e734022b9eafd907ef42ca754b` |
| `tools/test_dictionary_view_trimming.py` | `3c030ad196dd0ba2a514b8ba6b1c78a2e9a4737b32d0bca75a55b0eb8a34fe3d` |
| `tools/dictionary_view_shell.html` | `3b0a2034daddf0b009896040b7df9c555a1d959eb92ed309395111e77838fb15` |

Source fingerprints also match the implementer proof: full bank `1982bff83157b4cb912c5207bfada69ba170bef8f4ef29c041da57ff57a39c46`; evidence bank `38d6f633ea44ad20a88647b7ffc87f583400d9dc6a742923631f15fb648e9ec7`.

Residual limits: the recognizer is intentionally conservative and does not claim universal heading recognition. Mechanical fidelity does not establish semantic correspondence. Actual regenerated HTML/CSV, seed-sensitive export changes, and whole-branch release gates are the controller's expressly separate integration work.
