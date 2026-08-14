# Espel Lingua-BO-Wylie as a phonetics cross-oracle (2026-08-14)

**Source**: Roger Espel Llima's Perl modules (Lotsawa House's own
converter; Perl Artistic/GPL dual license), banked at
`third_party/espel_lingua_bo_wylie/` with PROVENANCE. Implements
**five conventions** (thl, rigpa en/es/fr/de, padmakara pt/en,
lhasey-en) — the DigitalTibetan plan's P3.

**Method**: 3,031 distinct single-word wylie headwords from the
spine, run through `bin/pronounce.pl -sty thl` and through
`allcore::thlPhonetics`, diffed.

## Results

- Raw agreement 86.8% — but the disagreement mass was **Sanskrit
  loanwords**, where our engine *flags* ⟨…⟩ (never guesses) and
  Espel emits guesses marked "(?)". Excluding both honest-refusal
  classes: **25 real divergences**.
- **21 of 25 were OUR defect**: rule 6's final-*ba*→*wa* shift was
  applied to monosyllables, where the *b* is word-initial and must
  stay *b* (བོད = *bö*, never *wö*). The 139-example standard
  battery contained no bare monosyllabic b-word, so it never
  caught this. **Fixed** (`i > 0` guard) — battery still 139/139,
  plus two new supplemental pins (*bod* = *bö*; *lta ba* = *tawa*
  proves the multi-syllable rule survives).
- After the fix: **4 remaining divergences, all Espel-side
  oddities** (their word-list segmentation mangles *dags*→"gé",
  *dangs*→"ngé", *gand*→"né"; *shrI* cluster handling differs on
  a Sanskrit word). No action on our side.

## Standing use

Rerun after any thl_phonetics change:
`perl third_party/espel_lingua_bo_wylie/Lingua-BO-Wylie/bin/pronounce.pl -sty thl in.txt out.txt`
vs a `thl_dump` harness build (recipe in the Developer Handbook).
**Future**: the other four conventions (Rigpa/Padmakara/Lhasey/
Lotsawa House) are candidate additional display options for the
Convert pane — port-and-prove with this same oracle when wanted.
