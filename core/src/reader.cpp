#include "allcore/reader.h"

#include "allcore/particles.h"

namespace allcore {

namespace {

// Clause-level connective particles (conservative v1 set — forms that are
// reliably clause boundaries when they appear as their own syllable).
// Deliberately absent: de (demonstrative), yang (often lexical/adverbial),
// nas/las (usually the noun-phrase ablative), gyi/kyi adversative (ambiguous
// with the genitive).
struct Connective {
    const char* acip;
    const char* wylie;
    const char* function;
};

const Connective kConnectives[] = {
    {"STE", "ste", "semi-final — \"…and/thus,\" links to what follows"},
    {"TE", "te", "semi-final — \"…and/thus,\" links to what follows"},
    {"NA", "na", "conditional/locative — \"if/when…\""},
    {"KYANG", "kyang", "concessive — \"although/even…\""},
    {"CING", "cing", "coordinating — \"…and, while\""},
    {"ZHING", "zhing", "coordinating — \"…and, while\""},
    {"SHING", "shing", "coordinating — \"…and, while\""},
    {"CES", "ces", "quotative — closes the quotation"},
    {"ZHES", "zhes", "quotative — closes the quotation"},
    {"SHES", "shes", "quotative — closes the quotation"},
};

// suffix-echo declarative finals: previous suffix letter + o
struct Echo {
    const char* tok;      // the particle token
    const char* licenses; // required final of the previous syllable; "" = vowel
};

const Echo kEchoes[] = {
    {"GO", "G"}, {"NGO", "NG"}, {"DO", "D"}, {"NO", "N"}, {"BO", "B"},
    {"MO", "M"}, {"'O", ""},    {"RO", "R"}, {"LO", "L"}, {"SO", "S"},
    {"TO", "D"},  // to after da (and historical da-drag)
};

bool endsWith(const std::string& s, const std::string& suf) {
    return s.size() >= suf.size() &&
           s.compare(s.size() - suf.size(), suf.size(), suf) == 0;
}

bool vowelFinal(const std::string& s) {
    if (s.empty()) return false;
    const char c = s.back();
    return c == 'A' || c == 'E' || c == 'I' || c == 'O' || c == 'U' || c == '\'';
}

}  // namespace

std::string expectedAllomorph(const std::string& prev_acip,
                              const std::string& tok_acip) {
    // family membership
    bool quot = (tok_acip == "CES" || tok_acip == "ZHES" ||
                 tok_acip == "SHES");
    bool coord = (tok_acip == "CING" || tok_acip == "ZHING" ||
                  tok_acip == "SHING");
    if (!quot && !coord) return "";
    if (prev_acip.empty()) return "";
    const char f = prev_acip.back();
    const char* form;
    if (f == 'G' || f == 'D' || f == 'B')
        form = quot ? "CES" : "CING";
    else if (f == 'S')
        form = quot ? "SHES" : "SHING";
    else   // ng n m ' r l and vowels all license the zh- form
        form = quot ? "ZHES" : "ZHING";
    return form;
}

int connectiveAllomorphCheck(const std::string& prev_acip,
                             const std::string& tok_acip) {
    const std::string want = expectedAllomorph(prev_acip, tok_acip);
    if (want.empty()) return -1;
    return want == tok_acip ? 0 : 1;
}

bool isFinalParticle(const std::string& prev_acip, const std::string& tok_acip) {
    for (const auto& e : kEchoes) {
        if (tok_acip != e.tok) continue;
        if (e.licenses[0] == '\0') return vowelFinal(prev_acip);
        // NG must not be read as a bare G license
        if (std::string(e.licenses) == "G" && endsWith(prev_acip, "NG"))
            return false;
        return endsWith(prev_acip, e.licenses);
    }
    return false;
}

std::vector<Clause> splitClauses(const std::vector<std::string>& tokens,
                                 const std::vector<bool>& barrier_after) {
    std::vector<Clause> out;
    const int n = (int)tokens.size();
    int beg = 0;
    auto close = [&](int endTok, std::string boundary, const char* fn) {
        if (endTok > beg)
            out.push_back({beg, endTok, std::move(boundary), fn});
        beg = endTok;
    };
    for (int i = 0; i < n; ++i) {
        bool closed = false;
        // clause-level connective as its own syllable (not clause-initial)
        if (i > beg) {
            for (const auto& c : kConnectives) {
                if (tokens[i] == c.acip) {
                    close(i + 1, c.wylie, c.function);
                    closed = true;
                    break;
                }
            }
            if (!closed && isFinalParticle(tokens[i - 1], tokens[i])) {
                std::string w = tokens[i];
                for (auto& ch : w)
                    if (ch >= 'A' && ch <= 'Z') ch = (char)(ch - 'A' + 'a');
                close(i + 1, w, "final declarative — ends the statement");
                closed = true;
            }
        }
        // punctuation barrier after this token (no-op if a connective already
        // closed the clause exactly here — close() drops empty spans)
        if (barrier_after[i]) close(i + 1, "barrier", "punctuation (shad/comma/line)");
        (void)closed;
    }
    close(n, "", "end of text");
    return out;
}

// ---- increment 2: chunks, verb, reading order ------------------------------

namespace {

// Closed-class verb forms (rule data, like the particle tables): copulas and
// auxiliaries that carry neither tense forms nor "to …" glosses in the
// dictionary (med, min, dgos), plus the debate-formula verbs whose na clause
// is conditional (zhe na / zer na / 'dod na — "if one asks/says/asserts").
bool isCopula(const std::string& wylie) {
    static const char* K[] = {"yin", "min", "yod",  "med",  "lags", "'dug",
                              "dgos", "nus", "zhe", "zer", "'dod"};
    for (const char* k : K)
        if (wylie == k) return true;
    return false;
}

std::string asciiLower(const std::string& s) {
    std::string out = s;
    for (auto& c : out)
        if (c >= 'A' && c <= 'Z') c = (char)(c - 'A' + 'a');
    return out;
}

// HGM verb evidence at a token: any covering span whose entry carries tense
// forms, a "to …" gloss, is a closed-class copula/auxiliary, or sits in
// Wilson's verb-class tables.
bool verbEvidenceAt(const allcore::OverlayDoc& doc, int tok) {
    for (int ix : doc.spansAt(tok)) {
        const auto& e = doc.entries[doc.spans[ix].entry_ix];
        if (!e.tenses.empty()) return true;
        if (isCopula(e.wylie)) return true;
        if (classifyVerbWithTenses(e.wylie, e.tenses)) return true;
        for (const auto& gl : e.hgm_gloss)
            if (gl.rfind("to ", 0) == 0) return true;
    }
    // no span (word missing from the dictionary): the token itself may still
    // be a copula — these forms map to wylie by lowercasing
    return isCopula(asciiLower(doc.tokens[tok]));
}

}  // namespace

std::vector<Clause> refineClauses(const OverlayDoc& doc,
                                  std::vector<Clause> clauses) {
    std::vector<Clause> out;
    for (size_t i = 0; i < clauses.size(); ++i) {
        Clause c = clauses[i];
        while (c.boundary == "na" && i + 1 < clauses.size()) {
            const int pre = c.end - 2;   // the word na attaches to
            // conditional after a verb; ALSO after the nominalizer pa/ba —
            // "…pa na" is the temporal clause "when …"
            if (pre >= c.beg &&
                (verbEvidenceAt(doc, pre) || doc.tokens[pre] == "PA" ||
                 doc.tokens[pre] == "BA"))
                break;
            const Clause& nxt = clauses[++i];                     // locative
            c.end = nxt.end;
            c.boundary = nxt.boundary;
            c.boundary_function = nxt.boundary_function;
        }
        out.push_back(c);
    }
    return out;
}

namespace {

struct RoleMarker {
    const char* acip;
    const char* wylie;
    const char* role;
};

// Freestanding case particles that close a phrase chunk. (na is normally
// consumed by splitClauses; it appears here for clauses where it survives.)
const RoleMarker kRoleMarkers[] = {
    {"GI", "gi", "connector — \"of\": reads with the NEXT chunk, right to left"},
    {"KYI", "kyi", "connector — \"of\": reads with the NEXT chunk, right to left"},
    {"GYI", "gyi", "connector — \"of\": reads with the NEXT chunk, right to left"},
    {"YI", "yi", "connector — \"of\": reads with the NEXT chunk, right to left"},
    {"GIS", "gis", "agent/instrument — \"by/with\""},
    {"KYIS", "kyis", "agent/instrument — \"by/with\""},
    {"GYIS", "gyis", "agent/instrument — \"by/with\""},
    {"YIS", "yis", "agent/instrument — \"by/with\""},
    {"LA", "la", "la don — \"to/at/in/for\""},
    {"DU", "du", "la don — \"to/at/as\""},
    {"RU", "ru", "la don — \"to/at/as\""},
    {"SU", "su", "la don — \"to/at/as\""},
    {"TU", "tu", "la don — \"to/at/as\""},
    {"NA", "na", "locative/conditional — \"in/at; if\""},
    {"NAS", "nas", "source — \"from/through\""},
    {"LAS", "las", "source — \"from\"; comparative \"than\""},
    {"NI", "ni", "topic — \"as for …\""},
    {"DANG", "dang", "conjunctive — \"and/with\": links to the NEXT chunk"},
};

const char* fusedRole(const std::string& ending) {
    if (ending == "'i") return "connector — \"of\": reads with the NEXT chunk, right to left";
    if (ending == "s") return "agent/instrument — \"by/with\"";
    if (ending == "r") return "la don — \"to/at/in\"";
    if (ending == "'am") return "alternative — \"or\"";
    if (ending == "'ang") return "concessive — \"also/even\"";
    return "";
}

bool attachesForward(const std::string& marker) {
    return marker == "gi" || marker == "kyi" || marker == "gyi" ||
           marker == "yi" || marker == "'i" || marker == "dang";
}

// innermost span ending exactly at token end with a fused-ending clitic
const allcore::OverlaySpan* fusedSpanEndingAt(const allcore::OverlayDoc& doc,
                                              int end_tok) {
    const allcore::OverlaySpan* best = nullptr;
    for (const auto& s : doc.spans) {
        if (s.end != end_tok || s.clitic.empty()) continue;
        if (!best || (s.end - s.beg) < (best->end - best->beg)) best = &s;
    }
    return best;
}

}  // namespace

std::vector<Chunk> chunkClause(const OverlayDoc& doc, const Clause& clause) {
    std::vector<Chunk> out;
    int beg = clause.beg;
    // the clause's own boundary particle is not part of any chunk
    int last = clause.end;
    if (clause.boundary != "" && clause.boundary != "barrier") --last;
    for (int i = beg; i < last; ++i) {
        const std::string& tok = doc.tokens[i];
        std::string marker;
        bool fused = false;
        const char* role = "";
        for (const auto& m : kRoleMarkers) {
            if (tok == m.acip) { marker = m.wylie; role = m.role; break; }
        }
        if (marker.empty()) {
            if (const auto* sp = fusedSpanEndingAt(doc, i + 1)) {
                const char* r = fusedRole(sp->clitic);
                if (r[0] != '\0') { marker = sp->clitic; fused = true; role = r; }
            }
        }
        if (!marker.empty() && i + 1 <= last) {
            out.push_back({beg, i + 1, marker, fused, role});
            beg = i + 1;
        }
    }
    if (beg < last)
        out.push_back({beg, last, "", false,
                       "unmarked — object/complement, or the predicate"});
    if (!out.empty() && out.back().marker.empty())
        out.back().role = "predicate slot — expect the verb here";
    return out;
}

VerbGuess spotVerb(const OverlayDoc& doc, const std::vector<Chunk>& chunks) {
    VerbGuess g;
    if (chunks.empty()) return g;
    auto found = [](int c, int tok, const Entry& e, std::string ev,
                    bool conf) {
        VerbGuess v{c, tok, e.wylie, std::move(ev), conf,
                    classifyVerbWithTenses(e.wylie, e.tenses)};
        return v;
    };
    // only the final chunk is searched for a confident verb, right-to-left
    {
        const int c = (int)chunks.size() - 1;
        for (int t = chunks[c].end - 1; t >= chunks[c].beg; --t) {
            for (int ix : doc.spansAt(t)) {
                const auto& span = doc.spans[ix];
                const auto& e = doc.entries[span.entry_ix];
                if (!e.tenses.empty())
                    return found(c, span.beg, e, "tenses: " + e.tenses, true);
                if (isCopula(e.wylie))
                    return found(c, span.beg, e,
                                 "closed-class verb (rule table)", true);
                if (classifyVerbWithTenses(e.wylie, e.tenses))
                    return found(c, span.beg, e, "Wilson verb-class table",
                                 true);
                for (const auto& gl : e.hgm_gloss)
                    if (gl.rfind("to ", 0) == 0)
                        return found(c, span.beg, e, "gloss: " + gl, true);
            }
        }
    }
    // no HGM verb evidence: report the last dictionary word, unverified
    const auto& lastChunk = chunks.back();
    for (int t = lastChunk.end - 1; t >= lastChunk.beg; --t) {
        auto at = doc.spansAt(t);
        if (!at.empty()) {
            const auto& span = doc.spans[at.front()];
            return found((int)chunks.size() - 1, span.beg,
                         doc.entries[span.entry_ix], "", false);
        }
    }
    return g;
}

std::vector<PlanStep> planReading(const std::vector<Chunk>& chunks,
                                  const VerbGuess& verb) {
    std::vector<PlanStep> plan(chunks.size());
    for (size_t i = 0; i < chunks.size(); ++i) plan[i].chunk = (int)i;
    int next = 1;
    auto assign = [&](int i, const char* how) {
        if (plan[i].order == 0 && plan[i].how[0] == '\0') {
            plan[i].order = next++;
            plan[i].how = how;
        }
    };
    const VerbClass* vc = verb.cls ? &verb.cls->cls : nullptr;
    // 0) attaching chunks first, so they never receive their own number.
    // Exception: under a conjunctive/disjunctive verb the dang chunk is a
    // verb qualifier ("related WITH x", "free OF x") and gets its own number.
    const bool dangCompletes =
        vc && (*vc == VerbClass::Conjunctive || *vc == VerbClass::Disjunctive);
    for (size_t i = 0; i < chunks.size(); ++i) {
        if (attachesForward(chunks[i].marker) &&
            !(dangCompletes && chunks[i].marker == "dang")) {
            plan[i].order = 0;
            plan[i].how = "no number: reads together with the next chunk "
                          "(\"the Y of X\" — right to left)";
        }
    }
    // 1) the verb frames the clause
    if (verb.chunk >= 0 && !attachesForward(chunks[verb.chunk].marker))
        assign(verb.chunk, "read FIRST: the clause-final verb frames everything");
    // 1b) Wilson class-specific roles override the generic passes
    if (vc) {
        auto isAgentMarked = [](const Chunk& c) {
            return c.marker == "gis" || c.marker == "kyis" ||
                   c.marker == "gyis" || c.marker == "yis" || c.marker == "s";
        };
        auto isLaDon = [](const Chunk& c) {
            return c.marker == "la" || c.marker == "du" || c.marker == "ru" ||
                   c.marker == "su" || c.marker == "tu" || c.marker == "r" ||
                   c.marker == "na";
        };
        auto isSource = [](const Chunk& c) {
            return c.marker == "nas" || c.marker == "las";
        };
        for (size_t i = 0; i < chunks.size(); ++i) {
            if (plan[i].order != 0 || plan[i].how[0] != '\0') continue;
            const Chunk& c = chunks[i];
            switch (*vc) {
                case VerbClass::Absence:
                    if (isAgentMarked(c))
                        assign((int)i, "what the subject is empty/devoid OF — "
                                       "the gis-family marker here is NOT an "
                                       "agent (Wilson Class IV absence)");
                    break;
                case VerbClass::Separative:
                    if (isSource(c))
                        assign((int)i, "what the subject is freed/separated "
                                       "FROM (Wilson Class IV separative)");
                    break;
                case VerbClass::Conjunctive:
                case VerbClass::Disjunctive:
                    if (c.marker == "dang")
                        assign((int)i, "completes the verb: WITH x / free OF "
                                       "x (Wilson Class IV, dang qualifier)");
                    break;
                case VerbClass::Necessity:
                    if (isLaDon(c))
                        assign((int)i, "the English SUBJECT — \"x needs …\" "
                                       "(Wilson Class VII: la marks the "
                                       "needer)");
                    break;
                case VerbClass::Existence:
                    if (isLaDon(c))
                        assign((int)i, "the place — or the possessor "
                                       "(\"x has …\", Wilson Class VIII use)");
                    break;
                case VerbClass::Motion:
                    if (isLaDon(c))
                        assign((int)i, "the DESTINATION of the motion "
                                       "(Wilson Class III)");
                    break;
                case VerbClass::Dependence:
                case VerbClass::Attitude:
                    if (isLaDon(c))
                        assign((int)i, "what it depends on / is felt about "
                                       "(Wilson Class II locative qualifier)");
                    break;
                default:
                    break;
            }
        }
    }
    // 2) topic, 3) agent, 4) unmarked, 5) la don, 6) source — textual order
    auto pass = [&](auto pred, const char* how) {
        for (size_t i = 0; i < chunks.size(); ++i)
            if (plan[i].order == 0 && plan[i].how[0] == '\0' && pred(chunks[i]))
                assign((int)i, how);
    };
    pass([](const Chunk& c) { return c.marker == "ni"; },
         "topic — \"as for X, …\"");
    pass([](const Chunk& c) {
             return c.marker == "gis" || c.marker == "kyis" ||
                    c.marker == "gyis" || c.marker == "yis" || c.marker == "s";
         },
         "the agent — \"by X\"");
    pass([](const Chunk& c) { return c.marker.empty(); },
         "unmarked — object/complement of the verb");
    pass([](const Chunk& c) {
             return c.marker == "la" || c.marker == "du" || c.marker == "ru" ||
                    c.marker == "su" || c.marker == "tu" || c.marker == "r" ||
                    c.marker == "na";
         },
         "destination/location/purpose — \"to/at/in X\"");
    pass([](const Chunk& c) { return c.marker == "nas" || c.marker == "las"; },
         "source — \"from X\"");
    // anything left over is honestly unplanned
    for (auto& p : plan)
        if (p.order == 0 && p.how[0] == '\0')
            p.how = "unplanned — this shape is beyond the rule tables (review)";
    return plan;
}

}  // namespace allcore
