# Replace in Files… with mandatory preview (F4, analysis suite batch 4) — release audit

*Added 2026-09-09. Files: `app/replace_files.inc` (planner `replf::`, window, selftests), `allcore::versions` changeset records (`core/include/allcore/versions.h`, `core/src/versions.cpp`; versions_smoke 20–25), wiring in `app/main.cpp`.*

## 1. Justification
The input-centre formatter's core (roadmap item A): one systematic fix across a whole folder of ACIP files, every occurrence shown with its line before and after, untick the ones that are right, apply only to ticked files, an honest count of what changed and what was skipped and why, and undo the whole run later. Without it the fix is a blind Replace All the style guide forbids, or a hand pass.

## 2. What it does

| Function | Where | Behaviour | Evidence |
|---|---|---|---|
| Plan (`replf::buildPlan`) | app planner | Scope through the ONE eligibility rule (`allcore::listEligibleFiles`, include/exclude globs, recursion, counted file cap); the rewrite reader is strict UTF-8 (binary, not-UTF-8, over 10 MB, unreadable each named); one `QRegularExpression` pass per file with the Find bar's own pattern, `$N` expansion and Preserve case; occurrences carry line, folio cite, old and new line; file EOL/BOM/final break recorded; a `key` hashes inputs and file hashes so Apply is valid only for this preview | F4-1, 2, 3, 10, 11, 12, 14 |
| Find ▸ Replace in Files… (⇧⌥⌘F) · Undo Last Replace in Files… · Files ▸ Replace in these files… · Search Results ▸ Replace in these folders… | menus / panes | Seeded from the Find dialog, the Files selection, or the Search pane's single plain term (button disabled with the stated tooltip for OR / NEAR queries) | wiring; F4-17 |
| Window | `ReplaceFilesWindow` | Mode tabs (Find and replace · Normalize (rules) — F3's panel embedded); scope; preview tree with lazy occurrence children, tick/untick, double-click → Compare before/after, right-click → Open in Overlay; footer counts; script-mismatch notice; two-step Apply (harness: `harnessAutoConfirm_`); result panel with clickable rows and the changeset id; Undo this run… | F4-17…18c |
| Apply (`applyPlan`) | planner | Phase 1 verify: re-hash (changed on disk → skipped), open with unsaved edits → skipped by name, library text without permission → skipped, confirmation required, Versions must be on. Phase 2 per file: `pre-replace` version with the changeset id must be kept, else the file is abandoned; bytes rewritten with the file's own EOL, BOM and final break via `saveOrWarn`; clean open documents reload; changeset record (unique id per run, ms + suffix) written; index note | F4-4, 5, 6, 7, 8, 16 |
| Undo (`undoChangeset`) | planner | Each recorded file whose hash still equals `postSha1` is banked (`pre-restore`) and restored from its `pre-replace` version; edited files skipped with the reason; the changeset is marked undone; a second undo is refused | F4-9, 9b, 9c |
| Normalize mode | planner | F3's rules replace the regex pass; changed lines are the tick unit | F4-15 |

## 3. Honesty
- Counts come from the files' text, never from the search index — the footer says so.
- Every skip is named; the footer and result rows list them; a stopped scan says "stopped early: yes" and blocks Apply.
- A script-mismatched zero carries the notice (Tibetan Unicode vs ACIP…); it never reads as a clean bill of health.
- Nothing is written until Apply; the preview is the apply (same function, same text, re-hashed).
- Write verdicts come only from `saveOrWarn`; NOT WRITTEN rows are never counted as changed.
- With Versions off the run is refused: there would be no undo copies.
- After apply the index note says the folder's index is out of date unless the checkbox ran the indexer, whose own line is shown.

## 4. Open
- The result panel's rows open the file at line 1 (the changeset does not record per-occurrence lines after the rewrite).
- Normalize-mode per-occurrence unticking rewrites whole files from the rule output (the tick unit is informational there).
