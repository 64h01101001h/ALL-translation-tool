# C02 CAMPAIGN — ACI Course 2 (Buddhist Refuge), full-depth alignment

The main C02 pass (segments 22–68, pages p01–p15) was delivered in an
earlier session. This ledger opens 2026-08-28 for the **coverage
backfill**: segments 1–22 were never scanned, because that campaign —
like C01's — began partway in. The failure and its systemic fix are
recorded in TODO.md and in C01_CAMPAIGN.md's correction of record.

Standing rules (Adam, 2026-08-28): GMR's English stays VERBATIM, typos
flagged never corrected; nothing binding without human review; every
batch records its producing model; completeness is proved by the
builder's COVERAGE GATE, never asserted in prose.

## ⚠ CORPUS FINDING — C02:6 carries markup the other courses do not

The mandala stanza appears identically in all three courses, and
**C01:6 and C03:6 are byte-identical to each other (410 chars)**.
**C02:6 is 414 chars** and differs by exactly one thing:

    C01 / C03 : ... Here is the great Earth, Filled with the smell ...
    C02       : ... Here is the great <1 Earth>, Filled with the smell ...

`<1 Earth>` is a stray corpus markup artifact — an annotation or
footnote wrapper that leaked into the English text of this course's
copy and not the others'. It is NOT Geshe Michael's wording; it is
scaffolding around his wording.

Per Adam's ruling 1 it is kept **verbatim** and flagged, not cleaned.
It is recorded here because it is a genuine three-way witness: two
courses preserve the clean reading, one preserves a marked-up reading,
and that is evidence about the corpus rather than about the text.

**THE ARTIFACT IS A SINGLETON.** A scan of all 8,961 segments across
C01–C18 for angle-bracket markup returns exactly ONE hit — this one.
It is an isolated defect, not a systemic leak, so the upstream repair
is a single edit with no class of similar cases behind it.

**For the review pass:** a strong candidate for corpus-level repair
upstream in the data project, rather than an editorial change here.
Flagged for Adam.

## Progress
c2L1–c2L3 (1–9) done; 10–22 open. Main pass 22–68 already delivered.

### BACKFILL C2-B1 (c2L1, C02:1–3) — 2026-08-28 — model: Opus
- Generated from C02's own spine rows. Segments 1–3 are byte-identical
  to C01's and C03's in both columns.
- 1: maNDla ↔ "mandel". 2: spos kyis ↔ "pukyi"; byugs ↔ "juk".
  3: nyi zlas ↔ "nyinde"; brgyan pa ↔ "gyenpa".
- Of the 22 backfill segments, 20 are byte-identical across all three
  courses; the exceptions are 6 (the markup artifact above) and 22
  (its own Course II syllabus, expected).
- COVERAGE GATE: C02 46→49; 19 open (4-22).

### BACKFILL C2-B2 (c2L2, C02:4–6) — 2026-08-28 — model: Opus
- 4, 5: byte-identical to C01 and C03. dbul bar ↔ "ulwar"; spyod par
  ↔ "chupar".
- 6: THE MARKUP ARTIFACT SHIPPED VERBATIM. The page displays
  `<1 Earth>` exactly as the corpus stores it, angle brackets and
  all. Required a new escaping path in the generator (untagged runs
  and span contents HTML-escaped) so text containing < and > can
  survive byte-exact onto the page; the letter-exact battery passes
  against the stored text including the brackets, and a dedicated
  check confirms the string "<1 Earth>" is present verbatim after
  unescaping.
- Not corrected. Flagged for UPSTREAM repair in the data project
  rather than editorial change here — fixing it in this layer would
  leave the dictionary silently disagreeing with its own source.
- COVERAGE GATE: C02 49→52; 16 open (7-22).

### BACKFILL C2-B3 (c2L3, C02:7–9) — 2026-08-28 — model: Opus
- All three byte-identical to C01 and C03. kyabdro semkye; chos ↔
  "chu"; bdag ni ↔ "dakni" against bdag gi/gis ↔ "dakki".
- Corpus-wide artifact scan run this batch: 8,961 segments, exactly
  one angle-bracket hit (C02:6). Singleton confirmed.
- COVERAGE GATE: C02 52→55; 13 open (10-22).
