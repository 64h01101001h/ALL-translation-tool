# pyewts defect-candidate review — VERDICTS
*2026-08-13 · closes the TODO item "Review the 13 pyewts
source-defect candidates … source check each before correcting"*

Method: three-way comparison per candidate — the master's stored
`tibetan` vs pyewts's conversion vs **our canonical engine**
(Handoff `ewts_unicode.wylie_to_unicode`, the 98.88%-battery
oracle). Two independent converters agreeing against the stored
field is treated as decisive; no correction is applied here (the
master lives with the data project — this is the review they asked
for).

## Verdict: all 13 are stored-Tibetan defects
In every case `engine == pyewts ≠ stored`. Three defect classes:

**A. Final syllable(s) truncated in the stored Tibetan (10):**
| wylie | stored | correct |
|---|---|---|
| khongs sa | ཁོངས | ཁོངས་ས |
| rgyu chos rjes dpog | …དཔོ | …དཔོག |
| brgya lam na | བརྒྱ་ལམ | བརྒྱ་ལམ་ན |
| dngos po'i stobs shugs kyi rigs pa | …རིགས | …རིགས་པ |
| dngos su bstan pa | …བསྟན | …བསྟན་པ |
| gtad rabs bdun | གཏད་རབས | གཏད་རབས་བདུན |
| rnam rig pa'i lugs | རྣམ་རིག་པ | རྣམ་རིག་པའི་ལུགས |
| rmi lam gyi shes pa | རྨི་ལམ | རྨི་ལམ་གྱི་ཤེས་པ |
| shes rab pha rol phyin sbyor | …ཕྱིན | …ཕྱིན་སྦྱོར |
| bai du' rya | …ར (subjoined ya lost) | …རྱ |

**B. Wrong word's Tibetan attached (1) — the standout:**
- `bsgrub bya` stores བསྒྲུབ་བྱེད (bsgrub *byed*, the prover) while
  the headword is bsgrub *bya* (the thing to be proven). Opposite
  members of the pair — the worst of the 13 for a dictionary.

**C. Different text stored (2):**
- `sba bzhed` stores སྦ་བཞེད་རྒྱས་བསྡུས — the *expanded* edition's
  title where the headword is the bare sba bzhed.
- `bsgrald` stores བསྒྲལ, dropping the wylie's own da-drag; the
  faithful conversion is བསྒྲལད. If the da-drag is unintended, the
  fix belongs on the wylie side — but the wylie is the master
  field, so the Tibetan should follow it.

## Recommendation to the data project (Cowork)
Regenerate `tibetan` for these 13 entries from the canonical
engine (which the battery already proves), tagging
`tibetan_source: regenerated-defect-fix-2026-08` — with a human
glance at `bsgrub bya` (class B) and `sba bzhed`/`bsgrald`
(class C), where the right fix touches meaning, not merely
conversion. The spine's `tibetan_source` is NULL for all 13, so
provenance of the original bad values must come from the master
JSON's fields.

This repo applies nothing: the release is consumed, never forked.
