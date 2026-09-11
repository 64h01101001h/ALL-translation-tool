# Rolling log — notable actions since the last digest

Append-only, one line per action, newest at the bottom, written AS IT
HAPPENS (not reconstructed). Every code change, decision, ruling, email
sent, release step or open question goes here. The next daily digest is
drafted from this file; once the digest is committed the entries are
moved under a dated heading in `docs/digests/LOG_ARCHIVE.md` and this
file starts empty again. Format: `HH:MM · category · what · by whom`.
Categories: code · decision · email · release · audit · data · question.

## Since 2026-09-08 digest (drafted ~13:30)

- 13:40 · email · Daily digest 2026-09-08 sent by Adam to Geshe Michael, John Brady, Joel Crawford (md/txt/docx attached) · Adam
- 14:00 · decision · Digest text goes inline in the email body from now on; attachments kept as archive copies · Adam
- 14:15 · release · Fresh DMG pressed through the full gate (101/101), installed and relaunched; `tm_84000.db` (279 MB) pruned from the DMG, rebuilt on first use · Adam
- 14:30 · audit · Shared GUI vocabulary for the Overlay agreed from Adam's screenshot; written into OverlayPane.md · Adam
- 14:45 · decision · Steelman step added to the audit method; CUT added to the dispositions · Adam
- 15:00 · audit · Document box steelman given; Adam's ruling pending (SHIP as is vs FIX to collapsible)
- 15:10 · audit · "How would I start translating Thar lam gsal byed" ten-step walkthrough given verbally; queued as manual chapter + Draft/Review/Export test script
- 15:30 · code · Translator workflow questionnaire drafted (`docs/distribution/TRANSLATOR_QUESTIONNAIRE_2026-09.md`), 14 questions, reply-by 15 Sept
- 15:45 · email · Questionnaire drafted in Gmail to Geshe Michael + the Mixed Nuts team (Nick Lashaw's 5 May list); sent by Adam · Adam
- 16:00 · release · GitHub `main` replaced with the current tree (446 commits); old main kept as `github-main-before-20260908` · Adam authorised
- 16:20 · email · "Inside the release audit — the Overlay pane" sent by Adam to GMR/John/Joel with OverlayPane.md as .md/.txt/.docx · Adam
- 16:25 · decision · Assistant tooling is never mentioned in correspondence; attribution line removed from the audit file · Adam
- 16:40 · code · File → Save (⌘S) and Save As… (⇧⌘S) added, routed to the front pane: Overlay Document box (wylie-converted files force Save As; library files ask first), Draft English draft (new), Manuscript (existing). Self-tests added for each path. Requested by Adam
- 16:45 · decision · A rolling action log (this file) is kept between digests · Adam
- 17:43 · release · Press + install + relaunch requested by Adam to pick up File → Save/Save As · Adam
- 17:45 · release · CORRECTION: the press FAILED at the battery (100/101 Release-build suites); no DMG, app NOT relaunched, desktop copy still 14:15 build. Investigating the failing suite.
- 17:46 · release · Press failed on constitution R3 (modal count 145→146: the new library-overwrite confirmation, harness-guarded); baseline bumped after review; re-pressing
- 17:49 · decision · Companion iOS/Android 'Tibetan Reader' (reference-only, reading not writing) filed to TODO with build options A (Qt Quick) / B (native + C bridge) for Adam's decision after 1.0 · Adam
- 17:50 · release · Re-press: 101/101 gates, installed to /Applications (dev-d173da3, byte-identical, launch-tested) and relaunched by hand; the press's own DMG verify step failed afterwards — DMG not distributable until re-verified
- 17:52 · release · DMG dev-d173da3 verified VALID after ending an orphaned diskimages-helper (PID 38398, spawned by hdiutil create at 17:49:18) that held the file and made hdiutil return EAGAIN; the image was never corrupt. App relaunched at 17:50 on the new build (File → Save/Save As live)
- 18:08 · code · Word-parity File menu: New Document ⌘N, New from Template ⇧⌘P, Close ⌘W (Close Window → ⇧⌘W), Save as Template, Move, Rename (carries glossary/properties sidecars), Properties ⌥⌘P (General/Summary/Statistics/Content/Custom; Tibetan stats: folios, syllables, shads; sidecar JSON in library/properties). 14 selftest checks; constitution baseline 146→154 (8 guarded dialogs). Requested by Adam
- 18:08 · question · Adam asked for Word-parity Edit, View, Insert, Format, Tools, Window menus and a Word-style Preferences panel; to be built through the steelman (only what serves a Tibetan translator) — plan and batches to follow
- 18:10 · decision · Menu-parity plan written (docs/MENU_PARITY_PLAN.md): BUILD/ADAPT/SKIP per Word item across Edit, View, Insert, Format, Tools, Window + Preferences panel; batches 1–7, File done
- 18:17 · code · Edit menu at Word parity: Paste and Match Formatting ⌥⇧⌘V, Paste Special (as ACIP / Wylie / Tibetan script — converts, never guesses), Clear, Find ⌘F / Find Next ⌘G / Find Previous ⇧⌘G / Replace ⌥⌘F (in-document, wraps once, Replace All = one undo step). 6 selftest checks. Batch 1 of the menu plan
- 18:18 · decision · Sublime Text added as a second reference (search/replace, document window, colour schemes, settings); its File menu items folded into the plan as batch 1b/1c · Adam
- 18:19 · decision · Sublime 'Selection' menu folded into the plan (batch 1d): expand family BUILD/ADAPT (to Folio, to Phrase), multi-caret items SKIP · Adam
- 18:25 · code · File menu, Sublime additions: Reopen with Encoding / Save with Encoding (UTF-8/BOM/UTF-16/Windows-1252/MacRoman/Latin-1; never guesses), Save All ⌥⌘S, Print ⌘P, Revert File, Close All Files. 8 selftest checks; PrintSupport linked
- 18:26 · decision · Sublime Find/View/Goto/Tools/Project/Window/Help menus folded into the menu plan with verdicts; Project = dossiers promoted; Goto Anything = Hunt · Adam
- 18:33 · decision · Adam: order the TENTATIVE aligned equivalents on the card by attestation frequency, highest first, with counts shown (most-attested ≠ 'most probable' — label honestly) · Adam
- 18:37 · code · Find menu (Sublime): Find/Next/Previous, Incremental ⌘I, Replace/Replace Next, Quick Find ⌥⌘G, Quick Find All ⌃⌘G (highlight+count), Use Selection for Find ⌘E / Replace ⇧⌘E, Find in Files ⇧⌘F → Search pane, F3 family; bar toggles regex/case/word/wrap/in-selection/highlight/preserve-case; 13 selftest checks
- 18:37 · data · Evidence layer now emits each headword's renderings most-attested first (763 headwords were alphabetical in the file; the app and the dictionary view already sorted by count, so the card was right — the FILE now agrees). 16 alignment gates green. thams cad: all ×30, every ×14, each and every ×10, All ×5 …
- 18:49 · code · Selection menu (Sublime): expand to line/word/paragraph/folio/phrase/brackets/quotes/whitespace/indentation, until…, reverse; View additions: Text as, Sidebar, Markup, Line Numbers, Word Wrap, Line Endings, Focus, Ribbon, Status Bar, Full Screen, Spell Check F6 family, Show Console, Zoom submenu, status-bar Line/Column·script·encoding·ending. 18 selftest checks
- 19:01 · decision · Adam: a Textual Research workflow (new tab group) — user collections, word studies across an author's works, edition/variant studies across monastery prints, place/lineage queries over the catalogue; filed to TODO with a plan, to be fleshed out with Adam and implemented after 1.0 · Adam
- 19:04 · decision · Adam: interactive local map of monasteries/holy sites (seven reference pages supplied); filed with a sources note — build from BDRC/Treasury of Lives/OSM open data, not by copying the pages · Adam
- 19:04 · code · FIX: status-bar position hook crashed at app shutdown under ctest (QLabel::setText on a destroyed label) — QPointer guards; app_selftest green under offscreen again
- 19:05 · question · TODO: Adam to ask Geshe Michael for research-suite requests; eight assistant suggestions filed beside it
- 19:08 · code · Goto menu (Sublime): Anything=Hunt, Heading (sa bcad), Dictionary Entry, Concordance, Line, Folio, Jump Back/Forward (caret history), Scroll, Bookmarks (gutter dots, sidecar), Matching Bracket. 14 selftest checks
- 19:17 · digest · Drafted tomorrow's digest from this log as docs/digests/2026-09-09.draft.md at Adam's request (preview)
- 19:31 · code · Insert menu (Word): Break ▸ Folio Marker / Page Break, Tibetan & ACIP Symbols palette, Footnote → Draft, Comment → team comments, Date and Time with the Tibetan year, File…, Manuscript Hyperlink/Table/Pictures. Format menu (Word): Font, Paragraph, Bullets and Numbering, Change Case (English cases + Tibetan To ACIP/Wylie/script), Style → Manuscript house styles. Document… deferred (nothing reads page setup yet). 12 selftest checks
- 19:33 · decision · Adam: nontraditional cursors, a loading cursor, and Tibetan Buddhist iconography (eight auspicious symbols) in the UI; busy cursor built now (dharma wheel from font, else platform wait cursor), the rest filed to TODO for later · Adam
- 19:52 · code · Tools menu (Spelling, Phrase Memory, Translate hand-offs, Word Count, Protect Document, Templates, Snippets, Command Palette, Cheat Sheet), Project menu (dossiers: open/recent/new/save position/close/edit, Files workspaces), Window menu (Minimize, Zoom, pane-group cycling, Bring All to Front, live window list), Help ▸ Data Status…. 7 selftest checks; menu bar now 12 menus beside the tabs
- 19:55 · decision · Adam: add a compare-and-merge suite drawing on Beyond Compare, WinMerge, UltraCompare and Araxis Merge — research, plan, build thoroughly; then rerun and update tomorrow's digest draft · Adam
- 20:28 · decision · Adam: cover notes for digests/technical documents must highly recommend the .docx (properly formatted; displays far better than .md/.txt) · Adam
- 20:29 · decision · Adam: free to add tab groups/panes and regroup features logically as the suite grows · Adam
- 20:31 · code · Compare & Merge suite, batch 1: core engine allcore::textdiff (Myers linear-space line diff, minor/important hunks under ignore rules incl. Tibetan punctuation, folio markers, ACIP apparatus, script-agnostic via the canonical converters; moved blocks; syllable/word/char in-line diff; diff3 three-way merge with resolutions; unified patch, HTML side-by-side, apparatus criticus Markdown/CSV, summary). Battery textdiff_smoke: 43/43. Plan: docs/COMPARE_SUITE_PLAN.md
- 21:05 · code · Compare & Merge suite, batch 2: Compare pane (Research group) — side-by-side aligned editors with change bars, in-line syllable marks, link strip with per-difference ◀ ▶ copy buttons, location map, Line Details panel, Show All / Differences / + Context, minor-as-differences, rules (case, spacing, blank lines, shad/tsheg marks, folio markers, apparatus, script-agnostic, moves, line filters, substitutions), navigation F7/F8, copy left/right/all, unlimited undo/redo, edit-in-place with rescan (F5), Save Left/Right (own line endings), reports (HTML, patch, apparatus criticus MD/CSV, summary), sessions + recent, binary files honest. Tools ▸ Compare ▸ (Files, Folders, Three-Way, with Clipboard, with Saved Version, Left↔Right in Files, Recent); Files pane context menu; --compare CLI. 19 selftest checks
- 21:06 · decision · Adam: the ALL style sheet wants two spaces after sentence ends (colons/semicolons to be confirmed) — built as House style (English) in Settings: sentence/colon/semicolon rules, apply on save, apply while typing; Format ▸ Apply House Style Spacing on demand; English-only, abbreviations left alone · Adam
- 21:07 · decision · Adam: research the wider text-analysis tool families (diff, three-way, document compare, folder compare, version control, normalisation, find/replace, duplicate detection, similarity, statistics, NLP, change tracking, patches, structured data) and EndNote as an in-house reference bank model; decide what serves Tibetan translation · Adam
- 21:07 · question · Adam to confirm the style sheet's colon and semicolon spacing rules (built as separate switches, default off)
- 21:40 · code · Compare & Merge suite, batch 3: Folder Compare page (Identical/Different/Minor only/Left only/Right only; bytes, rules-based or size criteria; filters; Copy/Update with preview and confirmation, never deletes; snapshots with SHA-1; HTML/CSV report), Three-Way Merge page (auto-merge, conflict list, Take Left/Right/Both/Base, markers, Save Merged), Open Conflict File, alignment pins, --compare CLI. 17 more selftest checks; audit file docs/release_audit/ComparePane.md
- 21:55 · release · Full battery: 102/102 suites green after the compare suite (textdiff_smoke 43/43; app selftest adds 39 Compare checks)
- 22:05 · audit · docs/TEXT_ANALYSIS_SUITE_RESEARCH.md: 16 tool families from Adam's screenshots (diff, three-way, document compare, folder compare, version control, normalisation, find/replace in files, near-duplicates, similarity/TRACER, statistics/Voyant, NLP, change tracking, patches, structured data, side-by-side) and EndNote/Zotero as the model for an in-house reference bank — each feature HAVE/BUILT/BUILD/ADAPT/LATER/SKIP with Tibetan-translation use; three follow-on batches filed in TODO (Versions, Replace in Files, Apply Patch, Repeated Passages, Statistics study, Entities, Table compare, References pane + Insert Citation)
- 22:30 · code · Preferences panel (View ▸ Preferences… ⌘,): Word-style icon grid, 11 pages, search, Show All, Apply; House Style page holds the style-sheet switches (Adam: toggleable in preferences); Compare defaults; Key Bindings list from the live menu bar; Colour Scheme Day/Night; SettingsDialog removed. 10 selftest checks. Menu-parity plan complete
- 22:31 · question · Adam: please supply the ALL style sheet so its remaining rules can be added as switches
- 22:50 · decision · Adam: start developing the research write-up (analysis-tool families + EndNote) — design phase launched as a multi-agent workflow (subsystem maps → three design panels with judges → completeness critic) · Adam
- 22:52 · digest · Rendered tomorrow's digest draft to HTML and delivered it inside the app at Adam's request (the .md link opened in Safari)
- 22:55 · audit · Adam: the seven monastery/holy-site map pages had been filed but never read — research workflow launched (one reader per page, open place-data sources BDRC / Treasury of Lives / THL places / OSM / Wikidata, three-skeptic licence check, Map pane design → docs/MAPS_RESEARCH.md) · Adam
- 23:10 · decision · Adam (via Jane's reading): the digest's split into "what got done" vs "Adam's decisions" misrepresented authorship — everything reported was done under Adam's direction. Digests are now written in Adam's first-person voice with decisions inside the narrative; template and draft rewritten · Adam
- 23:15 · decision · Adam: the Map pane (own interactive, localised, offline map informed by the seven reference pages) is to be built after the analysis suite; research continues now, build batches M1–M3 filed in TODO · Adam
- 23:40 · decision · Adam: every digest carries screenshots of what changed and charts where they explain the numbers, delivered to him as .docx; captures must not be crowded or low-resolution · Adam
- 23:55 · code · Digest visuals pipeline: --screenshots driver gains DCT_SHOT_EXTRA captures (Compare pane's three pages populated with a two-print demo, Preferences grid + House Style + Compare pages, every top-level menu rendered open); tools/build_digest.sh (high-DPI offscreen screen 2880x1800 = 1440x900 at ratio 2, keeps only referenced images, md → txt/docx/html with embedded images); tools/digest_chart.py (before/after bars). Preferences tiles renamed Locations / Colours / Shortcuts so no label truncates
- 23:58 · digest · 2026-09-09 draft rebuilt in Adam's voice with 8 figures; delivered as .docx and HTML in-app
- 00:05 · release · Press script: hdiutil verify now retries while the image is busy (EAGAIN) and reports busy vs corrupt distinctly (exit 10 vs 9) — the press3 defect · fix
- 00:20 · code · Press STATIC-R1 gate refused 17 compiler warnings: selftest fixtures wrote files without checking the write (the vacuous-fixture rule), plus an unused Find overload, two unused lambda captures and a dead helper. Fixed with checked writeFixture/readFixture helpers and dead-code removal; build now warning-free, 102/102 suites green
- 00:35 · release · Press refused again at the battery: repo_licensing caught two stray artefacts I had committed (a press transcript and a pandoc output file named "-"). Untracked, ignored, gate green; press re-run
- 00:45 · decision · Adam: schedule a working session with Ven. Utpala, the Diamond Cutter Classics editor, to learn her workflow from the ground up and put every available function at her disposal · Adam
- 00:50 · release · Editor's-tool session prepared: docs/distribution/EDITOR_TOOL_INTERVIEW_UTPALA.md (her workflow question set, the inventory of what the tool already offers an editor, ten candidates for her verdict); hold on the calendar Fri 11 Sep 15:30–17:00; invitation waiting in Gmail drafts for Adam to send (nothing sent)
- 01:05 · code · FIX (found by the press's visual gate): the screenshot sweep restored the window geometry saved by the last desktop session, so the blessed baseline depended on how large the window happened to be — the sweep had fallen from 1180x664 to 796x700 and fired the gate on all 22 panes for no code reason. Harness runs now neither restore nor save the window geometry. Compare pane's pages also wrapped in scroll areas so a wide page cannot force the window wider
- 01:08 · release · Visual baseline re-blessed at 24 panes: the only intentional change is the new Compare pane at position 16, which renumbers the panes after it
- 01:15 · release · Press GREEN: gates, batteries (102/102), constitution, visual regression, staging, Qt bundling, licences, SBOM, signing, arch/floor, data manifest, payload gate, DMG verified. Installed to /Applications and relaunched — Diamond-Cutter-Translation-Tool-1.0.0-rc.1-dev-8ff7465.dmg (405 MB), built from 8ff7465
- 01:30 · decision · Adam: sweep everything not yet filed onto the backlog. TODO.md now opens with an UNFILED SWEEP section: the three people-sessions, the ten editor's-tool candidates, twelve release-audit open items, four unfinished work streams, three standing process rules, and four defects fixed today · Adam
- 01:32 · decision · Adam: one-on-one sessions with each Mixed Nuts translator to learn their day-to-day workflow personally; roster taken from Nick Lashaw's Mixed Nuts thread cross-checked with the DWTC class list (9 translators besides Utpala) · Adam
- 01:50 · decision · Adam (second time): the digests must be highly readable — spacing between pictures, between captions and the next section, and around headings. Root cause found and fixed: pandoc styles list content as "Compact" (most of a digest), so the earlier Normal-only fix changed nothing; and 2x captures carried 96-dpi metadata, so a small menu image filled a whole page. Word styles retuned, tools/digest_image_dpi.py added, values tabled in docs/digests/README.md as a standing order · Adam
- 02:10 · audit · docs/ANALYSIS_SUITE_PLAN.md written from the design work: 9 subsystem maps, 3 judged batch specifications (28 agents: 9 readers, 9 designers, 6 judges, 3 synthesisers, 1 critic) and the critic's binding reconciliation — it caught the folio primitive being designed twice, six CSV writers where one belongs, three provenance idioms, three widenings of the constitution census, and three dead controls; 19-step build order across batches 4-6
- 02:15 · audit · docs/distribution/TRANSLATOR_ONE_ON_ONE_GUIDE.md written from three translator perspectives (newer, veteran, paper-first sceptic): session shape, per-person preparation, the question set in the order of a translator's day, what to watch for, what to show and when, mistakes to avoid. Nine personal invitations drafted; nine 45-minute slots held
- 02:40 · decision · Adam: find the Mixed Nuts / Diamond Cutter Classics style sheet and integrate all of it; then loop through the analysis-suite build · Adam
- 02:55 · audit · Style sheet FOUND — it was already in the repository (docs/standards/DCC_Style_Guide_2023-08-25.docx, banked 2026-08-14 with a Help distillation and a Draft "House style check"); mail, Dropbox and disk search also recovered the lineage: Geshe Michael's Style Sheet for Classics of Middle Asia (Feb–Apr 2017), Utpala's Mixed Nuts Style Guide (Nov 2021), the DCC Style Guide (Aug/Sep 2022, Jan 2023, Aug 2023). Inventory of every rule with status: docs/standards/DCC_STYLE_GUIDE_RULES.md
- 03:10 · code · Style sheet integration: the guide says ONE space after colon/comma/semicolon (two after a sentence) — the "two spaces after a colon" switch built yesterday contradicted it and is replaced; the Draft checker gained the guide's remaining mechanical rules (sentence spacing, punctuation spacing, indentation characters, spaced en dash, quote-punctuation placement, Independent/Consequence branch, Enlightened Being); every check is a switch on Preferences ▸ House Style; Tools ▸ House Style Check…; Help ▸ Style points at both. 2 selftest checks
- 03:40 · code · Analysis suite, batch 4 F0 (the substrate): allcore/textspan (the ONE folio-marker definition, apparatus spans with unbalanced-bracket refusal), allcore/filewalk (the ONE eligible-text-file rule + glob walk with counted caps) now used by Search and the library index, textdiff::applySelected; the app's four private folio regexes replaced by the shared one; the Draft's data root was never assigned so its properties sidecar never wrote — fixed and pinned; Compare with Saved Version flattens Manuscript HTML; constitution R3 census now covers app/*.inc (planted-modal regression check passes). Batteries: filewalk_smoke 12/12, textdiff_smoke +17, app selftest +4; 103/103 suites
- 03:50 · audit · Style-sheet sweep workflow (11 agents: repo, disk by name, disk by content, Gmail, Drive; then extraction) confirms the finding and adds one open thread: Geshe Michael's 2026-08-15 ruling on removing semicolons / adding spaces vs the guide's "commas and semi-colons: mainly preserve as written" — to ask Utpala
- 02:45 · code · Analysis suite, batch 4 F1 (Versions): every Save of a document, draft or manuscript now keeps a compressed copy with who/when/encoding/revision/statistics under library/versions; File ▸ Versions… (⌥⌘V), Tools ▸ Compare ▸ Compare with Version…, Properties ▸ Versions kept; the window compares a version with the editing text or two versions, restores with the on-disk text banked FIRST (even an outside edit), names and pins, answers Line Origins with the diff engine, shows damaged records red and excluded; Preferences ▸ Versions (on/off, caps, autosave slot); rename carries the history folder. Store is Qt-free allcore::versions (in-house SHA-1, strict record parser). Batteries: versions_smoke 32/32, app selftest +15, textdiff lineOrigins +4; 104/104 suites
- 02:54 · code · Analysis suite, batch 4 F2 (changed-folios report): "7 of 143 folios changed" — the apparatus grouped by the left text's folio markers, as Markdown (table + per-folio readings + unchanged count) or CSV; Save Report filters, Sessions ▾ ▸ Copy Changed Folios, a status-line fragment, and the --compare CLI by double suffix; the save dialog now reports the selected filter so two Markdown kinds never collide. Header states every grouping rule (pseudo-folio, span counted where it starts, minor rule, raw lines, cites from the left). textdiff_smoke +8, app selftest +8
- 03:35 · code · Analysis suite, batch 4 F3 (Normalize… with preview): the compare rules as transformations — collapse spacing, trailing blanks, blank-line runs, double-shad spelling (no default: awaiting the house ruling), spacing around the shad (ACIP/Unicode lines only, tsheg untouched), apparatus spans and folio markers (red, 'removes text'), line endings, final newline, house-style spacing — previewed as a diff in Compare, refused lines listed with reasons, Apply enabled only for the previewed settings and text. Format ▸ Normalize Text… (Manuscript refused), Compare Sessions ▾ ▸ Normalize Left/Right, Files ▸ Normalize… (strict UTF-8, BOM kept, pre-normalize version banked first). Qt-free allcore::textnorm; shared text_util.h. textnorm_smoke 44/44, app selftest +14
- 03:35 · code · Analysis suite, batch 4 F4 (Replace in Files… with mandatory preview): every occurrence per file with its line before and after, tick/untick, two-step apply only to ticked files, honest counts (binary / not UTF-8 / over 10 MB / past the cap / open with unsaved edits / changed since the preview / library text — each named), a script-mismatch notice so a zero never reads as clean, a changeset record and an undo that restores from the pre-replace versions and skips edited files. Find ▸ Replace in Files… (⇧⌥⌘F), Undo Last Replace in Files…, Files ▸ Replace in these files…, Search Results ▸ Replace in these folders… (one plain term only). Normalize mode runs F3's rules across a folder. allcore::versions gained changeset records. versions_smoke 42/42, app selftest +24; 105/105 suites
- 04:03 · code · Analysis suite, batch 4 F5 (Apply Patch…): a corrector's unified diff against the front document — each hunk placed exactly, with offset, with loosened context, already applied, or rejected (five verdicts, never folded); preview as a diff in Compare with the partial count in the name; rejected hunks to a .rej verbatim; pre-patch version banked before any write; Manuscript refused; Compare Sessions ▾ ▸ Apply Patch to Left/Right; --apply-patch CLI with dry-run and honest exit codes. Qt-free allcore::textpatch (patch(1) behaviour; context copied from the target so loose whitespace never reformats a file). textpatch_smoke 26/26, app selftest +13
- 04:03 · code · Analysis suite, batch 4 F6 (Word redline): the differences between two drafts as a .docx with real Word revision marks — a STORED zip writer with CRC-32 (no zlib) and a WordprocessingML writer (inline spans by syllable/word for 1:1 changes, whole paragraphs otherwise, paragraph-mark revisions, Tibetan complex-script font slot, minor differences always included and counted, an untracked first paragraph stating what was compared, under what rules, by whom, and that the marks are machine-computed; author '<name> via Diamond Cutter compare'). CLI first (--compare A B out.docx); the menu filter waits for Adam's Word check on docs/release_audit/redline_sample.docx. The research premise that a .docx writer already existed was false and is corrected. zipstore_smoke 5/5, docx_redline_smoke 14/14 (textutil reads it back), unzip -t clean; 108/108 suites
- 04:11 · press · Batch 4 pressed, signed, verified and relaunched on the desktop: dist/Diamond-Cutter-Translation-Tool-1.0.0-rc.1-dev-ca74251.dmg (release gate full battery, constitution holds, 23 blessed panes match, payload gate clean). The two capture-only column-width fixes made after the press (Versions table, Apply Patch table) ride the next press
- 04:11 · digest · Draft for today rebuilt with the four new captures (Versions window, Normalize dialog, Replace in Files, Apply Patch — each checked at 2x and corrected until every column and count read whole), the batch-4 section in first person, five new decisions, three new questions for leadership (double-shad ruling, bulk edits of library texts, the Word check), the numbers table and chart from the live batteries (108 suites; 214 core battery checks; 670 app self-test checks)
- 04:18 · audit · Release-audit sweep: eleven of the twelve open MainWindow items closed — the open-file probe joins the harness guard; the dead minimum-size call dropped; the Sanskrit glyph check registered as a test gate (109 suites); an orphaned Quick Access pin is shown as 'not in this build' instead of vanishing; the lifecycle log rotates at 1 MB; Storage… clears need a second, sized click; the Edit menu shows the standard shortcuts Help advertises; the menu mirror skips destructive controls; the reduced-battery escape hatch, the Import Data Release hook and the shipped data version (0.27.2 in the spine meta) verified on the installed build. Open: the two diagnostic reports' privacy profile (a ruling)
- 04:21 · press · Final press of the batch (dev-c6306d7) with the audit sweep and the capture column fixes: full battery (109 suites), constitution, visual gate, payload gate; installed and relaunched on the desktop. Digest draft rebuilt (batch-4 section, audit sweep, press, numbers and chart from the live batteries)
- 06:17 · gate · Batch 5 step 2: the binding-gloss census (G2) now reads app/main.cpp AND every app/*.inc, as the modal census already does; baseline stays 33 (no pane file touches hgm_gloss today). Mutation check: a planted hgm_gloss in versions_pane.inc fails the gate (33 → 34), reverted, gate holds. Adam's ruling on moving this gate is requested in the plan's open questions (Q8)
- 06:24 · decision · Maps research paused at Adam's request (token allowance; resume next week). Nothing written; the six reference links stay filed in TODO
- 06:58 · code · Idiom bank and register (Adam's request): the register that was started as data in August is now read, matched and proposable. allcore::idioms matches forms over syllables (longest match, whole-idiom, no substring accidents; a form ruled not an idiom is never marked again). Two files: the shipped seed stays read-only, the team register takes the rulings and overrides it. Draft ▸ Idioms and Tools ▸ Idioms… open one bank: filter by status, see occurrences in the open text, read who put a form forward and why, and see Geshe Michael's own renderings from his courses with their counts. Propose an idiom… files a proposal of kind idiom for his ruling with the passage as evidence; Update from rulings brings his approvals and declines into the team register with his name and date. The register holds no English of its own — that stays his. idioms_smoke 17/17, app selftest +11
- 06:58 · code · Batch 5 substrate and Table Compare landed together: allcore/textspan gained the promoted syllable tokenizer, script routing, folio walk and the apparatus policy (the existing compare and quotation batteries stayed green as the proof), allcore/provenance carries the report header, allcore/table lifted the quoted-field splitter out of the catalogue register, allcore/tablediff joins two tables by a key the user chooses and refuses to pair a duplicated key. The Study pane shell and the Compare pane's fourth page (Table Compare) are wired. 113/113 suites
- 06:58 · gate · The binding-gloss census caught the new Table Compare page: comparing two release files can put his English on screen. Reviewed — the cells are the files' own, the tier column is pinned so a tier can never be dropped, and a banner now says so. Baseline bumped 33 → 36 with that reasoning, as the gate requires
- 06:58 · finding · The promoted tokenizer turns a folio marker into a false syllable (@012a reads as the syllable 012a), so markers would count as word types in study frequencies. Left as found rather than silently changed: fixing it moves quotation-detection recall and needs its own unit and a re-baselined battery
- 07:12 · code · Preferences ▸ Features (Adam's request): every pane and every group can be turned off so the window carries only what a person uses. The list is built from the panes that exist at run time, so it cannot drift from the build; turning a group off hides its tab and its panes; a pane that was on screen when it was hidden hands over to the next one; turning everything off is refused, because a window with nothing in it is a fault, not a preference. Hiding is a view: nothing is deleted, and a hidden feature stays reachable from the menus so it can still be used occasionally. app selftest +5
- 07:12 · decision · The idiom bank was NOT given a ribbon button: the Draft's ribbon would have grown 80 px past the width ratchet. It sits on the source's right-click menu (where the Tibetan is) and on Tools ▸ Idioms… (from any pane). The ratchet is a promise, so the feature moved rather than the number
- 08:42 · digest · Today's digest promoted from draft to final and rebuilt: new sections for the idiom register and bank, the Features page, and the second analysis batch (the shared foundation, the Study pane shell, Table Compare) with fresh captures of all four; headline rewritten to cover the whole span since Monday; numbers and chart taken from the live batteries (113 suites, 313 engine checks, 712 app checks); 16 figures, 3,448 words. Delivered as .docx for Adam to send
- 09:17 · digest · Adam asked for the two-print comparison to lead the digest. Confirmed from the capture: the Sera Mey side reads THAMS CAD on line four, the Ganden side reads KUN, both marked at the word, with one line only in each print. Written up as the opening section with the use case he named — textual study across monasteries, regions and collections, and the input centres' twice-typed files — and with the honest note that the capture is a demonstration and the real editions have not been collated. Digest rebuilt (3,940 words, 16 figures) and the Gmail draft updated to lead with it
- 09:18 · backlog · Adam (back burner): ask on first run how the person will mainly use the tool — translating, cataloguing, and so on — and let the answer set which panes and tools start visible. Filed with its design notes: it writes the same switches the Features page owns so there is one mechanism, it waits until the editor and translator sessions tell us what each role actually needs, and it is a starting point the person can change, never a lock
- 09:36 · digest · Adam: put the digest itself in the body of the email rather than a description of it, and recommend the .docx strongly. Done — the draft now carries the whole digest as formatted HTML, with each figure's caption in place and a note that the picture is in the attachment. Reading it through before sending caught five statements the day had overtaken (Preferences pages, Apply Patch and the Word engine still called in-progress, a stale suite count, and a Next that promised twelve open audit items when eleven are closed); all corrected in the digest itself. Standing order recorded in docs/digests/README.md
- 09:59 · research · The paused maps run reported its partial results: six of the seven reference pages read and verified before it stopped. One finding matters for planning — the Columbia/Academia Sinica monastery map (2,777 monasteries from BDRC, sect-coloured, with a founding timeline) no longer works at all: its maps lived on a CARTO account that has been deleted, so every embed 404s, and the page states no licence, only a copyright line. Banked in TODO with the path to the notes; the licence checks and the synthesis did not run and are next week's work
- 10:03 · backlog · Adam (back burner): meet the editors beyond Utpala — Anatole and any others — and separately the cataloguers, both about their own workflow and where the day could be made shorter. Filed with the practical notes: the editor question set already exists and can be reused, the standing Thursday cataloging meeting is the natural venue rather than a new invitation, and both sessions are what will correct the Editing and Cataloguing workflow presets that shipped today as coarse guesses. No contact address for Anatole in the mail I can see, so Adam supplies it
- 10:32 · press · Pressed, signed, verified, installed and relaunched: dev-91869e0, carrying the idiom register and bank, Preferences ▸ Features with the workflow presets, and the batch-5 substrate with the Study pane and Table Compare. The first attempt was stopped by the visual gate: adding Study shifted every pane index after it, so eight panes read as missing and nine as new. Checked rather than assumed — every shifted pane is byte-identical to its old baseline, and the seven that genuinely differ do so for named reasons (the Research pane bar gained a button, Compare gained a page, Search gained a button, and four panes list live data). Baseline re-blessed at 25 panes and the press ran clean. The load guard was overridden on evidence: load 45 but the CPU 55 percent idle, the load being blocked I/O from Xcode's git service, not contention
- 10:37 · design · Adam's directive: a variety of cohesive colour themes for the whole interface, settable and customisable in Preferences, with the Overlay's phrase shading and every card-layer component working under any theme. His reference (the Figma colour-combination library) read and banked at docs/THEME_REFERENCE_FIGMA.md with what it gives (six harmony schemes, light grounds and dark ink) and what it does not (no hex values, no contrast figures, no proportion rule — so those numbers are ours). Design workflow running: a colour-role census, the readability arithmetic for washes on any paper, three independent designs judged on honesty, readability, implementability and coverage, then one plan
- 10:50 · fix · "Show these 106 in the sortable list" was slow and then showed the wrong texts. Cause found, not guessed: the person view called the whole-library fill, building a row for every one of the 7,586 installed texts, and then removed rows one at a time until the person's remained. Each removal shifts the rows after it and repaints, so the cost is quadratic, and the screenshot Adam sent is that unfiltered table caught mid-removal. It now builds the person's rows straight from the file index — no library walk at all, roughly a hundred rows instead of seven and a half thousand, and none created only to be destroyed. The panel says what it is showing, and says plainly when a person's works are not held here so an empty list never reads as a failure
- 10:50 · fix · The dharma-wheel cursor existed and was never seen: it was built but the operations that make a person wait never set it. There is now one guard, BusyWheel, beside the wheel itself, worn by the library fill and the person view, with a selftest pinning that it restores the cursor it took and is a no-op under the harness
- 10:51 · backlog · Adam: invite the keen beginners from the weekly Mixed Nuts lectures — he named Oscar Leon (surname to confirm) and Duo Duo — for a session on how the tool meets someone new. Filed as its own cohort with the reason: a beginner who uses it heavily is the only person who can report the first hour, which the experts have long since routed around. It is also the honest test of the workflow presets that shipped today, the Learn group and Help
- 11:20 · fix · Adam hovered the Library button and got nothing: "there is no label under the icons in dark mode... we will need the name of the tool under the icon", and "a description of each component displayed when the cursor is over any button or icon for more than a second". The pane switcher buttons carried no tooltip at all, and twenty-six ribbon buttons carried only their own name. Both now answer in one shape — the name in bold, then what the thing is for — with fourteen of the pane descriptions lifted verbatim from the user manual so tooltip and manual cannot drift. A latent trap fixed with it: the ribbon proxy copied its source's tooltip once, at construction, so a description written after the button joined the ribbon was silently dropped; it follows the source now. Two selftests hold the line at 24 panes and 121 buttons, none silent
- 11:30 · fix · Adam had Day selected in Preferences and dark chrome on his screen. Both were behaving as written: the old switch only ever SET a colour scheme when night was on, so with it off the app left the scheme "follow the system" and his Mac overrode a preference that plainly said Day. Appearance is three-state now — Day, Night, Match system appearance — applied unconditionally at startup, offered in Preferences and under View, and migrated once from the old switch. Match system is the answer to his other question, whether night can come on automatically: it follows the Mac, sunset switch included
- 11:32 · fix · The missing ribbon labels were never missing. An application stylesheet makes Qt bake each widget's palette when it polishes; switching appearance changed the palette but repolished nothing, so the labels kept light-mode ink on a dark band. The repolish watcher had been written days ago and never actually installed
- 11:45 · fix · Adam, on the Draft workspace: "i don't see a Load source button or option anywhere on THIS screen" and "what use are the structure and evidence labels here? i seriously don't understand how to use this." Both fair. Every verb had migrated to the ribbon, leaving three gold headings introducing nothing. The headings are cut and the pane carries its own way in: a Load source button pinned under the source box, outside the scroll area so it cannot sit below the fold, beside a line saying where the rest live. Its two empty-state placeholders now name the route instead of citing a button the reader cannot find
- 11:50 · press · Pressed, installed and relaunched: dev-20b33bb. The DMG verify had been exhausting all six retries with "image busy" — not a momentary helper, but four attachments left behind by earlier presses, one of them the image under verification. Attached-but-unmounted images never appear in Finder, so they piled up until a verify blocked. Verified by hand after detaching: checksum valid, the image was always sound. The press now releases its own leftovers first
- 12:00 · fix · The night-mode shading itself. Twenty-five stylesheets across the eight pane files had a manuscript ink baked into the string, which cannot follow an appearance change because there is nothing to follow — that is exactly Adam's "some panels are dark and others are half shaded". They build from tokens now: Day unchanged to the pixel, Night legible for the first time. The Compare table's five row washes were the worst, measured on cream but painted behind a tree row on chrome and read by near-white text at about 1.2 to 1; they keep their five meanings in a depth the chrome can carry. Two findings fell out: a damaged autosave in the Versions list was painted warning red and then quiet grey a line later, so the row that most needed to stand out was the only one that lost its colour; and the machine-derived ink clears AA on paper at 4.62 but only 4.22 on light chrome, so its chrome sibling is a shade darker. Three new pins hold it, including a constitution rule that fails on any hardcoded colour in a pane file
- 12:05 · decision · Adam, standing order for every digest from now on: section headers and comparable elements centred, and every screenshot centred with its caption, in the Word file and the HTML alike. Body prose stays left-aligned, because a centred paragraph is harder to read, not easier. Implemented in the Word reference styles and the HTML stylesheet, and today's digest rebuilt to prove it. He also asked to be reminded, every time and not once, to attach the .docx before sending
- 12:10 · reference · Nagarjuna's Root Verses on the Middle Way, both dedicatory verses and 1.1-1.2 and 24.18-24.19, generated through the canonical Sanskrit engine in IAST, ACIP Sanskrit input code, Devanagari and Tibetanized ACIP, for Adam to test the converter with. One finding already visible in the output: the Devanagari puts a virama only on the last word of a line, so a word-final consonant mid-line comes out bare
- 12:15 · design · The theme-system proposal landed and is banked at docs/THEME_SYSTEM_PLAN.md, awaiting Adam's rulings before anything is built. Eight papers computed from seeds by a solver that must clear twenty-two measured gates, with the epistemic hues frozen in identity across every paper — so a theme can say what surface you are reading on and is structurally unable to say whose English it is
- 12:30 · fix · Adam pasted a Nagarjuna verse into the Sanskrit workbench and four of the six rows showed a bare warning triangle with nothing beside them. Two defects. The tokenizer knew only the ASCII five as whitespace, so a non-breaking space — which is what a word processor or a web page puts on the clipboard — made it refuse the whole passage, as did a newline; it reads every kind of whitespace now, in the Python oracle and its C++ port together. And the refusal itself, though correct, would not say what it could not read; it now names the character, its position and its code point, says plainly that nothing is guessed, and says what to do next. A third defect surfaced while testing: Devanagari put a virama on the last word of a line and no other, so every word ending in a consonant mid-line printed bare with an implied vowel that is not in the Sanskrit. The only Devanagari case in the battery was the mani mantra, whose every word ends in a vowel, which is exactly why it survived; both batteries now carry a word-final consonant
- 12:35 · feature · The Geshe Michael Roach Dictionary now exports as CSV every time it is rebuilt, from the same assembled entries the published page renders so the two cannot drift. Four files: the full export at 20,933 rows over 10,758 headwords for courses one through five, each row carrying its own provenance so a spreadsheet separated from its covering note still says what tier it is; an English-to-Tibetan reverse index, which answers the question an editor actually asks and the page cannot; a per-course glossary ranked by frequency; and a change report against the previous export, because nobody re-reads six thousand headwords but everyone will read the delta. Two findings came out of building it: the ACIP column has been empty for every entry of the published page, because the index is keyed ACIP-to-wylie and was read the other way round; and the layer holds about fifteen headwords that are English front matter rather than Tibetan, harmless as text and fabrication the moment a script column is computed from them, so the computed columns are named tibetan_generated and pronunciation_generated and an in_acip_index column says whether the layer holds that headword at all. Draft to Joel prepared with the zip on Adam's Desktop
- 12:45 · decision · Adam's standing rule: never the naked name for Geshe Michael Roach. His title goes with his name everywhere — prose, interface strings, email, document titles and file names, the same courtesy owed a doctor. The dictionary is the Geshe Michael Roach Dictionary; its four CSVs, the web page, the page's own title and every reference in the tools and the specs were renamed, and the rule is recorded so it is not dropped again
- 12:55 · feature · The Sanskrit workbench takes a whole passage now, at Adam's direction, with From and To dropdowns and a Copy result button. Every notation still gives the full single-term analysis with Whitney's roots and the Mahavyutpatti bridge; picking one notation converts line by line. Rules, headings and bracketed labels are carried through untouched on four objective tests and no language guessing, with Adam's own test document as the selftest and a control asserting the heading is not itself run through the converter. A refused line is left as it was and named by line number with the character that stopped it. Reading a passage back from Tibetanized ACIP says plainly that the notation separates syllables rather than words, so word boundaries cannot be recovered — a limit of the notation, stated rather than hidden behind a run-on result
- 15:40 · feature · The ALL Sanskrit pronunciation standard, at Adam's direction: a third pronunciation line beside the classical IPA and the plain-letter strip, neither of which is touched. Three layers, later winning — the FPMT letter values as the base, readings MATCHED from Geshe Michael Roach's own published courses over them, and Adam's amendments on top. His two founding rules: hūṁ reads "hung", svāhā reads "soha". The evidence was already in the corpus and nobody had used it: 821 lines print a mantra as ACIP code in braces followed by his own reading of it, 681 align word for word, giving 346 distinct pairs over 5,875 weighted occurrences, now banked and used directly as the acceptance battery. Measured over that set: ś reads sh 309/309, retroflex marks drop 503/503, long vowels unmarked 2,900/2,900, ṛ reads ir 65/65, cch reads ch 205/205, visarga reads h 572/644. The standard agrees with his printed readings on 94.4% of those words; battery I holds the figure with the floor at 94% and ratchet-upward-only, and the Python oracle and C++ port agree exactly over all 79,452 Lokesh Chandra terms
- 15:45 · finding · Two things Adam needs to decide, both measured rather than asserted, and both stated in the document, in the code beside the rule, and in the workbench itself. His rule 1 is his own overwhelming practice: "hung" in 1,073 segments against "hum" in 37. His rule 2 is not: "soha" appears in NONE of his 42,199 published segments, where it is written "sva ha" 380 times. On his 662 cleanly aligned mantra lines that single amendment is the sole difference on 301 of them — without it whole-line agreement would be 73.6%, with it 28.1%. The ruling is implemented as given and labelled interim, awaiting Geshe Michael Roach's own word, following the le'ur precedent in the trust hierarchy. Also checked and deliberately not adopted: "benza" for vajra, the best-known marker of the chanting tradition, appears 0 times against vajra's 513
- 15:50 · fix · Two real gaps in the converter fell out of the work. His courses write the visarga as ":" (644 occurrences) and ś as "s*" or "s#" (327), and the reverse table read neither, refusing a fifth of his mantra corpus outright. Both engines read them now; that change alone took agreement from 80.1% to 92.6%. And the Tara mantra exposed a third: in Tibetan script svāhā is written as two syllables with a tsheg between them, so it arrives as two words and a whole-word rule never fires on it — the commonest written form of the very syllable rule 2 names. The standard matches phrases across word boundaries before anything else now
- 20:10 · fix · Adam sent seven screenshots of the Drills pane. Three faults, all measured rather than guessed. The scale mismatch he named first: makeCloze picks ONE CLAUSE but the prompt prints Geshe Michael Roach's English for the WHOLE SEGMENT — measured over 300 real drills, the clause shown averaged 27.5% of the segment whose English sat above it, so a forty-word English paragraph appeared over three Tibetan syllables. The Tibetan is now the whole segment too, with the answer blanked where it actually stands, located from the drilled clause's own position so a phrase repeating earlier cannot capture the blank. Second, the raw ACIP he saw among Tibetan options — "(RNAMS", "(GNAS BRTAN)", "(ZHES": disp() fed each token whole to the script converter and on failure printed the RAW ACIP inline with a tsheg after it as though it were script. Editorial markup is separated out and shown as what it is; a token that will not convert is flagged in angle brackets. It hit 3.3% of drills. Third, at Adam's request, a title line now says it is one — the signal is structural, not a guess about prose: 4,010 of 42,199 segments (9.5%) are title lines in their own courses, and all 796 with an "_AUTHOR" suffix are among them
- 20:50 · finding · The adversarial audit found why he kept drawing title lines, and it was not chance. randomSegment's adaptive branch scores candidates by vocabulary coverage with a penalty for length, so when coverage is flat the shortest of five wins — and the title catalogue is the shortest material in the corpus, 10 to 15 ACIP words against a corpus mean near 36. Measured: 20.8 to 25.0% of adaptive draws were title lines and TITLS was the single most-drawn course of all seventy-five. He has "adapt to my level" ticked, so roughly a quarter of his drills were catalogue entries. A second class was reaching him too: 254 segments (0.60%) hold English prose where the Tibetan should be, and thirteen of the seventy-five courses OPEN on one — handed to the converter it renders as plausible-looking nonsense, the same root cause as the fake Tibetan in this morning's dictionary export. Both refused at the draw itself, so one gate covers every mode. Measured after: zero of 400 on each path, pinned in the battery
- 21:00 · feature · Adam asked why his deck holds only 266 words. Those two numbers are his own activity counters, not the size of the bank: 266 is words he has clicked in, and the drill count is drills he has done — the drills already draw from the whole corpus. But he is right that a deck should not be filled a word at a time, so it can now be filled from the Geshe Michael Roach Dictionary layer itself, his own English on his own headwords, added TENTATIVE and tier-labelled
- 21:05 · backlog · Adam: take Drills and Trainer into a standalone iOS app once the desktop tool is debugged, and push it up in priority when that lands. Filed with what already helps — the drill logic is Qt-free C++ in allcore and compiles for iOS as-is — and what does not: the spine is 100 MB and wants a slice, progress is local and would need to sync, and the tier labels travel with it
- 21:30 · feature · The iOS app, path A: a SwiftUI Tibetan Translation Trainer with both modes, running in the simulator. 4,000 drills and 1,000 trainer passages, all generated on the Mac by the same allcore the desktop uses, so the phone generates nothing and inherits every refusal. Trainer carries the full six-layer reveal — chunks, particle roles, reading order, the verb that frames the clause, Geshe Michael Roach's English, full parse. The whole core compiles for a real iPhone unchanged: 77 of 77 files, zero edits, which was the one real risk in the estimate
- 21:50 · fix · Seeing it on a phone exposed a fault the desktop had hidden: one segment pairs 5,180 English words with five ACIP words. Measured over all 42,199 segments the median is 1.43 English words per ACIP word and p95 is 3.11, so a 6:1 ceiling refuses 774 segments (1.8%). Longest English in the pack fell from 8,133 words to 277
- 22:10 · fix · Adam: "doesn't having the parenthesis at the front of the first option kinda give away that that is the answer since there is a closing parenthesis in the example". It did. An option opening "(" against a segment closing ")" told the learner the answer before they read a word of Tibetan. Editorial markup is stripped from both sides now — zero options begin with a bracket
- 22:20 · finding · Adam was right that the English often looked too long to be the equivalent, and the cause was precise. His C15 segment measures 4.3 English words per ACIP word counting the [bracketed] material and 1.4 without — 1.4 being the corpus median exactly. Bracketed English is translator-supplied: debate restatements of the previous move, glosses, page numbers, with no Tibetan in that segment at all. 3,045 segments (7.2%) carry some. It is now greyed and labelled as supplied rather than passed off as the parallel, and the proportion guard measures only the real parallel so good segments are not wrongly refused
- 23:10 · fix · Adam asked why g-yas, g-yon and dvags were not rendering. The proper engine fix was written first and held its own battery exactly — 99.03% over 26,318 pairs, 256 mismatches, unchanged — but dropped the spine battery from 99.5% to 91.9%, because the wylie column banked in the spine was generated by the old engine and 6.6% of segments now disagreed with it. That is a coordinated change with the data project, not a late-evening one. So the correction went into the display path instead: the reader sees རི་དྭགས་ལ, nothing banked is contradicted, the spine battery is back to 99.635%, and the engine fix stays queued. One transform was dropped outright — EWTS is case-sensitive, H is visarga and h is ha, so handling ':' there rendered A'A: as "a-ha"; a wrong glyph is worse than an honest flag. Flagged drills fell from 45% this morning to 21%
- 23:40 · decision · Adam chose TestFlight over a public App Store listing, on the recommendation that it reaches the team and the Mixed Nuts students this week without putting unreviewed renderings of Geshe Michael Roach's work on a public store before he has ruled on them. Same build, same upload, up to ten thousand testers. Icon drawn (ཆོས on manuscript cream with a pecha rule, legible at 60 pixels inside Apple's mask), metadata written, and a one-command build script that refuses cleanly if no signing certificate exists. Blocked on three minutes in Xcode
- 01:15 · fix · Two more classes of flagged syllable resolved, taking the drill pack from 19% flagged to 14% — and from 45% yesterday morning. GA-Y was established from the corpus rather than guessed, which is why I left it alone the first time: the banked wylie reads "lag ga-yon zhags pas" (left hand, གཡོན), "ga-yas pas gzungs thag" (right, གཡས), "'thor ga-yab" (fan, གཡབ) and "rlung ga-yos" (the wind stirred, གཡོས) — four independent glosses, all the g.y form, across 382 segments. And the lone shad: acip_to_ewts strips leading and trailing commas before it does anything else, so a "," arriving alone reached the converter as an empty string and was refused; inside a longer run the same shad converts perfectly. It was 235 of the 1,051 remaining flags, the largest single class
- 01:20 · fix · The audit's finding on the chunk-order drill: you reorder ONE CLAUSE but the reveal printed Geshe Michael Roach's English for the WHOLE segment. Measured over a thousand drills the clause is 45% of the segment on a uniform draw and 59% with adaptive on, and under half in two drills of three. The reveal now says which text its English covers, and the bracketed supplied material is greyed the same way the cloze greys it
- 01:40 · research · The Learn tab vision: 117 agents, 108 tools proposed, 20 kept to build now, 23 for later, 65 CUT. Banked at docs/LEARN_TAB_VISION.md, nothing built, nothing ruled on. Its two load-bearing figures were re-measured independently before banking because claimed numbers in this project have been wrong before: the alignment layer covers 1,547 of 42,199 segments (3.7%) and 11,777 links carry no English exponent (28.2%) — both confirmed exactly. The plan flags its OWN frequency figures as unreliable (two passes differed 2.5x) and forbids printing them as fact until a disambiguating segmenter exists
- 01:45 · decision · The progression system the research recommends, for Adam's ruling: make the game layer a MEASUREMENT rather than a reward. XP proportional to measured difficulty with a receipt on every award; provisional-tier glosses pay less than curated ones, so the tier discipline reaches into the scoreboard; ranks gated on demonstrated reading and revocable when the evidence stops supporting them; and no streak on the main surface, because streaks have the weakest link to outcome and the strongest link to practising to protect a number. The distinctive mechanic is scored REFUSAL — "this is ambiguous" and "this cannot be verified" as first-class answerable options, worth marks when right and penalised when used to dodge. No language tool does it, and it turns the project's four inviolable rules into the curriculum

## 2026-09-10 · Kawachen Tibetan Reader harvested
- Decoded the reader at itibet.org by driving its own state machine rather
  than guessing from filenames: the site indexes recordings by SOUND, not
  spelling. Scheme written up in docs/KAWACHEN_READER_FORMAT.md.
- Harvested in full: **3,002 files, all distinct, 22.3 MB** — the complete
  54 x 5 x 11 syllable grid plus the 32 spelling-term recordings. Manifest
  labels 2,891 with the syllable they say; 111 kept unlabelled rather than
  guessed. tools/kawachen_harvest.py, output gitignored per Adam's ruling
  that the permission is in-house use, not redistribution.
- Recovered audio their own site cannot play: the reader requests a
  four-number filename for syllables ending in sa, which 404s. The
  recordings exist at suffix slot 10 and we fetch them directly.
- **First independent cross-check of pron_engine.py**: the reader's
  equivalence classes agree with our engine on 11 of 12 merges, 3 of 3
  folds, and all 9 tone-only splits behave as designed
  (tools/kawachen_crosscheck.py). The one disagreement is ours to keep —
  the spine's 400+ attested khr- and 229 phr- readings confirm GMR's
  convention flattens aspiration. Filed in docs/FINDINGS.md.
- OPEN: the audio must NOT go into the TestFlight build without asking
  Kawachen again — shipping to students is distribution, not in-house use.

## 2026-09-10 · Kawachen audio renamed descriptively
- Adam asked for filenames that say what they contain. Put the scheme through
  a four-way design panel with adversarial critique; the panel caught a fault
  in the obvious answer.
- **The dot is a live EWTS operator.** Adam's proposed `49_2_3.klud.glud.mp3`
  would have produced filenames that parse as valid but WRONG Tibetan:
  our own ewts_unicode.py turns `klud.glud` into ཀླུདྒླུད with success=True
  and no flag (`g.ya` is གཡ where `gya` is གྱ). `+` and `,` fail the same way.
  `-` fails loudly as ⟨klud-glud⟩ — the behaviour inviolable rule 3 wants —
  so `-` joins co-spellings and `--` separates key from sound. Same test
  chose the sentinel: `unknown` parses to ཨུནྐྣོཝན, so it is `UNKNOWN`.
- Renamed all 3,002: `syllable_mp3/49_2_3--klud-glud-blud-rlud-slud.mp3`.
  Site key first, so files sort in TIBETAN alphabet order, the harvester still
  resumes by key (zero re-downloads), and provenance leads the name.
- Added `by-sound/`: 3,936 relative symlinks named sound-first over 2,891
  recordings, so `ls by-sound/kla*` works. Unidentified files get no link —
  a link named for a spelling would assert the guess we refuse to make.
- **Three defects of my own, found and fixed en route:**
  1. `kawachen_harvest.py --verify` rewrote manifest.csv unconditionally, so a
     half-finished rename would have destroyed the only record of what each
     recording says. Now atomic, and refuses to shrink without `--force`.
  2. That manifest was gitignored with no copy anywhere. Now tracked (it holds
     no audio and no third-party prose), with ledger rows.
  3. The label tables lived in three places and had already drifted — the doc
     said the 111th unlabelled file was `zk_3`; it is `add_5`. Now one source,
     data/kawachen_labels.json, read by all three tools.
- New gate `kawachen_index` (ctest) fails on any drift between the two views
  and the manifest; skips cleanly (77) where the audio is absent.
- Batteries unchanged: ewts 99.03%, sanskrit 12/12, crosscheck 11/12+3/3+0/9.

## 2026-09-10 · The audio becomes a feature: two builds, and a C++ read-aloud core
- Adam's ruling: this splits into TWO PRODUCTS from one codebase. A PRIVATE
  build on his laptop with the Kawachen read-aloud features, and a PUBLIC
  release without them. Implemented as `-DALL_AUDIO=ON/OFF`: with OFF the
  audio code is not merely disabled, it is not compiled, so a public binary
  cannot reach a recording even if one were placed beside it. Proven both
  ways — the same battery passes in the private build and reports SKIP (77)
  in the public one. Never fork the tree for a release; flip the flag.
- **Measured what the audio can actually do, before designing anything on it.**
  Bridging both sides through pron_engine (so no second phonology exists):
  the bank covers **99.3% of the syllables in the 42,199-segment corpus**
  (94.7% one recording + 4.5% base-plus-ending) and 98.6% of 40,000
  dictionary headwords. The residue is almost all -bs finals and Sanskrit
  letters.
- Built `allcore::SpeakBank` — resolve a syllable to the recordings that say
  it, with an honest tier on every hit: exact / exact-with-a-silent-letter-
  dropped / two-recordings / APPROXIMATE. A miss is flagged, never
  approximated further.
- Exposed `allcore::pronSyllableReading()` — the same per-syllable function
  `pronounce()` has always used — so the audio path is selected by the
  authority that prints the phonetics and cannot drift from it.
- New `speak_battery`: the C++ resolver reproduces the Python prototype
  **exactly across all 11,939 syllables** of the corpus and dictionary, 0 tier
  and 0 file disagreements. Founding rule 2 satisfied.
- The battery caught a bug in my own oracle: the Python stripped a LEADING
  `v` as wa-zur, but wa-zur is subjoined and cannot open an onset, so stray
  Latin tokens in the corpus (`va`, `valid`) were being read aloud as Tibetan.
  263 syllables affected. The C++ was right; the oracle was corrected.
- The fixture gate then caught the new reference file as undocumented and
  failed the press until it was written into docs/FIXTURES.md. Working as
  designed.
- **The honest constraint, stated up front:** these are isolated syllables of
  0.29-0.65s. Playing them in sequence is syllable-accurate reading aloud in a
  real human voice — which is exactly how Tibetan is taught (sbyor klog) — but
  it is NOT a native speaker reading a sentence, and no surface may imply it
  is. Ideas that need natural prosody are dead on arrival.
- Read Kawachen's own English documentation of their iPhone app
  (kawachen.org/app/tibetanreader_en.html): four screens — Alphabet builder,
  Syllable display, History roster with replay, and a listen-and-identify
  Quiz. They document five limitations, and our engines already beat one of
  them: they cannot do the zla ba -> "da wa" particle sandhi; pron_engine can.
- Suites now 116, all green.

## 2026-09-10 · Xcode project for the iPhone app
- There was no Xcode project at all — `ios/testflight/build.sh` hand-compiled
  with swiftc and signed by hand, which can produce a simulator build but
  cannot install on a real device (no embedded provisioning profile).
- Generated `ios/TibetanTrainer.xcodeproj` from `tools/make_ios_project.py`:
  generated rather than committed by hand, because a .pbxproj is merge-hostile
  and Xcode rewrites it on any incidental click. Uses Xcode 16+ synchronized
  folders, so adding a Swift file or an asset needs no project edit.
- Signing left Automatic with an EMPTY team on purpose: Adam signs in once and
  Xcode mints the certificate itself. No credential ever passes through here.
- **Bundle id changed `org.asianlegacylibrary.DiamondDrills` ->
  `...TibetanTrainer`** while it is still free to change. Nothing is registered
  with Apple yet; after the first TestFlight upload it would be permanent.
- Two blockers found, both Adam's to clear and both needing his Apple ID:
  1. **Xcode's iOS 26.5 platform is not installed.** His phone runs 26.5; the
     SDK is present but the platform support is not, and without it Xcode
     offers NO destinations at all — not the phone, not even a simulator.
     Xcode ▸ Settings ▸ Components. Several GB.
  2. **No signing identity in the keychain** (`security find-identity` finds
     0). Xcode ▸ Settings ▸ Accounts.
- Isolated blocker 1 from our own code by compiling DiamondDrills.swift
  directly against the simulator SDK: exit 0, 698 KB binary. The source and
  the project are sound; the missing component is Xcode's.
- His iPhone (བདེ་མཆོག་, iPhone 16 Pro Max) is already connected and paired.
- Moved 14 loose icon PNGs out of the app folder to `ios/icon-sources/` — with
  a synchronized folder they would have been bundled into the app as stray
  resources. The asset catalog carries its own 1024 icon.
- Fresh drill pack generated: 4,000 cloze drills + 1,000 trainer passages,
  5.7 MB, 0 title-catalogue drills.
- Setup written for Adam at `ios/SETUP.md`, including the free-vs-paid
  distinction: a free Personal Team puts it on his phone today with a 7-day
  rebuild cycle; TestFlight to students needs the $99/yr programme.

## 2026-09-10 · Apple Developer Program confirmed paid — TestFlight unblocked
- Adam is enrolled in the **paid Apple Developer Program** as an Individual,
  from 10 Sep 2026, renewing 2027. This settles the morning's open question:
  TestFlight to the team and to students is available, and a build on his own
  phone lasts a year rather than the free tier's 7 days.
- Team ID stored at `ios/DeveloperTeam.txt` (gitignored — not a secret, but the
  repo is public) and written into the generated project, so it survives every
  regeneration and the archive step has it.
- Caught before it shipped: a blank team emitted `DEVELOPMENT_TEAM = ;`, which
  is a .pbxproj parse error — the project handed over an hour earlier would not
  have opened. Both the blank and populated paths are now verified to parse.
- TWO THINGS FOR LEADERSHIP, neither blocking:
  1. **Enrolled as an Individual, not an Organization.** TestFlight is
     identical either way; what differs is the publisher name a tester sees —
     Adam's, not Asian Legacy Library's. Moving to an Organization needs a
     D-U-N-S number for ALL and gets more awkward the longer an app has been
     published under a personal name. Worth putting to John Brady.
  2. **The registered-device list resets 10 Sep 2027** and only then. Free now,
     matters once a hundred students have come and gone.
- Still blocking a device build: Xcode's iOS 26.5 platform component
  (Xcode ▸ Settings ▸ Components) and signing in to Xcode. Both Adam's, both
  needing his Apple ID.

## 2026-09-10 · The iPhone app builds and runs through a real Xcode project
- Adam installed Xcode's iOS 26.5 platform; his phone (བདེ་མཆོག་) is now an
  available destination rather than an ineligible one.
- **`xcodebuild ... BUILD SUCCEEDED`** — the generated project compiles and
  links the app. Installed it on the simulator and verified both modes: the
  Trainer shows a passage with six reveal layers, the Drills view shows the
  whole segment with English in proportion, clean Tibetan options, no raw
  source code and no parenthesis tell. The drill pack (5.7 MB) is bundled.
- The device build now fails on exactly two things, both of which Xcode's
  automatic signing resolves once an Apple ID is configured: the phone is not
  yet registered to the account, and there is no provisioning profile for
  org.asianlegacylibrary.TibetanTrainer.
- Remaining: Xcode ▸ Settings ▸ Accounts. A certificate in the ACCOUNT is not
  a certificate in this Mac's KEYCHAIN, and it is the keychain that signs.
- Investigated `[EM:414]` in the corner of the Trainer, suspecting debug cruft
  left in a shipping build. It is not: it is the course code and segment
  number the passage came from — provenance, which this project keeps on
  purpose. No change made.

## 2026-09-10 · The app is on the phone, and one command keeps it there
- Device build succeeded and the app is INSTALLED AND RUNNING on བདེ་མཆོག་.
  The missing piece was `-allowProvisioningDeviceRegistration`:
  `-allowProvisioningUpdates` lets xcodebuild talk to Apple but explicitly
  refuses to register a NEW device without the second flag. With both, one
  command minted the certificate (keychain 0 -> 1 valid identity), registered
  the phone, created the profile, built and installed. The GUI hunt I sent
  Adam on was unnecessary — the manual had the answer.
- Answered "does a desktop Trainer change reach the phone?" by reading the
  code rather than asserting: allcore is genuinely shared (drills.cpp,
  tibdisplay) and reaches the phone THROUGH THE PACK; app/study_pane.inc (the
  desktop Trainer UI, 209 lines) and DiamondDrills.swift (the iOS UI, 459
  lines) are separate implementations and share nothing.
- **Found a real hazard: nothing regenerated the pack automatically.** A fix
  in allcore needed four manual steps to reach the phone, and a stale pack
  was indistinguishable from a fresh one. Two fixes:
  1. `tools/build_drill_pack` now stamps the pack with the commit and the UTC
     moment it was built (`built_from`), so staleness is visible.
  2. `tools/ios_deploy.sh` does all four steps in one command — rebuild
     allcore, regenerate the pack, build the signed app, install and launch.
     Verified end to end; the phone now reports what it is running.
- Audit running on where desktop and iOS behaviour is implemented twice, since
  silent divergence between two implementations is this project's
  characteristic failure mode (it bit us this morning with the audio keying).

## 2026-09-10 · Two honesty defects found on the phone, fixed and deployed
- The divergence audit confirmed two live rule violations on iOS, both verified
  by me against the shipped pack rather than taken on trust:
  1. **253 of 1,000 trainer passages carried a verb the engine could NOT
     attest, and the phone printed it at 21pt as the layer's answer** with a
     caveat underneath. The desktop withholds it entirely (main.cpp:21498).
     A guess in the answer position is still a guess. The phone now matches
     the desktop: "No verb the dictionary can confirm in this clause."
  2. **The Trainer layers describe a median 32% of the passage on screen** —
     933 of 1,000 under 90%, 737 under half — with nothing saying so, while
     the desktop already ships that disclosure for the same mismatch. The
     pack now emits `clause_share` (computed in C++, not re-derived in Swift)
     and the phone states it.
- A third claimed defect — nested brackets mislabelled as his English — is
  real code but has ZERO instances in the shipped pack. Latent, not live;
  recorded, not fixed.
- **The finding of the day, from Adam asking how one is meant to deduce the
  answer: 72.2% of cloze drills can be solved by the case particle alone.**
  Only 22.6% force a reader to engage the meaning. Distractors are drawn as
  "marked chunks from other random segments" without matching the answer's
  marker, so in three drills out of four exactly one option fits the slot
  grammatically. Length is not a tell (22.9%/21.6% against 25% chance).
  This explains his own report exactly — answering correctly while still not
  knowing what the chunk meant. His confusion was a symptom of a design flaw,
  not of him missing something.
- app_selftest timed out under -j4 but passes in 270s run alone. Fragile
  rather than broken; the default timeout is too tight for it.

## 2026-09-10 · The Quick Access strip, made to work as it claimed
- Adam: "I should be able to right-click a pin and unpin it." Two findings.
  1. Pinned buttons had NO context menu — unpinning meant a trip to
     Preferences for a thing you are looking straight at. They now carry
     Unpin, Move left and Move right (order was previously unchangeable once
     pinned). popup() not exec(), so this stays off the modal census.
  2. **The Preferences page has always said "right-click a ribbon button to
     pin it" — and that was never wired.** No ribbon button had a context
     menu anywhere in the app; the star menu was the only way in. Adam found
     it by trying the documented gesture. RibbonProxy now offers Pin/Unpin.
- The bridge that was missing: pins persist as MENU PATHS ("Group>Pane>Action")
  because a pane's button pointer does not survive a restart, while a ribbon
  button proxies a pane's button, not a QAction. `qatPathForLabel()` finds the
  matching menu entry by label; when there is none it says "cannot pin this
  one" rather than writing a pin that would resolve to nothing next launch.
- Dead pins ("not in this build") are now removable in place. They are greyed
  by STYLE rather than setEnabled(false) — a disabled widget receives no mouse
  events, so a context menu on it would never have opened, which is precisely
  the pin you most want to remove.
- Preferences text corrected to describe what the app now actually does.
- Selftest green (270s).
- Menu-bar regrouping under design: the bar carries NINETEEN top-level menus —
  twelve hand-written plus SEVEN generated one-per-pane-group at main.cpp:41789,
  which duplicate a ribbon and a tab bar that already do that job. macOS
  expects 6-9. Constraint found before touching it: saved pins resolve by
  walking the menu bar three levels deep, so any restructuring that changes
  depth or a top-level name breaks every saved pin.

## 2026-09-10 · The three Trainer changes, and where each one lands
- **The distractor fix.** Distractors must now carry the same role marker as
  the answer. Measured: guessable-by-particle 72.2% -> 0.0%, and 95.4% of
  drills now require reading (the residue carries no case particle at all).
  A drill that cannot be filled honestly is REJECTED rather than padded with
  an easier option; a full 4,000-drill pack still builds, in 1:42. This is in
  core/src/drills.cpp, so BOTH apps get it.
- **What the blanked word means.** allcore::checkTerminology already existed
  and already had a battery, so this was a call site rather than new engine
  code. 94% of drills carry a card; 63% lead with a word he actually used in
  that passage. Never "the answer means X" — the median glossed headword has
  four attested equivalents and the worst 137, so naming one would be picking
  a sense. Shown identically right or wrong, because being right is exactly
  when Adam reported learning nothing.
  The report orders unmatched first, which is right for a translator hunting
  gaps in a draft and backwards for a learner's card; re-sorted at both call
  sites.
- **Hint and lookup.** "Rule out one" strikes a single wrong option —
  deliberately weak, because revealing the ROLE instead leaves one option
  standing in roughly half of all drills, which is the answer and not a hint.
  "Look up" is reachable only after answering; before it, it would replace the
  retrieval effort that makes a drill work. The pack carries twenty
  equivalents so the button reveals something real rather than promising more
  and showing nothing.
- Two defects fixed on sight: the phone said "the answer is in green",
  pointing at a colour alone, and now names it; and the floating read/drills
  counter was sitting on top of the new meaning text.
- Wording, Adam: "the text has", not "his text has" — the Tibetan source is
  the classical work, not his writing. The English attributions stay. The
  desktop's particle mode already used that phrasing, so this was house style
  the phone had drifted from.
- **Asked whether these reach the desktop: only the first did.** The meaning
  card, the verdict wording and the counter fix were iOS-only, which is the
  divergence the audit warned about and which I had just widened. The meaning
  card is now on the desktop cloze too, reusing hgmGlossPhrase so the
  provenance claim is worded once in the app rather than a fourth time.
  Hint and lookup remain iOS-only and are listed as such.

## 2026-09-10 · Known here / known anywhere — the core, and a leak closed
- The deck now separates the two kinds of knowing. A word recognised only in
  the sentence it was met in is the weakest form of knowing and the one a bare
  flashcard measures; `vocab` gained `first_segment`, `transfer_segment` and
  `stage`, and being right in a DIFFERENT segment is what promotes a word from
  "known here" to "known anywhere".
- Proven on a scratch deck: met 2 -> right in its own segment -> here 1 ->
  right elsewhere -> anywhere 1; a WRONG answer elsewhere does not promote;
  and the origin segment survives meeting the word again somewhere else
  (222 stays 222 after a later sighting in 777).
- Migrated by ALTER rather than by editing the CREATE: decks already exist on
  disk and `CREATE TABLE IF NOT EXISTS` silently skips a changed definition,
  so the columns would never have appeared on a live deck and every read would
  have failed.
- **The leak the spec predicted is real and is closed.** The Trainer's six
  reveal layers touched the deck NOWHERE — a word could be shown, understood,
  and never come back. Revealed glosses now enter the deck.
- Three places record an origin of 0 ON PURPOSE, and each says why: the
  desktop Trainer analyses PASTED text with no corpus segment; the bulk
  deck-fill comes from the dictionary rather than from meeting a word in a
  sentence. Claiming an origin there would put a learner in a segment they
  have never read.

## 2026-09-10 · Mantras were being drilled as Tibetan grammar
- Adam photographed his phone showing a Trainer passage reading
  `ཨོཾ⟨hrI:⟩⟨ShTrI:⟩ཕྲེ་ཊི་ཧཱུ་ན་ན⟨hUM^⟩་ཐན` — a mantra from a Kali Wang
  empowerment text, with three syllables the converter had refused.
- The ⟨...⟩ flags were CORRECT: those are Sanskrit written in Tibetan letters,
  which the Tibetan converter will not render and so flags rather than guesses
  (rule 3). The defect was upstream of that — the passage should never have
  been drawn. A mantra has no verb, no particles and no reading order, so the
  Trainer's six layers have nothing true to say about it.
- Measured before fixing: **9.8% of trainer passages and 9.6% of cloze drills**
  carried an unconverted syllable, concentrated exactly where you would expect
  — GIE, TCS16KaliWang, TCS04Initiation, TCS04PrayerBook.
- `isDrillable` now refuses any segment the display converter cannot render,
  the same class of refusal as a title line. In allcore, so both apps inherit
  it. Trainer 9.8% -> 0.0%.
- One drill in 800 still leaked, for a DIFFERENT reason: the segment converts
  whole, but splitting it at the blank can leave a fragment that will not
  convert alone (`⟨s⟩`). The two halves are what the reader sees, so the pack
  builder now checks those too. Now 0 of 4,000.
- The mantras are not being dismissed — the Sanskrit workbench reads them
  properly. They are simply the wrong material for a drill about Tibetan
  grammar.

## 2026-09-10 · Known here / known anywhere, finished
- The review surface replaces the weakest thing in the Learn tab — a bare
  ACIP word with "Do you know this word? Grade yourself", which is precisely
  where a generic flashcard app is our equal. A word is now presented IN the
  sentence it was met in, and its second review in a sentence the reader has
  not seen.
- The question itself now says which is being asked: "do you know it HERE, in
  the sentence you met it in?" then "do you know it ANYWHERE?"
- **Gate 1, register divergence, implemented and measured rather than
  assumed.** 3,299 of the 12,004 glossed headwords carry more than one
  attested English — bsod nams literally holds "goodness (prayer register)"
  beside "merit" and "good deeds". Where a word has several, ALL are shown
  with their tier and the pane says: grade yourself on the sense here, not on
  matching one of them, because that difference is the lesson. Marking a
  reader wrong for producing the sense they learned would teach that one
  Tibetan word has one English, which inverts the project's central claim.
- **Gate 2, no second attestation.** When a word occurs nowhere else in the
  corpus the pane says so and the word stays at "known here". It is never
  re-served in its own segment and counted as transfer — that would measure
  familiarity with a sentence and call it knowledge of a word.
- A third case the plan did not name but the data forces: words that came from
  the bulk dictionary fill have no origin sentence at all, and the pane says
  that rather than implying one.
- Both readouts are shown after every review: met / known here / known
  anywhere.

## 2026-09-10 · The key / no-key badge
- Every passage now says, BEFORE any effort is invested, whether there is an
  answer key for it. The Trainer's sixth layer already degraded correctly when
  there was none; what was missing was that a reader could not see the
  degradation coming until the work was done.
- Two states, each with its own frame. KEY: "Geshe Michael Roach's own English
  for this passage is the answer key — COURSE:SEQ". NO KEY: "not in the
  corpus, so there is no answer to check against — only engine guidance. The
  engine can see particles, chunk boundaries and a verb; it cannot see what
  the passage MEANS, and it is not his English."
- The Trainer works on PASTED text, so it now looks the passage up: matched on
  the longest leading run rather than the whole, because a reader pastes a
  fragment, and a hit must actually CONTAIN that fragment — a search that
  merely ranks it first is not evidence it is the same passage.
- The three corpus drills carry the KEY badge by construction, with citation.
- Learn tab now 5 of 12 built: Zero to script, Boundary Hunt, Train this,
  Known here / known anywhere, the key / no-key badge.

## 2026-09-11 · The Learn tab plan, finished — and one entry that lied

- **All twelve tools in the Learn tab plan are built.** The last three landed
  today: the Debate Dojo (the reply-target half), Peel, and Boundary Hunt.
- **Boundary Hunt was marked BUILT and was not.** The plan carried that mark
  since 2026-09-10; the Drills pane had no such mode, and nothing in the shared
  core produced one. The mark appears to have been made on the strength of
  "Chunk order", which reorders chunks inside a clause and never asks where a
  clause ends. Found while auditing the plan's own claims, built the same day,
  and the false mark recorded in its own section rather than painted over.
- Boundary Hunt strips a real segment's punctuation and asks where the clauses
  end. Two pools, kept apart: where the scribe marked a boundary the key is
  ATTESTED; where he did not, the key is the engine's ruling and every line of
  the reveal says so. Three perfect answers in the first pool promote you to
  the second, and the promotion is announced rather than silent.
- **The plan had the honesty backwards, and the data settled it.** The plan
  said the clause splitter "declines where it cannot rule". It never declines:
  it splits on positive verb evidence and merges on the ABSENCE of it — which
  is not knowledge that the word is a noun. Those positions are now reported,
  and a reader who marks one is not counted wrong, because we do not know that
  they are.
- Peel: a span, and how many pieces it splits into at the next level down. The
  split is scored, not the translation. 39,945 measured nestings — the plan
  said 43,325, which counted page elements rather than nestings actually
  derived.
- A defect found while doing the above: **the Mixed set hid its own answer
  box.** Visibility was keyed to the mode box, which in a mixed set
  deliberately never names the kind — so when the queue picked chunk order the
  box stayed hidden and Check scored the empty string as wrong. One item in
  four was unanswerable on a mode that shipped the same day.

## 2026-09-11 · The phone catches up, and one thing deliberately left behind

- The phone had one drill kind where the desktop had eleven. It now has
  eleven of its own: cloze, chunk order, particle, script cards, debate,
  silent particle, his second thought, Peel, boundary hunt, known here /
  known anywhere, and the mixed set. In the Trainer it gained the key badge
  and the ungraded reading-order gate.
- **"What you can read next" stays on the desktop, on purpose.** It measures a
  reader's word deck against each work's full vocabulary — 66 works, 153,177
  glossed headwords. On the desktop that deck grows from reading, from Trainer
  reveals and from the concordance. The phone has none of those surfaces: its
  deck can only grow over what ships in the drill pack. Putting the shelf there
  would divide a real denominator by a numerator capped at a few hundred words
  and report that every text in the library is about 0.4% readable. That is not
  a smaller version of the feature; it is a false number wearing its clothes.
- One crash caught by the press rather than by any test: a database migration
  added columns without checking whether they already existed, so the app ran
  correctly the first time and aborted on the second launch. Every suite passed
  because test databases are always new. It would have met everyone on their
  next start.

## 2026-09-11 · Nineteen menus down to eleven

- The menu bar had twelve hand-written menus plus one generated for every pane
  group, so it grew a menu each time a group was added and had reached
  nineteen. It is now eleven, and fixed: File, Edit, Panes, Find, Insert,
  Format, Tools, Goto, View, Window, Help.
- Selection folded into Edit; Project folded into File; every pane group folded
  into one Panes menu, with the group's name kept as a header rather than
  becoming another submenu — a submenu per group would have put every pane
  command four levels down and added a hover-and-wait to reaching any of them.
- **The known risk was real.** Pinned commands on the Quick Access strip were
  found by walking the menus exactly three levels deep; 135 of the 510 commands
  now sit deeper than that, and every pin pointing at one would have reported
  itself dead. The finder now walks the whole bar, and a stale pin is rewritten
  only when its command name matches exactly one command in the new layout.
  Otherwise it is left saying "not in this build" — a pin that quietly starts
  running a different command is worse than one that admits it is broken.
