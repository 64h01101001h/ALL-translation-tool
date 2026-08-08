# Colloquial pronunciation register (Adam's request, 2026-08-08)

"Common Tibetan pronunciations that are essentially legal because they
are colloquial" — the spellings the dharma community actually writes
(gonpa, tulku, chenrezig, chorten, lhasa, khenpo…), which the strict
GMR convention renders differently (gunpa, trulku, chenre sik, chuten,
hlasa, kenpo…). Both are right; this register lets the app accept the
community forms.

## colloquial_pron.tsv

Columns: `colloquial · wylie · gmr_pron`. Multiple community spellings
of one term are separate rows (thangka/tanka/thanka).

**Admission rule (rule 3 — attestation, never invention):**
1. the community spelling is standard published usage (the anglicized
   dharma vocabulary as it appears across ACI materials and general
   Buddhist publishing), AND
2. the wylie IS a dictionary entry — every row is verified against the
   spine at generation time; candidates that are not entries are
   skipped and reported (a ni, dbu mdzad were dropped this way).

**Tier: CURATED SEED (started 2026-08-08, 58 community rows).** This
is a living list — Adam extends it by adding rows; the smoke test
verifies every row still resolves to a dictionary entry.

## The prenasal class (class = prenasal-derived, 164 rows)

HGM's stated rule (via Adam, 2026-08-08): **SKABS 'DIR is pronounced
"kamdir"** — a syllable-final -b/-bs nasalizes to **m** before a
following 'a-chung-prefixed syllable ('d/'g/'b/'j). The canonical
engine applies prenasalization after VOWEL-final syllables (dge 'dun →
gendun, bka' 'gyur → kangyur) but not after -b(s) codas (it gives
kabdir). The master's own data attests the m-form: **skyabs 'gro is
stored as "kyamdro"** (87 entries already carry nasal forms at these
junctures).

Derivation (tools/derive_prenasal.py — supersedes the first two ad-hoc
passes, whose site-count guard could transform a cluster at the WRONG
juncture; corrected same day): each juncture is located by PREFIX
PRONUNCIATION through the canonical engine (allowing the b/p voicing
alternation at the prefix edge), only entries whose stored pron equals
the engine's are transformed, and the WORD-INTERNAL condition is
enforced — the master and engine both apply prenasal sandhi only
inside a phonological word, so junctures at pron word boundaries are
regular and skipped (44 such). Result: 205 aligned m-forms; 4 curated
prons skipped; 0 unstable alignments. Tier: DERIVED per HGM's stated
rule — pending HGM/Adam review; the engine itself stays untouched (its
battery is the GMR-convention ground truth).

## The four prenasal systems (mapped 2026-08-08, prompted by Adam's
## n-vs-m question)

What the engine and the master's own pronunciations show, taken
together, is FOUR distinct treatments of a following 'a-chung
syllable — and the n/m difference falls out of them:

1. **Vowel-final syllable** → a HOMORGANIC nasal is inserted, taking
   the FOLLOWING consonant's place: n before 'd/'j (dge 'dun →
   geNdun, mi 'jigs → miNjik), ng before 'g (bka' 'gyur → kaNGyur),
   m before 'b (dge 'bangs → geMbang). Engine handles this; master
   consistent.
2. **-d coda** → the d is REPLACED by a homorganic nasal, again the
   following consonant's place: med 'gag → meNGak, brgyud 'dzin →
   gyuNdzin. Engine handles this; master fully consistent (206:0).
3. **-g(s) coda** → the k sound is KEPT and an n is INSERTED after
   it: bdag 'dzin → dakNdzin, rig 'dzin → rikNdzin, khrag 'byin →
   trakNjin. The master's dominant pattern (216 junctures vs 26
   plain), with 4 mixed lemmas (bdag 'dzin 12:4, rig 'dzin 12:1,
   mchog 'dzin 7:1, dag 'dus 1:2) — the 26 plain stragglers are
   DEFECT CANDIDATES for the data project (this class, unlike the
   b-class, is usually nasalized in the master already). The 26
   derived k+n rows in the register cover them meanwhile.
4. **-b(s) coda** → the b is replaced by m — keeping its OWN labial
   place regardless of what follows: skabs 'dir → kaMdir (m before a
   DENTAL — homorganic would be kandir), skyabs 'gro → kyaMdro (m
   before a velar). This is HGM's stated rule, doubly attested in
   the master's own kyamdro; the engine does not yet apply it, hence
   the register's 206 derived m-forms.

So: **m marks a labial source (the b-coda's own place) or a labial
target (vowel before 'b); n marks dental/palatal assimilation or the
post-velar inserted nasal; ng marks velar assimilation.**

Consistency: the b(s)-class shows ZERO mixed bigrams in the master
(kyamdro vs kyabdroy differ because the following syllables differ);
the g(s)-class shows the 4 mixed lemmas above, resolved by the
master's own 216:26 majority toward the inserted n.

## How the app uses it

Lookup pane fallback chain: exact headword → affix-stripped →
GMR pronunciation index → **this register** (label: "matched by
colloquial pronunciation — community usage register"). The register
never overrides the GMR convention anywhere; it only widens what the
search accepts.
