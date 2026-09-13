# Independent semantic review of a three-segment C05 batch

The dispatch names the batch directory, page and segments. Read the worktree's
AGENTS.md, ALIGNMENT_LAYER_SPEC.md, RECONCILE_BRIEF.md, REFUTE_BRIEF.md, and the
reconciled reports/specs/source evidence. Required reference access was verified.
Historical Claude role names are superseded by the authorized Codex handover.
Identify the actual producer. Do not delegate, commit, mutate repository/source
files, access paid APIs, or perform native/device/browser actions.

Read the original source independently from the worktree spine with SQLite URI
mode=ro. Check every proposed span, including all nulls, phrase recasts and d7
members. Independently rerun each real generator and reproduce exact body bytes.
Run Python with PYTHONDONTWRITEBYTECODE=1. Passing the generator proves fidelity,
not the correctness of the Tibetan-English correspondence.

Challenge overcapture, supplied participants/articles/auxiliaries, unsupported
dictionary units and inflated phrase boundaries. Check lexical coverage as well
as omissions: retain supportable useful words/compounds, but never invent English
or force a disputed mapping. d5 is a word/established compound; d7 is a strictly
tighter unique member immediately after its d5 parent. d3 requires a defensible
complete phrase; a broader phrase cannot hide a false mapping. A null asserts no
separately alignable exponent. Proven grammatical fusion may be null with an
explicit note; unresolved/distributed content is omitted with reasons.

For every repeated Tibetan and English string, verify the actual resolved source
ranges and intended occurrence, not just substring membership. Use the real
generator resolver and its English ordering/member handling. Check HGM glossary
and corpus evidence yourself for contested terms; comparative dictionaries never
supply GMR English. Preserve source wording and all PROVISIONAL labels.

For each surviving erratum, independently check ALL five refutation grounds,
every quoted source/witness field and every numerical claim. Default to refuted
when uncertain. Digital variants/reingests do not establish independent
publication. Do not file already registered classes, unrelated source variants,
or claims quoting another segment as their found text.

Write only within this batch's `semantic-review/` directory. Required outputs:

- `review.md`: APPROVE or NEEDS_CORRECTION, full material findings/dispositions,
  source evidence and limits. Machine approval is not human acceptance.
- `review.json`: measured per-segment span/null/word-pair counts, generator exits,
  final individual spec/body SHA-256 and bytes, verdict and open findings.
- `resolved-spans.json`: ordered records for EVERY accepted/proposed span, with
  exact keys seq,id,d,tib,eng,tib_range,eng_range. Ranges are zero-based half-open
  offsets in original wylie/English; eng_range is null when eng is null. Preserve
  spec order. This exact tuple/range identity is checked at landing.
- Evidence files as needed, including complete errata refutations and final
  `bank-ready-errata.json` (empty list if none). Any witness file needed to support
  a retained erratum must be included here so the landing helper commits it.

Flag corrections with exact segment/span IDs and a minimal proposed change; do
not edit reconciled specs. Root owns any correction and will request scoped
re-review of the final corrected files. Do not issue approval until the files
actually reviewed are sound. Preserve old review evidence when re-reviewing and
refresh hashes/ranges so accepted artifacts describe the final spec exactly.

Final reply: verdict, total spans/nulls, unresolved findings, generator proof,
errata result, and the review path. Keep the detailed record on disk.
