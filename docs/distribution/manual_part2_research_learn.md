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

### Searching the Library, and searching the whole Mac

Two more reaches, both restated from honest sources:

- **search the Library** points the folder search at your installed
  Library (the 2,700+ texts of ACIP Release 6). The Library carries
  a prebuilt full-text index, so answers come back instantly instead
  of file-crawling; after you install new texts, refresh the index
  with **Read → Library → Update search index** (a full rebuild of
  all 8.68 million lines takes under two minutes).
- **Search this Mac (Spotlight)** extends the hunt to everything on
  the machine — every text, PDF, and Word file macOS has indexed.
  Your query is stripped to plain phrases and searched *as typed*
  AND *as Tibetan script*, converted through the proven wylie and
  ACIP chains, since files on disk may be in any of the three. The
  results list the matching documents with their locations; click
  one to open it in its default application. (If Spotlight's index
  skips a volume or file type, the pane says so rather than
  pretending completeness.)

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
4. **A pronunciation in the master's convention**: type `jangchub`.
   There is no headword spelled that way, so the pane matches by
   sound and finds *byang chub*, with the green note "matched by
   **pronunciation** (GMR convention) — no exact headword for
   'jangchub'".
5. **A community spelling**: type `gonpa`. The colloquial register
   widens the search and finds *dgon pa*, noting "matched by
   **colloquial pronunciation** (community usage register)". The
   register only widens lookup — the GMR convention stays canonical.
6. **English, in reverse**: type an English word (say `suffering`)
   and the results end with an **English → Tibetan** section from
   the HGM reverse index: each Tibetan candidate with its
   pronunciation and its tier label — `[HGM (curated)]`,
   `[HGM (glossary)]`, or a red `[PROVISIONAL]` for auto-aligned
   matches, which are candidates, not rulings.

7. **Affix-stripping fallback**: type `po'i`. There is no such
   headword, so the pane strips the affixed particle and shows
   *po*, with the note "no entry for 'po'i' — showing **po**
   (affixed particle stripped)". Nothing is silently substituted —
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
