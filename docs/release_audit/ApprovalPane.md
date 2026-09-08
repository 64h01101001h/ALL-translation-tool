# Release audit — ApprovalPane

Source: `/Users/adamderickandrade/ALL-translation-tool/app/main.cpp`, class `ApprovalPane`
lines **30136–30834** (ctor 30138–30209, `selfTest` 30211–30334, private
`rebuild` 30337–30472, `onAction` 30474–30541, `bulkApprove` 30548–30662,
`upgradeColloquialRow` 30667–30672, `regenerateNow` 30677–30720,
`showArchive` 30724–30768, `exportApproved` 30770–30777,
`writeApprovedExport` 30786–30828, members 30830–30832). Next class
`ManuscriptPane` at 30835.

## 1. Purpose and placement

ApprovalPane is the authority's queue — the second half of the propose/approve
workflow. It reads the shared `proposals.tsv` store, lists every **pending**
proposal with Approve / Decline / Defer links, and stamps each ruling with the
approver's name and today's date. Register-class rulings (honorific, high/double
honorific, humilific, pronunciation, abbreviation, spelling) are **applied
in-app immediately** by regenerating the register files under `data/`;
dictionary/corpus-class rulings (word/phrase rendering, idiom, note, catalog
identity) are collected and leave the app only through a manually invoked TSV
export for the data project. The pane is **admin-gated**: `main()` adds the tab
only `if (g_isAdmin)` (`app/main.cpp:36407–36417`), with the pending count baked
into the tab label; `g_isAdmin` comes from QSettings `team/admin`, which the
user sets themselves via the Propose pane or Settings checkbox (see
ProposePane audit §2 — self-elevation). It is also force-enabled in
`--shot`/`--selftest`/`--sweep` runs (`36396–36400`). Reached by: Community
surface group → "Approval (N)" tab, visible only to a self-declared authority.
Unlike ProposePane it **does** build a RibbonBar with three groups —
`QUEUE` (30150), `RULINGS` (30151), `RELEASE` (30152).

## 2. User-visible functions

| Control/action | What it does (from code) | Code anchor | Automated coverage | Notes / risks |
|---|---|---|---|---|
| Banner label "Approval — the authority's queue…" | Static text stating register approvals apply immediately and "the app never edits the corpus itself" | ctor, 30140–30147 | NONE | The honesty statement the whole pane rests on. Verified true: no code path here writes the corpus or `hgm_gloss`. |
| RibbonBar group **QUEUE** → "Kind" filter combo (10 items) | Filters the pending list: All kinds / Pronunciations / Honorifics / HIGH honorifics / Humilifics / Double honorifics / Spelling flags / Idioms / Catalog identities / Words-phrases-notes ("export" = every non-register kind) | ctor, 30171–30182, `sess::remember(filter_, "approval/kind")` 30182, added 30183; consumed in `rebuild` 30392 and `bulkApprove` 30550 | selfTest: `check(filter_ && filter_->count() >= 6, "kind filter offers the taxonomy")` (30224) and `check(!th.contains("kamdir"), "filter hides other kinds (no pronunciations)")` (30229–30231) | Selection persists across sessions (`sess/approval/kind`). **The filter also scopes "Approve all in filter…"** — a stale remembered filter silently changes the blast radius of the bulk button on next launch. |
| RibbonBar QUEUE → "Refresh queue" (big, icon "clock") | Re-reads the store and re-renders | ctor, 30167, 30184; connect 30201 → `rebuild()` 30337 | selfTest exercises `rebuild()` at 30217 with `check(t.contains("pending"), "queue renders the pending count")`, `check(t.contains("kamdir"), "seeded prenasal queue lists kamdir")`, `check(t.contains("pronunciation"), "per-kind counts shown in the session overview")` | Read-only. Preserves scroll position (30346–30349) — deliberate, for 200+ item sessions. |
| RibbonBar **RULINGS** → "Approve all in filter…" (big, icon "check") + long tooltip | Collects every pending proposal matching the current filter, confirms with the count and filter name, asks one optional note, rules all Approved with approver+date, saves, regenerates registers, reports | ctor, 30168–30178, 30185; connect 30200 → `bulkApprove()` 30548 | **NONE** (selfTest never calls `bulkApprove`) | **Highest-consequence control in the app.** One click can approve hundreds of rows (the tooltip names the 205-row prenasal register). Confirmation dialog defaults to No (30591) and states the stamp. Save failure is correctly reported as "NOTHING has been stamped" (30617–30626) — but note the in-memory store was already mutated by `store.rule` before the save gate, and the function returns without reverting, so the on-screen state after `rebuild()` is re-read from disk (correct) yet the local `store` object is discarded (harmless). Register-regen failure is reported separately and names the unwritten paths (30656–30661). No undo. |
| RibbonBar RULINGS → "Rulings archive" (big, icon "ledger") + tooltip | Replaces the list view with a read-only record of every **ruled** proposal, newest first: status, kind, wylie, value, proposer, created, approver, ruled date, comment | ctor, 30169–30174, 30187; connect 30203 → `showArchive()` 30724 | NONE | Renders into the **same** `list_` widget, so the archive silently replaces the queue with no way back except Refresh. No count of total rulings shown other than the implicit list. Empty state "no rulings yet" (30766). |
| RibbonBar **RELEASE** → "Export approved dictionary candidates…" (big, icon "save") | File-save dialog defaulting to `<root>/data/candidate_alignments/approved_terms.tsv`, then writes every Approved non-register proposal as TSV | ctor, 30168, 30186; connect 30202 → `exportApproved()` 30770 → `writeApprovedExport()` 30786 | selfTest, extensive: `check(n == 1 && body.contains("APPROVED") && body.contains("dge ba"), "approved-candidates export writes the ruling")` (30259–30261); short-write drill under `RLIMIT_FSIZE`: `check(limApplied, "short-write fixture armed (RLIMIT_FSIZE)")` (30323), `check(nBad < 0, "a short write is REPORTED as a failure, never counted as an export (house rule 4)")` (30326), `check(greenAfter == greenBefore, "no green success line for an export that did not land")` (30329), `check(!QFile::exists(bad), "the truncated file is removed, not left to re-import as valid data with one corrupt row")` (30332); plus `check(opened && !verdict, "streamWriteOk reports a refused stream write as a FAILURE")` (30284) and `check(opened && !fileSawIt, "...and QFile::error() alone would NOT have seen it - the blindness that shipped")` (30287) | **Disk write** outside the data folder if the user picks elsewhere. This is the one artefact that feeds the master dictionary; DATA-4 hardening is in place and tested. Success is appended as green HTML into `list_` (30820–30826), which mixes an operation log into the queue view. `exportApproved` discards the return value (30776) — acceptable, since `writeApprovedExport` warns on failure itself. |
| RibbonBar RELEASE → "Regenerate registers" (big, icon "gear") + tooltip | Re-derives the approved register layer from the rulings store (idempotent strip-and-refold), reloads live layers, reports honorific/pron/abbrev row counts or names the unwritten files | ctor, 30188–30193; connect 30204 → `regenerateNow()` 30677 | ctest `regenreg_smoke` covers `allcore::regenerateApprovedRegisters` (`"1 honorific 1 pron 1 abbrev"` at line 66, byte-identical idempotence at 88, decline-falls-out at 106). The **button/handler** has NONE | **Writes three files** under `<root>/data/` (honorifics, pron_colloquial, abbreviations). Failure path is honest and names paths (30705–30716); success reports counts. Overwrites whatever a user may have hand-edited in those register files. |
| Queue item link **"✓ Approve"** (green anchor `approve:<id>`) | Prompts for an optional note, rules the proposal Approved with approver+date, saves, regenerates registers when the item is register-class, reloads live layers, re-renders | `rebuild` 30462–30464; `onAction` 30474 | selfTest: `check(t.contains("Approve") && t.contains("Decline"), "ruling actions offered")` (30222) — presence only; **the click path is NONE** | `QInputDialog` Cancel aborts (30494). Save failure is reported as "The ruling was NOT saved … exists only in memory" (30516–30523, SQA FAIL-2 fix). Register-write failure reported separately (30529–30538). No undo; the only reversal is ruling again (the regen is idempotent from the store, so a later Decline does fall out — `regenreg_smoke` line 106). |
| Queue item link **"✗ Decline"** (red anchor `decline:<id>`) | Same as Approve with status Declined; additionally, for `Pronunciation` kind, removes the derived colloquial register row via `upgradeColloquialRow(..., false)` | `rebuild` 30464–30466; `onAction` 30474, 30501–30505 | Presence-only as above; the pronunciation-removal path is covered in ctest `proposals_smoke` at the allcore level (`"a miss leaves the file byte-identical"` line 145, and the removal assertions at 126–131) | **Writes `data/pron_colloquial/colloquial_pron.tsv`.** The comment (30498–30500) claims only prenasal-derived rows are touched and community/hgm-attested rows are never affected — enforced inside `allcore::applyPronunciationRuling`, which `proposals_smoke` asserts (lines 117–131). The return value of `upgradeColloquialRow` is **discarded** at 30502 — a failed register edit here is silent (unlike every other write in the pane). |
| Queue item link **"⏸ Defer"** (muted anchor `defer:<id>`) | Rules the proposal Deferred with no note prompt | `rebuild` 30466–30468; `onAction` 30474, 30492 | NONE | A deferred item **leaves the pending list** (`rebuild` shows only Pending, 30398) and is only visible again in the Rulings archive. There is no "deferred" filter and no way to return an item to the queue from the UI. |
| Sync-conflict banner (amber) | When the store absorbed a Dropbox "conflicted copy", names the file(s), the number of absorbed rows, and the number of divergent rows, stating the main file's version is shown and the copy is untouched | `rebuild` 30365–30386 | ctest `proposals_smoke`: `"load with a conflicted sibling present"` (246), `conflictFiles().size() == 1` (247), `absorbedRows() == 1` (249), `divergentRows() == 1` (251), `"absorbed row is queryable"` (258), `"the main file's row wins on divergence"` (259). The **banner rendering** has NONE | Good honesty design (S1). Divergence is resolved by "main file wins" with no UI to inspect or choose — the user must open the conflicted copy by hand. |
| Session overview line (pending count · signed-in name · per-kind counts) | Header of every rebuild | `rebuild` 30356–30364 | selfTest 30218, 30226 | Shows "(unnamed)" when no identity set (30361). |
| Tab label auto-update "Approval (N)" | After each ruling, rewrites the parent tab text with the pending count | `rebuild` 30460–30471 | NONE | Relies on `parentWidget()->parentWidget()` being the `QTabWidget` — after the two-level navigation reparenting (36420+) this cast may fail, in which case the label silently stops updating. Worth verifying in the shipped nav. |
| Empty states | "nothing pending — the queue is clear." / "nothing pending in this kind." / "set the proposals folder in the Propose tab first" | `rebuild` 30340–30342, 30455–30459 | NONE | The folder-missing message points at Propose, correct. |
| Shortcuts / context menu / drag-drop / double-click | **None** defined in the class. `list_->setOpenLinks(false)` (30196) routes all anchors to `onAction` | ctor 30195–30196, 30205–30206 | — | Link scheme is `approve:` / `decline:` / `defer:` + raw id, parsed on the first `:` (30475–30477). Ids are store-generated, not user text, so no percent-encoding issue here — unlike the `openfile:`/`gloss:` schemes elsewhere in the file (note at 29704–29709). An unknown verb returns silently (30482). |

## 3. Internal functions

| Name | Line | Purpose | Called from |
|---|---|---|---|
| `ApprovalPane::ApprovalPane(const QString& root)` | 30138 | Builds banner, ribbon (QUEUE/RULINGS/RELEASE), filter, list browser; wires connects; calls `rebuild()` | `main()` 36414; `selfTest` harness 38422 |
| `ApprovalPane::selfTest(QStringList&)` | 30211 | 12 assertions: queue render, seeded row, actions offered, filter taxonomy, per-kind counts, filter exclusion, export correctness, `streamWriteOk` discrimination, and the RLIMIT_FSIZE short-write drill | `main()` 38421–38424 (constructs its own instance) |
| `ApprovalPane::rebuild()` | 30337 | Loads store, computes per-kind pending counts, renders conflict banner + pending cards + ruling links, restores scroll, updates tab text | ctor 30208; `onAction` 30510/30525/30540; `bulkApprove` 30578/30627/30643; `selfTest` 30217 |
| `ApprovalPane::onAction(const QString&)` | 30474 | Parses `verb:id`, prompts for a note, applies the pronunciation-decline register removal, rules, saves (gated), regenerates registers (gated), reloads layers, rebuilds | `list_` `anchorClicked` 30205–30206 |
| `ApprovalPane::bulkApprove()` | 30548 | Collects filtered pending ids, confirms with count, one batch note, rules all, saves (gated), regenerates, reports | bulkB click 30200 |
| `ApprovalPane::upgradeColloquialRow(const QString&, const Proposal&, bool)` | 30669 | Thin adapter to `allcore::applyPronunciationRuling` | `onAction` 30502 |
| `ApprovalPane::regenerateNow()` | 30677 | Loads store, `regenerateApprovedRegisters`, reloads layers, reports counts or unwritten paths | regenB click 30204 |
| `ApprovalPane::showArchive()` | 30724 | Renders every non-Pending proposal, newest first, into `list_` | archiveB click 30203 |
| `ApprovalPane::exportApproved()` | 30770 | Save-file dialog then `writeApprovedExport` | exportB click 30202 |
| `ApprovalPane::writeApprovedExport(const QString&)` | 30786 | Writes the approved non-register candidates TSV through `writeAllOrWarn`; returns row count or −1 | `exportApproved` 30776; `selfTest` 30256, 30318 |
| `reloadApprovedLayers()` (file-scope) | 3244 | Re-reads honorific register file; rebuilds `g_pronApprovedLive` from Approved Pronunciation proposals and `g_spellingValidLive` from Declined Spelling proposals | `onAction` 30539; `bulkApprove` 30641; `regenerateNow` 30683 |
| `writeAllOrWarn` (file-scope) | 3008 | All-or-nothing stream write with failure modal and partial-file removal | `writeApprovedExport` 30791 |
| `isoToday()` (file-scope) | 29714 | Date stamp on every ruling | 30510, 30573, 30588, 30604 |

## 4. Dependencies

**Files read**
- `<team/proposalsDir>/proposals.tsv` plus any Dropbox `*conflicted copy*.tsv`
  siblings (`core/src/proposals.cpp:153–167`).
- `<root>/data/honorifics/honorific_register.tsv` (via `reloadApprovedLayers`, 3246).

**Files written**
- `<team/proposalsDir>/proposals.tsv` — every ruling (`store.save()`, 30516, 30617).
- `<root>/data/honorifics/honorific_register.tsv` (`core/src/regenreg.cpp:96`).
- `<root>/data/pron_colloquial/colloquial_pron.tsv` (`regenreg.cpp:126`, and
  directly via `upgradeColloquialRow` on a pronunciation Decline, 30502).
- `<root>/data/abbreviations/approved_abbreviations.tsv` (`regenreg.cpp:177`).
- User-chosen export path, default
  `<root>/data/candidate_alignments/approved_terms.tsv`.
- selfTest scratch: `QDir::tempPath()/all_selftest_props` (+ `export.tsv`,
  `readonly_probe.txt`, `short_write.tsv`), `removeRecursively()`'d on entry (30238–30240).

**Network:** none.

**QSettings** (`"ALL"`, `"TranslationTool"`): reads `team/name`, `team/admin`,
`team/proposalsDir` via `loadIdentity()` (29718); `sess/approval/kind` for the
filter (30182); `dlg/<caption>` for the export dialog's start directory.

**Globals:** reads/writes `g_userName`, `g_isAdmin`, `g_proposalsDir`;
mutates `g_pronApprovedLive`, `g_spellingValidLive`, `*g_honorificsLive` through
`reloadApprovedLayers`. selfTest temporarily reassigns and restores
`g_proposalsDir` (30252/30255, 30310/30329) and `RLIMIT_FSIZE` + the `SIGXFSZ`
handler (30313–30321).

**ctest suites of record:** `proposals_smoke` (`core/CMakeLists.txt:183`),
`regenreg_smoke` (`:179`), `colloquial_smoke` (`:165`), `register_smoke` (`:302`).

## 5. Provenance / hgm_gloss risk

- **No control in this pane writes `hgm_gloss` or any dictionary entry.** The
  strongest write is the register layer (honorific / colloquial pronunciation /
  abbreviation TSVs) — reference-register data, not HGM English. Dictionary-class
  approvals are deliberately confined to `writeApprovedExport`, whose file header
  reads "approved in-app, **never auto-ingested**" (30787–30789). The class
  comment (30130–30134) and the banner (30140–30147) both state the boundary.
- **The gloss-adjacent surface is the export.** Approving a `WordRendering` /
  `PhraseRendering` / `Idiom` proposal produces a row whose `value` is proposed
  English, stamped with the approver's name in a file named
  `approved_terms.tsv` under `data/candidate_alignments/`. If that file were ever
  ingested unreviewed by the data project it would MATCH-vs-COMPOSE the rule.
  The app is clean; the risk is downstream of it and depends on the data
  project's ingest discipline.
- **"Approve all in filter…" is the one control that could mass-approve
  rendering proposals** — the "Words / phrases / notes" filter selects exactly
  the export lane (`want == "export"` → every non-register kind, 30554–30560),
  so a single confirmed click can bless hundreds of proposed English renderings
  into the export with the authority's stamp. The confirmation names the count
  and the filter, which is the only guard. It has **no automated coverage**.
- `reloadApprovedLayers` promotes approved pronunciations into the live
  `g_pronApprovedLive` map keyed by lowercased wylie (3256–3265), which the app
  then displays. That is register data and permitted, but it is the one path
  where a ruling changes what users see immediately, with no tier label applied
  at this layer.

## 6. Release questions

- **Gating:** the pane is admin-only, but "admin" is a self-set QSettings
  boolean the user can tick in Propose or Settings. Decide whether shipping the
  authority's queue behind a self-service checkbox is acceptable.
- **Bulk approve has zero automated coverage** and is the highest-consequence
  control in the application (hundreds of irreversible stamped rulings, register
  regeneration, export lane included). Ship, hide behind a preference, or add
  coverage first?
- `upgradeColloquialRow`'s return value is discarded at 30502 — a failed
  colloquial-register edit on a pronunciation Decline is the one silent write in
  a pane that otherwise reports every failure. Fix before ship?
- **Defer is a one-way door in the UI**: deferred items disappear from the queue
  with no filter to retrieve them. Should there be a "Deferred" filter entry, or
  a way to return an item to Pending?
- **No undo on any ruling.** Regeneration is idempotent from the store, so a
  corrective re-ruling does propagate — but there is no UI to re-rule an item
  once it leaves the pending list. Confirm this is intended.
- `showArchive` and the export success line both render into `list_`, replacing
  or appending to the queue view. Decide whether the archive deserves its own
  window/tab rather than hijacking the queue.
- Tab-label auto-update (30460–30471) depends on a two-level `parentWidget()`
  cast that may not hold after the navigation reparenting at 36420+. Verify the
  "Approval (N)" count actually updates in the shipped build.
- Sync-conflict divergence resolves silently as "main file wins" with the copy
  left on disk. Decide whether the authority needs an in-app diff/merge or
  whether the banner plus manual inspection is sufficient for release.
- `writeApprovedExport` defaults into `data/candidate_alignments/` but the user
  may save anywhere. Confirm the data project's handoff expects a fixed path.
- Evidence is truncated to 300 characters in the ruling card (30414) while the
  archive shows none of it. Is the authority ruling on enough information?
- `regenerateNow` overwrites three register files under `data/`. Confirm no
  hand-maintained content lives in those files that a regeneration would erase.
