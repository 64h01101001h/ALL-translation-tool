# Versions (F1, analysis suite batch 4) — release audit

*Added 2026-09-09. Files: `core/include/allcore/versions.h`, `core/src/versions.cpp`
(the store, Qt-free), `core/tests/versions_smoke.cpp` (32 checks),
`app/versions_pane.inc` (app glue + the Versions window), wiring in `app/main.cpp`.*

## 1. Justification (steelman before test)
An editor asked "what did I change last Thursday" has, today, only the file on
disk and whatever Word kept. Every Save in this app now keeps a compressed copy
of the bytes written, with who saved it, when, in which encoding, and the
text statistics of that moment. The copy is comparable (Compare pane), restorable
(bank-first), nameable (pinned versions are never pruned) and answers "which
version first introduced this line" with the diff engine. Without it the
Compare suite compares only against the disk or the clipboard. Utpala's
interview guide lists this as candidate 6; this is the build she will judge.

## 2. What it does (one row per function)

| Function | Where | Behaviour | Evidence |
|---|---|---|---|
| Keep a version on Save | Overlay `writeDocumentTo`, Draft `saveDraft`, Manuscript `save(autosave)` → `docprops::noteVersion` after `noteSave` | Encoded bytes as written (Manuscript: the HTML), qCompress "qz", SHA-1 of the document bytes, revision from the sidecar, savedBy from the Team name (or the login, labelled), statistics JSON; identical bytes are deduplicated and the hint says so | selftests 20, 20b, 21, 22, 23, 24, 25 |
| One rolling autosave slot | Manuscript autosave timer passes `autosave=true`; store keeps one `autosave` entry until the next Save | Preferences ▸ Versions "autosave slot" switch | versions_smoke 5 |
| Caps and pruning | `Limits` from `versions/capCount` (200) and `versions/capMB` (64); ≤0 = no cap | Newest, newest non-autosave and pinned are never pruned; the notice reports the count removed | versions_smoke 6, 7, 7b |
| Versions window | File ▸ Versions… (⌥⌘V) · Tools ▸ Compare ▸ Compare with Version… · Properties ▸ Statistics ▸ Versions… | Table newest-first (when, who, kind, name, rev, size, folios, syllables, shads, words, lines); filter All / Saves / Milestones / Batches; Show autosaves; "elsewhere" rows for same-name files in other folders | selftest 26 |
| Compare with Current / Two Selected | window buttons, double-click | Version on the left, editing text on the right, via `g_compareTexts`; older/newer ordered by stamp; refuses undecodable bytes with the count (nothing guessed) | selftest 27 |
| Restore… | two-step button (8 s arm) — under the harness `harnessAutoConfirm_` decides | Refuses while the text has unsaved edits; banks the on-disk bytes as `pre-restore` FIRST (even an outside edit); writes the version; the pane reloads in the version's recorded encoding; notice states which of the three cases happened | selftests 28, 29, 30, 30b |
| Save Version As… | button | Raw bytes to a chosen file; the current file untouched | — (uses `saveOrWarn`) |
| Name this Version… | button | Label + pin; pinned entries survive pruning | selftest 31 |
| Line Origins… | button (caret line by default) | `textdiff::lineOrigins` over saves and milestones, oldest first; MACHINE label; states its basis and that pruned versions and outside edits are not seen | selftest 32a; textdiff_smoke 4 checks |
| Damaged / unreadable records | list | SHA-1 mismatch or garbled record → shown red as "damaged", excluded from Compare and Restore; orphan blobs counted, never offered | selftest 32b; versions_smoke 13, 14, 15 |
| Versions off | Preferences ▸ Versions | A save keeps nothing and says nothing | selftest 32c |
| Rename follows the history | `docprops::renameFileTo` directory pairs | `<dataRoot>/library/versions/<key>/` moves with the file | selftest "Rename carries the version-history folder" |

## 3. Honesty
- Only saves made in this app leave versions; the footer says so, and that
  there is no Track Changes toggle for texts (accept/reject = Compare + Copy
  Left/Right + Save).
- "Who" states its source: Team name, or the login with "no Team name set".
- Statistics absent → "—" with the recorded note, never zero.
- Manuscript versions are compared as text; the window says formatting is
  not compared.
- The store never invents provenance; `put` refuses a record without a hash.

## 4. Open
- Provenance sidecar `_meta.json` "pruned" count is per document; a global
  "versions on disk" figure for Preferences is not shown yet.
- Same-base-name files under one data root share a key (dedupe is by hash
  across the directory); the "elsewhere" filter separates them by path.
