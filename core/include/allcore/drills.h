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

    void setAdaptive(bool on) { adaptive_ = on; }

    // Each returns nullopt only if no suitable segment was found within the
    // attempt budget (rare; caller may simply try again).
    std::optional<OrderDrill> makeOrder(std::mt19937& rng) const;
    std::optional<ClozeDrill> makeCloze(std::mt19937& rng) const;
    std::optional<ParticleDrill> makeParticle(std::mt19937& rng) const;

private:
    CorpusSegment randomSegment(std::mt19937& rng) const;
    CorpusSegment uniformSegment(std::mt19937& rng) const;
    const Spine& spine_;
    const HeadwordIndex& index_;
    const Progress* progress_ = nullptr;
    bool adaptive_ = false;
};

}  // namespace allcore
