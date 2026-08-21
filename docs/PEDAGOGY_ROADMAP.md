# Pedagogy Roadmap — training translators with this app
*(banked 2026-08-06, from Adam's question: what tools and techniques could
help train new, budding, intermediate, and advanced translators — and aid
translation itself?)*

## The core pedagogical asset

The 35,601-segment aligned corpus is not just reference data — it is **35,601
worked examples by the master translator**, machine-alignable at the clause
and chunk level with tools that already exist (lattice, clause splitter,
chunker, verb classes). Nearly every exercise below is *generated from the
corpus with HGM's own English as the answer key* — no invented answers, no AI
grading. The app's honesty rules become pedagogy: tier labels teach source
criticism from day one; "unverified" verb guesses teach healthy doubt.

A second structural asset: **difficulty is computable.** For any passage we
can score (a) vocabulary coverage against what this learner has already met,
(b) clause count and nesting depth, (c) span density, (d) provisional-tier
density, (e) verb-class coverage. That enables leveling, ramps, and "is this
text readable for *you* yet?"

## Tools that aid translation itself (the working translator)

1. **Draft workspace** (side-by-side, extends the oversight pane idea):
   translator writes English per clause; the app pins each clause's HGM
   anchors, register warnings, and provisional flags alongside. Nothing
   composes English for them — the engine *anchors* them.
2. **Terminology consistency checker**: across a document, the same Tibetan
   term rendered two different ways gets flagged (with HGM's tiered
   equivalents shown). The reverse too: one English word used for two
   different Tibetan terms. Deterministic, corpus-anchored.
3. **Concordance-first lookup**: for any term, every corpus occurrence
   grouped by source/register — "how did HGM render this in prayers vs
   philosophy" (the bsod nams problem, made visible instead of auto-resolved).
4. **Personal translation memory**: the translator's own approved segments
   accumulate as a personal corpus searched alongside HGM's (clearly
   layered: HGM binding > personal > reference).
5. **Draft QC** (extends core/qc): chunk-coverage check (every Tibetan chunk
   accounted for?), untranslated-term detector, register-mixing warnings,
   enumeration check (says "three" — are three items rendered?).
6. **Sa bcad (outline) extractor**: Tibetan commentaries carry explicit
   topical outlines ("dang po la gsum ste …"). Detect the markers, build the
   tree, let the translator navigate and translate structure-first. Big win,
   fully deterministic.
7. **Verse tools**: syllable-per-line meter counter (Tibetan verse is
   syllabic: 7/9/11), line-alignment view, verse-vs-prose register flag.
8. **Back-translation check** (AI, clearly labeled): AI back-renders the
   draft's structure for omission/addition diffing against the source. Never
   an authority — a reviewer's flashlight.

## Training tools by level

### New (script and sound)
- Script↔Wylie↔ACIP recognition drills via the battery-proven converters.
- Pronunciation practice from the phonetics engine (cards already banked).
- Particle flashcards straight from the particle tables (function + paradigm
  + suffix-agreement rule — the app can generate every legal/illegal pair).

### Budding (vocabulary into structure)
- **Personal SRS deck that builds itself**: every word the learner clicks in
  the Overlay/Trainer enters their deck, frequency-weighted by the corpus;
  spaced repetition on tier-1 glosses. No deck curation needed.
- **Particle-function drills from real text**: the app blanks a particle in a
  corpus clause; learner picks which variant fits (agreement tables know the
  answer) and what role it marks (chunker knows).
- **Parallel reading mode**: corpus texts served segment-by-segment, Tibetan
  first, English on demand; peeked segments resurface later (SRS over
  segments, not just words).

### Intermediate (the chunking/order hurdle — the Trainer's home ground)
- **Chunk-ordering game**: the app shows a clause's chunks scrambled (or
  HGM's English chunks scrambled); the learner numbers them; compare against
  the reading-order planner AND HGM's actual sentence. Directly trains the
  "which order do I read" skill.
- **Cloze from the corpus**: HGM's English with one chunk blanked; learner
  supplies it from the Tibetan; reveal the master's choice. Also the reverse:
  Tibetan chunk blanked, find it from the English.
- **Miss taxonomy**: when the learner's order/reading differs, the engine can
  usually say *why* (particle misread → agreement tables; verb class missed →
  verbclass; register — flagged). Feedback names the skill, not just "wrong".
- **Difficulty-ramped daily sets**: N clauses/day auto-picked at the
  learner's level (computable difficulty), deliberate-practice style.

### Advanced (style, register, judgment)
- **Translate-then-compare workbench**: learner translates an unseen corpus
  segment cold, then sees HGM's rendering with mechanical diffs (coverage,
  terminology, order). The app never grades taste — it shows the master's
  choice next to yours and lets the gap teach.
- **Register studies**: one term, all 60+ sources, renderings side by side —
  study *why* the same word shifts across genres.
- **Reverse composition drills**: English→Tibetan attempts validated by the
  deterministic layers the app already has (spellcheck, suffix agreement,
  verb-class syntax) — the diagnostic stack becomes a composition tutor.
- **Sight-translation timer** with progressive reveal as the safety net;
  longitudinal stats.
- **Worked-examples library**: every Analysis-pane report (18-section,
  QC-checked) auto-saves; curate the good ones into a browsable casebook.

## Cross-cutting techniques
- **One progress model, all local**: clicks, peeks, misses → a per-learner
  profile driving SRS, difficulty ramps, and "known-vocabulary coverage" of
  any pasted text. Offline, private, no accounts.
- **Answers only from HGM**: every exercise's answer key is corpus or
  dictionary data; engine guidance is labeled guidance; AI is labeled AI and
  optional everywhere.
- **The 18-section analysis as scaffold**: beginners consume analyses,
  intermediates fill in blanked sections, advanced learners write them and
  machine-QC their own work.

## Suggested build order (leverage-first)
1. **Corpus drill generator** — cloze + chunk-ordering + parallel reading.
   Cheapest to build (all machinery exists), highest pedagogical value,
   deterministic answers.
2. **Progress/SRS layer** (local SQLite; feeds every other tool).
3. **Draft workspace + terminology consistency + concordance view** (the
   working-translator trio; also serves ACI teachers preparing materials).
4. **Sa bcad outline extractor** (deterministic, delightful, aids reading AND
   translating).
5. **Verse meter tools** (SHIPPED 2026-08-14); 6. **miss taxonomy** (SHIPPED 2026-08-14 — 'My weak spots…' report in Drills: per-skill filing across all four drill modes, plain-language families, per-family training prescription); 7. **back-translation QC** (credit-gated)
   (AI-labeled) — later, after the deterministic wins.
