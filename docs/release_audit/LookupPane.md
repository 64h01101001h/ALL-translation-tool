# Lookup pane — release audit

## 1. Purpose and how it is reached

The Lookup pane is the stacked multi-dictionary: the translator types one headword in any
notation (wylie, Tibetan unicode, ACIP, English, or pronunciation) or browses the whole
dictionary in traditional Tibetan (ICU/CLDR "bo") collation order, and gets a single stacked
card in which Geshe Michael's `hgm_gloss` equivalents are the binding layer and every other
dictionary (Chandra, Hopkins, THL/OT/IW reference DB, Mahāvyutpatti, Whitney, 84000, StarDict
user layers, the AI-drafted Working Glossary) is rendered below it, labeled reference. It is
built by the free function `makeLookupPane(...)` (`/Users/adamderickandrade/ALL-translation-tool/app/main.cpp:3427`) and added
as the top-level tab named **"Lookup"** in the main `QTabWidget` at
`app/main.cpp:36062` — the user reaches it by clicking that tab in the main window's tab bar.
It receives five data handles constructed earlier in `main()`: `spine` (the SQLite/FTS5
`hgm_spine` release database — the HGM master), `refdict` = `allcore::RefDict("<root>/build/reference.db")`
(local-only THL/TD reference compilation, `app/main.cpp:35698`), `mvp` =
`allcore::Mvp("<root>/data/extracted/mahavyutpatti.tsv")` (DILA TEI, CC BY-SA 3.0,
`:35705`), `whitney` = `allcore::WhitneyRoots("<root>/data/whitney/whitney_roots.tsv")`
(Whitney Roots 1885 / Grammar 1879, public domain + Apache-2.0 digitization, `:35714`), and
`colloq` = `allcore::ColloquialPron("<root>/data/pron_colloquial/colloquial_pron.tsv")`
(`:35723`); all four besides the spine are optional and may be null.
**`class LookupPopup : public QDialog` (`app/main.cpp:2595`–2704) IS related and is included
below:** it is not constructed by `makeLookupPane`, but it shares the pane's entire lookup
machinery — both render through the same pure function `lookupResultsHtml()`
(`app/main.cpp:2054`) and handle the same custom anchor schemes. It is **not reachable from
the Lookup tab**; it is a single app-wide floating `Qt::Tool` window instantiated at
`app/main.cpp:36065` and opened only by the View-menu action "Look Up Selection…" / **⌘D**
(`app/main.cpp:37248-37264`), which looks up the current text selection from any pane.
(Lines 2705–3426 of the requested range are unrelated neighbours — `editionDiffHtml`,
`anthropicKey`, the `sess::` session-memory namespace, `warnWriteFail` — and are not part of
the Lookup surface except that `safeGetExistingDirectory` at `:3341` is used by the StarDict
button.)

## 2. User-visible functions

| Control / action | What it does | Code anchor | Automated coverage | Notes / risks |
|---|---|---|---|---|
| Banner text (Lookup explainer) | Static `QLabel` stating the binding-vs-reference rule | `makeLookupPane`, main.cpp:3433 | NONE (no test asserts the banner text) | The honesty claim in the banner is not pinned by any check; edits could silently drop the "binding layer" wording. |
| Query box (`QLineEdit`) + Return | Trims the text and renders the whole stacked card via `lookupResultsHtml(spine, ref, mvp, whitney, colloq, raw)` | `makeLookupPane`, main.cpp:3462, connect at :3595 | app_selftest — Lookup block, main.cpp:38276-38292: "known term returns HGM equivalents", "HGM binding layer precedes reference layers", "honorific term carries its badge"; plus :38363 "verb form klogs folds to present stem klog (CC0 bank, labeled)" and :38380 "ruled pronunciation finds its entry (kamdir class)". Driver-level only (calls `lookupResultsHtml`, not the widget). | The widget wiring itself (returnPressed → setHtml) is never exercised; only the pure function is. gauntlet_walk may type into it randomly. |
| "reference layers" checkbox | Shows/hides the whole reference block; persists to `QSettings` `lookup/showRefs` and re-runs the current query | `makeLookupPane`, main.cpp:3465, connect :3474 | NONE | Writes `QSettings` **without** the `g_harnessRun` guard used by `sess::put` (main.cpp:~3290) — a gauntlet/sweep click can flip the translator's saved preference. The toggle is the honesty control for unlicensed layers, and nothing pins that "off" actually suppresses them. |
| "Local dictionaries (StarDict)…" button | Opens a directory chooser, saves the folder to `QSettings` `lookup/stardictDir`, calls `loadStardicts()`, reports the count in a modal, re-runs the query | `makeLookupPane`, main.cpp:3520, connect :3531 | Indirect only: core suite **stardict_smoke** (core/CMakeLists.txt:351) covers the `allcore::StarDict` reader. The button, the settings write, and the modal: NONE | Loads arbitrary third-party `.ifo/.idx/.dict[.dz]` files of unknown licence into the results stack; parser is fed user-chosen binary data. Also an unguarded `QSettings` write. `safeGetExistingDirectory` (main.cpp:3341) is the sweep-safe wrapper — confirm it short-circuits under `g_sweepActive`. |
| Results view (`QTextBrowser`) | Renders the stacked card; `setOpenExternalLinks(true)` so link-out tier anchors open the system browser | `makeLookupPane`, main.cpp:3486, :3489 | app_selftest, main.cpp:38297: "Adarsha link-out carries a unicode query" (asserts `linkOutHtml`, main.cpp:1916) | `setOpenExternalLinks(true)` opens **any** `http(s)` anchor in the HTML with no confirmation; the URL text is built from dictionary data. External hosts include 84000.co, library.bdrc.io, www2.hf.uio.no, lotsawahouse.org, online.adarshah.org, places.kmaps.virginia.edu. Query terms (the user's headword) leave the machine. |
| Click `jae:<page>` anchor | Opens the Jäschke 1881 PDF at that page, then re-runs the query | `makeLookupPane` lambda, main.cpp:3494 | NONE (`showJaePage`, main.cpp:29752, untested) | Silent no-op if `data/das/jaeschke_1881.pdf` is absent (guarded at :29754) — check the user gets a message rather than nothing. |
| Click `das:<page>` anchor | Opens the Das 1902 PDF at that page, then re-runs the query | `makeLookupPane` lambda, main.cpp:3499 | NONE (`showDasPage`, main.cpp:29788) | Same absent-file path; PDF is a large shipped asset — confirm licensing/packaging. |
| Click `term:<wylie>` anchor (Working Glossary q.v.) | Puts the linked term in the box and re-runs the lookup | main.cpp:3502 | NONE | `anchorPayload` (main.cpp:259) is the decoder — no test pins its unescaping. |
| Click `propose:<term>` anchor | Opens `proposeTermDialog` with provenance string "looked up in the dictionary (Lookup pane)", then re-runs | main.cpp:3506 | app_selftest, main.cpp:38402: "propose-to-the-authority action offered" (asserts the anchor is present in the HTML; the dialog itself is not driven). Core suite **proposals_smoke** (core/CMakeLists.txt:183) covers the store. | The write path to the shared proposals store is only covered at core level. |
| Click `proposeai:<term>` anchor | Opens `promoteAiDraftDialog` — promotes an AI-drafted Working Glossary entry toward the authority — then re-runs | main.cpp:3511 | NONE | **Highest provenance risk on this pane:** the path by which machine-drafted English can move toward the HGM layer. Rule 1 says machine work may match, never compose. Nothing automated pins the labeling or the gate. |
| "Browse the dictionary (Tibetan order)" button | Builds the full headword list once: `spine.allAcipHeadwords()` → `acipToEwts` → `wylieToUnicode`, sorted with `QCollator(QLocale("bo"))`; relabels itself "<N> headwords, Tibetan dictionary order" | `makeLookupPane`, main.cpp:3450, connect :3556 | **collation_smoke** (app/CMakeLists.txt:79) pins the traditional Tibetan collation properties. The button/list build: NONE | Synchronous full-dictionary sort (~105k headwords) on the GUI thread with a single `processEvents()` — a multi-second freeze with no progress and no cancel. `browseBtn->setEnabled(false)` at :3560 is **never restored**, so the control is permanently dead after one click (it becomes a label). Rows whose `wylieToUnicode` fails fall back to displaying wylie — an untagged silent approximation in the display column (rule 3). |
| Browse list click (`QListView`) | Puts the row's wylie into the box and runs the normal lookup | main.cpp:3586 | NONE | Row→wylie mapping is a parallel `QStringList` (`browseWylie`) indexed by row; bounds-checked at :3588, but any future filter/sort of the model desynchronises it. |
| Hunt palette hand-off (⌘K → Lookup) | `g_lookupQuery` lambda sets the box, runs the query and raises this pane | main.cpp:3604 | app_selftest: `g_hunt->selfTest(log)` at main.cpp:38424 covers the Hunt palette's own answers; the hand-off into this pane: NONE | Global function pointer overwritten at construction — a second Lookup pane would silently steal the hook. |
| **⌘D "Look Up Selection…"** (LookupPopup) | Grabs the selection from the focused text widget and opens the floating dictionary window on it | menu action main.cpp:37248-37264; `LookupPopup::run`, main.cpp:2659 | app_selftest, main.cpp:38395-38399: "⌘D popup answers with the full stack" (drives `run("bsod nams")` and asserts the ACIP headword in `viewText()`). The menu action / selection grab: NONE | Empty selection just shows and focuses the window (:2663) — reasonable. Reachable only from the menu/shortcut, not from the Lookup tab. |
| Popup query box + Return | Same lookup, into the popup view | `LookupPopup` ctor, main.cpp:2603, connect :2620 | Same "⌘D popup answers with the full stack" check (driver-level) | — |
| Popup ← / → keys on the results | Steps the TENTATIVE alignment block (`alignCursor_ ±1`), re-renders | `LookupPopup::eventFilter`, main.cpp:2681 | NONE | Cursor is unbounded here; the card normalises it (per the 2026-09-04 comment) — nothing pins that invariant, so a drifting integer is one card-side regression away from an out-of-range render. |
| Popup `alignprev:` / `alignnext:` / `aligncur:<n>` anchors | Same stepper, via clickable links in the card | main.cpp:2624-2634 | NONE | `aligncur:` takes an arbitrary attacker-free but unvalidated integer from the HTML. |
| Popup `term:` / `jae:` / `das:` / `propose:` / `proposeai:` anchors | Same actions as the pane, with provenance string "looked up in the ⌘D dictionary window" | main.cpp:2634-2650 | NONE | Duplicated dispatch logic — the pane's and the popup's anchor handlers are two copies that must be kept in sync by hand. |
| Popup `http…` anchors | `setOpenLinks(false)` + explicit `QDesktopServices::openUrl(u)` | main.cpp:2651 | NONE | Better posture than the pane's blanket `setOpenExternalLinks(true)`, but still unconfirmed navigation. |

**Count: 18 user-visible controls/actions; 12 with NONE automated coverage.**

## 3. Internal functions

| Name | Line | Purpose | Called from |
|---|---|---|---|
| `makeLookupPane` | main.cpp:3427 | Builds the whole Lookup tab widget and wires every control | `main()`, main.cpp:36062 |
| `lookupResultsHtml` | main.cpp:2054 | The pure renderer: spine lookup + ACIP/affix fallbacks + every reference layer → one HTML card. The only thing the selftest can drive. | pane returnPressed (:3600), `LookupPopup::run` (:2671), app_selftest (:38280, :38290, :38366, :38384) |
| `linkOutHtml` | main.cpp:1916 | Builds the external link-out tier (84000, BDRC, Adarsha, Lotsawa House, Polyglotta, KMaps) | `lookupResultsHtml`; asserted in app_selftest :38297 |
| `anchorPayload` | main.cpp:259 (fwd :269) | Decodes the payload after a custom-scheme anchor prefix | pane anchor handler (:3503, :3508, :3512), `LookupPopup` (:2634, :2643, :2649) |
| `loadStardicts` | main.cpp:204 | Loads every StarDict dictionary in `lookup/stardictDir` into `g_stardicts` | StarDict button (:3540), lazily from `lookupResultsHtml` (:2258) |
| `safeGetExistingDirectory` | main.cpp:3341 | Sweep-safe wrapper over the native directory chooser | StarDict button (:3535) |
| `showJaePage` | main.cpp:29752 (fwd :858) | Opens the Jäschke 1881 PDF viewer at a page | pane (:3496), popup (:2637) |
| `showDasPage` | main.cpp:29788 (fwd :809) | Opens the Das 1902 PDF viewer at a page | pane (:3500), popup (:2640) |
| `proposeTermDialog` | (declared earlier in main.cpp) | Proposal-to-the-authority dialog with provenance string | pane (:3507), popup (:2642) |
| `promoteAiDraftDialog` | (declared earlier in main.cpp) | Promote an AI-drafted Working Glossary entry | pane (:3512), popup (:2648) |
| `LookupPopup::LookupPopup` | main.cpp:2597 | Builds the floating ⌘D window and its anchor dispatch | `main()`, main.cpp:36065 (single static instance) |
| `LookupPopup::run(QString)` | main.cpp:2659 | Normalises the query, resets the alignment stepper on a new headword, renders, shows and raises | ⌘D action (:37263), popup returnPressed (:2621), anchor handler, `eventFilter`, app_selftest (:38396) |
| `LookupPopup::viewText()` | main.cpp:2677 | Plain-text accessor — the only test seam on the popup | app_selftest, main.cpp:38397 |
| `LookupPopup::eventFilter` | main.cpp:2681 | ← / → stepping over the TENTATIVE block | installed on `view_` at main.cpp:2618 |
| `g_lookupPopup` | main.cpp:2704 | App-wide single popup pointer | ⌘D action (:37252), selftest (:38395) |
| `g_lookupQuery` / `g_raisePane` | assigned main.cpp:3604 / declared :2758 | Hunt-palette hand-off into this pane | Hunt palette (⌘K, main.cpp:37240) |

## 4. Dependencies

**Data read (all under the repo/app `root`)**
- `build/hgm_spine_v27_2.db` — the HGM master spine (binding layer), via `allcore::Spine`.
- `build/reference.db` — `allcore::RefDict`: local-only THL/TD reference compilation (OT, IW and the other layers named in the checkbox tooltip).
- `data/extracted/mahavyutpatti.tsv` — DILA TEI Mahāvyutpatti, CC BY-SA 3.0.
- `data/whitney/whitney_roots.tsv` — Whitney Roots 1885 / Grammar 1879; PD text, Apache-2.0 digitization.
- `data/pron_colloquial/colloquial_pron.tsv` — colloquial pronunciation register.
- `data/das/das_1902_bookmarked.pdf` + `data/extracted/das_headwords.tsv` (main.cpp:36025 area) — Das page link-outs.
- `data/das/jaeschke_1881.pdf` + `data/extracted/jaeschke_pages.json` (main.cpp:36044-36059) — Jäschke page link-outs.
- Any user-chosen folder of StarDict `.ifo/.idx/.dict[.dz]` files (`lookup/stardictDir`).
- The Working Glossary / AI-draft store and the proposals store (via `lookupResultsHtml` and the propose dialogs).

**Network** — no direct `QNetworkAccessManager`/`TimedNam` use in this pane. Outbound is
only via opened browser URLs from the link-out tier: `84000.co`, `library.bdrc.io`,
`online.adarshah.org`, `www.lotsawahouse.org`, `www2.hf.uio.no` (Polyglotta),
`places.kmaps.virginia.edu`. The user's search term is placed in the query string of each.

**QSettings (org "ALL", app "TranslationTool")**
- `lookup/showRefs` (bool, default true) — reference-layer visibility. Written at main.cpp:3477.
- `lookup/stardictDir` (string) — user StarDict folder. Written at main.cpp:3542.
Neither is routed through `sess::put`, so neither gets the `g_harnessRun` guard.

**Test suites bearing on this surface**: `app_selftest` (the Lookup block, main.cpp:38273-38403 — the only real coverage), `gauntlet_walk` (random 300-step UI walk, seed 1), `collation_smoke` (Tibetan browse order), `font_smoke`, `dmp_smoke` (unrelated). Core: `spine_smoke`, `stardict_smoke`, `mvp_smoke`, `whitney_smoke`, `colloquial_smoke`, `proposals_smoke`, `glossary_smoke`, `searchnorm_smoke`, `affixnorm_smoke`, `contractions_smoke`, `tm84000_smoke`. **No core `add_test` targets `RefDict` by name** — the THL/TD reference layer that this pane renders has no dedicated smoke suite.

## 5. Release questions

- **Per-dictionary licensing — `build/reference.db`.** The checkbox tooltip already calls these "unlicensed compilations, local lookup only". What exactly is inside (THL `thl.wrd`/`thl.def`, TibetanDictionary.dic from ACIPHypercontext, OT, IW), and is each one shippable? The roadmap flags the Hypercontext payload as "CHECK LICENSING BEFORE SHIPPING" and THL as "per-dictionary licensing review" — both are still open, and this pane is where they surface to the user.
- **StarDict user layer.** Shipping a loader for arbitrary third-party dictionaries is fine, but confirm the results card labels them "LOCAL DICTIONARIES · USER-SUPPLIED" in every render path, and that nothing exports or re-publishes their text.
- **PDF assets.** Das 1902 and Jäschke 1881 are public-domain texts, but confirm the specific scans/bookmark layers being bundled are redistributable, and that a missing PDF tells the user rather than doing nothing.
- **Mahāvyutpatti (CC BY-SA 3.0)** requires attribution and share-alike — is the attribution visible in the card, and does any export of the card carry it?
- **Provenance gate on `proposeai:`.** The AI-draft promotion path has zero automated coverage. Before release it needs an app_selftest check that the promoted item stays labeled and cannot land in the `hgm_gloss` layer (rule 1).
- **Settings writes under the harness.** `lookup/showRefs` and `lookup/stardictDir` bypass `sess::`; a `--gauntlet`/`--sweep` run can move the translator's furniture. Route them through `sess::put` or add the `g_harnessRun` guard.
- **Browse freeze.** A synchronous ~105k-row collation sort on the GUI thread with no progress or cancel, and a button that stays disabled forever afterwards. Decide whether this ships as-is.
- **Blanket `setOpenExternalLinks(true)`** on the pane's results view versus the popup's explicit `QDesktopServices::openUrl`. Pick one posture; the pane's is the looser of the two, and it sends the user's search term to six external hosts without confirmation — a privacy note for translators working on unpublished material.
- **Duplicated anchor dispatch** between `makeLookupPane` (:3491-3515) and `LookupPopup` (:2622-2652). Two copies, one of them untested. Worth extracting before more schemes are added.
