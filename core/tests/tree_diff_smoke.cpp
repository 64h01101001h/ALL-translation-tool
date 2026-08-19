// tree_diff_smoke — the divergence audit (queue item 6). Constructed
// fixtures prove each classification; the real-library half proves a
// self-diff is exactly clean and times the walk (the whole point is
// comparing two database copies).
//
// Usage: tree_diff_smoke <scratch_dir> [library_subtree]
#include <chrono>
#include <cstdio>
#include <filesystem>
#include <fstream>
#include <string>

#include "allcore/tree_diff.h"

static int failures = 0;
#define CHECK(cond, msg)                                        \
    do {                                                        \
        if (cond) std::printf("  [PASS] %s\n", msg);            \
        else { std::printf("  [FAIL] %s\n", msg); ++failures; } \
    } while (0)

namespace fs = std::filesystem;

static void put(const fs::path& p, const std::string& body) {
    fs::create_directories(p.parent_path());
    std::ofstream f(p, std::ios::binary);
    f << body;
}

int main(int argc, char** argv) {
    setvbuf(stdout, nullptr, _IONBF, 0);
    if (argc < 2) {
        std::printf("usage: tree_diff_smoke <scratch>\nFAILURES\n");
        return 1;
    }
    const fs::path scratch = fs::path(argv[1]) / "treediff_fixture";
    std::error_code ec;
    fs::remove_all(scratch, ec);

    // left: the "old copy"; right: the "current master"
    put(scratch / "L/shelfA/keep.txt", "same content here");
    put(scratch / "R/shelfA/keep.txt", "same content here");
    put(scratch / "L/shelfA/edited.txt", "the OLD wording of a text");
    put(scratch / "R/shelfA/edited.txt", "the NEW wording of a text!");
    put(scratch / "L/shelfA/moved.txt",
        "this text was refiled to another shelf entirely");
    put(scratch / "R/shelfB/moved_renamed.txt",
        "this text was refiled to another shelf entirely");
    put(scratch / "L/shelfC/gone.txt", "only the old copy has this");
    put(scratch / "R/shelfC/new_input.txt", "a newly input text");
    // ambiguity guard: identical twins on the right must not pair
    put(scratch / "L/twins/orig.txt", "twin twin twin");
    put(scratch / "R/twins/copy1.txt", "twin twin twin");
    put(scratch / "R/twins/copy2.txt", "twin twin twin");

    const auto r = allcore::diffTrees((scratch / "L").string(),
                                      (scratch / "R").string());
    CHECK(r.files_left == 5 && r.files_right == 6,
          "both trees fully walked");
    CHECK(r.identical == 1, "the unchanged file is identical");
    CHECK(r.modified.size() == 1 && r.modified[0] == "shelfA/edited.txt",
          "same path, different content -> modified");
    CHECK(r.renamed.size() == 1 &&
              r.renamed[0].first == "shelfA/moved.txt" &&
              r.renamed[0].second == "shelfB/moved_renamed.txt",
          "same content on a new shelf -> renamed (the refiling case)");
    bool goneOk = false, newOk = false;
    for (const auto& s : r.removed)
        if (s == "shelfC/gone.txt") goneOk = true;
    for (const auto& s : r.added)
        if (s == "shelfC/new_input.txt") newOk = true;
    CHECK(goneOk, "only-left -> removed");
    CHECK(newOk, "only-right -> added");
    CHECK(r.removed.size() == 2 && r.added.size() == 3,
          "ambiguous twins stay added/removed - never guessed into a "
          "rename");
    fs::remove_all(scratch, ec);

    if (argc >= 3) {
        const auto t0 = std::chrono::steady_clock::now();
        const auto self = allcore::diffTrees(argv[2], argv[2]);
        const auto ms =
            std::chrono::duration_cast<std::chrono::milliseconds>(
                std::chrono::steady_clock::now() - t0)
                .count();
        std::printf("  self-diff: %d files in %lld ms\n", self.files_left,
                    static_cast<long long>(ms));
        CHECK(self.files_left > 500 &&
                  self.identical == self.files_left &&
                  self.added.empty() && self.removed.empty() &&
                  self.renamed.empty() && self.modified.empty(),
              "a tree diffed against itself is exactly clean");
    }

    std::printf("%s\n", failures ? "FAILURES" : "tree_diff_smoke OK");
    return failures ? 1 : 0;
}
