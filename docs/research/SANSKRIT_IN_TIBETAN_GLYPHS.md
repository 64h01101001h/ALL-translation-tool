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


---

## Addendum (same day): the programme, prior art, and the census

**Prior art — the problem has a 1,200-year literature.** The
sgra sbyor bam po gnyis pa (c. 783 CE, Trisong Detsen's decree) is
the imperial standard for rendering Sanskrit — sutra, vinaya,
abhidharma, **and mantra/dhāraṇī** — in Tibetan, alongside the
Mahāvyutpatti (which we already ship as a lookup layer). Modern
algorithmic treatments: THL's EWTS Sanskrit section (the
transliteration standard our engines target) and Andrew West's
attested-stack inventories behind BabelStone Tibetan (non-standard
stack registry, Unicode-list discussions of Sanskrit conjuncts).
Verdict: we are NOT first — the algorithm exists in layers
(imperial rules → EWTS → Unicode ch.13 stacking); our job is
faithful implementation plus an attested exceptions list.

**The running list Adam asked for now exists — empirically.**
`tools/harvest_sanskrit_clusters.py` swept all 8,986 library files:
**82,834 distinct Sanskrit-flavored syllables, 9.66M occurrences**;
each converted through the canonical chain and classified
(data/extracted/sanskrit_cluster_census.tsv). Flag classes by
occurrence:

| class | distinct | occurrences | verdict |
|---|---|---|---|
| V-wasur-unmapped | 7,019 | 177,330 | THE fix: ACIP V = wa-zur; breaks everyday Tibetan (GRVA 19k, ZHVA, RTZVA, DVAGS…) AND all mantra sva/tva/rva |
| visarga ':' unmapped | 8,888 | 56,023 | ACIP ':' → EWTS H (ཿ) |
| '%' candrabindu etc. | (in other) | ~1,250+ | H'U% class → U+0F83 ྃ |
| A'A long vowel | 20 | 28 | merge to ཨཱ |
| x-mark | 687 | 20,098 | ACIP unclear-mark convention — pass-through policy needed |
| noise (RTF/English) | ~15k | ~110k | some library files are RTF with markup; English notes inside ACIP files — census filter classes, not engine bugs |

**Priority order for the canonical-engine fixes (data project):**
1. V → w (wasur) — 177k occurrences, Tibetan and Sanskrit alike;
2. ':' → H (visarga) — 56k;
3. '%' → candrabindu ྃ / '#' per the ACIP Sanskrit code chart;
4. A'A → A (long ā).
All fixes go into hgm_tools/ewts engines with battery additions,
then re-port to allcore; the census rerun + `--sanskritcheck` are
the acceptance gates.
