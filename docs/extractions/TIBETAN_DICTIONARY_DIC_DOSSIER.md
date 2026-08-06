# TibetanDictionary.dic — extraction & identification dossier
### Roadmap item 3 · extracted 2026-08-06 · LICENSING DECISION PENDING (Adam)

## What was extracted
- Source: `ACIPHypercontext Tool/…/en.lproj/TibetanDictionary.dic` (+ `.ndx`),
  4.3 MB, file-dated 2014-03-11, Ven. Phil Baker's ACIP Hypercontext Tool.
- Output: `data/extracted/tibetan_dictionary_dic.jsonl` — **59,043 entries**
  (wylie headword + tagged prose definition + per-entry source citations +
  provenance stamp). Extractor: `tools/extract_hypercontext_dic.py`.
- Binary format: int32le count; per record int16le lengths (which count UTF-16
  code units, not bytes — NSString artifact) + ASCII wylie + UTF-8 definition.
  Full 59,045-record walk parses cleanly (2 empty leading records dropped).

## What it is (evidence-based identification)
The roadmap's guess was "appears Illuminator-style (Tony Duff)". **The evidence
says otherwise — this is Ven. Phil Baker's personal multi-source compilation:**

1. **Cited layer (3,373 entries):** explicit `<sc XYZ>` citations naming 17+
   scholars, mapped in the tool's own `DisplayStringFormatter.m`:
   Geshe Lhundrup Sopa (817) · Joe B. Wilson (755) · Jäschke (581) ·
   Beyer (522) · Jeffrey Hopkins (383) · Roger Jackson (294) · Jampa Gendun ·
   Anne Klein · Sarat Chandra Das · **Geshe Michael Roach (36)** ·
   Dorje Wanchuk Kharto · Erik Hein Schmidt · Lopez · Thurman · Baker himself ·
   Mahavyutpatti. (A few codes — GTL 125, GM 23, DLS, LR, TR… — are not in the
   tool's mapping; likely additional informal sources.)
2. **Uncited base layer (~55,600 entries):** in a 237-entry random sample,
   **43% of definitions appear VERBATIM (60-char prefix match) in the THL
   merged bundle** (`thl.def`), which contains the Rangjung Yeshe, Valby,
   Ives Waldo, and Dan Martin dictionaries. Signature Rangjung Yeshe diction
   ("dharmakaya of empty cognizance") matches word-for-word. Control: only 3%
   of the *cited* entries match — so the base layer is substantially copied
   from published dictionary compilations, Rangjung Yeshe prominent among
   them, without per-entry attribution.
3. **No Illuminator markers found** — the entry style, tag language, and
   source roster do not point to Tony Duff's dictionary.

## Licensing assessment (for Adam's decision)
- The .dic aggregates **copyrighted dictionaries** (Rangjung Yeshe et al.)
  without systematic attribution. Ven. Phil could not license onward what he
  did not own.
- **In-house lookup use** (a private reference layer on Adam's own machine,
  same status as the THL bundle already sitting in ~/Tibetan Translation
  Tool) is the customary gray-zone use these files have always had.
- **Redistribution — including inside HGM release packages — is not
  defensible** without permissions from the underlying rights holders.

## Recommended handling (pending Adam's call)
1. Keep `tibetan_dictionary_dic.jsonl` as a LOCAL-ONLY reference layer.
2. If surfaced in the app's Lookup pane, tag every entry `{TD}` with the
   provenance string (already embedded per entry) and display "reference
   only — unlicensed compilation" in the UI.
3. NEVER merge into `hgm_dictionary` releases (founding rule 1 territory:
   reference comparanda only, and this one can't even ship).
4. Same decision applies, with the same logic, to the THL `thl.wrd/thl.def`
   extraction (roadmap item 4) — per-dictionary licensing review before any
   redistribution.

**DECISION NEEDED:** in-app local reference layer (option 2) vs. leave
extracted-but-unwired (option 1). Nothing has been merged anywhere.
