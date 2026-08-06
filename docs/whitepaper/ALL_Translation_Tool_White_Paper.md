**The ALL Translation Tool**

*A complete working environment for Tibetan texts and their translation*

Asian Legacy Library · Asian Classics Input Project --- White Paper,
August 2026

**1. Purpose**

The ALL Translation Tool is a desktop application that gathers, in one
place, every function needed to work with a Tibetan digital text:
reading it, looking words up, converting between writing systems,
analyzing grammar, drafting a translation, checking that draft, and
training the next generation of translators. It is built around one
central treasure: the dictionary of English equivalents established by
Geshe Michael Roach across decades of teaching and translation, together
with a parallel corpus of 37,807 Tibetan--English segments drawn from
more than sixty of his courses and works --- including, as of August
2026, three newly ingested published volumes: A Song of My Spiritual
Life, The Other Side of Emptiness, and The Sixty Verses of Nagarjuna.

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

  Parallel corpus                     37,807 aligned Tibetan--English
                                      segments from 60+ sources and
                                      published volumes --- the evidence
                                      base and the "answer key" for all
                                      training exercises

  Lokesh Chandra layer                79,305 Sanskrit-linked entries;
                                      88,973 headwords with IAST Sanskrit

  Reference dictionaries              445,772 definitions from legacy
                                      tools (multi-scholar compilations),
                                      kept strictly local as labeled
                                      reference --- never redistributed
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

**5. A tour of the eight panes**

**Overlay --- read with the dictionary underneath**

Open any ACIP text. Every word and phrase the dictionary knows is
shaded, with nested phrases shown inside longer ones. Clicking a word
shows its entry, its occurrences in the corpus with Geshe Michael's
actual translations, and reference definitions. Spelling errors get a
red underline; grammatical particles that disagree with the classical
suffix rules get a gentle review mark. A Display panel lets each reader
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
layers, and English-to-Tibetan reverse lookup.

**6. The grammar layer**

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

**7. Verification**

The application carries fourteen automated test suites --- batteries for
every engine, grammar calibrations against the full corpus,
exercise-generator checks, and end-to-end smoke tests --- all of which
must pass before any change ships. Where a result cannot be verified
mechanically, the interface says so rather than implying certainty.

**8. The road ahead**

- The input-center formatter: converting raw input-center documents to
  finished Tibetan Unicode, once the conversion standards document is
  supplied.

- Windows and Linux builds for the input centers.

- Deeper training: difficulty-ramped daily practice,
  translate-then-compare workbenches, and register studies across the
  corpus.

*Prepared with reverence for the lineage of translation this tool exists
to serve.*
