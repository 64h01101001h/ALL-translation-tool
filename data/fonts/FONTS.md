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

## Fonts survey (2026-08-07, Adam: "collect all the Tibetan fonts and
## choose the most widely used")

Every candidate family on this machine plus the bundled faces was
scored for coverage against the full Tibetan-block codepoint usage of
Adam's own 1,973-syllable ACIP↔Unicode table (from his hypercontext
development work) plus his syllables-that-fail-to-render list.

**Vetted Unicode faces (all 63/63 coverage) — the app's typeface combo,
in this order (only those present are listed):**

| face | why it matters |
|---|---|
| SambhotaDege | Adam's preferred face (system-installed; never bundled) |
| Noto Serif Tibetan | the modern cross-platform default (Google; bundled, OFL) |
| TibetanMachineUnicode | THL's long-time scholarly standard (installed) |
| BabelStone Tibetan Slim | widest rare-stack + bskungs-yig coverage (bundled, OFL) |
| TibetanClassicUnicode | installed classic face |
| Kailasa | Apple's system Tibetan (every Mac) |
| Kokonor | Apple system alternate |
| Microsoft Himalaya | the Windows default (relevant for the input-center builds) |

**Excluded, with reasons:**
- *TibetanMachineWeb 0–9* — the pre-Unicode THL fonts, custom glyph
  encodings (0/63 coverage). Documents typed in them are exactly what
  the UTFC legacy-font rescue lane converts (Uni2TMW/TMW2Uni tables).
- *Katari, Phosphate* — declare Tibetan support, cover none of it.
- *Songti SC/TC, STSong* — Chinese faces whose cmaps include Tibetan
  codepoints; not Tibetan typography.
- *HypercontextTibetan.ttf* (Adam's old app) — custom glyph encoding
  keyed by ACIP.kt, superseded by Unicode faces + our converter.
- ~~Font FILES present but not active as families~~ — ACTIVATED
  2026-08-08: the app (and font_smoke) now registers the five style-face
  files per-session from `~/Library/Fonts` when present, never bundled:
  TibetanChogyalUnicode (chos-rgyal), TibetanYigchung (cursive),
  TibetanCalligraphicUnicode, TCRC Youtso Unicode (TibetanUnicode.ttf),
  Tib-US Unicode (tibusrfa2.ttf). All five probed at full working-set
  coverage (64/64) and appended to the typeface combo + battery.

**Worth adding someday (not installed):** ~~Jomolhari~~ — BUNDLED
2026-08-08: `Jomolhari-Regular.ttf` (Chris Fynn, OFL **1.0** — license
banked as `Jomolhari-OFL.txt`; fetched from google/fonts ofl tree),
63/63 working-set coverage, registered at app start, third bundled
face in the combo + font battery. ~~Monlam~~ — BUNDLED 2026-08-08:
`MonlamBodyig-Regular.ttf` (family "Monlam-Bodyig"; Lobsang Monlam /
monlamit.com, **OFL 1.1** verified from the official MonlamIT GitHub
LICENSE — banked as `MonlamBodyig-OFL.txt`), 63/63 coverage, fourth
bundled face. **Qomolangma series (yalasoo): DO NOT BUNDLE** — probed
2026-08-08: free for NON-COMMERCIAL use only; commercial use requires
written permission from yalasoo. If Adam installs them locally the
curated combo could list them like SambhotaDege (register-if-present),
but they must never ship with the app.
