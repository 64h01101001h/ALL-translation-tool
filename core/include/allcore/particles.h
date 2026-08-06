// allcore/particles.h — the Wilson particle-morphology layer.
//
// Classical Tibetan case grammar implemented as rule tables (framework:
// Wilson, "Translating Buddhism from Tibetan" — rules only, no copyrighted
// text). The freestanding-particle inventory is cross-checked against the
// canonical pron_engine.py PARTICLES set; the fused-ending sandhi follows the
// standard suffix rules: a-chung clitics ('i 'o 'u 'am 'ang) attach to any
// stem the dictionary attests, bare -s (agentive) and -r (la don terminative)
// fuse ONLY onto open, vowel-final syllables (PA→PAS/PAR, DE→DES/DER) — a
// closed syllable like SEMS must never be split to SEM + s.
#pragma once

#include <optional>
#include <string>

namespace allcore {

// A freestanding grammatical particle (its own syllable in the text).
struct ParticleInfo {
    const char* wylie;     // canonical lowercase wylie form
    const char* group;     // full paradigm row it belongs to
    const char* function;  // grammatical function, human-readable
};

// Classify an ACIP token as a freestanding particle; nullptr if it is not one.
const ParticleInfo* classifyParticle(const std::string& acip_token);

// A fused ending split off the tail of one ACIP syllable (PA'I → PA + 'i).
struct FusedSplit {
    std::string base;      // ACIP stem with the ending removed
    std::string ending;    // ending in lowercase wylie: 'i 'o 'u 'am 'ang s r
    const char* function;  // grammatical function of the ending
};

// The (unique) fused-ending split of a token, or nullopt. Endings are tried
// longest-first; S/R apply only when the remaining stem is vowel-final.
std::optional<FusedSplit> splitFusedEnding(const std::string& acip_token);

// Given a candidate stem and the surplus tail (token == base + rest), return
// the lowercase-wylie ending if `rest` is a valid fused ending on that stem.
std::optional<std::string> fusedEndingIf(const std::string& base,
                                         const std::string& rest);

// Function label for an ending recorded on an OverlaySpan ("'i" → genitive…);
// empty string when unknown.
const char* fusedEndingFunction(const std::string& ending_wylie);

// ---- suffix agreement (sandhi) ---------------------------------------------
// Which variant of a variant-selecting particle (gi/kyi/gyi/yi, gis/kyis/…,
// kyang/yang, tu/du/su/ru, ste/te/de) is grammatical after which final letter.
// A soft DIAGNOSTIC layer: a mismatch is flagged for review, never corrected —
// historical da-drag stems (shin tu, yin kyang, gyur kyang) legitimately take
// the da-class variant after -n/-r/-l.
enum class Agreement {
    NotAParticleVariant,  // token is not one of the variant-selecting particles
    Agrees,               // variant matches the preceding suffix
    DaDragPossible,       // da-class variant after -n/-r/-l: valid iff the stem
                          // carries historical da-drag
    Disagrees,            // variant is wrong for the preceding suffix
};

struct AgreementResult {
    Agreement verdict = Agreement::NotAParticleVariant;
    std::string expected;  // the variant(s) the tables call for, lowercase wylie
    const char* note = "";
};

// Check the particle token against the syllable that precedes it.
AgreementResult checkAgreement(const std::string& prev_acip,
                               const std::string& particle_acip);

}  // namespace allcore
