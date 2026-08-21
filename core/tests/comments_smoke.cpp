// comments_smoke — the team-comments store: append-only round trip,
// escaping, two-writers union, conflicted-copy absorption.
// Self-cleaning at entry and exit (constitution R6).
#include <cstdio>
#include <filesystem>
#include <string>

#include "allcore/comments.h"

namespace fs = std::filesystem;

static int failures = 0;
#define CHECK(cond, what)                                   \
    do {                                                    \
        const bool ok = (cond);                             \
        std::printf("  [%s] %s\n", ok ? "PASS" : "FAIL",    \
                    what);                                  \
        if (!ok) ++failures;                                \
    } while (0)

int main() {
    const fs::path dir =
        fs::temp_directory_path() / "all_comments_smoke";
    std::error_code ec;
    fs::remove_all(dir, ec);
    fs::create_directories(dir);
    const std::string d = dir.string();

    {
        allcore::CommentStore a(d);
        a.load();
        CHECK(a.add("S0134I.txt", 12, "Tenzin",
                    "check this rendering\twith a tab",
                    "2026-08-21T09:00"),
              "first comment appends");
        CHECK(a.add("S0134I.txt", 40, "Dorje", "second\nline",
                    "2026-08-21T09:05"),
              "second comment appends");
    }
    {
        // a SECOND writer that never saw the first two — append-only
        // means its add cannot erase them
        allcore::CommentStore b(d);
        CHECK(b.load(), "reload sees the file");
        CHECK(b.all().size() == 2, "both comments round-trip");
        CHECK(b.all()[0].text.find('\t') != std::string::npos,
              "tab survives escaping");
        CHECK(b.all()[1].text.find('\n') != std::string::npos,
              "newline survives escaping");
        CHECK(b.add("OTHER.txt", 1, "Pema", "third",
                    "2026-08-21T10:00"),
              "second writer appends");
    }
    {
        allcore::CommentStore c(d);
        c.load();
        CHECK(c.all().size() == 3, "append-only union holds");
        CHECK(c.byFile("S0134I.txt").size() == 2,
              "byFile filters correctly");
        // Dropbox conflict drill: a sibling with one extra row
        fs::copy_file(dir / "comments.tsv",
                      dir / "comments (x's conflicted copy "
                            "2026-08-21).tsv",
                      ec);
        allcore::CommentStore x(d);
        x.load();
        // sibling rows all duplicate ids → union adds nothing
        CHECK(x.all().size() == 3 && x.conflictSiblings() == 1,
              "conflicted sibling detected, duplicates not doubled");
    }

    fs::remove_all(dir, ec);
    std::printf("comments_smoke: %s (%d failures)\n",
                failures ? "FAIL" : "ALL PASS", failures);
    return failures ? 1 : 0;
}
