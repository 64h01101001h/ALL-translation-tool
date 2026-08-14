# Lingua-BO-Wylie (Roger Espel Llima) — cross-oracle

Downloaded 2026-08-14 from
https://www.lotsawahouse.org/Static/Lingua-BO-Wylie-dev.zip
(linked from digitaltibetan.github.io's pronunciation tools page).

License: "free software; you can redistribute it and/or modify it
under the same terms as Perl itself" (Phonetics.pm / Wylie.pm
COPYRIGHT AND LICENSE sections) — the Perl Artistic/GPL dual
license. © 2008–2010 Roger Espel Llima.

Role here: ORACLE ONLY — never compiled into the app. Implements
Wylie⇄Unicode plus phonetics in five conventions (thl,
rigpa-en/es/fr/de, padmakara-pt/en, lhasey-en) via
bin/pronounce.pl. First use (docs/research/ESPEL_PHONETICS_ORACLE
.md): a 3,031-headword THL diff against allcore::thlPhonetics
that exposed and fixed our monosyllabic ba→wa over-application.
Rerun recipe:
  perl bin/pronounce.pl -sty thl <in.txt> <out.txt>
