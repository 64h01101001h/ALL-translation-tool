**The ALL Translation Tool**

*A complete working environment for Tibetan texts and their translation*

Asian Legacy Library · Asian Classics Input Project --- White Paper,
August 2026 (v8)

**1. Purpose**

The ALL Translation Tool is a desktop application that gathers, in one
place, every function needed to work with a Tibetan digital text:
reading it, looking words up, converting between writing systems,
analyzing grammar, drafting a translation, checking that draft, and
training the next generation of translators. It is built around one
central treasure: the dictionary of English equivalents established by
Geshe Michael Roach across decades of teaching and translation, together
with a parallel corpus of 42,199 Tibetan--English segments drawn from
more than sixty of his courses and works --- including, as of August
2026, three newly ingested published volumes (A Song of My Spiritual
Life, The Other Side of Emptiness, and The Sixty Verses of Nagarjuna)
and nearly three thousand paired Tibetan--English titles of canonical
works from the Project's own master catalogs.

The software is written as a fast, self-contained program for the Mac
(with Windows and Linux planned for the input centers). Everything
essential runs offline; the texts and the dictionary never leave the
machine.

**2. Founding principles**

- **Rule one ---** Geshe Michael's English is the binding layer. The
  dictionary field carrying his equivalents is the authority in every
  pane. Reference materials from other scholars are displayed only as
  clearly-labeled comparanda and are never promoted into his voice.
  Machine processes may MATCH his English from corpus evidence; they
  never compose English in his name.

- **Honest tiers ---** Anything derived automatically --- an aligned
  gloss, a guessed verb, a suggested reading order --- is labeled
  provisional or as guidance, and looks visibly different from attested
  data. Terms with different senses in different registers (prayer
  versus philosophy) carry warnings and are never auto-resolved.

- **Never guess ---** When a conversion or an analysis cannot be
  completed with certainty, the app flags the place and says so. It
  never silently approximates.

- **Provenance ---** The application imports versioned data releases; it
  does not own or alter the data. Every generated field carries its
  provenance.

**3. The data foundation**

  ----------------------------------- -----------------------------------
  **Asset**                           **Contents**

  Unified dictionary (v0.27.2)        105,634 entries; 12,004 with Geshe
                                      Michael's equivalents in tiered
                                      quality (curated / glossary /
                                      provisional)

  Parallel corpus                     42,199 aligned Tibetan--English
                                      segments from 60+ sources,
                                      published volumes, the Project's
                                      catalog titles, its bilingual
                                      subject headings, and
                                      join-verified author names --- the
                                      evidence base and the "answer key"
                                      for all training exercises

  Installed text library              the complete ACIP Release 6
                                      corpus: 2,716 texts, 8.68 million
                                      lines, fully search-indexed on
                                      the user's own machine --- and
                                      readable: any text in the canon,
                                      including a full 1.6MB volume,
                                      opens in the reading pane fully
                                      dictionary-shaded in under half
                                      a second, whether the file is in
                                      ACIP or scholarly transliteration

  Lokesh Chandra layer                79,305 Sanskrit-linked entries;
                                      88,973 headwords with IAST Sanskrit

  Reference dictionaries              445,772 definitions from legacy
                                      tools (multi-scholar compilations),
                                      kept strictly local as labeled
                                      reference --- never redistributed

  Attestation layers                  449,000+ word forms from the Monlam
                                      Grand Dictionary project and a
                                      2,491-row verb-paradigm database
                                      (both openly licensed) --- shown
                                      only as labeled reference, never
                                      binding

  Scholarly reference layers          the Mahāvyutpatti (9,379 classical
                                      Sanskrit--Tibetan equivalences);
                                      a hand-tagged classical
                                      part-of-speech lexicon (13,107
                                      forms); a manuscript-abbreviation
                                      table; and William Dwight
                                      Whitney's two preeminent Sanskrit
                                      works --- all 937 roots of the
                                      1885 Roots with per-root section
                                      citations into the 1879 Grammar
                                      --- every layer license-verified
                                      and clearly labeled comparanda

  The Project's own registers         a growing register of community
                                      pronunciations (gonpa, tulku,
                                      chenrezig...) beside the strict
                                      convention; the prenasal rule
                                      Geshe Michael teaches (SKABS 'DIR
                                      spoken "kamdir") applied across
                                      the dictionary as a derived,
                                      review-pending class of 206 forms
                                      --- and verified never to
                                      contradict the dictionary's own
                                      attested pronunciations, which
                                      are internally consistent at
                                      every such juncture; and an
                                      honorific (zhe sa) register whose
                                      terms wear their badge, with the
                                      ordinary word beside them, in
                                      every pane of the application ---
                                      with a high-honorific level
                                      awaiting his marking
  ----------------------------------- -----------------------------------

**4. Conversion engines --- and how they are proven**

Every conversion engine was first perfected as a canonical reference
implementation during the dictionary project, then ported into the
application and proven against large validation batteries. A port that
fails its battery does not ship. Current results:

- ACIP → scholarly transliteration (EWTS): exact on all corpus
  conversions (100.000%).

- Transliteration → Tibetan script (Unicode): 99.03% on 26,318
  source-attested ground-truth pairs --- identical to the canonical
  engine, with every miss traced to a documented source defect.

- Pronunciation in Geshe Michael's convention: character-for-character
  identical to the canonical engine on all 105,576 dictionary headwords,
  with 2,118 printed-card pronunciations as ground truth.

- Sanskrit converter: IAST ⇄ Tibetanized ACIP ⇄ both official ACIP
  input-code styles ⇄ Tibetan script ⇄ Devanagari --- identical to the
  canonical engine on all 79,452 Sanskrit terms in the Lokesh Chandra
  data, including failing on exactly the same malformed inputs. A new
  Devanagari-reading engine round-trips all 73,437 renderings exactly.

- Authentic Sanskrit pronunciation: classical scholarly IPA (with
  correct anusvāra assimilation) plus a per-letter "how to say it" guide
  following the FPMT transliteration standard.

- Simplified pronunciation: validated against the Project's own standard
  (ACIP Release IV, pp. 173--178) --- 51 of the standard's 52
  pure-Tibetan chart examples reproduce exactly.

- The Tibetan calendar: the traditional Modern Karana arithmetic ---
  true month, true weekday, true sun, and the general day count ---
  ported from Edward Henning's authoritative open-source
  implementation and proven against the original program running
  beside it: 76 of 76 spot fixtures, then every month header (25/25)
  and every day (721/721) of two complete printed years, with all 17
  doubled lunar days verified as exact pairs. Year designations
  (rabjung, element-animal) are computed independently and agree with
  the ported formulas.

- Word segmentation: a faithful port of the openly-licensed botok
  tokenizer (the ecosystem standard, and deterministic --- a trie over
  a lexicon, not machine learning), proven token-for-token against the
  original across the full corpus, feeding a clearly-labeled reference
  layer in the reading pane.

- Affixed-particle normalization: the rules behind the Buddhist
  Digital Resource Center's own search engine, ported so that a search
  for a bare word also finds its case-built forms --- everywhere: the
  dictionary lookup, the corpus, and the installed library --- while
  words whose final letter belongs to the root are never over-stripped.

- An independent cross-check: the ecosystem's standard converter was
  run over all 105,634 dictionary entries as a diff-oracle. It agreed
  with our engine on 91.4% exactly; nearly all the rest are entries
  where OUR side refuses to guess (the master's honesty flags) while
  the other converter mangles blindly --- and 13 genuine source
  defects it exposed were filed for correction.

**5. A tour of the fourteen panes**

**Overlay --- read with the dictionary underneath**

Open any ACIP text. Every word and phrase the dictionary knows is
shaded, with nested phrases shown inside longer ones. Clicking a word
shows its entry, its occurrences in the corpus with Geshe Michael's
actual translations, and reference definitions. Spelling errors get a
red underline; grammatical particles that disagree with the classical
suffix rules get a gentle review mark. Even a word the dictionary does
not know is not left blank: it is checked against nearly half a million
attested word forms, so the reader learns at a glance whether it is a
real word awaiting an entry or a probable typo; and verbs show their
full tense paradigm from an openly-licensed verb database --- both
clearly marked as reference, never as the master's voice. A Display panel lets each reader
choose their own information density --- text as Tibetan script, ACIP,
or Wylie, with separate switches for phonetics, definitions, corpus
usage, grammar marks, and each reference layer --- and remembers the
choices. Two publishing tools live here as well: one-button export of
the whole text to print-ready Tibetan Unicode (page markers annotated,
correction brackets handled by the Project's own conventions, classical
shad punctuation), and a translation-prep formatter that reproduces
Geshe Michael's demonstrated Mixed Nuts workflow --- flowing the raw
input, breaking paragraphs at double shads, and moving input-operator
brackets into folio-tagged notes.

For texts of the Derge or Lhasa Kangyur and the Derge Tengyur, one more
button appears: *Follow along in scans.* The original woodblock page
images, served by the Buddhist Digital Resource Center from its public
archive, display directly beneath the text --- and they turn their own
pages: as the cursor moves through the input, the scan follows the
folio marker at the cursor, with the current line marked by a soft
band (the input centers preserved the carving's own line breaks, so
the line is known exactly; only the band's position on the image is
approximate, and it says so). The folio-to-image mapping is BDRC's own
catalog labeling --- nothing is guessed --- and pages are cached so a
text revisited costs nothing. Texts outside those catalogs (the
Sungbum, for instance) are linked by a title search against BDRC's
database: the query is pre-filled from the text's own opening line,
the translator confirms the right edition from the candidates --- the
match is never asserted automatically --- and the confirmed link is
remembered. And the following now reaches the single word: an opt-in
button reads the woodblock image itself with the embedded text
recognition described below and draws a box around the very word under
the cursor --- the recognition is used only to locate, never as text,
and when it cannot find the word it says so.

**Analysis --- the eighteen-section passage analysis**

A passage is first analyzed by the deterministic engine (segmentation,
verified term anchors, corpus matches); those verified facts are
injected into the eighteen-section analysis template and an AI model
writes the full report, which the machine then re-checks
(reconstruction, dictionary quoting, text integrity). AI output is
always labeled as such. Reports save automatically.

**Trainer --- learning to read**

Built directly on the pedagogy of breaking a passage into phrases and
clauses, then learning the order in which to read them. The learner
peels away layers one at a time: clause and phrase boundaries; each
particle's grammatical role; a suggested reading order (find the verb
first) with the direction to read inside each phrase; vocabulary; the
answer key --- Geshe Michael's own English where the passage exists in
the corpus; and finally a complete grammatical parse in Wilson's system:
every unit designated, case particles carrying their eight-case numbers
assigned by the verb's class, and every syllable-dot named per the
Science of the Dots --- reproducing the diagrams of Translating Buddhism
from Tibetan exactly.

**Drills --- exercises that write themselves**

Every exercise is generated from a real corpus segment, so every answer
is the translator's own text: restore the order of a scrambled clause;
pick the Tibetan chunk that fills a blank, given the English; choose the
correct particle variant (explained from the classical tables); read a
course in parallel, revealing the English only when needed; and review
vocabulary on a spaced-repetition schedule. The app quietly adapts:
words the learner clicks join their personal deck, missed skills are
named ("verb position", "genitive attachment"), and drill material is
chosen near the learner's level.

**Draft --- the working translator's bench**

The source text splits into clauses; each clause shows its verified term
anchors; each term opens a full corpus concordance (how the master
rendered it, source by source). The translator writes English alongside
and presses one button to check terminology: terms whose equivalents do
not appear in the draft, one term rendered two ways, drafts leaning on
provisional glosses, and one English word serving two different Tibetan
terms are all flagged for review --- never auto-corrected. Two
structural tools assist: automatic extraction of the text's own topical
outline (sa bcad), and a verse meter check that names the meter and
flags broken lines. An optional AI "back-check" produces a coverage diff
(omissions and additions only --- it is forbidden to retranslate) and is
banner-labeled as machine output.

The Draft bench also carries the shared apparatus, described in section
6 below: one search box over every published footnote and bibliography
entry, one-click reuse with the citation attached, a composer that
assembles new bibliography entries in the house format exactly ---
filling itself in from the Project's catalog for 1,846 known works by
their ACIP number --- and quotation detection --- when the text being translated quotes a work
that exists in the corpus, the app finds the quotation, shows how the
master already rendered those lines, and offers the published
bibliography entry and any published footnotes on that passage. A
draft can be exported as a rich-text file in which marked name-parts
become true italics, following the published convention for names
woven into verse.

**Library --- the translator's own shelf**

The user's text collection lives here: the complete ACIP Kangyur,
Tengyur, and Sungbum install from the Asian Legacy Library's own
archive files; personal materials import alongside (Word documents
convert automatically). A browsable tree decodes every catalog file
name into its provenance --- collection, text number, verification
level, language --- with direct links to the original scans on BDRC
where the catalogs align. Name search, filters by collection and
verification status, and a recently-opened list make a
hundred-thousand-page canon feel close at hand; a prebuilt search
index answers library-wide queries instantly; and a flat list view
lays the whole shelf out as one sortable catalog table --- now showing
each work's English title wherever the Project's catalogs record one. A
legacy-font rescue lane converts documents typed in the pre-Unicode
Tibetan fonts (TibetanMachineWeb, Sambhota and their kin) to proper
Unicode through the Trace Foundation's converter, run as a separate
external tool and honestly labeled.

**Scan --- Tibetan OCR, embedded and proven**

Text recognition now lives inside the application: the Buddhist
Digital Resource Center's open models (used with BDRC's permission,
and credited wherever output appears) run locally, line detection and
recognition both proven line-for-line against BDRC's own program
running beside them. A page scan opens, its lines are found and read,
and every recognized line is shown in transliteration and Tibetan
script with an immediate first-pass quality check that counts and
marks every syllable failing the classical legality rules --- nothing
is smoothed, everything is flagged, and all output is tagged
unverified review material that never enters the corpus. A whole
folder of page scans --- a scanned volume --- processes in one batch.
Every recognized word carries its exact position on the page, so
clicking a word in the reading lights it up on the woodblock.

**Align --- the Hypercontext workflow, reborn**

The Project's original Hypercontext tool --- the program whose
hypertexted files fed the first dictionary building --- lives again:
click a Tibetan word (the word-finder cycles through the possible
lengths), select its English, press space, and the pair is linked;
sublinks nest inside longer links in their own color. Links save per
text, and the harvest exports as clearly-PENDING candidate pairs for
the dictionary project --- translator-authored attestations, never
auto-ingested. Legacy .hyp files from the original era import
directly, proven against the original tutorial file itself.

**Input --- the input-center workstation**

The software the input centers were meant to have --- the ACE lineage,
recovered from its original author and rebuilt --- is now a pane: the
page scan above, the typing area below, and the scan follows the
typist's cursor line by line (exactly, when the line detector has run;
approximately, in the original program's manner, when it has not).
Illegal syllables underline in red as they are typed; a folio-marker
button inserts the next @-marker per the Project's own specification;
and the double-keying discipline is built in --- compare your file
against your partner's and every discrepancy lights up, to be
corrected until the two files match exactly, which has always been
the rule.

**Search --- the Gofer grammar, restored**

The classic search grammar lives on: exact phrases, OR, and
NEAR-within-N-lines, over the corpus or over any folder of raw ACIP
files on disk.

**Convert --- every writing system**

Type ACIP, transliteration, Sanskrit IAST, or Devanagari --- the pane
detects which --- and receive every representation at once:
transliteration, Tibetan script, pronunciation, both official ACIP
Sanskrit codes, Devanagari, and classical IPA.

**Lookup --- the stacked dictionary**

One search across the master dictionary (tier-labeled), the reference
layers, and English-to-Tibetan reverse lookup --- and the search meets
the searcher halfway: a community spelling like *gonpa* or *tulku*
finds the entry (labeled as the colloquial register's work, with the
strict convention shown beside it); honorific terms wear their zhe-sa
badge with the ordinary word as cross-reference; a Sanskrit root or
even its past participle opens Whitney's entry with his meanings, verb
classes, and the exact sections of his Grammar --- one click reaching
the public-domain scan of the Grammar itself or the corresponding
Monier-Williams entry.

**6. The shared apparatus --- work performed once is never redone**

Geshe Michael has directed that footnotes created in the released Mixed
Nuts translations can and should be reused by translators as the same
topics come up. The application makes that directive a working system,
under a strict two-tier rule: **the official apparatus contains only
what has been published and fully approved** --- today, 344 footnotes
and 138 bibliography entries extracted from the three released volumes
--- and is compiled into two centralized MAIN documents that regenerate
automatically whenever a new volume is released. They are never edited
by hand and never fed from working drafts. Notes a translator proposes
along the way wait in a clearly-marked pending queue; there is no
"approve" button in the software, because approval is an act of the
publishing process, not of the app. Every reuse carries its source
citation, so borrowed scholarship stays attributed; and because the
published bibliography entries embed the Project's own catalog
numbers, a detected quotation, its citation, its bibliography entry,
and its published footnotes all connect to one another --- and to the
original woodblock scans --- through one deterministic chain.

**7. The grammar layer**

A deterministic grammar engine, built as rule tables after the framework
of Wilson's Translating Buddhism from Tibetan, underlies the Trainer and
Overlay: the complete case-particle system with suffix-agreement
checking (calibrated against the corpus --- 95.8% of one hundred seven
thousand particle pairs agree, and the exceptions are precisely the
famous da-drag stems); clause segmentation with honest disambiguation of
the particle na; phrase chunking by case roles; verb identification from
the dictionary's own tense data (never a guess --- 69.5% of the corpus'
147,902 clauses are identified with evidence, the rest say so); and
Wilson's eight verb classes, which correct the reading guidance ---
under a verb of absence, for example, the instrumental-looking phrase is
"empty OF x", not an agent. On top of the classes sits the full per-unit
parse: Wilson's eight cases assigned contextually by verb class,
negation and infinitive constructions made transparent, and his "Science
of the Dots" --- a designation for every syllable-separating dot (S, C
with its case number, NOM, SP, V, VB) that reproduces the book's own
worked examples.

**8. Verification**

The application carries thirty-six automated test suites --- batteries
for every engine, grammar calibrations against the full corpus,
exercise-generator checks, and end-to-end smoke tests --- all of which
must pass before any change ships. Where a result cannot be verified
mechanically, the interface says so rather than implying certainty.

The script engine has now also been cross-examined by two outside
witnesses: two independent open-source converters --- the ecosystem's
Python standard and the reference implementation the whole field
descends from --- each run over all 105,634 dictionary entries, and
agreeing with each other to within three hundredths of a percent. The two agree exactly on 91.4% --- and nearly every
disagreement is a place where this tool's engine refuses to guess at
defective input while the other converts it blindly, a direct
vindication of the never-guess rule. The comparison also surfaced
thirteen genuine defects in the source data, now reported back to the
dictionary project for correction.

**8a. Time itself**

The Convert pane now answers the translator's dating questions in
every direction: a western year yields its full Tibetan designation
(element, animal, rabjung cycle); an element-animal pair yields its
years; a full Tibetan date (lunar day, month, year) yields the western
weekday and date --- and the reverse --- with the calendar's own
irregularities honored rather than smoothed: doubled and omitted lunar
days are named as such, intercalary months appear in their correct
place with their correct names, and dates before the epoch are refused
rather than approximated.

**9. The road ahead**

- The Master Catalog harvest is in: 2,955 paired titles now feed the
  corpus, the bibliography composer fills itself in, and the Library
  shows English titles. One ruling remains --- whether the catalog's
  English titles stand as Geshe Michael's own binding equivalents (he
  directed the cataloging) or as a labeled reference layer.

- The compilation of Geshe Michael's translation standards continues:
  eleven sourced standards so far, mined from the published volumes'
  own notes and prefaces --- several already running as software
  features (the technical-spelling apparatus, italicized name-parts in
  export) --- with a prepared review sheet so proposed footnotes can be
  approved in his own sessions, never in the software.

- The registers grow under review: the prenasal pronunciation class
  (206 derived forms awaiting Geshe Michael's confirmation), the
  honorific register's high-honorific markings, and the reference
  books on honorific usage and grammar now being gathered.

- The input-center formatter: converting raw input-center documents to
  finished Tibetan Unicode, once the conversion standards document is
  supplied --- with the new Input pane already carrying the typing,
  checking, and double-keying half of that workflow.

- Cataloging has been deliberately set aside as its own future
  project; the knowledge mined from the Project's original cataloging
  toolchain is banked and waiting for it.

- Windows and Linux builds for the input centers.

*Prepared with reverence for the lineage of translation this tool exists
to serve.*
