# Tibetan tools survey — integration candidates
*(Part 1: GitHub ecosystem; Part 2: the wider internet — both surveyed
2026-08-07 by background agents at Adam's request. Licenses were
verified at the source where possible; "license unclear" is stated,
never guessed.)*

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


---

# Part 2 — beyond GitHub (web services, academic projects, data)

## Tier 1 — high value

1. **84000 glossary + translation memory** (84000.co · scholar.84000.co)
   — trilingual glossary of all published sutra translations + 
   sentence-aligned Ti–En TMX ("-v4" files manually corrected to
   99–100%). **License VERIFIED: glossary, metadata, and translation
   memory are CC BY 4.0** (the translations themselves are BY-NC-ND —
   different asset). Import once, fully offline, provenance-tagged as
   reference comparanda (never HGM equivalents — rule 1). The
   best-licensed large aligned Ti–En resource in existence.
   ⚠ DISCREPANCY to resolve before ingesting: the GitHub TM repo's
   README (part 1, item 14) says CC BY-NC-ND 3.0, while 84000's
   current terms-of-use page (verified here) says CC BY 4.0 for
   glossary/TM. The terms page is likely newer, but confirm against
   the downloaded files' own license statements — or simply ask
   84000 — before any ingestion.
2. **Tibetan calendar mathematics** — Henning's kalacakra.org
   open-source calendar programs (**MIT-licensed C**, verified) +
   Janson's "Tibetan Calendar Mathematics" paper; online converters
   (THL, Lotsawa House) usable as validation oracles. Port to a C++
   allcore engine + battery → colophon-date helper (rabjung cycle +
   element-animal year → Gregorian ranges, Phugpa & Tsurphu). Pure
   arithmetic, fully deterministic, fills a real gap.
3. **rKTs — Resources for Kanjur & Tanjur Studies** (rkts.org, Vienna;
   site unreachable during survey, data dump exists on GitHub
   brunogml/rKTs, license unclear — verify) — catalogues of 100+
   Kanjurs/Tanjurs, canonical text IDs cross-referenced to BDRC and
   84000, and a **6,000+ entry Tibetan abbreviations (bskungs yig)
   database** — natural deterministic expansion feature for
   input-center texts.
4. **THL kmaps gazetteers** (places.kmaps.virginia.edu, 21,000+ places;
   subjects too) — multilingual place records (Tibetan, Wylie, THL
   phonetics, Chinese, coordinates) with a no-auth JSON API. License
   for bundling unclear → user-initiated online lookup ("look up this
   place name") fits our policy exactly.
5. **Fonts (verified free)** — bundle **Noto Serif Tibetan** (OFL) +
   **BabelStone Tibetan** (free; widest Sanskrit-stack coverage); test
   the Sanskrit-stack battery output in both. Jomolhari (OFL) has
   known CoreText quirks — Qt's bundled HarfBuzz sidesteps them
   (validates our Qt choice). Monlam Bodyig license unclear — skip.

## Tier 2 — medium

6. **Mahavyutpatti via DILA glossaries** (glossaries.dila.edu.tw) —
   9,565 Skt-Tib-Eng-Chi entries, **TEI XML downloadable**; the
   canonical companion to our Sanskrit converter → offline Skt⇄Tib
   table in Lookup/Convert. Per-glossary rights vary (Hopkins there is
   copyrighted; Mahavyutpatti itself is a 9th-century text).
7. **Lotsawa House** — 6,000+ practice-register translations, texts
   **CC BY-NC 4.0 (verified), metadata CC0**. Exactly the liturgical
   register our bsod-nams warnings care about. NC → ask them for bulk
   access/permission (they cooperate; already feed OpenPecha).
8. **SOAS TIDC POS lexicon** (Zenodo 574876) — hand-tagged Classical
   Tibetan POS lexicon; check the record's CC license; deterministic
   parser enrichment.
9. **Dharmamitra / DharmaNexus** (Berkeley/Tsadra) — strongest ML
   option in the field (MT, semantic search, intertextuality); service
   only → at most a clearly-labeled optional AI aid, like our
   Analysis pane.
10. **Bibliotheca Polyglotta** (Oslo) — uniquely fine Skt–Tib–En
    alignment; no export, license unclear → link-out only.
11. **WTS Munich** (Bavarian Academy) — most rigorous scholarly
    dictionary, open online, letters A–N, German; link-out only.

## Tier 3 — low / situational

Adarsha (no API, redundant w/ BDRC — link-out) · RY wiki / Ives Waldo /
Jim Valby (content-rich, license-poor: permission via Tsadra before
shipping ANY of it) · Nitartha/Padmakara/FPMT (nothing importable) ·
GRETIL/SARIT (Sanskrit side; only if Skt–Tib display lands) · MonlamAI
services (no API docs/terms) · external input methods (macOS Wylie
keyboard partly broken; our in-app conversion is strictly better — don't
bundle IMEs).

## Notable negative findings (opportunities, not integrations)
- **No computational Tibetan verse-meter tool exists anywhere** — our
  verse pane is already novel; keep investing there.
- **No machine-readable honorific-register source exists** except
  honorific markings inside Monlam entries (Apache-2.0 lexicon data) —
  a zhe-sa layer would be another first.
- **Sum cu pa / rtags 'jug exist only as texts, not data** — our
  Wilson rule tables are ahead of the field.

## Part-2 shortlist (value × license safety × fit)
1. 84000 glossary+TM (CC BY, verified) — import as new source wave
2. Henning calendar C code (MIT, verified) — port + battery
3. Monlam lexica (Apache-2.0) — segmentation/spellcheck/honorifics
4. rKTs abbreviations + catalog IDs — after license verification
5. THL kmaps — on-demand lookup integration
6. Mahavyutpatti TEI — Sanskrit pane table
7. Fonts: Noto Serif Tibetan + BabelStone — bundle now
