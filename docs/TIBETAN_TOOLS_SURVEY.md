# Tibetan tools survey — integration candidates
*(Part 1: GitHub ecosystem, surveyed 2026-08-07 by background agent at
Adam's request. Part 2 — the wider internet — appended when its survey
returns. Licenses were verified via GitHub API/README where possible;
"license unclear" is stated, never guessed.)*

## Tier 1 — high value

1. **Botok** (OpenPecha/Botok · Python · Apache-2.0 · active 2026) —
   Tibetan word tokenizer: trie + maximal-match over an adjustable
   lexicon. **Deterministic, not ML** — fits our rules. Integration: C++
   port into allcore under the port-and-prove discipline (its test suite
   becomes the battery; diff-test vs Python), fed by our own lexicon.
   → the segmentation backbone the Overlay currently approximates.
2. **Monlam lexica** (MonlamIT/Tibetan-Lexicon · data · Apache-2.0 per
   README · 2024) — word lists: ~107k (Monlam) + ~367k (Grand). The
   segmenter lexicon + a word-level spellcheck layer above syllable
   legality. NOTE: only the word *lists* are open — Monlam definitions
   are NOT. Avoid the old iamironrabbit dump (license unclear).
3. **ewts-converter / pyewts / jsewts** (buda-base, OpenPecha ·
   Apache-2.0 / MIT) — the ecosystem-standard EWTS⇄Unicode converters.
   Use as an **independent diff-oracle** against our proven engines
   (run the 26,318 ground-truth pairs + whole dictionary through
   pyewts, classify divergences). Not a replacement.
4. **Tibetan collation** — CLDR shipped official Tibetan collation
   rules Oct 2021 (BDRC blog); test ICU from Qt first. Fallbacks:
   buda-base/tibetan-sort-js (verify MIT), tibetan-nlp/
   sorting-and-conversion (Apache-2.0 Java). Needed anywhere we sort
   headwords — codepoint order is wrong for Tibetan.
5. **eKangyur/eTengyur corpora** (Esukhia/derge-kangyur → OpenPecha/
   P000001; derge-tengyur; Esukhia/Corpora CC BY 4.0) — diff-proofread
   Derge canon, README states Public Domain. Import as a citable
   reference corpus beside the HGM corpus.
6. **antx + diff-match-patch** (Esukhia/antx Apache-2.0; upstream
   google/diff-match-patch Apache-2.0 **with an existing C++/Qt
   port**) — the double-keying comparison workflow. Link dmp directly;
   port antx's annotation transfer for the input-center proofreading
   pane (roadmap item A).
7. **py-tiblegenc** (buda-base · Apache-2.0 · pushed 2026-05) —
   pre-Unicode Tibetan font (Sambhota/TibetanMachine) → Unicode
   rescue, incl. PDFs. External tool first; the font maps are pure
   data, portable later. High value for input-center documents.

## Tier 2 — medium

8. **Steinert tibetan-dictionary** (code GPL — do not port; data
   per-dictionary copyright, `_input/dictionaries/public/` subset
   redistributable) — big Lookup-pane win IF per-dictionary review
   clears. Same posture as THL/TibetanDictionary.dic.
9. **Verb datasets** — tibetan-nlp/tibetan-verbs-database (**CC0** —
   import immediately; 4-stem verb tables → Wilson parser);
   lexicon-of-tibetan-verb-stems (Hill — no license file, check the
   Zenodo record 4726991 terms); Esukhia/bo-pos (per-file provenance).
10. **lucene-bo** (buda-base · Apache-2.0 · 2025) — BDRC's production
    Tibetan analyzer: affix stripping ('i/ba/bo…), EWTS folding.
    Port the normalization rules so FTS5 matches inflected forms;
    tibetan-stemming-data repo = ready battery.
11. **OCR landscape** — BDRC app stays the right horse (code MIT;
    model license confirmed UNSTATED in repo — the ask-BDRC plan
    stands). Transkribus TibSchol models are server-only (link-out at
    most). Tesseract bod = poor. namsel-ocr = unmaintained, license
    unclear, skip.
12. **bophono** (Esukhia · MIT · 2025) — Tournadre/KVP phonetics
    schemes as an optional "other conventions" toggle beside the
    binding GMR engine. Port only if users ask.
13. **OpenPecha toolkit/Data** (toolkit Apache-2.0; 9,500+ etext repos,
    licenses vary per repo) — long-tail etext source; OPF is plain
    text + YAML, parseable without their code. Check license per pecha.
14. **84000 translation memory** — phrase-aligned Ti–En TMX,
    **CC BY-NC-ND 3.0**: do NOT ingest without permission/legal
    review (NC+ND vs our distribution). Safe: link-out from the
    concordance.

## Tier 3 — low / situational

MonlamAI TTS/STT/MT (API-only, model terms unclear); facebook
mms-tts-bod (CC BY-NC, only offline Tibetan TTS found); tibetan-nlp
POS corpora (no license files); Esukhia/dakje (MIT botok-based writing
assistant — UX inspiration); Tibetan calendar libs (lscalendars/
tibcalendar is plain C — verify license if a date feature lands);
OpenPecha/tibetan-fonts (per-font); buda-base/tibetan-etext-tools
(Apache-2.0 ingest grab-bag); tibetan-nlp/awesome-tibetan-nlp (index
for periodic re-survey).

## License red flags (never ship without resolution)
84000 TM (BY-NC-ND) · Steinert data (per-author) · BDRC OCR models
(unstated) · MonlamAI models (unstated) · namsel-ocr, Hill verb-stems,
eroux/tibetan-collation, tibetan-nlp POS corpora (no license) ·
iamironrabbit Monlam dump (NOASSERTION).

## Quickest wins (agent's ranking, concurred)
1. Import CC0 verbs DB + Apache-2.0 Monlam lexica into the spine.
2. pyewts as a converter diff-oracle (validation harness, not runtime).
3. C++ Botok port for real word segmentation in the Overlay.
4. diff-match-patch C++/Qt + antx logic for the proofreading pane.
5. Test ICU/CLDR Tibetan collation from Qt.
