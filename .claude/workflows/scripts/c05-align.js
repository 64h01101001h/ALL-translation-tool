export const meta = {
  name: 'c05-align',
  description: 'C05 full-depth alignment: two angles per segment, reconcile, generator must exit 0',
  phases: [
    { title: 'Prefetch', detail: 'one dumper run when the launch did not supply the rows', model: 'opus' },
    { title: 'Propose', detail: 'two independent analysts per segment', model: 'opus' },
    { title: 'Reconcile', detail: 'merge into one spec the generator accepts', model: 'opus' },
  ],
}

// ---------------------------------------------------------------------------
// TRANSPORT. A transport audit of this campaign (2026-09-20) found that 98.1%
// of everything moved per segment was input-side, and that the agents were
// spending it fetching things the harness already knew: 2,644 sqlite
// round-trips for a row of a few hundred bytes, 566 Read calls across 567
// agents for the same two brief files, 1,866 greps of the alignment bank
// (data/alignment/alignment_full_v1.json, about ten megabytes and growing
// with every page, so its size is measured, never quoted). THOSE
// FIGURES CAME FROM SESSION TRANSCRIPTS THAT ARE NOT IN THIS REPOSITORY AND
// CANNOT BE RE-DERIVED FROM IT. They are quoted as the reason this script was
// rewritten, never as something this tree can prove. What this tree CAN prove
// is what the harness sends: tools/workflow_prompt_size.py runs this script
// against stubs and measures every prompt it builds, on demand, today.
//
// This script now HANDS the agents those three things. Nothing about what an
// agent is asked to JUDGE has changed -- the briefs below are byte-for-byte
// the files, the ten rules are untouched, and the generator still has to
// exit 0.
//
// THE GUARD THAT WAS RETIRED, AND EXACTLY HOW FAR ITS REPLACEMENT REACHES.
// The old line said "GET THE SOURCE TEXT YOURSELF, never from this prompt",
// and it was written to stop a MODEL retyping Tibetan. Two things stand in its
// place, and neither is a total defence:
//   * tools/gen_alignment_page.py opens the spine itself and refuses any span,
//     Tibetan or English, that is not verbatim in the spine's own text. So a
//     bad copy in this prompt CANNOT BANK A FALSE SPAN. It can only get a span
//     refused, by name, loudly.
//   * the pre-flight below checksums the very text inlined here and compares
//     THAT against the live spine row (--verify-inlined), so a bad copy is
//     named before the generator ever runs.
// What is still open, and is stated rather than papered over: a segment's
// `note` and `title` are free prose that nothing compares to the spine, so a
// false QUOTATION in an AI NOTE can be banked; and an alignment judged against
// the wrong sentence can produce spans that are each present in the spine and
// still wrongly matched. The generator sees presence, not correctness.
// ---------------------------------------------------------------------------

// args: { seqs?: [172,173,...],
//         segments?: { "172": {wylie, english, acip, sha:{wylie,english,acip}} },
//         precedent?: { "172": "extract text" | {text, provenance} },
//         note?: "page-specific known facts", scratch?: "/path" }
//
// The `sha` the dumper writes is the DUMP's provenance and is no longer what
// the pre-flight checks: hashes that travel beside a text prove nothing about
// the text, and a corrupted args blob with its hashes left alone used to print
// "spine ok". The pre-flight now checksums the string this script actually
// inlines (fnv1a/digestToken below) and has the dumper recompute it from the
// spine, so the check is bound to the copy the agent reads.
//
// Produce the whole object with one command:
//
//   tools/dump_spine_segments.py --course C05 --seqs 172-174 --precedent
//
// which prints segments (from the spine) and precedent (from
// data/alignment/precedent_index_v1.json) together. One process, no model
// between the database and the JSON. Drop --precedent and every agent keeps
// the brief's count-it-yourself instruction, which is worse but honest.

const ROOT = '/Users/adamderickandrade/ALL-translation-tool'
const SPINE = ROOT + '/build/hgm_spine_v27_2.db'
const COURSE = 'C05'

const SEG = (args && args.segments) || {}
// Discover the batch from whichever of the two the launch supplied, so a blob
// pasted straight from the dumper needs no second list kept in step with it.
const SEQS = (args && args.seqs && args.seqs.length)
  ? args.seqs
  : Object.keys(SEG).map(Number).sort((a, b) => a - b)
const EXTRA = (args && args.note) || ''
const PRECEDENT = (args && args.precedent) || {}
if (!SEQS.length) throw new Error('pass args.seqs or args.segments, e.g. {"seqs":[172,173,174]}')

// args.scratch, because the first copy of this script hardcoded ONE session's
// scratch directory and so could only ever run in the session that wrote it.
// Pass the session scratchpad; the fallback is repo-local and gitignored.
const SCRATCH = (args && args.scratch) || (ROOT + '/build/align_scratch/c05')

// ---------------------------------------------------------------------------
// The briefs, inlined byte-for-byte from docs/alignment_briefs/. Generated by
// tools/sync_workflow_briefs.py --write; DO NOT HAND-EDIT either literal.
//
// TWO gates stand over them, because the first one alone had a hole a two-line
// edit could walk through:
//   workflow_briefs_in_sync    — each literal is compared byte-for-byte with
//     its file, AND the census of (script, brief) pairs is frozen in
//     tools/workflow_briefs_baseline.txt, which lives OUTSIDE this file.
//     Coverage used to be discovered only from the `// >>> INLINED` markers
//     below, so deleting the two marker comments deleted the check and the
//     gate still printed success.
//   workflow_briefs_delivered  — runs this script against stubs, reads the
//     BRIEF FILES themselves, and counts the rendered prompts that carry each
//     one in full against a frozen count. A faithful copy that never reaches
//     the prompt is the same thing as no brief at all: removing PROPOSE_BRIEF
//     from proposeCommon() left both literals byte-perfect, halved the propose
//     prompt, and no gate in the tree noticed.
// ---------------------------------------------------------------------------

// >>> INLINED docs/alignment_briefs/PROPOSE_BRIEF.md
const PROPOSE_BRIEF = "# MODEL POLICY (Adam, 2026-09-01): PROPOSE agents run on Claude Opus 5 (`claude-opus-5`).\n# RECONCILE and REFUTE agents run on Claude Fable 5.1 (`claude-fable-5-1`), high effort.\n# Rationale in data/alignment/C03_CAMPAIGN.md (2026-09-01 entry): the judgment steps decide\n# whether a page lands and whether a claim leaves the building; the proposals are the\n# cheaper half and are checked twice downstream.\n\n# PROPOSE BRIEF \u2014 Diamond Cutter Translation Tool, ACI course full-depth alignment (the COURSE \u2014 C03, C04, \u2026 \u2014 is named in your task)\n\nYou are ONE OF TWO independent analysts for one segment; your task names the ANGLE you take.\nA reconciler will merge your proposal with the other analyst's, so be complete, be honest, and\nbe checkable: every span you claim will be re-verified against the source by someone told to\ndoubt it. Write everything to YOUR private directory (named in your task). Touch nothing else.\n\nGet the verbatim source text yourself:\n  sqlite3 /Users/adamderickandrade/ALL-translation-tool/build/hgm_spine_v27_2.db \\\n    \"select wylie, english, acip from corpus_segments where course='<COURSE>' and seq=<SEQ>\"\n\nCONTEXT. Read the \"Course shape\" section of data/alignment/<COURSE>_CAMPAIGN.md for the\ncourse you are on (C03: Applied Meditation, prose with embedded verse citations; C04: The\nProof of Future Lives, Buddhist logic \u2014 debate-format prose, `chos can`/`thal`/`phyir`\nsyllogisms, dense citation apparatus). Verse lines carry initial capitals mid-sentence,\nwhich is lineation, not apparatus. If the segment's English is GMR's PHONETICS rather\nthan translation, follow the PHONETICS PROTOCOL in C03_CAMPAIGN.md (d=7 links only, the\nphonetics label, the sound-to-spelling note). Prose of this length takes 25-60 spans. Cover the content words; null or omit\nparticles freely \u2014 an omitted particle costs nothing, a mis-depthed one costs the page.\n\nTHE RULES, inviolable. The generator checks each and REFUSES the page.\n\n1. VERBATIM. Every \"tib\" a contiguous substring of the wylie; every \"eng\" a\n   contiguous substring of the English. Preserved byte-for-byte INCLUDING\n   ERRORS - never silently correct one while quoting it.\n2. TIBETAN ORDER, enforced by a FORWARD-ONLY CURSOR over every span of\n   depth 1-6. Once a span is matched the next is searched for AFTER it.\n3. DEPTH:\n     d=1 sentence \u00b7 d=2 clause \u00b7 d=3 phrase \u00b7 d=4 embedded phrase\n     d=5  WORD or COMPOUND \u2014 the dictionary layer, where the value is\n     d=6  STANDALONE particle, which STARTS A SYLLABLE in the wylie\n     d=7  BOUND morpheme or COMPOUND MEMBER. **A d=7 SPAN MUST BE**\n          **IMMEDIATELY PRECEDED BY THE d=5 SPAN IT LIVES INSIDE.** It is\n          resolved within that parent's character range and does NOT\n          advance the flat cursor. A d=7 hung off a d=3 clause is refused:\n          a clause's parts are words (d=5), not compound-members.\n   POSITION, NOT CONTAINMENT, IS THE TEST, and the position that matters is\n   where the CURSOR will be. If a word sits inside a span you have already\n   listed at d<=6, it is a MEMBER of that span or it is left out - never a\n   separate later top-level entry, because the cursor has already passed it.\n   PREFER THE FLAT SHAPE: d=5 words and d=6 particles at top level in wylie\n   order, d=7 members inside their compounds, and NO clause layer unless the\n   clause is genuinely the thing being aligned. Every page in this campaign\n   that passes first time uses the flat shape.\n4. A MEMBER MUST BE UNIQUE INSIDE ITS PARENT, strictly TIGHTER than it, and\n   its ENGLISH MUST LIE INSIDE THE PARENT'S ENGLISH. A member whose English\n   is elsewhere in the sentence is not a member - null it. If you tighten a\n   parent, RE-CHECK every member: tightening often makes one degenerate.\n5. NO INVENTED ENGLISH. A morpheme with no exponent gets \"eng\": null.\n6. AMBIGUOUS ENGLISH: equal counts resolve, unequal do not. Count the string\n   you would ACTUALLY claim, not a shorter substring of it. When the English\n   spans cross the Tibetan order (they usually do), supply \"eng_order\": the\n   span ids in ENGLISH order. Without it the generator refuses any page whose\n   English side is out of order or ambiguous.\n7. NO OVER-CAPTURE - the dominant measured defect. Leftover English stays\n   UNWRAPPED. An article or a hinge GMR supplied has no Tibetan under it.\n8. NO BROKEN WORDS. The one exception is the negative-affix convention,\n   where the Tibetan negation owns the English negative - four confirmed\n   forms: \"n't\" (doesn't), \"un\" (unable), \"im\" (impossible), \"less\"\n   (effortlessly). Set \"subword\": true when you use it.\n9. AGENTIVE -S IS NOT SEPARABLE. Span \"gis\", \"kyis\", \"pas\", \"'dis\" whole.\n10. NEVER BANK A FALSEHOOD, EVEN A VERBATIM ONE. Rule 10 outranks coverage.\n    Losing a mapping is cheap; banking one you cannot point at is not. A\n    span that PASSES every mechanical check can still be false.\n\nSPEC FORMAT (what gen_alignment_page.py reads on stdin):\n  { \"course\": \"<COURSE>\",\n    \"segments\": [\n      { \"seq\": <SEQ>,\n        \"title\": \"<short lowercase title, a few words>\",\n        \"spans\": [ {\"id\":\"w1\",\"d\":5,\"tib\":\"zhi gnas\",\"eng\":\"quietude\"},\n                   {\"id\":\"m1\",\"d\":7,\"tib\":\"gnas\",\"eng\":\"quiet\"},\n                   {\"id\":\"p1\",\"d\":6,\"tib\":\"kyis\",\"eng\":null,\"cls\":\"case\",\"nul\":\"instrumental; unrendered\"},\n                   {\"id\":\"w2\",\"d\":5,\"tib\":\"ma\",\"eng\":\"n't\",\"subword\":true} ],\n        \"eng_order\": [\"w2\",\"w1\"],\n        \"note\": \"AI NOTE body, may contain HTML\" } ] }\n  Keep the proposals' span-object shape: id, d, tib, eng, and optionally cls, nul, subword.\n  Do NOT put the top-level \"generator\"/\"errata\" keys inside the spec segment object.\n\nPRE-FLIGHT \u2014 MANDATORY. Write the spec to <YOUR_DIR>/spec.json and run the REAL generator:\n  cd /Users/adamderickandrade/ALL-translation-tool\n  python3 tools/gen_alignment_page.py < <YOUR_DIR>/spec.json > <YOUR_DIR>/body.html; echo \"EXIT=$?\"\nIf it prints a REFUSED line, FIX THE SPEC AND RUN IT AGAIN until it exits 0. Do not return a\nspec that has not passed. If after genuine effort you cannot make a shape pass without banking\nsomething false, say so in report.md \u2014 an honest failure beats a passing lie. Never leave a\nspec.json in your directory that does not exit 0.\n\nERRATA DUTY.\n - Quote from THIS segment, verbatim (wylie, english or acip \u2014 ACIP is the source of record).\n   Evidence about other segments goes in the evidence field, cited by course:seq.\n - CHECK FOR A PARALLEL WITNESS BEFORE FILING. The same passage often appears in C16, P1 or a\n   TCS text (select course,seq from corpus_segments where english like '%...%'). An identical\n   string there is the PUBLISHED READING, not a corruption here. Say what you found, even when\n   you found nothing. P1 is largely the same text re-ingested and is NOT an independent witness.\n - Do NOT re-file these registered classes: the {SHl} converter drop (\"{SHl}OO KA\" -> \"oo ka\");\n   footnote numbers fused to a preceding period; folio markers spliced into caption numbers; a\n   following section's heading glued onto the end of the English; a row whose wylie field holds\n   an English running head (E-071); a field that ends at exactly 1,500 characters (E-107, our\n   own ingest cap \u2014 note it in report.md, do not file it).\n - An empty list is a perfectly good answer and the most common one.\n Write the records to <YOUR_DIR>/errata.json as a JSON list (keys: seq, kind\n [TIBETAN_SPELLING|ENGLISH_TYPO|ENGLISH_FACTUAL_ERROR|INGEST_ARTEFACT|FORMATTING], found,\n expected, evidence, severity [HIGH|MEDIUM|LOW], confidence [CONFIRMED|PROBABLE|UNCERTAIN]).\n Write it even when empty.\n\nDELIVERABLES in <YOUR_DIR>:  spec.json (exit 0 proven), body.html, errata.json, report.md\nreport.md: 6-15 lines \u2014 the spans you were least sure of and why (the reconciler reads this\nfirst), any span you left out on rule 10 grounds, the FINAL generator output verbatim (EXIT line\nand body byte count), and what you checked for errata.\nYour final chat reply: the seq, your angle, EXIT code, span count, errata count, and one\nsentence on the span you would challenge first. Nothing else.\n";
// <<< INLINED docs/alignment_briefs/PROPOSE_BRIEF.md

// >>> INLINED docs/alignment_briefs/RECONCILE_BRIEF.md
const RECONCILE_BRIEF = "# MODEL POLICY (Adam, 2026-09-01): PROPOSE agents run on Claude Opus 5 (`claude-opus-5`).\n# RECONCILE and REFUTE agents run on Claude Fable 5.1 (`claude-fable-5-1`), high effort.\n# Rationale in data/alignment/C03_CAMPAIGN.md (2026-09-01 entry): the judgment steps decide\n# whether a page lands and whether a claim leaves the building; the proposals are the\n# cheaper half and are checked twice downstream.\n\n# RECONCILE BRIEF \u2014 Diamond Cutter Translation Tool, ACI course full-depth alignment (the COURSE \u2014 C03, C04, \u2026 \u2014 is named in your task)\n\nYou are the RECONCILER for one segment. Two (sometimes three) analysts independently\naligned it; their proposals are in the JSON file named in your task, under \"proposals\".\nMerge them into ONE spec, prove it with the real generator, and write the results to\nYOUR private directory (named in your task). Nothing else on disk may be touched.\n\nGet the verbatim source text yourself (do not trust the proposals' quotes):\n  sqlite3 /Users/adamderickandrade/ALL-translation-tool/build/hgm_spine_v27_2.db \\\n    \"select wylie, english, acip from corpus_segments where course='<COURSE>' and seq=<SEQ>\"\n\nCONTEXT. Read the \"Course shape\" section of data/alignment/<COURSE>_CAMPAIGN.md for the\ncourse you are on (C03: Applied Meditation, prose with embedded verse citations; C04: The\nProof of Future Lives, Buddhist logic \u2014 debate-format prose, `chos can`/`thal`/`phyir`\nsyllogisms, dense citation apparatus). Verse lines carry initial capitals mid-sentence,\nwhich is lineation, not apparatus. If the segment's English is GMR's PHONETICS rather\nthan translation, follow the PHONETICS PROTOCOL in C03_CAMPAIGN.md (d=7 links only, the\nphonetics label, the sound-to-spelling note).\n\nTHE RULES, inviolable. The generator checks each and REFUSES the page.\n\n1. VERBATIM. Every \"tib\" a contiguous substring of the wylie; every \"eng\" a\n   contiguous substring of the English. Preserved byte-for-byte INCLUDING\n   ERRORS - never silently correct one while quoting it.\n2. TIBETAN ORDER, enforced by a FORWARD-ONLY CURSOR over every span of\n   depth 1-6. Once a span is matched the next is searched for AFTER it.\n3. DEPTH:\n     d=1 sentence \u00b7 d=2 clause \u00b7 d=3 phrase \u00b7 d=4 embedded phrase\n     d=5  WORD or COMPOUND \u2014 the dictionary layer, where the value is\n     d=6  STANDALONE particle, which STARTS A SYLLABLE in the wylie\n     d=7  BOUND morpheme or COMPOUND MEMBER. **A d=7 SPAN MUST BE**\n          **IMMEDIATELY PRECEDED BY THE d=5 SPAN IT LIVES INSIDE.** It is\n          resolved within that parent's character range and does NOT\n          advance the flat cursor. A d=7 hung off a d=3 clause is refused:\n          a clause's parts are words (d=5), not compound-members.\n   POSITION, NOT CONTAINMENT, IS THE TEST, and the position that matters is\n   where the CURSOR will be. If a word sits inside a span you have already\n   listed at d<=6, it is a MEMBER of that span or it is left out - never a\n   separate later top-level entry, because the cursor has already passed it.\n   PREFER THE FLAT SHAPE: d=5 words and d=6 particles at top level in wylie\n   order, d=7 members inside their compounds, and NO clause layer unless the\n   clause is genuinely the thing being aligned. Every page in this campaign\n   that passes first time uses the flat shape.\n4. A MEMBER MUST BE UNIQUE INSIDE ITS PARENT, strictly TIGHTER than it, and\n   its ENGLISH MUST LIE INSIDE THE PARENT'S ENGLISH. A member whose English\n   is elsewhere in the sentence is not a member - null it. If you tighten a\n   parent, RE-CHECK every member: tightening often makes one degenerate.\n5. NO INVENTED ENGLISH. A morpheme with no exponent gets \"eng\": null.\n6. AMBIGUOUS ENGLISH: equal counts resolve, unequal do not. Count the string\n   you would ACTUALLY claim, not a shorter substring of it. When the English\n   spans cross the Tibetan order (they usually do), supply \"eng_order\": the\n   span ids in ENGLISH order. Without it the generator refuses any page whose\n   English side is out of order or ambiguous.\n7. NO OVER-CAPTURE - the dominant measured defect. Leftover English stays\n   UNWRAPPED. An article or a hinge GMR supplied has no Tibetan under it.\n   HARD CHECK (2026-09-04 audit, 8 of 19 confirmed defects were this): before\n   you write any d=5 or d=7 \"eng\" that BEGINS with the/a/an/and/or/his/our/\n   your/I/you, name the Tibetan word under that first English word (a\n   demonstrative de/'di, a possessive rang gi/bdag gi, a connective dang/\n   zhing/cing, a genitive). If you cannot name it, the span starts one word\n   later. If you can, record it: the gate `no_supplied_span_head` refuses\n   any such span in a new course directory unless it is listed in\n   data/alignment/span_head_allow.json with that licensor.\n8. NO BROKEN WORDS. The one exception is the negative-affix convention,\n   where the Tibetan negation owns the English negative - four confirmed\n   forms: \"n't\" (doesn't), \"un\" (unable), \"im\" (impossible), \"less\"\n   (effortlessly). Set \"subword\": true when you use it.\n9. AGENTIVE -S IS NOT SEPARABLE. Span \"gis\", \"kyis\", \"pas\", \"'dis\" whole.\n10. NEVER BANK A FALSEHOOD, EVEN A VERBATIM ONE. Rule 10 outranks coverage.\n    Losing a mapping is cheap; banking one you cannot point at is not. A\n    span that PASSES every mechanical check can still be false.\n\nRECONCILIATION.\n - Agreement stands. On a BOUNDARY disagreement take the TIGHTER span, then\n   RE-CHECK its members \u2014 tightening often makes a member degenerate or pushes\n   its English outside the parent.\n - Where one nulls a morpheme and the other invents English for it, take the\n   null. Where one banks a member with an honest English and the other omits\n   it, take the member.\n - MINT A d=5 ONLY WHERE IT IS A REAL DICTIONARY UNIT, and a d=7 member only\n   where the English is honestly that morpheme's. A span can pass every\n   mechanical check and still be false: \"ma sogs bar\" -> \"without any\n   involvement\" passes, and banks sogs (gather/accumulate) as \"involvement\",\n   which is a lie. Reject those even when they pass.\n - Every proposal's self-reported generator result is UNPROVEN. In the last\n   batch two proposals said ACCEPTED and both exited 1 when re-run. Re-derive.\n\nSPEC FORMAT (what gen_alignment_page.py reads on stdin):\n  { \"course\": \"<COURSE>\",\n    \"segments\": [\n      { \"seq\": <SEQ>,\n        \"title\": \"<short lowercase title, a few words>\",\n        \"spans\": [ {\"id\":\"w1\",\"d\":5,\"tib\":\"zhi gnas\",\"eng\":\"quietude\"},\n                   {\"id\":\"m1\",\"d\":7,\"tib\":\"gnas\",\"eng\":\"quiet\"},\n                   {\"id\":\"p1\",\"d\":6,\"tib\":\"kyis\",\"eng\":null,\"cls\":\"case\",\"nul\":\"instrumental; unrendered\"},\n                   {\"id\":\"w2\",\"d\":5,\"tib\":\"ma\",\"eng\":\"n't\",\"subword\":true} ],\n        \"eng_order\": [\"w2\",\"w1\"],\n        \"note\": \"AI NOTE body, may contain HTML\" } ] }\n  Keep the proposals' span-object shape: id, d, tib, eng, and optionally cls, nul, subword.\n  Do NOT put the top-level \"generator\"/\"errata\" keys inside the spec segment object.\n\nPRE-FLIGHT \u2014 MANDATORY. Write the spec to <YOUR_DIR>/spec.json and run the REAL generator:\n  cd /Users/adamderickandrade/ALL-translation-tool\n  python3 tools/gen_alignment_page.py < <YOUR_DIR>/spec.json > <YOUR_DIR>/body.html; echo \"EXIT=$?\"\nIf it prints a REFUSED line, FIX THE SPEC AND RUN IT AGAIN. Keep going until it exits 0.\nIf after genuine effort you cannot make a shape pass without banking something false,\nsay so in report.md \u2014 an honest failure beats a spec that passes by asserting something untrue.\nNever leave a spec.json in your directory that does not exit 0; delete or rename it.\n\nERRATA \u2014 you are the gate. The proposals may carry \"errata\" records. Keep only what you\ncan confirm from THIS segment's own text (wylie, english, or acip \u2014 ACIP is the source\nof record). Drop anything quoting a different segment, and anything in these REGISTERED\nclasses: the {SHl} converter drop (\"{SHl}OO KA\" -> \"oo ka\"); footnote numbers fused to\na preceding period; folio markers spliced into caption numbers; a following section's\nheading glued onto the end of the English. CHECK FOR A PARALLEL WITNESS: the same passage\noften appears in C16, P1 or a TCS text (select course,seq from corpus_segments where\nenglish like '%...%' / wylie like '%...%'). An identical reading there is the PUBLISHED\nreading, not a corruption here. Say what you checked even when you found nothing.\nWrite the surviving records to <YOUR_DIR>/errata.json as a JSON list, each record with\nkeys: seq, kind (TIBETAN_SPELLING|ENGLISH_TYPO|ENGLISH_FACTUAL_ERROR|INGEST_ARTEFACT|FORMATTING),\nfound, expected, evidence, severity (HIGH|MEDIUM|LOW), confidence (CONFIRMED|PROBABLE|UNCERTAIN).\n\n**`found` MUST BE A VERBATIM SUBSTRING of that segment's own wylie, ACIP or\nEnglish \u2014 nothing else.** Not a description of the defect, not a quote with a\nparenthetical gloss appended, not text from a neighbouring row. Put every word\nof explanation in `evidence`, which is free prose and is where it belongs.\n\n`tools/merge_errata.py` refuses anything else, and it is right to: a register\nwhose quotes cannot be found in the documents they cite is worth nothing to\nthe people who will act on it. It refused FOUR entries on 2026-09-17 alone,\nevery one of them a real finding wrapped in prose \u2014\n\n    REFUSED  quoted text \"NUS PA'I SKAD CIG PAR MED PA ... (wy\" is not in\n             the spine at this segment \u2014 misquoted or mis-cited\n\n\u2014 and each had to be re-filed by hand. The finding survives that; the time\ndoes not. Write the quote as the document has it, and say why underneath.\nAn empty list is the normal outcome. Write it even when empty.\n\nDELIVERABLES in <YOUR_DIR>:  spec.json (exit 0 proven), body.html, errata.json, report.md\nreport.md: 10-25 lines \u2014 what the proposals disagreed on and how you ruled, any span you\nrejected despite it passing (and why), the FINAL generator output verbatim (EXIT line and\nbody byte count), and what you checked for errata. If only one analysis was usable, say so\nand be correspondingly conservative.\nYour final chat reply: the seq, EXIT code, span count, errata count, and one or two\nsentences on the hardest ruling. Nothing else \u2014 everything else goes in report.md.\n";
// <<< INLINED docs/alignment_briefs/RECONCILE_BRIEF.md

// ---------------------------------------------------------------------------
// The supplement. The briefs tell an agent to go and fetch three things; the
// harness has now fetched them. Rather than edit the briefs -- they are the
// authority and are inlined verbatim -- this says, once, where each of those
// three instructions now points. Nothing else in a brief is touched.
// ---------------------------------------------------------------------------

const SUPPLEMENT = [
  '=============================================================',
  'HARNESS SUPPLEMENT — read once. It resolves the three places where the',
  'brief above tells you to go and fetch something. Everything else in the',
  'brief stands exactly as written.',
  '',
  ' 1. THE SOURCE TEXT. The brief says to run sqlite3 yourself. The harness',
  '    already did, with one run of tools/dump_spine_segments.py against',
  '    ' + SPINE + ',',
  '    and the row is inlined below. Align against it.',
  '    THE INLINED TEXT IS A CONVENIENCE. THE SPINE IS THE SOURCE OF RECORD.',
  '    TWO THINGS STAND BEHIND THE COPY, AND HERE IS EXACTLY HOW FAR THEY',
  '    REACH, because an overstated guarantee is worse than none:',
  '      - the PRE-FLIGHT below checksums the inlined text itself and has the',
  '        spine recompute it, so a bad copy is named the first time you run',
  '        that command — before the generator produces anything;',
  '      - tools/gen_alignment_page.py opens the spine itself and refuses any',
  '        span, Tibetan or English, that is not verbatim in the spine text,',
  '        so a bad copy CANNOT BANK A FALSE SPAN — it can only be refused, by',
  '        name.',
  '    WHAT IS NOT COVERED, AND IS YOURS TO GET RIGHT: your "note" and your',
  '    "title" are free prose and NOTHING compares them to the spine. A',
  '    sentence you quote in an AI NOTE is banked as you typed it. Quote only',
  '    from text the pre-flight has passed, and never from memory. An',
  '    alignment judged against a wrong sentence can also produce spans that',
  '    are each present in the spine and still wrongly matched: the generator',
  '    sees presence, not correctness.',
  '    If the generator says a span is NOT PRESENT and you are certain it is a',
  '    correct substring of the text below, then the text below is wrong:',
  '    re-fetch the row with the brief\'s own command, align against that, and',
  '    say so in your report. That is the one case where you fetch it',
  '    yourself.',
  '',
  ' 2. THE BRIEF. It is inlined above, byte-for-byte from the file in',
  '    docs/alignment_briefs/. Do not open the file: two build gates prove it',
  '    on every build — workflow_briefs_in_sync (the inlined copy is the file,',
  '    byte for byte, and the census of inlined briefs cannot shrink) and',
  '    workflow_briefs_delivered (the file\'s text really does reach this',
  '    prompt, in full, in the number of prompts frozen at the measurement).',
  '',
  ' 3. PRECEDENT. Where the brief or your task says to count precedent in',
  '    data/alignment/alignment_full_v1.json, read the PRECEDENT EXTRACT block',
  '    below first — it tells you where to look. It does NOT stand in for the',
  '    count: re-derive against the file any count you actually rely on, and',
  '    say what you counted. If that block says NOT SUPPLIED, do the whole',
  '    thing yourself, exactly as the brief says.',
  '',
  'The MODEL POLICY header at the top of the brief records the campaign\'s',
  'intent. The model you are actually running on is set by the harness and is',
  'not yours to change; do not spend a step on it.',
  '',
  'Write nothing outside the directory your task names.',
  '=============================================================',
].join('\n')

const COURSE_CONTEXT = [
  'COURSE. You are working on the ACI Course 5 full-depth alignment campaign in',
  ROOT + '. Course 5 is "How Karma Works". Segments 24-511 are Vasubandhu\'s',
  'Treasure House of Knowledge (Abhidharmakosha) chapter IV on karma, with the',
  'First Dalai Lama Gendun Drup\'s commentary. The campaign resumed 2026-09-17',
  'at its paused point.',
  '',
  'TWO DOCUMENTS GOVERN THIS WORK. The first is the brief above, and it is',
  'already in front of you. THE SECOND IS STILL A FILE AND YOU MUST STILL READ',
  'IT, exactly as before:',
  '  ' + ROOT + '/data/alignment/' + COURSE + '_CAMPAIGN.md   ("Course shape" section)',
  '',
  'THE TEN RULES ARE INVIOLABLE AND THE GENERATOR ENFORCES THEM. They are in the',
  'brief above, in full, and THIS PROMPT DELIBERATELY CARRIES NO SECOND COPY OF',
  'THEM. It used to carry a hand-written précis of all ten, sitting a few',
  'thousand bytes under the verbatim brief with nothing comparing the two — the',
  'exact defect the gate over the inlined brief exists for, one layer up. It was',
  'already short of the brief in one measurable place: it stated rule 7 without',
  'the HARD CHECK the reconcile brief added on 2026-09-04, which is the check',
  'that catches 8 of the 19 confirmed defects that audit found. A paraphrase',
  'nobody compares is how a rule quietly changes, so the paraphrase is gone.',
  'Scroll up and read rules 1-10 as the brief states them. (The pre-flight',
  'below names rule 10 once, to say it outranks coverage; that is a pointer to',
  'the rule, not a second statement of the ten.)',
  '',
  'Prose of this length takes 25-60 spans. Cover the content words; null or omit',
  'particles freely - an omitted particle costs nothing, a mis-depthed one costs the page.',
  '',
  'Geshe Michael\'s English stays VERBATIM everywhere - quote it as it stands, never',
  'corrected. Where it is wrong, that is an erratum to report, never a licence to',
  'distort a span.',
].join('\n')

// ---------------------------------------------------------------------------
// THE DIGEST THE PRE-FLIGHT ACTUALLY CHECKS.
//
// The pre-flight used to hand the agent the hashes the dumper printed and tell
// it the check "compares the text inlined above against the spine". It did
// not. The hashes travelled BESIDE the text in the same args blob and nothing
// bound one to the other: corrupt args.segments["172"].english, leave the sha
// fields alone, and the check printed "spine ok" over a sentence Geshe Michael
// never wrote. Telling an agent something was checked when it was not is a
// rule 3 breach, so the check was made true rather than the sentence softened.
//
// These four lines compute the digest HERE, from the very string that is
// inlined below, and the dumper recomputes it from the live spine row
// (--verify-inlined). A workflow script has no crypto module, no Buffer and no
// TextEncoder, so: UTF-8 encoded by hand, FNV-1a 32 by the shift-and-add
// decomposition of the prime, and the exact utf-8 byte length carried beside
// it. tools/dump_spine_segments.py:fnv1a32 is written the same way on purpose;
// the two must agree byte for byte.
// ---------------------------------------------------------------------------

function fnv1a(s) {
  let h = 0x811c9dc5
  let n = 0
  const eat = b => {
    n++
    h = (h ^ b) >>> 0
    h = (h + ((h << 1) + (h << 4) + (h << 7) + (h << 8) + (h << 24))) >>> 0
  }
  for (let i = 0; i < s.length; i++) {
    let c = s.charCodeAt(i)
    if (c >= 0xd800 && c <= 0xdbff && i + 1 < s.length) {
      const lo = s.charCodeAt(i + 1)
      if (lo >= 0xdc00 && lo <= 0xdfff) {
        c = 0x10000 + ((c - 0xd800) << 10) + (lo - 0xdc00)
        i++
      }
    }
    if (c < 0x80) eat(c)
    else if (c < 0x800) { eat(0xc0 | (c >> 6)); eat(0x80 | (c & 63)) }
    else if (c < 0x10000) {
      eat(0xe0 | (c >> 12)); eat(0x80 | ((c >> 6) & 63)); eat(0x80 | (c & 63))
    } else {
      eat(0xf0 | (c >> 18)); eat(0x80 | ((c >> 12) & 63))
      eat(0x80 | ((c >> 6) & 63)); eat(0x80 | (c & 63))
    }
  }
  let hex = (h >>> 0).toString(16)
  while (hex.length < 8) hex = '0' + hex
  return n + ':' + hex
}

// null and '' are different states of a spine field and must not digest alike;
// the dumper uses the same token for a NULL column.
const digestToken = v =>
  (v === null || v === undefined) ? '-none-' : fnv1a(String(v))

// ---------------------------------------------------------------------------
// Per-segment blocks.
// ---------------------------------------------------------------------------

const FETCH_CMD = seq =>
  '  sqlite3 ' + SPINE + ' "select wylie, english, acip from corpus_segments' +
  ' where course=\'' + COURSE + '\' and seq=' + seq + '"'

// One place computes the three tokens, and both the source block and the
// pre-flight read them from it, so the numbers the agent is shown and the
// numbers the command carries cannot drift apart.
const digests = seq => {
  const s = SEG[String(seq)] || {}
  return {
    wylie: digestToken(s.wylie),
    english: digestToken(s.english),
    acip: digestToken(s.acip),
  }
}

const field = (label, v) =>
  v === null || v === undefined
    ? label + ': (the spine holds no ' + label.toLowerCase() + ' for this row)'
    : label + ':\n' + v

function sourceBlock(seq) {
  const s = SEG[String(seq)]
  if (!s) {
    return [
      '--- SOURCE TEXT ' + COURSE + ':' + seq + ' — NOT SUPPLIED BY THE HARNESS ---',
      'This launch did not carry a row for this segment, so nothing is inlined.',
      'Fetch it yourself exactly as the brief says, and note in your report that',
      'you had to:',
      FETCH_CMD(seq),
      '--- END SOURCE TEXT ---',
    ].join('\n')
  }
  const d = digests(seq)
  return [
    '--- SOURCE TEXT ' + COURSE + ':' + seq + ' — verbatim from the spine ---',
    field('WYLIE', s.wylie),
    '',
    field('ENGLISH', s.english),
    '',
    field('ACIP', s.acip),
    '',
    'checksum of the three fields exactly as they appear above, each as',
    '<utf-8 byte length>:<FNV-1a 32>, computed by this harness from this text:',
    '  wylie=' + d.wylie + ' english=' + d.english + ' acip=' + d.acip,
    'The pre-flight command below hands these to the spine and has them',
    'recomputed there, so the comparison is against THIS copy.',
    '--- END SOURCE TEXT ---',
  ].join('\n')
}

function precedentBlock(seq) {
  const p = PRECEDENT[String(seq)]
  const text = typeof p === 'string' ? p : (p && p.text)
  if (!text) {
    return [
      '--- PRECEDENT EXTRACT ' + COURSE + ':' + seq + ' — NOT SUPPLIED ---',
      'This launch carried no extract for this segment (the args were built',
      'without --precedent, or the index could not be read), so nothing is',
      'inlined at this point. Where you need precedent, count it yourself in',
      ROOT + '/data/alignment/alignment_full_v1.json, exactly as the brief says.',
      '(This is the injection point: a launch that carries args.precedent[' + seq + ']',
      'puts the extract for this segment\'s vocabulary right here, and this',
      'paragraph disappears.)',
      '--- END PRECEDENT EXTRACT ---',
    ].join('\n')
  }
  const prov = (p && p.provenance) ||
    'none supplied — treat every count in it as unverified'
  return [
    '--- PRECEDENT EXTRACT ' + COURSE + ':' + seq + ' — landed links for this',
    'segment\'s vocabulary, extracted by the harness ---',
    'PROVENANCE: ' + prov,
    'A count quoted to you is not evidence, and this extract is a quote. It',
    'saves you the SEARCH, never the verification: re-derive any count you',
    'actually rely on against',
    ROOT + '/data/alignment/alignment_full_v1.json and say what you counted. The',
    'bank grows every page, and a stale total is how a wrong precedent gets',
    'justified.',
    '',
    text,
    '--- END PRECEDENT EXTRACT ---',
  ].join('\n')
}

function preflight(seq, specPath, bodyPath) {
  const s = SEG[String(seq)]
  const gen = 'python3 tools/gen_alignment_page.py < ' + specPath + ' > ' +
    bodyPath
  const lines = [
    'PRE-FLIGHT — MANDATORY, and it is the brief\'s command.',
    '',
    '  cd ' + ROOT,
  ]
  if (s) {
    const d = digests(seq)
    lines.push(
      '  python3 tools/dump_spine_segments.py --verify-inlined ' + COURSE +
        ' ' + seq + ' ' + d.wylie + ' ' + d.english + ' ' + d.acip +
        ' && ' + gen,
      '  echo "EXIT=$?"',
      '',
      'What the check chained in front proves, exactly. The three tokens in it',
      'were computed by this harness FROM THE TEXT INLINED ABOVE — its utf-8',
      'byte length and an FNV-1a 32 checksum of it — and the tool recomputes',
      'both from the spine row as it stands right now. So it compares THIS',
      'COPY against the spine, not a hash that travelled beside it. It is a',
      'checksum, not a signature: it catches corruption, mistranscription and',
      'staleness, and does not pretend to stop a forger.',
      'A "SPINE MISMATCH" line means the text above is not what the spine',
      'holds: stop, re-fetch the row with the brief\'s own command, align',
      'against the spine, and say so in your report. Anything else is the',
      'generator speaking.')
  } else {
    lines.push('  ' + gen, '  echo "EXIT=$?"',
      '',
      'No row was inlined for this segment, so there is nothing here to check',
      'against the spine. Fetch the row yourself, as the brief says.')
  }
  lines.push(
    '',
    'A "REFUSED:" line is the generator naming your span. A refusal is',
    'information, not an obstacle — fix the span, never the rule. If a span',
    'cannot be made to pass honestly, DROP IT: rule 10 outranks coverage. Keep',
    'fixing and re-running until EXIT=0. Never return a spec that has not.')
  return lines.join('\n')
}

const proposeCommon = seq => [
  '=== BRIEF: docs/alignment_briefs/PROPOSE_BRIEF.md (verbatim, the authority) ===',
  PROPOSE_BRIEF,
  '=== END BRIEF ===',
  '',
  SUPPLEMENT,
  '',
  COURSE_CONTEXT,
  '',
  sourceBlock(seq),
  '',
  precedentBlock(seq),
  EXTRA,
].join('\n')

const reconcileCommon = seq => [
  '=== BRIEF: docs/alignment_briefs/RECONCILE_BRIEF.md (verbatim, the authority) ===',
  RECONCILE_BRIEF,
  '=== END BRIEF ===',
  '',
  SUPPLEMENT,
  '',
  COURSE_CONTEXT,
  '',
  sourceBlock(seq),
  '',
  precedentBlock(seq),
  EXTRA,
].join('\n')

const SPEC_SCHEMA = {
  type: 'object',
  properties: {
    seq: { type: 'number' }, angle: { type: 'string' }, title: { type: 'string' },
    spans: { type: 'array', items: { type: 'object', properties: {
      id: { type: 'string' }, d: { type: 'number' }, tib: { type: 'string' },
      eng: { type: ['string', 'null'] }, subword: { type: 'boolean' },
      cls: { type: 'string' }, nul: { type: 'string' } },
      required: ['id', 'd', 'tib'] } },
    eng_order: { type: 'array', items: { type: 'string' } },
    note: { type: 'string' },
    errata: { type: 'array', items: { type: 'string' } },
    reasoning: { type: 'string' },
  },
  required: ['seq', 'angle', 'title', 'spans', 'reasoning'],
}

const PREFETCH_SCHEMA = {
  type: 'object',
  properties: {
    rows: { type: 'array', items: { type: 'object', properties: {
      seq: { type: 'number' },
      wylie: { type: ['string', 'null'] },
      english: { type: ['string', 'null'] },
      acip: { type: ['string', 'null'] } },
      required: ['seq', 'wylie', 'english', 'acip'] } },
    note: { type: 'string' },
  },
  required: ['rows'],
}

const ANGLES = [
  { key: 'tibetan-first', how:
    'YOUR ANGLE IS TIBETAN-FIRST. Walk the WYLIE from its first syllable to its last.\n' +
    'For each Tibetan word or particle decide what, if anything, in the English renders\n' +
    'it. Let the Tibetan set the inventory: if a Tibetan word has no English exponent\n' +
    'that is a null, not a reason to stretch a nearby phrase. You will naturally produce\n' +
    'good d=5/d=7 structure and may under-read the English.' },
  { key: 'english-first', how:
    'YOUR ANGLE IS ENGLISH-FIRST. Walk Geshe Michael\'s ENGLISH from its first word to\n' +
    'its last. For each content word or phrase decide which Tibetan it renders. Where\n' +
    'Geshe Michael has supplied a hinge, an article or a connective with no Tibetan\n' +
    'under it, LEAVE IT\n' +
    'UNWRAPPED - that is rule 7 and it is the dominant measured defect. You will catch\n' +
    'English the Tibetan-first analyst misses, and may over-reach; do not.' },
]

// ---------------------------------------------------------------------------
// Prefetch, only when the launch did not carry the rows. One agent for the
// whole batch instead of three per segment. Its provenance is NOT the same as
// the dumper's -- a model transcribed the rows -- and the log and the prompt
// both say so.
//
// AND IT IS NOT WHERE THE SAVING IS. An agent costs a fixed preamble of about
// 55k tokens before it does anything, which is roughly what the three sqlite
// calls it replaces were costing. Treat this as the fallback that keeps the
// workflow runnable without a pasted args blob, not as the optimisation. The
// optimisation is `tools/dump_spine_segments.py --course C05 --seqs ...`
// piped into args, which costs nothing and puts no model between the database
// and the prompt.
// ---------------------------------------------------------------------------

const missing = SEQS.filter(seq => !SEG[String(seq)])
// Counted, not assumed. The run record used to say "prefetch agent (a model
// transcribed it)" whenever the prefetch was ENTERED -- including the run
// where the prefetch returned nothing and every agent fetched its own row,
// which made the record a false statement about where the text came from.
let prefetched = 0
if (missing.length) {
  phase('Prefetch')
  log('source text: ' + missing.length + ' of ' + SEQS.length + ' segments were ' +
      'not supplied in args; fetching them once with one agent. Produce args ' +
      'with tools/dump_spine_segments.py to skip this step and remove the ' +
      'transcription risk entirely.')
  const pre = await agent(
    'You are the transport step for a Course 5 alignment batch. You judge nothing.\n\n' +
    'Run exactly this, once, and return what it prints:\n\n' +
    '  cd ' + ROOT + '\n' +
    '  python3 tools/dump_spine_segments.py --course ' + COURSE + ' --seqs ' +
      missing.join(',') + '\n\n' +
    'It prints one JSON object with a "segments" map. Return its rows as the\n' +
    '"rows" array: seq, wylie, english, acip copied EXACTLY as the tool printed\n' +
    'them (byte-for-byte, including any typo, spacing or bracket).\n\n' +
    'Do not copy the "sha" values and do not compute any hash. You used to be\n' +
    'asked for them, and that made the check circular: the hashes you copied\n' +
    'were compared against the spine while the text you copied was compared\n' +
    'against nothing. The harness now checksums the text you return, itself, in\n' +
    'code, and the pre-flight compares THAT against the spine — so a slip in\n' +
    'your transcription is caught rather than certified.\n\n' +
    'Do not summarise, normalise, re-wrap or tidy anything. If the tool refuses,\n' +
    'return rows for whatever it did print and say what it refused in "note".',
    { label: 'prefetch:' + COURSE, phase: 'Prefetch', schema: PREFETCH_SCHEMA,
      model: 'opus', effort: 'low' })
  const rows = (pre && pre.rows) || []
  for (const r of rows) {
    SEG[String(r.seq)] = { wylie: r.wylie, english: r.english, acip: r.acip }
    prefetched++
  }
  log('source text: prefetch returned ' + rows.length + ' of ' + missing.length +
      ' rows. These passed through a model, so what stands behind them is the ' +
      'pre-flight checksum of the text itself (--verify-inlined, computed by ' +
      'this harness from what the model returned) and the generator\'s own ' +
      'spine lookup.')
}

const inlined = SEQS.filter(seq => SEG[String(seq)]).length
const withPrecedent = SEQS.filter(seq => {
  const p = PRECEDENT[String(seq)]
  return !!(typeof p === 'string' ? p : (p && p.text))
}).length
log('source text inlined for ' + inlined + '/' + SEQS.length + ' segments; ' +
    'precedent extract inlined for ' + withPrecedent + '/' + SEQS.length + '. ' +
    'Any segment not covered keeps the brief\'s fetch-it-yourself instruction.')

phase('Propose')
log(SEQS.length + ' segments, ' + (SEQS.length * 3) + ' agents')

const proposals = await parallel(
  SEQS.flatMap(seq => ANGLES.map(a => () => {
    const dir = SCRATCH + '/' + seq + '-' + a.key
    return agent(proposeCommon(seq) +
      '\n\nYOUR TASK: propose the full-depth alignment for ' + COURSE +
      ' segment ' + seq + '.\n\n' + a.how + '\n\n' +
      'YOUR DIRECTORY is ' + dir + '. Every deliverable the brief names goes\n' +
      'there — spec.json, body.html, errata.json, report.md — and nothing else\n' +
      'in the repository is yours to touch.\n\n' +
      preflight(seq, dir + '/spec.json', dir + '/body.html') + '\n\n' +
      'Return the spec as structured output. In "reasoning" say what you were UNSURE\n' +
      'about and where the other angle is likely to disagree with you - that honesty is\n' +
      'what the reconciler uses.',
      { label: 'propose:' + seq + ':' + a.key, phase: 'Propose', schema: SPEC_SCHEMA,
        model: 'opus', effort: 'high' })
      .then(r => ({ seq, angle: a.key, spec: r }))
  }))
)

const bySeq = {}
for (const p of proposals.filter(Boolean)) (bySeq[p.seq] = bySeq[p.seq] || []).push(p)

phase('Reconcile')

const reconciled = await parallel(SEQS.map(seq => () => {
  const pair = bySeq[seq] || []
  if (pair.length < 2) return Promise.resolve({ seq, failed: 'fewer than two proposals' })
  const dir = SCRATCH + '/' + seq + '-reconciled'
  // The spec path is the LANDER's, not the brief's: tools/land_alignment_page.py
  // reads <specs dir>/<seq>-reconciled.json. Moving it into the agent's own
  // directory to match the brief's wording would have quietly broken landing.
  const specPath = SCRATCH + '/' + seq + '-reconciled.json'
  const bodyPath = SCRATCH + '/' + seq + '-body.html'
  return agent(reconcileCommon(seq) +
    '\n\nYOUR TASK: reconcile the two independent proposals for ' + COURSE +
    ' segment ' + seq +
    ' into ONE spec, and PROVE IT by making the generator accept it.\n\n' +
    'The brief says the proposals are in a JSON file named in your task. They are\n' +
    'inlined here instead; there is no file to open.\n\n' +
    'PROPOSAL A (' + pair[0].angle + '):\n' + JSON.stringify(pair[0].spec, null, 1) + '\n\n' +
    'PROPOSAL B (' + pair[1].angle + '):\n' + JSON.stringify(pair[1].spec, null, 1) + '\n\n' +
    'YOUR DELIVERABLE IS A SPEC FILE THAT EXITED 0.\n\n' +
    'PATHS, and they are not the brief\'s: the brief says <YOUR_DIR>/spec.json,\n' +
    'but tools/land_alignment_page.py reads the merged spec from\n' +
    '<specs dir>/<seq>-reconciled.json and nothing else. So write the SPEC to\n\n' +
    '  ' + specPath + '\n\n' +
    'and put your other deliverables — errata.json and report.md — in\n' +
    '  ' + dir + '\n\n' +
    'The spec is in the format gen_alignment_page.py reads:\n\n' +
    '  { "course": "' + COURSE + '", "segments": [ { "seq": ' + seq + ', "title": "...",\n' +
    '      "spans": [...], "eng_order": [...], "note": "..." } ] }\n\n' +
    preflight(seq, specPath, bodyPath) + '\n\n' +
    'SETTLE DISAGREEMENTS AGAINST THE LANDED CORPUS, not by taste: count the existing\n' +
    'links for a contested Tibetan string in ' + ROOT + '/data/alignment/alignment_full_v1.json,\n' +
    'or read them off the PRECEDENT EXTRACT above if it carries them, and let precedent\n' +
    'decide. Say what you counted. Do not trust any total quoted to you - INCLUDING BY\n' +
    'THIS PROMPT AND BY THE PRECEDENT EXTRACT ABOVE - count the file yourself; the bank\n' +
    'grows every page, and a stale total is how a wrong precedent gets justified. The\n' +
    'extract saves you the SEARCH, never the verification.\n\n' +
    'ALSO FLAG, for the lander: any d=5 or d=7 span whose English begins with\n' +
    'the/a/an/and/or/his/our/your/I/you - it will trip the no_supplied_span_head gate\n' +
    'and needs a licensor in data/alignment/span_head_allow.json naming the Tibetan\n' +
    'word that owns the function word.\n\n' +
    'Return: seq, angle "reconciled", final title, final spans, eng_order, note,\n' +
    'errata, and in "reasoning" - the exit code you achieved, the span count, what the\n' +
    'analysts disagreed about and how you settled it, what you DROPPED rather than\n' +
    'bank, and any span-head licensor the lander must add.',
    { label: 'reconcile:' + seq, phase: 'Reconcile', schema: SPEC_SCHEMA,
      model: 'opus', effort: 'high' })
    .then(r => ({ seq, spec: r }))
}))

return {
  scratch: SCRATCH,
  transport: {
    source_text_inlined: inlined + '/' + SEQS.length,
    // Derived from what actually landed, not from whether the prefetch ran.
    source_text_from: !missing.length
      ? 'args, from tools/dump_spine_segments.py (no model in the loop)'
      : prefetched
        ? ('prefetch agent (a model transcribed ' + prefetched + ' row(s)); ' +
           (SEQS.length - inlined) + ' segment(s) still unsupplied, so those ' +
           'agents fetched their own row')
        : 'each agent fetched its own row — the prefetch returned nothing',
    source_text_checked: inlined
      ? ('every inlined row is checksummed in the pre-flight against the live '
         + 'spine (dump_spine_segments.py --verify-inlined)')
      : 'nothing was inlined, so there was nothing to check',
    precedent_inlined: withPrecedent + '/' + SEQS.length,
    briefs_inlined: 'PROPOSE_BRIEF.md, RECONCILE_BRIEF.md (gates: ' +
      'workflow_briefs_in_sync, workflow_briefs_delivered)',
  },
  proposals: proposals.filter(Boolean).map(p => ({ seq: p.seq, angle: p.angle, spans: (p.spec && p.spec.spans || []).length })),
  reconciled: reconciled.filter(Boolean).map(r => ({
    seq: r.seq, spans: (r.spec && r.spec.spans || []).length,
    title: r.spec && r.spec.title, errata: (r.spec && r.spec.errata) || [],
    reasoning: r.spec && r.spec.reasoning })),
}
