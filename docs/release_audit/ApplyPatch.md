# Apply Patch… (F5, analysis suite batch 4) — release audit

*Added 2026-09-09. Files: `core/include/allcore/textpatch.h`, `core/src/textpatch.cpp` (Qt-free; the documented behaviour of patch(1)), `core/tests/textpatch_smoke.cpp` (26 checks), `app/apply_patch.inc` (driver, dialog, CLI, selftests), wiring in `app/main.cpp` and `app/compare_pane.inc`.*

## 1. Justification
A corrector sends a unified diff for a long ACIP file instead of the whole file. Today the translator has no way to apply it except by hand or with a command-line tool that forces or fails silently. Apply Patch places each hunk and NAMES the placement — exact, with offset, with loosened context, already applied, rejected — previews the proposal as a diff in the Compare pane, exports the rejected hunks verbatim for the corrector, and keeps the previous text as a version before writing.

## 2. What it does

| Function | Where | Behaviour | Evidence |
|---|---|---|---|
| `textpatch::parseUnified` / `applyPatch` / `rejectedAsUnified` / `reversePatch` / `pickFileForTarget` | core | GNU unified diff parser (git noise tolerated as warnings; a body/header mismatch is a per-hunk error naming the line); applier with running delta, outward search, fuzz that never relaxes a '-' line and rejects a hunk left without context, AlreadyApplied as a verdict, loose whitespace on context only, reverse, onlyHunks; context lines copied from the TARGET so loose whitespace never reformats the file | textpatch_smoke 1–15 (26 checks) |
| Tools ▸ Compare ▸ Apply Patch… | Tools menu after Open Conflict File… | Target pre-filled from the front document; Manuscript refused with the stated message; dialog with target/patch/clipboard, file-in-patch combo (multi-file, no guess), fuzz 0–2, reverse, loose whitespace, hunks table with the five verdicts, Re-preview with ticked hunks, Save rejected hunks as .rej…, two-step Apply (harness `harnessAutoConfirm_`) | selftests F5-16…19 |
| Preview | `patchapp::previewFile` / `previewText` | Strict UTF-8 read (F4's reader); parse errors → no preview; the Compare pane's right name carries "(patched — k of n hunks)"; all-rejected → "compare the two files instead"; header names the patch, its hash and the read time | F5-16, 16b, 18 |
| Apply to file | `patchapp::applyToFile` | Disabled while the target is open with unsaved edits (reason shown); re-read must equal the previewed bytes; `pre-patch` version must be kept, else nothing is written; bytes in the target's own EOL, BOM and final break; `<target>.rej` with the rejected hunks verbatim under a header naming the patch; clean open document reloads | F5-16c…16f, 17, 21 |
| Compare Sessions ▾ ▸ Apply Patch to Left/Right | `g_applyPatchSideHook` | The preview shows (side, patched); Apply puts the original pair back and sets the side through one undo snapshot, so Undo Merge returns to the unpatched pair; Save Left/Right writes | F5-20, 20b |
| CLI `--apply-patch <target> <patch> [--out f] [--dry-run] [--fuzz N] [--reverse]` | `patchapp::cli` | Per-hunk verdicts printed; exit 0 all applied / already applied, 1 any rejected, 2 unreadable; says plainly that the CLI keeps no version | F5-22, 22b |

## 3. Honesty
- Five distinct verdicts; "already applied" is never a failure and never produces a .rej entry.
- The right-hand Compare name carries the partial count; the header states the patch hash and read time.
- Rejected hunks are shown in full, exported verbatim, never forced; "not selected" hunks are excluded from the .rej.
- A Manuscript target is refused before any dialog.
- Nothing is written until Apply; Apply banks first or does not write.

## 4. Open
- Multi-target runs from the Files pane (a changeset of kind apply-patch) are not built; the single-target path covers the corrector workflow.
