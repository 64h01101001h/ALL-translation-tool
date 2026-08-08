# ACE recovered — Aaron Cram's ACIP software (2026-08-08)

Adam asked for any trace of **ACE**, the software meant for the old
asianclassics.org that linked the input cursor to the woodblock scan
being typed at the input centers. Found, recovered, and banked.

## What was found, where

- **The trail**: Firefox bookmark backups on this machine hold
  "ACE — Classical Indian Works Preserved in Tibetan" at
  `http://www.aciparchive.org/ace/` (with deep-link anchors like
  `#col(tendg)`). The site is dead (no DNS); the Wayback Machine holds
  2010–2012 captures showing it was a **GWT** (Google Web Toolkit)
  web app — compiled JS only, not recoverable as source from the
  archive.
- **The source**: Aaron Cram is `rekursiv` on GitHub. His
  **`rekursiv/acip-support`** repo ("Support software for the Asian
  Classics Input Project", last pushed 2015-03-31, **Apache-2.0** per
  the wiki) contains the whole ecosystem, wiki included. Cloned to:
  - `~/ACIP Software Developmeent/acip-support/`
  - `~/ACIP Software Developmeent/acip-support.wiki/`
  (His `rekursiv/hypercontext` is also the ancestor of Adam's
  Hypercontext fork.)

## What ACE actually is — two halves

1. **`org.acipview`** — the ACE web viewer that ran on
   aciparchive.org: a small GWT app (Main/AppUI/EntryModel/EntryRepo/
   LinkManager) over a CouchDB catalog (`acip-center-nlm-catalog`).
   This is the public archive-browser half.
2. **`org.asianclassics.center.input`** — the INPUT-CENTER desktop
   module (Java/SWT/Guice/Guava-EventBus, CouchDB+JGroups cluster):
   scan viewer above, ACIP editor below in a movable sash, and THE
   CURSOR↔SCAN LINK Adam remembers:
   - the editor fires `AcipEditorCaretMoveEvent(line, offset)` on
     every caret move;
   - `ScanPanel` (ScrollLinkMode.CARET) scrolls the scan to
     `(offset × avgCharWidth, line × avgCharHeight)` where
     `avgCharWidth = widthOutsideView/200` and
     `avgCharHeight = heightOutsideView/5` — i.e. a PROPORTIONAL
     HEURISTIC assuming ~5 scan lines per view and ~200 chars per
     line; there is also a SYNC mode (editor scroll × 4 × zoom).
   - Around it: the full double-keying workflow — blocks of N pages
     (default 10) typed by two partnered operators, then re-presented
     for correction "until the text input by both input operators
     match exactly", with a Tibetan spell checker (red squiggles) and
     **diff_match_patch discrepancy highlighting** in the editor
     (RSyntaxTextArea; `org.asianclassics.text.edit`).
   - Sibling modules: `center.catalog` (the Mongolia cataloging app),
     `center.input.dispatch/upload/admin/db`, and the abstract
     `center` base (login, clustering, task dispatch).

## Employing it moving forward — assessment

- **The cursor→scan idea is already reborn in our tool, stronger.**
  ACE's link was a proportional guess; our Overlay word-locate and
  Scan-pane word boxes use REAL per-word coordinates (pyctcdecode
  frame spans + colMap, battery-proven). For reading/verifying against
  scans, we have surpassed ACE.
- **What we should take from ACE**: the INPUT side — a typing pane
  beside the scan with live caret↔scan tracking, spellcheck, and the
  double-keying diff loop. That is exactly roadmap item A (the
  input-center formatter/proofreading pane), and acip-support is its
  reference implementation, Apache-2.0, by the original author:
  1. an **Input pane**: scan above (BDRC image or input-center scan),
     editor below; caret-linked scroll (ACE's mode) UPGRADED with our
     OCR word anchors when models are present;
  2. **double-keying diff** via diff_match_patch (already on the TODO
     with antx logic) — acip-support's DiffUtil/DiffTokenMaker show
     the exact wiring the input centers used;
  3. our existing syllable-legality spellchecker slots in where their
     RSyntaxTextArea checker sat.
- The 2015 Java/SWT/CouchDB stack itself is not worth reviving as-is;
  the VALUE is the workflow design + Apache-2.0 reference code.

## Provenance

Aaron Cram (github.com/rekursiv, Vancouver WA), for the Asian Classics
Input Project. Wiki states Apache 2.0 for all source. The wiki pages
(Technical-Overview, Input-Center-Module, Input-Workflow,
Input-Data-Flow, Network-Topology, Catalog-Module, Database-Setup)
are the design documentation, with screenshots in `images/`.
