# The ALL Working Glossary — AI-provisional tier

Adam's proposal (2026-08-12): amass dictionary entries for terms
Geshe Michael has not defined — while NEVER letting AI touch His
translations.

THE WALL (inviolable):
- Entries exist ONLY for terms with no hgm_gloss; suppressed from
  display the moment a real HGM gloss exists.
- Generation prompts receive ONLY reference comparanda (Lokesh
  Chandra Sanskrit, Hopkins) — hgm_evidence and all corpus English
  are withheld, so the model cannot paraphrase Geshe Michael.
- Stored in its own file and tier ("ai-draft"), displayed under an
  unmistakable violet banner: "ALL WORKING GLOSSARY — AI-DRAFTED,
  PROVISIONAL · not Geshe Michael's English".
- INSUFFICIENT is a permitted answer (rule 3: flagged, not guessed).
- Path to legitimacy: entries may be filed through Propose→Approval;
  a human-approved entry becomes "reviewed" tier — still never HGM.
- Target structure (Adam, from the Illuminator's example): POS,
  numbered senses, Sanskrit, cross-references, usage examples from
  the corpus's own Tibetan. The Illuminator itself is NEVER
  paraphrased — its entries are Tony Duff's copyrighted work
  (licensing inquiry sent instead).

Generator: tools/build_ai_glossary.py (requires ANTHROPIC_API_KEY).

## Wall audit (2026-08-13) — the lane is proven clean

All 1,395 AI-drafted entries were screened against the complete
aligned corpus of Geshe Michael's published English with
tools/mt_wall_screen.py (verbatim word-run detection):

- At 8-word runs: **3 flags, all benign on inspection** — one
  metrical commonplace ("at the end of a line of verse"), one
  logical connective ("it is not the case that there is"), and
  one Tibetan proper name in transliteration (an author's
  alternative name, correctly cited). ZERO reproductions of his
  renderings.
- At 6-word runs: 26 flags, all of the same stock-phrase class.

The same screen is now REQUIRED for any future machine lane
(including any Hugging Face model, whose weights may have
memorized published Buddhist translations): no machine English is
kept until mt_wall_screen passes or a human has ruled on every
flag.
