// gofer_ast.h — the Gofer query lexer/parser, shared between the corpus/file
// evaluators (gofer.cpp) and the library index (libindex.cpp). INTERNAL to
// allcore — not installed in include/.
#pragma once

#include <cctype>
#include <cstdlib>
#include <memory>
#include <stdexcept>
#include <string>
#include <vector>

namespace allcore {
namespace gofer_ast {

struct Tok {
    enum Kind { TERM, OR, NEAR, LPAR, RPAR, END } kind = END;
    std::string text;      // TERM payload
    int near_n = 3;        // NEAR window
};

inline std::vector<Tok> lex(const std::string& q) {
    std::vector<Tok> toks;
    size_t i = 0;
    auto isBare = [](char c) {
        return std::isalnum((unsigned char)c) || c == '\'' || c == '+' ||
               c == '-' || c == '.' || c == '@';
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
        size_t j = i;
        while (j < q.size() && isBare(q[j])) ++j;
        if (j == i)
            throw std::runtime_error(std::string("unexpected character '") + c +
                                     "'");
        std::string w = q.substr(i, j - i);
        std::string upper = w;
        for (auto& ch : upper) ch = (char)std::toupper((unsigned char)ch);
        if (upper == "OR") {
            toks.push_back({Tok::OR});
        } else if (upper.rfind("NEAR", 0) == 0) {
            Tok t{Tok::NEAR};
            if (upper.size() > 4 && (upper[4] == '/' || upper[4] == '-'))
                t.near_n = std::max(0, std::atoi(upper.c_str() + 5));
            toks.push_back(t);
        } else {
            Tok t{Tok::TERM};
            t.text = w;
            toks.push_back(t);
        }
        i = j;
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
        if (t[i].kind != Tok::END)
            throw std::runtime_error("trailing input in query");
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
        if (t[i].kind != Tok::TERM)
            throw std::runtime_error("expected a search term");
        auto n = std::make_unique<Node>();
        n->kind = Node::TERM;
        n->term = t[i].text;
        ++i;
        return n;
    }
};

}  // namespace gofer_ast
}  // namespace allcore
