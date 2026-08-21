// allcore/glossary.h — the per-text glossary store (Lodestar).
//
// A translator's own equivalents for ONE text — binding for that
// text's display, filed as proposals when offered to the authority.
// Extracted from the Overlay's private pair so the card, the
// Glossary Workbench, and future Draft integration share one truth.
// Format (unchanged from the Overlay era): a # header line, then
// "wylie<TAB>gloss" per row.
#pragma once

#include <map>
#include <string>

namespace allcore {

class GlossaryStore {
public:
    explicit GlossaryStore(const std::string& path);
    bool load();   // missing file = empty store, true
    // docName appears in the header comment (provenance of purpose)
    bool save(const std::string& docName) const;
    void set(const std::string& wylie, const std::string& gloss);
    bool remove(const std::string& wylie);
    std::string get(const std::string& wylie) const;
    const std::map<std::string, std::string>& all() const {
        return items_;
    }

private:
    std::string path_;
    std::map<std::string, std::string> items_;
};

}  // namespace allcore
