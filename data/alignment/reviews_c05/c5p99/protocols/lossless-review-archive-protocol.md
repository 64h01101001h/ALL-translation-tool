# Future large review archives — lossless storage only

Observed2026-09-12: laptop filesystem has about35GiB available. Existing
accepted/reviewed files, originals, patches and manifests are retained. The
helper compress_review_evidence.py never overwrites or deletes its source.

Future newly generated large oneline/stat/U10 packages and binary patches may
be preserved as gzip containers, with both compressed and complete raw byte
counts/SHA-256 pinned. The unchanged actual review-package script must still
generate the complete package; no summary, diff section or original is omitted.
Generate only into a newly allocated private temporary file, preserve failed
outputs, compress, verify every decompressed byte against that temporary original,
and record the generating command/exit/current Git inputs and artifact hash.
Only a newly created temporary original may be removed after its complete
lossless container and proof exist. Never delete an existing frozen/reviewed
path or change an earlier manifest to disguise its removal. If a task needs
the raw file, decompress to its own new temporary path and verify the raw hash.

Readers must distinguish archive SHA from uncompressed-content SHA, decompress
before parsing full diffs, and prove both. Read-only apply --check must receive
the actual complete raw patch bytes. Semantic specs, source copies and other
landing interfaces retain their existing formats; this change is only for
large external checkpoint packages/patch archives. No source, generator,
semantic, fixture, retention, fidelity or acceptance gate is removed.

Real-file verification: continuation-through261-review-package.diff remains
81464313bytes at its accepted SHA. The helper produced13013422bytes gzip,
restored all81464313bytes exactly and independently reread the unchanged
source. Full proof is integration/continuation-through261-review-package.lossless-proof.gz.proof.json.
This demonstrates lossless storage, not overall runtime speedup. The sample
original is retained, so the test itself did not reclaim disk space.
