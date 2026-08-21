// allcore/unicode_wylie.h — Tibetan unicode → EWTS wylie, the reverse
// of the canonical wylieToUnicode.
//
// Ported FROM THE FILE (rule 2): pyewts.py::toWylie (OpenPecha/pyewts
// master; Apache-2.0, Roger Espel Llima / BDRC) — the same library the
// forward engine was cross-validated against. The lookup tables are
// AUTO-GENERATED from the imported module's own runtime maps
// (tools/gen_unicode_wylie_tables.py → unicode_wylie_tables.inc).
// Battery: 100% match against pyewts' own output on 109,490 fixtures
// (the master's 26k+ ground-truth Tibetan strings + strided native
// 84000 unicode) — build/towylie_reference.tsv.
//
// Untranslatable characters are escaped exactly as pyewts escapes
// them ([\uXXXX] forms), never guessed; `warns` counts the pipeline's
// warnings so a caller can say "this reverse conversion is doubtful".
#pragma once

#include <string>

namespace allcore {

struct UnicodeToWylieResult {
    std::string wylie;
    int warns = 0;
};

UnicodeToWylieResult unicodeToWylie(const std::string& unicode_utf8);

}  // namespace allcore
