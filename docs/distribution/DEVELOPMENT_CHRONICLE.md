# The ALL Translation Tool
## A Development Chronicle — Seven Days, Three Hundred Increments

**Adam Derick Andrade · Asian Legacy Library · August 2026**

---

This is the complete development record of the ALL Translation Tool:
a native desktop research suite for translators of classical Tibetan,
built for the Asian Legacy Library on the foundation laid by the
Asian Classics Input Project. It went from an empty repository to a
pressed, installable, self-testing application — nineteen working
panes, forty automated test suites, roughly forty-five thousand lines
of code — in seven days of continuous, disciplined iteration:
**333 recorded increments between August 6 and August 12, 2026.**

I am publishing this chronicle for two reasons. First, because the
project's method is as much the product as the software is: every
feature below went through the same loop — idea, ruling, build,
prove, document, press — and the record shows how quickly a serious
tool can take shape when that loop is tight. Second, because the
decisions matter more than the code. At every fork in this project
there was a design question — what should a translator see first?
what may a machine never do? what does honesty look like in a user
interface? — and this document records what I decided and why.

---

## The Vision

Two goals, stated on day one and never revised:

1. **Save translators time.** Every layout decision in the tool is
   settled by one measure: does this shorten the path from "I need
   X" to "X is on screen"? I call it the translator's minute — it is
   the unit of value the whole application is priced in.
2. **Protect the integrity of the tradition.** The tool serves a
   living translation lineage. Its data model enforces, in code,
   the same discipline the lineage enforces in practice.

From those goals I derived four inviolable rules, written into the
project's constitution before the first pane existed:

- **The binding layer is sacred.** Geshe Michael Roach's English
  equivalents are the point of the whole corpus. Reference
  dictionaries — Hopkins, Lokesh Chandra, Das, 84000 — are
  comparanda, always labeled, never promoted. Machine processes may
  *match* the master's English from corpus evidence; they may never
  *compose* it.
- **Never reconstruct an engine from memory.** Every conversion
  engine (transliteration, phonetics, calendar, segmentation) is
  ported from its canonical source and proven against a validation
  battery before it ships. A port that fails its battery does not
  ship.
- **Never guess.** A syllable the converter cannot prove renders as
  ⟨wylie⟩ in visible markers — in the reading pane, in the PDF
  export, everywhere. Failed parses are flagged and surfaced, not
  silently approximated. This rule turns out to be a *feature*:
  users trust a tool that admits what it doesn't know.
- **Provenance is sacred.** Every generated datum carries its source
  tag; every approval carries a name and a date; every tier label
  (curated / glossary / provisional) survives every transformation.

---

## Day One — August 6: The Foundation

The first day established the working method along with the working
software: small increments, each committed with its rationale, each
documented the hour it shipped.

- **The Library** came first — the reader's home for the ACIP
  collections (Kangyur, Tengyur, Sungbum): name search, recents, a
  prebuilt full-text index, and hand-off into OCR for scanned
  material.
- **The translation-standards program** began the same morning: I
  started compiling the house's sourced translation standards
  (STD-001 through STD-008 by nightfall) and immediately turned
  them into working features rather than shelf documents — the
  technical-spelling helper, the house bibliography composer, and
  the **shared footnote bank**, implementing the master's directive
  that published apparatus should be *reused*, not rewritten.
- **Two-tier apparatus architecture**: official published documents
  in a MAIN tier, candidates in a pending queue — the first
  appearance of a pattern that would organize the whole project:
  *official and provisional material never mix silently.*
- **Quotation detection**: passages that exactly match corpus
  segments are flagged as attested quotations — and recommend the
  published footnotes for the works they quote. Detection is exact
  match only; nothing is inferred.
- **Scan follow-along**: the BDRC woodblock page tracks the cursor's
  folio as you read, with an approximate line-band following the
  cursor's line. Reading the e-text and checking the carving became
  one motion instead of two applications.
- RTF draft export with typographic italics, Sungbum scan linking by
  confirmed title search, and the first white paper closed the day.

**The instinct on display:** ship the standard *as a tool*, not as a
document. A style guide nobody opens saves nobody's minute.

---

## Day Two — August 7: The Reference Day

The heaviest single day of the project — the day the tool acquired
its scholarly depth.

- **The catalog waves**: 2,955 bilingual title pairs, Sungbum
  subject classifications, ACIP's own bilingual subject headings,
  and author-name pairs, each consumed as a versioned data wave with
  its provenance recorded. The bibliography composer began
  auto-filling from the catalog — 1,846 works by ACIP number.
- **The segmentation engine**: I ported the botok Tibetan word
  segmenter to C++ in five recorded increments — chunking layer,
  trie and maximal-match tokenizer, segmenter facade, a compact
  trie so the Monlam word lists could join the lexicon, and
  affix-split display — each increment proven against the canonical
  implementation at corpus scale before the next began.
- **The Tibetan calendar**: a colophon-dating helper at year level
  ("fire bird" ⇄ 1357) grew by day's end into a full day-level
  calendar — ported and proven **76/76 exact** against the
  canonical implementation, then extended to intercalary months
  with 721/721 days exact. A translator dating a colophon never
  leaves the app.
- **Affix normalization** (the pa/ba and verb-lemma rules from
  BDRC's search stack) applied uniformly across corpus search,
  library search, and lookup — so *po'i* finds *po*, labeled as an
  affixed form. One normalization authority, three surfaces.
- **Pronunciation search**: typing "jangchub" finds *byang chub* —
  the first brick of a larger phonetics program.
- **The reference layers arrived in force**: the Mahāvyutpatti
  (the classical Sanskrit–Tibetan glossary), the SOAS part-of-speech
  lexicon (which finally resolved the catalog's mysterious
  grammatical dot-codes *by evidence*, one dot class per commit —
  NA, ADV, UP, OM — never by guess), the TibSchol abbreviations
  table, a CC0 verbs database, and Monlam attestation lists.
- **Independent cross-validation**: I diff-tested my
  wylie→Unicode engine against a second, independent codebase
  (THL's converter) across the full dictionary — two independent
  implementations agreeing is worth more than any code review.
- **OCR stage 2**: line detection, line building, and CTC
  recognition ported to C++ — each stage exact against its
  canonical pipeline — ending in a working Scan pane that reads
  Tibetan woodblock images.
- **The ancestors surveyed**: I audited the historical ACIP
  development folders — two decades of prior tools — and
  resurrected the best of them the same day: the Hypercontext
  aligner's authoring surface (the Align pane), the UTFC legacy
  font rescue lane, and the bampo/shloka structural layer. Fonts
  were collected, vetted for license, and bundled (Noto Serif
  Tibetan, then BabelStone, later Jomolhari and Monlam Bodyig —
  every one under a redistributable license, verified).

**The instinct on display:** depth through *proof*. Nothing joined
the reference stack without either a license check or a battery —
usually both.

---

## Day Three — August 8: Registers and Recovery

- **The honorific and colloquial-pronunciation registers** began — 
  at my request, because a dictionary that cannot tell you *zhe sa*
  from plain speech is not a translator's dictionary. Honorific
  badges appeared in the lookup cards, then everywhere.
- **The prenasal pronunciation program**: the four prenasal systems
  of spoken Tibetan mapped (n versus m, the g-coda class), then the
  derivation *rebuilt* when the first version proved sloppy —
  syllable-aligned, word-internal, honest. Every derived register
  row was marked derived, pending review. (This discipline paid off
  four days later when the whole set flowed through the approval
  channel as 205 reviewable proposals.)
- **The Input pane — ACE reborn**: the input-center workflow
  recovered from the historical ACIP codebase and rebuilt native:
  the page scan above, the typist below, the scan following the
  typing cursor, and the classic *double-keying comparison* — two
  operators type the same page and the machine diffs them — running
  on a vendored, proven diff engine.
- **The reverse transliterator** (EWTS → ACIP), round-trip proven,
  so wylie files could live in an ACIP-native interface.
- **Whitney's Sanskrit reference** (Roots 1885, Grammar 1879) joined
  Lookup with per-homonym classes and live-verified deep links into
  Monier-Williams.
- **The oversight loop opened**: a Review pane for translation
  oversight; **phrase memory** ("how has this phrase been rendered
  before?" — answered only from attested renderings); and the
  **proposal & approval channel** — the team proposes, the authority
  approves, every ruling stamped with name and date. I scoped
  cataloging *out* the same day — a deliberate no is a design
  decision too.
- A leadership overview document closed the day: every function, the
  design story, the vision — written for the humans who would
  decide the project's future.

**The instinct on display:** when a derivation is sloppy, rebuild
it before anyone depends on it — and route every machine-derived
judgment through a human authority with provenance.

---

## Day Four — August 9: Verification Becomes Culture

- **The self-test harness**: a `--selftest` mode in the application
  itself, growing through four recorded waves in a single day to
  43 checks — every pane covered, plus robustness cases and a
  performance-regression guard. From this day forward, no build
  shipped without the app *testing itself*.
- **Screenshot mode** (`--screenshots`) rendered every pane
  automatically — for documentation that can never drift from the
  product, and later for design audits.
- **The distributable build**: a pressed macOS DMG with a release
  gate — build, full test suite, staged launch test — so "it works
  on my machine" was retired as a concept.
- **The release validator**: a customs check at the data border,
  validating every incoming data release before installation.
- **The approval channel closed its loop both ways** (proposals from
  cards, rulings back into the app), and the prenasal review flowed
  through it — the ruling mutation itself moved into the core
  library with its own proof.
- **The honorific evidence hunt**: four scholarly sources mined in
  one evening (Hackett's verb lexicon, the Manual of Authentic
  Tibetan, Goldstein's Essentials, and the MST appendix that
  finally answered the four-level honorific taxonomy question) —
  each contributing register rows *with its evidence named*.

**The instinct on display:** testing is not a phase, it is a
faculty. The app carries its own verification everywhere it goes.

---

## Day Five — August 10: The Workshop Becomes an Application

- **Information architecture, round one**: I reorganized the growing
  pane collection into six workflow groups — Read, Translate,
  Research, Learn, Input, Community — with a menu bar that mirrors
  every pane's controls, a Settings dialog, a Suggested Workflows
  opener ("Where do I start?"), and a searchable Help system with a
  tutorial for every pane.
- **Gofer reborn**: the original input-center search tool — the
  master's own daily search interface — rebuilt over the modern
  engine, its classic multi-term OR / AND / NEAR-within-lines
  grammar intact.
- **The Manuscript pane**: a rich-text writing surface with a
  corpus sidebar, because the end of the pipeline is a finished
  translation, not a database row.
- **The Sanskrit pane and Sanskrit OCR**: Devanagari images feed
  the converter chain.
- **The full published apparatus ingested**: 2,234 footnotes and
  705 bibliography entries from the released volumes, live in the
  cards, in Review, and in search.
- **Performance discipline**: a full canonical volume opened in
  1.9 seconds by day's end (from an infinite hang two days
  earlier), with the diagnosis recorded so the mistake class —
  quadratic scans hiding in paint paths — stays fixed.
- **The app got its face**: the letter ཨ on manuscript cream, then
  my final pick — gold on maroon; night mode by default with the
  reading surfaces deliberately kept warm parchment (the page glows
  under a reading lamp; the chrome recedes); the longest-phrase
  click-to-highlight implementing the master's own reading
  directive.
- Idioms register banked and wired; spelling-doubts working list;
  the distribution document set (white paper, technical paper,
  feature guide) refreshed to match reality.

**The instinct on display:** identity is a design deliverable. A
tool people live in needs a face, a mood, and an obvious front door.

---

## Day Six — August 11: The Living Layers

- **The People layer**: the catalog's 266 authors resolved against
  the BDRC knowledge base in three recorded passes (151, then +25
  with a paged phrase-window search when I found the first pass had
  been reading only page one, then +8 Sanskrit panditas via
  accent-insensitive name matching) — authors in the Library link to
  their full biographical records, and their other works link back
  into the Library.
- **The Das layer**: Sarat Chandra Das's 1902 dictionary, in-app at
  approximately the right page, built from my own bookmark map of
  the syllable onsets.
- **The teaching index**: the project's most original layer. Every
  recorded teaching of Geshe Michael's with caption tracks —
  harvested with authorization, provenance recorded — indexed
  against the dictionary's own vocabulary, so a term's card can say
  **"Geshe Michael teaching this term"** and jump to the moment in
  the recording. By day's end: both YouTube channels, a
  spoken-Tibetan phonetic tier ("he says this word"), a
  context-fit-ranked "Teachings for this text" report, and a hard
  evidence rule I set personally: classes in other languages are
  excluded entirely, because machine-translated captions are not
  evidence.
- **The woodblock graphics program**: illustration-candidate
  geometry (regions of the folio not covered by text lines — where
  the miniatures live), candidate overlays, a folder-wide
  illustration gallery, and finally whole-volume galleries with a
  per-work scan cache — after I diagnosed, from a live probe, why
  the first cache-matching design could never match.
- **The testing campaign formalized**: a release checklist; a
  permanent *fit sweep* (every pane must fit an 800-pixel window);
  a library health census (11,700 files, 204.6 million tokens, an
  input-error map); session restore; an About dialog; independent
  app versioning; in-app update checking.
- **The complete User Manual** — every pane, every control — written
  and then *wired into the app* as browsable Help chapters, because
  documentation that lives outside the product dies.

**The instinct on display:** connect layers. A dictionary entry
that can show the master *saying the word aloud*, the woodblock
*carving of it*, and its published footnote is more than the sum of
its files.

---

## Day Seven — August 12: The Marathon

The final recorded day runs from before dawn to past midnight and
contains over a hundred increments. It divides into six movements.

### Movement I — The "Wow" Features

I set out to build the features that make translators sit up:

- **The Evidence Ribbon**: as the cursor moves through a source
  text in the Draft workbench, the evidence *follows it* — clause
  anchors, a reading scaffold (chunks, the verb, reading order),
  exact corpus parallels ("the master has translated this clause"),
  and quotation alerts — with zero clicks.
- **Hunt Everywhere (⌘K)**: one palette, one query, every source —
  dictionary, English reverse index, pronunciation, corpus,
  teachings — with Enter routing into the right pane.
- **The Four-Layer view**: one woodblock folio as scan · OCR ·
  e-text · attested English, row-aligned, click-to-light-the-band.
- **The Translator's Survey**: measure a text *before* committing
  to it — tier coverage, unknown forms, quotations, structure,
  verse, and a difficulty estimate with its formula printed
  (an estimate that hides its formula is an opinion).
- **The terminology live-guard**: a spellcheck for renderings — as
  you draft, an unobtrusive line notes established equivalents not
  yet used. Advisory only; it never writes English for you.
- **The Citation Web**: which works quote which, across the whole
  corpus — 500 attested edges, no fuzzy matching.
- **Predictive ACIP typing** in the Input pane, completing from the
  dictionary's own headword inventory — deterministic, nothing
  invented.
- **Arrow-key reading** and a whole-text **Pronunciation display
  mode** in the reading pane (my request after watching real
  sessions): the entire text as phonetics, with the engine's own
  word segmentation so *bsod nams* renders as one spoken word.
- **The rulings layer**: when I caught the engine printing *kabdir*
  where the master says *kamdir*, the fix wasn't a hack — approved
  pronunciation rulings became a live layer that overrides the
  engine wherever the authority has spoken.

### Movement II — The Dictionary Program

- **The ALL Working Glossary** — my proposal for the dictionary's
  future: machine-drafted *provisional* entries, in the deep-entry
  form of the great dictionaries (numbered senses with context
  prose, part of speech, Sanskrit, cross-references) — but under a
  wall I consider absolute: drafts exist **only** where the master
  has not defined a term, are grounded **only** in the open
  reference layers, live in their own violet-bannered tier, and are
  suppressed the moment the binding layer speaks. Part-of-speech
  tags come only from hand-tagged scholarly evidence; every
  cross-reference resolves to a real headword; "insufficient
  evidence" is an accepted answer. 1,160 entries drafted by
  midnight, zero forced.
- **The 84000 glossary**: 24,576 terms from the canonical
  translation project, ingested as a toggleable layer after I
  verified the license from the source's own terms — and filed the
  contradiction I found between their stated license and a stale
  repository notice.
- **The dictionary strategy dossier**: the whole field surveyed and
  sorted into licensing lanes — ingest now (open-licensed), public
  domain by calendar (with dates), licensable (with a contact
  plan), and circulating-but-never-ship. The lane system means the
  project can grow its reference stack for years without ever
  cutting a legal corner.

### Movement III — The Pecha Maker

A traditional long-format pecha is a typographic act with fixed
conventions, and I wanted producing one to be a first-class feature:

- **v1**: framed folio sides, rotated Tibetan folio numerals,
  choice of page measures, optional phonetics interlinear, live
  preview — every syllable through the proven script chain.
- **v2**: the traditional dress — an ornamental title folio (1a,
  with the text beginning on 1b, as in the woodblocks), the margin
  title and volume letter in the rotated recto margin, the opening
  head mark, three rule weights, and **two-up imposition with cut
  marks** so any office printer can produce foldable folios.
- **The corpus-attested English interlinear** — the feature I am
  proudest of in this movement: a segment gets its English beneath
  it *only* when that exact segment exists in the aligned corpus of
  published translations. A verbatim match or nothing. The build
  proves this end-to-end: a self-test finds a real attested
  segment, renders it, and verifies the published English landed in
  the PDF.
- **v2.1**: batch mode (a whole folder becomes a pecha set), the
  Degé woodblock measure (68 × 10 cm), and the **classical
  opening** — the first two text sides at five lines with
  proportionally larger letters, exactly as the great blockprints
  do it — which required rebuilding the text-flow engine to
  re-measure line height per side.

### Movement IV — The UX Program

Mid-marathon I stopped adding and started *judging*. I commissioned
a full information-architecture audit of all 19 panes and 145
controls, wrote a principled plan (a pane is a place of work, not a
drawer; the viewer follows the work; one obvious home per function;
progressive disclosure; empty states that teach), and executed it:

- **The Translate ladder**: Draft is the *workbench* (understand
  the source, gather evidence); publishing tools moved to
  *Manuscript*; the path reads Workbench → Manuscript → Review →
  Export.
- **The naming collision dissolved** (woodblock "Scans" versus
  input "Scan" → the latter is now OCR), duplicated controls given
  one home, the calendar given its own labeled section, the
  reading pane's dense display panel regrouped under three labeled
  headings, library maintenance folded into a menu, the propose
  form slimmed to its job.
- **Export and Scans became their own panes** in the Read group —
  my reorg after using the tool at length: one-shot actions do not
  belong inside the reading surface.
- **Design polish with rulings I'll stand behind**: the
  nested-phrase highlight became an "X-ray" — a light field for
  the longest phrase with terms darkening by depth, in an opaque
  amber ramp (chosen over transparency because stacked washes
  drift off-hue); the parchment reading surfaces stay parchment in
  night mode, deliberately, because the page under a reading lamp
  *is* the product's identity.
- **Performance findings fixed the same day they were found**: the
  Translator's Survey went from minutes to 1.2 seconds on a
  273,000-token volume (two quadratic scans replaced by
  single-pass structures), and the teachings report to 33
  milliseconds — each with a permanent benchmark guard so the
  regression class is dead, not just the instance.

### Movement V — The Scholarly Standards Sweep

I directed a systematic survey of the Tibetan & Himalayan Library's
entire published standards corpus — the field's reference
infrastructure — and brought it home:

- **The complete EWTS transliteration specification** (all sixteen
  rules, all charts, a 246-entry character map) and **the complete
  THL Simplified Phonetics standard** (every rule, every example,
  the full exception list) — captured with sources cited and gaps
  flagged rather than filled.
- **Their text-input, proofreading, scanning, and cataloging
  standards** distilled into a new in-app **"Working with Tibetan:
  Guidelines & Protocols"** layer — per-medium chapters (print,
  digital text, transliteration, pronunciation, scans, catalogs,
  audio) merging the field's standards with this house's rules,
  every claim cited.
- **A numbering trap avoided**: the assumed equivalence between two
  major catalog numbering systems was *refuted* by a twenty-record
  live verification before a single wrong link shipped — the
  offsets drift, and the record of that refutation now protects
  every future integration. All 1,118 catalog records were then
  harvested for a proper concordance.
- **The THL Phonetics engine, same night**: implemented
  rule-for-rule from the captured standard and proven against the
  standard's own published examples — **139 of 139 cases**,
  including all nineteen exceptions — then shipped as a second
  pronunciation display mode, credited on-screen, with the one
  internal contradiction in the source resolved rules-first and
  documented. The scholarly standard and the master's convention
  now sit side by side in the same menu, each labeled as what it
  is.

### Movement VI — The Archive Programs

The same day, two preservation programs went from idea to running
system:

- **The recordings archive**: a master inventory of every known
  recording of the master's teachings — 3,633 seeded from the two
  public channels with full titles and links, then 377 legacy links
  from my old course spreadsheets probed one by one: **68 unlisted
  recordings** (reachable only by their links — the class that
  vanishes silently) recovered into the tracker, 309 more found
  live on other channels, zero dead. A leads sheet tracks the
  unpublished holdings and the personal-collection drive.
- **The translations archive**: a contributor drop folder with a
  full briefing README (provenance questions, a restricted-material
  marking convention, a promise list of what happens to every
  contribution), plus a master tracking spreadsheet designed for
  non-technical archival helpers — full titles, no abbreviations.
- **The transcription program**: after documenting that platform
  auto-captions corrupt Tibetan terms, I surveyed the current
  speech-recognition field, selected a service on the strength of
  its custom-vocabulary capacity, and — the key move — built the
  vocabulary itself from the project's own corpus: the thousand
  most-frequent terms *in their spoken phonetic form*, with a
  mapping table back to canonical spelling for the post-pass. The
  archive's own data teaches the transcriber its vocabulary.

### The Night Shift

The day ended with a mechanized design audit of every pane —
which caught clipped labels, a dead link, unexplained empty
surfaces, and one real bug (a pane whose data loaded after its
list was built, masked by its own test — the test was rebuilt so
that class of bug can never hide again). The fixes shipped the
same night, along with a hardened testing harness: after a stack
trace proved that *any* native file dialog could hang the
automated control sweep, every file dialog in the application was
routed through sweep-aware wrappers. The final press of the night
carried the phonetics engine, the audit fixes, and a bulk-approval
workflow that turns a 205-click review queue into one considered
ruling.

---

## The Numbers, as of August 12

| | |
|---|---|
| Recorded increments | **333** in seven days |
| Working panes | 19, in six workflow groups |
| Code | ≈ 45,500 lines; ≈ 90% original to this project |
| Automated test suites | 40 (ctest), plus ~105 in-app self-checks |
| Engine batteries | transliteration (full-dictionary diff, two independent oracles) · GMR phonetics (42 + 2,118 attested lines) · THL phonetics (139/139 of the standard's own examples) · calendar (76/76 days, 721/721 month grid) · segmentation (corpus-scale vs. canonical) |
| Dictionary | 105,634 entries; binding tier + labeled reference layers |
| Aligned corpus | 35,601 Tibetan–English segments |
| Teaching index | 243,231 timecoded moments across 3,413 recordings |
| Apparatus | 2,234 published footnotes + 705 bibliography entries |
| Recordings inventory | 4,010 recordings tracked, 68 unlisted recoveries |
| Working Glossary | 1,160 provisional deep entries, zero forced |

---

## What This Record Shows

Reading back over seven days, the through-lines are clear to me:

1. **Rules before features.** The four inviolable rules were written
   before the first pane. Every hard call afterward — what the
   glossary may draft, what the pecha may print, what a card may
   claim — was already decided in principle.
2. **Prove, then ship.** Nothing conversion-critical shipped on
   inspection alone. Ports were proven against canonical
   implementations; standards were implemented against their own
   published examples; and when a source contradicted itself, the
   contradiction was documented and a policy chosen — never papered
   over.
3. **Honesty as interface.** Provisional data looks provisional.
   Failed conversions are visible. Estimates print their formulas.
   Empty screens teach. The tool's credibility *is* a feature, and
   it is designed like one.
4. **Find it, fix it, guard it.** Every performance problem and
   every bug class found in live use ended with a permanent guard —
   a benchmark, a self-check, a harness rule — so the record grows
   monotonically safer.
5. **The archive is the horizon.** The tool exists so that a
   tradition's texts, teachings, and recordings survive with their
   provenance intact. Every design decision above bends toward that.

The project continues: a print-production pecha program, the
catalog concordance, the re-transcription of the teaching archive,
partnership conversations with the field's institutions, and the
long patient growth of the dictionary — each already scoped, each
already governed by the same rules.

*— A.D.A., August 2026*
