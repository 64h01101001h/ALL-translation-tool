// allcore/reader.h — the Translation Trainer's deterministic reading layer
// (docs/TRAINER_DESIGN.md). Increment 1: clause segmentation.
//
// Splits a tokenized ACIP passage into clauses at punctuation barriers and at
// clause-level particles (Wilson's connectives): semi-final ste/te,
// conditional na, concessive kyang, coordinating cing/zhing/shing, quotative
// ces/zhes/shes, and the suffix-echo final particles (go ngo do no bo mo 'o
// ro lo so to). Everything is a rule table; where a form is ambiguous with a
// lexical word the splitter demands corroboration (the echo rule) or leaves
// it alone — flags, never guesses.
#pragma once

#include <string>
#include <vector>

#include "allcore/lattice.h"
#include "allcore/verbclass.h"

namespace allcore {

struct Clause {
    int beg = 0;                     // token span [beg, end)
    int end = 0;
    std::string boundary;            // lowercase wylie of what ended the clause:
                                     // "ste", "na", "cing", "so", … or
                                     // "barrier" (shad/comma/line) or "" (end
                                     // of text)
    const char* boundary_function = "";  // human label for the boundary
};

// The declarative final particle echoes the preceding suffix (…s so, …ng ngo,
// vowel 'o). Returns true only when `tok` is the echo that the suffix of
// `prev` licenses — which is exactly what disambiguates particle SO from the
// noun so "tooth", lo "year", mo "she", etc.
bool isFinalParticle(const std::string& prev_acip, const std::string& tok_acip);

// Allomorph licensing for the cing/zhing/shing and ces/zhes/shes
// families (Wilson's tables): cing/ces after final g d b;
// zhing/zhes after ng n m ' r l and vowels; shing/shes after s.
// Returns: 0 = correct allomorph for the preceding final,
// 1 = mismatched (the string names the expected form via
// expectedAllomorph), -1 = tok is not in these families.
int connectiveAllomorphCheck(const std::string& prev_acip,
                             const std::string& tok_acip);
// the form the preceding final licenses ("CING"/"ZHING"/"SHING"
// or "CES"/"ZHES"/"SHES"); empty when tok is not in the families
std::string expectedAllomorph(const std::string& prev_acip,
                              const std::string& tok_acip);

// Split a tokenized document (tokens + barrier flags from tokenizeDocument)
// into clauses.
std::vector<Clause> splitClauses(const std::vector<std::string>& tokens,
                                 const std::vector<bool>& barrier_after);

// na is conditional ("if/when") after a verb but locative ("in/at") after a
// noun. Re-examine every na clause boundary: keep the split only when the
// word before na carries HGM verb evidence (tenses or a "to …" gloss);
// otherwise merge with the following clause so the chunker can mark na as the
// locative it is. Data-anchored disambiguation, no guessing.
std::vector<Clause> refineClauses(const OverlayDoc& doc,
                                  std::vector<Clause> clauses);

// ---- increment 2: phrase chunks, verb, reading order -----------------------

struct Chunk {
    int beg = 0, end = 0;      // token span within the document
    std::string marker;        // lowercase wylie of the role marker ("" = none)
    bool marker_fused = false; // marker was fused into the final word (pa'i…)
    const char* role = "";     // Wilson role label
};

// Cut one clause into phrases: a chunk closes after each freestanding case
// particle, and after a word the lattice matched THROUGH a fused ending
// (span.clitic — dictionary-anchored, so lexical las/nas/gnas are never
// misread as inflected). The final chunk is the predicate slot.
std::vector<Chunk> chunkClause(const OverlayDoc& doc, const Clause& clause);

struct VerbGuess {
    int chunk = -1;            // index into the chunk list (-1 = none found)
    int tok = -1;              // token index where the verb word starts
    std::string wylie;         // matched entry headword
    std::string evidence;      // "tenses: …" or "gloss: to …" or "" (unverified)
    bool confident = false;    // true = HGM data marks it a verb
    const VerbClassInfo* cls = nullptr;  // Wilson class, when the tables know it
};

// Wilson step 1: find the clause verb. Search the final chunk right-to-left
// for a word whose HGM entry carries tense forms or a "to …" gloss. If none,
// the last dictionary word is reported as an UNVERIFIED candidate (flagged,
// never guessed — rule 3).
VerbGuess spotVerb(const OverlayDoc& doc, const std::vector<Chunk>& chunks);

struct PlanStep {
    int chunk = -1;
    int order = 0;             // 1-based suggested reading order; 0 = attaches
                               // to the following chunk (genitive, dang)
    const char* how = "";      // guidance text for the learner
};

// Wilson reading order over the chunk roles: verb first (it frames the
// clause), then topic, agent, unmarked object/complement, la don,
// source. Genitive and dang chunks get order 0: they read together with the
// chunk that follows ("the Y of X" — right to left).
std::vector<PlanStep> planReading(const std::vector<Chunk>& chunks,
                                  const VerbGuess& verb);

}  // namespace allcore
