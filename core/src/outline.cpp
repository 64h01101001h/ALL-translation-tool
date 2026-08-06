#include "allcore/outline.h"

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

}  // namespace allcore
