# MODEL POLICY (Adam, 2026-09-01): PROPOSE agents run on Claude Opus 5 (`claude-opus-5`).
# RECONCILE and REFUTE agents run on Claude Fable 5.1 (`claude-fable-5-1`), high effort.
# Rationale in data/alignment/C03_CAMPAIGN.md (2026-09-01 entry): the judgment steps decide
# whether a page lands and whether a claim leaves the building; the proposals are the
# cheaper half and are checked twice downstream.

# ADVERSARIAL CHECK — Diamond Cutter Translation Tool, ACI Course 3 errata

Your job is to REFUTE a claimed defect in a source document, not to confirm it. Default to
refuted=true when uncertain. A false erratum sent to ALL's editors is worse than a missed one,
because it makes us the unreliable party in a conversation about someone else's errors.

The claim(s) are in the JSON file named in your task. Repo: /Users/adamderickandrade/ALL-translation-tool
(read-only; you write ONLY the verdict file named in your task).

Check each claim yourself against the spine (sqlite3 is installed):
  sqlite3 /Users/adamderickandrade/ALL-translation-tool/build/hgm_spine_v27_2.db \
    "select wylie, english, acip from corpus_segments where course='C03' and seq=<SEQ>"

REFUTE IT IF ANY OF THESE HOLD, and check every one:
 (a) the quoted "found" text is NOT present verbatim at that segment — check wylie, english AND
     acip, since ACIP is the source of record and a quote may legitimately come from it;
 (b) a PARALLEL WITNESS carries the same reading as an INDEPENDENT publication — search C16, P1
     and the TCS texts for the same passage:
       select course,seq from corpus_segments where english like '%...%';
       select course,seq from corpus_segments where wylie like '%...%';
     A witness that is merely the same source re-ingested (compare the two segments directly and
     quote the differences) is NOT independent. If an independent witness has the same reading,
     it is the published reading, not a defect here. Say which witness and quote it;
 (c) it belongs to a REGISTERED CLASS already recorded in docs/errata_register.json — the {SHl}
     converter drop, a footnote number fused to a period, a folio marker in a caption number, a
     section heading glued onto the English. Grep the register for the class and for the segment;
     a claim already filed for this segment is a duplicate, and a duplicate is refuted;
 (d) the "expected" correction is itself wrong, or the reading as it stands is defensible Tibetan
     or defensible English — check the dictionary tables in the same database (.tables; search
     headwords for the disputed form and for the proposed form);
 (e) the evidence cites counts or segments that do not check out when you re-run them yourself.
     Re-run every count. Do not take any number on trust.

Set refuted=true unless the claim survives ALL FIVE. If a claim survives but its severity or
confidence is overstated, keep refuted=false and say what they should be.

Write the verdicts as a JSON LIST to the verdict file named in your task, one object per claim, in
the same order as the claims, with keys: seq, found (first 80 chars of the claim's found text),
refuted (boolean), reason (the decisive check and what you found), parallel_witness (what
C16/P1/TCS say at the same passage, quoted, or "none found"), counts_rechecked (object mapping each
claimed count to what you measured), suggested_severity, suggested_confidence.

Your final chat reply: one line per claim — refuted true/false and one sentence why. Nothing else.
