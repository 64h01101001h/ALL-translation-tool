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
c5p1–c5p7 (1–21) cloned 2026-09-05; c5p8–c5p49 (22–147) landed 2026-09-05; c5p50–c5p55 (148–165) landed 2026-09-08. c5p56 (166–168) landed 2026-09-12 after fresh independent proposals, reconciliation and skeptical review. c5p57 (169–171) landed after independent review. c5p58 (172–174) landed after independent review. c5p59 (175–177) landed after independent review. c5p60 (178–180) landed after independent review. c5p61 (181–183) landed after independent review. c5p62 (184–186) landed after independent review. c5p63 (187–189) landed after independent review. c5p64 (190–192) landed after independent review. c5p65 (193–195) landed after independent review. c5p66 (196–198) landed after independent review. c5p67 (199–201) landed after independent review. c5p68 (202–204) landed after independent review. c5p69 (205–207) landed after independent review. c5p70 (208–210) landed after independent review. c5p71 (211–213) landed after independent review. c5p72 (214–216) landed after independent review. c5p73 (217–219) landed after independent review. c5p74 (220–222) landed after independent review. c5p75 (223–225) landed after independent review. c5p76 (226–228) landed after independent review. c5p77 (229–231) landed after independent review. c5p78 (232–234) landed after independent review. c5p79 (235–237) landed after independent review. c5p80 (238–240) landed after independent review. c5p81 (241–243) landed after independent review. c5p82 (244–246) landed after independent review. c5p83 (247–249) landed after independent review. c5p84 (250–252) landed after independent review. c5p85 (253–255) landed after independent review. c5p86 (256–258) landed after independent review. c5p87 (259–261) landed after independent review. c5p88 (262–264) landed after independent review. c5p89 (265–267) landed after independent review. c5p90 (268–270) landed after independent review. Coverage 270/511; current verification is recorded in the latest batch entry below.

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

### c5p86 · C05:256–258 — 2026-09-12 — Codex reconciliation/review

Tibetan-first and English-first readers independently proposed84 and88 spans.
Fresh reconciliation and separate SPEC/QUALITY review approve91 spans
(40/38/13),61 non-null d5 word pairs,12 d3 phrases,12 d6 particles and6 d7
members, with no nulls. All remain PROVISIONAL machine attestations. Complete
case-marked perspective idioms retain d3 scope without lexical eyes claims.
The distributive type compound retains its whole inflected word. Four reality
predicates, repeated beings/liquids and wet/flowing descriptions have exact
source owners. The causal pas/Since remains a complete particle. Five actual
valid-perception expressions stay separate from the spelled-out first-option
conditional. Existential-modal and joint water/not-water/both constructions
retain complete phrase boundaries. Technical correct apprehension and
incompatibility retain defensible full HGM wording. The paired qualities and
mutual-exclusion definition preserve their full logical scope. Total three is
not mapped to residual other two despite a broader glossary variant.

All nine suspected errata were independently refuted on all five grounds.
No errata or head allowances added; all192 earlier errata remain unchanged.
C15:402's differing wording, P7's shorter/different fields and C16:834's
following heading retain their exact measured limits. Digital identity does
not establish independent publication or ingestion. Source/master and hgm_gloss
remain unchanged. Omission substring lists are distinguished from actual
linguistic occurrences; distributed content does not become a false null.

Root read the complete semantic report, all91 final assessments,172 original
dispositions,16 material omissions and nine five-ground refutations. The reviewer
independently replayed nine canonical bodies,71 saved queries,27 exact master
selections and18 physical extents, with additional complete lexical evidence.
Root reverified all599 original mappings including66 historical Git copy
records, all91 exact tuples and463 complete semantic file copies. Final
reconciled bodies are6306/7300/3513bytes. All original/superseded/failed
proposal attempts and governing snapshots remain preserved.

A post-landing copy audit caught11 unsupported Python helper extensions.
The original reviewer preserved all five prior interfaces and453 frozen files,
relocated11 helpers byte-for-byte to .py.txt paths and refreshed only packaging
interfaces. Root verified the exact prior/current manifest delta,462 current
frozen files, unchanged specs/bodies/notes/verdict and every copied byte.
The failed audit and original acceptance proof survive alongside the corrected
proof. The reviewer also preserved its earlier failed master-key assumption.

Both actual canonical builders ran once and exited0. A root wrapper then
rejected the view builder's normal stderr summary; actual logs and the failed
wrapper record survive, with no rerun or CSV-seed change. All17 targeted suites
passed11.16seconds. Coverage258/511; bank44,476links (+94 including3anchors),
11,895nulls,1,594notes and11trees; evidence7,059heads/15,386pairs. Main/reverse/
course/changes CSV rows22,255/21,915/23,769/43, seeded fromd428993. Every
course/depth group matches actual main references and metadata. Earlier links,
notes, trees, evidence and lexical content/citations survive. One earlier
d7 mi'i/humans record gains proven MI'I ACIP metadata through new d5 mi'i/human
at C05:256; the exact source witness is recorded. Original archives are unchanged.

Staged whitespace is checked against exact source/review pins before commit,
without normalizing CSV CRLF or physical evidence. Independent whole-checkpoint
review remains required. Rendering remains unverified after browser-policy
refusal; native/installed-phone proof remains through171. Locked launch and
MAIN integration remain open. The campaign continues to the Tuesday cutoff.

The initial whitespace pin preparation encountered ILL and a physical C05
excerpt beyond the historical ReadingASCII filename filter. The immediate
v3 invocation also failed closed. Both failed outputs are preserved externally
with committed hashes. Scoped v4 proves the complete ILL original and exact
4,329-byte C05 original range against frozen copies; it also pins the one
empty brief EOF. All44,036 diagnostics are accounted for before commit,
including source whitespace and canonical CSV CRLF. No source, review or
export bytes were normalized. The raw22,401,520-byte final log is SHA-pinned
externally; the final staged diagnostic result must match after proof staging.

### c5p87 · C05:259–261 — 2026-09-12 — Codex reconciliation/review

The independent Tibetan-first and English-first readers proposed 82 and 95
spans. Fresh reconciliation and separate SPEC/QUALITY review approved 96 spans
(32/46/18): 55 non-null d5 word pairs, 7 d3 phrases, 19 d6 particles and 15 d7
members. All remain PROVISIONAL machine attestations. The whole bzang ngan
contrast retains its two tighter good/bad members. Reporting and possessive
phrases retain complete source owners. gnyis/two is a local classifier-boundary
judgment: the full HGM inventory also contains two types. dbang gis/forces and
sogs's indefinite exemplifying gesture are grounded in complete lexical and
local evidence. Four valid-perception occurrences, three human occurrences,
two mental streams, exact contraction subword scope, the nonhuman copula and
all sensory/original-position members retain their distinct ranges. C05:261
uses the full analytical idiom sgro 'dogs chod pa at d3; the supplied object
is outside its exponent. Reciprocal agents/objects and distributed reasons
retain their actual source owners without inventing a null.

Root read all 177 original dispositions, 96 final assessments, 27 material
omissions and seven complete five-ground refutations. The independent reviewer
replayed nine canonical bodies and all original/final source slices, re-executed
119 distinct saved queries plus four additional queries, and checked 186 full
master-tier selections plus nine additional selections. Root verified all 649
original-to-copy records, including 21 historical Git records, and all 447 frozen
files plus the root freeze itself. All 448 semantic files copied exactly.
Physical C05/C15/C16 files and measured parallel extents are preserved; shared
digital text does not establish independent publication. C16:837's following
heading remains outside the complete 310-character C05:261 English.

All seven errata suspicions were refuted on all five grounds. No errata or head
allowances were added; all 192 prior errata remain unchanged. The original
English-first count typo and every failed/superseded attempt remain preserved.
Root's first reconciliation identity audit assumed a flat tuple schema; the
corrected proof checks complete span objects and actual ranges. The first
semantic freeze excluded nested original freeze.json filenames from its list;
its exact failed interface survives, and corrected enumeration includes every
nested original freeze. Neither correction changes a source, spec or verdict.

Both actual canonical builders ran once and exited zero. The exact previous
main CSV was pinned to data baseline 48bd9d4 before execution; normal view
stderr is preserved and inspected. All 17 targeted suites passed (12.08s).
Coverage 261/511; bank 44,575 links (+99 including three anchors), 11,895 nulls,
1,597 notes and 11 trees; evidence 7,065 heads / 15,400 pairs. Main/reverse/
course/changes CSV rows 22,288 / 21,948 / 23,810 / 33. All earlier links,
notes, trees, evidence, ACIP metadata, lexical rows and citations survive;
no prior CSV metadata required enrichment. Every course/depth group matches
actual main-CSV refs and metadata. Source and master archives are unchanged.

Staged whitespace is checked using exact original-source/review pins, preserving
physical whitespace and canonical CSV CRLF. Independent whole-checkpoint review
remains required. Rendering remains unverified after browser-policy refusal;
native and installed-phone proof remains through171. Locked launch and MAIN
integration remain open. Continuation is scheduled every five minutes through
Tuesday, September15,2026 at07:00 America/Denver; fidelity gates remain intact.

The staged whitespace audit accounted for all 52,622 diagnostics: 29,916
complete physical-source lines, 22,701 canonical CSV CRLF lines, three exact
unified-diff context lines and two frozen empty EOFs. The external raw log is
22,967,113 bytes, SHA-256 7fb1d0e0ba603ff43d34daecb7f4efe2c569da2d63e1c3de53ab243ace393df5.
No source, evidence or export bytes were normalized.


## 2026-09-12 — Codex c5p88 / C05:262–264

The two independent original proposals contain191 spans. A separate reconciler
produced107 spans, then root found an incorrect one-member structural limit.
Correction1 restored khrag/blood as the second contiguous d7 sibling under
rnag khrag/blood and pus; rnag/pus remains the first Tibetan member. The
unchanged canonical generator orders the two English members within their
parent. Complete prior freezes, all191 original dispositions and the exact
correction diff remain preserved. Ruling: the specification's immediately-after
wording permits a contiguous member block; canonical multi-child handling is
the affirmative authority, without relaxing parent containment or uniqueness.

A different non-author independently approved SPEC and QUALITY with no open
findings. Available unrelated-batch readers were reused under explicit capacity
rulings after a fresh-instance dispatch failed; no fresh-context claim is made.
Final43/47/18 spans total108:76 d5 (75 non-null),27 d6,3 d3 and2 d7;11 nulls.
The temporal ma fusion-null is explicitly justified with its positive HTG2016
until/not-yet variants preserved, distinct from matrix negation. Common-locus
phrase, repeated occurs/provides/water/glass, wider positive glossary variants,
causal roles and inflected mi'i/chur received separate semantic scrutiny.
No source/master change, glossary promotion or human editorial acceptance.

The review independently checked143 full-row SQL sets plus3 additional queries,
51 master records,24 physical extents,108 final/191 original tuples, six
original and three final canonical bodies, and two superseded historical bodies.
Root verified764 frozen review files plus the top-level freeze,689 original
mappings and38 historical Git pins. All765 review files survive landing; all
correction histories and failed reviewer/root adapters remain available.
No erratum or head allowance was added; all192 earlier errata remain intact.

Both actual canonical builders ran once with the real previous-main-CSV seed
from235bd2ebabc2278a91a8c06c95f8371f2e5a77fe, and both exited0. All17 targeted
CTest suites passed in13.57seconds. Exact source/retention checks preserve prior
links, notes, trees, evidence references/ACIP and CSV metadata without ACIP
enrichment. Bank44686 links /11906 nulls /1600 notes /11 trees; evidence7065
headwords /15415 pairs. CSV22311 main /21971 reverse /23840 course-depth /
23 changes; all23840 course-depth groups match actual references and metadata.
The page adds108 analyst spans and3 sentence anchors; C05 coverage264/511.

All results remain PROVISIONAL machine attestations. Rendered-page inspection
remains unverified after browser-policy refusal; native/installed-phone proof
remains through171. Locked launch and MAIN integration remain open. A fixed
checkpoint review will verify the committed data/documentation and perform a
new read-only MAIN preflight; this entry does not claim that future review done.
Next source batch265–267 is prepared without a premature baseline. Five-minute
continuation runs through2026-09-15 at07:00 America/Denver.

The staged-byte audit accounted for all119316 warnings:96696 preserved physical
source warnings across exact current/history copies,22607 canonical CSV CRLF
warnings,9 empty unified-diff context lines and4 empty EOF warnings. Raw log
40716639bytes/SHAf69d0588fa6b2c4606a47baff3f0de2a9978a08ff4f1dba76780d4c4931d7812
is preserved externally. The v5 audit adds only an explicit constrained freeze
filename argument for this review's actual freeze.json; v4 remains intact and
all classification/index/source checks are unchanged. A first classification
refused one redundant root-helper diff copy outside the semantic freeze. Its
complete original stays external with a SHA pin; both full helper sources and
failed/superseded manifests are banked. No source whitespace was normalized.

## 2026-09-12 — Codex private continuation through C05:267

Private data commit f3c1ca804a933d53f84214d6968a84e7e0193c6e lands C05:265–267
as page89: 92 analyst spans (19/38/35), four explicit nulls (2/0/2), 61 non-null
d5 pairs and three sentence anchors. C05 reaches267/511 across89 pages. Since
segment165,34 new pages contain2920 analyst spans and102 anchors. All remain
PROVISIONAL machine attestations; source/master bytes and hgm_gloss are unchanged.

The primary Codex reader authored the Tibetan original, disclosing its prior
source preparation, and froze all three drafts before opening English proposals.
A separate reader authored English; two distinct non-authors reconciled and
semantically reviewed the result. The explicit capacity ruling preserves these
four roles while reusing existing child records. No fresh-context claim is made,
and the primary's mechanical verification is not independent semantic approval.

The independent semantic report approves SPEC and QUALITY with zero findings.
Root read its complete29641 bytes and all92 adjudications; report SHA-256
16d8c1b797e7920e566b2f696d2932b4fabef8caee44cc6fdde383b14e6b94f8.
All171 original span identities survive with dispositions. Reviewers reopened
186 primary and five supplemental SQL sets,65 full master records,75 raw corpus
records and24 complete physical extents. All nine original/final canonical bodies
reproduce exactly. The current freeze lists525 files;526 including the freeze
were copied exactly, with527 original-copy mappings and19 historical Git pins.

Material decisions preserve265's second provides occurrence, active agentive
nulls and lexical sogs/and so on;266's collected root, three, matrix is/not,
contracted n't and two blood/pus members;267's whole rigs 'dra phyi ma/continuation,
look root, no/fullness/agentive distinctions, causal Since, experiencing root and
two blood/pus members. Positive glossary variants were considered alongside full
source syntax. One exact allowance, pages_c05/c5p89/s265w15, licenses the internal
and in sogs/and so on using HTG2016 HGM20919 and the local enumeration. No new
errata were accepted; all192 previous entries and previous allowances survive.

Both actual canonical builders ran once and exited0, using the real previous
main CSV from2c1e0768629aff301b9f80a510d7ccbcf53d6ba7. All17 targeted suites passed
in10.77seconds. Prior links, notes, trees, lexical rows, evidence references and
ACIP metadata were retained without enrichment. Bank44781 links/11910 nulls/
1603 notes/11 trees; evidence7067 heads/15430 pairs. CSV rows:22341 main,
22001 reverse,23875 course-depth and30 changes. Every course-depth group matches
actual references and metadata. ACIP forms are proven for7064 of7067 headwords;
the builder's rounded100.0% display does not mean every headword has a proven form.

Root verification errors are preserved. The CTest process passed, but its wrapper
then asserted a different footer and failed; root verified every numbered Passed
record and the actual footer without rerunning tests. Parent python -B did not
disable bytecode in CTest children: three tracked engine caches changed. Their
changed bytes were preserved before restoring only those caches to exact HEAD
bytes in the private checkout. Future subprocesses must explicitly inherit
PYTHONDONTWRITEBYTECODE=1. No source or engine code changed.

The first whitespace classifier refused three original physical lines inside a
frozen English collection log. Root mistakenly launched the dependent v5 audit
after that failure; it also failed because the pins file had not been written.
Both failures and full raw diagnostics survive. Strict v6 adds exact original
physical-line witnesses: frozen log hash, indexed/live equality, warning line
set, full original-file hash, byte ranges and actual line boundaries must agree.
No source whitespace was normalized and no general exception was introduced.
All59823 warnings are accounted for:37076 whole physical-source warnings,
22731 canonical CSV CRLF,three exact physical-source log lines,nine empty diff
contexts and four blank EOF warnings. The complete25478080-byte raw log has SHA
f870c2d24362d44014be0ae88ea1ef24b8a5b8529073223bd050599434a34a10;
its full bytes stayed identical after final proof staging. Both whole helper
versions and all failure history are retained; the complete helper diff is
external with a SHA pin. The private data commit contains597 scoped files.

The fully closed preceding checkpoint is7d3d232, through264. Fixed through267
data/documentation review is still required, including all seven exact output
reproductions, all1912 fixtures per checkout and a fresh read-only complete
original-base MAIN patch preflight. This entry does not claim those future checks
passed. No changes have been applied to MAIN. Native/installed-phone verification
remains through171; rendered inspection, locked launch and MAIN integration
remain open. C13 remains blocked pending its upstream correction.

Next268–270 baseline was captured from clean dataf3c1ca8 with all nine current
hashes and eight required references checked. No next original proposals exist
at this entry. Source preparation is complete through294;295–297 is partially
prepared and explicitly unfinished. The authorized five-minute continuation
remains active until2026-09-15 at07:00 America/Denver. No additional files needed.

## 2026-09-12 — Codex private continuation through C05:270

Private data commit 32472a829c3336f13667d19c3df92538cca3340f lands C05:268–270 as page90: 123 analyst spans (25/34/64), five explicit nulls
(0/1/4), 75 non-null d5 pairs and three sentence anchors. The corrected builder
proves270/511 across90 pages. Since segment165,35 pages contain3043 reviewed
analyst spans and105 anchors. These remain PROVISIONAL machine attestations;
source/master bytes and hgm_gloss remain unchanged.

Root authored the Tibetan originals, disclosed its earlier source preparation,
and froze all three before reading the separate English proposals. Distinct
non-authors reconciled and semantically reviewed the batch. Contexts were reused
with disclosed roles; root performed mechanical acceptance, not semantic approval.
Every227 original identity retains its disposition:200 retained,26 recast and
one rejected. All123 exact final tuples, optional fields and occurrences survive.

The initial full semantic review returned one blocking finding: the retained
sweet-to-clear erratum candidate did not defeat uncertainty. The non-author
reconciler removed that candidate from current errata and corrected its note.
Both dvangs and sweet remain unwrapped; refuting an editorial allegation does
not establish a lexical correspondence. Positive and contrary dictionary
records, original sources, all five refutation grounds and historical proposals
remain intact. No source text was corrected and no new erratum was accepted.
All192 previous errata remain unchanged.

The independent scoped re-review resolved C05-268-ERRATA-01 and approved SPEC
and QUALITY with zero open findings. It combines the completed initial full
alignment review with exact verification of the note-only correction; it does
not claim a fresh full semantic pass. Final report31823bytes SHA-256
cff14cd1bf28cc2d25b6c55444df31ea7e44149e2d1f251b578ba6f909bf4fba.
Current freeze285239bytes SHA-256
dd97122be4eaf14d7c848ebddf1f87cf97f9eadead09ee21196ed2eac06faa25
lists1394 files;1395 including itself are copied exactly. Current manifest has
1926 mappings and78 historical Git pins. Initial governing repository objects
remain pinned to2a1b9a8; changed live paths use explicit immutable-history
transport. All573 initial review files,923 root-preserved historical files,
936 transports and20 nested freezes remain verifiable. Initial packages retain
their initial status; no old NEEDS_CORRECTION result was relabeled APPROVE.

Material alignments preserve268's repeated human/world/karma/taken references,
created by as a complete phrase, and lexical sogs/and so on. Its single specific
head allowance is pages_c05/c5p90/s268w21, licensed by HGM20919 and source ranges
Wylie[311,315), English[426,435). Segment269 distinguishes all four spirit
references, outer-domain phrase expansion and omitted inner-domain explanatory
prose; three obstacle/with pairs and six compound members have distinct ranges.
Segment270 distinguishes repeated threes, two complete mantra-of-steel names
and a later abbreviated name, the second provides, two sets of application/hand/
contact predicates, contracted n't and the final coordination-licensed myong
null. Supplied words and unresolved correspondence stay unwrapped.

Nine initial original/final canonical executions pass. The reviewer ran exactly
one new canonical268 execution for the changed note;269/270 remain exact prior
successful runs. Root independently regenerated268 and then the assembled page.
The assembled body is21400bytes SHA-256
9df2ab728dac4c87cfdc0ea3877b95cac1cd4d624f757e053a485be5b93f88b9;
combined spec SHA-256
f8035b43d6147b779c6d89fa040f52077cd7277e63cb4076129cdb2c8e987a72.

Root initially omitted page90 from the builder registry after updating the page,
index and allowance. Both builders exited0 but still reported267 coverage;
root stopped acceptance before testing or committing. All15 execution artifacts
and seven actual generated outputs were preserved, then all seven outputs were
restored byte-for-byte from baselinef3c1ca8. The sole builder-code change registers
c5p90:[268,269,270]. Corrected builders ran again and exited0 with270 coverage.
They each ran twice for this batch; canonical-builds-once.json describes only
its particular attempt. The historical first attempt remains explicitly separate.
Both attempts use the exact previous main CSV fromf3c1ca8. No algorithm changed.

All17 targeted CTest suites ran once and passed in12.29seconds with explicit
PYTHONDONTWRITEBYTECODE=1 inherited by children. All44781 prior links, earlier
notes/trees/evidence/ACIP,22341 prior main CSV rows and23875 course-depth groups
are retained without enrichment. Current bank44907 links/11915 nulls/1606 notes/
11 trees; evidence7073 heads/15452 pairs. CSV rows:22385 main,22045 reverse,
23932 course-depth and44 changes. All23932 course groups match exact main-CSV
references and metadata. ACIP is proven for7070/7073 heads; rounded100.0% does
not mean complete headword coverage. Source/master hashes remain exact.

The full staged whitespace audit accounts for141409 warnings:114913 complete
original physical-source warnings,22821 canonical CSV CRLF,3638 exact whole
ILL/original ZIP-member warnings,30 print-separator warnings,4 existing empty
EOF warnings,2 exact diff-context lines and1 exact root-log EOF. The complete
44112924-byte diagnostic SHA-256 is
ba7343bb6a19e47b73e66c2922e208db4fa079a0a8a7030236a6c20e282cd186.
It remained byte-identical after all final proof staging. No source or evidence
whitespace was normalized. The private data commit contains1862 scoped files.

The earlier pin preparer refused these new evidence forms. A bounded candidate
adds exact verification of the actual producer print call/default separator,
full SQLite/corpus field equality, complete original ILL bytes, original DOCX
member equality, and the specific root output's manifest/execution/empty EOF.
Root's initial inference that six trailing log spaces came from source fields
was wrong: they are print-separator bytes. An AST regression test exposed the
need to verify the actual producer call even if pins were updated. Full actual
failed attempts remain, including the candidate's initial refusal of the root
log and its final verifier's wrong source-line22 assumption corrected to23.
Sharedv6 and original root preparer remain unchanged. No general whitespace
exception or semantic-freeze relabeling was introduced.

Root independently verified all434 candidate files, both complete baseline
diffs and all16 exact pins against real staged/original bytes; fresh16 witness
tests and1 dispatch test pass. The dispatch test uses a test Git transport;
actual current staged verification is separate. The real full staged audit
passed at00:29:04UTC Sep13, writing only to a fresh external output directory.
Candidate freeze SHA-256
af8db137b011bf7accfdfda477ae506609a5acb80f73f1c6c2975049b00a9538
remains immutable. All434 full candidate files/818971825 bytes and actual test
fixtures/failure history remain available; banked code/proofs/metadata and
explicitly pinned external large/raw witnesses preserve every version.

The final root copy manifest has448 exact records and175 explicitly retained
external records. Earlier128-record before-whitespace and441-record before-
final-stage manifests remain exact historical snapshots. Initial staging proof
covers1855 files; a separate final supplement verifies the7 additional proof
records, the sole updated current manifest and unchanged1854 prior file pins,
then binds all1862 actual staged files. Its own last proof stays external to
avoid a self-referential hash. The immutable data commit binds the final tree.

The preceding fully closed checkpoint is2487bfc through267. A separate fixed
through270 checkpoint review, full seven-output reproduction and fresh read-only
original-base MAIN patch check are still required. No changes applied to MAIN.
Native/installed-phone verification remains through171; rendered inspection,
locked launch and MAIN integration remain open. C13 remains blocked upstream.
Source preparation through303 is not accepted alignment. Evidence-copy v2 is
available only for explicitly pinned future batches and was not used here; a
reading-index candidate remains unadopted. The active continuation is every five
minutes through2026-09-15 at07:00 America/Denver. No additional files are needed.

## 2026-09-13 — C05 through 273; private data commit 2e5d5159

Private data commit `2e5d515987c1b30d7812ab9d1ac7f42397b6ad37` adds
C05:271–273 as page 91. It contains 2,625 scoped files. Generated coverage is
273/511; this does not claim that C05 is finished. The preceding independently
closed global checkpoint is `7166534a9f47a201e5ab580a843c14ba2a910891`
through 270. This entry records page acceptance; the separate fixed-head global
through-273 review and fresh read-only MAIN preflight remain pending.

The page has 101 independently reviewed analyst spans (52/21/28), eight explicit
nulls (4/4/0), 63 non-null d5 pairs and three sentence anchors. Root authored
the Tibetan originals and froze them before reading separate English proposals.
Distinct non-authors reconciled and reviewed them with disclosed reused context.
All 198 original identities retain full dispositions: 180 retained, 16 recast
and two distributed omissions. Root's acceptance checks are mechanical;
semantic approval comes from the independent SPEC and QUALITY review.

The initial reviewer upheld one causal-scope finding in 273. The correction
removed only p22 and its English-order entry and appended an explanatory note.
The first phyir [196,201) and first for [91,94) remain verbatim and unwrapped:
they connect different premises in the two versions. Positive HGM11962 evidence
for for/because/since remains preserved. This is distributed restructuring,
not absent meaning or an explicit null. The distinct final phyir/for mapping at
[255,260)/[265,268) remains. Scoped independent re-review approved both gates
with zero open findings. All 101 surviving full spans, optional fields, parentage,
source ranges and rationales match the initial review after removing only p22;
the other 197 original dispositions remain unchanged. Six five-ground errata
refutations survive; no erratum or head allowance was added. All 192 prior
errata remain exact, and the initial NEEDS_CORRECTION package stays unchanged.

The final semantic freeze contains 2,032 files plus its top manifest, SHA-256
`b4f9b53d8cbc18385b006f7d8dff7a04cee73fa5bf61a98e9b031bcb32c1cf4c`.
Its 12,568 full source mappings retain 1,975 distinct copies, 9,145 historical
source-version transports and 283 Git aliases against 11 original objects.
Root reopened complete originals/copies before landing. The assembled spec is
17,245 bytes, SHA-256
`f77fb9eebff766ffc97db8c3290ba17c09cbc2b009920ef21e4c85ac3f56adbc`;
the canonical body is 17,220 bytes, SHA-256
`0d3ccc58baef88af484d202aaf5d6020101e733308c47e2c1db0acc47e922c33`.
The sole builder change registers c5p91:[271,272,273]; navigation and index
advance exactly. Source/master bytes and binding hgm_gloss are unchanged;
machine attestations remain PROVISIONAL.

Both live builders ran once, exited 0 and reported coverage 273. The view used
the exact prior main CSV seed from 7166534. All 17 targeted CTest suites passed
once in 12.16 seconds. All 44,907 prior links, earlier notes/trees/evidence/ACIP,
22,385 main CSV rows and 23,932 course-depth groups remain without enrichment.
Current totals: 45,011 links, 11,923 nulls, 1,609 notes, 11 trees, 7,078 evidence
heads and 15,469 pairs. CSV rows: 22,413 main, 22,073 reverse, 23,974 course-depth
and 28 changes. Every course group matches exact main CSV references/metadata.
Actual ACIP head coverage is 7,075/7,078; rounded 100.0% is not complete coverage.
The missing spellings and six unchanged prior empty-ACIP rows remain explicit.

An independent isolated run reproduced both JSON banks, dictionary HTML and all
four CSVs byte for byte with the preserved previous CSV seed and frozen date.
The retention oracle passed 223,906 checks. Its first ACIP diagnostic and scoped
recovery are preserved. Root reopened all 2,785 frozen reproduction files, 2,715
full input aliases, seven immutable prior Git outputs and seven current outputs.
Reproduction freeze SHA-256:
`6e4c34741e2712221a22806cf602440e99ab8d2ec1ce1ae3bcd8e89240d7a34f`.
No builder or CTest rerun was used to close staging.

Independent review of the first whitespace candidate found discarded supplied
root aliases, rejection of 33 exact physical CRLF EOF fragments, and rejection
of a complete 2,176-byte diff with blank context at EOF. The corrected helper
preserves and rechecks aliases, proves complete physical-source ownership, and
parses balanced full diffs; ordinary LF EOF checks remain strict. Author 60
tests and independent 63 boundary/changed-fragment checks pass. Both actual
prepare and audit ran once and exited 0. The raw Git diagnostic itself exits 2;
all 95,906 warnings in 58 files have exact source evidence: 73,208 physical-byte,
22,691 canonical CSV CRLF, five diff-context and two blank-EOF warnings. Its full
36,268,143 bytes, SHA-256
`ce93fc36cce920a0d24660ef2ac1b7f735059a96beb3b9e4e4ee82921e02a766`,
remain identical after final staging. No source/evidence whitespace was changed.
All failed, rejected and superseded tooling versions and full witnesses survive.

Ordinary banking preserved 261 source/copy and 2,981 external records. Final
banking preserves 575 source/copy and 6,959 external records; its manifest is
4,159,097 bytes, SHA-256
`89a9f4b919aa32c27af1fd4ffa550771ce7dba1e1c6ac1da65d21f278b28d9e4`.
The 576 bank files include that final manifest. The original pre-banking raw
untracked capture is preserved: 2,039 of its paths became staged, 576 later
root-bank paths were added, and ten already tracked changes give 2,625 staged
files. Only the intentional dcc_captions directory remains untracked.

The full staging v2 execution stopped on an incorrect Git-tracked-source
assumption: the dictionary gzip exists as a local source alias, not an object
in the incoming Git commit. The actual failed git-show command, exit 128 and
complete 1,478-file run are retained. The minimal v3 correction checks canonical
alias/target identity, baseline hashes, unique non-Git provenance, the frozen
semantic copy and complete original/copy bytes. The rest of v2 is byte-exact.
Fifteen focused author tests and fifteen fresh independent tests pass; the
initial dispatch-test failure is preserved. Actual full v3 execution passed.
Root reopened all 24,467 consumed file pins (2,814,784,746 bytes), all 2,625
current staged files, all 2,935 complete command records and the current index.
The final stage proof is 1,015,312 bytes, SHA-256
`53bb499f5c36f626bf184220c8987a05361676efc23eb1bb6d7ddfd7bb5079e4`.
The final bank is immutable. Its later execution, staging v2 failure/v3 fix,
reviews, actual v3 run, root acceptance and commit records remain complete
external evidence under campaign-artifacts/integration; they are not claimed
inside the earlier bank. This avoids changing an input to include its own proof.

The current through-273 global checkpoint retains its full original-base patch,
lossless archive, fixture census and fresh read-only MAIN preflight commitment.
Root verified the 49-file frozen preparation, 39 original/copy mappings and 43
exact workspace copies. The unchanged helper is approved to execute only in
the separate unfrozen workspace after the actual data/documentation heads are
supplied; the old preparation must remain immutable. No global verdict is
claimed by this ledger entry.

After that checkpoint closes, the future global cadence is six accepted pages
(18 segments), next scheduled through 291, following ALIGNMENT_LAYER_SPEC §7.
Every three-segment page still requires independent original angles,
reconciliation, semantic review, original evidence preservation, canonical
generation, all 17 fidelity suites, source/prior-data retention, independent
seven-output reproduction, exact staging/whitespace checks and a page ledger
and commit. Only the repeated whole-history archive census, complete fixture
census and full campaign-to-MAIN applicability check move to the outer cadence.
Run the global checkpoint earlier for a course boundary, deadline, material
handoff, integrity discrepancy or a substantive source/schema/algorithm/proof
helper change. A routine reviewed page-registry addition retains its page gates.
Any actual integration still requires fresh applicability against current MAIN.
The evidence-backed cadence recommendation is retained at
campaign-artifacts/integration/checkpoint-cadence-review/report.md. No measured
speedup or APFS copy optimization is claimed. Earlier commitments stay intact.

Native and installed-phone verification remain through 171; rendered inspection,
locked launch and MAIN integration remain open. No changes were applied to MAIN.
C13 remains blocked upstream. Source preparation through 303 is not accepted
alignment. Finish all 511 C05 source segments before starting C06; 177 contiguous
C06 rows are available locally. The five-minute continuation remains active
through 2026-09-15 at 07:00 America/Denver. No additional files are currently needed.

## 2026-09-13 — C05 through 276; private data commit 2602b7d7

Private data commit `2602b7d7966f3ee332393e860770f25a6b3a3f79` adds
C05:274–276 as page 92, with 2,446 exact scoped files. The previous independently
closed global checkpoint is `81942cfc6022a785894d3cc2a20de81dc4c6f599`
through 273; its final root closure is retained under
campaign-artifacts/integration/through273-root-closure/final/proof.json.
Current generated and page-verified coverage is 276/511, with 235 segments open
(277–511). The early through276 global checkpoint is still pending here.

Independent proposal, reconciliation and semantic review accepted 112 analyst
spans (42/59/11), three explicit nulls (one per segment), 81 non-null D5 pairs
(31/43/7), and three sentence anchors. All 211 original proposal identities
retain dispositions: 198 retained, 12 recast and one distributed omission.
Uncertain correspondence remains unbanked with its reason. The initial review
required three explanatory metadata corrections in 275; no approved lexical
span, source range, optional field, occurrence, note or canonical body changed.
The corrected explanations distinguish zer/claim, the residual genitive and
the negative clause's D5 correspondence from its quoted D3 parent. Complete
initial review and pre-correction originals survive. Scoped independent final
SPEC and QUALITY both APPROVE, with 16 checks passing and zero open findings.
The verdict is model review, with role/context disclosures preserved.

The final semantic freeze contains 2,286 files, SHA-256
`694dceeb5248484e702d1973c84be78b7f6b34750fa75708113338e0887fee8a`.
All 3,234 original/copy mappings, 14 governing Git objects, source archive
aliases and nine baseline pins were reopened. The canonical landing helper
omits 24 extensionless raw execution files; a separately reviewed supplement
copied exactly those 6,646 bytes with a persistent attempt/completion journal.
The complete 2,287-file semantic tree, including freeze, is byte-identical.
Rejected copy helper and failed root suffix preflight remain preserved. No new
errata or head allowances were added. GMR wording, master/source bytes and
binding hgm_gloss remain unchanged; machine attestations stay PROVISIONAL.

Both canonical builders ran once, with the exact incoming main CSV seed; all
17 targeted CTest suites passed once in 11.32 seconds. Complete actual commands,
stdout, stderr and exits are retained, including the view's normal trim output.
The bank contains 45,126 links, 11,926 nulls, 1,612 notes, 11 trees, 7,083 evidence
heads and 15,500 pairs. All 45,011 prior links and prior notes, trees, evidence,
ACIP spellings and lexical export fields remain exact. The 115 new links are
the 112 spans plus three anchors. CSV rows are 22,457 main, 22,117 reverse,
24,024 course-depth and 44 changes. All course-depth groups match main references
and metadata. Original CSV BOM/CRLF and source-page digest semantics survive.
ACIP head coverage is 7,080/7,083 (99.957645%); rounded 100.0% remains incomplete.
Three missing heads and six unchanged historical empty-ACIP rows are explicit;
five of those rows have source segments, and one historical row has seg=null.

Independent isolated reproduction used one layer build and one correctly
seeded view build. All seven complete outputs are byte-identical; 224,556
retention checks passed. Its freeze covers 3,051 files, SHA-256
`672ca1b2f1eae5c72d6259d552ecc810e177595fb73975e10661aaa4f5272280`.
Root verified all 2,979 input mappings, seven prior Git outputs, all current
source/output aliases and all twelve frozen staged payloads. No passed builders
or CTest suites were repeated for banking or staging.

The first actual whitespace prepare failed on four aliases of one preserved
historical Python SyntaxError log. Its clipped source echo ends in an original
ASCII space; the complete physical source line does not. The failed helper,
full diagnostic and all partial evidence remain unchanged. A narrowly reviewed
new predicate binds the original failed source, invocation, full streams,
manifest aliases and exact source ranges. Compile-only exception rendering
reproduces all 2,220 stderr bytes; the failed source body is never executed.
Eleven focused author tests, actual fallback dispatch and fourteen independent
negative provenance checks passed. The code and descriptor were independently
approved and fully root-verified before fresh actual prepare/audit each passed.

All 62,470 warnings in 56 staged files are accounted for: 39,554 original
physical-byte warnings, 22,912 canonical CSV CRLF warnings and four recorded
SyntaxError source-echo warnings. The full raw diagnostic is 27,652,514 bytes,
SHA-256 `cb2a68b32f29fc54d10dae2f745aff0599110ff01dab868789fe2aaed353c1dd`.
It remained byte-identical during final staging verification. No source or
evidence whitespace was normalized. This substantive predicate change requires
an early global checkpoint through276 before starting 277–279; the regular
six-page global checkpoint remains scheduled through291 afterward.

Ordinary banking preserved 123 copy records and 11,063 external records. Final
banking preserves 142 copy records and 12,611 complete external records; its
6,634,433-byte manifest has SHA-256
`0c7a9f21ac4f3e61431a9854644f3da2cfbb625d10fdcef2827c3b35673149bf`.
The original pre-bank untracked stream is retained unchanged. Exactly 2,427
initial files plus 19 later bank files were staged. The complete final verifier
passed; root reopened 15,211 consumed inputs totaling 2,227,467,786 bytes,
all 2,487 complete command records and every staged blob, then checked a fresh
identical index and unchanged HEAD. Final-stage proof SHA-256 is
`3e1d221662d00067fcb1c0534f1237c0ce98755ec8b90ee93f4548f80eb5d507`.
Only the intentional dcc_captions alias remains untracked. The final bank and
landing directory are immutable. Later final-bank/staging/root/commit/global
execution evidence is retained externally under campaign-artifacts/integration;
it is not falsely claimed inside its earlier input bank.

The pending global276 checkpoint will bind the existing reproduction to actual
data/documentation commits, produce full canonical review packages, review all
current source/history/data/documentation obligations, and measure a fresh
read-only original-base-to-final MAIN applicability conjunction. The exact
accepted whole-file partition method will be used from the outset; the prior
oversized unsplit failure stays history. No patch has been applied to MAIN.
Private JSON, HTML and CSV consumption artifacts are current through276;
native/installed-phone verification remains through171. Rendered inspection,
locked launch, current native/device consumption and MAIN integration remain
open and must not be described as deployed. C13 remains blocked upstream.
Finish all 511 C05 segments before C06 (177 contiguous local sources). The
five-minute continuation remains active through 2026-09-15 at 07:00
America/Denver. No additional files, paid API generation or release is needed.

### 2026-09-13 — Correction to the through276 checkpoint entry

Independent global review found two inaccuracies in the preceding entry's
description of the metadata correction. There was one blocking correction and
two nonblocking explanatory clarifications. The phrase "quoted D3 parent" is
incorrect: C05:275 w4, mi 'thad par / mistaken (D5), and r14, mthong med /
don't see (D3), are separate alignments. Their Tibetan ranges [28,40) and
[146,156), and English ranges [78,86) and [283,292), do not contain one another.
The intended description is "the negative evaluation's D5 correspondence from
the separate quoted D3 predicate." This corrects the ledger prose only; the
accepted source, specs, canonical bodies, ranges and generated data are unchanged.
The original entry, private commit bbf32652 and its full review packages/patch
remain preserved. The early global276 checkpoint must use the subsequent
documentation commit and newly bound packages/applicability evidence.


### 2026-09-13 — C05 segments 277–279 (page 93)

Independent proposal, reconciliation and skeptical semantic review approved
146 correspondences (61/21/64), including eight explicit null correspondences.
Uncertain correspondences remain unbanked with recorded reasons. GMR's source
wording and the master sources are unchanged; all new alignments remain
provisional and none were promoted to hgm_gloss. The complete source specs,
original proposals, review decisions, omissions and actual execution history
are retained in reviews_c05/c5p93 and the external campaign evidence manifests.

One root layer/view build and all 17 relevant fidelity suites passed. The
independent isolated rebuild reproduced all seven generated outputs byte for
byte. Full retention checks preserved all 45,126 prior links and added 149
links, including three sentence anchors. Coverage is now 279 of 511 C05
segments, with 232 remaining. The full bank has 45,275 links, 11,934 nulls,
7,084 heads and 15,517 evidence pairs. ACIP is present for 7,081 of 7,084
heads; the three missing heads remain an explicit limit. Main/reverse/course
CSV row counts are 22,491/22,151/24,061; the changes CSV has 34 data rows.

Actual prepare/audit accounted for every preserved source, CSV and historical
formatting warning without changing the evidence bytes. Final bank and staged
source checks precede this append. Complete actual later bank, stage, commit
and closure outputs are retained externally; they cannot be inside their own
earlier input manifests. The fixed DATA and documentation commit identities
are recorded in the subsequent root closure proof.

Private JSON, HTML and CSV artifacts are current through 279. Native and
installed-phone consumption remain verified only through 171. The separately
reviewed phone parser correction is still external and unadopted; a fresh
production build and complete pack fidelity check remain pending. MAIN
integration is open; its last successful read-only applicability check was
2026-09-13 at 17:25:51 UTC, before later unrelated native edits. No MAIN patch
or release was applied. Finish all 511 C05 segments before C06, then courses
in order; C13 remains blocked upstream. The next regular outer checkpoint is
291. The five-minute continuation ends 2026-09-15 at 07:00 America/Denver.

## C05 through279 — parser code verified; private phone pack withheld

Page DATA746a6f549207a4254049aab747cb898f530d1b44 and page documentation f7686dce07f03c356f565ec71c6bdbaa18cf11e7 remain valid. Separate private CODE59d668e5407b085494220f9f4d4601659ebe5dd0 changes only tools/build_drill_pack.cpp and tools/drill_evidence_json.h. These are the exact independently reviewed CP276-01 sources: JSON decoding now preserves complete English strings and integer counts for second/vocab. No source wording, source archive, spine, hgm_gloss or seven accepted page outputs changed. All machine alignments remain provisional. Code/source/object binding: campaign-artifacts/integration/through279-parser-code-commit/proof.json in the campaign root.

The original58 failures and72 passing corrected controls remain retained. A fresh current verification separately passed the unchanged historical72 checks and three complete current modes:7084 heads/15517 English-count rows in exact source order; second300 heads/1237 rows with exact source multiplicities/ranks; vocab6138 pre-conversion/FTS candidates, not6138 emitted drills. Root acceptance is through279-parser-current-root-acceptance/proof.json; actual current freeze1255234a61bc9258c3c44efbd221a4c2460d967d6746d1d1b119a56805d98c84. Existing page279 builders,17 fidelity suites and224711 independent reproduction/retention checks were not rerun for this code bookkeeping.

A fresh normal private CMake configure retained118 written/registered suites and fixtures. One target build compiled80 units and linked79 allcore object members plus the Darwin archive symbol table. Full compile/link/SDK/dependency provenance is retained at through279-private-pack-build-provenance-complete/proof.json. The new production binary is488656 bytes, SHA6be6273004f31dbda69e8ae3118de93d68a16b926be048d1e1accbb185496822. A provenance assertion originally counted the archive symbol table as an extra object; its failure and corrected retention are preserved. No successful build/test was repeated.

Exactly one external DIAGNOSTIC phone pack was produced in136.73005429 seconds:10208944 bytes, SHA6592c01f45d52621b32556155d247003d1c8380f7d4f03312c02b5db5489e6c1. Counts: cloze4000, order1000, particle1000, script1200, debate200, silent300, second300, peel250, boundary144, vocab400, trainer1000. A separately compiled, source-reviewed read-only native comparator ran once in121.01812517 seconds and matched every pack byte and ordering choice;39037 conversion entries cover18580 requests. This is observed behavior evidence, not an independent semantic oracle.

The independent strict-UTF8 source checker ran once with that trace, preserving complete schemas, raw source ownership and structured silent/peel checks. Its bounded PASS cannot approve the pack. A discovered UTF8 boundary error in the original checker was corrected through an exact one-function change and20 passing targeted tests after9 actual negative failures; all earlier failures remain. Full comparison to the old through171 pack records6775 changed pointers:2416 second,4358 vocab,1 metadata. Other arrays are structurally unchanged. Four inherited TAB-to-space English display transformations retain their complete original strings; original-byte equality is not claimed. Source checker, native trace, complete difference and review evidence are retained externally under campaign-artifacts/integration.

Independent semantic review found four INHERITED C01 standalone vocabulary failures, all also present in the old through171 pack: 'da' bar -> “gressing” (C01:180/s180w9); 'ong rgyu -> “a” (C01:170/s170w7); bden par ma grub pas -> “has any true existence” (C01:412/s412w5); bden par med pa -> “exists truly” (C01:422/s422w2). One cuts an English word, one assigns an article to distributed predication, and two lose Tibetan negation when shown alone. Exact links, complete source paragraphs/physical byte intervals and old/current exposure are preserved. No replacement English or C01 correction was composed. Separate bounded source review is required before correction or quarantine.

QUALITY is NEEDS_CORRECTION for the private pack. It remains external and UNACCEPTED; it has not replaced WT/iOS or installed content. Root disposition is through279-private-pack-root-disposition/proof.json; independent source review freeze50b9c2a12b03d8432305d67826525d093e1b456f398809a09460ae0a9ca4545b. These inherited consumer failures do not invalidate accepted C05:277–279/page93 evidence. Seven selected output locations use current page93 alignment refs; the sole direct C05:278 cloze example is corpus/factory sourced and is not evidence that new alignment generated that drill. There are278 explicit C13 addresses, zero in63–82, and no selected alignment refs toC13. Hidden dependencies are not fully attributed, and no universal C13 refusal is claimed. C13 alignment remains blocked upstream.

Coverage remains279/511 C05,232 segments remaining; full layer45275 links,11934 nulls,1615 notes,11 trees,7084 heads/15517 pairs. All45126 prior links remain. Desktop HTML/CSV data are current279; native rendering, installed phone and QSettings routing remain unverified beyond the recorded through171 boundary. MAIN integration remains OPEN; no patch, release or device operation occurred. Failed/deferred consumer disposition is recorded separately from fixed page/code acceptance. Mandatory early279 global packages/history/fixtures/partitioned read-only MAIN checks and independent/root closure remain outstanding before280 originals. The regular outer checkpoint through291 remains scheduled; it does not waive this early trigger.

Complete consumer evidence is retained at campaign-artifacts/integration/through279-parser-final-ledger-preparation/complete-consumer-evidence-manifest.json. It lists complete full-byte external originals, including source specs/reviews, actual commands/results, corrected and failed tooling, binaries, data, dependencies and current measured coverage. This ledger and later checkpoint callers are subsequent evidence, not falsely self-banked. Continue C05 beforeC06; no spend, paid API, account messages, release or overwrite of others' work. Standing local authorization requires no further user confirmation.


### 2026-09-14 UTC — C05 segments 280–282 (page 94)

Independent proposal, reconciliation and semantic review approved 124
provisional correspondences (54/46/24), including four grammatical nulls and
102 nonnull depth-5 correspondences. All 225 original proposal dispositions,
15 material omission reasons and 11 investigated/refuted suspicions survive.
Exactly two source-specific sogs head licenses were added; no errata were
added. Uncertain correspondences remain unbanked. GMR's wording and master
sources are unchanged; none of this machine work enters hgm_gloss. Complete
sources, specs, proposals, decisions, generator results and semantic review
are retained in reviews_c05/c5p94 and the external campaign evidence records.

One root layer/view run and all 17 relevant fidelity suites passed; the suite
took 12.62 seconds. The independent isolated reproduction matched all seven
generated outputs byte for byte with the immutable incoming main CSV seed.
Full retention preserved 45,275 prior links and added 127, including three
sentence anchors. Coverage is 282 of 511 C05 segments, with 229 remaining.
The full bank has 45,402 links, 11,938 nulls, 1,618 note blocks, 11 trees,
7,092 heads and 15,546 evidence pairs. Proven ACIP covers 7,089 of 7,092
heads; three missing heads and five source-linked spans remain explicit
limits. Main/reverse/course CSV rows are 22,528/22,188/24,105; changes has
37 rows. Producer metadata date is September 13 local time; the execution
records are September 14 UTC. No date-driven regeneration was performed.

The actual 13-file payload includes the two-license sidecar and preserves
all prior sidecar values. Staged whitespace checks account for the original
source and CSV bytes without normalization. Complete current bank and stage
checks precede this append. Later bank, stage, commit and closure results
remain external at their true boundaries; they are not claimed inside their
own earlier input manifests. Source-preparation failures and corrections
are preserved. Three retention-preparation caller results are explicitly
labeled conversation transcriptions: original raw streams were unavailable.
This limitation does not replace the full actual generator/build/test or
independent reproduction captures. Fixed DATA and documentation commit
identities are recorded by the subsequent root closure.

Private JSON/HTML/CSV outputs are current through 282. MAIN remains
OPEN_NO_APPLICATION; the September 13 23:40–23:42 UTC compatibility check
passed all 13 chunks but applied nothing. The new private phone pack remains
FAILED_UNACCEPTED for four inherited C01 source-correspondence defects and
must not be installed. The existing through171 phone resource is unchanged;
no current native rendering, device installation or saved data-root acceptance
is claimed. No release, paid API, spending, account message or source/gloss
correction occurred. C13 remains blocked upstream. Finish all C05 through511
before C06, then proceed in order. Next regular outer checkpoint is291.
The existing one-minute continuation ends September 15, 2026 at07:00
America/Denver (13:00 UTC); safely checkpoint and pause at the deadline.

### 2026-09-14 UTC — C05 segments 283–285 (page 95)

Independent proposal, reconciliation and semantic review approved 76
provisional correspondences, with no grammatical nulls and 61 nonnull
depth-5 correspondences. All 145 original proposal dispositions survive:
129 were retained and 16 recast. The review preserves 77 material omission
reasons and four investigated/refuted suspicions. Exactly two source-specific
sogs head licenses were added; no errata were added. Uncertain correspondences
remain unbanked. GMR's wording and master sources are unchanged; none of this
machine work enters hgm_gloss. Complete sources, specifications, proposals,
review decisions and actual generator/test results are retained in
reviews_c05/c5p95 and the external campaign evidence records.

One root layer/view run and all 17 relevant fidelity suites passed; the suites
took 11.25 seconds. The independent isolated reproduction matched all seven
generated outputs byte for byte with the immutable incoming main CSV seed.
It passed 225,597 retention checks with 3,031 input aliases. Full retention
preserved 45,402 prior links and added 79, including three sentence anchors.
Coverage is 285 of 511 C05 segments, with 226 remaining. The full bank has
45,481 links, 11,938 nulls, 1,621 note blocks, 11 trees, 7,093 heads and
15,557 evidence pairs. Proven ACIP covers 7,090 of 7,093 heads; three missing
heads and five source-linked spans remain explicit limits. Main/reverse/course
CSV rows are 22,546/22,206/24,126; changes has 18 rows. Producer metadata date
is September 13 local time; execution records are September 14 UTC. No
date-driven regeneration was performed.

The 13-file payload includes the two-license sidecar and preserves all prior
sidecar values. Actual whitespace prepare and audit account for all 93,740
native warnings in 112 files through exact physical/source/CSV predicates;
the original Git diagnostic and exit 2 are retained without normalization.
Complete final bank and staging verification precede this append. Later bank,
stage, commit and closure results remain external at their actual boundaries;
they are not claimed inside their own earlier input manifests. All failed
preparations, superseded sources and corrections are preserved, including the
failed first reproduction preparation, three whitespace source-analysis
failures, the root intake recipe-reader failure, and the independently reviewed
STAGE-001 variable-shadowing correction. Native captures distinguish source
preparation from actual execution. Fixed DATA and documentation commit
identities are recorded by the subsequent root closure.

Private JSON/HTML/CSV outputs are current through 285. MAIN remains
OPEN_NO_APPLICATION; the September 13 23:40–23:42 UTC compatibility check
passed all 13 chunks but applied nothing. The private phone pack remains
FAILED_UNACCEPTED for four inherited C01 source-correspondence defects and
must not be installed. The existing through171 phone resource is unchanged;
no current native rendering, device installation or saved data-root acceptance
is claimed. No release, paid API, spending, account message or source/gloss
correction occurred. C13 remains blocked upstream. Finish all C05 through511
before C06, then proceed in order. Next regular outer checkpoint is291.
The existing one-minute continuation ends September 15, 2026 at07:00
America/Denver (13:00 UTC); safely checkpoint and pause at the deadline.

Final verification exposed a source-archive ownership lookup mismatch; a
separately reviewed correction verifies both complete original-to-copy chains.
Its two real archive checks and twenty rejection controls passed. A later full
verification attempt then stopped on internal-disk exhaustion. Its partial
captures remain unaccepted; unavailable target exit and outer streams were not
reconstructed. A fresh complete final-stage run is required before this append.

Five closed C05 campaign patch records, totaling 5,053,241,758 bytes, were copied
to the attached Oct2024(8TB) drive and compared byte for byte. Their existing
filenames now resolve to those verified copies through individual symlinks.
The first temporary-link creation itself failed for lack of space; the first
path was restored after an explicitly recorded brief gap, and the remaining
four replacements were atomic. Original bytes and historical physical metadata
are preserved. No worktree, MAIN, semantic, master-source or index file moved.
Recovery evidence is under the external campaign archive's
storage-recovery-20260914T1200Z directory. Keep that drive connected for evidence
verification. These later storage and verifier records remain outside their own
earlier input manifests, with both failures and successful recovery retained.

## C05 286–288 verified — 2026-09-14T13:51:43.732934+00:00

Private DATA commit 1a5e457403400e0d3bca86a482189e0f38eff8fe; incoming 129e86afae8431aacfb1ee1cee65d8635dd265b8. C05 coverage is 288/511, with 223 segments remaining (289–511). Independent original proposals, reconciliation and fresh semantic review accepted 137 provisional spans: 99 non-null depth-5 correspondences and five nulls, plus three source anchors. A wrong-clause negation in an original proposal was corrected before acceptance. No source errata or new head allowances were required. GMR wording remains verbatim; nothing is promoted into hgm_gloss.

Both canonical builders ran once in 13.67 seconds; all 17 fidelity suites passed once in 11.46 seconds. Independent complete-byte reproduction of all seven generated outputs passed with the actual incoming CSV seed and producer date 2026-09-14. All 45,481 prior links and existing notes, trees, evidence and CSV content were retained. Totals: 45,621 links, 11,943 nulls, 1,624 notes, 11 trees, 7,102 heads and 15,587 pairs. ACIP is evidenced on 7,099 of 7,102 heads; five source-linked spans remain unproved. CSV rows main/reverse/course/changes: 22,594/22,254/24,177/48. One existing klung→stream row gained newly witnessed KLUNG ACIP from C05:287.

Source specs, complete review decisions and readable semantic evidence are banked under reviews_c05/c5p96. Exact native checks are retained in its compact verification archives and manifests. All 43,922 whitespace warnings in 20 files were accounted for by exact original-source or canonical CSV formatting; final staged diagnostics are byte-identical to the accepted audit. No source bytes were normalized. Final accepted index and Git closure records are in campaign-artifacts/C05-286-288/compact-closure outside the checkout. Immutable evidence is stored once and proposal/review stages overlap; no fidelity or semantic gate was removed.

Generated HTML/CSV consumption paths in the isolated checkout are refreshed and mechanically verified. MAIN remains OPEN_NO_APPLICATION. Private iOS pack remains FAILED_UNACCEPTED for four inherited C01 semantic findings; no installation, new native/device/rendering or saved data-root acceptance is claimed. Installed resources through171 remain unchanged. Keep the external evidence drive connected. Continue C05 before C06; C13 remains blocked upstream. Existing one-minute automation ends 2026-09-15 at 13:00 UTC, then checkpoint and pause. No spending, paid API, account messages, releases or changes to others' work.

## C05 289–291 verified — 2026-09-14T14:07:41.313328+00:00

Private DATA commit 711b6bdea7b7c71a0689832791cc6c82df92cd36; incoming ba35c1bd2007e0b43eebe7661e89d420c1a1fc3e. C05 coverage is 291/511, with 220 segments remaining (292–511). Independent original proposals, reconciliation and fresh semantic review accepted 109 provisional spans: 67 non-null depth-5 correspondences and two grammatical nulls, plus three source anchors. All 191 original proposals have recorded dispositions. No new erratum survived seven five-ground source-suspicion checks. Only the specifically approved sogs/and so on head allowance c5p97/s290w23 was appended, with prior allowance bytes retained. GMR wording remains verbatim; no hgm_gloss promotion.

Both canonical builders ran once in 13.53 seconds; all 17 fidelity suites passed once in 10.83 seconds. Independent complete-byte reproduction of all seven outputs passed using the actual incoming CSV and producer date 2026-09-14. All 45,621 prior links, notes, trees, evidence and prior CSV content were retained. Totals: 45,733 links, 11,945 nulls, 1,627 notes, 11 trees, 7,104 heads and 15,609 pairs. ACIP is proved for 7,101 of 7,104 heads, with five source-linked spans unproved. CSV main/reverse/course/changes rows: 22,631/22,291/24,219/37. No prior CSV lexical or ACIP content was altered.

Readable source specs and independent semantic decisions are banked under reviews_c05/c5p97; compact verification archives retain exact native streams and member maps. The 278-file semantic freeze and all 297 original-to-copy records passed identity checks. All 27,892 whitespace warnings across six files preserve source or canonical CSV formatting; final staged diagnostics are exactly unchanged. No source normalization. Exact stage/tree and append-only ledger binding is recorded under campaign-artifacts/C05-289-291/compact-closure.

Failures remain explicit: root's first retention command omitted two required segment arguments and stopped before checks; corrected invocation passed. The first reproduction preparation was mistakenly launched before its required retention proof existed and stopped before any builder; the subsequent fresh destination ran each isolated builder once and passed. The semantic review's source-archive/Git bookkeeping failure and superseded planning attempts remain preserved. Registration v2 passed 21 isolated fixtures; v3 only accepts the frozen review's equivalent explicit source_origin_commit field, with actual named-origin/ancestor checks retained and root-tested. No final source spec or approved meaning changed to accommodate tooling.

Isolated desktop HTML/CSV consumption paths are refreshed and mechanically verified. The regular outer checkpoint through291 remains OPEN: its current MAIN applicability/whole-fixture census and independent fixed-checkpoint review still need completion. MAIN was read-only observed clean at bfc0162b on September14; no changes were applied. Private phone pack remains FAILED_UNACCEPTED for four inherited C01 semantic findings; installed resources through171 are unchanged. No device/rendering or saved data-root acceptance is claimed. Before accepting294, close or precisely record the outer integration limits using current facts without recursive historical evidence copies. Keep the external evidence drive connected. C05 before C06; C13 blocked upstream. Deadline September15,2026 at13:00UTC; then checkpoint and pause the one-minute automation. No spending, paid APIs, account messages, releases or overwriting others.

## C05 292–294 verified — 2026-09-14T14:47:38.314574+00:00

Private DATA commit ed8335e83520a81758b7d2b4a61848e4deaf1d71; incoming c107f1ecd9cfb68968245870ad81ba5d9a070721. C05 coverage is294/511, with217 segments remaining (295–511). Independent Tibetan-first and English-first proposals, reconciliation and fresh semantic review accepted111 provisional spans:65 non-null depth-5 pairs,16 compound members and4 grammatical nulls, plus3 source anchors. All210 original proposals retain exact recorded dispositions. Seven source-suspicion screens considered all five grounds; no new erratum was accepted. Only the three specifically reviewed la sogs pa/sogs head allowances c5p98/s292w3, s292w22 and s293w12 were appended. All prior allowance bytes and192 errata records are retained. GMR wording remains verbatim; no hgm_gloss promotion.

Both canonical builders ran once in13.28 seconds; all17 fidelity suites passed once in10.53 seconds. Independent complete-byte reproduction of all7 outputs passed from the actual incoming CSV seed and producer date2026-09-14. All45,733 prior links and prior notes, trees, evidence, ACIP and CSV content were retained. Added114 links (33/35/46 by segment). Totals:45,847 links,11,949 nulls,1,630 notes,11 trees,7,110 heads and15,632 pairs. ACIP is proved for7,107 of7,110 heads;5 source-linked spans remain unproved. CSV main/reverse/course/changes:22,675/22,335/24,267/44 rows. All24,219 prior course/depth groups remain. No prior ACIP enrichment or wording change was needed.

The full independent review corrects an evidence-attribution overstatement: HGM8829's curated drang don gloss is the figurative; the actual local expanded predicate and adverb are contextual corpus attestations. Those defensible source correspondences remain, and the original overstatements are preserved with the explicit correction. The review also retained the tight positive fruitlessness/bare mapping and separately resolved repeated perception and meaning roles. The343-file semantic freeze,415 original-to-copy records and41 historical Git references passed identity checks. Source origin129e86af through285 and actual acceptance baseline c107f1ec through291 remain distinct. No acceptance was inferred from historical generator success.

Readable specs, original/final decisions and source evidence are banked under reviews_c05/c5p98. Compact native evidence and post-bank archives preserve exact streams with original paths and byte hashes. All32,638 whitespace warnings across7 files are accounted: complete physical C05/C15 source, canonical CSV CRLF and3 original print-separator spaces. Whitespace helperv8 retains allv7 checks and adds only reconstruction of that frozen84-byte native stdout from its3 actual frozen body/stderr/exit records. Root reviewed this bounded addition; it is not an independent semantic review or source normalization. Final staged diagnostics remain byte-identical. Full staged Git tree, accepted blobs, DATA parent/tree and three identical append-only ledger additions are bound in campaign-artifacts/C05-292-294/compact-closure/proof.json.

Preserved failures include original occurrence/planning and physical-decoding attempts, the reconciler's corrected293 occurrence plan, and the semantic supplement's incorrect assumption that the external master was Git-tracked. Remaining supplement checks resumed with the correct external/Git distinction; no source, final spec or generator was rewritten for bookkeeping. Root's initial original-reason intake assumed every member had individual prose;14 English members legitimately refer to parent/shared rationale. Their actual missing original-reason field and explicit reconciliation decisions remain distinct. All current production/generator/17-test/retention/reproduction calls passed on their first invocation.

The bounded outer291 assessment is complete with explicit integration limits. Independent review verified the15-commit history from273, all301 accepted291 blobs, current7-output binding, three append-only ledgers and the whole inherited fixture census: MAIN1,912 and campaign1,935 primary files, with1,877 captions retaining640,921,656 bytes. Actual native git apply --check passed for all119 production/document paths against MAINbfc0162b on September14. The33,401 audit additions have zero destination/ancestor collisions; their full native applicability remains UNTESTED. Nothing was applied to MAIN. Exact assessment and240-file freeze are referenced in integration/through291-current-assessment, with root disposition integration/through291-root-disposition.json. Historical reference correction: the successful288 closure is C05-286-288/compact-closure-attempt02/proof.json; compact-closure is the preserved failed first attempt.

Isolated desktop HTML/CSV consumption is refreshed and mechanically verified through294. MAIN remains OPEN_NO_APPLICATION. Private phone pack remains FAILED_UNACCEPTED for four inheritedC01 semantic findings, with no installation; installedthrough171 status is carried from historical evidence, not a new device check. Current native/rendering/QSettings/device acceptance remains open. Before eventual MAIN application, perform then-current complete applicability and consumer checks. The next broad six-page cadence boundary is309; every three-segment batch still requires its own semantic/fidelity/retention/reproduction/commit gates. External evidence drive remains required. C05 must finish beforeC06; C13 remains blocked upstream. Deadline September15,2026 at13:00UTC; safely checkpoint and pause the one-minute automation then. No spending, paid APIs, account messages, releases or overwriting others.

## C05 295–297 verified — 2026-09-14T15:22:07.136008+00:00

Private DATA commit 6a551b6bf39e80c067aa2a834173ca16d4b4bc8b; incoming ce646d0c48f9338ff47e40f46582ad14c8a3eaf4. C05 is297/511, with214 segments remaining (298–511). Two independent original angles, reconciliation and fresh skeptical semantic review accepted134 provisional spans:82 non-null depth-5 pairs,16 compound members and2 justified grammatical nulls, plus3 source anchors. All259 original proposals retain exact recorded dispositions. Six complete five-ground suspicion screens were refuted; no new erratum. Only c5p99/s297w43 sogs/and the other was allowlisted, with source-specific licensor and the supplied two excluded. All prior allowance bytes and192 errata records are retained. GMR wording remains verbatim; no hgm_gloss promotion.

Both canonical builders ran once in14.17 seconds; all17 fidelity suites passed once in11.78 seconds. Independent full-byte reproduction of all7 outputs passed from553 registered pages, the actual incoming16,344,258-byte CSV seed and producer date2026-09-14. Every45,847 prior link and all prior notes, trees, evidence and ACIP indices remain. Added137 links (53/34/50 by segment). Totals:45,984 links,11,951 null exponents,1,633 notes,11 trees,7,115 heads and15,655 pairs. ACIP is proven for7,112 of7,115 heads;5 source-linked spans remain unproved. CSV main/reverse/course/changes:22,715/22,375/24,314/40 rows. All24,267 prior course/depth groups and prior CSV content/references remain. One existing depth-7 sreg/red-hot export acquired proven ACIP SREG from three newly banked source witnesses at295/296; its wording, tier and provenance fields were not rewritten. This verified source enrichment is explicit in retention-source-check.json.

The fresh review confirms all7 mthong and3 dang por occurrences in295, both complete stop-seeing recasts, contextual zhen attribution distinct from curated craving/desire, and the bare-branches restriction. It checks the distributed prevention grammar in296 without false nulls, and three source rivers versus four English mentions plus complete potential phrases in297. Both nulls are active-subject kyis with no separately alignable exponent. Full positive compound alternatives and omitted expansions were considered. The484-file freeze af2021f6085ca1d2ed4884d971fcb5c63f67975cd5f2cc9bd18c87f3e5a30732,459 exact copy mappings and36 historical Git references passed root identity checks. Source originba35c1bd through288 and actual acceptance baselinece646d0c through294 remain distinct. External archives were compared directly; historical mutable governance uses named Git objects.

Readable source/spec/review/decision material is banked under reviews_c05/c5p99. Compact native and late archives retain exact original paths and stream hashes. All32,663 whitespace warnings across12 files are accounted by unchanged v8: full physical C05/C15 bytes, five exact C05 source excerpts, one frozen diff-context space, and canonical CSV CRLF. The20,154,851-byte native diagnostic SHA25692dccf07dc849e3079a13079ca56644457bf6814da8c92e0e99bac5d76e97aaf remains byte-identical across failed/successful classification attempts and final staging. No source normalization or whitespace gate change occurred. The entire staged Git tree, accepted blobs, DATA parent/tree and three identical append-only ledger additions are bound in campaign-artifacts/C05-295-297/compact-closure/proof.json.

Failures are preserved, including original/reconciliation setup histories; the semantic review's flat-versus-course-keyed archive assumption and difflib autojunk bookkeeping correction; and root's initial empty whitespace pin map. Root's first classification preparation wrongly expected a single witness path; the next wrongly expected a unique complete file across matching C05/C15 parallels. Both refused before writes. A premature second audit repeated the unchanged missing-pin refusal. Exact C05 excerpt ranges and all equivalent C05 source copies were then verified with existing v8 kinds; actual audit attempt03 passed. Successful source, semantic, canonical-builder,17-test and reproduction stages were not repeated for packaging. Root's earlier four-mthong brief error is preserved with the explicit seven-occurrence correction.

Outer291 integration assessment remains the latest bounded broad check: all119 production/document native applicability checks passed against MAINbfc0162b;33,401 audit additions were collision-checked only and full native applicability remains UNTESTED. Assessment240-file freeze/root disposition remain in integration/through291-current-assessment and integration/through291-root-disposition.json. No new broad MAIN test or application is claimed. The successful288 closure path remains compact-closure-attempt02/proof.json. Next broad six-page boundary309; each three-segment batch still passes its own full acceptance gates.

Isolated desktop HTML/CSV consumption is refreshed and mechanically verified through297. MAIN remains OPEN_NO_APPLICATION. Private phone pack remains FAILED_UNACCEPTED for4 inheritedC01 semantic defects; no installation. Installedthrough171 is historical evidence, not a fresh device check. Current native/rendering/QSettings/device acceptance remains open. Preserve others' work and the external evidence drive. C05 must finish beforeC06; C13 remains blocked upstream. Deadline September15,2026 at13:00UTC: safely checkpoint and pause the existing one-minute automation then. No spending, paid APIs, account messages, releases or source edits.

## C05 298–300 verified — 2026-09-14T16:10:49.360495+00:00

Private DATA 921bb556c6b51a1e7c9c871632604a8efd76ee80; incoming 079507630dbfff60fc32278d8780ce56509cbd2a. C05 is300/511, with211 remaining (301–511). Two independent original proposals, reconciliation and fresh skeptical semantic review accepted154 provisional spans:78 non-null depth-5 pairs,36 compound members and1 grammatical null, plus3 source anchors. All269 original dispositions retained:240 exact proposals and29 recasts/depth decisions. All15 material omissions reviewed; uncertain and distributed correspondences remain unbanked. GMR wording and source bytes remain verbatim; no hgm_gloss promotion.

Fresh SPEC/QUALITY APPROVE,0 open findings. Frozen571 files SHA0e021025842a374afe1ea0a01f18d038c5388587c9147107c824200d8a0c0427;527 original-copy mappings with37 historical Git references verified exact by root before and after landing. Source origin c107f1ecd9cfb68968245870ad81ba5d9a070721 through291 remains distinct from actual acceptance baseline079507630dbfff60fc32278d8780ce56509cbd2a through297. Reviewer reopened all330 central master occurrences across109 queries and additional full master/corpus witnesses. Final validation checked all154 seven-key resolved spans; three fresh actual semantic generators exited0 with exact bodies.

One LOW/PROBABLE editorial record E-193:298 less that what they really are → less than what they really are. Complete physical C05 line2956, full C05:298/C15:443/P7:39 and actual register classes were independently checked across all five grounds. Found2 corpus rows versus corrected1; C15 preserves Tibetan and changes only that/than, P7 shares704/718 English characters and omits14. Digital parallels do not establish independent publication or ingestion lineage.299 appears/to exist and300 lha yi/la suspicions refuted; source untouched. Three reviewed sogs allowances use exact distinct source/English ranges. All192 old errata and old allowance bytes retained.

New separately root-reviewed nonempty-errata registration helper SHA9aa0682f996325b435cc1d8126315c5744b2aff796cebc6263440cb99a0c3886 passed64 actual fixtures; complete preparation/native evidence archived once. Actual production registration passed with explicit accepted errata/freeze hashes, original proposal-spine binding, unchanged canonical merge/prose/gates in isolated staging, exact approved five-file delta and original bytes/modes preserved. Successful owned source DB copy removed with identity proof; source unchanged. Seven native commands and recovery evidence banked. Canonical prose's inherited stale coverage sentence remains an explicit integration limit; it is not current campaign coverage.

Both canonical builders ran once in15.14 seconds; all17 fidelity suites passed once in12.46 seconds. Independent reproduction from554 registered pages, actual incoming16,372,232-byte CSV seed and producer date2026-09-14 matched all7 complete outputs. All45,984 old links, prior notes, trees, evidence, ACIP and CSV content/references retained. Added157 links (44/51/62). Totals46,141 links,11,952 null exponents,1,636 notes,11 trees,7,123 heads and15,684 pairs. ACIP proven for7,120/7,123 heads;5 source-linked spans remain unproved. CSV main/reverse/course/changes22,774/22,434/24,377/59. All24,314 prior course/depth groups retained; no existing CSV ACIP enrichment this batch.

All33,118 whitespace warnings across16 files accounted by unchanged v8: two complete original C05 copies, ten exact physical excerpts and canonical CSV CRLF. Native20,013,009-byte diagnostic SHA79c7c720de8cd8c0c0f529d3e728a292ed0767d36eacee4796e64daf526c868d is unchanged. First bank refused because root named the already passing CSV proof differently; exact saved result copied to the required interface, no checks rerun. First whitespace audit refused because root's preparatory scan omitted .bin physical excerpts; all seven were located exactly in full C05 and pinned with the existing class. Both failed callers and old methods preserved; successful bank and whitespace callers are attempt02. Semantic body-parser and inherited original failures remain recorded. No passing source, generator, test or reproduction stage was repeated for packaging.

Readable specs/reviews/decisions and compact native/late archives banked under reviews_c05/c5p100. Full accepted staged bytes/modes/tree, DATA parent/tree and three identical append-only ledger additions verified by campaign-artifacts/C05-298-300/compact-closure/proof.json. Next301–303 fresh review overlaps this checkpoint;304–306 reconciliation complete, awaiting root intake and fresh review. Sequential acceptance remains three segments at a time.

Isolated desktop HTML/CSV mechanically verified through300; rendering/native/QSettings acceptance remains open. MAIN remains OPEN_NO_APPLICATION. Latest broad outer291 assessment:119 production/document native applicability passes against MAINbfc0162b;33,401 audit additions collision-only, full native applicability UNTESTED. Next broad integration boundary309; recheck then-current MAIN and preserve others' work. Private phone pack remains FAILED_UNACCEPTED for4 inheritedC01 semantic defects; no installation. Installedthrough171 is historical evidence, not a new device check. External evidence drive required. Finish C05 beforeC06; C13 blocked upstream. At September15,2026 13:00UTC safely checkpoint and pause existing one-minute automation. No spending, paid APIs, account messages, releases or source edits.


### C05 301–303 — verified provisional checkpoint, 2026-09-14T17:11:10.294108+00:00

DATA bdd7309fb5e96be805276a3f9721d761828230b9 follows verified predecessor b5047596859f6537b71f259c1e7df2e70f867fb2. C05 now303/511 segments;208 remaining304–511. Source/governance origin ce646d0c48f9338ff47e40f46582ad14c8a3eaf4 through294 remains distinct from actual acceptance baseline through300. Source wording/master/corpus unchanged; machine alignments remain provisional and never enter hgm_gloss. C13 stays blocked upstream. Campaign deadline September15,2026 13:00UTC.

Two independent originals, reconciliation, fresh skeptical semantic review and root judgment accepted104 spans/6 justified grammar-fusion nulls/57 non-null depth5 pairs/10 depth7 members plus3 source anchors. All197 original dispositions retained:175 exact tuples and22 explicit recasts;14 distributed or uncertain contributions remain unbanked with reasons. Fresh review APPROVE with zero open findings;843-file review freeze071b68bccae17ffcf5b75747545347a687e40bea7b9a4ea629d2eeecf49eb241. Current96-file reconciliation freeze30d8d73f4a02c09ac58c75ebb99fd3e18e83352812d891275c152d9269b44dfb. Root read all original/final decisions, all104 fresh judgments/14 omissions/full five-ground errata reports and complete final report changes. Mechanical original-copy verification886 records/707 distinct targets/12 distinct historical Git objects passed before and after landing; source/governance and actual predecessor bound separately.

Fresh review corrected302w23 thal/must from depth6 to depth5 because it is the Tibetan lexical consequence predicate. Wording and all ranges unchanged. Only302 was regenerated once by root and once independently by the reviewer; successful301/303 evidence was reused. Complete prior95-file reconciliation/root/package boundary and prior63-file NEEDS_CORRECTION boundary preserved with byte-verified remapping.55 older copies relocated under the explicit history prefix, retaining exact bytes. Root's first correction wrapper failed its data-a/data-l matcher after native generator success; corrected wrapper reused that saved output. An initial root copy-path assertion also failed before the separate mechanical acceptance call succeeded; the saved root judgment explicitly records the chronology and does not treat that mechanical call as earlier semantic approval. No worktree write preceded actual root approval. Original failed callers and helper sources remain preserved.

Editorial record E-194 is LOW/PROBABLE302 internal typo: “and then as the sizzle in his throat and stomach” → “and then as they sizzle in his throat and stomach”. The full original wording is preserved; the correction is an editorial record only. Exact duplicate C15:447 and related P7:41 are internal evidence, not independent publication witnesses. All193 prior errata retained,194 total. All three source-specific sogs span-head allowances reviewed. The approved nonempty-errata registration helper9aa0682f996325b435cc1d8126315c5744b2aff796cebc6263440cb99a0c3886 executed successfully with explicit accepted errata/review-freeze hashes and the exact approved five-file registration delta. Its complete64-fixture method archive was banked once at300; this batch stores a precise reference rather than copying that tree again.

Canonical builders executed once in13.881941seconds; all17 relevant CTest fidelity suites passed once in11.39seconds. Retention/course checks passed. Totals46,248 links/11,958 nulls/1,639 note blocks/11 tree blocks;7,131 heads/15,704 pairs. ACIP7,128/7,131 heads,5 source-linked spans still unproved. CSV22,809 main/22,469 reverse/24,414 course/35 changes. All46,141 prior links,22,774 main rows,24,377 course-depth groups, notes, trees, evidence, ACIP, existing content and references retained. No existing CSV ACIP enrichment in this batch. Canonical published view retains its inherited42 row-depth shortfalls/28 trim calls; these are explicit integration limits.

Independent compact reproduction at /Volumes/Oct2024(8TB)/Codex-ACI-Alignment-20260915-01a09398/compact-reproduction-through303 passed all seven complete outputs byte-for-byte with555 registered pages, producer date2026-09-14 and16,414,467-byte incoming CSV seed c1aa3aaa7533403ae22515a73f9c47e4fcf562bd015cad5340a7133159803840. Final SHA-256: full1cdd6476d47235d485f26b21b15ea5368c946f1ed2e90e764bfd139ff674d28c; evidence0d418780cd63c6c85f4f357cebeb231949ed30a5a9e450ab3f95cda398578b27; HTML191bd1831296fd42017b3126459c84a518e26fd894398e69f64d2dd698395b48; main910437c4e858d2718f9b78e9389c5a5b9d7348e3defce087559084ef24ee8552; reverse314e5fb0084a494e3d0c55bba7844d6f5bed5787d12d992a9490b9b19f23a344; coursef96b539f4061d8666897fd6119caedb44137519b78c4c43c7b8142fda34e2bd6; changes34ad6f07278db506bfea67855d236323514e049733438d127ec1095d71d0bbd7.

Whitespace audit v9 PASS attempt02:47,501 warnings in26 files, every warning accounted for. Complete22,680,113-byte diagnostic SHA1c0d0311b2b8ef6ea675454558be19a128ad764900647f1bd95bebecf4c58962. Initial unchanged v8 refused four trailing print separators in an exact433-byte historical native log. The original log/failed diagnostic/pins remain byte-identical. Root-reviewed private v9 b3029500b9001e4cee44f5db6de000f6e5c7b163da6f15a418c71a5b2b7b7e86 adds only a fixed-log/fixed-helper class reconstructing four original zero-exit stdout/stderr pairs through18 independently frozen provenance witnesses.33 focused controls pass,35 protected identities unchanged, complete old v8 recovered exactly by removing three additions. The first synthetic fixture mistake is preserved. Existing physical/source/CSV classes and every linguistic gate remain unchanged. No source normalization or passing generator/test/reproduction rerun for this bookkeeping correction.

Native source specs, review decisions, generator streams, gates, retention/coverage/reproduction results, failed attempts and bounded workflow-method evidence are in this page's verification archives with byte manifests. Post-bank closure checks exact staged blob bytes/modes/tree, unchanged full whitespace diagnostic, DATA.parent=baseline, DOCS.parent=DATA, and three identical append-only ledger additions before declaring a clean private checkpoint. Source-specific future proposal protocol now emphasizes actual Tibetan function, exact repeated-occurrence rationales, legitimate multiple contiguous depth7 children, non-null pair counts and unchanged-proof reuse. Historical frozen protocols remain untouched. Later reviews overlap; acceptance remains sequential.

Desktop HTML/CSV consumption through303 mechanically verified in isolated checkout only; rendering/native/QSettings acceptance remains open. MAIN OPEN_NO_APPLICATION; no MAIN changes. Latest broad assessment remains through291:119 production/docs native applicability passed against then-MAIN bfc0162b,33,401 audit additions checked for collisions only, full33,520-file native applicability untested. Next broad boundary309 must recheck current MAIN and preserve other work. Private phone pack remains FAILED_UNACCEPTED on four inherited C01 fidelity failures; no installation. Historical installed phone through171 only. C05 completion, C06 and later courses remain outstanding. No paid API, spending, usage reset, account messaging or release occurred.


### C05 304–306 — verified provisional checkpoint, 2026-09-14T17:32:30.334979+00:00

DATA cce3bdef61a1bbab8009540b6a5004bc3d7b492e follows verified predecessor f1651c64b0ad9b0c7ea85f9961332ac1a3787e83. C05 now306/511 segments;205 remain307–511. Historical source/governance ce646d0c48f9338ff47e40f46582ad14c8a3eaf4 through294 stays distinct from actual303 baseline. GMR/source/master/corpus bytes preserved; machine alignments stay provisional and never enter hgm_gloss. C13 blocked upstream. Campaign deadline September15,2026 13:00UTC.

Two independent originals, separate reconciliation, independent skeptical review and root acceptance approved75 spans/12 supported grammar-fusion nulls/43 nonnull depth5 pairs,44totald5. Segment counts30415/4/7,30522/3/15,30638/5/21; five depth7 entries include one null. Three source anchors add78layerlinks. All141 original dispositions retained with individual reasons. All16 omission/gap judgments and allfive full source-error screens read by root. Unknown/distributed correspondences remain unbanked. Two source-specific complete sogs closing gestures retain their precise head allowances.

Fresh SPEC/QUALITY APPROVE0open at1700-file freeze06678172d0eef7bdbe90807164806dd278c60e561b71d6a420de7a9e0d1b77c2; root full prior review and complete scoped report changes read. Current reconciliation1066filefreeze45becb5b818c5db6381966f3ea4e2499efb6ff010da9d206b6c700eb6e72c356. Root native/copy proof2866 current original-to-copy mappings passed before and after landing, including exact historical Git objects. Actual303 baseline/register194 copied separately, without replacing historical governance.

F1 restored original English306m1 as mPus rnag/pus W[47:51]E[145:148], under w5 rnag khrag/pus and blood before retained w6 khrag/blood. Both are independently meaningful, unique and strictly tighter; canonical with_members/resolve supports multiple contiguous children. The unsupported one-child cap is preserved as rejected history. F2 corrected first rab rib/cataracts w10 rationale atW[86:93]E[227:236] under first rab rib can/person with cataracts, not the later sense-power verse. Existing74 seven-key tuples, all previous spec annotations/notes/English order and ranges unchanged. Removing only two newmPus HTML wrappers reproduces the old306body exactly.

Corrected306 generated once by corrector and once independently by fresh reviewer, exit0/empty stderr/exactbody/source/ranges. Passing304/305 and unchanged original lexical/source/errata judgments reused; no root third generator repeat. Current306spec6835bytes SHA6cab22351bdbb24d50d9a01f2fa0f22ef0c3c87dd94a95eff6228669acc65e6b;body6833bytes SHA8d64f640cc64da104fd6dbb94a87e81988132cad67f4133793b43784d48a49a2. Prior whole798-file semantic boundary and1085-file old reconciliation/root boundary preserved exactly. Root-history-byte-bank-check proves every one of1085 historical files already byte-present in the current semantic bank; the explicit path map is archived without recursively duplicating that tree.

Zero retained errata; register194 unchanged. Original306 authorship candidate MEDIUM/PROBABLE preserved, refuted for filing as LOW/UNCERTAIN bibliography. C05:247/C16:823 share only the second line; ILL:2323/2319–2328 provide contextual support, not a verified independent edition establishing the bracketed author's replacement. Original water/yiDVAGS/basin-rhinoceros/hair-comb suspicions do not force source correction. Positive competing glossary evidence retained, reference wording never promoted.

Root scoped intake first failed read-only because it looked for eng_order outside segments[0]; actual failure and helper preserved, corrected intake passed. Fresh reviewer preserved its shared-protocol provenance setup failure and rebound only the exact historical4770-byte preappend protocol. Initial registrationv3 failed before registry/allowance/index writes because it required review-freeze.json while the frozen packet uses freeze.json. Root-reviewed v4 SHA02bb095d09c042f4d4ecededab0e20a8f6bdc5b37d7f7072052c4e91137c9585 adds explicit filename+acceptedSHA binding and strict known-schema/header/baseline/count checks; existing guarded three-file writes/byte-retention checks unchanged. Actual root-register-attempt02-caller and independent register-copy check passed. Source/semantic metadata were not rewritten to fit a helper.

Canonical builders once14.216191seconds; all17 fidelity CTest suites once11.41seconds. Retention/source/course checks passed. Totals46,326links/11,970nulls/1,642notes/11trees/7,134heads/15,714pairs. ACIP7,131/7,134heads. CSV22,830main/22,490reverse/24,437course/21changes. All46,248priorlinks/22,809priorCSVrows/24,414priorcoursegroups, notes/trees/evidence/ACIP/content/references retained. Two existing depth7 rab rib cataract/cataracts rows gain exact RAB RIB from newly bankedC05:306 references; their English remains unchanged. Navigation targets exist, rendering remains unverified.

Independent compact reproduction at /Volumes/Oct2024(8TB)/Codex-ACI-Alignment-20260915-01a09398/compact-reproduction-through306 passed all seven complete outputs byte-for-byte.556registeredpages;2026-09-14producerdate;16,440,076-byte incoming CSVseed910437c4e858d2718f9b78e9389c5a5b9d7348e3defce087559084ef24ee8552. Final SHA256: full0af03e70e8775d209df614f41eacc6257722845f147b380e61940fb309c2447a; evidencee82a6ccb6bb2693d7019a0153a71305f66e09bb6b1e5f386ce499f926eb09af8; HTMLdf6b77aa734f5b78331beed7e2e0bf5d1a6cbcac744b51066697b8b2fb7cd78d; main95639f2be3cdae8f0d30c008895e160e8d74aacb8b1b803ff4b0332382e5bac4; reverse86d8f393ef2180cae8e9d621d92f7bba7e34c37a9973e7dd3539f3cba46e4e7f; coursee487d679cd9f8abb1f5fbd09b28c30d81e5b2b5e5de5a7648fb6ceb77e60ede7; changes60fca78a45ba062c9a73211b9128b232e1938609b91ce15f19df196cab859431.

Unchanged whitespacev8 PASS first attempt:42,088warnings8files, all exact source/physical/CSV whitespace accounted for; complete22,629,365-byte diagnostic733d5651e9d1f66b8c0ffb4381397d7c35122667f036ba38884efded19aefdef. Argument-only pin-preparer v2 reads the explicit freeze filename, with every old classification/byte check unchanged. No warning exemptions added for this batch. Banked native results, failure streams, source specs/review decisions/coverage, helper sources and bounded method reviews have exact archive manifests. Closure binds every staged blob/mode and accepted tree, both commit parents, three identical append-only ledgers and clean tracked state. Passing native work was not repeated for packaging.

Isolated desktop HTML/CSV mechanically verified through306. MAIN OPEN_NO_APPLICATION, no MAIN edits. Latest broad integration remains291; native applicability119production/docs passed against thenMAINbfc0162b,33401auditadditions collision-only, full33520 applicability untested. Next broad boundary309 must recheck then-current MAIN and preserve others' fixtures/captions. Render/native/QSettings remain open; inherited canonical view limits persist. Private phone pack FAILED_UNACCEPTED four inherited C01 fidelity errors; no installation, historical installedthrough171only. No paid API, spending, usage reset, account messages or release.307–309 has independent review approval but still requires root full intake, actual306 baseline and sequential production; no acceptance inferred here.


### 2026-09-14 — independently reviewed C05:307–309; private coverage 309/511

- Root accepted 129 provisional spans, four null exponents, 75 nonnull depth-5 pairs (76 total depth-5), and 30 depth-7 members. Per segment:307 48/2/25;308 57/2/35;309 24/0/15 (spans/nulls/nonnull d5). Three source anchors bring the actual new layer links to132. C05 is verified through309, with202 remaining310–511; C06 has not started and C13 remains blocked upstream.
- Source/governance origin remains b5047596859f6537b71f259c1e7df2e70f867fb2 through300. Actual acceptance baseline is 51d73c889a3f175b90e4c466f3dc4b1f7af3bca5 through306, captured after the fresh review finished. Root separately bound unchanged source/master/corpus/physical/spine bytes and canonical generator to that actual baseline. No retrospective claim that the reviewer consumed a later baseline. GMR wording is unchanged, machine data stays provisional, and nothing enters hgm_gloss.
- Independent originals: Tibetan110-file freeze63204cb888eb18983948cdd7c208c557cc33481c38433f7a523d412b906d3c60; English84-file freezeb16de542d7780bfa0334123c8132b3826cdef3815f32ac0218618d8213ce27f8. English original81 total d5 is80 nonnull. Filename-only discovery of other-angle paths in the Tibetan original remains disclosed; no other-angle contents were reported read. Reconciliation641-file freeze0ebc1b362a42c3382549cc14ce7c5bdcb5a63794fbc9c42953ae6cb7d608e2d0. All237 original dispositions,17 material omissions,32 original omission dispositions and nine full five-ground errata screens are retained.
- Fresh SPEC/QUALITY APPROVE, zero open findings:1249-file review freeze8d4ada44323d8df7100624fdc1975faee95af0f24ee9a40df0f049135ff1b68f; review.json6acc6da3e6bcb05744563d25165cbfe1761f70fca4ae79b678ae9379210f5700; review.md2305cdb66028f9f5daf9a81e9137766d2fe6fb9d59fb926ed3ea73b05ec3a24f. All1183 original-to-copy mappings verified before and after landing. Root read the full semantic decisions/source texts/original-to-final reasons, all omissions and all five-ground judgments, with full critical master and context records separately reopened. Root then verified206 complete master objects,89 complete corpus objects, both original freezes, current641/1249-file boundaries and312/608-file prior boundaries, plus313 reconciliation-history mappings.
- F1 corrected exactly three308 rationale strings: final w27 and original Tibetan w27/English w24 now identify burning-steel snang/appears W[200,205)/E[349,356), preserving the original snang ba→snang tightening. No spec/body/range/count changed. The full old313-file reconciliation boundary and609-file semantic NEEDS_CORRECTION boundary, including freezes, remain exact. Existing native results were reused for this rationale-only correction. Root independently resolved all366 original/final spans and checked six original plus three fresh final native stdin/stdout/empty stderr/exit0. Root made no redundant individual generator invocation. Actual aggregate landing generation and wrapper both passed.
- Four nulls are supported:307 gnyis is fused into the two-school enumeration (not later both of object/perception),307 zhig has no separate indefinite exponent,308 gyis and yis are fused into active subjects. The309 mental-intention idiom remains a whole phrase; no fabricated independent g-yo equivalent. All uncertain/distributed content remains unbanked with reasons. No new errata; register194 unchanged. Three exact sogs closing-gesture allowances: c5p103/s308w23, /s309w8 and /s309w11. Registration v4 passed first invocation with the actual review-freeze hash; full original registration semantics and source guards retained.
- Canonical builders ran once in14.010722s, with expected view diagnostics retained. All17 relevant fidelity CTest suites passed once in11.41s. Retention and course CSV checks passed:46,326 prior links,22,830 previous main CSV rows,24,437 course-depth groups and all prior notes/trees/evidence/ACIP/content/references retained. Current totals:46,458 links;11,974 nulls;1,645 note blocks;11 tree blocks;7,137 evidence heads;15,734 pairs;7,134 heads in ACIP index. Three heads still lack ACIP proof. CSV rows:22,865 main;22,525 reverse;24,479 by-course;35 changes. No prior CSV ACIP enrichment in this batch. The view's11,975 skipped links is its own diagnostic, distinct from the full-layer11,974 null count.
- Independent minimal-input reproduction passed for all seven complete outputs,557 registered pages, source date2026-09-14. External directory: /Volumes/Oct2024(8TB)/Codex-ACI-Alignment-20260915-01a09398/compact-reproduction-through309. Exact incoming CSV seed16,455,238bytes SHA95639f2be3cdae8f0d30c008895e160e8d74aacb8b1b803ff4b0332382e5bac4. Reproduced hashes: full25aced729a7b82a09d1cdc58bf5cdd49aa81ab427381e6630e61557f9de2f4a5; evidencec8ddc260fb750494a21613dbb2ff37e5eba996aa36f05a718b5dbfac98d68a42; HTML196c7ce89f9611c0b15d349711a531f84e7d24cfcd877549eab7dee3c83d8c9a; main89ecafd1461e2b08ae5e4efc48174a97af5397e0061320da892a1eb349212558; reverse32942d6aadbfa984e164b09b7b391aefd078f2cf5d30489c28dd31ef8651c136; course01c16b82be06813ef563afad25f0b7c7c01b0c823c3da81629ec3015720e2734; changesaaecbb847385ec2867cb8733f7ddeef86ffb12c443f531d7370f8986cec024c8. No successful production generator/test/reproduction rerun for packaging.
- First v8 whitespace audit refused four exact frozen copies of two old Python helpers, each with one outside-token trailing space at line53. Original diagnostics/pins are preserved. Root reviewed the narrow v10 addition: full-file SHA/length, exact line, original/frozen identity, token-stream equality after comparison-only removal and successful compilation of both forms. Source/review bytes were not edited. Actual v10 audit attempt02 passed all33,388 warnings; complete20,544,482-byte diagnostic SHA2e26f2531b7565f625fe8e80b7732f7467d04dc3ec87b4279f7410e0ead2e111. All v8 code is recovered exactly by removing this bounded addition. The known two helper hashes are the only added class, not a general waiver. Earlier native archive remains intact; late archive banks full failed/successful diagnostics and current method/review/history proofs.
- Fixed private DATA 34225a5914ac2c8c94b4a1045c659cc4b6044606 follows 51d73c889a3f175b90e4c466f3dc4b1f7af3bca5; closure checks the exact staged tree, all current frozen files and seven generated bytes, then separately appends this identical entry to all three ledgers and commits DOCS. Exact completion time 2026-09-14T17:53:29.664927+00:00. Root closure proof and actual Git streams identify final tree/parents and clean tracked state; the preexisting caption alias remains the only untracked path.
- Integration: isolated desktop-consumable outputs through309 are mechanically verified; native/render/QSettings acceptance remains open. MAIN has not been changed. Due outer309 assessment is in progress independently; previous291 assessment proved119 production/document paths applicable and33,401 audit additions collision-free, but did not test the full audit patch. No later comprehensive applicability is implied here. Private phone pack remains FAILED_UNACCEPTED for four inherited C01 correspondences; no installation, historical installedthrough171 unchanged. Inherited42 published-row depth shortfalls/28 trim calls remain. Preserve all ongoing MAIN work, fixtures/captions and the unrelated snapshot store. External evidence drive remains required.
- Next: both310–312 originals are frozen; separate reconciliation is active, fresh semantic review/root acceptance outstanding. Work continues in three-segment batches, C05 before C06, without new paid generation or external account actions. Existing one-minute automation remains subject to2026-09-15 07:00 America/Denver deadline; no duplicate automation.


### 2026-09-14 — independently reviewed C05:310–312; private coverage 312/511

- Accepted 110 provisional spans, one null, 73 nonnull depth-5 pairs (74 total d5), 16 depth-7 members and three source anchors:113 new layer links. Per310:40/1/25;311:13/0/7;312:57/0/41 (spans/nulls/nonnull d5).199 C05 segments remain313–511. C06 waits for complete C05; C13 remains blocked upstream.
- Historical source origin51d73c889a3f175b90e4c466f3dc4b1f7af3bca5 through306; actual acceptance baseline 8575edf1e0de695677194c1fde60f1d9e2c2f204 through309. Originals88/111 files, reconciliation359 files, fresh semantic442 files. Fresh SPEC and QUALITY APPROVE, no open findings, freeze5d255ecbb1c8acd37afbea717859d3f9851fd1aac5520583c6f66eecf589e6d9. All599 original-copy records verified before and after landing. All195 original dispositions,110 final source-specific decisions,20 material omissions and six complete five-ground screens are banked. No new errata or head allowances;194 register entries unchanged.
- Root independently checked all305 original/final canonical source resolutions,41 corpus queries,190 complete master objects,289 complete corpus objects and six physical extents. Root read all source/spec/report/decision interfaces plus31 full critical master records and complete critical corpus witnesses; broad metadata identity checks are not claimed as separate linguistic reading. Fresh reviewer generated three final bodies once, exact with empty stderr. Six original replay proofs and root's earlier checks reused only for unchanged inputs. Entire extent supersedes the earlier extent draft with both successful versions preserved.
- Sole null310 lus has no separate body/bodily English exponent. Distributed negative force, shared311 med par, first312 validity recast, mi bzod/imperceptibility and the full near-god cupful relation remain unbanked with reasons. Keep lexical consequence thal, occurrence-specific necessity, three visual faculties and original HGM-supported ocean depths/entire extent. GMR/source bytes remain verbatim; no machine correspondence enters hgm_gloss.
- Canonical production builders ran once in14.174221s. All17 fidelity tests passed in11.40s. All prior46,458 links,22,865 CSV rows, notes, trees, evidence, ACIP and references retained. Current measured totals46,571 links/11,975 nulls/1,648 notes/11 trees/7,140 heads/15,761 pairs;7,137 heads carry ACIP proof. CSV22,902 main/22,562 reverse/24,529 course/37 changes. Inherited42 display depth shortfalls and28 trimming calls remain; view diagnostic11,976 skipped links differs from full-layer null count.
- All seven complete outputs independently reproduced exactly from558 registered pages and the actual incoming CSV seed. Passing proof: /Volumes/Oct2024(8TB)/Codex-ACI-Alignment-20260915-01a09398/compact-reproduction-through312-attempt02/proof.json. Actual failed first preparation remains at compact-reproduction-through312: missing retention proof due root dependency ordering, zero native builder runs. No passing production/test/reproduction was rerun for packaging.
- Actual root master-wrapper schema failure and registration v4 format rejection remain banked. Successful source preflight attempt02 normalized only record/entry wrappers; registration v5 supports the actual v1 review schema and dictionary manifest while keeping exact baseline/source/freeze gates. Whitespace preparation v3 and audit v11 only normalize manifest representation; existing warning classes and byte checks are unchanged. Every staged whitespace diagnostic is checked and retained in late evidence; frozen source/review bytes are not normalized.
- Fixed private DATA 3f2f32f1cc1cb23c6b9af1d8828c2e19b36e0f04 follows 8575edf1e0de695677194c1fde60f1d9e2c2f204. Exact staged-tree and source/review/output bindings precede that commit; this same append-only entry is separately committed to all three ledgers. Completion 2026-09-14T18:52:36.197496+00:00. Closure proof retains actual Git streams, parents/tree, clean tracked state and preexisting untracked caption alias.
- Desktop-consumable outputs through312 are verified in isolation. The complete through309 MAIN applicability assessment is now root accepted:314 actual native whole-file partitions,38,782 paths,3502 frozen evidence files. Its prior audit-patch gap is closed. This batch references the preserved1.235GB compressed patch proof once; it does not claim a new312 MAIN applicability assessment. MAIN is not applied; native rendering, QSettings and merged runtime remain open. Phone pack remains FAILED_UNACCEPTED for four inherited C01 mappings; no install, historical installedthrough171 unchanged. External evidence drive required; preserve all other work.
- Next313–315 has both originals and730-file reconciliation frozen; fresh independent semantic review active. Both316–318 independent original proposals run separately. Original source309 is distinct from future acceptance312/315. Existing one-minute automation remains active through2026-09-15 07:00 America/Denver, then pause. No paid generation, releases or external account actions.


### 2026-09-14 — independently reviewed C05:313–315; private coverage315/511

- Accepted95 provisional spans,1 null,67 nonnull depth-5 pairs (68 total d5),11 depth-7 members and3 source anchors:98 new links. Per313:32/0/24;314:54/0/39;315:9/1/4 (spans/nulls/nonnull d5).196 C05 segments remain316–511; C06 waits for full C05, C13 stays blocked upstream.
- Source/proposal/reconciliation origin8575edf1e0de695677194c1fde60f1d9e2c2f204 through309 is distinct from actual acceptance 49b66f3af9cbb47599bb40286637fc9e683b7cdf through312. Originals139/127 files; reconciliation730; fresh semantic408. Fresh SPEC/QUALITY APPROVE, zero open findings, freeze7a237aa7df978736cda8aa8851d9e6af98720d4d5a5fd7fcce2151febe1b1bf6. All1045 original-copy records checked before/after landing. All183 original dispositions,95 final decisions,17 material omissions and15 five-ground screens retained. Two exact314 la sogs pa head licences registered; no new erratum,194 register entries unchanged.
- Root independently resolved278 original/final spans, reran31 SQL queries, checked162 complete master and99 complete corpus objects and6 physical fields. Complete source-specific decisions were read, with37 full critical master objects and full C05:28/146 witnesses separately read. English external evidence has19 field queries plus3 complete-tuple comparisons; the historical shorthand22 queries is not22 homogeneous SQL executions. Original Tibetan five-ground file is tibetan/evidence/five-ground-errata-review.json. Broad identity checks do not claim fresh linguistic scrutiny of every metadata string.
- Preserve complete313 necessity/negation and tshad grub pa with members, one causal because,314 enumeration/shape/steel compounds and own occurrences,315 whole author epithet and work title. Sole315 rtsa ba null records absent separate root-text wording; mdzod owns Abhidharmakosha. Isolated yod pa/occupy, extra mountain repetition and distributed authorship/relations stay unbanked. Original apparatus remains verbatim; nothing enters hgm_gloss.
- Fresh reviewer generated each final once, exact with empty stderr; six original and three reconciled generator proofs reused after byte checks. Aggregate production builders passed once in14.399356s. All17 fidelity tests passed12.13s. All46571 prior links and22902 prior main-CSV rows, notes, trees, evidence, ACIP and references retained. Current46669 links,11976 nulls,7144 heads,15778 evidence pairs,22928 main-CSV rows and24558 course-depth groups. Seven full outputs reproduce exactly once from559 registered pages and the actual16509904-byte incoming CSV seed SHA8d696e062ac18a46b8cd458cc3f69e9b246aa163299e38dcf5d2e82917fb9a01. External proof: compact-reproduction-through315/proof.json under the campaign evidence drive.
- Five actual root intake failures arose from representation assumptions: optional source-copy path, null metadata beyond canonical ranges, query/comparison mix, record references versus embedded objects, and a register wrapper. Failed native streams and each method version are preserved; attempt06 passed. No source, spec or individual generator changed. Registration v6 explicitly binds the actual reconciliation freeze filename; whitespace preparation v3/audit v11 first failed on an unpinned blank EOF and a reversible Latin1-to-UTF8 source view. The EOF uses the existing exact class. Reviewed v12 adds a tightly pinned bidirectional encoding proof and checks every cited CRLF line; final attempt03 passes without changing source/view bytes. Both failed audit streams and initial v12 preparation assertion failure are retained. No blanket warning waiver or source normalization.
- Fixed private DATA dab7b5d330358c7d7c303e335f1b021675f30557 follows 49b66f3af9cbb47599bb40286637fc9e683b7cdf. Exact staged-tree/source/review/output bindings precede commit; this identical append-only entry is committed to all three ledgers. Completion 2026-09-14T19:34:03.433578+00:00. Actual Git streams, parent/tree and clean tracked state remain in compact-closure/proof.json.
- Isolated desktop-consumable outputs are verified through315. Existing complete through309 MAIN applicability assessment remains referenced once:314 native whole-file partitions/38782 paths/3502 evidence files. No new315 MAIN applicability or runtime claim; MAIN remains unapplied. Phone remains FAILED_UNACCEPTED for four inherited C01 mappings; no install, historical through171 unchanged. Rendering/QSettings/runtime,42 depth shortfalls and28 trim calls remain open. Preserve other work and external evidence.
- Workflow now overlaps root acceptance,316–318 fresh review and319–321 independent originals. Removed unused preliminary lexical scan; standard freeze filename/dictionary formats and indexed full evidence are requested for new work. Acceptance stays sequential in three-segment batches. Existing one-minute automation remains active through2026-09-15 07:00 America/Denver, then pause. No paid generation, release or external-account action.


### 2026-09-14 — independently reviewed C05:316–318; private coverage318/511

- Accepted79 provisional spans,3 explicit topic nulls,54 nonnull depth-5 pairs and9 depth-7 members, plus3 source anchors: 82 new links. Per316:22/1/15;317:26/1/19;318:31/1/20 (spans/nulls/nonnull d5).193 C05 segments remain319–511; C06 waits for complete C05 and C13 stays blocked upstream.
- Source/proposal/reconciliation origin 8575edf1e0de695677194c1fde60f1d9e2c2f204 through309 remains distinct from actual acceptance af139c573f85aaf33278cd079719947ee9af8cdd through315. Originals 98/119 files; reconciliation356 SHA51aa8fc419bbd25ea20a21d0ea4524b1c964888891c7aa0c400b449c3f6ba314; fresh semantic494 SHA81e6c82c71c035e51661aeed9056b502fecf5b2de8a5bdd01bc9c887a5098d21. Fresh SPEC/QUALITY APPROVE with zero open findings. All608 original-copy records remain byte-bound before/after landing;150 original dispositions,79 final decisions and21 material omissions remain readable. No erratum or supplied-head allowance is added; existing register and allowances retain their bytes.
- Root independently resolved229 original/final spans, reexecuted50 SQL queries and checked105 complete master queries/102 unique master objects,209 complete corpus objects and6 physical extents. Root's separate reading covers35 full critical master records and the complete source-specific decisions. Broad identity checks do not claim fresh linguistic reading of every repeated metadata string.
- 316 retains distinct virtue/Non-virtue, realm coordination, capital taken In/Respectively, whole compounds and independently supported de/it, zad/end, byed pa/brings. 317 preserves three distinct ripening roles and four black occurrences; Just is unresolved because competing HGM entries cannot establish ownership. 318 preserves distinct stream/with occurrences, whole negative-mixture compounds and members; first gcig/in question and su/by remain unresolved. BA/PA remains uncertain after the complete negative HGM ma dres ba headword with actual apostrophe supplies material counterevidence. No source correction is filed. Exactly three ni topic markers: no separate English word, topic-predicate syntax carries their contribution; explicit copulas retain their actual predicates. Uncertain, redistributed and omitted phrases do not become nulls. 316 whitespace-start GNAG regex count0 is preserved as that literal method. Punctuation-aware lexical count is1 at Wylie[66,70);317 has4 under both methods. This is audit-label correction only; no frozen source/spec/native output changed. Original apparatus stays verbatim; nothing enters hgm_gloss.
- Three fresh final generations and six original native generations are reused after exact code/input/stream/exit checks; root repeated no generator. Two actual root intake adapter failures and their helper versions remain preserved; attempt03 passed. Frozen reviewer intake failures and the Tibetan318 note-only supersession remain preserved independently. Registrationv6 uses the actual reconciliation freeze.json. Unchanged independently approved whitespace preparationv4/auditv12 classify only exact frozen evidence; the actual staged audit remains a required closure gate.
- Aggregate production passed once in15.293561s; all17 fidelity tests passed in11.58s. All46669 prior links and22928 prior main-CSV rows, notes, trees, evidence, ACIP and references are retained. Current46751 links,11979 nulls,7151 heads,15798 evidence pairs,22957 main-CSV rows and24590 course-depth groups. Seven full outputs reproduce exactly once from560 registered pages and the actual16531659-byte incoming CSV seed SHA37d36de4b6e8de68dd8820afab0f89ed496f94a0c33ce643b2b2ae05bd581f05. External proof: /Volumes/Oct2024(8TB)/Codex-ACI-Alignment-20260915-01a09398/compact-reproduction-through318/proof.json.
- Fixed private DATA 8471770720df965e27f1a5377ddde304f3ee9c47 follows af139c573f85aaf33278cd079719947ee9af8cdd. Exact staged mode/blob/tree, source/review/output bindings precede commit; all three ledgers receive this identical append-only entry. Completion 2026-09-14T20:06:33.040039+00:00. Actual Git streams, parent/tree, source fidelity and clean tracked/untracked preservation remain in compact-closure/proof.json. Current DATA/DOCS close through318 with193 remaining; prepared319–321 material is not accepted by this checkpoint.
- Storage pressure required relocation of three completed historical proof files (2006986110 bytes) to the external evidence drive. The original paths remain readable symlinks to the byte-and-mode-verified files. An earlier directory attempt failed on an existing symlink before copying or relocating anything; both helpers, both actual caller streams, the empty failed proof-directory inventory and the passing small proof are preserved in root-current-method. Current source, master, worktree and active-batch evidence were unchanged; the external drive remains required. Large historic proof files are not recopied for this checkpoint.
- Isolated desktop-consumable outputs are verified through318. Existing through309 MAIN applicability evidence is referenced once by landing/root-current-method/integration-reference.json; no new318 applicability or runtime claim. MAIN remains unapplied. Phone remains FAILED_UNACCEPTED for four inherited C01 mappings; no installation, historical installed through171 unchanged. Rendering/QSettings/runtime,42 depth shortfalls and28 trim calls remain open. Other work and external evidence are preserved; no paid generation, release or external-account action.

- Late native-log packaging addendum: the first staged whitespace audit failed on the semantic freeze caller stdout, which completed after and is explicitly excluded from the494-file freeze. Its six complete caller files remain byte-identical in verification/post-freeze-caller-evidence.tar.gz with an exact manifest; only their unfrozen duplicate readable bank directory was removed. Original semantic-review/freeze-caller files remain unchanged and readable. The failed audit/native diagnostics and archive helper/proof/callers are preserved. The unchanged v12 audit then passed on attempt02. All494 frozen semantic files,608 original-copy identities, source/spec/review/generator bytes and seven reproduced outputs remain unchanged. This changes evidence placement only; it adds no whitespace exception or semantic approval.
