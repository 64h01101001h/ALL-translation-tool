# 84000 layers — provenance & license

## Glossary layer (g84000.json)
Base: 84000's cumulative glossary (dict, definitions, synonyms,
Sanskrit), as exported in the vertical-bar CSV form mirrored in
Christian Steinert's tibetan-dictionary repository
(_input/dictionaries/public/43..46-84000*). Retrieved 2026-08-12.

Merged 2026-08-20: the official live glossary harvested from 84000's
own GraphQL endpoint (graphql.84000.co/api/graphql) — 62,430 term
instances across all 456 published works (publicationStatus 1/1.a):
definitions, English renderings, Sanskrit, and per-work Tohoku
attestation lists ("t" arrays; links use the live-verified
https://84000.co/translation/<toh> pattern). The harvest spells the
wylie apostrophe as U+2019 — folded to ASCII ' before keying, or
9,317 entries would have silently duplicated. 29,383 keys after the
merge.

## Translation Memory comparanda (tm_84000.tsv.gz)
Source: github.com/84000/data-translation-memory, cloned 2026-08-20.
Combined export of every aligned Tibetan–English segment: 319 texts
from the repo's json/ exports + 69 texts parsed from their TMX
(v1–v4 alignment methods carried per segment) = 400,745 segments
across 388 canonical texts, every one with its Tohoku number and
(where the source carries it) Degé folio ref. The app builds an FTS5
database from this file on first use (data/extracted/tm_84000.db)
and shows matching segments BESIDE the HGM corpus concordance.

## License — both layers
Creative Commons Attribution 4.0 (CC BY 4.0) — declared by 84000's
own canonical Terms of Use (github.com/84000/all-data/
Terms_of_Use.md, rev. 2023-06-14): "Translation memory … CC-BY" and
"Glossaries and other translation data … CC-BY". Re-verified live
2026-08-20. The TM repo README still carries an outdated blanket
BY-NC-ND 3.0 notice that itself defers to those Terms ("For full
details, see Terms of Use documentation"); the Terms table is the
specific, current grant. 84000's published *translations* (Reading
Room HTML/PDF/ePub) are CC BY-ND — a different asset, not ingested.
The courtesy/data-partnership email to info@84000.co remains queued
(docs/drafts/EMAIL_84000_license.md, Adam to send).

Attribution: "84000: Translating the Words of the Buddha"; shown
wherever either layer appears.

Tier: REFERENCE ONLY — never HGM material (rule 1).
