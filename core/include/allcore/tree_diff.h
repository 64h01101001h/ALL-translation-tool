// allcore/tree_diff.h — the divergence audit (queue item 6).
//
// Session 3's live problem: GMR's copy of the database and Nick's
// Dropbox master "have diverged from each other probably" — the task he
// gave Jamie was to compare the two trees and list the differences, so
// Nick could confirm subject categories for anything one copy lacked.
// This is that comparison, for any two roots the two browsers hold.
//
// Classification per file (by path relative to each root):
//   identical — same relative path, same size, same sampled content
//   modified  — same relative path, different size or sampled content
//   renamed   — same size + sampled content, different relative path
//               (the file moved shelves — exactly what happens when one
//               copy reorganizes)
//   added     — only in the right tree;  removed — only in the left
//
// HONESTY TIER: "sampled content" is a hash of the first 4 KB + last
// 4 KB + size, not the full bytes — fast enough for whole databases,
// and the report says so. A cataloger confirming a difference opens the
// files; this list tells them where to look.
#pragma once

#include <string>
#include <vector>

namespace allcore {

struct TreeDiffResult {
    int files_left = 0, files_right = 0;
    int identical = 0;
    std::vector<std::string> added;      // relative paths, only right
    std::vector<std::string> removed;    // relative paths, only left
    std::vector<std::pair<std::string, std::string>> renamed;  // L -> R
    std::vector<std::string> modified;   // same path, different content
};

// Walk both roots recursively (every regular file) and classify.
TreeDiffResult diffTrees(const std::string& left_root,
                         const std::string& right_root);

}  // namespace allcore
