// allcore/idioms.h — the idioms register and its matcher (2026-09-09,
// Adam: "a bank of previously used idioms, and a way of adding new ones
// that need to be approved by Geshe Michael").
//
// What this module is, and what it deliberately is not:
//
// The register marks WHICH Tibetan strings are idioms — fixed expressions
// that must not be read word by word. It carries NO English of its own.
// English belongs to Geshe Michael (rule 1): the app may MATCH Geshe Michael's English
// from corpus evidence, never COMPOSE it. So an entry here is a Tibetan
// form, a status, and the evidence for it — nothing more. The renderings a
// translator sees beside an idiom come from the parallel corpus and the
// dictionary, each with its own count and tier, fetched at the moment of
// asking and never copied into this list.
//
// An entry's status is the authority's ruling, not the app's opinion:
//   proposed  — put forward, awaiting Geshe Michael's ruling
//   approved  — ruled an idiom
//   declined  — ruled NOT an idiom; the app stops marking that form
// A declined form is kept, because "we asked and the answer was no" is
// worth more than silence: it stops the same proposal coming back.
//
// The matcher is a longest-match walk over a syllable sequence, so the
// substring accidents of a character search (pa inside pad ma) cannot
// happen. It is deliberately small: the register holds tens to hundreds of
// forms. When the Entities automaton (batch 5 F6) lands, both should share
// its Aho–Corasick machine; the API here is shaped so that swap is internal.

#pragma once

#include <string>
#include <vector>

namespace allcore {
namespace idioms {

enum class Status { Proposed, Approved, Declined };

const char* statusName(Status s);
Status statusFromName(const std::string& s);   // unknown text reads as Proposed

// One registered form. Every field is provenance; none is a rendering.
struct Entry {
    std::string wylie;        // the idiom, in wylie, as registered
    Status status = Status::Proposed;
    std::string evidence;     // why it was put forward, in the proposer's words
    std::string proposedBy;   // who put it forward
    std::string proposed;     // ISO date it was put forward
    std::string ruledBy;      // who ruled (empty while proposed)
    std::string ruled;        // ISO date of the ruling
    std::string note;         // the authority's note with the ruling
    std::string source;       // which register file it came from
};

// One occurrence in a text, in syllable indices. The caller owns byte
// offsets and citations: this module never walks folios.
struct Hit {
    int begSyllable = 0, endSyllable = 0;   // [beg, end)
    std::string wylie;                       // the registered form that matched
    Status status = Status::Approved;
};

// Split a registered form into its matchable syllables: lower-cased,
// whitespace-separated, shad and punctuation dropped. The same rule the
// matcher applies to the text, so both sides are canonical in one place.
std::vector<std::string> splitPattern(const std::string& wylie);

class Register {
public:
    // Later wins by wylie, so a team register loaded after the shipped
    // seed overrides it — a ruling can never be undone by load order.
    void add(const Entry& e);
    void addAll(const std::vector<Entry>& v);
    void clear();

    const std::vector<Entry>& all() const { return items_; }
    const Entry* find(const std::string& wylie) const;   // exact, canonicalised
    std::size_t size() const { return items_.size(); }
    int countByStatus(Status s) const;

    // Every occurrence of an approved or proposed form, longest match first,
    // non-overlapping, in text order. Declined forms are NOT reported: they
    // were ruled ordinary language and marking them would nag.
    std::vector<Hit> findIn(const std::vector<std::string>& syllables) const;

    // The longest registered form that begins at `at`, or nullptr.
    const Entry* longestAt(const std::vector<std::string>& syllables, int at) const;

private:
    std::vector<Entry> items_;
    std::vector<std::vector<std::string>> patterns_;   // parallel to items_
};

}  // namespace idioms
}  // namespace allcore
