// allcore/comments.h — team comments on texts (9n-6).
//
// A translator reads a line and wants to say something to the team
// about THAT line of THAT text. The store is a TSV beside the shared
// proposals store, and it is APPEND-ONLY by design: comments are
// never rewritten, so two machines can never fight over the file —
// Dropbox conflicts reduce to a union, which load() performs over
// any "conflicted copy" siblings (same discipline as ProposalStore,
// stronger guarantee: no rewrite path exists at all).
//
// Identity is provenance, not a login (the Propose pane's rule).
#pragma once

#include <string>
#include <vector>

namespace allcore {

struct TextComment {
    std::string id;        // file|line|author|created|n — unique enough
    std::string file;      // basename of the text commented on
    int line = 0;          // 1-based line in that text
    std::string author;
    std::string created;   // ISO date-time, caller-supplied
    std::string text;
};

class CommentStore {
public:
    explicit CommentStore(const std::string& dir);
    // reads comments.tsv plus any Dropbox "conflicted copy" siblings
    // (union by id; the siblings stay on disk untouched)
    bool load();
    // append ONE row — never rewrites the file
    bool add(const std::string& file, int line,
             const std::string& author, const std::string& text,
             const std::string& createdIso);
    const std::vector<TextComment>& all() const { return items_; }
    std::vector<const TextComment*> byFile(
        const std::string& file) const;
    size_t conflictSiblings() const { return conflictFiles_; }

private:
    std::string dir_;
    std::vector<TextComment> items_;
    size_t conflictFiles_ = 0;
};

}  // namespace allcore
