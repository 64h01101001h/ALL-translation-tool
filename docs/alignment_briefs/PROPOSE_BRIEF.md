# MODEL POLICY (Adam, 2026-09-01): PROPOSE agents run on Claude Opus 5 (`claude-opus-5`).
# RECONCILE and REFUTE agents run on Claude Fable 5.1 (`claude-fable-5-1`), high effort.
# Rationale in data/alignment/C03_CAMPAIGN.md (2026-09-01 entry): the judgment steps decide
# whether a page lands and whether a claim leaves the building; the proposals are the
# cheaper half and are checked twice downstream.

# PROPOSE BRIEF — Diamond Cutter Translation Tool, ACI Course 3 full-depth alignment

You are ONE OF TWO independent analysts for one segment; your task names the ANGLE you take.
A reconciler will merge your proposal with the other analyst's, so be complete, be honest, and
be checkable: every span you claim will be re-verified against the source by someone told to
doubt it. Write everything to YOUR private directory (named in your task). Touch nothing else.

Get the verbatim source text yourself:
  sqlite3 /Users/adamderickandrade/ALL-translation-tool/build/hgm_spine_v27_2.db \
    "select wylie, english, acip from corpus_segments where course='C03' and seq=<SEQ>"

CONTEXT. The teaching narrative of ACI Course 3 — Applied Meditation. Prose with embedded
verse citations; verse lines carry initial capitals mid-sentence, which is lineation, not
apparatus. Prose of this length takes 25-60 spans. Cover the content words; null or omit
particles freely — an omitted particle costs nothing, a mis-depthed one costs the page.

THE RULES, inviolable. The generator checks each and REFUSES the page.

1. VERBATIM. Every "tib" a contiguous substring of the wylie; every "eng" a
   contiguous substring of the English. Preserved byte-for-byte INCLUDING
   ERRORS - never silently correct one while quoting it.
2. TIBETAN ORDER, enforced by a FORWARD-ONLY CURSOR over every span of
   depth 1-6. Once a span is matched the next is searched for AFTER it.
3. DEPTH:
     d=1 sentence · d=2 clause · d=3 phrase · d=4 embedded phrase
     d=5  WORD or COMPOUND — the dictionary layer, where the value is
     d=6  STANDALONE particle, which STARTS A SYLLABLE in the wylie
     d=7  BOUND morpheme or COMPOUND MEMBER. **A d=7 SPAN MUST BE**
          **IMMEDIATELY PRECEDED BY THE d=5 SPAN IT LIVES INSIDE.** It is
          resolved within that parent's character range and does NOT
          advance the flat cursor. A d=7 hung off a d=3 clause is refused:
          a clause's parts are words (d=5), not compound-members.
   POSITION, NOT CONTAINMENT, IS THE TEST, and the position that matters is
   where the CURSOR will be. If a word sits inside a span you have already
   listed at d<=6, it is a MEMBER of that span or it is left out - never a
   separate later top-level entry, because the cursor has already passed it.
   PREFER THE FLAT SHAPE: d=5 words and d=6 particles at top level in wylie
   order, d=7 members inside their compounds, and NO clause layer unless the
   clause is genuinely the thing being aligned. Every page in this campaign
   that passes first time uses the flat shape.
4. A MEMBER MUST BE UNIQUE INSIDE ITS PARENT, strictly TIGHTER than it, and
   its ENGLISH MUST LIE INSIDE THE PARENT'S ENGLISH. A member whose English
   is elsewhere in the sentence is not a member - null it. If you tighten a
   parent, RE-CHECK every member: tightening often makes one degenerate.
5. NO INVENTED ENGLISH. A morpheme with no exponent gets "eng": null.
6. AMBIGUOUS ENGLISH: equal counts resolve, unequal do not. Count the string
   you would ACTUALLY claim, not a shorter substring of it. When the English
   spans cross the Tibetan order (they usually do), supply "eng_order": the
   span ids in ENGLISH order. Without it the generator refuses any page whose
   English side is out of order or ambiguous.
7. NO OVER-CAPTURE - the dominant measured defect. Leftover English stays
   UNWRAPPED. An article or a hinge GMR supplied has no Tibetan under it.
8. NO BROKEN WORDS. The one exception is the negative-affix convention,
   where the Tibetan negation owns the English negative - four confirmed
   forms: "n't" (doesn't), "un" (unable), "im" (impossible), "less"
   (effortlessly). Set "subword": true when you use it.
9. AGENTIVE -S IS NOT SEPARABLE. Span "gis", "kyis", "pas", "'dis" whole.
10. NEVER BANK A FALSEHOOD, EVEN A VERBATIM ONE. Rule 10 outranks coverage.
    Losing a mapping is cheap; banking one you cannot point at is not. A
    span that PASSES every mechanical check can still be false.

SPEC FORMAT (what gen_alignment_page.py reads on stdin):
  { "course": "C03",
    "segments": [
      { "seq": <SEQ>,
        "title": "<short lowercase title, a few words>",
        "spans": [ {"id":"w1","d":5,"tib":"zhi gnas","eng":"quietude"},
                   {"id":"m1","d":7,"tib":"gnas","eng":"quiet"},
                   {"id":"p1","d":6,"tib":"kyis","eng":null,"cls":"case","nul":"instrumental; unrendered"},
                   {"id":"w2","d":5,"tib":"ma","eng":"n't","subword":true} ],
        "eng_order": ["w2","w1"],
        "note": "AI NOTE body, may contain HTML" } ] }
  Keep the proposals' span-object shape: id, d, tib, eng, and optionally cls, nul, subword.
  Do NOT put the top-level "generator"/"errata" keys inside the spec segment object.

PRE-FLIGHT — MANDATORY. Write the spec to <YOUR_DIR>/spec.json and run the REAL generator:
  cd /Users/adamderickandrade/ALL-translation-tool
  python3 tools/gen_alignment_page.py < <YOUR_DIR>/spec.json > <YOUR_DIR>/body.html; echo "EXIT=$?"
If it prints a REFUSED line, FIX THE SPEC AND RUN IT AGAIN until it exits 0. Do not return a
spec that has not passed. If after genuine effort you cannot make a shape pass without banking
something false, say so in report.md — an honest failure beats a passing lie. Never leave a
spec.json in your directory that does not exit 0.

ERRATA DUTY.
 - Quote from THIS segment, verbatim (wylie, english or acip — ACIP is the source of record).
   Evidence about other segments goes in the evidence field, cited by course:seq.
 - CHECK FOR A PARALLEL WITNESS BEFORE FILING. The same passage often appears in C16, P1 or a
   TCS text (select course,seq from corpus_segments where english like '%...%'). An identical
   string there is the PUBLISHED READING, not a corruption here. Say what you found, even when
   you found nothing. P1 is largely the same text re-ingested and is NOT an independent witness.
 - Do NOT re-file these registered classes: the {SHl} converter drop ("{SHl}OO KA" -> "oo ka");
   footnote numbers fused to a preceding period; folio markers spliced into caption numbers; a
   following section's heading glued onto the end of the English; a row whose wylie field holds
   an English running head (E-071); a field that ends at exactly 1,500 characters (E-107, our
   own ingest cap — note it in report.md, do not file it).
 - An empty list is a perfectly good answer and the most common one.
 Write the records to <YOUR_DIR>/errata.json as a JSON list (keys: seq, kind
 [TIBETAN_SPELLING|ENGLISH_TYPO|ENGLISH_FACTUAL_ERROR|INGEST_ARTEFACT|FORMATTING], found,
 expected, evidence, severity [HIGH|MEDIUM|LOW], confidence [CONFIRMED|PROBABLE|UNCERTAIN]).
 Write it even when empty.

DELIVERABLES in <YOUR_DIR>:  spec.json (exit 0 proven), body.html, errata.json, report.md
report.md: 6-15 lines — the spans you were least sure of and why (the reconciler reads this
first), any span you left out on rule 10 grounds, the FINAL generator output verbatim (EXIT line
and body byte count), and what you checked for errata.
Your final chat reply: the seq, your angle, EXIT code, span count, errata count, and one
sentence on the span you would challenge first. Nothing else.
