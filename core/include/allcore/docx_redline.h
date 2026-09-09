// allcore/docx_redline.h — the tracked-changes .docx writer (analysis-suite
// plan F6). Created 2026-09-09.
//
// Turns a textdiff::Result into a WordprocessingML document whose
// differences are real Word revision marks (w:ins / w:del), so Geshe
// Michael can accept or reject each change in Word itself. Pure C++: no
// Qt, no third-party, no zlib — the container is allcore::zipstore.
//
// The honesty rules this writer is built to keep:
//  * The FIRST paragraph is an untracked provenance note naming the two
//    drafts, the rules in force, the counts, and — in plain words — that
//    these marks are machine-computed and are not an editor's edits.
//  * Minor differences under the rules in force are ALWAYS marked and
//    counted in the note. A redline that quietly omits differences is a
//    lie to the reviewer, so there is one policy here, not a checkbox.
//  * No gloss or dictionary material is ever written (rule 1): the file
//    holds the two texts' lines and the note, nothing else.
//  * Tibetan is exported as it stands in the sources; nothing is
//    converted. Runs carrying U+0F00–U+0FFF get the complex-script font
//    slot (w:rFonts w:cs + w:cs), which is what Word actually consults for
//    Tibetan shaping; the note names the font.
//  * Control characters that Word cannot hold become U+FFFD and are
//    COUNTED, never dropped silently.
#pragma once

#include <string>
#include <vector>

#include "allcore/textdiff.h"

namespace allcore {
namespace docx {

struct RedlineOptions {
    // The revision author Word shows on every balloon. Use
    // "<name> via Diamond Cutter compare" — a bare person's name would
    // present machine hunks as that person's own edits.
    std::string author;
    // ISO 8601. A bare "2026-09-08" is widened to "2026-09-08T00:00:00Z";
    // a stamp without the zone gets "Z". Word wants the zone.
    std::string dateIso;
    std::string aName, bName;   // the two drafts, as the note should name them
    // The rules in force, as a phrase — textdiff::optionsDescription(o) is
    // the intended source. Empty means "none". The writer wraps this into
    // the full note because the note's counts are only known once the body
    // has been written.
    std::string provenance;
    std::string title;          // docProps/core.xml dc:title; may be empty
    std::string tibetanFont = "Noto Serif Tibetan";
    // Refine 1:1 Change hunks to the differing spans (syllables for
    // Tibetan, words for English). False marks whole lines instead.
    bool inlineRefine = true;
};

struct RedlineStats {
    int insertions = 0;       // w:ins runs written (paragraph marks not counted)
    int deletions = 0;        // w:del runs written
    int minorMarked = 0;      // minor hunks marked as revisions anyway
    int comparedRaw = 0;      // lines the script-agnostic rule could not convert
    int moved = 0;            // moved blocks (pairs), exported as delete + insert
    int unrepresentable = 0;  // control characters replaced by U+FFFD
};

// Escapes &, <, > and " and replaces control characters other than tab by
// U+FFFD. Public because the callers building the note and the file name
// need the same rule.
std::string xmlEscape(const std::string& s);

// The body. One w:p per line; the first is the untracked provenance note.
// st may be null.
std::string documentXml(const std::vector<std::string>& a, const std::vector<std::string>& b,
                        const textdiff::Result& r, const RedlineOptions& o, RedlineStats* st);

std::string stylesXml(const RedlineOptions& o);
std::string settingsXml();
std::string coreXml(const RedlineOptions& o);
std::string appXml();
std::string contentTypesXml();
std::string relsXml();
std::string documentRelsXml();

// The whole package: the eight parts, STORED, deterministic. st may be null.
std::string buildRedlineDocx(const std::vector<std::string>& a, const std::vector<std::string>& b,
                             const textdiff::Result& r, const RedlineOptions& o,
                             RedlineStats* st);

}  // namespace docx
}  // namespace allcore
