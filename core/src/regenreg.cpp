#include "allcore/regenreg.h"

#include <filesystem>
#include <fstream>
#include <sstream>
#include <vector>

#include "allcore/colloquial.h"
#include "allcore/engines.h"

namespace allcore {

namespace {

// the marker this tool (and the older live-append path) writes;
// stripping exactly these rows is what makes regeneration idempotent
const char* kMarker = "# approved by ";

std::vector<std::string> readLines(const std::string& path) {
    std::vector<std::string> out;
    std::ifstream f(path);
    std::string line;
    while (std::getline(f, line)) {
        if (!line.empty() && line.back() == '\r') line.pop_back();
        out.push_back(line);
    }
    return out;
}

bool writeLines(const std::string& path,
                const std::vector<std::string>& lines) {
    std::ofstream f(path, std::ios::trunc);
    if (!f) return false;
    for (const auto& l : lines) f << l << "\n";
    // SQA FAIL-2 (critical): this ended `return true;` after an
    // UNCHECKED ofstream. Measured against the real allcore on a
    // volume with a 16,384-byte hole: save() returned TRUE having
    // written 16,384 of 123,576 bytes. The stores either side of it
    // in the same probe reported honestly, because they end with
    // `return (bool)f;` — the rule was right and held only where its
    // author remembered it. Flushing first turns a buffered short
    // write into a stream error we can actually report (house rule 4:
    // nothing reports success that did not verify the bytes landed).
    // Flush AND close before judging. Measured 2026-08-23 on a full
    // 2 MB volume: flush() alone reported success after writing
    // 155,648 of ~240,000 bytes — the failure only surfaces when the
    // filebuf is closed, so a check before close() is still a lie.
    // Caught by tools/test_shortwrite.sh, which is the whole reason
    // that test exists.
    f.flush();
    f.close();
    return !f.fail();
}

std::string provenance(const Proposal& p) {
    std::string who = p.approver.empty() ? "authority" : p.approver;
    return std::string(kMarker) + who + " " + p.ruled;
}

// a proposal's Tibetan may arrive as ACIP (defined uppercase) —
// registers store wylie
std::string toWylie(const std::string& s) {
    bool upper = false;
    for (char c : s) upper |= (c >= 'A' && c <= 'Z');
    return upper ? acipToEwts(s) : s;
}

const char* honorificLevel(ProposalKind k) {
    switch (k) {
        case ProposalKind::HighHonorific: return "high";
        case ProposalKind::Humilific: return "humilific";
        case ProposalKind::DoubleHonorific: return "double";
        default: return "honorific";
    }
}

// CSV fields for the abbreviations file: commas/newlines would break
// the row shape, so they become spaces (shape-preserving, lossless
// for real EWTS which uses neither)
std::string csvField(const std::string& s) {
    std::string o = s;
    for (char& c : o)
        if (c == ',' || c == '\n' || c == '\t') c = ' ';
    return o;
}

}  // namespace

RegenStats regenerateApprovedRegisters(const ProposalStore& store,
                                       const std::string& dataRoot) {
    namespace fs = std::filesystem;
    RegenStats st;

    // ---- honorific register: strip folded rows, re-fold from store
    {
        const std::string path =
            dataRoot + "/data/honorifics/honorific_register.tsv";
        if (fs::exists(path)) {
            std::vector<std::string> lines;
            for (auto& l : readLines(path))
                if (l.find(kMarker) == std::string::npos)
                    lines.push_back(l);
            for (const auto& p : store.all()) {
                if (p.status != ProposalStatus::Approved) continue;
                if (p.kind != ProposalKind::Honorific &&
                    p.kind != ProposalKind::HighHonorific &&
                    p.kind != ProposalKind::Humilific &&
                    p.kind != ProposalKind::DoubleHonorific)
                    continue;
                // honorific_wylie \t ordinary \t domain \t level
                lines.push_back(toWylie(p.wylie) + "\t" + p.field +
                                "\t" + p.value + "\t" +
                                honorificLevel(p.kind) + "\t" +
                                provenance(p));
                ++st.honorific;
            }
            writeLines(path, lines);
        }
    }

    // ---- colloquial pronunciation: strip folded rows, then per
    // approval prefer the battery-proven in-place upgrade of a
    // machine-derived row; a novel form folds in as an appended
    // approved row
    {
        const std::string path =
            dataRoot + "/data/pron_colloquial/colloquial_pron.tsv";
        if (fs::exists(path)) {
            {
                std::vector<std::string> keep;
                for (auto& l : readLines(path))
                    if (l.find(kMarker) == std::string::npos)
                        keep.push_back(l);
                writeLines(path, keep);
            }
            for (const auto& p : store.all()) {
                if (p.status != ProposalStatus::Approved) continue;
                if (p.kind != ProposalKind::Pronunciation) continue;
                const std::string who =
                    p.approver.empty() ? "authority" : p.approver;
                if (applyPronunciationRuling(path, p.value,
                                             toWylie(p.wylie), true,
                                             who, p.ruled)) {
                    ++st.pron;
                    continue;
                }
                // already upgraded in place on an earlier run?
                bool present = false;
                for (auto& l : readLines(path)) {
                    std::stringstream ss(l);
                    std::string c0, c1;
                    std::getline(ss, c0, '\t');
                    std::getline(ss, c1, '\t');
                    present |= (c0 == p.value &&
                                c1 == toWylie(p.wylie));
                }
                if (!present) {
                    auto lines = readLines(path);
                    // colloquial \t wylie \t gmr_pron \t class
                    lines.push_back(p.value + "\t" +
                                    toWylie(p.wylie) +
                                    "\t\tapproved\t" + provenance(p));
                    writeLines(path, lines);
                }
                ++st.pron;
            }
        }
    }

    // ---- approved abbreviations: the file is ENTIRELY
    // approval-derived, so it regenerates whole — same CSV shape as
    // the TibSchol base table so AbbrTable::load reads it as a
    // second layer (5th column = provenance, ignored by the loader).
    // Unicode renderings come from the proven converter; a form it
    // refuses leaves the column empty (never guessed).
    {
        const std::string path =
            dataRoot + "/data/abbreviations/approved_abbreviations.tsv";
        std::vector<std::string> rows;
        for (const auto& p : store.all()) {
            if (p.status != ProposalStatus::Approved) continue;
            if (p.kind != ProposalKind::Abbreviation) continue;
            const std::string aw = toWylie(p.wylie);
            const std::string ew = toWylie(p.value);
            auto [au, aok] = wylieToUnicode(aw);
            auto [eu, eok] = wylieToUnicode(ew);
            rows.push_back(csvField(aw) + "," +
                           (aok ? csvField(au) : std::string()) + "," +
                           csvField(ew) + "," +
                           (eok ? csvField(eu) : std::string()) + "," +
                           csvField(provenance(p)));
            ++st.abbrev;
        }
        if (!rows.empty() || fs::exists(path)) {
            std::vector<std::string> lines;
            lines.push_back(
                "Abb. Wylie,Abb. Unicode,Expan. Wylie,Expan. Unicode,"
                "Approval");
            for (auto& r : rows) lines.push_back(r);
            writeLines(path, lines);
        }
    }

    return st;
}

}  // namespace allcore
