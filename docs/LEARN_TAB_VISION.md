<!-- Produced 2026-09-10 by a 117-agent research pass at Adam's direction:
     make the Learn tab the best part of the tool; think laterally; consider
     gamification; research language acquisition and what other tools do.
     Ideas proposed 108, kept to build now 20, kept for later 23, CUT 65.

     NOT YET RULED ON and nothing here is built except where marked SHIPPED.

     Two load-bearing figures were re-measured independently before this was
     banked, because several claimed numbers in this project have been wrong:
       - alignment layer covers 1,547 of 42,199 segments (3.7%) — CONFIRMED
         (C01 496, C02 68, C03 620, C04 198, C05 165)
       - 11,777 links carry no English exponent, 28.2% — CONFIRMED exactly
     The plan's own §2 flags that its frequency-coverage numbers are NOT
     reliable (two passes differed by 2.5x) and must not be printed as fact
     until a disambiguating segmenter exists. That warning stands. -->

# The Learn Tab

**A vision and build plan for the Diamond Cutter Translation Tool**
Status: proposal. Nothing here is built except where marked SHIPPED.
Written 2026-09-10. Supersedes nothing; feeds `docs/PEDAGOGY_ROADMAP.md`.

---

## 1. The thesis

Every other language-learning tool teaches a language. This one teaches a **library** — 42,199 aligned Tibetan–English segments carrying one teacher's consistent English, plus a word-level alignment layer that records, span by span at seven depths, which Tibetan words his English came from and which Tibetan words he did not render at all. That closure buys a learner three things nothing else can offer. First, **every answer key is attested**: when the tool says "this is what it means here", it is quoting Geshe Michael Roach's own published rendering of that exact line, with a citation, not a dictionary's opinion and not a machine's paraphrase. Second, **the ambiguity is visible instead of hidden**: because one hand wrote all of it, the places where he rendered the same Tibetan two different ways are not noise between translators — they are one translator's principled judgment, and they can be shown to a learner as the exercise rather than smoothed away as an inconsistency. Third, **the silences are recorded**: the alignment layer marks 11,777 spans where a Tibetan morpheme has no English exponent at all, which is the single hardest fact for a beginner to accept and the one no glossary can represent. A learner working here is not memorising a language in the abstract; they are learning to read the texts they came for, checked at every step against the translator whose English they are trying to understand.

The honest limit, stated up front: the alignment layer covers 1,548 of 42,199 segments — 3.7%, courses C01–C05, tier TENTATIVE. For 96.3% of the corpus the depth machinery does not reach, and the tool must say so rather than approximate. Several of the best ideas below are therefore scoped small on purpose, and the refusal path is the majority path.

---

## 2. What the evidence supports, and what it does not

We surveyed the learning-science literature and the tool landscape. The findings are uneven, and the plan below reflects that unevenness rather than treating every citation as equally load-bearing.

### Strong — build on these

**Retrieval practice.** Testing yourself beats re-reading, at g≈0.50–0.61 across hundreds of studies, and the effect grows with material complexity, retrieval effort, and the presence of feedback. This is the best-supported study intervention that exists. Our Drills pane already sits on the right side of it.

**Distributed practice.** Spacing beats massing (classroom d≈0.54), and the optimal gap scales with the retention horizon. Our simplified SM-2 is adequate for this.

**Form-focused instruction.** Explicit attention to grammatical form has a large measured effect (g≈1.06), with the honest caveat that the outcome measures rewarding it are partly the ones that test explicit knowledge. Our particle tables, role markers, and reading-order planner are form-focused instruction, and the evidence says deliver them — best at the moment of a felt gap rather than as a reference chapter.

**Transfer is bounded, and the bound is the design constraint.** Retrieval practice transfers at d=0.40 overall, but transfers *weakest* to rearranged stimulus–response pairs — which is precisely what a vocabulary deck is. Practice wins when its processing matches the criterion task. Our criterion task is: read Tibetan, produce defensible English. So a word-pair deck is a component of reading, not a proxy for it, and the UI must not imply otherwise.

### Moderate — build carefully, label honestly

**Interleaving** helps for rule-like material (g=0.42) but the heterogeneity is severe (I²=77%) and for *word learning specifically* it is null to negative. Directional instruction: interleave the grammar, block the vocabulary. Interleaved practice also *looks worse during the session* and wins at a delay, so in-session accuracy must not be the headline number.

**Pretesting / errorful generation.** A failed guess followed by corrective feedback beats equal-time errorless study. Learners dislike the condition that helps them, so the tool must show them their own delayed-retention evidence or they will switch it off.

**Glossing decays.** Gloss benefits fall from g≈0.46 immediate to g≈0.28 delayed. A shown gloss with no scheduled return is a gain that leaks.

**Gamification is two effects, not one.** The cognitive effect (g=0.49) is methodologically stable under a high-rigour subsplit. The motivational effect (g=0.36) and behavioural effect (g=0.25) are smaller and unstable. Meta-analytic work finds gamification lifts motivation, autonomy and relatedness with *minimal effect on competency*.

### Weak, contested, or not supported — do not lean on these

**Deliberate practice as a mastery promise.** Practice hours explain about 4% of performance variance in education. The dispute with Ericsson is unresolved. What survives is not "hours" but "specific diagnostic feedback on an identified weakness". Never show hours practised as a headline metric.

**The 98% lexical-coverage threshold.** The best-powered study (N=661, two texts, eight countries) found the coverage/comprehension relationship *linear, with no threshold*. Report coverage as a continuous percentage; never build a lock that refuses a text below a score.

**Classical-language pedagogy generally.** The Greek/Latin literature is program advocacy, not controlled trials. We cannot borrow authority there. The honest posture is: built on the best-evidenced general principles, with your own retention data shown to you.

**Our own headline frequency numbers.** Two independent passes over the same database, using two defensible tokenizations, produced coverage figures ~2.5× apart (199 vs 380 items for 50% coverage; 3,266 vs 9,101 for 95%). The lattice does not make a segmentation decision — by design, it keeps nested matches. **No frequency-coverage figure may be printed as fact until a disambiguating segmenter with a documented tie-breaking policy exists and is battery-proven.** See §6.

**FSRS over our SM-2.** The 20–30% efficiency claim is simulation over review logs, not a trial with learners. Better scheduling is real but it is not where our advantage is. Deferred indefinitely.

---

## 3. The tools

Each entry states what the learner sees, what data drives it, how the tool knows they succeeded, and how TENTATIVE material stays labelled. Nothing below composes English. Every answer key is either Geshe Michael Roach's own text, a rule table (particle agreement, role markers), or an explicit refusal.

### 3.1 For beginners — someone who cannot yet read the script

#### Zero to script — **BUILD NOW**

**Sees.** Tibetan letter and syllable recognition, then pronunciation cards, selectable by course so a C05 student drills the C05 card.

**Data.** The 26,318 *source-attested* unicode forms only — never the 79,316 generated ones. Pronunciations from the 1,308 readings attested on his own course Language Study Guides, plus `data/card_phonetics.json` (2,118 lines). The `tibetan_source` and `pronunciation_source` columns in the spine already carry the provenance that makes this filter possible.

**Success.** Recognition accuracy at a delay, per glyph and per syllable-shape.

**Honesty.** Generated forms are excluded outright rather than labelled — this is the beginner rung and a beginner cannot evaluate a tier badge. Speech scoring is explicitly out of scope: we already declined it because Whisper's Tibetan cannot support it honestly.

**Why first.** Adam asked for a tab where a beginner finds something that moves them forward. Today every mode takes ACIP or wylie as input, so a true beginner cannot start at all. Largest audience gain for the least work in the plan.

#### Boundary Hunt — **BUILD NOW**

**Sees.** A real segment with its punctuation stripped. Mark where the clauses end.

**Data.** `splitClauses` + `refineClauses` in `core/src/reader.cpp`, plus the shad and clause-particle positions actually present in the text. Two pools: punctuated segments (confidence-building) and the 11.1% with no punctuation (the real skill).

**Success.** Boundary precision and recall against the text's own punctuation, tracked separately for the two pools.

**Honesty.** `refineClauses` already makes a dictionary-anchored ruling on each ambiguous `na` and already declines where it cannot rule. Where it declines, the item is not scored — it is shown as a genuine ambiguity.

**Why.** We measured that clause order survives translation: adjacent clause pairs are reversed in his English only 4.8% of the time, against 49.0% for adjacent word pairs. Find the clause boundaries and most of the corpus becomes readable left-to-right at clause granularity. Highest payoff per hour in the language, and the generator already exists.

---

### 3.2 For intermediates — someone who reads slowly and guesses

#### Known here / known anywhere (formerly "glosses as debts") — **BUILD NOW**

**Sees.** Any word or span revealed — in the Overlay, a Trainer reveal, the concordance — enters the deck. Its first review re-presents it **inside the segment it came from**, next day or later, gloss hidden. Its second presents it in a *different* corpus segment. Two separate readouts: "you know it here" and "you know it anywhere".

**Data.** Existing deck plus a schema migration (`vocab` today is `wylie PRIMARY KEY, first_seen, last_seen, views, ease, interval_days, due` — no segment id, no depth, no stage). Transfer segments via `corpusSearch`, already proven and capped.

**Success.** The two readouts are the measure, and they are the honest instrument for the transfer question the literature says self-assessment gets wrong.

**Honesty — two mandatory gates.**
1. **Register divergence.** The transfer segment may be one where he rendered the same Tibetan differently — *bsod nams* as "goodness" in prayer and "merit" in prose. If the pane marks the learner wrong for producing the gloss they learned, it teaches that one Tibetan word has one English, which inverts the project's central lesson. Required: self-graded on comprehension, both renderings shown as his with tier labels, and where they diverge **the pane says so explicitly**. That divergence is the lesson.
2. **No second attestation.** Deep spans often recur nowhere else. Say "no second attestation in the corpus" and leave the item at "known here". Never silently re-serve the origin segment and call it transfer.

**Why.** This replaces the weakest thing in the Learn tab — a bare ACIP word with "Do you know this word? Grade yourself", which is exactly where a generic flashcard app is our equal. It also repairs a live defect: `touchWord` fires on Overlay click, concordance, and the bulk deck-fill, but **the Trainer's six reveal layers touch the deck nowhere**. Reveals leak today.

**Pilot condition.** If the "here" score shows no variance across learners, cut the scoreboard and keep only the context-in-review change, which stands on its own.

#### Train this — the weak-spot loop — **BUILD NOW**

**Sees.** The weak-spots report stops being a terminal dialog. Each named skill gets a grammar note, a **"train this"** button that generates a drill set targeting that skill, and a scheduled re-test. The learner watches a weakness close instead of a counter grow.

**Data.** The miss taxonomy already files every wrong answer under the skill it reveals (`miss:particle:*`, `miss:cloze-role:*`, `miss:order:{chunk-order, verb-position, genitive-attach, agent-chunk, ladon-chunk}`, `miss:vocab:*`). The grammar notes mostly already exist **as data**: `ParticleInfo{group, function}` and `checkAgreement` in `core/include/allcore/particles.h`, and `kRoleMarkers` in `core/src/reader.cpp` (one-line note per role). Authoring shrinks to a paragraph for the four order sub-skills. `miss:vocab:*` needs nothing — the deck already resurfaces those.

**Missing.** Targeting. `makeOrder`/`makeCloze`/`makeParticle` draw a random segment and take whatever family or role turns up; there is no `DrillTarget` in the tree. Also missing: skill-tagged *hits* (only misses carry a skill key today, so accuracy has no denominator).

**Success.** Per-skill accuracy at the scheduled re-test, on fresh items.

**Honesty — four binding conditions, each disqualifying if skipped.**
1. A targeted draw that cannot meet its target returns `nullopt` and the pane says so. Never silently hand back an off-target drill labelled as training that skill.
2. `makeCloze` deliberately blanks a *marked* chunk because its role makes the question fairly answerable. Targeting the "unmarked" and "predicate" roles forces questions the generator's own code says are unfair, manufacturing a weakness that is a generator artefact. Refuse those two targets, with the reason shown.
3. Order-miss attribution uses the first divergent position, and one transposition shifts everything after it. Fine as an aggregate signal; not a claim about the learner. Write "these misses point at", not "you misread the genitive".
4. No trend line until skill-tagged hits exist and attempts clear a floor. Below it: "4 attempts — too few to call a trend."

**Why.** Anki can schedule a card. It cannot diagnose which grammar skill a wrong chunk-order reveals, generate a fresh question exercising exactly that skill from a different real segment, and measure whether the weakness closed. Strongest "only our data" case in the tab, and the expensive half is already shipped.

#### Mixed sets — interleave the grammar, block the vocabulary — **BUILD LATER**

**Sees.** A "Mixed set" option alongside the existing modes: N items assembled by a scheduler, grammar categories interleaved, vocabulary items blocked and kept semantically apart.

**Why it is justified by a defect, not only by literature.** The mode combo box **leaks the category**. In "Particle choice" the learner already knows the answer is a particle. The one judgment a Tibetan reader actually makes at a fresh clause — is this a case question, a role question, or an order question — is pre-answered by a dropdown before any Tibetan is read. Every session today is 100% blocked practice with the category handed over.

**Scope corrections.** It covers modes 0/1/2/4 only. Parallel reading and Translate & compare are sessions of their own kind and cannot be item 4 of 12.

**Honesty.** "Semantically unrelated" must be computed as **mechanical token overlap on his own English glosses** — no embeddings, no similarity model, because a machine judgment about meaning is composition. It is an ordering *preference* with a visible note when it cannot be satisfied, never a silent drop. And per-item tier badges, not a session banner: a learner three items deep cannot tell which item is binding. Required gate: a selftest proving a TENTATIVE vocabulary item inside a mixed set still renders TENTATIVE.

**In-session accuracy is deliberately not the headline** — interleaving depresses it by design. The replacement is computable today with no schema change: the miss taxonomy stores timestamped per-skill outcomes, so the headline becomes *delayed* accuracy on categories last met N days ago.

**Why later.** Grading logic for modes 0–4 is untested end-to-end; only mode 5's tier labelling is proven by test. A session runner means driving the drill state from a queue instead of from `mode_->currentIndex()` — refactoring the least-covered grading code in a ~1,700-line pane. Land selftests on modes 0–4 first. This is the first thing to build after 1.0 ships.

---

### 3.3 For advanced readers — someone translating for real

#### What you can read next (formerly "Ascend through the texts") — **BUILD NOW**

**Sees.** A shelf of the corpus's works, each with a readiness card printing the actual numbers: your coverage of its vocabulary, its clause count, what stands between you and it — *41 unknown headwords, three verb classes not yet met*. "Next" recommends the shallowest unread work above your current position.

**Data.** `Progress::coverage()` already answers "readable for you?" and is already wired into the Trainer for a pasted passage. `Spine::corpusCourses()` and `corpusWindow()` already serve per-course segments. `DrillFactory::isDrillable` already refuses the title catalogue.

**Success.** Coverage rising against a named work, and the itemised gap shrinking.

**Honesty.** Coverage is shown as a continuous percentage with the 95% and 98% research marks drawn on it and cited as a research-derived heuristic — never folded into a proprietary level number, and never a lock. The best-powered study found no threshold; we do not build one.

**What was amputated.** The composite difficulty score and the climb framing are cut (§6). What remains is a shelf ordered by measured coverage, which is honest, and which is the part that actually helps.

#### The Silent Particle — **BUILD NOW**

**Sees.** A particle highlighted in its real segment. First: "did Geshe Michael Roach render this in English here?" Then, if yes: "with which word?" The reveal shows his actual sentence, the particle's role name, and the citation. **"I cannot tell from this alone" is a scoreable answer, marked correct wherever the data genuinely does not determine it.**

**Data.** 11,777 null-English spans in `data/alignment/alignment_full_v1.json`; the role table already in `core/src/reader.cpp`. Scoped to the 1,548 aligned segments.

**Success.** Accuracy on the "did he render it" question, which for case particles is "no" 93.9% of the time — a distribution no learner predicts correctly at first.

**Honesty.** Every item labelled TENTATIVE from the layer meta. The refusal answer is not a dodge; it is scored, and it is the mechanism by which rule 3 becomes the scoring rule itself.

**Why.** Every other tool teaches particles as if they always surface in English, which is measurably false. 1,595 of 1,698 case markers in the layer are unrendered.

#### His second thought — attested variation — **BUILD NOW**

**Sees.** One Tibetan term, several of his own competing renderings as the options. Which one did he use *here*? The reveal shows all of them with citations, ordered by frequency, and does not resolve the ambiguity.

**Data.** `data/alignment/alignment_evidence_v1.json` already stores multiple `{eng, refs, n}` entries per key with citations — 2,244 multi-rendering headwords across C01–C05. Difficulty is the rendering count, so `sogs` (84) is an expert item and a two-rendering word is a beginner item.

**Success.** Accuracy against his attested choice for that citation.

**Honesty.** Every distractor is something he actually wrote, so nothing is composed and every option is legitimate. TENTATIVE label on all of it. Vetting recommended on the extreme tail so options are sampled rather than dumped.

**Why.** This is the *bsod nams* problem turned from a warning into an exercise, and it is unbuildable from a merged dictionary — it needs one translator's complete aligned output.

#### The Debate Dojo — **BUILD LATER**

**Sees.** Split a real formal statement into subject / consequence / reason. Then choose the legal reply from the closed set (`MA GRUB NA`, `MA KHYAB NA`, `RTAGS MA GRUB`, `RTZA BAR 'DOD NA`) and name which element it attacks. Then follow a real exchange move by move with the English hidden.

**Data.** 1,052 segments carry the full `CHOS CAN` / `THAL` / `PHYIR` template. Needs a template parser and a move-type annotation pass over C15.

**Success.** Move-type accuracy on unseen exchanges.

**Honesty.** Word-level drills are **refused inside this mode**: his English for these segments is an expansion with the interlocutor's turns supplied, not a word-to-word rendering, and a word-level exercise would actively mis-teach. Bracketed material always displayed as supplied, never as source.

**Standing order.** C15 leads. Do not scan C13 until the upstream column offset is resolved (`docs/upstream/C13_COLUMN_OFFSET.md`). Reading its existing spine segments is fine; the restriction is on new scanning.

**Why later.** The annotation pass is real work and the payoff is narrow but deep. Highest-value item for the expert audience once it exists.

#### Peel — **BUILD LATER**

**Sees.** A segment as one bar. Peel it down clause → phrase → word → compound member, predicting each split before it opens. Scores the *split prediction*, not the translation.

**Data.** 43,325 measured parent→child nestings, including 3,191 compound decompositions; 1,330 segments peel three or more levels.

**Prerequisite.** The nesting is currently expressed in the page HTML. It must be lifted into `allcore` as a spans-with-parent table before a widget can consume it — do not parse HTML at runtime.

**Honesty.** TENTATIVE throughout; refuses outside the aligned courses.

---

### 3.4 Cross-cutting — every audience

#### The key / no-key badge — **BUILD NOW**

Every passage in Trainer, Drills and the reader carries an explicit state: *in-corpus* ("there is a key — his own English for this segment", with citation) or *out-of-corpus* ("no key: engine guidance only"), with a distinct frame and one line on what the guidance can and cannot see. The Trainer's sixth layer already degrades correctly; this makes the degradation visible *before* the learner invests effort. Costs almost nothing and is the thing a working translator will value most: knowing exactly when they are on their own.

#### One ungraded reading-order gate in the Trainer — **BUILD LATER**

The full commit-before-reveal proposal was cut (§6). What survives is one gate: before the reading-order layer opens, ask which chunk you read first. **Ungraded.** Default off. Offered only where `spotVerb` reports `confident == true`. The attempt is recorded with **no correct bit**, and the reveal appears beside it under its existing guidance label. Verb-first reading order in a case-marked SOV language is the skill translators need and a flashcard app cannot touch. The retrieval-practice gain comes from the attempt plus feedback, not from a score, so the ungraded form keeps the documented benefit and drops the breach. Build after the redundancy with drill mode 5 is settled.

---

## 4. The progression system

Adam asked for gamification. The research says gamification's measured effect is on motivation, autonomy and relatedness, with **minimal effect on competency** — and that tangible rewards can undermine intrinsically motivated learners, which describes this audience exactly. So the answer is not to decline it and not to bolt on a cartoon. The answer is to **make the game layer a measurement rather than a reward**.

### The principle

Every game object must be a claim the app can defend with a number printed on the card. If we cannot print the evidence, we do not award the thing.

### XP — earned, with a receipt

XP is proportional to measured item difficulty: span depth, prior failure rate on that skill, first attempt versus retry, recognition versus production. Grinding easy items earns little. **Every award opens into a ledger row**: what the item was, why it was worth that much, what you answered.

Two hard rules:
- **No XP for an item whose key is unverifiable or refused.** Refusals are still valuable (see below), but they are scored on the refusal question, not on a fabricated key.
- **Provisional-tier glosses pay less than curated ones, and the card says so.** This carries the tier discipline all the way into the scoreboard, so the honesty rules become the game's integrity model rather than a warning label bolted on beside it.

**Deferred:** difficulty weighting that depends on corpus frequency rank is **blocked** until the segmentation question in §6 is resolved. Until then, difficulty is computed from features we can actually defend — alignment depth, clause count, chain depth, rendering count, prior per-skill failure rate.

### Scored refusal — the distinctive mechanic

Make **"this is ambiguous"** and **"this cannot be verified from the evidence"** first-class answerable options across every drill. Worth XP when correct. Penalised when used to dodge a resolvable item. Tracked as its own skill in the miss taxonomy with its own progress track.

No language-learning tool does this. It trains the single habit that separates a translator from a machine — knowing when the evidence does not decide — and it converts the project's four inviolable rules from constraints on the software into the curriculum. It also turns every `spotVerb` UNVERIFIED and every unruled segmentation fork from a dead end into teaching material, which *multiplies* the drillable corpus rather than shrinking it.

### Ranks — capability, not accumulation

Points measure practice. **Ranks measure capability**, and conflating the two is what makes gamification feel cheap to serious learners. A rank is gated on demonstrated reading at its band, with the band's actual requirements printed on the card, and it **drops if the evidence stops supporting it**. Rank names come from the courses and the tradition.

Ranks are blocked on the mastery ledger below. Do not ship ranks before the ledger.

### Streaks — harmful as normally built; here is the alternative

Streaks are the mechanic with the **weakest link to learning outcome and the strongest link to the crowding-out failure mode**. They demonstrably generate loss-aversion behaviour that displaces the original goal: the learner practises to protect the number, not to read better. And hours-practised is the metric that explains ~4% of variance in education.

**Our answer:**
- No streak on the main surface. An optional "days practised" line at most, off by default.
- If a streak exists at all it carries **bankable repair credit**, so illness or travel does not erase months. A broken streak must be recoverable.
- The headline number is never volume. It is **what you can now read that you could not**, and **which named weakness closed**.

### The mastery ledger — the prerequisite for all of it

Today "did they get it" collapses to "did they click the right radio button". Four additions to the local progress database make ranks meaningful:
1. Elapsed time per item.
2. Which chunk the learner opened first (the order drill already holds the data).
3. Accuracy **per SRS interval**, not pooled.
4. Accuracy on the same skill **one difficulty band above** where it was learned.

A skill turns green only on the fourth. Transfer to harder unseen material is the only accepted evidence of learning rather than item memorisation, and it is the one thing the current model cannot see. The weak-spots report gains a facing page: **what you have actually got, and the evidence.**

### The whole layer is switchable off in one place

The motivational evidence is too weak to impose this on an expert translator who wants none of it. One toggle, and the tab is a scholarly reading environment with a progress model — which is what every serious classical-language tool in every language already is.

---

## 5. Build order — the first three

**1. Zero to script.** Because there is currently no first rung at all: every mode takes ACIP or wylie as input, so a genuine beginner cannot start. The data is already provenance-labelled (`tibetan_source`, `pronunciation_source`, `card_phonetics.json`) and the converters are battery-proven. Largest audience gain for the least work, and it is the one gap that makes the current tab unusable for a whole audience Adam named.

**2. Known here / known anywhere.** Because it repairs a live defect — the Trainer's six reveal layers touch the deck nowhere, so every reveal leaks — and because it replaces the single weakest surface in the tab (a context-free self-graded ACIP flashcard) with the version only our corpus can serve: the word inside his own segment with his own rendering as the reveal. It also builds the schema the mastery ledger needs, so it is on the critical path for ranks.

**3. Train this — the weak-spot loop.** Because the diagnosis half is already shipped and the counter only grows; because the grammar notes already exist as rule-table data rather than prose to be authored; and because it is the strongest "only our data" argument in the plan. No generic tool can name which grammar skill a wrong chunk-order reveals and then generate a fresh item exercising exactly that skill.

The three compose: (1) opens the front door, (2) builds the ledger schema, (3) turns the ledger into direction. Boundary Hunt, the key/no-key badge, The Silent Particle and His second thought are all small and can land opportunistically alongside.

---

## 6. What was cut, and why

The steelman rule says justify every component before testing it, and cut what cannot be justified even if it would work. These were cut after measurement, not after taste.

**Scored Translate & Compare, and making it the default.** The mode already ships with `checkTerminology`. Three of the four proposed checks fail against his own English: feeding the checker *his* published translation as the learner's draft flags 98.3% of segments on terms, 57.7% on chunks, and contradicts his enumeration in 36.7% of cases. Reading-order distance is not computable at all — nothing aligns a learner's free English to Tibetan chunks, and `eng_order` is absent from the consumable bank. Wiring a score with a 57.7% false-alarm floor to XP teaches one behaviour: pad the English with dictionary words to farm the metric. Making it the first-run default hands a beginner a blank box. **Salvage:** the chunk check survives only as an unscored, neutrally-worded review flag with the baseline disclosed — which is a re-layout of a line already printing three lines above it.

**Your finite mountain — the frequency-ordered curriculum.** Cut on honesty. Two independent tokenizations of the same database gave results 2.5× apart, because the lattice deliberately makes no segmentation decision. The proposal's escape hatch ("use the lattice's decision") does not exist. And "every page in this library" overstates by two orders of magnitude: the pass covers ~0.46% of the 204.6M-token library, and one teacher's course corpus is not the frequency profile of the canon. **Salvage, and it is real:** `entries.corpus_n_segments` already ships in the spine, populated for 29,512 entries, and is referenced by zero lines of C++. Use it as the `ORDER BY` in the deck fill (which today inserts ~10.7k headwords in map order, all due at once) and display it as *attestation* — "attested in N segments" — never as a rank or a percentage.

**Interlinear beginner reading.** Cut on measurement. An interlinear line is English in *Tibetan* order. Across 1,284 aligned segments, 99.2% have his English crossing Tibetan span order, 23.4% of span pairs are inverted, and the median segment has only 42.9% of his English inside an aligned span — so ~57% of his prose would silently vanish. The result is a machine-composed sequence of his verbatim tokens, missing most of his sentence, presented as his rendering. Also: the differentiating part already ships — `alignGrammarHtml()` already renders d=6 particles and d=7 compound members with zero-exponent spans shown as "∅ (no English exponent)", and the alignment pages are already a two-column linked reader with a depth slider. **Recommended instead:** embed that existing page as a Learn-tab reader — his English in his own order on one side, Tibetan on the other, spans lit on hover, depth slider as the difficulty ramp.

**Formula speed trainer.** Cut on measurement. The design needs one attested rendering per high-frequency sequence. The opposite is true: the top-400 glossed n-grams average 13.7 renderings (median 9) against 1.9 for random glossed headwords — `la sogs pa` has 50, `thams cad` 38. Frequency and rendering-plurality correlate about 7:1. A timed drill is also the worst possible surface for TENTATIVE material, because the design goal is to make the learner answer before deliberating, which is the exact state in which a provenance label does not register. Rewarding one fast answer for a 38-rendering term trains auto-resolution of multi-register terms — the *bsod nams* failure. **Salvage:** the debate formulae it surfaced (`yin pa'i phyir` 2,048, `ma grub na` 727) have fixed *moves* even without fixed English — that belongs to the Debate Dojo.

**Six-layer graded commit-before-reveal.** Cut and re-scoped to one ungraded gate (§3.4). Its load-bearing claim was false: only layer 5 has an answer key. Layers 1, 2, 3 and 6 are engine guidance, and the pane's own banner already declares that guidance is not an answer. A commit gate's whole point is a verdict, so grading against TENTATIVE engine output promotes guidance to binding at the moment of feedback — unfixable with a label, because the verdict *is* the feature. It also duplicates Drills, which is already commit-then-check in all five modes. And six gates × N clauses is ~48 commit boxes for an eight-clause paragraph.

**FSRS.** Deferred indefinitely. The efficiency claim is simulation, not a trial. Item quality dominates schedule quality, and our items are where our advantage is.

**Composite difficulty score and the climb framing.** Cut from "what you can read next". A mystery number is the opposite of the house style. What survived is the readiness card that prints the actual features.

**Crowd-sourced mnemonics.** Never. User-authored content flowing into a tool built on tiered provenance is a contamination vector, not a feature. A *private, local, never-exported* note field is fine and is labelled "your note".

---

## 7. Open questions for Adam

1. **Segmentation policy.** Frequency-based ordering, difficulty weighting, and any coverage claim all block on one decision: do we adopt a disambiguating segmenter with a documented tie-breaking policy, accepting that every tie it breaks is a choice the lattice currently refuses to make? Or do we permanently forgo frequency-based features and use `corpus_n_segments` attestation counts only? This is the largest single unblock in the plan.

2. **Course audio.** Does ACI course audio exist for these segments, what is its licence, and does any segment-level timing exist? Glossika and Legentibus both make synchronised audio their core value, and it is the one first-tier reader feature our stack cannot supply from data we hold. Listen-and-read over the teacher's own voice reading his own texts would be unmatched in the field.

3. **Genre and register tags per course.** The register drill and the readiness shelf both improve sharply with a genre label per work (prayer / debate / commentary / oral instruction). We do not have one. Is it worth an editorial pass, and who rules on it?

4. **C15 debate annotation.** May the move-type annotation pass over C15 proceed now, given C13 remains under the standing scan order? The template is regular enough that the parser is small; the question is whether the annotation is a data-project deliverable or an app-repo one.

5. **The 407 teaching passages.** `data/teaching/teaching_moments.json` and `docs/research/GMR_TRANSLATION_METHOD.md` hold 407 located passages where he teaches translation method, 32 of them ruling a rendering *out*. Linking to the timestamp is safe (candidate tier, recording is the authority, and the captions mishear — "sutra" as "citrus"). Do you want that link on answer cards now, or does it wait until anything derived from it clears the approval queue?

6. **Ranks.** Ranks named from the courses and the tradition is our proposal, but naming is yours and Geshe Michael Roach's to rule on. Do you want ranks at all, or is the readiness shelf plus the closed-weakness list sufficient? The research says the shelf is the part that works.

7. **Does the tab default to game-off?** Our recommendation is yes — a scholarly reading environment with a progress model, with the progression layer opt-in. That is the opposite of every consumer tool and, we think, correct for this audience.