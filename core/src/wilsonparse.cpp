#include "allcore/wilsonparse.h"

#include <algorithm>

#include "allcore/engines.h"
#include "allcore/particles.h"
#include "allcore/poslex.h"
#include "allcore/verbclass.h"

namespace allcore {

namespace {

bool inSet(const std::string& s, std::initializer_list<const char*> set) {
    for (const char* x : set)
        if (s == x) return true;
    return false;
}

// closed classes (rule data)
bool isPronoun(const std::string& acip) {
    return inSet(acip, {"NGA", "NGED", "KHYOD", "KHYED", "KHO", "KHONG",
                        "BDAG", "RANG", "DE", "'DI", "GANG", "SU", "CI"});
}
bool isNumber(const std::string& acip) {
    return inSet(acip, {"GCIG", "GNYIS", "GSUM", "BZHI", "LNGA", "DRUG",
                        "BDUN", "BRGYAD", "DGU", "BCU", "BRGYA", "STONG"});
}
bool isPostposition(const std::string& acip) {
    return inSet(acip, {"NANG", "STENG", "'OG", "RJES", "SNGON", "BAR",
                        "MDUN", "RGYAB", "DRUNG"});
}

// eight-case number for a case particle, contextual on the clause verb class
std::string caseLabel(const std::string& acipTok, const VerbGuess& verb) {
    std::string low;
    for (char c : acipTok)
        low += (c >= 'A' && c <= 'Z') ? (char)(c - 'A' + 'a') : c;
    const VerbClass* vc = verb.cls ? &verb.cls->cls : nullptr;
    if (inSet(low, {"gi", "kyi", "gyi", "yi"}))
        return "case particle — 6th (connective, \"of\")";
    if (inSet(low, {"gis", "kyis", "gyis", "yis"})) {
        if (vc && *vc == VerbClass::Absence)
            return "syntactic particle — quality absent (NOT 3rd case here; "
                   "Wilson Class IV absence)";
        return "case particle — 3rd (agentive, \"by\")";
    }
    if (inSet(low, {"nas", "las"})) {
        if (vc && *vc == VerbClass::Separative)
            return "case particle — 5th (originative: freed/separated FROM)";
        return "case particle — 5th (originative, \"from\"; las also "
               "comparative)";
    }
    if (inSet(low, {"la", "na", "du", "ru", "su", "tu"})) {
        if (vc) {
            switch (*vc) {
                case VerbClass::Motion:
                case VerbClass::NomAction:
                    return "case particle — 2nd (objective: destination/place "
                           "of activity)";
                case VerbClass::Necessity:
                    return "case particle — 4th (beneficial-purposive: the "
                           "one who needs)";
                case VerbClass::Existence:
                case VerbClass::Living:
                    return "case particle — 7th (locative: place / possessor)";
                case VerbClass::Dependence:
                case VerbClass::Attitude:
                    return "case particle — 7th (referential locative, "
                           "\"about/toward\")";
                default:
                    break;
            }
        }
        return "case particle — la-group (2nd/4th/7th; verb class undetermined)";
    }
    return "";
}

// verb evidence on the token itself (tenses / verb class / "to …" gloss)
bool verbEvidenceTok(const OverlayDoc& doc, int t) {
    if (t < 0 || t >= (int)doc.tokens.size()) return false;
    for (int ix : doc.spansAt(t)) {
        const auto& e = doc.entries[doc.spans[ix].entry_ix];
        if (!e.tenses.empty()) return true;
        if (classifyVerbWithTenses(e.wylie, e.tenses)) return true;
        for (const auto& g : e.hgm_gloss)
            if (g.rfind("to ", 0) == 0) return true;
    }
    return false;
}

}  // namespace

std::vector<ClauseParse> wilsonParse(const Spine& spine, const OverlayDoc& doc,
                                     const std::vector<Clause>& clauses,
                                     const PosLexicon* pos) {
    std::vector<ClauseParse> out;
    for (const auto& cl : clauses) {
        ClauseParse cp;
        cp.clause = cl;
        auto chunks = chunkClause(doc, cl);
        cp.verb = spotVerb(doc, chunks);
        for (int t = cl.beg; t < cl.end;) {
            const std::string& tok = doc.tokens[t];
            ParseUnit u;
            u.beg = t;
            // 0a) the clause's own boundary particle (final so/ngo/'o, or a
            // connective) — splitClauses identified it; designate it as such
            if (t == cl.end - 1 && !cl.boundary.empty() &&
                cl.boundary != "barrier") {
                u.end = t + 1;
                u.text = tok;
                u.category = std::string("clause-boundary particle — ") +
                             cl.boundary_function;
                cp.units.push_back(std::move(u));
                ++t;
                continue;
            }
            // 0v) interjections (closed list; Wilson's vocative examples:
            // kyi ma, kyi hud …) — clause-initial or following another
            // interjection; they carry VOC dots
            {
                const bool vocContext =
                    t == cl.beg ||
                    (!cp.units.empty() &&
                     cp.units.back().category.rfind("interjection", 0) == 0);
                int len = 0;
                if (vocContext) {
                    auto is2 = [&](const char* a, const char* b) {
                        return t + 1 < cl.end && tok == a &&
                               doc.tokens[t + 1] == b;
                    };
                    if (t + 2 < cl.end && tok == "E" &&
                        doc.tokens[t + 1] == "MA" && doc.tokens[t + 2] == "HO")
                        len = 3;
                    else if (is2("KYI", "MA") || is2("KYI", "HUD") ||
                             is2("KYE", "MA") || is2("E", "MA"))
                        len = 2;
                    else if (tok == "KYE")
                        len = 1;
                }
                if (len > 0) {
                    u.end = t + len;
                    for (int k = t; k < t + len; ++k) {
                        if (k > t) u.text += ' ';
                        u.text += doc.tokens[k];
                    }
                    u.category = "interjection (vocative)";
                    cp.units.push_back(std::move(u));
                    t += len;
                    continue;
                }
            }
            // 0) constructions the dictionary must not swallow: negation and
            // the pa-r infinitive marker are shown transparently even when a
            // compound entry (mi bya "do not") exists
            if ((tok == "MI" || tok == "MA") && verbEvidenceTok(doc, t + 1)) {
                u.end = t + 1;
                u.text = tok;
                u.category = "negation particle";
                u.detail = "negates the following verb (mi: present/future · "
                           "ma: past/imperative)";
                cp.units.push_back(std::move(u));
                ++t;
                continue;
            }
            if ((tok == "PAR" || tok == "BAR") && !cp.units.empty() &&
                (cp.units.back().category.rfind("verb", 0) == 0 ||
                 cp.units.back().category.rfind("verbal noun", 0) == 0) &&
                verbEvidenceTok(doc, t + 1)) {
                u.end = t + 1;
                u.text = tok;
                u.category = "infinitive marker — nominalizer pa/ba + la-don "
                             "r (verb phrase: V pa-r + auxiliary)";
                cp.units.push_back(std::move(u));
                ++t;
                continue;
            }
            // 1) case particle with contextual case number
            const std::string cl8 = caseLabel(tok, cp.verb);
            const ParticleInfo* pinfo = classifyParticle(tok);
            if (!cl8.empty()) {
                u.end = t + 1;
                u.text = tok;
                u.category = cl8;
                if (pinfo) u.detail = std::string("paradigm: ") + pinfo->group;
                cp.units.push_back(std::move(u));
                t = cp.units.back().end;
                continue;
            }
            // 2) other freestanding particles (syntactic / lexical) —
            // never clause-initial, and the lexically ambiguous ones need
            // their licensing suffix on the previous syllable (shes after -s,
            // de after -d; otherwise they are the verb "know" / demonstrative)
            bool particleOk = pinfo != nullptr && t > cl.beg;
            if (particleOk && tok == "SHES")
                particleOk = !doc.tokens[t - 1].empty() &&
                             doc.tokens[t - 1].back() == 'S';
            if (particleOk && tok == "DE")
                particleOk = !doc.tokens[t - 1].empty() &&
                             doc.tokens[t - 1].back() == 'D';
            if (particleOk) {
                u.end = t + 1;
                u.text = tok;
                u.category = std::string("particle — ") + pinfo->function;
                u.detail = std::string("paradigm: ") + pinfo->group;
                cp.units.push_back(std::move(u));
                t = cp.units.back().end;
                continue;
            }
            // 3) closed lexical classes
            if (isPronoun(tok) || isNumber(tok) || isPostposition(tok)) {
                u.end = t + 1;
                u.text = tok;
                u.category = isPronoun(tok)
                                 ? "pronoun"
                                 : (isNumber(tok) ? "number" : "postposition");
                cp.units.push_back(std::move(u));
                t = cp.units.back().end;
                continue;
            }
            // 4) dictionary span (innermost) — verb, verbal noun, or term
            auto at = doc.spansAt(t);
            if (!at.empty()) {
                // the designation unit is the WORD: prefer the longest
                // GLOSSED span starting here (so sangs rgyas parses as one
                // term, not two syllables), staying inside the clause
                int chosen = -1, bestLen = 0;
                bool bestGlossed = false;
                for (int ix : at) {
                    const auto& s2 = doc.spans[ix];
                    if (s2.beg != t || s2.end > cl.end) continue;
                    const bool g =
                        !doc.entries[s2.entry_ix].hgm_gloss.empty();
                    const int len = s2.end - s2.beg;
                    if (chosen < 0 || (g && !bestGlossed) ||
                        (g == bestGlossed && len > bestLen)) {
                        chosen = ix;
                        bestLen = len;
                        bestGlossed = g;
                    }
                }
                if (chosen < 0) chosen = at.front();
                const auto& span = doc.spans[chosen];
                const auto& e = doc.entries[span.entry_ix];
                u.beg = t;
                u.end = std::max(t + 1, span.end);
                for (int k = u.beg; k < u.end; ++k) {
                    if (k > u.beg) u.text += ' ';
                    u.text += doc.tokens[k];
                }
                const auto* vcls = classifyVerbWithTenses(e.wylie, e.tenses);
                if (!e.tenses.empty() || vcls) {
                    u.category = vcls ? std::string("verb — ") + vcls->label
                                      : "verb (tense forms attested)";
                    u.detail = !e.tenses.empty()
                                   ? "tenses: " + e.tenses
                                   : (vcls ? vcls->expects : "");
                } else if ((e.wylie.size() > 3 &&
                            (e.wylie.rfind(" pa") == e.wylie.size() - 3 ||
                             e.wylie.rfind(" ba") == e.wylie.size() - 3))) {
                    // verbal noun iff the stem itself shows verb evidence
                    const std::string stem = e.wylie.substr(0, e.wylie.size() - 3);
                    bool stemVerb = false;
                    for (const auto& se : spine.lookup(stem)) {
                        if (!se.tenses.empty() ||
                            classifyVerbWithTenses(se.wylie, se.tenses))
                            stemVerb = true;
                        for (const auto& g : se.hgm_gloss)
                            if (g.rfind("to ", 0) == 0) stemVerb = true;
                    }
                    if (stemVerb) {
                        u.category = "verbal noun (nominalized " + stem + ")";
                    } else {
                        u.category = "dictionary term";
                    }
                    if (!e.hgm_gloss.empty()) u.detail = "≡ " + e.hgm_gloss.front();
                } else {
                    u.category = "dictionary term";
                    if (!e.hgm_gloss.empty()) u.detail = "≡ " + e.hgm_gloss.front();
                }
                if (!span.clitic.empty())
                    u.detail += (u.detail.empty() ? "" : " · ") +
                                std::string("fused ending ") + span.clitic +
                                " (" + fusedEndingFunction(span.clitic) + ")";
                cp.units.push_back(std::move(u));
                t = cp.units.back().end;
                continue;
            }
            // 5) honestly unresolved
            u.end = t + 1;
            u.text = tok;
            u.category = "unresolved — not in the dictionary or rule tables";
            cp.units.push_back(std::move(u));
            ++t;
        }
        // ---- post-passes: constructions that live BETWEEN units ----
        auto& us = cp.units;
        for (size_t i = 0; i < us.size(); ++i) {
            // postposition construction: genitive + postposition (+ la-group)
            if (us[i].category == "postposition" && i > 0) {
                const bool genBefore =
                    us[i - 1].category.find("6th") != std::string::npos ||
                    us[i - 1].detail.find("fused ending 'i") !=
                        std::string::npos;
                if (genBefore)
                    us[i].detail = "postposition construction: genitive + " +
                                   us[i].text + " (usually + la-group: "
                                   "\"in/on/after … of x\")";
            }
            // auxiliary verb phrase: V + pa-r/ba-r + auxiliary verb
            if (i >= 1 && us[i].category.rfind("verb", 0) == 0 &&
                us[i - 1].category.rfind("infinitive marker", 0) == 0) {
                us[i].category +=
                    " · auxiliary in a verb phrase (V + pa-r + auxiliary: "
                    "will/should/causes)";
            }
        }
        // ---- the Science of the Dots: designate every tsheg ----
        {
            std::vector<int> unitOf(cl.end - cl.beg, -1);
            for (size_t ui = 0; ui < us.size(); ++ui)
                for (int k = us[ui].beg; k < us[ui].end; ++k)
                    unitOf[k - cl.beg] = (int)ui;
            auto caseDigit = [](const std::string& cat) -> std::string {
                for (const char* n : {"2nd", "3rd", "4th", "5th", "6th", "7th"})
                    if (cat.find(n) != std::string::npos)
                        return std::string(1, n[0]);
                return "";
            };
            auto isCase = [](const std::string& cat) {
                return cat.rfind("case particle", 0) == 0;
            };
            auto isVerbCat = [](const std::string& cat) {
                return cat.rfind("verb —", 0) == 0 ||
                       cat.rfind("verb (", 0) == 0;
            };
            for (int t = cl.beg; t < cl.end; ++t) {
                const int cu = unitOf[t - cl.beg];
                const int nu =
                    (t + 1 < cl.end) ? unitOf[t + 1 - cl.beg] : -1;
                DotMark d;
                d.after_tok = t;
                const auto& cur = us[cu];
                if (nu == cu) {
                    d.label = "S";   // within one word / particle / phrase
                } else if (cur.category.rfind("interjection", 0) == 0) {
                    d.label = "VOC";
                } else if (nu >= 0 && isCase(us[nu].category)) {
                    const std::string n = caseDigit(us[nu].category);
                    d.label = n.empty() ? "C" : "C " + n;
                } else if (isCase(cur.category)) {
                    const std::string n = caseDigit(cur.category);
                    d.label = n.empty() ? "C" : "C " + n;
                } else if (cur.category.rfind("infinitive marker", 0) == 0) {
                    d.label = "V";   // open verb: after the infinitive
                } else if (cur.category == "negation particle") {
                    d.label = "S";   // lexical, bound to its verb
                } else if (cur.category.rfind("clause-boundary", 0) == 0 ||
                           cur.category.rfind("particle —", 0) == 0 ||
                           cur.category.rfind("syntactic particle", 0) == 0) {
                    d.label = "SP";
                } else if (isVerbCat(cur.category)) {
                    // final verb → VB; verb before a continuative → V
                    if (nu < 0)
                        d.label = "VB";
                    else if (us[nu].category.rfind("clause-boundary", 0) == 0)
                        d.label = us[nu].category.find("final declarative") !=
                                          std::string::npos
                                      ? "VB"
                                      : "V";
                    else
                        d.label = "V";
                } else if (cur.category.rfind("verbal noun", 0) == 0 &&
                           nu < 0) {
                    d.label = "V";   // clause-ending verbal
                } else if (nu < 0) {
                    d.label = "NOM";   // clause-final unmarked nominal
                } else if (isVerbCat(us[nu].category) ||
                           us[nu].category == "negation particle" ||
                           us[nu].category.rfind("verbal noun", 0) == 0) {
                    d.label = "NOM";   // nominative right before the verb
                } else {
                    // two bare nominals: NOM vs NA/NN/APP needs POS data.
                    // With the SOAS hand-tagged lexicon (CC BY 4.0) the NA
                    // case resolves on an UNAMBIGUOUS adjective tag, and a
                    // noun-noun pair narrows to NN|APP (apposition remains
                    // undecidable from POS alone). Evidence is named;
                    // anything ambiguous stays undetermined.
                    d.label = "NOM|NA|NN|APP (undetermined)";
                    if (pos) {
                        auto uniOf = [](const std::string& acip) {
                            auto [u, ok] = wylieToUnicode(acipToEwts(acip));
                            return ok ? u : std::string();
                        };
                        const std::string nextUni = uniOf(us[nu].text);
                        if (!nextUni.empty() && pos->unambiguousAdj(nextUni)) {
                            d.label = "NA (adjective — SOAS lexicon)";
                        } else {
                            const std::string curUni = uniOf(cur.text);
                            if (!curUni.empty() && !nextUni.empty() &&
                                pos->unambiguousNoun(curUni) &&
                                pos->unambiguousNoun(nextUni))
                                d.label = "NN|APP (both nouns — SOAS lexicon; "
                                          "apposition undecidable)";
                        }
                    }
                }
                cp.dots.push_back(std::move(d));
            }
        }
        out.push_back(std::move(cp));
    }
    return out;
}

}  // namespace allcore
