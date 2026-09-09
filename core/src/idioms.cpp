// allcore/idioms.cpp — the idioms register and its longest-match walk.
// See idioms.h for what this module refuses to do (it holds no English).

#include "allcore/idioms.h"

#include <algorithm>
#include <cctype>

namespace allcore {
namespace idioms {

const char* statusName(Status s) {
    switch (s) {
        case Status::Approved: return "approved";
        case Status::Declined: return "declined";
        case Status::Proposed: break;
    }
    return "proposed";
}

Status statusFromName(const std::string& s) {
    if (s == "approved") return Status::Approved;
    if (s == "declined") return Status::Declined;
    return Status::Proposed;   // anything unrecognised is NOT treated as a ruling
}

namespace {

// A syllable as the matcher compares it: lower-cased, with the punctuation
// that separates syllables removed. Shads, brackets and the ACIP markers
// never carry a distinction the register cares about.
std::string canonicalSyllable(const std::string& in) {
    std::string out;
    for (unsigned char c : in) {
        if (c >= 'A' && c <= 'Z') { out += char(c - 'A' + 'a'); continue; }
        // keep the letters, the apostrophe and the wylie stack marks
        if ((c >= 'a' && c <= 'z') || (c >= '0' && c <= '9') || c == '\'' || c == '+' || c == '.' || c == '~' || c == '-') { out += char(c); continue; }
        if (c >= 0x80) { out += char(c); continue; }   // non-ASCII passes through
        // everything else (space, shad, brackets, @, digits' separators) is a boundary and is dropped here
    }
    return out;
}

}  // namespace

std::vector<std::string> splitPattern(const std::string& wylie) {
    std::vector<std::string> out;
    std::string cur;
    auto flush = [&] { const std::string c = canonicalSyllable(cur); if (!c.empty()) out.push_back(c); cur.clear(); };
    for (char ch : wylie) {
        const unsigned char c = (unsigned char)ch;
        if (c == ' ' || c == '\t' || c == '\n' || c == '\r' || c == ',' || c == ';' || c == '/' || c == '|' || c == '.') flush();
        else cur += ch;
    }
    flush();
    return out;
}

void Register::add(const Entry& e) {
    const std::vector<std::string> pat = splitPattern(e.wylie);
    if (pat.empty()) return;   // a form with no syllables cannot be matched or listed
    for (std::size_t i = 0; i < patterns_.size(); ++i) {
        if (patterns_[i] == pat) { items_[i] = e; return; }   // later wins: a ruling overrides the seed
    }
    items_.push_back(e);
    patterns_.push_back(pat);
}

void Register::addAll(const std::vector<Entry>& v) { for (const auto& e : v) add(e); }

void Register::clear() { items_.clear(); patterns_.clear(); }

const Entry* Register::find(const std::string& wylie) const {
    const std::vector<std::string> pat = splitPattern(wylie);
    if (pat.empty()) return nullptr;
    for (std::size_t i = 0; i < patterns_.size(); ++i)
        if (patterns_[i] == pat) return &items_[i];
    return nullptr;
}

int Register::countByStatus(Status s) const {
    int n = 0;
    for (const auto& e : items_) n += (e.status == s) ? 1 : 0;
    return n;
}

const Entry* Register::longestAt(const std::vector<std::string>& syllables, int at) const {
    if (at < 0 || at >= (int)syllables.size()) return nullptr;
    const Entry* best = nullptr;
    std::size_t bestLen = 0;
    for (std::size_t i = 0; i < patterns_.size(); ++i) {
        if (items_[i].status == Status::Declined) continue;   // ruled ordinary: never marked
        const auto& pat = patterns_[i];
        if (pat.size() <= bestLen) continue;                  // a shorter form cannot beat the one we hold
        if (at + (int)pat.size() > (int)syllables.size()) continue;
        bool eq = true;
        for (std::size_t k = 0; k < pat.size() && eq; ++k)
            eq = canonicalSyllable(syllables[at + k]) == pat[k];
        if (eq) { best = &items_[i]; bestLen = pat.size(); }
    }
    return best;
}

std::vector<Hit> Register::findIn(const std::vector<std::string>& syllables) const {
    std::vector<Hit> out;
    for (int i = 0; i < (int)syllables.size();) {
        const Entry* e = longestAt(syllables, i);
        if (!e) { ++i; continue; }
        const int len = (int)splitPattern(e->wylie).size();
        Hit h;
        h.begSyllable = i;
        h.endSyllable = i + len;
        h.wylie = e->wylie;
        h.status = e->status;
        out.push_back(h);
        i += len;   // non-overlapping: an idiom is consumed whole
    }
    return out;
}

}  // namespace idioms
}  // namespace allcore
