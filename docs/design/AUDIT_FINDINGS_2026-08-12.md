# Design Audit — Round 2 (2026-08-12)

Panes audited: 02-library, 03-scans, 06-manuscript, 08-review, 10-search, 11-lookup,
12-sanskrit, 13-convert, 14-analysis, 16-drills, 17-input, 18-ocr, 19-propose, 20-approval.
(01, 04, 05, 07, 09, 15 audited and fixed in round 1 — skipped.)

Judged against: the translator's minute; first-glance teachability; consistency
(gold eyebrows, banner pattern, parchment reading surfaces); dark-mode chrome
legibility; no clipped text; no dead space that could teach.
Screenshots: scratchpad `shots/` (offscreen selftest render, 2360×1520).
Code anchors are `app/main.cpp` line numbers at commit 6ef2982.

---

## 02-library — NEEDS WORK

1. **Banner clipped at the right window edge** (defect, principle 5). The banner ends
   "…Double-click a text to open it in the C" — the sentence is cut mid-word at the
   viewport edge. The label *has* `setWordWrap(true)` (main.cpp:9589–9595), so the
   clipping means the pane's layout is being forced wider than the window (tree column
   minimums + the fixed-width "Recently opened" panel). Fix: constrain the right panel /
   tree so the pane fits the window, or put the banner in a width-bound container so the
   wrap actually engages. Verify at the selftest window size.
2. **"Recently opened" shows one raw path as an 8-line blue link.** The full relative
   path (`sungbum/Sungbum/TSAD MA_STUDY OF…/S05450M_…TXT`) wraps across the whole
   panel in underlined link style — unreadable, and the one thing a translator wants
   (the title) is buried. Built at main.cpp:10316. Fix: display the leaf title (filename
   sans catalog prefix, or the last two path segments), full path as tooltip; one line
   per entry, elided middle.
3. **Name column truncates while the Size column is dead weight.** Every visible row is
   a folder, so Size shows "--" for the whole column while titles like
   "'DUL BA_VOWED MORALI…" clip. Fix: shrink/auto-size Size, give the reclaimed width
   to Name (or drop Size in tree view — it only means something for files).
4. **Dead parchment below "Recently opened".** ~70% of the right panel is empty. Cheap
   win: extend recently-opened to a list of the last 5–10, or add a one-line hint
   ("Double-click any text to read it in the Overlay") so the panel teaches.

## 03-scans — CLEAN

Banner teaches, the four big actions each carry a caption, the gold status line
("document: … · no scan volume linked") states the precondition honestly. One optional
note: the lower ~60% of the pane is empty dark chrome; if a volume is linked this space
could show the link status / cached-page count, but this is enhancement, not defect.

## 06-manuscript — MINOR

1. **Editor placeholder clipped mid-sentence** (defect). Shows "…paste Tibetan script
   from the Overlay or Convert panes and it keeps its" and stops — the placeholder
   (main.cpp:13516–13520) is longer than one line and QPlainTextEdit paints it clipped.
   Fix: shorten to one line that fits ("The manuscript. Write here — Tibetan keeps its
   shape; the sidebar searches the corpus.") or paint a custom multi-line empty state.
2. **Corpus sidebar results area is blank parchment with no empty state.** The Gofer
   placeholder in the query box is excellent; the browser below "Find in corpus" should
   carry the round-1 empty-state treatment ("Results land here — click a hit to see the
   full segment; 'insert' places the master's English at your cursor.").
3. **Formatting buttons are cryptic.** The three tiny buttons after Export RTF render as
   bare glyphs (B/I/underscore at ~40px) with no visible labels; the bold glyph reads as
   "Ξ" at this size. Fix: proper QToolButtons with bold/italic/underline styled glyphs
   and tooltips, or a size bump.

## 08-review — MINOR

1. **Report area (bottom half) is featureless empty parchment.** The guard text ("both
   the source and the draft are…") only appears after clicking Run review
   (main.cpp:10556). Set teaching HTML at construction: "Flags appear here after Run
   review — register warnings, provisional-tier glosses, unmatched lines. Nothing is
   auto-corrected." Costs one line, converts the pane's biggest surface from dead space
   into instruction.
2. Otherwise clean: banner is honest about oversight semantics, both inputs carry
   placeholders, three-button toolbar is unambiguous.

## 10-search — NEEDS WORK

(The known "unwired search pane" — this audit confirms the UX side needs the same pass.)

1. **No banner.** Every other pane opens with "**Name** — what it's for." This one opens
   with "Enter Text to Go For:" (main.cpp:5953). A newcomer cannot tell this is the
   Gofer grammar (OR / NEAR-within-N-lines) or what corpus it hits. Add the standard
   banner: "**Search** — Gofer across the corpus and your library: fill any boxes, pick
   OR or NEAR, Find."
2. **Four unlabeled, placeholder-less text fields in a 2×2 grid.** Nothing says what the
   left vs right column means or that boxes combine via the OR/NEAR selector. Fix:
   placeholders ("term 1…", "term 2…") and a caption tying them to the operator row.
3. **"proximity: 0" has no units and no enable/disable logic shown.** Gofer proximity is
   lines. Fix: label "within N lines (NEAR)" and grey it out while OR is selected.
4. **Scope list is a huge black void below 4 rows** — the emptiness reads as broken.
   Size the list to contents and let the settings form breathe, or use the space for a
   grammar cheat-sheet (the round-1 empty-state pattern).
5. **Add / Remove / Duplicate / Save button row is ambiguous** — saved searches or
   scopes? Group them under the object they act on (gold eyebrow "SAVED SEARCHES" /
   "SCOPES").
6. Consistency nit: sub-tab "Search Setting" → "Search Settings"; the blue pill sub-tabs
   are a third tab style — acceptable if deliberate, but note it.

## 11-lookup — NEEDS WORK

1. **No banner.** The pane jumps straight to a button and a search box. Add: "**Lookup**
   — THL-style stacked dictionaries: type a headword (wylie, Tibetan, or ACIP) or browse
   in Tibetan order."
2. **Both result surfaces are empty with zero guidance.** The left browse list is a black
   void; the right parchment sheet is blank. This is the exact defect class fixed in
   round 1. Left: "Browse the dictionary (Tibetan order) fills this list — ka, kha, ga…".
   Right: "Definitions stack here, every source labeled — HGM's gloss is binding;
   everything else is reference." That last sentence is core product doctrine and
   currently appears nowhere on the pane.
3. **~90% of the pane is dead space at first open** — consequence of 1–2; the empty
   states cure it.

## 12-sanskrit — MINOR

1. **Result surface (≈85% of the pane) is blank parchment with no empty state.** The
   banner and the input placeholder are good; the sheet below should preview the
   deliverable: "Analyze renders every notation — IAST · Devanagari · ACIP input-code ·
   next-letter · EWTS · Tibetan — plus Whitney's root and the Mahāvyutpatti bridge."
2. Otherwise clean; banner is one of the best in the app.

## 13-convert — MINOR

1. **No pane banner.** The pane opens with the DATES eyebrow (main.cpp:6680); there is
   no "**Convert** — …" line establishing the whole pane (wylie/ACIP/unicode + Sanskrit
   + dates). One line above DATES restores the app-wide pattern.
2. **Output sheet is unlabeled.** Input has a bold label (main.cpp:6911); the bottom
   parchment area has neither a label nor an empty-state hint, so first glance can't
   tell it's the converted output (or that conversion is live). Add eyebrow "CONVERTED —
   EVERY NOTATION" or an empty-state line.
3. Consistency: "Input" uses plain bold while DATES uses the gold eyebrow — promote the
   converter section header to the same eyebrow style.

## 14-analysis — NEEDS WORK

1. **No banner.** Goes straight to "Passage (ACIP)" (main.cpp:1128). A newcomer can't
   tell what Analyze produces or that it calls Claude. Add: "**Analysis** — send a
   passage (and optionally your draft) to the assistant for a structured read:
   word-by-word, grammar, and QC flags."
2. **The two right-hand browsers (report_, qc_) are unlabeled blank parchment** —
   two-thirds of the pane, and nothing distinguishes the big panel from the strip below
   it. Add headers ("REPORT", "QC FLAGS" in gold eyebrow style) and empty-state HTML in
   each.
3. Good: the disabled Analyze button sits directly above the ANTHROPIC_API_KEY
   explanation (main.cpp:1160+) — the guard teaches. Keep.

## 16-drills — MINOR

1. **Drill surface is blank parchment with no empty state**, even though the pane knows
   the deck state (85 due). It should say "Click New drill — 85 words due. Chunk-order:
   rebuild the master's line from shuffled chunks." First glance currently teaches
   nothing about what a drill looks like.
2. **Bottom feedback sheet is a second blank parchment** — label it or collapse it until
   a drill is checked. Two indistinguishable empty sheets read as a rendering bug.
3. Stats line ("deck: 85 words (85 due) · drills: 12…") is in the dim chrome gray —
   verify it uses the fixed #8A8A8A, not the old low-contrast value; it is the dimmest
   text on the pane and it carries real data.

## 17-input — NEEDS WORK

1. **Toolbar clips at least six control labels** (worst defect in this audit round).
   One QHBoxLayout (main.cpp:11239–11323) holds 12+ controls; at the selftest window
   width they render as ")pen scan.", "n scan folc", "Predictive ty", "ect lines (C",
   "ə next folic", "ə with partı". Input-center typists are the least expert users of
   the whole suite and get the most garbled chrome. Fix: split into two rows (file/nav
   row: Open scan…, Open scan folder…, ◀ page, page ▶, zoom, Save…; typing row:
   Predictive typing, scan follows cursor, Detect lines (OCR), Pre-fill from OCR,
   @ next folio, Compare with partner file…), or use a QToolBar with overflow. No
   control label may elide.
2. Everything else is right: banner credits the ACE design and teaches the double-keying
   loop; scan strip and editor both carry good empty states/placeholders.

## 18-ocr — MINOR

1. **Deskew checkbox label clipped** (defect): renders "override deskew to 0° (DEVIATION
   from" and stops (main.cpp:12123). Fix: short label ("override deskew to 0°") with the
   full DEVIATION-from-BDRC-pipeline rationale as a tooltip — same crowded-toolbar cause
   as 17-input.
2. **Output sheet (bottom ~70%) has no empty state.** Add: "OCR text lands here, one
   line per detected band — review material only, never corpus-bound." (Reinforces the
   orange warning, which is excellent.)
3. Good: the CC BY-NC / OCR-DERIVED warning in the banner is exactly the honest-tiering
   voice the app promises.

## 19-propose — MINOR

1. **All three field placeholders are elided** ("the Tibetan (ACIP …", "proposed English
   …", "optional: ordinary …") — the teaching text is cut before it teaches (defect by
   principle 5). The QFormLayout centers a narrow column in a 2360px pane. Fix: let the
   form stretch (fields ≥ 480px) — the pane has enormous unused width.
2. **The collapsed identity box renders as an empty rounded frame.** When configured,
   the checkable QGroupBox hides its children (main.cpp:12788–12801) but keeps its
   frame + layout margins — a mysterious empty capsule under the "Filing as Adam" title.
   Fix: also set a small maximumHeight when folded (title-only), restore on unfold.
3. **Bottom half of the pane is dead chrome.** "My proposals — what happened?" is a
   natural thing to open by default when it has content — the filer's minute is spent
   wondering whether past proposals landed.

## 20-approval — MINOR

1. **Banner clipped at the right edge** (defect): ends "…Dictionary/corpus" and the rest
   of the sentence — including the doctrinally important "the app never edits the corpus
   itself" — is invisible. Cause: this QLabel never gets `setWordWrap(true)`
   (main.cpp:13023–13029), unlike the Library/Propose banners. One-line fix.
2. **205 near-identical machine proposals, one click each.** The queue header counts
   "205 pronunciation", all "proposed by prenasal derivation engine (machine)" with the
   same rationale, and the only affordances are per-row Approve/Decline/Defer. That is
   205 clicks of the authority's minute. High-value: "Approve all from this engine run…"
   (with count + confirm), or at least select-multiple. Keep per-row for human
   proposals.
3. Polish: the kinds summary line ends with a dangling separator ("…205 pronunciation ·");
   drop the trailing dot. Row rationale text is long and repeated verbatim per row —
   consider dimming the boilerplate and bolding only the per-row deltas (baseline says X)
   so the eye finds what differs.
4. Good: green kind headers on parchment, tier/provenance/date on every row, and the
   applies-in-app tag — the honest-provenance voice done right.

---

## Cross-pane summary

| Pane | Verdict |
|---|---|
| 02-library | NEEDS WORK |
| 03-scans | CLEAN |
| 06-manuscript | MINOR |
| 08-review | MINOR |
| 10-search | NEEDS WORK |
| 11-lookup | NEEDS WORK |
| 12-sanskrit | MINOR |
| 13-convert | MINOR |
| 14-analysis | NEEDS WORK |
| 16-drills | MINOR |
| 17-input | NEEDS WORK |
| 18-ocr | MINOR |
| 19-propose | MINOR |
| 20-approval | MINOR |

### Top 5 fixes by value

1. **17-input: un-clip the toolbar** — six garbled button labels on the pane aimed at
   the least-expert users; split into two rows.
2. **20-approval: bulk-approve for machine engine runs** — 205 identical-provenance
   items at one click each is the single biggest translator's-minute leak found; plus
   the one-line `setWordWrap` banner fix.
3. **Empty-state teaching HTML across all blank result surfaces** (11-lookup both
   panels, 14-analysis both browsers, 08-review report, 12-sanskrit sheet, 06-manuscript
   sidebar, 16-drills, 18-ocr output) — one fix class, eight panes stop looking broken
   at first open and start teaching.
4. **10-search: give the Gofer pane a face** — banner, field placeholders, proximity
   units, grouped buttons; currently the app's least self-explaining pane.
5. **Missing banners on 11-lookup, 13-convert, 14-analysis** (and the clipped banners on
   02-library / 20-approval) — restore the app-wide "**Name** — purpose" contract on
   every pane.


## CLOSE-OUT (2026-08-14 verification pass)

Re-verified against fresh --screenshots captures: the Input
toolbar two-row split, OCR deskew label, Search banner/
placeholders/NEAR label, Lookup/Convert/Analysis banners,
Manuscript B/I/U, Library recents, and the empty-state round are
ALL LANDED (fixed in the post-audit rounds; several "never
landed" notes in the session log were stale). Completed today:
the Propose form's Proposed/Secondary fields now carry the same
360px minimum as the Tibetan field (the audit's half-applied
fix). This audit round is closed; future UI findings go to the
attack board.
