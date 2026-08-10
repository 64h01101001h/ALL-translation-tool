# The ALL Translation Tool — White Paper

*A complete working environment for Tibetan texts and their
translation*

Asian Legacy Library · Asian Classics Input Project — August 2026

---

## 1. The mission

Forty years ago, the Asian Classics Input Project began typing a
civilization's library, keystroke by keystroke. In input centers
across Asia, operators sat before woodblock page scans and entered
the Kangyur, the Tengyur, and the collected works of the great
masters — twice each, by two independent typists, corrected until
the two files matched exactly, which has always been the rule. That
labor preserved the texts. This tool is what the library just
became: not an archive, but a living working environment in which
those same texts can be read, understood, translated, checked, and
taught.

The ALL Translation Tool is a desktop application that gathers, in
one place, every function needed to work with a Tibetan digital
text: reading it, looking words up, converting between writing
systems, analyzing grammar, drafting a translation, checking that
draft, training the next generation of translators, and typing the
next text into existence. It is built around one central treasure:
the dictionary of English equivalents established by Geshe Michael
Roach across decades of teaching and translation, together with a
parallel corpus of 42,199 Tibetan–English segments drawn from more
than sixty of his courses and works — including three published
volumes (*A Song of My Spiritual Life*, *The Other Side of
Emptiness*, and *The Sixty Verses of Nagarjuna*) and nearly three
thousand paired Tibetan–English titles of canonical works from the
Project's own master catalogs.

The software is a fast, self-contained program for the Mac, with
Windows and Linux planned for the input centers. Everything
essential runs offline; the texts and the dictionary never leave
the machine.

## 2. The binding-layer principle

One rule governs every pane of this application, and it deserves to
be stated before anything else:

**Geshe Michael's English is the binding layer.** The dictionary
field carrying his equivalents is the authority everywhere in the
software. Reference materials from other scholars — Hopkins, Lokesh
Chandra, the Monlam project, Whitney's Sanskrit works — are
displayed only as clearly-labeled comparanda, and are never
promoted into his voice. Machine processes may MATCH his English
from corpus evidence; they never compose English in his name.

In the demo this was said in one line, and it remains the truest
summary of the design: *the machine never invents English — it only
ever shows you what the master already said.* When a student
reveals the "answer key" in the Trainer, the answer is Geshe
Michael's own published rendering of that very passage. When a
translator presses "Phrase memory" in the Draft pane, what appears
is every time he has ever rendered those words — nothing more.
Where the corpus holds no answer, the software says so honestly
rather than manufacturing one.

## 3. What the tool is: one window, six workflows, sixteen panes

The application is one window. Across the top run six workflow
tabs; inside each, a second row holds its working areas — sixteen
panes in all, sharing one dictionary of 105,634 entries, 12,004 of
them carrying Geshe Michael's English in tiered quality (curated,
glossary, provisional).

**Read** holds the Overlay and the Library. The Library is the
preserved canon on your own disk — 2,716 texts and 8.68 million
lines of the complete ACIP Release 6, browsable as a tree that
decodes every catalog file name into its provenance, alongside your
own imported materials. Double-click any text and it opens in the
Overlay, the flagship reader: every word and phrase the dictionary
knows is shaded, nested phrases inside longer ones; clicking a word
opens its card — the entry with its quality tier, Geshe Michael's
actual corpus renderings course by course, reference definitions
honestly marked as references, honorific badges, community
pronunciations. Spelling that fails the classical syllable rules is
underlined in red — and can also be gathered into a small working
list of every doubted syllable, each one a click away in context, so
likely input errors can be filed for the authority's ruling rather
than silently ignored. A particle disagreeing with its suffix gets a
gentle review mark. The text displays as Tibetan script, ACIP, or
Wylie at the reader's choice. For texts of the Kangyur and Tengyur,
one more button appears: *Follow along in scans* — the original
woodblock page images, served from the Buddhist Digital Resource
Center's public archive, display beneath the text and turn their
own pages as the cursor moves. The student sits with the digital
text and the actual carving of it — anywhere on earth.

**Translate** holds the Draft bench, the Review pane, and Align.
In Draft, the source text splits into clauses; each clause shows
its established terms banded in color, with each term's English
chipped in the same color, so which English goes with which Tibetan
reads at a glance. Every term opens a full corpus concordance. One
button checks the finished terminology — terms missing from the
draft, one term rendered two ways, drafts leaning on provisional
glosses — flagged for review, never auto-corrected. The bench also
carries the published apparatus (section 5), outline and verse
tools, and export to a formatted rich-text file. Review is for the
senior translator checking someone else's draft: source on the
left, English on the right, and a report of advisories — register
warnings, honorific cautions, provisional-gloss flags — guidance,
never verdicts. Align is the Project's original Hypercontext
workflow reborn: click a Tibetan word, select its English, link the
pair — building tomorrow's dictionary from translator-authored
attestations, always labeled PENDING until approved.

**Research** holds Search, Lookup, Sanskrit, Convert, and Analysis.
Search is Gofer — the tool Geshe Michael has used to search his
library every day for decades — rebuilt inside with the same
algorithms and its original layout: exact terms, OR, AND, and
NEAR-within-N-lines, over the aligned corpus, the published
apparatus (all 2,234 footnotes and 705 bibliography entries), and
any folders on disk. Lookup is the stacked multi-dictionary: his
entry first and binding, then each reference layer labeled, with
English-to-Tibetan reverse lookup and traditional Tibetan-order
browsing. Sanskrit is a whole workbench: enter IAST, Devanagari, or
ACIP input-code and receive the full cascade of scripts plus
Whitney's root analyses and the Mahāvyutpatti bridge to classical
Tibetan. Convert renders any input in every writing system at once
— and answers the translator's dating questions through a full
traditional Tibetan calendar. Analysis produces an
eighteen-section grammatical analysis of a passage: the
deterministic engine extracts everything verifiable first, an AI
model writes the report, and the machine re-checks every quotation
— with the output banner-labeled as AI, always.

**Learn** holds the Trainer and the Drills. The Trainer teaches
reading layer by layer: the student attempts a passage bare, then
reveals clause boundaries, each particle's grammatical role, the
suggested reading order (find the verb first), vocabulary, the
answer key — Geshe Michael's own English wherever the passage
exists in his corpus — and finally the complete grammatical parse
in the system of Wilson's *Translating Buddhism from Tibetan*,
down to a designation for every syllable-separating dot. The
Drills write themselves from the corpus, so every answer is the
master's own text: unscramble a clause, fill the blank given his
English, choose the correct particle, read a course in parallel,
review vocabulary on a spaced schedule. The app quietly adapts to
the learner, naming missed skills and choosing material near their
level.

**Input** holds the Input workstation and the Scan pane — the
software the input centers were meant to have. Input is the ACE
lineage recovered from its original author and rebuilt: the page
scan above, the typing area below, the scan following the typist's
cursor line by line; illegal syllables underline in red as they are
typed, and the double-keying discipline is built in — compare your
file against your partner's, and every discrepancy lights up to be
corrected until the two match exactly. Scan is Tibetan text
recognition, embedded and proven: BDRC's open recognition models
run locally on a page image or a whole scanned volume, and every
line of output is banner-marked as unverified review material that
never enters the corpus — nothing is smoothed, everything is
flagged.

**Community** holds Propose and Approval — the living half of the
design, described next.

## 4. The community model: Propose → Approval

A dictionary of a living teaching lineage cannot be frozen, and it
cannot be a free-for-all. The tool resolves this with one simple
structure.

Anyone on the team can **propose**: a pronunciation, an honorific
marking, an abbreviation, a word or phrase rendering, a note — or a
doubted spelling the software's own legality rules have flagged as a
likely input error. When the authority confirms a spelling error it
is exported for the input teams to correct at the source (the app
never edits the texts); when the authority rules the form valid, the
software stops doubting it — everywhere, for the whole team.
Proposals file from the Propose pane or straight from any entry
card, with the passage in question automatically attached as
evidence, and each proposal carries its proposer's name for
provenance. Proposals wait in a clearly-marked pending queue — and
there is deliberately no shortcut around it.

Every proposal then comes to the **authority's** queue in the
Approval pane, where each item appears with its evidence, its
source citation, and its proposer, to be ruled on one by one:
approve, decline, or defer, each with an optional note. When the
authority approves a register item, it becomes part of the app
immediately — stamped with who approved it and when — for every
student, everywhere. Dictionary proposals are exported to the data
project rather than applied directly, because the app never edits
the master data. A permanent rulings archive records every
decision.

His judgment is the binding layer. That is the whole design.

The same discipline governs the published apparatus: Geshe Michael
has directed that footnotes created in the released translations
can and should be reused as the same topics recur. The official
apparatus therefore contains only what has been published and fully
approved; every reuse carries its source citation so borrowed
scholarship stays attributed; and notes a translator proposes along
the way wait in the pending queue. There is no "approve" button for
scholarship in the software, because approval is an act of the
publishing process, not of the app.

## 5. Work performed once is never redone

The published apparatus — 2,234 footnotes and 705 bibliography
entries — is searchable from one box and reusable with one click,
citation attached. A bibliography composer assembles new entries in
the house format exactly, filling itself in from the Project's own
catalog for 1,846 known works by their ACIP number. And when the
text being translated quotes a work that exists in the corpus, the
app finds the quotation, shows how the master already rendered
those lines, and offers the published bibliography entry and any
published footnotes on that passage. A detected quotation, its
citation, its bibliography entry, and the original woodblock scans
all connect to one another through one deterministic chain.

## 6. Offline first — and why that matters

Everything essential runs on the user's own machine, with no
account, no server, and no subscription. The complete canon, the
dictionary, the corpus, the search index, the conversion engines,
the grammar engine, even the text recognition — all local. Exactly
two features reach the network, and both say so plainly: the
Analysis pane's AI-written report, and the woodblock images served
from BDRC's public archive (which are cached, so a text revisited
costs nothing).

This is not a technical footnote; it is a commitment. The tool must
work in a monastery in Nepal exactly as it works in New York. Input
centers with unreliable connections must be able to type, check,
and compare all day. A translator's drafts, glossaries, and study
progress belong to the translator, on the translator's disk. And a
library that took forty years to type should never depend on
someone else's server staying up.

## 7. Provenance and honesty as design values

Three commitments run through every pane:

**The tool never guesses.** When a conversion or an analysis cannot
be completed with certainty, the app flags the place and says so —
it never silently approximates. Provisional material *looks*
provisional: machine-aligned glosses carry an amber tint that is a
warning, not a decoration. AI-written output is banner-labeled as
such, always. Text recognition output is review material only,
never trusted as text. Terms with different senses in different
registers — prayer versus philosophy — carry warnings and are never
auto-resolved.

**Provenance is preserved.** Everything machine-derived says so;
every approved item carries who approved it and when; every mined
fact carries its source and page. The application imports versioned
data releases and never alters them — the dictionary project
remains the keeper of the data, and the app is its honest reader.

**Everything is proven.** Each conversion engine was first
perfected as a canonical reference implementation during the
dictionary project, then ported into the application and proven
against large validation batteries; a port that fails its battery
does not ship. The transliteration-to-Tibetan-script engine is
verified against 26,318 source-attested ground-truth pairs; the
pronunciation engine matches the canonical engine on every
dictionary headword, with 2,118 printed-card pronunciations as
ground truth; the simplified-pronunciation style reproduces 51 of
the 52 pure-Tibetan examples in the Project's own published
standard; the traditional Tibetan calendar was proven against two
complete printed years, every month and every day. In all, the
application carries 38 automated test suites that must pass before
any change ships, and any user can run its built-in self-test — 72
checks that print pass or fail and localize any problem. Where a
result cannot be verified mechanically, the interface says so
rather than implying certainty.

## 8. What comes next

- **The registers grow under review.** A class of
  pronunciation forms derived from Geshe Michael's own teaching
  awaits his confirmation through the Approval queue, along with
  high-honorific markings — every item carrying its evidence and
  citation.
- **The translation standards compile.** Sourced standards mined
  from the published volumes' own notes and prefaces are being
  gathered, several already running as software features, with a
  prepared review sheet so proposed footnotes can be approved in
  Geshe Michael's own sessions — never in the software.
- **The input-center formatter.** Converting raw input-center
  documents to finished publication-ready Tibetan, once the
  conversion standards document is supplied; the Input pane already
  carries the typing, checking, and double-keying half of that
  workflow.
- **One ruling on catalog titles.** Whether the master catalogs'
  English titles stand as Geshe Michael's own binding equivalents
  or as a labeled reference layer awaits his decision.
- **Windows and Linux builds** for the input centers.
- Cataloging has been deliberately set aside as its own future
  project; the knowledge mined from the Project's original
  cataloging toolchain is banked and waiting for it.

## 9. How to get it

The tool is distributed by the ALL team as a standard Mac disk
image (a DMG): open the image, drag the application to your
Applications folder, and — the first time only — right-click the
app and choose Open. Everything it needs is inside; no installer,
no account, no connection required. Distribution is through the
team directly; students and translators who worked with the demo
should ask their program coordinator for the current build.

Help lives inside the application itself: a searchable Help window
documents every pane, every button, and the suggested workflows —
from a student's first hour to a translation checked end to end.

---

Sixteen panes, one principle: the machine may match the master's
English — it may never compose it.

*Prepared with reverence for the lineage of translation this tool
exists to serve.*
