#include "allcore/verse.h"

#include <map>

#include "allcore/lattice.h"

namespace allcore {

VerseReport analyzeVerse(const std::string& acip) {
    VerseReport rep;
    // split into candidate lines at newlines and shad/comma
    std::vector<std::string> raw;
    std::string cur;
    for (char c : acip + "\n") {
        if (c == '\n' || c == '\r' || c == ',' || c == ';' || c == '/') {
            raw.push_back(cur);
            cur.clear();
        } else {
            cur += c;
        }
    }
    int lineNo = 0;
    for (const auto& r : raw) {
        std::vector<std::string> toks;
        std::vector<bool> bars;
        tokenizeDocument(r, toks, bars);
        if (toks.empty()) continue;
        VerseLine l;
        l.number = ++lineNo;
        l.syllables = (int)toks.size();
        for (size_t i = 0; i < toks.size(); ++i) {
            if (i) l.text += ' ';
            l.text += toks[i];
        }
        rep.lines.push_back(std::move(l));
    }
    if (rep.lines.size() < 4) return rep;   // too short to call it verse
    std::map<int, int> counts;
    for (const auto& l : rep.lines) ++counts[l.syllables];
    int best = 0, bestN = 0;
    for (auto& [len, n] : counts)
        if (n > bestN) { best = len; bestN = n; }
    // verse when the dominant count covers at least 70% of the lines and the
    // lines are long enough to be metrical
    if (best >= 5 && bestN * 10 >= (int)rep.lines.size() * 7) {
        rep.is_verse = true;
        rep.meter = best;
        for (auto& l : rep.lines) {
            l.irregular = (l.syllables != best);
            rep.irregular_count += l.irregular;
        }
    }
    return rep;
}

std::vector<VerseStanza> groupStanzas(const VerseReport& report,
                                      const std::string& raw_acip) {
    std::vector<VerseStanza> out;
    if (report.lines.empty()) return out;

    // does a raw source line close a group with a double shad? "//"
    // or the unicode ༎ (U+0F0E) at the trimmed line end
    auto closesStanza = [](const std::string& t) {
        std::string s = t;
        while (!s.empty() &&
               (s.back() == ' ' || s.back() == '\t' || s.back() == '\r'))
            s.pop_back();
        while (!s.empty() && (s.back() == ',' || s.back() == ';'))
            s.pop_back();
        if (s.size() >= 2 && s.compare(s.size() - 2, 2, "//") == 0)
            return true;
        if (s.size() >= 3 &&
            (unsigned char)s[s.size() - 3] == 0xE0 &&
            (unsigned char)s[s.size() - 2] == 0xBC &&
            (unsigned char)s[s.size() - 1] == 0x8E)
            return true;
        return false;
    };
    // the report drops trailing shads, so read boundaries off the raw
    // lines, indexed to match the report's cleaned lines in order
    std::vector<std::string> rawLines;
    {
        std::string cur;
        for (char c : raw_acip) {
            if (c == '\n') { rawLines.push_back(cur); cur.clear(); }
            else cur += c;
        }
        if (!cur.empty()) rawLines.push_back(cur);
    }
    bool anyClose = false;
    for (const auto& r : rawLines) anyClose |= closesStanza(r);

    auto push = [&](int lo, int hi) {
        VerseStanza st;
        st.first_line = report.lines[lo].number;
        st.last_line = report.lines[hi].number;
        st.regular = true;
        for (int k = lo; k <= hi; ++k) {
            st.lines.push_back(report.lines[k].text);
            if (report.meter && report.lines[k].syllables != report.meter)
                st.regular = false;
        }
        out.push_back(std::move(st));
    };

    const int n = (int)report.lines.size();
    // pair each report line with its raw line: analyzeVerse keeps only
    // non-empty lines, so walk the raw lines skipping blanks in step
    std::vector<bool> closes(n, false);
    {
        int ri = 0;
        for (int i = 0; i < n; ++i) {
            while (ri < (int)rawLines.size()) {
                std::string t = rawLines[ri];
                bool blank = t.find_first_not_of(" \t\r") == std::string::npos;
                if (!blank) break;
                ++ri;
            }
            if (ri < (int)rawLines.size()) {
                closes[i] = closesStanza(rawLines[ri]);
                ++ri;
            }
        }
    }
    if (anyClose) {
        // group by shad-closed runs (the poet's own boundaries)
        int start = 0;
        for (int i = 0; i < n; ++i) {
            if (closes[i]) {
                push(start, i);
                start = i + 1;
            }
        }
        if (start < n) push(start, n - 1);
    } else {
        // no explicit boundaries: the natural shloka is four lines
        for (int i = 0; i < n; i += 4)
            push(i, std::min(i + 3, n - 1));
    }
    return out;
}

}  // namespace allcore
