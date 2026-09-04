# MODEL POLICY (Adam, 2026-09-01): first-read AUDITORS run on Claude Opus 5 (they propose a
# verdict, the cheaper half); SECOND READERS and SKEPTICS run on Claude Fable 5.1, high effort.

# AUDIT BRIEF — correctness of one alignment-layer pair (C03-stratified audit, 2026-09-04)

You are auditing ONE (Tibetan headword, English rendering, citation) pair from the shipped
alignment evidence layer, which the campaign's own meta calls TENTATIVE (machine-matched,
unreviewed). Your job is to say whether the pairing is CORRECT as an attestation: does GMR's
English fragment really render this Tibetan headword in this segment? Read
docs/ALIGNMENT_LAYER_SPEC.md §3 first: the ten rules are the standard.

Your item is a JSON file named in your task with: headword (wylie), acip, eng (the banked
rendering), ref (course:seq), siblings (every other rendering the layer banks for this
headword, with citations), and segment (the full wylie, english and acip of the cited row).
DO NOT retype anything; quote from the file. Re-query the spine yourself if you need context:
  sqlite3 /Users/adamderickandrade/ALL-translation-tool/build/hgm_spine_v27_2.db \
    "select wylie, english, acip from corpus_segments where course='C03' and seq=<SEQ>"
The same database carries the dictionary tables (.tables) for checking what the headword means.

CALL ONE OF:
  SOUND        — the English is a defensible rendering of exactly this Tibetan in this segment,
                 with no supplied word inside the span and nothing of the Tibetan word's
                 meaning missing that GMR actually rendered elsewhere in the sentence.
  DEFECTIVE    — give ONE kind:
     OVER_CAPTURE   the span includes English with no Tibetan under it (an article, a hinge,
                    a supplied pronoun, GMR's own frame);
     UNDER_CAPTURE  the span omits English that is the exponent of this very word;
     WRONG_MAPPING  the English renders a DIFFERENT Tibetan word of the segment, or the pairing
                    is positional rather than lexical (a false dictionary entry);
     INVENTED       the English does not occur in the segment at all (should be impossible;
                    if you see it, say so loudly).
  UNDECIDABLE  — only if the segment itself is defective (row skew, dittograph) so that no
                 pairing can be judged; name the errata class.

METHOD, in order: (1) find the Tibetan word in the wylie and read its clause; (2) find the
banked English in GMR's sentence and read its clause; (3) ask what ELSE in the Tibetan clause
could license that English, and what else in the English could render this Tibetan — an
auditor who cannot see the siblings invents licensors, so use them; (4) check the dictionary
for the headword; (5) decide. Over-capture claims are the ones most often refuted: before
you call OVER_CAPTURE, name the Tibetan word you believe has NO English under the disputed
words, and check that no other word in the clause licenses them.

WRITE your verdict as JSON to the verdict path named in your task:
  {"i": <item index>, "headword": ..., "eng": ..., "ref": ..., "verdict": "SOUND"|"DEFECTIVE"|"UNDECIDABLE",
   "kind": null|"OVER_CAPTURE"|"UNDER_CAPTURE"|"WRONG_MAPPING"|"INVENTED",
   "confidence": "HIGH"|"MEDIUM"|"LOW",
   "licensor_check": "<the Tibetan you say does or does not license each disputed English word>",
   "reason": "<two or three sentences, quoting the segment>"}
Final chat reply, under 60 words: item index, headword -> eng, verdict, kind, one sentence why.
