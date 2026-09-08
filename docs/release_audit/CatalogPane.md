# Release audit — `CatalogPane` (app/main.cpp, lines 31614–34082)

## ⚠️ SCOPE FLAG: CATALOGING IS SIDELINED

> "## SCOPE DECISION (Adam, 2026-08-08): cataloging is SIDELINED"
> — `/Users/adamderickandrade/ALL-translation-tool/TODO.md`, line 471

> "A separate in-house cataloging tool/app will be created as its own
> project in the future. All cataloging-tool discussion and
> implementation is sidelined HERE from this date."
> — `TODO.md`, lines 473–475

> "SIDELINED (banked knowledge kept for the future project):
> catalog-toolchain follow-ups … **any catalog-editing/catalog-building
> features in this app**."
> — `TODO.md`, lines 476–481 (emphasis added)

> "STAYS IN SCOPE here (serves translators, not cataloging): the
> Library pane's catalog DISPLAY (decoder, titles, subjects,
> verification levels), the Input pane build (#30 — input centers,
> not cataloging), OCR, registers, and everything else."
> — `TODO.md`, lines 484–487

> "Cataloging sidelined (own future project, SCOPE DECISION 08-06)"
> — `TODO.md`, line 1389

**`CatalogPane` IS the catalog-building feature that line 481 names.**
It renames files, moves files onto shelves, renames folders, writes
worksheets, writes ASCII catalog lists, writes an identity roster with
passphrase hashes, and runs a staging/approval ledger. None of that is
"the Library pane's catalog DISPLAY".

**Unreconciled counter-evidence, recorded honestly:** `TODO.md` line 812
records item 9g — "**CATALOGING WORKFLOW (Adam, 2026-08-19: 'this is
about … uncataloged data into our official database/catalog')**" — with
"v1 SHIPPED same day: a Catalog tab group…", and lines 1030–1046 record
the 2026-08-20 access-layer work, and lines 1098–1167 make the Catalog
pane the **pilot** for the new RibbonBar rollout. So the pane was built
and shipped eleven days *after* the sideline ruling, and later work
depends on it. The repo does not say which decision superseded which.
`docs/FEATURE_STATUS.md` carries **no row for a Catalog pane**; its
"catalog" mentions (lines 13, 65, 148) are Library-pane display,
Library provenance, and a "catalog tier ruling" — none of them this
pane. The roadmap file contains no "catalog" hits at all.

**This is the single biggest release question in this document.**

---

## 1. Purpose and how the user reaches it

`CatalogPane` is the in-house cataloging intake workbench: uncataloged
material on the left, the official library on the right, and one
read-only `QTextBrowser` card below that renders whatever file is
selected — decoded identity, three "states" lights, provenance
signals, suggested identity with candidates, and the file's entire text.
Above the trees sits a `RibbonBar` with five groups (ACCESS, SUGGEST,
INVENTORY, QC, HANDOFF) carrying fourteen buttons. Ten of those are
**gated**: disabled until a roster member signs in
(`gated_`, 31770–31771; `applyLock`, 33716). The pane's charter, stated
in its own banner, is that machine work locates and suggests, humans
approve, and the official catalog changes only through data releases —
but the pane nonetheless performs irreversible filesystem writes
(rename, move, folder rename) once a user is signed in.

**How the user reaches it: unconditionally.** `app/main.cpp:36402–36403`:

```cpp
auto* catalogPane = new CatalogPane(root);
tabs.addTab(catalogPane, "Catalog");
```

No `if (g_isAdmin)`, no settings check, no feature flag — contrast the
very next pane, `ProposePane`, whose badge work *is* wrapped in
`if (g_isAdmin)` (36405+). The Catalog tab is in the tab bar for every
user of every build. It is also driven by the screenshot/demo path
(`app/main.cpp:40947–40968`), which copies two library files into
`/tmp/all_catalog_demo`, calls `scanFolder`, and calls
`selectFirstUncataloged()` — so the pane appears in demo screenshots.

Supporting types covered here: `struct CatalogMember` (31316) and
`IdentitySuffixDelegate` (31436) — see Internal functions and
Dependencies.

**Coverage note.** `CatalogPane` has **no `selfTest` member of its
own.** The `selfTest` at line 31227 belongs to **`ManuscriptPane`** —
verified: its `check` lambda logs `"  [%1] Manuscript: %2"` (31229–31234).
All CatalogPane coverage lives in the app-level selftest block at
`app/main.cpp` ≈ 38789–39670, which mostly emits `log << QString("  [%1]
Catalog: …")` lines directly rather than through a `check(...)` lambda;
one local `check` lambda exists only for the move+stamp block
(≈39316–39380). Exact strings are quoted below. There are also `ctest`
suites over the *engine* functions in `core/` — `catalog_qc_smoke`,
`catalog_list_smoke`, `catalog_name_smoke`, `catalog_audit_smoke`,
`catalog_id_smoke`, `catalog_actions_smoke`, `register_smoke`,
`worksheet_smoke`, `title_xlat_smoke`, `volsplit_smoke`
(`core/CMakeLists.txt:290–384`) — these prove the algorithms, never the
UI wiring.

## 2. User-visible functions

### Ribbon — ACCESS group

| Control/action | What it does (from code) | Code anchor | Automated coverage | Notes/risks |
|---|---|---|---|---|
| **"Official library…"** button | Directory picker for the team's Dropbox-synced official root; writes `sess::put("catalog/officialRoot", d)`, re-roots the destination tree (`remember=false`), calls `applyLock()` | ctor lambda, 31676–31685 | NONE (the key is read in the in-house-gate check, ≈39180, but the button is never clicked) | **Not gated** — any user can repoint the official root. Writes a QSettings key. No validation that the folder looks like a library. |
| **"Sign in…"** button | Opens `signIn()`: combo of active roster names + password field; SHA-256(salt+pass) compared to the roster row; on success sets `g_catalogUser`/`g_catalogRoles`, persists `sess::put("catalog/lastUser")`, and **seeds `g_userName` + `QSettings("ALL","TranslationTool").team/name`** if unset | ctor 31687; `signIn()` 33744–33835 | `"  [%1] Catalog: in-house gate — roster verifies, the lock holds, a cataloger's move stages and the approver's ruling shelves it"` (≈39180) — exercises `catalogHash`/`applyLock`/`stageForApproval` directly, **not** the dialog | Comment at 33822 calls this "access discipline, not cryptography". Salt is `SHA256(name+epoch_ms).left(16)` (33907–33915) — predictable-ish. Unsalted-iteration hash, no KDF. Writes a global app setting (`team/name`) as a side effect of signing in. Failure message routes to the info panel, not a dialog. |
| **"Team…"** button | `teamDialog()`: list of members; add member (name/initials/roles combo cataloger·approver·admin·admin,approver/passphrase ×2); "Revoke selected"; bootstrap mode when no roster exists makes the first entry ADMIN and signs them in | ctor 31692; `teamDialog()` 33851–33975 | Roster save/load and the failure path: `"  [%1] Catalog: a roster save that cannot put its bytes on disk reports FALSE and leaves the existing roster intact"` (≈39223) | **Enabled when logged out if no roster exists** (`applyLock`, 33724–33726) — i.e. anyone who can point at an official root can bootstrap themselves as admin. That is the documented perimeter model ("the Dropbox share itself is the real perimeter"), but it means the app's own gate is bypassable by anyone with the folder. Writes `CATALOG_TEAM.tsv` (temp+rename, checked — good). Revoke is immediate, no confirm. Passphrases handled in plaintext `QLineEdit`s in memory. |
| **"Approvals…"** button (`pendB_`) | `approvalsDialog()`: `ActionLedger` table of pending staged actions (id/when/who/file/shelf/note); "Approve — place on the shelf" and "Reject…" enabled only for role `approver` | ctor 31680; `approvalsDialog()` 33977–34080 | Approve path covered inside the in-house-gate check (`led.approve` + shelved assertion, ≈39160–39180) | **`led.approve()` moves the staged file onto the real shelf** — an irreversible disk write from a dialog. Errors from approve/reject go to a `QMessageBox::warning` and then `refill()` regardless. Reject requires a reason (recorded). Gated on sign-in. |
| Signed-in status label (`who_`) | Text: "signed in: <name> (roles)" or one of three locked reasons | `applyLock`, 33731–33741 | Lock state proved by the in-house-gate check (`lockOk` loop over `gated_`, ≈39145–39152) | Read-only. Honest wording. |

### Ribbon — SUGGEST group

| Control/action | What it does | Code anchor | Automated coverage | Notes/risks |
|---|---|---|---|---|
| **"Suggest splits…"** | Reads the selected file (cap 32 MB), null-byte sniff, `allcore::suggestVolumeSplits`, renders candidate boundaries with folio/percent/rule/warning. Cuts nothing | ctor 31921–31930; `splitHtml()` 32887–32964 | `"  [%1] Catalog: chop assist finds both texts in a two-text volume with evidence, and cuts nothing"` (≈38907); ctest `volsplit_smoke` | Read-only. Honest cap disclosure. 32 MB synchronous read on the UI thread. Quoted accuracy figures (~93% recall, ~2 false/volume) are baked into the HTML — verify they are still true before ship. |
| **"Compose name…"** | Modal dialog: catalog number / Tibetan title / English title / author, live preview of `allcore::composeCatalogFilename`, "Copy name", "Rename in intake…", "Close". Rename applies in place, writes a META companion when truncated, then optionally **renames the containing folder** with a date+initials stamp | ctor 31840; `composeNameDialog()` 32100–32219; `applyComposedName()` 32812–32885 | `"  [%1] Catalog: compose-name renames in place and the META companion rejoins to the full name"` (≈38946); `"  [%1] Catalog: an existing name is refused, never overwritten"` (≈38959); ctest `catalog_name_smoke` | **Irreversible disk writes.** The undo logic in `applyComposedName` (32849–32882) is careful and well-commented (BOUNTY #4). "Rename in intake…" is enabled only for files under the intake root. The **folder stamp** is a second, cascading rename that moves every sibling file — confirmed by a `QMessageBox::question` whose text does not say "this renames the folder for every file in it". The dialog is driven only through `applyComposedName`'s seam in tests; **the dialog itself, the Copy button, and the stamp prompt are untested.** |
| **"Translate title…"** | Seeds a `QInputDialog` from the selected file's title page, then `titleWorkbenchHtml`: whole-title matches + per-phrase attested renderings + explicitly "no attestation" leftovers | ctor 31896–31912; `titleWorkbenchHtml()` 32621–32711 | `"  [%1] Catalog: the title workbench attests published renderings, never composes"` (≈39022); ctest `title_xlat_smoke` | Read-only. Strong honesty discipline ("the machine attests; you compose"). Depends on `titlePairBank()`, which can be **empty on an ordinary install** (see Dependencies) — the workbench then renders "an honest blank", which is correct but indistinguishable from "no match". |

### Ribbon — INVENTORY group

| Control/action | What it does | Code anchor | Automated coverage | Notes/risks |
|---|---|---|---|---|
| **"Audit bibliographies…"** | `bibliographyAuditHtml()`: loads `mixed_nuts_bibliography.json` + `apparatus_bibliography.json`, extracts ACIP citations, compares against `collectLibraryNumbers(destRoot)`, groups missing works by citing volume with evidence and BDRC witness links | ctor 31948; `bibliographyAuditHtml()` 33059–33196 | `"  [%1] Catalog: bibliography audit renders cited-but-absent works with evidence and the honest caveat"` (≈39591); ctest `catalog_audit_smoke` | Read-only. Explicit "banks are not installed" branch (33098) — good. States the lower-bound caveat. Emits `<a href='https://library.bdrc.io/…'>` links. |
| **"Generate catalog list…"** | Save-file dialog, then `generateAsciiCatalog` over the destination root, `saveOrWarn`, honest counts or "NOT written" | ctor 31882–31894; `generateListHtml()` 32780–32810 | `"  [%1] Catalog: the ASCII list is a faithful, marked inventory (T* for from-the-text titles)"` (≈38990); `"  [%1] Catalog: an unwritable list target answers NOT written, no stump, no statistics (WP-8)"` (≈39006); ctest `catalog_list_smoke` | **Disk write** to a user-chosen path. Write verdict correctly gates the success headline (WP-8). Defaults the target to `~/catalog_list.txt`. |
| **"Compare trees…"** | `treeDiffHtml(intakeRoot, destRoot)`: `allcore::diffTrees` — identical / modified / renamed-refiled / only-left / only-right, capped at 150 (100 for renames) per block | ctor 31885–31896; `treeDiffHtml()` 32577–32619 | `"  [%1] Catalog: the tree comparison classifies modified and refiled files and changes nothing"` (≈39053) | Read-only. Honestly states content is matched by "size + sampled bytes (first/last 4 KB)" — i.e. **it can call two different files identical**; the caveat is printed. Unbounded recursive walk of both trees, synchronous. |
| **"Load register…"** | Open-file dialog (CSV/TSV/TXT), `register_.loadText`, then the three-states report: issued / cataloged in dest tree / issued-but-absent (first 100) | ctor 31875–31883; `loadRegisterHtml()` 32222–32330 | `"  [%1] Catalog: the register loads read-only and the three states light per file"` (≈39110); ctest `register_smoke` | Read-only; explicitly says the app never writes the register. Honest "No usable number column found" branch. Loads the whole file into memory. |

### Ribbon — QC group

| Control/action | What it does | Code anchor | Automated coverage | Notes/risks |
|---|---|---|---|---|
| **"QC intake…"** | Refuses with an inline note if no intake root; else `qcIntakeHtml`: lane 1 `qcTitleTranslationMismatch` (English half from the wrong work), lane 2 `qcDuplicateTitles` (shared Tibetan title, told apart by colophon) | ctor 31812–31821; `qcIntakeHtml()` 33221–33313 | `"  [%1] Catalog: QC flags the wrong-text English and tells shared titles apart by colophon"` (≈39517); ctest `catalog_qc_smoke` | Read-only. Contains the BOUNTY #5 fix (33240–33253): an empty title-pair bank now renders an amber "**Nothing was compared.**" instead of a green all-clear. That fix is **not** itself pinned by a named check. Full recursive tree scan, synchronous. |
| **"Worksheet…"** | Refuses inline if no file selected; else `worksheetDialog(lastFile_)`: 52 fields from `allcore::worksheetSchema()`, prefilled by `prefillWorksheet` and overlaid by any saved sidecar; buttons "Save worksheet", "Export row (CSV)…", "Close" | ctor 31823–31831; `worksheetDialog()` 32419–32498; `prefillWorksheet()` 32332–32417 | `"  [%1] Catalog: the worksheet prefills number, Tohoku, title-page title and colophon from the file itself"` (≈39413) — proves `prefillWorksheet` only; ctest `worksheet_smoke` | **Disk writes**: sidecar `<file>.worksheet.tsv` beside the intake file, plus a user-chosen CSV. Both go through `saveOrWarn`. Reads the whole file to compute an MD5. The dialog's Save/Export buttons are **untested**. Human-entered values correctly outrank machine prefills. |

### Ribbon — HANDOFF group

| Control/action | What it does | Code anchor | Automated coverage | Notes/risks |
|---|---|---|---|---|
| **"Move to shelf…"** | `requireLogin()`; refuses unless the selected file is under the intake root; with no destination shelf selected renders `shelfSuggestHtml` instead. **Non-approver:** confirm dialog → `stageForApproval` (COPY into AWAITING APPROVAL + ledger row). **Approver:** confirm dialog → `moveToShelf` (real move) → optional shelf folder stamp → `finishHandoffMove` | ctor 31842–31874; `moveToShelf()` 32500–32541; `stageForApproval()` 33825–33849; `finishHandoffMove()` 33430–33470 | `"  [%1] Catalog: the handoff moves the file WITH its META companion, refuses the nameless and the colliding, deletes nothing"` (≈39303); plus five `check(...)` strings in the move+stamp block: "the change-log stamp reports the shelf's new path, so the mover can follow the rename"; "after move + stamp the pane points at the real file on the renamed shelf, not at a path the rename erased"; "the completed move states its own outcome and names the shelf the file landed on"; "the card after filing never asserts \"not cataloged\" over a move that happened"; "the card is read back from the file at its new home - its shelf path and its own bytes"; "a filing whose file is not on the shelf flags instead of printing the success line". ctest `catalog_actions_smoke` | **The most dangerous control in the pane.** Irreversible `QFile::rename` of library material, plus an optional folder rename that moves everything else in the shelf. Refusals are good (undecodable name, collision, META collision with rollback). The confirm dialog's quoted rule — "A book on the wrong shelf is lost forever" — is exactly right and exactly why this needs sign-off before ship. |
| **"Approvals…"** | (listed under ACCESS above; the button is added to `gHandoff`, 31679) | 31679 | as above | Group placement of `pendB_` is HANDOFF while its siblings are ACCESS — cosmetic inconsistency. |

### The info panel (`QTextBrowser`, `setOpenLinks(false)`) — link schemes

Every anchor is dispatched by hand in `handleInfoAnchor` (32003–32064). A
scheme with no branch is a dead link that still draws a pointing hand —
the comment at 31991–32002 records that exact past defect.

| Link scheme | What it does | Code anchor | Automated coverage | Notes/risks |
|---|---|---|---|---|
| `catopen:<path>` | Calls the global `g_openAtLine(path, 0)` — opens the file in the Overlay pane | 32005–32006 | NONE (the witness-hunt check ≈39630 asserts *other* schemes do not reach the URL opener, not that `catopen:` works) | Silently does nothing if `g_openAtLine` is unset. |
| `proposeid:<path>` | `proposeIdentity(path)` — writes a PENDING `CatalogIdentity` row into the shared `ProposalStore` | 32007–32029; `proposeIdentity()` 33314–33375 | `"  [%1] Catalog: routing files a catalog-identity proposal with evidence; titleless files are refused honestly"` (≈39476) | **Disk write** into `g_proposalsDir` (a shared folder). Refuses honestly when name or proposals dir is unset, or the file has no title page. Carries the worksheet sidecar (first 600 chars) as evidence. |
| `exportmissing:` | Save-file dialog → `exportMissingList` — field-coded witness-hunt handout with BDRC search URLs | 32030–32045; `exportMissingList()` 32988–33057 | Exercised inside the bibliography-audit check (`exportMissingList(mout)`, ≈39575) | Disk write; uses the `streamWriteOk` idiom and removes a partial file (WP-9). Re-runs the whole audit to produce the file. |
| `shelfsel:<path>` | `dest_->selectPath(...)` then tells the user to click "Move to shelf…" again | 32046–32053 | NONE for the anchor itself; `shelfSuggestHtml` is covered by `"  [%1] Catalog: shelf suggestions surface the subject shelf with reasons, never a placement"` (≈39547) | Two-step confirm by design — good. |
| `cleanslash:<path>` | `writeCleanedCopy(path)` — writes `<name> CLEANED.<ext>` beside the original | 32054–32062; `writeCleanedCopy()` 32556–32575 | `"  [%1] Catalog: slash corruption is bannered and cleaned into a COPY - the mother copy untouched"` (≈39065) | Disk write; refuses if a CLEANED copy exists; goes through `saveOrWarn` (WP-13). Mother copy untouched. |
| `http…` | `urlOpener_(u)` → `QDesktopServices::openUrl` — **network / external browser** | 32063–32064; seam at 31989–31990 | `"  [%1] Catalog: a witness-hunt click dispatches that exact BDRC URL to the browser, and no other scheme does"` (≈39625) | The only outbound network affordance in the pane. Host: `library.bdrc.io`. Seam is public so the test can intercept — good design. |

### Other user-visible behaviour

| Control/action | What it does | Code anchor | Automated coverage | Notes/risks |
|---|---|---|---|---|
| Selecting any file in either tree | `showFile()` renders: filename, folder, decoded change-stamp from the nearest stamped ancestor (6 levels up), collection/number or "uncataloged", the three STATES line, an "Open in the Overlay" link, `identityHtml` for unrecognized files, a PROVENANCE line (folio marks / western pagination / lowercase runs / line-slash corruption), and the **entire text up to 20 MB** | `showFile()` 33710(private, ≈33475–33700) | `"  [%1] Catalog: the panel shows the entire text, not a truncation"` (≈38866); `"  [%1] Catalog: a binary file gets an honest note, not a garbage dump"` (≈38879) | Reads up to 20 MB for display and separately up to 32 MB for `scanAcipCleanup`, synchronously, on every selection. Binary sniff via null byte in the first 4 KB. Caps are disclosed. |
| Suggested-identity card (auto, for unrecognized files) | `identityHtml`: title read from the text with rule, Sanskrit side, N candidates from `titleBank()` with score/shared-syllables/basis/source, the Tohoku-first note for KD/TD keys, the "Propose this identity…" link, and COLOPHON CANDIDATES labelled composition vs "translation credit — not the author" | `identityHtml()` 33377–33544 | `"  [%1] Catalog: title page in an uncataloged file yields an evidence-carrying candidate"` (≈38833); `"  [%1] Catalog: a file with no title page is reported as needing a cataloger, not guessed"` (≈38844); ctest `catalog_id_smoke` | Excellent honesty discipline throughout; reports the bank size so a thin bank cannot read as confidence. Reads the whole file a second time for colophons. |
| Banner text (static) | States the charter: nothing is written to the official catalog from here; suggestions route through the approval channel | ctor 31621–31634 | NONE | Partly at odds with the pane's actual behaviour: it does not write the *catalog*, but it does move, rename, and stamp real files on the official share. |
| Ribbon group headers ACCESS/SUGGEST/INVENTORY/QC/HANDOFF | Layout only | ctor 31637–31641 | NONE | This pane is the RibbonBar pilot (`TODO.md` line 1117). |
| Shortcuts / context menus / drag-drop | None registered anywhere in the class | — | N/A | Ten disk-touching actions, zero keyboard shortcuts and zero context menus. |

## 3. Internal functions

| Name | Line | Purpose | Called from |
|---|---|---|---|
| `CatalogPane(root, parent)` | 31616 | Builds banner, RibbonBar (5 groups, 14 buttons), the two `CatalogTree`s, the info browser; wires every button; seeds roots from `sess::` and `catalogOfficialRoot()`; calls `applyLock()` | `main()`, 36402 |
| `urlOpener_` (public member) | 31989 | Test seam replacing `QDesktopServices::openUrl` | `handleInfoAnchor` http branch; swapped in selftest ≈39621–39632 |
| `handleInfoAnchor(url)` | 32003 | Hand-dispatch of all six link schemes | `info_->anchorClicked` (31946); selftest directly (≈39625, 39630) |
| `scanFolder(dir)` | 32066 | Public seam: `intake_->setRoot(dir)` | selftest ≈38811, ≈39547; demo path 40966 |
| `fileCount()` / `recognizedCount()` / `titledCount()` | 32067–32069 | Census accessors forwarded from `intake_` | selftest ≈38812–38814 |
| `destRoot()` / `setDestRoot(dir)` | 32070–32071 | Destination-root accessors (set uses `remember=false`) | selftest ≈39330–39331, 39377, 39651 |
| `currentFile()` | 32073 | The path the card is pointing at | selftest ≈39346, 39370 |
| `infoText()` | 32075 | Plain text of the card, for assertions | selftest, many |
| `showFilePublic(p)` | 32076 | Public wrapper over `showFile` | selftest ≈38862, 38873, 39069, 39103, 39246 |
| `selectFirstUncataloged()` | 32077 | Selects `intake_->firstUncataloged()` and renders it immediately (does not wait on the lazy model) | demo/screenshot path, 40967 |
| `composeNameDialog()` | 32100 | The filename composer modal | "Compose name…" button, 31840 |
| `loadRegisterHtml(path)` | 32222 | Read-only register load + three-states report | "Load register…" button, 31881; selftest ≈39095 |
| `prefillWorksheet(path)` | 32332 | Machine suggestions for the 52-column worksheet (filename grammar, Tohoku from KD/TD, bytes+MD5, folio range, BDRC permalink, title page, composition colophon) | `worksheetDialog`; selftest ≈39392 |
| `worksheetDialog(path)` | 32419 | The 52-field worksheet modal; sidecar overlays prefills | "Worksheet…" button, 31829 |
| `moveToShelf(src, destDir)` | 32500 | The real move; refuses undecodable names and collisions; carries the META companion and rolls back if it cannot | "Move to shelf…" approver branch, 31871; selftest ≈39269–39299, 39332 |
| `stampFolder(dirPath, initials, newPath*)` | 32543 | Renames a folder to carry today's date + initials; reports the new path so callers can follow | composer stamp branch (32204), move stamp branch (31889), selftest ≈39241, 39334 |
| `finishHandoffMove(shelf, stamped)` | 33430 | Tail of the move: follows the stamp rename, refreshes both censuses, prints the outcome only where the file verified on the shelf | "Move to shelf…" 31898; selftest ≈39344, 39371 |
| `writeCleanedCopy(path)` | 32556 | Writes `… CLEANED.<ext>`; refuses if it exists or nothing to strip | `cleanslash:` anchor; selftest ≈39073 |
| `treeDiffHtml(L, R)` | 32577 | The divergence report | "Compare trees…" 31894; selftest ≈39045 |
| `titleWorkbenchHtml(tib)` | 32621 | Attested-renderings report | "Translate title…" 31910; selftest ≈39015 |
| `titlePairBank()` | 32713 | Lazily builds the (tib, eng) pair bank from `<root>/library` + `catalog_works.json`; **sets its built flag only if something loaded** (documented 4th-instance bug fix) | `titleWorkbenchHtml`, `qcIntakeHtml` |
| `generateListHtml(root, out)` | 32780 | ASCII catalog list; write verdict gates the headline (WP-8) | "Generate catalog list…" 31892; selftest ≈38978, 39001 |
| `applyComposedName(path, r)` | 32812 | The rename itself + META companion, with full all-or-nothing undo (BOUNTY #4) | `composeNameDialog`; selftest ≈38929, 38956 |
| `splitHtml(path)` | 32887 | Chop-assist report, 32 MB cap | "Suggest splits…" 31929; selftest ≈38900 |
| `witnessLink(number, citation)` | 32966 | A BUDA link: direct scan URL when the number maps, else a percent-encoded title search; empty when neither is honest | `bibliographyAuditHtml` 33172 |
| `exportMissingList(outPath)` | 32988 | Field-coded witness-hunt handout | `exportmissing:` anchor; selftest ≈39575 |
| `bibliographyAuditHtml()` | 33059 | GMR's job #1 report | "Audit bibliographies…" 31948; selftest ≈39571, 39605 |
| `shelfSuggestHtml(path)` | 33198 | Ranked shelf suggestions with reasons and `shelfsel:` links; quotes its own measured accuracy (31% top-1, 43% top-3) | "Move to shelf…" when no shelf chosen, 31858 |
| `qcIntakeHtml(root)` | 33221 | The two QC lanes | "QC intake…" 31820; selftest ≈39510 |
| `proposeIdentity(path)` | 33314 | Files a `CatalogIdentity` proposal with evidence | `proposeid:` anchor; selftest ≈39439, 39465 |
| `identityHtml(path)` | 33377 | The suggested-identity card | `showFile` for unrecognized files; selftest ≈38823, 38840 |
| `titleBank()` | 33528 | Lazily builds the identity bank from `<root>/library` + `catalog_works.json`. **Sets `bankBuilt_ = true` before loading** (33530) — the anti-pattern `titlePairBank()` was explicitly fixed for | `identityHtml`, `proposeIdentity` |
| `showFile(path, banner="")` | ≈33475 (private) | Renders the whole card | `onFile` callbacks, `showFilePublic`, `finishHandoffMove`, composer tail, `cleanslash:` |
| `applyLock()` | 33716 | Enables/disables all gated buttons and rewrites `who_` | ctor 31804; "Official library…" 31684; after `teamDialog` 33974; `signIn` 33818; selftest ≈39144, 39150, 39179 |
| `signIn()` | 33744 | The sign-in modal | "Sign in…" 31686 |
| `requireLogin()` | 33805 | Guard: official root chosen + signed in, else an inline note | "Move to shelf…" 31844; `approvalsDialog` 33979 |
| `stageForApproval(src, shelfAbs, note, rootOverride)` | 33825 | COPY into AWAITING APPROVAL + ledger row; refuses shelves outside the official root | "Move to shelf…" non-approver branch 31848; selftest ≈39155 |
| `teamDialog()` | 33851 | Roster management modal | "Team…" 31694 |
| `approvalsDialog()` | 33977 | Pending-actions table + approve/reject | "Approvals…" 31681 |
| **File-scope helpers** (outside the class, used by it) | | | |
| `struct CatalogMember` | 31316 | Roster row: name, initials, roles, salt, hash, addedBy, addedOn, status; `active()`, `has(role)` | roster load/save, `teamDialog`, `signIn`, selftest ≈39126 |
| `g_catalogUser` / `g_catalogRoles` / `catalogRoleHas()` | 31324–31328 | Global signed-in identity | `applyLock`, `requireLogin`, move branch, `teamDialog`, `approvalsDialog` |
| `catalogOfficialRoot()` | 31329 | `sess::path("catalog/officialRoot")` | throughout; also `main()` 36100 and 37455 |
| `catalogRosterPath()` / `catalogHash()` | 31332, 31335 | `<root>/CATALOG_TEAM.tsv`; SHA-256(salt:pass) | roster I/O, `signIn`, selftest ≈39130 |
| `catalogRosterLoad()` / `catalogRosterBytes()` / `catalogRosterSave()` | 31341, 31356, 31389 | TSV roster I/O; save is temp-file + checked write + flush + atomic `std::rename` with a `.previous` fallback (FAIL-1 fix, documented 31379–31388) | `signIn`, `teamDialog`, selftest ≈39135, 39214 |
| `IdentitySuffixDelegate` | 31436 | Paints the decoded identity after each filename in a `CatalogTree` | installed by `CatalogTree` ctor, 31491 |

## 4. Dependencies

**Data folders / files (relative to `root_`, the app data root)**
- `<root>/library` — the default destination tree, the source of both banks' library half, and `collectLibraryNumbers` target. **Gitignored** — absent on an ordinary install.
- `<root>/data/extracted/catalog_works.json` — GMR's works table; the other half of both `titleBank()` and `titlePairBank()`. **Gitignored** — absent on an ordinary install. (Documented at 32715–32722.)
- `<root>/data/extracted/mixed_nuts_bibliography.json` and `<root>/data/extracted/apparatus_bibliography.json` — required by "Audit bibliographies…" and `exportMissingList`; explicit "not installed" branch exists only in the former.
- `g_proposalsDir` (shared proposals folder, set in Settings) — `ProposalStore` reads/writes here for `proposeid:`.
- `catalogOfficialRoot()` — the team's Dropbox-synced folder; holds `CATALOG_TEAM.tsv`, the `ActionLedger`, `AWAITING APPROVAL/`, `REJECTED/`, and the shelves.
- Per-file sidecars written beside intake files: `<file>.worksheet.tsv`, `<name> CLEANED.<ext>`, `<NUMBER> META.<ext>`.
- User-chosen save targets: the ASCII catalog list (default `~/catalog_list.txt`), the missing-works handout (default `~/missing_works.txt`), the worksheet CSV row (default `~/worksheet_row.csv`).
- Temp probe folders used by the harness/demo: `QDir::temp()/all_catalog_probe`, `QDir::tempPath()/all_catalog_demo`.

**Network hosts**
- `library.bdrc.io` — witness-hunt links (`witnessLink`, 32966–32987) and `bdrcScanUrl` permalinks, opened via `QDesktopServices::openUrl` through the `urlOpener_` seam. This is the pane's only outbound network affordance and it is user-click-initiated only.

**QSettings / `sess::` keys**
- `sess/catalog/officialRoot` — written by "Official library…"; read by `catalogOfficialRoot()`, `main()` (36100), and the backup-inventory list (37455).
- `sess/catalog/intakeDir` — written by `CatalogTree::setRoot` for the intake tree; read at ctor 31803.
- `sess/catalog/destDir` — read at ctor 31805; **never written** (the destination tree is always set with `remember=false`).
- `sess/catalog/lastUser` — written by `signIn` on success; read to preselect the sign-in combo.
- `QSettings("ALL","TranslationTool")` key `team/name` — written by `signIn` when `g_userName` is empty (the "LODESTAR L2 collapse", 33810–33817).

**allcore surface used** — `decodeAcipFilename`, `extractAcipTitle`,
`findColophonCandidates`, `suggestIdentity`, `TitleBank`,
`TitlePairBank`, `buildTitleWorkbench`, `composeCatalogFilename`,
`composeChangeStamp`, `parseChangeStamp`, `suggestVolumeSplits`,
`stripLineSlashes`, `scanAcipCleanup`, `acipFolioRange`, `bdrcScanUrl`,
`diffTrees`, `generateAsciiCatalog`, `collectLibraryNumbers`,
`baseCatalogKey`, `extractAcipCitations`, `auditPresence`,
`witnessSearchQuery`, `suggestShelves`, `qcTitleTranslationMismatch`,
`qcDuplicateTitles`, `CatalogRegister`, `Worksheet`/`worksheetSchema`/
`parseWorksheet`/`serializeWorksheet`/`worksheetCsvRow`,
`ProposalStore`, `ActionLedger`.

## 5. Release questions

- **Should this pane ship at all?** `TODO.md:481` sidelines "any
  catalog-editing/catalog-building features in this app", and this pane
  is exactly that. It nonetheless ships in every build with no gate at
  `app/main.cpp:36402–36403`. The owner must rule: ship, hide behind
  `g_isAdmin` (as `ProposePane`'s badge already is), hide behind a
  setting, or remove. **Whatever the ruling, `TODO.md` needs the
  contradiction between line 471 and line 812 resolved in writing.**
- **If it ships, does it ship to everyone?** Ten of fourteen buttons are
  sign-in-gated, but "Official library…" is not, and "Team…" is
  *enabled while logged out* whenever no roster exists — so any user who
  can point at a shared folder can bootstrap themselves as admin. The
  code says the Dropbox share is the real perimeter. Is that acceptable
  in a shipped build a non-team user can open?
- **The identity file is not cryptography and says so.** SHA-256 of
  `salt:passphrase`, no KDF, no iteration, 16-hex salt derived from
  `name + epoch_ms`. Fine for "team discipline plus attribution",
  wrong if anyone reads `CATALOG_TEAM.tsv` as a security boundary.
  Ship with the disclaimer visible in the UI, not only in comments?
- **Irreversible filesystem writes on shared library material.**
  "Move to shelf…" (rename), the shelf/folder **stamp** (renames a
  folder and everything under it), "Compose name…" rename, and
  ledger approve. Confirmations exist; undo does not. The folder-stamp
  prompt in particular does not tell the user it moves every sibling
  file. Should the stamp be a separate, explicit action?
- **`titleBank()` still sets `bankBuilt_ = true` before loading**
  (line 33530) — the exact anti-pattern `titlePairBank()` was fixed for
  at 32713–32726 with a comment naming four other instances. On an
  ordinary install (`library/` and `catalog_works.json` both gitignored)
  the identity bank silently stays empty for the whole session with no
  banner and no retry, and `identityHtml` will report "matched against
  0 known titles". This is a real, currently-live defect.
- **Both banks are absent on a normal install**, so "Translate title…",
  the QC title lane, and the identity card degrade to honest blanks —
  correct behaviour, but a shipped tab whose main features never produce
  output is a support burden.
- **UI-thread cost.** Selecting a file reads up to 20 MB + scans up to
  32 MB; "Compare trees…", "QC intake…", and `firstUncataloged()` walk
  entire trees uncapped; every one of these is synchronous with no
  progress indicator or cancel.
- **Coverage shape.** Roughly 27 named selftest lines plus 10 ctest
  suites cover the *seams and algorithms* well. What is **untested** is
  every modal dialog's own buttons (composer Copy/Rename/stamp prompts,
  worksheet Save/Export, sign-in, Team add/revoke, Approvals
  approve/reject via the table), every ribbon button *click*, and the
  `catopen:` anchor. All the disk-destructive paths are reached through
  dialogs.
- **The banner's charter vs the pane's behaviour.** "Nothing is written
  to the official catalog from here" is true of the *catalog*, but the
  pane writes files, renames folders, and mutates the shared roster and
  ledger inside the official library. Consider rewording before ship.
- **`sess/catalog/destDir` is dead** (read, never written) — honour it
  or delete it.
- **`pendB_` ("Approvals…") is filed under HANDOFF** while its ACCESS
  siblings are grouped together — cosmetic, but this pane is the
  RibbonBar pilot other panes will copy.
