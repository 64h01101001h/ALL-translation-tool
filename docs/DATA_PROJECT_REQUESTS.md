# Standing requests to the HGM data project (Cowork sessions)

*Consolidated 2026-08-10. This repo consumes the data project's
releases; these are the items the app side is waiting on or offering
back. Carry this page into the next data session.*

## Offered back (ready for ingest)

1. **Prenasal defect candidates** —
   `data/pron_colloquial/prenasal_defect_candidates.tsv` (30
   letter-level diffs found while deriving the m-forms): likely data
   defects in the master's card pronunciations; each row cites the
   entry.
2. **Approved-terms channel** — when the authority approves
   dictionary/corpus proposals in-app, the export
   (`Approval → Export approved dictionary candidates`) produces a
   cited TSV for release ingest. Format: kind, wylie, value,
   evidence, approver, ruled.
3. **Four honorific source banks** (facts with citation, mined
   2026-08-09/10): Hackett verb lexicon (`data/hackett/`), Choephel
   Manual (`data/manual_authentic/`), Goldstein Essentials
   (`data/goldstein/`), MST Appendix 3 (`data/mst/`) — candidate
   register material once ruled on; also the 31 Wilson-class
   causative pairs (`data/hackett/wilson_class_pairs.tsv`) for the
   verbs database.

## Requested from the data project

1. **More "(…register)" gloss annotations.** The Review pane's
   register warning fires on glosses carrying "(… register)" markers
   — currently only bsod nams has them. Every annotated entry makes
   the oversight pane sharper. Candidates: the single-register terms
   GMR has flagged in courses.
2. **Status-vocabulary confirmation** — the release validator warns
   on new status strings; current known set includes "hgm-attested
   (not in Great Dictionary)". Please keep the app side posted when
   new statuses are minted so the validator's known set stays true.
3. **Verb tense coverage** — spotVerb marks verbs "unverified" when
   the entry lacks tense evidence; any tense enrichment pass on
   high-frequency verbs directly improves Trainer/Draft guidance.
4. **The LC battery's pair-selection script** — the 92.2%/4,582-pair
   Sanskrit battery is not reproducible from banked artifacts (noted
   in validate_engines output); if the script survives in a data
   session, bank it.

## Blocked on people (not the data project)

- 84000 license email (drafted, awaiting Adam's send)
- Mixed Nuts style guide + input-center raw samples (roadmap item A)
- Catalog tier ruling (sidelined with the cataloging tool)
