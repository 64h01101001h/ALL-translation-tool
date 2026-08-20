// allcore/catalog_qc.h — the QC lanes over an intake set (9g step 2,
// the Lon-demo adoptables from DCC_CATALOG_AGENT_DEMO.md).
//
// Lane 1 — TITLE vs TRANSLATION MISMATCH: the copy-paste disease the
// demo found six of — a filename whose ENGLISH half belongs to a
// DIFFERENT text. Signature: this file's English matches another
// work's published English closely while the two Tibetan titles barely
// overlap. Every flag carries the other work as evidence; a flag is a
// question, not a verdict.
//
// Lane 2 — DUPLICATE TITLES, COLOPHON-AWARE: files sharing one
// normalized Tibetan title are grouped, then their COLOPHONS are
// compared — the session lesson that two works can share a title
// ("same title, different work") while true duplicates share their
// colophon too. Verdicts name their evidence class; "no colophon
// evidence" is a real verdict, not a guess.
#pragma once

#include <string>
#include <vector>

#include "allcore/title_xlat.h"

namespace allcore {

struct QcMismatch {
    std::string file;                 // the flagged file's name
    std::string own_tib, own_eng;     // its own filename fields
    std::string other_key, other_tib, other_eng;   // the work whose
                                                   // English this looks like
    double eng_sim = 0.0, tib_sim = 0.0;
};

// Scan the ACIP-named files of a tree whose names carry both fields;
// flag those whose English half matches another bank pair's English at
// >= eng_floor while the Tibetan overlap stays <= tib_ceiling.
std::vector<QcMismatch> qcTitleTranslationMismatch(
    const std::string& root, const TitlePairBank& bank,
    double eng_floor = 0.85, double tib_ceiling = 0.3, int max_flags = 50);

struct QcDupMember {
    std::string file;
    std::string colophon;   // first composition clause, "" when none
};

struct QcDupGroup {
    std::string title_norm;
    std::vector<QcDupMember> members;
    std::string verdict;   // "same colophon - true duplicates" |
                           // "different colophons - distinct works sharing a title" |
                           // "no colophon evidence - needs a human read"
};

// Group intake files by normalized Tibetan title (filename field, else
// the text's own title page) and compare colophons within each group.
std::vector<QcDupGroup> qcDuplicateTitles(const std::string& root,
                                          int max_groups = 50);

}  // namespace allcore
