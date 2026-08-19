// register_smoke — the registrar's spreadsheet, read-only (queue item 9).
// Nick's real spreadsheet is not in this repo (it is the registrar's own
// working file) — the battery pins the parser on both delimiter styles
// and the three-state classification against the installed library.
//
// Usage: register_smoke [library_root]
#include <cstdio>
#include <string>

#include "allcore/catalog_audit.h"
#include "allcore/catalog_register.h"

static int failures = 0;
#define CHECK(cond, msg)                                        \
    do {                                                        \
        if (cond) std::printf("  [PASS] %s\n", msg);            \
        else { std::printf("  [FAIL] %s\n", msg); ++failures; } \
    } while (0)

int main(int argc, char** argv) {
    setvbuf(stdout, nullptr, _IONBF, 0);

    // CSV with quoted fields and the Apr-28 folio columns
    {
        allcore::CatalogRegister reg;
        const int n = reg.loadText(
            "Catalog Number,Title,Folio Start,Folio End,Date,Initials\n"
            "S25239,\"The Amazing, Wondrous Biography\",1A,24B,"
            "2026-01-26,GMR\n"
            "S25240,Second Biography,113A,130B,2026-02-02,NL\n"
            "S464,Sixty Verses citation,,,2026-01-05,NL\n"
            "not-a-number,junk row,,,,\n");
        CHECK(n == 3, "CSV rows with real numbers load; junk rows are "
                      "refused, not guessed");
        CHECK(reg.find("S00464") && reg.find("S00464")->initials == "NL",
              "padding-insensitive lookup (S464 == S00464)");
        const auto* e = reg.find("S25239");
        CHECK(e && e->folio_start == "1A" && e->initials == "GMR" &&
                  e->title.find("Amazing, Wondrous") != std::string::npos,
              "padding-insensitive lookup; quoted commas survive; folio "
              "and initials columns land");
    }
    // TSV with differently named headers
    {
        allcore::CatalogRegister reg;
        const int n = reg.loadText(
            "No.\tWork Title\tDate issued\tBy\n"
            "TD04210\tCommentary on Valid Perception\t1999-05-01\tMR\n");
        CHECK(n == 1 && reg.find("TD4210") &&
                  reg.find("TD4210")->initials == "MR",
              "TSV with fuzzy header names parses the same");
    }
    CHECK(allcore::CatalogRegister().find("S1") == nullptr,
          "an empty register answers nullptr, never a guess");
    {
        allcore::CatalogRegister reg;
        CHECK(reg.loadText("Title,Author\nfoo,bar\n") == 0,
              "a sheet with no number column loads nothing (0), "
              "loudly");
    }

    if (argc < 2) {
        std::printf("  [SKIP] three-state measure (no root)\n");
        std::printf("%s\n", failures ? "FAILURES" : "register_smoke OK");
        return failures ? 1 : 0;
    }

    // three-state classification against the real library: a register
    // holding one shelved work, one bibliography-missing work, and one
    // never-input number
    {
        allcore::CatalogRegister reg;
        reg.loadText(
            "number,title\n"
            "TD3996,shelved work\n"      // in the library (loop-1 audit)
            "KL824,cited but absent\n"   // missing per the audit
            "S99999,never input\n");
        const auto have = allcore::collectLibraryNumbers(argv[1]);
        int cataloged = 0, issuedOnly = 0;
        for (const auto& e : reg.entries()) {
            if (have.count(allcore::normalizeCatalogNumber(e.number)))
                ++cataloged;
            else
                ++issuedOnly;
        }
        std::printf("  three-state: %d cataloged, %d issued-but-absent "
                    "of %zu register rows\n",
                    cataloged, issuedOnly, reg.size());
        CHECK(cataloged == 1 && issuedOnly == 2,
              "the register separates cataloged from issued-but-absent "
              "against the real library");
    }

    std::printf("%s\n", failures ? "FAILURES" : "register_smoke OK");
    return failures ? 1 : 0;
}
