#include "allcore/outline.h"

#include <algorithm>
#include <utility>

namespace allcore {

namespace {

// cardinal numbers as they appear in division announcements ("la gsum ste")
int numberOf(const std::string& tok) {
    if (tok == "GNYIS") return 2;
    if (tok == "GSUM") return 3;
    if (tok == "BZHI") return 4;
    if (tok == "LNGA") return 5;
    if (tok == "DRUG") return 6;
    if (tok == "BDUN") return 7;
    if (tok == "BRGYAD") return 8;
    if (tok == "DGU") return 9;
    if (tok == "BCU") return 10;
    return 0;
}

// units for compound teens: bcu gcig = 11 … bcu dgu = 19
int unitOf(const std::string& tok) {
    if (tok == "GCIG") return 1;
    if (tok == "GNYIS") return 2;
    if (tok == "GSUM") return 3;
    if (tok == "BZHI") return 4;
    if (tok == "LNGA") return 5;
    if (tok == "DRUG") return 6;
    if (tok == "BDUN") return 7;
    if (tok == "BRGYAD") return 8;
    if (tok == "DGU") return 9;
    return 0;
}

// ordinals opening a part: dang po = 1st, gnyis pa = 2nd, …
int ordinalOf(const std::string& a, const std::string& b) {
    if (a == "DANG" && b == "PO") return 1;
    if (b != "PA") return 0;
    if (a == "GNYIS") return 2;
    if (a == "GSUM") return 3;
    if (a == "BZHI") return 4;
    if (a == "LNGA") return 5;
    if (a == "DRUG") return 6;
    if (a == "BDUN") return 7;
    if (a == "BRGYAD") return 8;
    if (a == "DGU") return 9;
    if (a == "BCU") return 10;
    return 0;
}

std::string joinTokens(const std::vector<std::string>& toks, int beg, int end) {
    std::string s;
    for (int t = beg; t < end && t < (int)toks.size(); ++t) {
        if (t > beg) s += ' ';
        s += toks[t];
    }
    return s;
}

}  // namespace

OutlineNode extractOutline(const std::vector<std::string>& tokens,
                           const std::vector<bool>& barrier_after) {
    OutlineNode root;
    // stack of open nodes, root at the bottom; stored as index paths into the
    // tree to survive vector reallocation
    std::vector<std::vector<int>> stack = {{}};   // {} = root path
    auto nodeAt = [&root](const std::vector<int>& path) -> OutlineNode* {
        OutlineNode* n = &root;
        for (int ix : path) n = &n->children[ix];
        return n;
    };
    const int n = (int)tokens.size();
    auto clauseEnd = [&](int from) {
        int e = from;
        while (e < n && !(e > 0 && barrier_after[e - 1])) ++e;
        return e;
    };
    auto clauseStart = [&](int from) {
        int b = from;
        while (b > 0 && !barrier_after[b - 1]) --b;
        return b;
    };

    for (int i = 0; i < n; ++i) {
        // ---- division announcement: [LA] <number> STE/TE ----
        if (i + 1 < n) {
            int num = numberOf(tokens[i]);
            int numLen = 1;
            // compound teens: LA BCU GCIG STE = "has eleven parts"
            if (tokens[i] == "BCU" && i + 2 < n && unitOf(tokens[i + 1])) {
                num = 10 + unitOf(tokens[i + 1]);
                numLen = 2;
            }
            const bool ste = i + numLen < n && (tokens[i + numLen] == "STE" ||
                                                tokens[i + numLen] == "TE");
            const bool la = i > 0 && tokens[i - 1] == "LA";
            if (num >= 2 && ste && la) {
                OutlineNode* cur = nodeAt(stack.back());
                if (cur->announced == 0) {
                    cur->announced = num;
                    if (cur->heading.empty())
                        cur->heading = joinTokens(tokens, clauseStart(i), i - 1);
                    if (cur->tok < 0) cur->tok = clauseStart(i);
                }
                continue;
            }
        }
        // ---- ordinal opener: <a b> (or teen <bcu unit pa>) at clause start,
        //      or followed by NI/LA ----
        if (i + 1 < n) {
            int k = ordinalOf(tokens[i], tokens[i + 1]);
            int ordLen = 2;
            if (tokens[i] == "BCU" && i + 2 < n && unitOf(tokens[i + 1]) &&
                tokens[i + 2] == "PA") {
                k = 10 + unitOf(tokens[i + 1]);
                ordLen = 3;
            }
            if (k > 0) {
                const bool clauseInitial = (i == 0) || barrier_after[i - 1];
                const bool marked =
                    i + ordLen < n && (tokens[i + ordLen] == "NI" ||
                                       tokens[i + ordLen] == "LA");
                if (!clauseInitial && !marked) continue;
                // find the innermost open node whose NEXT child is exactly k
                int depth = -1;
                bool irregular = false;
                for (int d = (int)stack.size() - 1; d >= 0; --d) {
                    OutlineNode* cand = nodeAt(stack[d]);
                    if ((int)cand->children.size() + 1 == k &&
                        (cand->announced == 0 || k <= cand->announced)) {
                        depth = d;
                        break;
                    }
                }
                if (depth < 0) {
                    // tolerate a skip: nearest frame that announced >= k
                    for (int d = (int)stack.size() - 1; d >= 0; --d) {
                        if (nodeAt(stack[d])->announced >= k) {
                            depth = d;
                            irregular = true;
                            break;
                        }
                    }
                }
                if (depth < 0) { depth = 0; irregular = true; }
                stack.resize(depth + 1);
                OutlineNode* parent = nodeAt(stack.back());
                OutlineNode child;
                child.label = (parent->label.empty() ? "" : parent->label + ".") +
                              std::to_string(k);
                child.heading = joinTokens(tokens, i, clauseEnd(i + ordLen));
                child.tok = i;
                child.irregular = irregular;
                parent->children.push_back(std::move(child));
                auto path = stack.back();
                path.push_back((int)parent->children.size() - 1);
                stack.push_back(std::move(path));
                i += ordLen - 1;   // consume the remaining ordinal tokens
            }
        }
    }
    return root;
}

namespace {

// uppercase (marker files come in both cases) and strip the slar-bsdu
// final 'O written on closing ordinals: PO'O → PO, PA'O → PA
std::string upstrip(std::string t) {
    for (char& c : t)
        if (c >= 'a' && c <= 'z') c -= 32;
    if (t.size() > 2 && t.compare(t.size() - 2, 2, "'O") == 0)
        t.erase(t.size() - 2);
    return t;
}

// decade word pairs in compound ordinals (nyi shu / sum cu …)
int decadeOf(const std::string& a, const std::string& b) {
    if (a == "NYI" && b == "SHU") return 20;
    if (a == "SUM" && (b == "CU" || b == "BCU")) return 30;
    if (a == "BZHI" && b == "BCU") return 40;
    if (a == "LNGA" && b == "BCU") return 50;
    if (a == "DRUG" && (b == "CU" || b == "BCU")) return 60;
    if (a == "BDUN" && (b == "CU" || b == "BCU")) return 70;
    if (a == "BRGYAD" && (b == "CU" || b == "BCU")) return 80;
    if (a == "DGU" && b == "BCU") return 90;
    return 0;
}

// per-decade connector particles, which the canon also uses ALONE as
// decade contractions ("bam po zhe gsum" = 43, KL4568 dkar chag)
int connectorDecade(const std::string& t) {
    if (t == "SO") return 30;
    if (t == "ZHE") return 40;
    if (t == "NGA") return 50;
    if (t == "RE") return 60;
    if (t == "DON") return 70;
    if (t == "GYA") return 80;
    if (t == "GO") return 90;
    return 0;
}

// slar-bsdu echo written as its own token after a bare number
// ("bam po gcig go", KL0021)
bool slarBsduTok(const std::string& t) {
    return t == "GO" || t == "NGO" || t == "'O" || t == "TO" ||
           t == "DO" || t == "NO" || t == "BO" || t == "MO" ||
           t == "RO" || t == "LO" || t == "SO";
}

// parse a marker ordinal starting at tokens[j] (clause-bounded at `end`),
// covering the forms attested in the ACIP canon files: dang po / N pa /
// bare N / bcu N / bco lnga(brgyad) / decade pairs with and without
// rtsa-style connectors / nyer contractions / connector-decades /
// brgya dang N. Returns {value, tokens consumed}; {0, 0} = no parse
// (caller keeps the raw text — rule 3, never guess).
std::pair<int, int> parseMarkerOrdinal(const std::vector<std::string>& toks,
                                       int j, int end) {
    auto at = [&](int k) {
        return k < end ? upstrip(toks[k]) : std::string();
    };
    // optional trailing PA/PO after k tokens of a value
    auto withPa = [&](int val, int j0, int used) -> std::pair<int, int> {
        const std::string nxt = at(j0 + used);
        if (nxt == "PA" || nxt == "PO") return {val, used + 1};
        return {val, used};
    };
    const std::string a = at(j), b = at(j + 1);
    if (a.empty()) return {0, 0};
    if (a == "DANG" && b == "PO") return {1, 2};            // dang po = 1st
    if (a == "BCO" && (b == "LNGA" || b == "BRGYAD"))       // bco lnga = 15
        return withPa(b == "LNGA" ? 15 : 18, j, 2);
    if (a == "BCU") {                                       // bcu [N] [pa]
        if (int u = unitOf(b)) return withPa(10 + u, j, 2);
        return withPa(10, j, 1);
    }
    if (int d = decadeOf(a, b)) {                           // nyi shu … sum cu …
        int k = j + 2;
        const std::string c = at(k);
        if (c == "RTSA" || connectorDecade(c))              // nyi shu rtsa gcig
            if (int u = unitOf(at(k + 1))) return withPa(d + u, j, 4);
        if (int u = unitOf(c)) return withPa(d + u, j, 3);  // nyi shu gcig
        return withPa(d, j, 2);                             // nyi shu [pa]
    }
    if (a == "NYER") {                                      // nyer gnyis = 22
        if (int u = unitOf(b)) return withPa(20 + u, j, 2);
        return withPa(20, j, 1);
    }
    if (int d = connectorDecade(a))                         // zhe gsum = 43
        if (int u = unitOf(b)) return withPa(d + u, j, 2);
    if (a == "BRGYA") {                                     // brgya [dang N]
        if (b == "DANG") {
            auto [v, used] = parseMarkerOrdinal(toks, j + 2, end);
            if (v > 0 && v < 100) return {100 + v, 2 + used};
        }
        return withPa(100, j, 1);
    }
    if (int u = unitOf(a)) {                                // gcig [pa|go] …
        if (b == "PA" || b == "PO") return {u, 2};
        if (slarBsduTok(b)) return {u, 2};                  // gcig go = 1
        return {u, 1};                                      // bare "drug"
    }
    return {0, 0};
}

}  // namespace

TextStructure extractStructure(const std::vector<std::string>& tokens,
                               const std::vector<bool>& barrier_after) {
    TextStructure st;
    const int n = (int)tokens.size();
    // a text syllable: has letters, is not a page/folio reference
    // (@012A style) or numbered token
    auto isSyllable = [](const std::string& t) {
        bool alpha = false;
        for (char c : t) {
            if (c == '@' || (c >= '0' && c <= '9')) return false;
            if ((c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z')) alpha = true;
        }
        return alpha;
    };
    // clause window from `from` to the next barrier (markers are often
    // clause-initial, so the boundary BEFORE `from` must not stop the scan)
    auto clauseEnd = [&](int from) {
        int e = from;
        while (e < n && !(e > from && barrier_after[e - 1])) ++e;
        return e;
    };
    for (int t = 0; t < n; ++t)
        if (isSyllable(tokens[t])) ++st.syllables;

    int prevBampo = 0, prevChapter = 0;
    for (int t = 0; t < n; ++t) {
        // BAM PO markers (BAM and PO must share a clause; both cases —
        // the canon files come uppercase and lowercase)
        if (t + 1 < n && upstrip(tokens[t]) == "BAM" &&
            upstrip(tokens[t + 1]) == "PO" && !barrier_after[t]) {
            const int end = clauseEnd(t);
            StructMarker m;
            m.tok = t;
            auto [val, used] = parseMarkerOrdinal(tokens, t + 2, end);
            m.number = val;
            m.label = joinTokens(tokens, t,
                                 std::min(end, t + 2 + (used ? used : 3)));
            if (val > 0) {
                m.irregular = prevBampo > 0 && val != prevBampo + 1;
                prevBampo = val;
            }
            st.bampos.push_back(std::move(m));
            t += 1 + used;
            continue;
        }
        // LE'U chapter markers — recorded only with a parsed ordinal
        // (bare le'u mentions in prose are references, not divisions)
        if (upstrip(tokens[t]) == "LE'U") {
            const int end = clauseEnd(t);
            int j = t + 1;
            if (j < end && (upstrip(tokens[j]) == "STE" ||
                            upstrip(tokens[j]) == "TE")) ++j;
            auto [val, used] = parseMarkerOrdinal(tokens, j, end);
            if (val > 0) {
                StructMarker m;
                m.tok = t;
                m.number = val;
                m.label = joinTokens(tokens, t, j + used);
                m.irregular = prevChapter > 0 && val != prevChapter + 1;
                prevChapter = val;
                st.chapters.push_back(std::move(m));
                t = j + used - 1;
            }
        }
    }

    // per-bampo syllable spans (marker → next marker / end of text)
    for (size_t i = 0; i < st.bampos.size(); ++i) {
        const int beg = st.bampos[i].tok;
        const int end2 =
            i + 1 < st.bampos.size() ? st.bampos[i + 1].tok : n;
        long s = 0;
        for (int t = beg; t < end2; ++t)
            if (isSyllable(tokens[t])) ++s;
        st.bampos[i].syllables = s;
    }
    if (!st.bampos.empty()) {
        long s = 0;
        for (int t = 0; t < st.bampos[0].tok; ++t)
            if (isSyllable(tokens[t])) ++s;
        st.preamble_syllables = s;
    }
    return st;
}

}  // namespace allcore
