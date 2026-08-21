# THE TEST CAMPAIGN — a systematic real-life pass over everything
*Started 2026-08-20 at Adam's directive: "a systematic real life
test of every workflow and every pane and every function/tool."*

## How to run it
- Work group by group, pane by pane, in the order below (it follows
  the app's own tab order). Check items off as you go.
- Every item has an ID (e.g. `R-OV-03`). To report a finding, just
  tell the machine the ID and what you saw — one line is enough
  ("R-OV-03: the card came up empty for XYZ"). Findings get fixed
  and re-pressed as you keep testing.
- Verdicts: ✅ works · 🐞 broken · 🤔 confusing/unexpected ·
  💡 works but could be better. All four are useful; 🤔 and 💡
  findings are how the polish list grows.
- Use REAL material throughout — your actual texts, your actual
  intake folder, the real Dropbox library. The selftest already
  proves the plumbing (59 suites, 200+ checks); this campaign tests
  the part no harness can: whether it works FOR YOU.

---

## READ → Overlay
- [ ] R-OV-01 Open an ACIP file; the document renders with phrase
      shading; click a word → the card shows tiered HGM equivalents.
- [ ] R-OV-02 Arrow keys after a click: ← → walk phrases; ↓ steps
      into the nest; ↑ steps back out.
- [ ] R-OV-03 The card's layers: phonetics, HGM definitions, corpus
      usage, 84000 glossary block (with "attested in" toh links —
      new), Das/Jäschke page-scan links, teaching moments (with the
      new "index not loaded" warning if the data is absent).
- [ ] R-OV-04 Click a term in the anchors panel → corpus
      concordance; below the HGM hits, the 84000 Translation Memory
      comparanda section (new) with toh+folio refs.
- [ ] R-OV-05 REVIEW tools, one by one: Outline (sa bcad) ·
      Citations & quotations · Verse meter · Prose & prayer reader ·
      Typography check · Text DNA strip (new — hover + click-jump) ·
      Teachings for this text · Spellcheck (Check all + File for
      ruling).
- [ ] R-OV-06 Scans follow-along: woodblock follows the cursor;
      locate word (OCR) boxes the cursor word; NEW: hover a word on
      the carving → OCR-read wylie + HGM gloss tooltip.
- [ ] R-OV-07 Display toggles: each checkbox changes the card/marks
      as labeled, and the setting survives a relaunch.
- [ ] R-OV-08 Zoom: ⌘+/⌘−/⌘0 in both the text and the card panes.

## READ → Library
- [ ] R-LB-01 Install a collection ZIP; import personal materials
      (.docx converts); the tree sorts and previews.
- [ ] R-LB-02 Survey… button on a selected text (length, folios,
      structure); Maintenance menu (UTFC rescue, Send to OCR,
      Update index).
- [ ] R-LB-03 Catalog links on files: BDRC scan links, THL catalog
      links (Degé + Lhasa), person links on author names.
- [ ] R-LB-04 ⌘K Hunt Everywhere: files, entries, corpus, persons
      (👤), apparatus (📎), teachings (🎧) lanes all answer.

## READ → Files
- [ ] R-FL-01 Dual-pane browsing, tabs, favorites, Drop Stack,
      archives (ZIP/TAR) browse + extract.
- [ ] R-FL-02 Remote connections (SFTP/FTP/WebDAV) if a server is
      handy; transfers up and down; folder sync.
- [ ] R-FL-03 Spotlight search panel (async, Stop works).

## READ → Scans
- [ ] R-SC-01 Open a scan; page navigation; deskew view.
- [ ] R-SC-02 BDRC IIIF fetch for a cataloged text (needs network).

## READ → Export
- [ ] R-EX-01 Prepare for translation (Mixed Nuts format) on a real
      text; NOTES section folio-tagged.
- [ ] R-EX-02 Export print Tibetan (Unicode); syllable counts
      reported; failures flagged never guessed.
- [ ] R-EX-03 Pecha Maker: single text → PDF (native + A4 two-up +
      booklet); verse lineation; title folio; cover sheet; batch
      folder mode. Compare a page against a real print if one is at
      hand (the print-QA half of the pecha program is still open).
- [ ] R-EX-04 EPUB/e-reader export is NOT built (deferred) — confirm
      nothing pretends it is.

## TRANSLATE → Draft
- [ ] T-DR-01 Load source; clause list; anchors panel; Evidence
      Ribbon follows the cursor.
- [ ] T-DR-02 Terminology guard flags a drifting rendering; Phrase
      memory answers before you invent one.
- [ ] T-DR-03 STRUCTURE tools; EVIDENCE tools (incl. apparatus
      search); PUBLISH tools: Propose footnote, Compose bibliography
      entry (ACIP number auto-fill), RTF export with *italics*.
- [ ] T-DR-04 AI back-check (needs API key): coverage report only,
      banner-labeled AI.
- [ ] T-DR-05 Gofer search sidebar: grammar queries (OR, NEAR/5),
      insert places the master's English at the cursor.

## TRANSLATE → Manuscript
- [ ] T-MS-01 Write + save; Gofer sidebar; Send to Manuscript from
      Draft carries the draft over.

## TRANSLATE → Apparatus
- [ ] T-AP-01 The bank loads populated; search narrows; bibliography
      + footnotes both reachable.
- [ ] T-AP-02 TIBETAN NAMED IN THIS NOTE blocks: body-quoted
      evidence sentences; reverse-index candidates (labeled tiers);
      NEW passage-evidence blocks ("WHERE HIS PUBLISHED ENGLISH USES
      THIS LEMMA") on single-book notes. 444 of 2,234 notes carry
      candidates — the rest are honestly bare.

## TRANSLATE → Review
- [ ] T-RV-01 Run a review on a real draft; each advisory type
      renders and reads sensibly.

## TRANSLATE → Align
- [ ] T-AL-01 Load two sides; make links; import/export.

## RESEARCH → Search
- [ ] S-SE-01 Gofer grammar over the corpus: plain, OR, NEAR;
      fold modes; Stop mid-search; per-file rollup view;
      open-at-line.

## RESEARCH → Lookup
- [ ] S-LU-01 Wylie, ACIP, Tibetan-script, pronunciation
      ("jangchub"), and English reverse lookups all land.
- [ ] S-LU-02 The card: every layer from R-OV-03 plus Whitney roots
      on Sanskrit, link-out tier, person cards on author names,
      propose-to-authority at the bottom.
- [ ] S-LU-03 Tibetan-order browsing (ka…a) works.

## RESEARCH → Sanskrit
- [ ] S-SK-01 IAST ⇄ ACIP ⇄ Devanagari ⇄ Tibetan conversions; the
      --sanskritcheck stacks render (badzra, padma, kSha…).
- [ ] S-SK-02 Devanagari OCR (needs tesseract installed).

## RESEARCH → Convert
- [ ] S-CV-01 Live as-you-type: wylie → Tibetan + phonetics + THL.
- [ ] S-CV-02 ACIP (uppercase) auto-detected.
- [ ] S-CV-03 NEW: paste/edit Tibetan SCRIPT → live reverse card
      (wylie, ACIP, phonetics; pyewts-parity engine, 109,490-fixture
      battery). Try editing the Tibetan and watch the wylie follow.
- [ ] S-CV-04 Sanskrit input (IAST or Devanagari) → the full
      Sanskrit card.
- [ ] S-CV-05 Dates section: ISO ↔ Tibetan calendar both systems;
      "me bya 6 m3" style queries; TISE keys (* and _).

## RESEARCH → Analysis
- [ ] S-AN-01 (needs API key) Passage analysis: engine anchors +
      corpus hits injected; labeled AI; gates hold without a key.

## LEARN → Trainer
- [ ] L-TR-01 Progressive-reveal reading; phonetics modes (GMR +
      THL); real corpus lines.

## LEARN → Drills
- [ ] L-DR-01 Drills generate from real segments; the miss-taxonomy
      report names families.

## INPUT → Input
- [ ] I-IN-01 The block workflow: folder of pages, scan follows
      typing, @ next folio, five-pass conventions.
- [ ] I-IN-02 OCR pre-fill (draft) into an EMPTY editor only;
      Compare with partner file (double-keying diff); auto-pairing
      suggestion.
- [ ] I-IN-03 Save/export with headers.

## INPUT → OCR
- [ ] I-OC-01 Models download/manage dialog; Run OCR is now ASYNC —
      the UI stays live, the button becomes Stop, stopping keeps
      partial results labeled.
- [ ] I-OC-02 Word boxes click-highlight on the page; legality
      flags per line; Save to ocr_out; Batch folder.
- [ ] I-OC-03 Illustration candidates + gallery.

## CATALOG → Catalog  (the 9g workflow, end to end)
- [ ] C-CT-01 NEW in-house gate: Official library… → point at the
      team Dropbox folder; Team… → create the roster (you become
      admin); Sign in…; action buttons unlock. Sign out survives?
      (relaunch = signed out by design).
- [ ] C-CT-02 Census lines on both trees; identity suffixes in the
      tree; the info panel shows the whole text.
- [ ] C-CT-03 Suggested identity on an uncataloged file (evidence
      card, bank size shown); Propose this identity → lands in
      Approval.
- [ ] C-CT-04 Audit bibliographies; Suggest splits; Compose name
      (+META for over-long); Generate catalog list; Translate
      title workbench; Compare trees; QC intake; Load register;
      Worksheet (52 columns, sidecar, CSV row); shelf suggestions;
      change-log stamp.
- [ ] C-CT-05 NEW approval pipeline: as a cataloger, Move to shelf →
      stages into AWAITING APPROVAL with a ledger row; as the
      approver, Approvals… → approve (lands on the shelf) and
      reject (moves to REJECTED with reason). Check the ledger reads
      plainly in a text editor.

## COMMUNITY → Propose
- [ ] M-PR-01 One-time identity; each proposal kind files; My
      proposals shows outcomes.

## COMMUNITY → Approval
- [ ] M-AP-01 (authority) Queues by kind incl. catalog-identity;
      approve/reject; the export package.

## THE APP ITSELF
- [ ] A-AP-01 Menu bar mirrors every pane's controls; About shows
      live counts; night mode: flip macOS dark mode and walk 3-4
      panes (KNOWN open item — expect rough edges, note them).
- [ ] A-AP-02 Quit and relaunch: session restores (pane, files,
      settings); nothing clobbered.
- [ ] A-AP-03 The DMG on another Mac if available (clean-machine
      install is a known untested item).

---

## FINDINGS
*Record here, or just tell the machine the item ID + one line.*

| ID | Verdict | What you saw |
|---|---|---|
|  |  |  |
