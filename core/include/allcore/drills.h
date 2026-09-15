// allcore/drills.h — the corpus drill generator (docs/PEDAGOGY_ROADMAP.md,
// build-order item 1).
//
// Every drill is generated from a real corpus segment and its answer key is
// HGM's own data: the segment's actual chunk order, its actual particle, its
// actual English. The engine invents questions, never answers. Drills draw
// segments with a caller-provided RNG so sessions are seedable (tests use a
// fixed seed; the GUI seeds from the clock).
#pragma once

#include <optional>
#include <random>
#include <string>
#include <vector>

#include "allcore/lattice.h"
#include "allcore/progress.h"
#include "allcore/reader.h"
#include "allcore/spine.h"

namespace allcore {

// ---- Targeting, for the weak-spot loop (docs/LEARN_TAB_VISION.md,
// "Train this"). A drill can be asked to exercise ONE named skill, so a
// diagnosed weakness can be trained rather than merely counted.
//
// The skill names are the same strings the miss taxonomy already files
// under, so a weakness recorded as "miss:cloze-role:agent" targets as
// DrillTarget{Kind::ClozeRole, "agent"} with no translation table between
// them — a table is a place for the two to drift apart.
struct DrillTarget {
    enum class Kind { None, ClozeRole, ParticleFamily, OrderSkill };
    Kind kind = Kind::None;
    std::string skill;                  // e.g. "agent", "gi-family"
};

// Two cloze roles are REFUSED as targets, and the refusal is not a
// limitation to be worked around. makeCloze deliberately blanks a MARKED
// chunk, because its role marker is what makes the question fairly
// answerable from the English. Targeting "unmarked" or "predicate" forces
// questions the generator's own code says are unfair, and a weakness
// measured on them would be an artefact of the generator rather than
// anything about the learner. A caller asking for one is told why.
bool targetRefused(const DrillTarget& t, std::string* why = nullptr);



// Restore the chunks of a real clause to their original order.
struct OrderDrill {
    CorpusSegment segment;              // answer context (HGM English inside)
    std::vector<std::string> chunks;    // ACIP chunk texts, CORRECT order
    std::vector<std::string> markers;   // role marker per chunk ("" = none) —
                                        // feeds the miss taxonomy
    std::vector<int> presented;         // presentation order: indices into
                                        // `chunks`, shuffled (never identity)
    std::string boundary;               // the clause boundary, for display
    std::vector<PlanStep> plan;         // reading-order guidance (labeled
                                        // guidance, revealed after checking)
    VerbGuess verb;
};

// One chunk of the Tibetan is blanked; pick it from four options.
struct ClozeDrill {
    CorpusSegment segment;
    std::vector<std::string> chunks;    // clause chunks with [...] at blank
    int blank = 0;                      // which chunk is blanked
    std::vector<std::string> options;   // 4 ACIP options, shuffled
    int correct = 0;                    // index into options
    std::string role;                   // the blanked chunk's role label
};

// A variant-selecting particle is blanked; pick the right family member.
struct ParticleDrill {
    CorpusSegment segment;
    std::vector<std::string> tokens;    // segment tokens, blank replaced by ▢
    int blank_tok = 0;
    std::vector<std::string> options;   // the family variants (wylie)
    int correct = 0;
    std::string explanation;            // agreement-table explanation
};

// ---- Boundary Hunt (docs/LEARN_TAB_VISION.md) ----
//
// The segment with its punctuation stripped: mark where the clauses end.
//
// Why it is worth a mode of its own: we measured that clause ORDER survives
// translation — adjacent clause pairs are reversed in Geshe Michael's English only 4.8%
// of the time, against 49.0% for adjacent word pairs. Find the clause
// boundaries and most of the corpus becomes readable left-to-right at clause
// granularity. The generator is `refineClauses`, which already exists.
//
// Two pools, kept apart because they are different skills. A segment whose
// source carried a shad or a comma is the confidence-building pool: the
// learner is recovering a boundary the scribe already marked. A segment with
// no punctuation at all is the real skill — nothing on the page tells you
// where the clause ends except the particle.
//
// UNSCORED POSITIONS. `refineClauses` rules on every ambiguous `na`:
// conditional ("if/when") after a verb, locative ("in/at") after a noun. When
// it splits, it does so on POSITIVE evidence — the preceding word carries HGM
// verb evidence, or is the nominalizer pa/ba. When it merges, it does so on
// the ABSENCE of that evidence, which is a much weaker thing and is not the
// same as knowing the word is a noun. Those merge positions are reported in
// `unscored` and are neither right nor wrong: a learner who marks one is not
// told they are wrong, because we do not know that they are.
struct BoundaryDrill {
    CorpusSegment segment;
    std::vector<std::string> tokens;     // ACIP tokens, punctuation stripped
    std::vector<int> ends;               // mark AFTER token i — the answer key
    std::vector<std::string> functions;  // parallel to `ends`: what ended it
    // Parallel to `ends`. TRUE when the scribe marked that boundary himself
    // (a shad or comma stood there before stripping) — the key is attested.
    // FALSE when the splitter ruled from the particle — the key is ENGINE
    // GUIDANCE and must be labelled as such wherever it is shown. In the
    // unpunctuated pool every end is false by construction, which is the
    // honest price of that pool being the real skill.
    std::vector<bool> attested;
    std::vector<int> unscored;           // merged-na positions; see above
    bool punctuated = false;             // which pool this came from
};

// ---- where the blank goes -------------------------------------------------
//
// A cloze shows the WHOLE segment with one chunk blanked, which means finding
// that chunk's position in the segment's own ACIP. It is not a plain find:
// a short phrase often repeats earlier in the same segment, so the search
// starts at the clause's own position and only falls back to the first
// occurrence.
//
// Lifted here on 2026-09-11 because it was written twice — once in the app and
// once in the pack builder — and the two had already drifted apart on the case
// that matters. The app explained itself and fell back to showing the clause;
// the builder dropped the drill with no record. Two copies of a rule cannot
// stay honest about their own failures, and this one's failure IS the
// interesting part: the answer not appearing verbatim in the segment means
// the chunker and the segment disagree, and that is worth counting.
struct BlankSplit {
    bool ok = false;
    std::string before;   // segment ACIP before the blank
    std::string after;    // segment ACIP after it
    // Why not, in words a surface can show. Never a guessed position: a blank
    // placed in the wrong place teaches the wrong sentence.
    std::string why;
};

BlankSplit placeBlank(const std::string& segment_acip,
                      const std::vector<std::string>& chunks,
                      const std::string& answer);

// The skill a drill exercises, in the same vocabulary as the target. Used to
// tag HITS as well as misses: until now only misses carried a skill, so
// per-skill accuracy had no denominator and no trend could honestly be drawn.
std::string clozeSkill(const ClozeDrill& d);
std::string particleSkill(const ParticleDrill& d);


// ---- The Debate Dojo (docs/LEARN_TAB_VISION.md) ----
//
// A formal Tibetan debate statement has three parts, and they are marked:
//
//   <subject> CHOS CAN,  <consequence> THAL,  <reason> PHYIR
//   "take <subject>:     it follows that <consequence>,  because <reason>"
//
// and the reply is drawn from a CLOSED set, each answer attacking a
// different element:
//
//   MA GRUB NA        the subject is not established
//   RTAGS MA GRUB     the reason is not established
//   MA KHYAB NA       the reason does not entail the consequence
//   RTZA BAR 'DOD NA  accept it, and the root claim with it
//
// That closed set is what makes debate drillable at all: the answer space is
// fixed by the tradition rather than invented here.
struct DebateStatement {
    bool ok = false;            // the full template was found
    // Anything before the subject: a conditional, a citation, a scene-setter.
    // Kept rather than swallowed into the subject, because "if X is not a
    // valid perception, then take Y" has a subject of Y, not of the whole
    // sentence — and a drill that highlighted the whole sentence as the
    // subject would teach the template wrong.
    std::string preamble;
    std::string subject;        // the clause immediately before CHOS CAN
    std::string consequence;    // between CHOS CAN and THAL
    std::string reason;         // between THAL and PHYIR
};

// Split a real ACIP statement into its three elements. Returns ok=false and
// nothing else when the template is absent — never a partial guess at which
// span is which.
DebateStatement parseDebate(const std::string& acip);

// Which element a legal reply attacks: "subject", "reason", "entailment",
// "accept", or empty when the reply is not one of the four.
std::string debateReplyTarget(const std::string& reply);

class DrillFactory {
public:
    // With a Progress and adaptive on, segment draws prefer material at the
    // learner's level: known-vocabulary coverage near the sweet spot (hard
    // enough to teach, familiar enough to attempt), and sometimes a segment
    // they previously peeked at (resurfacing). Without a Progress the draws
    // are uniform — and fully seed-reproducible either way.
    DrillFactory(const Spine& spine, const HeadwordIndex& index,
                 const Progress* progress = nullptr)
        : spine_(spine), index_(index), progress_(progress) {}

    // A drill must come from teaching material: not the title catalogue
    // (TITL*, AUTH, SUBJ) and not a segment whose Tibetan field holds English.
    static bool isDrillable(const CorpusSegment& seg);

    void setAdaptive(bool on) { adaptive_ = on; }

    // Each returns nullopt only if no suitable segment was found within the
    // attempt budget (rare; caller may simply try again).
    std::optional<OrderDrill> makeOrder(std::mt19937& rng) const;
    // A targeted draw that cannot meet its target returns nullopt rather
    // than an off-target drill labelled as training that skill — the
    // caller must be able to say "could not find one" honestly.
    std::optional<ClozeDrill> makeCloze(std::mt19937& rng,
                                        const DrillTarget& target =
                                            DrillTarget{}) const;
    std::optional<ParticleDrill> makeParticle(
        std::mt19937& rng,
        const DrillTarget& target = DrillTarget{}) const;
    // `want_unpunctuated` draws from the harder pool. Returns nullopt rather
    // than silently handing back a punctuated segment when the harder pool
    // has nothing in the attempt budget — the caller must be able to say
    // which pool the learner actually drilled.
    std::optional<BoundaryDrill> makeBoundary(
        std::mt19937& rng, bool want_unpunctuated = false) const;

    // A level-appropriate segment for free-form exercises (the
    // translate-and-compare workbench draws these directly).
    CorpusSegment pickSegment(std::mt19937& rng) const {
        return randomSegment(rng);
    }

private:
    CorpusSegment randomSegment(std::mt19937& rng) const;
    CorpusSegment uniformSegment(std::mt19937& rng) const;
    const Spine& spine_;
    const HeadwordIndex& index_;
    const Progress* progress_ = nullptr;
    bool adaptive_ = false;
};

}  // namespace allcore
