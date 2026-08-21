// allcore/catalog_name.h — the filename composer + META convention
// (TODO 9g 2b item 3). Sessions 4-5 and May 5 fixed the grammar:
//
//   NUMBER_TIBETAN TITLE_ENGLISH TITLE_AUTHOR (DATES).TXT
//
// with capital .TXT, no colons (a single hyphen, no spaces, stands in),
// BZHUGS SO / trailing "nama" never part of the title, no diacritics. At
// the filesystem's name limit the name is filled TO THE MAX and stopped
// EVEN MID-WORD, a "+" is appended, and a companion "NUMBER META.TXT" in
// the same folder carries the continuation from the exact cut point — so
// a future rejoin is pure concatenation.
//
// GROUND TRUTH: the installed library itself — 1,457 "+.TXT" names with
// 1,468 "NUMBER META.TXT" companions (legacy METAs are RTF from the Word
// era; the composer writes plain ASCII per the plain-text doctrine, and
// the battery reads both). The registrar supplies the number as free
// text: THIS MODULE NEVER MINTS NUMBERS.
#pragma once

#include <string>

namespace allcore {

// Make one field safe for the filename grammar: colons become a single
// hyphen (no spaces), underscores (the field separator) become spaces,
// common IAST diacritics fold to ASCII, control characters and path
// separators are dropped, whitespace collapses. Returns the cleaned field.
std::string sanitizeNameField(const std::string& field);

// Strip a trailing "BZHUGS SO" (and its punctuation) from a Tibetan title
// field — it means "this book resides here" and is never part of the name.
std::string stripBzhugsSo(const std::string& tibetan_title);

struct ComposedName {
    std::string filename;       // the (possibly truncated) name, with ext
    bool truncated = false;
    std::string meta_filename;  // "NUMBER META.TXT" when truncated
    std::string meta_content;   // the continuation from the exact cut
    std::string full_stem;      // the untruncated logical name (no ext)
};

// Compose per the grammar. Empty fields are skipped (with their
// separator). max_name is the filesystem's limit for the whole filename
// including extension (255 on APFS/ext4/NTFS).
ComposedName composeCatalogFilename(const std::string& number,
                                    const std::string& tibetan_title,
                                    const std::string& english_title,
                                    const std::string& author,
                                    const std::string& ext = ".TXT",
                                    size_t max_name = 255);

// The rejoin: a truncated stem ending in '+' plus its META continuation
// gives back the full logical name. Pure concatenation, per the May 5
// ruling (mid-word stop, no padding spaces).
std::string rejoinMetaName(const std::string& truncated_stem,
                           const std::string& meta_continuation);

// Extract the visible text from a legacy RTF META file (the Word-era
// companions); plain-text content is returned as-is (trimmed).
std::string metaContinuationText(const std::string& file_content);

}  // namespace allcore
