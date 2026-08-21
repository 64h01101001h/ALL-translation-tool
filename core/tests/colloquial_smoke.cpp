// colloquial_smoke — the colloquial/prenasal pronunciation register +
// the honorific register (data/pron_colloquial, data/honorifics).
// Attestation checks: every register row must resolve to a real
// dictionary entry in the spine.
#include <cstdio>
#include <fstream>
#include <sstream>
#include <string>

#include "allcore/colloquial.h"
#include "allcore/spine.h"

static int failures = 0;
#define CHECK(cond, msg)                                        \
    do {                                                        \
        if (cond) std::printf("  [PASS] %s\n", msg);            \
        else { std::printf("  [FAIL] %s\n", msg); ++failures; } \
    } while (0)

int main(int argc, char** argv) {
    if (argc < 4) {
        std::fprintf(stderr,
                     "usage: colloquial_smoke <colloquial_pron.tsv> "
                     "<honorific_register.tsv> <spine.db>\n");
        return 2;
    }
    allcore::Spine spine(argv[3]);

    allcore::ColloquialPron reg;
    CHECK(reg.load(argv[1]), "colloquial register loads");
    std::printf("  %zu colloquial rows\n", reg.size());
    CHECK(reg.size() >= 200, "community + prenasal rows present");

    auto gonpa = reg.byColloquial("gonpa");
    CHECK(!gonpa.empty() && gonpa[0]->wylie == "dgon pa",
          "gonpa -> dgon pa (community)");
    auto tulku = reg.byColloquial("tulku");
    CHECK(!tulku.empty() && tulku[0]->wylie == "sprul sku",
          "tulku -> sprul sku");
    // HGM's own example: SKABS 'DIR = kamdir (prenasal rule)
    auto kamdir = reg.byColloquial("kamdir");
    CHECK(!kamdir.empty() && kamdir[0]->wylie == "skabs 'dir" &&
              kamdir[0]->cls == "prenasal-derived",
          "kamdir -> skabs 'dir (HGM prenasal rule, derived class)");
    CHECK(!kamdir.empty() && kamdir[0]->gmrPron == "kabdir",
          "engine form kabdir preserved beside the derived form");

    // attestation: every register wylie is a dictionary entry
    {
        std::ifstream f(argv[1]);
        std::string line;
        int bad = 0, rows = 0;
        while (std::getline(f, line)) {
            if (line.empty() || line[0] == '#') continue;
            std::stringstream ss(line);
            std::string c, w;
            std::getline(ss, c, '\t');
            std::getline(ss, w, '\t');
            ++rows;
            if (spine.lookup(w).empty()) {
                if (++bad <= 5)
                    std::printf("    not an entry: %s\n", w.c_str());
            }
        }
        std::printf("  %d rows, %d unresolvable\n", rows, bad);
        CHECK(bad == 0, "every colloquial row resolves to an entry");
    }
    // honorific register: parses, and every honorific is an entry
    {
        std::ifstream f(argv[2]);
        CHECK(bool(f), "honorific register opens");
        std::string line;
        int bad = 0, rows = 0, high = 0;
        while (std::getline(f, line)) {
            if (line.empty() || line[0] == '#') continue;
            std::stringstream ss(line);
            std::string h, o, dom, lvl;
            std::getline(ss, h, '\t');
            std::getline(ss, o, '\t');
            std::getline(ss, dom, '\t');
            std::getline(ss, lvl, '\t');
            ++rows;
            if (spine.lookup(h).empty()) ++bad;
            if (lvl == "high") ++high;
        }
        std::printf("  honorifics: %d rows, %d unresolvable, %d high\n",
                    rows, bad, high);
        CHECK(rows >= 25 && bad == 0,
              "every honorific resolves to an entry");
        CHECK(high >= 1, "high-honorific level representable");
    }

    std::printf("colloquial_smoke: %s (%d failures)\n",
                failures ? "FAIL" : "ALL PASS", failures);
    return failures ? 1 : 0;
}
