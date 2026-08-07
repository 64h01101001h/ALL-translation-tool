#include "allcore/lexicon.h"

#include <fstream>

namespace allcore {

bool RefLexicon::load(const std::string& path, const std::string& label) {
    std::ifstream f(path);
    if (!f) return false;
    auto& set = labelA_.empty() || labelA_ == label ? a_ : b_;
    (labelA_.empty() || labelA_ == label ? labelA_ : labelB_) = label;
    std::string line;
    while (std::getline(f, line)) {
        while (!line.empty() &&
               (line.back() == '\r' || line.back() == ' '))
            line.pop_back();
        if (!line.empty()) set.insert(line);
    }
    return !set.empty();
}

std::string RefLexicon::attested(const std::string& w) const {
    std::string out;
    if (a_.count(w)) out = labelA_;
    if (b_.count(w)) out += (out.empty() ? "" : " + ") + labelB_;
    return out;
}

}  // namespace allcore
