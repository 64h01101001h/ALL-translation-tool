# Whitney reference layer (Adam's request, 2026-08-08)

William Dwight Whitney's two preeminent works, integrated as a
reference layer in the Lookup pane:

1. **The Roots, Verb-Forms and Primary Derivatives of the Sanskrit
   Language** (Leipzig, 1885) — the 937-root list with Whitney's own
   homonym numbering and meanings.
2. **Sanskrit Grammar** (1879; Harvard 1950 repr.) — per-root section
   citations (§§), with the digitization's markers preserved:
   ✦ = the root has a dedicated paragraph; ⚠ = the Grammar calls the
   root out as an exception. Link-out to the public-domain scan:
   https://archive.org/details/sanskritgrammari00whituoft

## Provenance & licensing

- The WORKS are public domain (Whitney died 1894).
- The DIGITIZATION is github.com/gasyoun/WhitneyRoots (**Apache-2.0**,
  banked as LICENSE-Apache-2.0; source files under src/). Related:
  sanskrit-lexicon/csl-whitroot serves the 1885 book's page scans;
  Cologne CDSL's own transcriptions are CC BY-NC-SA and were NOT used.

## Files

- `whitney_roots.tsv` — the banked layer (regenerate:
  `python3 tools/build_whitney.py`). Columns: id, root (IAST),
  homonym, meaning, classes(form-level), grammar_secs, dcs_classes,
  notes.
- `src/Whitney-numbered-2026.md` — the digitization's reference master
  (roots + meanings).
- `src/Whitney_Grammar_Citations.md` — the Grammar §§ table.

## Data-quality caveats (verified 2026-08-08 — honor rule 3)

- **No principal-part (PPP) forms banked**: the upstream class-PP table
  has column-bleed corruption (rows 106–108 et al.). Add PPP only when
  a clean source is verified.
- **Class column is FORM-level**: homonyms written identically share
  one row upstream (Whitney's 1 kṛ "make" vs 2 kṝ "scatter" conflate
  under "kṛ"), so per-homonym class attribution is NOT trustworthy;
  the app labels the column "as digitized (form-level)" and shows the
  DCS corpus classes beside it as comparanda.
- Whitney's own annotations are preserved verbatim: "(?)" doubtful
  roots, "see √X" cross-references.

## Rule 1

Reference comparanda only — nothing from this layer is ever presented
as, or promoted into, HGM equivalents.
