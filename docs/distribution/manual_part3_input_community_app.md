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
- **zoom** (slider, 25–200%) — scales the scan image. 100% is
  actual size.
- **@ next folio** — inserts the next folio marker per the ACIP
  specification. See §12.5.
- **Compare with partner file…** — the double-keying diff. See
  §12.7.
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

# CHAPTER 13 — THE SCAN PANE (Input → Scan)

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

- **With the search box empty**, the left list shows every
  tutorial **chapter** (marked 📖) — one per pane, plus Getting
  Started, Suggested Workflows, and Concepts. Click a chapter to
  read it.
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
