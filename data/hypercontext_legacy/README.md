# Hypercontext-era fixtures (Adam's own development work, banked 2026-08-07)

From `~/ACIP Software Developmeent/hypercontext-master - MY DEVELOPMENT
WORK` (Adam's Hypercontext development, ~2015):

- `syllable-table-with-unicode.txt` — 1,973 ACIP↔Unicode syllable pairs
  (header line `syllables:<n>`, then `ACIP<TAB>unicode`). Used as an
  independent fixture corpus: the font battery draws its codepoint set
  from it, and it doubles as a converter cross-check.
- `syllablesThatFailToRender_testFile_HyperContext.txt` — the syllables
  that failed to render in the original Hypercontext app; the historical
  hard cases every vetted font must now cover.

The vetted-font list itself lives in `data/fonts/FONTS.md`.
