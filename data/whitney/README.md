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

## Data-quality history (honor rule 3)

- v1 (2026-08-08 morning) shipped WITHOUT classes-as-authoritative or
  PPP: the repo's *display* class-PP table has column-bleed corruption
  and form-level homonym conflation.
- v2 (same day) upgraded from the repo's machine hub
  (`src/roots.csv`, crosswalk/): PER-HOMONYM classes that pass the
  kṛ acid test (1 kṛ I|II|V|VIII incl. karoti's VIII; 2 kṛ VI),
  accented PPP forms (kṛtá/gatá/bhūtá; 475 roots), class_uncertain
  candidate lists (shown as "also?"), MW entry ids (562) and MW/Apte
  sense citations (622). The master (937 roots) stays the spine; hub
  rows enrich by whitney_no with a zero-mismatch root-form check.
- Whitney's own annotations are preserved verbatim: "(?)" doubtful
  roots, "see √X" cross-references.

## Rule 1

Reference comparanda only — nothing from this layer is ever presented
as, or promoted into, HGM equivalents.
