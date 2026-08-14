# Rendering Sanskrit in Tibetan glyphs — research + verdict
*2026-08-13 · Adam's directive: "make sure that our ACIP input will
render the tibetanized sanskrit properly when it is rendered in
Tibetan glyphs."*

## Executive verdict
The **font/shaping layer is already sound** — proven empirically,
not assumed: a new permanent guard (`--sanskritcheck`) shapes 23
hard Tibetanized-Sanskrit stacks through the app's real font chain
and found **zero missing glyphs** (bundled Noto Serif Tibetan;
the Palatino app font falls back to it correctly). This includes
བཛྲ, པདྨ, དྡྷ, ཛྙཱ, ཀྵ, ཙྪ, སརྦ, སྭཱ, རྭ, སཏྟྭ, ཨོཾ, ཧཱུཾ, ཨཱཿ, ཎ, ཌ, ཥཊ, དྷྱཱ, མངྒ,
and the three-deep རྐྵྱ.

The **real gaps are in conversion**, precisely diagnosed below —
all in the canonical Python engines, so the fixes belong to the
data project (port-and-battery rules apply; nothing patched
app-side).

## How Sanskrit-in-Tibetan must be encoded (Unicode ch. 13)
Order within a stack: base consonant (U+0F40–0F6A) → subjoined
consonants (U+0F90–0FBC, any depth — Sanskrit conjuncts beyond
native Tibetan rules are explicitly supported) → a-chung vowel
lengthener U+0F71 → vowel signs → ཾ U+0F7E (anusvara) / ཿ U+0F7F
(visarga). Our engine's output already follows this order (the
sanskritcheck strings are its own products).

## What the real ACIP path already does RIGHT
Actual library keying through the canonical chain
(`acip_to_ewts` → `wylie_to_unicode`):
- BADZRA → བཛྲ · PADMA → པདྨ · SIDDHI → སིདྡྷི · DZNY'A → ཛྙཱ ·
  SARBA → སརྦ · TZTSE → ཙྪེ · TADYATH'A → ཏདྱཐཱ — authentic stacks,
  correctly composed, correctly shaped.

## The three conversion gaps (canonical-engine findings)
1. **ACIP V (wa-zur clusters) is unmapped.** SV'AH'A → ⟨svAhA⟩,
   SATTVA → ⟨sattva⟩, SARVA → ⟨rva⟩ — flagged, never guessed
   (honesty held), but these are everyday mantra forms. EWTS has
   no "v"; the subjoined wa (wasur) is "w". Fix: in
   `hgm_tools.acip_to_ewts`, cluster-internal V → w
   (SV'A → swA → སྭཱ; SATTVA → sat+twa → སཏྟྭ; SARVA → sarwa →
   སརྭ). The target glyphs are proven present in the font.
   Note: `sanskrit_convert`'s own table maps IAST v → ACIP W, yet
   its cluster output emits V (SV'A H'A) — the two engines
   disagree with each other; align while fixing.
2. **A'A (long ā on the vowel-bearer) does not merge.**
   A'ARYA → ཨའརྱ (standalone འ letter) where the authentic form is
   ཨཱརྱ (a-chung U+0F71 under ཨ); A'A: flags outright. Fix:
   A'A → EWTS "A".
3. **Anusvara keying note (data-side, not engine):** OM / H'UM in
   the files render ཨོམ/ཧཱུམ (final letter ma). The standard's
   anusvara is lowercase m → ཨོཾ/ཧཱུཾ. Where the carved original has
   ཾ, the keying should use lowercase m; a corpus sweep for
   uppercase-M mantra syllables would find candidates for review.

## Font landscape (the internet sweep)
- **Noto Serif Tibetan** (bundled, OFL): verified sufficient for
  everything our chain emits — see the guard.
- **BabelStone Tibetan** (OFL): the deepest stack coverage in the
  free world — hundreds of extra conjuncts (shorthand
  contractions, Tangut-transcription stacks), tall line height
  sized for the extreme religious-text stacks. The right
  *fallback bundle* if we ever emit a stack Noto lacks; the guard
  will tell us the day that happens.
- Tibetan Machine Uni: broad but aging; Jomolhari: the ancestor of
  BabelStone (Chris Fynn). No shaping-engine work is needed —
  HarfBuzz composes Tibetan stacks from subjoined codepoints, and
  Qt's shaping handled all 23 cases.

## The permanent guard
`ALLTranslationTool --sanskritcheck` — shapes the battery through
the app's actual fonts and exits non-zero on any missing glyph.
Run it after any font bundle change and after each engine re-port.

Sources: Unicode core spec ch. 13 (Tibetan), unicode.org;
BabelStone Tibetan font page, babelstone.co.uk; Digital Tibetan
font survey, digitaltibetan.github.io; THL Extended Wylie scheme,
texts.mandala.library.virginia.edu.
