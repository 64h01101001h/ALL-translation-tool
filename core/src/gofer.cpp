#include "allcore/gofer.h"

#include "gofer_ast.h"

#include <algorithm>
#include <cctype>
#include <filesystem>
#include <fstream>
#include <functional>
#include <map>
#include <memory>
#include <sstream>
#include <stdexcept>

namespace allcore {
namespace {

using namespace gofer_ast;

// ---------------- evaluation ----------------
struct Window {
    std::string course;
    int lo = 0, hi = 0;
};

std::vector<Window> evalNode(const Spine& spine, const Node& n,
                             const std::string& course) {
    switch (n.kind) {
        case Node::TERM: {
            // quote for FTS phrase semantics; generous candidate limit
            std::string q = '"' + n.term + '"';
            std::vector<Window> w;
            for (const auto& seg : spine.corpusSearch(q, course, 2000))
                w.push_back({seg.course, seg.seq, seg.seq});
            return w;
        }
        case Node::OR: {
            auto a = evalNode(spine, *n.lhs, course);
            auto b = evalNode(spine, *n.rhs, course);
            a.insert(a.end(), b.begin(), b.end());
            return a;
        }
        case Node::NEAR: {
            auto a = evalNode(spine, *n.lhs, course);
            auto b = evalNode(spine, *n.rhs, course);
            // group rhs by course for the join
            std::map<std::string, std::vector<const Window*>> byCourse;
            for (const auto& w : b) byCourse[w.course].push_back(&w);
            std::vector<Window> out;
            for (const auto& wa : a) {
                auto it = byCourse.find(wa.course);
                if (it == byCourse.end()) continue;
                for (const auto* wb : it->second) {
                    const int gap = std::max({wb->lo - wa.hi, wa.lo - wb->hi, 0});
                    if (gap <= n.near_n)
                        out.push_back({wa.course, std::min(wa.lo, wb->lo),
                                       std::max(wa.hi, wb->hi)});
                }
            }
            return out;
        }
    }
    return {};
}

}  // namespace

std::vector<FileGoferHit> goferSearchFiles(const std::string& root_dir,
                                           const std::string& query, int limit,
                                           GoferScan* scan) {
    // SQA PERF-1. Matches LibraryIndex::kScanCap (libindex.h:76): the
    // two evaluators diverged once and that divergence WAS the bug.
    constexpr int kWindowCap = 200000;
    GoferScan local;
    GoferScan& sc = scan ? *scan : local;
    namespace fs = std::filesystem;
    auto toks = lex(query);
    Parser parser(toks);
    auto ast = parser.parseQuery();

    // load eligible files (path → lines); modest caps keep this predictable
    std::vector<std::pair<std::string, std::vector<std::string>>> files;
    const fs::path root(root_dir);
    if (!fs::exists(root)) throw std::runtime_error("no such directory: " + root_dir);
    for (auto it = fs::recursive_directory_iterator(
             root, fs::directory_options::skip_permission_denied);
         it != fs::recursive_directory_iterator(); ++it) {
        if (!it->is_regular_file()) continue;
        std::string ext = it->path().extension().string();
        for (auto& c : ext) c = (char)std::tolower((unsigned char)c);
        if (ext != ".txt" && ext != ".acip" && ext != ".md" &&
            ext != ".act" && ext != ".inc" && ext != ".ace") continue;
        if (it->file_size() > 10u * 1024 * 1024) continue;
        // PERF-1: eligible but past the cap - COUNT it and keep
        // walking. Breaking here is what made 4,988 files vanish
        // without the pane ever being able to say so.
        if ((int)files.size() >= sc.file_cap) { ++sc.files_skipped; continue; }
        std::ifstream f(it->path());
        if (!f) continue;
        std::vector<std::string> lines;
        std::string line;
        while (std::getline(f, line)) {
            if (!line.empty() && line.back() == '\r') line.pop_back();
            lines.push_back(line);
        }
        files.push_back({fs::relative(it->path(), root).string(), std::move(lines)});
    }
    sc.files_scanned = (int)files.size();

    auto upper = [](std::string s) {
        for (auto& c : s) c = (char)std::toupper((unsigned char)c);
        return s;
    };

    // evaluate the AST to windows of (file_ix, line_lo, line_hi), 0-based
    struct FWin { int file = 0; int lo = 0; int hi = 0; };
    std::function<std::vector<FWin>(const Node&)> eval =
        [&](const Node& n) -> std::vector<FWin> {
        switch (n.kind) {
            case Node::TERM: {
                const std::string needle = upper(n.term);
                std::vector<FWin> w;
                for (int fi = 0; fi < (int)files.size(); ++fi) {
                    const auto& lines = files[fi].second;
                    for (int li = 0; li < (int)lines.size(); ++li)
                        if (upper(lines[li]).find(needle) != std::string::npos)
                            w.push_back({fi, li, li});
                }
                return w;
            }
            case Node::OR: {
                auto a = eval(*n.lhs), b = eval(*n.rhs);
                a.insert(a.end(), b.begin(), b.end());
                return a;
            }
            case Node::NEAR: {
                auto a = eval(*n.lhs), b = eval(*n.rhs);
                std::map<int, std::vector<const FWin*>> byFile;
                for (const auto& w : b) byFile[w.file].push_back(&w);
                std::vector<FWin> out;
                for (const auto& wa : a) {
                    auto it2 = byFile.find(wa.file);
                    if (it2 == byFile.end()) continue;
                    if ((int)out.size() >= kWindowCap) { sc.cut = true; break; }
                    for (const auto* wb : it2->second) {
                        if ((int)out.size() >= kWindowCap) { sc.cut = true; break; }
                        const int gap = std::max({wb->lo - wa.hi, wa.lo - wb->hi, 0});
                        if (gap <= n.near_n)
                            out.push_back({wa.file, std::min(wa.lo, wb->lo),
                                           std::max(wa.hi, wb->hi)});
                    }
                }
                return out;
            }
        }
        return {};
    };
    auto wins = eval(*ast);
    std::sort(wins.begin(), wins.end(), [](const FWin& a, const FWin& b) {
        if (a.file != b.file) return a.file < b.file;
        if (a.lo != b.lo) return a.lo < b.lo;
        return a.hi < b.hi;
    });
    wins.erase(std::unique(wins.begin(), wins.end(),
                           [](const FWin& a, const FWin& b) {
                               return a.file == b.file && a.lo == b.lo &&
                                      a.hi == b.hi;
                           }),
               wins.end());

    std::vector<FileGoferHit> hits;
    for (const auto& w : wins) {
        if ((int)hits.size() >= limit) break;
        FileGoferHit h;
        h.file = files[w.file].first;
        h.line_lo = w.lo + 1;
        h.line_hi = w.hi + 1;
        for (int li = w.lo; li <= w.hi && li < (int)files[w.file].second.size(); ++li)
            h.lines.push_back(files[w.file].second[li]);
        hits.push_back(std::move(h));
    }
    return hits;
}

std::vector<GoferHit> goferSearch(const Spine& spine, const std::string& query,
                                  const std::string& course, int limit) {
    auto toks = lex(query);
    Parser p(toks);
    auto ast = p.parseQuery();
    auto windows = evalNode(spine, *ast, course);

    // dedupe + sort + merge identical windows
    std::sort(windows.begin(), windows.end(), [](const Window& a, const Window& b) {
        if (a.course != b.course) return a.course < b.course;
        if (a.lo != b.lo) return a.lo < b.lo;
        return a.hi < b.hi;
    });
    windows.erase(std::unique(windows.begin(), windows.end(),
                              [](const Window& a, const Window& b) {
                                  return a.course == b.course && a.lo == b.lo &&
                                         a.hi == b.hi;
                              }),
                  windows.end());

    std::vector<GoferHit> hits;
    for (const auto& w : windows) {
        if ((int)hits.size() >= limit) break;
        GoferHit h;
        h.course = w.course;
        h.seq_lo = w.lo;
        h.seq_hi = w.hi;
        h.window = spine.corpusWindow(w.course, w.lo, w.hi);
        hits.push_back(std::move(h));
    }
    return hits;
}

}  // namespace allcore
