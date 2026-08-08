# Tibetan fonts

## Preferred system face (not bundled)

**SambhotaDege** — Adam's preferred face (requested 2026-08-07). Used
automatically wherever it is installed: it heads the Overlay's typeface
combo, and the app-wide font falls back per-character to it, so inline
Tibetan in every pane (cards, trainer, drills, lookup) renders in it
while Latin text keeps the system face. It is NOT bundled — the Sambhota
fonts are not ours to redistribute; when absent, the bundled OFL faces
below take over gracefully.

## Bundled OFL faces

Both faces are bundled so complex-stack shaping is consistent on any
machine; the Overlay's Display panel offers the choice (script mode only —
ACIP/Wylie stay in the system face).

- `NotoSerifTibetan.ttf` — Noto Serif Tibetan (Google Noto project),
  SIL Open Font License 1.1 (`OFL.txt`). Variable font, all weights.
- `BabelStoneTibetanSlim.ttf` — BabelStone Tibetan Slim v10.011
  (2023-10-01), Andrew West / BabelStone. SIL Open Font License 1.1 —
  license statement verified both on babelstone.co.uk/Fonts/Tibetan.html
  and EMBEDDED in the font's own name table (retrieved 2026-08-07).
  Notable for very wide coverage of rare stacks and bskungs-yig
  contractions — pairs well with the abbreviations layer.
