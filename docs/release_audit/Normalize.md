# Normalize… with preview (F3, analysis suite batch 4) — release audit

*Added 2026-09-09. Files: `core/include/allcore/textnorm.h`, `core/src/textnorm.cpp` (Qt-free; helpers promoted from textdiff into `core/src/text_util.h`), `core/tests/textnorm_smoke.cpp` (44 checks), `app/normalize_pane.inc` (targets, pure driver, file path, dialog), wiring in `app/main.cpp` and `app/compare_pane.inc`.*

## 1. Justification
A raw input-centre file arrives with double spaces, tabs, trailing blanks, mixed line endings, inconsistent double-shad spelling and stray `[notes]`. Today each is a hand pass or a blind Replace All the style guide forbids. Normalize offers the compare rules as transformations, shows every line that would change as a diff in the Compare pane, lists every line the rules refused and why, and only then applies — with the original kept as a version for files. It is the rule engine batch normalization (F4, Normalize mode) reuses.

## 2. What it does

| Function | Where | Behaviour | Evidence |
|---|---|---|---|
| `textnorm::normalizeText` | core | Fixed idempotent order per line: classify (ACIP / Tibetan Unicode / Other / Mixed) → strip apparatus only when both bracket kinds balance (else refuse) → strip folio markers (glued marker refuses) → shad rules on ACIP/Unicode lines only as a codepoint walk (double shad atomic; tsheg never touched; Mixed refused) → collapse spaces → trim; then blank-line runs, EOL, final newline. Never joins or splits lines; nothing converts scripts | textnorm_smoke 1–16 (44 checks) |
| Format ▸ Normalize Text… | Format menu | Focused Document box or Draft (selection extended to whole lines, one edit block); the Manuscript is refused with the stated message | selftests F3-17, 18, 21 |
| Compare Sessions ▾ ▸ Normalize Left… / Right… | `ComparePane::setSideLines` via `g_normalizeSide` | One undo snapshot; Undo Merge reverts; Save Left/Right writes | selftests F3-22, 22b |
| Files pane right-click ▸ Normalize… | `FilesPane` context menus | Strict UTF-8 (`enc::decode` bad count), binary refused, BOM kept, EOL as chosen, `pre-normalize` version banked FIRST (else nothing written); a clean open Overlay document reloads | selftests F3-19, 19b, 20 |
| Preview in Compare / Apply gating | `NormalizeDialog` | Result label counted from the diff; Apply enabled only for a preview computed with exactly these settings and this text; any change disables it | selftests F3-23…23d |
| Presets | `<dataRoot>/data/house_style/normalize_presets.json` + `QSettings normalize/presets` | Combo hidden when none exist; `source` as tooltip; Save as preset… | — (no presets ship until the formatting sheet arrives) |

## 3. Honesty
- Banner: the sheet has not been supplied; nothing is guessed; per line only.
- Double-shad form and space-before-shad have NO default ("awaiting the house ruling").
- Lossy switches are red and say "removes text"; every removal shows as a deletion in the preview.
- Refused lines are byte-identical and listed with the reason; clicking one jumps the editor there.
- "final line break added" is said only when the text lacked one.
- Tibetan Unicode canonical/stacking normalization is not offered (deliberately omitted).

## 4. Open
- Engine interpretations recorded by the implementer (word-level ACIP classification with a two-letter floor; a respelled double shad owns its left edge; script-matched respelling; nested/overlapping apparatus refused) — to confirm against the formatting sheet.
