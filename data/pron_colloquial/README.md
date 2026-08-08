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

Derivation: every entry whose wylie matches `b(s) + ' + d/g/b/j` and
whose stored pron still has the b/p cluster gets a derived m-form
(kabdir → kamdir), with a juncture-count guard — entries where wylie
junctures and pron cluster sites disagree are SKIPPED (2 rows), never
guessed. The 'b-cluster junctures (-b(s) before 'bum/'bul-type
syllables, bb/pb → mb) were derived in a second guarded pass
(+42 rows, 0 ambiguous, 2026-08-08). Tier:
DERIVED per HGM's stated rule — pending HGM/Adam review; the engine
itself stays untouched (its battery is the GMR-convention ground
truth).

## How the app uses it

Lookup pane fallback chain: exact headword → affix-stripped →
GMR pronunciation index → **this register** (label: "matched by
colloquial pronunciation — community usage register"). The register
never overrides the GMR convention anywhere; it only widens what the
search accepts.
