#include "allcore/colloquial.h"

#include <fstream>
#include <sstream>

namespace allcore {

namespace {
// the pron_index fold: lowercase, letters only
std::string foldPron(const std::string& s) {
    std::string out;
    for (char c : s) {
        if (c >= 'a' && c <= 'z') out += c;
        else if (c >= 'A' && c <= 'Z') out += (char)(c - 'A' + 'a');
    }
    return out;
}
}  // namespace

bool ColloquialPron::load(const std::string& tsvPath) {
    std::ifstream f(tsvPath);
    if (!f) return false;
    std::string line;
    while (std::getline(f, line)) {
        if (line.empty() || line[0] == '#') continue;
        std::vector<std::string> cols;
        std::stringstream ss(line);
        std::string col;
        while (std::getline(ss, col, '\t')) cols.push_back(col);
        if (cols.size() < 2) continue;
        ColloquialEntry e;
        e.colloquial = cols[0];
        e.wylie = cols[1];
        if (cols.size() > 2) e.gmrPron = cols[2];
        if (cols.size() > 3) e.cls = cols[3];
        const int ix = static_cast<int>(entries_.size());
        ix_[foldPron(e.colloquial)].push_back(ix);
        byWylie_[e.wylie].push_back(ix);
        entries_.push_back(std::move(e));
    }
    return !entries_.empty();
}

std::vector<const ColloquialEntry*> ColloquialPron::byWylie(
    const std::string& wylie) const {
    std::vector<const ColloquialEntry*> out;
    auto it = byWylie_.find(wylie);
    if (it != byWylie_.end())
        for (int ix : it->second) out.push_back(&entries_[ix]);
    return out;
}

std::vector<const ColloquialEntry*> ColloquialPron::byColloquial(
    const std::string& query) const {
    std::vector<const ColloquialEntry*> out;
    auto it = ix_.find(foldPron(query));
    if (it != ix_.end())
        for (int ix : it->second) out.push_back(&entries_[ix]);
    return out;
}

bool applyPronunciationRuling(const std::string& tsvPath,
                              const std::string& colloquial,
                              const std::string& wylie,
                              bool approve,
                              const std::string& ruler,
                              const std::string& isoDate) {
    std::ifstream in(tsvPath);
    if (!in) return false;
    std::vector<std::string> lines;
    bool hit = false;
    std::string line;
    while (std::getline(in, line)) {
        if (!line.empty() && line.back() == '\r') line.pop_back();
        if (!line.empty() && line[0] != '#') {
            // split the 4 columns (class may carry a trailing comment)
            std::vector<std::string> c;
            std::string cur;
            for (char ch : line) {
                if (ch == '\t') { c.push_back(cur); cur.clear(); }
                else cur += ch;
            }
            c.push_back(cur);
            if (c.size() >= 4 && c[0] == colloquial && c[1] == wylie &&
                c[3].rfind("prenasal-derived", 0) == 0) {
                hit = true;
                if (!approve) continue;   // decline: drop the derived row
                lines.push_back(c[0] + "\t" + c[1] + "\t" + c[2] +
                                "\tapproved\t# ruled by " +
                                (ruler.empty() ? "authority" : ruler) +
                                " " + isoDate);
                continue;
            }
        }
        lines.push_back(line);
    }
    in.close();
    if (!hit) return false;
    std::ofstream out(tsvPath, std::ios::trunc);
    if (!out) return false;
    for (const auto& l : lines) out << l << "\n";
    return true;
}

}  // namespace allcore
