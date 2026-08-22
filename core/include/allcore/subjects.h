// allcore/subjects.h — what the CATALOG says a text is about.
//
// R6's bilingual subject headings (243 of them) and the Sungbum
// catalog's Tibetan headings, flattened to one work-keyed table by
// tools/build_subject_index.py. Someone wrote these down: they are
// facts with an author, and they display as REFERENCE.
//
// This is NOT a register layer, and the distinction is the whole
// point. Nothing here says how a word should be READ in a genre.
// Saying that would require Geshe Michael to have said it, and
// measurement found he has for about five terms out of 12,004
// (docs/FINDINGS.md, 2026-08-22 — `bsod nams` "goodness (prayer
// register)" is nearly alone). Ranking or re-ordering glosses by
// genre would be the machine deciding what he meant: composition
// wearing the binding tier's clothes, which rule 1 forbids. A caller
// may state the subject and group evidence by it; it may not let the
// subject change which English is offered as his.
//
// Coverage is partial and must be said so: 1,906 of the library's
// 5,607 work keys carry a label. "No subject recorded" is a real
// answer, not a failure.
//
// Vinaya carries its own flag because Adam ruled (2026-08-22) that it
// reads differently, being the oldest language in the monastic
// curriculum. The flag is set only on an EXACT match of R6's
// "Vowed_Morality--'DUL_BA" heading — deliberately not a substring
// test, because "…MTHA'_'KHOB_'DUL_BA'I_BLO_SBYONG" contains 'DUL_BA
// and is a lojong text, not Vinaya.
#pragma once

#include <string>
#include <unordered_map>
#include <vector>

namespace allcore {

struct WorkSubject {
    std::string english;   // "" for Sungbum rows: no English side exists
    std::string tibetan;
    std::string raw;       // the label exactly as catalogued
    std::string source;    // "r6" | "sungbum"
    bool vinaya = false;
};

class Subjects {
public:
    // data/extracted/work_subjects.tsv
    bool load(const std::string& tsvPath);
    size_t rows() const { return rows_; }
    size_t works() const { return byWork_.size(); }

    // Empty when nothing is catalogued for this key — which is the
    // common case and must be reported as such, never as an error.
    const std::vector<WorkSubject>& forWork(const std::string& workKey) const;

    bool isVinaya(const std::string& workKey) const;

private:
    std::unordered_map<std::string, std::vector<WorkSubject>> byWork_;
    std::vector<WorkSubject> empty_;
    size_t rows_ = 0;
};

}  // namespace allcore
