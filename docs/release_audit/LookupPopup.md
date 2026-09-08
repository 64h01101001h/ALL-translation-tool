# Release audit — `LookupPopup` (app/main.cpp:2595–2703)

## 1. What it is and how it is reached

`LookupPopup` is the app-wide floating dictionary window (`QDialog` with `Qt::Tool`,
540×500, titled "Lookup — ⌘D") — the reborn TibetDoc dictionary window. It is not a
tab: exactly one instance is created as a function-local `static` in `main()`
(app/main.cpp:36065) right after the Lookup tab is added, and its address is parked in
the file-scope pointer `g_lookupPopup` (app/main.cpp:2704, assigned at 36067). The user
reaches it by selecting text in ANY text surface in the app and pressing ⌘D
(`Ctrl+D`), wired as the View-menu action "Look Up Selection…" with
`Qt::ApplicationShortcut` (app/main.cpp:37247–37264); that handler pulls the selection
from whichever of `QPlainTextEdit` / `QTextEdit` / `QTextBrowser` / `QLineEdit` has
focus and calls `run(sel)`. An empty selection just raises the window with its search
box focused. All content comes from the shared `lookupResultsHtml()`
(app/main.cpp:2054) — the same stack the Lookup tab renders — so the popup owns no
dictionary logic of its own, only navigation state (`alignCursor_`, `alignKey_`).

## 2. User-visible functions

| Control/action | What it does | Code anchor | Automated coverage | Notes/risks |
|---|---|---|---|---|
| ⌘D global shortcut ("Look Up Selection…") | Reads the selection from the focused text widget and calls `LookupPopup::run()`; raises the window | View-menu lambda, main.cpp:37247–37264; `run` main.cpp:2655 | `"⌘D popup answers with the full stack"` (main.cpp:38397, in the app-`--selftest` Lookup block; drives `run("bsod nams")` + `viewText()`) | The selftest calls `run()` directly, NOT the menu action — the focus-widget extraction path (4 casts) is uncovered. Non-text focus ⇒ empty query ⇒ window merely raised, no message. |
| Search box (`QLineEdit`, placeholder "wylie · ACIP · English · pronunciation…") + Return | `returnPressed` → `run(box_->text())` — re-renders the whole stack for the typed term | ctor lambda main.cpp:2618–2619; `run` 2655 | NONE (no test presses Return; only `run()` is called directly) | No session memory: unlike other panes this box is not passed through `sess::remember` — the last query is forgotten at quit. |
| Results view (`QTextBrowser`, `setOpenLinks(false)`) | Renders `lookupResultsHtml(...)` HTML; all links routed through the app's own `anchorClicked` handler | ctor main.cpp:2610–2616 | Indirectly: `"known term returns HGM equivalents"`, `"HGM binding layer precedes reference layers"`, `"honorific term carries its badge"` (main.cpp:38282–38293) test the HTML, not the widget | Empty-state italic placeholder text only until first query. |
| Link `alignprev:` / `alignnext:` (TENTATIVE evidence stepper) | Decrements/increments `alignCursor_` then re-runs the query; the card normalises out-of-range integers | anchor handler main.cpp:2622–2633 | `"ALIGN-2: the TENTATIVE block steps through attested witnesses in context"` (main.cpp:40092) — exercises `entryHtml`, not the popup's handler | Cursor is unbounded here; correctness depends entirely on the card clamping. Shared with the Overlay's duplicate handler at main.cpp:7360 — two copies of the same logic. |
| Link `aligncur:<n>` (jump to a listed rendering) | Sets `alignCursor_ = n` from the anchor tail, re-runs | main.cpp:2628–2630 | `"ALIGN-2: the cursor changes the shown witness; renderings are jump targets"` (main.cpp:40095) | `toInt()` failure silently yields 0 — a malformed anchor resets the stepper without saying so. |
| ← / → key (plain or ⌥) on the results view | Same stepper via `eventFilter`; consumes the key | `eventFilter` main.cpp:2678–2691 | NONE | Steals ← → from the browser's own caret navigation whenever the view has focus. Keypad modifier stripped; other modifiers fall through. |
| Link `term:<wylie>` (q.v. cross-reference) | Percent-decodes the payload and looks that term up in place | main.cpp:2634–2635; `anchorPayload` main.cpp:259; anchors emitted at main.cpp:1391 | NONE (no selftest clicks a `term:` link) | No history/back — a q.v. hop cannot be undone except by retyping. |
| Link `das:<page>` (Das 1902 facsimile) | Opens a modeless `QPdfView` dialog at that PDF page, then re-runs the query | main.cpp:2640–2642; `showDasPage` main.cpp:29788 | NONE | Needs `data/das/das_1902_bookmarked.pdf`; missing file ⇒ information box. Without `ALL_HAVE_QTPDF` the whole feature is a "build lacks the PDF viewer" box. Disk read of a large PDF on the GUI thread. |
| Link `jae:<page>` (Jäschke 1881 facsimile) | Same, for `data/das/jaeschke_1881.pdf` | main.cpp:2637–2639; `showJaePage` main.cpp:29752 | NONE | Same as above; PDF only registered when the file exists at startup (main.cpp:36055–36061). |
| Link `propose:<wylie>` (propose to the authority) | Opens `proposeTermDialog` with evidence string "looked up in the ⌘D dictionary window"; files a proposal to the approval queue | main.cpp:2643–2649; `proposeTermDialog` main.cpp:2510 | `"propose-to-the-authority action offered"` (main.cpp:38401) — asserts the anchor is PRESENT; the dialog itself is not driven | Writes to the proposals store on disk via `fileProposal`. Two chained `QInputDialog`s; cancel at either step aborts silently (correct). Not sweep-guarded here. |
| Link `proposeai:<wylie>` (promote AI draft) | Opens `promoteAiDraftDialog` — human edits the AI draft gloss, files it as their own word-rendering proposal with full provenance | main.cpp:2650–2652; `promoteAiDraftDialog` main.cpp:2564; anchors emitted main.cpp:1402 | NONE | Silently returns if `g_aiGlossary` is null or the wylie is absent — a click that does nothing at all. Provenance string is composed here; it is the rule 1 firewall between AI drafts and HGM glosses. |
| Link `http…` / `https…` (Adarsha, Warnemyr, archive.org, BDRC) | Hands the URL to `QDesktopServices::openUrl` — leaves the app for the system browser | main.cpp:2653–2654; anchors at main.cpp:348, 361, 405, 1926 | `"Adarsha link-out carries a unicode query"` (main.cpp:38299) — tests URL construction only | Outbound network by the user's browser (adarshah.org, warnemyr.com, archive.org, library.bdrc.io). No confirmation, no offline detection. Return value of `openUrl` discarded. |
| Window close / hide | Ordinary `QDialog` close; the static instance survives and reopens with prior state | implicit (`QDialog`) | `g_lookupPopup->hide()` at main.cpp:38399 (cleanup, not an assertion) | State (`alignCursor_`, box text) persists across hides — intended, but never persisted to disk. |

## 3. Internal functions

| Name | Line | Purpose | Called from |
|---|---|---|---|
| `LookupPopup(Spine&, RefDict*, Mvp*, WhitneyRoots*, ColloquialPron*, QWidget*)` | 2597 | Builds the dialog (box, browser, event filter) and wires every anchor scheme | `main()` main.cpp:36065 |
| `run(const QString&)` | 2655 | Normalises the query (strips U+2029, `simplified()`), resets the align stepper on a new headword, renders `lookupResultsHtml`, shows + raises | ⌘D action (37263), every anchor branch, selftest (38396) |
| `viewText() const` | 2676 | Plain-text dump of the results browser | selftest only (main.cpp:38397) |
| `eventFilter(QObject*, QEvent*)` | 2678 | Turns ← / → on the results view into stepper moves | Qt, via `view_->installEventFilter(this)` (main.cpp:2617) |
| `alignCursor_` (public data) | 2692 | TENTATIVE-block cursor | anchor handler, eventFilter, `run` |
| `alignKey_` (public data) | 2693 | The headword the cursor belongs to; a change resets the cursor | `run` |

Note: everything between main.cpp:2704 and 3616 (`editionDiffHtml`, `anthropicKey`,
`namespace sess`, `warnWriteFail` / `saveOrWarn` / `writeAllOrWarn` / `warnOpenFail`,
`usage::`, `bdrcErrorLine`, `jsonObjectOrFlag`, `restoreStoreWithSafety`, `sanDerate`,
`streamWriteOk`, `makeLookupPane`) is free-function file scope, **not** members of
`LookupPopup`, and is audited with its own owners.

## 4. Dependencies

- **Data**: the spine DB (`allcore::Spine`, injected), `reference.db` via `RefDict*`,
  `Mvp*`, `WhitneyRoots*`, `ColloquialPron*` — all injected by `main()`, all read-only here.
- **Files read**: `<root>/data/das/jaeschke_1881.pdf`, `<root>/data/das/das_1902_bookmarked.pdf`
  (existence checked at startup, main.cpp:36055; re-checked in `showJaePage`/`showDasPage`).
- **Files written**: none directly. Indirectly via `propose:` / `proposeai:` →
  `fileProposal` (proposals store).
- **Network**: none from this class. `QDesktopServices::openUrl` delegates to the system
  browser for `online.adarshah.org`, `www.warnemyr.com`, `archive.org`, `library.bdrc.io`.
- **QSettings** (read by `lookupResultsHtml`, not by the popup itself):
  `ALL/TranslationTool` → `lookup/showRefs` (main.cpp:2215, 2221). The popup reads and
  writes **no** QSettings and registers **no** `sess::` key.
- **Build flags**: `ALL_HAVE_QTPDF` gates the two facsimile viewers.

## 5. Release questions

- Should the ⌘D search box remember its last query across launches (`sess::remember`)?
  Every other pane's search box does; this one does not.
- The ← → event filter swallows caret navigation inside the results browser — is that the
  intended trade for the stepper, or should it require ⌥?
- `proposeai:` returns silently when `g_aiGlossary` is unloaded. Should a missing AI
  glossary say so rather than making the link inert?
- The `alignprev:/alignnext:/aligncur:` handler is duplicated verbatim at main.cpp:7360
  (Overlay). Confirm both copies were updated together, or factor before release.
- Verify by hand on a release build: ⌘D from each of the four focusable text-widget kinds
  (the automated test bypasses the menu action entirely).
- Verify the two facsimile PDFs actually ship in the release data folder, and decide what
  the shipped build should do when `ALL_HAVE_QTPDF` is off — today the `das:`/`jae:` links
  are visible but only produce an apology box.
- Confirm the external link-outs are acceptable in an offline/air-gapped input-center
  install; there is no offline detection and no confirmation prompt before leaving the app.
- Decide whether `propose:` from this window should be blocked under `g_sweepActive`
  (the pecha lanes are; this dialog path is not).
