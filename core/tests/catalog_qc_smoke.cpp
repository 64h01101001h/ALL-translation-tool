// catalog_qc_smoke — the QC lanes (9g step 2). Constructed pins prove
// both lanes' verdicts; the library half measures the natural flag rate
// (our own library should be nearly clean — the lanes exist for intake).
//
// Usage: catalog_qc_smoke <scratch_dir> [sungbum_root]
#include <cstdio>
#include <filesystem>
#include <fstream>
#include <string>

#include "allcore/catalog_qc.h"

static int failures = 0;
#define CHECK(cond, msg)                                        \
    do {                                                        \
        if (cond) std::printf("  [PASS] %s\n", msg);            \
        else { std::printf("  [FAIL] %s\n", msg); ++failures; } \
    } while (0)

namespace fs = std::filesystem;
static void put(const fs::path& p, const std::string& b) {
    fs::create_directories(p.parent_path());
    std::ofstream f(p, std::ios::binary);
    f << b;
}

int main(int argc, char** argv) {
    setvbuf(stdout, nullptr, _IONBF, 0);
    if (argc < 2) {
        std::printf("usage: catalog_qc_smoke <scratch>\nFAILURES\n");
        return 1;
    }
    const fs::path sc = fs::path(argv[1]) / "qc_fixture";
    std::error_code ec;
    fs::remove_all(sc, ec);

    // ---- lane 1: the planted mismatch ------------------------------------
    allcore::TitlePairBank bank;
    bank.add("SHES RAB KYI PHA ROL TU PHYIN PA'I SNYING PO",
             "The Heart of the Perfection of Wisdom", "S00001");
    bank.add("BYANG CHUB SEMS DPA'I SPYOD PA LA 'JUG PA",
             "A Guide to the Bodhisattva Way of Life", "S00002");
    // the disease: a Kalachakra text wearing the Heart Sutra's English
    put(sc / "in/S09001_DUS KYI 'KHOR LO'I RGYUD KYI RNAM "
             "BSHAD_The Heart of the Perfection of Wisdom.txt",
        "@1A BDEN,");
    // an innocent file: its English matches its own Tibetan's pair
    put(sc / "in/S09002_SHES RAB KYI PHA ROL TU PHYIN PA'I SNYING "
             "PO_The Heart of the Perfection of Wisdom.txt",
        "@1A GNYIS,");
    const auto flags = allcore::qcTitleTranslationMismatch(
        (sc / "in").string(), bank);
    CHECK(flags.size() == 1 &&
              flags[0].file.rfind("S09001", 0) == 0 &&
              flags[0].other_eng.find("Heart") != std::string::npos,
          "the planted mismatch is flagged with the other work as "
          "evidence; the innocent match is not");

    // ---- lane 2: duplicates, colophon-aware ------------------------------
    const std::string colo1 =
        std::string(9000, 'K') +
        ",,DGE SLONG BLO BZANG GIS RI KHROD DU SBYAR BA'O,,";
    const std::string colo2 =
        std::string(9000, 'K') +
        ",,MKHAN CHEN RIN CHEN DPAL GYIS DGON CHEN DU SBYAR BA'O,,";
    put(sc / "dup/S09010_BLO SBYONG DON BDUN MA_Seven Points A.txt",
        colo1);
    put(sc / "dup/S09011_BLO SBYONG DON BDUN MA_Seven Points B.txt",
        colo1);
    put(sc / "dup/S09012_BLO SBYONG DON BDUN MA_Seven Points C.txt",
        colo2);
    const auto groups =
        allcore::qcDuplicateTitles((sc / "dup").string());
    CHECK(groups.size() == 1 && groups[0].members.size() == 3,
          "one shared-title group of three is found");
    CHECK(!groups.empty() &&
              groups[0].verdict.find("different colophons") !=
                  std::string::npos,
          "a differing colophon in the group yields the "
          "distinct-works verdict, not a duplicate guess");
    // and a true-duplicate pair
    fs::remove_all(sc / "dup2", ec);
    put(sc / "dup2/S09020_SGROL MA'I BSTOD PA_Praise A.txt", colo1);
    put(sc / "dup2/S09021_SGROL MA'I BSTOD PA_Praise B.txt", colo1);
    const auto g2 = allcore::qcDuplicateTitles((sc / "dup2").string());
    CHECK(g2.size() == 1 &&
              g2[0].verdict.find("true duplicates") != std::string::npos,
          "matching colophons yield the true-duplicates verdict");
    fs::remove_all(sc, ec);

    if (argc < 3) {
        std::printf("  [SKIP] library measure (no root)\n");
        std::printf("%s\n", failures ? "FAILURES" : "catalog_qc_smoke OK");
        return failures ? 1 : 0;
    }

    // ---- the library's own rates -----------------------------------------
    allcore::TitlePairBank libBank;
    libBank.addLibraryTree(argv[2]);
    const auto libFlags = allcore::qcTitleTranslationMismatch(
        argv[2], libBank, 0.85, 0.3, 200);
    const auto libGroups = allcore::qcDuplicateTitles(argv[2], 200);
    int dupTrue = 0, dupDistinct = 0, dupUnknown = 0;
    for (const auto& g : libGroups) {
        if (g.verdict.find("true duplicates") != std::string::npos)
            ++dupTrue;
        else if (g.verdict.find("distinct works") != std::string::npos)
            ++dupDistinct;
        else
            ++dupUnknown;
    }
    std::printf("  library: %zu mismatch flag(s) · %zu shared-title "
                "group(s): %d true-dup · %d distinct · %d no-evidence\n",
                libFlags.size(), libGroups.size(), dupTrue, dupDistinct,
                dupUnknown);
    for (size_t i = 0; i < libFlags.size() && i < 3; ++i)
        std::printf("    flag: %.60s | eng-sim %.2f tib-sim %.2f\n",
                    libFlags[i].file.c_str(), libFlags[i].eng_sim,
                    libFlags[i].tib_sim);
    CHECK(libFlags.size() <= 25,
          "the installed library's own mismatch rate stays low (the "
          "lane exists for intake material)");

    std::printf("%s\n", failures ? "FAILURES" : "catalog_qc_smoke OK");
    return failures ? 1 : 0;
}
