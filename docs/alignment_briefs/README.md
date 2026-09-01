# Alignment campaign agent briefs

Promoted out of the session scratchpad 2026-09-01, for the same reason
wrap_alignment_page.py was: the pipeline that produces pages must be
reproducible from a clean checkout, and until today the reconcile and refute
briefs lived only in a workflow script inside one session's state.

- `RECONCILE_BRIEF.md` — the reconciler merges two (or three) analysts'
  proposals into one spec, runs the REAL generator until it exits 0, and writes
  spec.json / body.html / errata.json / report.md to a private directory.
  **The deliverable is the file that exited 0, never a returned object** — the
  2026-08-28 batch lost `eng_order` to a StructuredOutput schema and reported
  three passing specs as REFUSED.
- `REFUTE_BRIEF.md` — every erratum a reconciler keeps goes to an independent
  skeptic instructed to refute it, defaulting to refuted. Verdicts are a JSON
  list, one per claim.

Model policy (Adam, 2026-09-01): propose on Claude Opus 5; reconcile and
refute on Claude Fable 5.1. Every batch records its producing models in the
campaign log heading.
