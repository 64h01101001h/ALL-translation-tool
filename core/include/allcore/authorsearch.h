// allcore/authorsearch.h — find a catalog author when you can only type
// the name one of the ways it is written, or only know how it sounds.
//
// Adam's requirement (backlog #33, 2026-08-22): the query side must
// accept ACIP or Wylie interchangeably, AND match approximately by
// pronunciation, so someone who knows how a name SOUNDS but not how it
// is spelled still finds the works.
//
// The tiers are ordered by how much they claim, and the caller is
// expected to SHOW that ordering rather than flatten it:
//
//   Exact     — canonical-space identity. The name you typed is the
//               name that is banked.
//   Contains  — canonical-space substring. "tsong kha pa" inside
//               "rje tsong kha pa blo bzang grags pa".
//   Spacing   — the same letters with different syllable spacing.
//               Tibetan names are written both ways ("pandi ta" and
//               "pandita", "sa skya" and "sakya"), so collapsing
//               spaces is a normalization, not a guess. Still
//               orthographic — it claims nothing about sound.
//   Phonetic  — the pronunciations agree. This is a GUESS about
//               identity, not evidence of it: Tibetan names are
//               homonym-dense and two different people can sound
//               identical. It must look weaker than the tiers above.
//   PhoneticNear — the pronunciations are CLOSE but not equal (a
//               bounded edit distance). Weakest of all; the distance
//               travels in the evidence so a reader can judge it.
//
// A phonetic query is typed in ENGLISH PHONETICS ("tsongkapa"), not in
// wylie, so the query is folded BOTH ways — raw, and through
// pronounce() for someone who typed wylie — and either may match.
// Feeding a phonetic spelling through pronounce() as if it were wylie
// yields garbage: "tsongkapa" came back "tsong", which then
// substring-matched an unrelated name. Short folds are refused for
// exactly that reason.
//
// Homonyms are NEVER merged. Every banked author that matches comes
// back as its own hit, so a human disambiguates rather than the
// machine picking (inviolable rule 3).
//
// Canonical space follows the handbook's §8 duality rule: ACIP is
// defined uppercase, scholarly wylie lowercase, so a query carrying an
// ASCII uppercase letter is ACIP and goes through acipToEwts. Tibetan
// unicode is reversed with unicodeToWylie first.
#pragma once

#include <string>
#include <vector>

namespace allcore {

enum class AuthorTier {
    Exact = 0,
    Contains = 1,
    Spacing = 2,       // same letters, different syllable spacing
    Phonetic = 3,
    PhoneticNear = 4,
};

// A fold shorter than this is never used for substring or
// near matching — short strings match everything.
inline constexpr size_t kMinPhoneticFold = 4;

// The same floor for the orthographic Contains tier, in canonical
// space. Found by the battery: a one-letter query substring-matched
// nearly every author, because the length floor guarded only the
// phonetic path. Exact matching is deliberately NOT floored — an
// author genuinely named with one syllable must still be findable.
inline constexpr size_t kMinContainsLen = 3;

struct AuthorHit {
    std::string author;    // the author string exactly as banked
    AuthorTier tier = AuthorTier::Exact;
    std::string evidence;  // why it matched, for honest display
};

// Normalize one name into canonical space: dates stripped, whitespace
// collapsed, ACIP folded to EWTS, lowercased. Exposed for the battery
// and for callers that want to key their own indexes the same way.
std::string authorCanonical(const std::string& name);

// The pronunciation fold used by the Phonetic tier: pronounce() with
// spaces removed. Empty when the name yields no pronunciation.
std::string authorPhoneticFold(const std::string& name);

// Rank `authors` against `query`. Best tier per author, Exact first,
// then Contains, then Phonetic; alphabetical within a tier. An empty
// or whitespace-only query returns nothing.
std::vector<AuthorHit> matchAuthors(const std::string& query,
                                    const std::vector<std::string>& authors);

}  // namespace allcore
