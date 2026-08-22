# PART ONE — READ & TRANSLATE

*The Diamond Cutter Translation Tool User Manual, Part 1 of 3. Asian Legacy
Library · Asian Classics Input Project.*

This part walks you through the two READ panes (Overlay, Library)
and the four TRANSLATE panes (Manuscript, Draft, Review, Align),
control by control, for a reader who has never opened the app.

**How paths are written.** The window's top row holds six workflow
tabs; inside each, a second row holds its panes. "Read → Overlay"
means: click the **Read** tab in the top row, then the **Overlay**
tab in the row beneath it. The menu bar mirrors the window exactly —
one menu per workflow, one submenu per pane — so every button in
this manual is also reachable from the menus, from anywhere. On a
Mac, open the Help menu and type a control's name into the search
box: the system finds the menu item and points an arrow at it.

**The three rules you will see restated everywhere.** (1) Geshe
Michael's English is the binding layer; all other scholarship is
clearly-labeled reference. (2) The tool never guesses — failures
are flagged, provisional material LOOKS provisional (amber-tinted),
machine-located material says so. (3) Nothing enters the official
data without the authority's approval.


## Overlay

*Where: Read → Overlay. The flagship reader: a Tibetan text with
the dictionary underneath.*

### The layout, top to bottom

- **Top left**: a SMALL editable box labeled **Document (ACIP)** —
  this is the only place you can paste or type.
- **Left column** below it: the buttons and the Display panel,
  organized under three small gold headings — **REVIEW**, **SCANS**,
  and **FORMAT & EXPORT** — plus the **Display** toggles. If your
  window is short, this column scrolls; it never squeezes.
- **Right**: the BIG read-only display where the text renders. You
  cannot type or paste there — it is for reading and clicking.
- **Below the display**: the entry CARD — the dictionary panel that
  fills in when you click a word.

**The document box is yours to size.** Drag the divider directly
beneath the small Document (ACIP) box: down for a bigger pasting
surface, up for more reading room. The height you choose is
remembered.

### Loading a text — three ways

1. **From the Library (easiest).**
   1. Click Read → Library.
   2. Double-click any text — for example **S0134**.
   3. The app switches to the Overlay for you and the text opens.
2. **Paste.**
   1. Click into the small Document (ACIP) box at top-left.
   2. Paste ACIP or wylie — the app auto-detects which; you never
      declare it.
   3. Press **Load into overlay** (left column, right under the
      box). The text appears in the big display. A full canonical
      volume opens in about two seconds.
3. **Open a file.** Press **Open ACIP file…** (left column, first
   button) and pick a .txt/.acip/.act/.inc file. If the filename is
   a recognized ACIP catalog name, the top of the card area shows
   the decoded identity — collection, text number, part, language,
   verification status — read from the filename itself.

*What you should see:* the text renders CLEAN — no shading at all.
That is deliberate (next section). If you opened a file from an
`ocr_out` folder, an extra warning line appears first: "⚠
OCR-DERIVED — unverified review material", with a count of how
many syllables fail the legality check. OCR output is never
trusted as text.

### Reading: click-to-longest-phrase (Geshe Michael's directive)

Per Geshe Michael's direction, the page opens clean and the
dictionary answers only when asked:

1. Click any word in the big display.
2. The **longest phrase the dictionary knows at that spot** lights
   up in gold. If the entry is provisional (machine-aligned) tier,
   the gold leans amber — that tint is a warning, not decoration.
3. Click the same word again: the highlight **steps down to the
   next shorter phrase inside** — dropping a particle at a time —
   until only the single word is lit. Click once more and the
   chain starts over at the longest phrase.
4. The card below follows every step of the chain.

*Worked example:* open **S0134** from the Library, click **sems
can** ("sentient being"). If the text has a longer known phrase at
that spot (say *sems can thams cad*, "all sentient beings"), THAT
lights up first; a second click narrows to *sems can*; the card
updates each time. Above the card a small grey **nesting**
breadcrumb lists the whole chain — e.g. `sems can thams cad ‹ sems
can` — with the current step in bold, and "(click again to step
down to the shorter phrase)".

Double-clicking is safe: rapid clicks walk the phrase chain — the
app swallows the usual double-click word-selection so the whole
line never lights up by accident.

**Marks that appear in every mode:**

- **Red wavy underline** = that syllable fails the classical
  syllable-legality rules — a spelling doubt (see REVIEW below).
- **Dotted amber underline** = a grammatical particle disagreeing
  with the preceding suffix per the classical sandhi tables — a
  review hint, never auto-corrected.

The display follows the source file's own line breaks and blank
lines — the text is never re-flowed.

### The Display panel (left column, under "Display")

Every choice here is remembered across sessions.

- **text as** (drop-down): **Tibetan script / ACIP / Wylie** — the
  same text, the same shading, three scripts. The default is ACIP
  (the input centers' native script); switch to Tibetan script to
  read in native letters. *Try it:* with S0134 open, set text as →
  Tibetan script and watch the whole display re-render in dbu-can.
- **Typeface picker** (next to "text as"): the vetted Tibetan
  faces found on your machine — SambhotaDege, Noto Serif Tibetan,
  Jomolhari, Monlam-Bodyig, TibetanMachineUnicode, BabelStone
  Tibetan Slim and others, plus "system". Applies in Tibetan-script
  mode; the choice is saved by family name.
- **shading** (drop-down), two modes:
  1. **on click (longest phrase first)** — the default described
     above: clean page, gold on demand.
  2. **all matches (wash)** — the coverage view: every known word
     and phrase washed in color at once, adjacent phrases in
     different hues, nested phrases inside longer ones, and
     provisional glosses amber-tinted everywhere. Use it to see at
     a glance how much of a text the dictionary covers. Clicking
     still works — the active span shows in violet.

**The nine reference-layer toggles**, each a checkbox you can turn
on or off (defaults in parentheses):

| Toggle | What it changes |
|---|---|
| **phonetics** (on) | the GMR-convention pronunciation on the card |
| **HGM definitions** (on) | Geshe Michael's English equivalents on the card |
| **corpus usage (contextual)** (on) | his actual corpus renderings, course by course |
| **grammar marks & particle notes** (on) | particle roles, suffix-agreement lines, fused-particle notes on the card AND the dotted-amber marks in the document |
| **Sanskrit reference** (off) | Sanskrit equivalences (Mahāvyutpatti) on the card |
| **Hopkins reference** (off) | the Hopkins reference layer on the card |
| **reference dictionaries (LC/TD/THL)** (on) | the Lokesh Chandra / TibetanDictionary / THL reference stacks on the card |
| **Botok segmentation (reference)** (off) | an independent segmenter's reading of the clicked spot, labeled reference-only |
| **unattested-word hints (segmenter + Monlam)** (off) | hints in the document for words no list attests |

Card-only toggles re-render the **current card instantly, in
place** — your position in the phrase chain is kept. Only the two
document-affecting toggles (grammar marks, unattested-word hints)
rebuild the display — and they restore your card afterward.

Below the toggles a hint line reminds you: "Click a shaded word to
see its context; click again to cycle."

### The card — what you get when you click

The panel below the display shows the entry for the current step
of the phrase chain. Reading it top to bottom:

- The **nesting breadcrumb** (described above).
- **Your per-text glossary line** (see below) with its edit link
  and the "propose to the authority…" link.
- **The quality tier** — curated / glossary / provisional. Curated
  and glossary tiers are hand-anchored; provisional is
  machine-aligned and looks provisional (amber) wherever it
  appears.
- **Geshe Michael's English** (the binding layer) and, with
  "corpus usage" on, his **actual renderings course by course** —
  the concordance of how HE rendered this word across his corpus.
- **Reference definitions**, each clearly labeled as reference —
  never presented as HGM material.
- **Sanskrit equivalences** (Mahāvyutpatti bridge), if toggled on.
- **Honorific badge** with the ordinary counterpart beside it,
  when the term is register-marked.
- **Community pronunciations** ("also heard"), each with its
  class.
- **Monlam word-list attestation** — a labeled reference line
  telling you the form is a real word in the Monlam lists. On a
  word the Geshe Michael Roach Dictionary does NOT know, this is how the card
  distinguishes "a real word awaiting an entry" from "a probable
  typo".
- **Verb tense paradigms**, where the dictionary has tense
  evidence.
- **Expansions of abbreviations**: orthographic abbreviations
  (bskungs yig, from the TibSchol/rKTs table, CC0 — "reference;
  not a typo") and bsdus-tshig contractions with their shared
  gloss — both labeled with their derived register.
- **Published footnotes** that deal with this term.
- **"Geshe Michael teaching this term"** — timecoded links that
  open his recorded classes at the moment he teaches the term.
  These are machine-located from class captions: candidates. **The
  recording is the authority**, always; non-English sessions are
  tagged so you are not surprised.
- **"He says this word"** — a second tier locating moments where
  he speaks the Tibetan itself, matched phonetically on his own
  convention. Candidates; homophones share moments — the card says
  so.
- **"Das 1902 · ~p.N (reference)"** — a link that opens Sarat
  Chandra Das's 1902 dictionary INSIDE the app at approximately
  the entry's page (the "~" is honest: page location is
  approximate). Public domain, clearly a reference, never HGM
  material.
- **Links out** to 84000, BDRC, and Lotsawa House, which open in
  your browser.

**Clicking a word with no dictionary span** still teaches: if it
is a freestanding grammatical particle, the card names its
function and paradigm (Wilson rule tables); otherwise it says "no
dictionary span here" plus any Monlam attestation or abbreviation
expansion it can offer — never a guessed gloss.

### Your per-text glossary

Each text you open from a file gets its own glossary — the
translator's own working equivalents, binding **for this text's
display only** and outranking the general dictionary on the card.

1. Click a word. On the card, click **＋ add to this text's
   glossary** (small grey link at the top of the card).
2. Type your gloss and confirm. *What you see:* the card now opens
   with a green box — "FROM THIS TEXT'S GLOSSARY: your gloss (the
   translator's own — outranks the general dictionary here)" —
   every time you click that word in this text.
3. Click **edit** on the green box to change or blank it. The
   glossary saves beside your library as a plain
   wylie-TAB-gloss file, human-editable.

### Propose from the card

Next to the glossary link on every card: **propose to the
authority…**.

1. Click it. A dialog asks what you are proposing for the term:
   Honorific term (↔ ordinary) · HIGH honorific marking ·
   Humilific (self-lowering) form · DOUBLE honorific (subject +
   recipient) · Pronunciation exception · Abbreviation /
   contraction candidate · Rendering for this word (→ dictionary)
   · Idiom / fixed expression · Note about a passage.
2. Enter the proposed value (the ordinary counterpart for an
   honorific, the pronunciation for an exception, the English for
   a rendering).
3. The **passage around your cursor is auto-attached as
   evidence**, tagged with the source filename. The proposal files
   into the authority's Approval queue (Community → Approval);
   track it later in Community → Propose → "My proposals — what
   happened?".

### REVIEW (left column, under the gold REVIEW heading)

**Show spelling doubts** (checkbox). Turn it on and a small docked
list opens under the checkbox: every syllable in the text that
fails the classical legality rules — the same doubts the red waves
mark — gathered as a working list, one row per distinct form, with
columns **Form**, **×** (its count), and **Wylie**.

1. Click a row → the text jumps to that syllable, selected.
2. Click the same row again → the next occurrence, cycling through
   all of them.
3. Most doubts are input-operator typos. Tick the checkboxes on
   the rows you want ruled on — or press **Check all** to tick
   every row at once.
4. Press **File for ruling**. Each checked form goes into the
   authority's Approval queue as a potential input error, with its
   context attached as evidence.

What the rulings mean: when the authority **APPROVES** a flag, it
is a confirmed input error, exported for the input teams — the app
never edits the texts itself. When the authority **DECLINES** one,
the form has been ruled VALID, and the app stops doubting it —
everywhere, for the whole team, from the next launch on.

**Teachings for this text…** (button). Press it with a document
loaded and the app sweeps every term the text contains and reports
where Geshe Michael taught each one — a dialog titled "Teachings
for this text — machine-located; the recordings are the
authority", listing timecoded class links per term, both by his
English and by the spoken Tibetan. This is the translator's
companion sheet: print-worthy preparation for translating the
text. Terms with no located moments say so honestly — the index
grows as more classes are indexed.

### SCANS (left column, under the gold SCANS heading)

This is the one Overlay feature that touches the network (BDRC's
public image servers).

**Follow along in scans (BDRC)** — the main button. Its label
depends on what the app knows:

- If the open text's catalog identity maps to a known BDRC scan
  volume, the button reads **Follow along in scans (bdr:…)**.
  Press it: the original woodblock page images open UNDER the
  text, and the pages turn following the @folio marker at your
  cursor — move your cursor through the text and the carving
  keeps pace. The current line is banded on the image, its
  placement honestly approximate. Folio-to-image mapping comes
  from BDRC's own IIIF manifest labels — nothing is guessed.
- If no catalog mapping exists, the button reads **Find scans on
  BDRC (title search)…** and opens a dialog instead:
  1. The query box is pre-filled with the text's own opening line
     (converted to wylie). Edit it if needed.
  2. Press **Search BDRC**. You see "searching…", then a list of
     matching works, each as `title [bdr:ID]`. If the network is
     down you see "BDRC unreachable: …"; if nothing matches, "no
     instances matched — edit the title and search again".
  3. Select the right edition and press **Use selected (saves the
     link)**. The match is YOURS to confirm — it is never asserted
     automatically — and the link is remembered for this text, so
     next time the button goes straight to Follow along.

**◀ folio / folio ▶** — page through the scans by hand, one folio
side at a time.

**locate word (OCR)** — a toggle button beside the folio arrows
(present when the OCR component is installed). Turn it on and the
app runs recognition on the current folio image and draws a box
around **the very word under your cursor, on the woodblock
itself**. Recognition is used only to LOCATE — never as text.

**Illustration gallery (cached scans)** — searches this volume's
already-downloaded pages for woodblock miniatures and diagrams and
shows the crops as a gallery. Candidates only, machine-found,
never claimed complete — and only pages you have already viewed
are searched (pages cache as you view them in Follow along). If no
volume is linked yet, the app tells you to link one first (Follow
along / Search BDRC).

*Worked example:* Read → Library → double-click **KD0016** (the
Diamond Cutter). Press **Follow along in scans (BDRC)** — the
woodblock pages open beneath the text. Click into different parts
of the document box and watch the folio image turn to follow your
cursor. Press **locate word (OCR)** and the word at your cursor is
boxed on the carving.

### FORMAT & EXPORT (left column, under the gold heading)

- **Prepare for translation (Mixed Nuts)…** — runs Geshe Michael's
  demonstrated translation-preparation format on the loaded text:
  paragraphs broken at double shads, input-operator bracket notes
  pulled out folio-tagged into a NOTES section. You choose where
  to save the .txt; the card area reports what was done ("N
  paragraphs at double shads, N bracket note(s) folio-tagged").
- **Export print Tibetan (Unicode)…** — converts the whole
  document to print-ready Tibetan script and saves it where you
  choose. The report counts the syllables converted and how many
  were **flagged ⟨…⟩ (never guessed)** — any syllable the
  converter could not prove is wrapped in visible markers instead
  of being silently approximated, and the first few are listed.

### Resuming work

The Overlay restores your session on every normal launch: same
file, same scroll position, same cursor. Quit mid-sentence and the
app reopens exactly there.


## Library

*Where: Read → Library. The preserved canon on your own disk —
2,716 texts and 8.68 million lines of ACIP Release 6, plus your
own materials.*

### The layout

- **Top row of buttons**: Install collection ZIP… · Import my
  materials… · List view · Send to OCR… · Legacy font rescue
  (UTFC)… · Update search index, then the **find in library by
  name…** search box.
- **Second row**: three filter drop-downs — collection,
  verification status, language.
- **Left**: the folder tree (or the flat table when List view is
  on).
- **Right**: the info panel — details about whatever you select,
  or your recently-opened list when nothing is selected.

### Browsing and opening

1. Expand folders in the tree. **The tree remembers which folders
   you had open, across restarts.**
2. Click any file once → the info panel on the right fills in (see
   below).
3. **Double-click a text → it opens in the Overlay** — the app
   switches tabs for you. *Try it:* double-click **S0134** and you
   land in Read → Overlay with the text loaded clean.

**List view** (toggle button): switches the tree to a flat,
sortable catalog table of every file — columns **File ·
Collection · No. · English title (catalog) · Verification ·
Language · KB**. Every filename is decoded into its catalog
identity, with English titles from the Project's published
catalogs. Click any column header to sort; double-click a row to
open it in the Overlay. Press the button again to return to the
tree.

**The three filters** narrow what browsing and name-search show:
- **collection**: any collection / Kangyur / Tengyur / Sungbum /
  Reference / Graphics.
- **status**: any status / high (L/M/N/F) / mid (C/D/E) / raw
  (A/B) / incomplete — the ACIP verification levels decoded from
  the filenames.
- **language**: any language / Tibetan / English / Sanskrit /
  mixed.

**The search box**: type part of a filename (e.g. `KD0016`) and
press Enter — matches list in the info panel, each a click from
opening. With filters set and the box empty, Enter gives a
filtered browse instead. Clear everything and the panel returns to
**Recently opened** — your last ten texts, each a link.

### The info panel — what selecting a text shows

- Filename, size, and date; the **catalog English title** and the
  **bilingual subject classifications** (Sungbum catalog and ACIP
  Release 6), each labeled with its source.
- **The AUTHOR**, with dates, and links to the person's **BDRC
  record** and, where one exists, their **Treasury of Lives
  biography**. Where the name is a homonym, ALL candidates are
  listed for you to disambiguate — "N candidates — homonyms, you
  disambiguate" — never auto-merged.
- **Their works in your Library** — the same author's other texts
  on your disk, each a click from opening in the Overlay.
- The decoded ACIP identity (collection, text number, part,
  verification status, language, INCOMPLETE where marked) and,
  for catalog texts, a **"view the original scans on BDRC"** link.
- A short text preview, and the reminder: double-click to open in
  the Overlay.
- Files from `ocr_out` carry the amber **ocr-derived** warning —
  unverified review material.

### The maintenance buttons

- **Install collection ZIP…** — installs the canon. Download the
  Kangyur / Tengyur / Sungbum ZIPs from
  asianlegacylibrary.org/library (the link is right in the pane's
  header), press the button, pick the ZIP. The info panel shows
  "unpacking…", then "Installed. N file(s) now under library/…".
- **Import my materials…** — brings your own texts into
  `library/my_materials` (.docx, .txt, .acip, .act, .md, .rtf;
  Word documents convert automatically).
- **Send to OCR…** — routes a scan image to the Input → Scan pane
  for recognition.
- **Legacy font rescue (UTFC)…** — converts documents typed in
  pre-Unicode Tibetan fonts of the 1990s (TibetanMachineWeb,
  LTibetan, Sambhota…) to proper Unicode Tibetan, via the
  Universal Tibetan Font Converter run as a separate external
  tool. The result is labeled **utfc-derived** — machine-converted,
  review before treating as canonical — and if no Tibetan tsheg
  appears in the output the app warns you the source encoding was
  probably wrong (wrong guesses produce garbage, not silent
  errors).
- **Update search index** — refreshes the full-text index that
  Research → Search's "search the Library" uses. The report counts
  files added / updated / removed / unchanged; a full build
  indexes all 8.68 million lines in under two minutes, and after
  it the Search pane answers from the index instantly.


## Manuscript

*Where: Translate → Manuscript. The writing surface — the
manuscript itself, with the corpus one keystroke away.*

### The layout

A toolbar across the top; the big editor filling the left; the
Gofer search sidebar on the right; a small grey status line at the
bottom ("No file — Save as… to name the manuscript", then the
filename and last-saved time).

### Writing and saving

1. Write in the big editor. Pasted Tibetan script keeps its shape.
2. **B / I / U** (toolbar) — bold, italic, underline; each applies
   to the current selection. The **size spinner** beside them
   (9–48) sets the font size of the selection.
3. **Save as…** names the manuscript — stored losslessly as
   .html. From then on:
   - it **autosaves every minute** whenever there are unsaved
     changes;
   - **Save** is always there for an immediate save (the status
     line shows "filename — saved HH:MM");
   - the pane **reopens your last manuscript on launch**.
4. **Open manuscript…** opens a .html manuscript saved from this
   pane.
5. **Export RTF…** produces a file for Word or Pages (via the
   Mac's own converter).

### The Gofer search sidebar

The **Search sidebar** checkbox (toolbar, right end) shows or
hides it; it is on by default.

1. Type a query into the box. It speaks the **full Gofer
   grammar** — the same grammar as Research → Search:
   - `bden pa` — a plain term;
   - `stong OR bden` — either term;
   - `"sdug bsngal" NEAR/5 "bden pa"` — both terms within five
     lines of each other.
2. Press **Find in corpus** (or Enter). The search runs over the
   aligned corpus — Geshe Michael's translations beside the
   Tibetan.
3. *What you see:* a count of windows, then each hit: the course
   it came from in italics, the Tibetan line, and **the master's
   English in bold** beneath it, each with an **insert** link.
4. Click **insert** — that English is placed at your cursor in the
   manuscript. **Matched from his corpus, never composed**: the
   sidebar can only give you sentences he actually wrote.
5. If a query is malformed, the sidebar says "Query not
   understood" and why — it never silently searches something
   else.

*Worked example:* type `bden pa` ("truth") and press Find in
corpus. Hits arrive with his English bold beside each Tibetan
line. Click **insert** under the rendering you want and it drops
into your manuscript at the cursor.


## Draft

*Where: Translate → Draft. The working translator's bench: source
above, your English below, evidence tools on both.*

### The layout

Two levels split by a draggable divider:

- **Top, three columns**: (1) the source box with the Load button
  and the STRUCTURE and EVIDENCE tool buttons under their gold
  headings (this column scrolls if the window is short); (2) the
  clause list; (3) the anchors panel.
- **Bottom, two columns**: (1) your English draft box with **Check
  terminology**, the apparatus search box, and the PUBLISH tools
  under their gold heading; (2) the report panel where results
  render.

### Loading and working clause by clause

1. Paste the Tibetan source (ACIP or wylie — auto-detected) into
   the source box.
2. Press **Load source**. The text splits into clauses in the
   middle list.
3. Click a clause. Its **ANCHORS** appear on the right: the clause
   in Tibetan script with each established term banded in a color,
   and below it each term's English chipped in the SAME color —
   which English goes with which Tibetan reads by color. Only
   established equivalents appear; the pane never writes English
   for you.
4. Click any term in the anchors panel for its **full corpus
   concordance** — every corpus segment containing it, with Geshe
   Michael's English beside each. A "back" link returns to the
   clause's anchors.
5. On an anchor's **technical spelling** link, a click inserts the
   apparatus form at your draft cursor — the pronunciation
   spelling plus "(technical spelling: …)" — the published house
   convention (STD-002), typed for you.

*Worked example:* paste `/ /blo sbyong snyan brgyud chen mo'i
'khrid yig /sems can thams cad bde ba dang ldan par gyur cig /`,
press Load source, click the second clause. The anchors panel
shows *sems can* banded in one hue with its English chip in the
same hue. Click *sems can* to see its corpus concordance.

### Phrase memory — before you invent any rendering

1. Select any Tibetan in the source box.
2. Press **Phrase memory** (under the EVIDENCE heading).
3. *What you see:* every prior rendering of those words — segments
   from **his corpus (binding)**, plus any matches from the Align
   pane's harvest, which are translator-authored and **labeled
   PENDING**. The beginnings of a Project translation memory,
   built only from attested work.

The working translator's habit: Phrase memory before inventing —
Geshe Michael may already have a rendering.

### STRUCTURE tools (gold heading, source column)

- **Extract outline (sa bcad)** — pulls the text's own topical
  outline from its internal markers into the report panel.
- **Structural units (bam po / le'u)** — finds the canonical
  divisions: explicit BAM PO and numbered LE'U (chapter) markers
  written in the text are reported as authoritative; alongside
  them a syllable-derived shloka/bampo reckoning is given and
  **labeled as the estimate it is** (30-syllable prose shloka,
  300-shloka bampo).
- **Verse meter** — names the meter of verse passages and flags
  broken lines.
- **Verse reading order** — stanza-by-stanza reading guidance with
  the verb named. Verse does not read line by line: the meter
  forces the poet to displace words and drop understood particles,
  so this tool groups lines into stanzas (four verse lines, or the
  poet's own double-shad closes) and applies the verb-first
  reading order to each stanza as a whole.

### EVIDENCE tools (gold heading)

- **Detect quotations** — finds passages of the source that
  exactly match corpus segments (7+ syllables): **attested
  quotations, never inferred**. Each match shows the published
  rendering of those very lines, with citation, and matched works
  recommend their published bibliography entries — reuse, don't
  retranslate.
- **Check terminology** (under your draft box) — audits your
  English draft against the source: established equivalents that
  never appear in your draft; one Tibetan term rendered two
  different ways; renderings resting on PROVISIONAL-tier glosses.
  Everything is **flagged for review, never auto-corrected**.
- **The apparatus search box** (below Check terminology): type a
  word and press Enter to search the shared published apparatus —
  all released footnotes and bibliography entries ("GMR: reuse
  released work"). Click a result to insert it.

### PUBLISH tools (gold heading, under your draft)

- **Propose footnote (pending GMR approval)** — select draft text
  and press: it is saved as a CANDIDATE note into the pending
  queue. There is deliberately **no approve button in the
  software** — the official apparatus only ever contains
  published, GMR-approved notes; approval belongs to the
  publishing process.
- **Compose bibliography entry…** — assembles an entry in the
  exact published house format (STD-007). For 1,846 known works,
  entering the ACIP number fills the fields from the Project's
  catalog. Fields are used exactly as typed — enter technical
  spelling as it should print.
- **Export draft (RTF, \*italics\*)…** — writes your draft as RTF
  for the publishing workflow. Text between \*asterisks\* becomes
  italic (STD-004: name-parts woven into a verse are italicized in
  the English).
- **AI back-check (API, labeled AI)** — one of the app's two
  network features. Produces a coverage report only — omissions
  and additions between source and draft — and is **forbidden by
  construction to retranslate**. Output is banner-labeled AI. The
  button is disabled (with a tooltip saying why) unless an
  ANTHROPIC_API_KEY is configured.


## Review

*Where: Translate → Review. The overseer's bench — a senior
translator checking someone else's finished draft.*

### Running a review

1. Put the Tibetan source in the LEFT box: paste it, or press
   **Open source…** and pick the file.
2. Put the finished English draft in the RIGHT box: paste, or
   **Open draft…**.
3. Press **Run review**. (Both sides are required — with one
   missing, the report simply says so.)
4. The report renders below. Every flag is **advisory only —
   never a verdict**; nothing is auto-corrected. The report
   guides; you decide.

### Reading each advisory type

- **Register-sensitive terms** — "established senses differ by
  register", listing the senses: confirm the rendering fits THIS
  text's register. *Example:* a source containing *bsod nams*
  raises it, because Geshe Michael renders it "goodness" in
  prayers but "merit / good deeds" in prose — the app never
  auto-resolves which.
- **Provisional-gloss renderings** — a draft rendering that rests
  on a PROVISIONAL-tier (machine-aligned) equivalent is flagged in
  red-lettered honesty: "this equivalent is PROVISIONAL tier;
  verify against the corpus."
- **Established terms absent from the draft** — an established
  equivalent that never appears in the English. Possibly
  deliberate — worth a look. *Example:* source `SEMS CAN THAMS
  CAD` with the draft "every single one of them" surfaces *sems
  can*, whose established "sentient being" is missing.
- **One English word serving two Tibetan terms** — a collision
  that can flatten a distinction the Tibetan is making.
- **Terms with three or more established senses** — wide sense
  spread; check the one chosen.
- **Honorific terms in the source** — each honorific found is
  listed with its level (honorific / **HIGH honorific** /
  humilific / double honorific), its ordinary counterpart, and
  the advisory: "check that the English carries the register of
  respect." *Example:* source `GZIGS PA` (honorific "to see")
  with draft "he saw it" raises it.
- **Published footnotes that apply** — released notes whose
  lemmas appear in your draft, with their numbers and sources:
  reuse, don't rewrite.

A good habit: walk the advisories with Read → Overlay open beside
you for any passage you want to see in context.


## Align

*Where: Translate → Align. Building tomorrow's dictionary — the
Project's original Hypercontext authoring workflow, reborn. You
align Tibetan with its English translation and harvest
translator-authored word pairs.*

### The layout

A button row on top; below it, the Tibetan text on the left and
the English translation on the right; at the bottom, the links
list showing every link made so far.

### Loading the two sides

1. Paste the Tibetan (ACIP or wylie — auto-detected) into the left
   box and the English translation into the right box, then press
   **Load pasted texts**. Or press **Open ACIP file…** to load the
   Tibetan side from your library, then paste the English and
   Load.
2. On Load, both sides become read-only click surfaces, and any
   links previously saved for this text reload automatically —
   **links save per text** (as a plain .tsv beside your library),
   so you can stop and resume across sessions.

### Making links

1. **Click a Tibetan word.** The word-finder selects a span in
   purple. **Click the same word again to cycle** through the
   possible word lengths the dictionary knows at that spot —
   longer phrase, shorter phrase, down to the bare syllable — the
   same phrase-chain idea as the Overlay.
2. **Select its English counterpart** in the right box (drag
   across the words).
3. Press **Link (space)** — the button, or just the space bar.
4. *What you see:* both spans tint blue, the pair appears in the
   links list at the bottom, and the link is saved immediately.
5. **Finer sublinks nest inside longer links in their own color**:
   link a whole phrase first, then link a word inside it to its
   English word — the nested link shows green inside the blue.
6. **Delete last link** undoes the most recent link. In the links
   list, each row also carries its own delete link, so you can
   remove any link, not just the last.

*Worked example:* Tibetan `sems can thams cad bde ba dang ldan par
gyur cig`, English "May all sentient beings be endowed with
happiness." Click *sems* — the finder offers *sems can*; select
"sentient beings" on the right; press space. Both phrases tint
blue and the pair `sems can = sentient beings` appears in the
links list. Now click *thams* for *thams cad*, select "all", space
— a second link. Link *sems can thams cad* as a whole to "all
sentient beings" and the inner links nest green inside it.

### Import and export

- **Import .hyp…** — reads a hypertexted file from the original
  Hypercontext era: both texts and every numbered link come in as
  Align-pane links (sub-syllable links are widened to whole
  syllables and counted). Proven against the original tutorial
  file.
- **Export aligned pairs (PENDING)…** — produces the candidate
  word pairs for the dictionary project. Every exported pair is
  **translator-authored attestation, always labeled PENDING** —
  nothing you align here enters the dictionary until a human
  review admits it. The machine may match; only the authority
  admits.
