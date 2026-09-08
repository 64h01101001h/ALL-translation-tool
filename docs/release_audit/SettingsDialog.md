# Release audit — `SettingsDialog`

Source: `/Users/adamderickandrade/ALL-translation-tool/app/main.cpp`, class body lines **34083–34246**.

## 1. Purpose and reach

`SettingsDialog` is the app's single preferences surface: a 560×520 modal `QDialog` with four
groups (Appearance, Reading, Team & proposals, Data) that reads and writes the
`QSettings("ALL", "TranslationTool")` store directly. It is opened from the menu bar via
**View menu → "Settings…"** (`main()`, line 37412–37417); the action carries
`QAction::PreferencesRole`, so on macOS it is relocated by Qt into the
**application menu → Diamond Cutter Translation Tool → Settings…** with the standard
`QKeySequence::Preferences` shortcut (**⌘,**). It is constructed with an `applyNight` callback
supplied by `main()`, and shown with `exec()` (application-modal). A second, non-UI construction
site exists in the `--selftest` path (line 38257–38260) purely to run `selfTest()`. There is no
other entry point — no toolbar button, no Hunt-palette command, no context menu.

## 2. User-visible functions

| Control/action | What it does (from code) | Code anchor (function + line) | Automated coverage | Notes/risks |
|---|---|---|---|---|
| **View → "Settings…" menu action** (⌘, / macOS app menu) | Constructs `SettingsDialog dlg(applyNight, &win)` and `exec()`s it modally | `main()` lambda, 37415–37416 | NONE (no selfTest drives the menu action; selfTest constructs the dialog directly at 38258) | The only route in. If the View menu is hidden/reordered the dialog becomes unreachable. |
| **"Night mode (dark chrome, cream pages)" checkbox** | Reads `app/nightMode` (default `true`) at construction; on Save writes it and calls `applyNight_(checked)` to restyle the running app live | ctor 34105–34108; Save lambda 34177, 34189 | `selfTest`: `check(..., "stored settings reach the widgets on construction")` (34216–34219) and `check(..., "Save writes the edited widgets back to the store")` (34226–34229) — both assert `app/nightMode` round-trip | `applyNight_` is null in the selfTest construction, so the *live restyle* branch (`if (applyNight_) applyNight_(...)`) is never exercised by any test. Also mutated independently by the View-menu Night toggle at 37234–37239 — two writers, no synchronisation; a dialog left open while the menu toggle flips will overwrite on Save. |
| **"Overlay displays text as" combo** (`Tibetan script` / `ACIP` / `Wylie`) | Reads `overlay/scriptMode` (default **1** = ACIP) as the current index; Save writes the current index back | ctor 34112–34117; Save 34178 | NONE | Default index 1 means the shipped default overlay script is **ACIP, not Tibetan** — verify that is intended. Value is a bare index: reordering `addItems` silently re-maps every existing user's stored preference. Consumed at line 7080 (`settings.value("overlay/scriptMode", 1)`) — check whether the overlay re-reads live or only at construction. |
| **"Your name (provenance)" line edit** | Reads `team/name` (no default → empty); Save writes `text().trimmed()` | ctor 34122; Save 34179 | `selfTest` `"stored settings reach the widgets on construction"` and `"Save writes the edited widgets back to the store"` (34216–34229) | Feeds `g_userName` (line 29721), i.e. the **provenance byline stamped onto proposals**. Empty is accepted with no validation and no warning — proposals can be authored anonymously. Read at startup only (29721): editing it mid-session may not affect already-loaded panes. |
| **"Authority role (Geshe Michael / Adam) — shows the Approval pane" checkbox** | Reads `team/admin` (default `false`); Save writes it | ctor 34125–34128; Save 34180 | NONE | **Privilege gate with no authentication.** Any user can tick it and, on next launch, obtain the Approval pane (`g_isAdmin`, line 29722). Takes effect only next launch (stated in the inline note at 34163–34166). Highest-risk control in the dialog. |
| **"Shared proposals folder" line edit** | Reads `team/proposalsDir`; Save writes `text().trimmed()` | ctor 34129; Save 34181 | NONE | Free-text path, never validated for existence or writability. Drives `g_proposalsDir` (29723) — the team Dropbox where proposals are written. A typo silently redirects team output. |
| **"Choose…" button (proposals folder)** | Opens `safeGetExistingDirectory(this, "Shared proposals folder (the team Dropbox)")`; on non-empty result sets the line edit | ctor 34130–34138 | NONE | Native directory panel; wrapped in the harness-safe helper (`safeGetExistingDirectory`, 3341) so headless runs do not hang. Does not itself save — the user must still press Save; no cue that the choice is unsaved. |
| **"Team updates folder" line edit** | Reads `app/updatesDir`; Save writes `text().trimmed()` | ctor 34146; Save 34182 | NONE | Consumed by "Check for Updates…" (line 37361) to scan for DMGs. Unvalidated path; pointing it at an untrusted folder makes the update check offer arbitrary DMGs. **Supply-chain-adjacent setting with zero coverage.** |
| **"Choose…" button (updates folder)** | Opens `safeGetExistingDirectory(this, "Team updates folder (where new DMGs arrive)")`; sets the line edit | ctor 34147–34155 | NONE | Same as above; no validation that the folder contains DMGs or is trusted. |
| **"Data folder override" line edit** (placeholder "auto-detected — set only to override") | Reads `app/dataRoot`; on Save, **removes** the key if blank, otherwise writes the trimmed text | ctor 34157–34161; Save 34183–34187 | NONE | The one key with delete-on-empty semantics. A bad path here can leave the app unable to find the spine/dictionary on next launch, with the dialog itself being the only way back — verify there is a recovery path (line 29659 reads it at startup). Takes effect next launch only. |
| **Static advisory label** ("Changes to the data folder or the authority role take effect on the next launch.") | Non-interactive HTML `QLabel` | ctor 34162–34166 | NONE | Correct as written, but it does **not** mention that `overlay/scriptMode` and `team/name` may also be read only at startup — under-warns. |
| **"Cancel" button** | `QDialog::reject()` — discards all edits, writes nothing | ctor 34172, 34176 | NONE | No dirty-state check: edits are silently lost with no "discard changes?" prompt. |
| **"Save" button** (default button; Enter activates it) | Writes all seven keys, calls `applyNight_`, then `accept()` | ctor 34173–34191 | `selfTest` clicks the real button by text lookup (34221–34223) and asserts the write for `team/name` + `app/nightMode` only | Save is `setDefault(true)`, so **Enter anywhere in the dialog commits immediately**, including a half-typed path in the data-root field. All-or-nothing: no per-field validation, no error path, no confirmation for the authority-role or data-root changes. |
| **Escape key** (inherited `QDialog` behaviour) | Rejects the dialog, discarding edits | inherited, no explicit code | NONE | Same silent-discard risk as Cancel. |
| **Window close button** | Rejects the dialog | inherited | NONE | Same. |

Notable **absences** worth recording: there is no Reset/Restore-defaults action, no import/export of
settings, no "reveal in Finder" for the chosen folders, and no keyboard mnemonics on any control.

## 3. Internal functions

| Name | Line | Purpose | Called from |
|---|---|---|---|
| `SettingsDialog::SettingsDialog(std::function<void(bool)>, QWidget*)` | 34085 | Builds the whole UI, applies the cream/maroon stylesheet, loads every key from `QSettings`, wires the two "Choose…" pickers and the Cancel/Save buttons | `main()` 37415 (View→Settings…); `main()` 38258 (`--selftest`); `selfTest` itself 34215 |
| Save-button lambda (unnamed) | 34176–34190 | The commit path: writes all seven keys, delete-on-empty for `app/dataRoot`, invokes `applyNight_`, `accept()` | Connected to `save` `QPushButton::clicked` |
| Cancel-button lambda | 34175 (`connect(cancel, ..., &QDialog::reject)`) | Rejects | Connected to `cancel` clicked |
| Proposals "Choose…" lambda | 34134–34138 | Directory picker → `dir_->setText` | `pick` clicked |
| Updates "Choose…" lambda | 34151–34155 | Directory picker → `updDir_->setText` | `updPick` clicked |
| `SettingsDialog::selfTest(QStringList& log)` | 34201 | TEST-7 round-trip drill: plants `team/name`/`app/nightMode`, constructs a fresh dialog, asserts widgets loaded, edits them, clicks the real Save button, asserts the store changed, then restores both original values (including removing them if they were absent) | `main()` `--selftest`, 38259 |
| `check` lambda inside `selfTest` | 34203–34208 | Logs `[PASS]/[FAIL] Settings: <what>` and counts failures | `selfTest` |
| `safeGetExistingDirectory(QWidget*, const char*)` (free function) | 3341 | Harness-safe wrapper over `QFileDialog::getExistingDirectory` | both "Choose…" lambdas |

Private members: `applyNight_`, `night_`, `script_`, `name_`, `admin_`, `dir_`, `dataRoot_`,
`updDir_` (34194–34201 region, 34240–34245).

## 4. Dependencies

**QSettings store:** `QSettings("ALL", "TranslationTool")` — organisation `ALL`, application
`TranslationTool` (native backend; on macOS `~/Library/Preferences/com.ALL.TranslationTool.plist`).

Every key this dialog touches:

| Key | Default when read here | Written on Save? | Read elsewhere in main.cpp |
|---|---|---|---|
| `app/nightMode` | `true` (34107) | yes, `setValue` (34177) | 35766 (startup theme), 37236 (View→Night toggle writes it too) |
| `overlay/scriptMode` | `1` → "ACIP" (34116) | yes (34178) | 7080 (overlay pane) |
| `team/name` | *(none — empty QString)* (34122) | yes, trimmed (34179) | 29721 → `g_userName` |
| `team/admin` | `false` (34127) | yes (34180) | 29722 → `g_isAdmin` (Approval pane gate) |
| `team/proposalsDir` | *(none — empty)* (34129) | yes, trimmed (34181) | 29723 → `g_proposalsDir` |
| `app/updatesDir` | *(none — empty)* (34146) | yes, trimmed (34182) | 37361 (Check for Updates… DMG scan) |
| `app/dataRoot` | *(none — empty)* (34159) | yes if non-empty, **`remove()` if empty** (34183–34187) | 29659 (data-root resolution at startup) |

**Disk:** no reads or writes of its own beyond the settings backend; the two "Choose…" buttons only
capture paths. **Network:** none — the dialog makes no network calls (the `app/updatesDir` DMG scan
it configures is a local-filesystem scan at 37361). **Data folders:** none read directly; it
*configures* the data root and the proposals/updates folders consumed elsewhere.

**Code dependencies:** `safeGetExistingDirectory` (3341), the `applyNight` closure built in `main()`
(around 35785–35792), Qt Widgets (`QGroupBox`, `QFormLayout`, `QComboBox`, `QLineEdit`,
`QCheckBox`, `QPushButton`).

**Automated harness:** `ctest` test **`app_selftest`** — `add_test(NAME app_selftest COMMAND
DiamondCutterTranslationTool --selftest)`, `app/CMakeLists.txt:89`. `SettingsDialog::selfTest`
contributes exactly **two** `check(...)` assertions to that suite.

## 5. Release questions

- **Authority role is self-service.** `team/admin` is a bare checkbox that grants the Approval pane
  with no password, no key file, and no server check. Ship as-is, or gate it? At minimum, should
  ticking it require a confirmation naming the consequence?
- **Only 2 of the ~14 user-visible controls are asserted.** `team/admin`, `team/proposalsDir`,
  `app/updatesDir`, `app/dataRoot`, and `overlay/scriptMode` all round-trip with **NONE** coverage,
  and they are the five that change app behaviour. Extend `selfTest` to cover all seven keys?
- **`overlay/scriptMode` default is `1` = ACIP.** Is ACIP the intended out-of-box script for a
  Tibetan reading tool, or should the default be `0` (Tibetan script)? Also: the stored value is a
  positional index, so any future reordering of the combo silently reinterprets existing prefs —
  should it be a string token instead?
- **No validation on any path field.** A non-existent or unwritable `app/dataRoot` is accepted and
  can brick the next launch. Confirm there is a recovery path (env override? safe-mode launch?) or
  add existence/writability checks before Save.
- **`app/updatesDir` points the update checker at an arbitrary folder of DMGs.** Confirm the update
  flow does signature/notarisation verification before offering anything from that folder; otherwise
  this field is an install-arbitrary-code vector configured by a plain text box.
- **Two writers for `app/nightMode`** (this dialog and the View→Night menu toggle at 37234–37239).
  Decide the precedence rule, or have the dialog re-read on show.
- **Silent discard on Cancel/Escape/close** with no dirty check, and **Save is the default button**
  so Enter commits mid-edit. Acceptable for this audience?
- **"Choose…" does not save.** The picked folder only fills a text box; users who close the dialog
  after picking will believe it was set. Should the pickers imply Save, or should the dialog show a
  "unsaved changes" indicator?
- **The next-launch caveat label names only the data folder and authority role.** Verify whether
  `team/name`, `team/proposalsDir`, and `overlay/scriptMode` are also latched at startup (they are
  read at 29721–29723 and 7080) and, if so, widen the warning.
- **`selfTest` mutates the user's real settings store** (plants "Selftest Probe", then restores).
  It restores correctly on the happy path, but a crash or failure mid-drill leaves the probe values
  in the developer's real prefs. Acceptable, or should it use a scoped/temporary organisation name?
- **No Restore Defaults.** Once a user has broken their data root or script mode, there is no
  one-click way back. Ship without one?
