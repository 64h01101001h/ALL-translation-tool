// allcore/speak.h — reading Tibetan aloud from the Kawachen recordings.
//
// The bank is 2,970 recordings of single Tibetan syllables spoken by native
// speakers, plus 32 recordings naming the orthographic components. It covers
// 99.3% of the syllables in our 42,199-segment corpus.
//
// WHAT THIS IS AND IS NOT. Playing these in sequence gives syllable-accurate
// reading aloud in a real human voice. It is NOT a native speaker reading a
// sentence: each recording carries its own pitch contour and final release,
// and connected Tibetan has phrase-level prosody these atoms cannot supply.
// That is not a defect to be hidden — reading syllable by syllable is how
// Tibetan is taught (sbyor klog) — but the interface must never imply
// fluent speech, and every caller gets a Tier saying how exact the match is.
//
// LICENSING. Kawachen granted in-house use, not redistribution. The bank is a
// local asset that is never bundled into anything shipped. available() is
// false when it is absent, and every surface must degrade quietly when it is.
#pragma once

#include <map>
#include <string>
#include <vector>

namespace allcore {

// How well a recording matches the syllable asked for. Anything short of
// exact must reach the user as a label, not be smoothed away.
enum class SpeakTier {
    None = 0,      // we hold no recording; say so, never approximate further
    Exact,         // the bank says exactly this syllable
    SilentDropped, // a post-suffix or wa-zur was removed first — still exact,
                   // because Tibetan does not pronounce either
    BaseEnding,    // a genitive contraction, played as base + ending
    Reduced,       // a subscript was dropped to find a match: APPROXIMATE
};

const char* tierLabel(SpeakTier t);

struct SpokenSyllable {
    std::string wylie;               // the syllable asked for
    std::vector<std::string> files;  // absolute paths, played in order
    SpeakTier tier = SpeakTier::None;
};

// The recordings, loaded from a manifest produced by tools/kawachen_harvest.py.
// Construction is cheap to fail: an absent bank is the normal state of a
// public build, not an error.
class SpeakBank {
public:
    // `root` is the directory holding manifest.csv; empty means the default
    // location beside the repository's data/.
    explicit SpeakBank(const std::string& root = std::string());

    /// True only when a recording actually opens on disk.
    bool available() const { return available_; }
    /// True when the label record loaded, whether or not the audio
    /// is present — a public build can tell the whole truth about
    /// what the bank contains while playing none of it.
    bool manifestLoaded() const { return recordings_ > 0; }
    int recordings() const { return recordings_; }
    int readings() const { return (int)byReading_.size(); }
    const std::string& root() const { return root_; }

    // One syllable of wylie -> the recordings that say it.
    SpokenSyllable resolve(const std::string& wylie) const;

    // A passage -> one entry per syllable, in order, including the ones we
    // cannot say (tier None), so a caller can show the gaps rather than
    // silently skipping them.
    std::vector<SpokenSyllable> readAloud(const std::string& wylie) const;

    // The recording that NAMES an orthographic component ("ga sngon jug",
    // "gi gu", "ya btags"), for spelling a syllable aloud the way it is
    // taught. Empty when we hold no such recording.
    std::string termFile(const std::string& termWylie) const;

private:
    std::string lookup(const std::string& syl) const;

    bool available_ = false;
    int recordings_ = 0;
    // one real path, so available() can prove a file opens rather
    // than trust a row count
    std::string firstPath_;
    std::string root_;
    // reading -> file. Several spellings share one reading; the first wins,
    // which is correct because by construction they are the same sound.
    std::map<std::string, std::string> byReading_;
    std::map<std::string, std::string> endings_;   // 'i, 'u, 'o
    std::map<std::string, std::string> terms_;     // component names
};

}  // namespace allcore
