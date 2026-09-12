# APPROVE — C05:190–192 / c5p64

Codex independent semantic review. All 44 proposed spans accepted, including two justified nulls; 30 d5 word pairs. This approves PROVISIONAL machine alignment for landing, not human acceptance or promotion into binding hgm_gloss. No unresolved semantic correction is required.

Original source and neighboring context were independently read from the worktree spine using SQLite URI mode=ro. All eight required reference paths are readable. The actual unchanged generator was rerun separately for each final spec under PYTHONDONTWRITEBYTECODE=1 with read-only SQLite. Exits are 0/0/0, stderr empty, and every complete stdout equals its saved body byte-for-byte (1707, 2322, 4860 bytes). Exact hashes and spec byte sizes are in review.json. Real resolve and with_members were called independently with actual eng_order; resolved-spans.json records all 44 exact tuples in spec order, and occurrences.json records repeated-string alternatives. Passing fidelity is not the semantic proof: the per-span judgments below are separate.

Material decisions: chen mo/Great Book survives because HGM 22779 expressly attests great book within the established title, not because English surface shape suggests it. la/on preserves the domain relation across the presentation recast. gyi is a documented attributive fusion null; ni is an unexpressed topic null. gsum gsum/three kinds is a defensible complete nominal classification recast at d3, never a dictionary equation for kinds. The shared birth predicate is represented once at final born [416,420), not default-first born [258,262); the chosen placement agrees with final Tibetan animal branch. sogs licenses all of and the rest and needs exactly pages_c05 → c5p64/s192w10; no gate was edited.

Omissions were challenged as well: 190 rjes remains underdetermined, chen pos has distributed honorific and agency content, and pa'i has no useful separate atom. Selection framing, dates and supplied title article stay outside. In 191 de dag is expanded anaphorically; las participates in the enumerative beginning-with recast. These are omitted, not misrepresented as absence nulls. In 192 yang, gzhi, enumerative gsum, de la and first/final re res contribute to larger restructurings without a clean isolated lexical equivalent; mo and final la are closure/continuation. The existing notes adequately document them. Supplied instances, participants, articles and auxiliaries are excluded. No additional member can be added merely to increase coverage.

RJES erratum: both candidates are independently refuted under all five checks, fully recorded in errata-refutations.json. The spelling hypothesis remains possible, but rje+s agency is not excluded by lexical headword senses or by chen pos also bearing case. SOS:284 is a different construction supporting the formal possibility, not proof of this heading's grammar. Actual ASCII corroborates RJES; that same-lineage source is not an independent publication. The relevant printed reading edition was not inspected; C5Tibetan.pdf is merely the language guide. Counts and complete corpus rows were independently queried and saved. C16:768 is not a second exact Tibetan copy. C09/C16 copies of 191–192 and C17:497's heading-appended English do not establish publication independence. No erratum survives; bank-ready-errata.json is an empty list.

## Every span

| Segment / span | Depth | Tibetan → English | T / E range | Independent disposition |
|---|---:|---|---|---|
| 190/w1 | 5 | `tsong kha pa` → Tsongkapa | [5, 17] / [90, 99] | Tsongkapa is the name; dates and Lord excluded. Whole name at d5 supported by HGM 14956. |
| 190/w2 | 5 | `brtsoms` → composed | [27, 34] / [73, 81] | Composed is the composition verb in this exact colophon. brtsoms lacks an HGM gloss but fourteen corpus attestations support composition usage; no substitute spelling or pa'i member invented. |
| 190/w3 | 5 | `lam rim chen mo` → Great Book on the Steps of the Path | [40, 55] / [36, 71] | Established title, HGM 25380, retained whole at d5. Internal on the/of the are title grammar; leading the excluded. |
| 190/m1 | 7 | `lam rim` → Steps of the Path | [40, 47] / [54, 71] | Unique tighter title member: stages/steps of the path, HGM 19325. Capitalization is verbatim. No separate rim dictionary pair forced. |
| 190/m2 | 7 | `chen mo` → Great Book | [48, 55] / [36, 46] | HGM 22779 explicitly gives great book. This independently defeats the supplied-Book objection here; chen mo is the unique tighter title member, not a universal chen=book equation. |
| 190/p1 | 6 | `las` → from | [56, 59] / [27, 31] | Citation ablative las licenses from. Selection framing and is are excluded. |
| 191/w1 | 5 | `gsum pa` → third | [0, 7] / [12, 17] | Ordinal gsum pa licenses third, not the supplied section. |
| 191/p1 | 6 | `gi` → of | [15, 17] / [62, 64] | Genitive gi links deeds to consequences; first whole-word of after consequences is correct. Later of bad deeds belongs inside the explanatory anaphor. |
| 191/w2 | 5 | `'bras bu` → consequences | [18, 26] / [49, 61] | First result noun corresponds first consequences. |
| 191/p2 | 6 | `la` → on | [27, 29] / [42, 44] | Locative/domain la of consequences is expressed by on in presentation on the consequences. Reordered domain marking remains a valid particle correspondence; presentation itself is not captured. |
| 191/w3 | 5 | `gsum` → three | [30, 34] / [111, 116] | Cardinal gsum licenses three; explanatory parts excluded. |
| 191/w4 | 5 | `rnam smin` → ripened | [40, 49] / [160, 167] | rnam smin licenses attributive ripened (HGM 10884); quotes remain outside. |
| 191/p3 | 6 | `gyi` → ∅ | [50, 53] / None | Accept null: gyi genitive relation is fused into the English attributive construction ripened consequences, with no separately alignable word. Explicit note preserves contribution. |
| 191/w5 | 5 | `'bras bu` → consequences | [54, 62] / [169, 181] | Second result noun corresponds second consequences; not the first occurrence. |
| 191/p4 | 6 | `ni` → ∅ | [63, 65] / None | Accept null: topic ni has no separate exponent in this enumerative recast. What are called is explanatory naming, not evidence that ni means called. Its general topic function is not denied. |
| 192/w1 | 5 | `las lam` → paths of karma | [0, 7] / [24, 38] | Established las lam compound (HGM 19403) licenses paths of karma, with internal relation of. |
| 192/m1 | 7 | `las` → karma | [0, 3] / [33, 38] | Karma is the unique las member under that compound; local usage does not replace curated standalone deeds gloss. |
| 192/m2 | 7 | `lam` → paths | [4, 7] / [24, 29] | Paths is the unique lam member, reordered inside the English parent. |
| 192/w2 | 5 | `bcu po` → ten | [8, 14] / [20, 23] | First bcu po licenses opening ten; of the excluded. |
| 192/w3 | 5 | `re re` → each one | [15, 20] / [4, 12] | Repetition re re distributes individually and licenses each one (HGM 19101); not supplied participant. |
| 192/w4 | 5 | `dug gsum` → three poisons | [31, 39] / [142, 155] | Established dug gsum technical compound (HGM 23398), complete three poisons. |
| 192/m3 | 7 | `dug` → poisons | [31, 34] / [148, 155] | Unique poison member; plural poisons supported locally and in glossary. |
| 192/m4 | 7 | `gsum` → three | [35, 39] / [142, 147] | Unique gsum member resolves second English three inside three poisons, not three kinds. |
| 192/w5 | 5 | `chung` → lesser | [40, 45] / [79, 85] | First lesser in the introductory grading list. |
| 192/w6 | 5 | `'bring` → medium | [46, 52] / [87, 93] | First medium in introductory grading list. |
| 192/w7 | 5 | `che` → greater | [53, 56] / [99, 106] | First greater in introductory grading list. |
| 192/f1 | 3 | `la brten pas` → depending on | [62, 74] / [108, 120] | Complete dependence construction la brten pas corresponds depending on; d3 keeps relational construction out of dictionary layer. |
| 192/w8 | 3 | `gsum gsum` → three kinds | [75, 84] / [66, 77] | Accept d3 nominal classification recast three kinds. Distributive gsum gsum means three each in this context; kinds is the nominal realization of this classification, not an independent gsum gloss. Earlier re re supplies overt each one. Can itself be divided into is excluded; no invented member kinds. |
| 192/w9 | 5 | `srog gcod` → killing | [97, 106] / [229, 236] | Srog gcod is established killing compound (HGM 21036); no fabricated life member. |
| 192/w10 | 5 | `sogs` → and the rest | [107, 111] / [237, 249] | Sogs licenses the complete closing expression and the rest, explicitly HGM 20919. Rest is the anaphoric remainder, not a supplied topical noun. Requires the named head allowance. |
| 192/w11 | 5 | `chen po` → greater | [112, 119] / [208, 215] | Second greater describes the serious/greater instances; instances excluded. |
| 192/w12 | 5 | `bcu po` → ten | [120, 126] / [204, 207] | Second ten belongs greater instances clause. |
| 192/w13 | 5 | `dmyal ba` → hells | [134, 142] / [272, 277] | Dmyal ba licenses hells; into the and preceding birth predicate excluded. |
| 192/w14 | 5 | `'bring` → medium | [144, 150] / [297, 303] | Second medium belongs medium consequences clause. |
| 192/w15 | 5 | `bcu po` → ten | [151, 157] / [329, 332] | Third ten belongs medium clause, after English each. |
| 192/w16 | 5 | `re res` → each | [158, 164] / [317, 321] | Middle re res selected after middle ten in Tibetan; English each is the explicit distributive for this branch. Bound agency remains within inflected form, no false from member. |
| 192/w17 | 5 | `yi dvags` → insatiable spirit | [165, 173] / [354, 371] | Established insatiable spirit (HGM 25069); as an excluded. |
| 192/p1 | 6 | `dang` → and | [174, 178] / [373, 376] | Dang coordinates final branch and resolves final and, not earlier grade-list or and the rest. |
| 192/w18 | 5 | `chung ngu` → lesser | [181, 190] / [390, 396] | Second lesser belongs final branch; full chung ngu retained. |
| 192/w19 | 5 | `bcu po` → ten | [191, 197] / [386, 389] | Fourth ten belongs final branch. |
| 192/w20 | 5 | `dud 'gror` → animal | [205, 214] / [427, 433] | Inflected dud 'gror is the animal lexeme with terminative; no suffix split or as an overcapture. |
| 192/w21 | 5 | `skye bar` → born | [215, 223] / [416, 420] | Single shared skye bar takes final born in the animal branch, matching terminal Tibetan location. Earlier born and take birth are repeated renderings of the same predicate. This documented representative is valid; auxiliary are and you excluded. |
| 192/w22 | 5 | `sa'i dngos gzhir` → Main Stage of Levels | [224, 240] / [161, 181] | Whole inflected citation title; Main Stage of Levels is actual source English, not substituted glossary actual level. The English leading The excluded; internal of licensed title structure. |
| 192/w23 | 5 | `gsungs` → states | [241, 247] / [182, 188] | Gsungs licenses states at first reporting occurrence. Later It says is repeated reporting framing, left unwrapped. |
