# Release audit — `HelpWindow`

Source: `/Users/adamderickandrade/ALL-translation-tool/app/main.cpp`, class body lines **34247–34578**.

## 1. Purpose and reach

`HelpWindow` is the in-app documentation surface: a 1020×680 non-modal `QDialog` laid out as a
search box over a `QSplitter` of a result `QListWidget` (left, min 290px) and a `QTextBrowser`
(right, Iowan Old Style / Palatino / Georgia at 14pt). It merges two content sources into one
searchable index: (a) **Markdown chapters** parsed out of five files under `<root>/data/help/`, and
(b) a **live auto-index of every control in every pane**, harvested by walking the main
`QTabWidget` and collecting the text of every `QPushButton`, `QCheckBox`, and every `QComboBox`
item, together with the owning pane name and the widget's tooltip. Clicking a control result offers
an `pane:<n>` link that switches the main window to the pane that owns it.

It is reached from the **Help menu** (built at 37423) by three actions, all of which lazily
construct a single shared `helpWin` (`if (!helpWin) helpWin = new HelpWindow(&tabs, root, &win);`)
and then `show()`/`raise()`/`activateWindow()`:
- **Help → "Diamond Cutter Help & Tutorials…"** (37426–37434), shortcut `QKeySequence::HelpContents`
  (⌘? on macOS) — opens the browse view.
- **Help → "Suggested Workflows…"** (37558–37566) — opens, then calls `openWorkflows()`.
- **Help → "User Manual…"** (37566–37574) — opens, then calls `openManual()`.

A fourth construction site exists in `--selftest` (38485) for the automated checks. Because the
window is non-modal and cached in a `main()`-local pointer, its **control auto-index is frozen at
first open** — panes created or relabelled afterwards are never re-indexed.

## 2. User-visible functions

| Control/action | What it does (from code) | Code anchor (function + line) | Automated coverage | Notes/risks |
|---|---|---|---|---|
| **Help → "Diamond Cutter Help & Tutorials…"** (⌘? / `HelpContents`) | Lazily constructs `HelpWindow`, shows/raises/activates it | `main()` lambda 37428–37433 | NONE (selfTest constructs the window directly at 38485, never through the menu) | Lazy singleton: first open pays the full parse of five Markdown files plus a recursive widget walk of every pane, on the UI thread — a visible hitch. |
| **Help → "Suggested Workflows…"** | Same open, then `openWorkflows()` — replaces the list with the "Suggested Workflows" chapter plus one row per bolded workflow name | `main()` lambda 37559–37566; `openWorkflows()` 34424 | `check`: `[..] Help: Suggested Workflows chapter present` — asserts `hw.hitCount("Suggested Workflows") >= 1 && hw.openWorkflowsCount() >= 7` (38488–38494) | Silently does nothing visible if `chapters_` lacks "Suggested Workflows": `showChapter` no-ops and the list shows only the "(all)" header row. Chapter name is a hard-coded literal — renaming the heading in `tutorials.md` breaks the menu item with no error. |
| **Help → "User Manual…"** | Same open, then `openManual()` — sets the search box to the literal `"Manual: "`, filtering the list to manual chapters | `main()` lambda 37567–37574; `openManual()` 34419 | `check`: `[..] Help: User Manual chapters loaded (%2 chapters total)` — asserts `hasChapter("Manual: Overlay") && hasChapter("Manual: Lookup") && hitCount("Manual: ") >= 40` (38510–38517) | Implemented as a *text search for a prefix string*, not a real filter: a user who types over the box loses the manual view, and any chapter whose body happens to contain "Manual: " also matches. |
| **Search box** (`QLineEdit`, placeholder "Search any feature, button, pane, or topic…") | On every `textChanged`, runs `doSearch(q)` synchronously: matches chapters by title **or full body text**, then controls by label, tooltip, or pane name — all case-insensitive substring | ctor 34281–34285, 34383–34385; `doSearch` 34450 | `check`: `[..] Help: search finds a feature by name` — asserts `hitCount("Load into overlay") >= 1` (38504–38508) | **No debounce.** Every keystroke re-scans every chapter body (five Markdown files' worth) plus the entire control vector — linear-scan-on-keystroke; measure on the full manual. Substring-only: no stemming, no ranking, no fuzzy match, so "look up" does not find "Lookup". |
| **Empty search / clearing the box** | Renders the browse view: every chapter in `chapterOrder_` with a 📖 prefix, then every indexed control grouped under an uppercase `— PANE —` header row, and loads the "Getting Started" chapter into the body | `doSearch` 34455–34492 | Indirectly by `[..] Help: feature auto-index built (%2 entries)` asserting `indexSize() > 150` (38499–38503) | Falls back to the literal string `"Welcome. Pick a chapter."` if "Getting Started" is absent — an unstyled placeholder that would ship as the first thing a user sees. |
| **Result list row — chapter (📖 …)** | `currentRowChanged` → `showResult(r)` → `showChapter(title)` → `body_->setMarkdown(chapters_[title])` | ctor 34386–34387; `showResult` 34540; `showChapter` 34568 | Covered indirectly via `hasChapter`/`hitCount` assertions | Selection-change driven, so arrow-keying through the list renders each chapter — fine, but there is no explicit activation affordance. `showChapter` **silently no-ops** on an unknown name (34569–34570). |
| **Result list row — control (`Label — Pane`)** | `showResult` builds an HTML card: the label as `<h2>`, "Located in the **Pane** pane — [open that pane now]", the tooltip if any, a "Read the full … tutorial →" chapter link, and a fixed grey tip about the menu bar | `showResult` 34551–34566 | NONE for the rendering itself (only `indexSize()` is asserted) | The chapter link is chosen by `c.pane.startsWith(t.left(6))` — a **6-character prefix heuristic** that will mis-pair panes/chapters sharing an opening prefix, and pairs nothing at all for most. The trailing tip ("every button is also in the menu bar under its pane's menu") is an unverified blanket claim. |
| **Result list row — workflow (▸ …)** | Marked `rows_[r].first == -2`: shows the "Suggested Workflows" chapter, moves the cursor to the start, then `body_->find(name)` to scroll to that workflow | `showResult` 34541–34549 | NONE (only the row *count* is asserted, not the jump) | `QTextBrowser::find` return value is **discarded** — if the name is not found the page silently stays at the top with no indication. |
| **Section-header rows (`— PANE —`)** | Non-selectable (`Qt::NoItemFlags`), gold `#9A7A33` foreground; mapped to sentinel `-3` | `doSearch` 34468–34476 | NONE | Sentinel `-3` is not handled in `showResult` — it falls through `row.first < 0` into `showChapter(QString())`, which no-ops. Harmless but unintentional. |
| **`pane:<n>` link in the body** | `anchorClicked` → parses the integer after `pane:`, bounds-checks against `locs_`, sets the outer tab index and, when present, the inner tab index, then `raise()`s the help window | ctor 34388–34406 | NONE | Cross-window side effect: it **navigates the main window** from a help link. Indices are positional into `locs_`, built once at construction — if the tab set changes the link points at the wrong pane. `raise()` keeps Help on top of the pane it just revealed, which may hide the very thing it navigated to. |
| **`chapter:<title>` link in the body** | `anchorClicked` → `showChapter(anchorPayload(a, 8))` | ctor 34406–34408 | NONE | No-ops on a bad title; the user sees a dead link. |
| **`http…` link in the body** | `anchorClicked` → `QDesktopServices::openUrl(u)` | ctor 34408–34410 | NONE | **External network/browser launch driven by Markdown content in `data/help/*.md`.** No allow-list, no confirmation, no scheme check beyond the `http` prefix. Whoever can edit the help data folder can make the app open arbitrary URLs on a click. |
| **Any other link scheme in the body** | Falls through the if/else chain — nothing happens (`setOpenLinks(false)` prevents the default) | ctor 34388–34411 | NONE | Silent dead link; `file:`/`mailto:` in help Markdown would simply not work with no message. |
| **Splitter handle drag** | Resizes the list/body split (list has `setMinimumWidth(290)`, body has stretch factor 1) | ctor 34285–34305 | NONE | Position is **not persisted** — resets on every construction. |
| **Window resize / close** | Standard `QDialog`; the instance is cached in `main()`'s `helpWin` so state survives a close and re-open | `main()` 37422, 37428 | NONE | Non-modal and never deleted; geometry not saved to `QSettings`. Closing does **not** rebuild the control index, so a stale index persists for the session. |
| **Escape key** | Inherited `QDialog::reject()` — closes the window | inherited | NONE | Consistent with the app's other dialogs. |
| **Text selection / copy in the body** (`QTextBrowser` default) | Standard read-only text selection, ⌘C copy, and the default context menu (Copy, Select All) | inherited from `QTextBrowser` | NONE | The default context menu is unmodified — no "open in browser", no "copy link". |

**Absent, worth noting:** no back/forward navigation in the body despite `QTextBrowser` supporting
it (`setOpenLinks(false)` plus manual handling means history is never populated), no print, no
"search within chapter", no external-links-open-in-browser confirmation, and no way to refresh the
help content without restarting the app.

## 3. Internal functions

| Name | Line | Purpose | Called from |
|---|---|---|---|
| `HelpWindow::HelpWindow(QTabWidget*, const QString& root, QWidget*)` | 34249 | Builds the UI and stylesheets; loads all five help Markdown files via `loadChapters`; walks the tab tree building `locs_` and `controls_`; wires search/selection/anchor signals; calls `doSearch("")` | `main()` 37428, 37561, 37568; `--selftest` 38485 |
| `loadChapters` lambda (`path, prefix, skipHead`) | 34311–34337 | Opens a Markdown file, splits on `"\n## "`, strips leading `#`s from titles, drops empty/"Contents" titles, truncates a chapter at any following `"\n# "` PART divider, and stores it into `chapters_` + `chapterOrder_` under `prefix + title` | ctor, five times (34338–34357) |
| `indexPane` lambda (`w, pane, loc`) | 34358–34372 | Recursively harvests non-empty `QPushButton` texts, `QCheckBox` texts, and every `QComboBox` item text from a pane widget into `controls_` with pane name, tooltip, and location index | ctor tab walk 34373–34382 |
| `doSearch(const QString& q)` | 34450 | The whole search/browse engine: empty query → full browse listing with pane headers; non-empty → chapter title/body matches then control label/tip/pane matches; sets row 0; renders the "no matches" or missing-chapters fallback HTML | ctor 34384 and 34411; `hitCount` 34414; `openWorkflows` (indirectly via `search_->clear()`) |
| `showResult(int r)` | 34540 | Dispatches a selected row: `-2` → workflow jump inside the Workflows chapter; other negatives → `showChapter(row.second)`; non-negative → renders the control HTML card with `pane:` and `chapter:` links | `results_` `currentRowChanged` 34386 |
| `showChapter(const QString& name)` | 34568 | `body_->setMarkdown(chapters_[name])` if present; silently no-ops otherwise | `showResult` 34544/34550; `openChapter` 34423; `openWorkflows` 34427 |
| `indexSize() const` | 34413 | `controls_.size() + chapters_.size()` | `--selftest` 38499, 38502 |
| `hitCount(const QString& q)` | 34414 | Runs `doSearch(q)` and returns `results_->count()` — a test-facing probe that mutates visible UI state | `--selftest` 38489, 38504, 38513 |
| `chapterCount() const` | 34418 | `chapters_.size()` | `--selftest` 38487, 38498, 38516 |
| `hasChapter(const QString& t) const` | 34419 | `chapters_.contains(t)` | `--selftest` 38511–38512 |
| `openChapter(const QString& title)` | 34423 | Public wrapper over `showChapter` | no in-tree caller found — **dead public API** |
| `openManual()` | 34419 (comment 34417) | `search_->setText("Manual: ")` | Help → User Manual… (37573) |
| `openWorkflows()` | 34424 | Shows the Workflows chapter, clears search/list/rows, adds the "(all)" header row, then regex-scans the chapter for `^\*\*(.+?)\*\*` bold lines, trimming a trailing period and skipping "Where do I start…", adding a `▸` row per workflow | Help → Suggested Workflows… (37565); `openWorkflowsCount` 34444 |
| `openWorkflowsCount()` | 34443 | `openWorkflows()` then `results_->count()` | `--selftest` 38490 |
| `struct Ctl { QString label, pane, tip; int tab; }` | 34447 | One indexed control | `controls_` |
| `struct Loc { int outer; QTabWidget* inner; int ii; }` | 34448 | An outer-tab / inner-tab coordinate for `pane:` links | `locs_` |

Members: `tabs_`, `search_`, `results_`, `body_`, `chapters_` (`QMap<QString,QString>`),
`chapterOrder_` (`QStringList`), `controls_` (`std::vector<Ctl>`), `locs_` (`std::vector<Loc>`),
`rows_` (`std::vector<std::pair<int,QString>>`) — 34572–34578.

## 4. Dependencies

**Data files** (all under the resolved data `root`, all read once at construction, all optional —
a missing file is a silent early `return` from `loadChapters`, 34315–34316):

| File | Chapter prefix | `skipHead` | Line |
|---|---|---|---|
| `<root>/data/help/tutorials.md` | *(none)* | `false` | 34338 |
| `<root>/data/help/USER_MANUAL.md` | `"Manual: "` | `true` | 34339–34340 |
| `<root>/data/help/GUIDELINES.md` | `"Guidelines: "` | `true` | 34351–34352 |
| `<root>/data/help/SOURCES.md` | `"Sources: "` | `true` | 34354–34355 |
| `<root>/data/help/STYLE.md` | *(none)* | `true` | 34357–34358 |

All five exist in the repo working tree (`data/help/`). `SOURCES.md` is noted in-code as carrying
"the hierarchy Geshe Michael ratifies … in his own words" — provenance-sensitive content rendered
through a generic Markdown path with no integrity check.

**Network hosts:** none of its own. It *can* open **any** `http`/`https` URL present in the help
Markdown, via `QDesktopServices::openUrl` (34408–34409) — the reachable host set is whatever the
data folder contains.

**QSettings:** **none.** `HelpWindow` reads and writes no settings keys — no persisted geometry, no
persisted splitter position, no "last chapter viewed", no "don't show again".

**Runtime dependencies:** the live main-window `QTabWidget*` passed in (it holds the pointer and
mutates the main window through `pane:` links); the resolved data `root` string; Qt Widgets and
`QRegularExpression`; `QDesktopServices`; `anchorPayload` (link-payload helper).

**Automated harness:** `ctest` test **`app_selftest`** (`app/CMakeLists.txt:89`). `HelpWindow` has
**no `selfTest()` method of its own** — its checks are written inline in `main()`'s selftest block
(38484–38520+): Suggested Workflows present, chapter count ≥ 16, auto-index > 150 entries, search
finds "Load into overlay", User Manual chapters loaded (≥ 40 `Manual: ` hits, `Manual: Overlay` and
`Manual: Lookup` present), plus a Guidelines check beginning at 38519.

## 5. Release questions

- **Arbitrary-URL launch from data files.** `anchorClicked` hands any `http`-prefixed anchor
  straight to `QDesktopServices::openUrl` with no allow-list and no confirmation. Ship as-is, or
  restrict to known hosts / show the destination first? This is the only outbound action in an app
  documented as having no telemetry.
- **Zero coverage on every interactive path.** All five automated checks probe the *index and
  search results*; not one exercises `showResult`, the `pane:` link, the `chapter:` link, the
  `http` link, or the workflow jump. Should `HelpWindow` get a real `selfTest()` like its siblings?
- **The control auto-index is frozen at first open.** The window is a lazily created, never-deleted
  singleton, and `controls_`/`locs_` are built once in the constructor. Confirm no pane is created,
  relabelled, or reordered after the user's first ⌘? — otherwise the index and every `pane:` link
  go stale for the rest of the session.
- **`pane:` links navigate the main window and then `raise()` the help window over it.** Is that
  the intended feel, or should the help window step back after navigating?
- **The 6-character prefix heuristic** at 34558–34560 (`c.pane.startsWith(t.left(6))`) decides which
  tutorial a control links to. Verify what it actually pairs across the real pane/chapter names —
  it is very likely mispairing or, more often, linking nothing.
- **"Manual" and "Workflows" are string-literal couplings.** `openManual()` searches for the
  literal `"Manual: "`; `openWorkflows()` hard-codes the chapter title `"Suggested Workflows"` and
  a `^\*\*(.+?)\*\*` bold-line convention. Editing the Markdown headings or un-bolding a workflow
  name breaks a menu item silently. Should these be asserted at startup?
- **All five help files fail silently when missing.** Only the *total absence of every chapter*
  produces the "Help chapters not found" message (34478–34490); losing, say, `SOURCES.md` alone is
  invisible. Should the window report which help files it could not load?
- **No search debounce.** Every keystroke re-scans every chapter body plus the whole control vector.
  Measure on the shipped manual — if it is perceptible, add the same 220 ms debounce the Hunt
  palette uses.
- **`hitCount()` and `openWorkflowsCount()` are test probes that mutate visible UI.** They are
  public and unguarded. Keep them public in the shipping binary, or move behind a test-only guard?
- **`openChapter()` is public and has no caller.** Remove, or wire it to something (a deep-link
  from the Hunt palette would be the obvious use).
- **`"Welcome. Pick a chapter."`** is the shipped fallback body text when "Getting Started" is
  missing (34492–34494). Confirm `tutorials.md` really does define a `## Getting Started` chapter.
- **Nothing persists** — window geometry, splitter position, last chapter. Intentional for release?
- **`QTextBrowser::find` result discarded** in the workflow jump (34547). Should a miss say so?
