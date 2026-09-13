# C05:268–270 whitespace candidate

DONE: isolated implementation and evidence complete; root code/provenance review and the actual staged audit remain pending. This is the reused English-original author acting as a bounded tooling implementer, with no new semantic approval or fresh-context claim. Only this candidate directory was written. Shared v6, active preparation, semantic freezes, source/master files, repository and index were not changed.

## Exact exceptions

- `retained_print_separator`: five frozen aliases, six exact warning lines each: 1, 3, 5, 7, 9, 11, for P7:23–25 and C16:844–846. **The source WYLIE fields have no trailing spaces.** Root's initial statement that the whitespace came from the source fields was an incorrect inference. The additional ASCII space is the default separator before the producer's `"\nENGLISH:"` argument. Every alias requires whole original stdout, frozen and staged equality; full pinned corpus and SQLite reopens; complete original-object/SQLite-field equality; the exact prefix, complete UTF-8 field and one separator byte; and independently reproduced full six-record WYLIE/English print bytes. The actual pinned producer's line 23 is parsed as an AST: precisely the known six-argument call, no keyword overrides, default separator/end, and no `print` rebinding. Its full producer bytes must also equal the frozen producer copy. No producer helper is executed.
- Existing v6 `physical_bytes`: three whole ILL text aliases, 1,212 warnings each, match all 2,981,468 original bytes. Two raw XML aliases, one warning each, match all 50,562,402 bytes of the unique `word/document.xml` member freshly read from the pinned original DOCX. The original document/text provenance is checked through frozen extent records and original-to-copy mappings. The XML's optional ZIP-source assertion strengthens the existing physical-byte branch; it is no physical-filename exemption. All full source snapshots, the original DOCX, extracted member and full selected query rows are retained.
- `root_course_index_blank_eof`: exactly `landing-proof/course-index.stdout`, exactly warning line 24. It requires the unchanged 699-byte original capture and its known SHA-256, the exact 128-file before-whitespace manifest, external/banked manifest equality, exact original-path/size/hash records, successful execution metadata and its byte count, and an empty tail from line 24 through EOF. Both banked manifest and execution bytes also compare to their actual staged blobs. This is separate root evidence, never relabeled as semantically frozen material.

## Results

Final classification (`attempts/13-prepare-final`, exit 0) accounts for **141,409 warnings** and produces **16 exact pins**:

| Class | Warnings |
| --- | ---: |
| Existing whole original physical bytes | 114,913 |
| Canonical CSV CRLF | 22,821 |
| Whole ILL / exact original ZIP member | 3,638 |
| Retained print separator | 30 |
| Existing blank EOF | 4 |
| Existing diff context | 2 |
| Exact root course-index blank EOF | 1 |

`special-proof/proof.json` separately records real read-only `git show` commands and exact staged blobs for all ten initially unclassified aliases (3,668 warnings). `prepared/special-witness-proof.json` contains the final five full source-query/AST proofs and root evidence proof. Pin generation compares every cited working file to its staged bytes; it does not claim to replace the final fresh `git diff --cached --check` audit.

Final tests: **16/16 witness tests**, exit 0 (`attempts/14-final-witness-tests`), plus **1/1 complete candidate audit-dispatch test**, exit 0 (`attempts/15-final-audit-dispatch`). Faults cover corrupted staged/frozen/source/producer data; changed prefix/field/separator; missing, duplicate, extra or moved warning lines; changed source corpus and SQLite rows even with refreshed file pins; a changed producer with both pin and frozen bytes refreshed; missing witnesses; changed/duplicate ZIP members; physical corruption/ranges; root output, manifest, execution, banked copies, path and warning corruption. Literal expected print framing is constructed independently in the fixtures. The dispatch test uses real evidence files with a test Git transport, succeeds on the six exact warnings and rejects a corrupt staged file before writing a proof. It creates no repository/index. Actual current `git show` verification is separate and real.

`attempts/17-final-verification` exits 0, confirms final code is the tested code, all authoritative copied inputs remain byte-identical, all classifications/counts agree, and the shared helpers remain unchanged. These are tooling tests, not reruns or substitutes for the campaign's source/canonical/semantic/test gates.

## Preserved failures and limits

`inputs/` preserves root's failed classification and the complete 44,112,924-byte initial whitespace diagnostic (SHA-256 `ba7343bb6a19e47b73e66c2922e208db4fa079a0a8a7030236a6c20e282cd186`). Attempt 01 runs the actual v6 pin-kind branch AST and fails on the new formatter class, as expected. Attempt 04 preserves the initial full candidate's failure on the root-owned log absent from the semantic freeze; its full output/evidence trees are retained under that attempt with explicit relocation records. Root then authorized the separate root-log class. Attempt 05 proves that merely pinning a changed producer was insufficient; its new fault fails before the AST check and passes afterward. Attempt 16 preserves a mistaken verification expectation of producer line 22; the actual saved AST/source identifies line 23, corrected in attempt 17. A rejected atomic `apply_patch` attempt is recorded separately. Initial wrong-path read produced no edits. No failed attempt was erased.

Final test fixture trees are retained under `test-fixtures/`; earlier test versions and every actual command/stdin/stdout/stderr/exit are in `attempts/`. Earlier temporary test fixture trees were cleaned up, so those attempts preserve exact runnable test sources/streams rather than claiming retained trees. The duplicate-ZIP test deliberately emits Python's duplicate-member warning, retained in stderr.

The final full staged audit has **not** been executed. Root must review the exact candidate and then run it with a fresh output directory. The current input corpus/SQLite and original documents remain read-only; no spelling, whitespace or source bytes were normalized. No broader root-log, hash-only, physical-name or source-field exemption was added. The source and ZIP proofs concern bytes/provenance only and give no alignment or lexical judgment.

## Candidate interface and files

The audit keeps the page/first-segment/freeze-name arguments and adds an output-directory environment override. Its directory must not exist. Root's intended post-review invocation is:

```sh
PYTHONDONTWRITEBYTECODE=1 \
WHITESPACE_AUDIT_OUTPUT='/Users/adamderickandrade/Documents/ChatGPT/Geshe Michael Roach Tib _ Eng Alignment/campaign-artifacts/C05-268-270/whitespace-audit-root' \
python3 -B '/Users/adamderickandrade/Documents/ChatGPT/Geshe Michael Roach Tib _ Eng Alignment/campaign-artifacts/whitespace-candidate-270/audit_staged_source_whitespace_candidate.py' 90 268 freeze.json
```

Do not use the default output inside this now-frozen candidate. Root can preserve the successful new audit outputs afterward; no current shared helper was installed. The preparer is write-once and has already produced the final `prepared/` artifacts. Keep historical versions and consumption claims intact.

Review `witnesses.py`, `prepare-whitespace-pins.py`, `audit_staged_source_whitespace_candidate.py`, their baseline diffs, both test files, `prepared/review-whitespace-exact-pins.json`, `prepared/special-witness-proof.json`, and `candidate-freeze.json`.

Final SHA-256 values:

- Witness verifier: `d0b4091cd61ea0a1ab1665d535985d0f5edc451f9a1e56b449acc57cfdebb1d3`
- Pin generator: `1b10cde739ef529bd990c150405d0dbd9440ba6a00763904d7c618c1819d36c7`
- Audit candidate: `11cf8a4120fae57d289e9c2ab97520fe6cc972b3bc64efabc7b353e4a01f8d3c`
- Final exact pins: `a91eba745a70f2391f95e81a9809776b45b766fbacc13d436eb1086e85da36c2`
