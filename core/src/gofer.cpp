#include "allcore/gofer.h"

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

// ---------------- lexer ----------------
struct Tok {
    enum Kind { TERM, OR, NEAR, LPAR, RPAR, END } kind = END;
    std::string text;      // TERM payload
    int near_n = 3;        // NEAR window
};

std::vector<Tok> lex(const std::string& q) {
    std::vector<Tok> toks;
    size_t i = 0;
    auto isBare = [](char c) {
        return std::isalnum((unsigned char)c) || c == '\'' || c == '+' || c == '-' ||
               c == '.' || c == '@';
    };
    while (i < q.size()) {
        char c = q[i];
        if (std::isspace((unsigned char)c)) { ++i; continue; }
        if (c == '(') { toks.push_back({Tok::LPAR}); ++i; continue; }
        if (c == ')') { toks.push_back({Tok::RPAR}); ++i; continue; }
        if (c == '"') {
            size_t j = q.find('"', i + 1);
            if (j == std::string::npos) throw std::runtime_error("unclosed quote");
            Tok t{Tok::TERM};
            t.text = q.substr(i + 1, j - i - 1);
            toks.push_back(t);
            i = j + 1;
            continue;
        }
        // word
        size_t j = i;
        while (j < q.size() && isBare(q[j])) ++j;
        if (j == i) throw std::runtime_error(std::string("unexpected character '") + c + "'");
        std::string w = q.substr(i, j - i);
        std::string upper = w;
        for (auto& ch : upper) ch = (char)std::toupper((unsigned char)ch);
        if (upper == "OR") {
            toks.push_back({Tok::OR});
        } else if (upper.rfind("NEAR", 0) == 0) {
            Tok t{Tok::NEAR};
            if (upper.size() > 4 && (upper[4] == '/' || upper[4] == '-'))
                t.near_n = std::max(0, std::atoi(upper.c_str() + 5));
            else if (j < q.size() && q[j] == '/') {   // "NEAR/5" split by lexer? no—handled above
                t.near_n = 3;
            }
            toks.push_back(t);
        } else {
            // bare words run together into one phrase term until an operator
            Tok t{Tok::TERM};
            t.text = w;
            toks.push_back(t);
        }
        i = j;
        // NEAR/5: '/' is not a bare char, absorb "/N" here
        if (!toks.empty() && toks.back().kind == Tok::NEAR && i < q.size() &&
            q[i] == '/') {
            size_t k = i + 1, s = k;
            while (k < q.size() && std::isdigit((unsigned char)q[k])) ++k;
            if (k > s) toks.back().near_n = std::atoi(q.substr(s, k - s).c_str());
            i = k;
        }
    }
    // coalesce adjacent bare TERMs into phrases ("bden pa" typed unquoted)
    std::vector<Tok> out;
    for (auto& t : toks) {
        if (t.kind == Tok::TERM && !out.empty() && out.back().kind == Tok::TERM)
            out.back().text += " " + t.text;
        else
            out.push_back(t);
    }
    out.push_back({Tok::END});
    return out;
}

// ---------------- AST + parser ----------------
struct Node {
    enum Kind { TERM, OR, NEAR } kind = TERM;
    std::string term;
    int near_n = 3;
    std::unique_ptr<Node> lhs, rhs;
};

struct Parser {
    const std::vector<Tok>& t;
    size_t i = 0;
    explicit Parser(const std::vector<Tok>& toks) : t(toks) {}

    std::unique_ptr<Node> parseQuery() {
        auto n = parseOr();
        if (t[i].kind != Tok::END) throw std::runtime_error("trailing input in query");
        return n;
    }
    std::unique_ptr<Node> parseOr() {
        auto lhs = parseNear();
        while (t[i].kind == Tok::OR) {
            ++i;
            auto n = std::make_unique<Node>();
            n->kind = Node::OR;
            n->lhs = std::move(lhs);
            n->rhs = parseNear();
            lhs = std::move(n);
        }
        return lhs;
    }
    std::unique_ptr<Node> parseNear() {
        auto lhs = parseTerm();
        while (t[i].kind == Tok::NEAR) {
            int nn = t[i].near_n;
            ++i;
            auto n = std::make_unique<Node>();
            n->kind = Node::NEAR;
            n->near_n = nn;
            n->lhs = std::move(lhs);
            n->rhs = parseTerm();
            lhs = std::move(n);
        }
        return lhs;
    }
    std::unique_ptr<Node> parseTerm() {
        if (t[i].kind == Tok::LPAR) {
            ++i;
            auto n = parseOr();
            if (t[i].kind != Tok::RPAR) throw std::runtime_error("missing ')'");
            ++i;
            return n;
        }
        if (t[i].kind != Tok::TERM) throw std::runtime_error("expected a search term");
        auto n = std::make_unique<Node>();
        n->kind = Node::TERM;
        n->term = t[i].text;
        ++i;
        return n;
    }
};

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
                                           const std::string& query, int limit) {
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
        if (ext != ".txt" && ext != ".acip" && ext != ".md") continue;
        if (it->file_size() > 10u * 1024 * 1024) continue;
        std::ifstream f(it->path());
        if (!f) continue;
        std::vector<std::string> lines;
        std::string line;
        while (std::getline(f, line)) {
            if (!line.empty() && line.back() == '\r') line.pop_back();
            lines.push_back(line);
        }
        files.push_back({fs::relative(it->path(), root).string(), std::move(lines)});
        if (files.size() >= 4000) break;
    }

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
                    for (const auto* wb : it2->second) {
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
