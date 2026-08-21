// qc_smoke — validate the QC checker against the REAL first live report
// (fixtures/analysis598.md) and against a synthetic misquote it must catch.
#include <cstdio>
#include <fstream>
#include <sstream>
#include <string>

#include "allcore/qc.h"
#include "allcore/spine.h"

static int failures = 0;
#define CHECK(cond, msg)                                        \
    do {                                                        \
        if (cond) std::printf("  [PASS] %s\n", msg);            \
        else { std::printf("  [FAIL] %s\n", msg); ++failures; } \
    } while (0)

static const char* kPassage =
    "SNGA DRO'I KA BA PHYI DRO MED PA DUS MA 'DRES PA'I DON YIN GYI, "
    "KA BA SNGA DRO YOD NA PHYI DRO MED DGOS PA DUS CHA MA 'DRES PA'I DON MA YIN LA,";

int main(int argc, char** argv) {
    if (argc < 3) {
        std::fprintf(stderr, "usage: qc_smoke <spine.db> <fixture.md>\n");
        return 2;
    }
    allcore::Spine spine(argv[1]);
    std::ifstream f(argv[2]);
    std::stringstream ss;
    ss << f.rdbuf();
    const std::string report = ss.str();

    auto fx = allcore::validateReport(spine, report, kPassage);
    int errs = 0, warns = 0, infos = 0;
    for (const auto& x : fx) {
        if (x.severity == "error") ++errs;
        else if (x.severity == "warn") ++warns;
        else ++infos;
    }
    std::printf("%s", allcore::findingsToMarkdown(fx).c_str());
    CHECK(!fx.empty(), "validator produced findings on the live report");

    bool anchorSummary = false, reconA = false;
    for (const auto& x : fx) {
        if (x.check == "hgm-anchor" && x.message.find("rows verified") != std::string::npos)
            anchorSummary = true;
        if (x.check == "reconstruction-a") reconA = true;
    }
    CHECK(anchorSummary, "item 8 table was parsed and diffed against the spine");
    CHECK(reconA, "item 6 reconstruction check ran");

    // synthetic misquote: claim the spine's default for KA BA is "table leg"
    std::string bad =
        "## 5) base\n**A) Literal / close:**\n> \"x\"\n**B) Readable / teaching:**\n> \"x\"\n"
        "## 6) seg\nKA BA = \"x\"\n"
        "## 7) seg\nKA BA = \"x\"\n"
        "## 8) terms\n| ACIP term | HGM default English | Chosen English | Notes |\n"
        "|---|---|---|---|\n"
        "| KA BA | table leg | table leg | looks fine |\n"
        "## 9) g\n## 10) g\n";
    auto fx2 = allcore::validateReport(spine, bad, kPassage);
    bool caught = false, devCaught = false;
    for (const auto& x : fx2) {
        if (x.check == "hgm-anchor" && x.severity == "error" &&
            x.message.find("KA BA") != std::string::npos)
            caught = true;
        if (x.check == "deviation-mark" && x.message.find("KA BA") != std::string::npos)
            devCaught = true;
    }
    CHECK(caught, "synthetic misquoted HGM default is flagged as error");
    CHECK(devCaught, "unmarked deviation is flagged");

    // synthetic silent "fix" of the Tibetan in item 9
    std::string fixed = bad;
    fixed += "KA BA SNGA DRO MED NA = \"tampered\"\n";
    auto fx3 = allcore::validateReport(spine, fixed, kPassage);
    bool integrity = false;
    for (const auto& x : fx3)
        if (x.check == "acip-integrity" && x.severity == "error") integrity = true;
    CHECK(integrity, "altered ACIP quote is flagged as integrity error");

    std::printf("%s (%d failures; live report: %d err / %d warn / %d info)\n",
                failures ? "QC SMOKE FAILED" : "QC SMOKE OK", failures, errs, warns,
                infos);
    return failures ? 1 : 0;
}
