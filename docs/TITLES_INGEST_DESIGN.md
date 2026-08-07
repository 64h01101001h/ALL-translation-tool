# Kangyur / Tengyur / Sungbum title pairs → corpus/dictionary
*(banked 2026-08-06; Adam's request: isolate every work in the three
collections that carries BOTH a Tibetan title and an English equivalent,
strip the pairs out, and bring them into the corpus/dictionary.)*

## Why this is valuable
Titles are dense, curated terminology: short noun phrases whose English
was chosen deliberately. A few thousand title pairs = a few thousand
high-quality aligned segments plus attestation contexts for exactly the
kind of terms learners look up first.

## Sources, in order of reliability
1. **The ACIP Master Catalog / Adam's spreadsheet** — the Release IV
   manual documents catalog fields including translated titles ("the
   translated titles in the abbreviated printed catalog"). A structured
   catalog beats scraping text headers. *Adam has offered to upload the
   spreadsheet/catalog — likely the fastest and cleanest path.*
2. **The collection ZIP files themselves** — once installed via the
   Library pane, a scanner can walk the texts and their catalog files for
   title lines (ACIP texts open with the Tibetan title; English titles
   live in catalog materials rather than the text files, so expect source
   1 to matter more).
3. The Varanasi Catalog download on the same page, if it carries
   translated titles.

## The provenance question (needs Adam / the data project)
Rule 1: whose English are the catalog titles? ACIP's catalog translations
were produced under the Project (GMR as Project Director), but they are
not automatically "HGM's own English" in the dictionary's binding sense.
Decision needed before dictionary-side use:
- **corpus side** — safe either way: pairs enter as segments with course
  id `TITLES` (or per-collection `TITLK/TITLT/TITLS`) and
  `source_wave: catalog-titles`, provenance stating the catalog edition;
- **dictionary side** — whether title-derived attestations may feed
  hgm_gloss candidates at glossary tier, or stay a labeled reference
  layer, is a tier ruling for Adam/the data project.

## Plan
1. Adam uploads the catalog/spreadsheet (or installs the ZIPs so we can
   inventory what's actually inside them).
2. Extractor in the data project's tools (like the volumes ingest):
   parse rows → (tibetan title, english title, catalog no.) → filter to
   rows with BOTH → normalize (ACIP→wylie via canonical converter) →
   corpus segments with full provenance; report of skips.
3. Tier ruling on dictionary-side use; then attestation recount in a
   data session.
4. Bonus once ingested: the Library pane can show each installed file's
   ENGLISH title by joining its catalog number against the pairs.
