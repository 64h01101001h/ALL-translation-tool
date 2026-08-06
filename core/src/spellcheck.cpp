#include "allcore/spellcheck.h"

#include <fstream>
#include <sstream>
#include <stdexcept>

#include "allcore/engines.h"

namespace allcore {
namespace {

std::string slurp(const std::string& path) {
    std::ifstream f(path);
    if (!f) throw std::runtime_error("spellcheck data missing: " + path);
    std::stringstream ss;
    ss << f.rdbuf();
    return ss.str();
}

// lenient scan of suffixes.json (the upstream file has missing commas):
// captures "CLASS": [ "s1", "s2", ... ] as class → set of strings.
std::map<std::string, std::set<std::string>> parseSuffixes(const std::string& j) {
    std::map<std::string, std::set<std::string>> out;
    std::string cls;
    std::vector<std::string> strings;
    size_t i = 0;
    auto readString = [&](size_t& p) {
        std::string s;
        ++p;  // opening quote
        while (p < j.size() && j[p] != '"') s += j[p++];
        ++p;  // closing quote
        return s;
    };
    while (i < j.size()) {
        if (j[i] == '"') {
            std::string s = readString(i);
            // a string followed (after spaces) by ':' is a class key
            size_t k = i;
            while (k < j.size() && (j[k] == ' ' || j[k] == '\n' || j[k] == '\r'))
                ++k;
            if (k < j.size() && j[k] == ':') {
                cls = s;
                i = k + 1;
            } else if (!cls.empty()) {
                out[cls].insert(s);
            }
            continue;
        }
        if (j[i] == ']') cls.clear();
        ++i;
    }
    return out;
}

}  // namespace

SyllableChecker::SyllableChecker(const std::string& dir) {
    classes_ = parseSuffixes(slurp(dir + "/suffixes.json"));
    if (!classes_.count("A") || !classes_.count("C"))
        throw std::runtime_error("suffixes.json lacks class A/C");

    // upstream's own standard-syllable-structure doc lists འ among the ten
    // suffixes, but the A list omits it (their AB class carries it) — add it,
    // validated by the GT calibration (dga'/dpa' are ubiquitous real syllables)
    classes_["A"].insert("\xE0\xBD\xA0");   // འ U+0F60

    // NB = A minus the ba-suffix forms (བ U+0F56 as the suffix consonant)
    {
        std::set<std::string> nb;
        for (const auto& s : classes_["A"]) {
            const bool hasBa = s.find("\xE0\xBD\x96") != std::string::npos;
            if (!hasBa) nb.insert(s);
        }
        classes_["NB"] = std::move(nb);
    }

    auto loadDic = [this](const std::string& path) {
        std::istringstream in(slurp(path));
        std::string line;
        while (std::getline(in, line)) {
            while (!line.empty() && (line.back() == '\r' || line.back() == ' '))
                line.pop_back();
            if (line.empty()) continue;
            auto slash = line.find('/');
            if (slash == std::string::npos) bases_.push_back({line, ""});
            else bases_.push_back({line.substr(0, slash), line.substr(slash + 1)});
        }
    };
    loadDic(dir + "/root.txt");
    loadDic(dir + "/rare.txt");
    loadDic(dir + "/wasurs.txt");
    loadDic(dir + "/proper-names.txt");
    loadDic(dir + "/exceptions.txt");
    if (bases_.size() < 300)
        throw std::runtime_error("spellcheck data implausibly small");
}

bool SyllableChecker::legalUnicode(const std::string& raw) const {
    // strip any trailing tsheg the caller left on
    std::string syl = raw;
    const std::string TSHEG = "་";
    while (syl.size() >= TSHEG.size() &&
           syl.compare(syl.size() - TSHEG.size(), TSHEG.size(), TSHEG) == 0)
        syl.resize(syl.size() - TSHEG.size());
    if (syl.empty()) return false;

    for (const auto& [base, cls] : bases_) {
        if (syl.compare(0, base.size(), base) != 0) continue;
        const std::string rest = syl.substr(base.size());
        if (cls.empty()) {
            if (rest.empty()) return true;
            continue;
        }
        auto it = classes_.find(cls);
        if (it != classes_.end() && it->second.count(rest)) return true;
    }
    return false;
}

bool SyllableChecker::legalWylie(const std::string& wylie_syl) const {
    auto [u, ok] = wylieToUnicode(wylie_syl);
    if (!ok || u.empty()) return false;
    return legalUnicode(u);
}

}  // namespace allcore
