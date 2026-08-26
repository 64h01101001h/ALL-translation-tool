# data/extracted — derived working data

The files here are the *outputs* of the ingest tools in `tools/` —
concordances, indexes, and censuses derived from sources whose terms
are recorded per-file in `docs/REPO_LICENSING.md`.

**What is deliberately NOT here:** the raw THL catalogue records
(1,926 scraped HTML pages, `thl_dege_records/` + `thl_lhasa_records/`)
were removed from the tree and from git history on 2026-08-26. They
are THL's own prose and markup; the shipped notices promise "numbers
only — no THL prose", and a public repository must keep that promise
everywhere, not just inside the DMG. The numbers-only concordances
derived from them remain. The scrape cache lives off-repo at
`/Volumes/Oct2024(8TB)/ALL-ingest-caches/thl_records_2026-08-26/` for
any future re-derivation; the build tools' docstrings point there.
