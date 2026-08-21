#include "allcore/dossier.h"

#include <algorithm>
#include <filesystem>
#include <fstream>
#include <sstream>

namespace fs = std::filesystem;

namespace allcore {

namespace {
std::string esc(const std::string& s) {
    std::string o;
    for (char c : s) {
        if (c == '\t') o += "\\t";
        else if (c == '\n') o += "\\n";
        else if (c == '\\') o += "\\\\";
        else o += c;
    }
    return o;
}
std::string unesc(const std::string& s) {
    std::string o;
    for (size_t i = 0; i < s.size(); ++i) {
        if (s[i] == '\\' && i + 1 < s.size()) {
            const char n = s[++i];
            o += n == 't' ? '\t' : n == 'n' ? '\n' : n;
        } else {
            o += s[i];
        }
    }
    return o;
}
std::vector<std::string> cols(const std::string& line) {
    std::vector<std::string> out;
    std::string cur;
    for (char c : line) {
        if (c == '\t') { out.push_back(cur); cur.clear(); }
        else cur += c;
    }
    out.push_back(cur);
    return out;
}
}  // namespace

DossierStore::DossierStore(const std::string& dir) : dir_(dir) {}

bool DossierStore::load() {
    items_.clear();
    std::ifstream f(dir_ + "/dossiers.tsv");
    if (!f) return true;   // no desks yet — an empty store, honestly
    std::string line;
    while (std::getline(f, line)) {
        if (!line.empty() && line.back() == '\r') line.pop_back();
        if (line.empty() || line[0] == '#') continue;
        auto c = cols(line);
        if (c.size() < 6) continue;
        Dossier x;
        x.slug = unesc(c[0]);
        x.title = unesc(c[1]);
        x.textPath = unesc(c[2]);
        x.line = std::atoi(c[3].c_str());
        x.created = unesc(c[4]);
        x.touched = unesc(c[5]);
        items_.push_back(std::move(x));
    }
    sortByTouched();
    return true;
}

bool DossierStore::save() {
    std::error_code ec;
    fs::create_directories(dir_, ec);
    std::ofstream f(dir_ + "/dossiers.tsv", std::ios::trunc);
    if (!f) return false;
    f << "# translation dossiers - slug, title, text, line, "
         "created, touched (a desk, not the shared record)\n";
    for (const auto& x : items_)
        f << esc(x.slug) << '\t' << esc(x.title) << '\t'
          << esc(x.textPath) << '\t' << x.line << '\t'
          << esc(x.created) << '\t' << esc(x.touched) << '\n';
    return (bool)f;
}

std::string DossierStore::create(const std::string& title,
                                 const std::string& textPath,
                                 int line,
                                 const std::string& stamp) {
    if (title.empty() || textPath.empty()) return "";
    std::string base;
    for (char c : title)
        base += std::isalnum((unsigned char)c)
                    ? (char)std::tolower((unsigned char)c)
                    : '-';
    std::ostringstream slug;
    slug << base.substr(0, 24) << "-" << (items_.size() + 1);
    Dossier x;
    x.slug = slug.str();
    x.title = title;
    x.textPath = textPath;
    x.line = line < 1 ? 1 : line;
    x.created = stamp;
    x.touched = stamp;
    items_.push_back(std::move(x));
    sortByTouched();
    return slug.str();
}

bool DossierStore::touch(const std::string& slug, int line,
                         const std::string& stamp) {
    for (auto& x : items_)
        if (x.slug == slug) {
            if (line >= 1) x.line = line;
            x.touched = stamp;
            sortByTouched();
            return true;
        }
    return false;
}

bool DossierStore::remove(const std::string& slug) {
    const size_t n = items_.size();
    items_.erase(std::remove_if(items_.begin(), items_.end(),
                                [&](const Dossier& x) {
                                    return x.slug == slug;
                                }),
                 items_.end());
    return items_.size() != n;
}

void DossierStore::sortByTouched() {
    std::stable_sort(items_.begin(), items_.end(),
                     [](const Dossier& a, const Dossier& b) {
                         return a.touched > b.touched;
                     });
}

}  // namespace allcore
