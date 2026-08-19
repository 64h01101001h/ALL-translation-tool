// catalog_name_smoke — the filename composer + META convention (9g 2b/3).
// Pins fix the grammar; the battery verifies the convention against the
// library's own 1,457 real "+.TXT" / "NUMBER META.TXT" pairs — the ground
// truth the convention was recovered from.
//
// Usage: catalog_name_smoke [library_root]
#include <cctype>
#include <cstdio>
#include <filesystem>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>

#include "allcore/catalog_name.h"
#include "allcore/tibexport.h"

static int failures = 0;
#define CHECK(cond, msg)                                        \
    do {                                                        \
        if (cond) std::printf("  [PASS] %s\n", msg);            \
        else { std::printf("  [FAIL] %s\n", msg); ++failures; } \
    } while (0)

int main(int argc, char** argv) {
    setvbuf(stdout, nullptr, _IONBF, 0);

    // ---- grammar pins ----------------------------------------------------
    {
        const auto r = allcore::composeCatalogFilename(
            "S25239",
            "DPAL LDAN RED MDA' BA CHEN PO'I RNAM THAR NGO MTSAR RMAD "
            "BYUNG BZHUGS SO",
            "The Amazing and Wondrous Biography of the Great and "
            "Glorious Rendawa",
            "RATNA SAMBHAVA (fl. 1405)");
        CHECK(!r.truncated && r.meta_filename.empty(),
              "a name that fits gets no '+' and no META at all");
        CHECK(r.filename.find("BZHUGS SO") == std::string::npos,
              "BZHUGS SO never enters the filename");
        CHECK(r.filename.rfind("S25239_", 0) == 0 &&
                  r.filename.substr(r.filename.size() - 4) == ".TXT",
              "NUMBER leads, capital .TXT ends");
        const auto inf = allcore::decodeAcipFilename(r.filename);
        CHECK(inf.recognized,
              "the composed name round-trips through decodeAcipFilename");
    }
    CHECK(allcore::sanitizeNameField(
              "A Clarification of the Meaning: the Stages") ==
              "A Clarification of the Meaning-the Stages",
          "a colon becomes a single hyphen, no spaces (session 5)");
    CHECK(allcore::sanitizeNameField(
              "Pram\xC4\x81\xE1\xB9\x87"  // Pramāṇ
              "a \xC5\x9B\xC4\x81stra") == "Pramana shastra",
          "IAST diacritics fold to ASCII");
    CHECK(allcore::stripBzhugsSo("MDO CHEN PO BZHUGS SO, ,") ==
              "MDO CHEN PO",
          "trailing BZHUGS SO and its punctuation strip");

    // ---- the +/META machinery: ONLY for over-long names ------------------
    {
        std::string longTib(180, 'K'), longEng(120, 'e');
        for (size_t i = 4; i < longTib.size(); i += 5) longTib[i] = ' ';
        for (size_t i = 4; i < longEng.size(); i += 5) longEng[i] = ' ';
        const auto r = allcore::composeCatalogFilename(
            "S25280", longTib, longEng, "JETSUN RENDAWA (1349-1412)");
        CHECK(r.truncated, "an over-limit name truncates");
        CHECK(r.filename.size() == 255,
              "the truncated name fills the limit exactly (mid-word "
              "stop, no padding)");
        CHECK(r.filename[250] == '+',
              "'+' marks the continuation before the extension");
        CHECK(r.meta_filename == "S25280 META.TXT",
              "the companion is 'NUMBER META.TXT' (the library's own "
              "practice - space, not underscore)");
        const std::string stem =
            r.filename.substr(0, r.filename.size() - 4);
        CHECK(allcore::rejoinMetaName(stem, r.meta_content) ==
                  r.full_stem,
              "rejoin = pure concatenation back to the full name");
    }

    // ---- legacy RTF META extraction --------------------------------------
    CHECK(allcore::metaContinuationText(
              "{\\rtf1\\ansi\\ansicpg1252\n{\\fonttbl\\f0 Helvetica;}\n"
              "\\f0\\fs24 \\cf0 m in Tiers or as a General Mass_AUTHOR}")
              == "m in Tiers or as a General Mass_AUTHOR",
          "the Word-era RTF companion yields its continuation text");

    if (argc < 2) {
        std::printf("  [SKIP] library battery (no root)\n");
        std::printf("%s\n", failures ? "FAILURES" : "catalog_name_smoke OK");
        return failures ? 1 : 0;
    }

    // ---- battery: the library's own pairs --------------------------------
    namespace fs = std::filesystem;
    std::error_code ec;
    int plus = 0, paired = 0, joined = 0, checked = 0;
    for (fs::recursive_directory_iterator it(
             argv[1], fs::directory_options::skip_permission_denied, ec),
         end;
         it != end; it.increment(ec)) {
        if (ec) break;
        if (!it->is_regular_file(ec)) continue;
        const std::string name = it->path().filename().string();
        std::string up = name;
        for (char& c : up)
            c = static_cast<char>(std::toupper(static_cast<unsigned char>(c)));
        if (up.size() < 6 || up.substr(up.size() - 5) != "+.TXT") continue;
        ++plus;
        // the number is everything before the first '_'
        const size_t u = name.find('_');
        if (u == std::string::npos) continue;
        const std::string num = name.substr(0, u);
        // companion in the same folder, either extension case
        fs::path metaP;
        for (const char* ext : {".TXT", ".txt"}) {
            const fs::path p =
                it->path().parent_path() / (num + " META" + ext);
            if (fs::exists(p, ec)) { metaP = p; break; }
        }
        if (metaP.empty()) continue;
        ++paired;
        if (checked < 200) {
            ++checked;
            std::ifstream mf(metaP, std::ios::binary);
            std::ostringstream ss;
            ss << mf.rdbuf();
            const std::string cont =
                allcore::metaContinuationText(ss.str());
            const std::string stem = name.substr(0, name.size() - 4);
            const std::string full =
                allcore::rejoinMetaName(stem, cont);
            // the rejoin must extend the name (non-empty continuation)
            // and never create a doubled space at the junction
            if (!cont.empty() && full.size() > stem.size() &&
                full.find("  ") == std::string::npos)
                ++joined;
        }
    }
    std::printf("  library: %d '+.TXT' names · %d with a META companion "
                "· %d of %d sampled rejoins clean\n",
                plus, paired, joined, checked);
    CHECK(plus > 1000, "the truncation convention is real (1,457 as "
                       "shipped)");
    CHECK(paired * 100 >= plus * 92,
          "at least 92% of truncated names have their META companion "
          "(measured 93.5% - 95 real pairs are genuinely incomplete; "
          "a finding for the cataloging team, not a bug here)");
    CHECK(checked > 0 && joined * 100 >= checked * 90,
          "at least 90% of sampled companions rejoin cleanly");

    std::printf("%s\n", failures ? "FAILURES" : "catalog_name_smoke OK");
    return failures ? 1 : 0;
}
