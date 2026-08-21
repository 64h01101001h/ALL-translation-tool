# STEWARDSHIP ENGINE — LEDGER
Per docs/STEWARDSHIP_PROMPT.md. The data hierarchy governs: human
judgments outrank computed data.

## S1 — THE MANY-HANDS SURFACE (closed 2026-08-20)
Hunted the proposals store as the shared-Dropbox reality it lives in.
- **S1-01 · P0 · FIXED.** save() truncated and rewrote proposals.tsv —
  last writer wins. A proposal filed on one machine while the authority
  ruled on another could vanish into a "conflicted copy" nobody reads.
  save() now reload-merges first: rows with ids we have never seen are
  preserved; our own ids stay authoritative (single-authority model
  rules same-id edits). A never-loaded store can no longer wipe the
  queue (the smoke test relied on exactly that wipe and was corrected
  to an explicit file reset).
- **S1-02 · P1 · FIXED.** load() was blind to Dropbox "conflicted copy"
  siblings — forked queue history was invisible. Now: siblings are
  detected, their UNSEEN rows join the queue (absorbed), same-id rows
  that differ are COUNTED but never guess-merged (main file wins; the
  copy stays on disk for a human). The Approval pane banners the event:
  file names, rows absorbed, rows diverging.
- **S1-03 · P1 · FIXED.** Proposal ids were kind-date-count — two
  machines proposing the same day collided, and a ruling addresses rows
  BY id. The proposer's initials now join the scheme.
- **Proof**: a battery drill fabricates a conflicted copy with one new
  and one diverged row and pins detection/absorption/divergence/
  main-wins; battery run twice back-to-back to prove the drill cleans
  up after itself (first version didn't — caught and fixed).

## Q:T2 — SANITIZED BATTERY (closed 2026-08-20)
Full ASan+UBSan build, all 59 suites: ONE failure — a heap-use-after-
free in abbr_smoke itself (byWylie pointers held across a second
load(); reallocation dangled them). Test re-fetches; the lifetime
contract is now documented in abbr.h. App call sites verified: loads
complete before queries. Sanitized battery green.
