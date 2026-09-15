# Reuse the existing accepted v1 alignment interface

This is a packaging contract for new batches. It supplies no linguistic judgment,
acceptance or permission to change sources. Existing frozen batches keep their
actual schemas. Failed or pending reviews must retain their actual verdicts.

For an accepted new three-segment batch, use the ordinary through318 v1 DICT
interface consumed by the existing shared closure. Do not invent a per-batch
field name, a future baseline, or another verification program.

Keep reconciled `SEQ/spec.json`, `body.html`, `report.md`, `errata.json`;
preserve original-proposal dispositions, final rationales and omissions in
structured files. `reconciled/freeze.json` has schema
`c05-independent-reconciliation-freeze-v1`, `source_origin_git_commit`, ordered
`segments`, integer `total_spans`, `total_nulls`, `total_word_pairs`, `total_d7`,
and a `files` dictionary of relative paths to `{bytes,sha256}`. Count nonnull d5
as word pairs; do not confuse it with total d5. Every native claim must identify
the real executed command, input, output, error stream and exit.

The independent semantic package uses `reviewed-final/SEQ/` for exact accepted
spec/body copies, plus `review.json`, `review.md`, `resolved-spans.json`,
`original-to-copy-manifest.json`, `bank-ready-errata.json`,
`span-head-allow-needed.json`, and `freeze.json`.

Only after actual independent acceptance, both semantic metadata files have
`verdict`, `spec_verdict`, `quality_verdict` equal to `APPROVE`, and
`open_findings: []`. Both record `source_origin_git_commit`,
`later_supplied_acceptance_baseline`, `later_baseline_accepted_through`, the four
measured totals above, and `reconciliation_freeze_sha256`. Source origin and
acceptance may be the same actual commit; the inherited field name does not
assert that there was a later-baseline event.

`review.json` schema is `c05-independent-semantic-review-v1`; ordered segment
rows contain `seq`, `span_count`, `null_count`, `word_pair_count`, `spec_bytes`,
`spec_sha256`, `body_bytes`, `body_sha256`, `generator_exit`, `resolver_exit`.
Add explicit native evidence paths rather than making a consumer guess.
`freeze.json` schema is `c05-independent-semantic-review-freeze-v1`; `files` is
a DICT and `file_count` is its exact length. Exclude the freeze itself and
complete native captures before freezing. Do not make a success record refer
to its own still-running recorder.

Resolved span objects use ordered keys
`seq,id,d,tib,eng,tib_range,eng_range`, matching canonical actual ranges.
Copy records contain `original,copy,bytes,sha256`; immutable Git originals also
contain `original_git_commit,repository_relative_path`. Keep complete material
source/query/master evidence once where possible; cite exact original
identities and confine copies to the current role. A hash proves identity,
not that a model read or understood the evidence. Record actual reading scope.

Existing path for empty policy additions: semantic acceptance v2, landing v2,
landed semantic acceptance v2, registration v7 with explicit freeze pin,
canonical builds, relevant tests, retention, reproduction, compact bank,
whitespace v4/v12, shared base Closure. Existing generic policy requires
`bank-ready-errata.json=[]` and
`span-head-allow-needed.json={"pages_c05":{}}`. These are method limits, not
instructions to drop findings. Nonempty policy additions require the existing
appropriate reviewed path or a narrowly reviewed extension; never conceal a
finding to fit an interface. Do not reuse Q's hardcoded receipt/boundary profile
as though it were universal.

Root input/intake/disposition are separate factual receipts. Base Closure expects
`PASS_ROOT_INPUTS`, actual `baseline`, `source_origin`, `root_generator_runs:0`
and `verified_native_generations:9`. Use this only when the actual receipt scope
supports every value: six successful proposal inputs and three reconciled
inputs verified from their native evidence, without new generation by that
input verification. If execution differs, record the truth and resolve the
method contract; never relabel a run. Root must independently read all material
decisions and current source evidence before semantic acceptance.

Reuse approved mechanics and successful unchanged evidence. Review changed code
when material; for routine new data/configurations, root checks the exact
configuration and the existing validator runs its gates. Do not commission a
new configuration-review program or copy old fixture trees for each page.
Independent proposal angles and the independent semantic skeptic remain
mandatory. Preserve GMR wording, provisional labels, failures, uncertainty
reasons, source hashes, measured coverage and integration limits throughout.

Select disposition records by explicit source identity: sequence, angle and
original span ID, or sequence, angle, field and index for grouped observations.
Verify the complete original value at that key. Do not assign linguistic
decisions from loose substring matches in prose: an unrelated word can contain
the letters of a particle and receive the wrong rationale. Review changed
dispositions before freezing. This changes no source or accepted judgment.
