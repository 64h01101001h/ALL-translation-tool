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
c5p1–c5p7 (1–21) cloned 2026-09-05; c5p8–c5p49 (22–147) landed 2026-09-05; c5p50–c5p55 (148–165) landed 2026-09-08. c5p56 (166–168) landed 2026-09-12 after fresh independent proposals, reconciliation and skeptical review. c5p57 (169–171) landed after independent review. c5p58 (172–174) landed after independent review. c5p59 (175–177) landed after independent review. c5p60 (178–180) landed after independent review. Coverage 180/511; current verification is recorded in the latest batch entry below.

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
