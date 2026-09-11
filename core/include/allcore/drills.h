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

// The skill a drill exercises, in the same vocabulary as the target. Used to
// tag HITS as well as misses: until now only misses carried a skill, so
// per-skill accuracy had no denominator and no trend could honestly be drawn.
std::string clozeSkill(const ClozeDrill& d);
std::string particleSkill(const ParticleDrill& d);

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
