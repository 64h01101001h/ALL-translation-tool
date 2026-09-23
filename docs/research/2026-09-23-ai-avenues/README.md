# Every AI avenue for the Geshe Michael Roach dictionary -- research, 2026-09-23

*Adam, 2026-09-23: "use all of your collective might and research prowess to seek out and explore any and all possible avenues for ai tools and ai services and ai workflows and ai automation ... to increase both the speed and accuracy of this project while simultaneously reducing cost."*

**How this was made.** 21 research clusters and 6 gap clusters named by a completeness critic, each researched against current public sources and then attacked by its own adversarial verifier -- 56 agents in all. Where a verifier corrected a claim, the corrected version is used. Every figure is labelled MEASURED (local transcripts, git, config), VENDOR (a price or claim), or ESTIMATE (inference). The full output is `research_output.json.gz` beside this file; the 295 measuring scripts the researchers wrote are in `instruments/`; the complete working directory is archived on the external drive under `ALL-research-archive/`.

**Verified independently before this was banked**, not taken from the research: the `opus` alias moved to Opus 5.5 on 2026-09-23 between 11:49 and 15:51 (every workflow before served `claude-opus-5`, every one after `claude-opus-5-5`); the account is a flat Max 20x subscription with extra usage disabled; no licensor in `span_head_allow.json` records a human ruling; 273 C18 rows are byte-identical to C13 rows; C18:614-623 is mispaired (read directly: each Tibetan row sits one below its English); the teaching-index card says "Geshe Michael teaches this idea" and its builder filters by language only, never by speaker; and HEAD briefly carried a live generator mutant, fixed in d352a01f.

*This readable version applies the project's naming rule: two pronouns whose referent is Geshe Michael were replaced with the name. `synthesis.json` is left verbatim as the record of what the research wrote.*


## Executive summary

BOTTOM LINE (2026-09-23)

This synthesis covers 21 research clusters and 6 gap clusters, each checked by an adversarial verifier. Where a verifier corrected a claim, the corrected version is used. The relayed task text was cut off partway through, so the synthesis was rebuilt read-only from the workflow's own transcripts.

The finding is unglamorous. The biggest gains come from four things:
- stopping silent faults;
- keeping the scarce resource (the weekly plan allowance) for the campaign;
- cutting the round trips that re-send the same context;
- building the one instrument that makes accuracy measurable: a small, blind human yardstick with named readers.

No AI tool or service found anywhere can make accuracy measurable by itself. Every published model-quality claim is graded by the vendor, by an LLM, or here by Claude. None of them covers Wylie/ACIP span alignment.

WHAT "COST" ACTUALLY MEANS HERE [MEASURED]

- Adam's Claude Code runs on a Claude Max 20x subscription, with extra usage disabled (~/.claude.json). The $6.69 per segment and the $27-29K for the mandate are list-price equivalents, not cash.
- The binding constraints are the weekly usage allowance (which sets calendar time) and human adjudication.
- Human adjudication stands at ZERO. The '38 adjudicated' licensors in span_head_allow.json were all named by the reconciler agent, not by a person.
- From 6 Aug to 23 Sep, alignment workflows used about 10.6% of Adam's consumption. Main conversational sessions used about 61.5%, three-quarters of it re-reading long conversations. Other workflows used about 17.9%; that includes audits and research sweeps like this one.
- At that share the mandate takes on the order of 60-95 weeks. Giving the campaign most of the quota is worth more than any vendor switch.

IN ORDER, WHAT MOST INCREASES SPEED AND ACCURACY WHILE CUTTING COST

1. BEFORE ANY FURTHER BATCH (free)
- HEAD commit f36ccffa contains a live mutant of the generator: 'before = True', so it accepts spans that start mid-word. It also contains tools/.mutate.lock. The working tree is correct, but the span-ID compiler takes the generator as its authority, so commit the restored file first.
- Pin the model. The unpinned 'opus' alias in c05-align.js now resolves to Opus 5.5 (this synthesis itself runs on claude-opus-5-5), so the next batch would change producer with nothing on the page to show it.
- Move SCRATCH off /private/tmp. The C05:367-369 specs are already lost.
- Keep both proposals from every run. They are the free routing signal for human review.
- Adam should read his 'Help improve Claude' setting. Under consumer terms it decides whether everything already sent is kept 30 days or up to 5 years, and whether it may be used for training.

2. PROTECT THE QUOTA (free)
- Use a fresh, small-context session per batch. The long-lived orchestrator was about 16% of spend.
- Keep engineering and research fan-outs out of campaign weeks.
- Replace the propose/reconcile barrier with pipeline(): 1.6x per run, by replay of real durations.
- Size runs to the weekly budget left, because weekly limits never auto-resume.
- Pace the work across the week: 3.5 days of the week of 09-15 sat idle.

3. CUT TOKENS PER SEGMENT (free, needs pilots)
- [MEASURED] On the same course, cost per segment roughly tripled, from about $2.1 to $6.69, when workflow-spawned agents replaced direct-spawned ones. Longer segments explain only about 1.25x of that.
- 58% of cache reads are a fixed preamble of about 51K tokens: the Claude Code system prompt, a skill listing, MCP tool names, CLAUDE.md and memory, even Adam's email. It is re-read on each of about 57 round trips.
- 10-13 of the roughly 19 requests per agent fetch deterministic data.
- The fixes: a lean custom agent type, an inlined context pack, the span-ID output contract Adam just approved, and medium effort on the proposers. [ESTIMATE] Together they cut list-equivalent cost 35-61%, depending on model. That roughly doubles segments per weekly window IF quota tracks tokens, which is unverified.
- First re-derive the base with one batch at the direct-era settings.

4. MAKE ACCURACY MEASURABLE (needs Adam to name readers)
- Start with a timing-and-agreement pilot: about 10 whole segments aligned blind by two readers, about 4-8 expert-hours.
- Use a static local HTML pack built on span-ID tokens. Readers click Geshe Michael's tokens and never type English, so Rule 1 holds by construction.
- Then seal a whole-segment yardstick of about 50-60 segments (about 23-53 reader-hours) BEFORE any re-cut of shipped pages.
- Only whole segments measure recall as well as precision. Until this set exists, every cheaper-model, router or cross-vendor claim measures agreement with Claude.

5. RETIRE THE QUEUE BY RULINGS, NOT BY ITEMS
- Most of the 2,256 flagged spans are one Rule 7 class: English articles Geshe Michael supplied, with nothing under them in the Tibetan. That ruling has been pending for 19 days.
- A licensor-checked deterministic trim, with an exclusion list and a 60-item sample, could retire most of it. But it re-cuts shipped C01 pages, and the verifiers found traps: 'a kind of' trimmed to 'kind of' changes the meaning.
- About 80% of the disagreement between the Claude bank and the unmerged Codex bank is depth and granularity convention, which a few rulings would collapse.

6. STOP FALSE CORRESPONDENCES BEFORE THEY ARE MADE (free, local)
- A row-pairing check that uses only Geshe Michael's attested English, compared byte for byte, found the known C13 offset with exact boundaries.
- It also found a NEW, unregistered mispairing at C18:614-623, plus probable ones at C18:655-661, C15:148-165 and C18:1007-1024. All are inside the mandate. This refutes the repo's doc saying the defect class is undetectable.
- 273 C18 rows are byte-identical clones of C13 rows, and C13 is excluded from the mandate for exactly this defect.
- Hold C18:560-760 and the flagged rows until Adam reads them.

7. THEN CHOOSE THE MODEL DELIBERATELY
- Opus 5.5 lists at about 35% less on the same token profile [VENDOR price; quality unmeasured; whether plan quota weights price is unknown].
- The 2026-09-01 Fable-for-reconcile ruling lapsed. Restoring it would cost 12-14% more than Opus 5 and about 40% more than Opus 5.5, and would draw on Fable's separate cap, which has already been hit.
- Sonnet proposers are a pilot, judged by reconciler drops and yardstick labels, not by dollars.

8. LATER BETS
- Single-shot or batched proposing under commercial terms. This turns quota into cash but narrows the data terms.
- A human-read tier: for segments a person will read anyway, one cheap draft plus that person replaces three agents.
- A non-agentic cross-family refuter.
- Oral evidence from the public ACI class transcripts.

DATA

Corpus text already leaves the machine:
- to Anthropic, under consumer terms;
- to OpenAI: the unmerged Codex branch sent C05:166-378 and project files between 11 and 15 September under a consumer ChatGPT Pro plan;
- to GitHub, through the per-batch pushes;
- and agents have read tantric TCS rows through parallel-witness queries.

The C01-C18 English readings have been publicly posted since about 2009 and are in Common Crawl, so 'unpublished corpus' is partly false. The decision should be made per data class, not as one yes or no.

MONEY

- At list price the mandate is about 1.7% of ALL's FY2024 revenue.
- The cheapest cash for extra capacity is usage bundles on the existing plan: $700 buys $1,000 of list usage.
- A nonprofit Team plan mainly buys commercial data terms, not capacity.
- Money moves the objective metric only through paid reviewer time.


## Free and now

1. COMMIT THE RESTORED GENERATOR BEFORE ANYTHING ELSE. [MEASURED today: git diff HEAD -- tools/gen_alignment_page.py] Commit f36ccffa has `before = True` at line 95, so HEAD's find_word accepts spans that start mid-word. It also carries tools/.mutate.lock. The working tree has the correct left-boundary check. The span-ID compiler Adam just approved imports the generator as its authority, so commit the restored file and remove the lock BEFORE the compiler is baselined. Also correct the compiler docstring's '4.7 / 3.7 generator runs' rationale. The real figure is about 1.8-2.0 runs per agent; 62% and 44% of the counted calls were agents READING the generator source. The compiler is still justified, by Rule 1 holding by construction and by removing the eng_order, subword and occurrence burdens that cause those reads. Needs only a session allowed to write the repo.

2. PIN MODEL AND EFFORT; RECORD WHAT ACTUALLY SERVED. Replace model:'opus' at c05-align.js lines 521, 560 and 614 with full model ids, and keep effort explicit. [VENDOR + MEASURED] Since Claude Code 2.1.280 the alias resolves to claude-opus-5-5, and workflow agents in this session already answered as 5.5. Three CLI versions are on this machine: 2.1.224 on PATH, 2.1.255 and 2.1.280 in Desktop. Interim default: claude-opus-5 at all three sites, for continuity with the Opus-5-reconciled C05:166+ pages, until Adam rules (see decisions). Opus 5 retirement is not before 2027-07-24. Every landed page should record: served model (from the response), effort, CLI version, brief hash. That closes TODO AC 'Record on every landed page which pipeline produced it'. $0.

3. MAKE SCRATCH DURABLE AND KEEP THE PROPOSALS. [MEASURED] The session scratch tree has 400 directories and 0 files, and the C05:367-369 specs are gone. Point SCRATCH at a backed-up path outside build/ and keep the propose-A spec, propose-B spec, report.md and reconciled spec for every segment. Extract the recoverable C03-C05 proposals from transcripts NOW: 335 C05 via StructuredOutput, 727 older ones from Bash heredocs, 615 from Write calls, leaving about 237 of 1,579 to replay. Non-Desktop transcripts are deleted after 30 days. Label the store 'machine teacher data, not human truth, training use not authorized'. These proposals are the free routing signal for human review, the only way to re-derive '41 of 4,782 unanimous spans dropped', and the sampling frame for the yardstick.

4. BANK THE SCRATCHPAD INSTRUMENTS INTO tools/ WITH MUTATION TESTS before the session scratchpad is cleared. Otherwise every finding below becomes a number nobody can re-derive.
- cost_ledger.py: dedupe by keeping the LARGEST usage row per request. First-row dedup undercounts by 29-87%, depending on role. Price each response by the model id the response reports, and refuse unknown ids. Add a plan-% column and an expert-minutes column.
- repair_census.py.
- The row-offset detector plus its planted-offset test.
- coverage_forecast.py.
- The proposer-vote and drops diff.
- The 189-segment three-way archive.
Four independent parsers reproduced $6.65-6.71 per segment and 57.1-57.8 calls. Neither $6.69 nor '41 of 4,782' can be re-derived from the tree today.

5. CHECK THE DATA SETTINGS THAT GOVERN WHAT HAS ALREADY BEEN SENT. [VENDOR]
- Adam opens claude.ai privacy settings and records the state and date of 'Help improve Claude'. Under consumer terms it decides 30-day versus up-to-5-year retention and training use.
- Set CLAUDE_CODE_DISABLE_FEEDBACK_SURVEY=1 and DISABLE_FEEDBACK_COMMAND=1 for campaign sessions; feedback uploads are kept 5 years.
- Note that Remote Control syncs transcripts to Anthropic while connected.
- Do the same check on the ChatGPT Pro account used for the Codex run.
- Pause the per-batch `git push origin main` until Adam rules on pushing (already TODO 'Adam: say whether to push').
Costs nothing and sends nothing new.

6. PROTECT THE WEEKLY QUOTA; THIS IS THE CAMPAIGN'S REAL CURRENCY. [MEASURED]
Where the quota went, 6 Aug to 23 Sep: alignment workflows 10.6%, main sessions 61.5%, other workflows 17.9%. This research sweep used about $367 of list-equivalent on 09-23 by mid-afternoon.
Standing rules:
- a fresh, small-context driver session per batch (the long-lived orchestrator re-read up to about 0.97M cached tokens per call and cost about 16% of campaign spend);
- never mix engineering and campaign work in one session (that mix is what committed the mutant);
- no large fan-outs in campaign weeks;
- log rate_limits.seven_day.used_percentage from the statusline;
- size runs to the remaining weekly budget, because weekly limits are never auto-resumed and a mid-run failure re-runs every agent started after it;
- pace across the week (3.5 idle days observed);
- keep usage credits off or capped before any unattended scheduling.

7. REPLACE THE PARALLEL() BARRIER WITH PIPELINE(). Each segment's reconciler starts as soon as its own two proposals land. [MEASURED by replaying real agent durations] 1.59x faster per 9-segment run: 34.6 to 22.2 minutes, 11.96 to 7.54 run-hours over the sampled runs. Launch the slowest-expected segments first. Zero accuracy effect, under an hour of work. Raising CLAUDE_CODE_WORKFLOW_MAX_CONCURRENT_AGENTS is only optional: it spends the weekly window faster and widens the resume replay.

8. RUN THE LEXICAL ROW-PAIRING CHECK BEFORE EVERY COURSE, AND HOLD WHAT IT FLAGS.
What it is: for each row, attested renderings from the bank and hgm_gloss are matched byte for byte against the row's own English and its neighbours'. It uses no similarity model and no embedding.
[MEASURED, verifier-reproduced]
- Ranks the known C13 offset in the top 10 of 41,251 windows.
- Planted ±1 offsets of 10-20 rows are found 97-100% of the time, with no false windows at the planted sites. Runs of 5 rows or fewer are missed; add ±2 lags.
- Flags about 46 mandate rows by reading: C18:614-623, C18:655-661 (probable), C15:148-165 and C18:1007-1024.
- 273 C18 rows (163 in C18:560-760) are byte-identical to C13, the course excluded for this very defect.
Actions: hold those rows; Adam reads about 7 regions (about 20-40 min); file the confirmed ones upstream beside docs/upstream/C13_COLUMN_OFFSET.md; correct COLUMN_OFFSET_UNDETECTABLE.md through the normal process; apply the same gate to duplicate transfer and the precedent index, so a mispaired row is never copied or cited. Saves only about $190-310 list, but prevents roughly 150-700 false correspondences (Rule 10).

9. SEND DISAGREEMENT TO PEOPLE, DETERMINISTICALLY. Diff the two proposals against the reconciled spec for every segment. Every dropped unanimous span, and every A/B dispute, goes to the human queue with its vote pattern stamped: both, one, reconciler-new, overturned. Disagreement is a label-free LOWER bound on error. Agreement proves nothing, because both proposers are the same model and correlated errors are documented (Kim et al., ICML 2025). $0, about a day. Never auto-accept unanimity.

10. FIX TWO HONESTY DEFECTS ON SHIPPED APP SURFACES.
(1) Teaching-index cards say 'Geshe Michael teaches this idea / says the Tibetan phrase'. build_teaching_index.py applies no speaker filter. [MEASURED by verifier] At least 43.5% of English-card moments and 19.2% of spoken-Tibetan moments come from videos titled for other teachers. Change the header now to 'in the recorded teachings; speaker not verified'. An allow-list of Geshe Michael videos is Adam's ruling.
(2) The evidence card ranks renderings by refs.size() (app/main.cpp about line 45680). Transfers and repeated liturgy inflate that count. Show transfer provenance and count independent passages (already TODO AC).

11. BUILD THE SOURCE-WITNESS PACKET FROM WHAT IS ON DISK, NO OCR.
[MEASURED]
- 98.2% of the mandate's non-trivial Tibetan traces to ACIP master files already on disk, so OCR is never intake here.
- ACIP status letters on sibling releases settle 19 of 26 open C05 TIBETAN_SPELLING errata locally. The course copied the M-level release; the expert-proofread N-level release has the corrected reading.
- Add an English-side in-spine parallel index (shingles, re-ingest vs independent). 81% of agents' witness searches are English LIKE queries.
- Estimated saving about $200-1,260 list over the mandate.
Same commit, needs Adam because the briefs are gated: amend the 'PUBLISHED READING' clause so that agreement with the master never cancels an erratum. Most errata are inherited from ACIP, so master agreement is lineage, not corroboration.

12. CORRECT THE PROJECT DOCUMENTS THAT NOW STATE FALSEHOODS (Rule 10), via the normal process. The claims to correct:
- §VII.3: harness repair iterations are not free local calls. Each is a full API round trip re-reading about 90K tokens, and the '4.7 runs' figure is mostly source reads.
- §VII.4: the fan-out cache stagger already exists and works. 487 of 506 agents read the shared 32,908-token prefix on their first call.
- 'Unpublished corpus' (TODO.md:4496, BACKLOG.md:364, AUDIT §VII:57): qualify it by data class.
- '38 adjudicated': these are reconciler-named, and no human is recorded.
- 'precision 1.00 / recall 0.58': in-sample and Claude-labelled.
- '222 of 223 duplicates agree': 883 of 908 of those spans are d=7 phonetic links, so the figure says little about d=5 transfer.

13. START A RULINGS REGISTER THAT CODE AND GATES READ, plus a weekly decision pack delivered IN THE CONVERSATION.
[MEASURED] Rulings asked in chat closed in minutes. Rulings filed in documents sat 14-32 days, and none of 15 TODO ruling checkboxes was ever ticked. The model-policy lapse went at least 5-6 days unseen.
What the register needs:
- The gate must compare the SERVED model from transcripts against the ruled policy, not just the code constant.
- The census must also match deviation wording ('fell back', 'instead', 'MODEL DEVIATION').
- Any row whose effect relaxes a gate must quote Adam verbatim, checked against a user-role message in the local transcripts. All 459 commits since 09-01 are authored 'vajra108', so git cannot show who ruled.
- The first pack is the ten TODO AC 'Waiting on Adam' items, each with measured options.

14. BANK THE COVERAGE FORECAST AND ADD 'WHAT THE NEXT DOLLAR BUYS' COLUMNS TO THE LEDGER. [MEASURED] Good's coverage is 0.83 for d=5 headwords and 0.50 for (Tibetan, English) pairs, so half of the next links will be pairs never banked. Yield of new headwords per link fell from 0.44 (C01) to 0.13 (C05). [ESTIMATE] The remaining mandate adds roughly 2.4K-6.4K new headword strings: about 2.7K on a ≤2-syllable lexical core, 5.9-6.3K counting surface strings. Print the whole cross-method range, never one headline. It costs $0 and answers the question the pending reading-order and scope rulings need.

15. UNBLOCK THE TOOLS FOR C06-C18. [MEASURED] land_alignment_page.py requires page ids matching c(\d)p(\d+) and writes to pages_c0%s, so it will refuse C10-C18. c05-align.js hard-codes COURSE='C05' and Course-5 context. Parameterise both before automating beyond C05.

16. LOCAL SAFETY RAILS FOR ANY UNATTENDED RUN.
- Put sandbox.filesystem.denyWrite plus Edit deny rules on briefs, generator, test_*.py, baselines, mutation_sweep.json, span_head_allow.json and workflow scripts. Every write that mattered overnight was a Bash heredoc, so a hook on Edit/Write alone would not have stopped them.
- Block `git add -A`, `--no-verify`, force-push and push.
- Land through a deterministic land_batch.py that sorts each segment into LANDED, FAILED or UNVERIFIED from exit codes and files present, never from `tail -1` or a model's summary. Measured overnight: `git add -A` 41 times, push 35 times, gate output read through tail -1 132 times.
- Run mutation sweeps in their own worktree.


## Cheap pilots

1. TIMING-AND-AGREEMENT PILOT: the one measurement every hour figure depends on (needs Adam to name 2 readers).
Design:
- 10 segments aligned WHOLE and blind by two readers: 7 random, 3 from the flagged pool, and at least some UNLANDED C06 segments, because Adam has already seen C01-C05 output.
- Use a static local HTML pack generated from span_id_compiler tokens: click-pairing (not drawn lines), a 'possible' state separate from FLAG, forced disposition of every Tibetan token including d=6 and d=7, and a timer. Readers never type English.
Record minutes per segment, seconds per produce decision against verify decision, and chance-corrected agreement.
Cost: $0 cash, about 4-8 expert-hours, nothing leaves the organisation.
Why it matters: every estimate in this report (15-45 s per link, 1-3 min per adjudication) is an assumption until this runs. The only external anchor is 86% expert agreement on Ancient Greek-English (Yousef et al., LREC 2022).

2. RE-DERIVE THE BASE: why did cost per segment triple? [MEASURED on C05]
- Direct-spawned agents, 09-01 to 09-08: medians $0.75 propose and $0.61 reconcile, 9 and 7 turns, about 11K and 7K output tokens, 2 generator runs.
- Workflow agents: $2.24 and $2.05, 19 and 18 turns, about 36K and 29K output, 5 runs.
Candidate causes (unverified): effort 'high' on c05-align.js lines 560 and 614, inlined briefs and precedent, new gates, reconcile moved from Fable to Opus.
Experiment: one 9-segment batch at direct-era settings against the current settings, on C05:166+ (the Opus-5-reconciled era), with a leave-one-out bank so segments cannot see their own landed links.
Compare tokens, generator runs, reconciler drops and overrides. Report disagreement, NOT accuracy; the extra spend may have bought the post-audit quality.
Cost: about $60-120 list-equivalent in quota.

3. SPAN-ID CONTRACT INSIDE THE HARNESS (TODO E03), with an unconstrained pointer echo.
Setup:
- Replace SPEC_SCHEMA's retyped strings with integer token ranges.
- cls becomes an enum; every field required; additionalProperties false.
- Errata 'found' becomes a token range.
- Each span carries a short free-text echo that the compiler checks against the slice. A neighbouring-token mismatch is a mis-point; no match is a typo.
First, at zero engineering, add one brief line telling agents to pass an object, not a string: 11 of 12 schema rejections were stringified payloads.
Measure on the next pinned batch: source reads (94% of generator-source reads happen before the first run), requests per agent, refusals by code, echo mismatches.
Quota-neutral or better.

4. LEAN CUSTOM AGENT TYPE (smoke test, then pilot).
What changes: .claude/agents/align-propose.md and align-reconcile.md with tools Bash, Read, Write; omitClaudeMd; pinned model and effort; maxTurns counted as a failure. The body is the verbatim brief plus a gate-synced excerpt of the Inviolable Rules AND the memory-borne rules (e.g. 'name Geshe Michael').
Step 1: one agent on a trivial task (cents). Read its first-call cache_read and attachments from the transcript. The docs and observed behaviour disagree about whether memory still attaches, so this test cannot be skipped.
Step 2: a 3-9 segment pilot. Pass if the fixed prefix is 15K tokens or less and every generator exit is 0.
[ESTIMATE] Saves $1.2-1.7 per segment list at Opus 5, $0.6-0.8 at Opus 5.5, less once round trips fall. Strictly LESS data leaves: CLAUDE.md, memory, MCP tool names and Adam's email stop going out about 58 times per segment.

5. CONTEXT PACK v2, TRANSPORT ITEMS ONLY.
Precompute deterministically and inline:
- Course shape (byte-synced and gated).
- A generator-contract summary, with a gate that every refusal class is listed.
- Neighbour rows.
- An align_check command that prints a compact span table, so agents stop cat/sed-reading body.html.
- Precedent counts stamped with the bank's sha256, re-verified in the pre-flight.
The stamped counts override the written instruction 'count it yourself' (c05-align.js lines 598-604), so Adam must approve that part.
Keep hgm_gloss n-gram rows and witness candidates OUT. Giving both proposers the same pre-fill correlates their errors, which is exactly why unanimity is unsafe (hard fact 3); test those in a separate ablation.
[ESTIMATE] About 19 requests per agent fall to 6-7; about -35% alone, about -49% with the lean agent at Opus 5.

6. PROPOSER EFFORT 'MEDIUM' A/B. Reconcile stays at high.
[MEASURED] Thinking is about 24K tokens per proposer and about 24% of today's bill. It becomes about 60% after the transport fixes.
[VENDOR CLAIM] On other tasks, medium kept accuracy at 50-87% of the cost.
Run inside the pinned shadow on the same 9-18 segments. Measure thinking tokens, reconciler overturns, dropped unanimous spans and generator runs.
A cascade that escalates to high on refusal saves little unless escalation is rare: 0.7 + 0.2 x 1.0 = 0.9 of the original cost.

7. SONNET 5 PROPOSERS WITH AN OPUS RECONCILER, in shadow.
Cost [VENDOR prices]: 2/3 x $2.68 + 1/3 x $4.34 = $3.23 per segment list, against $4.34 all-Opus-5.5 and $6.69 today. That is about -26% against the new base, not -52%.
What to measure: reconciler overrides, dropped unanimous spans and the number of spans needing adjudication. Do not judge it by $ per segment: expert seconds per link move all-in cost more than vendor choice does.
About $120 list. Freeze the shadow set and re-run it when Sonnet 5.5 or Haiku 5.5 ship ('coming weeks', vendor 2026-09-22).

8. DRY-RUN THE RULE 7 ARTICLE CLASS IN A WORKTREE. Land nothing.
[MEASURED] 1,430 of the 2,256 flagged spans (63%) are a supplied article before a lowercase word. That is an UPPER bound.
The verifier's sample of 25 found 4 doubtful trims: 'a number of', 'a great many', 'a kind of' becoming the hedge 'kind of', and 'the person himself'. About 36 a/an quantifier idioms and some HTML-entity demonstratives must be excluded.
Build the licensor-checked trim with:
- the idiom and quantifier exclusion list;
- HTML-unescaping;
- the genitive and possessive exclusion;
- pronoun renderings excluded;
- a re-run of gen_alignment_page.py.
Count trims and exceptions. Then a seeded 60-item stratified sample goes to a named reader, but ONLY after the yardstick is sealed: the flag is both the yardstick's flagged stratum and the PPI predictor.
Adam then rules; the item has waited 19 days. This touches shipped pages, which are 'ratcheted, not re-cut'.

9. CONVENTION-RULING PACK AND RULE 10 QA FROM THE CODEX BANK. No new model calls; the Codex bank is never merged.
[MEASURED] On C05:178-366, Claude and Codex agree on 57.6% of exact d=5 pairs. About 80% of the disagreement is depth, granularity and null convention.
One pack for Adam (1-2 hours) on:
- nested vs flat spans;
- subword negation splits;
- nulls vs omissions;
- the d=1-4 layers under any human route.
Apply the rulings deterministically.
The residue becomes a small queue on the already-landed Claude bank: 14 different-English conflicts, 162 null conflicts on content words such as rang bzhin, yi dvags and mig shes, and 5 errata only Codex found.
Caveat: both lines shared the briefs and precedent, so this measures conditional independence, not verification.

10. READER RECRUITMENT TEST. Three in-house bilingual ACI-trained readers each adjudicate the same 30 randomly drawn spans, blind and timed. Include one reader outside the lineage for the 15% overlap, so register sharing is visible. About 3-8 person-hours. Produces the first human minutes-per-span and inter-rater figure. Drop 'Tibetan-only reviewers': every measured defect class (13 OVER, 4 UNDER, 2 WRONG_MAPPING) needs a bilingual judgment.

11. DETERMINISTIC NEAR-DUPLICATE LANE.
[MEASURED] 72-110 remaining segments differ from a landed segment only in punctuation, case or markers. The two verifiers' counts differ; the tool must settle it.
Move them by a generator-checked transfer with no agent, under a distinct provenance mark.
Dry run: free. Upper bound $480-736 list.
Needs Adam's ruling on the provenance class.
NEVER transfer on 80% overlap: 346-690 segments share Tibetan with landed ones but carry different English. That is exactly where a verbatim span can be false, and it is translation-history evidence.

12. LOCAL DETERMINISTIC ALIGNER AS A RAISE-ONLY RANKER (IBM2/HMM with posteriors).
The numpy IBM1 floor exists and is reproducible. eflomal is not recommended here: it has no seed and no per-link posteriors, and it has an open Apple-Silicon build issue.
Plan: score every landed d=5 link from the corpus-only arm, then SEAL the scores before any yardstick labels exist.
Pre-registered stop condition: on links where the precedent index has no answer (about 41% of located links), it must beat that index. [MEASURED] It matched the exact span only 7.7% there.
Use it only to ORDER existing queues, never to add candidates or clear spans. Its statistics endorse supplied English hinges, which are the dominant over-capture class.
$0, about 1 day. It is a statistical correspondence model, so Adam should confirm it falls under 'surfacing only'.

13. PER-COURSE HUMAN ACCEPTANCE SAMPLING, once readers exist. Run SPRT on C05, the course the pipeline actually produces, with p0 = 5%, p1 = 15% and alpha = beta = 0.10. Average sample is about 25-35 items, about 1 expert-hour [ESTIMATE]. 'Accepted' certifies a bound on the course's defect rate, never an individual link, and never moves a tier label. Across all 17 lots: 7-34 expert-hours.

14. ORAL GLOSSES FROM THE PUBLIC ACI 1-18 CLASS TRANSCRIPTS: the one witness that is Geshe Michael's own voice, not Claude.
Pilot on C05:
- 61 auto-resolved '{term} means ...' frames plus 30 unresolved ones.
- Tibetan resolved FORWARD only, with pron_engine on that course's own reading.
- Restricted first to headwords with 0-4 bank links.
- Blind to the bank's English; clips for half the items; 15% double-coded.
Only a listener's verbatim of Geshe Michael's speech may ever be banked; regex captures are truncated.
38.6 MB of public PDFs come in; nothing goes out. It competes for the same scarce readers, so run it after the yardstick. It bears on wrong mappings and register, not on over-capture.


## Strategic bets

1. A NAMED READER PROGRAMME AND A HUMAN-READ PROVENANCE TIER. This is the only route to the objective metric's numerator.
Before the first human link lands, add a per-pair `review` field (reviewer, blind or assisted, produced or verified, seconds, disposition) to the evidence-layer schema, so that the imported release, not a Dropbox side-store, carries 'human-reviewed' (PHASE3 §5c.1 and §6).
Who the readers could be:
- the ACI-trained translators (the Mixed Nuts roster and the DCC editor);
- input-centre staff;
- supervised Tibetan-track students with gold checks.
The reader who sets conventions must be kept separate from the reader who grades the yardstick, or it becomes a circular battery.
[ESTIMATE] Measurement alone is 40-110 expert-hours over 2-4 months. Verifying every new d=5 link (about 60-77K) is 250-960 hours at 15-45 s, or 1,100-3,300 hours at 1-3 minutes. That is only feasible with a programme, so the realistic claim is certified rates plus human-verified strata.

2. THE HUMAN-READ PRODUCTION REFRAME, for the segments people will read anyway. Once a person reads a segment, that person is the reconciler. So pay for ONE span-ID draft and a human check, instead of three agents plus the human.
[ESTIMATE at an unverified $40/h] Draft plus verification: $12.4-24.3K for the mandate. Status quo plus verification: $29.2-49.5K. The draft only loses if it adds more than 5.8-9.7 minutes of correction per segment.
[INDEPENDENT] Schroeder et al. 2025 (410 annotators) found LLM suggestions did NOT speed annotation and pulled labels toward the model. Measure produce-versus-verify time in an 18-segment crossover before relying on it.
The 15% double-read must be blind PRODUCTION by a second reader. Output from this route is never the yardstick.

3. SINGLE-SHOT PROPOSE, THEN BATCH, THEN RECONCILE TRANSPORT.
Order of work:
- First, a single-turn variant INSIDE the harness: no new billing, no new terms.
- Only after Adam's rulings on billing, terms and 'no paid API generation' (BACKLOG.md:359): Message Batches at 50% off. Batch results are stored up to 29 days, can be deleted after retrieval, are not ZDR-eligible, and run under commercial API terms, which can NARROW exposure compared with consumer terms.
[ESTIMATE] If single-shot thinking stays near today's 26-29K, the saving is 44-51% of segment cost, not 63%.
Once propose is single-shot, the agentic reconciler becomes 75-85% of the bill, so the next lever is reconcile transport: count once from the index.
Errata duty, including the parallel-witness search, must be precomputed in a witness packet, or a deliverable is silently dropped.
On a subscription this converts free quota into new cash of roughly $5-8K list. It pays only if calendar time or data terms bind.

4. A COMMERCIAL-TERMS SEAT STRATEGY, sized by measured capacity.
Options:
- Claude for Nonprofits Team premium seats. [VENDOR] $100-125 per seat, 'no model training on your content by default', Fable only on premium seats. Allowance compared with Max 20x is unknown and may be LOWER per dollar.
- Enterprise: seat price plus usage at API rates. That turns about 200 segments a week into about $1,338 a week at list unless a usage discount applies, and none is documented.
First measure segments per weekly window per seat type (statusline logger plus quiet-machine batches). The strongest reason to move is data terms, not price.

5. A NON-AGENTIC CROSS-FAMILY REFUTER, only after the yardstick and a data ruling.
What it is: one structured call per reconciled segment, from GPT-6 Sol or Astra, or Gemini 3.8 Flash. It emits only FLAGS: span id, reason, optional verbatim alternative. A local gate refuses non-verbatim English, and flags go to people, never to the bank.
[ESTIMATE] About $0.05-0.50 per segment, $200-2,000 for the mandate. The agentic Codex version would be about $13.3K.
It must show INCREMENTAL precision over the free gate on the same segments, or it only adds queue.
Evidence for choosing a family:
- Kim et al. (ICML 2025): accurate models' errors correlate even across providers, so a different vendor does not buy independence.
- DharmaBench span DETECTION on classical Tibetan: Claude 4 Sonnet scored below chance on quotation detection (55.2 vs 60.1), while Gemini 2.0 Flash scored 76.2. That is a prior for which family to enter, not a result.
It adds a new recipient of Geshe Michael's English.

6. A COVERAGE-DEFINED FINISH LINE AND A SCOPE RULING.
Define 'done' by headword-incidence coverage, certified by a sealed random sample, plus a price cap per new LEXICAL headword that Adam sets and the ledger recomputes each batch.
[MEASURED, $0 text proxy] The mandate's unique remainder holds only about 16-23% of the uncovered vocabulary across the unread spine; TCS alone holds more.
For 'the Michael dictionary', Adam's scope ruling beyond C18 moves more vocabulary than any reading order within it.
Vocabulary-greedy order (TODO AB Lever 2) front-loads yield, which changes both the forecast and any stopping rule.

7. A RESEARCH-COMMUNITY LAYER, for the wider spine rather than the mandate timeline.
- Pooled adjudication, starting with the pool that already exists: propose-A, propose-B and the reconciler, then eflomal raise-only, then Codex only after a ruling.
- A blind, cluster-sampled audit of the unanimous region, published with the layer.
- Later, a Tibetan-English span-alignment shared task (LT4HALA 2028 or ALP 2027, EvaCun template) on class-A text, plus a thesis/GSoC pipeline with Dharmamitra/Berkeley and UGARIT/Leipzig.
The mandate's own results arrive long before any shared task. Its value is independent systems and independent annotators, which the project has none of.

8. A LOCAL, GRAMMAR-BOUND, POINTER-ONLY OPEN MODEL (MITRA-Qwen3.5 2B/9B, Apache-2.0, reads Wylie).
Use it as a shadow proposer or raise-only ranker whose output is only token ids, so it cannot compose English.
Blockers:
- disk: 16 GiB free; the 9B bf16 checkpoint alone is 17.9 GB;
- mlx-lm documents no constrained decoding;
- a single-shot local model drops errata duty and report.md;
- replacing a Claude analyst collapses the two-analyst design.
Candidate only after the yardstick, and after the 83 GB Codex working directory is archived to external storage.


## Avoid -- and why, so nobody re-proposes it

1. LETTING THE 'opus' ALIAS CHOOSE THE MODEL, or restoring the lapsed Fable 5.1 reconcile policy without pricing it. Fable costs +12-14% against Opus 5 and about +40% against Opus 5.5. It has a separate 50% weekly cap, already hit on 09-04, 09-06 and 09-09. It refused on safeguards on 09-16. It cannot run under ZDR on the API.

2. AUTO-ACCEPTING UNANIMITY in any form: Prodigy --auto_accept, 'Double Triangle' consensus, banking where both analysts agreed, or treating oral-transcript and bank agreement as verification. The reconciler dropped 41 of 4,782 unanimous spans, in about one segment in six. Two Claude analysts are one model.

3. SHOWING REVIEWERS THE MACHINE VERDICT, REASON OR CONFIDENCE BEFORE THEY COMMIT. [INDEPENDENT] Berzak et al. 2016 measured 33.7-49.2% apparent error reduction from anchoring. Schroeder et al. 2025 found slower annotation and label overlap with the LLM rising from 40% to 81-87%. Reveal the machine note only after the reader commits, and record both judgments.

4. REPLACING ONE LLM ANALYST WITH eflomal, a local scoring tier or a student model, and skipping or distilling the reconciler. Each collapses the two-analyst design that Rule 10 relies on. The measured saving ceiling is $2.27 per segment at best, and the aligner matches the exact span only 7.7-12% of the time where precedent cannot help.

5. FINE-TUNING OR DISTILLING ON THE CLAUDE-BUILT BANK, including RL or DPO with the generator as reward. The student is bounded by a teacher with a Claude-labelled 19% defect rate. Training on Claude outputs needs Anthropic's written authorisation under its Usage Policy. The reward teaches imitation and rewards verbatim falsehoods. The margin over a Sonnet or Haiku proposer is at most about $1.8-3.7K.

6. CHEAP ANTHROPIC-COMPATIBLE ENDPOINTS AND FLAT-RATE THIRD-PARTY PLANS: DeepSeek, Kimi, GLM and the Z.ai coding plan, Copilot pools, Meta Muse Spark's Contributor tier. [VENDOR] DeepSeek silently remaps unknown model names and ignores cache_control, and its data is stored in the PRC. Z.ai serves GLM-5.3-Flash when GLM-5.4 is requested. Discount-for-training tiers hand Geshe Michael's corpus to someone else's training set.

7. ANY MODEL THAT PRODUCES ENGLISH IN PLACE OF MATCHING IT. That covers LLM-drafted definitions, sense labels, register labels or examples; word-sense induction; translation models (Qwen-MT, Hunyuan-MT, MITRA-translate, NLLB, MADLAD, Monlam MT); pivot alignment through MT; and audio-LLM extraction. All are Rule 1 composition. Word-sense induction scored ARI 0.21-0.30 in this domain.

8. TREATING THE ARTICLE CLASS AS 'MINOR, TOLERATE'. Rule 7 calls an unlicensed supplied article over-capture, i.e. English with no Tibetan under it, and banking it knowingly violates Rule 10. The same goes for any 'certified' statistical region shown with verified styling: a certified region knowingly holds up to alpha falsehoods and must look TENTATIVE.

9. USING THE PRECEDENT MAJORITY AS A DEFECT SIGNAL, or auto-anchoring known pairs. [MEASURED] About 2 of 3 auto-anchors disagree with the landed page, and 33.5% of C05 d=5 links are renderings never attested before. Queueing Geshe Michael's minority renderings as suspect normalises Geshe Michael's terminology toward the majority, which breaks translation-history fidelity. Dense or semantic precedent retrieval is also out: its measured headroom is at most about 2.4% of links, and a neighbour's rendering is not Geshe Michael's rendering of this word.

10. TRANSFERRING ALIGNMENTS ON NEAR-DUPLICATE OVERLAP (for example 80% of shingles), or merging the Codex bank blind. Near-duplicates with edited English are exactly where verbatim spans can be false. Two banks for C05:166-369 must be compared, never merged.

11. HOSTED PLATFORMS THAT TAKE THE CORPUS for a unit they do not model. Eval and trace platforms (Braintrust, Langfuse, LangSmith, OpenAI Evals, promptfoo --share), hosted annotation (Label Studio Cloud, Ugarit hosting, Zooniverse), hosted lexicography (Sketch Engine, Lexonomy, DURel) and vendor prompt optimisers all fit this. None models a link between two numbered token streams tied to a source hash, and each adds a recipient and a retention window.

12. GATEWAYS THAT CARRY THE MAX OAUTH TOKEN or rewrite requests: LiteLLM, a localhost tap proxy, Vercel passthrough. The benefit is capped at about 5.9%. They put a credential in the path (LiteLLM had a PyPI compromise and several CVEs in 2026). A proxy that strips cache_control makes the campaign 4.2x more expensive with no error.

13. HARNESS FEATURES THAT RAISE COST HERE. A 1-hour cache TTL adds $0.7-0.9 per segment. Lockstep batching of the agent loop can triple the bill at 30-98% batch cache hits. Several segments per agent context adds $0.49 per segment and anchors later segments on earlier ones. Context editing, compaction and the memory tool rewrite the cache and carry unverified notes forward. Programmatic tool calling and Managed Agents are not ZDR-eligible and save no tokens. Heavy orchestrators (Temporal, Prefect, Dagster, Inngest, LangGraph) solve distribution, which this project does not have.

14. ROUTERS AND CASCADES THAT ADD TIERS WITHOUT A MEASURED ESCALATION SIGNAL. Commercial and learned routers (RouteLLM, OpenRouter Auto) and the Jaccard-gated escalation cascade fall here: [MEASURED] at a 0.6 cutoff it costs 32% MORE than all-Opus-5.5. Segment Jaccard does not locate reconciler overturns (medians 0.550 vs 0.568).

15. SELF-EVOLVING PROMPT AND RULE LEARNERS (ACE, SIMBA, CASD, TextGrad, label-free SPO) and GEPA before its prerequisites. They write rules or renderings by machine, which is a Rule 1 back door through paraphrase. They optimise toward agreement with Claude. Best-case payback is $1.1-4.3K at list, and may be $0 in cash.

16. PAID EXPERT-DATA MARKETPLACES AND ENGLISH-ONLY CROWDS (Mercor at about $109/h, Surge, Prolific, MTurk). No classical-Tibetan supply was found, they cost about 2.7x the working rate, and they give the widest data exposure.

17. FRONTIER VISION MODELS AS TIBETAN OCR, and bulk OCR-versus-ACIP diffs. On BDRC's benchmark general models are 3-35x worse than the specialised Yigdzin model (Fable 5 43.2% CER, Opus 4.8 56.2%). A bulk diff yields about 12,000 candidates for a queue that clears tens. The mandate's Tibetan is already digital, so OCR is at most a page locator.

18. FUNDING THAT STEERS MODEL CHOICE. Hyperscaler nonprofit credits (Azure explicitly, and AWS through its Marketplace exclusion) cannot buy Claude, and would pull the project toward models with no Tibetan evidence. NEH is ineligible on subject. Monlam services may train on submissions.


## Roadmap

1. STEP 0, TODAY, BEFORE ANY BATCH. Free; no data leaves; needs only a session allowed to write.
Actions: commit the restored generator and remove .mutate.lock; pin full model ids (interim claude-opus-5); make SCRATCH durable; keep all proposals; extract recoverable C03-C05 proposals from transcripts; bank the scratchpad instruments into tools/; Adam reads his training toggle.
Unlocks: a trustworthy generator for the span-ID compiler; comparable measurements; the only Opus 5 baseline left; routing signals for human review.

2. STEP 1, DAYS 1-7. Free, local.
Actions:
- Cost ledger (served model, effort, per-agent, plan-%, expert-minutes column).
- Quota hygiene and pipeline().
- Row-pairing check with planted-offset test; hold C18:560-760 and the flagged C15 rows; Adam reads about 46 rows.
- Teaching-index speaker-claim fix.
- Source-witness packet.
- Rulings register and first decision pack, in conversation.
- Correct stale docs.
- Parameterise tools for C10-C18.
Needs Adam: reading the held rows (about 20-40 min); the E-107 ruling on the 1,500-character English cap, which truncates 97 mandate segments, before C06, C11, C12 and C16-C18.
Unlocks: an honest base; no false-pairing correspondences; a campaign that can run past C09.

3. STEP 2, WEEKS 1-3: THE HUMAN GROUND TRUTH. Nothing depends on it for SPEED; everything depends on it for ACCURACY.
Needs Adam or the organisation: name 2-3 bilingual readers, decide whose decisions count as 'human-verified', and approve showing about 60-70 segments to them.
Actions:
- Build the static local HTML reader pack on span-ID tokens.
- Run the 10-segment timing-and-agreement pilot.
- Condense the ten rules plus campaign conventions into one reader's guide (unbudgeted work that must come first).
- Seal a 50-60 segment whole-segment yardstick (unique text, split by passage family, stratified by course era and propose agreement, part of it unlanded C06+).
- Add the `review` schema field.
Must be sealed BEFORE any Rule 7 re-cut.
Unlocks: measured precision AND recall, human agreement (the ceiling), seconds per decision, and the numerator of the objective metric.

4. STEP 3, IN PARALLEL WITH STEP 2. No money; quota only.
Efficiency pilots on C05:166+ in shadow, each measured by the ledger against the pinned baseline with a leave-one-out bank:
- the base re-derivation (direct-era settings);
- the span-ID contract (E03);
- the lean agentType smoke test and pilot;
- context pack v2 (the stamped-counts part needs Adam);
- proposer effort at medium.
Report disagreement, never accuracy.
Unlocks: an estimated 35-61% fewer tokens per segment, so more segments per weekly window, before any model decision.

5. STEP 4, AFTER THE YARDSTICK EXISTS. Needs Adam's rulings.
- Model decision: shadow Opus 5.5, and Sonnet proposers, scored on the yardstick.
- Rule 7 class ruling: dry-run counts plus a 60-item sample.
- Convention rulings from the Codex comparison.
- Per-course acceptance sampling; stratified or PPI estimates.
- The first human-verified cost-per-correct-correspondence figure.
Unlocks: retiring most of the 2,256-span queue by class; a chosen, recorded model; certified per-course defect bounds.

6. STEP 5: RUN THE MANDATE on the chosen configuration, course by course. Each course passes the row check first, then gets per-course sampling after landing.
Needs Adam: reading order (course order vs vocabulary-greedy); the near-duplicate provenance class; the liturgy transfers; the ranking rule.
Money only if calendar binds: usage bundles with a cap.
Unlocks: the 4,073 unique segments, with coverage and $ per new headword tracked each batch.

7. STEP 6: DATA AND BILLING RULINGS, THEN THE BIGGER BETS. Needs the organisation.
- Per-class data ruling, including the past OpenAI and TCS flows, whether agents may query class-C (tantric) text for witnesses, and consent from Geshe Michael and co-translators.
- Consumer vs commercial terms (nonprofit Team verification).
- Whether 'no paid API generation' still stands.
Then, as justified by measurements: single-shot/batch propose; the human-read production tier; a non-agentic cross-family refuter; oral evidence.
Unlocks: lower exposure terms, capacity beyond one seat, independent error checks.

8. STEP 7: FINISH LINE AND SCOPE.
Adam rules on 'done' by certified coverage plus a price cap per new lexical headword. He then rules on scope beyond C18 (TCS, ILL and others hold more uncovered vocabulary than the mandate), and optionally the research-community layer (shared task, university partners) for the wider 42,199-segment spine.


## Projection

Labels: MEASURED (from local transcripts, git or config on 2026-09-23), VENDOR (price or claim), ESTIMATE (inference). Every dollar figure is a list-price equivalent unless marked cash.

TODAY
- Machine cost per segment: $6.65-6.71 [MEASURED; four independent recounts, Opus 5, agents only]. About $8.32 all-in including the orchestrating session [MEASURED, one overnight window, corrected dedup].
  - Split: cache reads 37.5%, cache writes 23.3%, thinking 24.3%, visible output 14.9% [MEASURED].
  - About 5.4M tokens moved and 57.8 round trips per segment [MEASURED].
- Cash per extra segment: about $0 [MEASURED plan: Max 20x, extra usage off; VENDOR $200/month].
- Throughput: 15.8 segments per run-hour at 8 concurrent agents [MEASURED]. About 200 segments before hitting the weekly limit once [ESTIMATE from git timestamps, one observation].
- Weekly allowance: about $2.7-4.3K list-equivalent across all of Adam's use [MEASURED; two windows disagree by about 23%; a separate Fable cap applies].
- Mandate at today's method: 4,073 x $6.69 = about $27.2K list.
  - About 20 weeks if every week looked like 09-17/18.
  - About 60-95 weeks at the Aug-Sep average 10.6% campaign share.
  - About 8-13 weeks only if the campaign got the whole quota [ESTIMATE; assumes quota tracks list-equivalent dollars, which is UNVERIFIED].
- Expert minutes: 0. Human-verified correspondences: 0. Cost per human-verified correct correspondence: UNDEFINED.

RECOMMENDED PATH
- Machine cost per segment after the free levers (lean agent + context pack + span-ID contract + medium proposer effort):
  - about $3.4 on Opus 5, about $2.6 on Opus 5.5 [ESTIMATE]; plausible range $2.0-4.5;
  - the direct-era C05 configuration ran at about $2.1 [MEASURED, earlier era, quality unknown];
  - restoring Fable reconcile adds 12-14% (40% against Opus 5.5).
- Tokens per segment: down about 45-60% [ESTIMATE], so segments per weekly window rise about 1.8-2.5x IF quota scales with tokens [UNVERIFIED].
- Calendar for the mandate [ESTIMATE]:
  - campaign gets about 50% of weekly quota and costs about $3 per segment: about 450-715 segments/week, so about 6-9 campaign weeks, realistically 10-16 calendar weeks once holds, rulings and course-by-course sampling are included;
  - share stays near 10-20%: about 25-45 weeks;
  - usage bundles can buy overflow at 70% of list (e.g. about $2.1 cash per segment at $3 list) [VENDOR bundle pricing; ESTIMATE].
- Expert time for MEASUREMENT (not full verification) [ESTIMATE], about 40-110 expert-hours over 2-4 months:
  - timing pilot 4-8 h;
  - sealed whole-segment yardstick 23-53 h (two readers, d=5-7);
  - row-offset reading 0.5-1.5 h;
  - Adam's rulings about 5-10 h;
  - class sample checks 1-3 h;
  - per-course acceptance sampling 7-34 h.
- Full per-link verification of about 60-77K new d=5 links: 250-960 h at 15-45 s, or 1,100-3,300 h at 1-3 minutes [ESTIMATE; seconds per decision UNMEASURED]. The near-term honest claim is certified per-course rates plus human-verified strata, not every link.
- Cost per human-verified correct correspondence [ESTIMATE, illustrative] = (machine $ per link + human $ per link) / precision.
  - Machine: about $0.41-0.46 per d=5 link today; about $0.16-0.27 after the levers; about $0 cash on the subscription.
  - Human: 30 s at an unverified $40/h is $0.33.
  - Precision is UNMEASURED; 0.81 is only the complement of a Claude-labelled audit.
  - Result: about $0.6-1.0 per human-verified correct link list-equivalent after the levers. With volunteer readers, cash is about $0 and reader-hours become the constraint.
  - On a human-read production route: about $0.16-0.30 per verified link at $40/h, if drafting really saves reader time (contradicted by the only 2025 evidence).

UNCERTAINTY

Four figures are the wide error bars: quota weighting by model and token class, Opus 5.5's thinking volume at high effort, seconds per human decision, and precision. Each is settled by one small measurement named in the roadmap. Until they are measured, no figure above should be quoted as a result.


## Decisions only Adam or the organisation can make

1. WHICH MODEL RUNS THE CAMPAIGN (and rescind or re-rule the lapsed 2026-09-01 policy). Options:
(a) claude-opus-5 at every stage: $6.69 list; continuity with the C05:166+ pages.
(b) The written policy, Opus 5 propose plus Fable 5.1 reconcile: +12-14% list. Fable's separate 50% weekly cap has already bound three times; it refused on safeguards on 09-16; no ZDR on the API.
(c) Opus 5.5 everywhere: about $4.34 list on the same profile [VENDOR price]. Always-on thinking; no Tibetan evidence; launched 09-22.
(d) Opus 5.5 reconcile plus Sonnet 5 proposers: about $3.23 list; general-capability gap (AA 38 vs 51+).
Consequences: the bank already mixes Fable-reconciled (C03, C04, C05:22-147) and Opus-reconciled (C05:166+) pages. Doing nothing lets the alias pick (c) unrecorded.
Recommended: pin (a) now; shadow (c) and (d) on C05:166+; switch only on yardstick evidence.

2. NAME THE HUMAN READERS, AND DECIDE WHOSE DECISIONS COUNT AS 'HUMAN-VERIFIED' (Ruling 2: nothing binding without human review). Options:
- Adam alone: a bottleneck, and not blind on landed segments he has seen.
- In-house ACI-trained bilingual translators: they share Geshe Michael's register, which is good for fidelity, but add one reader from outside the lineage for the overlap.
- Supervised students with gold checks: need adjudication.
- A university partner via a Khyentse grant.
- A paid outside Tibetanist: independent, but pulls toward academic convention.
Consequence: without this ruling the objective metric has no numerator, and every accuracy claim in every cluster stays agreement with Claude. It should head every decision pack until closed.

3. RULE 7 ARTICLE CLASS (pending 19 days; TODO 'RULING NEEDED - the layer-wide article habit', punchlist #61). Options:
(a) A licensor-checked deterministic trim with idiom, quantifier, genitive and pronoun exclusions, plus a 60-item human sample. Re-cuts shipped C01-C03 pages ('ratcheted, not re-cut'). Retires most of about 1,400 spans at near-zero expert minutes. Stops C01's article habit being re-taught through the precedent extract.
(b) Accept the articles as a convention: banks a known over-capture class, a Rule 10 problem.
(c) Item by item: about 25+ expert-hours at an assumed 40 s each.
Recommended: (a) as a dry run now, applied only after the yardstick is sealed.

4. CONVENTION RULINGS: depth, granularity, nulls vs omissions, subword negation. Also layer scope: whether a human-read route may produce d=5-7 only, dropping the d=1-4 layers Adam ruled on for C02. About 80% of cross-family disagreement is convention. Ruling it collapses the queue to about 0.5-1 genuine conflict per segment. Without the ruling, any second production line produces a mixed-convention bank.

5. HOLD C18 AND THE FLAGGED ROWS, AND FIX INTAKE UPSTREAM. Options: accept holding C18:560-760 plus C15:148-165 and C18:1007-1024 until read, or scan them anyway (Rule 10 risk: about 46 mispaired rows at about 15 links each). Related: re-derive C13 upstream to bring its 1,041 segments into the mandate, and rule on E-107, the 1,500-character English cap that truncates 97 mandate segments; truncated tails invite alignment against English that is not there.

6. DATA: REPLACE 'MAY CORPUS TEXT LEAVE' WITH PER-CLASS RULINGS THAT COVER PAST FLOWS.
Facts [MEASURED]:
- Class A1: the C01-C18 English readings are posted publicly, file times from 2009, all 18 in Common Crawl.
- Class B (bank, briefs, precedent) goes to Anthropic on every call and to GitHub on push.
- Class C, including tantric TCS rows, has already reached Anthropic through parallel-witness queries.
- C05:166-378 and project files went to OpenAI via the Codex branch under consumer Pro terms.
- The Tibetan side is ACIP keying under a licence conditional on written permission.
Options per class: which recipients, what terms (consumer, commercial, ZDR), and whether agents may query class-C text. Restricting them removes TCS witnesses that have confirmed errata.
The organisation should also be told the Knowledge Base Portal's bucket is publicly listable (including a SQL sample).

7. BILLING AND CAPACITY. Options:
- Stay on Max 20x and protect quota: cash about $0 per segment; limited by calendar time.
- Usage bundles with a cap: VENDOR, $700 buys $1,000 of list usage, up to $2,000 of value a month.
- Nonprofit Team premium seats: commercial terms and no training by default; allowance against Max 20x unknown.
- Enterprise, API or Batch: metered cash of roughly $5-27K depending on method; commercial terms; batch results stored up to 29 days.
Consequence: moving to metered billing turns list-price 'savings' into new spend, and collides with the standing 'no paid API generation' rule (BACKLOG.md:359). Recommended: measure segments per week per seat first.

8. QUOTA POLICY: how much of the weekly allowance non-campaign work (main sessions, audits, research sweeps like this one) may use. At the historical 10.6% campaign share the mandate takes on the order of 60-95 weeks; at 50% about 10-20. This is the largest speed lever and it is Adam's own usage pattern.

9. READING ORDER AND FINISH LINE. Options: course order, which is the current mandate, vs vocabulary-greedy set-cover (front-loads new headwords). Define 'done' by certified coverage plus a price cap per new lexical headword. Scope beyond C18. The stopping-rule saving (about $5K list at a $5 cap under random order) must be recomputed for whichever order is chosen.

10. WHETHER STATISTICAL OR EMBEDDING MODELS MAY SURFACE CANDIDATES for a person (the LEARN_TAB similarity exception).
The lexical row check needs no ruling: it matches attested strings only.
A deterministic IBM2/HMM aligner used as a raise-only queue ranker, and a MITRA embedder as a second row-pairing witness, would. The condition would be that they order or raise items, never clear, gate or decide, and that the UI says so.

11. CONTEXT-PACK PROVENANCE: whether precedent counts stamped by a deterministic tool with the bank's sha256 may replace the written instruction 'count it yourself' (c05-align.js lines 598-604). A stamped count can be re-derived; an agent's self-count cannot. It removes about 2.4-4.4 round trips per agent.

12. THE CODEX BRANCH AND DISK. Options:
- Never merge the Codex bank.
- Use it as a review stratum (recommended).
- Archive the 83 GB working directory (du; may over-count clones) to the external drive. Disk is at 16 GiB free and blocks local models, audio and OCR work.
Also decide whether to disclose the September OpenAI flow to the organisation in the data ruling.

13. CONSENT AND RIGHTS. Whether to ask Geshe Michael and the co-translators for consent to named-vendor processing under no-training terms. Nine DCC volumes are co-copyrighted, so Geshe Michael's consent alone does not clear them. The request must disclose, not ask prospectively, that tantric TCS material (TCS04Initiation, TCS16KaliWang and others) has already been processed. Record ACIP's written permission for the Tibetan keying.


## Funding and partnerships

1. IN-HOUSE BILINGUAL READERS FIRST: the only money or partnership that moves the objective metric. Ask the ACI-trained translators, the DCC editor and the input centres. Pilot: 3 readers x 30 identical spans, blind and timed (3-8 person-hours). Readers inside the organisation settle most of the data question for adjudication and share Geshe Michael's register. Add one reader from outside the lineage for the overlap.

2. USAGE BUNDLES ON THE EXISTING PLAN, if calendar time binds. [VENDOR, support.claude.com, fetched 2026-09-23] $45 buys $50, $200 buys $250, $700 buys $1,000, up to $2,000 of bundle value a month on Pro/Max. No application and no sales conversation. Turn credits on only with a cap and a governor, because an unattended loop with uncapped credits spends list-price cash.

3. CLAUDE FOR NONPROFITS: complete Goodstack verification for ALL (EIN 83-2826030 per the funding research; about 3 minutes; no purchase). Then send Anthropic sales one written email asking:
(1) Does the nonprofit discount apply to usage, or only to seats?
(2) What is the nonprofit premium-seat price?
(3) What is the premium-seat Claude Code allowance compared with Max 20x?
(4) Is research or academic pricing available?
[VENDOR] Up to 75% off Team/Enterprise; eligibility unverified. Its main value is commercial terms (no training by default), and it is the prerequisite for Claude Corps.

4. CLAUDE CORPS HOST (next cohort February 2027; the host deadline is unpublished, so ask now). [VENDOR] A free full-time fellow for a year, a $10,000 host grant and $2,500 in Claude credits. The fellow would build the reader workstation, ledger, register and sampling tools. The fellow cannot adjudicate Tibetan. It costs ALL about 3 hours a week of senior supervision [ESTIMATE].

5. KHYENTSE FOUNDATION ACADEMIC DEVELOPMENT GRANT through a university partner. The window closes 2026-12-15, the only hard deadline in the next three months. Size the request to adjudication: about 90-226 hours, $5-15K at graduate rates [ESTIMATE]. The grantee is the university. The data agreement must override the foundation's public-domain norm for AI data and forbid derived lexicons.

6. BDRC. ALL is a listed partner and co-released the CC0 ALL-BDRC-alignments dataset. Ask BDRC to add the unaligned high-traffic ACIP masters (S00069M, SE05525M, SL05525N, SE00023M, S00201I, S00021N) to its alignment queue; that gives exact-edition page witnesses for errata. Ask Dharmaduta for a quote only after confirming it has bilingual reviewers; its documented work is matching images to transcriptions. Consider BDRC's Apache/CC-BY OCR models (Yigdzin 1, Woodblock2/PhotiLines_v2) to replace the app's CC BY-NC OCR models, a licence fix for the product rather than campaign work.

7. RESEARCH PARTNERS FOR INDEPENDENT SYSTEMS AND ANNOTATORS, one email each from Adam; no corpus text is attached.
- DharmaBench authors: their classical-Tibetan tasks draw on asianlegacylibrary.org data, and they have expert annotators.
- Dharmamitra (Nehrdich, Keutzer): thesis or GSoC on a Claude-independent aligner.
- UGARIT/Leipzig (Yousef, Palladino): gold-standard alignment design and a double-labelling pipeline.
- LT4HALA/EvaLatin (Sprugnoli, cc Anderson): is a Tibetan-English span-alignment track viable for ALP 2027 or LT4HALA 2028?

8. PEER OUTREACH FOR THE MISSING PARAMETER. Ask Mangalam's team (Lugli), which proofreads 30-50 aligned citations per headword, and 84000, which runs manual post-correction of alignments, for their measured minutes per judgment. Costs nothing, discloses no corpus text, and could bound seconds-per-decision before the pilot runs. Send the queued 84000 licence-clarification email too: GitHub says CC BY-NC-ND 3.0, while the repo's tm_meta says CC BY 4.0.

9. THE ORGANISATION'S OWN BUDGET. The list-price mandate of $17.7-29K is about 1.7-1.9% of ALL's FY2024 revenue. Real cash is probably under $10K over 3-5 months with bundles [ESTIMATE]. Re-derive the figure with the ledger before any budget memo. Ring-fence part of the budget for reader time, which is where a dollar moves the metric.

10. ORGANISATION READINESS, zero cost, before any application. Candid/GuideStar (fetched 2026-09-23) shows ALL US Inc with no program information, no recorded board members, and NTEE code B28, which misdescribes a digital library. Fix these. Decide in advance how to present governance, since funders run reputational due diligence on Geshe Michael's public record.

11. ONE-EMAIL-ONLY ITEMS.
- Anthropic AI for Science: API credits only. The help page says $20K over 6 months, conflicting with $50K elsewhere; the listed fields do not include linguistics. Using it requires API-key authentication.
- Mellon and Schmidt Sciences HAVI: relationship emails, since neither takes unsolicited proposals.
- AWS Imagine Pathfinder: 2027 cycle; first ask whether its credits can buy Anthropic models on Bedrock.
- TypeSafe: a direct question about JEV terms (already filed).

12. DO NOT PURSUE NOW: NEH (subject restriction confirmed); Azure, AWS, Google or OpenAI nonprofit credits (cannot buy the incumbent, and pressure a model switch); Dharmamitra's or Kaggle's leaderboards as measurement venues (MT-only, LLM-judged, and Dharmamitra's own Tibetan test set was 81% near-duplicated in its index).


## What this research did NOT establish

1. ACCURACY. No human-verified correspondence exists. Nothing in this research establishes the precision or recall of the bank or of any alternative. Every agreement figure quoted (93-97% aligner agreement, 57.6% Claude-Codex agreement, 0.84 proposal F1, 19/100 defects) is agreement with Claude or among machines.

2. SECONDS PER HUMAN DECISION, human agreement on this task, and the size and willingness of any reader pool. Every expert-hour figure and every human cash figure ($40/h, $60/h, 15-45 s, 1-3 min) is an assumption.

3. HOW PLAN QUOTA WEIGHS MODELS AND TOKEN CLASSES. It is unknown whether Opus 5.5 or cache reads consume less of the Max 20x allowance per list dollar. The two measured weekly windows disagree by about 23%. The Max 20x weekly allowance is unpublished. So every 'segments per week' projection is an estimate.

4. WHY COST PER SEGMENT TRIPLED between the direct-spawned era and the workflow era on C05. Candidate causes were named but not tested, and it is unknown whether the extra spend bought quality.

5. OPUS 5.5 ON THIS TASK: thinking volume at high effort, round trips, drop and disagreement rates, and whether any c05 batch has already run on it (no c05 run has completed since 2026-09-18). The -35% is list price on Opus 5's token profile, an upper bound.

6. ANY MODEL'S ABILITY ON WYLIE/ACIP CLASSICAL TIBETAN SPAN ALIGNMENT. The newest Tibetan-specific evidence covers Claude 4 Sonnet, Gemini 2.5 and GPT-4o, on Unicode classification and detection tasks (DharmaBench). No evidence exists for Opus 5/5.5, Fable 5.1, Sonnet 5, GPT-6, Gemini 3.x, DeepSeek V4, Kimi K3 or GLM-5.

7. THE EFFECT OF THE LEAN AGENT, CONTEXT PACK, SPAN-ID CONTRACT OR SINGLE-SHOT DESIGN ON JUDGMENTS. All are 'intended neutral'. No post-transport-repair run has completed, so the real token profile after the repair is unknown, as is the single-shot refusal rate. The pipeline's own run-to-run noise floor is unmeasured, and shadow tests on landed segments are contaminated unless the bank is leave-one-out.

8. WHETHER THE C18 AND C15 REGIONS ARE TRUE ROW OFFSETS. That is the verifiers' reading of the Tibetan and English, pending Adam. Also unknown: the detector's recall on offsets under about 6 rows, on ±2 lags, and on course copies not yet checked (C15, P-series and TCS copies of the debate passage).

9. THE 'SUPPLIED ARTICLE' CLASS SIZE. 1,430 is an upper bound from a regex and capitalisation heuristic. A 25-item sample found 4 doubtful trims. The true mechanical-trim share needs the refined dry run and a human sample.

10. DATA FACTS NOT CONFIRMED: the state of Adam's 'Help improve Claude' and ChatGPT training settings; whether any session was rated or reported (which triggers 5-year retention); whether Knowledge Base Portal transcription used the hosted OpenAI Whisper API; the copyright owner of the ACI reading materials, so 'class A1 = published' is itself an inference; Google, xAI and Z.ai batch retention; Moonshot's data location.

11. LEGAL AND TERMS QUESTIONS, not answered: whether using a personal consumer Max plan for an organisation's corpus conforms to Anthropic's consumer terms (the Max article addresses individual consumers and points organisations to Team or Enterprise); the automated-access clause for unattended runners; whether ALL qualifies for Claude for Nonprofits or AI for Science; nonprofit premium-seat price and allowance. This is not legal advice.

12. COVERAGE FORECASTS. They depend on the species definition (surface string vs lexical core; the forecast moves 2.4K-6.4K). They were calibrated on folds anchored on C01, which behaves like a different instrument, and they assume random reading order. Near-duplicate passage clustering in the remainder was not done.

13. WHETHER eflomal BUILDS ON THIS MAC (Apple clang lacks -fopenmp; issue #22 open); its accuracy here, which every local figure measured only as a numpy IBM1 floor; whether botok words beat syllables as the aligner's unit.

14. THE 41-OF-4,782 FIGURE'S DEFINITION. It is not in any tracked file. 13 of 15 cross-family 'drops' were granularity re-segmentation, so the unanimity-drop class may be mostly convention.

15. THE PROPOSE/RECONCILE COST SPLIT FOR ERAS OTHER THAN C05. It was measured on C05 only: about 68% propose and 32% reconcile. C03 and C04 runs are stored outside workflows/ and were not compared like for like.

16. CODEX FLAT-RATE CAPACITY. The about-27x Pro leverage was measured over one promotional week that consumed banked resets. The Pro 20x tier Adam holds, Codex's served model (its logs record only the requested model) and OpenAI Business retention were not verified.

17. EXTERNAL VENUES AND PROGRAMMES were not confirmed: ALP 2027 and LT4HALA 2028 calls, the Claude Corps host deadline, the Khyentse award size, Dharmaduta rates, Tibetan-literate annotator pools (Esukhia, OpenPecha, MonlamAI, RYI), Foradi 2019's scaffolded-non-expert claim, Transcribe Bentham costs.

18. SEVERAL VENDOR AND BENCHMARK FIGURES rest on single fetches or search summaries: Artificial Analysis pages are undated and its cost units unclear; the Opus 5.5 '40% less' and '>30% faster' claims; the Muse Spark and MiMo prices; Gemini Flex at 50%; the OpenAI promptfoo acquisition.

19. PROCESS LIMITS OF THIS SYNTHESIS. The relayed task text arrived truncated partway through the word-alignment cluster. This synthesis was rebuilt from all 27 cluster outputs and their verifications in the local workflow transcripts, read-only. The session's web-search budget (200/200) was exhausted before verification, so verifiers used direct fetches, and some claims stayed unconfirmed. The synthesis itself re-checked only local facts: the HEAD generator mutant, the Codex branch and worktree, 16 GiB free, and the Max 20x plan keys. It did not re-fetch any vendor page. Several key instruments and findings live only in the session scratchpad (/private/tmp/claude-501/-Users-adamderickandrade-ALL-translation-tool/463f1ae3-dff6-4666-a672-f84264aa4eb2/scratchpad/) until banked. This sweep also consumed campaign quota.
