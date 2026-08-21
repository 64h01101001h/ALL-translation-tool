# DCC cataloging-agent demo (Lon & Tom) — what it is, what we take

*2026-08-18 class, "Mixed Nuts Translation with Geshe Michael
(Xuanzang's Tower)", youtube D73k1RUDBv8, 1:05. Analyzed from the
full auto-caption transcript at Adam's request. Demonstrator: "Lon,"
a programmer-scholar friend of the cataloging class (17 years, met at
Diamond Mountain; in Thailand with partner Tom), presenting to Nick's
cataloging class with GMR responding at the end.*

## What was demonstrated

An **AI-agent librarian/cataloger** over the DCC corpus (their copy:
9,000 files, 6,800 distinct works — Kangyur ~800, Tengyur 3,000+,
Sungbum ~2,600, June snapshot):

1. **Corpus analysis + visual subject trees** per collection, with
   completion stats (how many works have English titles).
2. **Unrequested Tohoku concordance** for the Kangyur + missing-file
   report.
3. **Filename/metadata QC sweep**: canonical author-name inference
   (16 authors, 41 misspellings), year typos, glued names, missing
   authors; offers to fix in place once trusted.
4. **Title↔translation mismatch detector** (6 found where the English
   half of a filename belongs to a DIFFERENT text; suggested the
   correction with provenance).
5. **A "GMR catalog dictionary"** auto-built from the 2,800 bilingual
   title pairs: 3,800 English–Tibetan word pairs + ~200 subject-name
   pairs, used to keep FUTURE title translations consistent with past
   ones.
6. **37-field catalog schema** (identity 10, people/roles, genre/
   commentary-on, the five great subjects, translation status, where
   GMR taught it, completeness) + **77 doctrinal topic tags** +
   **life-application tags** (money/business, anger, relationships).
7. **Semantic search**: "karma" filters texts that are ABOUT las in
   the karma sense — not textual las matching.
8. **Conversational librarian**: chat over their data only, answers
   in GMR's own translation vocabulary, multilingual, reachable via
   web/Telegram/WhatsApp; can write approved changes to Dropbox with
   an HTML changelog.
9. **Automated cataloging of a new volume**: given the raw ACIP file
   of Sungbum vol. 3/4 — splits works at ACIP delimiters, checks
   duplicates against the corpus, proposes title translation with a
   word-by-word breakdown and a SOURCE for each choice ("found in
   Course 18 reading 9"), proposes shelving with rationale, fills the
   37 fields, waits for human approve/correct; **detected two
   different works sharing one title by their distinct colophons**.

## GMR's responses (the value signals)

- **"There's a trick I taught Adam"** — the longest-match dictionary
  walk: add a syllable while the lookup still matches, display the
  longest string that makes sense ("it's going to revolutionize the
  dictionary"). **This is exactly our overlay lattice** —
  longest-span-first is its design. On the record: our approach is
  his stated method.
- **His 23-item Russian physical catalog format** (12 years of use):
  paper W×H plus **printed-area W×H as a woodblock FINGERPRINT** —
  they matched Mongolia↔Russia copies from it — ink type, paper and
  print quality, multi-volume format, binding. He will send the list
  to Lon; **Adam should request a copy** — it directly serves our
  scan/edition work.
- **Chölen (Q&A genre) topic-splitting** as a wished-for use (his own
  month of manual splitting).
- Praise for Lon's "sensitivity to the old-school people"; a Luddite
  parable; and a ruling that the work should be **paid, via
  sponsors** ("it's a beautiful gift to them").
- Verdict: "use it next week… apply it slowly to small tasks."

## What our tool already has (no action)

Longest-match lattice (the "trick") · Tohoku + KL concordances,
verified not inferred · catalog-title ingest (Élie CSV, Sungbum
layers) · author-name resolution pass · flag-don't-fix conventions ·
proposal/approval channel (their approve-workflow equivalent).

## Adoptable — in our architecture, under our rules

1. **Title↔translation mismatch QC** over the library's bilingual
   filenames — deterministic, honest, small. (Their 6 hits argue ours
   will find some too.)
2. **Doctrinal-topic + life-application tag layers** on the Library
   info panel/filters — but topic ASSIGNMENT is machine judgment, so
   it ships as a labeled PROVISIONAL layer through the proposal
   channel, never silently (rule 1 analog).
3. **Catalog-title gloss tier**: GMR's published title translations
   are MATCHED English (his own words) — a legitimate labeled
   reference layer for title vocabulary; we already hold the pairs.
4. **Colophon-aware duplicate/same-title detection** for the library.
5. **The 23-item physical format** — adopt the fields into our
   catalog schema when Adam obtains the list; the printed-area
   fingerprint could join the scan viewer's info panel.
6. **Conversational librarian** — the one piece needing an LLM API;
   already Adam-gated under API credits. If built: retrieval over OUR
   spine + corpus, answers restricted to attested material, tier
   labels carried through.

## Coordination note

Three tool efforts now orbit the same corpus: Lon's cataloging agent
(DCC class), Jamie/Ora's TKB archive work, and this translation tool.
Lon's agent COMPOSES suggested title translations (human-approved) —
their choice for their workflow; our rule 1 stands for ours. Worth a
compare-notes conversation like the one already drafted to Ora.
