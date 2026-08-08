# The ALL Translation Tool
## A Complete Overview: Every Function, Its Design, and the Vision Ahead

*Prepared August 2026 for Holy Geshe Michael Roach and the Director of
the Asian Legacy Library, at the request of Adam Andrade.*

---

## Why this document

The Asian Classics Input Project spent four decades rescuing the
literature of Tibet — organizing input centers, training operators,
typing a civilization's library keystroke by keystroke, and cataloging
what was saved. The Asian Legacy Library now carries that work
forward. This document describes a single desktop application built to
serve the next part of the mission: **turning the preserved library
into translations, and turning students into translators** — with
Geshe Michael's own English as the guiding voice throughout.

The application gathers, in one program that runs entirely on the
user's own computer, every function needed to work with a Tibetan
text: reading it with the dictionary underneath, looking anything up,
converting between every writing system, analyzing the grammar,
consulting the original woodblock, drafting a translation, checking
that draft against the master's established terminology, reusing the
scholarship already performed, typing new texts from scans exactly as
the input centers always have — and, throughout, teaching.

Three commitments govern every feature, and are worth stating before
any feature is:

1. **Geshe Michael's English is the binding layer.** His equivalents,
   established across decades of teaching, are the authority in every
   pane. All other scholarship — and there is a great deal of it
   integrated — appears only as clearly-labeled reference, never
   promoted into his voice. Machine processes may *match* his English
   from the evidence of his own corpus; they never compose English in
   his name.
2. **The tool never guesses.** Where a conversion or an analysis
   cannot be completed with certainty, the place is flagged and the
   uncertainty stated. Nothing is silently approximated. Everything
   derived automatically is labeled provisional and *looks*
   provisional.
3. **Work performed once is never redone.** Published footnotes,
   bibliography entries, catalog titles, alignments, and even the
   Project's own historical software have been recovered, banked, and
   put back into service.

---

# PART I — EVERY FUNCTION THE APPLICATION PROVIDES

The application is one window with twelve working areas ("panes"),
built on one shared core. Everything except two clearly-marked AI
features runs fully offline — a deliberate choice, so that the tool
serves equally in New York, in a monastery library, or at an input
center with no reliable connection.

## 1. Overlay — reading with the dictionary underneath

Open any text from the preserved library — ACIP transliteration or
scholarly Wylie, the program detects which — and it appears with every
word and phrase the dictionary knows gently shaded, nested phrases
shown within longer ones. A complete canonical volume opens fully
shaded in under half a second.

Clicking any word opens its card: the dictionary entry with its
quality tier honestly labeled (curated, glossary, or provisional);
**the concordance of Geshe Michael's actual translations of that term,
course by course**; reference definitions from other scholars, marked
as such; Sanskrit equivalences from the classical Mahāvyutpatti
glossary; attestation from the half-million word forms of the Monlam
project (so even a word the dictionary does not yet know is identified
as a real word awaiting an entry, or a probable typo); verb tense
paradigms; expansions of manuscript abbreviations and syllabic
contractions (the traditional bsdus-tshig, including letter-level
fusions like *sher phyin*); the honorific badge with the ordinary
counterpart shown beside; the community pronunciations of the term
("also heard"); the translator's own per-text glossary, editable in
place; and curated links out to the major online resources (84000,
BDRC, Bibliotheca Polyglotta, Lotsawa House).

While reading, the text quietly diagnoses itself: spelling errors
carry a red underline (checked against the classical syllable-legality
rules); particles that disagree with the preceding suffix per the
classical tables carry a soft review mark, never an auto-correction;
provisional glosses are visibly dashed; and each reader controls their
own information density through a Display panel — script as Tibetan,
ACIP, or Wylie; each reference layer on or off; a choice of thirteen
vetted Tibetan typefaces — remembered across sessions.

**And the woodblock itself is present.** For texts of the Derge and
Lhasa Kangyurs and the Derge Tengyur, one button opens the original
page images from BDRC's public archive directly beneath the text — and
the pages turn themselves, following the folio marker at the reader's
cursor, with the current line marked. The mapping between folio and
image is BDRC's own catalog labeling; nothing is guessed. At the
finest level, an opt-in control reads the woodblock image with the
embedded text recognition and draws a box around the very word under
the cursor — the recognition is used only to *locate*, never trusted
as text. A student can now sit with the digital text and the actual
carving of it, line by line and word by word, anywhere in the world.

Two publishing utilities complete the pane: one-button export of a
text to print-ready Tibetan script following the Project's own
conventions, and a formatter reproducing Geshe Michael's demonstrated
translation-preparation workflow.

## 2. Analysis — the eighteen-section report

For deep study, a passage can be submitted to the full
eighteen-section analysis: the deterministic engine first extracts
everything *verifiable* — segmentation, established term anchors,
corpus matches — and injects it into the analysis template; an AI
model then writes the full report; and the machine re-checks the
result (reconstructing the segments, verifying every dictionary
quotation, confirming the Tibetan was not altered). AI output is
banner-labeled as such, always. This is one of only two features
requiring a connection, and it costs cents per passage.

## 3. Trainer — learning to read, layer by layer

This pane embodies the pedagogy directly: a student pastes a passage,
attempts it, and then reveals help one layer at a time — first the
clause and phrase boundaries; then each particle's grammatical role;
then the suggested reading order (find the verb first — identified
from the dictionary's own tense evidence, or honestly marked
unverified), with connected phrases marked to be read together; then
the vocabulary with its quality tiers; then **the answer key: Geshe
Michael's own English, wherever the passage exists in his corpus**;
and finally the complete grammatical parse in the system of Wilson's
*Translating Buddhism from Tibetan* — every unit designated, case
particles carrying their eight-case numbers as assigned by the verb's
class, and every syllable-separating dot named according to the
Science of the Dots, reproducing the book's own diagrams exactly. To
our knowledge no software anywhere else renders this parse.

The principle: engine guidance is guidance; **only the master's own
corpus lines are ever presented as answers.**

## 4. Drills — exercises that write themselves

Every exercise is generated from a real corpus segment, so every
answer is Geshe Michael's own text — the machine invents nothing to
teach with. Restore the order of a scrambled clause; choose the
Tibetan that fills a blank, given his English; choose the correct
particle variant, explained from the classical tables; read a course
in parallel, revealing the English only when needed; and review
vocabulary on a spaced-repetition schedule that builds itself from the
words the student clicked while reading. The most ambitious drill,
Translate & compare, asks the student to render an unseen passage at
their level and then shows the master's own rendering beside theirs,
with a terminology comparison — which of the source's established
terms found their equivalents in the student's draft. Nothing grades
style; the master's version teaches. Missed skills are named; progress
stays in a local file that belongs to the student.

## 5. Draft — the working translator's bench

The source text splits into clauses; each clause shows its verified
term anchors; each term opens the full concordance of how the master
has rendered it, source by source. The translator writes alongside,
and one button checks the draft's terminology: terms whose established
equivalents do not appear, one term rendered two different ways,
drafts leaning on provisional-tier glosses, and one English word
serving two different Tibetan terms — all flagged for review, never
auto-corrected.

The bench carries the structural instruments of traditional
scholarship: extraction of the text's own topical outline (sa bcad)
from its internal markers; the canonical structural units — explicit
bam-po divisions and numbered chapter colophons, with the traditional
shloka reckoning shown as the labeled estimate it is; and a verse
meter check that names the meter and flags broken lines.

It also carries the shared apparatus, under Geshe Michael's directive
that **work performed once is never redone**: every footnote (344) and
bibliography entry (138) from the released Mixed Nuts volumes,
searchable and insertable with citation attached; a bibliography
composer in the exact house format, filling itself in from the
Project's catalog for over 1,800 known works; and quotation detection
— when the text being translated quotes a work in the corpus, the app
finds the quotation, shows the published rendering of those very
lines, and offers the published citation and any published footnotes
on that passage. Notes a translator proposes wait in a clearly-marked
pending queue; there is deliberately no "approve" button in the
software, because approval belongs to the publishing process, not the
tool. An optional AI back-check produces a coverage report only —
omissions and additions — and is forbidden by construction to
retranslate.

## 6. Align — the dictionary-building workflow, reborn

The Project's original Hypercontext program — the tool whose
hypertexted files fed the first dictionary building — lives again.
Click a Tibetan word (the word-finder cycles through the possible
word lengths), select its English, press the space bar: linked.
Finer sublinks nest inside longer links in their own color. Every
text's links are saved, and the harvest exports as clearly-PENDING
candidate word pairs for the dictionary project — translator-authored
attestations that a human reviews before anything enters the
dictionary. Legacy hypertexted files from the original era import
directly, verified against the original tutorial file itself. Every
translator working in this pane is quietly extending the dictionary
for everyone who follows.

## 7. Input — the input-center workstation

The software the input centers were always meant to have. Its lineage
is the Project's own: the ACE system built for ACIP years ago was
recovered from its original author's published source, its design
studied, and its workflow rebuilt on the modern engines — better than
it could be built then.

The page scan sits above; the typing area below; and the scan follows
the typist's cursor — line by line exactly, once the line detector has
looked at the page; approximately, in the original program's manner,
otherwise. Illegal syllables underline in red as they are typed. A
folio-marker button inserts the next marker in the Project's exact
specification. A whole folder of scans becomes a work queue — page N
of M, each page's typing saved the moment the operator moves on,
restored the moment they return. And the input centers' own
double-keying discipline is built in: compare your file against your
partner's, and every discrepancy lights up — down to a single dropped
letter — to be corrected until the two files match exactly, which has
always been the rule.

## 8. Library — the preserved canon, close at hand

The complete ACIP Release 6 collection installs from the Library's own
archive files: 2,716 texts, 8.68 million lines, fully search-indexed
on the user's machine. Every catalog filename is decoded into its full
provenance — collection, text number, sub-number, verification level,
language — by the complete grammar recovered from the Project's own
cataloging toolchain, and displayed with the work's English title,
its subject classifications (bilingual, from the Project's catalogs),
and a direct link to the original scans where the mapping is certain.
Personal materials import alongside; Word documents convert
automatically; documents typed in the pre-Unicode Tibetan fonts of
the 1990s are rescued to proper Tibetan script through a dedicated
conversion lane. A flat catalog view lays the entire shelf out as one
sortable table.

## 9. Search — the classic grammar, restored

The Project's beloved Gofer search grammar lives on: exact phrases,
OR, and NEAR-within-N-lines, over the corpus, over any folder of raw
files, or over the indexed library. Searches can be saved by name.
And the search now extends to the scholar's own machine: one button
asks the computer's own index for any document containing the term —
searched as typed *and* as Tibetan script, through the proven
converters.

## 10. Convert — every writing system, and time itself

Type in any of the four systems — ACIP, scholarly Wylie, Sanskrit
IAST, or Devanagari; the pane detects which — and receive every
representation at once: transliteration, Tibetan script, both official
ACIP Sanskrit input codes, Devanagari, the pronunciation in Geshe
Michael's convention, the classical scholarly IPA with a per-letter
pronunciation guide, and the simplified pronunciation per the
Project's own published standard. Sanskrit roots in the input are
recognized and annotated from Whitney (below). And the pane answers
the translator's dating questions in every direction — western year to
full Tibetan designation, element-animal pair to its years, complete
Tibetan date to western weekday and back — with the traditional
calendar's own irregularities honored rather than smoothed.

## 11. Lookup — the stacked dictionary

One search box, many doors. Exact headword in any script; a fallback
that strips affixed particles (searching *po'i* finds *po*); a
pronunciation search in the master's convention; **a community
pronunciation register**, so a student who knows only "gonpa" or
"tulku" still arrives at the entry — with the strict convention shown
beside, so the register teaches rather than replaces; English-to-
Tibetan reverse lookup; and, for Sanskrit, the two preeminent works of
William Dwight Whitney integrated whole — all 937 roots of the 1885
*Roots, Verb-Forms and Primary Derivatives* with his meanings, verb
classes, and principal parts, each root carrying its section citations
into the 1879 *Sanskrit Grammar*, with one click reaching the
corresponding Monier-Williams entry or the Grammar itself. Honorific
terms wear their badge with the ordinary word beside them. And the
entire dictionary can be browsed in true Tibetan alphabetical order.

## 12. Scan — Tibetan text recognition, embedded and proven

Text recognition runs inside the application: the Buddhist Digital
Resource Center's open models (used with BDRC's permission, credited
wherever output appears), with both the line detector and the
recognizer proven line-for-line against BDRC's own program running
beside them. A page scan opens; its lines are found and read; every
line appears in transliteration and Tibetan script with an immediate
quality check that counts and marks every syllable failing the
classical legality rules. Every recognized word knows its exact
position on the page — click a word in the reading and it lights up on
the woodblock. A whole scanned volume processes in one batch. And the
Project's discipline governs all of it: OCR output is tagged
unverified review material, is never smoothed, and never enters the
corpus.

## The machinery beneath (what every pane stands on)

- **Proven conversion engines.** Every converter was perfected as a
  canonical implementation in the dictionary project, ported, and
  required to reproduce its original's validation battery before
  shipping — exact on all corpus ACIP conversions; 99% on 26,318
  ground-truth script pairs with every miss traced to a documented
  source defect; pronunciation character-identical on all 105,000+
  headwords; the Sanskrit family identical on 79,000+ terms. Two
  independent outside converters have been run over the entire
  dictionary as cross-examining witnesses; they agree with each other
  to three-hundredths of a percent, and nearly every disagreement
  with our engine is a place where ours refuses to guess at defective
  input while they convert it blindly.
- **The grammar layer.** Wilson's system as rule tables: the complete
  case-particle system with suffix agreement calibrated against
  107,000 corpus particle pairs; verb identification from the
  dictionary's own tense data (never a guess — and where there is no
  evidence, it says so); the eight verb classes correcting reading
  guidance; and all twelve designations of the Science of the Dots.
- **Twelve reference and register layers**, every one license-verified
  and labeled: from the Mahāvyutpatti to Whitney to the Project's own
  growing registers of community pronunciation, the prenasal rule
  Geshe Michael teaches, and the honorifics.
- **Thirty-six automated test batteries** run before any change ships.

---

# PART II — HOW THESE FEATURES CAME TO BE:
# THE ITERATIVE DESIGN PROCESS

The tool was not designed once and then built. It grew under a small
set of inviolable rules, and nearly every feature's final shape was
forced by evidence discovered on the way. The recurring pattern is
worth showing, because it *is* the quality guarantee:
**port → prove → integrate → let the battery correct you.**

**The engines set the standard.** Each converter began as a canonical
implementation in the dictionary project and was ported line by line,
its quirks preserved and marked, and made to reproduce the original's
validation battery before shipping. This rigor pays for itself
continually: a one-byte error in a data loader, an ordering subtlety
in a brand-new converter, a porting slip in a third-party library —
each was caught by a battery within minutes of its introduction, not
by a user months later.

**The reading pane grew rings like a tree.** First the bare
dictionary shading; then each diagnostic layer arrived only as its
data was verified — spelling underlines when the open-licensed
syllable rules were banked; provisional dashing when the tier system
hardened; particle-agreement marks only after the classical tables
were calibrated against the full corpus. That calibration exercise
itself became scholarship: the "failures" turned out to be precisely
the famous da-drag stems, and what began as a bug list became a
documented exception class. From this a design law emerged that now
governs everything: **richness through labeled layers and toggles,
never through invention.**

**Good news simplified the scans.** The follow-along design assumed
the mapping from folio numbers to page images would need painstaking
calibration. BDRC's own manifests turned out to label every image
with its folio side — so the guessing machinery was deleted before it
was built, and the feature shipped *more* honest than designed: the
line band admits on screen that its placement is approximate. When
the recognition program later made word-exact location possible, the
deliberate choice was to use it **only as a locator** — recognized
text never enters the reading.

**The recognition program refused every shortcut.** The canonical
pipeline was reproduced stage by stage, each stage compared against
the original program running beside it — which is how a genuine bug
was discovered *upstream*, in the reference implementation's own
page-rotation mathematics, now documented for its maintainers while
our port offers the correction as a labeled option. The text decoder
was not approximated but ported to the letter, down to the floating-
point behavior of the original's numerical libraries, until output
matched byte for byte. The word-position feature then came almost
free — the canonical decoder computes word coordinates internally and
its own application discards them; ours keeps them, and proves them
against the original exactly.

**Two recoveries turned history into architecture.** A question —
"could we integrate Hypercontext?" — led to the discovery, inside the
program's own tutorial, that Hypercontext was never merely a reader:
it was the alignment-authoring tool whose files fed the first
dictionary building. Its reading half already existed in this tool;
its authoring half became the Align pane. A second question — "can
you find ACE?" — led through old browser bookmarks to a vanished
website to the original author's published source code, Apache-
licensed, containing the entire input-center software ecosystem: the
scan-beside-editor workstation, the cursor-following scan, the
double-keying discipline. Reading that source showed its
cursor-following was an approximation; the rebuilt Input pane keeps
the workflow the input centers were trained on and upgrades the
following with true word coordinates. Institutional memory, recovered
and put back to work.

**The registers taught the sharpest lessons.** A single example from
Geshe Michael — *SKABS 'DIR is pronounced "kamdir"* — became a
research program. Applying the rule naively produced plausible
results; sustained questioning (notably: what distinguishes the
prenasals that become *n* from those that become *m*?) uncovered four
distinct sound systems, the discovery that the rule applies only
within a phonological word — and a flaw in our own derivation's
safety check, which could transform letters at the wrong place while
every count said it was right. The derivation was rebuilt on true
syllable alignment through the canonical engine, and a confident 232
forms became a *proven* 205. The register's finest rows came last:
mining the corpus's English for the master's own published spellings
— Lobsang, Ganden, Tengyur — the highest attestation possible, his
own usage. The lesson is now doctrine: counts are not proof;
alignment is proof.

**Verification matured from belief to demonstration.** Early features
were trusted because their parts were tested. Recent features are
trusted because they have been *watched working on material they have
never seen* — the final proof of the recognition stack was fetching a
folio of the Pramanavarttika live from BDRC's archive, a page the
system had never touched, and reading it correctly on the spot, with
its garbled patches honestly flagged rather than smoothed. That is
the standard the whole tool now aims at: proofs that would convince
a skeptic who trusts nothing but the output.

---

# PART III — THE ROAD AHEAD

## What awaits review and decision (the leadership's part)

1. **The prenasal pronunciation register** — 205 derived forms
   applying Geshe Michael's stated rule, each mechanically aligned
   and verified never to contradict an attested pronunciation. A
   one-sitting review sheet is prepared; his approval promotes the
   class from "derived" to "approved."
2. **The honorific register** — the seed pairs await his
   high-honorific designations, and the reference books being
   gathered will grow it from attested sources.
3. **The 84000 license clarification** (email drafted) — resolving it
   unlocks the best-licensed aligned Tibetan–English dataset in
   existence for the corpus.
4. **The catalog title ruling** — whether the catalogs' English
   titles stand as his binding equivalents (he directed the
   cataloging) or as a labeled reference layer.
5. **A brief hands-on pass** through the newest panes — the machinery
   is machine-verified; experienced eyes on the workflow will polish
   what proofs cannot.

## What should be built next (my recommendations)

1. **OCR-assisted input.** The groundwork landed this week: the tool
   can now pre-fill an input operator's editor with the recognized
   draft of the page — in ACIP, through a newly proven reverse
   converter — for the operator to *correct* against the scan rather
   than type from nothing. With the double-keying discipline
   unchanged on top, this could multiply input-center throughput
   while keeping the accuracy guarantee that made ACIP's data
   trustworthy for forty years.
2. **The oversight view** — unifying the draft-checking instruments
   into the side-by-side reviewer's pane the roadmap envisioned, so
   that a senior translator reviewing a junior's work sees provisional
   -tier reliance and register warnings at a glance.
3. **Alignment memory.** The Align pane accumulates expert word
   pairs; a view over the accumulated harvest plus the corpus would
   let any translator ask "how has this whole phrase been rendered
   before?" — the beginnings of a Project translation memory, built
   entirely from attested work.
4. **Verse reading-order guidance** — the meter's effect on word
   order is rule-describable from Wilson and the corpus, and would
   complete the Trainer's guidance for poetry.
5. **Register-aware display** — once the prenasal forms are approved,
   a toggle to show them inline while reading, making the register a
   pronunciation aid for students, not only a search convenience.
6. **The .hyp harvest** — if hypertexted files from the original era
   survive on old machines, each is a packet of expert alignments;
   the importer stands ready.
7. **Windows and Linux builds** for the input centers — the code was
   kept cross-platform-clean from the first commit; this is packaging
   work, not construction.
8. **A release validator** — one command that takes each new
   dictionary release and runs every battery, oracle, register, and
   index, emitting a report — so the data project can ship and the
   tool can absorb without ceremony.

## The larger vision

For the **academic and scholastic community**, this tool puts things
in one place that have never been in one place: the preserved canon,
searchable and readable at native speed; the original woodblocks,
following along page by page and now word by word; the classical
reference tradition (Mahāvyutpatti, Whitney, the grammars) beside the
living tradition (a working master's forty years of established
English); grammatical analysis at a depth — down to the designation
of every syllable dot — that exists in no other software; and a
standard of intellectual honesty, enforced by the software itself,
in which every claim on the screen declares whether it is attested,
derived, provisional, or guessed. A graduate student, a geshe, and a
lay practitioner are served by the same instrument.

For **translation training**, the tool is built on a conviction: new
translators are best trained by the master's own work. Every drill
answer, every Trainer answer key, every concordance line is Geshe
Michael's actual text — the software never invents a teaching
example. The student's path — read with help, peel the help away,
drill what was missed, translate and compare, then draft with the
terminology checker watching — is the traditional apprenticeship,
instrumented. And because the Align pane turns every trained
translator's reading into candidate dictionary attestations, **the
students the tool trains become contributors to the very dictionary
that trains the next students.** That circle — preservation feeding
translation feeding training feeding the dictionary feeding
preservation — is the Project's whole mission, running on one
machine, owned by no one but its users.

*Prepared with reverence for the lineage of translation this tool
exists to serve.*
