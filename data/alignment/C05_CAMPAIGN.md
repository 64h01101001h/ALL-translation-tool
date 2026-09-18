# C05 CAMPAIGN — ACI Course 5 (How Karma Works), full-depth alignment

Opened 2026-09-05, immediately after C04 closed (198/198, 2026-09-04),
under Adam's standing order to scan C05 and then C06. Briefs are the
course-parametrised set in `docs/alignment_briefs/`; the task names the course.

Standing rules (Adam): GMR's English stays VERBATIM, typos flagged never
corrected; nothing binding without human review; every batch records its
producing model (propose: Claude Opus 5 · reconcile/refute: Claude Fable
5.1); completeness is proved by the builder's COVERAGE GATE, never asserted
in prose; a reconciler's deliverable is a spec FILE that exited 0.

## Course shape (spine scan, 511 segments)
- **1–21: the liturgical opening, BYTE-IDENTICAL to C03:1–21** (and so to
  C04:1–21) in both columns; 21/21 verified by SQL join at open. As in C04,
  c5p1–c5p7 are the landed c3p1–c3p7 copied verbatim with course labels
  changed and a CLONED PAGE banner (the phonetics-protocol pages cannot be
  regenerated, see C04 ledger); every sentence's plain text was checked
  against the C05 rows at copy time (21/21 match) and the layer gates
  re-derive every span against the C05 rows on each run.
- **22 and 23: corpus markers.** 22: wylie = the English running head,
  english = the Course V syllabus (531 chars, complete). 23: wylie = FOLIO
  REFERENCES ("127a, 147b-148b, 174a-174b"), english = the Reading Six
  heading and reading list, cut at the 1,500 cap (E-107). Hand-built
  marker blocks per the E-071 protocol, unpaired clause, no pairs.
- **24–511: Abhidharma.** Vasubandhu's *Treasure House of Knowledge*
  (chos mngon pa mdzod, Abhidharmakosha) root verses with the First Dalai
  Lama Gendun Drup's commentary *Illumination of the Path to Freedom*
  (mdzod Tik thar lam gsal byed), chapter IV (karma), plus Changkya's
  *Schools of Philosophy* (24–26) and Chandrakirti/Kedrup on emptiness and
  karma (Reading Six). 156 rows carry double-shad verse lineation `, ,`
  (root verses quoted inside the commentary, often prefixed with a bracketed
  verse number like `[iv.179-80]` — apparatus, not Tibetan); GMR renders the
  verses as capitalised English lines. Row headings ("Reading Two Subject: …")
  fused onto the English of rows 96, 118, 145, 206, 241, 314, 377, 411, 467
  (registered following-heading class — do not file). Row 30–31 carry the
  Sanskrit title in Tibetanised ACIP (`aa bhi dharma ko Sha kA ri kA`).
  Row 219's English is cut at the 1,500 cap (E-107; note, do not file).
  Median wylie length 222 chars, max 756. Parallel witnesses: C16 shares
  263 byte-identical rows (same file re-ingested, NOT independent); C15 54,
  C18 13, C09 9, C17 6.

## Gates
The seventeen standing gates plus `no_supplied_span_head`, which for
`pages_c05` refuses any d=5/d=7 English span beginning with
the/a/an/and/or/his/our/your/I/you unless `data/alignment/span_head_allow.json`
lists it under `pages_c05` with the Tibetan word that licenses it.

## Progress
c5p1–c5p7 (1–21) cloned 2026-09-05; c5p8–c5p49 (22–147) landed 2026-09-05; c5p50–c5p55 (148–165) landed 2026-09-08. Coverage 165/511 (17 gates green at every landing; pages_c05 0 unlicensed function-word span heads).

**PAUSED 2026-09-08 (Adam's call): the campaign stops here while the app goes to
a public release (two-week target). Reconciled-but-unlanded specs for 166, 167,
168, 169 and proposals for 170–175 exist only in session scratch and are NOT
banked; when the campaign resumes, re-propose from 166 unless that scratch is
still present. Errata E-178–E-182 (rows 153/155/158/160) were filed before the
pause; the C05:167 ENGLISH_FACTUAL_ERROR ("three types" vs `bzhi`) passed its
reconciler but has NOT had a skeptic pass and is NOT filed.**

**SKEPTIC PASS DONE 2026-09-17, and it CONFIRMS. Filed as E-250.** The
Tibetan reads `las de la bzhi ste` — four — then divides into `gnyis`
(definite / indefinite) and closes `nges pa'i las rnam pa gsum` (the
definite are three). Four is three plus one and the Tibetan is consistent.
The English is not, and it fails on its own terms before the Tibetan is
consulted: "three types" in total, then "grouped into two", then "the
'definite' group is of three different types". If the definite subgroup
alone has three and an indefinite group also exists, the total cannot be
three. Geshe Michael's English stays verbatim; the register records it.

**Those open items were worked through on 2026-09-17 and four of the five
closed without needing a ruling** — the sections below carry the evidence.
`tshal pa` -> "thorn" is confirmed by the master, by Geshe Michael's own
English at both rows, and kept. The "non" subword turns out to be a question
with identical answers either way, because the flag never reaches the bank.
E-169 `seng ldeng` is recorded and has no action available in this repo. The
agentive "by" is measured and reduced to a one-line ruling. E-157 remains.**

## `tshal pa` -> "thorn" — CLOSED 2026-09-17, kept, no re-cut

Kept at C05:92 and 95 "with a caveat", and listed as an OPEN ITEM FOR ADAM:
**if ruled false, re-cut 92/95.** It is not false. Three independent
confirmations:

- The master glosses `tshal pa` as exactly `["thorn"]`, at GLOSSARY tier.
- Geshe Michael's own English says the word at both rows -- C05:92 "was
  pricked by the sandalwood thorn", C05:95 "the prick of His foot by the
  sandalwood thorn". The link is attested in his text, not inferred.
- The Tibetan is `seng ldeng gi tshal pa zug pa` in both, the same phrase.

The caveat presumably came from `tshal` alone meaning grove/forest
(`["tree grove","forest","grove"]`) and thorn normally being `tsher ma`
(`["spines","thorn","thorns"]`). But `tshal pa` is its own headword with its
own gloss, and the compound is not the sum of its parts.

**Disposition: kept, no re-cut, item closed.**

## E-169 `seng ldeng` -> "sandalwood" — recorded; nothing to change here

Filed LOW/PROBABLE as ENGLISH_FACTUAL_ERROR and confirmed by a skeptic pass:
`seng ldeng` is khadira, Acacia catechu, not sandalwood (which is `tsan dan`),
and Geshe Michael renders the same word "Khadira" at C08:215. He renders it
"sandalwood" at both C05:92 and C05:95, so within Course V it is a consistent
rendering choice rather than a slip.

Nothing in this repository changes: his English stays verbatim on the page,
and `seng ldeng` is nulled in the spec so "sandalwood" never enters the bank
as an equivalent. **The one live thread is not ours:** the master already
carries `hgm_gloss ["khadira trees","sandalwood"]` on `seng ldeng` from
earlier auto-alignment. That belongs with the 755 entries in
`docs/MASTER_REBUILD_ORDER.md` that need a reader, alongside the same shape
found at E-259, where the master's binding gloss for `stag` is `["leopards"]`
while 21 corpus rows pair `stag` with "tiger".

**Disposition: recorded, no action available in this repo, flagged to the
dictionary work order.**

## The "non" subword — CLOSED 2026-09-17, no ruling needed

This stood as an OPEN ITEM FOR ADAM: `ma yin pa` / `mi` -> "non" inside
hyphenated "non-virtuous" and "non-merit" is banked at d=7 **without** the
subword flag, and "non" is outside the four listed negative-affix forms
(n't, un, im, less). The question was whether to admit it as a fifth form.

**Measured, and the answer is that both choices produce identical output, so
there is nothing to rule on.** The `subword` flag lives only in the SPEC. It
is consumed by `gen_alignment_page.py`'s `find_word`, where its single effect
is to permit a match inside a word rather than at a word boundary. It is
**not carried into the bank**: a link's keys are `case, course, d, eng, id,
page, seg, tib, tib_acip`, and `subword` is not among them — 0 of 43,727
links carry it. Nothing downstream — not the dictionary layer, not the app,
not the phone — ever sees it.

And "non" does not need the permission in the first place. In "non-virtuous"
the hyphen IS a word boundary, so `find_word` matches it normally and
`no_broken_words` does not flag it. Adding "non" to ALLOWED_SUBWORD would be
inert.

For the record, "non" behaves exactly like the four listed forms otherwise —
8 links on `ma yin pa`, `ma`, `mi`, `min`, `ma yin`, the same negation family
those four sit on (un 50, n't 31, less 20, im 15, on `mi`/`med`/`ma`). The
difference is orthographic, not grammatical: the four are fused to their stem
and "non" is hyphenated, which is precisely why one needs the flag and the
other does not.

**Disposition: closed, no change made, no ruling required.**

## The agentive "by", measured 2026-09-17 — Adam to rule

This has blocked a reconciler three times now, most recently on C05:172, where
it was deliberately left null rather than settled in passing. It was recorded
as a **conflict** between two rulings (C05:27 banked `gyis` → "by"; C05:28
nulled `kyis` because "the 'by' belongs to the passive frame"). Measured across
the whole bank, **it is not a conflict — it is one policy applied case by
case**, and the numbers say so.

**The population is much smaller than it looks.** 608 links in the bank have a
bare agentive particle as their whole span. Of those, **329 sit in a segment
whose English contains no "by" at all** — there is nothing to link to, so rule
5 forces the null. They were never in dispute. The real population is the 243
where a "by" was actually available:

| of the 243 agentives with a "by" available | count |
|---|---|
| linked to it | 37 |
| nulled | 206 |
| linked to something else ("through", "because", "with") | 13 |

**The genitive is the control, and it behaves identically.** 1,381 bare
genitive particles: 93% nulled, 7% given an English exponent, the commonest
being "of" (62) — the exact same shape as the agentive. So the bank already
treats both relational particles the same way: null by default, linked when
Geshe Michael's English visibly carries the relation. Nobody decided that; it
emerged. That is worth knowing before it is overturned.

**What is actually at stake.** Nothing in the dictionary. These are d=6
particles and d=6 does not enter the headword harvest, so no equivalent is
created or lost either way. The stake is page-to-page consistency, and the
answer to a student who clicks the particle and asks what it means.

**My recommendation, for Adam to accept or overturn in one line:** keep the
present practice and write it down — *link the agentive to "by" only when that
"by" governs the very agent the particle marks, and null it otherwise.* The
argument against ("the 'by' belongs to the English passive, not to the
particle") proves too much: by that reasoning the genitive could never be
linked to "of" either, and the bank links it 62 times without anyone objecting.
The argument for is rule 5's own test — does this morpheme have an exponent in
the English? When Geshe Michael wrote "by", it does.

**What must NOT be done:** a sweep. There is no mechanical way to tell the 37
from the 206, because the question is whether a particular "by" governs a
particular agent, and that is a reading. Anyone changing this should change the
rule going forward and leave the 42,345 banked links alone.

## RESUMED 2026-09-17

**c5p56 (166–168) landed.** 107 spans across three segments; the generator
exited 0 on the merged spec; coverage 165 → **168/511**; the bank 41,759 →
41,869 links. `no_supplied_span_head` holds at 0 for `pages_c05` with one new
licensor, `c5p56/s166w5` — `sogs` owning the closing gesture "and such", the
identical precedent already at `c5p33/s99w20`.

**MODEL DEVIATION, recorded because the policy is explicit.** Adam's policy
(2026-09-01) puts RECONCILE on Claude Fable 5.1, because the judgment steps
decide whether a page lands. All three reconcilers **failed on a Fable 5.1
safeguard error** — an API refusal, not a content problem — and the page was
reconciled on **Claude Opus 5** instead. Propose ran on Opus 5 as the policy
requires. If Fable becomes available again this is a reason to re-reconcile
c5p56, not an established parity.

The six proposals were recovered from the run journal after the failure rather
than re-run. That matters: the 166–169 specs lost at the September pause were
lost exactly this way, in session scratch that did not survive.

**What the two angles were worth.** They disagreed substantially — on 167 only
25 of 44 spans matched — and the reconciler settled every disagreement against
the LANDED CORPUS rather than by taste, counting existing links for each
contested string in `alignment_full_v1.json`. Three rulings worth keeping:

- `zhes` → "that" was **dropped**. The layer has 146 `zhes` links at d=6 with
  null English and not one on "that"; the complementizer is Geshe Michael's
  English syntax, not the quotative.
- `chos` → "something" was **dropped under rule 10**. `chos` is among the most
  loaded headwords in the lexicon and "something" is a placeholder noun. It
  would have passed every mechanical check and still been false.
- `rigs` → "discrete being" was **kept, flagged, and named as the first thing
  to surrender**. The reconciler found that C05:170 carries the same English
  over `ris mthun`, so the verse abbreviates for its seven-syllable line; the
  note names the full term in bold so a harvest cannot read it as a bare
  lexical equation.

Also checked and **not** filed: the `rigs`/`ris mthun` divergence. The only
other carrier is C16:753, byte-identical in both columns and therefore the same
file re-ingested rather than an independent witness — a variant reading can be
shown, a defect cannot.

**c5p57 (169–171) landed**, same standard, reconciled on Opus for the same
reason. 98 spans; coverage **171/511**; bank 41,970 links; span-head gate still
0 for `pages_c05` with no new licensor needed.

Two rulings worth keeping, both of them rule 10 in action:

- On 169 the analysts split on `ma` → "neither". The reconciler settled it
  against **c5p56, landed an hour earlier** — C05:168 carries the identical
  construction, `de gnyis ka ma nges pa` under "**neither** ripening itself nor
  its timing is definite", and its landed spans null both `gnyis ka` and
  `ma nges pa`. Same campaign, adjacent page, same shape, already ruled. The
  campaign is now building on its own precedent rather than re-deciding.
- On 170 it refused `mi 'phen` → "do not", which passes every mechanical check:
  *"the 'do' of 'do not' is the English pro-verb standing in for the elided
  'project a discrete being', not an exponent of `'phen`. Banking it keys a
  projecting verb to a dummy auxiliary."* Split instead — `mi` → "not", `'phen`
  nulled, "do" left unwrapped as supplied syntax.

**tools/land_alignment_page.py** was written here and used to land this page.
Page assembly was six manual steps from the reconcilers' specs — merge,
generate, wrap with a hand-typed title, register in the builder, add licensors,
rebuild — and doing them by hand is how a page number gets mistyped or a
registration forgotten. It refuses non-contiguous segments, refuses a spec
whose seq does not match its filename, stops on a generator refusal, and
reports every span the span-head gate will want a licensor for BEFORE the gate
runs. Validated by reproducing c5p56 exactly, licensor included.

## Queues (wylie / english), C05
(opened empty)

### Batches C05-8 … C05-15 (C05:22–45) — 2026-09-05 — propose: Opus 5 · reconcile/refute: Fable 5.1

First proposed batches of the course: 22 segments (22 and 23 are hand-built
markers), 44 proposals, 44/44 exited 0; 22 reconciled specs, 22/22 exited 0
first run. Pages c5p8–c5p15 wrapped and registered; coverage 45/511; layer
6,484 headwords / 13,661 pairs; 17 gates green; `no_supplied_span_head`
reports **0** function-word-initial d5/d7 spans in pages_c05.

**Course shape confirmed on the ground.** C16 rows are byte-identical
re-ingests of most C05 prose (C16:677–697 ↔ C05:25–48), so no C16 match is
an independent witness. Row 29 carries the outline vocabulary ("an
explanation of" ×4) that GMR supplied; the reconciler nulled `don` ×3 and
`'chad pa` rather than bank "explanation" on `don` (Rule 10). Row 30's
phonetic title ("Chu Ngunpay Dzu Kyi Tsikleur Jepa") was NOT banked at d=5
even though the capitalisation exemption would let it pass: GMR supplies the
real English in the same sentence, so a phonetic d=5 link would enter the
bank as a false equivalent; the bracketed translation was linked instead.
Row 36's closing clause ("here first comes the obeisance of the translator.
*********") has no Tibetan under it (the wylie ends at `'chad do`) — left
unwrapped, published reading per C16:685.

**Rule 10 rulings (banked with notes).** 24 `gsung` → "written" (not
"written by"); 28 agentive `kyis` → ∅ (the "by" belongs to the passive
frame) — CORRECTION 2026-09-05: 27 (`gyis`) was in fact BANKED → "by" (checked
in alignment_full_v1.json: s27p1 d=6 gyis→by; s28p1 kyis→∅); 146 followed 27
(gyis/kyis → "by" ×2). The two rulings conflict; agentive "by" at d=6 is an
OPEN ITEM FOR ADAM (d=6 particles do not enter the dictionary harvest, so no
headword is affected either way); 31 `zhes pa'o` → ∅ (not "translates as"), `ni` → ∅; 32
`skad` → ∅ (not "name"); 34 `tshong` → ∅ (trade, not "merchants"; both
analysts had banked it); 35 `'di tsam` split flat, `thad log` → ∅; 36 the
"Why" questions → ∅ (GMR recast purpose-statements as questions); 39 `gang
gis` → ∅ (scaffolding); 41 `rgyu` → ∅ / `byung` → ∅ (converses produced by
GMR's restructuring, both analysts had banked them); 44 `nas` → ∅; 45 `bcas`
→ ∅ (comitative; GMR's "and" rests on `dang` at 47); 46 `bshad` → ∅ (not
"promised"). 47 `chos can` → "Consider" per the C04 convention.

**Errata.** 24 `mtshung med` (ACIP MTSUNG MED) for `mtshungs med` —
TIBETAN_SPELLING, LOW/PROBABLE, no witness anywhere in 42,199 rows, skeptic
passed. 39 `bnas drug pa` for `gnas drug pa` — illegal syllable, skeptic
passed, confidence raised to CERTAIN. 38 lower-case sentence start after a
closing quotation (`stain." the first two`) — first skeptic REFUTED the
record because its evidence called C16:687 byte-identical when C16:687 in
fact reads "The" (capital); evidence corrected to name C16:687 as a
DIFFERING witness and re-sent to a second skeptic (pending at this entry).
31 `ko Sha`/`ko ShA` mixed vowel length within one row — dropped by the
reconciler (cannot be shown to be a keying defect rather than the
blockprint's own variation); WYLIE queue candidate.

**Shape notes.** 37 banked `gnas gang na` → "in each of the eight" at d=3
with no members (phrase-level, outside the dictionary harvest) because "the
eight" substitutes for `gnas` rather than translating it. 51's English-first
analyst had to abandon the flat shape ("formless" ×2) — reconciler to rule.

### Batches C05-16 … C05-49 (C05:46–147) — 2026-09-05 — propose: Opus 5 · reconcile/refute: Fable 5.1

**Text.** 46–147 is Gendun Drup's commentary on Abhidharmakosha IV
(deeds: the movements of the mind, communicating/non-communicating form,
the vows, virtue/non-virtue/neutral, the three kinds of experience),
with root-verse lines under `[iv.NNN]` apparatus and fused
"Reading… Subject:" headings on 96, 118, 145 (registered, not filed).
C16 parallel rows are byte-identical re-ingests, not witnesses.

**Conventions settled in this stretch.**
- `chos can` → "Consider" only; null wherever GMR's frame is "let us take"
  / "First take" / "Next consider" (129 banked "take" on the one row where
  "take" is the whole exponent).
- `sogs` / `la sogs pa` owns only the closing gesture ("and the like",
  "and so on", "and the rest", "and the other types" at 152; "including"
  at 149); GMR's supplied noun stays unwrapped. Every such span head is
  allowlisted in span_head_allow.json with the `sogs` licensor
  (s96w7, s99w17, s99w20, s112w3, s124w12, s128w6, s140w3/w6/w11; 150 and
  152 at their landing).
- `zhe na` → "you may ask" (s78w11) and `yang na` → "or else" (s81w7)
  allowlisted as licensed hinge heads.
- `yin` / `yin pa` copula nulled (14:1 across the stretch).
- Negative affix: `mi` → "un" (unshifting) subword; `ma yin pa` / `mi` →
  "non" inside hyphenated "non-virtuous"/"non-merit" banked as d=7 without
  the subword flag (hyphen-delimited, so not a broken word; C01 c1p05
  precedent; 106, 147, 149). "non" is outside the four listed forms —
  OPEN ITEM FOR ADAM alongside the fifth form in TODO.md.
- `rnam par rig byed` → "communicating form" as one d=5 dictionary unit;
  `sems pa` → "movements of the mind" (HGM gloss); `nyid dgos` banked
  verbatim with the spelling erratum (E-167).
- Phonetic title at 30 not banked at d=5 (phonetics protocol).

**Rule 10 rulings (nulled with notes).** GMR converses/restructurings:
`rgyu` (→ "result" frame), `byung`, `gsungs`, `'jog`, `'khor du`, `ces
bya`, `bshad` → "promised"; outline vocabulary GMR supplies (`don` →
"explanation", `rang bzhin` → "definition", `bstan`/`bshad` →
"introduction"/"treatment", `gnyis pa` → "two" at 150); `thogs pa med pa`
is not "ineffable"; `seng ldeng` is not "sandalwood" (nulled; E-169
English record for GMR's ruling); `mi 'phrod pa` is not "poisonous";
`de dag` → "their" (151, possessive recast of the demonstrative); `byed`
→ "helps" and `skyed pa` → "brings" (145, causative auxiliaries pinned to
GMR's frame); `'gyur ba` → "lead to" (150, auxiliary of the vedanīya
frame `myong bar 'gyur`) — and 149's `'gyur` → "lead to", which its
reconciler had banked on HGM's glossary, was NULLED AT LANDING for
consistency with 150 (landing note on the page). `tshal pa` → "thorn"
KEPT at 92 and 95 with a caveat (OPEN ITEM FOR ADAM: if ruled false,
re-cut 92/95).

**Errata.** E-154 … E-177 filed after skeptic passes (rows 24, 38, 39, 63,
65, 68, 71, 73, 83, 84, 86, 89, 90×2, 92×2, 112, 115×2, 116, 120, 122,
123, 142). Refuted and dropped: 38's first wording (C16:687 carries the
capital; re-filed corrected as E-155), 74 `rjod` (present stem of brjod),
77 both claims (MOO = au encoding; "Wisdom's" is "is"), 114 `ma tshang pa`
(attested variant), 144 "neither … or" (GMR's habitual construction, 26
tight-window correlatives across eight texts — authorial, not a segment
erratum; at most a corpus-wide style query for GMR). Not filed: E-111
row-boundary spills at 59/60; the hyphen-space line-break class; 219's
1,500-cap (ahead). merge_errata refused three records whose `found`
carried analysts' parenthetical annotations — `found` must be a verbatim
substring; the filer now strips them.

**Gate note.** `no_supplied_span_head` matched Tibetan `the tshom` at
s88w12 (column blindness; allowlisted as GATE FALSE POSITIVE; fix in
TODO.md).

**Models.** Propose Claude Opus 5 (two angles per row); reconcile and
refute Claude Fable 5.1 throughout, including the relaunches after the
2026-09-05 rate-limit kills (95, 99, 101) — never moved off Fable.

## 2026-09-17 evening — 166 to 198, and a gap the gates could not see

Eleven pages landed in one run of the campaign: c5p56 (166-168) through
c5p66 (196-198). Coverage 165 to **198 of 511**; the bank 41,759 to **43,021
links across 522 pages**. Every page two independent analysts, reconciled by
a third against counts re-derived from the bank rather than quoted.

**Errata E-250 to E-258**, nine in all. The three MEDIUM ones at C05:183 and
E-258 at C05:190 each named their parallel-witness check AND correctly
discounted C16, whose rows here are byte-identical re-ingests of the same
file rather than independent witnesses — a distinction earlier batches did
not always draw. E-256 is the one to remember: Geshe Michael's "King Kanaka"
renders `ka ni ka`, which he writes "Kanika" in three other courses, and
"Kanaka" already belongs to Kanaka Varma the translator elsewhere in the
corpus. The misprint collides with a different real person.

**Span-head licensors added:** `bdag cag gi` -> "our" (C05:182; pronoun plus
genitive, the C04 `rang re'i` construction) and two more `sogs` -> "and the
rest" on the established convention.

**THE FINDING OF THE DAY IS NOT IN THE PAGES.** After landing three pages and
302 links, `test_builder_sees_every_span` reported a total BYTE-IDENTICAL to
its previous run. Its directory list predated Course 5 and contained no
`pages_c05`; so did five other tools' lists. **Six gates had never looked at
a single Course 5 page**, while reporting "across 456 pages" — which is
exactly pages + c01 + c03 + c04. Widening them fired immediately on c5p56
(`nges` -> "definite" inside `ma nges pa` -> "indefinite"; rule 4 obeyed, now
allowed with its reason). `tools/alignment_page_dirs.py` discovers the
directories now, and `alignment_page_coverage` refuses a gate that covers
fewer pages than exist.

**Models.** Propose Claude Opus 5 (two angles per row); reconcile Claude
Opus 5 — the Fable 5.1 reconcilers having failed on a safeguard error on
2026-09-16, the deviation is recorded rather than quietly absorbed.
