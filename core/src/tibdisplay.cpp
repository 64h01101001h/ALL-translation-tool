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
    // A RUN at a time, not a token at a time. Converting each space-separated
    // token alone throws away the context the engine needs: a bare "," is a
    // shad and converts to nothing on its own, while "sangs rgyas, ,bcom ldan"
    // converts perfectly — and the shad alone accounted for 61% of all failures
    // in the first drill pack (1,945 of 3,178). Runs are split only at
    // editorial markup, which is the one thing that must never be converted.
    std::vector<DisplayPiece> out;
    std::string cur, mark;
    auto flushMark = [&] {
        if (mark.empty()) return;
        out.push_back({DisplayPiece::Markup, mark});
        mark.clear();
    };
    auto flush = [&] {
        if (cur.empty()) return;
        // trim, so a trailing space in a run does not become a failure
        size_t b = cur.find_first_not_of(' ');
        size_t e = cur.find_last_not_of(' ');
        if (b == std::string::npos) { cur.clear(); return; }
        const std::string run = cur.substr(b, e - b + 1);
        auto [u, ok] = wylieToUnicode(acipToEwts(run));
        if (ok && !u.empty())
            out.push_back({DisplayPiece::Script, u});
        else if (!u.empty())
            // The engine converts what it can and marks the rest in ⟨ ⟩ itself.
            // Keep its partial rendering: a run of good Tibetan must not be
            // thrown away because one syllable in it would not convert.
            out.push_back({DisplayPiece::Script, u});
        else
            out.push_back({DisplayPiece::Failed, run});
        cur.clear();
    };
    for (char c : acip) {
        if (isMarkupChar(c)) { flush(); mark += c; }
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
