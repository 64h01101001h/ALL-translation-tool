// glossary_smoke — TDD RED first: the per-text glossary as a proper
// allcore store (extracted from the Overlay's private pair so the
// Workbench, the card, and future Draft integration share ONE truth).
// Format unchanged: header comment + "wylie<TAB>gloss" lines.
// Self-cleaning at entry and exit (constitution R6).
#include <cstdio>
#include <filesystem>
#include <fstream>
#include <string>

#include "allcore/glossary.h"

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
        fs::temp_directory_path() / "all_glossary_smoke";
    std::error_code ec;
    fs::remove_all(dir, ec);
    fs::create_directories(dir);
    const std::string p = (dir / "S0134I.tsv").string();

    {
        allcore::GlossaryStore g(p);
        g.load();
        CHECK(g.all().empty(), "fresh glossary is empty");
        g.set("bden pa", "truth");
        g.set("sdug bsngal", "suffering");
        CHECK(g.save("S0134I.txt"), "glossary saves");
    }
    {
        allcore::GlossaryStore g(p);
        CHECK(g.load(), "glossary reloads");
        CHECK(g.all().size() == 2, "two entries round-trip");
        CHECK(g.get("bden pa") == "truth", "lookup by wylie");
        g.set("bden pa", "truth; the real");
        CHECK(g.save("S0134I.txt"), "edit saves");
    }
    {
        allcore::GlossaryStore g(p);
        g.load();
        CHECK(g.get("bden pa") == "truth; the real",
              "edit round-trips");
        CHECK(g.remove("sdug bsngal"), "remove works");
        CHECK(!g.remove("never there"), "ghost remove fails honestly");
        g.save("S0134I.txt");
    }
    {
        allcore::GlossaryStore g(p);
        g.load();
        CHECK(g.all().size() == 1, "removal persisted");
        // the on-disk format keeps its header contract
        std::ifstream f(p);
        std::string first;
        std::getline(f, first);
        CHECK(!first.empty() && first[0] == '#',
              "header comment preserved");
    }

    fs::remove_all(dir, ec);
    std::printf("glossary_smoke: %s (%d failures)\n",
                failures ? "FAIL" : "ALL PASS", failures);
    return failures ? 1 : 0;
}
