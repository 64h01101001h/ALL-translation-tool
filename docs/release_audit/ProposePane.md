# Release audit — ProposePane

Source: `/Users/adamderickandrade/ALL-translation-tool/app/main.cpp`, class `ProposePane`
lines **29878–30135** (ctor 29880–30034, `selfTest` 30035–30063, private
`showMine` 30066–30124, members 30126–30134).

## 1. Purpose and placement

ProposePane is the translator-facing half of the propose/approve workflow. Any
user (no login exists — identity is provenance only) can offer a term,
honorific marking, pronunciation exception, abbreviation, rendering, idiom, or a
free note; the item is appended to a shared TSV proposal store
(`<proposalsDir>/proposals.tsv`) which a Dropbox-style shared folder syncs to
the authority, who rules on it in ApprovalPane. The pane also carries the
one-time identity setup (name / authority flag / proposals folder), duplicated
in Settings (`app/main.cpp:34122–34185`), and a "what happened to my proposals"
feedback view. It is constructed unconditionally in `main()` at
`app/main.cpp:36404–36405` (`tabs.addTab(proposePane, "Propose")`) — **not**
admin-gated — and is then reparented by the two-level navigation into the
**Community** surface group alongside Approval
(`app/main.cpp:36564: mkGroup("Community", {"Propose", "Approval"})`; icon
mapping `"Propose" → "tag"` at 36440). The pane has **no RibbonBar**; all
controls are plain widgets in a vertical layout. Reached by: Community group →
Propose tab.

## 2. User-visible functions

| Control/action | What it does (from code) | Code anchor | Automated coverage | Notes / risks |
|---|---|---|---|---|
| Banner label "Propose — offer a term…" | Static explanatory text; states name is recorded for provenance and "nothing is a login" | ctor, 29882–29888 | NONE | Honest about the non-login model. Text only. |
| Identity group box header (checkable) | Collapses/expands the identity form; title reads "Filing as <name> (authority)" once configured, else "Who you are (one-time)" | ctor, 29893–29907; `applyFold` 29903–29906 | NONE | `applyFold` iterates `idBox->findChildren<QWidget*>()` and hides all descendants — cosmetically hides the box's own contents; groupbox title remains the only affordance. Auto-fold via `QTimer::singleShot(0, idBox, …)` at 29930–29935 (MEM-9 fix, context-guarded). |
| "Your name" line edit | Free-text proposer name, seeded from `g_userName` | ctor, 29908–29909 | NONE | No validation, no uniqueness. Name is the *only* provenance key — two people typing the same name are indistinguishable in the archive; a typo'd name orphans the user's own "My proposals" list (matching is exact string compare at 30084). |
| "I am an authority (can approve) — Geshe Michael / Adam only" checkbox | Sets `team/admin`; on next launch adds the Approval tab | ctor, 29910–29913; consumed at 36407 | NONE | **Self-elevation.** Any user can tick this box and gain the full Approval queue (approve/decline/bulk-approve/regenerate registers/export). The restriction is a sentence in the label, not a control. Same box exists in Settings (34127). Highest governance risk in this pane. |
| "Proposals folder" line edit | Path of the shared TSV store | ctor, 29915–29918 | NONE | Free text; a nonexistent path is not validated here — failure only surfaces later as "The proposals folder could not be written" (29744). |
| "Choose…" button | Opens `safeGetExistingDirectory` picker, writes result into the folder field | ctor, 29919, connect 29936–29940 | NONE | Disk read only; picker start dir remembered under `dlg/…` QSettings key. |
| "Save identity" button | Writes `team/name`, `team/admin`, `team/proposalsDir` to QSettings, calls `loadIdentity()`, shows a confirmation naming the restart requirement for the Approval tab | ctor, 29927; connect 29941–29955 | NONE | Persistent config write. Message says "restart to show it" — Approval tab is only added at startup (36407), so the claim is accurate. `loadIdentity()` early-returns when `g_identityPinned` (harness/screenshot mode), so in `--selftest`/`--sweep` the saved values do not take effect (29719, 36400). |
| "Kind" combo (10 items) | Selects `allcore::ProposalKind`: Honorific, HighHonorific, Humilific, DoubleHonorific, Pronunciation, Abbreviation, WordRendering, PhraseRendering, Idiom, Note | ctor, 29958–29979; `sess::remember(kind_, "propose/kind")` 29978 | ctest `proposals_smoke` covers kind naming and register/export lane routing (`"humilific routes to the register lane"`, `"idiom routes to the export lane"`, `kindFromName("catalog-identity")`); the **combo itself** has NONE | The Catalog-identity and Spelling kinds exist in the store but are **not** offerable here (filed by other panes: `fileSpellingProposals` 29845). Selection is remembered across sessions (`sess/propose/kind`). |
| "Tibetan" line edit | ACIP or wylie of the proposed item | ctor, 29981–29985 | NONE | No wylie/ACIP validation or normalisation at file time — an unparseable string is stored verbatim and reaches the authority as typed. |
| "Proposed" line edit | The proposed English / pronunciation / expansion | ctor, 29986–29990 | NONE | **Provenance-sensitive:** for kinds WordRendering/PhraseRendering/Idiom this field is proposed *English*. See §5. |
| "Secondary" line edit | Optional ordinary form, domain, or note | ctor, 29991–29996 | NONE | Stored as `field`. |
| "Evidence" plain text edit (max height 90) | Free text: where seen, passage, rationale | ctor, 29997–30001 | NONE | Tabs/newlines are TSV-escaped in the store (`proposals_smoke` asserts escaped tab and newline round-trip at lines 60–62 of `core/tests/proposals_smoke.cpp`). Truncated to 300 chars **for display** in the approval queue (30414) — the full text is stored but the ruler sees only the first 300 chars. |
| "File proposal for review" button | Validates that Tibetan or Evidence is non-empty; `loadIdentity()`; `fileProposal(...)`; clears the four inputs | ctor, 30002–30003; connect 30017–30033; `fileProposal` 29727–29750 | NONE for the button; ctest `proposals_smoke` covers the store (`"propose returns distinct ids"`, `"two pending after two proposals"`, `"store saves"`, `"store reloads"`) | **Disk write** to `<proposalsDir>/proposals.tsv`. Failure *is* reported ("Could not save", 29748). Fields are cleared **unconditionally after** `fileProposal` returns — including when the call bailed out on missing identity (29731) or when the save failed — so a user whose write failed loses the typed text. Only "Tibetan or Evidence" is required: a proposal with a Proposed value but no Tibetan and no evidence is rejected; one with Tibetan only and zero evidence is accepted. |
| "My proposals — what happened?" button (+ tooltip) | Reveals the `mine_` browser and renders `showMine()` | ctor, 30006–30012, connect 30016 | selfTest: `check(m.contains("zhal") && m.toUpper().contains("PENDING"), "my-proposals lists own item with status")` (30057–30059) | Reads the whole store on each click; no incremental load. |
| "My proposals" result browser (`mine_`) | HTML list of the user's own proposals, newest first, colour-coded status, kind, wylie, value, created date, and ruling (approver/date/comment) | `showMine` 30066–30124 | selfTest as above, plus fixture guard `check(st.save(), "selftest fixture: the proposal store saved …")` (30049–30052) | `QTextBrowser` default link handling: no links are emitted so no anchor scheme exists here. Empty-state strings at 30070–30073 and 30122. Colour array indexed by `int(p.status)` (30076–30079) — a status enum gaining a fifth value would read past the four-element array. |
| Window/shortcut/context menu | **None.** No `QShortcut`, no `setContextMenuPolicy`, no drag/drop, no double-click handler in the class | — | — | Copy from `mine_` is the built-in QTextBrowser context menu only. |

## 3. Internal functions

| Name | Line | Purpose | Called from |
|---|---|---|---|
| `ProposePane::ProposePane()` | 29880 | Builds banner, identity box, proposal form, feedback browser; wires all connects | `main()` 36404 |
| `applyFold` (lambda) | 29903 | Show/hide identity box children | groupbox `toggled` (29907), deferred initial fold (29935) |
| `ProposePane::selfTest(QStringList&)` | 30035 | Two assertions over a throwaway temp store in `QDir::tempPath()/all_selftest_mine`; saves and restores `g_proposalsDir` | `main()` selftest block, 38272 |
| `ProposePane::showMine()` | 30066 | Loads the store, filters to `p.proposer == g_userName`, renders HTML | mineBtn click (30016), `selfTest` (30054) |
| `loadIdentity()` (file-scope) | 29718 | Reads `team/*` from QSettings into globals; no-op when `g_identityPinned` | ctor saveId (29944), submit (30023), `showMine` (30067) |
| `fileProposal()` (file-scope) | 29727 | Guard on identity, load store, `propose`, `save`, success/failure modal | submit (30024), and other panes (2586, 39684) |
| `isoToday()` (file-scope) | 29714 | `yyyy-MM-dd` stamp | `fileProposal` 29749 |

## 4. Dependencies

- **Disk (read+write):** `<team/proposalsDir>/proposals.tsv` — via `allcore::ProposalStore`
  (`core/src/proposals.cpp:109, 153, 197`). `load()` also absorbs Dropbox
  "conflicted copy" sibling `.tsv` files in the same folder
  (`core/src/proposals.cpp:154–167`).
- **Disk (selfTest only):** `QDir::tempPath()/all_selftest_mine`, created and
  `removeRecursively()`'d (30046–30048).
- **QSettings** (`"ALL"`, `"TranslationTool"`): `team/name`, `team/admin`,
  `team/proposalsDir` (written 29941–29943, read 29721–29723); `sess/propose/kind`
  (29978); `dlg/<caption>` for the folder-picker start dir.
- **Network:** none.
- **Globals mutated:** `g_userName`, `g_isAdmin`, `g_proposalsDir` (via
  `loadIdentity`); `g_proposalsDir` temporarily reassigned and restored in
  `selfTest`.

## 5. Provenance / hgm_gloss risk

The inviolable rule is that `hgm_gloss` is binding and machine work may MATCH
but never COMPOSE HGM English. Findings:

- **No control in this pane writes `hgm_gloss` or any dictionary field.** The
  only write is an append to `proposals.tsv`. The rendering kinds
  (`WordRendering`, `PhraseRendering`, `Idiom`) are labelled "→ dictionary" and
  are routed to the **export lane** (`isRegister() == false`,
  `core/tests/proposals_smoke.cpp:66, 178, 188`), meaning they can only ever
  leave the app as a TSV of candidates for the data project (ApprovalPane's
  `writeApprovedExport`, `app/main.cpp:30775`), never as an in-app gloss. The
  export header itself says "approved in-app, never auto-ingested" (30787).
- **However**, the combo labels "Rendering for a word (**→ dictionary**)" and
  "Rendering for a phrase/clause (→ dictionary)" (29968–29972) read to a
  translator as though filing here edits the dictionary. Nothing in the form
  states that HGM's English is binding and that a proposal is a candidate, not
  a gloss. This is a wording/expectation risk, not a data-integrity breach.
- **Indirect live effect:** approved `Pronunciation` proposals *are* folded into
  the running app (`reloadApprovedLayers`, 3249–3269, into `g_pronApprovedLive`),
  and declined `Spelling` proposals into `g_spellingValidLive`. Both are register
  layers, not gloss — permitted — but it means a proposal filed here can change
  what the app shows after an approval, with the authority's name/date stamp as
  the only gate.
- **Self-elevation** (the authority checkbox, 29910) is the one path by which a
  non-authority could reach those write powers. Flagged as the pane's top risk.

## 6. Release questions

- Ship the "I am an authority" checkbox as-is? Any user can grant themselves the
  Approval pane (approve, bulk-approve, regenerate registers, export). If the
  answer is "trusted install only", that should be stated in the pane; if not,
  the flag needs an out-of-band gate (a shared secret, an admin file in the
  proposals folder, or removal from the user-facing UI, leaving it in Settings
  only).
- The identity box duplicates Settings (34122–34185). Keep both, or make Propose
  link to Settings? Two writable copies of the same three keys is a divergence
  surface.
- Should the submit handler preserve the form when `fileProposal` fails or bails
  on missing identity? Current code clears the four fields unconditionally
  (30028–30032) — a save failure destroys the user's typed work while showing
  "Could not save".
- The proposer name is unvalidated free text and is the sole provenance key.
  Decide whether that is acceptable for a record intended to last "over the
  years" (the archive tooltip's own framing, 30144).
- Should the rendering kinds carry an explicit on-form honesty line ("proposals
  are candidates for the data project; HGM's English is binding and is never
  set by this app")?
- Evidence is stored in full but shown to the ruler truncated at 300 chars
  (30414). Confirm the authority is expected to rule on truncated evidence, or
  add an expander in ApprovalPane.
- `selfTest` covers 2 assertions out of ~14 user-visible controls. Decide
  whether the submit path, the identity save, and the fold behaviour need
  coverage before ship.
- No accessibility pass is evident (no `setAccessibleName`, no tab-order
  statement, colour-only status coding in `showMine` at 30076–30079 —
  status text is also spelled out, so this is likely acceptable, but it should
  be confirmed against the project's accessibility-audit habit).
