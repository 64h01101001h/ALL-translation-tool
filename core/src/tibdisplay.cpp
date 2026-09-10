#include "allcore/tibdisplay.h"

#include "allcore/engines.h"

namespace allcore {
namespace {
bool isMarkupChar(char c) {
    return c == '{' || c == '}' || c == '(' || c == ')' || c == '[' ||
           c == ']' || c == '@' || c == '*' || c == '%' || c == '#';
}
}  // namespace

std::vector<DisplayPiece> acipDisplayPieces(const std::string& acip) {
    std::vector<DisplayPiece> out;
    std::string cur, mark;
    auto flushMark = [&] {
        if (mark.empty()) return;
        out.push_back({DisplayPiece::Markup, mark});
        mark.clear();
    };
    auto flush = [&] {
        if (cur.empty()) return;
        auto [u, ok] = wylieToUnicode(acipToEwts(cur));
        if (ok && !u.empty())
            out.push_back({DisplayPiece::Script, u + "་"});
        else
            out.push_back({DisplayPiece::Failed, cur});
        cur.clear();
    };
    for (char c : acip) {
        if (isMarkupChar(c)) { flush(); mark += c; }
        else if (c == ' ') { flush(); flushMark(); }
        else { flushMark(); cur += c; }
    }
    flush();
    flushMark();
    return out;
}

std::string acipToTibetanPlain(const std::string& acip) {
    std::string out;
    for (const auto& p : acipDisplayPieces(acip))
        out += p.kind == DisplayPiece::Failed
                   ? "⟨" + p.text + "⟩"
                   : p.text;
    return out;
}

}  // namespace allcore
