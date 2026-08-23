// shelf_suggest_smoke — shelf suggestions (phase-3/17). The battery is
// HELD-OUT SHELF PREDICTION over the installed Sungbum: for sampled
// files already shelved, exclude the file itself and ask where it
// should go; the real shelf must rank high. The machine only ever
// SUGGESTS — the measured numbers are why the human still confirms.
//
// Usage: shelf_suggest_smoke [sungbum_root]
#include <algorithm>
#include <cctype>
#include <cstdio>
#include <filesystem>
#include <string>
#include <vector>

#include "allcore/shelf_suggest.h"

static int failures = 0;
#define CHECK(cond, msg)                                        \
    do {                                                        \
        if (cond) std::printf("  [PASS] %s\n", msg);            \
        else { std::printf("  [FAIL] %s\n", msg); ++failures; } \
    } while (0)

int main(int argc, char** argv) {
    setvbuf(stdout, nullptr, _IONBF, 0);

    if (argc < 2) {
        std::printf("usage: shelf_suggest_smoke <root>\nFAILURES\n");
        return 1;
    }
    namespace fs = std::filesystem;
    std::error_code ec;
    std::vector<fs::path> files;
    for (fs::recursive_directory_iterator it(
             argv[1], fs::directory_options::skip_permission_denied, ec),
         end;
         it != end; it.increment(ec)) {
        if (ec) break;
        if (!it->is_regular_file(ec)) continue;
        std::string up = it->path().filename().string();
        for (char& c : up)
            c = static_cast<char>(std::toupper(static_cast<unsigned char>(c)));
        if (up.find(" META.") != std::string::npos) continue;
        if (up.size() < 5 || up.substr(up.size() - 4) != ".TXT") continue;
        // held-out prediction needs the grammar fields
        if (it->path().filename().string().find('_') == std::string::npos)
            continue;
        files.push_back(it->path());
    }
    std::sort(files.begin(), files.end());
    std::printf("  corpus: %zu shelved, named files\n", files.size());
    CHECK(files.size() > 1000, "a real shelved corpus to predict on");
    // BUILD-7: with no corpus there is nothing to hold out. This used to
    // fall through, divide by zero, print two [PASS] lines carrying a
    // hardcoded "(measured 31%)" over ZERO queries, and then crash on
    // files[0]. An absent fixture is a failure, never a green.
    if (files.empty()) {
        std::printf("  [FAIL] no shelved corpus under %s - the held-out "
                    "battery measured nothing and does NOT pass\n", argv[1]);
        std::printf("FAILURES\n");
        return 1;
    }

    const size_t stride = std::max<size_t>(1, files.size() / 100);
    int queries = 0, top1 = 0, top3 = 0, none = 0;
    for (size_t i = 0; i < files.size() && queries < 100; i += stride) {
        const std::string name = files[i].filename().string();
        const std::string truth =
            fs::relative(files[i].parent_path(), argv[1], ec).string();
        const auto sug = allcore::suggestShelves(argv[1], name, "", {},
                                                 /*exclude=*/name, 3);
        ++queries;
        if (sug.empty()) { ++none; continue; }
        if (sug[0].shelf == truth) { ++top1; ++top3; continue; }
        for (size_t k = 1; k < sug.size(); ++k)
            if (sug[k].shelf == truth) { ++top3; break; }
    }
    const double pct1 = queries ? 100.0 * top1 / queries : 0.0;
    const double pct3 = queries ? 100.0 * top3 / queries : 0.0;
    std::printf("  held-out: %d queries · top-1 %d (%.1f%%) · top-3 %d "
                "(%.1f%%) · no suggestion %d\n",
                queries, top1, pct1, top3, pct3, none);
    // MEASURED REALITY (four configurations tried, 2026-08-19):
    // author-heavy 24/36, flat mixed-path 29/42, leaf-weighted 23/29,
    // flat Tibetan-half 31/43 (shipped). Shelf choice is a fine-grained
    // SUBJECT judgment — exactly the human expertise GMR guards ("you
    // have got to do a few hundred") — so the machine's ceiling from
    // surface signals is modest, every suggestion carries its reasons,
    // and the human confirms. These pins hold the shipped floor.
    // BUILD-7 / house rule 4: the ratio pins below are vacuously true at
    // queries == 0, and their labels used to carry a hardcoded
    // "(measured 31%)" that printed on PASS whatever the run measured.
    // The count is now pinned first and every number in a label is the
    // number this run produced.
    CHECK(queries > 0, "the held-out battery actually ran queries");
    char lbl1[224], lbl3[224];
    std::snprintf(lbl1, sizeof lbl1,
                  "the real shelf leads the list for at least a quarter of "
                  "held-out files (measured %.1f%% over %d queries)",
                  pct1, queries);
    std::snprintf(lbl3, sizeof lbl3,
                  "the real shelf is in the top three for at least 35%% "
                  "(measured %.1f%% over %d queries)", pct3, queries);
    CHECK(queries > 0 && top1 * 100 >= queries * 25, lbl1);
    CHECK(queries > 0 && top3 * 100 >= queries * 35, lbl3);

    // evidence is always attached
    {
        const std::string name = files[0].filename().string();
        const auto sug = allcore::suggestShelves(argv[1], name, "", {},
                                                 name, 3);
        bool reasons = !sug.empty();
        for (const auto& s : sug) reasons &= !s.reasons.empty();
        CHECK(reasons, "every suggestion carries its reasons");
    }

    std::printf("%s\n", failures ? "FAILURES" : "shelf_suggest_smoke OK");
    return failures ? 1 : 0;
}
