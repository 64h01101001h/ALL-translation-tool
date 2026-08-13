# The ALL Translation Tool — Complete Feature Guide & Workflows

*Asian Legacy Library · Asian Classics Input Project — August 2026*

*Every pane, every control, and how to put them together. Companion
documents: the White Paper (the mission and the design) and the
Technical Paper (under the hood). The same material lives inside the
application — Help menu → "ALL Tool Help & Tutorials…" — with a
search box that finds any button and shows where it lives.*

---

## 1. Introduction: how to read this guide

**The three rules.** Everything in this guide obeys three commitments
you will see restated pane by pane, because they are the design:

1. **Geshe Michael's English is the binding layer.** His equivalents
   are the authority everywhere; all other scholarship appears as
   clearly-labeled reference. The machine may *match* his English
   from his own corpus; it never composes English in his name.
2. **The tool never guesses.** Failures are flagged, provisional
   material *looks* provisional (an amber tint), AI output is
   banner-labeled, recognition output is review material only.
3. **Nothing enters the official data without approval.** Proposals
   wait in the Approval queue for the authority's ruling.

**The six workflows.** The window's top row holds six workflow tabs;
inside each, a second row holds its panes — seventeen in all:

| Workflow | Panes | In one phrase |
|---|---|---|
| **Read** | Overlay, Library | the canon, read with the dictionary underneath |
| **Translate** | Manuscript, Draft, Review, Align | the page, the bench, the overseer, the dictionary-builder |
| **Research** | Search, Lookup, Sanskrit, Convert, Analysis | find, look up, convert, analyze |
| **Learn** | Trainer, Drills | the apprenticeship, instrumented |
| **Input** | Input, Scan | the input-center workstation and recognition |
| **Community** | Propose, Approval | the team proposes; the authority rules |

Paths in this guide are written as **Workflow → Pane** ("Translate →
Draft" means: click the Translate tab, then the Draft tab inside it).
The menu bar mirrors this structure exactly — one menu per workflow,
one submenu per pane, every function inside — and on a Mac the
system Help search finds any menu item and points an arrow at it.

---

## 2. READ

### 2.1 Overlay — the flagship reader

*Read → Overlay. Reading a Tibetan text with the dictionary
underneath.*

**Loading text — two ways.**

1. *From the Library* (easiest): Read → Library → double-click any
   text. The app switches to the Overlay for you.
2. *Paste*: click the SMALL editable box at top-left (under
   "Document (ACIP)"), paste ACIP or wylie — the app detects which —
   then press **Load into overlay**. The big right-hand area is the
   read-only display; you cannot paste there. **Open ACIP file…**
   opens a file directly.

**Reading the display.** Per Geshe Michael's direction the text
opens CLEAN. Click any word: the LONGEST phrase the dictionary knows
at that spot highlights in gold (amber-leaning when the entry is
provisional); click again to step down to the next shorter phrase
inside, dropping a particle at a time. The Display panel's "shading"
control also offers ALL MATCHES — the coverage view where every
known word and phrase is washed in color, nested and tier-tinted.
In both modes: red wavy underline = the syllable fails the classical
legality rules; dotted amber = a particle disagreeing with the
preceding suffix (a review hint, never an auto-correction). A
complete canonical volume opens in about two seconds.

**The card.** Click any shaded word and the panel below shows its
entry: the quality tier (curated / glossary / provisional); **the
concordance of Geshe Michael's actual renderings, course by
course**; reference definitions marked as references; Sanskrit
equivalences (Mahāvyutpatti); attestation from the Monlam word
lists (so an unknown word is identified as a real word awaiting an
entry, or a probable typo); verb tense paradigms; expansions of
manuscript abbreviations and bsdus-tshig contractions; the honorific
badge with the ordinary counterpart beside it; community
pronunciations ("also heard"); published footnotes that deal with
the term; your own per-text glossary, editable in place; and curated
links out (84000, BDRC, Lotsawa House). **"Propose to the
authority…"** on the card files a proposal with the passage
auto-attached as evidence (§7).

**Display panel.** "text as": ACIP / Tibetan script / Wylie — same
shading in all three. A typeface picker (thirteen vetted Tibetan
faces) applies in script mode. Each reference layer toggles on and
off; all choices are remembered across sessions.

**REVIEW (gold heading).** **Show spelling doubts** opens a small
docked list of every syllable that fails the legality rules — the
same doubts the red waves mark, gathered as a working list: one row
per distinct form with its count and wylie. Click a row and the text
jumps to that syllable, selected; click again to cycle its
occurrences. Hover for the context. Check rows and press **File for
ruling**: each goes to the authority's Approval queue as a potential
input error, context attached as evidence. When the authority
APPROVES a flag it is a confirmed error, exported for the input
teams (the app never edits the texts); when the authority DECLINES
one, the form has been ruled valid and the app stops doubting it —
everywhere, for the whole team. **Check all** checks every row.

**SCANS (gold heading).** **Follow along in scans (BDRC)** opens the
original woodblock page images beneath the text; the pages turn
following the folio marker at your cursor, with the current line
banded (its placement honestly approximate). **◀ folio / folio ▶**
page by hand. **locate word (OCR)** draws a box around the very word
under your cursor on the woodblock — recognition used only to
locate, never as text. **Search BDRC** finds the scan volume for the
open text; **Use selected** saves the link for next time.

**FORMAT & EXPORT (gold heading).** **Export print Tibetan
(Unicode)…** produces print-ready Tibetan script per the Project's
conventions. **Prepare for translation (Mixed Nuts)…** reproduces
Geshe Michael's demonstrated translation-preparation format.

**Teachings and references on every card.** Entry cards now carry:
a **"Geshe Michael teaching this term"** section — timecoded links
into his recorded classes (machine-located from class captions; the
recording is the authority; non-English sessions tagged); a **"He
says this word"** tier locating moments where he speaks the Tibetan
itself (phonetic match on his own convention — candidates,
homophones share moments); and a **"Das 1902 · ~p.N"** link opening
Sarat Chandra Das's dictionary inside the app at approximately the
entry's page (public domain, clearly a reference). And in REVIEW:
**"Teachings for this text…"** sweeps every term the loaded text
contains and reports where he taught each one — the translator's
companion sheet.

*Try this now:* Read → Library, double-click **S0134**. When it opens
shaded, switch "text as" to Tibetan script, click **sems can**, and
read his renderings course by course. Then turn on "Show spelling
doubts" and see what the legality layer doubts in a real input file.

### 2.2 Library — the preserved canon

*Read → Library. 8,986 texts and 14.1 million lines of the current
public Kangyur, Tengyur, and Sungbum releases, plus your own materials, on your own disk.*

Browse the folder tree or the flat **List view** (a sortable
catalog: every filename decoded into collection, text number,
sub-number, verification level, with English titles and bilingual
subject classifications from the Project's catalogs).
**Double-click a text → it opens in the Overlay.** The tree
remembers which folders you had open, across restarts.

Controls: **Install collection ZIP…** installs the canon from the
Library's own archive files. **Import my materials…** brings in your
own texts (Word documents convert automatically). **Legacy font
rescue (UTFC)…** converts documents typed in pre-Unicode Tibetan
fonts of the 1990s to proper Tibetan script. **Send to OCR…** routes
a scan to the Scan pane. **Update search index** refreshes the
full-text index the Search pane uses (a full build indexes all 14.1
million lines in a few minutes).

---

## 3. TRANSLATE

### 3.0 Manuscript — the writing surface

*Translate → Manuscript. The manuscript itself, with the corpus one
keystroke away.*

Write in the big editor — **B / I / U** and the size control apply
to the selection, and pasted Tibetan script keeps its shape. **Save
as…** names the manuscript (stored losslessly as .html); after that
it autosaves every minute, **Save** is always there, and **Export
RTF…** produces a file for Word or Pages. The pane reopens your
last manuscript on launch.

The **search sidebar** (toggle it with "Search sidebar") speaks the
full Gofer grammar — `bden pa`, `stong OR bden`, `"sdug bsngal"
NEAR/5 "bden pa"` — over the aligned corpus. Every hit shows the
master's English in bold beside the Tibetan, and **insert** places
that English at your cursor: matched from his corpus, never
composed.

### 3.1 Draft — the working translator's bench

*Translate → Draft.*

Paste the source and press **Load source**. The text splits into
clauses in the middle list. Click a clause and its **anchors**
appear on the right: the clause in Tibetan script with each
established term banded in a color, and each term's English chipped
in the SAME color — which English goes with which Tibetan reads by
color. Click a term for its full corpus concordance. Select any
Tibetan and press **Phrase memory**: every prior rendering of those
words — his corpus (binding), plus Align-pane harvests labeled
PENDING.

**STRUCTURE (gold heading).** **Extract outline (sa bcad)** pulls
the text's own topical outline from its internal markers.
**Structural units (bam po / le'u)** finds the canonical divisions
and numbered chapter colophons, with shloka reckonings labeled as
the estimates they are. **Verse meter** names the meter and flags
broken lines. **Verse reading order** gives stanza-by-stanza
guidance with the verb named.

**EVIDENCE (gold heading).** **Detect quotations** finds passages
quoting works that exist in the corpus and shows the published
rendering of those very lines, with citation. **Check terminology**
audits your English draft: established equivalents that never
appear, one term rendered two ways, reliance on provisional glosses
— flagged for review, never auto-corrected.

**PUBLISH (gold heading).** **Propose footnote (pending GMR
approval)** files a note into the pending queue — there is
deliberately no approve button in the software, because approval
belongs to the publishing process. **Compose bibliography entry…**
assembles an entry in the exact house format, filling itself in from
the Project's catalog for 1,846 known works by ACIP number.
**Export draft (RTF, \*italics\*)…** produces the formatted file.
**AI back-check (API, labeled AI)** produces a coverage report only
— omissions and additions — and is forbidden by construction to
retranslate.

### 3.2 Review — the overseer's bench

*Translate → Review. For the senior translator checking someone
else's draft.*

**Open source…** / **Open draft…** (or paste): Tibetan left, English
right. Press **Run review**. The report guides attention without
issuing verdicts: register-sensitive terms (confirm the rendering
fits this text's register — prayer and philosophy differ);
renderings resting on provisional glosses; established terms absent
from the draft (possibly deliberate — worth a look); one English
word serving two Tibetan terms; terms with three or more established
senses; honorific terms in the source ("check the English carries
the register of respect"); and published footnotes that apply —
reuse, don't rewrite.

### 3.3 Align — building tomorrow's dictionary

*Translate → Align. The Project's original Hypercontext authoring
workflow, reborn.*

**Load pasted texts** (or **Open ACIP file…**) for the Tibetan and
English sides. Click a Tibetan word — the word-finder cycles through
possible word lengths — select its English, and press **Link
(space)**. Finer sublinks nest inside longer links in their own
color. **Delete last link** undoes. Links save per text.
**Import .hyp…** reads hypertexted files from the original
Hypercontext era (proven against the original tutorial file).
**Export aligned pairs (PENDING)…** produces candidate word pairs
for the dictionary project — translator-authored attestations,
always labeled PENDING until a human review admits them.

---

## 4. RESEARCH

### 4.1 Search — Gofer, restored

*Research → Search. Geshe Michael's daily search tool for thirty
years, rebuilt with the same algorithms and its original layout.*

Type terms into any of the eight "go for" boxes. Choose the
combiner: **OR** (any term), **AND** (all terms in the same file),
or **NEAR** with a proximity (all terms within N lines). Check the
targets: the **HGM aligned corpus** (his English beside the
Tibetan), the **published apparatus** (all 2,234 footnotes and 705
bibliography entries), and any folders — **Add / Remove /
Duplicate** manage the folder list exactly as the original Gofer
did, and it persists. **Find** runs the search (**Stop** interrupts);
results land in the Search Results tab with hits per target.
**Save** stores the whole search by name in the Saved Search tab —
double-click to reload. The library folder row uses the prebuilt
index for instant answers; the opt-in **This Mac (Spotlight)**
source row extends the hunt to everything on the machine,
searching each term as typed *and* as Tibetan script through the
proven converters — results open in their own application.

*Try this now:* terms `sdug bsngal` and `bden pa`, combiner NEAR,
proximity 5, target the corpus → Find. His English arrives beside
every hit.

### 4.2 Lookup — the stacked dictionary

*Research → Lookup.*

Type a headword — wylie, Tibetan script, ACIP, a pronunciation in
the master's convention, or a community spelling like "gonpa" — and
press Return. Results stack with **HGM's entry first and binding**,
then each reference layer labeled. Affix-stripping fallback (*po'i*
finds *po*). English→Tibetan reverse lookup. Honorific badges with
the ordinary counterpart, community pronunciations, published-
footnote flags. **Browse the dictionary (Tibetan order)** lists all
headwords in traditional collation. **"propose to the authority…"**
at the bottom files a proposal for the searched term.

### 4.3 Sanskrit — the whole stack

*Research → Sanskrit.*

Enter IAST (*pramāṇa*), Devanagari, or ACIP input-code (*prama#n%a*)
— auto-detected — and press **Analyze**: the full cascade renders
(both official ACIP input-code styles, Tibetanized ACIP, Tibetan
script, Devanagari, IPA), then Whitney's root analyses — all 937
roots of the 1885 *Roots*, with past-participle reverse lookup
(*gata* finds *gam*) and section citations into the 1879 *Grammar* —
the Mahāvyutpatti bridge to classical Tibetan, and the
Monier-Williams link-out. **Sanskrit OCR…** reads a Devanagari image
into the same pipeline (uses the optional tesseract install;
instructions appear if it is missing).

### 4.4 Convert — every writing system, and time itself

*Research → Convert.*

Type in any system — ACIP, wylie, IAST, Devanagari; the pane detects
which — and every representation renders at once, including the
pronunciation in Geshe Michael's convention and the simplified
pronunciation per the Project's published standard. Sanskrit roots
in the input are annotated from Whitney. The **calendar tools**
answer dating in every direction — western year to full Tibetan
designation, element-animal pair to its years, complete Tibetan date
to western weekday and back — honoring the traditional calendar's
own irregularities rather than smoothing them.

### 4.5 Analysis — the eighteen-section report

*Research → Analysis. One of only two features that touch the
network.*

Paste a passage and run. The deterministic engine first extracts
everything *verifiable* — segmentation, established term anchors,
corpus matches — then an AI model writes the full eighteen-section
grammatical report, and the machine re-checks the result:
reconstructing the segments, verifying every dictionary quotation,
confirming the Tibetan was not altered. Output is banner-labeled AI,
always. Costs cents per passage; reports save to the analyses
folder.

---

## 5. LEARN

### 5.1 Trainer — reading, layer by layer

*Learn → Trainer.*

Paste a passage, press **Load**, and attempt it yourself. Then
reveal help one layer at a time with the checkboxes:

1. clause and phrase boundaries;
2. each particle's grammatical role;
3. suggested reading order — find the verb first (identified from
   the dictionary's own tense evidence, or honestly marked
   unverified), with connected phrases marked to read together;
4. vocabulary with quality tiers;
5. **the answer key** — Geshe Michael's own English wherever the
   passage exists in his corpus; the machine never invents answers;
6. the full Wilson parse — every unit designated, case numbers
   assigned by verb class, every syllable dot named per the Science
   of the Dots, reproducing the book's own diagrams.

### 5.2 Drills — exercises that write themselves

*Learn → Drills.*

Pick a drill type; **New drill** generates the next one; **Check /
Reveal** grades or shows the answer. Every exercise comes from a
real corpus segment, so every answer is the master's own text:
restore a scrambled clause; fill the blank given his English; choose
the correct particle variant (explained from the classical tables);
parallel reading (reveal his English only when needed); spaced
vocabulary review built from the words you clicked while reading;
and Translate & compare — render an unseen passage, then see his
rendering beside yours with a terminology comparison. Nothing grades
style; the master's version teaches. Progress stays in a local file
that belongs to the student.

---

## 6. INPUT

### 6.1 Input — the input-center workstation

*Input → Input. The ACE lineage, recovered and rebuilt.*

**Open scan folder…** turns a folder of page scans into a work queue
(page N of M; each page's typing saves the moment you move on,
restores when you return), or **Open scan…** for a single page. The
scan sits above, the typing area below, and the scan follows your
cursor — line-exact once **Detect lines (OCR)** has looked at the
page, approximately otherwise. Illegal syllables underline in red as
you type. **@ next folio** inserts the next folio marker in the
Project's exact specification. **Pre-fill from OCR (draft)** drafts
the page for you — ONLY onto an empty page, never over your typing —
banner-marked as review material to *correct* against the scan.
**Compare with partner file…** is the double-keying discipline:
every discrepancy lights up (orange = only in yours, green seam =
partner has extra), corrected until the two files match exactly,
which has always been the rule. **Save…** exports the block.

### 6.2 Scan — text recognition, embedded and proven

*Input → Scan.*

**"mark illustration candidates"** outlines folio regions not
covered by detected text lines — where woodblock miniatures live —
and **"Illustration gallery…"** runs that search across a whole
folder, showing the crops as a gallery (save them named by source
page). Candidates only, never claimed complete. The Overlay's SCANS
column offers the same gallery over a linked volume's cached pages.

**Open scan image…** then **Run OCR**: lines are detected and
recognized with BDRC's open models running locally (used with
permission, credited wherever output appears). Every line renders in
transliteration and Tibetan script with an immediate quality check
counting the syllables that fail the legality rules. Click a word in
the reading and it lights up on the woodblock. **Batch folder…**
processes a whole scanned volume. **Save to ocr_out…** stores the
output — tagged unverified review material that never enters the
corpus and is never smoothed.

---

## 7. COMMUNITY

### 7.1 Propose — the team's channel to the authority

*Community → Propose.*

One-time setup at the top: your name, whether you hold the authority
role (Geshe Michael / Adam only), and the shared proposals folder —
point it at the team Dropbox and the whole team's queue synchronizes
with no server, no accounts, offline-capable and fully audit-trailed.
(**Save identity** stores it; the same three settings live in
Settings, §8.) Then propose: pick the kind — honorific, HIGH
honorific, humilific, double honorific, pronunciation exception,
abbreviation, word/phrase rendering, note — enter the Tibetan, the
proposed value, and evidence, and press **File proposal for
review**. Proposals also file from any entry card ("propose to the
authority…") with the passage auto-attached, and from the Overlay's
spelling-doubts list (§2.1). **My proposals — what happened?** shows
each of yours with its status and the authority's comment.

### 7.2 Approval — the authority's queue

*Community → Approval. Visible only with the authority role; the tab
carries a live pending count.*

Each pending item appears with its evidence, source citation, and
proposer. **Approve / Decline / Defer** — one click, plus an
optional note. The data boundary is enforced in the software:
register items (honorific, pronunciation, abbreviation) apply in the
app the moment they are approved, stamped with who ruled and when,
for every student everywhere; dictionary items are approved but
never written into the corpus — **Export approved dictionary
candidates…** produces the signed package for the data project's
next release. Spelling flags have their own filter: APPROVE confirms
the input error for export to the input teams; DECLINE rules the
form valid, and the app stops doubting it from the next launch on.
Filter by kind to take one category per sitting; the list keeps your
scroll position between rulings. **Rulings archive** is the
permanent record of every decision — over the years, a record of the
authority's own judgments.

---

## 8. Settings, menus, and Help

**Settings** (View menu → Settings…; on a Mac also the app menu,
⌘,): Night mode (dark chrome, cream reading pages — same switch as
View → Night mode); the Overlay's default script (Tibetan script /
ACIP / Wylie); your name, authority role, and shared proposals
folder (mirrors the Propose pane's setup — change either place); and
a data-folder override, normally left empty. Data-folder and
authority-role changes take effect on the next launch.

**Menus.** The menu bar mirrors the window: one menu per workflow
group, one submenu per pane, every function inside — reachable no
matter which tab is in front.

**Help** (Help menu → "ALL Tool Help & Tutorials…", or ⌘?): a
searchable window with a tutorial chapter for every pane. Type any
feature's name and the results show the how-to *and* which workflow
tab it lives under — clicking takes you there. "Suggested
Workflows…" in the Help menu opens the recipes chapter directly.

**Self-test.** If anything misbehaves, run the app from Terminal
with `--selftest`: 89 checks print PASS/FAIL and localize the
problem. (thirty-eight automated test batteries also run before any change to
the software ships.)

---

## 9. Suggested workflows

Recipes for putting the panes together — first by task, then by
role. Each step names its tab path.

### By task

**Read a text (the daily bread).** Read → Library → double-click →
it opens shaded in the Overlay → set "text as" to Tibetan script →
read; click anything shaded for its card → for a hard word,
"Propose to the authority…" right from the card, evidence attached.
When you want the carving itself: SCANS → Follow along in scans.

**Translate a text end to end.**
1. Read it first in the Overlay (Read) — get the shape of it.
2. Translate → Draft: Load source; work clause by clause with the
   color-linked anchors; **Phrase memory before you invent any
   rendering** — the master may already have one.
3. Research → Search: NEAR-search the terms you are unsure of, to
   see his English in living context.
4. Translate → Draft → EVIDENCE: Detect quotations (reuse the
   published renderings of quoted lines); Check terminology against
   your English.
5. PUBLISH: footnotes (reuse first — the apparatus is searchable),
   bibliography entries auto-filled by ACIP number, then RTF export.
6. Hand it on: Translate → Review for the senior pass.

**Check someone else's finished draft.** Translate → Review: source
left, draft right, Run review — then walk the advisories with the
Overlay open in Read for any passage you want in context. The report
guides; you decide.

**Hunt a term across everything.** Research → Lookup for the stacked
dictionary view → Research → Search with NEAR for corpus and
apparatus usage → in the Overlay, click it anywhere it appears
shaded for the course-by-course concordance.

**Sanskrit deep-dive.** Research → Sanskrit: enter IAST or
input-code → Analyze → the cascade, Whitney's root, the
Mahāvyutpatti bridge. Quick conversions and colophon dates live in
Research → Convert.

### By role

**A new student's first week.**
- *Day 1:* Read → Library → open a short text; read with everything
  on. Click freely — every card teaches.
- *Day 2:* Learn → Trainer with one passage: attempt it bare, then
  reveal layer by layer down to the Wilson parse.
- *Day 3:* Learn → Drills, ten minutes — the app adapts to what you
  missed and drills the words you clicked while reading.
- *Day 4:* Read the same passage again in the Overlay with the
  woodblock following (SCANS). Notice how much less help you need.
- *Day 5:* Research → Lookup and Search — learn to answer your own
  questions. Ask your coordinator for the proposals-folder setup so
  your questions can become proposals.

**A working translator's text.** The end-to-end recipe above, with
two habits: Phrase memory before inventing, and quotations detected
before footnoting — work performed once is never redone.

**A senior reviewer's session.** Translate → Review on the draft;
keep Read → Overlay open beside it for context; for a term whose
rendering you doubt, Research → Search NEAR to see every attested
use; if the register is genuinely ambiguous, propose a note
(Community → Propose) rather than silently overruling.

**An input-center operator's day.** Input → Input: Open scan
folder…; type beside the following scan; Pre-fill from OCR only on
empty pages, then correct against the scan; Compare with partner
file… until the diff is clean; Save. Batch recognition of a raw
volume runs in Input → Scan (review material, always). Finish the
day in Read → Overlay: open the finished text, turn on REVIEW →
Show spelling doubts, and file the genuine doubts for the
authority's ruling.

**The authority's weekly sitting.** Community → Approval: filter to
one kind and take the category whole — pronunciations one week,
honorifics the next, spelling flags after an input batch. Each item
carries its evidence and proposer; approve, decline, or defer with a
note. Approved registers apply immediately, everywhere; approved
dictionary items export for the data project; declined spelling
flags quietly teach the whole team's software that the form is
valid. The Rulings archive keeps the record.

---

## 10. Appendix

**Tiers.** curated (hand-verified) > glossary (from his glossaries)
> provisional (machine-aligned; amber-tinted everywhere it appears).

**Registers.** Honorifics have four levels (per Tournadre): general
honorific, HIGH honorific (high lamas and dignitaries — rare),
humilific (one's own actions), double honorific. Pronunciations
carry classes: community, hgm-attested, prenasal-derived (pending
ruling), approved.

**Writing systems.** ACIP (the Project's uppercase input
transliteration) · wylie/EWTS (scholarly lowercase transliteration)
· Tibetan Unicode script · IAST (Sanskrit romanization with
diacritics) · Devanagari · the ACIP Sanskrit input codes
(*prama#n%a* / *prama-n.a* styles, per the official standard).

**Provenance.** Everything machine-derived says so; every approved
item carries who approved it and when; every mined fact carries its
source and page. The app imports versioned data releases and never
edits them.

**Troubleshooting.** Help → Troubleshooting holds the whole kit:
Show Logs / Show Data Folder in Finder, Copy Installation Info,
**Generate Diagnostic Report…** (a full health report written to
the Desktop — attach it to any bug report), **Report a Problem…**
(a pre-addressed email with the template filled in), **Storage…**
(cache sizes with safe clears), Verbose Logging, and Reset
Settings…. From Terminal, `--selftest` runs 96 checks localizing
any problem, and `--sweep <Pane>` click-tests every control on a
pane. The menu bar reaches every function if a pane misbehaves.
The Help window opens onto a browse list of every chapter and
every control in the app — and the complete **User Manual** lives
there too (Help → User Manual…), every pane, every control, click
by click.

---

## The August wave — new since the sections above

- **⌘D, anywhere**: select a word or phrase in any pane and the
  floating dictionary window answers with the full lookup stack,
  following its own cross-references.
- **Rulings on every surface**: the authority's approved
  pronunciations override the engine in the whole-text display,
  on entry cards (green ⟪ruled⟫ tag — including phrases embedded
  in longer titles), and in search, where the ruled form finds
  its entry.
- **Pronunciation (THL)**: the scholarly transcription beside the
  house convention — in the Overlay's display modes and the
  Convert pane's live table — proven against the published
  standard's own 139 examples.
- **Edition collation** (Align → Compare editions…): two
  witnesses, every variant colored, a numbered apparatus you can
  save. The editor rules; the machine never resolves.
- **Search locations, TibetDoc-style**: folder results roll up
  per file with hit counts; one click opens the text at the hit.
- **The Pecha Maker, production-grade**: Degé measures, classical
  five-line openings, title folios, ya-yig and volume letters,
  verse lineation, two-up imposition with cut marks, cover
  sheets, whole-folder batches — and a corpus-attested English
  interlinear that is matched, never composed.
- **Self-updating collections** (Library → Maintenance): the app
  checks the official release buckets, compares fingerprints,
  and installs updates in place.
- **The Working Glossary, complete**: 1,395 AI-drafted deep
  entries where the master is silent — violet-bannered, POS from
  hand-tagged evidence only, resolving cross-references — with a
  review-and-promote lane into the Approval queue, and a
  corpus screen that proves no machine output reproduces the
  master's English.
- **Approve all in filter**: machine-seeded proposal batches
  ruled in one considered act, every ruling stamped.
- **THL Places** in the link-out row; **Old-Tibetan spellings**
  (da-drag) folding into classical search; entry cards in a
  deliberate typographic order with ACIP beside the Tibetan.

---

### Text ↔ woodblock (August 13)

The selection-to-carving jump: right-click in a loaded text for
your position by folio and line (from the text's own @markers and
the input centers' preserved line breaks), then open that folio in
the reading panel, the Input workflow, or the full scan viewer
with the selected words boxed on the woodblock (OCR word boxes,
locator only, misses always named). The viewer follows the BUDA
idiom — self-completing thumbnail rail, dark canvas, click-zoom,
grab-pan, pinch zoom, go-to, full screen, display-only image
adjustment — plus a whole-work scroll view whose e-text toggle
lays each side's own keying beneath its image. One folio-named
offline store serves reading and keying alike; a folio-faithful
export writes one file per woodblock side. The Input workflow
gained the same locate power over its own scans, folio jumps,
anchored zoom, and a Recent-scans memory.

*Twenty panes, one principle: the machine may match the master's
English — it may never compose it.*
