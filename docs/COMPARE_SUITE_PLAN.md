# Compare & Merge Suite — plan (Adam, 2026-09-08 evening)

Adam: "integrate these features and tool set as well. Do your research on
Beyond Compare, WinMerge, UltraCompare and Araxis Merge to bring all of
their combined functionality into our translation tool … thoroughly and
perfectly … amazingly useful."

## Why a translation tool needs this (the steelman)
- **Editions.** Two prints of the same Tibetan text (Sera Mey vs Ganden,
  ACIP input vs a later correction pass) differ in readings. Today that
  comparison happens by eye or in a programmer's diff tool that knows
  nothing about syllables, scripts, shads or folio markers.
- **Proofreading.** Input centres deliver a raw file and a corrected file;
  the correction report is the diff — cited by folio, exportable.
- **Drafts.** Two translators' drafts of the same passage, or a draft
  against Geshe Michael's published rendering, merged into one Manuscript.
- **Team merge.** Base text + two editors' copies → one, with conflicts
  shown, not guessed.
- **Folders.** Two release folders or two input-centre batches: what is
  new, what changed, what only one side has; sync with confirmation.

## Sources consulted (2026-09-08)
- WinMerge manual, "Comparing and merging text files"
  (manual.winmerge.org/en/Compare_files.html) and "Using Filters".
- Araxis Merge product page (araxis.com/merge) and folder-comparison
  overview.
- Beyond Compare: vendor summaries via search (scootersoftware.com) — the
  help pages were not reachable; feature names below are BC's public ones.
- UltraCompare: vendor page summary via search (ultraedit.com) — page
  blocked direct fetch.
Everything below is a re-implementation from the feature *names*; no code
from any of these products is used.

## Feature matrix → verdict
| Feature (who has it) | Ours | Verdict |
|---|---|---|
| Side-by-side text compare with colour blocks (all four) | Compare pane, two Document-style editors, line numbers, change bars | BUILD |
| Word/character-level in-line highlights (WinMerge, Araxis, BC "line details") | **Syllable-level for Tibetan** (any of ACIP/Wylie/Unicode), word-level for English, character fallback | ADAPT (better than the originals for our texts) |
| Linking lines between panes (Araxis) / arrows per difference (BC) | link strip with curves + ◀ ▶ copy buttons per difference | BUILD |
| Location pane / overview map (WinMerge, Araxis overview marks, BC thumbnail) | clickable map at the right edge | BUILD |
| Diff pane / line details (WinMerge, BC) | Line Details panel under the editors | BUILD |
| First/Prev/Next/Last difference, next section (all) | ribbon + F7/F8-style shortcuts | BUILD |
| Copy left/right, copy-and-advance, all-left/all-right (WinMerge, BC, UC) | same | BUILD |
| Unlimited undo of merges (Araxis, UC) | snapshot undo/redo stack across merges and rescans | BUILD |
| In-place editing with rescan (all) | editable panes; Rescan (F5) and auto-rescan when idle | BUILD |
| Show all / differences only / differences + context (BC, WinMerge) | same, with context count | BUILD |
| Minor / unimportant differences toggle (BC), ignore whitespace / case / blank lines / EOL (all) | same, plus **Tibetan punctuation, folio markers, ACIP apparatus, script-agnostic (ACIP=Wylie=Unicode)** | ADAPT — the Tibetan rules are the point |
| Line filters by regex (WinMerge), unimportant text via rules (BC), regex ignore (Araxis) | Line filters… (regex; matching lines are minor) | BUILD |
| Substitution filters (WinMerge) | Substitutions… (regex → replacement before compare) | BUILD |
| Moved-block detection (WinMerge) | detected and coloured; Goto Moved Line | BUILD |
| Manual alignment: sync points (WinMerge), synchronization links (Araxis), alignment overrides (BC) | Align these lines (pin two lines; the diff runs above and below the pin) | BUILD (batch 3) |
| Three-way compare and merge with automatic merge of non-conflicting changes (all four) | Three-Way Merge page: Left · Base · Right + merged result; Take Left/Right/Both/Base per conflict; Auto-merge on load | BUILD |
| Conflict file opening (WinMerge) | Open a file with `<<<<<<<` markers as a two-pane compare | BUILD (batch 3) |
| Folder compare with colour status, filters, show-same/different/orphans (all) | Folder Compare page: table of Identical/Different/Minor-only/Left only/Right only, glob filter, show filters | BUILD |
| Comparison criteria: size/timestamp/CRC/binary/rules-based (BC), byte-by-byte verification (Araxis) | bytes (always honest) or rules-based text compare with the same ignore options | BUILD |
| Folder sync: copy selected, update, mirror (all) | Copy to Right/Left (selected), Update Right/Left (missing + different), Mirror (with a preview list and confirmation; never deletes without a second confirmation) | BUILD |
| Folder snapshots (BC, UC) | Save Snapshot… (paths, sizes, hashes) · Compare with Snapshot… | BUILD (batch 3) |
| Archive compare (BC, Araxis) | via the Files pane's archive browser | SKIP for now (Files pane opens archives; compare needs an extraction step) |
| Sessions / recent comparisons (BC, UC, Araxis) | Save Session… / Open Session… (`library/compare_sessions/*.json`), Recent Comparisons ▸ | BUILD |
| Reports: HTML side-by-side, unified/context/normal patch, XML, HTML slideshow, CSV (all) | HTML side-by-side · unified patch · **apparatus criticus** (Markdown + CSV, cited by folio) · plain summary | ADAPT — XML/slideshow SKIP (no consumer) |
| Compare to clipboard (BC, WinMerge, Araxis) | Compare with Clipboard (front editor vs clipboard) | BUILD |
| Compare with saved / earlier revision (Araxis, VCS integration in all) | Compare with Saved Version (editor vs disk) · Compare with Backup (the app's own backups where present) | BUILD (backup part batch 3) |
| Table/CSV compare (BC, WinMerge) | glossary files are CSV/JSON: table compare by key column | LATER (filed; not in this sprint) |
| Hex/binary compare (all) | byte-identical or not, with first differing offset | BUILD (minimal, honest) |
| Image compare (WinMerge, Araxis, BC picture) | scans of the same folio side by side with a blend slider | LATER (filed; needs the Scans pane's viewer) |
| Word/Excel/PDF text extraction compare (Araxis, UC) | .docx text extraction exists in the export path; compare extracted text | LATER |
| FTP / network compare (UC) | — | SKIP |
| Version-control integration (all) | — | SKIP (no VCS in the translators' workflow) |
| Scripting / command line (BC, Araxis) | `--compare A B` CLI mode printing the summary and writing a report | BUILD (batch 3) |
| Syntax highlighting (all) | the Overlay's script colouring is not a syntax; line-number gutter and change bars only | SKIP |
| Duplicate finder (UC) | — | SKIP |
| Bookmarks in compare panes (WinMerge, BC) | reuse NumberedEdit bookmarks | ADAPT |
| Printing (Araxis, BC) | Print the HTML report | ADAPT |

## Architecture
- **Core (`allcore::textdiff`, no Qt):** line splitting; normalisation
  under the ignore options (script-agnostic through the canonical
  converters — a reverse conversion that warns leaves the line raw and
  says so); Myers O(ND) linear-space line diff (ported from the published
  algorithm, not from any product); hunks (Equal/Insert/Delete/Change,
  unimportant flag, moved link); syllable/word/char in-line diff; diff3
  three-way merge with conflict chunks and resolutions; reports (unified
  patch, HTML side-by-side, apparatus criticus Markdown/CSV, summary).
  Battery: `core/tests/textdiff_smoke.cpp` (ctest `textdiff_smoke`).
- **App (`ComparePane`, Research group):** ribbon (SESSION · VIEW ·
  NAVIGATE · MERGE · RULES · REPORT), pages: Text Compare, Three-Way
  Merge, Folder Compare. Folder walking uses Qt's filesystem API.
- **Hooks:** Tools ▸ Compare ▸ …; Files pane context menu (Compare with
  the other side · Compare the two selected · Compare folders left ↔
  right); Overlay/Draft/Manuscript "Compare with Saved / Clipboard".

## Batches
1. Core engine + battery — DONE 2026-09-08 (textdiff_smoke 43/43).
2. Compare pane: text compare, navigation, merge, undo, rules, line
   details, location pane, reports, sessions, Tools ▸ Compare, Files hooks,
   selftests — DONE 2026-09-08 (19 checks; placed in the Research group).
3. Three-way merge page, folder compare page with sync and snapshots,
   conflict-file opening, manual alignment pins, `--compare` CLI — DONE
   2026-09-08 (Compare with Backup deferred: the backup module's API is
   next; table/image/.docx compare filed for later).
4. Audit rows, LOG, TODO, digest draft update; press when Adam asks.

## Honesty rules that apply
- The pane never edits a file on disk until Save is pressed; sync
  operations preview and confirm; nothing is deleted by a sync.
- Script-agnostic comparison uses the canonical converters only; a line
  that will not convert cleanly is compared raw and flagged "not
  normalised" in the status line.
- "Minor" is defined by the rules the user switched on; the count of
  minor differences is always shown next to the important count.
