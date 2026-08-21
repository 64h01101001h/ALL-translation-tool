# Honorific (zhe sa) register (Adam's request, 2026-08-08)

A register of honorific terms and — once marked — "high honorific"
terms/phrases. Started as a CURATED SEED of the classical zhe-sa
pairs; every honorific wylie is verified to be an HGM dictionary
entry at generation time (phyag mdzod was dropped this way).

## honorific_register.tsv

Columns: `honorific_wylie · ordinary_wylie · domain · level`.
- `ordinary_wylie` is filled only when the plain counterpart is ALSO a
  dictionary entry (else left empty rather than guessed).
- `level` is `honorific` for the seed rows; `high` is available for
  Adam/HGM to mark (only zhal snga nas seeded as high). NOTE: the HGM
  glosses themselves mark honorifics in only ONE entry (dgongs pa,
  "thought (honorific)") — so levels cannot be mined from glosses;
  they need human marking. 27 rows seeded.

## Planned app use

- Overlay/Lookup cards: an "honorific (register)" badge on registered
  terms, with the ordinary counterpart shown as cross-reference.
- Translator oversight: register-consistency warnings (honorific verb
  with non-honorific subject etc.) are a possible later layer — only
  with a reviewed, larger register.
