// catalog_qc.cpp — see allcore/catalog_qc.h.
#include "allcore/catalog_qc.h"

#include <algorithm>
#include <cctype>
#include <filesystem>
#include <fstream>
#include <map>
#include <set>
#include <sstream>

#include "allcore/catalog_id.h"
#include "allcore/tibexport.h"

namespace allcore {
namespace {

// lowercase content words of an English title (short function words
// dropped — they match everything)
std::set<std::string> engWords(const std::string& s) {
    std::set<std::string> out;
    std::string w;
    auto flush = [&] {
        if (w.size() >= 3 && w != "the" && w != "and" && w != "for" &&
            w != "with" && w != "from" && w != "into")
            out.insert(w);
        w.clear();
    };
    for (char c : s) {
        if (std::isalpha(static_cast<unsigned char>(c)))
            w.push_back(static_cast<char>(
                std::tolower(static_cast<unsigned char>(c))));
        else
            flush();
    }
    flush();
    return out;
}

double setSim(const std::set<std::string>& a,
              const std::set<std::string>& b) {
    if (a.empty() || b.empty()) return 0.0;
    int shared = 0;
    for (const auto& x : a)
        if (b.count(x)) ++shared;
    return static_cast<double>(shared) /
           static_cast<double>(std::max(a.size(), b.size()));
}

std::set<std::string> tibSyls(const std::string& norm) {
    std::set<std::string> out;
    std::istringstream is(norm);
    std::string w;
    while (is >> w) out.insert(w);
    return out;
}

struct NamedFile {
    std::string path, name, tib, eng;
};

std::vector<NamedFile> namedFiles(const std::string& root) {
    namespace fs = std::filesystem;
    std::vector<NamedFile> out;
    std::error_code ec;
    if (!fs::exists(root, ec)) return out;
    for (fs::recursive_directory_iterator it(
             root, fs::directory_options::skip_permission_denied, ec), end;
         it != end; it.increment(ec)) {
        if (ec) break;
        if (!it->is_regular_file(ec)) continue;
        const std::string name = it->path().filename().string();
        std::string up = name;
        for (char& c : up)
            c = static_cast<char>(std::toupper(static_cast<unsigned char>(c)));
        if (up.find(" META.") != std::string::npos) continue;
        const size_t dot = up.rfind('.');
        const std::string ext = dot == std::string::npos ? "" : up.substr(dot);
        if (ext != ".TXT" && ext != ".ACT" && ext != ".INC") continue;
        NamedFile nf;
        nf.path = it->path().string();
        nf.name = name;
        if (decodeAcipFilename(name).recognized) {
            std::string stem = name.substr(0, name.rfind('.'));
            std::vector<std::string> fields;
            size_t start = 0;
            while (true) {
                const size_t u = stem.find('_', start);
                if (u == std::string::npos) {
                    fields.push_back(stem.substr(start));
                    break;
                }
                fields.push_back(stem.substr(start, u - start));
                start = u + 1;
            }
            if (fields.size() >= 2) nf.tib = fields[1];
            if (fields.size() >= 3) nf.eng = fields[2];
        }
        out.push_back(std::move(nf));
    }
    std::sort(out.begin(), out.end(),
              [](const NamedFile& a, const NamedFile& b) {
                  return a.path < b.path;
              });
    return out;
}

}  // namespace

std::vector<QcMismatch> qcTitleTranslationMismatch(
    const std::string& root, const TitlePairBank& bank,
    double eng_floor, double tib_ceiling, int max_flags) {
    std::vector<QcMismatch> out;
    const auto files = namedFiles(root);
    for (const auto& f : files) {
        if (f.tib.empty() || f.eng.empty()) continue;
        const auto ownTib = tibSyls(normalizeAcipTitle(f.tib));
        const auto ownEng = engWords(f.eng);
        if (ownEng.size() < 3) continue;   // too thin to judge
        for (const auto& e : bank.entries()) {
            const double es = setSim(ownEng, engWords(e.eng));
            if (es < eng_floor) continue;
            const double ts = setSim(ownTib, tibSyls(e.tib_norm));
            if (ts > tib_ceiling) continue;
            QcMismatch m;
            m.file = f.name;
            m.own_tib = f.tib;
            m.own_eng = f.eng;
            m.other_tib = e.tib_raw;
            m.other_eng = e.eng;
            m.other_key = e.source;
            m.eng_sim = es;
            m.tib_sim = ts;
            out.push_back(std::move(m));
            break;   // one witness is enough to raise the question
        }
        if (static_cast<int>(out.size()) >= max_flags) break;
    }
    return out;
}

std::vector<QcDupGroup> qcDuplicateTitles(const std::string& root,
                                          int max_groups) {
    const auto files = namedFiles(root);
    std::map<std::string, std::vector<const NamedFile*>> byTitle;
    for (const auto& f : files) {
        std::string norm;
        if (!f.tib.empty()) {
            norm = normalizeAcipTitle(f.tib);
        } else {
            std::ifstream in(f.path, std::ios::binary);
            std::string head(4000, '\0');
            in.read(&head[0], 4000);
            head.resize(static_cast<size_t>(in.gcount()));
            const auto t = extractAcipTitle(head);
            if (t.found) norm = t.title;
        }
        if (tibSyls(norm).size() < 3) continue;
        byTitle[norm].push_back(&f);
    }
    std::vector<QcDupGroup> out;
    for (const auto& [title, members] : byTitle) {
        if (members.size() < 2) continue;
        QcDupGroup g;
        g.title_norm = title;
        for (const auto* m : members) {
            QcDupMember mem;
            mem.file = m->name;
            std::ifstream in(m->path, std::ios::binary);
            std::ostringstream ss;
            ss << in.rdbuf();
            for (const auto& sp : findColophonCandidates(ss.str()))
                if (sp.kind == "composition") {
                    mem.colophon = sp.text;
                    break;
                }
            g.members.push_back(std::move(mem));
        }
        // colophon verdict
        int withColo = 0;
        for (const auto& m : g.members)
            if (!m.colophon.empty()) ++withColo;
        if (withColo < 2) {
            g.verdict = "no colophon evidence - needs a human read";
        } else {
            bool allSame = true;
            const auto first =
                tibSyls(normalizeAcipTitle(g.members.front().colophon));
            for (const auto& m : g.members) {
                if (m.colophon.empty()) continue;
                const double s = setSim(
                    first, tibSyls(normalizeAcipTitle(m.colophon)));
                if (s < 0.6) allSame = false;
            }
            g.verdict = allSame
                            ? "same colophon - true duplicates"
                            : "different colophons - distinct works "
                              "sharing a title";
        }
        out.push_back(std::move(g));
        if (static_cast<int>(out.size()) >= max_groups) break;
    }
    return out;
}

}  // namespace allcore
