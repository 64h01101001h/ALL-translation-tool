# Gate integrity audit — 2026-09-17

*Twelve gates, one agent each, asked four questions: what does it assert;
**can it fail**; is its threshold at its measurement; what does it not cover.
Every claim at MEDIUM or above then went to a second agent told to REFUTE it.
**67 claims, 38 confirmed, 4 refuted.** The auditors were read-only and the
tree was byte-identical afterwards to a snapshot taken before they started.*

The audit was commissioned because six gates had already been found not doing
their job in a single day, every one by accident: a floor below its own
measured value; a ceiling twenty-five times above it; six gates blind to a
whole course; three suites registered in no build directory; a gate using
`node --check` on a file it silently would not parse; and a checker that read
a clean run as a failure. Six in a day is a pattern, not luck.

## Fixed already

- **`naming_check` could not see the word it forbids.** `\b(his|he|He)\b`
  omitted capital *His*. Five live user-facing strings said it, two of them in
  the pane Adam photographed. Fixed, pattern widened, 23 lines ruled by
  reading and baselined in six groups with reasons.
- **`no_supplied_span_head` could not see a bare function word.** Its regex
  required a trailing space, so `>the</span>` never matched — the purest form
  of the defect it exists to catch. 73 spans were invisible. Fixed; the
  legitimate ones (pronoun rendering a pronoun, `'am` -> "or") licensed by a
  class rule checked against the span's own Tibetan column; seven genuine
  suspects named in `docs/SPAN_HEAD_BARE_REVIEW.md`.

## Confirmed — with dispositions

*Every finding below carries a **Disposition** line saying what happened to it. Twelve of the seventeen are closed and mutation-proved; five are open and say so. A page that says "still open" about work already done is the same decay the audit was commissioned to find.*

### [CRITICAL]  — blind-spot

**Claim.** PRON = r"\b(his|he|He)\b" is case-sensitive and omits His, him, Him, himself, Himself. The gate is therefore blind to the exact phrase Adam's ruling forbids whenever it starts a sentence or fragment. Nine live user-facing strings in the shipped app and two distribution docs say "His English" / "His order" / "His glossary words" / "matched him" with Geshe Michael as the referent, and the gate reports a clean pass over all of them.

**Disposition.** CLOSED 2026-09-17. PRON and IN_STRING widened to the full alternation; 23 lines ruled by reading and baselined in six groups with reasons.

**Proposed fix.** Widen PRON and IN_STRING to \b(his|His|he|He|him|Him|himself|Himself)\b, run the gate, fix the ~11 real UI/doc strings, and --write-baseline the rest with reasons. Do NOT make the regex case-insensitive wholesale: (?i)he would match "He" inside nothing useful but would collide with the existing word-boundary assumptions less predictably than an explicit alternation.

*Mutation-tested: yes*

### [CRITICAL] CMakeLists.txt:607-609) — blind-spot

**Claim.** Six of the 35 bundled components are queried under Homebrew formula names that OSV does not recognise, so each returns zero candidates and is printed as clean — 333 candidate advisories and 8 needs-attention rows the scan has never asked about, including openssl, which is the docstring's own worked example.

**Disposition.** CLOSED 2026-09-17. ALIAS map added (openssl@3->openssl, icu4c@N->icu, ...), applied before querying and pinned in the offline selftest. A first draft of the accompanying "name miss is UNVERIFIED" guard was DESIGNED AND REMOVED instead of shipped: OSV rejects name-only queries with HTTP 400, so it would have flagged every component. A gate that cries on everything gets switched off.

**Proposed fix.** Add a formula->upstream alias map (openssl@3->openssl, icu4c@N->icu, jpeg-turbo->libjpeg-turbo, little-cms2->lcms2, abseil->abseil-cpp, webp->libwebp, ...) applied before querying, and — more durably — treat a zero-candidate result for a component whose name carries an @version suffix or has no OSV presence as UNVERIFIED rather than clean: print it in a "NOT FOUND IN OSV UNDER THIS NAME" block the way unchecked components already are, so a name miss can never read as a clean bill.

*Mutation-tested: NO — reasoning only*

### [CRITICAL] CMakeLists.txt:663, — cannot-fail

**Claim.** The gate cannot see a d=5/d=7 span whose leading function word is the WHOLE element — i.e. `>the</span>` — because its regex requires a literal trailing space after the function word. That bare one-word span is precisely the defect the gate was built for: its own docstring says the 2026-09-04 C03 audit found "eight were exactly this: a one-word article or hinge wrapped at d=5". 78 unallowlisted English spans of this shape exist in the shipped layer today, including 7 in pages_c04, where the gate reports 0 and the rule is supposed to be hard zero with a named licensor.

**Disposition.** CLOSED 2026-09-17. FN's trailing space became (?=[ <]); the three baselines re-based (50->48, 1643->1631, 603->578); the legitimate hits licensed by class rules CHECKED AGAINST THE SPAN'S OWN TIBETAN COLUMN, never inferred from the English; seven genuine suspects named in docs/SPAN_HEAD_BARE_REVIEW.md.

**Proposed fix.** Change FN's trailing-space requirement to a boundary that also accepts the element close, e.g. `(?:the|The|...|you|You)(?=[ <])` or `(?:the|...)(?: |</span>)`. Re-freeze the three baselines in the same commit (they will rise to roughly 52 / 1674 / 641) and triage the 7 new pages_c04 hits into span_head_allow.json with their licensors (kho bos / khyod / 'am / nga) or trim them under rule 7.

*Mutation-tested: yes*

### [CRITICAL] constitution_check.py — cannot-fail

**Claim.** C2's "the press keeps its gates ALIVE" check inspects only the invocation LINE, so every one of the six gates it polices can be fully disarmed by neutering its failure handler while C2 keeps reporting it LIVE. Every real gate in this repo is written as `if ! X; then ...; exit N; fi` or `X || { ...; exit N; }` — the part C2 never reads.

**Disposition.** CLOSED 2026-09-17. C2 now requires the attached shell block to TERMINATE (exit inside the brace group / before the matching fi), and asserts set -euo pipefail with no later set +e.

**Proposed fix.** For each gate token, require BOTH (a) a live invocation, as today, and (b) that the shell block attached to it terminates: for the `X || { ... }` form, require `exit ` inside the brace group; for the `if ! X; then ... fi` form, require `exit ` before the matching `fi`. Additionally assert that package_macos.sh and release.sh still contain `set -euo pipefail` and no later `set +e`. This is the same shape as C2's existing log-filename refinement — a second structural condition, not a new file.

*Mutation-tested: yes*

### [HIGH]  — blind-spot

**Claim.** The gate checks that the evidence contains SOME citation, not that it names the WITNESS. Its docstring and its output line both claim the stronger thing ("the evidence must NAME IT by course:seq" / "all 89 name it"), but 19 of the 89 with-witness entries name no witness at all — 15 of them pass on a boilerplate citation copied verbatim into many entries' evidence.

**Disposition.** CLOSED 2026-09-17. The test is an INTERSECTION now (named & want), not a non-emptiness; CITE widened first so course codes can satisfy it. 108 of 108 with-witness entries name their witness today.

**Proposed fix.** Require intersection, not non-emptiness: flag when `named & {'%s:%d' % o for o in others}` is empty (keeping the current 'names none' message for the empty-named case). Widen CITE first (see the notation finding) or 18 course codes cannot satisfy the tightened test. Re-run and expect ~19 entries to need a witness added — each is a real unchecked-parallel, and each is a one-line evidence edit, exactly like the C16:754 fix already recorded in the register.

*Mutation-tested: yes*

### [HIGH]  — blind-spot

**Claim.** The manifest can NEVER report a clean git tree — the `"clean"` literal in build_manifest.py is unreachable dead code — and the shipped dist/stage/BUILD_MANIFEST.json carries the resulting falsehood right now. The gate makes zero assertions about product.git_commit or product.git_tree.

**Disposition.** CLOSED 2026-09-18, both parts as proposed. (a) probe() takes allow_empty=True, used for `git status --porcelain`, whose EMPTY OUTPUT IS THE ANSWER. (b) The fixture source is a real `git init` repo now, and the gate asserts the sha shape, `clean` on the committed tree, and `DIRTY` after touching a file. Mutation-proved in BOTH directions: reverting probe() fails the clean assertion, and hard-coding "clean" fails the dirty one — the second matters, because without it a gate that always says clean passes the first.

**Proposed fix.** Two parts. (a) Fix the tool: give probe() an `allow_empty=True` mode, or special-case `git status --porcelain` so empty output means clean — e.g. run it directly rather than through probe(). (b) Add gate assertions: build the fixture source as a real `git init` repo, assert product.git_tree == "clean" with no changes, then touch a file and assert it becomes "DIRTY", and assert product.git_commit matches ^[0-9a-f]{40}$.

*Mutation-tested: yes*

### [HIGH]  — blind-spot

**Claim.** The .framework indexing branch — the one that maps Qt's framework binaries to their formula and LGPL licence — is never exercised by the fixture. 16 of the 168 components in the real shipped manifest depend on it, and if it regressed they would all silently become UNMAPPED with no licence recorded.

**Disposition.** STILL OPEN 2026-09-18.

**Proposed fix.** Add a fourth fixture object: a Cellar keg containing `lib/Fake.framework/Versions/A/Fake` (a copy of the seed binary, no extension) and a matching `Contents/Frameworks/Fake.framework/Versions/A/Fake` in the bundle; assert it maps to formula/version/licence like the dylib does. That makes the in_framework branch load-bearing in the battery.

*Mutation-tested: yes*

### [HIGH]  — blind-spot

**Claim.** The wholesale data/ exclusion also excludes data/help/*.md, which is not source material. It is hand-written help prose that app/main.cpp loads into the Help window and shows to the user. Three lines of it refer to Geshe Michael by pronoun, in lowercase, i.e. they would fail the gate on the instant it were allowed to look.

**Disposition.** CLOSED 2026-09-18, and it was BIGGER than this claim said. The audit reported three lines; the real population is 53, because the narrow docs rule (name and pronoun on the SAME line) would have caught 3 of them. data/help/ is flagged on the PRONOUN ALONE, for the digest's reason -- these pages are about Geshe Michael from their first line. 51 lines fixed across four shipped help files; two baselined with reasons, one of them the trap this gate exists for (SOURCES.md:54 "his own words" is DR. BRUNO LAINE, not Geshe Michael).

**Proposed fix.** Narrow the exclusion from data/ to the parts that actually carry source English (data/alignment/, the dictionary, the corpus) and add data/help/**/*.md to the docs scan with the same name+pronoun rule. Then fix the three lines: "Geshe Michael's aligned corpus", "...wherever the passage exists in the corpus of his — of Geshe Michael's — English", "(Geshe Michael's translations beside the Tibetan)".

*Mutation-tested: yes*

### [HIGH] ) — blind-spot

**Claim.** SKIP_KINDS=('LAYER_ARTEFACT',) is an uncapped bypass, and it now exempts 7 rows the exemption was not written for. A stale quote on any of them produces byte-identical passing output.

**Disposition.** CLOSED 2026-09-17. Skipping is by SHAPE now (a `found` carrying the mapping arrow), not by kind, and the skipped population is under a ceiling.

**Proposed fix.** Stop skipping on `kind`. Skip on the shape the rationale actually describes -- a `found` containing the ` -> ` mapping arrow, or better an explicit per-row `spine_checkable: false` flag -- so the 7 plain-text layer rows get checked like everything else. Independently, add the skipped population to CEILING ('ours (describe the layer, not a document)': 16) so growth in the unchecked set has to be a deliberate edit, exactly as not-a-single-segment-citation already is.

*Mutation-tested: yes*

### [HIGH] CMakeLists.txt:607-609) — cannot-fail

**Claim.** classify(), the entire triage classifier, has zero selftest coverage: a mutant that returns "probably-fixed" for every advisory — the precise failure its own docstring forbids — passes the gate green.

**Disposition.** STILL OPEN 2026-09-18.

**Proposed fix.** Add offline classify() fixtures to cmd_selftest using synthetic advisory dicts: (a) fixed=1.6.99 vs ours 1.6.58 -> needs-attention; (b) fixed=1.6.40 vs ours 1.6.58 -> probably-fixed; (c) no fixed event -> undetermined; (d) epoch case fixed="2:1.6.40-8.el10" vs ours 1.6.58 -> probably-fixed (pins bug #1); (e) date case fixed="20180101-5.1" vs ours "2025-11-05" -> probably-fixed (pins bug #3); (f) fixed="1.6.58-r1" vs ours 1.6.58 -> needs-attention (pins the suffix rule). No network needed.

*Mutation-tested: yes*

### [HIGH] CMakeLists.txt:607-609) — cannot-fail

**Claim.** Rule 1 is pinned for query_osv() but not for osv_query_one() — the query function used by --triage-all, the path the docs tell you to re-run after every dependency upgrade. A give-up that returns a clean empty list passes the gate.

**Disposition.** STILL OPEN 2026-09-18.

**Proposed fix.** Extend cmd_selftest with an injectable endpoint for osv_query_one (as query_osv already has) and assert that an unreachable endpoint yields (None, <error>), plus a cmd_triage_all-level assertion that an unchecked component forces a non-zero return.

*Mutation-tested: yes*

### [HIGH] CMakeLists.txt:700) — blind-spot

**Claim.** Section 3's regex requires exactly six hex digits, so it cannot read three-digit CSS shorthand. 144 live card inks written as `color:#777` / `#888` / `#aaa` / `#c80` are below AA on cream right now and are never examined — while the gate prints "all inks clear AA on their own ground". This is the same defect class the gate was written for, wearing a different notation.

**Disposition.** CLOSED 2026-09-17. Both patterns widened to #(?:[0-9A-Fa-f]{3}){1,2}(?![0-9A-Fa-f]) with shorthand expanded before lum(). NOTE ON THE NUMBER: this claim says 144 and I said 150 when I closed it. The AUDITOR WAS RIGHT -- my 150 counted white-on-red plaques the gate handles by context.

**Proposed fix.** Widen both the ink and background patterns to `#([0-9A-Fa-f]{3}){1,2}` with a bounding `(?![0-9A-Fa-f])`, expand shorthand before `lum()` (`if len(h)==3: h=''.join(c*2 for c in h)`), then either migrate the 144 sites (#777->ux::kFaint 4.66, #888->ux::kSoft 4.51, #c80->ux::kWarn) or write the 144 into contrast_baseline.txt so the pass line stops reading as a blanket all-clear.

*Mutation-tested: yes*

### [HIGH] CMakeLists.txt:827) — blind-spot

**Claim.** The gate is blind to all six suites in app/CMakeLists.txt: a literal `add_test(` inside a comment starts a parenthesis-balanced "block" that runs to end of file, so derive() extracts one phantom suite named `counts` and no real app suite. Claim B is therefore never applied to any app suite, and claim A sees app paths only in corrupted form. Introduced TODAY by commit 91cf1eae.

**Disposition.** CLOSED 2026-09-18. Two changes, as proposed. (1) strip_cmake_comments() blanks `#`-to-end-of-line outside quotes before the balanced scan. (2) THE CLASS IS CLOSED, not the instance: derive() now counts the add_test(...NAME) declarations a plain regex sees and REFUSES when the parser walked fewer. Mutation-proved by regressing the strip to a no-op: "app/CMakeLists.txt declares 6 ... and the parser walked 1". Suites derived went 1 -> 128, matching count_checks.

**Proposed fix.** Two changes. (1) Make add_test_blocks() skip comments — strip `#` to end-of-line outside quotes before scanning, or anchor on `add_test(` only at the start of a line / require `add_test(` followed by optional whitespace then NAME, matching the regex app/CMakeLists.txt:14 already uses for the census. (2) Add a claim that closes the class rather than this instance: assert that the number of suites derive() returns equals the number of `add_test(...NAME` declarations counted textually in the same three files (and, where the binary dir is readable, that every ctest-registered suite name appears in derive()'s output). A gate whose parse silently loses a whole directory should say so.

*Mutation-tested: yes*

### [HIGH] CTestTestfile.cmake:209; — cannot-fail

**Claim.** Any tier string other than the literal "auto-aligned" disarms the gate's entire load-bearing arm: the machine-matched entries vanish from the count, so the absolutes check, the "must mention auto-aligned" check and the per-entry provisional check all skip, and the 2026-09-15 falsehood re-ships green.

**Disposition.** CLOSED 2026-09-17. An unknown tier is a FAILURE now, scoped to entry nodes; the positive coupling is asserted rather than one magic string counted.

**Proposed fix.** Assert the positive coupling instead of counting one magic string: (a) fail on any tier value outside {curated, glossary, auto-aligned, ""}; (b) fail when an entry carries English (gloss/glosses/used/aligned_eng non-empty) and its tier is not one of the three known tiers; (c) fail when a non-empty pack reports 0 auto-aligned entries — today's pack has 751, so zero means either a builder regression or a pack that no longer matches its own sentence.

*Mutation-tested: yes*

### [HIGH] constitution_check.py — cannot-fail

**Claim.** The three --static sub-checks (S1 notices-anchor, S2 press-still-calls-the-gate, S3 nothing-unclassified-in-data/extracted) have NO negative fixture. Any one of them can be silently neutered and --selftest — the only thing ctest and constitution_check ever run — stays green at "11 checks passed". The selftest asserts only that cmd_static produces zero failures against the clean live repo; it never asserts that cmd_static CAN produce one.

**Disposition.** STILL OPEN 2026-09-18.

**Proposed fix.** Give cmd_static negative fixtures the way check() gives walk_stage them: build a throwaway root in tempfile (manifest + notices + a stub package_macos.sh + a data/extracted), plant one unclassified file / one bogus anchor / one deleted --press call, and assert each produces the matching finding. Bump CHECKS accordingly.

*Mutation-tested: yes*

### [HIGH] constitution_check.py — cannot-fail

**Claim.** Deleting tools/release.sh outright makes all three release-ritual rules (sanitizer, mutation sweep, press-before-tag) pass silently. This is precisely the defect STATIC-3/L3 was fixed for — 'deleting the very file L3 exists to police left the gate printing all rules hold with exit 0' — repeated ~30 lines below the shell_verdict code that closed it.

**Disposition.** CLOSED 2026-09-18. The `rel_sh and` conjunct is gone: a missing tools/release.sh now FAILS all three, exactly as G2/G3 do for a missing baseline. Mutation-proved -- moving the file aside produces three C2 failures where it produced silence.

**Proposed fix.** Drop the `rel_sh and` conjunct and let a missing tools/release.sh be a FAIL, exactly as G2/G3 do for a missing baseline file ('the gate no longer installs it itself, GATE-4') and as L2/L3 do for a missing subprocess.

*Mutation-tested: yes*

### [HIGH] constitution_check.py — cannot-fail

**Claim.** R3 still auto-installs its baseline when tools/constitution_baseline.txt is absent: the gate WRITES the tracked file at whatever the current count is and exits 0. That is two faults the file itself documents as GATE-4 — the ratchet resets with nobody reviewing it, and a check run leaves `git status` dirty, which package_macos.sh REL-1 reads and answers by renaming the artifact `-dirty`. GATE-4 was fixed for G2 and for G3; R3 is the one left behind.

**Disposition.** CLOSED 2026-09-18. The auto-install branch is a FAIL with G2's wording. Mutation-proved: hiding tools/constitution_baseline.txt fails the gate AND the gate does not recreate the file, so a check run can no longer leave `git status` dirty and get a press renamed -dirty.

**Proposed fix.** Replace the auto-install branch with the G2 wording: FAIL when tools/constitution_baseline.txt is absent. Installing a first baseline becomes a person's one-line commit, as it already is for the other two.

*Mutation-tested: yes*

## Counts

- CRITICAL: 4 claims — **4 closed**, 0 open
- HIGH: 13 claims — **9 closed**, 4 open

The four still open, so they can be found without reading the page:
the untested .framework
indexing branch; vuln_check's untested `classify()`; Rule 1 unpinned for
`osv_query_one()`; and constitution_check's three `--static` sub-checks with
no negative fixture. All four are *cannot-fail* or *blind-spot*, none is a
wrong answer being produced today.
