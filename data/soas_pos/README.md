# SOAS Classical Tibetan POS lexicon (banked source)

`classical-lexicon.txt` — "A part-of-speech (POS) lexicon of Classical
Tibetan for NLP": 15,642 hand-tagged entries from the **Tibetan in Digital
Communication** research project (SOAS, University of London, 2012–2015,
funded by the AHRC), incorporating verb data from Nathan Hill's 2010
verb lexicon and manually tagged training-corpus data.

**License: Creative Commons Attribution 4.0 International (CC BY 4.0).**
Source: Zenodo record 574876, DOI 10.5281/zenodo.574876 (Lexicons.zip,
retrieved 2026-08-07). Attribution: the Tibetan in Digital Communication
project, SOAS University of London.

Format: `word<TAB>tag -<TAB>tag -...` — one row per orthographic form,
one or more tags (the trailing `-` per field is the source's own
placeholder column). The same word can appear with and without a final
tsheg carrying different tag rows; the loader normalizes on the
tsheg-stripped form and UNIONS the tags, so ambiguity is never hidden.
Tagset (SOAS scheme): n.count / n.mass / n.prop, adj, adv.*, v.past /
v.pres / v.fut / v.imp / v.invar, nominalized n.v.*, d.det, numeral,
case markers, punc, skt, and the project's own honest `dunno`.

In-app use: the Wilson Science-of-the-Dots layer consults this lexicon
ONLY where Wilson's rules say POS data is required (the NOM vs NA/NN/APP
decision between two bare nominals), and only on unambiguous tags; every
resolved dot names the lexicon as its evidence. Everything else stays
rule-table-driven. Reference data, never HGM-tier.
