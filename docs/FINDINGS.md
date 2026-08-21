# FINDINGS — the inbox
*(S6: one file where every field finding lands, and the promise that
every entry gets a disposition in a ledger. File from the app —
Help → Troubleshooting → File a Finding — or by hand below. Campaign
items use their TEST_CAMPAIGN.md IDs.)*

Disposition stamps: FIXED(commit) · RULED(where) · DEFERRED(stamp) ·
NEEDS-INFO(question back to the filer).

## Standing: Adam's outstanding verdicts (never silently expire)
- The sems can card probe (BCAS report follow-up) — awaiting verdict.
- Card format letter A/B/C — B shipped in slice form; formal verdict open.
- GUI layout 1/2/3 — the band ≈ layout 1 shipped; formal verdict open.
- Campaign findings by ID — campaign run pending on the RC (#20).

## Inbox
<!-- newest first; the app's File a Finding appends a template here -->

### 2026-08-21 · design-wing heuristic pass · Team comments dialog
(first combined act of the installed skills: design-critique/Nielsen
lenses on the newest surface, findings → engineering flow → verify)
- **H1/H9 violation:** with no identity set, the Add button LOOKS
  active but silently no-ops (the caption hints, the button lies).
  DISPOSITION: FIXED — Add disabled with explanatory tooltip until
  an identity exists.
- **H5/wording:** the dialog never states that comments are permanent
  (append-only); only the band tooltip says so. DISPOSITION: FIXED —
  the permanence line now lives in the dialog itself.
- Append-only/no-delete: RULED (matches the rulings-record ethos;
  permanence now stated where it applies).

### 2026-08-21 15:00 · FIELD CRASH (Adam) · card layers dark → toggle → SIGABRT
Crash report: uncaught C++ exception from allcore::Spine::corpusSearch
(__cxa_throw → terminate), 15:00:33 — exactly while press step 6c
rsync'd the data folder UNDER a live instance Adam had reopened
mid-press. The yanked sqlite file explains BOTH symptoms: layers went
dark (queries failing) and the corpus toggle then died on the throw.
DISPOSITION: **FIXED, twice over, same hour** —
1. All 15 public Spine query methods now degrade to empty on any
   internal error (never abort; stderr notes the suppression); the
   exact field event is fossilized as spine_resilience_smoke
   (suite 70: healthy copy answers → file yanked+garbled under the
   open handle → five hammer rounds → alive and empty).
2. The press installer refuses a live target: any running instance is
   politely quit (osascript, 20s grace, then stopped) BEFORE rsync
   touches /Applications; step 8's relaunch brings the new build back.
Blindspot class: *the installer was only ever tested with the app
closed — the human habit of reopening mid-press was never modeled.*
