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
