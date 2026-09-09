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
