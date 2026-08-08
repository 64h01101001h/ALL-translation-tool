#include "allcore/poslex.h"

#include <fstream>
#include <sstream>

namespace allcore {

namespace {

// strip a final tsheg (U+0F0B, UTF-8 e0 bd 8b) and trailing spaces
std::string stripKey(const std::string& w) {
    std::string out = w;
    while (!out.empty() && (out.back() == ' ' || out.back() == '\r'))
        out.pop_back();
    if (out.size() >= 3 &&
        static_cast<unsigned char>(out[out.size() - 3]) == 0xE0 &&
        static_cast<unsigned char>(out[out.size() - 2]) == 0xBC &&
        static_cast<unsigned char>(out[out.size() - 1]) == 0x8B)
        out.resize(out.size() - 3);
    return out;
}

}  // namespace

bool PosLexicon::load(const std::string& path) {
    std::ifstream f(path);
    if (!f) return false;
    std::string line;
    while (std::getline(f, line)) {
        if (!line.empty() && line.back() == '\r') line.pop_back();
        if (line.empty()) continue;
        std::stringstream ss(line);
        std::string field;
        std::string word;
        bool first = true;
        while (std::getline(ss, field, '\t')) {
            if (first) {
                word = stripKey(field);
                first = false;
                continue;
            }
            // field = "tag -": drop the source's placeholder column
            if (field.size() >= 2 && field.compare(field.size() - 2, 2, " -") == 0)
                field.resize(field.size() - 2);
            while (!field.empty() && field.back() == ' ') field.pop_back();
            if (!word.empty() && !field.empty()) map_[word].insert(field);
        }
    }
    return !map_.empty();
}

const std::set<std::string>* PosLexicon::find(const std::string& w) const {
    auto it = map_.find(stripKey(w));
    return it == map_.end() ? nullptr : &it->second;
}

std::vector<std::string> PosLexicon::tags(const std::string& w) const {
    const auto* s = find(w);
    return s ? std::vector<std::string>(s->begin(), s->end())
             : std::vector<std::string>();
}

bool PosLexicon::unambiguousAdj(const std::string& w) const {
    const auto* s = find(w);
    return s && s->size() == 1 && *s->begin() == "adj";
}

bool PosLexicon::unambiguousAdv(const std::string& w) const {
    const auto* s = find(w);
    if (!s || s->empty()) return false;
    for (const auto& t : *s)
        if (t.rfind("adv", 0) != 0) return false;
    return true;
}

bool PosLexicon::unambiguousNoun(const std::string& w) const {
    const auto* s = find(w);
    if (!s || s->empty()) return false;
    for (const auto& t : *s)
        if (t != "n.count" && t != "n.mass" && t != "n.prop") return false;
    return true;
}

}  // namespace allcore
