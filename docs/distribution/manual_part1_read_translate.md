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
| **84000 glossary (CC BY)** (on) | the 84000 translation glossary block on the card — reference only, clearly labeled, with its link back to the original entry and (since the 2026-08-20 merge of 84000's official live glossary) "attested in" links to the published works that carry the term |
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
  "corpus usage" on, the **actual renderings course by course** —
  the concordance of how Geshe Michael rendered this word across
  that corpus.
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
  (bskungs yig, from the TibSchol table, CC0, and the rKTs
  Abbreviations list, CC BY 4.0 — each expansion names its
  attesting table — "reference;
  not a typo") and bsdus-tshig contractions with their shared
  gloss — both labeled with their derived register.
- **Published footnotes** that deal with this term.
- **"Geshe Michael teaching this term"** — timecoded links that
  open the recorded classes at the moment Geshe Michael teaches the term.
  These are moments where Geshe Michael says the Tibetan phrase itself (phonetic match; machine-located from class captions: candidates. **The
  recording is the authority**, always; non-English sessions are
  tagged so you are not surprised.
- **"Geshe Michael says this word"** — a second tier locating moments
  where Geshe Michael speaks the Tibetan itself, matched phonetically
  on the GMR convention. Candidates; homophones share moments — the card says
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

**Text DNA (structure strip)…** — the whole text's shape in one
picture: each source line is one colored column — gold on the
dominant meter, vermilion for a verse line off the meter, grey for
prose, pale for blank, with a green tick over lines carrying a
quote announcement (…*zhes/ces* + a speech verb). The summary line
gives the counts; hover for any line's number and syllables; click
anywhere and the document jumps there. Pure measurement — the
strip shows only what the syllable arithmetic and the quote
grammar actually found.

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
authority", listing timecoded class links per term, both by Geshe
Michael's English and by the spoken Tibetan. This is the translator's
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

**English on the carving (hover).** Once locate-word has run on a
side, hovering any word *on the woodblock image* shows a small tip:
the OCR-read wylie and Geshe Michael's English for that word (when
the dictionary carries it). The tip always says what it is —
OCR-read from the carving, review material, never text. Hovering
never starts an OCR run by itself; the boxes come from the
locate-word pass you already ran.

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

Three ways in: the visible **Survey…** button in the Library's
control row, **Maintenance… → Translator's survey (selected
text)**, and the info panel's **Translator's survey…** link — all
three read the selected text. The survey answers the
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

> **House styles (2026-08-20).** The Manuscript bar is now a ribbon
> with a **HOUSE STYLES** gallery — Body, Heading, Section, Verse,
> Colophon — each button previewing its own look. Select paragraphs
> and click: the style applies (indent, weight, size, italics per
> the published books' grammar) and the RTF export carries it.

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
   manuscript. **Matched from Geshe Michael's corpus, never composed**:
   the sidebar can only give you sentences Geshe Michael actually wrote.
5. If a query is malformed, the sidebar says "Query not
   understood" and why — it never silently searches something
   else.

*Worked example:* type `bden pa` ("truth") and press Find in
corpus. Hits arrive with Geshe Michael's English bold beside each Tibetan
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
4. Click any term in the anchors panel for its **corpus
   concordance** — the card first states how many corpus segments
   contain the term (the measured total, never a page count), then
   shows the opening few with Geshe Michael's English beside each.
   When more exist than the card can list, it says how many are
   being shown and how many there are, and the per-course breakdown
   beneath says which rows it was computed over. Narrow the term to
   reach the rest. A "back" link returns to the clause's anchors.
5. On an anchor's **technical spelling** link, a click inserts the
   apparatus form at your draft cursor — the pronunciation
   spelling plus "(technical spelling: …)" — the published house
   convention (STD-002), typed for you.

*Worked example:* paste `/ /blo sbyong snyan brgyud chen mo'i
'khrid yig /sems can thams cad bde ba dang ldan par gyur cig /`,
press Load source, click the second clause. The anchors panel
shows *sems can* banded in one hue with its English chip in the
same hue. Click *sems can* to see its corpus concordance.

Below the corpus hits, the concordance shows **84000 Translation
Memory comparanda** — up to five aligned Tibetan–English segments
from 84000's published canon translations (400,745 segments across
388 Kangyur/Tengyur texts, CC BY 4.0 per 84000's own Terms of Use;
ingested 2026-08-20). Each carries its Tohoku number, Degé folio
where the source records it, and a link to the full translation at
84000.co. These are *published-translation comparanda, reference
only* — another team's renderings beside Geshe Michael's, never
mixed with them. The first concordance click after an update
builds the search index (a few seconds); after that it's instant.


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
- **"the master has translated this clause"** with the published
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
3. *What you see:* how many prior renderings of those words exist —
   the measured count of segments from **Geshe Michael's corpus
   (binding)** —
   and the opening twelve of them, plus any matches from the Align
   pane's harvest, which are translator-authored and **labeled
   PENDING**. When the corpus holds more than are listed, the card
   says how many. The beginnings of a Project translation memory,
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
- **AI back-check (API, labeled AI)** — one of the app's few
  network-touching features (see the Analysis chapter for the full
  list). Produces a coverage report only — omissions
  and additions between source and draft — and is **forbidden by
  construction to retranslate**. Output is banner-labeled AI. The
  button is disabled (with a tooltip saying why) unless an
  ANTHROPIC_API_KEY is configured.


### The Apparatus pane — the published bank itself

The **Apparatus** pane (same Translate group) is the bank the
search box reaches: all 2,234 published footnotes and the
bibliography, scrollable and searchable, official tier only —
what shipped in the released volumes, nothing pending.

Under a footnote's text you may see **TIBETAN NAMED IN THIS
NOTE** — the Tibetan term(s) the note is talking about, each in
Tibetan script with its wylie and the evidence it was located
from. These are **machine-located candidates, never a ruling**:
either the note itself quotes the Tibetan in its own body (the
evidence line shows that sentence), or — added 2026-08-20 —
Geshe Michael's own English equivalent for a dictionary entry is
exactly this footnote's lemma (the evidence names the gloss, its
tier, and marks PROVISIONAL tiers plainly). A third route (also
2026-08-20) adds **passage evidence** for notes from the three
published single-book sources: where the lemma appears verbatim in
the published English of the note's *own work* (the aligned corpus
carries each book under one course), the matching passages appear
under "WHERE HIS PUBLISHED ENGLISH USES THIS LEMMA" — explicitly
the passage, never a term equivalence; you read the Tibetan
yourself. 444 of the 2,234 notes carry candidates today; the rest
stay honestly bare rather than guessed at.

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
