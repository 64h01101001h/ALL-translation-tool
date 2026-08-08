#include "allcore/mvp.h"

#include <fstream>
#include <sstream>

namespace allcore {

namespace {

// ASCII-lowercase only: IAST diacritics and Tibetan are left untouched,
// so the fold is byte-safe on UTF-8.
std::string foldKey(const std::string& s) {
    std::string out;
    out.reserve(s.size());
    for (char c : s)
        out.push_back(c >= 'A' && c <= 'Z' ? static_cast<char>(c + 32) : c);
    while (!out.empty() && (out.back() == ' ' || out.back() == '\r'))
        out.pop_back();
    size_t b = 0;
    while (b < out.size() && out[b] == ' ') ++b;
    return out.substr(b);
}

std::vector<std::string> splitPipes(const std::string& s) {
    std::vector<std::string> out;
    std::stringstream ss(s);
    std::string piece;
    while (std::getline(ss, piece, '|'))
        if (!piece.empty()) out.push_back(piece);
    return out;
}

}  // namespace

bool Mvp::load(const std::string& tsvPath) {
    std::ifstream f(tsvPath);
    if (!f) return false;
    std::string line;
    while (std::getline(f, line)) {
        if (!line.empty() && line.back() == '\r') line.pop_back();
        if (line.empty()) continue;
        std::vector<std::string> cols;
        std::stringstream ss(line);
        std::string col;
        while (std::getline(ss, col, '\t')) cols.push_back(col);
        if (cols.size() < 7) continue;
        MvpEntry e;
        try {
            e.key = std::stoi(cols[0]);
        } catch (...) {
            continue;
        }
        e.chapter = cols[1];
        e.iast = cols[2];
        e.deva = cols[3];
        e.wylie = splitPipes(cols[4]);
        e.tibetan = splitPipes(cols[5]);
        e.chinese = splitPipes(cols[6]);
        int ix = static_cast<int>(entries_.size());
        entries_.push_back(std::move(e));
        const MvpEntry& stored = entries_.back();
        if (!stored.iast.empty()) iastIx_[foldKey(stored.iast)].push_back(ix);
        for (const auto& w : stored.wylie) wylieIx_[foldKey(w)].push_back(ix);
        keyIx_.emplace(stored.key, ix);
    }
    return !entries_.empty();
}

std::vector<const MvpEntry*> Mvp::byWylie(const std::string& wylie) const {
    std::vector<const MvpEntry*> out;
    auto it = wylieIx_.find(foldKey(wylie));
    if (it != wylieIx_.end())
        for (int ix : it->second) out.push_back(&entries_[ix]);
    return out;
}

std::vector<const MvpEntry*> Mvp::byIast(const std::string& iast) const {
    std::vector<const MvpEntry*> out;
    auto it = iastIx_.find(foldKey(iast));
    if (it != iastIx_.end())
        for (int ix : it->second) out.push_back(&entries_[ix]);
    return out;
}

const MvpEntry* Mvp::byKey(int key) const {
    auto it = keyIx_.find(key);
    return it == keyIx_.end() ? nullptr : &entries_[it->second];
}

}  // namespace allcore
