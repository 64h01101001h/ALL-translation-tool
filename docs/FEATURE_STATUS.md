# Feature status register — everything discussed, audited 2026-08-07

Verified against the app source and test suites (30/30 green), not from
memory. ✅ live · 🟡 partial · ⬜ not built · 🔒 externally gated.

## The apparatus (bibliography & footnotes) — ALL LIVE, Draft pane

- ✅ **Footnote reuse bank** — 344 published endnotes (3 volumes),
  search field, one-click insertion ALWAYS with citation
- ✅ **Bibliography bank** — 138 published entries (118 with ACIP nos.)
- ✅ **Compose bibliography entry…** — house-format assembly (exact
  published-B3 battery), live preview, STD-002 technical-spelling
  helper, BDRC link from the ACIP number, **auto-fill from the catalog
  for 1,846 works**, inserts labeled "pending publication"
- ✅ **Propose footnote** — pending queue, clearly PENDING, never mixed
  with the official tier; **PENDING_REVIEW.docx** generator for GMR
  review sessions
- ✅ **MAIN_FOOTNOTES.md / MAIN_BIBLIOGRAPHY.md** — regenerated per
  release, never hand-edited, official = published+approved only
- ✅ **Quotation detection** — attested corpus match ≥7 syllables,
  cross-segment merge, one-click published bib entry + published
  footnotes recommended on the matched passage
- 🔒 STD-008 in GMR's own words (Adam to obtain); rerun per release

## Overlay (reading)

✅ depth-wash nesting · click cards w/ HGM concordance · provisional
dashed amber · spellcheck red wave · particle cards + suffix-agreement
diagnostics · fused-particle finds (PA'I/PAS/PAR) · Monlam attestation
(both lists — UTF-16 bug fixed) · CC0 verb paradigms · bskungs-yig
abbreviation cards ("not a typo") · bsdus-tshig contraction cards ·
Botok segmentation layer (toggle) · unattested-word hints (toggle) ·
Tibetan script/ACIP/Wylie + full Display toggles · SambhotaDege
everywhere + 8-face vetted typeface combo · wylie documents (Release 6)
· print-Tibetan export · Mixed Nuts prep formatter · BDRC scan
follow-along w/ line band (Kangyur/Tengyur auto, Sungbum title-search,
cached, license shown) · open-file provenance decoding

## Trainer / Drills / pedagogy — ①–⑦ ALL LIVE

✅ progressive reveal (chunks/roles/order/vocab/answer key/full Wilson
parse) · **all 12 Science-of-the-Dots designations** (S OM NA NN APP VB
V ADV NOM VOC C-n UP SP — NA/ADV via SOAS, UP by attestation, OM via
the derived register) · verb classes (~70 verbs) · na disambiguation ·
readable-for-you coverage note · drills ×6 (order/cloze/particle/
parallel/SRS/translate-and-compare) · adaptive difficulty · miss
taxonomy ("weak spots") · local progress.db

## Draft (translator's bench)

✅ clause anchors · concordance-first term view · terminology checker
(mixed renderings, shared English, provisional flags) · sa bcad
extractor · verse meter · AI back-check (diff-only, labeled) · RTF
export w/ true italics (STD-004) · technical-spelling insertion
(STD-002) · link-out row (84000/BDRC/Polyglotta/Lotsawa House)

## Search / Lookup / Convert / Library / Analysis / Scan

✅ Gofer grammar (corpus + file trees + prebuilt library index) ·
pronunciation search ('jangchub') · affix-stripped lookup (lucene-bo) ·
Tibetan-order dictionary browse (ICU/CLDR) · Mahāvyutpatti layer (both
directions) · reference dictionaries · reverse lookup · full Sanskrit
converter card + IPA + letter guide · Tibetan calendar (year + full
day-level, Henning-proven) · Release 6 library (2,716 texts indexed) ·
catalog provenance/titles/subjects · OCR: full in-app pipeline (Scan
pane) + external hand-off + ocr_out review flow · Analysis pane (18
sections, QC'd, auto-saved)

## Originally not-yet-built (most since shipped — see ✅ marks inline; the honest list)

- ✅ **Per-text glossary** (Ven. Phil's ancestor feature; survey ②) —
  Overlay `gloss:` anchors + `library/glossaries/<text>.tsv`
- ✅ **Saved searches** (Gofer had them; survey ③) — Search pane,
  QSettings-persisted
- ✅ **Acipcode.txt converter-completeness battery** (survey ①) —
  `acipcodes_smoke`, 100.0% of 411 codes via proven engines
- ✅ **Font render test suite** (survey ④) — `font_smoke` ctest:
  bundled faces must fully cover the Hypercontext syllable corpus
- ✅ **UTFC legacy-font rescue lane** (survey ⑤) — `tools/setup_utfc.sh`
  builds the GPL binary (external process, never linked); Library-pane
  "Legacy font rescue (UTFC)…" converts TMW/TM/LTibetan/Sambhota-era
  documents to Unicode, UTF-8-normalized into `my_materials/`, output
  labeled utfc-derived
- ✅ **Bampo/shloka structural layer** (survey ⑥) —
  `allcore::extractStructure`: explicit BAM PO + numbered LE'U markers
  (both cases, compound/contracted ordinals incl. bco/nyer/zhe-style;
  out-of-sequence flagged, unparsed kept raw), 30-syllable shloka /
  300-shloka bampo ESTIMATES; Draft-pane "Structural units" report.
  Real-canon check: KL0009I1 yields bampos of ~10k syllables each
  (definition says 9,000) and correctly flags the source's own skipped
  `bam po bzhi pa`
- ✅ **Spotlight federated search** (wysearch idea) — Search pane
  "Search this Mac (Spotlight)": mdfind probes with the query as typed
  plus its Tibetan-unicode conversions (both proven chains)
- ✅ **Batch-volume OCR** (Than Grove pattern) — Scan pane "Batch
  folder…": whole folder → headered OCR-DERIVED per-page files with
  progress, legality counts, loud failures
- ✅ **Word-level OCR coordinates + follow-along** — pyctcdecode's own
  `text_frames` ported into the beam (proven byte-exact vs the oracle
  on every text-identical line); `LineImage::colMap` inverts
  mask_n_crop's interior-gap squeeze back to page x; Scan pane draws
  per-word boxes and clicking any recognized word highlights it on the
  page.
- ✅ **Overlay word-locate on BDRC scans** (2026-08-08) — opt-in
  "locate word (OCR)" button in the follow-along nav: runs the
  canonical pipeline on the cached BDRC folio (deskew forced 0° so
  boxes stay in the original image — labeled deviation), matches the
  cursor's token (ACIP→wylie, or lowercase wylie as-is) against
  recognized line text (expected line first), highlights the covering
  word boxes on the woodblock. OCR is a LOCATOR only — its text is
  never surfaced. Per-folio results cached in-session. Matcher math
  verified on the Pramanavarttika fixture ("khyed" → line 0,
  x 230–267/2002).
- ✅ **Style-face registration** — the five inactive files in
  ~/Library/Fonts (chos-rgyal, yig-chung, calligraphic, TCRC Youtso,
  Tib-US) registered per-session by app + font_smoke; 64/64 coverage;
  in the typeface combo
- ✅ **Letter-level contraction fusions** (2026-08-08) — "fused" class
  in the derived register: onset+vowel + next first consonant (sher ←
  shes rab, myang ← mya ngan, phar ← pha rol, rdor ← rdo rje); 9
  pairs, exact-gloss-only, corpus-attested
- ✅ **Jomolhari + Monlam Bodyig bundled** (2026-08-08) — OFL 1.0 /
  1.1 verified and banked; 4 bundled faces now. Qomolangma recorded
  DO-NOT-BUNDLE (non-commercial-only license)
- ✅ **.hyp importer** (2026-08-08) — `allcore::parseHypFile` (33rd
  suite, proven on Adam's own tutorial.hyp: all 15 numbered links,
  the `<!…>` note, `<t$>` escapes, zero unknown tags); Align pane
  "Import .hyp…" maps char spans → token spans (sub-syllable links
  widened and counted, unmappable skipped and counted — never
  guessed); links land in the normal library/links flow
- ✅ **Whitney reference layer** (Adam's request, 2026-08-08) — both
  preeminent works: the 937 roots of *Roots, Verb-Forms and Primary
  Derivatives* (1885) with homonym numbers, meanings, per-homonym
  classes, accented PPP; per-root §§ citations + topical ranges into
  the *Sanskrit Grammar* (1879) with ✦/⚠ markers; MW ids/senses +
  SLP1-keyed Cologne MW deep-links (verified live); archive.org PD
  Grammar link-out. Lookup matches by root, past participle
  (gata→gam), or English meaning; Convert pane annotates exact root
  forms. Works PD; digitization Apache-2.0 (gasyoun/WhitneyRoots),
  banked with license; 34th suite whitney_smoke
- ⬜ **Live composing editor** (Jskad idea — waits for the input-center
  formatter) · ⬜ Old-Tibetan affixnorm rules (remote session's
  thread)

## Externally gated

🔒 84000 glossary+TM (email w/ Adam) · input-center formatter + samples
(uploads) · Mixed Nuts style guide (editor) · catalog tier ruling
(Adam) · eKangyur/eTengyur (Adam's disk OK) · rKTs + DILA refresh
(hosts down) · THL kmaps + Adarsha (their sites broken) · Steinert
(license review) · Lotsawa House bulk (permission) · attestation
recount + release (data project) · BDRC written OCR confirmation +
deskew-bug report upstream
