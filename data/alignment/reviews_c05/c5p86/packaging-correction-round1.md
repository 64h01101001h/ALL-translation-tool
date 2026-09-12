Evidence packaging correction, round 1: the landing extension filter excluded 11 preserved Python helpers. Their exact bytes now reside at .py.txt paths, and all 599 current manifest mappings were verified against their original files or immutable Git pins. The previous manifest, review.md, review.json, review-freeze.json and final-review-check.json are preserved under history/packaging-round1-before/. All 453 previously frozen evidence files retain their bytes, with the 11 helper path relocations and explicitly refreshed interfaces accounted for. Approved specs, bodies, notes, span decisions and SPEC/QUALITY APPROVE verdicts are unchanged. No source or generator checks were rerun. See packaging-correction-round1.json for the exact audit and changed/added/relocated file lists.

Changed current interfaces:

- `original-to-copy-manifest.json`
- `review.md`
- `review.json`
- `review-freeze.json`
- `final-review-check.json`

Added files:

- `governing/tools/build_alignment_layer.py.txt`
- `governing/tools/gen_alignment_page.py.txt`
- `history/packaging-round1-before/boundary.json`
- `history/packaging-round1-before/final-review-check.json`
- `history/packaging-round1-before/original-to-copy-manifest.json`
- `history/packaging-round1-before/review-freeze.json`
- `history/packaging-round1-before/review.json`
- `history/packaging-round1-before/review.md`
- `packaging-correction-round1.json`
- `packaging-correction-round1.md`
- `packaging-correction-round1.py.txt`
- `proposal-inputs/reconciled/256/assess-attempt01.py.txt`
- `proposal-inputs/reconciled/256/assess.py.txt`
- `proposal-inputs/reconciled/256/canonical.py.txt`
- `proposal-inputs/reconciled/256/collect.py.txt`
- `proposal-inputs/reconciled/256/decisions.py.txt`
- `proposal-inputs/reconciled/256/draft.py.txt`
- `proposal-inputs/reconciled/256/finalize.py.txt`
- `proposal-inputs/reconciled/256/generate.py.txt`
- `proposal-inputs/reconciled/256/replay.py.txt`

Relocated old paths (identical bytes at appended `.txt` paths):

- `governing/tools/build_alignment_layer.py`
- `governing/tools/gen_alignment_page.py`
- `proposal-inputs/reconciled/256/assess-attempt01.py`
- `proposal-inputs/reconciled/256/assess.py`
- `proposal-inputs/reconciled/256/canonical.py`
- `proposal-inputs/reconciled/256/collect.py`
- `proposal-inputs/reconciled/256/decisions.py`
- `proposal-inputs/reconciled/256/draft.py`
- `proposal-inputs/reconciled/256/finalize.py`
- `proposal-inputs/reconciled/256/generate.py`
- `proposal-inputs/reconciled/256/replay.py`
