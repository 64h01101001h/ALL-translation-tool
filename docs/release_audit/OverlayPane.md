# Release audit — `OverlayPane` (app/main.cpp:4569–15515)

## 1. What it is

## Shared vocabulary (agreed with Adam, 2026-09-08 — use these names in every row and every conversation)

Window frame: **Title bar** · **Pin bar** (★ row) · **Group tabs** (Read … Community, **Hunt (⌘K)**) · **Ribbon** with ribbon sections **READ** (pane switchers), **DOCUMENT** (Open ACIP file, Load into overlay, Outline, Citations & quotations, Verse meter, Prose & prayer reader), **REVIEW** (Text DNA, Typography check, Glossary, Team comments, Teachings for this text), **SPELLING** (Show spelling checkbox) · **Ribbon scrollbar** · **Status bar** (release · user/role · proposals pending).

Left column (control column): **Document box** (Document (ACIP) editor) · **Author line** (author, dates, BDRC / Treasury of Lives / works links) · **Scan viewer button** · **Display panel** = **Script row** (text as, font) + **Shading row** + **Card layers** (presets + 11 checkboxes) + **Document marks** (3 checkboxes) · **Document summary** (collection, text, tokens, spans, entries, flag counts).

Right column: **Reading pane** (upper text view; the **document-title strip** added 2026-09-08 belongs along its top) · **Card pane** = **Segmentation line** · **Nesting line** · **Action links** (add to glossary · propose to the authority) · **Headword block** (script, ACIP, [generated script], pron) · **HGM block** · **Reference lines** (Das 1902, sanskrit, suffix agreement, Monlam) · **Evidence block** (EVIDENCE / FROM THE CORPUS citation cards) · **Card scrollbar**.


The Overlay is the tool's centre of gravity and by far its largest single class (~10,950 lines, roughly a quarter of `app/main.cpp`). It is the Hypercontext-style reading surface: the translator opens an ACIP or Wylie text, the pane tokenises it, shades every span the dictionary knows, and clicking a shaded word raises a *card* in the right-hand panel carrying HGM's binding English plus the labelled reference layers (Sanskrit, Hopkins, 84000, Das/Jäschke, LC/TD/THL/OT/IW, corpus concordance, teaching-moment links, machine grammar, AI-tentative alignment). Around that core it has accreted a large number of side tools — outline (sa bcad), citations, verse meter, meter reader, Text DNA, typography check, spelling doubts, per-text glossary, team comments, BDRC scan following with an OCR word-locator and a full woodblock viewer window, eKangyur comparison, and the whole Pecha/Mixed-Nuts PDF export engine. It is constructed in `main()` and added as the first tab, `tabs.addTab(overlay, "Overlay")` (main.cpp:35851), inside the "Read" ribbon group (`mkGroup("Read", {"Overlay", "Library", "Files", "Scans", …})`, main.cpp:36554). The user reaches it by launching the app (it is the default tab and restores the previous session via `restoreSession()`, main.cpp:40549), by File ▸ Open ACIP File… (⌘O), File ▸ Open Recent, File ▸ Dossiers…, or by any cross-pane jump that calls `overlay->openFile()` / `openFileAtLine()`.

Its `selfTest(QStringList&, const QString& root)` (main.cpp:4573–6492) is ~1,920 lines and asserts **135** `check(...)` conditions — the largest battery in the app. It runs under the `app_selftest` ctest suite (`app/CMakeLists.txt:89`, `DiamondCutterTranslationTool --selftest`).

---

## 2. User-visible functions

### 2a. Ribbon / toolbar buttons (constructed 6598–7524)

| Control/action | What it does | Code anchor | Automated coverage | Notes/risks |
|---|---|---|---|---|
| **Open ACIP file…** (button) | `safeGetOpenFileName` then `openFile()`; detects ACIP vs Wylie, converts, loads, pushes onto `file/recents` | ctor lambda main.cpp:7013 → `openFile()` 6493 | `"ACIP sample detected as ACIP"`, `"lowercase sample detected as wylie"`, `"full-volume openFile under the widget ceiling"` (6479) | Wylie files are auto-converted; the hint text discloses this (`"wylie conversion is labeled in the hint"`, 5498). Writes QSettings `file/recents`. |
| **Load into overlay** (button) | Re-tokenises whatever is in the Document (ACIP) box and rebuilds the shaded view | 7488 → `loadDoc()` 7551 | `"tokens parsed"`, `"dictionary spans found"`, `"entries resolved"`, `"script conversion clean (no failure markers)"` | `loadDoc()` is ~610 lines and is the pane's hot path; it is re-entered by every doc-affecting toggle. |
| **Outline (sa bcad)…** | Extracts the text's topical-outline headings into a navigable tree dialog | 6771 → `showSaBcad()` 11250, `extractSaBcad()` 11181 | `"expected 5-node tree"` (6004), `"its announcing ordinal (TEST-10)"` (6033), `"old 64-slot bound, and still yields an outline"` (4748), `"parent exactly one level above it"` (4766) | Heading detection is heuristic (cardinal/ordinal Tibetan number words); no confidence label on the tree. |
| **Citations && quotations…** | Scans the text for quoted passages, locates each in the library, opens a report with pin/export | 6781 → `showCitations()` 12299, `extractCitations()` 11911 | `"extracts in order, deduped"` (6042), `"lands on the quotation it names"` (5693), `"is instead of jumping into the cited text"` (5710), `"built for and the text now open"` (5715), `"is reported, never a silent no-op"` (5727) | `citeStaleNotice_` guards the case where the report was built for a different document — good. `locateQuoteInFile()` (11999) does a linear file scan per citation. |
| **Verse meter…** | Counts syllables per line, blocks them into stanzas, reports irregularities | 6790 → `showVerseMeter()` 11426, `meterUnits()` 11340, `meterBlocks()` 11371 | `"stanza gap)"` (5347), `"marked, prose labeled"` (5752), `"grouped (stricter rule)"` (5923), `"flagged"` (5940) | Reads `data/extracted/meter_census.json` via `loadMeterCensus()` (11495); absent file degrades silently. |
| **Meter reader** (button, label at 6792) | Opens the reading view laid out by metrical unit, clickable back to source line | 6802 → `showMeterReader()` 11823, `meterReaderHtml()` 11509 | `"and the strip's jump lands on the line"` (5961) | Uses QSettings `meters` and `share_pct`. |
| **Text DNA (structure strip)…** | Renders a structural fingerprint strip of the whole text (verse/prose/heading bands) | 6814 → `showTextDna()` 11702, `textDnaSummary()` 11674 | `"end (FAIL-3's first consumer)"` (5988) | Purely visual summary; no export. |
| **Typography check** (button 6815) | Lists shad/tsheg/spacing findings with jump-to-line | 6825 → `showTypographyCheck()` 11625, `typographyFindings()` 12822 | `"clean page yields none"` (6084), `"shad and tsheg ends refused"` (5857), `"༈ rides an NBSP block"` (5881) | |
| **Show spelling doubts** (checkbox) | Toggles the spelling-doubts side panel built from the allcore syllable checker | 6827, toggle at 6919 → `rebuildSpellPanel()` 8206, `collectSpellHits()` 8166 | `"illegal syllable raises one spellcheck flag"` (4967), `"spelling-doubts panel lists the doubted form"` (4970) | |
| **Check all** (in spell panel) | Runs the checker across the whole document rather than the visible window | 6863 | Covered indirectly by the two spell checks above | No progress UI on a long volume. |
| **File for ruling** (in spell panel) | Files the doubted forms into the proposals store for authority ruling | 6867 → `fileCheckedSpellHits()` 8431 | NONE | **Writes to the proposals store**; no selfTest asserts the write. |
| **spell-hit row click** | Jumps the document cursor to that doubted syllable | 6869 → `jumpToSpellHit()` 8414 | NONE | |
| **Teachings for this text…** | Builds the timecoded report of moments in HGM's recordings that touch this text | 6899 → `teachingsReportHtml()` 8472 | `"teachings-for-this-text report reaches the corpus"` (6090), `"PROVISIONAL beside the recording of him saying it"` (4932) | Emits `https://www.youtube.com` links (opened externally). Also reachable via `--teachings-report` CLI (38242). |
| **Glossary…** | Opens the per-text working glossary workbench (add/edit/delete terms, q.v. links) | 6887 → `glossaryWorkbench()` 14918; **Set** button 14963, **Close** 15014 | `"the glossary path is keyed to the document on screen"` (4798), `"opening a second document moves docFile_ with it"` (4793), `"land in the previous text's file"` (4795) | **Writes** `library/glossaries/<basename>.tsv`. |
| **Team comments…** | Per-document comment thread against the shared comment store | 6896 → `commentsDialog()` 15023; **Add comment** 15094, **Close** 15113 | NONE | **Writes** to the proposals/comment store; uses QSettings `team/name`. No selfTest. |
| **Follow along in scans (BDRC)** | Resolves the open text to a BDRC work, fetches the IIIF manifest, binds folios to cursor | 6957 → `followScans()` 9549, `fetchNextManifest()` 9616 | `"live-verified shape"` (5531), `"(never guessed)"` (5538) | **Network**: `iiifpres.bdrc.io`, `purl.bdrc.io`, `library.bdrc.io`. Writes `linksFile()` (9430). |
| **person badge** (link label) | `authorpid:` link → opens the author's other works | 6941, scheme handled near 4619 area | `"the person's other works"` (4622), `"id"` (4619), `"showing nothing at all"` (4630) | |
| **◀ folio / folio ▶** | Steps the bound scan one folio side back/forward | 7009/7010 → `stepFolio()` 14451 | `"scan-sync: cursor resolves to folio 1a line 2"` (5509), `"…1b line 3/3"` (5512), `"001a/001b sides"` (5516), `"lowercase @folio markers resolve"` (5527) | |
| **locate word (OCR)** (toggle) | Runs the PhotiLines OCR model on the current folio and boxes the selected word | 6990 → `locateSelectionWindow()` 10102, `locateNeedleRects()` 10142, `ensureFolioOcrModels()` 14535 | `"counted column, never silence"` (5549), `"are disclosed against the true total"` (5554), `"not implied"` (5557) | Requires `library/ocr_models/BDRC_PhotiLines/PhotiLines.onnx`; `#ifdef ALL_HAVE_OCR`. Long-running, `folioOcrBusy_` guard. |
| **Scan viewer (window)…** | Opens the full woodblock viewer window (see 2d) | 7006 → `openWoodblockViewer()` 10210 | Indirect only | ~775-line function; see its own sub-table. |
| **text as** (combo: script mode) | Switches the document render between Tibetan unicode / Wylie / ACIP / phonetics / THL phonetics | 7055, 7489 | `"script renders %2"` (6262), `"pronunciation mode renders convention words"` (5012), `"pronunciation mode clean on the sample"` (5015), `"THL phonetics mode renders (sö/bön umlauts)"` (5086) | QSettings `overlay/scriptMode`. |
| **shading** (combo) | Click-mode vs always-shade span highlighting | 7084, 7494 | `"nested terms tone inside the highlighted phrase"` (5001) | QSettings `overlay/shading`. |
| **Tibetan font** (combo) | Chooses the Tibetan face for the document view | 7107, 7146 | `font_smoke` ctest (`app/CMakeLists.txt:83`) | QSettings `overlay/tibFontFamily`. |
| **presets: reading · research · minimal** (links) | One click sets the whole card-layer toggle set | 7220 → `applyPreset()` 8360 | `"card survives display toggles instantly"` (6280) | scheme `preset:`. |

### 2b. Card-layer and document-mark checkboxes (`mkToggle`, main.cpp:7159–7311)

Each persists to QSettings under `overlay/<key>`; non-doc-affecting ones call `refreshCard()`, doc-affecting ones re-run `loadDoc()` and restore the card.

| Checkbox (settings key) | What it does | Coverage | Notes/risks |
|---|---|---|---|
| phonetics (`overlay/phonetics`) | shows the `pron:` line | `"pronunciation mode renders convention words"` | |
| **HGM definitions (`overlay/glosses`)** | the binding ≡ layer | `"a curated term of the loaded text"` (4929), `"known term carries an HGM gloss"` (4957) | **Deliberately re-armed to ON at every launch** (7175–7182) after Adam found it persistently off — inviolable rule 1. Togglable within a session only. |
| corpus usage (`overlay/corpus`) | FROM THE CORPUS block | `"answers sems can"` (5144), `"cap"` (5150) | |
| Sanskrit reference (`overlay/sanskrit`) | Sanskrit comparanda line | `engines_battery` ctest | default off |
| Hopkins reference (`overlay/hopkins`) | Hopkins line | NONE at toggle level | reference-only labelling asserted at 4686 |
| 84000 glossary (`overlay/g84000`) | 84000 CC BY block | `"84000 ledger prints the UNCAPPED segment total"` (5153), `"total"` (5156) | CC BY attribution must survive |
| teaching links (`overlay/teachings`) | timecoded YouTube chips | `"PROVISIONAL beside the recording…"` (4932) | external network on click |
| Das/Jäschke page links (`overlay/das`) | `das:` / `jae:` page anchors | NONE at toggle level | |
| reference dictionaries (`overlay/refs`) | LC/TD/THL/OT/IW block | `"names the layer as reference only"` (4686), `"shown ONCE, not twice"` (4644), `"many, rather than quietly thinning the layer"` (4650), `"capping never empties the layer"` (4672) | **tooltip says "LOCAL ONLY … unlicensed compilations, never release data"** — licensing decision required before release |
| footnotes && bibliography (`overlay/notes`) | published apparatus chips | `"honesty note"` (5642) | reads mixed_nuts notes/bibliography JSON |
| HGM tentative (`overlay/alignTent`) | the AI-matched TENTATIVE layer | `"— the tier is read, not stamped on every row"` (4936) | machine amber; must never look binding |
| grammar marks (`overlay/grammar`, doc-affecting) | particle/verb-stem/bsdus-tshig/honorific marks | `"case particles carry Wilson classes"` (4591), `"the grammar block never wears a reserved green (G1)"` (4595), `"absence, not fabrication"` (4598) | |
| Botok segmentation (`overlay/segmentation`) | dotted reference word boundaries | `botok_smoke`, `botok_tok_smoke` ctest; `"boundaries (tsema namdrel)"` (5039) | needs `data/botok` |
| unattested-word hints (`overlay/attestation`, doc-affecting) | shades words the segmenter + Monlam lists don't know | `"engine (kamdir)"` (5025); build cost measured at 39904 | Builds a **548k-form** lexicon on first use (`ensureSegmenter()` 15340, `loadLexicon()` 14835); cancellable (`segCanceled_`), retry armed by unticking (7312). PERF-5. |

### 2c. Anchors / link schemes on the card (`context_` anchorClicked, main.cpp:7357–7422) and in the source box

| Scheme | What it does | Anchor | Coverage | Notes/risks |
|---|---|---|---|---|
| `alignprev:` / `alignnext:` / `aligncur:N` | steps the TENTATIVE witness stepper | 7360 → `alignStep()` 8396 | `"the tier is read…"` (4936) | duplicated verbatim in `LookupPopup` (main.cpp:2622) — drift risk |
| `citefind:` / `citeopen:` / `citefile:` | citation-report navigation | `handleCiteAnchor()` 12105 | `"lands on the quotation it names"` (5693), `"is instead of jumping into the cited text"` (5710) | |
| `jae:N` | opens Jäschke 1881 scanned page N | 7374 | NONE | |
| `das:N` | opens Das 1902 scanned page N | 7378 | NONE | |
| `corpusall:` / `corpusless:` | expands/collapses the corpus block in place | 7382 | `"card survives display toggles instantly"` (6280) | |
| `term:` | q.v. → runs the term in the Lookup popup | 7390 | NONE | no-ops silently if `g_lookupQuery` is null |
| `gloss:` | prompts for a per-text glossary equivalent and saves it | 7396 → `saveGlossaryEntry()` 15282 | `"the glossary path is keyed to the document on screen"` (4798) | **writes to disk**; `"custom-scheme anchors decode spacey payloads"` (6318) |
| `propose:` | files a dictionary proposal from the card | 7418 → `proposeFromCard()` 15205 | `proposals_smoke` ctest | **writes to the proposals store** |
| `proposeai:` | promotes an AI draft gloss toward the authority | 7420 → `promoteAiDraftDialog()` | NONE | the most sensitive lane in the app; no selfTest |
| `preset:` | applies a layer preset | `applyPreset()` 8360 | `"card survives display toggles instantly"` | |
| `http…` | `QDesktopServices::openUrl` | 7422 | NONE | **external browser**, unfiltered host |
| `line:` / `jumppos:` / `card:` / `edit:` / `del:` | in-report jumps, glossary edit/delete | various in 11500–15300 | `"and the strip's jump lands on the line"` (5961) | `del:` deletes a glossary row — no confirm found |
| `authorpid:` | opens the person's other works | ~4619 region | `"the person's other works"` (4622) | |

### 2d. Document (ACIP) box context menu (main.cpp:6626–6756)

| Item | What it does | Coverage | Notes/risks |
|---|---|---|---|
| *(folio location header, disabled)* | shows the resolved folio/line for the cursor | `resolveFolioAt()` checks 5509/5512 | |
| Show this folio in the scan panel | binds the scan panel to that folio | 5509 family | |
| Open this folio in the Input workflow | hands the folio to InputPane | NONE | |
| Locate selection on the woodblock… | OCR word-locate for the selection | `"counted column, never silence"` (5549) | OCR run, slow |
| Download all N folio scans | bulk IIIF fetch of the whole volume | `downloadAllFolios()` 9982; `"while any folio is missing from the store"` (5561), `"when every folio is in the store"` (5566), `"absent, so the next run re-fetches it"` (5585), `"as already present"` (5593) | **network + many disk writes**; partial-file stub handling at 9868–9919 |
| Open scan viewer (window)… | see 2e | indirect | |
| Link the scans first (Follow along…) | disabled-state hint | n/a | |
| Export folio-faithful text files… | writes one text file per folio side | `exportFolioSides()` 10052; `splitFolioSides()` 5516 | **disk writes** |
| Compare with the Kangyur/Tengyur (Derge edition) | diffs against the eKangyur e-text | `compareWithEkangyur()` 11007; `"(markers preserved)"` (5492), `"eKangyur marker stripping keeps pure text"` (6047), `"concordance (Toh 551 -> D.555)"` (5601), `"through the verified concordance"` (5829), `"spine pointer honored, traversal refused"` (5829), `"(KL16 -> part 0018, the Diamond Cutter)"` (6058) | reads `editions/` |
| Copy location | copies the folio/line string | NONE | |

### 2e. Woodblock viewer window (`openWoodblockViewer()`, main.cpp:10210–10986)

| Control | What it does | Anchor | Coverage | Notes |
|---|---|---|---|---|
| work crumb (toolbutton) | climbs to the whole-volume overview grid | 10271, 10945 | NONE | |
| folio crumb (toolbutton) | folio picker | 10283 | NONE | |
| collapse (toolbutton) | hides the thumbnail rail | 10317 | NONE | |
| tool buttons row | adjust / info / etc. | 10346 | NONE | |
| go-to folio (combo) | jumps to a folio | 10376 | NONE | |
| **Show e-text** (checkbox) | overlays the e-text beside the scan | 10395 | NONE | |
| zoom % (combo, incl. "Fit") | zoom level | 10411 | NONE | |
| brightness / contrast sliders | image adjustment | 10786–10809 | NONE | |
| Invert (white on black) (checkable action) | inverts the scan | 10810 | NONE | |
| About this scan (info button) | work / folio-sides / license message box | 10820 | `scanLicense_` surfaced | license text must be correct at release |
| ⌘→ / ⌘← | step folio | 10960/10962 | NONE | |
| ⌘+ / ⌘= / ⌘− / ⌘0 | zoom in / out / fit | 10964–10971 | NONE | |

**The entire viewer window has NO automated coverage.**

### 2f. Keyboard, mouse and window-level

| Action | What it does | Anchor | Coverage | Notes |
|---|---|---|---|---|
| click a shaded word | raises the card; repeated clicks cycle outward through nesting | `onClick()` 8760 (~430 lines) | `"arrow-key reading walks the phrase highlight"` (4988), many card checks | |
| **double-click** in the text | counts as one more nesting step and swallows auto-selection | `eventFilter()` 8626 | `"double-click does not select the line"` (6330) | |
| ← / → (text) | walk phrase by phrase | `navPhrase()` 8710 | `"arrow-key reading walks the phrase highlight"` (4988) | |
| ↓ / ↑ (text) | step down / up the nesting chain | `eventFilter()` 8664–8680 | 4988 family | |
| ⌥← / ⌥→ (text), ←/→ (card) | TENTATIVE witness stepper | `eventFilter()` 8637–8650 | `"the tier is read…"` (4936) | |
| hover on the woodblock image | OCR-derived wylie + HGM English tooltip, **cache-only** | `carvingHoverText()` 14617 | NONE | `#ifdef ALL_HAVE_OCR`; explicitly never starts an OCR run |
| document-title resize | re-elides the path label | `eventFilter()` 8620 → `refreshDocTitle()` 12670 | `"pasted document (unsaved)"` behaviour per `documentLabel()` 12654 | STATIC-5 lesson: never a stale filename |
| session autosave (30 s timer + aboutToQuit) | persists scroll/cursor/nest position | 7504/7513 → `saveSession()` 8302, `restoreSession()` 8317, `restoreSpot()` 8336 | `"the saved cursor spot + nest rung"` (6131), `"restored as nothing"` (6181) | QSettings `overlay/last*` |
| splitter drag | remembers the Document-box split | 7477 | NONE | QSettings `overlay/inputSplit` |

### 2g. Menu bar actions that drive this pane (main.cpp:36660–36870, 37200–37225)

| Menu item | What it does | Anchor | Coverage | Notes |
|---|---|---|---|---|
| File ▸ Open ACIP File… (⌘O) | `overlay->openFile()` | 36669 | shares `openFile` checks | |
| File ▸ Open Recent ▸ … | reopens from `file/recents`; vanished files shown **disabled**, never guessed away | 36686–36704 | NONE | |
| File ▸ Dossiers… (`open:` / `save:` / `drop:` anchors, **New from current text**, **Close**) | reassembles a text's working world; jumps overlay to the saved line | 36710–36858 | `"overlay->docFile() == txt && overlay->currentLine() == 37"` gauntlet check at 39732 | all three write paths warn on failure (good) |
| View ▸ Larger / Smaller / Actual Text Size (⌘+ / ⌘− / ⌘0) | routes to `overlay->zoomRouted()` by focus | 37202–37225 → `zoomRouted()` 8271 | `"step, and reset lands on the raised defaults"` (6240) | routes to text / card / input box separately |
| Read ▸ Overlay submenu (auto-generated from the pane's buttons) | mirrors every pane button into the menu bar | 36554 | `"MenuBar: Read > Overlay submenu"` assertion at 40194 (requires > 8 actions) | count-only check — it does not verify the actions work |

### 2h. Pecha / Mixed-Nuts export (lives in OverlayPane, driven from ExportPane)

These four are *invoked* by ExportPane's buttons (main.cpp:15900–15949) but implemented here.

| Function | What it does | Anchor | Coverage | Notes/risks |
|---|---|---|---|---|
| Prepare for translation (Mixed Nuts) | produces the Mixed-Nuts prep document | `prepareForTranslation()` 12694 | `"text passes, break protection applied"` (5796) — driven end-to-end at 5968–5980 | selftest stubs the save dialog and runs the real path |
| Pecha PDF export (dialog) | full pecha imposition dialog and PDF write | `pechaExport()` 13768 (~370 lines dialog) + `pechaWritePdfText()` 13186 (~580 lines) | `"imposed two-up PDF with interlinears"` (5189), `"sheets the PDF really has"` (5268), `"booklet layouts (no silent downgrade)"` (5405), `"woodblock dims and its 8-line rule (DATA-10)"` (5422), `"flags the label page's effect on the saddle"` (5472), `"exact; 6 pads with blanks)"` (5895), `"never invented"` (5909) | **writes PDFs**; ~12 QSettings `pecha/*` keys read/written |
| Pecha batch | folder of mixed ACIP+Wylie → many PDFs | `pechaBatch()` 14276, `pechaBatchDirs()` 14185 | `"→ 3 pecha PDFs"` (5387) | **bulk disk writes**, no per-file error surface reviewed |
| Export print Tibetan | plain print-ready Tibetan text | `exportPrintTibetan()` 12776 | `tibexport_smoke` ctest | ExportPane blanks the results view on empty input |
| Pecha dialog controls | preset combo (13780), rules combo (13864), layout combo (13873), title-folio (13891), heads (13903), verse (13908), cover sheet (13918), shad conversion (13928), classical opening (13939), interlinear phonetics (13949), interlinear English (13953), **Preview…** (14059), **Make PDF…** (14064) | 13780–14070 | layout/sheet-count checks above | 13 controls; only the *computed output* is tested, never the controls |

### 2i. Other user-visible entry points

| Control/action | What it does | Anchor | Coverage | Notes |
|---|---|---|---|---|
| Title search dialog (**Search BDRC**, **Use selected (saves the link)**) | searches BDRC for the text's title and saves the resolved work link | `titleSearchDialog()` 9456, buttons 9467/9471, `deriveTitleQuery()` 9438 | NONE | **network** to BDRC; **writes** `linksFile()` |
| Four-layer view | side-by-side Tibetan / phonetics / gloss / English | `fourLayerView()` 12478, `attestedEnglish()` 12444 | `"English — attestation means the segment IS the line"` (4813), `"an exactly-attested line still shows its English"` (4820), `"corpus segment only CONTAINS"` (4877), `"empty and unattested input yield no English"` (4836), `"attested count (DATA-11)"` (4832) | strong coverage; the honesty rule here is well pinned |
| Illustration gallery (cached / whole volume) | thumbnail gallery of the volume's scans | `galleryCached()` 14294, `galleryWholeVolume()` 14326 | NONE | whole-volume = **unbounded network fetch**; also surfaced by ScansPane |
| Citations report **Pin to side panel** / **Save as text file…** | pins the report, or writes it out | 12390, 12086 → `exportCitationsDoc()` 12072, `citationsDocText()` 12017 | `"is reported, never a silent no-op"` (5727) | **disk write** |
| Scans action / status | drives and reports the scan binding | `scansAction()` 14415, `scansStatus()` 14423 | ScansPane's single weak check | |

---

## 3. Internal functions

| Name | Line | Purpose | Called from |
|---|---|---|---|
| `selfTest` | 4573 | 135-check battery | `main()` 38253 (`app_selftest`) |
| `openFile` | 6493 | open + detect script + load + record recent | menus, gauntlet, CLI, dossiers |
| `OverlayPane` (ctor) | 6598 | builds the whole UI | `main()` |
| `washColor` | 7524 | span wash colour by hue/depth/provisional | `loadDoc`, card render |
| `tokEwts` | 7547 | token → EWTS | card + lookup paths |
| `loadDoc` | 7551 | tokenise, resolve spans, render the document | Load button, every doc-affecting toggle |
| `SpellHit` / `collectSpellHits` | 8161/8166 | gather syllable-checker hits | `rebuildSpellPanel` |
| `rebuildSpellPanel` | 8206 | fill the doubts table | spell toggle, `loadDoc` |
| `scaleCardPx` | 8238 | rescale card CSS px by percent | `setCardHtml` |
| `setCardHtml` | 8262 | set card HTML at current zoom | `refreshCard`, `onClick` |
| `zoomRouted` | 8271 | route zoom to focused surface | View menu |
| `textZoomStep` / `inputZoomStep` / `cardZoomStep` | 8277/8285/8293 | per-surface zoom | `zoomRouted` |
| `saveSession` / `restoreSession` / `restoreSpot` | 8302/8317/8336 | session persistence | timer, quit, startup |
| `applyPreset` | 8360 | set layer toggles from a preset | `preset:` links |
| `alignStep` | 8396 | TENTATIVE witness stepper | anchors + ⌥ arrows |
| `refreshCard` | 8404 | re-render card in place | every card-only toggle |
| `jumpToSpellHit` | 8414 | cursor → doubted syllable | spell table click |
| `fileCheckedSpellHits` | 8431 | file doubts for ruling | "File for ruling" |
| `teachingsReportHtml` | 8472 | teaching-moment report | button + CLI |
| `eventFilter` | 8619 | keyboard/dblclick/hover routing | Qt |
| `navPhrase` | 8710 | phrase-by-phrase walk | ← / → |
| `tokenAt` | 8750 | binary-search char pos → token | `onClick` (hot path) |
| `onClick` | 8760 | the card builder — the pane's core | cursor moves, dblclick, arrows |
| `alignGrammarHtml` | 9191 | machine-morphology block | `onClick` |
| `tohToThl` / `klToH` | 9284/9314 | Toh↔THL and KL↔H concordances | eKangyur compare |
| `setScanTarget` | 9335 | bind pane to a scanned work | `followScans`, InputPane |
| `linksFile` | 9430 | path of the saved scan-link file | link save/load |
| `deriveTitleQuery` | 9438 | build a BDRC search query from the text | `titleSearchDialog` |
| `titleSearchDialog` | 9456 | BDRC title search UI | scan-follow fallback |
| `followScans` | 9549 | resolve + fetch manifests | scan button |
| `docFolioSequence` | 9596 | folio order implied by the document | folio nav |
| `fetchNextManifest` | 9616 | IIIF manifest pump | `followScans` |
| `syncFolioToCursor` | 9733 | keep scan in step with the caret | cursor moves |
| `FolioLoc` / `resolveFolioAt` | 9792/9794 | char pos → folio + line | context menu, sync |
| `splitFolioSides` | 9821 | split text into folio sides | `exportFolioSides` |
| `folioScansDir` / `folioLocalPath` / `folioCached` | 9849/9854/9873 | scan cache layout | download paths |
| `ensureFolioLocal` | 9878 | fetch one folio if absent; stub-safe | `showFolio`, downloads |
| `folioBatchReport` | 9945 | honest got/had/missing report | `downloadAllFolios` |
| `downloadAllFolios` | 9982 | bulk volume fetch | context menu |
| `exportFolioSides` | 10052 | write per-folio text files | context menu |
| `selectionWylie` | 10086 | selection → wylie | OCR locate |
| `locateSelectionWindow` | 10102 | OCR word-locate driver | locate toggle |
| `locateNeedleRects` | 10142 | find the word's boxes on the folio | above |
| `openWoodblockViewer` | 10210 | the viewer window (~775 lines) | viewer buttons |
| `ekStripMarkers` | 10987 | strip e-text apparatus | eKangyur compare |
| `editionDir` | 10995 | edition folder path | eKangyur compare |
| `compareWithEkangyur` | 11007 | diff against Derge e-text | context menu |
| `SaBcadNode` / `SaBcadShape` / `saBcadShapeOf` / `saBcadCardinal` / `saBcadOrdinal` / `extractSaBcad` / `showSaBcad` | 11107–11250 | outline extraction + dialog | Outline button |
| `MeterUnit` / `MeterBlock` / `meterUnits` / `meterBlocks` / `showVerseMeter` / `loadMeterCensus` / `meterReaderHtml` / `showMeterReader` / `jumpToSourceLine` | 11333–11878 | metre analysis + reader | Verse meter / Meter reader |
| `showTypographyCheck` / `typographyFindings` | 11625/12822 | typography audit | Typography button |
| `textDnaSummary` / `showTextDna` | 11674/11702 | structure strip | Text DNA button |
| `Citation` / `libTitleIndex` / `extractCitations` / `locateQuoteInFile` / `citationsDocText` / `exportCitationsDoc` / `handleCiteAnchor` / `citationsReportHtml` / `showCitations` | 11878–12299 | the citations subsystem | Citations button |
| `attestedEnglish` / `fourLayerView` | 12444/12478 | attestation-gated English | four-layer view |
| `tibDigits` | 12641 | int → Tibetan numerals | pecha, folio labels |
| `hasDocument` / `documentLabel` / `refreshDocTitle` | 12651/12654/12670 | document identity strip | title label |
| `prepareForTranslation` | 12694 | Mixed-Nuts prep | ExportPane |
| `exportPrintTibetan` | 12776 | print-ready Tibetan | ExportPane |
| `pechaLineTransform` / `pechaDoubleShadEnd` / `pechaBreakProtect` / `pechaBookletOrder` / `pechaSheetCount` / `pechaLayoutName` / `pechaImposeLine` / `stanzaShad` / `PechaOpts` | 12892–13049 | pecha typesetting primitives | pecha writers |
| `openFileAtLine` | 13089 | open + scroll to a line | cross-pane jumps (35970) |
| `wylieDocToAcip` | 13118 | whole-document wylie → ACIP | `openFile` |
| `tibetanizeField` | 13171 | Tibetanise a metadata field | pecha covers |
| `pechaWritePdf` / `pechaWritePdfText` | 13181/13186 | the PDF writers | `pechaExport`, batch |
| `pechaExport` / `pechaOptsFromSettings` / `pechaBatchDirs` / `pechaBatch` | 13768–14276 | pecha UI + batch | ExportPane |
| `galleryCached` / `galleryWholeVolume` | 14294/14326 | scan galleries | ScansPane |
| `scansAction` / `scansStatus` | 14415/14423 | scan-state facade | ScansPane |
| `stepFolio` / `volCacheDir` / `showFolio` / `fetchScan` | 14451–14497 | folio navigation + fetch | folio buttons |
| `FolioOcr` / `ensureFolioOcrModels` / `folioOcrFor` / `folioPageX` / `carvingHoverText` / `testSeedFolioOcr` / `setScanPixmap` | 14528–14708 | folio OCR + hover English | locate, hover |
| `loadLexicon` | 14835 | build the 548k-form attestation lexicon | `ensureSegmenter` |
| `docGenLabel` / `docFile` / `currentLine` / `scrollToLine` | 12896…14904–14908 | document identity accessors | menus, dossiers, gauntlet |
| `glossaryWorkbench` | 14918 | per-text glossary UI | Glossary button |
| `commentsDialog` | 15023 | team comments UI | Team comments button |
| `segmenterBuildMs` / `segmenterInfo` | 15156/15160 | perf + provenance reporting | gauntlet 39904 |
| `glossaryPath` / `loadGlossary` / `saveGlossaryEntry` / `glossaryStorePath` / `glossaryHtml` | 15188–15295 | glossary storage | card, workbench |
| `proposeFromCard` | 15205 | file a dictionary proposal | `propose:` |
| `ensureSegmenter` | 15340 | lazy botok + lexicon build (cancellable) | attestation/segmentation toggles |
| `segmentationHtml` | 15445 | segmentation block on the card | `onClick` |

---

## 4. Dependencies

**Data files / folders read** (relative to `dataRoot_`):
`data/botok` · `data/extracted/` (`meter_census.json`, `monlam_lexicon_1.txt`, `monlam_lexicon_2.txt`, `verb_stems.tsv`, `bsdus_tshig.tsv`, `kl_lhasa_concordance.json`, `thl_dege_concordance.json`, `mixed_nuts_notes.json`, `mixed_nuts_bibliography.json`) · `data/abbreviations/approved_abbreviations.tsv`, `tibschol_abbr.csv` · `editions/` (Derge eKangyur/Tengyur e-texts) · `library/` · `library/glossaries/*.tsv` (**written**) · `library/scans/`, `library/scan_cache` (**written**) · `library/ocr_models/BDRC_PhotiLines/PhotiLines.onnx` · `library/acip_release6/*` (selftest fixtures) · the allcore Spine (FTS5 db), `SyllableChecker`, the proposals/comment stores, and the AI-gloss store.

**Network hosts**: `iiifpres.bdrc.io`, `iiif.bdrc.io`, `purl.bdrc.io`, `library.bdrc.io` (scan manifests + images), `old.thlib.org` (Das/Jäschke page scans), `www.youtube.com` (teaching links, opened in the external browser). `example.com` appears in test/placeholder text only. All go through `TimedNam`.

**QSettings** (`ALL` / `TranslationTool`): `file/recents` · `overlay/{glosses, phonetics, corpus, sanskrit, hopkins, g84000, teachings, das, refs, notes, alignTent, grammar, segmentation, attestation, scriptMode, shading, tibFontFamily, inputSplit, lastFile, lastScroll, lastCursor, lastNestTok, lastNestCycle}` · `pecha/{preset, rules, layout, lines, title, titleFolio, marginTitle, headMarks, verseLines, coverSheet, shadConv, classicalOpening, interEng, interPhon, volLetter}` · `meters` · `share_pct` · `team/name` · `sess/dlg/last`.

**ctest suites relevant here**: `app_selftest` (the 135 checks), `gauntlet_walk`, `font_smoke`, `botok_smoke`, `botok_tok_smoke`, `engines_battery`, `proposals_smoke`, `tibexport_smoke`, `verse_smoke`, `quotation_smoke`, `outline_smoke`, `lexicon_smoke`, `spellcheck_smoke`, `abbr_smoke`, `verbstems_smoke`, `register_smoke`, `ocr_smoke`.

---

## 5. Release questions

- **The reference-dictionary layer is labelled "LOCAL ONLY … unlicensed compilations, never release data" in its own tooltip.** Does the shipped build carry LC/TD/THL/OT/IW data at all, and if so under what licence? This is a ship-blocker-shaped question, not a UI one.
- **The woodblock viewer window (`openWoodblockViewer`, ~775 lines, 13 controls, 6 shortcuts) has zero automated coverage.** Either it gets a walk-through in the selftest or it needs a manual pass before release.
- **`proposeai:` — the lane that promotes an AI-drafted gloss toward the binding authority — has no selfTest at all**, and its handler returns silently when the AI store is unloaded. Highest-consequence untested path in the pane.
- **Bulk network operations are unbounded and unmetered**: "Download all N folio scans", `galleryWholeVolume()`, and `fetchNextManifest()` will pull an entire BDRC volume. Confirm the confirm-dialog, the cancel path, and what a mid-download failure leaves in `library/scan_cache` (the stub handling at 9868–9919 suggests this bit people before).
- **Team comments and "File for ruling" both write to shared stores with no automated coverage.** Verify by hand that a failed write is surfaced, not swallowed (the Dossiers dialog does warn — these may not).
- **The Read ▸ Overlay submenu assertion only counts actions (`overlayActions > 8`, main.cpp:40194).** It certifies the menu is populated, not that any item works.
- **The align-stepper anchor handler is duplicated verbatim between OverlayPane (7360) and LookupPopup (2622).** Decide whether to unify before release or accept the drift risk.
- **`del:` on a glossary row appears to delete without confirmation** — verify.
- **Card layer defaults**: `glosses` is force-re-armed at launch by design; confirm no *other* persisted preference can hide a binding layer.
- **First use of the attestation hints builds a 548k-form lexicon** (measured at 4.8 s in Release, 35.6 s unoptimised). Confirm the shipped build is Release and that the cancel path leaves an honest "hints off, and here is why" state.
- **Half-built / heuristic surfaces to judge**: sa bcad outline detection, Text DNA, and the meter census all present machine inference without an explicit confidence label — decide whether each is ship, hide, or label.
