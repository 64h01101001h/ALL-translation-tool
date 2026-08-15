# The ALL Translation Tool — User Manual
*The complete tutorial: every pane, every control, every workflow,
click by click. Version 0.9.0 · August 2026 · Asian Legacy Library.*

*How to use this manual: each entry tells you WHERE the control is
(its exact tab path), WHAT it does, the STEPS to use it with a real
worked example, and what you should SEE. Companion documents: the
Complete Feature Guide (conceptual reference), Install Quick Start,
and Troubleshooting FAQ. The same material is searchable inside the
app: Help menu → ALL Tool Help & Tutorials.*

## Contents
- **PART ONE — READ & TRANSLATE**
  - Overlay
  - Library
  - Files
  - Manuscript
  - Draft
  - Review
  - Align
- **PART TWO — RESEARCH & LEARN**
  - Search (Gofer)
  - Lookup
  - Sanskrit
  - Convert
  - Analysis
  - Trainer
  - Drills
- **PART THREE — INPUT, COMMUNITY & THE APP ITSELF**
- **CHAPTER 12 — THE INPUT PANE (Input → Input)**
  - 12.1 The toolbar, control by control
  - 12.2 Tutorial: your first page
  - 12.3 Tutorial: the block workflow (a folder of pages)
  - 12.4 Scan-follows-typing, in detail
  - 12.5 The @ next folio button
  - 12.6 Pre-fill from OCR (draft) — and its deliberate refusal
  - 12.7 Compare with partner file… — the double-keying diff
  - 12.8 Save… — exporting the work
- **CHAPTER 13 — THE OCR PANE (Input → OCR)**
  - 13.1 The toolbar, control by control
  - 13.2 One-time setup: the models
  - 13.3 Tutorial: recognize a page
  - 13.4 Marking illustration candidates on the open page
  - 13.5 The Illustration gallery (folder mode)
  - 13.6 Save to ocr_out…
  - 13.7 Batch folder… — a whole volume
- **CHAPTER 14 — THE PROPOSE PANE (Community → Propose)**
  - 14.1 One-time identity setup
  - 14.2 Making a proposal
  - 14.3 The other ways proposals get filed
  - 14.4 My proposals — what happened?
- **CHAPTER 15 — THE APPROVAL PANE (Community → Approval)**
  - 15.1 Reading the queue
  - 15.2 Ruling on an item
  - 15.3 What each ruling actually does
  - 15.4 The kind filter — one category per sitting
  - 15.5 Export approved dictionary candidates…
  - 15.6 The Rulings archive
- **CHAPTER 16 — THE APP ITSELF**
  - 16.1 The window: six workflow groups
  - 16.2 The menu bar mirrors the window
  - 16.3 The app menu: About, Check for Updates, Settings
  - 16.4 The Settings dialog — every field
  - 16.5 The Help window
  - 16.6 Night mode
  - 16.7 Session restore — the app reopens where you left off
  - 16.8 The self-test (--selftest)
  - 16.9 Help → Troubleshooting
  - 16.10 Hunt Everywhere (⌘K)


# PART ONE — READ & TRANSLATE

*The ALL Translation Tool User Manual, Part 1 of 3. Asian Legacy
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


### Reading with the keyboard

Once any phrase is lit, the arrow keys become the reading
controls:

- **→ / ←** — walk the highlight through the text, phrase by
  phrase: at each stop the longest phrase the dictionary knows
  lights up, exactly as if you had clicked there. Stops with no
  dictionary phrase (bare particles) still show their card and the
  walk continues.
- **↓** — step down the nesting chain in place (same as clicking
  again).
- **↑** — climb back up to the longer phrase.

The card follows every move, and the view scrolls to keep the
highlight visible. Until something is highlighted, the arrows keep
their ordinary text-caret behavior.

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
| **84000 glossary (CC BY)** (on) | the 84000 translation glossary block on the card — reference only, clearly labeled, with its link back to the original entry |
| **Das 1902 / Jäschke 1881 page links** (on) | the "open the page scan" links into Das's 1902 and Jäschke's 1881 dictionaries (both public domain), shown with the other reference material — Jäschke opens at letter-section accuracy (±page) |
| **reference dictionaries (LC/TD/THL/OT/IW)** (on) | the Lokesh Chandra / TibetanDictionary / THL reference stacks on the card |
| **Botok segmentation (reference)** (off) | an independent segmenter's reading of the clicked spot, labeled reference-only |
| **unattested-word hints (segmenter + Monlam)** (off) | hints in the document for words no list attests |

Card-only toggles re-render the **current card instantly, in
place** — your position in the phrase chain is kept. Only the two
document-affecting toggles (grammar marks, unattested-word hints)
rebuild the display — and they restore your card afterward.

Below the toggles a hint line reminds you: "Click a shaded word to
see its context; click again to cycle."


Choosing **Pronunciation (GMR)** renders the ENTIRE text as
phonetics in Geshe Michael's convention — the same battery-proven
engine the cards use, applied with its own word segmentation, so
syllables that merge into one spoken word render once (*bsod
nams* = **sunam**, never "so nam"; *skabs 'dir* = **kabdir**) and
junction sound changes are correct. Clicking and the arrow keys
work exactly as in the other modes (a merged word answers for its
first syllable). Anything the engine cannot pronounce appears in
⟨angle brackets⟩ rather than being dropped or guessed.

**Pronunciation (THL)** renders the text in the THL Simplified
Phonetic Transcription (Germano & Tournadre, THL) — the scholarly
standard used across the Tibetan & Himalayan Library. The engine
was implemented rule-for-rule from THL's published standard and is
proven against the standard's own complete example battery (139
cases, including its 19-word exception list) on every build. It
renders per syllable in THL's own interim machine convention:
word-final é and the *ba*→*wa* rule apply only where word
boundaries are certain, and unresolvable syllables appear in
⟨angle brackets⟩ — never guesses. GMR remains the binding spoken
convention of this project; the THL mode is a labeled scholarly
reference.

**Approved rulings override the engine.** When the authority
approves a pronunciation proposal (the 205-row prenasal register is
seeded in the Approval queue — its first row is Geshe Michael's own
*kamdir* for *skabs 'dir*), the ruling becomes a live layer over
the engine: the display says **kamdir** where the engine alone
would say *kabdir*, from the next launch. The canonical engine is
never modified — rulings are data, each with who ruled and when.


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
  word the HGM dictionary does NOT know, this is how the card
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
  These are moments where Geshe Michael says the Tibetan phrase itself (phonetic match; machine-located from class captions: candidates. **The
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

**Verse meter…** — syllable-count analysis of the document.
Sustained runs of shad-delimited lines on one odd count (7, 9,
11…) are reported as verse blocks; lines off the count are
flagged in red for YOUR judgment — an extra syllable from a
keying slip and one from poetic license look identical to
arithmetic, so the tool never decides which. Click any block or
flagged line to jump there.

**Prose & prayer reader (syllable counts)…** — the whole
document, line by line, every line carrying its syllable count
in a bracket at the left margin. This is the structural view:
verse blocks appear grouped under their meter heading
(**7-SYLLABLE VERSE**), on-meter counts show in green, an
off-meter line shows its arithmetic in red (`8 ≠ 7`), and prose
stretches are labeled and left plain with grey counts. The
summary line at the top gives the shape of the text at a glance —
how many lines, how many in verse, on which meters (7s and 9s
dominate the Tengyur). Tap any line to jump to it in the
document. The reader is **live**: keep it open while you edit and
the counts re-tally as you type — watching the count is itself a
way of learning the structure. Even-count **song meters (mgur —
6 or 8 syllables)** group too, under a stricter rule so prose
cannot masquerade as song, and their headings say SONG METER
(MGUR). Each meter heading also tells you how common that meter
is **across the whole installed canon** — a census over all
14 million lines of the Kangyur, Tengyur, and Sungbum found
333,802 verse lines, of which the classic 7-syllable meter
carries 61%, the 5-syllable 15%, the 9-syllable 8.5%, and the
6-syllable song meter 6% — so the reader can tell you when a
text is written in something rare.

**Citations & quotations…** — the upfront citation work, done
for you. The moment you open a text, the hint line tells you how
many quotations it carries and how many cited sources resolved to
works in your Library. The button builds the full report: **TEXTS
YOU WILL ENCOUNTER**, each with a direct open-in-Library link and,
beneath it, every quotation from your document — clickable both
into *your* text and, where the exact phrasing was found, **into
the cited source at its line**. Quotations whose source isn't
announced get a one-click hunt across the whole Library.
**Export quotations & bibliography document…** compiles
everything — full quotations grouped under their works with file
paths and located lines — into a side document you can save.
Detection is heuristic (…*zhes/ces* + a speech verb; *zhes bya
ba* namings excluded) and everything says so: verify before
publication.

**Outline (sa bcad)…** — the text's own structural outline,
extracted from its enumeration grammar (…*la gnyis* opens a
division; *dang po*, *gnyis pa*… are its headings). The tree is
machine-derived and heuristic — honestly incomplete where the
text's phrasing departs from the classic formulas. Click any
heading to jump the document there; the scan follow-along keeps
pace, so outline → passage → carving is one click.

**Teachings for this text…** (button). Press it with a document
loaded and the app sweeps every term the text contains and reports
where Geshe Michael taught each one — a dialog titled "Teachings
for this text — machine-located; the recordings are the
authority", listing timecoded class links per term, both by his
English and by the spoken Tibetan. This is the translator's
companion sheet: print-worthy preparation for translating the
text. Terms with no located moments say so honestly — the index
grows as more classes are indexed.

### SCANS (the viewer lives here; the one-shot tools moved to Read → Scans)

This is the one Overlay feature that touches the network (BDRC's
public image servers).

**THL Degé catalog link** — when the open text is a Degé Kangyur
file (KD…), a link appears under the scans button: **THL Degé
catalog: D.NNN**. It maps your text's Tohoku number to THL's own
catalog number through the 919-entry verified title concordance —
never by assuming the numbers are equal (they drift). Texts the
concordance could not match honestly get no link.

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
- Sungbum texts: over 500 collected-works texts across some
  thirty authors now map straight to their own scans (title-
  matched links; the tier is recorded per text). Where the
  archive's manifest carries no folio labels (common for
  collected-works volumes), the viewer POSITION-MAPS your
  keying's own @folio sequence onto the pages in order — labeled
  as such, with ◀ ▶ to nudge if the alignment drifts a page.
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

**Right-click in the document box — the folio jump menu.** The
top of the context menu identifies where you are — **folio 94a ·
line 3/7** — read from the text's own @folio markers and the
input centers' preserved line breaks (exact counting, never a
guess). Below it:

- **Show this folio in the scan panel** — turns the follow-along
  viewer to that side.
- **Open this folio in the Input workflow** — downloads that one
  folio image if needed (it lands in the offline store, below) and
  opens it in the Input pane's scan viewer, switching you there —
  ready to key or compare against the carving.
- **Locate selection on the woodblock…** — opens the full scan
  viewer on that folio with your **highlighted words boxed on the
  carving itself** (OCR word boxes, used only to locate — the
  recognized text is never shown as text). The line you were in is
  banded for context, the result is reported beside the breadcrumb
  ("2 of 2 words located…"), and any word OCR could not find on
  that side is named there, never silently dropped. All the
  viewer's tools (zoom, pan, adjust, thumbnails) work on the
  highlighted page.
- **Download all N folio scans (offline + Input)…** — fetches the
  whole volume into `library/scans/<work>/` as folio-named files
  (`094a.jpg`). From then on the follow-along works offline, and
  **Open scan folder…** in the Input workflow opens the volume in
  folio order. Strongly recommended when you start working with a
  new text — the app suggests it as soon as the scans are linked.
- **Compare with the eKangyur / eTengyur edition (Toh N)…** —
  Degé Kangyur (KD) and Degé Tengyur (TD) texts: your document (converted through the proven
  ACIP→Unicode chain) is collated against the same text in the
  Esukhia eKangyur, a Public-Domain e-text of the entire Degé
  Kangyur installed alongside the app. Red = a reading only in
  your keying, green = only in the eKangyur; a numbered apparatus
  lists the sites. Differences are witness readings for YOUR
  judgment — the tool never auto-corrects either side.
- **Export folio-faithful text files…** — splits the document at
  its @folio markers into one text file per woodblock side
  (`001a.txt`, `001b.txt`…), named to sit beside the downloaded
  scans — an OCR-ready keying/comparison set that can never
  disagree with the markers it came from.
- **Copy location** — puts `work · folio · line` on the clipboard
  for notes or citations.

**◀ folio / folio ▶** — page through the scans by hand, one folio
side at a time.

**locate word (OCR)** — a toggle button beside the folio arrows
(present when the OCR component is installed). Turn it on and the
app runs recognition on the current folio image and draws a box
around **the very word under your cursor, on the woodblock
itself**. Recognition is used only to LOCATE — never as text.

**Scan viewer (window)…** — the full viewer in its own window,
modeled closely on BDRC's own BUDA viewer, with its two levels:

*Single-image level.* Left: a collapsible thumbnail rail, one
entry per folio side (`94a · img.12`), the current side marked in
red. Pages already downloaded or viewed get their thumbnails
instantly; the rest fill in on their own from small BDRC preview
images, fetched politely one at a time — the rail becomes complete
without downloading the volume. Center: the page on a white sheet over a dark canvas.
Direct manipulation, exactly as on BDRC: **click the page to zoom
in at that spot**, **drag to pan**, **pinch (trackpad) or
⌘-scroll to zoom at the pointer** — plain scrolling still pans.
Bottom toolbar, left to right: **⬇ Download images** (the whole
volume into the offline store), **Go to** (a dropdown listing
every image in the volume — pick one, or type a folio like `94a`
or an image number and press Return), **◀ / ▶** with the current
position between them, pan arrows, **⊖ / ⊕** zoom with a percent
box (**Fit** sizes the page to the window), **⤢ full screen**,
**◐** brightness/contrast/invert (display-only — the scan file is
never altered; invert helps faint carvings), and **ⓘ** the scan's
identity, license, and attribution. Keyboard: **⌘→ / ⌘←** turn
folios, **⌘+ / ⌘−** zoom, **⌘0** fits the page.

*Whole-work level.* The top of the window is a **breadcrumb**:
`▤ work | folio 94a`. Click the work name and the canvas becomes
one continuous scroll of **every folio side in the volume**, each
captioned, downloaded pages shown full width (pages not yet
downloaded say so — ⬇ Download images fills them in). Click any
page (or the folio crumb) to drop back to the single-image level
on that side. Here a **Show e-text** toggle appears: under every
folio image, the input centers' own keying of that side, split
from the loaded document's @folio markers — scan and e-text
scrolling together, side by side. Also reachable from the
document box's right-click menu.

**Illustration gallery (cached scans)** — searches this volume's
already-downloaded pages for woodblock miniatures and diagrams and
shows the crops as a gallery. Candidates only, machine-found,
never claimed complete — and only pages you have already viewed
are searched (pages cache as you view them in Follow along). If no
volume is linked yet, the app tells you to link one first (Follow
along / Search BDRC).

**Four-layer view (this folio)…** — the folio you are reading in
four synchronized layers: the woodblock scan on top; beneath it a
line table with the OCR reading (machine-derived review material,
clearly labeled; absent when the models are not installed), the
e-text exactly as the input centers keyed it, and the master's
published English wherever that exact line is attested in the
aligned corpus. Click any row and its band lights on the carving —
with the OCR's real line geometry when available, uniform
arithmetic (labeled approximate) when not.

**Illustration gallery (whole volume)…** — the same candidate
search over the ENTIRE linked volume: every folio side BDRC lists
is fetched (reduced size, cached under the volume's own folder,
with a progress bar and a Stop button; already-cached pages are
not refetched), then all of them are searched. Fetch failures and
early stops are reported in the gallery title, never hidden.

*Worked example:* Read → Library → open the Kangyur folder
**7. SHER PHYIN SNA TSOGS** → VOL 1 (KA) → double-click
**KL00016E — the Diamond Cutter**. Press **Follow along in scans
(bdr:MW26071_0018)** — the number differs from the file's KL16 on
purpose: the app routes Lhasa texts through a verified title
concordance, because BDRC's part numbers follow the Lhasa (H)
catalog, not ACIP's KL numbers. The
woodblock pages open beneath the text. Click into different parts
of the document box and watch the folio image turn to follow your
cursor. Press **locate word (OCR)** and the word at your cursor is
boxed on the carving.

### FORMAT & EXPORT — MOVED: these tools now live in Read → Export

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


### The Pecha Maker

**Make pecha (PDF)…** turns the loaded document into an authentic
long-format pecha — the TibetDoc workflow, rebuilt native and
crash-free:

1. Load any text (Library double-click or paste) and press the
   button under FORMAT & EXPORT.
2. Choose the page — **traditional pecha (42 × 9 cm)**, wide
   (45 × 10 cm), A4 landscape, or the **Degé woodblock measure
   (68 × 10 cm)** — the lines per side (5–9; seven is the
   classical look), and the **rule weight** (Fine, Classic,
   Bold — the page furniture). The **classical opening** option
   sets the first two text sides at 5 lines with larger letters,
   as in the great woodblock editions (per THL's Degé
   documentation). For verse texts, **verse lineation** keeps the
   poet's own lines: each source line becomes a pecha line and a
   blank line becomes a stanza gap — off by default, since prose
   pechas flow continuously.
3. Choose the **print layout**: native folio sheets at true size
   for a print shop; **two-up on A4 / US Letter with cut
   marks** — two folio sides per office sheet, corner ticks to
   trim on, each side labeled; **three-up on A4** — three folio
   strips per sheet, the classic office pecha layout; or the
   **A5 booklet** — sides imposed in saddle-stitch order, two
   rotated pages per A4 face with a dashed fold line and the
   duplex instruction (flip on the short edge) printed on the
   first sheet, short runs padded with blanks to a multiple of
   four. Lines justify the **traditional way** (v4, from the
   W3C/r12a layout rules): a line ending in a tsheg pads with
   repeated tshegs to the margin, exactly as woodblocks do; and
   when a lone final syllable wraps onto a new line, its shad is
   set as the **rin chen spungs shad ༑** — the classical optical
   signal, recomputed automatically on every reflow. Lines ending in a double shad justify the second classical way: the gap stretches so the closing shad sits flush at the margin. The sbrul
   shad ༈ travels in an unbreakable block. Office layouts can open with a
   **cover sheet** — a framed label page carrying the title,
   volume letter, folio count, and date — the handling page a
   print partner or a shelf needs.
4. Optionally give the pecha its traditional dress:
   - **Ornamental title folio (1a)** — type the title in wylie or
     Tibetan; it is set centered in a double ornamental frame on
     the first side, and the text begins on 1b, as in a woodblock
     print.
   - **Margin title (ya-yig)** and **volume letter** — a short
     title and the volume letter (ka, kha, …) join the folio
     numeral in the rotated recto margin.
   - **Head mark ༄༅། ། (yig mgo)** opens the text.
5. Optionally check the interlinear lines:
   - **Phonetics under each segment** — Geshe Michael's
     convention, through the same battery-proven engine as the
     cards.
   - **English where the corpus attests the exact segment** — a
     segment gets its English beneath it ONLY when that whole
     segment exists in the aligned corpus of published
     translations. The line is a verbatim match; nothing is ever
     machine-composed, and unattested segments simply show no
     English. (A selftest proves this end-to-end on every build.)
6. **Preview…** shows the whole pecha in the viewer before you
   commit; **Make PDF…** writes it. All options are remembered
   for next time.

**Batch mode** (Export → *Batch: folder of texts → pecha set…*)
turns every text file in a chosen folder — ACIP or wylie — into
its own pecha PDF in an output folder, using your saved options.
Wylie files convert through the round-trip-proven engine;
per-text dress (title folio, margin title) stays off in batch,
since a saved title would be wrong on every other text. It needs
no loaded document.

Every folio side is framed with the traditional double rule and
carries its Tibetan folio numeral, rotated on the recto margin as
in a woodblock print. The script runs through the battery-proven
conversion chain in your chosen typeface; any syllable the engine
cannot convert appears as ⟨wylie⟩ — flagged, never guessed. The
PDF prints at 300 dpi.

### Resuming work

The Overlay restores your session on every normal launch: same document, same scroll, same cursor — and now the same highlight: the span that was lit when you left (including which rung of the nest) lights again on reopen. Your position is also autosaved a few seconds after every cursor move, so even a crash or force-quit loses almost nothing.


## Library

**Keeping the collections current.** Library → **Maintenance… →
Check for collection updates…** asks asianlegacylibrary.org for
the official Kangyur / Tengyur / Sungbum releases, shows each
one's date and size against what this machine has installed
(release fingerprints are remembered), and offers **Download &
install…** right in the app — confirm, watch the progress, and
the collection unpacks exactly as a manual install would. Your
own materials and working folders are never touched; run "Update
search index" afterward.

**Importing a new data release.** When the HGM data project
publishes a new release package (a folder holding
`hgm_dictionary_v*.json.gz`, and optionally a new parallel corpus
and reverse index), Library → **Maintenance… → Import data
release…** and choose the folder. The tool shows what it found —
the newest version of each file wins — copies the files into the
data area, rebuilds the spine database with the canonical builder,
and switches to the new spine on the next launch. Nothing is
switched if the build fails; the app keeps running on the current
spine throughout. After restarting, use Approval → **Regenerate
registers** to re-fold the approved layer onto the fresh release.


*Where: Read → Library. The preserved canon on your own disk —
8,986 texts and 14.1 million lines of the current public
Kangyur, Tengyur, and Sungbum releases, plus your
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


### The Translator's survey

The info panel's **Translator's survey…** link answers the
questions asked before a translation is assigned or begun — from
the text itself, deterministically:

- **Dictionary coverage** per token, by tier: curated, glossary,
  PROVISIONAL (amber), reference-only, uncovered.
- **Top unknown forms** with counts — the vocabulary work ahead.
- **Canonical quotations** (seven syllables or more, attested
  only) with the master's published English.
- **Structure**: explicit bam po / le'u markers, plus the
  30-syllable-shloka size ESTIMATE, labeled as such.
- **Form**: verse (with dominant meter and irregular-line count)
  or prose.
- **A difficulty ESTIMATE with its formula printed** — uncovered%
  plus half the provisional% — so you can weigh it yourself.

**Save as Markdown…** writes the report for a team lead's
assignment discussion. A full volume takes a few seconds; Stop
abandons cleanly.

### The maintenance buttons

- **Install collection ZIP…** — installs the canon. Download the
  Kangyur / Tengyur / Sungbum ZIPs from
  asianlegacylibrary.org/library (the link is right in the pane's
  header), press the button, pick the ZIP. The info panel shows
  "unpacking…", then "Installed. N file(s) now under library/…".
- **Import my materials…** — brings your own texts into
  `library/my_materials` (.docx, .txt, .acip, .act, .md, .rtf;
  Word documents convert automatically).
- **Maintenance… → Send to OCR** — routes a scan image to the Input → Scan pane
  for recognition.
- **Maintenance… → Legacy font rescue (UTFC)** — converts documents typed in
  pre-Unicode Tibetan fonts of the 1990s (TibetanMachineWeb,
  LTibetan, Sambhota…) to proper Unicode Tibetan, via the
  Universal Tibetan Font Converter run as a separate external
  tool. The result is labeled **utfc-derived** — machine-converted,
  review before treating as canonical — and if no Tibetan tsheg
  appears in the output the app warns you the source encoding was
  probably wrong (wrong guesses produce garbage, not silent
  errors).
- **Maintenance… → Legacy font rescue (py-tiblegenc)** — a second
  rescue engine covering **28+ encodings through 70 font tables**
  (the Dedris/Sambhota families and the TibetanMachine range).
  One-time setup: `bash tools/setup_tiblegenc.sh`. Pick the source
  font, pick the file, save the rescued Unicode — the engine's own
  report (including anything it could not map) is shown for
  review. A rescue is a per-glyph mapping, never blind trust.
- **Maintenance… → Update search index** — refreshes the full-text index that
  Research → Search's "search the Library" uses. The report counts
  files added / updated / removed / unchanged; a full build
  indexes all 14.1 million lines in under two minutes, and after
  it the Search pane answers from the index instantly.


## Files

*Where: Read → Files. A dual-pane file browser built into the
app — browse your whole disk side by side, and everything you
open routes straight into the right pane of the Tool.*

### The layout

Two independent browser panels sit side by side, with the **Drop
Stack** shelf on the far right. Each panel has:

- **Tabs** along its top — press **+** to open another tab at the
  current location; close any tab with its ✕ (the last tab
  stays).
- **Breadcrumbs** under the tabs — the path as clickable
  segments; click any ancestor to jump straight up to it.
- **The file listing** — folders always sort above files; click
  a column header to re-sort. The left panel starts in your
  Library folder, the right in your home folder.

### Moving around

- **Double-click a folder** to enter it; use the breadcrumbs to
  climb back out.
- **Double-click a file** and it opens in the *right place*:
  Tibetan text files (.txt/.act/.inc/.acip/.md) open in the
  Overlay; images (.png/.jpg/.tif…) open in the Input pane's
  scan viewer; everything else opens in its own Mac
  application.
- **hidden files** (top-left toggle) shows dotfiles in both
  panels.

### Copying and moving between the panes

Select files in one panel (⌘-click and ⇧-click select several;
**quick select** below grabs whole patterns), then use **Copy →
/ ← Copy / Move → / ← Move**: everything selected goes to the
folder the *other* panel is showing. Folders copy and move too,
recursively. Existing files are never overwritten — collisions
are skipped and reported.

### Quick select

Type a pattern in the **quick select** box (`*.act`, `S01*`, or
a full regular expression) and press Return: every matching name
in the active panel is selected at once, ready to copy, move,
batch-rename, or trash together.

### Batch rename

Select files, press **Batch rename…**: type find/replace (tick
**regex** for patterns), watch the live preview of every
old → new name, then Apply. A rename that would collide with an
existing name is skipped — nothing is ever overwritten.

### Compare panes

**Compare panes…** reports the two shown folders against each
other: files only on the left, only on the right, and same-name
files whose sizes differ. (This level only; subfolders are not
descended.)

### Sync folders

**Sync folders…** synchronizes the two shown folders — built for
double-keying partners exchanging work. Choose the direction
(left → right, right → left, or two-way where the newer copy
wins) and read the **full plan first**: every row says what would
be copied and whether it replaces an older copy. Nothing moves
until you press Apply, and sync **never deletes** anything.

### Commander keys (opt-in)

The **Commander keys** toggle turns on the classic
Total-Commander function keys for those who grew up with them:
**F3/F4** open · **F5** copy to the other pane · **F6** move ·
**F7** new folder · **F8** move to Trash. It is strictly opt-in
and never replaces normal Mac behavior; renaming in place (click
the name, or F2) works regardless. The choice is remembered.

### Sync browsing (opt-in)

With **sync browsing** on, entering a folder in one panel also
enters the same-named folder in the other panel when it exists —
made for walking two parallel trees side by side (an original
and its double-keyed copy, two editions of a collection).

### Remote connections

The **Connections** button reaches servers: **FTP, FTPS, WebDAV
(http/https), and SFTP**. Save a profile (name, protocol, host,
user, start path) — **passwords are never stored**: you are asked
when you connect, and the answer lives only until the window
closes. SFTP authenticates with your SSH keys (~/.ssh) instead.
Once connected: browse folders, double-click to enter,
**Download into the active pane** — files or **whole folders,
recursively** — and **Upload the active pane's selected files
or whole folders** (the folder structure is recreated on the
server).
The conflict rule is yours to choose per batch: **skip existing**
(the default — never touches what you have), **keep both**
(the new copy arrives renamed), or **overwrite** (explicit,
never silent). Every operation is written to the Activity log
with its result, honestly including failures.

### Workspaces

The **Workspaces** menu saves your entire layout — every tab in
both panels — under a name, and restores it in one click. Keep
one workspace per project (a text and its scans side by side, a
double-keying pair, a comparison set) and switch between them
instantly. Delete from the same menu.

### Finder tags

Right-click any file: the menu shows its **Finder tags**, so a
tagging system you maintain in the Finder stays visible here.
(Finder remains the editor for tags.)

### Favorites and Terminal

**Favorites** saves locations: add either panel's current folder,
jump back with one click, remove from the same menu. **Terminal**
opens Terminal.app at the active panel's folder (also on every
file's right-click menu).

### ZIP and TAR archives

Double-click a .zip, .tar, .tgz, or .tar.gz and it opens as a
browsable list *without extracting anything*. From there: **Extract selected & open**
(pulls just those entries to a scratch folder and opens them with
the usual smart routing) or **Extract all into the other pane**
(never overwrites existing files).

### Tabs remember themselves

Both panels' tabs — and the folder each one was showing — are
restored exactly on the next launch.

### The Drop Stack

The shelf on the right is for *gathering*: right-click any file
or folder in a listing and choose **Add to Drop Stack** to park
it there while you collect a project's materials from all over
the disk. The stack **persists across sessions** — quit and
relaunch, and your gathered pile is still there. Double-click a
stacked item to open it (with the same smart routing);
right-click for Open / Remove / Clear.

### Right-click, anywhere

Every file row offers: **Add to Drop Stack** · **Open** (smart
routing) · **Reveal in Finder** · **Open in Terminal here** ·
**Move to Trash** (recoverable from the macOS Trash, always).

*Phases 1–2 of the file-browser programme (Path Finder ×
ForkLift × Commander One feature study —
docs/design/FILE_BROWSER_PLAN.md). Still scheduled: tags,
workspaces, folder synchronization, ⌘K file targets, and remote
connections (SFTP/FTP).*

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

> **The ladder (2026-08-12):** the Draft is the WORKBENCH — understand and gather evidence; press **Send to Manuscript →** to carry your draft to the writing surface, where the PUBLISH tools (bibliography, footnotes) now live.

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


### The Evidence Ribbon — the evidence follows your cursor

Once a source is loaded you never need to click a clause row again:
click or arrow **into any clause of the source box itself** and the
anchors panel fills for that clause automatically —

- its established terms, color-banded as always;
- a **scaffold**: the master's attested glosses arranged in
  Wilson reading order, ready to arrange and complete. ⟨…⟩ marks a
  chunk with no attested gloss; a red **?** marks a provisional
  one. The scaffold arranges the master's own words — it never
  composes English for you;
- **"the master has translated this clause"** with his published
  English, whenever the exact clause exists in the aligned corpus;
- a **quotation** alert with source and published English when the
  clause quotes the canon (seven syllables or more, attested only).

Everything in the ribbon is deterministic evidence — the same
anchors, corpus, and quotation machinery as the buttons, now
arriving unasked, about the clause you are working on.


### The live terminology guard

As you type in the draft box, a quiet status line underneath keeps
score — the same deterministic checker as **Check terminology**,
running on a typing pause: **green** when every established term
in the source has a rendering in your English and no two Tibetan
terms share one English word; **amber** with counts (and the first
offender) when something needs attention. Advisory only — nothing
is ever auto-corrected; the full detail stays one press away.

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
- **Compose bibliography entry…** now emits the **Diamond Cutter
  Classics templates exactly**: en dashes in date and folio
  ranges, house sentence spacing, and — with the **Sanskrit
  work** checkbox — the full canonical citation down to volume,
  section, collection, and edition, reproducing the guide's own
  Dharmakīrti example structurally.
- **House style check…** — checks your English draft against the
  **Diamond Cutter Classics style guide's** mechanical rules:
  straight vs. curly quotes, `--` where an em dash belongs,
  hyphens in number ranges (en dash territory), ampersands in
  serial lists, era style (500bc, no space), and the house
  word-use list (accurate perception, mindstream, the Jewel of
  Realizations, and the rest). Findings are **flags, never
  auto-fixes** — the guide itself forbids blind Replace All.
  Click a finding to jump to it. The full guide lives in Help
  under the **Style:** chapters, with the editor's original
  banked in the repository.
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


# PART TWO — RESEARCH & LEARN

*The ALL Translation Tool User Manual · Asian Legacy Library / ACIP*

This part walks you, click by click, through the five panes of the
**Research** workflow tab — Search (Gofer), Lookup, Sanskrit,
Convert, Analysis — and the two panes of the **Learn** workflow tab
— Trainer and Drills.

How to read the paths: "Research → Search" means click the
**Research** tab in the window's top row, then the **Search** tab in
the second row that appears inside it. The menu bar mirrors the same
structure (a Research menu and a Learn menu, one submenu per pane),
so everything below is also reachable from the menus, from anywhere.

Two of the app's three rules matter constantly in this part: the
tool never guesses (failures are flagged, provisional material looks
provisional, AI output is banner-labeled), and Geshe Michael's
English is the binding layer (everything else is labeled reference).
You will see both rules restated wherever they apply.

---

## Search (Gofer)

*Research → Search. Geshe Michael's daily search tool for thirty
years, rebuilt with the same algorithms and its original layout.*

The Search pane is itself organized as three inner tabs, exactly as
the original Gofer was:

- **Search Setting** — where you build and run a search;
- **Saved Search** — your named, stored searches;
- **Search Results** — where the hits appear.

### The Search Setting tab, top to bottom

1. **The eight "go for" boxes.** Under the heading "Enter Text to
   Go For:" sit eight text fields — four above the combiner row,
   four below it. Each box holds one search term, and a term is
   always treated as a whole phrase ("sdug bsngal" matches those two
   syllables together, in order). Fill as many boxes as you need;
   empty boxes are ignored.

2. **The combiner and proximity.** Between the two banks of boxes is
   a drop-down with three choices, plus a small "proximity:" field:
   - **OR** — a hit is any place where ANY one of your terms
     appears.
   - **AND (same file)** — a hit requires ALL your terms to appear
     in the same file (or, in the corpus, the same text).
   - **NEAR** — a hit requires ALL your terms within N lines of one
     another; N is whatever you type in the proximity field. (If
     you leave proximity at 0 or blank, NEAR quietly uses 3.)

3. **The fold menu.** A drop-down offering "Ignore space and
   capitalization when searching" (the default, and the original
   Gofer's behavior), "Ignore space when searching", and "Ignore
   nothing when searching". It controls how strictly your typed
   terms must match the text.

4. **The target list.** A checkable list of everything the search
   will look in. The first two rows are always present:
   - **HGM aligned corpus (all courses)** — the master's
     translations beside the Tibetan, tens of thousands of aligned
     segments;
   - **Published apparatus (footnotes + bibliography)** — all 2,234
     published footnotes and 705 bibliography entries.
   Below those come folders of files on your disk (your installed
   Library folder is there by default). A checked row is searched;
   an unchecked row is skipped. Your checkmarks and folder list are
   remembered across sessions.

5. **The button row.** **Add** opens a folder picker and appends
   that folder to the target list. **Remove** deletes the selected
   folder row (the corpus and apparatus rows cannot be removed —
   only unchecked). **Duplicate** copies the selected folder row,
   handy when you want the same folder checked in one saved search
   and unchecked in another, exactly as in the original Gofer.
   **Save** stores the current search under a name (see Saved
   Search, below). **Find** runs the search; **Stop**, beside it,
   interrupts a long folder search.

### Worked example: two terms NEAR each other

Suppose you want every place where Geshe Michael's corpus has
*sdug bsngal* (suffering) close to *bden pa* (truth) — the first
noble truth territory.

1. Click **Research → Search**. You land on the Search Setting tab.
2. Type `sdug bsngal` into the first "go for" box.
3. Type `bden pa` into the second box.
4. Set the combiner drop-down to **NEAR**.
5. Type `5` into the **proximity** field: both terms within five
   lines.
6. In the target list, make sure **HGM aligned corpus (all
   courses)** is checked. (Leave the apparatus checked too — a
   footnote about the truth of suffering is worth knowing about.)
7. Press **Find**.

Folder results roll up **per file**, TibetDoc-style: each file
shows its hit count and first matching line, sorted by count —
and clicking the file name opens it in the Overlay **at the first
hit**, reading cursor placed on the line. The classic
search-locations workflow: survey where a term lives across a
whole collection, then step into any witness in one click.

The pane switches to the **Search Results** tab by itself. What you
see, top to bottom:

- a gray line echoing the exact query the engine ran, e.g.
  `"sdug bsngal" NEAR/5 "bden pa"`;
- a block per checked target, each headed by the target's name and
  its hit count — e.g. **HGM aligned corpus (all courses) — 14
  hit(s)**;
- under the corpus heading, the hits themselves: each begins with a
  small `[course:line]` citation, then the Tibetan in wylie with
  *the master's English in italics beside it*. This is the point of
  the tool — his attested renderings, in context, never composed;
- under the apparatus heading, matching footnotes ("note 156 — …"
  with its source book) and bibliography entries;
- under each folder heading, file hits as `filename:line` with the
  matching lines;
- a **total hit(s)** line at the bottom.

The first 20 hits of each target are shown in full; the count above
each block reports the whole tally.

To try the other combiners: switch the drop-down to **OR** and Find
again — now every segment containing either term appears. Switch to
**AND (same file)** — now both terms must occur somewhere in the
same text, at any distance.

### Saving a search, and the Saved Search tab

A search you will run again — a term list you check every time a new
text is input, say — should be saved:

1. Build the search on the Search Setting tab.
2. Press **Save**. A dialog asks for a name; type something you will
   recognize ("suffering NEAR truth") and confirm.
3. Click the **Saved Search** tab. Your search is in the list.

From the Saved Search tab: **double-click a saved search** (or
select it and press **Load**) and the pane returns to Search Setting
with all eight boxes, the combiner, the proximity, and the fold mode
restored — press Find and it runs. **Delete** removes the selected
saved search. Saved searches persist across restarts.


### The Citation web

**Citation web…** answers a question scholars usually spend months
on: which works quote which? Every exact Tibetan passage shared
between two courses of the aligned corpus becomes an edge — the
list shows every connected pair with its shared-passage count;
click a pair to read examples with the master's published English
from both sides. Attested reuse only (no fuzzy matching, seven
syllables or more), rebuilt from the corpus per release. The
heaviest edges are discoveries in themselves: a commentary and its
root text, a prayer book and the sadhana it feeds.

### Searching the Library, and searching the whole Mac

Two more reaches, both in the Search Setting tab's source list:

- **The library folder row** (checked by default) searches your
  installed Library (the current public collection releases (8,986 texts)). The
  Library carries a prebuilt full-text index, so answers come back
  instantly instead of file-crawling; after you install new texts,
  refresh the index with **Read → Library → Update search index**
  (a full rebuild of all 14.1 million lines takes under two
  minutes).
- **This Mac (Spotlight)** — an opt-in source row — extends the
  hunt to everything on the machine: every text, PDF, and Word
  file macOS has indexed. Each of your terms is searched *as
  typed* AND *as Tibetan script*, converted through the proven
  wylie and ACIP chains, since files on disk may be in any of the
  three. (NEAR has no meaning outside the corpus, so Spotlight
  simply looks for each term.) The results list the matching
  documents with their locations; click one to open it in its
  default application. (If Spotlight's index skips a volume or
  file type, the pane says so rather than pretending
  completeness.)

---

## Lookup

*Research → Lookup. The stacked multi-dictionary: HGM's entry first
and binding, every other layer labeled reference.*

The pane is split in two. On the right: a search box and the results
area. On the left: the Tibetan-order browser (below).

### Looking a word up — every input form

Type into the box at the top right and **press Return**. The box
accepts a headword in any form the team actually uses, and each
fallback announces itself honestly in a small green note so you
always know how your match was found:

1. **Wylie**: type `bsod nams`, press Return. The HGM entry stacks
   up first.
2. **Tibetan script**: paste བསོད་ནམས་ — same result.
3. **ACIP**: type `BSOD NAMS` (uppercase input is auto-detected as
   ACIP) — same result.
**⌘D — the floating dictionary window.** Select a word or phrase
in ANY text surface — the Manuscript, a draft, the Input editor,
even another pane's results — and press **⌘D** (or View → Look Up
Selection). The full Lookup stack answers in a floating card that
stays above your work: every match lane, every layer, the q.v.
links following inside the card, and the propose/promote actions
one click away. TibetDoc's dictionary window, reborn app-wide.

4. **A pronunciation in the master's convention**: type `jangchub`.
   There is no headword spelled that way, so the pane matches by
   sound and finds *byang chub*, with the green note "matched by
   **pronunciation** (GMR convention) — no exact headword for
   'jangchub'".
5. **The authority's ruled form**: when a pronunciation ruling has
   been approved (say *kamdir* for *skabs 'dir*), typing the RULED
   form finds its entry even though the release index only knows
   the engine's baseline — noted as "matched by the authority's
   **ruled pronunciation** ⟪ruled⟫". Rulings reach every surface:
   the whole-text pronunciation view, the entry card's pron line
   (green ⟪ruled⟫ tag), and search.
6. **A community spelling**: type `gonpa`. The colloquial register
   widens the search and finds *dgon pa*, noting "matched by
   **colloquial pronunciation** (community usage register)". The
   register only widens lookup — the GMR convention stays canonical.
7. **English, in reverse**: type an English word (say `suffering`)
   and the results end with an **English → Tibetan** section from
   the HGM reverse index: each Tibetan candidate with its
   pronunciation and its tier label — `[HGM (curated)]`,
   `[HGM (glossary)]`, or a red `[PROVISIONAL]` for auto-aligned
   matches, which are candidates, not rulings.

8. **Affix-stripping fallback**: type `po'i`. There is no such
   headword, so the pane strips the affixed particle and shows
   *po*, with the note "no entry for 'po'i' — showing **po**
   (affixed particle stripped; standalone *ba/bo* folds to *pa/po*; and an unambiguous past/future/imperative verb form folds to its present stem from the CC0 verb bank — every fallback is labeled in green above the results, never silent)". Nothing is silently substituted —
   the note is always there.

### Reading a result card

Entries stack in tier order with **HGM's entry first**. Each card
carries, where the data exists:

- the headword in Tibetan script, with its wylie in bold. Script
  that was machine-generated (rather than source-attested) is
  marked *[generated script]*;
- a **published-footnote flag** — "dealt with in published footnote
  N", naming the book — when the term's English is treated in the
  released apparatus. Reuse, don't rewrite;
- an **honorific badge** — "honorific (zhe sa)", or "HIGH
  honorific", "humilific", "double honorific" — with the ordinary
  counterpart printed beside it ("ordinary: …");
- an **IDIOM badge** for registered fixed expressions — amber with
  "(proposed)" while awaiting a ruling, green once approved;
- a **"Das 1902 · ~p.N (reference)"** link. Click it and Sarat
  Chandra Das's 1902 dictionary opens INSIDE the app at
  approximately the entry's page — the tilde is honest, the
  page-finding is approximate. Public domain, clearly a reference,
  never HGM material;
- **"Geshe Michael teaching this term"** — up to three timecoded
  links into his recorded classes, machine-located from class
  captions; the label says so, and the recording is the authority.
  Non-English sessions are tagged with their language;
- **"He says this word"** — moments where he speaks the Tibetan
  itself, matched phonetically on his own convention. Labeled as
  candidates; homophones share moments;
- the pronunciation (GMR convention), with ⟪card⟫ marking
  card-attested phonetics, and an **"also heard:"** line listing
  registered community spellings and prenasal-derived forms, each
  labeled by class — they never replace the convention above;
- the English equivalents, each with its tier: `[HGM (curated)]`,
  `[HGM (glossary)]`, or the red **PROVISIONAL (auto-aligned)** —
  provisional must look provisional, everywhere;
- Sanskrit and Hopkins lines, both explicitly marked "(reference)".

Below the HGM entries, the stack continues, every layer labeled:

- **Reference layers** — the reference dictionaries, each hit
  wearing its layer badge, headed by the honest small print
  "(unlicensed compilations — local lookup only, never for release
  data)";
- **Mahāvyutpatti** — the classical ninth-century Sanskrit⇄Tibetan
  glossary (DILA TEI, CC BY-SA 3.0; reference only), keyed by exact
  wylie;
- **Whitney** — see the Sanskrit pane below; for a Latin-alphabet
  query, Whitney's *Roots* (1885) answers here too, matched as a
  root, as a **past participle** (type `gata` and the card says
  "matched by past participle" and shows **√gam** "go"), or as an
  English meaning word. Each root shows its verb classes, its ppp,
  Monier-Williams senses with an "MW entry →" link to the Cologne
  scan, **section citations into Whitney's 1879 Sanskrit Grammar**
  (✦ marks a specific section, ⚠ an exception), a compact
  Grammar-by-topic index, and a link to the public-domain Grammar
  scan on archive.org;
- a **link-out tier** — "search elsewhere (external sites, links
  only)": 84000, BDRC etexts, Bibliotheca Polyglotta, Lotsawa
  House. Links only; nothing is ingested;
- the **English → Tibetan** reverse section (form 6 above);
- **"propose to the authority…"** — always the last line. Click it,
  pick the kind of proposal (honorific, pronunciation exception,
  rendering, idiom, note…), enter the proposed value, and it files
  into the team's Approval queue with "looked up in the dictionary
  (Lookup pane)" attached as provenance. Nothing enters the
  official data without the authority's ruling.

### Browsing in Tibetan order

On the left side of the pane:

1. Press **Browse the dictionary (Tibetan order)**. The button
   reads "sorting the dictionary…" while it builds the list once
   (traditional Tibetan collation, ka through a), then relabels
   itself with the count — e.g. "105,634 headwords, Tibetan
   dictionary order".
2. Scroll the list — every headword, in the order a Tibetan
   dictionary would print them.
3. Click any headword and the full stacked lookup for it runs in
   the right-hand pane, exactly as if you had typed it.

---

**Local dictionaries (StarDict)** — point Lookup at a folder of
StarDict dictionaries (.ifo/.idx/.dict, the format GoldenDict
and most shared Tibetan dictionaries use) and they join your
results as a clearly labeled **LOCAL DICTIONARIES ·
USER-SUPPLIED** layer. Queries match as typed, as wylie, and as
Tibetan script. Their licenses remain their own; nothing is
redistributed.

## Sanskrit

*Research → Sanskrit. The whole Sanskrit stack in one workbench:
every notation, Whitney's roots, and the Mahāvyutpatti bridge to
classical Tibetan.*

### The input, and the Analyze cascade

The single input line at the top accepts, auto-detected:

- **IAST** (the scholarly romanization): `pramāṇa`
- **Devanagari**: paste नमः or any Devanagari string
- **ACIP Sanskrit input-code** (the `a#/n%` style of the official
  standard): `prama#n%a`
- **a bare verbal root**, for Whitney: `bhū`

Worked example, the classic term of valid perception:

1. Click **Research → Sanskrit**.
2. Type `pramāṇa` (or, if you have no diacritics handy, the
   input-code form `prama#n%a` — the pane converts it first and
   notes "(from ACIP input-code)").
3. Press **Analyze** (or just hit Return).

The full cascade renders as a table — every representation of the
same word:

- **input-code** — `prama#n%a` (the official ACIP Sanskrit Input
  Code style);
- **next-letter** — `prama-n.a` (the standard's second style);
- **Tibetanized ACIP** — the `{PRA M'A nA}` transcription style;
- **Tibetan script** — the word as Tibetan letters, large;
- **Devanagari** — the word in Devanagari, large;
- **IPA** — the classical pronunciation.

Honesty note: any representation the engine cannot legally produce
carries a ⚠ mark instead of a silent approximation — flagged, never
guessed.

Below the table, three reference layers:

- **Whitney, roots (1885)** — matches from William Dwight Whitney's
  *Roots, Verb-Forms and Primary Derivatives of the Sanskrit
  Language*, all 937 roots. Type a root (`bhū`) for its entry; type
  an inflected past participle and the **PPP reverse lookup**
  answers — `gata` finds **√gam** "go". Clearly labeled
  "(reference)". For the fullest Whitney card — verb classes,
  Monier-Williams senses and entry links, and the section citations
  into Whitney's 1879 *Sanskrit Grammar* — type the same Latin form
  into **Research → Lookup**, whose Whitney layer carries the
  Grammar links (see the Lookup chapter above);
- **Mahāvyutpatti** — the bridge to classical Tibetan: the MVP
  entry number, the IAST, and the canonical Tibetan equivalents in
  script and wylie. For `pramāṇa` this is where you see the
  classical Tibetan rendering (*tshad ma*) the translators of the
  ninth century fixed;
- **search elsewhere** — a Monier-Williams (Cologne) link-out for
  the term. A link only; nothing ingested.

### Sanskrit OCR

**Sanskrit OCR…**, beside Analyze, reads a photographed or scanned
Devanagari word into the same pipeline:

1. Press **Sanskrit OCR…**.
2. If the optional recognition engine is not installed, the pane
   stops and tells you exactly what it needs (the tesseract engine
   with the Sanskrit model, `san.traineddata`) and how to install
   it — the converter works fine without it.
3. If it is installed, pick the image file. The recognized
   Devanagari lands in the input and the cascade runs.
4. The output is topped by an amber banner: **⚠ OCR-DERIVED —
   review material**. Recognition output is never trusted as text;
   verify against the image before any further use.

---

## Convert

*Research → Convert. Every writing system, and time itself.*

### Converting text — no button, it just converts

Type or paste into the big input box and every equivalent renders in
the output area as you type. The pane auto-detects what you gave it
and says so in the first row of the result ("detected: …").

**Tibetan input (ACIP or wylie).** Type `BSOD NAMS` (uppercase =
ACIP) or `bsod nams` (lowercase = EWTS wylie). The card shows:

- **detected** — ACIP or EWTS wylie;
- **wylie** — the transliteration;
- **Tibetan** — the word in Tibetan script, large;
- **phonetics** — the pronunciation in Geshe Michael's convention.

If any syllable cannot be legally converted, it renders as ⟨…⟩ with
a red note: "⟨…⟩ marks syllables the engine cannot legally convert —
flagged, never guessed." Matching Mahāvyutpatti entries, if any,
append below.

**Sanskrit input (IAST or Devanagari).** Type `pramāṇa` or paste
Devanagari, and the full Sanskrit card renders: IAST (decoded from
Devanagari when needed), Tibetanized ACIP, both official input-code
styles, wylie, Tibetan script, Devanagari, classical IPA, and the
**simplified pronunciation** per the Project's published standard
(the Release IV manual, banked in the docs). A **"how to say it"**
letter guide explains each diacritic letter, per the FPMT
transliteration standard. Any word in the input that IS a Whitney
root form gets its root annotation (no stemming — inflected words
are not guessed at), and Mahāvyutpatti matches append.

**Sanskrit OCR** lives here too: the button "Sanskrit OCR —
recognize a Devanagari image…" above the input behaves exactly as
in the Sanskrit pane — optional install, instructions if missing,
and an **⚠ OCR-DERIVED — unverified review material** banner over
everything it recognizes.

### The calendar tools — colophon dating in every direction

The **Colophon year** line at the top of the pane answers Tibetan
dating questions as you type. One input line, seven directions,
auto-detected by the shape of what you type:

1. **Western year → Tibetan designation.** A colophon-verifying
   classic: you have dated a text to 1357 and want its Tibetan
   designation. Type `1357`. The answer: "**1357** begins the
   Tibetan year **fire-bird** (me bya) — rabjung 6, year 31 of 60",
   with the honest small print that Losar falls in February/March,
   so an early-1357 western date may still belong to the previous
   Tibetan year. Years before the first rabjung (1027) get a plain
   refusal — no cycle designation exists.

2. **Element-animal pair → its years.** The direction colophons
   actually demand: the colophon says the text was finished in a
   *me bya* (fire-bird) year — but which one? Type `me bya` (wylie)
   or `fire bird` (English). The answer lists every fire-bird year
   with its rabjung — 1357 (rabjung 6), 1417 (rabjung 7), 1477
   (rabjung 8), … — and you pick the one history allows.

3. **Element-animal + rabjung → the year.** If the colophon names
   the cycle too, type `fire bird 6` and the answer narrows to
   **1357** alone. (An impossible combination — each element pairs
   with only half the animals — is refused with an explanation of
   the parity rule, not fudged.)

4. **Full Tibetan date → western weekday and date.** Type
   `3 8 2011` (tshes 3, month 8, year 2011) and the answer names
   the weekday and the western date, with the astronomical working
   (Julian day, zla-dag, gza-dag) in small print — verified 76/76
   against Edward Henning's original implementation.

5. **Western date → Tibetan date.** Type `2011-08-31` and the
   answer gives the tshes, month, and year. The traditional
   calendar's own irregularities are honored, never smoothed: a
   date carried by no lunar day is reported as an omitted (*chad*)
   day, and a doubled lunar day is flagged as duplicated (*lhag*).

6. **A year's months.** Type `2012 months` for the year's month
   list with element-animal names and any intercalary month marked
   ᵢ.

7. **A month's day table.** Type `2012 m3` (or `2012 m3i` for the
   intercalary third month, where one exists) for all thirty lunar
   days with their western dates, chad and lhag days annotated —
   verified 721/721 against the original program's printed years.

Honesty note: the day-precision engine (directions 4, 5, 7) covers
the Modern Karana epoch, 2009 onward. Ask it for an earlier full
date and it refuses rather than guesses — year-level questions
(directions 1–3) reach all the way back to 1027.

---

**TISE keys in Convert** — the de-facto Wylie-keyboard
conventions work in the Input box: `*` joins syllables with the
non-breaking tsheg (༌) and `_` inserts a non-breaking space —
useful when preparing text whose line breaks must not fall at
those joints. The markers are display conveniences; the canonical
converter never sees them.

## Analysis

*Research → Analysis. The full eighteen-section grammatical analysis
of a passage. One of only two features in the app that touch the
network.*

### What it is, and what it costs

The Analysis pane produces a complete, teaching-grade grammatical
report on a pasted passage. The division of labor is strict:

- the **deterministic engine** first extracts everything verifiable
  — segmentation, the HGM key-term anchors for every dictionary hit,
  corpus concordance matches;
- an **AI model** then writes the prose of the report around those
  verified materials;
- the **machine re-checks the result** — reconstructing the
  segments, verifying every dictionary quotation, confirming the
  Tibetan was not altered.

It needs a network connection, costs cents per passage, and its
output is banner-labeled AI, always. The pane requires an API key:
if `ANTHROPIC_API_KEY` is not set, the Analyze button is disabled
and the status line tells you so ("start the app from a terminal
where the key is exported").

### Running an analysis

1. Click **Research → Analysis**. Left side: the passage input;
   right side: the report area with the QC panel beneath it.
2. Paste your passage into the **Passage (ACIP)** box.
3. Optionally paste your own English attempt into **Draft English
   (optional)** — the report will engage with it.
4. Press **Analyze**.
5. Watch the status line: first the engine pre-pass reports its
   verified haul — "engine: 12 anchors, 4 corpus hits — asking
   Claude…" — then the report streams into the right-hand panel as
   it is written.

### The eighteen sections

The finished report renders as formatted text, in this fixed order:

1. Source (as given)
2. Segmentation (sentences → clauses → hinges)
3. Textual flags (only if needed)
4. Ultra-literal compositional translation (Tibetan order)
5. Base translation (two layers: literal and readable)
6. Segmentation / anchoring of the literal translation
7. Segmentation / anchoring of the readable translation
8. Key terms (HGM-anchored mini table)
9. Phrase glossing (3–10 key phrases)
10. Grammar + function (clause level)
11. Semantic tree diagram (fully expanded)
12. Philosophical implication summary
13. Three-layer context + example diagram (passage-specific)
14. Cross-referenced explanations (project files)
15. Philosophical rundown by schools
16. Dual-layer explanation (meaning + argument)
17. Optional: Geshe Michael Roach–style rephrase (two versions)
18. QC (quick)

Provenance is labeled inside the report itself: engine-derived
sections are marked verified; model-drafted sections are marked as
the model's; and the style-rephrase section (17) always carries its
own banner — **"AI imitation — NOT HGM text"** — because the machine
may match the master's English, never compose it in his name.

### The QC panel, and saving

When the stream finishes:

- the report re-renders formatted, and the **QC panel** below it
  fills with the machine's re-check findings — every dictionary
  quotation verified against the spine, the segmentation
  reconstructed, the Tibetan compared character by character;
- the status line gives the verdict: "done — machine QC passed", or
  "done — QC flagged 2 error(s), see the panel below". A flagged
  report is still shown — flagged, not hidden — so you can judge
  the flags yourself;
- the report **saves itself** to the analyses folder as a
  timestamped file (e.g. `analysis-20260811-143052.md`) containing
  the source passage, the full report, and the QC verdict — the
  status line names the saved file.

---

## Trainer

*Learn → Trainer. Learning to read, layer by layer — the pedagogy
pane. Engine guidance is labeled guidance; only HGM corpus lines are
answers.*

### Loading a passage

1. Click **Learn → Trainer**.
2. Paste a passage into the input box at the top — ACIP or wylie,
   auto-detected. For a first run, try the pane's own example:
   `SANGS RGYAS KYIS CHOS BSTAN` ("the Buddha taught the Dharma").
3. Press **Load**.

The passage appears below, split into clauses, each in its own
bordered block — and *nothing else*. The discipline of the pane is
that you attempt the passage yourself first, then reveal help one
layer at a time.

If you have drill history (see Drills, below), a small note appears
first: "readable for you? your deck covers ~40% of this text's 12
dictionary words — words you click join the deck." That is your own
local progress data measuring the text against your vocabulary.

A **Tibetan script** checkbox at the right end of the control row
re-renders the whole passage in Tibetan script instead of
transliteration, at any time, with all layers intact.

### The six reveal layers

Six checkboxes sit in a row under the input. Check them in any
order; each adds its layer to every clause, and unchecking removes
it. Take them in the intended order:

1. **1 · chunks** — clause and phrase boundaries. Each clause
   redraws as a row of outlined pills, one per grammatical chunk.
   The clause-final boundary particle sits outside the pills in its
   own color. Click any pill and a panel opens inside the clause
   showing the full tier-labeled dictionary entries for the words
   in that chunk (click the pill again to close it); every word you
   click this way joins your drill deck.

2. **2 · particle roles** — under each clause, a line per chunk
   naming its grammatical marker and role: e.g. "SANGS RGYAS KYIS
   ⟨kyis⟩ — agent (the one who acts)". Fused particles are marked
   "fused"; the boundary particle gets its own line naming its
   function.

3. **3 · reading order** — the find-the-verb-first discipline. Each
   pill gains a purple number showing the suggested order of
   attack, and below the clause the guidance spells itself out:
   first a verb line — "verb: **bstan** (…)" in green, citing the
   dictionary's own tense evidence for the identification, and the
   verb's class with what that class expects its cases to be. When
   the dictionary holds no verb evidence for the clause, the line
   says so honestly, in amber: "verb: no HGM verb evidence in this
   clause — candidate unverified." Then a numbered step list ("1.
   read the verb…, 2. …"); chunks marked **+** are connected
   phrases to read together with their neighbor.

4. **4 · vocabulary** — under each clause, one line per dictionary
   word: "≡ **sangs rgyas**: Buddha", each with its quality tier.
   Provisional glosses carry the red **[PROVISIONAL]** tag — a
   machine-aligned candidate, not a ruling. A word with no HGM
   equivalent says exactly that.

5. **5 · answer key** — THE ANSWER KEY, and the pane's honesty
   pivot: wherever this clause exists in Geshe Michael's aligned
   corpus, his own English appears in a green block with its
   `[course:line]` citation — "HGM: …". The machine never invents
   answers: if the clause is not in the corpus, the layer says "not
   in the corpus — the Analysis pane can give an AI reading
   (labeled AI)" and leaves it at that.

6. **6 · full parse (Wilson)** — the complete parse in the manner
   of Joe Wilson's *Translating Buddhism from Tibetan*: every unit
   designated with its category and detail (case numbers assigned
   by verb class), and beneath it the **dots line** — every
   syllable dot (tsheg) named per the Science of the Dots,
   reproducing the book's own diagram style: each token followed by
   its dot's label.

The suggested study rhythm (see also Part Three's workflows): load,
attempt bare, then 1 → 2 → 3 to check your structural reading, 4
for the words you missed, 5 to compare your translation with the
master's, 6 when you want the full grammatical accounting.

---

## Drills

**My weak spots…** — the miss taxonomy. Every wrong drill answer
is filed under the skill it reveals — a particle family, a cloze
sentence role, a reading-order step, a vocabulary item — and this
report names them all, with counts and the drill that trains
each. Wrong answers are never wasted here. The record is local
(progress.db); nothing leaves your machine.

*Learn → Drills. Exercises that write themselves from the corpus —
every exercise comes from a real corpus segment, so every answer is
Geshe Michael's own text. Engine guidance is labeled guidance.*

### The controls

Across the top, under the progress line:

- **the drill-type drop-down** — six exercise types (below);
- **New drill** — generates the next exercise of the current type;
- **Tibetan script** — a checkbox that re-renders the current
  exercise in Tibetan script instead of ACIP, any time;
- **adapt to my level** — a checkbox (on by default when progress
  tracking is available): the generator picks segments matched to
  your deck — more of what you have missed, at a difficulty you can
  attempt;
- **a course drop-down** — appears only for Parallel reading, to
  choose which course you read through.

Below the exercise area: an answer row (radio buttons, when the
drill type offers choices), a text field for typed answers, a
**Check / Reveal** button, and the result area.

At the very top, the **progress line** — your own local record,
stored in a file that belongs to you: "deck: 34 words (5 due) ·
drills: 120 (96 right) · reading: 210 segments (14 peeked) · weak
spots: kyis ×4, las ×2". Words join the deck when you click them
while reading (Overlay or Trainer); the weak spots are the particles
and words you miss most.

### The six drill types, one by one

**Chunk order.** Restore a scrambled clause. The exercise presents a
real corpus clause cut into chunks and shuffled, lettered A, B, C…,
with its `[course:line]` source cited. Read them, work out the
original order, and type the letters into the text field — e.g.
`C A B` — then press **Check / Reveal**. The result says "Correct."
or "Not yet.", then shows the original order, Geshe Michael's
English for the whole segment in a green block ("HGM: …"), and the
reading-order guidance — the verb, with its evidence, and the
numbered plan — so a miss becomes a lesson.

**Cloze (fill the blank).** The exercise shows HGM's English for a
whole segment, then the Tibetan with one chunk replaced by [ … ],
and radio-button options for what fills the blank. Pick one, press
**Check / Reveal**: right or wrong, the result names the answer and
the grammatical role of the blanked chunk.

**Particle choice.** A real clause with one particle blanked (▢) and
radio options for the variants (kyi/gyi/gi…, kyis/gyis…, and so on).
Choose, then **Check / Reveal**: the result gives the answer, the
explanation *from the classical particle tables* (why this suffix
takes this variant), and HGM's English for the segment.

**Parallel reading.** Read a course straight through, segment by
segment, revealing his English only when you need it. Pick the
course in the drop-down that appears. Each exercise shows one
Tibetan segment with its citation. If you can read it, press **New
drill** to move on — that counts as a clean read. If you need the
answer, press **Check / Reveal** — HGM's English appears, and the
progress record honestly counts it as a peek.

**Vocabulary review (SRS).** Spaced repetition over your own deck —
the words you clicked while reading. The exercise shows the next due
word, large, and asks: do you know it? Grade yourself with the two
radio buttons ("I knew it" / "I did not know it"), then press
**Check / Reveal** to see the full entry and confirm. The interval
adapts: known words retreat, missed words return soon. When nothing
is due, the pane says so and sends you back to reading — clicked
words join the deck.

**Translate & compare.** The capstone: an unseen, level-appropriate
corpus segment appears with its citation. Write your own translation
in the draft box below it — *before* revealing anything — then press
**Check / Reveal**. His rendering appears beside yours, along with a
terminology comparison of your draft against the established
equivalents. Nothing grades your style; the master's version
teaches.

Every drill ends the same honest way: the answer shown is never
generated — it is the master's own attested text, cited to its
course and line.


# PART THREE — INPUT, COMMUNITY & THE APP ITSELF

*The ALL Translation Tool User Manual, Part Three of Three.
Asian Legacy Library · Asian Classics Input Project.*

This part walks you, click by click, through the last two workflow
groups — **Input** (the Input and Scan panes) and **Community** (the
Propose and Approval panes) — and then through everything that
belongs to the application as a whole: the window and menu
structure, the About box, Check for Updates, the Settings dialog,
the Help window, night mode, session restore, and the built-in
self-test.

Paths are written as **Workflow → Pane**: "Input → Scan" means
click the **Input** tab in the window's top row, then the **Scan**
tab in the second row that appears inside it.

Three rules govern everything you will read here, as they govern
the whole tool: Geshe Michael's English is the binding layer; the
tool never guesses (failures are flagged, machine output is labeled
and looks provisional); and nothing enters the official data
without the authority's approval.

---

# CHAPTER 12 — THE INPUT PANE (Input → Input)

**What it's for.** This is the input-center workstation — the ACE
lineage recovered and rebuilt (Aaron Cram's acip-support design,
Apache-2.0). You open a page scan, type the ACIP transliteration
beneath it, and the scan follows your typing. When you and your
partner have both typed the same block, the pane runs the classic
double-keying comparison: every discrepancy lights up, and you
correct until the two files match exactly — which has always been
the input-center rule.

**The layout.** A banner across the top states the pane's purpose.
Below it runs a single toolbar row holding every control. The rest
of the pane is split vertically: the **scan viewer** on top (a
scrollable image area reading "open a page scan (PNG/JPG/TIFF)"
until you open one) and the **typing editor** below it (placeholder
text: "Type the ACIP transliteration here, one line per woodblock
line…"). A status line at the very bottom reports what the pane
just did.

## 12.1 The toolbar, control by control

All of these sit in one row across the top of the pane, left to
right. (Every one of them is also reachable from the menu bar:
**Input menu → Input submenu** — see Chapter 16.)

- **Open scan…** — opens a single page image (PNG, JPG, JPEG, TIF,
  TIFF). Use this for a one-page job.
- **Open scan folder…** — opens a whole folder of page scans as a
  work queue: the block workflow. See §12.3.
- **Recent ▾** — a menu of the last ten scans and scan folders you
  opened, newest first. One click reopens the entry (a folder
  reopens as its block, restoring your page and typing from the
  work folder as usual). Entries whose file has moved or been
  deleted simply don't appear. The list survives restarts.
- **Right-click in the typing area** — two woodblock actions:
  **Locate selection on the scan (OCR)** boxes your selected typed
  ACIP on the scan image itself and scrolls it into view — your
  own line is searched first (typed line N = scan line N), OCR is
  used only to locate, and the status line reports what was found
  and names anything that wasn't. And the menu identifies the
  folio of the nearest @marker above your cursor; when the open
  scan folder contains that folio's image (folio-named files from
  the Overlay's "Download all folio scans", or any filename
  carrying the folio token), it offers **Jump to this folio's scan
  (page N)** — the viewer turns straight to that woodblock side.
- **◀ page** and **page ▶** — step backward and forward through
  the pages of an open folder. Between them a label shows your
  place, e.g. **page 3/12**. Both buttons are greyed out until a
  folder is open, and "◀ page" greys out on page 1, "page ▶" on
  the last page.
- **scan follows cursor** (checkbox, on by default) — as you type,
  the scan viewer scrolls so the region you are transcribing stays
  in view. Untick it if you prefer to scroll the scan by hand.
- **Detect lines (OCR)** — runs line *detection* (the BDRC
  PhotiLines model, running locally) on the current scan so
  cursor-following jumps to the exact line band instead of ACE's
  proportional estimate. Detection only — no text is recognized.
- **Pre-fill from OCR (draft)** — recognizes the page (BDRC
  Woodblock model) and fills the **empty** editor with an ACIP
  draft for you to correct. It refuses to touch a page you have
  typed on. See §12.6.
- **− / zoom slider / + / percent box** — the zoom controls
  (25–400%; 100% is actual size). Click **−** or **+** to step by
  15%, drag the slider, or use the percent box: pick a preset
  (50%–300%), type an exact figure, or choose **Fit width** /
  **Fit page** to size the scan to the viewer. Keyboard, whenever
  the pane has focus: **⌘＋** (or ⌘⇧＋) zooms in, **⌘−** zooms
  out, **⌘0** returns to 100%. Hold **⌘ and scroll the mouse
  wheel** (or trackpad) over the scan to zoom around the point
  under the pointer — the spot you're inspecting stays put, as in
  the BDRC scan viewer.
- **@ next folio** — inserts the next folio marker per the ACIP
  specification. See §12.5.
- **Compare with partner file…** — the double-keying diff. See
  §12.7. After a compare, **◀ disc / disc ▶** step the cursor
  from discrepancy to discrepancy with an "i/N" readout — no
  scroll-hunting for the orange patches.
- **Save…** — exports your typing: the current page as a text
  file, or (in folder mode) the whole block combined in page
  order. See §12.8.

*(Detect lines and Pre-fill appear only in builds with the OCR
models component — every standard team build has it.)*

## 12.2 Tutorial: your first page

1. Click the **Input** tab in the top row, then the **Input** tab
   inside it.
2. Click **Open scan…**. A file dialog opens in your `library`
   folder. Pick any page image and click Open.
3. The scan appears in the viewer. The status line confirms it:
   the file name, its pixel size, and a note that the scan will
   follow your cursor — "(proportional — ACE mode, approximate)"
   for now, because no lines have been detected yet.
4. Click **Detect lines (OCR)**. The status line says
   "detecting lines…", then reports something like
   "7 line band(s) — cursor now follows exact lines (typed line N
   = scan line N, per the input convention)". Faint violet
   rectangles now outline each detected line on the scan.
5. Click into the editor and start typing the first line of the
   page in ACIP — one editor line per woodblock line. As your
   cursor sits on editor line 1, the first line band on the scan
   fills with an amber highlight and the viewer scrolls to keep it
   in view. Press Return and type line 2: the amber band moves to
   the second scan line. As you type rightward along a line, the
   view tracks your column too.
6. Deliberately type an impossible syllable — say `BKRAG SHIS`
   instead of `BKRA SHIS`. The moment you finish the token,
   **`BKRAG` gains a red wavy underline**: it fails the classical
   syllable-legality rules (the CC0 tibetan-spellchecker data,
   the same rules the Overlay uses). Fix the spelling and the wave
   disappears. Nothing is ever auto-corrected — the underline is a
   flag, not an edit. (Folio markers like `@001A` are structure,
   not Tibetan, and are never flagged. On a pasted text over 200k
   characters the live checker pauses and says so — the Overlay's
   checker handles full texts.)

## 12.3 Tutorial: the block workflow (a folder of pages)

This is how a real input assignment is worked: a folder of page
scans, typed in order, with your work autosaved page by page.

1. Click **Open scan folder…** and choose the folder of page
   images. (Pages are taken in filename order.)
2. The first page loads. The label between **◀ page** and
   **page ▶** reads **page 1/12** (or however many pages the
   folder holds), and the status line tells you where the typing
   is kept: "12 page(s) — typing saves per page into
   `library/input_work/<folder name>/`; use ◀ ▶ to move through
   the block".
3. Type page 1 (with Detect lines on each page if you want exact
   line-following — bands are per page).
4. Click **page ▶**. Two things happen: **your page-1 typing is
   saved automatically** to its own file
   (`page_001_<scanname>.txt` inside the work folder), and page 2
   loads with an empty editor — or with whatever you typed on it
   before, if you are returning. You never press Save just to
   move between pages.
5. Work through the block. Come back any time: **Open scan
   folder…** on the same folder restores the queue, and every
   page's typing reloads exactly as you left it when you step
   onto that page.
6. When the block is done, click **Save…** — see §12.8 for what
   is exported.

## 12.4 Scan-follows-typing, in detail

The **scan follows cursor** checkbox controls the pane's signature
behavior:

- **Before line detection** ("ACE mode"), the pane estimates your
  place proportionally — editor line 3 of 9 puts the view about a
  third of the way down the page, and your column position maps
  across the width. It is approximate by design, and the banner
  and status line say so honestly.
- **After Detect lines (OCR)**, following is line-exact: typed
  line N is scan line N (the input convention). The current line's
  band is highlighted in amber; the others are outlined faintly.

If the line model is not installed, Detect lines says "line model
missing (see the Scan pane for download instructions)" — Chapter
13, §13.2 tells you where the models go.

## 12.5 The @ next folio button

ACIP texts mark each woodblock side with a folio marker: `@001A`
(folio 1, side A), `@001B`, `@002A`, and so on — `@` in column 1,
the number zero-padded to three digits.

Click **@ next folio** and the pane:

1. reads your document to find the *last* folio marker in it;
2. computes the next one (after `@001A` comes `@001B`; after
   `@001B` comes `@002A`; an empty document starts at `@001A`);
3. inserts it on a fresh line (opening one if your cursor is
   mid-line, so the `@` lands in column 1), followed by a new
   line for you to keep typing.

*Worked example.* Your page ends with the text of side 4B, whose
marker `@004B` is already in the document. Click **@ next folio**:
`@005A` appears on its own line at the cursor, and the cursor
waits on the next line, ready for side 5A's first line of text.

## 12.6 Pre-fill from OCR (draft) — and its deliberate refusal

**What it does.** Recognizes the current scan with the BDRC
Woodblock model (running locally, CC BY-NC 4.0, used with BDRC's
permission), converts the recognized wylie to ACIP through the
tool's round-trip-proven converter, and fills the editor with the
draft — one line per detected woodblock line.

**What it will never do.** Overwrite your typing. Try this on
purpose so you trust it:

1. Type anything at all into the editor — even one syllable.
2. Click **Pre-fill from OCR (draft)**.
3. Nothing changes in the editor. The status line explains:
   *"the editor already has typing — pre-fill only starts from an
   empty page (it never overwrites your work)."*

That refusal is a designed guard, and the app's self-test pins it
on every release.

**Using it properly:**

1. On an **empty** page (fresh page in the queue, or Select All +
   Delete), click **Pre-fill from OCR (draft)**. If lines have not
   been detected yet, detection runs first automatically.
2. The status line counts through "recognizing line 1/7…",
   "recognizing line 2/7…", and then the editor fills with the
   ACIP draft.
3. Read the status line's standing orders: *"OCR draft filled:
   7 line(s), ocr-derived — CORRECT it against the scan (doubtful
   syllables are underlined); the double-keying pass vs your
   partner applies as always."*
4. Every syllable the spellchecker doubts is already red-wave
   underlined. Correcting the draft against the scan **is** your
   typing pass — the draft is review material, never trusted text,
   and the partner comparison still applies unchanged on top.

If the recognition model is missing, the status line says so and
points you to the Scan pane's download instructions.

## 12.7 Compare with partner file… — the double-keying diff

Double keying is the input centers' accuracy discipline: two
operators type the same block independently, then reconcile every
difference until the files match exactly. This button runs that
reconciliation.

1. With your typing in the editor, click **Compare with partner
   file…**.
2. Pick your partner's file (`.txt`, `.act`, `.inc`, or any text
   file).
3. The status line says "comparing…", then every discrepancy
   lights up in the editor, in two colors:
   - **Orange highlight** — text that is in *your* file only.
   - **Green seam** — a thin green mark at the spot where your
     partner's file has *extra* text that yours is missing.
4. The status line reports the count and the rule: *"3 discrepancy
   region(s) — orange = here only, green seam = partner has extra
   text there. Correct until the texts match exactly (the
   input-center rule)."*
5. Examine each lit region against the scan, fix whichever file is
   wrong (edit yours here; tell your partner about theirs), and
   run **Compare with partner file…** again.
6. Iterate until the status line says: *"double-keying PASS — your
   text and the partner file match exactly."* That line is the
   goal of the whole exercise.

*Worked example.* You typed `... BDE BA DANG ,`; your partner
typed `... BDE MA DANG ,`. After the compare, your `BA` glows
orange (yours differs) with a green seam beside it (the partner's
variant `MA` exists there instead). The scan shows the woodblock
reads `BA` — your partner mis-keyed. You leave yours, your partner
corrects theirs, and the next compare passes.

*Scale note.* The comparison is meant for input blocks, not whole
volumes: if either file exceeds 500,000 characters the pane asks
you to split it first, and says so in the status line.

## 12.8 Save… — exporting the work

**Single-page mode** (you used Open scan…): **Save…** proposes
`library/input_work/<scan name>.txt` and writes the editor's
contents there. The status line confirms: "saved …/input_work/
S0001-1a.txt".

**Block mode** (you used Open scan folder…): **Save…** first
flushes the current page to its per-page file, then exports the
**whole block combined in page order** — every non-empty page's
text, concatenated — proposing the name
`<work folder>-combined.txt`. The status line reports, e.g.:
*"combined 11 typed page(s) of 12 into
…/library/input_work/KANGYUR_V031-combined.txt"* — a page you
skipped simply contributes nothing yet.

Your per-page files remain in `library/input_work/<folder>/`
untouched; the combined file is the deliverable.

---

# CHAPTER 13 — THE OCR PANE (Input → OCR)

**OCR models…** — download additional BDRC recognition models
(Lhasa Kangyur, Derge Tengyur, dbu-can book hands, modern typeset
print; all CC BY-NC 4.0, used with BDRC's permission) and pick
which one Run OCR and the Input pane's OCR pre-fill use. The
bundled Woodblock model stays the fallback whenever a pick is
missing; the switch takes effect on the next recognition run.

**What it's for.** Tibetan text recognition on woodblock page
images — embedded, proven, and honest about itself. The models are
by the Buddhist Digital Resource Center (CC BY-NC 4.0, used with
BDRC's permission, credited wherever output appears), and they run
entirely on your machine — no network. Everything this pane
produces is **OCR-DERIVED review material**: banner-marked,
quality-checked against the syllable-legality rules, never trusted
as text, and never allowed anywhere near the corpus.

**The layout.** A banner restates exactly that rule. Below it, one
toolbar row; below that, a vertical split — the **page viewer** on
top ("open a scan image (PNG/JPG/TIFF)" until you do) and the
**results area** underneath, where the recognized lines render.

## 13.1 The toolbar, control by control

- **Open scan image…** — opens one page image (PNG, JPG, JPEG,
  TIF, TIFF). Enables Run OCR.
- **override deskew to 0°** (checkbox) — labeled in full:
  "override deskew to 0° (DEVIATION from the BDRC pipeline —
  workaround for its angle bug on straight pages)". The BDRC
  pipeline normally estimates and corrects page rotation; on some
  perfectly straight pages that estimate misfires. Ticking this
  forces the angle to zero. Because it deviates from the reference
  pipeline, the output header says so explicitly whenever it is
  on. Leave it off unless a straight page comes out skewed.
- **Run OCR** — greyed out until an image is open; runs the full
  pipeline on the page. See §13.3.
- **mark illustration candidates** (checkbox) — outlines folio
  regions *not* covered by detected text lines — side panels and
  large vertical gaps, where woodblock miniatures and diagrams
  live. Machine-found candidates from pure line geometry; never
  claimed complete. See §13.4.
- **Illustration gallery…** — runs that same candidate search
  across a whole *folder* of pages and shows the crops as a
  gallery. See §13.5.
- **Save to ocr_out…** — greyed out until OCR has run; saves the
  recognized text as a headered review file. See §13.6.
- **Batch folder…** — OCRs every page image in a folder (a
  scanned volume), one output file per page. See §13.7.

## 13.2 One-time setup: the models

The first time you click Run OCR (or Detect lines in the Input
pane) without the models installed, the results area tells you
exactly what to fetch and where to put it:

> **models missing.** Download from Hugging Face
> (huggingface.co/BDRC — CC BY-NC 4.0):
> · PhotiLines.onnx + config.json →
>   `library/ocr_models/BDRC_PhotiLines/`
> · OCRModel.onnx + model_config.json →
>   `library/ocr_models/BDRC_Woodblock/`

Place the four files, click Run OCR again, and you are in
business. (Team builds normally ship with the models already in
place.)

## 13.3 Tutorial: recognize a page

1. Click **Input → Scan**.
2. Click **Open scan image…** and choose a page. The page appears
   in the viewer; the results area reads "ready — Run OCR".
3. Click **Run OCR**. You watch the pipeline work: "detecting
   lines…", then the page redraws with violet line boxes and small
   green word-underline boxes, then "recognizing line 1/7…",
   2/7, and so on.
4. When it finishes, the results area holds the full report:
   - A header line: *"deskew 0.34° (BDRC pipeline) · 7 line(s) ·
     every line OCR-DERIVED (review before use) · click a word to
     see it on the page"*. (With the deskew override on, that
     header instead brands the run "**(override — deviation)**".)
   - **One block per line**, numbered: the recognized text in
     wylie (monospace), the same line rendered in Tibetan script
     below it, and an immediate quality check — e.g. "**0/9
     syllable(s) fail legality**" in green, or "**2/9 syllable(s)
     fail legality**" in warning orange. That count is the
     legality QC: each recognized syllable is tested against the
     classical syllable rules, and failures are counted, not
     hidden.
   - A footer with the total: *"total legality flags: 3 — OCR
     output is review material (models: BDRC, CC BY-NC 4.0)"*.
5. **Click any word** in a wylie line. The word **lights up on the
   woodblock**: its box fills amber on the page image and the
   viewer scrolls to it. This is how you verify a doubtful
   syllable — click it, look at the actual carving, judge for
   yourself. Click other words to move the highlight around.

## 13.4 Marking illustration candidates on the open page

After an OCR run (the search needs the detected lines), tick
**mark illustration candidates**. The page redraws with heavy gold
rectangles around every region the geometry suspects: side panels
beyond the text envelope (where miniatures sit on first and last
folios) and vertical gaps taller than twice the median line
height. Each rectangle is labeled right on the page:
**"image? (candidate)"**.

Candidates only — machine-found from line geometry, never claimed
complete. A miniature the geometry cannot see is not disproven.
Untick the box to clear the overlays.

## 13.5 The Illustration gallery (folder mode)

To sweep a whole volume for woodblock art:

1. Click **Illustration gallery…** and choose a folder of page
   images. (If the folder has no images, the pane says "No page
   images in that folder.")
2. A progress dialog runs — "Scanning pages for illustration
   candidates…" — with a **Stop** button; every page gets line
   detection and the same geometry search.
3. The gallery window opens, titled with its honest census, e.g.
   **"Illustration candidates — 14 region(s), machine-found,
   review material"**. Inside is a grid of thumbnail crops, each
   captioned with the file name of the page it came from.
4. **Double-click any thumbnail** to see the crop full size, in
   its own window titled "<page file> — candidate crop". Close it
   to return to the grid.
5. Click **Save crops…** to export: choose a destination folder,
   and every crop is written as a PNG **named by its source page**
   (e.g. `0042-illus3.png` came from page image `0042`). A
   confirmation reports "14 crop(s) saved — named by source
   page."
6. Click **Close** to dismiss the gallery.

If nothing is found, the pane says so without pretending
completeness: "No candidates found (candidates only — a miniature
the geometry cannot see is not disproven)."

*(The Overlay's SCANS column offers this same gallery over a
linked volume's already-cached BDRC pages — see Part One.)*

## 13.6 Save to ocr_out…

After a single-page run, click **Save to ocr_out…**. The dialog
proposes `library/ocr_out/<page name>-ocr.txt`. The saved file
carries its provenance in its first line, permanently:

    # OCR-DERIVED (unverified review material) — source:
    <page file> — models: BDRC (CC BY-NC 4.0, with permission)

followed by one wylie line per recognized line. The results area
confirms the save and adds a useful tip: opening that file in the
Overlay runs the ocr-derived banner and a first-pass QC there too.
The header travels with the file so that no downstream reader can
mistake recognition output for verified text.

## 13.7 Batch folder… — a whole volume

1. Click **Batch folder…** and choose the volume's folder of page
   images.
2. A progress dialog runs page by page ("Batch OCR — 214
   page(s)"), naming each file as it goes, with a **Stop** button
   that ends the run early (everything already written stays
   written).
3. Output: **one `-ocr.txt` per page** under
   `library/ocr_out/<folder name>/`, each with the same
   OCR-DERIVED header as §13.6.
4. When it finishes, the results area shows the audit: pages
   written, total lines, total syllable-legality flags, failures
   (unreadable or errored pages are listed by name), whether the
   run was stopped early — and the standing banner: *"every file
   is OCR-DERIVED review material (models: BDRC, CC BY-NC 4.0)"*.
   Below the summary, a per-page detail list shows each page's
   line count and its legality-flag count in orange when nonzero.

The deskew override checkbox applies to batch runs too.

---

# CHAPTER 14 — THE PROPOSE PANE (Community → Propose)

**What it's for.** The team's channel to the authority. Anyone can
propose — a new honorific marking, a pronunciation exception, a
rendering for a word, an idiom, a note — and every proposal waits
for the authority's ruling before it changes anything official.
Your name rides on every proposal for provenance; nothing is a
login, there is no server and no account. The shared folder (a
Dropbox folder works perfectly) *is* the whole synchronization
system: offline-capable and fully audit-trailed.

**The layout.** A banner, then two boxed sections — **"Who you are
(one-time)"** and **"Make a proposal"** — then the **"My proposals
— what happened?"** button with its results area below.

## 14.1 One-time identity setup

Do this once, on your first day (a coordinator will usually tell
you the folder path):

1. Click **Community → Propose**.
2. In **"Who you are (one-time)"**, type your name into **Your
   name**. It is stamped on everything you file — that is its
   only job.
3. **"I am an authority (can approve) — Geshe Michael / Adam
   only"** (checkbox): leave it OFF unless you are the authority.
   Ticking it (and restarting) reveals the Approval pane
   (Chapter 15).
4. **Proposals folder**: click **Choose…** and select the team's
   shared proposals folder (the dialog hints: "Dropbox-synced is
   ideal"), or paste the path into the field.
5. Click **Save identity**. A confirmation appears — "Identity
   saved." (For an authority it adds: "The Approval tab is now
   available (restart to show it).")

These same three settings also live in the **Settings dialog**
(View menu → Settings…, under *Team & proposals* — see §16.4);
change them in either place, they are one and the same.

## 14.2 Making a proposal

The **"Make a proposal"** box is a five-field form:

1. **Kind** (dropdown) — what sort of thing you are proposing.
   The ten kinds, and what each means:
   - **Honorific term (↔ ordinary)** — a term of the general
     honorific register, paired with its ordinary counterpart.
   - **HIGH honorific marking** — the elevated register used for
     high lamas and dignitaries (rare).
   - **Humilific (self-lowering) form** — the register for one's
     own actions (the honorific is never used of oneself).
   - **DOUBLE honorific (subject + recipient)** — honorific in
     both directions at once.
   - **Pronunciation exception** — a word whose spoken form
     departs from the rules.
   - **Abbreviation / contraction candidate** — a manuscript
     abbreviation or bsdus-tshig contraction and its expansion.
   - **Rendering for a word (→ dictionary)** — a proposed English
     equivalent for a word; goes to the data project if approved.
   - **Rendering for a phrase/clause (→ dictionary)** — the same,
     for a longer unit.
   - **Idiom / fixed expression** — marks a string as an idiom or
     fixed expression. Its card wears an IDIOM badge — amber
     while proposed, green once approved. The English stays the
     dictionary's.
   - **Note about a passage** — anything worth the authority's
     eyes that fits none of the above.
2. **Tibetan** — the term itself, in ACIP or wylie.
3. **Proposed** — the proposed English / pronunciation /
   expansion, as the kind requires.
4. **Secondary** — optional: the ordinary counterpart (for
   honorifics), a domain, or a clarifying note.
5. **Evidence** — where you saw it, the passage, and why you are
   proposing it. Evidence is what the authority rules on — give
   the citation.

Then click **File proposal for review**. The form clears; your
proposal is now a pending item in the shared folder, visible in
the authority's queue. If you click File with both the Tibetan and
the Evidence empty, the pane declines politely: "Nothing to
propose — Add the Tibetan or a note."

*Worked example.* You keep meeting *zhal* used as the honorific
for *kha* ("mouth"). Kind: **Honorific term (↔ ordinary)**.
Tibetan: `zhal`. Proposed: `mouth (hon.)`. Secondary: `kha`.
Evidence: the passage and text where you saw it. **File proposal
for review** — done. It now waits for the ruling.

## 14.3 The other ways proposals get filed

You rarely need to fill the form by hand, because proposals also
file themselves from where you are working:

- **From any entry card** (Overlay or Lookup): the **"Propose to
  the authority…"** link files a proposal with the passage you
  were reading auto-attached as evidence.
- **From the Overlay's spelling-doubts list** (REVIEW → Show
  spelling doubts → check rows → **File for ruling**): each
  checked doubt becomes a *spelling flag* in the queue, its
  context attached as evidence. These have their own meaning at
  ruling time — see §15.4.

## 14.4 My proposals — what happened?

Click **"My proposals — what happened?"** at the bottom of the
pane. A list opens of every proposal *you* have filed, newest
first, each with:

- its **status** in color — PENDING (amber), APPROVED (green),
  DECLINED (maroon), DEFERRED (grey);
- the kind, the Tibetan, and your proposed value;
- the filing date;
- and once ruled: **who ruled it, when, and the authority's
  comment** in italics — the feedback half of the loop.

If you have not set your name and folder yet, the list says so
instead: "set your name and the proposals folder above first".

---

# CHAPTER 15 — THE APPROVAL PANE (Community → Approval)

**What it's for.** The authority's queue — every proposal, ruled
one by one. Nothing enters the registers without a ruling, and
every ruling stamps who ruled and when.

**Who sees it.** Only users with the authority role (the checkbox
in §14.1 or Settings → Team & proposals; the change takes effect
on the next launch). For everyone else the pane simply does not
exist. When there is work waiting, the tab itself carries a live
count: **Approval (7)** — and the count updates after every
ruling.

**The layout.** A banner stating the data boundary, then one
toolbar row — the **kind filter**, **Refresh queue**, **Export
approved dictionary candidates…**, **Rulings archive** — and below
it the queue itself.

## 15.1 Reading the queue

The queue opens with a session overview: *"7 pending · signed in
as Adam"*, followed by a small per-kind census (e.g. "2 honorific
· 3 pronunciation · 2 spelling"). Then each pending item appears
as a boxed card showing:

- the **kind**, in capitals, with its consequence labeled right
  beside it: **(applies in-app)** in green for register kinds, or
  **(→ data project)** in maroon for dictionary kinds;
- the **Tibetan**, the proposed value, and any secondary field;
- the **evidence** (first 300 characters);
- **who proposed it and when**;
- and the three actions: **✓ Approve · ✗ Decline · ⏸ Defer**.

If the proposals folder is not set yet, the pane says: "set the
proposals folder in the Propose tab first."

## 15.2 Ruling on an item

1. Click **✓ Approve** or **✗ Decline** on any card. A small
   dialog asks for an **optional note** — write one when the
   proposer will benefit ("good catch; prayer register only"), or
   leave it empty. Your note travels back to the proposer's "My
   proposals — what happened?" list. Cancel the dialog to abort
   the ruling entirely.
2. Click **⏸ Defer** to push the decision to another day — no
   note dialog, the item just leaves the pending queue as
   deferred. Deferring is recorded like everything else.
3. The queue refreshes — and **keeps your scroll position**, so
   ruling your way down a 200-item queue never loses your place.
   The tab's pending count ticks down as you go.

Every ruling is stamped with your name (from Settings) and the
date, permanently.

**Approve all in filter…** rules the whole filtered queue in one
considered act — built for the machine-seeded queues, where a
205-row derived register deserves one decision, not 205 clicks.
It shows the exact count and filter first and asks; one optional
note is recorded on every ruling in the batch; register items
apply in the app immediately, exactly as single approvals do.
Decline or defer the exceptions *before* running it — the batch
takes the queue as it stands.

## 15.3 What each ruling actually does

The data boundary is enforced in the software itself — different
kinds have different consequences, and none of them ever edits the
corpus or the texts.

**Register kinds — honorific (all four levels), pronunciation
exception, abbreviation.** Marked **(applies in-app)**. Approving
writes the item into the tool's own register files **immediately**,
tiered "approved" with your name and the date — for everyone using
the shared folder, from their next launch. No export step. Two
special behaviors for pronunciations: approving a machine-derived
(prenasal-derived) pronunciation upgrades its existing register
row in place rather than duplicating it; **declining** a
machine-derived pronunciation **removes** it (community and
hgm-attested rows are never touched by a ruling).

**Dictionary kinds — word rendering, phrase/clause rendering,
idiom / fixed expression, note.** Marked **(→ data project)**.
Approval records the ruling but the app **never** writes these
into the dictionary or corpus. They accumulate as approved
candidates until you export them (§15.5) for the data project's
next versioned release. An approved idiom's card badge turns from
amber to green — but its English remains the dictionary's.

**Spelling flags** — filed from the Overlay's spelling-doubts
list — have their own filter and their own inverted logic:

- **Approve** = you confirm it is an **input error**. It goes into
  the export for the input teams to fix at the source — the app
  itself never edits the texts.
- **Decline** = you rule the form **valid**. From the next launch
  on, the software **stops doubting that form everywhere** — no
  more red wave on it, for the whole team. A decline here is not
  a rejection; it is a positive ruling that teaches the software.

## 15.4 The kind filter — one category per sitting

The dropdown at the toolbar's left filters the queue to a single
kind. Its entries: **All kinds · Pronunciations · Honorifics ·
HIGH honorifics · Humilifics · Double honorifics · Spelling flags
· Idioms · Words / phrases / notes**.

The recommended cadence is to take **one kind per sitting**:
filter to it and rule the category whole — pronunciations one
week, honorifics the next, spelling flags after an input batch
comes in. Judgments stay consistent within a category, each
sitting stays short, and the queue never becomes a wall. When a
filtered view is empty the pane says "nothing pending in this
kind."; when everything is ruled, "nothing pending — the queue is
clear."

**Refresh queue** re-reads the shared folder — click it when
teammates have been filing while you work (a synced Dropbox folder
delivers their proposals to your disk in the background).

## 15.5 Export approved dictionary candidates…

When you have approved dictionary-kind items to hand over:

1. Click **Export approved dictionary candidates…**.
2. The save dialog proposes
   `data/candidate_alignments/approved_terms.tsv`.
3. The written TSV opens with its own charter — *"# APPROVED
   dictionary/corpus candidates for the data project — approved
   in-app, never auto-ingested"* — followed by one row per item:
   kind, wylie, value, evidence, approver, ruling date.
4. The pane confirms: "exported 5 approved dictionary
   candidate(s) to …".

*Worked example.* You approved *dge ba* → "virtue" (a word
rendering). The export contains its row with your name and the
date; the data project reviews it into the next release; until
that release ships, it remains exactly what the file calls it — a
candidate.

## 15.6 The Rulings archive

Click **Rulings archive**. The queue area becomes the permanent,
read-only record of **every** decision, newest first: status in
color (approved green, declined maroon, deferred grey), kind,
Tibetan, value, who proposed it and when, **who ruled it, when,
and the note**. Nothing is ever silently discarded — over the
years this is a record of the authority's own judgments. Click
**Refresh queue** to return to the pending view.

---

# CHAPTER 16 — THE APP ITSELF

This chapter covers everything that belongs to the application as
a whole rather than to any one pane.

## 16.1 The window: six workflow groups

The window's **top row** holds the six workflow tabs; inside each,
a **second row** holds its panes — seventeen in all:

| Workflow | Panes |
|---|---|
| **Read** | Overlay, Library |
| **Translate** | Manuscript, Draft, Review, Align |
| **Research** | Search, Lookup, Sanskrit, Convert, Analysis |
| **Learn** | Trainer, Drills |
| **Input** | Input, Scan |
| **Community** | Propose, Approval |

"Input → Scan" always means: click **Input** in the top row, then
**Scan** in the row inside it. (Approval appears only for the
authority role, and carries a live pending count on its tab.)

## 16.2 The menu bar mirrors the window

The menu bar has **one menu per workflow group** (Read, Translate,
Research, Learn, Input, Community), and inside each, **one submenu
per pane** — the same structure as the tabs, so anything you can
find in the window is in the same place in the menus, reachable
from anywhere. Each pane's submenu holds:

- **Show pane** — jumps the window to that pane (the right top
  tab and the right inner tab, both switched for you);
- then **every button** the pane has, as a menu action — choosing
  one raises the pane and clicks the button;
- then **every checkbox**, as a checkable menu item that mirrors
  the box's live state — toggle it in either place.

Try it: from anywhere in the app, open **Input → Input →
Compare with partner file…** from the menu bar. The window
switches to the Input pane and the compare dialog opens.

On a Mac there is a bonus: open the system **Help** menu and type
into its search box — macOS finds any menu item by name and points
an arrow at it. Since the menus mirror everything, that means the
system can literally point at any feature in the app.

## 16.3 The app menu: About, Check for Updates, Settings

On a Mac these three live in the application menu (the one named
after the app); they are also under **View** on every platform.

**About ALL Translation Tool** opens the About box:

- the app name and **two version numbers** — the app's own
  VERSION and the **HGM DATA** release version it is running
  (e.g. v0.27.2), so support always knows both at a glance;
- the creator credit — Adam Derick Andrade (Loppun Pawo, a.k.a.
  StaticSky) — for Geshe Michael Roach · the Asian Legacy Library
  · Diamond Cutter Classics;
- the mission, verbatim: *"To turn the preserved library of Tibet
  into translations, and students into translators — with Geshe
  Michael Roach's English as the binding layer. The machine may
  match the master's English; it may never compose it."*;
- the build line (C++20, the Qt version, 38 automated test
  batteries, runs fully offline), a contact email link, and a
  note that third-party components and data sources are credited
  in OPEN_SOURCE_NOTICES, shipped with every release.

**Check for Updates…** — updates are distributed by the ALL team
as DMG files in a shared folder (the team Dropbox), and this menu
item checks it:

1. The **first** time, the app asks: "Updates are distributed by
   the ALL team as DMGs in a shared folder (e.g. the team
   Dropbox). Choose that folder now? It will be remembered."
   Click Yes and pick the folder — once, ever. (You can also set
   or change it any time in Settings → Data → **Team updates
   folder**.)
2. The app scans the folder for files named
   `ALL-Translation-Tool-<version>.dmg` and compares the newest
   against the version you are running.
3. If a newer one exists: **"Update available — Version 0.28.0 is
   available (you are running 0.27.2)"**, with an **Open** button
   that opens the updates folder so you can install the DMG.
4. If not: **"Up to date — You are running version 0.27.2 — the
   newest in the team folder."** (If the folder holds no DMGs at
   all, it honestly appends "(no DMGs found there)".)

**Settings…** (⌘, on a Mac) opens the Settings dialog — next
section.

## 16.4 The Settings dialog — every field

Everything the app remembers, in one dialog, in four groups:

**Appearance**
- **Night mode (dark chrome, cream pages)** (checkbox) — the same
  switch as View → Night mode (§16.6). Applies immediately when
  you Save.

**Reading**
- **Overlay displays text as** (dropdown: Tibetan script / ACIP /
  Wylie) — the script the Overlay uses by default.

**Team & proposals** *(mirrors the Propose pane's setup fields —
change them in either place, they are the same settings)*
- **Your name (provenance)** — stamped on your proposals and
  rulings.
- **Authority role (Geshe Michael / Adam) — shows the Approval
  pane** (checkbox).
- **Shared proposals folder** — the team Dropbox path, with a
  **Choose…** button.

**Data**
- **Team updates folder** — where the team's release DMGs arrive,
  with a **Choose…** button; used by Check for Updates (§16.3).
- **Data folder override** — normally left empty (the field's own
  placeholder says "auto-detected — set only to override"); point
  it elsewhere only if support asks you to.
- A note in the dialog itself reminds you: **changes to the data
  folder or the authority role take effect on the next launch**.
  (Night mode and the rest apply right away.)

**Cancel** discards your edits; **Save** writes them all.

## 16.5 The Help window

**Help menu → "ALL Tool Help & Tutorials…"** (or the standard help
shortcut, ⌘? on a Mac) opens the searchable Help window: a search
box across the top, a results list on the left, the reading page
on the right.

- **With the search box empty**, the left list is the full browse
  index: every tutorial **chapter** (marked 📖) — one per pane,
  plus Getting Started, Suggested Workflows, and Concepts —
  followed by **every control in the app, grouped under its pane's
  gold heading**. When you don't know a feature's name, scroll:
  the whole tool is on this list. Click a chapter to read it, or a
  control to see what it does and jump to its pane.
- **This User Manual** also lives here: chapters prefixed
  "Manual: " (Help menu → "User Manual…" filters straight to
  them).
- **Type anything** — "Search any feature, button, pane, or
  topic…" — and the list fills with matching chapters *and*
  matching **controls**: every button, checkbox, and dropdown
  item in the whole app is indexed by its label and tooltip.
  A control result reads like "Pre-fill from OCR (draft) —
  Input"; click it and the page shows the control's name, which
  pane it lives in, its explanation, and an **"open that pane
  now"** link that jumps the main window straight there.
- **Help menu → "Suggested Workflows…"** opens the same window
  directly onto the workflow-recipes chapter, with the left rail
  listing **every workflow by name** (▸ Read a text, ▸ Translate
  a text end to end, ▸ Input-center day, ▸ Team proposal cycle,
  …); click one and the page jumps to that recipe.

Try it now: open Help, type "partner". The result "Compare with
partner file… — Input" appears; click it, then click "open that
pane now" — you are standing in the Input pane.

## 16.6 Night mode

**View menu → Night mode** (checkable), or the same switch in
Settings → Appearance. Dark chrome, cream reading pages — the
reading surfaces stay warm and paper-like while the frame goes
dark. It applies immediately and is remembered across launches.

## 16.7 Session restore — the app reopens where you left off

Quit the app mid-passage and relaunch it: the **Overlay reopens
the same text, scrolled to the same place, with the cursor where
you left it** — saved on quit and after every open, restored on
every normal launch. Around it, each pane keeps its own memory:
the Manuscript pane reopens your last manuscript; the Library tree
remembers which folders you had open; the Search pane's folder
list persists exactly as the original Gofer's did; the Overlay's
display choices (script, typeface, reference layers) and even the
document box's height are remembered. You set the tool up once; it
stays set up.

## 16.8 The self-test (--selftest)

If anything ever misbehaves, the app can examine itself. Open
Terminal and run the app binary with the `--selftest` flag:

    "/Applications/ALL Translation Tool.app/Contents/MacOS/ALL Translation Tool" --selftest

The app constructs its real panes offscreen against the real data
and exercises them — 89 checks at this writing (the battery grows
with every feature), each printing PASS or FAIL with a one-line
description:

    [PASS] Input: identical double-keying reports clean
    [PASS] Input: pre-fill refuses a non-empty editor (never
           overwrites)
    [PASS] Approval: queue renders the pending count
    ...

A clean run exits with status 0; any failure exits nonzero and the
FAIL lines localize the problem — copy them into your message to
support and the diagnosis is already half done. (Behind the
scenes, thirty-eight automated test batteries also run before any
change to the software ships.)

---

*End of Part Three — and of the manual. Seventeen panes, one
principle: the machine may match the master's English — it may
never compose it.*


## 16.9 Help → Troubleshooting

Everything a tester needs to file a useful finding in one click:

- **Show Logs in Finder** — reveals the lifecycle log (launches,
  quits, and — with Verbose Logging on — every pane switch).
- **Show Data Folder in Finder** — opens the ALL Tool Data folder.
- **Copy Installation Info** — one-line version/platform summary
  to the clipboard.
- **Generate Diagnostic Report…** — writes a full report to the
  Desktop (versions, data-file health, disk space, settings, log
  tail) and reveals it; attach it to any bug report.
- **Report a Problem…** — opens a pre-addressed email with a
  what-happened template and your installation info filled in.
- **Storage…** — cache sizes with safe clear buttons (woodblock
  scan cache; library search index, rebuildable). Your texts and
  OCR output are never touched.
- **Verbose Logging** — adds pane switches to the lifecycle log.
- **Reset Settings…** — all preferences back to defaults, with
  confirmation; texts and data untouched; next launch.


## 16.10 Hunt Everywhere (⌘K)

Press **⌘K** anywhere (or View → Hunt Everywhere…). Type anything
in any form — *bsod nams*, *BSOD NAMS*, *merit*, or *sunam* — and
one list answers from every source at once:

- **📖 dictionary** rows with the binding gloss and its tier;
- **🔁 English reverse** rows (your English → his Tibetan);
- **🗣 phonetics** rows through the GMR-convention fold;
- **📜 corpus** rows with the master's published English;
- **🎧 teaching** rows that open the recording at the timecode;
- **📁 file** rows — Library filenames that match what you typed
  (a catalog number like *TD04156*, a title fragment): Enter
  opens the file itself — texts in the Overlay, scan images in
  the Input viewer.

Arrow to a row and press **Enter**: dictionary/reverse/phonetics
rows land you in Lookup with the search already run; corpus rows
land you in Search; teaching rows open the recording. Escape
closes. Everything is the same deterministic machinery as the
panes — the palette only saves you the walk.
