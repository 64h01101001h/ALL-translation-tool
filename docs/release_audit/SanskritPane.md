# SanskritPane — release audit

*(app/main.cpp lines 17311–17512; audited 2026-09-08)*

## 1. Purpose and placement

`SanskritPane` is the app's Sanskrit workbench: one input box that auto-detects IAST, Devanagari, or ACIP Sanskrit input-code (`a#`/`n%`/`n~`), and one read-only report showing the term in *every* notation the project supports — ACIP input-code, next-letter style, Tibetanized ACIP, Tibetan script, Devanagari, IPA — plus Whitney's *Roots* (1885) reference hits and the Mahāvyutpatti bridge, and an outbound link to Monier-Williams at Cologne. It is constructed unconditionally at `app/main.cpp:36068` and added as the flat tab `"Sanskrit"` at `:36069`; the surface-band regrouping at `:36560` files it into the **Research** group (`mkGroup("Research", {"Search", "Lookup", "Sanskrit", "Convert", "Analysis"})`). It is **visible by default — not gated behind any flag, setting, or identity role** (unlike the neighbouring `OCR` tab, which is `#ifdef ALL_HAVE_OCR`, and the `Approval` tab, which is role-gated). The user reaches it as Research → Sanskrit.

## 2. User-visible functions

| Control / action | What it does | Code anchor (main.cpp) | Automated coverage | Notes / risks |
|---|---|---|---|---|
| Query line edit `in_` (placeholder `pramāṇa · नमः · prama#n%a · root: bhū`) | Free-text term entry; auto-detects Devanagari / input-code / IAST at analyze time; contents persisted to session memory | ctor, `:17328–17333`; `sess::remember(in_, "sanskrit/query")` `:17331` | indirectly: `app_selftest` drives it (`in_->setText(...)` `:17364`, `:17376`); `gauntlet_walk` random walk | The "root: bhū" hint in the placeholder is **not implemented** — a literal `root: bhū` string is fed to the converters unparsed; only bare `bhū` reaches Whitney. Misleading affordance. |
| **Analyze** button | Runs `analyze()` — full notation table + Whitney + Mvp + Cologne link | `:17336–17337`, connect `:17348` | `app_selftest`: `"input-code style renders"`, `"Devanagari renders"`, `"Whitney roots reachable"` | — |
| Return key in the line edit | Same as Analyze | connect `:17349` | NONE (self-test calls `analyze()` directly, not via the signal) | Signal wiring itself untested. |
| **Sanskrit OCR…** button | File dialog → runs external `tesseract -l san` on a chosen image, pushes recognized Devanagari into the input, analyzes, and prefixes an `⚠ OCR-DERIVED — review material` banner | `:17338–17343`, connect `:17350`; `runOcr()` `:17469–17509` | **NONE** | Spawns an external process with a 60 s blocking `waitForFinished` on the GUI thread; degrades to a red "needs tesseract + san.traineddata" message. Hard-codes Homebrew tessdata paths. |
| Result browser `out_` (QTextBrowser) | Renders the notation table, Whitney block, Mvp block, Cologne link; `setOpenExternalLinks(true)` | `:17344–17347` | content asserted by the three self-test checks | External links open in the system browser with **no confirmation** — the only network egress in the pane. |
| Notation row: *input-code* | `allcore::iastToInputcode` | `analyze()` `:17418` | `app_selftest` `"input-code style renders"`; core `engines_battery` `E: iastToInputcode("pramāṇa") == "prama#n%a"` + Battery F full-LC diff vs Python oracle | strongest-covered row |
| Notation row: *next-letter* | `allcore::iastToNextletter` | `:17421` | `engines_battery` (`prama-n.a`, Battery F) — **no app-level check** | |
| Notation row: *Tibetanized ACIP* (⚠ on failure) | `allcore::iastToAcip` | `:17424` | `engines_battery` (`E: iastToAcip`, Battery F) — no app-level check | |
| Notation row: *Tibetan script* (⚠ on failure) | `allcore::iastToTibetan` | `:17427` | `engines_battery` (`E:` oṃ maṇi padme hūṃ, Battery F) — no app-level check | |
| Notation row: *Devanagari* (⚠ on failure) | `allcore::iastToDevanagari` | `:17432` | `app_selftest` `"Devanagari renders"`; `engines_battery` E + F | |
| Notation row: *IPA* (⚠ on failure) | `allcore::iastToIpa` | `:17437` | `engines_battery` IPA cases (`saṃgha`, `oṃ maṇi padme hūṃ`, `saṃsāra`, `namaḥ`) — **no app-level check** | This is the pane's *pronunciation* surface; the header labels it "IPA" while the engine header calls the sibling `iastToPronunciation` "provisional". See §5. |
| Devanagari input path | `allcore::devanagariToIast`; annotates "(partial Devanagari decode)" when not clean | `:17392–17395` | NONE directly (self-test never types Devanagari) | The `ok=false` case degrades to a small parenthetical, not a flag. |
| ACIP input-code input path | `allcore::inputcodeToIast`; annotates "(from ACIP input-code)" | `:17396–17399` | `engines_battery` `"E: roundtrip inputcode -> iast"` — **no app-level check** | Detection is a bare `#`/`%`/`~` character test; an IAST string containing those chars is misrouted. |
| Whitney roots block | `whitney_->byRoot()`, falling back to `byPpp()`; up to 6 hits, labelled "(reference)" | `:17442–17459` | `app_selftest` `"Whitney roots reachable"` (only when `whitney_` non-null); core `whitney_smoke` | Silently absent when the data file is missing — the self-test check is skipped too, so absence is invisible. |
| Mahāvyutpatti block | `mvp_->byIast()` → shared `mvpHtml()` (`:1951`) | `:17460–17463` | core `mvp_smoke` — **no app-level check**, and no self-test guard for `mvp_==nullptr` | |
| Monier-Williams (Cologne) link | Percent-encoded IAST into `sanskrit-lexicon.uni-koeln.de` MWScan URL | `:17464–17472` | NONE | Sends the user's query term to a third-party host on click; `input=SLP1` is passed while the term is IAST — likely a wrong-parameter bug. |

**Counts: 17 user-visible functions/controls; 6 with NONE app- or core-level coverage** (Return-key wiring, OCR button, result-browser external links, Devanagari input path, Cologne link, and — counting engine-level batteries as coverage — the OCR banner path).

## 3. Internal functions

| Name | Line | Purpose | Called from |
|---|---|---|---|
| `SanskritPane::SanskritPane(Mvp*, WhitneyRoots*)` | 17316 | Builds banner, input row, Analyze/OCR buttons, result browser; wires three connects | `main()` `:36068` |
| `SanskritPane::selfTest(QStringList&)` | 17354 | Three `check(...)` assertions (input-code, Devanagari, Whitney); clears the box afterwards | `main()` `:38271` (`app_selftest`) |
| `SanskritPane::analyze()` | 17380 | Detect notation → convert through six allcore entry points → build HTML table → append Whitney, Mvp, Cologne link | Analyze button, returnPressed, `runOcr()`, `selfTest()` |
| `SanskritPane::runOcr()` | 17469 | Locate `tesseract` + `san.traineddata`, open image, run OCR, feed result to `analyze()`, prepend OCR-derived warning | OCR button |
| local lambda `row(k,v)` | 17400 | Formats one HTML table row | `analyze()` |
| local lambda `check(ok,what)` | 17356 | Logs PASS/FAIL, increments `fails` | `selfTest()` |

## 4. Dependencies

**allcore entry points:** `devanagariToIast`, `inputcodeToIast`, `iastToAcip`, `iastToTibetan`, `iastToDevanagari`, `iastToIpa`, `iastToInputcode`, `iastToNextletter` — all declared in `core/include/allcore/sanskrit.h`, implemented in `core/src/sanskrit.cpp` (the C++ port of `engines/sanskrit_convert.py`). Also `allcore::Mvp::byIast`, `allcore::WhitneyRoots::byRoot/byPpp`.

**Data files (loaded by the owner, not the pane):** `data/whitney/whitney_roots.tsv` (via `WhitneyRoots`, cf. `core/CMakeLists.txt:159` `whitney_smoke`); `data/extracted/mahavyutpatti.tsv` (via `Mvp`, `core/CMakeLists.txt:269` `mvp_smoke`). The pane holds raw pointers and tolerates null for both.

**External binaries / paths (OCR path only):** `tesseract` via `QStandardPaths::findExecutable`, else hard-coded `/opt/homebrew/bin/tesseract`, `/usr/local/bin/tesseract`; requires `/opt/homebrew/share/tessdata/san.traineddata` (Homebrew prefix hard-coded — Intel/`/usr/local` and Linux installs fail the check even with the model present). User-chosen image file via `safeGetOpenFileName` (`:3356`).

**Network hosts:** `https://www.sanskrit-lexicon.uni-koeln.de` (Monier-Williams, click-through via `setOpenExternalLinks(true)`). No other egress; no in-pane HTTP.

**QSettings keys:** one — `ALL/TranslationTool` → `sess/sanskrit/query` (last query text, written on every keystroke via `sess::remember`, suppressed under `g_harnessRun`).

**Test suites:** `app_selftest` (app/CMakeLists.txt, drives `selfTest`), `gauntlet_walk` (`--gauntlet auto 300`, random UI walk; touches this pane nondeterministically), `engines_battery` (core, Batteries E/F + IPA), `whitney_smoke`, `mvp_smoke`, `acipcodes_smoke`.

## 5. Release questions

- **Should the provisional pronunciation style ship?** The pane exposes only `iastToIpa`, labelled plainly "IPA", and never calls `iastToPronunciation` — whose header comment marks the reader-pronunciation table as provisional pending the ACIP pronunciation standards doc. Decide explicitly: (a) ship IPA only, as now, and confirm the IPA table itself is standards-backed, or (b) add the reader pronunciation row with a visible PROVISIONAL label per the honesty rules. Shipping an unlabelled "IPA" row that users read as authoritative pronunciation is the risk either way.
- The placeholder advertises `root: bhū` but no root-prefix parsing exists. Implement it or change the placeholder before release.
- The Cologne link passes `input=SLP1` while sending IAST. Verify against the live endpoint or drop the parameter.
- Should a third-party lookup link fire without confirmation? It transmits the user's query term off-machine on click.
- OCR blocks the UI for up to 60 s and has zero automated coverage; its Homebrew-only tessdata probe means the feature silently reports "not installed" on Intel macOS and Linux. Ship, hide, or fix the probe.
- The Devanagari and input-code decode failures degrade to small grey parentheticals rather than the project's `⟨…⟩`/review flag conventions — reconcile with inviolable rule 3 (never guess; flag).
- `mvp_ == nullptr` and `whitney_ == nullptr` are silently invisible in the UI *and* skip their self-test check, so a missing data payload passes the battery. Consider a payload-presence assertion.
