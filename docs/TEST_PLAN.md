# Systematic feature test plan — every pane, every feature

*Started 2026-08-09 at Adam's direction. Three automation tiers:*

- **core** — proven by an allcore battery (runs in the 37-suite ctest;
  named below).
- **selftest** — proven by the app's own `--selftest` mode (suite 38,
  `app_selftest`): the real panes constructed against the real spine,
  exercised programmatically offscreen, assertions on their actual
  state. `./ALLTranslationTool --selftest` prints per-check PASS/FAIL
  and exits nonzero on any failure.
- **manual** — needs eyes or hardware (rendering quality, network,
  drag-and-drop); scripted here as a checklist step for a human pass.

Status marks: ✅ automated and green · 🔜 automation queued · 👁 manual.

## 1. Overlay

| Feature | How to test | Tier | Status |
|---|---|---|---|
| ACIP document loads, tokens/spans/entries counted | selftest: load ACIP sample, assert tokens>0, spans>0, entries>0 | selftest | ✅ |
| Wylie document detected and loads identically | selftest: same text as lowercase wylie, assert detection + same token count | selftest | ✅ |
| Tibetan-script display converts without garbling | selftest: script mode, assert no ⟨…⟩ failure marker for clean input | selftest | ✅ |
| Phrase shading: hues cycle, singles neutral, no gaps | capture + eyes (already proven in gallery) | manual | 👁 |
| Click word → card with tier label, concordance, references | selftest: entryHtml for known headword contains gloss + tier | selftest | ✅ |
| Spellcheck red-wave on illegal syllable | selftest: doc with one bad syllable → spellFlags==1 | selftest | ✅ |
| Particle-agreement soft flag | core (`wilson_smoke`) + selftest count on crafted input | core | ✅ |
| Honorific badge, colloquial "also heard" | selftest: entryHtml for honorific term contains badge | 🔜 | 🔜 |
| Per-text glossary add/edit | manual (dialog interaction) | manual | 👁 |
| BDRC scan follow-along + word-locate | manual (network + eyes; task #31) | manual | 👁 |
| Print-Tibetan export | core (`tibexport` in engines flow) | core | ✅ |
| Mixed Nuts formatter | manual against the demonstrated workflow | manual | 👁 |
| Segmenter layers (Botok, Monlam attestation) | core (`botok_smoke`, `segtrie_smoke`) | core | ✅ |

## 2. Analysis

| Feature | How to test | Tier | Status |
|---|---|---|---|
| Deterministic extraction fills template | core (`analysis_smoke`) | core | ✅ |
| AI report + machine re-check | manual (network, cost) | manual | 👁 |

## 3. Trainer

| Feature | How to test | Tier | Status |
|---|---|---|---|
| Passage loads, clauses split | selftest: load sample, clauses>0 | selftest | ✅ |
| Layer reveal: boundaries/particles/order/vocab/answers | selftest: all layers on adds guidance incl. verb-first | selftest | ✅ |
| Wilson parse designations + dots | core (`wilson_smoke`, `wilsondots_smoke`) | core | ✅ |

## 4. Drills

| Feature | How to test | Tier | Status |
|---|---|---|---|
| Each drill type generates from real corpus segment | selftest: order/cloze/particle drills generate; cloze offers options | selftest | ✅ |
| Progress tracking persists | core (`progress_smoke`) | core | ✅ |

## 5. Draft

| Feature | How to test | Tier | Status |
|---|---|---|---|
| Source loads, clauses listed | selftest: demo() → clauses>0 | selftest | ✅ |
| Clause anchors: script line, hue bands, gloss chips share hue | selftest: anchors_ HTML contains hue span + matching chip | selftest | ✅ |
| Term concordance on click | selftest: showConcordance HTML has corpus hits | selftest | ✅ |
| Phrase memory (corpus + PENDING harvest) | selftest: known phrase → corpus section non-empty | selftest | ✅ |
| Outline (sa bcad), bam po units | core (`outline_smoke`, `bampo_smoke`) | core | ✅ |
| Verse meter + verse reading order | core (`verse_smoke`) | core | ✅ |
| Quotation detection | core (`quotation_smoke`) | core | ✅ |
| Terminology check (draft vs HGM equivalents) | core (`terminology_smoke`) + Review selftest | core | ✅ |
| RTF export, bibliography compose, footnote proposal | manual (file dialogs) | manual | 👁 |

## 6. Review

| Feature | How to test | Tier | Status |
|---|---|---|---|
| Report renders; register-sensitive flag fires | selftest: bsod nams source + "merit" draft → register warning present | selftest | ✅ |
| Provisional-tier caution | selftest: crafted provisional match flagged | selftest | ✅ |
| Unmatched-term listing | selftest: term absent from draft appears in report | selftest | ✅ |

## 7. Align

| Feature | How to test | Tier | Status |
|---|---|---|---|
| Text pair loads; token positions | selftest: load pair, tokens>0, wylie detected | selftest | ✅ |
| Link creation writes TSV row (proper wylie) | selftest: harvest wylie stays lowercase for wylie docs (row write manual) | selftest | ✅ |
| .hyp import | core (`hypfile_smoke`) | core | ✅ |
| Candidate export (PENDING labeling) | manual (file dialog) | manual | 👁 |

## 8. Input

| Feature | How to test | Tier | Status |
|---|---|---|---|
| Scan opens; follow (proportional + bands) | manual (eyes on real scan) | manual | 👁 |
| Spellcheck + double-keying compare (dmp) | core (`dmp_smoke`) + selftest compare on crafted pair | 🔜 | 🔜 |
| OCR pre-fill honesty (banner, review-material) | selftest (OCR build only): prefill refuses non-empty editor | 🔜 | 🔜 |
| Block workflow (folio nav, combined export) | manual | manual | 👁 |

## 9. Library

| Feature | How to test | Tier | Status |
|---|---|---|---|
| Catalog decode of filenames | core (`acipfile_smoke`) | core | ✅ |
| List + open into Overlay | selftest: catalog populated (>1,000 rows); open-click manual | selftest | ✅ |

## 10. Search

| Feature | How to test | Tier | Status |
|---|---|---|---|
| Gofer grammar (OR, NEAR/n) over corpus | core (`gofer_smoke`) | core | ✅ |
| Library file search + saved searches | selftest: known query hits known file | 🔜 | 🔜 |

## 11. Convert

| Feature | How to test | Tier | Status |
|---|---|---|---|
| ACIP⇄EWTS⇄unicode, Sanskrit 7 notations, Devanagari | core (`engines_battery` A–J: the heaviest coverage in the app) | core | ✅ |
| Whitney annotation of Sanskrit | core (`whitney_smoke`) | core | ✅ |

## 12. Lookup

| Feature | How to test | Tier | Status |
|---|---|---|---|
| Stacked search: HGM first, references labeled | selftest: known term → HGM section precedes reference sections | 🔜 | 🔜 |
| Honorific badge + colloquial variants | core (`colloquial_smoke`) + selftest presence | core | ✅ |
| Reverse (English→Tibetan) lookup | core (spine reverse index in `spine_smoke`) | core | ✅ |

## 13. Scan (OCR builds)

| Feature | How to test | Tier | Status |
|---|---|---|---|
| Line detection, CTC recognition, word coords | core (`ocr_smoke`, `ocr_ctc_smoke`, oracle diffs) | core | ✅ |
| Page pipeline + review-material banner | manual on a real scan | manual | 👁 |

## 14. Propose

| Feature | How to test | Tier | Status |
|---|---|---|---|
| Store round-trip, escaping, ids | core (`proposals_smoke`) | core | ✅ |
| Identity setup, form files a proposal | core (`proposals_smoke` store ops); form dialog manual | core | ✅ |
| My-proposals view shows own items + rulings | selftest: temp store own item + PENDING status listed | selftest | ✅ |

## 15. Approval

| Feature | How to test | Tier | Status |
|---|---|---|---|
| Queue lists pending with evidence | selftest: seeded temp store → HTML lists item | selftest | ✅ |
| Ruling mutations (upgrade in place / remove / untouchable) | core (`proposals_smoke` ruling section) | core | ✅ |
| Export approved candidates | selftest: writeApprovedExport → APPROVED header + ruled row | selftest | ✅ |
| Pending badge on tab | selftest: tab text "Approval (N)" asserted | selftest | ✅ |

## The manual checklist (run before any release hand-off)

1. Open a Kangyur volume in Overlay; script mode; check shading reads
   calm, phrases banded, no gaps; click three words — cards complete.
2. BDRC follow-along on KD0016: pages turn with cursor; word-locate
   box lands on the word (task #31).
3. Input pane with a real scan: follow tracks typing; OCR pre-fill
   banner present; compare flags a planted difference.
4. Mixed Nuts export on a short text: format matches the demonstrated
   workflow.
5. DMG on a machine that never had the dev tools (ideally no
   Homebrew): both drag-installs, first-launch right-click→Open,
   data found, five random panes exercised.
