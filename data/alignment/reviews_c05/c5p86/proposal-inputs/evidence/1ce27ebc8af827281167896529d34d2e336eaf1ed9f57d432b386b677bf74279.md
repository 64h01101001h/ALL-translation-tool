# C05 CAMPAIGN — ACI Course 5 (How Karma Works), full-depth alignment

Opened 2026-09-05, immediately after C04 closed (198/198, 2026-09-04),
under Adam's standing order to scan C05 and then C06. Briefs are the
course-parametrised set in `docs/alignment_briefs/`; the task names the course.

Standing rules (Adam): GMR's English stays VERBATIM, typos flagged never
corrected; nothing binding without human review; every batch records its
actual producer (historical campaign: Claude Opus 5 / Fable 5.1; resumed
2026-09-12 campaign: Codex under Adam’s explicit handover); completeness is proved by the builder's COVERAGE GATE, never asserted
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
c5p1–c5p7 (1–21) cloned 2026-09-05; c5p8–c5p49 (22–147) landed 2026-09-05; c5p50–c5p55 (148–165) landed 2026-09-08. c5p56 (166–168) landed 2026-09-12 after fresh independent proposals, reconciliation and skeptical review. c5p57 (169–171) landed after independent review. c5p58 (172–174) landed after independent review. c5p59 (175–177) landed after independent review. c5p60 (178–180) landed after independent review. c5p61 (181–183) landed after independent review. c5p62 (184–186) landed after independent review. c5p63 (187–189) landed after independent review. c5p64 (190–192) landed after independent review. c5p65 (193–195) landed after independent review. c5p66 (196–198) landed after independent review. c5p67 (199–201) landed after independent review. c5p68 (202–204) landed after independent review. c5p69 (205–207) landed after independent review. c5p70 (208–210) landed after independent review. c5p71 (211–213) landed after independent review. c5p72 (214–216) landed after independent review. c5p73 (217–219) landed after independent review. c5p74 (220–222) landed after independent review. c5p75 (223–225) landed after independent review. c5p76 (226–228) landed after independent review. c5p77 (229–231) landed after independent review. c5p78 (232–234) landed after independent review. c5p79 (235–237) landed after independent review. c5p80 (238–240) landed after independent review. c5p81 (241–243) landed after independent review. c5p82 (244–246) landed after independent review. c5p83 (247–249) landed after independent review. c5p84 (250–252) landed after independent review. c5p85 (253–255) landed after independent review. Coverage 255/511; current verification is recorded in the latest batch entry below.

**Historical pause, superseded by Adam’s 2026-09-11 continuation request. PAUSED 2026-09-08 (Adam's call): the campaign stops here while the app goes to
a public release (two-week target). Reconciled-but-unlanded specs for 166, 167,
168, 169 and proposals for 170–175 exist only in session scratch and are NOT
banked; when the campaign resumes, re-propose from 166 unless that scratch is
still present. Errata E-178–E-182 (rows 153/155/158/160) were filed before the
pause; the C05:167 ENGLISH_FACTUAL_ERROR ("three types" vs `bzhi`) passed its
reconciler but has NOT had a skeptic pass and is NOT filed. Open items for Adam
(agentive "by", "non" subword, tshal pa, seng ldeng E-169, E-157) stand.**

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


### c5p56 · C05:166–168 — 2026-09-12 — Codex

Resumed under Adam’s explicit instruction to continue through Tuesday, September 15,
07:00 America/Denver. Two fresh independent proposal angles, a separate reconciler,
and a separate skeptical semantic reviewer examined all 85 retained spans.
Accepted reproducible spec: `specs_c05/c5p56.json`; full review evidence and actual
producer credits: `reviews_c05/c5p56/`. All three individual generators and the
assembled page generator exited 0 with empty stderr. Page keeps the original
source and PROVISIONAL label.

C05:167’s initial `bzhi`/“three” disagreement remains visible and unpaired; later
`gsum` owns the correct later “three.” Recast temporal expressions remain d3 rather
than false dictionary units. Genuine lexical ellipsis is distinguished from an
unresolved mapping. `la sogs pa` owns only “and such,” licensed by `sogs` in
`span_head_allow.json`. Zero new errata.

Registry coverage: 168/511. Full bank: 41,847 links (+88: 85 reviewed spans plus
three generated sentence links), 11,791 null English exponents. Evidence: 6,761
headwords, 14,546 pair records. The dictionary HTML and CSV exports were rebuilt.
New page credits name Codex; the mixed bank credits its builder and refers to
source-page producers. The new page defines the previously missing depth-7
slider label. Added the absent C05 index and connected c5p55 → c5p56 navigation.

All 16 targeted CTest fidelity/constitution suites passed (10.93 seconds).
Structural checks found zero broken local links in the C05 index, previous page
and new page; all seven depth labels are defined. These checks are recorded beside the durable
batch in `campaign-artifacts/C05-166-168/landing/`. Browser preview remains
unverified: the in-app browser’s URL policy blocked local HTML; no workaround
was used. This limitation does not change the source-verification evidence or
turn machine semantic review into human acceptance. Desktop/iOS integration is
being checked separately; no deployment claim is made for this batch yet.


### c5p57 · C05:169–171 — 2026-09-12 — Codex reconciliation/review

Recovered historical Tibetan-first/English-first proposals were preserved and
all six rerun through the current generator (exit 0). A fresh Codex reconciliation
and a separate Codex skeptic reviewed every one of the 87 retained spans.
The three individual specs and assembled page exit 0 with no stderr. Accepted
spec: `specs_c05/c5p57.json`; complete review and source evidence:
`reviews_c05/c5p57/`. No new function-head allowlist entries were required.

The four-way classification at 169 preserves each correct repeated English
occurrence. Distributed negatives and the experienced-in-this-life category
stay phrase-level; they are not false dictionary atoms or absence claims.
170 `gnyis pa` → “Next” is restored. The proposed correction of `'phan` was
rejected after the current HGM glossary and other source uses corroborated it.
171 retains genuine compositional members for `dge ba'i`, `der`, and
`mi brtan pa` without confusing separate “stable” occurrences.

Filed E-183, C05:171 `skyes myod mi byed` → probable `skyes myong mi byed`,
LOW/PROBABLE after all five independent refutation checks. ACIP and Wylie
agree on the observed spelling; internal verse and commentary support the
restoration. No independent witness or blockprint was found, and TCS10:338
disproves an old uniqueness claim. Source text remains unchanged.

Coverage 171/511; full bank 41,937 links (+90, including three sentence links),
11,804 null English exponents; evidence 6,768 headwords / 14,567 pair records.
Dictionary HTML/CSV and both errata-register forms were rebuilt. The master
dictionary and original corpus SHA-256 values match the campaign baseline.
Structural checks verify the new/previous page navigation and C05 index;
actual bank records contain the expected 27/30/33 links for 169/170/171.
All 16 targeted CTest fidelity/constitution suites passed (11.13 seconds);
results are preserved in this batch’s durable landing directory.
Browser rendering remains unverified under the recorded URL-policy block.


### c5p58 · C05:172–174 — 2026-09-12 — Codex reconciliation/review

The six recovered historical independent proposals all reran successfully.
Fresh reconciliation and a separate skeptical reviewer examined all 89 final
spans (78 linked, 11 grammatical nulls). Parent reruns and the assembled page
also exit 0 with empty stderr and exact bodies. Reproducible spec and portable
review evidence: `specs_c05/c5p58.json`, `reviews_c05/c5p58/`. No additional
function-head allowlist entries are needed.

Restored overt locatives, genitives and negatives that earlier proposals had
called absent or ambiguous. Ordinary numeral/noun and locative phrases remain
outside dictionary depth. 173’s whole-word “no” and sentence-initial “For”
have unambiguous intended occurrences. At 174, `gtan nas mi` jointly owns
“never” at phrase depth; the emphatic component is not falsely labelled null.
Distributed or unresolved material stays unwrapped with a reason.

Filed E-184, the C05:172 “In the hells through…” → probable “though…” typo,
LOW/PROBABLE after all five independent refutation checks. The original ASCII
reading at line 1553 confirms the source spelling; it is not an independent
witness. Searches found no independent parallel to this commentary. The source
is unchanged and the broken adversative remains unpaired.

Coverage 174/511. Bank: 42,029 links (+92, including three sentence links),
11,815 null English exponents; evidence 6,770 headwords / 14,580 pair records.
Generated dictionary and errata documents rebuilt. Structural checks found no
broken new-page/index navigation and confirmed bank counts 35/28/29 for the
three new segments. All 16 targeted CTest fidelity/constitution suites passed
(10.93 seconds). Browser inspection remains unverified.

Consumer checkpoint: native app selftest passed with the bank through 171,
after private-library setup and the Catalog routing test’s temporary-store
fix (929f012). Signed iPhone pack through 171 was built, verified, and installed;
launch is pending because the phone was locked. Newer alignment bank rows
are not claimed present in that installed pack.


### c5p59 · C05:175–177 — 2026-09-12 — Codex reconciliation/review

Both historical proposal angles for 175 and both fresh Codex angles for 176–177
were preserved and rerun successfully. Separate reconciliation and skeptical
review approved all 97 final spans (83 linked, 14 grammatical nulls), including
53 d5/d7 dictionary pairs. Accepted spec and exhaustive review:
`specs_c05/c5p59.json`, `reviews_c05/c5p59/`. Parent generator reruns and the
assembled page exited 0 with empty stderr and exact bodies. No new errata or
function-head allowlist entries.

175’s complete loss-to-restoration interval stays d3, avoiding a false
positive atom for the negative restoration syntax. 176’s first demonstrative
is retained in its complete subject/predicate clause at d2; exclusive
`kho na` → “but” remains a particle, not an adversative. 177’s state-to-person
recast stays phrase-level, while the earlier attested `bar do` state term
remains at dictionary depth.

The whole 177 definite/indefinite opposition is retained at d3 after explicit
challenge: the elided English predicate can include “definitely,” and 178
resumes the eleven definite cases. The ambiguity note MUST remain. This is
not a dictionary equation of `ma nges` with “not experienced”; higher depth
was not accepted as a reason to ignore a contradiction.

Coverage 177/511; full bank 42,129 links (+100, including three sentence links),
11,829 null English exponents; evidence 6,774 headwords / 14,593 pair records.
All 42,029 prior links and every prior note/tree page value are unchanged.
Structural checks verify navigation, all seven depth labels, and expected
new segment counts 45/19/36. All 16 targeted CTest fidelity/constitution suites
passed in 11.08 seconds. Master dictionary and original corpus SHA-256
values still match the starting snapshot. Browser rendering and native
consumption of this newest bank remain separately unverified; the tested
desktop/installed-phone checkpoint is through 171.


### c5p60 · C05:178–180 — 2026-09-12 — Codex reconciliation/review

Two fresh independent proposal angles, separate reconciliation and an independent
skeptical reviewer approved 104 analyst spans (92 linked, 12 grammatical nulls),
including 69 d5 dictionary pairs. All six proposals and all final reconciliations
reran with exit 0, empty stderr and exact bodies. Accepted spec, exhaustive
per-span judgments, omissions, source positions and evidence are preserved in
`specs_c05/c5p60.json` and `reviews_c05/c5p60/`. No function-head exception is needed.

Restored both temporal `gnas skabs kyi`/when phrases and distinct copulas at 178,
the full-list relative and final copula at 179, and the concluding relative at
180. The technical `mthong chos` unit stays lexical; ordinary temporal/genitive
recasts remain phrases. `shugs drag po` retains the complete fierce-emotions
phrase with direct glossary support. Initial 180 `myong` stays omitted as
distributed meaning, without a false absence claim. The agentive `gis`/by
mapping at d6 follows an existing precedent and leaves the older policy question
open. A temporary awaiting-review note prefix was removed before final review;
all final hashes and generator reruns reflect that correction.

E-185 records 180 MYON MONGS PA → NYON MONGS PA as LOW/PROBABLE after all five
independent refutation checks. The root verse, glossary and distinct SL05525N
digital reading support the candidate; the identical SE aliases and C16 re-ingest
preserve MYON. Digital publication independence is unestablished; no blockprint
was inspected. PHUNG BO is attested and was not filed. All source spellings remain
unchanged and no hgm_gloss was edited or promoted.

Coverage 180/511; full bank 42,236 links (+107, including three sentence anchors),
11,841 null exponents; evidence 6,784 headwords / 14,618 pair records. All 42,129
prior links, previous notes/trees, evidence references and ACIP mappings remain
intact. Reviewed tuples and source ranges exactly match the landed spec; source
hashes and navigation targets check. Both banks, the dictionary view, four CSV
exports and the errata document were rebuilt. All 16 targeted CTest fidelity and
constitution suites passed in 10.45 seconds. Rendering remains unverified;
native/installed-phone verification remains at the earlier 171 checkpoint.


### c5p61 · C05:181–183 — 2026-09-12 — Codex reconciliation/review

Two independent proposal angles and separate reconciliation/semantic review
approved 101 analyst spans (85 linked, 16 grammatical/fusion nulls), including
52 d5/d7 pairs. Every tuple, null, source gap and repeated English occurrence
was checked. Accepted spec and evidence are in `specs_c05/c5p61.json` and
`reviews_c05/c5p61/`. Final actual generators exited 0 with empty stderr and
exact bodies, including the corrected 182 spec/body hashes. No allowlist addition.

An upheld overcapture narrowed 182 bsdums from bring the different sides to an
agreement to agreement. Explicit participant wording stays unwrapped; greater
depth would not cure the defect. The first mastery predicate retains its relative
and past-perfect recast, with temporal na explicitly distributed into that
construction and the former-life frame. Canon-holder and five-hundred compounds
retain only defensible members. The shortened master title has a justified
no-separate-numeral null. All repeated monks, women, turned/into pairs and final
as were checked against actual positions. The old auto-aligned bsdums reference
conflicts with glossary bsdums pa/agreement; the master was not changed.

Both proposed 183 spelling corrections were refuted independently. SNYING RDZE
is glossary-attested; intact snying rdzes/with compassion remains a phrase.
DOD has a defensible wanting sense: the alternate DED reading and English driving
do not prove a unique error. Driving and the unresolved rescue construction stay
unpaired; a later causal pas is not forced onto an earlier occurrence. The
identifiable anaphoric cattle/fate phrases remain phrase-level, not dictionary
claims. No new erratum. The preceding E-185 citation was corrected to the exact
committed commentary-local-witnesses.json path; its substance and confidence
remain unchanged.

Coverage 183/511; full bank 42,340 links (+104, including three sentence anchors),
11,857 null exponents; evidence 6,799 headwords / 14,646 pairs. All 42,236 prior
links, prior note/tree values, evidence citations and ACIP mappings are retained.
All 21,101 prior CSV content records and citations survive. One older phonetic
row for dge 'dun/gendun gains its previously empty ACIP field from the newly
cited d5 C05:182 entry; that enrichment was traced to the exact new source link.
Other old CSV lexical fields are unchanged. Both banks, dictionary HTML and all
four CSVs were regenerated. All 16 targeted CTest fidelity/constitution suites
passed in 10.22 seconds; master/corpus hashes and navigation targets check.
Rendering and native consumption of the newer bank remain unverified; the earlier
native/installed-phone checkpoint remains 171. Next: 184–186, source snapshot only.


### c5p62 · C05:184–186 — 2026-09-12 — Codex reconciliation/review

Two fresh independent proposal angles, reconciliation and skeptical semantic
review approved 95 analyst spans (88 linked, 7 grammatical/fusion nulls), with
62 linked d5 word pairs and five tighter compound members. All twelve source-side
proposal inputs for this and the following batch were separately preserved;
this page's six proposals and three final specs reproduce exact generator bodies.
Accepted spec: `specs_c05/c5p62.json`; source/range/glossary evidence and the
preserved initial review: `reviews_c05/c5p62/`.

The upheld 185 q1 correction narrows de la/to any of the following at d3 to
la/to at d6. de remains unpaired as distributed list-reference, not nulled.
The broader glossary counterevidence was weighed explicitly; no universal claim
about de/following is made. Root applied only the selected span and its note;
scoped independent re-review approved final spec SHA-256
f46b79414e7fc076b23a9bc4dac093eddaf6f58f008c770abc69561f5c941fec and
body SHA-256 ffeaa66088c1f0c98c0f770873efe4f059329e994816d21f1fef1139acd92603.

184 retains both copulas, additive also, the result-bearing can/gives, and the
established present-life compound with a tighter seeing member. 185 keeps the
abbreviated technical list and distinct result occurrences. 186 recovers
langs/come out plus las/of at both emergence clauses; first and third just are
selected. Its complete personal-responsibility term is directly glossary-attested.
The privative free-of phrase stays d3, and very sharp excludes separately rendered
clear. Prospective/cause constructions are conservatively omitted with distributed
notes, never falsely nulled. No new erratum or allowlist addition. Source heading
glue and verse capitalization are preserved; C16 parallels do not establish
independent publication or prove an ingestion history solely through text identity.

Coverage 186/511; full bank 42,438 links (+98, including three sentence anchors),
11,864 null exponents, 1,522 note blocks, 11 trees. Evidence: 6,807 headwords /
14,673 pairs. All 42,340 prior links, notes/trees, evidence citations and ACIP
mappings survive; all 21,156 prior main CSV lexical rows and citations survive
without a new ACIP enrichment. Both banks, dictionary HTML and all four CSVs
were rebuilt. The main CSV has 21,198 rows; changes are 42 new rows versus70804c9.
The course CSV's 22,506 rows each match actual course/depth refs and metadata;
all 22,451 previous groups and their occurrence counts survive.

All 17 targeted fidelity, export and constitution suites passed in 10.46 seconds.
The extra suite is the repaired course-export regression; 118 remains a registered
suite count, not a full-suite execution claim. Original master/corpus hashes and
navigation targets check. The alignment spec now names the CSV artifacts and
this regression. Rendered/native/device limits remain at the earlier checkpoint;
no new device action or main integration occurred. Next: 187–189, already through
independent semantic review but not yet banked at this entry.


### c5p63 · C05:187–189 — 2026-09-12 — Codex reconciliation/review

Two fresh independent proposal angles, separate reconciliation and skeptical
semantic review approved 73 analyst spans (all linked, zero nulls), including
55 d5 word pairs and four tighter d7 members. All six proposal bodies and three
final individual bodies reproduce exactly; final review tuples and exact source
ranges agree with the landed spec. Accepted spec: `specs_c05/c5p63.json`;
source, glossary, skeptical errata evidence and generator proofs: `reviews_c05/c5p63/`.

The seeing/habituation elimination-category compounds are glossary-attested;
supplied undesirable is excluded. Emergence predicates, temporal just and
ablative of are separately licensed at their precise occurrences. The 189
nonreturner name and later predicate negation remain distinct. Complete negative
attainment and the contextual mental-illness topic stay at phrase depth; no
lexical illness gloss is invented for g-yengs. C16:768 repeats Tibetan/ACIP but
has a different English suffix; the proposals' stronger identity claim was
corrected in reconciliation and independently checked.

Ruling: retain the occurrence-level 'gyur/lead(s) approvals here, alongside the
conservative distributed omissions in 185/186 — both independent reviewers
explicitly weighed the construction and glossary #2598, and neither omission
asserts absent meaning. These are cited translation occurrences, not a universal
causation definition. The cost of a wrong correspondence remains human review
and reversible provisional-data correction, not modification of hgm_gloss.

E-186 is LOW/PROBABLE 'BRAL DU→'PHRAL DU at 187. Its original text and quick remain
unpaired, never corrected or nulled. The independent skeptic rechecked all five
grounds: actual source, same-passage witnesses, 185 prior register entries, lexical
sense and every numerical claim. The two SE filenames have identical bytes and
read BRAL; SL reads PHRAL. Raw contexts, hashes and folio @125A are preserved in
`reviews_c05/c5p63/commentary-local-witnesses.json`. Digital differences do not
prove publication independence or local ingestion corruption. All 185 previous
errata entries remain unchanged; the JSON and generated register are in sync.

Coverage 189/511; full bank 42,514 links (+76 including three sentence anchors),
11,864 null exponents, 1,525 notes, 11 trees. Evidence 6,812 headwords/14,696 pairs.
All 42,438 previous links, notes/trees, evidence/ACIP mappings and 21,198 prior main
CSV lexical records/citations survive unchanged, with no ACIP enrichment.
Rebuilt both banks, HTML and all four CSVs: main 21,229, reverse 20,889,
course/depth 22,539, changes 31 new rows versus bb77f01. Every course/depth row,
metadata field and count matches actual main CSV refs; all 22,506 prior groups
and occurrence counts survive.

All 17 targeted fidelity/export/constitution suites passed in 10.51 seconds.
Original master/corpus hashes, accepted body/spec hashes and navigation check.
Independent fixed-commit reproduction and full-campaign read-only main preflight
are pending the checkpoint review. No main integration or new native/device
execution occurred; actual rendering remains unverified after the browser policy
block. Next 190–192 has source/context snapshots only, no accepted proposals yet.


### c5p64 · C05:190–192 — 2026-09-12 — Codex reconciliation/review

Two fresh independent proposal angles, separate reconciliation and skeptical
semantic review approved 44 analyst spans (42 linked, two grammatical/fusion
nulls), including 30 d5 word pairs and six d7 members. All six proposal bodies
and three final individual bodies reproduce exactly. All accepted tuples and
source ranges agree with the landed spec. Accepted spec: `specs_c05/c5p64.json`;
review, source, glossary and generator evidence: `reviews_c05/c5p64/`.

This passage begins a Lamrim Chenmo excerpt, explicitly introduced at 190;
the older broad course-shape description above does not identify this section.
The RJES/rjes introduction candidate was refuted as underdetermined: rje+s
inflection remains possible. The original ASCII reading has the same spelling;
a second local copy is byte-identical, not an independent publication. The
located Tibetan PDF is a language study guide, not this reading. No correction
or Lord pairing is banked, and uncertain rjes is omitted rather than nulled.
The title's chen mo/Great Book member is explicitly glossary-supported. The
191 consequences occurrences are distinct; gyi and ni have reviewed fusion
and topic nulls. The 192 repeated counts and final born were checked against
actual resolver ranges. Three kinds remains a classification phrase at d3.
Only `c5p64/s192w10` is added to the head allowance: sogs owns the complete
closing gesture and the rest, supported by glossary 20919 and independent
review. All older allowance entries survive unchanged. No new erratum.

Coverage 192/511; full bank 42,561 links (+47, including three sentence anchors),
11,866 null exponents, 1,528 note blocks, 11 trees. Evidence: 6,816 headwords /
14,708 pairs. All 42,514 previous links, notes/trees, evidence citations and ACIP
mappings survive; all 21,229 prior main CSV lexical rows and citations survive
without ACIP enrichment. Both banks, dictionary HTML and all four CSVs rebuilt.
Main CSV: 21,249 rows; reverse: 20,909; changes: 20 new rows versus d21deed.
All 22,568 course CSV rows match actual course/depth refs and metadata; all
22,539 prior groups and counts survive. The 22 semantic evidence files were
copied byte-for-byte. Master, corpus and errata register remain unchanged.

All 17 targeted fidelity, export and constitution suites passed in 10.62 seconds.
The registered 118 suites were not all run. Navigation and source hashes check.
Alignment-page rendering remains unverified; no new native/device action or
main integration occurred. Next: 193–195, independently semantically approved
but not banked at this entry.


### c5p65 · C05:193–195 — 2026-09-12 — Codex reconciliation/review

Two fresh independent proposal angles, separate reconciliation and skeptical
semantic review approved 71 analyst spans, all linked, including 56 d5 word
pairs; no nulls or d7 members. All six proposal bodies and three final individual
bodies reproduce exactly. Accepted tuples and actual generator source ranges
agree. Accepted spec: `specs_c05/c5p65.json`; complete independent review and
rejected-attempt evidence: `reviews_c05/c5p65/`.

The 193 contrastive ni/however and tight ldog/reverse preserve their source
contribution. 194 keeps livelihood deprivation and problems keeping at phrase
depth; skur pa/don't believe is glossary-attested. The broader listening clause
was rejected because it captured supplied you; nyan pa/listens alone is secure.
195 retains the complete worker-group, trustworthiness, negative concord and
lack-of-confidence constructions at phrase depth. mi bsrun pa/inconsistent is
explicitly glossary-attested without normalization. Reduplicated gnyis gnyis/two
is a reviewed numerical exponent in already distributive scope, not a universal
non-distributive definition. Both harm predicates were checked with their
respective object and agent; supplied English passive auxiliaries stay outside.

The first 195 help attempt passed the generator but selected the first Tibetan
phan pa inside mi phan pa tshol ba'am and the second English help. The reviewer
independently reproduced that mismatch. The final spec omits this uncertain
pair; the broader seeking-unhelpfulness versus finding-oneself-without-help
agency recast remains unpaired. No null or broader phrase hides the uncertainty.
Exact attempted specs, ranges and output are preserved. Equal-English C09:152
and C17:499 have differing Tibetan/ACIP fields; C16:773 matches all three fields.
Digital duplicates are not claimed as independent publications. No new erratum
or head allowance; master, corpus and errata register remain unchanged.

Coverage 195/511; full bank 42,635 links (+74, including three sentence anchors),
11,866 null exponents, 1,531 note blocks, 11 trees. Evidence: 6,832 headwords /
14,741 pairs. All 42,561 prior links, notes/trees, evidence citations and ACIP
mappings survive. All 21,249 prior main CSV lexical rows and citations survive;
two formerly empty ACIP export fields gain source-attested forms: GNYIS GNYIS
from C05:195 and RMONGS from C05:194. These are spelling enrichments, not changes
to old translations or depth. Both banks, dictionary HTML and four CSVs rebuilt.
Main CSV: 21,297 rows; reverse: 20,957; changes: 48 new rows versus 827fd9b.
All 22,622 course CSV rows match actual course/depth refs and metadata; all
22,568 previous groups and occurrence counts survive.

All 17 targeted fidelity, export and constitution suites passed in 10.53 seconds.
An accidental second export invocation reset the changes delta; it was rebuilt
once from 827fd9b's previous main CSV, with both banks, HTML and other CSV bytes
proved identical. The export, view/layer and generated-doc suites passed again.
No new canonical exporter change was needed. Navigation and source hashes check.
Rendering remains unverified; no newer native/device execution or main integration
occurred. Next: c5p66, C05:196–198, with source/context snapshots only.


### ERRATA to c5p65 review description — 2026-09-12

The c5p65 entry above and its initial semantic review incorrectly describe the
rejected attempt's English help as the second occurrence. Fresh source offsets
and the actual resolver prove FIRST Tibetan phan pa [564,571) paired with FIRST
English help [1083,1087), inside without help. The second occurrences are Tibetan
[584,591) and English [1107,1111). Reconciliation's first-English description was
correct. Independent integration review identified this evidence-description
error; the original and corrected semantic reviews and scoped factual re-review
are preserved under reviews_c05/c5p65. No accepted span, source range, null,
bank, dictionary output or errata-register record changes. The final omission of
the disputed help pairing remains approved. This correction concerns the machine
review account, not an error in GMR's source.


### c5p66 · C05:196–198 — 2026-09-12 — Codex reconciliation/review

Both independent proposal angles, separate reconciliation and fresh skeptical
semantic review completed. APPROVE covers 53 analyst spans (18/21/14), two
explicit grammatical fusion nulls, 41 d5 pairs and six d7 members. All six
proposal bodies and three final bodies reproduce exactly with zero stderr;
all final tuples and actual source ranges match the accepted spec. Evidence:
`specs_c05/c5p66.json` and `reviews_c05/c5p66/`. Machine approval remains
PROVISIONAL and does not constitute human acceptance.

196 keeps the second English killing, separate pleasure/become predicates,
both established result compounds and their tighter members. 197 retains the
complete synonymous result-category phrase, digest/disease, and the complete
premature-death recast; nutrition/potency correctly cross in English. 198
anchors the first, second and fifth Tibetan crop nouns to the three English
crops. Unproved long ba/non-emergence remains omitted, without a false null.
All proposed source corrections are refuted; no errata or allowances are added.

The semantic review explicitly supersedes the 198 report's unsupported
“known re-ingest” wording: identical source fields do not prove ingestion
history or publication independence. Two differently named local S05392L files
have identical relevant contexts and a 1,239,948-byte common suffix. Their first
difference at byte167559 lies in body formatting, not a header; they remain
unequal after whitespace removal. Root's earlier different-headers description
is explicitly corrected. No claim of independent publication is made.

Coverage 198/511. Bank:42,691 links (+56 including three sentence anchors),
11,868 null exponents,1,534 notes,11 trees. Evidence:6,845 headwords/14,768 pairs.
All42,635 prior links, earlier notes/trees/evidence/ACIP mappings and21,297 prior
CSV lexical rows and citations survive unchanged. Main CSV:21,335 rows;
reverse20,995; changes38 versus5c4d784. All22,665 course/depth groups match the
actual main CSV references, retaining all22,622 old groups. Master/corpus hashes
are unchanged. All17 targeted fidelity/export/constitution suites passed in
10.50seconds. Visual rendering remains unverified; no newer native/device or
main integration action occurred. Next:199–201, independently approved for landing.


### c5p67 · C05:199–201 — 2026-09-12 — Codex reconciliation/review

Fresh independent proposal angles, separate reconciliation and skeptical semantic
review completed. APPROVE covers 32 analyst spans (10/9/13), zero nulls, 21 d5
pairs and six d7 members. All six proposals and all three final bodies reproduce
exactly with zero stderr. All canonical tuples/ranges match the accepted spec;
all eligible review evidence is copied byte-for-byte. See `specs_c05/c5p67.json`
and `reviews_c05/c5p67/`. These remain PROVISIONAL machine alignments.

199 keeps the crossed piss/shit members and final adjectives, the complete
filth compression, and HGM-attested everywhere with explicit scope-recast limits.
200 retains the whole negative prosperity/concord predicates; the latter owns
can't work well together without claiming separate ability vocabulary. Shipping,
cooperation and distributed work/end remain omitted with explanations. 201 keeps
crags, the high/low and difficult-travel compounds with useful members, and the
whole fear-ground phrase. The full phra ma glossary gloss was examined, but no
continuous target span preserves its speech/division distinction without supplied
participants and possessive framing; it remains omitted, not falsely nulled.

The GRU SPYOD→GROGS SPYOD candidate is refuted after all five grounds: ship/use
is defensible, while the proposed cooperation compound is unattested. Four
corpus rows and both local S05392L contexts retain GRU SPYOD; none establishes
independent publication. Exact raw bytes, ranges and full query results are in
the review. No errata or allowances are added; all 186 register records and
existing allowances remain byte-identical to the prior checkpoint.

Coverage 201/511. Bank: 42,726 links (+35 with three sentence anchors), 11,868
null exponents, 1,537 notes, 11 trees. Evidence: 6,857 headwords / 14,784 pairs.
All 42,691 prior links, earlier notes/trees/evidence/ACIP mappings and 21,335
prior main CSV lexical rows and citations survive. Main CSV: 21,364 rows;
reverse: 21,024; changes: 29 versus ef8c0e4. All 22,697 course/depth groups match
actual main CSV references, retaining all 22,665 old groups. Source master and
corpus hashes remain unchanged. All 17 targeted fidelity/export/constitution
suites passed in 10.46 seconds. Rendering remains unverified; no newer native,
device or main integration action occurred. Next: 202–204, reconciliation active.


### ERRATA to c5p66 null description — 2026-09-12

The c5p66 entry's “two explicit grammatical fusion nulls” overstates the kind
of the second null. C05:196 p1 rnams marks plurality fused into English lamas;
p2 do marks assertion closure with no distinct English exponent. Read the
entry as “two explained grammatical nulls: one fused plurality and one assertion
closure.” The accepted spec and semantic review already make this distinction
correctly; neither changes. This correction closes the independent integration
review's factual ledger finding while preserving the original account above.


### c5p68 · C05:202–204 — 2026-09-12 — Codex reconciliation/review

Fresh independent proposal angles, separate reconciliation and skeptical semantic
review completed. APPROVE covers 59 analyst spans (24/24/11), zero nulls, 47 d5
pairs, one d7 member, five d6 particles and six d3 phrases. All six proposals and
three final bodies reproduce exactly with zero stderr; all canonical tuples and
source ranges agree. All 39 eligible semantic evidence files were copied exactly.
See `specs_c05/c5p68.json` and `reviews_c05/c5p68/`. These remain PROVISIONAL
machine attestations; no glossary promotion or human acceptance is claimed.

202 recovers HGM-attested rgo/poisoned and keeps dgon dung/burning hot. The
tha chad/useless reading is corroborated by C01:179 beyond the repeated target
paragraph. rtsub po/harsh is the FIRST rtsub occurrence within ngag rtsub po;
the later rtsub pa owns rough. Root's initial dispatch calling the member second
was explicitly corrected; actual source ranges and final files were already right.
203 selects the second Tibetan fruit noun, preserves the two ripe occurrences
in the correct English order, retains the full dus su/right times phrase, and
recovers causal rgyu/make without supplied things/around/you. Parks has original
C06 support; pools of cool water has an explicit whole HGM gloss. The leisure
negative-many recast remains omitted without a false null. 204 retains four
seasons, get worse, less and less and the complete day-by-day phrase; annual
recurrence is distributed and omitted. No source correction is established.

Independent review reproduced 3,287 quoted source/glossary fields and 32 corpus
query records. All original variant fields are preserved: 202 GYI SA versus
C09/C17 GYIS, and 204 four seasons versus four season. These do not establish
C05 errors or publication independence. No errata or allowances were added;
all 186 existing records and earlier allowance entries remain unchanged.

Coverage 204/511. Bank: 42,788 links (+62 including three sentence anchors),
11,868 null exponents, 1,540 notes, 11 trees. Evidence: 6,879 headwords / 14,822
pairs. All 42,726 prior links, prior notes/trees/evidence/ACIP and 21,364 old main
CSV lexical records and citations survive. One old phonetic row ring du/ringdu
gains its empty ACIP field as RING DU, proven by new C05:203 d5 source evidence;
its lexical content and depth are unchanged. Main CSV: 21,413; reverse: 21,073;
changes: 49 versus 59de591. All 22,751 course/depth groups match actual source
references, retaining all 22,697 old groups. Source hashes are unchanged.
All 17 targeted fidelity/export/constitution suites passed in 10.52 seconds.
Actual rendering and newer native/device execution remain unverified; no main
integration occurred. Next: c5p69, 205–207, with source/context snapshots only.


### c5p69 · C05:205–207 — 2026-09-12 — Codex reconciliation/review

Fresh independent proposals, reconciliation and skeptical semantic review APPROVE
45 analyst spans (21/16/8), three explicitly explained nulls and 30 nonnull d5
pairs. All six proposals and three final bodies reproduce exactly; accepted
ranges and the assembled canonical body match. Evidence and reconciliation
records are preserved in `reviews_c05/c5p69/`, with the accepted spec alongside.
All alignments remain PROVISIONAL; machine approval is not human acceptance.

205 keeps binding chaos and the contextual leopards attestation, while preserving
plague/tiger counterevidence without promoting either to a source correction.
The firefly/biting-worm mismatch and distributed abundance remain omitted. The
second Tibetan mang ba maps to the second everywhere, with both ranges recorded.
206 distinguishes topic ni from inchoative 'gyur ba: the latter has no separate
English exponent because disappearing carries the change, not because is is a
copula equivalent. Protective help/protect and repeated happiness/world/things
select their reviewed occurrences. 207 retains the established title and author
members; the honorific/agentive chen pos remains omitted, not falsely nulled.
All proposed English corrections were refuted as uncertain; no new errata or
allowances were added. All 186 earlier register records remain unchanged.

The first landing exposed a real gate failure: 41 shortened rows exceeded the
legacy cap of 40. C05:206 retained page 73 and its numbered ACI course heading.
The bounded repair recognizes that documented following-heading boundary and
requires exact canonical shortening with a valid source/depth at every citation.
It preserves the original phonetic extraction. Ten older depth-1 display rows
receive the same repair (C03:131,199; C04:37,49,117,133,168; C05:96,118,145), plus
new C05:206. These are derived-display cuts, never edits to full banked source.
The visible disclosure now explains heading and phonetic-line shortening.

Separate engineering SPEC/QUALITY review APPROVE records all four code hashes,
11 exact cut ranges, meaningful RED/GREEN and mutation evidence. Actual generated
HTML/main/reverse/course outputs independently differ from their preserved
pre-fix copies by exactly those 11 rows; all other fields and phonetics are
identical. The original broader 16-row attempt and failing gate remain preserved.
See `reviews_c05/c5p69/heading-trim-fix/` for the complete repair and review proof.

Coverage 207/511. Full bank: 42,836 links (+48 with three sentence anchors),
11,871 null exponents. Evidence: 6,891 headwords / 14,846 pairs. All 42,788 prior
links, notes, trees, evidence and ACIP mappings survive exactly. All 21,413 prior
main CSV records are accounted for: 21,403 identical lexical keys and the ten
explicit reviewed sentence-heading repairs, with citations retained. The old
nub/western d7 row gains NUB from the newly banked C05:206 d5 witness; its lexical
content is unchanged. Main CSV: 21,447; reverse: 21,107; course: 22,788. Changes:
54 versus 4aa4aa6 (44 newly displayed keys, ten withdrawn partial-heading keys).
All course/depth groups match actual source references; ten prior keys receive
only the documented repair. Master/corpus hashes are unchanged. All 17 suites,
including the ten new literal trimming tests, pass in 10.57 seconds. Rendering
and newer native/device execution remain unverified; no main integration occurred.
Next: c5p70, 208–210, independently approved for landing.


### c5p70 · C05:208–210 — 2026-09-12 — Codex reconciliation/review

Independent semantic SPEC/QUALITY APPROVE covers 86 analyst spans (26/27/33),
zero nulls, 51 d5 pairs, 17 d7 members, 11 particles and seven d3 phrases. All
six proposals and three final canonical bodies reproduce exactly. All 46 query
count claims match fresh original-spine searches. The 93 copied evidence files
include all three complete original raw witnesses and failed attempts. Accepted
spec, source ranges and assembled body agree. These remain PROVISIONAL machine
attestations, never new binding glossary entries or human acceptance.

208 keeps the doctrinal 'dod don/belief attestation without replacing binding
position or absorbing reflects. First rang gi owns their with system; the second
is not reused. Whole discourse/manner and emphatic recasts stay d3; unresolved
argument framing is omitted. 209 retains all six unique eye-mind/consciousness
members, including of at [70,72) and [346,348). Whole planting and progressive
constructions are d3. The final arises/produces voice reversal is omitted. The
passing wrong-of proposal and separate canonical ordering refusals remain in
the review history; a successful generator is not proof of semantic correctness.
210 keeps the complete temporal-relative construction, including its source
relation, as immediately before producing. The second disputed temporal phrase
is wholly omitted. Faculty compounds, their useful members, the full conceiving
predicate and final whole-word exist select the reviewed source occurrences.

E-187 is the sole addition: LOW/PROBABLE PAR→BAR in the exact C05:210 quote.
All five skeptical grounds were independently applied. ILL:2242 literally marks
PAR {%BAR}; C16 matches the three source text fields, while P7 differs in
punctuation and truncation. All three full digital originals retain PAR. None
establishes publication independence, blockprint authority or ingestion history.
The candidate changes no source or accepted alignment. All 186 older errata and
all span-head allowances remain unchanged; printed-source verification is open.

Coverage 210/511. Bank: 42,925 links (+89 including three sentence anchors),
11,871 null exponents. Evidence: 6,903 headwords / 14,871 pairs. All 42,836 prior
links, notes/trees/evidence/ACIP and 21,447 previous CSV lexical keys and citations
survive. Main CSV: 21,494; reverse: 21,154; changes: 47 versus cc7d690. All 22,848
course/depth groups match actual main CSV references, retaining all 22,788 old
groups. Source/master hashes are unchanged. All 17 fidelity/export/constitution
suites pass. Rendering and newer native/device execution remain unverified;
no main integration occurred. Next: c5p71, 211–213, independently approved.


### ERRATA to c5p70 historical reconciliation gloss description — 2026-09-12

Independent integration review found that the preserved 210 reconciliation
report says HGM bar has “interval/between renderings.” Root reopened original
entry 12330: hgm_gloss includes between, in between and intermediate, but not
interval. Interval belongs to the Hopkins/comparative field. The final semantic
review, five-ground decision and E-187 already distinguish this correctly.
This appended correction supersedes the historical reconciler's attribution;
all original evidence bytes remain intact. No source, accepted spec, dictionary
entry or errata-bank record changes. Full reopened entry is preserved in
`reviews_c05/c5p71/landing-proof/HGM-bar-description-correction.json`.

### c5p71 · C05:211–213 — 2026-09-12 — Codex reconciliation/review

Independent semantic SPEC/QUALITY APPROVE covers 79 analyst spans (16/19/44),
zero nulls, 44 d5 pairs, 28 d7 members, four d3 phrases and three particles.
All six proposals and three final individual bodies reproduce exactly. All
accepted tuples and ranges match the assembled spec/body. The 36 copied semantic
evidence files include all three complete raw witnesses. Source-specific
attestations remain PROVISIONAL; machine approval does not confer human acceptance.

211 keeps inflected de'i/behind it with a unique de/it member; it does not claim
'i alone means behind. Technical factor and contributing retain their evidence.
212 uses the complete seed-producing-eye-consciousness relative phrase at d3,
preserving causal roles rather than calling a bare arising verb produces. The
unresolved thinking frame remains omitted. 213 rejects the whole-verse d1 phrase
and the earlier ces-null cursor workaround. The disputed object and citation
closure remain omitted with their distributed-expression note. Four foundation
consciousness occurrences and the reversed of/member inside kun gzhi'i retain
exact source offsets. Master and Stiramati are separate supported spans.

Only two reviewed supplied-head allowances are added: c5p71/s213w7 for sogs/and
the rest and c5p71/s213w33 for sogs/and so on. Original HGM 20919 explicitly
attests both complete closing gestures; no extra lexical noun is absorbed.
E-188 is the sole new erratum, LOW/PROBABLE SKYE BO→SKYE'O at 213. Independent
five-ground review preserves all three raw SKYE BO readings, ILL's explicit
editorial alternative and GK:358's shorter line ending SKYE'O; GK does not
witness DE DON MED. No publication independence or blockprint reconstruction
is claimed. The proposed English verse rewrite is refuted as uncertain, and
both original languages remain untouched. All 187 earlier errata and prior
allowances are retained exactly as structured records.

Coverage 213/511. Bank: 43,007 links (+82 including three sentence anchors),
11,871 null exponents, 1,549 note blocks and 11 trees. Evidence: 6,914 headwords /
14,893 pairs. All 42,925 prior links, notes/trees/evidence/ACIP and 21,494 earlier
main CSV lexical keys and citations survive. The old lngar/into five d7 row gains
LNGAR from a newly accepted C05:213 d5 source witness; lexical content is unchanged.
Main CSV: 21,532; reverse: 21,192; changes: 38 versus 5aeb6e9. All 22,896 course/
depth groups match actual main CSV references, retaining 22,848 earlier groups.
Master/corpus hashes are unchanged. All 17 fidelity/export/constitution suites
pass in 10.81 seconds. Rendering and newer native/device execution remain
unverified; no main integration occurred. Next: c5p72, 214–216, source/context
snapshots and focused original-parallel checks only; no proposals yet.


### c5p72 · C05:214–216 — 2026-09-12 — Codex reconciliation/review

Independent semantic SPEC/QUALITY APPROVE covers 136 analyst spans (56/49/31),
zero nulls, 93 d5 pairs, 25 d7 members, 16 particles and two d3 phrases.
All six proposals and three final bodies reproduce through the actual canonical
generator. All accepted tuples and source offsets agree with the assembled page.
The 165 copied semantic evidence files and three reconciliation reports preserve
original witnesses, rejected alternatives and genuine failed attempts. These are
PROVISIONAL machine attestations; human acceptance remains separate.

214 aligns medial rnam shes with consciousness in the seed-relative clause,
leaving the earlier supplied grasper occurrence unpaired. The causal las/byung
ba'i construction is split at d6/d5. Faculty and consciousness compounds retain
useful members; uncertain possession and the final source conclusion remain
unwrapped. Selection Two apparatus remains verbatim. Four independently approved
sogs closing gestures receive narrowly named head allowances, with full original
HGM entry 20919 supporting the exact English ranges.

215 retains the established negative predicate, nonvirtue and beginningless
compounds and exact basis occurrences. Supplied bracketed English and dummy it
are unpaired; the genuine conditional and yang/And remain. 216 treats ltar na/
According to as a d3 phrase, keeps the final me occurrence with the natural-self
expression, and uses the first two foundation-consciousness occurrences. The
SGO/stained spelling concern is refuted after all five grounds; no erratum is
filed. The alternative whole natural-self phrase remains review evidence.

Coverage 216/511. Bank: 43,146 links (+139 including three sentence anchors),
11,871 null exponents, 1,552 note blocks and 11 trees. Evidence: 6,928 headwords /
14,932 pairs. All 43,007 prior links, notes/trees/evidence/ACIP and 21,532 prior
main CSV lexical keys and citations survive exactly. No old ACIP field changes.
Main CSV: 21,585; reverse: 21,245; changes: 53 versus a143082. All 22,971 course/
depth groups match actual main CSV references, retaining all 22,896 earlier groups.
All 188 prior errata remain unchanged; only four reviewed c5p72 head allowances
are added. Master/corpus hashes are unchanged. All 17 fidelity/export/constitution
suites pass in 10.62 seconds. Rendering and newer native/device execution remain
unverified; no main integration occurred. Next: c5p73, 217–219, correction round1
completed and awaiting scoped independent re-review before landing.


### c5p73 · C05:217–219 — 2026-09-12 — Codex reconciliation/review

Independent semantic SPEC/QUALITY APPROVE covers 125 analyst spans (28/49/48),
zero nulls, 94 d5 pairs, 16 d7 members, ten particles and five d3 phrases.
All six original proposals and all three final bodies independently reproduce.
The initial review required four corrections to the 218 note, errata disposition
and narrative evidence. Reconciliation round1 implemented them and scoped
independent re-review closed all four findings. All 125 tuples stayed unchanged;
only the 218 canonical note/body changed. Earlier review and producer attempts
remain preserved. The 422 copied semantic files include complete C05/C16/ILL
raw witnesses and their exact hashes; three final reconciliation reports are
also copied. All new matches remain PROVISIONAL machine attestations.

217 retains the HGM-attested rigs 'dra/continuation of later, similar instances
compound with a unique similar member. Four mind/stream occurrences and the
second also retain exact ranges. 218 identifies mur thug pa/continuity from the
local four-item list while explicitly recording absent HGM equivalence and
divergent ILL interpretations. It does not promote a comparative definition.
The closing affirmative application instruction cannot own the English question
rejecting another basis: that frame is omitted, with six useful lexical atoms
retained. The later bag chags occurrence is selected after the genuine sogs
span; no null cursor anchor is used. The reviewed list-closing gesture receives
one precise c5p73/s218w46 allowance.

219 retains a complete causal-seed phrase and the established spang gnyen opposed
pair with useful members. Dormant occurrences1,2,4 and all four basis occurrences
are distinct. The final apply-this-reasoning phrase includes its demonstrative.
The 1500-character English cap affects appended heading text, after the complete
main antidotes sentence. No source tail is invented and E-107 is not refiled.

E-189 is the sole added erratum: LOW/PROBABLE English polarity review at218,
with all five grounds independently checked and complete digital originals
preserved. It is an editorial review request, not replacement GMR prose. The
separate 'GAG→'BAG spelling filing is refuted because recorded 'gag/stop remains
grammatical and matches its English. ILL's explicit competing annotation and
'BAGS variant remain evidence; no checked Teng page or independent publication
is claimed. Both source languages and hgm_gloss are unchanged.

Coverage 219/511. Bank: 43,274 links (+128 including three sentence anchors),
11,871 null exponents, 1,555 note blocks and 11 trees. Evidence: 6,940 headwords /
14,971 pairs. All 43,146 prior links, notes/trees/evidence/ACIP and 21,585 prior
CSV lexical keys and citations survive. No old ACIP field changes. Main CSV:
21,642; reverse: 21,302; changes: 57 versus f242fdc. All 23,043 course/depth
groups match actual main CSV references, retaining all 22,971 prior groups.
All 188 prior errata survive; only E-189 and the reviewed sogs allowance are added.
Master/corpus hashes are unchanged. All 17 fidelity/export/constitution suites
pass in 10.61 seconds. Rendering and newer native/device execution remain
unverified; no main integration occurred. Next: c5p74,220–222, undergoing fresh
independent semantic review before landing.


### c5p74 · C05:220–222 — 2026-09-12 — Codex reconciliation/review

Independent semantic approval and scoped round1 re-review cover 68 analyst spans
(17/35/16), one null, 44 d5 pairs, six d7 members, nine d3 phrases, two d2 clauses
and seven d6 particles including the null. All six original proposals and all
three final bodies independently reproduce. Root also replayed all corrected
bodies and verified that only C05:220 w2's tuple changed; 221 is byte-identical and
222 changes only its canonical note. The 427 copied semantic files include complete raw
C05/C16/ILL originals, all six frozen proposals, rejected attempts and earlier
review states. All 217 additional packaged input files match their originals,
review copies and landed bytes. Three final reconciliation reports are copied.
All alignments remain PROVISIONAL machine attestations, not human acceptance.

220 retains the established natural-existence and karma/consequence compounds
with strictly tighter members, and full HGM evaluative/closing gestures. The
initial ni null is corrected to d6 Here is at Tibetan [9,11), English [0,7), first
in English order: the original ni glossary and this topic introduction support
a separate exponent. The remaining supplied workings frame and participants
stay unpaired. This does not make every ni equivalent to Here is.

221 keeps the complete Buddhist-schools unit provisionally, distinguishing its
auto-aligned HGM evidence from curated equivalents and comparative dictionaries.
Repeated karma/result occurrences retain their actual roles. The temporal phrase
owns the complete interval; gives has karma as agent and result as object. The
final d2 includes the entire negative causal relation, preserving cause/result
under the English converse verb. A bare arising verb is not equated with gave
rise. 222 likewise uses complete temporal-event and interrogative recasts, with
third/fourth deed occurrences and both med exponents fixed. Its distinct ni
has no separately alignable exponent and remains the sole justified null; the
recast conditional is omitted. The functional explanatory relative is unpaired.

E-190 is the sole new erratum, LOW/PROBABLE English affect→effect at C05:222. Independent
five-ground review rejected the imagined nominal-affect defense: the causal
function context supports ordinary nominal effect, and repeated corpus/HGM
strings do not validate grammar. The source remains verbatim pending human review.
The exact original phrase is at C05 line 2085 and C16 line 8753. ILL's alternative
is physical section [1873], Tibetan line 18445 / English line 18449, corresponding to spine sequence 1878;
next physical [1874] begins LAN DU. All three complete raw files and original-byte
contexts are preserved. Digital agreement establishes no publication independence
or ingestion history. Two exact sogs allowances, s220w10 and s221w15, are added.

Coverage 222/511. Bank: 43,345 links (+71 including three sentence anchors),
11,872 null exponents, 1,558 note blocks and 11 trees. Evidence: 6,945 headwords /
14,997 pairs. All 43,274 prior links, notes/trees/evidence/ACIP and 21,642 prior
CSV lexical keys and citations survive. No old ACIP field changes. Main CSV:
21,685; reverse: 21,345; changes: 43 versus 12b77ec. All 23,090 course/depth groups
match actual main CSV references, retaining all 23,043 earlier groups. All 189
prior errata survive; only E-190 and the two reviewed allowances are added.
Master/corpus hashes are unchanged. All 17 fidelity/export/constitution suites
pass in 10.56 seconds. Rendering and newer native/device execution remain
unverified; no main integration occurred. Next: c5p75, C05:223–225, source/context
snapshots and root lexical preparation only; no proposals or approval yet.


### c5p75 · C05:223–225 — 2026-09-12 — Codex reconciliation/review

Independent SPEC and QUALITY approval cover 113 analyst spans (38/43/32), one
null, 79 non-null d5 pairs, 16 d7 members, 7 d3 phrases and 10 d6 particles. Root
read all final specs/reports/decisions and the separate review, replayed all
canonical bodies and verified exact ordered tuples/ranges and final hashes.
All six current proposals independently reproduce; earlier English drafts and
superseded proof boundaries remain explicitly preserved. All 262 manifest inputs
match their originals, review copies and landed bytes. The 284 copied semantic
files and three reconciliation reports preserve full evidence and decisions.
All alignments remain PROVISIONAL machine attestations, not human acceptance.

223 retains the attested document/debt, two-deed, foundation-consciousness and
non-associated-factor units with legitimate tighter members. The whole written
document equivalent is supported by the original HGM field; final up remains
unpaired. Repeated deeds and others have exact reviewed occurrences. Distributed
temporal framing is omitted without false nulls.224 preserves three distinct
seed occurrences and the causal subjects of production. Its sole cessation null
at Wylie 98..103 reflects no separately alignable English ceased exponent; the
concessive even is separately retained at 221..225. The knowing/application
recast stays unpaired while same/other/three survive.225 retains whole proper
names and the full synonymous title, including parenthetical Abhidharmakosha,
on positive HGM evidence. Distinct group references, school members and final
qualified predicate keep their source roles; final yang/alternate is omitted
as uncertain, never nulled or silently corrected.

No new errata or head allowances. All 190 earlier errata and all allowances remain
unchanged. Coverage 225/511. Bank 43,461 links (+116 including three anchors),
11,873 null exponents, 1,561 note blocks, 11 trees; evidence 6,961 headwords/15,042
pairs. All 43,345 prior links, notes, trees, evidence, ACIP and 21,685 CSV lexical
keys/citations survive. No old ACIP metadata changes. Main CSV 21,745; reverse 21,405;
changes 60 versus 21964bb; all 23,158 course/depth groups match actual main CSV
references and preserve 23,090 prior groups. Source/master hashes unchanged.
All 17 targeted fidelity/export/constitution suites pass in 11.45 seconds.
Rendering and newer native/device execution remain unverified; no main integration.
Next 226–228 has frozen reconciliation and an independent semantic reviewer active.


### c5p76 · C05:226–228 — 2026-09-12 — Codex reconciliation/review

Independent SPEC and QUALITY approval cover 147 analyst spans (68/36/43), two
nulls, 105 non-null d5 pairs, six d7 members, 20 d3 phrases and 15 d6 particles.
All six current proposals and all final bodies independently reproduce. Root
read every final spec, report, decision and independent review, verified all
147 canonical tuples/ranges and hashes, and checked 459 original/review/landed
manifest entries before registration. The 495 copied semantic files and three
reconciliation reports preserve originals, rejected drafts, failed validation
attempts and evidence. These remain PROVISIONAL machine attestations.

226 retains thob bya/retained by the hold as the attested complete object unit,
without the false thob/hold member that would assign the retaining agent to the
object. Two intransitive arising/causative produce mappings are omitted; the
separate result-subject come remains valid. Both two-karma compounds retain
legitimate members. The local la lar/any given match is explicitly contextual,
and the bskal pa/millions of years unit has original HGM evidence. Its ni null
means no separate topical exponent; distributed discourse remains unpaired.
The sole reviewed allowance is c5p76/s226w33, sogs/or the like. Bracketed causal
examples remain verbatim and unwrapped.

227 recovers the attested positions and first answer words. One Tibetan
intrinsic-existence predicate owns the first of GMR's two repeated qualifiers;
the later Tibetan predicate owns the third occurrence. The sole sogs null
reflects no English closing gesture in the quoted passage. Final deed selects
the fourth occurrence, bypassing the supplied quotation participant.

228 keeps de/this distinct from nyid/exactly, and the fourth verse line's
negative separate from the positive go-away idiom. Its second prose med selects
the third no at English 414..416, with karma occurrences 1, 2 and 4. The disputed
rtog pa/invent match is supported by its local non-loss complement, with contrary
HGM think/bzung-invent evidence explicitly preserved and independently weighed.
The preceding intrinsic-existence assumption is omitted as unresolved, never
silently repaired or nulled. The full Selection Four tail remains unchanged.

No new errata; all 190 prior records and all old allowances survive. Coverage
228/511. Bank: 43,611 links (+150 including three anchors), 11,875 null exponents,
1,564 note blocks and 11 trees. Evidence: 6,973 headwords / 15,088 pairs. All
43,461 prior links, notes, trees, evidence, ACIP and 21,745 CSV lexical keys and
citations survive. The newly witnessed d5 cig at C05:226 proves ACIP CIG and
fills previously empty ACIP metadata on three earlier cig rows (d6 May, d7 A,
and phonetic chik); their lexical content and citations remain unchanged.
Main CSV: 21,821; reverse: 21,481; changes: 76 versus 11a388a. All 23,250 course/
depth groups match actual main CSV references and preserve 23,158 prior groups.
Master/corpus hashes unchanged. All 17 targeted suites pass in 11.57 seconds.
Rendering and newer native/device execution remain unverified; no main
integration occurred. Next 229–231 has independent semantic review active.


### c5p77 · C05:229–231 — 2026-09-12 — Codex reconciliation/review

Independent SPEC PASS and QUALITY APPROVE cover 107 analyst spans (48/26/33),
two plural-fusion nulls, 74 non-null d5 pairs, eight d7 members, ten d3 phrases
and 15 d6 particles. Root read every final spec, report, detailed decision and
independent review, compared all canonical tuples/ranges and final hashes, and
checked 317 preserved original/review/landed manifest entries. The 415 copied
semantic files and three reconciliation reports retain all original proposals,
rejected versions, 21 replayed current/historical specs and full evidence.
The concurrently changed builder was independently inspected: both pre/post
c5p76 registration versions are pinned to 11a388a/9f9edc6 and verified by exact
Git objects. Canonical generator/resolver remained unchanged and final bodies
were replayed after that registration. All matches remain PROVISIONAL.

229 retains the whole attested flower-opening and all-phenomena equivalents,
with literal tighter members. The palace ornament remains unpaired because
mchog and yid 'ong ba contribute to its compressed rendering; full original HGM
and parallel evidence are preserved. Two rnams nulls record only plural fusion
into the distinct objects occurrences. Five selected conception-family spans
have reviewed exact ranges; the final repeated conceptual-establishment phrase
is omitted as distributed, never hidden in the incomplete English p10 recast.

230 retains the correct world/ignorance roles while omitting a distributed
causal conversion. Its causal For, consequent then and interrogative why have
distinct source licensors. The complete many-worlds and instrumental/reflexive
through-essence units have original HGM support and legitimate members. The
first ma/none negative subject and grub pa/does so existential recast preserve
the full clause's relation. The proposed punctuation change is refuted after
all five grounds: literary punctuation remains defensible, with no independent
publication evidence establishing a defect. No source words or punctuation change.

231 preserves the abbreviated work title without a false numeral member, both
reordered citation verbs, complete negative predicates, rope phrase and two
through-essence units. The unrendered positive premise is explicitly omitted.
The final definition's subject/dependence relation is not endorsed by its local
Conception/occur atoms. Short parallels support only their actual quoted extent.
The sole approved allowance is c5p77/s231w6 for the attested sogs/and such gesture.

No new errata; all 190 prior records and all existing allowances survive.
Coverage 231/511. Bank: 43,721 links (+110 including three anchors), 11,877 null
exponents, 1,567 note blocks and 11 trees. Evidence: 6,988 headwords / 15,129 pairs.
All 43,611 prior links, notes, trees, evidence, ACIP and 21,821 CSV lexical keys
and citations survive. No old ACIP fields change in this page. Main CSV: 21,883;
reverse: 21,543; changes: 62 versus 9f9edc6. All 23,325 course/depth groups match
actual main CSV references, retaining 23,250 earlier groups. Source/master hashes
unchanged. All 17 targeted suites pass in 11.40 seconds. Rendering and newer
native/device execution remain unverified; no main integration occurred.
Next 232–234 has source/context snapshots, original-parallel checks and independent
role briefs only; no proposals or semantic approval. Final bounded integration
review is active on the three fixed data heads and forthcoming documentation.


### c5p78 · C05:232–234 — 2026-09-12 — Codex reconciliation/review

Three independent Tibetan/English proposal pairs, fresh reconciliation and separate
semantic SPEC/QUALITY APPROVE yield 92 analyst spans (47/20/25), zero nulls,
68 non-null d5 pairs and four d7 members. Source wording and PROVISIONAL labels
remain unchanged; no human editorial acceptance or hgm_gloss promotion is implied.

All six snakes are retained with their actual owners. Rope source occurrences
1/2/4 own English ropes1/2/4; the implicit third source rope and repeated fifth
genitive are omitted, as are English anaphoric expansions. The broad instance
recasts and supplied personal frames stay unwrapped. Temporal continuum/time
are separate lexical units; single moment retains moment/single members. The
explicit yin/is corrects a proposed false null. Outside and its ablative of
are separate; the full instrumental/reflexive essence unit has two members.
Creation remains local provisional corpus attestation, not a glossary correction.

No new errata; CHUNG/CUNG remains underdetermined, and the original Selection
Five tail stays unwrapped under the registered heading class. One exact sogs/
and such allowance is added at c5p78/s232w4; all prior allowances and190 errata
remain unchanged. Review/source/copy proofs verify415 original mappings,
including10 immutable Git snapshots, and394 copied semantic files. All150
original proposal dispositions and failed attempts survive; exact-body evidence
is independently reproduced. Identical immutable evidence shares one copy with
every original path retained. No review or generator check is skipped.

Coverage234/511. Bank43,816 links (+95 with three anchors),11,877 nulls,
1,570 notes,11 trees; evidence6,997 heads/15,160 pairs. Main/reverse/course/changes
CSV rows21,922/21,582/23,376/39, seeded against1809556. Every course-depth group
matches actual source references; prior content, links, notes, trees, citations,
ACIP values and source/master hashes survive. No old ACIP enrichment.

All17 targeted suites passed (11.37s). Exact copies, body/range/source retention,
complete exports and navigation targets passed. A first root registration
assertion expected compact dict syntax and stopped before mutation; the original
spaced form was read and preserved in the retry. Review extraction's failed
null-marker check and its corrected full replay remain preserved. Rendering
remains unverified after browser-policy refusal; native/installed-phone execution
remains verified only through171, locked launch unverified. Main integration open.


### c5p79 · C05:235–237 — 2026-09-12 — Codex reconciliation/review

Fresh independent proposals, reconciliation and semantic SPEC/QUALITY APPROVE
yield67 spans (24/13/30), zero nulls,43 non-null d5 pairs and seven members.
All original text and PROVISIONAL labels remain; no human editorial acceptance
or hgm_gloss promotion is implied. Whole Reasoning/Sixty, living-world and
environment-world units retain defensible members. Repeated minds/karma and
conditional/cessation predicates have explicit occurrence owners. Nominal
'khrul pa owns the complete contextual mistaken states of mind, excluding your.
Supplied pronouns, repeated explanations and disputed relations stay unwrapped.

E-191 retains only a LOW/PROBABLE causal/concessive concern at235; the original
HIGH broad disappearance correction is rejected. Actual HGM25759 attests
gsal du 'gro/disappear, defeating categorical clarity-only reasoning. The
236 concern defaults to refuted under uncertainty and its disputed phrase
stays unaligned. E-192 retains LOW/PROBABLE THUN MANG/MONG spelling review
at237 without changing source. Every five-ground check is preserved, with
full C05/C16/ILL/SVN physical originals and exact counts. Digital sameness
and filenames prove neither independent publication nor ingestion lineage.
All190 earlier errata survive; no head allowances are added.

Root corrected four missing apostrophes in reviewer-authored compound-name
narrative through the original reviewer. Old JSON/report/freeze, exact scoped
diff and separate SPEC/QUALITY approval survive; all13 accepted inputs and
all alignment/errata decisions are byte-identical. No new generator run was
needed for that prose correction. Root verified566 original-copy mappings
including15 immutable Git snapshots and440 exact copied semantic files.
All117 proposal decisions,67 final decisions and failed attempts survive.

Coverage237/511. Bank43,886 links (+70 with three anchors),11,877 nulls,
1,573 notes,11 trees; evidence7,003 heads/15,184 pairs. Main/reverse/course/changes
CSV rows21,961/21,621/23,419/39, seeded against6232214. Every course-depth group
matches actual references. A previously empty cis/why depth6 export field gains
CIS from the new235 d5 source witness; no wording/depth/citation change. All
previous links/evidence/notes/trees/lexical keys and source/master hashes survive.

All17 targeted suites passed (10.99s). Exact body/range/copy, source-retention,
export and navigation checks passed. Rendering remains unverified after browser
policy refusal; native/installed-phone execution remains through171, locked
launch unverified. Main integration remains open.


### c5p80 · C05:238–240 — 2026-09-12 — Codex reconciliation/review

Fresh independent proposals, reconciliation and semantic SPEC/QUALITY APPROVE
yield77 spans (18/31/28), zero nulls,66 non-null d5 pairs and five members.
Original wording and PROVISIONAL labels remain; no human editorial acceptance
or hgm_gloss promotion is implied. Complete mdongs/intricate patterns excludes
a duplicate variety attribution. The first/third sogs own the two list closures;
objects/things and the expanded final instruction remain unwrapped. Compact
collective karma has a member; the relative-clause recast at239 stays in atoms.

The verse's mountains/dark/time and jewels/trees remain lexical attestations
only; changed modifier/coordination relationships are expressly not endorsed.
Independent five-ground review considers defensible readings and does not
refute factual concerns merely because they are not typos. The possession/
accompaniment relation at240 stays unresolved, with sems/mind alone retained.
The complete work title and both school names retain tighter members. Maker/
master English reversal, all four karma tokens and quoted-only ownership are
explicit. Distributed double negations and explanatory participants are omitted.

No new errata survive; all192 previous records remain. Four exact allowances
cover two sogs/and other such gestures, yin min/or not and de ltar na/And so.
Root verified336 original-copy mappings including12 immutable Git snapshots,
358 copied semantic files and all123 proposal decisions. All original and
superseded attempts, physical sources, dictionary fields and queries survive.
All nine original/final individual generator replays are byte-exact.

Coverage240/511. Bank43,966 links (+80 with three anchors),11,877 nulls,
1,576 notes,11 trees; evidence7,015 heads/15,221 pairs. Main/reverse/course/changes
CSV rows22,007/21,667/23,471/46, seeded againstaa580e8. Every course-depth group
matches actual references. The previously empty d7 sems tsam/Mind-Only ACIP
field gains SEMS TZAM from new239d5 witnesses; its wording/depth/citations
remain unchanged. Prior links, notes, trees, evidence, lexical keys and source/
master hashes survive. All17 targeted suites passed (11.03s), with exact copy,
source/range/body retention, export and navigation checks.

Rendering remains unverified after browser-policy refusal; native and installed
phone verification remains through171, locked launch unverified. Main
integration remains open. Next241–243 independent proposals are preparation
only until separate reconciliation and semantic review. The throughput refinement
uses two Astra/high proposal readers and shared identical evidence with full
provenance; no semantic, generator or fidelity check is removed.


### c5p81 · C05:241–243 — 2026-09-12 — Codex reconciliation/review

Fresh independent proposals, reconciliation and semantic SPEC PASS / QUALITY
APPROVE yield 95 spans (50/7/38), zero nulls, 63 non-null d5 pairs and nine d7
members. Source wording and PROVISIONAL labels remain; no human editorial
acceptance or hgm_gloss promotion is implied. Whole mind propositions and
other-than-mind relations preserve occurrence and argument roles. Actual
HGM bshad pa/scriptural references, nominal name/title units and abbreviated
practitioner names support the accepted coverage. The negative/positive-side
compounds are local provisional attestations with exact corpus corroboration.
The complete mode-of-apprehension expression holds its object excludes the
wider supplied path subject it. Distributed negations and repeated explanations
remain unpaired. Following Reading Six apparatus remains in the source anchor.

No errata or allowances added; all 192 prior records survive. The reconciler
corrected 42 overbroad structured decision mappings, preserving old records
and all 178 original proposal dispositions. Root and independent reviewer
checked every final tuple and the corrected component links. One later reviewer
reason spelling was corrected from dngos gal to dngos 'gal; preserved history
and exact diff prove the candidate files, all 95 tuples and semantic verdict
unchanged. No repeated generator run was needed for that narrative correction.
The final integration review will independently check its scope. Root verified
622 original-copy records, including 49 immutable Git snapshots, and 686
exact copied semantic files. All failed/superseded attempts and sources survive.

Coverage 243/511. Bank 44,064 links (+98 including three source anchors),
11,877 nulls, 1,579 notes and 11 trees; evidence 7,020 heads / 15,246 pairs.
Main/reverse/course/changes CSV rows 22,049 / 21,709 / 23,523 / 42, using the
preceding 499e907 main CSV (also unchanged in incoming 72fe719) as seed. Every
course-depth group matches the actual main CSV references. No ACIP enrichment
was needed; prior links, notes, trees, evidence, lexical content and references
remain. Both source/master hashes are unchanged. All 17 targeted suites passed
(10.48 seconds); complete canonical body/range/copy, retention and navigation
checks passed.

Rendering remains unverified after browser-policy refusal. Native/installed
phone verification remains through171; locked launch and main integration
remain open. The campaign continues through the authorized Tuesday deadline.

Post-commit audit supplement: the first whitespace-log parser failed on a
legacy source byte before writing its JSON; data commit 3a88d13 already existed.
The completed byte-safe audit records all 69,289 warnings across six exact
source copies and four intentional CRLF CSVs. Each file equals the committed
Git object; every CSV warning line is verified CRLF. Failed audit attempts are
recorded in the proof. This supplement changes no source or generated data.


### c5p82 · C05:244–246 — 2026-09-12 — Codex reconciliation/review

Fresh independent proposals, reconciliation and semantic SPEC APPROVE / QUALITY
APPROVE yield 117 spans (36/30/51), zero nulls, 91 non-null d5 pairs and eight
d7 members. No source wording changes or HGM-gloss promotion; all work remains
PROVISIONAL machine review. Complete direct-perception and comprehensive-knowledge
units survive. The person/self-nature apposition and two collect-karma occurrences
retain exact owners. The paired root-text/commentary and Tathagata-essence units
have tighter members. Coordinated emotion predicates use their actual occurrences;
distributed perception/agency and English repetitions remain unpaired.

No errata or allowances added; all 192 prior records survive. All 217 original
proposal dispositions and 117 final tuples were read and checked. The independent
reviewer reopened full source/master fields and 109 lexical/corpus queries, with
five complete errata refutations. Seventeen reviewer-reason apostrophe spellings
were corrected, with exact old files/diffs retained. Root checked the 17 actual
reason changes, all 117 unchanged verdicts, 19 unchanged files and exact Python
literal quoting in the reviewer helper. The initial scope checker expected raw
strings within Python literals and failed on quote delimiters; page materialization
ran before that ancillary scope check was repaired. Completed scope proof preceded
registration/build/commit. Candidate acceptance and source tuples were unchanged.
Root verified 590 original mappings, 19 immutable Git pins and 487 exact semantic
file copies. Full original sources, proposals and correction histories survive.

Coverage 246/511. Bank 44,184 links (+120 with three anchors), 11,877 nulls,
1,582 notes and 11 trees; evidence 7,027 heads / 15,287 pairs. Main/reverse/
course/changes CSV rows 22,108 / 21,768 / 23,596 / 59, seeded from 81dbbe4.
Every course/depth group independently matches main CSV references. Prior links,
notes, trees, evidence, citations and lexical keys survive; no ACIP enrichment.
Source/master hashes remain unchanged. All 17 targeted suites passed (11.78s);
complete body/range/copy, retention and navigation checks passed. Source and CSV
whitespace are audited byte-safely before commit without normalization.

Rendered inspection remains unverified following browser-policy refusal. Native
and installed-phone proof remains through171; locked launch and main integration
remain open. A fresh integration reviewer is checking the saved data commits.


### c5p83 · C05:247–249 — 2026-09-12 — Codex reconciliation/review

Fresh independent proposals, reconciliation and semantic SPEC APPROVE / QUALITY
APPROVE yield 74 spans (32/26/16), three nulls (0/1/2), 55 non-null d5 pairs and
five d7 members. Source English/Tibetan stay verbatim and provisional; machine
review is not human acceptance. The cataract bearer and tighter condition member
have exact HGM support. Hair/basin and reversed skeleton occurrences retain their
own objects. The class-distribution and negative-reality predicates are complete
d3 phrases; supplied participants stay outside. The commentary/explained recast
preserves authorship and explanatory content. Copular yin fuses into finite say;
kyis is realized as active agency and final dang has no row-local exponent while
continuing the next example. Nulls do not assert uncertainty as lexical absence.

No errata added; all 192 prior records survive. Basin/rhinoceros and venered
suspicions were challenged against full fields and witnesses. The internal
Chandrakirti/Dharmakirti attribution remains unresolved: shared verse and differing
quotation extents do not establish a unique correction or independent publication.
One exact c5p83/s249w12 allowance records la sogs pas/and the like with actual
sogs licensor. Source/raw metadata, comparative fields, weaker auto-aligned lexical
evidence and limits on P5 physical verification remain explicit in the review.

Root read all 141 original dispositions and 74 final rationales, verified 544
original mappings with 53 immutable Git pins, and copied 370 semantic files
exactly. Six original and three final canonical bodies replayed exactly. Complete
originals, failed/superseded attempts, unsupported-extension byte copies and source
hash-pin/extract distinctions survive. The corrected 248 note was regenerated;
later evidence-only mtha' spelling, covered/with and stale ni-ID reasons retain
prior versions and diffs. Their candidate alignment tuples remain unchanged.

Coverage 249/511. Bank 44,261 links (+77 with three anchors), 11,880 nulls,
1,585 notes and 11 trees; evidence 7,042 heads / 15,323 pairs. Main/reverse/
course/changes CSV rows 22,156 / 21,816 / 23,652 / 48, seeded from885c766.
Every course/depth group independently matches actual main CSV references.
Prior links, notes, trees, evidence, lexical keys and citations survive without
ACIP enrichment. Source/master hashes unchanged. All 17 targeted suites passed
(12.95s); exact body/range/copy, retention and navigation proofs pass. Staged
whitespace is audited byte-safely against complete sources and canonical CSV CRLF.

Rendered inspection, newer native/device execution and main integration remain
open under the existing limitations. The fresh whole-checkpoint reviewer is
checking all saved commits before final integration approval.


### c5p84 · C05:250–252 — 2026-09-12 — Codex reconciliation/review

Fresh independent proposals, reconciliation and semantic SPEC APPROVE / QUALITY
APPROVE yield 50 spans (8/22/20), 13 nulls (1/7/5), 25 non-null d5 pairs,
nine d7 members and one d3 phrase. All remain PROVISIONAL machine attestations.
The residence, water and perception occurrences keep exact source owners.
Whole inflected chur/der and nam mkhar retain their tighter stem/particle members.
The nominal 'du shes/ability to conceptualize excludes their and is not reduced
to ability. Case, topic and closure material without a separate English exponent
has an explicit rationale; distributed content stays unwrapped.

No errata added; all 192 prior records survive. Seven suspicions receive complete
five-ground refutations, with actual parallel extents and contrary lexical
material. One exact c5p84/s250w3 allowance licenses la sogs/or such through sogs;
following pas remains intact. Root read all 84 original dispositions and 50 final
rationales. The reviewer independently replayed nine canonical bodies, 78 SQL
queries (760 full rows), 42 master selections and 21 complete witness extents.

Six report/audit apostrophes and six corresponding helper literals were corrected
by the original reconciler and scoped-reviewed by the original reviewer. All 123
pre-fix files survive, with a byte/AST proof: accepted specs, notes, bodies and
canonical proofs are unchanged. Root's evidence-audit helper initially assumed
the wrong master key and then exact-headword-only selection; actual collector
semantics resolved both assumptions. Failed helper versions and corrected proof
are retained. These were audit-helper assumptions, not source/candidate defects.
Root verified 720 original mappings with 69 immutable Git pins and 1,227 exact
semantic file copies. Original proposals, source files and review histories survive.

Coverage 252/511. Bank 44,314 links (+53 with three anchors), 11,893 nulls,
1,588 notes and 11 trees; evidence 7,046 heads / 15,338 pairs. Main/reverse/
course/changes CSV rows 22,181 / 21,841 / 23,682 / 25, seeded from87a5296
(the baseline9aa617e CSV bytes are identical). Every course/depth group matches
actual main CSV references. Prior links, notes, trees, evidence, lexical keys,
citations and ACIP metadata survive without enrichment. Source/master hashes
remain unchanged. All 17 targeted suites passed (11.44s); exact body/range/copy,
retention and navigation proofs pass. Staged whitespace is audited byte-safely
against complete sources and canonical CSV CRLF before commit.

Rendered inspection remains unverified after browser-policy refusal. Native and
installed-phone proof remains through171; locked launch and main integration
remain open. The broader new checkpoint still requires independent final review.
The first whitespace audit failed closed on frozen review whitespace. A scoped
checker verified eight exact review-freeze pins and every cited empty report
field, EOF line and unified-diff context line; all 56,274 warnings are accounted
for alongside original physical sources and CSV CRLF. No bytes were normalized.


### c5p85 · C05:253–255 — 2026-09-12 — Codex reconciliation/review

Fresh independent proposals, reconciliation and semantic SPEC APPROVE / QUALITY
APPROVE yield 65 spans (16/19/30), two nulls (0/0/2), 48 non-null d5 pairs,
five d3 phrases, eleven non-null d6 particles and one d7 member. All remain
PROVISIONAL machine attestations. The outline's final first, two positions and
meaning-of relation retain exact owners. Actual weak mined glosses and wider
binding variants are confronted explicitly. Complete causal and alternative
recasts remain d3; the Buddha stem excludes the institutional suffix. Distributed
bzang ngan comparison stays omitted with reasons, without a false atom or null.
Single med/thal occurrences select their first exponents, leaving repeated
English consequence/negative formulations unwrapped. Existential yod/do retains
its local ellipsis. The refuge compound has a tighter mchi/go member; pronouns,
causal for occurrences and restrictive alone retain their actual owners. Both ni
nulls describe topic/cleft grammar without a separately alignable English word.

No errata added; all 192 prior records survive. Non- Buddhists remains verbatim
under the registered hyphen-plus-space class, after all five refutation grounds.
Full C15/C16 equality and shorter P7/P9 extents do not establish independent
publication or support the unquoted continuation. Four exact head allowances
name bdag cag, bdag, khyod and khyed. Source/master and hgm_gloss stay unchanged.

Root read all 123 original dispositions and 65 final rationales, plus the entire
semantic report, 65 independent assessments and complete five-ground refutation.
The reviewer independently replayed nine canonical bodies, 123 original and 65
final tuples, 66 SQL queries and 42 full master selections, with seven additional
contested lexical records. Root verified 462 original mappings and 11 immutable
Git pins, then 323 exact semantic file copies. All original proposals, complete
source evidence, failed/superseded attempts and historical governing bytes survive.
The new landing baseline is 0561829; earlier proposal context remains pinned to
87a5296. No post-freeze correction of accepted proposals was required.

Coverage 255/511. Bank 44,382 links (+68 with three anchors), 11,895 nulls,
1,591 notes and 11 trees; evidence 7,048 heads / 15,360 pairs. Main/reverse/
course/changes CSV rows 22,212 / 21,872 / 23,719 / 31, seeded from 0561829.
Every course/depth group independently matches actual main CSV references.
Prior links, notes, trees, evidence, lexical keys, citations and ACIP metadata
survive without enrichment. All 17 targeted suites passed (11.60s); exact body,
source range, copy, retention and navigation checks pass. Staged whitespace is
audited byte-safely before commit, preserving physical source and CSV CRLF bytes.

The fresh reviewer is checking both new data heads and final documentation.
Rendered inspection remains unverified after browser-policy refusal; native and
installed-phone proof remains through171. Locked launch and main integration
remain open. The larger campaign continues through the authorized Tuesday cutoff.

The first whitespace audit rejected two exact frozen brief EOFs. The pin adapter
then assumed relative freeze paths and failed on this reviewer's absolute paths;
an immediate v2 invocation also failed closed without pins. Scoped v3 resolves
either schema strictly inside the review directory. Both exact pins and all
22,564 diagnostics pass before commit. Failed attempts remain preserved; no
source, review evidence or generated data was normalized or changed.
