# DrillsPane — release audit

`class DrillsPane` (app/main.cpp:18646–19329) is the learner-facing exercise pane: it generates six kinds of drill from real corpus segments held in the SQLite spine (chunk order, cloze, particle choice, parallel reading, vocabulary SRS review, translate-and-compare), grades the answer, shows HGM's own English as the answer key, and records every result — including a per-skill classification of each miss — into the learner's local `progress.db`. It is constructed at app/main.cpp:35860 with `new DrillsPane(spine, progress)` and added as a top-level tab, `tabs.addTab(drillsPane, "Drills")` (app/main.cpp:35861), in the same tab run as Trainer and Draft; the user reaches it by clicking the **Drills** tab in the main window. The `progress` pointer comes from `static allcore::Progress pr(root + "/progress.db")` (app/main.cpp:35755), inside a try/catch that leaves `progress == nullptr` on failure — so the whole pane degrades to a stateless drill trainer if the progress database cannot be opened.

## User-visible functions

| Control/action | What it does | Code anchor (main.cpp line) | Automated coverage | Notes/risks |
|---|---|---|---|---|
| **"My weak spots…"** button | Opens a modeless `QDialog` (WA_DeleteOnClose, 520×560) showing `missReportHtml(progress_->topMisses(100), stats.drills_done)` — the miss taxonomy grouped into four families with the drill that trains each | ctor 18660–18667; `showMissReport()` 18937–18954 | `missReportHtml` only, via the app-wide selftest block at 40337–40355: "Drills: miss-taxonomy report groups families, names skills, honest when empty" (ctest suite `app_selftest`) | The button silently no-ops when `progress_ == nullptr` (18938 `if (!progress_) return;`) — no message to the user. The dialog itself is never exercised by any test. |
| **Mode combo** (`mode_`) — Chunk order / Cloze / Particle choice / Parallel reading / Vocabulary review (SRS) / Translate & compare | On change: shows course combo only for mode 3, the order-entry line edit only for mode 0, the translation draft box only for mode 5, then generates a new drill | ctor 18670–18675, 18727–18732 | `selfTest` drives indices 0,1,2 and 5 directly ("order drill generated from corpus", "cloze drill offers options", "particle drill generated", plus the four mode-5 answer-key checks) | Modes **3 (Parallel reading)** and **4 (Vocabulary review / SRS)** are never selected by `selfTest`: NONE. Persisted as QSettings `sess/drills/mode` (18691). |
| **"New drill"** button | `newDrill()` — clears answers/result, resets all drill state, builds the drill for the current mode from `factory_`/spine, re-renders and refreshes stats | ctor 18676–18677, 18725; `newDrill()` 18979–19023 | Indirectly, all `selfTest` checks call `newDrill()` | In mode 3 it advances the reading position and, if not revealed, records a clean read into progress.db (18997–19004) — a disk write from a plain "next" click. |
| **"Tibetan script"** checkbox (`script_`) | Toggles rendering of ACIP text through `acipToEwts` → `wylieToUnicode` per token; re-renders the question | ctor 18678–18679, 18733; `disp()` 18804–18826 | NONE (never toggled by `selfTest`) | Conversion failure falls back silently to the raw ACIP string (18813) — no flag surfaced, which brushes against house rule 3. QSettings `sess/drills/script`. |
| **"adapt to my level"** checkbox (`adaptive_`) | Sets `factory_.setAdaptive(on)` so drills are picked at the learner's level; auto-checked and enabled only when a progress log exists | ctor 18680–18686 | NONE | Disabled state is the only signal that progress.db failed to open. QSettings `sess/drills/adaptive`. |
| **Course combo** (`course_`) | Chooses which corpus course the Parallel-reading mode walks; hidden except in mode 3 | ctor 18687–18694, 18728 | NONE | Switching course calls `spine_.corpusWindow(course, 0, 9999999)` (19000) — loads an entire course into memory with no cap. QSettings `sess/drills/course`. |
| **Order input line** (`input_`) | Free-text letters ("C A B") parsed as the restored chunk order in mode 0 | ctor 18716–18718; parsed in `checkDrill()` 19105–19112 and 19259–19270 | NONE (selfTest never types an order) | Out-of-range letters are silently dropped; a partially-typed answer just grades as wrong. |
| **Translation draft box** (`transDraft_`) | Free-text English draft for mode 5, compared against HGM's rendering | ctor 18711–18715 | `selfTest` sets it ("my translation mentions nothing relevant") for the mode-5 answer-key checks | Visible only in mode 5. |
| **"Check / Reveal"** button | `checkDrill()` — grades the current drill, prints the original order / correct option / HGM's English, and writes drill + miss records to progress.db | ctor 18719–18720, 18726; `checkDrill()` 19096–19310 | Mode 5 only: the four house-rule-1 checks ("answer key reports both unrendered terms", "…tier-labeled PROVISIONAL", "an auto-aligned gloss is NEVER printed under the label \"HGM has:\"", "a binding gloss (bum pa) still reads as HGM's own") | Grading paths for modes 0–4 are uncovered. Every path can write to progress.db. In mode 3 Check counts as a "peek" (19087–19090). |
| **Radio buttons** (cloze options / particle options / "I knew it" / "I did not know it") | The answer selector for modes 1, 2, 4; `pickedRadio()` returns the chosen index | `addRadios()` 18836–18844; `pickedRadio()` 19089–19094 | Existence only, via "cloze drill offers options" | Selecting the SRS "I did not know it" radio reschedules the word and files a `miss:vocab:` record (19215–19222) — uncovered. |
| Drill/miss recording (implicit on every Check) | `progress_->recordDrill(...)` for order/cloze/particle/translate, plus a classified `miss:*` record naming the skill revealed | 19187–19191, 19215–19222, 19233–19286 | NONE end-to-end (core-level `progress_smoke` / `drills_smoke` ctest suites cover the library, not this wiring) | Disk write on every Check. `selfTest` deliberately nulls `progress_` for its mode-5 block (18761–18763) so the battery never pollutes the real log — good. |
| Stats label (`stats_`) | Read-only line: deck size, due count, drills done/correct, segments read/peeked, top-3 weak spots | ctor 18658; `refreshStats()` 18956–18977 | NONE | Blank (not explanatory) when `progress_` is null. |
| Question / result views (`question_`, `result_`) | `QTextBrowser`s showing the drill and the answer key | ctor 18701–18710, 18721–18722 | Result text read back by the mode-5 checks | No `anchorClicked` handler — links in this HTML are inert; no custom link scheme is defined. |

No `addAction`, context menus, `setShortcut`, drag/drop, double-click handlers, `QFileDialog`, `QProcess`, `QDesktopServices`, `QNetworkAccessManager`/`TimedNam`, `QFile`/`QSaveFile` appear anywhere in 18646–19329. The pane makes **no network calls and writes no files directly**; its only persistence is via `allcore::Progress` (progress.db) and `sess::remember` (QSettings). No TODO/FIXME/XXX/HACK markers in range.

## Internal functions

| Name | Line | Purpose | Called from |
|---|---|---|---|
| `DrillsPane(Spine&, Progress*)` | 18648 | Builds the whole UI, wires signals, restores session settings | app/main.cpp:35860 |
| `selfTest(QStringList&)` | 18737 | Pane battery: generates modes 0/1/2, then a synthetic mode-5 segment to prove tier labeling in the answer key | app/main.cpp:38263 (`--selftest`, ctest suite `app_selftest`) |
| `disp(const std::string&)` | 18804 | ACIP → display text, optionally converted to Tibetan script per token | `addRadios`, `renderQuestion`, `checkDrill` |
| `clearAnswers()` | 18828 | Deletes the answer-row widgets and clears `radios_` | `newDrill` |
| `addRadios(options, asAcip)` | 18836 | Adds one radio per option to the answer row | `newDrill` |
| `missReportHtml(misses, drillsDone)` (public, static) | 18847 | Builds the weak-spots HTML: sorts `miss:` keys into 4 families, counts them, names the training drill; honest empty state | `showMissReport`, and the app-wide selftest at 40337 |
| `showMissReport()` | 18937 | Opens the weak-spots dialog | "My weak spots…" button |
| `refreshStats()` | 18956 | Rebuilds the one-line stats label from `progress_->stats()` | ctor, `newDrill`, `checkDrill` |
| `newDrill()` | 18979 | Generates the drill for the current mode | "New drill", mode combo, `selfTest` |
| `renderQuestion()` | 19025 | Renders the current drill's prompt HTML for all six modes | `newDrill`, script checkbox |
| `pickedRadio()` | 19089 | Index of the checked radio, or -1 | `checkDrill` |
| `checkDrill()` | 19096 | Grades, reveals HGM's English, records results and classified misses | "Check / Reveal", `selfTest` |

## Dependencies

- **Data read:** the SQLite spine via `allcore::Spine` (`corpusCourses()`, `corpusWindow()`, `lookup()`), `allcore::HeadwordIndex`, `allcore::DrillFactory`, `allcore::checkTerminology`. The spine path is set by the host app (`build/hgm_spine_v27_2.db` in the ctest suites).
- **Data written:** `<root>/progress.db` — the learner's local SRS/progress log — exclusively through `allcore::Progress` (`recordDrill`, `reviewWord`, `recordSegmentRead`, and reads `stats`, `topMisses`, `dueWords`). The handle is created at app/main.cpp:35755, not inside this class.
- **Network hosts:** none.
- **QSettings** (organization "ALL", application "TranslationTool", via `sess::remember`, main.cpp:2851/3090+): reads and writes `sess/drills/mode`, `sess/drills/course`, `sess/drills/script`, `sess/drills/adaptive`.
- **ctest:** `app_selftest` (app/CMakeLists.txt:89) runs this pane's `selfTest`. Adjacent core batteries that cover the underlying engines but not this UI: `drills_smoke`, `progress_smoke`, `terminology_smoke` (core/CMakeLists.txt:135, 140, 144).

## Release questions

- Modes **3 (Parallel reading)** and **4 (Vocabulary review / SRS)** have zero automated coverage — they are the two modes that write reading/SRS records into progress.db. Decide whether to add selfTest checks or to manually verify both before release.
- `showMissReport()` and the "My weak spots…" button silently do nothing when progress.db failed to open. Should the button be disabled (like `adaptive_`) or show an explanatory message?
- `disp()` falls back to raw ACIP when `wylieToUnicode` fails, with no visible flag — confirm this is acceptable under inviolable rule 3 ("never guess", failed conversions are flagged, not silently approximated).
- Course switching in mode 3 calls `corpusWindow(course, 0, 9999999)`, loading a whole course at once. Verify memory/latency on the largest shipped course.
- Grading logic for modes 0–4 (`checkDrill`) is untested end-to-end; only the mode-5 tier-labeling honesty is proven. Confirm the answer keys for cloze/particle/order are manually spot-checked against the corpus.
- The mode-5 answer key is the only place house rule 1 is enforced by test. Confirm no other reveal path in this pane can print an auto-aligned gloss without the PROVISIONAL label.
