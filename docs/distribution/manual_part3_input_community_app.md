# PART THREE — INPUT, COMMUNITY & THE APP ITSELF

*The Diamond Cutter Translation Tool User Manual, Part Three of Three.
Asian Legacy Library · Asian Classics Input Project.*

This part walks you, click by click, through the last two workflow
groups — **Input** (the Input and Scan panes) and **Community** (the
Propose and Approval panes) — and then through everything that
belongs to the application as a whole: the window and menu
structure, the About box, Check for Updates, the Settings dialog,
the Help window, night mode, session restore, and the built-in
self-test.

Paths are written as **Workflow → Pane**: "Input → OCR" means
click the **Input** tab in the window's top row, then the **OCR**
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
**Input menu → Input submenu** — see Chapter 17.)

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

> **The folio strip (2026-08-20).** In block mode a thumbnail rail
> runs beneath the editor — every page of the block as a small
> image, a ✓ on pages that already carry typing, the current page
> highlighted. Click any thumbnail to jump there. Thumbnails fill
> in lazily, so a 400-page volume opens instantly.

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
  pipeline on the page. Since 2026-08-20 recognition runs in the
  background: the interface stays live, the button becomes **Stop**
  while it works, and stopping keeps the lines already recognized,
  clearly labeled as partial. See §13.3.
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

1. Click **Input → OCR**.
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

# CHAPTER 14 — THE CATALOG PANE (Catalog → Catalog)

**What it's for.** Bringing in-house uncataloged material into the
official catalog. A folder of files arrives — scanned, keyed, or
inherited from an old drive — and somebody has to work out what
each file actually *is* before it can be filed. This pane does the
part a machine can honestly do, and it stops exactly where a
machine should stop.

**The charter, printed on the pane itself.** The machine LOCATES
and SUGGESTS. Humans approve. The official catalog changes only
through data releases — nothing you do here writes it. Every
suggestion below is a candidate carrying its evidence, offered so
a cataloger can judge it, never so the app can decide.

**The layout.** A banner, then two file browsers side by side —
**Uncataloged intake** on the left, **Destination — the library**
on the right — and the file panel across the bottom. Both browsers
are tree views: subfolders expand in place with the disclosure
triangle, so a box of volumes reads as the tree it is. Each tree
has its own **Choose folder…** button, its own census line, and
each file is painted with its decoded identity right in the tree
(*TD03996\_… — Tengyur (Derge) 3996*, or *— uncataloged*).

## 14.1 Choosing the folders

Click either tree's **Choose folder…** and pick its folder. Each
is walked recursively, so a folder of subfolders is fine. The
first 5,000 files are counted in the census; if there are more,
the census line says so rather than quietly showing a partial
picture.

The left tree is your intake — the material being worked through.
The right tree is where cataloged files are eventually filed; it
starts at the app's own library and can be pointed anywhere. Both
roots are remembered between sessions and reopened at launch, so
the work is where you left it.

## 14.2 Reading the census lines

Each tree's census line is its whole survey in one sentence:

> *37 file(s) · 12 identifiable by name · 25 uncataloged (16
> announce a title in their own text) · box12*

- **identifiable by name** — the filename follows an ACIP naming
  convention, so the file's catalog identity is already readable
  from its name alone.
- **uncataloged** — the filename says nothing the decoder can use.
- **announce a title in their own text** — of those uncataloged
  files, how many open with a title page the machine can read.
  This is the number that matters for planning: it separates the
  files a machine pass can propose identities for from the ones a
  cataloger must take from scratch.

## 14.3 The file panel — what the machine can already tell

Click any file in either tree. For an identifiable file the panel
gives the decoded identity: collection, catalog number,
verification status, language, and whether the input is marked
incomplete. For an uncataloged file it says so plainly. Either way
you get the file's folder, **Open in the Overlay** to read the
text in the reading pane, and the file's **complete text** below —
the panel never truncates a text file. (A binary file — a scan, a
PDF — gets an honest "no preview" note instead of a garbage dump,
and a text file beyond 20 MB, far past any real ACIP text, is
capped with the cap stated.)

## 14.4 Suggested identity — how a nameless file names itself

A file whose *name* says nothing may still announce itself in its
*text*. Tibetan works open with a title page, and this pane reads
it.

Two title-page shapes are recognized, both taken from the
collections themselves:

- **the canonical bilingual head** — "RGYA GAR SKAD DU, …, BOD
  SKAD DU, «the title»," as Kangyur and Tengyur texts open (works
  translated from Chinese use RGYA'I SKAD DU the same way). The
  Sanskrit side of the head is shown too, as further evidence.
- **the title block closing in BZHUGS SO** — the shape Sungbum
  works use.

The title is then matched against every title the app knows: the
installed library's own filenames plus the catalog works table —
around 8,700 titles. The panel tells you how many it matched
against, because a thin bank must never look like a confident
verdict.

Up to five candidates are listed, best first. Each carries:

- **the score** — the share of syllables the two titles have in
  common, measured against the longer of the two. This is what
  stops a three-syllable fragment from claiming a thirty-syllable
  work.
- **the catalog number and the title as filed**, with its English
  title when the source has one.
- **the evidence line** — how many syllables of how many were
  shared, whether the titles matched exactly or by overlap, and
  which bank the candidate came from.

**What it does when it cannot tell.** A file that begins mid-text
— an extract, a fragment, a continuation page — gets no guess at
all. The panel says the file begins mid-text and needs a
cataloger's eye. Likewise a title that reads cleanly but matches
nothing closely enough is reported as exactly that: either the
bank does not hold the work yet, or the title needs a human. An
empty answer is a real answer here.

**How well it works.** Measured against the installed library
itself, where the correct answer is already known: of the files
that carry a title page, the right work is the top candidate about
81% of the time and is somewhere in the five about 90% of the
time. Roughly four in ten files carry a readable title page at
all; the rest begin mid-text. The wrong answers are mostly genuine
ambiguity — two Tengyur works can carry nearly the same title —
which is the plainest possible argument for why a human approves
every one of these. The battery re-measures these numbers on every
test run, so they cannot quietly drift.

## 14.5 The bibliography audit

The **Audit bibliographies…** button runs the first job Geshe
Michael assigned the cataloging class (December 22, 2025): every
ACIP number cited in the published Mixed Nuts bibliographies must
exist in the database — "if a book made it into a bibliography, it
was important enough that it sure as hell better be in the
database."

The report lands in the file panel: how many distinct works the
published bibliographies cite, how many are present in the
destination tree, and — the point — the **missing ones, grouped by
the volume that cited them**, each with its citation text as
evidence. Padding variants of the same number (S464, S00464) are
counted once; a malformed citation is refused rather than guessed
at.

Two honesty clauses, printed on the report: the check runs against
*this app's installed library*, which is a lower bound — a work
missing here may still exist in ACIP's master tree, and presence
here is not proof of presence there. And the audit writes nothing:
missing works are candidates for the intake workflow — locate a
witness, input, propose, approve.

**The witness hunt.** Each missing work now carries its lead:
**scans on BDRC** when the number maps deterministically (KD/KL/TD
are Tohoku numbers), otherwise **search BUDA** — a title search
built from the citation's own Tibetan span (measured: 45 of the 50
missing works yield a query from their citation). **Export missing
list…** writes the field-coded handout for the hunt — number,
citing volume, citation, search link — the sheet a session hands
to whoever is asked to check.

## 14.6 Suggested splits — the chop assist

The team's single biggest manual task is "chopping": a typed volume
holds many works, and somebody must find every boundary before the
works can be named and filed. Select a file in either tree and
click **Suggest splits…** — the file is scanned for candidate text
boundaries and the candidates land in the panel, each with its
evidence:

- the **rule** that found it — a canonical bilingual head (RGYA GAR
  SKAD DU … BOD SKAD DU …) or a title block closing in BZHUGS SO;
- the **folio** it sits on and how far into the file it lies;
- whether a **closing formula** (RDZOGS SO, DGE'O, MANGGA LAM…)
  stands just before it — corroboration that one work ended there;
- the **title** the new segment announces.

Two warnings implement the house rule *don't split what the author
didn't split*: a boundary whose title names a chapter (LE'U) is
flagged as likely a part of one work, and boundaries very close
together are flagged as possible sections of a single text (an
empowerment's parts, a broken head).

**Nothing is cut.** The assist only reads; the mother copy is never
touched. Measured against synthetic volumes built from this very
library (real texts concatenated, so every boundary is known):
about 93% of true boundaries are found, with about two false
candidates per volume — almost all from one kind of text where
"bzhugs so" is ordinary narrative. Every candidate needs a human
eye; that is the design, not a shortcoming.

## 14.7 Compose name — the house filename grammar

When a text has been identified, it needs its proper name. Click
**Compose name…** and fill the four fields:

- **Catalog number** — from the registrar. The app never mints a
  number; the field is free text because issuing numbers is a
  human's act.
- **Tibetan title** — prefilled from the selected file's own title
  page when it has one. A trailing BZHUGS SO is stripped
  automatically; it is never part of a name.
- **English title** and **Author (dates)**.

The preview updates as you type, applying the house rules: fields
joined by underscores, capital .TXT, no colons (a single hyphen, no
spaces, stands in), IAST diacritics folded to plain ASCII.

**Over-long names.** A filename past the filesystem's 255-character
limit is filled to the maximum and stopped *even mid-word*, a `+`
marks the cut, and a companion **NUMBER META.TXT** in the same
folder carries the rest of the name from the exact cut point — so
rejoining is pure concatenation. This is precisely the library's
own convention: 1,457 existing `+.TXT` names with their META
companions. (A name that fits gets no `+` and no META — the
machinery exists only for names that are too long.)

**Rename in intake…** applies the name to the selected file — in
the intake tree only, never the destination — after a preview and a
confirmation. An existing name is refused, never overwritten, and
nothing is ever deleted. **Copy name** puts the composed name on
the clipboard for use anywhere else.

## 14.8 Generate catalog list — the ASCII inventory

In session 3 Geshe Michael set the task plainly: "We have the
database, we have the folders, but we don't have a separate LIST of
what's in the folders." **Generate catalog list…** writes that
list, over whatever the destination tree currently holds, to a file
you choose.

The format follows the St. Petersburg lineage — field-coded plain
text, one record per file, readable by any computer forever:
`S:` catalog number (or "uncataloged") · `F:` filename · `D:`
folder · `T:` Tibetan title (`T*:` when it had to be read from the
text's own title page — marked, never silent) · `E:` English title
· `A:` author · `V:` verification status · `P:` size, lines, and
the folio range seen in the text.

The header states what the file is: a LIST of the folders, not the
official catalog — the catalog changes only through data releases.

## 14.9 Translate title — the attestation workbench

Translating a title is the translator's work — but the published
record already carries thousands of GMR's own title renderings, and
consistency with them is the house standard ("people are going to
emulate your work"). **Translate title…** brings that record to the
bench.

Type (or accept the prefilled) Tibetan title. The workbench answers
with two kinds of evidence, from ~5,500 published title pairs (the
library's own bilingual filenames plus the catalog works table):

- **Whole-title matches** — titles the record already renders that
  share most of this title's syllables, each with its published
  English and its source.
- **Attested phrases** — the title is covered greedily by its
  longest phrases that appear in *other* published titles; each
  phrase card lists up to three full Tibetan–English pairs so you
  can see where the phrase landed in English. No word alignment is
  invented — the card shows whole pairs, and you read them.

Syllables with no attestation are listed plainly: *they need the
translator, not the machine.* Nothing here is machine translation —
the machine attests, you compose. Measured on held-out titles:
attested phrases cover about 90% of a typical title's syllables,
and a near whole-title match surfaces for about 62%.

## 14.10 Compare trees — the divergence audit

Session 3's live problem: Geshe Michael's copy of the database and
the team's master copy "have diverged from each other probably" —
and the task Geshe Michael set was to compare the two trees and list the
differences. **Compare trees…** runs that comparison between
whatever the two browsers are pointed at: left (intake) root
against right (destination) root, recursively.

Every file is classified: **identical** · **modified** (same
relative path, different content) · **renamed / refiled** (same
content on a different path — a text moved shelves) · **only left**
· **only right**. Ambiguity is never guessed at: identical twins on
one side stay listed as added/removed rather than being paired into
a rename.

Honesty tier, printed on the report: content is matched by size
plus sampled bytes (the first and last 4 KB), not full reads — fast
enough for whole database copies (about a thousand files in a third
of a second), and the list tells a cataloger where to look; confirm
any difference by opening the files. The comparison changes
nothing.

## 14.11 Tohoku numbers and colophon candidates

Two refinements ride inside the suggested-identity card:

**Tohoku first.** The house rule for numbering (session 4): check
the Tohoku catalog before issuing anything — "if they have assigned
one, we use their catalog number." ACIP's KD (Derge Kangyur) and TD
(Derge Tengyur) numbers *are* Tohoku numbers, so when a candidate
identification is a KD or TD work, the card says so: *Tohoku N — if
this identification is right, the Tohoku number is the number to
use; no new number needed from the registrar.*

**Colophon candidates.** The author comes from the colophon — never
assumed from the collection, never copied from a catalog without
verification. For an uncataloged file the card now scans the text's
tail for clauses carrying composition verbs (SBYAR, MDZAD, BRIS,
BRTZAMS…) or translation credits (BSGYUR, LO TSĀ BA), and lists
them nearest-the-end first, labeled for what each is evidence of.
The label on a translation credit is blunt on purpose: *the
translator is NOT the author* (session 8's warning — read past the
translator's line to find "composed by"). Measured on the library:
about 88% of real texts yield at least one candidate clause. The
machine locates the evidence; reading it is the cataloger's work.

## 14.12 Provenance banner and the cleanup lane

Clicking a file now also reads what the file's own mechanics say
about its witness, in a small PROVENANCE line:

- **folio marks** counted — a pecha input carries @NNNA/@NNNB
  sides;
- **western pagination** (page marks with no folio side) — the
  session-8 reading applies and is printed: *a typed book: suspect*;
- **lowercase runs** — "somebody typed in lowercase letters
  illegally"; search still works, fix later;
- **line-slash corruption** — the case from the August 4 session: a
  slash inserted between every line breaks cross-line search. The
  banner counts the slashed lines and offers **write a cleaned
  copy…** — the one write this lane can do. The cleaned file lands
  *beside* the original as `NAME CLEANED.ext`; the mother copy is
  never touched, and an existing cleaned copy is refused rather
  than overwritten. Mid-line slashes (real text) are left alone.

Measured on the installed library: 0 slash-corrupted files (the
lane exists for intake material), 0 lowercase runs, and 5
western-paginated typed books flagged — the signal works.

## 14.13 Load register — the three states

Catalog numbers are issued by one human — the registrar — from a
spreadsheet. **Load register…** reads that spreadsheet (CSV or TSV;
the columns are found by their header names: number/catalog, title,
folio start/end, date, initials) strictly read-only: the app never
writes the register and never mints a number.

Loading renders the register report against the destination tree:
how many issued numbers are **cataloged** there, and — the list
that matters — the numbers **issued but absent**, each with its
title, initials, and date from the sheet. This is the session-1
disease made visible: works can carry numbers for years without
ever entering the database.

Once a register is loaded, clicking any file also shows its three
state lights, which are independent by design:

1. **number issued** — the filename (or the register) says so, with
   a ✓ when the register holds the row;
2. **input exists** — the file itself;
3. **cataloged** — the file sits inside the destination tree, or a
   plain warning that it does not.

## 14.14 The change-log stamp

The house policy, stated on camera: "You have to rename the copy
with your initials, so that we know you changed it on this day —
and to blame you if something went wrong. That is our policy."

After a successful **Rename in intake…**, the app offers to stamp
the file's folder with today's date and your initials (taken from
Settings → your name): the folder becomes `Name - updated
2026-08-19 ADA`. Re-stamping replaces the previous stamp — the full
history lives in the shared storage's version history; the name
carries the latest change. A collision is refused, never forced.

Reading works everywhere: clicking any file whose folder (or an
ancestor up to six levels) carries a stamp — including the older
hand-made styles — shows *last changed DATE by INITIALS* in the
panel.

## 14.15 Move to shelf — the handoff

The two browsers are the two ends of one move: intake material,
once identified and properly named, goes onto its shelf in the
destination tree. Select the file on the left, click the target
shelf (folder) on the right, then **Move to shelf…**.

Since the in-house access layer (14.20), what this button does
depends on your role: an **approver** moves directly, as described
here; a **cataloger's** click stages the placement for approval
instead — nothing touches the official shelves until Geshe Michael
rules.

The move follows the house rules to the letter:

- **Move, not copy** — after the handoff the file lives in exactly
  one place;
- a **META companion travels with its file** — the pair is never
  split (if the companion cannot move, the whole move undoes);
- a **collision is refused**, never overwritten; nothing is ever
  deleted;
- a file whose name carries **no catalog identity is refused** with
  the workflow's order spelled out: identify, name (Compose
  name…), then shelve;
- the **shelf choice is the cataloger's** — the confirmation quotes
  the warning that drives the whole design: *"a book on the wrong
  shelf is lost forever"*;
- after the move, the app offers the **change-log stamp** on the
  shelf folder, per the policy.

Both censuses refresh after a move — the intake count falls, the
destination count rises, and the work is visibly further along.

## 14.16 The worksheet

**Worksheet…** opens the cataloging worksheet for the selected file
— built on the team's own live spreadsheet schema, all 52 columns
in the sheet's own order, from *ACIP Number* through the titles in
three languages, authorship, the BDRC identifiers, publisher,
**colophon**, physical dimensions, condition, down to *ISBN
number*. Nothing was guessed: the column list is verbatim from the
live sheet.

The form arrives **prefilled from what the app already knows** —
the number and language from the filename, the Tohoku number for
KD/TD works, the title-page title and Sanskrit head from the text
itself, the first composition-colophon candidate, the byte count
**with its MD5 checksum** (the column asks for both), the **folio
range** read from the text's own @-marks, and the **BDRC
permalink** where the number maps deterministically (KD/KL/TD).
Every prefill is a machine suggestion to verify; everything is
editable, and anything you saved before outranks the machine's
suggestions on reopen.

**Save worksheet** writes a sidecar (`<name>.worksheet.tsv`) beside
the intake file — working data, never inside the destination tree,
never the official catalog. **Export row (CSV)…** emits one line in
the sheet's own column order, ready to paste into the live
spreadsheet — which remains the team's master.

## 14.17 Propose this identity — the routing

The suggested-identity card now ends with **Propose this
identity…** — the bridge from the machine's suggestion to the
team's ordinary decision channel.

Clicking it files a proposal of kind **catalog identity** in the
shared queue (the same folder-backed channel as every other
proposal — Chapter 15), carrying everything a ruler needs: the
title read from the text and the rule that read it, the Sanskrit
side of the head, the ranked candidates with their scores, and —
if you filled one — a snapshot of the file's worksheet sidecar.
Your name rides on it as provenance (set it in Settings first).

The authority rules on it in Community → Approval, where a
**Catalog identities** filter now sits beside the others. An
approved identity leaves through the same **Export approved
candidates** file as the dictionary material — a candidates
package for the data project. The boundary holds at every step:
the app never writes the catalog, and catalog numbers remain the
registrar's to issue. A file with no title page is refused with a
real answer: there is no machine identity to propose.

## 14.18 QC intake — the two quality lanes

**QC intake…** runs two checks over the whole intake tree, both
suggestion-only — every flag is a question with its evidence, never
a verdict, and nothing is changed:

- **Title vs translation.** The copy-paste disease: a filename
  whose English half belongs to a *different* text. A file is
  flagged when its English matches another work's published English
  almost entirely while the two Tibetan titles barely share a
  syllable — the report shows both works and both percentages and
  asks the question: is the English half from the wrong text?
- **Shared titles, told apart by colophon.** Files carrying the
  same Tibetan title are grouped, and their colophons compared —
  the session lesson that two works can share a title. Verdicts
  name their evidence: *same colophon — true duplicates* ·
  *different colophons — distinct works sharing a title* · *no
  colophon evidence — needs a human read*.

Run against our own installed Sungbum, the lanes found real work:
8 mismatch flags and 200 shared-title groups (107 reading as true
duplicate filings, 18 as distinct works, 75 needing a human) — the
battery re-measures this on every test run.

## 14.19 Where might this belong? — shelf suggestions

Click **Move to shelf…** *without* a shelf selected and, instead of
a bare refusal, the panel now answers the real question: *where
might this belong?* Ranked existing shelves of the destination
tree, each with its reasons:

- *the shelf's own name appears in the title* — the subject folders
  are named by their phrases, and the file's title carries them;
- *closest title here* — the nearest neighbor by syllable overlap,
  named;
- *works by the same author already here* — counted, deliberately a
  light signal (a prolific author's biggest folder must not drag
  every file to it).

Click a suggested shelf to select it in the right tree, then Move
to shelf… again to confirm — the machine never places a book.

The honest numbers are printed on the report: measured on held-out
files of the installed Sungbum, the right shelf leads the list
about **31%** of the time and appears in the top three about
**43%** — an aid, not an oracle. That ceiling is the design's own
lesson: shelf choice is the fine-grained subject judgment the
sessions reserve for trained humans, which is exactly why the
human confirms every move.

## 14.20 In-house access — sign in, stage, approve

The cataloging workflow is **in-house only** (2026-08-20). Three
pieces make that real, all living inside one folder — the team's
**official library**, which should be the shared Dropbox-synced
folder everyone already uses. Because everything lives inside that
folder, Dropbox itself carries every staged file, every ledger row,
and every approved placement to the whole team; the app never talks
to Dropbox, it just works inside the folder Dropbox syncs.

**The roster and sign-in.** *Official library…* points the pane at
the shared folder. *Team…* creates and manages `CATALOG_TEAM.tsv`
inside it — who may use the workflow, and as whom. The first entry
created becomes the **admin** (that should be Adam); the admin adds
members, each with their own passphrase, and revokes access. Give
Geshe Michael the **approver** role. Until someone signs in, every
action button in the pane is disabled — browsing stays open, acting
does not. Honesty note: this is team discipline plus attribution
(salted passphrase hashes, every action signed with a name), not
cryptography — the Dropbox share itself is the real wall.

**Staging.** When a signed-in **cataloger** clicks *Move to
shelf…*, the file does not go onto the shelf. A **copy** goes into
`AWAITING APPROVAL/` inside the official library (the mother copy
in the intake is untouched), under its proposed catalog name, and a
row is appended to `ACTIONS.tsv` beside it: who, when, which file,
which proposed shelf, and the evidence. The ledger is plain ASCII —
Geshe Michael can read the list of actions in any editor, forever.

**Approving.** *Approvals…* shows the pending list — every staged
action with its actor, proposed name, proposed shelf, and evidence.
Anyone signed in can read it; only an **approver** (Geshe Michael)
can rule. *Approve* moves the staged file onto its proposed shelf
(the placement then syncs to the whole team); *Reject…* moves it to
`AWAITING APPROVAL/REJECTED/` with the reason recorded in the
ledger — nothing is ever deleted. An approver's own *Move to
shelf…* still acts directly, exactly as before.

## 14.21 What the workflow will do next

The workflow's core is complete: census, identity, audit, splits,
naming, the ASCII list, the title workbench, tree comparison,
register, provenance, worksheet, routing, QC, and the handoff all
ship above. What comes next grows from use — the team's own
sessions will show where the workbench should deepen.

---

# CHAPTER 15 — THE PROPOSE PANE (Community → Propose)

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

## 15.1 One-time identity setup

Do this once, on your first day (a coordinator will usually tell
you the folder path):

1. Click **Community → Propose**.
2. In **"Who you are (one-time)"**, type your name into **Your
   name**. It is stamped on everything you file — that is its
   only job.
3. **"I am an authority (can approve) — Geshe Michael / Adam
   only"** (checkbox): leave it OFF unless you are the authority.
   Ticking it (and restarting) reveals the Approval pane
   (Chapter 16).
4. **Proposals folder**: click **Choose…** and select the team's
   shared proposals folder (the dialog hints: "Dropbox-synced is
   ideal"), or paste the path into the field.
5. Click **Save identity**. A confirmation appears — "Identity
   saved." (For an authority it adds: "The Approval tab is now
   available (restart to show it).")

These same three settings also live in the **Settings dialog**
(View menu → Settings…, under *Team & proposals* — see §16.4);
change them in either place, they are one and the same.

## 15.2 Making a proposal

The **"Make a proposal"** box is a five-field form:

1. **Kind** (dropdown) — what sort of thing you are proposing.
   The form offers ten kinds (two more ride the channel from
   elsewhere: spelling flags filed by the Overlay's doubt marks,
   and catalog identities filed from the Catalog pane's
   suggested-identity card). The ten, and what each means:
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

## 15.3 The other ways proposals get filed

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

## 15.4 My proposals — what happened?

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

# CHAPTER 16 — THE APPROVAL PANE (Community → Approval)

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

## 16.1 Reading the queue

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

## 16.2 Ruling on an item

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

## 16.3 What each ruling actually does

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

## 16.4 The kind filter — one category per sitting

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

## 16.5 Export approved dictionary candidates…

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

## 16.6 The Rulings archive

Click **Rulings archive**. The queue area becomes the permanent,
read-only record of **every** decision, newest first: status in
color (approved green, declined maroon, deferred grey), kind,
Tibetan, value, who proposed it and when, **who ruled it, when,
and the note**. Nothing is ever silently discarded — over the
years this is a record of the authority's own judgments. Click
**Refresh queue** to return to the pending view.

---

# CHAPTER 17 — THE APP ITSELF

> **New in the Word-style wave (2026-08-20):** an **Edit menu**
> (Undo/Redo, Cut/Copy/Paste/Delete, Select All) acting on
> whichever editor has focus; a visible **🔎 Hunt (⌘K)** button
> beside the group tabs; and the **Quick Access Toolbar** — the
> thin strip above the tabs where the ★ menu lets you pin any
> command from any pane, kept one click away everywhere. The
> Catalog, OCR, Library, and Manuscript panes now present their
> tools as a **ribbon** — labeled groups in one strip, captions
> beneath — with more panes following.

This chapter covers everything that belongs to the application as
a whole rather than to any one pane.

## 17.1 The window: seven workflow groups

The window's **top row** holds the seven workflow tabs; inside
each, a **second row** holds its panes — twenty-two in all:

| Workflow | Panes |
|---|---|
| **Read** | Overlay, Library, Files, Scans, Export |
| **Translate** | Draft, Manuscript, Apparatus, Review, Align |
| **Research** | Search, Lookup, Sanskrit, Convert, Analysis |
| **Learn** | Trainer, Drills |
| **Input** | Input, OCR |
| **Catalog** | Catalog |
| **Community** | Propose, Approval |

"Input → OCR" always means: click **Input** in the top row, then
**OCR** in the row inside it. (Approval appears only for the
authority role, and carries a live pending count on its tab.)

## 17.2 The menu bar mirrors the window

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

## 17.3 The app menu: About, Check for Updates, Settings

On a Mac these three live in the application menu (the one named
after the app); they are also under **View** on every platform.

**About Diamond Cutter Translation Tool** opens the About box:

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
- the build line (C++20, the Qt version, the number of automated
  test batteries — counted at build time, so it is always current
  — runs fully offline), a contact email link, and a
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

## 17.4 The Settings dialog — every field

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

## 17.5 The Help window

**Help menu → "Diamond Cutter Help & Tutorials…"** (or the standard help
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

## 17.6 Night mode

**View menu → Night mode** (checkable), or the same switch in
Settings → Appearance. Dark chrome, cream reading pages — the
reading surfaces stay warm and paper-like while the frame goes
dark. It applies immediately and is remembered across launches.

## 17.7 Session restore — the app reopens where you left off

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

## 17.8 The self-test (--selftest)

If anything ever misbehaves, the app can examine itself. Open
Terminal and run the app binary with the `--selftest` flag:

    "/Applications/Diamond Cutter Translation Tool.app/Contents/MacOS/Diamond Cutter Translation Tool" --selftest

The app constructs its real panes offscreen against the real data
and exercises them — 203 checks at this writing, 2026-08-20 (the battery grows
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
scenes, fifty-five automated test batteries (as of 2026-08-20; the
count grows with every feature) also run before any
change to the software ships.)

---

*End of Part Three — and of the manual. Seventeen panes, one
principle: the machine may match the master's English — it may
never compose it.*


## 17.9 Help → Troubleshooting

Everything a tester needs to file a useful finding in one click:

- **Show Logs in Finder** — reveals the lifecycle log (launches,
  quits, and — with Verbose Logging on — every pane switch).
- **Show Data Folder in Finder** — opens the Diamond Cutter Tool Data folder.
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


## 17.10 Hunt Everywhere (⌘K)

Press **⌘K** anywhere (or View → Hunt Everywhere…). Type anything
in any form — *bsod nams*, *BSOD NAMS*, *merit*, or *sunam* — and
one list answers from every source at once:

- **📖 dictionary** rows with the binding gloss and its tier;
- **🔁 English reverse** rows (your English → the matching Tibetan);
- **🗣 phonetics** rows through the GMR-convention fold;
- **📜 corpus** rows with the master's published English;
- **🎧 teaching** rows that open the recording at the timecode;
- **📎 apparatus** rows — the published footnotes and
  bibliography entries;
- **👤 person** rows — the authors layer: type any part of a
  name (*gzhon nu dpal*, an anglicized fragment, a BDRC variant)
  and matching authors appear with their dates and catalog work
  count; Enter opens the person's BDRC page (with Treasury of
  Lives where linked). Their works surface right below through
  the file lane — the same query finds both;
- **📁 file** rows — Library filenames that match what you typed
  (a catalog number like *TD04156*, a title fragment): Enter
  opens the file itself — texts in the Overlay, scan images in
  the Input viewer.

Arrow to a row and press **Enter**: dictionary/reverse/phonetics
rows land you in Lookup with the search already run; corpus rows
land you in Search; teaching rows open the recording. Escape
closes. Everything is the same deterministic machinery as the
panes — the palette only saves you the walk.
