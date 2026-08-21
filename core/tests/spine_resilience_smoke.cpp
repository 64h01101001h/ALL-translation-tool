// spine_resilience_smoke — the 2026-08-21 field crash, fossilized.
// Adam's app died (SIGABRT) when the press replaced the spine db
// under a live instance: corpusSearch threw and nothing caught it.
// Contract now: a yanked/corrupted database DEGRADES to empty
// results — the process never aborts, the card shows honest absence.
#include <cstdio>
#include <filesystem>
#include <fstream>
#include <string>

#include "allcore/spine.h"

namespace fs = std::filesystem;

static int failures = 0;
#define CHECK(cond, what)                                   \
    do {                                                    \
        const bool ok = (cond);                             \
        std::printf("  [%s] %s\n", ok ? "PASS" : "FAIL",    \
                    what);                                  \
        if (!ok) ++failures;                                \
    } while (0)

int main(int argc, char** argv) {
    if (argc < 2) {
        std::printf("usage: spine_resilience_smoke <real spine db>\n");
        return 2;
    }
    const fs::path work =
        fs::temp_directory_path() / "all_spine_resilience";
    std::error_code ec;
    fs::remove_all(work, ec);
    fs::create_directories(work);
    const fs::path copy = work / "spine_copy.db";
    fs::copy_file(argv[1], copy, ec);
    if (ec) {
        std::printf("copy failed: %s\n", ec.message().c_str());
        return 2;
    }

    allcore::Spine spine(copy.string());
    CHECK(!spine.lookup("bsod nams").empty(),
          "healthy copy answers a lookup");
    CHECK(!spine.corpusSearch("\"bsod nams\"", "", 3).empty(),
          "healthy copy answers a corpus search");

    // THE FIELD EVENT: the file is yanked and replaced with garbage
    // while the handle is open (what the press's rsync did)
    {
        std::ofstream f(copy, std::ios::trunc);
        f << "not a database anymore";
    }
    // sqlite may serve some cached pages; hammer several queries —
    // the ONLY acceptable outcomes are results or empty, never death
    for (int i = 0; i < 5; ++i) {
        (void)spine.corpusSearch("\"sdug bsngal\"", "", 5);
        (void)spine.lookup("chos");
        (void)spine.headwordSearch("sems", 5);
        (void)spine.entryCount();
        (void)spine.metaValue("release_version");
    }
    CHECK(true, "yanked-db queries returned without aborting "
                "(the 2026-08-21 crash class is dead)");

    fs::remove_all(work, ec);
    std::printf("spine_resilience_smoke: %s (%d failures)\n",
                failures ? "FAIL" : "ALL PASS", failures);
    return failures ? 1 : 0;
}
