# UX AUDIT — PHASE 2 LEDGER (the lived-experience audit)
Per docs/UX_AUDIT_2_PROMPT.md. Findings carry: ID · severity (P0 data-loss/
dishonesty · P1 blocks-a-journey · P2 friction · P3 polish) · repro · status.

## W2 — FAILURE-PATH UX (in progress)

### Dependency inventory (what can break)
spine db · release json import · 84000 TM gz→FTS build · teaching indices ·
reference dicts · StarDict user dicts · verb-stems TSV · concordances ·
BDRC network (search + IIIF folios) · FTP/FTPS/WebDAV/SFTP connections ·
Dropbox official root (catalog staging) · OCR models · QSettings store ·
caches under data/extracted.

### Verified-good failure surfaces (spot-checked, honest)
- BDRC search: list shows "BDRC unreachable: <error>" (app/main.cpp:7211).
- Folio scan fetch: warning box "Could not fetch the folio image" (7585).
- Follow-along folio fetch: caption "image fetch failed (<error>)" (11646).
- Teaching index absent: card carries a labeled amber notice with the fix.
- Missing data root: designed ask-once dialog (findDataRoot).

### Findings
- **W2-01 · P1 · CONFIRMED by sabotage.** Cold start with no settings and no
  findable data root blocks on a modal "Locate the data folder" dialog BEFORE
  any window exists, and the dialog speaks jargon ("build/hgm_spine_…db").
  Repro: copy the .app to a scratch dir, HOME=<scratch>, launch — 45 s, zero
  windows/screenshots rendered. Under --screenshots/selftest this hangs the
  harness itself. Status: OPEN (fix: never-modal under harness; W1 will
  propose the human first-run).
- **W2-02 · P1 · CONFIRMED by code.** findDataRoot's cancel path returns
  QDir::currentPath() with no visible marker — the app then runs "empty"
  with per-pane symptoms and no single banner saying data was never located.
  Status: OPEN (fix: persistent degraded-state banner naming the missing
  root + a "Choose data folder…" action).
- **W2-03 · P2 · CONFIRMED by code.** Folio-scan cache write (7594):
  `if (f.open(WriteOnly)) f.write(...)` then `done(out)` unconditionally —
  a failed write silently hands downstream a nonexistent cache path.
  Status: OPEN.
- (Silent open-fail triage of 30 detector hits: running — table lands here.)

### W2/W9 write-fail batch (triage agent's ranked list) — FIXED 2026-08-20
Shared voice: warnWriteFail() — names the file, the reason, and that the
screen state is NOT saved; harness runs log instead of blocking.
- W2-04 P0 saveLinks (Align): every link silently unpersisted → warned. FIXED
- W2-05 P0 saveGlossaryEntry: card showed the gloss as taken from memory
  while the file write failed (actively deceptive) → warned. FIXED
- W2-06 P0 InputPane::save combined export (typed transcription work). FIXED
- W2-07 P0 catalogRosterSave callers: add/revoke appeared to succeed while
  the access-perimeter file was unchanged → result checked, in-memory
  change ROLLED BACK, warned. FIXED
- W2-08 P0 OcrPane::saveOut (expensive OCR results). FIXED
- W2-09 P1 exportPairs (Align export after a chosen destination). FIXED
- W2-10 P1 writeApprovedExport (authority's export; caller discarded the
  return) → warns at the write site. FIXED
- W2-11 P1 witness-hunt export message rendered errors in success green →
  color now follows outcome. FIXED
- W2-03 folio-cache phantom path → done() only on successful write. FIXED
- W2-01 harness cold-start modal → --screenshots/--selftest/--sweep never
  see the ask-folder dialog. FIXED (W2-02 degraded banner: OPEN, lands
  with W1's first-run proposal)

SILENT-ACTION queue (next wave): 4525 OverlayPane::openFile ·
28406 translator survey · 22511 InputPane::compare · 21297 importHyp ·
21093 AlignPane::openFile · 19699 downloadAndInstall.
