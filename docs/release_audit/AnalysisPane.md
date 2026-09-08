# AnalysisPane — release audit

Audited against `app/main.cpp` lines 3616–3870 (class body), plus
`core/src/analysis.cpp`, `core/include/allcore/analysis.h`,
`core/tests/analysis_smoke.cpp`, `core/CMakeLists.txt`. No source file was
modified.

## 1. What the pane is for

`AnalysisPane` produces a labeled-AI reading of a single ACIP passage against
the HGM spine: the C++ engines run a deterministic pre-pass (term anchors,
corpus concordance, optional reference-library citations), that evidence is
assembled into a prompt from a markdown template, the prompt is sent to the
Anthropic Messages API as a streaming request, and the returned report is then
put through a machine validation pass (`allcore::validateReport`) that checks
each claim against the corpus. Its own banner states the contract: "AI output is
always labeled; nothing enters the corpus from here." It is a **top-level tab
labeled "Analysis"** in the main window's `QTabWidget`, constructed at
`app/main.cpp:35856` —
`tabs.addTab(new AnalysisPane(spine, tplPath, root + "/analyses"), "Analysis")` —
sitting between the "Export" and "Trainer" tabs. The user reaches it by clicking
that tab; there is no menu item, shortcut, or command-palette route into it. It
is constructed unconditionally, whether or not an API key exists; when no key is
found the Analyze button is disabled and the status label explains where to put
one.

## 2. User-visible functions

| Control/action | What it does (from the code) | Code anchor (main.cpp line) | Automated coverage | Notes/risks |
|---|---|---|---|---|
| "Passage (ACIP)" text box | Multiline `QPlainTextEdit` with placeholder "Paste the ACIP passage here…"; its content is the passage sent to the API. Contents persist across launches via `sess::remember(passage_, "analysis/passage")` (debounced 1.5 s, 256 KB cap) | ctor, 3633–3636 | NONE | The pasted passage is written to `QSettings("ALL","TranslationTool")` under `sess/analysis/passage` — plaintext user work persisted to disk with no visible indication |
| "Draft English (optional)" text box | Multiline `QPlainTextEdit`; if non-empty its text is appended to the passage inside the `<<INPUT START>>` block of the user prompt. Persisted as `sess/analysis/draft` | ctor, 3638–3640 | NONE | Same silent persistence; also silently uploaded with the passage |
| "Analyze" button (`analyze_`) | `connect(analyze_, &QPushButton::clicked, [this] { run(); })` — runs the engine pre-pass, builds the prompt, and POSTs a streaming request to the Anthropic API | ctor 3641/3678; `run()` 3681 | NONE for the pane; indirect: ctest `analysis_smoke` covers `runPrePass` / `buildAnalysisPrompt` / `buildMessagesRequestJson` offline. The main selfTest block has one *adjacent* pin ("harness runs never see the Anthropic API key", 38779–38788) | **Release-critical.** A single click issues a paid API call with no confirmation step. Disabled at construction only if `anthropicKey()` is empty (3671–3677); re-enabled in `onDone` |
| Analyze with empty passage | `status_->setText("Paste a passage first.")` and returns without any network call | `run()`, 3683–3686 | NONE | Correct guard |
| Status label (`status_`) | Word-wrapped `QLabel` reporting, in sequence: the missing-key message; "engine: N anchors, N corpus hits, N library citation(s) — asking Claude…"; refusal/API-error/network-error text; truncation notice; and the final "done — …" plus save verdict | 3639–3646, 3672, 3730, 3819+, 3855 | NONE | This is the only error surface — there are no message boxes except the one inside `saveOrWarn` |
| Report browser (`report_`) | `QTextBrowser` with `setOpenExternalLinks(false)`; streams the model's text as plain text during the response, then re-renders as markdown via `setMarkdown` when complete | 3649–3655, `onData()` 3796, `onDone()` 3830 | NONE | `setOpenExternalLinks(false)` is set but **no `anchorClicked` handler is connected**, so links in AI-generated markdown are inert rather than handled — safe by omission, not by design |
| QC / validation browser (`qc_`) | Second `QTextBrowser` below the report; filled by `validateReport` → `findingsToMarkdown` after a complete response | 3656–3660, `onDone()` 3831–3833 | NONE | Only populated on a complete stream; deliberately left empty on truncation |
| Horizontal + vertical splitters | Left column (inputs) vs right column; right column splits report (stretch 4) over QC (stretch 1); user-draggable | 3630, 3648, 3661–3670 | NONE | Splitter positions are not persisted |
| Auto-save of the analysis | On a complete response, writes `<saveDir>/analysis-yyyyMMdd-HHmmss.md` containing the source passage in an HTML comment, the AI report, and the QC findings; `saveDir_` is `<root>/analyses` | `onDone()`, 3835–3854 | NONE (ctest `shortwrite` covers the `saveOrWarn` helper generally) | Happens **without asking**; write failures are surfaced through `saveOrWarn` (modal once per path) and named in the status line as "NOT auto-saved (see warning)" |

Absent by grep across 3616–3870: no `addAction(`, `QToolButton`, `QCheckBox`,
`QComboBox`, `setShortcut`, `setToolTip`, `customContextMenu`, `anchorClicked`,
`QProcess`, drag/drop, or double-click handlers. No `TODO` or `FIXME`. Direct
`QSettings` use is only via the `sess::` helpers; direct `QFile` use is only via
`saveOrWarn`, with `QFileInfo::exists` used to probe the library index.

**Count: 10 user-visible functions/controls; 10 with NONE direct coverage.**

## 3. Internal functions

| Name | Line | Purpose | Called from |
|---|---|---|---|
| `AnalysisPane(allcore::Spine&, QString templatePath, QString saveDir)` | 3618 | Builds the whole UI, wires Analyze, and disables the button with an explanatory status when `anthropicKey()` is empty | `app/main.cpp:35856` |
| `run()` | 3681 | Guards empty input, disables the button, clears state, runs `allcore::runPrePass`, optionally augments with `LibraryIndex` hits, builds the prompt and JSON body, sets headers and the 60 s transfer timeout, POSTs, and connects `readyRead`/`finished` | Analyze click lambda (3678) |
| `onData()` | 3789 | Buffers the SSE stream, splits on newlines, keeps only `data:` lines, appends `sseTextDelta` output to `accum_`, live-renders and autoscrolls; also sets `complete_` on `message_stop`, and sets status text on `stop_reason:"refusal"` and `"type":"error"` | `QNetworkReply::readyRead` |
| `onDone(const std::string& passage)` | 3817 | If `message_stop` was never seen but text arrived, renders the fragment under a loud TRUNCATED banner, does **not** QC and does **not** save; otherwise renders markdown, runs `validateReport`, renders findings, counts errors, composes and auto-saves the .md via `saveOrWarn`, sets the final status, deletes the reply, re-enables Analyze | `QNetworkReply::finished` |

Members: `spine_`, `templatePath_`, `saveDir_`, the six widgets, `TimedNam net_`,
`QNetworkReply* reply_`, `QByteArray buf_`, `std::string accum_`, `bool complete_`.

## 4. Dependencies

**Files / folders read**
- The spine database, via the injected `allcore::Spine&` (resolved in `main` at
  `app/main.cpp:35648`, default `resolveSpinePath(root)`).
- The prompt template: `templatePath_`, defaulting to
  `<root>/docs/analysis/PASSAGE_ANALYSIS_TEMPLATE.md` (`app/main.cpp:35651`),
  overridable by `argv[2]`. `buildAnalysisPrompt` throws
  `"cannot read template: …"` if unreadable; the pane catches it and shows
  `error: …` (3784–3787).
- The reference-library index, if present: `QFileInfo(saveDir_).path() +
  "/library/.index.db"` — i.e. `<root>/library/.index.db` — searched for up to 4
  multi-word anchors, 3 hits each, snippets truncated to 160 chars (3697–3728).
  Failure to open is swallowed on purpose (`// index unreadable → honest
  fallback text`) because the prompt has a documented no-hits branch.

**Files written**
- `<root>/analyses/analysis-yyyyMMdd-HHmmss.md` (directory created with
  `QDir().mkpath`).

**Settings keys written/read** — `QSettings("ALL", "TranslationTool")`:
`sess/analysis/passage`, `sess/analysis/draft`. Both suppressed when
`g_harnessRun` is set.

**Environment variables read** — `ANTHROPIC_API_KEY` (indirectly, via
`anthropicKey()`).

**Network hosts** — exactly one: `api.anthropic.com` (HTTPS). No other host is
contacted by this pane.

### Anthropic API call

**Key source** — `anthropicKey()` at `app/main.cpp:2816`, in priority order:

1. If `g_harnessRun || g_sweepActive`, return empty — harness/sweep runs are
   deliberately blinded so an automated click of Analyze cannot bill.
2. `qgetenv("ANTHROPIC_API_KEY")`, trimmed, if non-empty.
3. A plaintext file, first match wins:
   `~/Library/Application Support/Diamond Cutter Translation Tool/anthropic_api_key`,
   then the legacy `~/Library/Application Support/ALL Translation Tool/anthropic_api_key`.

No macOS Keychain, no `QSettings` storage, no in-app entry field. The key lives
either in the environment or as a plaintext file on disk with whatever
permissions the user's umask gave it. The pane's own no-key message (3673–3677)
names only the **legacy** "ALL Translation Tool" directory, not the current
"Diamond Cutter Translation Tool" one — a user following that instruction lands
in the fallback path, which still works but is documented as deprecated.

**URL** — `https://api.anthropic.com/v1/messages`, `POST` (3745, 3757).

**Headers** (3746–3750):
```cpp
req.setRawHeader("x-api-key", anthropicKey());
req.setRawHeader("anthropic-version", "2023-06-01");
req.setRawHeader("anthropic-beta", "server-side-fallback-2026-07-01");
req.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
```

**Model and body** — built by `allcore::buildMessagesRequestJson`
(`core/src/analysis.cpp:216`), whose defaults come from
`core/include/allcore/analysis.h:87`: `model = "claude-opus-5"`,
`max_tokens = 32000`. The pane calls it with **no overrides**, so both defaults
apply. The serialized body:
```cpp
j += "\"model\":\"" + jsonEscape(model) + "\",";
j += "\"max_tokens\":" + std::to_string(max_tokens) + ",";
j += "\"stream\":true,";
j += "\"fallbacks\":\"default\",";
j += "\"system\":[{\"type\":\"text\",\"text\":\"" + jsonEscape(prompt.system) +
     "\",\"cache_control\":{\"type\":\"ephemeral\"}}],";
j += "\"messages\":[{\"role\":\"user\",\"content\":\"" + jsonEscape(prompt.user) +
     "\"}]";
```
(JSON is hand-serialized through `jsonEscape`, not `QJsonDocument`.)

**What user/document text is transmitted** — from `buildAnalysisPrompt`
(`core/src/analysis.cpp:109`):
- `system`: the contents of `PASSAGE_ANALYSIS_TEMPLATE.md` **after** the first
  `\n---\n` divider (the implementation preamble is stripped and not sent).
- `user`: engine-derived evidence followed by the verbatim user text —
  every HGM term anchor (`acip`, `wylie`, `tibetan`, pronunciation, tier, and
  glosses) matched in the passage; every corpus concordance hit including
  **HGM's Tibetan and his published English** (`"- [" + h.course + ":" + seq +
  "] T: " + h.wylie + "\n  E: " + h.english`); every reference-library hit as
  `file:line` plus a text snippet — meaning **local filesystem paths from the
  user's library are sent to the API**; and finally:
  ```cpp
  u += "\n━━━…━━━\n<<INPUT START>>\n\n";
  u += acip_passage;
  if (!draft_english.empty()) u += "\n\n" + draft_english;
  u += "\n\n<<INPUT END>>\n";
  ```
  i.e. the complete pasted passage and the complete optional English draft,
  verbatim.

**Timeout** — `req.setTransferTimeout(60000)` (60 s), explicitly overriding the
20 s default that `TimedNam` (`app/main.cpp:39`) applies to every network
manager in the file. The in-code comment records that before this fix the
request could hang forever, leaving the Analyze button permanently greyed.

**Error handling** — surfaced, not swallowed, in four distinct ways:
`stop_reason:"refusal"` → "The safety classifiers declined this request…";
`"type":"error"` → `API error:` plus the first 300 chars of the raw SSE line;
transport failure with no text → `network error: ` + `errorString()`; a stream
that ended without `message_stop` → the report is replaced by a TRUNCATED banner,
QC is skipped and nothing is saved. Only two things are deliberately silent: an
unreadable library index, and `sseTextDelta` returning "" for a non-text delta.

## 5. Coverage

**AnalysisPane does NOT appear in the `--selftest` dispatch block at
`app/main.cpp:38250–38290` — verified.** That block calls `selfTest()` on
overlay, exportPane, scansPane, ocrPane, SettingsDialog, apparatusPane,
trainerPane, drillsPane, draftPane, manuscriptPane, reviewPane, alignPane,
libraryPane, inputPane, goferPane, sanskritPane, proposePane, plus inline Lookup
checks. `AnalysisPane` has **no `selfTest` member at all**, so nothing in the
harness constructs, clicks, or asserts on it. The one related pin lives later in
the same harness (38779–38788) and asserts the *inverse*: that `anthropicKey()`
is empty during harness runs so a sweep cannot fire a paid call —
`[PASS] Safety: harness runs never see the Anthropic API key`.

`grep -n "add_test" CMakeLists.txt cmake/*.cmake core/CMakeLists.txt`: the root
`CMakeLists.txt` has no `add_test` calls (only comments about the wrapper), and
`cmake/AllFixtureTests.cmake` merely *overrides* `add_test` to skip suites with
missing fixtures. All suites live in `core/CMakeLists.txt`. The only one touching
this feature is **`analysis_smoke`** (`core/CMakeLists.txt:97`, source
`core/tests/analysis_smoke.cpp`), which runs entirely offline and asserts:
tokenizer behaviour, four expected anchors, the `ka ba` → "pillar" gloss,
template presence in the system prompt, that the implementation preamble is *not*
sent, anchor and passage injection into the user prompt, and that the body
carries `"model":"claude-opus-5"`, `"stream":true`, `cache_control`, and
`"fallbacks":"default"`. **It exercises `allcore` only — it never constructs
`AnalysisPane`, never makes a network call, and covers none of the pane's UI,
streaming, truncation, QC, or auto-save paths.**

## 6. Release questions

- **Consent.** A single Analyze click ships the user's passage, their English
  draft, HGM corpus English, and local library file paths to a third-party API
  with no confirmation dialog and no per-run cost estimate. Does the release want
  a one-time (or per-session) consent gate?
- **Key storage.** The key is a plaintext file under `~/Library/Application
  Support/…` or an env var — no Keychain, no file-permission check on read, and
  no in-app way to enter or clear it. Acceptable for the demo build?
- **Stale instruction text.** The no-key status message names only the legacy
  `ALL Translation Tool` directory, while `anthropicKey()` prefers
  `Diamond Cutter Translation Tool`. Worth correcting before release.
- **Data egress of file paths.** Reference-library hits are sent as
  `file:line` — real paths from the user's disk. Confirm this is intended.
- **Cost.** `max_tokens = 32000` on `claude-opus-5` per click, with a full
  corpus-evidence prompt. There is no cap, no counter, and no cancel button
  (the Analyze button is disabled for the duration; the only bound is the 60 s
  transfer timeout).
- **Offline behaviour.** With a key present but no network, the user waits the
  full 60 s before seeing `network error:`. No cancel affordance exists.
- **No cancel / no re-entrancy guard beyond the disabled button.** If `onDone`
  is somehow not reached, `reply_` is never cleared and the pane stays dead for
  the session.
- **Zero UI-level automated coverage.** Truncation handling, QC rendering, and
  auto-save-failure messaging are all release-critical honesty behaviours with
  no test. Consider an `AnalysisPane::selfTest` that drives `onData`/`onDone`
  with canned SSE bytes — no key and no network required.
- **`anthropic-beta: server-side-fallback-2026-07-01`.** Verify this beta header
  and the `"fallbacks":"default"` body field are still valid at ship date; a
  rejected beta header would surface only as an `API error:` string.
