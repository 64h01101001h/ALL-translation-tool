// allcore/stardict.h — a reader for the StarDict dictionary format
// (.ifo + .idx + .dict[.dz]) — the ecosystem's dominant local
// dictionary container (GoldenDict, Christian Steinert's payloads).
//
// Scope (honest): READ-ONLY format support for USER-SUPPLIED local
// dictionaries. The reader confers no license — each dictionary's
// terms stay whatever they are, and the Lookup pane labels every
// StarDict layer as user-supplied/local-only. 32-bit .idx offsets
// only (idxoffsetbits=64 is refused, not guessed); .dict.dz is
// decompressed whole via zlib (dictzip is gzip-compatible for full
// reads). sametypesequence text types are rendered as UTF-8 text;
// typed blocks without a sametypesequence keep their lowercase
// (null-terminated) text segments and skip binary ones.
#pragma once

#include <string>
#include <vector>

namespace allcore {

class StarDict {
public:
    // Opens book.ifo (+ .idx + .dict/.dict.dz beside it).
    // ok() reports success; error() says why not.
    explicit StarDict(const std::string& ifo_path);

    bool ok() const { return ok_; }
    const std::string& error() const { return error_; }
    const std::string& bookname() const { return bookname_; }
    size_t wordCount() const { return entries_.size(); }

    // case-insensitive exact match; every matching entry's
    // definition text (a word may repeat in the index)
    std::vector<std::string> lookup(const std::string& word) const;

    // headwords beginning with the (case-insensitive) prefix,
    // in index order, capped
    std::vector<std::string> prefix(const std::string& p,
                                    int limit = 8) const;

private:
    struct Entry {
        std::string word;
        uint32_t off = 0, size = 0;
    };
    std::string defText(const Entry& e) const;

    bool ok_ = false;
    std::string error_;
    std::string bookname_;
    std::string sametypesequence_;
    std::vector<Entry> entries_;      // index order (file order)
    std::vector<size_t> sorted_;      // entry ids by lowercase word
    std::string dict_;                // whole .dict, decompressed
};

}  // namespace allcore
