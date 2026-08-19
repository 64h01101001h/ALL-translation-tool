// catalog_id_smoke — the identity proposal lane (TODO 9g step 1).
//
// Two halves. The pins fix the extraction rules on real title-page shapes
// taken verbatim from the installed collections. The BATTERY then re-measures
// the whole claim against the library itself: for files whose filename
// already asserts an identity, does the TEXT ALONE recover it? Thresholds are
// set just under the measured numbers, so drift fails here rather than in
// front of a cataloger.
//
// Usage: catalog_id_smoke [library_root]
// Without a library root the pins still run and the battery reports skipped.
#include <algorithm>
#include <cctype>
#include <cstdio>
#include <filesystem>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>

#include "allcore/catalog_id.h"

static int failures = 0;
#define CHECK(cond, msg)                                        \
    do {                                                        \
        if (cond) std::printf("  [PASS] %s\n", msg);            \
        else { std::printf("  [FAIL] %s\n", msg); ++failures; } \
    } while (0)

int main(int argc, char** argv) {
    setvbuf(stdout, nullptr, _IONBF, 0);

    // ---- pins: the canonical bilingual head (Tengyur TD02022) -------------
    {
        const std::string doc =
            "@85A #, ,RGYA GAR SKAD DU, YA M'A RI YANTRA AA BA LI,\n"
            "BOD SKAD DU, GSHIN RJE GSHED KYI 'KHRUL 'KHOR GYI PHRENG BA, "
            "BCOM\nLDAN 'DAS GSHIN RJE GSHED LA PHYAG 'TSAL LO, ,\n";
        const auto t = allcore::extractAcipTitle(doc);
        CHECK(t.found && t.rule == "bod-skad-du",
              "bilingual head recognized by the BOD SKAD DU rule");
        CHECK(t.title == "GSHIN RJE GSHED KYI 'KHRUL 'KHOR GYI PHRENG BA",
              "title taken from BOD SKAD DU up to the next comma");
        CHECK(t.sanskrit.find("YA M'A RI") != std::string::npos,
              "the Sanskrit side of the head is carried as evidence");
    }

    // a Chinese-source Tengyur head (TD04208) uses RGYA'I SKAD DU
    {
        const std::string doc =
            "@88B #, ,RGYA'I SKAD DU, G-YEN MING GZHEE' [?] CING LI'I\n"
            "LUN, BOD SKAD DU, TSAD MA'I\n\nBSTAN BCOS RIGS PA LA 'JUG PA "
            "ZHES BYA BA, 'JAM PA'I DBYANGS LA\nPHYAG 'TSAL LO, ,";
        const auto t = allcore::extractAcipTitle(doc);
        CHECK(t.title == "TSAD MA'I BSTAN BCOS RIGS PA LA 'JUG PA ZHES BYA BA",
              "title spanning a line break and a folio gap is reassembled");
        CHECK(t.sanskrit.find("G-YEN MING") != std::string::npos &&
                  t.sanskrit.find("[?]") != std::string::npos,
              "the Chinese-side head is carried verbatim, flags and all");
    }

    // ---- pins: the Sungbum title block closing in BZHUGS SO ---------------
    {
        const std::string doc =
            "@001A *, ,CHOS MNGON PA MDZOD KYI DGONGS 'GREL GYI BSTAN BCOS\n"
            "THUB BSTAN NOR BU'I GTER MDZOD DUS GSUM RGYAL BA'I BZHED DON "
            "KUN\nGSAL LAS GNAS BRGYAD PA BZHUGS SO,,\n\n@001B #, ,NA MO GU "
            "RU MANYDZU GHOsh'A YA";
        const auto t = allcore::extractAcipTitle(doc);
        CHECK(t.found && t.rule == "bzhugs-so",
              "title block recognized by the BZHUGS SO rule");
        CHECK(t.title.rfind("CHOS MNGON PA MDZOD KYI DGONGS 'GREL", 0) == 0,
              "block starts after the folio marker and its punctuation");
        CHECK(t.title.find("BZHUGS") == std::string::npos,
              "the closing BZHUGS SO is not part of the title");
        CHECK(t.variants.size() == 1 &&
                  t.variants[0] == "GNAS BRGYAD PA",
              "the tail after LAS is offered as a second reading");
    }

    // ---- pin: a fragment announces nothing, and says so ------------------
    {
        const std::string doc =
            "` MKHA' 'GRO SDE LNGA'I BSNYEN PA'I SKABS ZHI BA'I LAS LA MTSON "
            "NA, DBUS\nKYI SANGS RGYAS MKHA' 'GRO'I THUGS KAR ZLA GDAN LA";
        const auto t = allcore::extractAcipTitle(doc);
        CHECK(!t.found && t.title.empty() && t.rule.empty(),
              "a file that begins mid-text yields no title, not a guess");
        allcore::TitleBank b;
        b.add("S0001", "MKHA' 'GRO SDE LNGA'I BSNYEN PA", "test");
        CHECK(allcore::suggestIdentity(t, b).empty(),
              "no title page -> no candidates offered");
    }

    // ---- pins: normalization and the scorer ------------------------------
    CHECK(allcore::normalizeAcipTitle("@012A *, ,rnam [?] bshad (Skt) BA,,") ==
              "RNAM BSHAD BA",
          "normalize: folio marker, brackets, parentheses, punctuation out");
    {
        allcore::TitleBank bank;
        CHECK(bank.add("TD02022", "GSHIN RJE GSHED KYI 'KHRUL 'KHOR GYI "
                                  "PHRENG BA", "test"),
              "bank accepts a real title");
        CHECK(!bank.add("TD9999", "BZHUGS SO", "test"),
              "bank refuses a title under three syllables");
        bank.add("TD0001", "SHES RAB KYI PHA ROL TU PHYIN PA", "test");

        allcore::TitleExtraction t;
        t.found = true;
        t.title = "GSHIN RJE GSHED KYI 'KHRUL 'KHOR GYI PHRENG BA";
        const auto c = allcore::suggestIdentity(t, bank);
        CHECK(c.size() == 1 && c[0].key == "TD02022",
              "the matching work is proposed, the unrelated one is not");
        if (!c.empty()) {
            CHECK(c[0].score == 1.0 && c[0].basis == "exact title",
                  "an identical title scores 1.0 and is labelled exact");
            CHECK(c[0].shared == c[0].bank_syllables,
                  "evidence counts are carried with the candidate");
        }
        // a short title must not claim a long work
        allcore::TitleExtraction s;
        s.found = true;
        s.title = "SHES RAB KYI";
        CHECK(allcore::suggestIdentity(s, bank).empty(),
              "three shared syllables of a nine-syllable title stay below "
              "the floor");
    }

    // ---- the battery: re-measure against the installed library -----------
    if (argc < 2) {
        std::printf("  [SKIP] library battery (no library root given)\n");
        std::printf("%s\n", failures ? "FAILURES" : "catalog_id_smoke OK");
        return failures ? 1 : 0;
    }
    namespace fs = std::filesystem;
    const std::string root = argv[1];
    std::error_code ec;
    if (!fs::exists(root, ec)) {
        std::printf("  [SKIP] library battery (no library at %s)\n",
                    root.c_str());
        std::printf("%s\n", failures ? "FAILURES" : "catalog_id_smoke OK");
        return failures ? 1 : 0;
    }

    allcore::TitleBank bank;
    const int banked = bank.addLibraryTree(root);
    std::printf("  bank: %d titles from the library tree\n", banked);
    CHECK(banked > 1000, "the installed library banks a real title bank");

    // Collect the same files, deterministically strided so the battery stays
    // a few seconds. No randomness: the same files every run.
    std::vector<std::string> files;
    for (fs::recursive_directory_iterator it(
             root, fs::directory_options::skip_permission_denied, ec), end;
         it != end; it.increment(ec)) {
        if (ec) break;
        if (!it->is_regular_file(ec)) continue;
        const std::string name = it->path().filename().string();
        std::string up = name;
        for (char& c : up) c = static_cast<char>(std::toupper(
                                static_cast<unsigned char>(c)));
        if (up.find("META") != std::string::npos) continue;
        const size_t dot = up.rfind('.');
        const std::string ext = dot == std::string::npos ? "" : up.substr(dot);
        if (ext != ".TXT" && ext != ".ACT" && ext != ".INC" && ext != ".ACE")
            continue;
        if (name.find('_') == std::string::npos) continue;
        files.push_back(it->path().string());
    }
    std::sort(files.begin(), files.end());
    std::printf("  library: %zu named files\n", files.size());

    int examined = 0, titled = 0, top1 = 0, top5 = 0, wrong = 0, none = 0;
    int byBod = 0, byBzhugs = 0;
    const int stride = std::max<size_t>(1, files.size() / 400);
    for (size_t i = 0; i < files.size(); i += stride) {
        const std::string& p = files[i];
        const std::string stem = fs::path(p).stem().string();
        const std::string own = stem.substr(0, stem.find('_'));
        std::ifstream f(p, std::ios::binary);
        if (!f) continue;
        std::string head(4000, '\0');
        f.read(&head[0], 4000);
        head.resize(static_cast<size_t>(f.gcount()));
        ++examined;
        const auto t = allcore::extractAcipTitle(head);
        if (!t.found) continue;
        ++titled;
        if (t.rule == "bod-skad-du") ++byBod; else ++byBzhugs;
        const auto c = allcore::suggestIdentity(t, bank, 5);
        if (c.empty()) { ++none; continue; }
        if (c[0].key == own) { ++top1; continue; }
        bool in5 = false;
        for (const auto& x : c) in5 |= (x.key == own);
        if (in5) ++top5; else ++wrong;
    }
    std::printf(
        "  battery: %d files examined · %d carry a title page "
        "(%d bilingual, %d bzhugs-so)\n",
        examined, titled, byBod, byBzhugs);
    if (titled > 0) {
        const double p1 = 100.0 * top1 / titled;
        const double p5 = 100.0 * (top1 + top5) / titled;
        std::printf(
            "  battery: top-1 %d (%.1f%%) · within top-5 %d (%.1f%%) "
            "· wrong %d · no match %d\n",
            top1, p1, top1 + top5, p5, wrong, none);
        CHECK(titled * 100 >= examined * 35,
              "at least a third of library files carry an extractable title");
        CHECK(p1 >= 75.0, "top-1 identity recall holds at or above 75%");
        CHECK(p5 >= 88.0, "correct identity is within the top 5 at least 88% "
                          "of the time");
        CHECK(wrong * 100 <= titled * 8,
              "confidently wrong answers stay under 8%");
    }

    std::printf("%s\n", failures ? "FAILURES" : "catalog_id_smoke OK");
    return failures ? 1 : 0;
}
