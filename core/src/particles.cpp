#include "allcore/particles.h"

#include <cstring>

namespace allcore {

namespace {

// ---- freestanding particles -------------------------------------------------
// Keyed by ACIP form (uppercase). Inventory ⊇ pron_engine.py PARTICLES; the
// paradigm/function labels are the Wilson case-grammar rule tables.
struct NamedParticle {
    const char* acip;
    ParticleInfo info;
};

const NamedParticle kParticles[] = {
    // genitive: connects two nominals ("of", relative clauses)
    {"GI", {"gi", "gi kyi gyi yi · fused 'i", "genitive — \"of\", connective"}},
    {"KYI", {"kyi", "gi kyi gyi yi · fused 'i", "genitive — \"of\", connective"}},
    {"GYI", {"gyi", "gi kyi gyi yi · fused 'i", "genitive — \"of\", connective"}},
    {"YI", {"yi", "gi kyi gyi yi · fused 'i", "genitive — \"of\", connective"}},
    // agentive (instrumental): marks the agent/instrument ("by", "with")
    {"GIS", {"gis", "gis kyis gyis yis · fused -s", "agentive — \"by/with\""}},
    {"KYIS", {"kyis", "gis kyis gyis yis · fused -s", "agentive — \"by/with\""}},
    {"GYIS", {"gyis", "gis kyis gyis yis · fused -s", "agentive — \"by/with\""}},
    {"YIS", {"yis", "gis kyis gyis yis · fused -s", "agentive — \"by/with\""}},
    // la don: the seven meanings of la — location, destination, purpose…
    {"LA", {"la", "la don: su ru du tu na la · fused -r", "la don — \"to/at/in/for\""}},
    {"NA", {"na", "la don: su ru du tu na la · fused -r", "la don — \"in/if/when\""}},
    {"DU", {"du", "la don: su ru du tu na la · fused -r", "la don — \"to/at/as\""}},
    {"RU", {"ru", "la don: su ru du tu na la · fused -r", "la don — \"to/at/as\""}},
    {"SU", {"su", "la don: su ru du tu na la · fused -r", "la don — \"to/at/as\""}},
    {"TU", {"tu", "la don: su ru du tu na la · fused -r", "la don — \"to/at/as\""}},
    // ablative / originative
    {"NAS", {"nas", "nas las", "ablative — \"from\", also \"through/after\""}},
    {"LAS", {"las", "nas las", "ablative — \"from\", also comparative \"than\""}},
    // semi-final connective (gerundial)
    {"STE", {"ste", "ste te de", "semi-final connective — \"…and/thus,\""}},
    {"TE", {"te", "ste te de", "semi-final connective — \"…and/thus,\""}},
    {"DE", {"de", "ste te de", "semi-final connective — \"…and/thus,\""}},
    // conjunctive
    {"DANG", {"dang", "dang", "conjunctive — \"and/with\""}},
    // topic marker
    {"NI", {"ni", "ni", "topic marker — sets off the subject"}},
    // concessive
    {"YANG", {"yang", "yang kyang · fused 'ang", "concessive — \"also/even/but\""}},
    {"KYANG", {"kyang", "yang kyang · fused 'ang", "concessive — \"also/even/but\""}},
    // plural
    {"RNAMS", {"rnams", "rnams dag", "plural marker"}},
    {"DAG", {"dag", "rnams dag", "plural marker (also dual/emphatic)"}},
    // delimiting
    {"TSAM", {"tsam", "tsam", "delimiting — \"just/only/about\""}},
    // coordinating gerund ("…and", links clauses)
    {"CING", {"cing", "cing zhing shing", "coordinating — \"…and, while\""}},
    {"ZHING", {"zhing", "cing zhing shing", "coordinating — \"…and, while\""}},
    {"SHING", {"shing", "cing zhing shing", "coordinating — \"…and, while\""}},
    // quotative (closes a quotation)
    {"CES", {"ces", "ces zhes shes", "quotative — marks the end of a quote"}},
    {"ZHES", {"zhes", "ces zhes shes", "quotative — marks the end of a quote"}},
    {"SHES", {"shes", "ces zhes shes", "quotative — marks the end of a quote"}},
};

// ---- fused endings ----------------------------------------------------------
struct Ending {
    const char* acip;      // suffix as it appears on the ACIP token
    const char* wylie;     // lowercase wylie recorded on the span
    const char* function;
    bool needs_open_stem;  // true = stem must end in a vowel (bare -s / -r)
};

// Longest-first; final characters are pairwise distinct, so at most one can
// structurally match a given token.
const Ending kEndings[] = {
    {"'ANG", "'ang", "concessive — \"also/even\"", false},
    {"'AM", "'am", "alternative — \"or\"", false},
    {"'I", "'i", "genitive — \"of\"", false},
    {"'O", "'o", "final declarative — ends the statement", false},
    {"'U", "'u", "diminutive", false},
    {"S", "s", "agentive — \"by/with\"", true},
    {"R", "r", "la don — \"to/at/in\"", true},
};

bool endsInVowel(const std::string& stem) {
    if (stem.empty()) return false;
    const char c = stem.back();
    return c == 'A' || c == 'E' || c == 'I' || c == 'O' || c == 'U' ||
           c == 'a' || c == 'e' || c == 'i' || c == 'o' || c == 'u';
}

}  // namespace

namespace {
// within the particle/suffix domain, wylie and ACIP differ only by
// letter case (no TS/TZ-class letters occur in this closed set), so an
// uppercase fold makes both scripts exact here
std::string upAcip(const std::string& t) {
    std::string u = t;
    for (auto& c : u)
        if (c >= 'a' && c <= 'z') c = (char)(c - 'a' + 'A');
    return u;
}
}  // namespace

const ParticleInfo* classifyParticle(const std::string& token) {
    const std::string t = upAcip(token);
    for (const auto& p : kParticles)
        if (t == p.acip) return &p.info;
    return nullptr;
}

std::optional<FusedSplit> splitFusedEnding(const std::string& token) {
    // ACIP tokens carry uppercase endings, wylie tokens lowercase —
    // the two suffix sets are case-disjoint, so one pass over each is
    // unambiguous
    for (const auto& e : kEndings) {
        for (const char* suf : {e.acip, e.wylie}) {
            const size_t n = std::strlen(suf);
            if (token.size() <= n ||
                token.compare(token.size() - n, n, suf) != 0)
                continue;
            std::string base = token.substr(0, token.size() - n);
            if (e.needs_open_stem && !endsInVowel(base))
                return std::nullopt;
            return FusedSplit{std::move(base), e.wylie, e.function};
        }
    }
    return std::nullopt;
}

std::optional<std::string> fusedEndingIf(const std::string& base,
                                         const std::string& rest) {
    for (const auto& e : kEndings) {
        if (rest != e.acip && rest != e.wylie) continue;
        if (e.needs_open_stem && !endsInVowel(base)) return std::nullopt;
        return std::string(e.wylie);
    }
    return std::nullopt;
}

const char* fusedEndingFunction(const std::string& ending_wylie) {
    for (const auto& e : kEndings)
        if (ending_wylie == e.wylie) return e.function;
    return "";
}

// ---- suffix agreement -------------------------------------------------------
namespace {

// The suffix class of a syllable = its last letter (secondary suffix -s counts,
// as the sandhi tables key on it). NG must be read as the nga digraph.
enum class Sfx { Open, Ga, Nga, Da, Na, Ba, Ma, Achung, Ra, La, Sa, Unknown };

Sfx suffixClassOf(const std::string& acip) {
    if (acip.empty()) return Sfx::Unknown;
    const char c = acip.back();
    switch (c) {
        case 'A': case 'E': case 'I': case 'O': case 'U': return Sfx::Open;
        case '\'': return Sfx::Achung;
        case 'G':
            return (acip.size() >= 2 && acip[acip.size() - 2] == 'N') ? Sfx::Nga
                                                                      : Sfx::Ga;
        case 'D': return Sfx::Da;
        case 'N': return Sfx::Na;
        case 'B': return Sfx::Ba;
        case 'M': return Sfx::Ma;
        case 'R': return Sfx::Ra;
        case 'L': return Sfx::La;
        case 'S': return Sfx::Sa;
        default: return Sfx::Unknown;
    }
}

enum class Family {
    Genitive, Agentive, Concessive, Terminative, SemiFinal,
    Coordinating, Quotative,
};

struct Variant {
    const char* acip;
    Family family;
    bool da_class;  // the variant selected after -d (da-drag candidate)
};

const Variant kVariants[] = {
    {"GI", Family::Genitive, false},   {"KYI", Family::Genitive, true},
    {"GYI", Family::Genitive, false},  {"YI", Family::Genitive, false},
    {"GIS", Family::Agentive, false},  {"KYIS", Family::Agentive, true},
    {"GYIS", Family::Agentive, false}, {"YIS", Family::Agentive, false},
    {"KYANG", Family::Concessive, true}, {"YANG", Family::Concessive, false},
    {"TU", Family::Terminative, true},   {"DU", Family::Terminative, false},
    {"SU", Family::Terminative, false},  {"RU", Family::Terminative, false},
    {"STE", Family::SemiFinal, false},   {"TE", Family::SemiFinal, false},
    // DE is deliberately absent: standalone DE is almost always the
    // demonstrative "that", so diagnosing it as the semi-final variant would
    // be noise. (checkAgreement returns NotAParticleVariant for it.)
    {"CING", Family::Coordinating, true}, {"ZHING", Family::Coordinating, false},
    {"SHING", Family::Coordinating, false},
    {"CES", Family::Quotative, true},     {"ZHES", Family::Quotative, false},
    // SHES is deliberately absent from diagnostics: standalone SHES is very
    // often the verb "to know" (shes pa), so it is classified but never
    // agreement-flagged.
};

// The variant each family's table calls for after a given suffix class.
std::string expectedVariant(Family f, Sfx s) {
    switch (f) {
        case Family::Genitive:
            if (s == Sfx::Ga || s == Sfx::Nga) return "gi";
            if (s == Sfx::Da || s == Sfx::Ba || s == Sfx::Sa) return "kyi";
            if (s == Sfx::Na || s == Sfx::Ma || s == Sfx::Ra || s == Sfx::La)
                return "gyi";
            return "'i (fused) or yi";
        case Family::Agentive:
            if (s == Sfx::Ga || s == Sfx::Nga) return "gis";
            if (s == Sfx::Da || s == Sfx::Ba || s == Sfx::Sa) return "kyis";
            if (s == Sfx::Na || s == Sfx::Ma || s == Sfx::Ra || s == Sfx::La)
                return "gyis";
            return "-s (fused) or yis";
        case Family::Concessive:
            if (s == Sfx::Ga || s == Sfx::Da || s == Sfx::Ba || s == Sfx::Sa)
                return "kyang";
            if (s == Sfx::Open || s == Sfx::Achung) return "yang or 'ang (fused)";
            return "yang";
        case Family::Terminative:
            if (s == Sfx::Ga || s == Sfx::Ba) return "tu";
            if (s == Sfx::Sa) return "su";
            if (s == Sfx::Open || s == Sfx::Achung) return "ru or -r (fused)";
            return "du";
        case Family::SemiFinal:
            if (s == Sfx::Da) return "de";
            if (s == Sfx::Na || s == Sfx::Ra || s == Sfx::La || s == Sfx::Sa)
                return "te";
            return "ste";
        case Family::Coordinating:
            if (s == Sfx::Ga || s == Sfx::Da || s == Sfx::Ba) return "cing";
            if (s == Sfx::Sa) return "shing";
            return "zhing";
        case Family::Quotative:
            if (s == Sfx::Ga || s == Sfx::Da || s == Sfx::Ba) return "ces";
            // classical tables call for shes after -s, but zhes is the
            // dominant convention in the HGM corpus (rnams zhes 71×) — both
            // accepted, shes taught first
            if (s == Sfx::Sa) return "shes (classical) or zhes";
            return "zhes";
    }
    return "";
}

std::string lowercased(const std::string& s) {
    std::string out = s;
    for (auto& c : out)
        if (c >= 'A' && c <= 'Z') c = (char)(c - 'A' + 'a');
    return out;
}

}  // namespace

AgreementResult checkAgreement(const std::string& prev_token,
                               const std::string& particle_token) {
    const std::string particle_acip = upAcip(particle_token);
    const Variant* v = nullptr;
    for (const auto& x : kVariants)
        if (particle_acip == x.acip) { v = &x; break; }
    if (!v) return {};
    const Sfx s = suffixClassOf(upAcip(prev_token));
    if (s == Sfx::Unknown) return {};
    AgreementResult r;
    r.expected = expectedVariant(v->family, s);
    const std::string given = lowercased(particle_acip);
    // "or"-listed expectations count any listed form as agreement
    if (r.expected == given ||
        r.expected.find(given + " ") == 0 ||
        r.expected.find(" " + given) != std::string::npos) {
        r.verdict = Agreement::Agrees;
        return r;
    }
    // da-class variant after -n/-r/-l: legitimate iff the stem carries
    // historical da-drag (shin tu, yin kyang, gyur kyang…)
    if (v->da_class && (s == Sfx::Na || s == Sfx::Ra || s == Sfx::La)) {
        r.verdict = Agreement::DaDragPossible;
        r.note = "valid only if the stem carries historical da-drag "
                 "(as in shin tu, yin kyang, gyur kyang)";
        return r;
    }
    r.verdict = Agreement::Disagrees;
    r.note = "particle variant disagrees with the preceding suffix";
    return r;
}

}  // namespace allcore
