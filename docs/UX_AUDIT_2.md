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

## W9 — STATE, UNDO, AND THE COST OF A MISTAKE (reconnaissance)
Verified-good: deletes route through QFile::moveToTrash (recoverable) in
both Files idioms; batch rename has full preview; sync folders previews
and never deletes; native save dialogs confirm overwrite; register
rulings append-only (QIODevice::Append).

### Findings
- **W9-01 · P2.** F8 (Commander keys) trashes the whole multi-selection
  with no count confirmation — Total Commander itself confirms F8. Fix:
  "Move N items to Trash?" when N>1 (single item stays instant, Finder
  parity).
- **W9-02 · P2.** "Add to my materials" silently QFile::remove()s an
  existing same-named file before copying (main.cpp ~19801). A second
  import of a revised file clobbers the first without a word. Fix:
  keep-both suffixing or an ask.
- **W9-03 · P1.** Data-release import does remove(dst) THEN copy(src) —
  if the copy fails (disk full, network drive), the OLD release file is
  already destroyed: an update attempt can delete the working release.
  Fix: copy to dst+".tmp~" then atomic rename over dst.

### §10 adversarial review of the W2 batch — verdicts and remediation
The independent reviewer CONFIRMED three bugs in the batch itself:
- **R1 · P0 · FIXED.** Failed roster bootstrap still granted admin
  identity and closed the dialog as success (phantom sign-in over a
  missing CATALOG_TEAM.tsv). Now: rollback + warn + return; typed
  fields survive for retry.
- **R2 · P1 · FIXED.** warnOpenFail could hang the scriptable --survey
  and --teachbench CLI modes on a modal. Both modes joined the
  g_harnessRun list; findDataRoot's hand-rolled flag re-parse replaced
  by the same single flag (two lists had already diverged).
- **R3 · P1 · FIXED.** Revoke-rollback blanket-restored "active" — a
  failed save of an ALREADY-revoked member would un-revoke them on the
  next successful save. Now restores the previous status.
Weaknesses accepted and fixed: modal-per-link spam (warnWriteFail now
warns once per failing path, then logs — Space-bar alignment cadence
protected); the survey dialog's own Save-as-Markdown was still silent
(same commit, 30 lines away — now warned); prose-coupled outcome color
(dead "unreadable" branch removed, startsWith sentinel); launch-time
restore now honors sess::path's no-modal contract (unreadable
remembered files restore as nothing, Overlay + Align). Noted, accepted:
folio/roster warnings use raw QMessageBox (unreachable under harness in
practice).

## W1 — COLD START (first findings; the sabotage rig keeps paying)
- **W1-01 · P0 · FIXED.** With no locatable data root the app CRASHED
  (uncaught std::runtime_error from the Spine constructor) — the Phase-1-era
  ask-folder modal had been masking a crash-on-launch for every
  moved-the-app-without-its-data install. New startup contract: locate the
  data or exit cleanly — an honest dialog explains what the data folder is,
  offers the picker, retries on a wrong pick, and Close quits with no crash;
  harness runs print FATAL + exit 2. Repro rig: scratch HOME + copied .app.
- **W2-02 · SUPERSEDED.** The planned degraded-state banner assumed an app
  that runs data-less; the spine is constitutive, so the correct surface is
  the locate-or-exit dialog above. (A partial-data state — spine present,
  layers missing — still shows per-layer honest notices, which Phase 1
  already built.)

## W5 — MEASURED ACCESSIBILITY (2026-08-20)
Method: WCAG relative-luminance ratios computed for every token pair in
actual use against kPaper (#FAF6EE).
Passing already: kInk 14.6 · kError 7.6 · kPeople 7.1 · kAct 6.0 ·
kDoc 5.8 · kMuted 5.2 · kMachine 4.6 (all AA-normal).
- **W5-01 · P1 · FIXED.** Failing text colors corrected in place, hue
  character kept: kSoft #9C948A (2.78!) → #78706A (4.51); kFaint
  #8A8A8A (3.20) → #6F6F6F (4.66); proposed-by gray #999 (2.64) →
  #6F6F6F; kWarn #B26B00 (3.90) → #935800 (5.36); kGold TEXT →
  #82672A (4.97) while chrome gold (borders, washes, checked frames)
  keeps #9A7A33 — non-text contrast rules differ. 80 literal sites
  migrated with the tokens; all 59 suites and every rendered pin hold.
- **W5-02 · verified by design.** Color is never the sole carrier:
  rulings carry ✓/✗/⏸ glyphs, epistemic badges carry their text, register
  warnings are prose. Deuteranopia-safe by redundancy.
- **W5-03 · noted for W7.** Band captions (gold 9px on
  palette(alternate-base)) sit on a theme-dependent ground — measured
  3.41 on dark chrome; ruled in W7's theme pass, not here.

## W7 — THEME COHERENCE (2026-08-20)
Ruling was already Adam's own (2026-08-10, selftest-pinned): "night
chrome, paper page" — reading surfaces stay cream in all modes; the
chrome follows the system appearance. W7 verified the ruling holds
(app-wide QTextEdit/QTextBrowser/QPlainTextEdit cream + ink, pinned)
and swept the boundary: widget labels sitting ON the chrome.
- **W7-01 · P2 · FIXED.** Chrome-ground labels with single-appearance
  inks: #777 status/census/who labels measured 2.2–2.9:1 on BOTH
  grounds; gold and hint labels marginal on dark. New ux::darkChrome()/
  chromeMuted()/chromeGold() pick the ink at use time — values measured
  on both grounds (5.4–6.0:1 dark, 4.5–6.0:1 light). Six label sites +
  every band caption (RibbonGroup) migrated.
- Dark-panel viewers (four-layer scan, e-text) keep their own light-on-
  dark inks by design — they own their background explicitly. HOLDS.

## W6 — TIBETAN TYPE POLICY (2026-08-20)
- Shaping: --sanskritcheck battery re-run on the current build — 0
  missing-glyph cases across both bundled fonts, including the deep
  stack rkSya (རྐྵྱ). HOLDS.
- Line height: the card body already renders inside line-height:1.7 —
  stacked vowels/subjoined letters have head- and foot-room. HOLDS.
- **W6-01 · P2 · FIXED.** Inline Tibetan in the 84000 TM evidence rows
  rendered at the 13px Latin card-body size — stacks illegible. New
  token ux::kFsTibetanInline = 16 is the ruled FLOOR for Tibetan
  outside the headword; TM rows migrated (and gained honest elision,
  which the multi-line form had missed).
- Dedicated viewers (four-layer, e-text, editors) own their fonts at
  larger sizes. HOLDS.

## W8 — TERMINOLOGY TABLE (2026-08-20)
One concept, one word — as the UI actually speaks, now ruled:
- **Wylie** — the user-facing name for the transliteration (script
  menus, row labels). **EWTS** appears only where the machine standard
  itself matters (Convert pane's auto-detect note). RULED, consistent.
- **HGM** (badge) / **the master's attested English** (four-layer
  caption) — the same referent in two registers: badge grammar vs
  reading prose. RULED acceptable: the badge is an identity mark, the
  caption is narrative; both are honesty-load-bearing and stay verbatim.
- **The authority** — the approver ROLE (Geshe Michael in the Approval
  workflow); distinct concept from HGM-the-gloss-source, and the UI
  keeps them distinct. HOLDS.
- **Corpus** = the 35,601-segment parallel evidence base, everywhere.
  HOLDS.
- Interaction idioms verified in W2/W9 passes: one warning voice for
  writes (warnWriteFail), one for opens (warnOpenFail), Trash always
  "recoverable", previews before destructive syncs. HOLDS.
No renames shipped: the audit found register variation, not concept
collision — renames would churn honesty wording for zero clarity gain.

## W10 — THE USAGE LEDGER (built 2026-08-20)
Opt-in (default OFF), local-only, never transmitted: date·kind·name TSV
in Application Support. Hooks: every pane raise (notePane) and every
band-action click (RibbonProxy). Help → Usage Ledger…: the opt-in
toggle, the aggregated counts ranked, Reveal file. Harness runs never
tick. The H3 re-derivation from real data waits for a week of use.

## §11 — VISUAL-REGRESSION GATE (built 2026-08-20)
tools/shot_diff.py (perceptual: 256px grayscale mean-|delta|, threshold
0.022) + tools/bless_shots.sh (explicit, logged blessing) + press step
2b (exit 7 on unexplained change). The gate found its first bug BEFORE
guarding anything: the sweep was non-deterministic — a pane whose
minimum sizes exceeded the window grew it MID-SWEEP and reflowed every
later capture (and async settles differ run-to-run at loose sizes).
Fixed: the sweep pins the window at 1180x760. Same-build re-run now
matches 22/22; baseline blessed.
