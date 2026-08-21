#include "allcore/glossary.h"

#include <filesystem>
#include <fstream>

namespace fs = std::filesystem;

namespace allcore {

GlossaryStore::GlossaryStore(const std::string& path)
    : path_(path) {}

bool GlossaryStore::load() {
    items_.clear();
    std::ifstream f(path_);
    if (!f) return true;
    std::string line;
    while (std::getline(f, line)) {
        if (!line.empty() && line.back() == '\r') line.pop_back();
        if (line.empty() || line[0] == '#') continue;
        const auto tab = line.find('\t');
        if (tab == std::string::npos || tab == 0) continue;
        auto trim = [](std::string s) {
            const auto a = s.find_first_not_of(" \t");
            const auto b = s.find_last_not_of(" \t");
            return a == std::string::npos
                       ? std::string()
                       : s.substr(a, b - a + 1);
        };
        const std::string w = trim(line.substr(0, tab));
        const std::string g = trim(line.substr(tab + 1));
        if (!w.empty() && !g.empty()) items_[w] = g;
    }
    return true;
}

bool GlossaryStore::save(const std::string& docName) const {
    std::error_code ec;
    fs::create_directories(fs::path(path_).parent_path(), ec);
    std::ofstream f(path_, std::ios::trunc);
    if (!f) return false;
    f << "# per-text glossary for " << docName
      << " — wylie <TAB> gloss; the translator's own, binding for "
         "this text's display\n";
    for (const auto& [w, g] : items_)
        f << w << '\t' << g << '\n';
    return (bool)f;
}

void GlossaryStore::set(const std::string& wylie,
                        const std::string& gloss) {
    if (!wylie.empty() && !gloss.empty()) items_[wylie] = gloss;
}

bool GlossaryStore::remove(const std::string& wylie) {
    return items_.erase(wylie) > 0;
}

std::string GlossaryStore::get(const std::string& wylie) const {
    const auto it = items_.find(wylie);
    return it == items_.end() ? std::string() : it->second;
}

}  // namespace allcore
