// catalog_audit_smoke — the bibliography audit (TODO 9g 2a, GMR's job #1).
// Pins the scanner and normalizer on constructed cases, then RE-RUNS the
// real audit: every ACIP number cited in the extracted bibliography banks,
// checked against the installed library. Exact counts are pinned to the
// data as shipped (347 cited / 296 present / 51 missing, 2026-08-19); a
// data release legitimately moves them — update the pins WITH the data.
//
// Usage: catalog_audit_smoke <mixed_nuts_bibliography.json>
//                            <apparatus_bibliography.json> <library_root>
#include <cstdio>
#include <fstream>
#include <sstream>
#include <string>

#include "allcore/catalog_audit.h"

static int failures = 0;
#define CHECK(cond, msg)                                        \
    do {                                                        \
        if (cond) std::printf("  [PASS] %s\n", msg);            \
        else { std::printf("  [FAIL] %s\n", msg); ++failures; } \
    } while (0)

static std::string slurp(const char* p) {
    std::ifstream f(p, std::ios::binary);
    std::ostringstream ss;
    ss << f.rdbuf();
    return ss.str();
}

int main(int argc, char** argv) {
    setvbuf(stdout, nullptr, _IONBF, 0);

    // ---- scanner pins ----------------------------------------------------
    {
        const auto c = allcore::extractAcipCitations(
            "Tibetan translation at ACIP TD03971, ff. 251a-253b; see also "
            "ACIP S464 and ACIP\nKL00824. Not a citation: ACIPX123, "
            "PRINCIPLE (ACIP 12).");
        CHECK(c.size() == 3, "scanner finds exactly the three real citations");
        CHECK(c.size() == 3 && c[0].number == "TD03971" &&
                  c[1].number == "S464" && c[2].number == "KL00824",
              "numbers captured verbatim, across a line break too");
    }
    CHECK(allcore::normalizeCatalogNumber("S464") ==
              allcore::normalizeCatalogNumber("S00464"),
          "S464 and S00464 normalize to the same number");
    CHECK(allcore::normalizeCatalogNumber("TD03971").first == "TD" &&
              allcore::normalizeCatalogNumber("TD03971").second == 3971,
          "prefix and value split correctly");
    CHECK(allcore::normalizeCatalogNumber("NOPE").second == -1,
          "a non-number is refused, not guessed");

    // ---- audit pins on constructed data ----------------------------------
    {
        std::vector<allcore::AcipCitation> cited;
        for (const char* n : {"S464", "S00464", "KL00824"}) {
            allcore::AcipCitation c;
            c.number = n;
            cited.push_back(c);
        }
        std::set<std::pair<std::string, int>> have{{"S", 464}};
        const auto r = allcore::auditPresence(cited, have);
        CHECK(r.cited_distinct == 2 && r.present == 1 && r.missing == 1,
              "padding variants collapse; presence counted once");
        CHECK(!r.entries.empty() && !r.entries.front().present,
              "missing entries lead the report");
        CHECK(r.entries.back().citations == 2,
              "citation counts survive the collapse");
    }

    if (argc < 4) {
        std::printf("  [SKIP] real-data audit (paths not given)\n");
        std::printf("%s\n", failures ? "FAILURES" : "catalog_audit_smoke OK");
        return failures ? 1 : 0;
    }

    // ---- THE REAL AUDIT --------------------------------------------------
    const std::string biblio = slurp(argv[1]) + slurp(argv[2]);
    CHECK(biblio.size() > 100000, "bibliography banks load");
    const auto cited = allcore::extractAcipCitations(biblio);
    std::printf("  citations in the published bibliographies: %zu\n",
                cited.size());
    const auto have = allcore::collectLibraryNumbers(argv[3]);
    std::printf("  distinct catalog numbers in the library: %zu\n",
                have.size());
    const auto r = allcore::auditPresence(cited, have);
    std::printf("  audit: %d distinct cited · %d present · %d MISSING\n",
                r.cited_distinct, r.present, r.missing);
    CHECK(r.cited_distinct == 347,
          "347 distinct ACIP numbers cited (pinned to data as shipped)");
    CHECK(r.present == 296, "296 present in the installed library (pinned)");
    CHECK(r.missing == 51, "51 missing (pinned) — GMR's job #1, measured");
    bool klSeen = false;
    for (const auto& e : r.entries)
        if (!e.present && e.number == "KL00824") klSeen = true;
    CHECK(klSeen, "known missing work KL00824 (Sixty Verses citation) "
                  "appears in the missing list");

    std::printf("%s\n", failures ? "FAILURES" : "catalog_audit_smoke OK");
    return failures ? 1 : 0;
}
