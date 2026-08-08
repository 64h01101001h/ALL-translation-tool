#include "allcore/whitney.h"

#include <cctype>
#include <fstream>
#include <sstream>

namespace allcore {

std::string foldIast(const std::string& s) {
    static const std::map<std::string, char> folds = {
        {"ā", 'a'}, {"ī", 'i'}, {"ū", 'u'},   // ā ī ū
        {"ṛ", 'r'}, {"ṝ", 'r'},                    // ṛ ṝ
        {"ḷ", 'l'}, {"ḹ", 'l'},                    // ḷ ḹ
        {"ṃ", 'm'}, {"ṁ", 'm'},                    // ṃ ṁ
        {"ḥ", 'h'},                                     // ḥ
        {"ṅ", 'n'}, {"ñ", 'n'}, {"ṇ", 'n'},   // ṅ ñ ṇ
        {"ṭ", 't'}, {"ḍ", 'd'},                    // ṭ ḍ
        {"ś", 's'}, {"ṣ", 's'},                    // ś ṣ
    };
    std::string out;
    out.reserve(s.size());
    for (size_t i = 0; i < s.size();) {
        bool hit = false;
        for (int len = 2; len <= 3 && !hit; ++len) {
            if (i + len <= s.size()) {
                auto it = folds.find(s.substr(i, len));
                if (it != folds.end()) {
                    out += it->second;
                    i += len;
                    hit = true;
                }
            }
        }
        if (!hit) {
            char c = s[i++];
            if (c >= 'A' && c <= 'Z') c = static_cast<char>(c + 32);
            out += c;
        }
    }
    return out;
}

bool WhitneyRoots::load(const std::string& tsvPath) {
    std::ifstream f(tsvPath);
    if (!f) return false;
    std::string line;
    while (std::getline(f, line)) {
        if (line.empty() || line[0] == '#') continue;
        std::vector<std::string> cols;
        std::stringstream ss(line);
        std::string col;
        while (std::getline(ss, col, '\t')) cols.push_back(col);
        if (cols.size() < 4) continue;
        WhitneyRoot r;
        try {
            r.id = std::stoi(cols[0]);
        } catch (...) {
            continue;
        }
        r.root = cols[1];
        r.homonym = cols[2];
        r.meaning = cols[3];
        if (cols.size() > 4) r.classes = cols[4];
        if (cols.size() > 5) r.classUncertain = cols[5];
        if (cols.size() > 6) r.ppp = cols[6];
        if (cols.size() > 7) r.grammarSecs = cols[7];
        if (cols.size() > 8) r.dcsClasses = cols[8];
        if (cols.size() > 9) r.mwId = cols[9];
        if (cols.size() > 10) r.senses = cols[10];
        if (cols.size() > 11) r.notes = cols[11];
        if (cols.size() > 12) r.slp1 = cols[12];
        byFolded_[foldIast(r.root)].push_back(
            static_cast<int>(entries_.size()));
        entries_.push_back(std::move(r));
    }
    return !entries_.empty();
}

std::vector<const WhitneyRoot*> WhitneyRoots::byRoot(
    const std::string& iast) const {
    std::vector<const WhitneyRoot*> out;
    auto it = byFolded_.find(foldIast(iast));
    if (it != byFolded_.end())
        for (int ix : it->second) out.push_back(&entries_[ix]);
    return out;
}

std::vector<const WhitneyRoot*> WhitneyRoots::byMeaning(
    const std::string& word, int limit) const {
    std::string w;
    for (char c : word)
        w += (c >= 'A' && c <= 'Z') ? static_cast<char>(c + 32) : c;
    std::vector<const WhitneyRoot*> out;
    for (const auto& r : entries_) {
        if (static_cast<int>(out.size()) >= limit) break;
        // whole-word match in the meaning, case-folded
        std::string m;
        for (char c : r.meaning)
            m += (c >= 'A' && c <= 'Z') ? static_cast<char>(c + 32) : c;
        size_t at = m.find(w);
        while (at != std::string::npos) {
            const bool lOk = at == 0 || !std::isalpha(
                static_cast<unsigned char>(m[at - 1]));
            const bool rOk = at + w.size() >= m.size() ||
                             !std::isalpha(static_cast<unsigned char>(
                                 m[at + w.size()]));
            if (lOk && rOk) {
                out.push_back(&r);
                break;
            }
            at = m.find(w, at + 1);
        }
    }
    return out;
}

}  // namespace allcore
