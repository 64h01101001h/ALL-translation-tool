# Independent semantic review: c5p103, C05:307–309

Read ../semantic-review-protocol.md first, then the worktree AGENTS, alignment
spec, RECONCILE_BRIEF and REFUTE_BRIEF. The shared protocol is binding; actual
Codex producer roles supersede historical model names. This task has no permission
to change source/master/repository files, commit, spawn agents, use paid APIs or
perform native/device/browser actions. Write only this batch's semantic-review/
directory. SQLite mode=ro and PYTHONDONTWRITEBYTECODE=1 throughout.

Read every final reconciled spec/report/body, both original proposal angles and
all material original evidence. The accompanying semantic-input-review-package.txt
contains full source/spec context and proposal-to-reconciled diffs. Read that
whole package; root will not dispatch this review until it exists. Root's proofs
are mechanical evidence only. Check source fields, lexical attribution, occurrence
labels, omissions and every seven-depth correspondence independently.

Snapshot the original proposal inputs, including all superseded versions and
failed attempts, into semantic-review/proposal-inputs/. Keep complete current
individual specs/bodies/evidence under semantic-review/reviewed-final/<seq>/.
Include a manifest mapping each preserved original to its copy with hash and
byte count. Preserve source/witness bytes necessary to reproduce material claims.
The landing helper copies .json, .md, .html, .stderr, .stdout, .log, .txt, .bin and
.exit. Preserve other required input extensions as exact-byte .txt or .bin copies
with the original-to-copy mapping explicit (for example helper.py.txt). Do not
include Python caches. Do not silently overwrite historical evidence on re-review;
preserve the old copies and identify the new final hashes.

For every actual span, independently replay the canonical resolver and real
generator; compare exact complete output with the saved body. Produce the shared
protocol's exact seven-key resolved-spans.json in spec order. review.json must
contain verdict, open_findings, and a segments LIST for the current reviewed
files. For each segment record seq, span_count, null_count, word_pair_count
(non-null d5), spec_sha256, spec_bytes, body_sha256, body_bytes and generator_exit.
Use distinct SPEC and QUALITY verdicts in review.md. Approval is machine review,
not human editorial acceptance. If correction is needed, name exact span IDs and
minimal changes; do not edit the reconciler's files.

If a head allowance is needed, write span-head-allow-needed.json as a mapping:
{"pages_c05": {"c5p103/s<seq><span-id>": "exact Tibetan licensor and reason"}}.
An empty mapping is valid. Require a real source-specific licensor. Supply
bank-ready-errata.json, [] when none; every retained candidate needs independent
five-ground refutation, actual query/count proofs and complete physical original
witness files, with no publication/ingestion independence inferred from digital
identity. Preserve already registered classes and original source words.

No page has been accepted merely because a generator passes. Review all evidence
and current files before issuing APPROVE. Final reply: verdict, counts, open
findings, generator proof, errata result and report path; details stay on disk.

Mutable governing files may change when root registers preceding reviewed pages.
If you preserve any such file, record its original_git_commit and
repository_relative_path in the manifest, and prove the copied bytes equal that
Git object. Do not later compare a historical snapshot against a changed live
working file. Preserve all versions; root verifies copies before registration.
Include total_spans, total_nulls and total_word_pairs as well as per-segment data.

Use original-to-copy-manifest.json as a LIST of records with exact keys
original (absolute path), copy (relative to semantic-review), sha256 and bytes.
For historical repository snapshots additionally include original_git_commit
and repository_relative_path; copied bytes must equal that immutable Git object.
Preserve helpers with unsupported extensions as exact-byte .txt copies.

## Efficient evidence preservation, with unchanged review coverage

For identical immutable evidence, preserve one exact-byte copy and map every
original path to it in original-to-copy-manifest.json. A content-addressed copy
may live at proposal-inputs/evidence/<sha256>.<ext>. The helper
../copy_evidence_once.py (in campaign-artifacts, resolve from the batch parent)
implements this and has a real-file byte/hash verification record beside it.
Inspect it before use. It only copies evidence: you still choose all required
inputs, independently reopen sources and verify every claim. Keep current and
historical core proposal spec/body/report/errata and per-segment final interfaces
readable under their expected directories; preserve all attempts and decisions.
Mutable repository governing snapshots still need explicit Git commit/path pins.
Preserve the exact helper bytes as .py.txt if used, and compare all original/copy
bytes yourself. The manifest may map multiple distinct original paths to one
identical copy. No generator run, semantic check, or fidelity suite is removed.

The shared proposal protocol includes an append-only throughput refinement.
These proposal readers were dispatched after that refinement. Preserve the actual
protocol version each reader consumed; earlier policy history is reference only.
Do not imply that a reader consumed a version it did not actually read.


## Evidence packaging preflight before freezing

Before issuing the final review, verify every original-to-copy manifest target
exists, exactly matches its original bytes/hash/size or immutable Git object,
and has an extension included by land_reviewed_page.py. Inspect the actual
helper's whitelist. Required .py evidence must have an exact-byte supported
copy such as .py.txt. Validate all manifest targets before freezing, preserving
failed attempts and earlier interfaces if a correction is necessary. Source
bytes, accepted semantic content and existing review gates are unchanged.
