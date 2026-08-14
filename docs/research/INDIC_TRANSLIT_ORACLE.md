# indic_transliteration as a Sanskrit cross-oracle (2026-08-14)

*Found via Adam's directed re-scour of DigitalTibetan: the site's
Sanskrit page is a stub, but its pointer into the
computational-dharma notebook names the `indic_transliteration`
Python library (the sanscript module) — a mature, multi-scheme
Sanskrit transliteration engine. Used here as an independent
diff-oracle for our canonical IAST→Devanagari chain, the same
pattern that exposed the THL monosyllable defect.*

## Method

All 63,667 pure-letter IAST terms from the canonical
`build/sanskrit_reference.tsv` (the Lokesh Chandra extract dump),
converted by `sanscript.transliterate(IAST → DEVANAGARI)`,
NFC-normalized, diffed against the canonical engine's Devanagari
column. (Marker-bearing terms — leading hyphens, ellipses, ACIP
flags — excluded: 95.2% raw agreement over the full 66,857 was
entirely marker-handling noise.)

## Result: 63,664 / 63,667 identical — **99.995%**

The three divergences, each a finding rather than a defect:

1. **`om` → ओम् (ours) vs ॐ (library)** and `oṃ` → ओं vs ॐ.
   The library renders the standalone sacred syllable as the OM
   ligature U+0950; the canonical engine letter-spells it. Both
   are legitimate conventions — but for MANTRA display, ॐ is the
   authentic sign. **Filed to the data project as a
   display-convention question** (canonical engine's call, per
   the scope rules; note the parallel with our
   Sanskrit-in-Tibetan-glyphs census, where mantra authenticity
   drove the rules).
2. **`tisḥ` → तिसः (ours) vs तिस्ः (library)**: bare consonant +
   visarga with no vowel — the library inserts the virama, which
   is the stricter reading; the input itself is a degenerate LC
   entry. **Filed as an edge-case candidate** for the canonical
   engine's tokenizer (with the entry itself likely the real
   defect).

## Standing use

`pip install indic_transliteration` (Apache-2.0) in any harness
venv; rerun the diff whenever `sanskrit_convert.py` or its C++
port changes. The library speaks many schemes (IAST,
Harvard-Kyoto, ITRANS, SLP1, Velthuis, Devanagari…) — also a
candidate oracle for any future additional input notations. The
scope fence applies as everywhere: oracles inform findings; only
the data project changes the canonical engine.
