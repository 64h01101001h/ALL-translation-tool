// cleanup_smoke — cleanup + provenance lanes (queue item 8). The Aug-4
// directive (line-slash corruption breaks cross-line search) and the
// standing provenance signals, pinned on constructed cases and measured
// over the installed library.
//
// Usage: cleanup_smoke [library_root]
#include <cstdio>
#include <filesystem>
#include <fstream>
#include <sstream>
#include <string>

#include "allcore/catalog_id.h"

static int failures = 0;
#define CHECK(cond, msg)                                        \
    do {                                                        \
        if (cond) std::printf("  [PASS] %s\n", msg);            \
        else { std::printf("  [FAIL] %s\n", msg); ++failures; } \
    } while (0)

int main(int argc, char** argv) {
    setvbuf(stdout, nullptr, _IONBF, 0);

    // ---- the slash corruption --------------------------------------------
    {
        std::string corrupt;
        for (int i = 0; i < 30; ++i)
            corrupt += "SEMS CAN THAMS CAD BDE BA DANG /\n";
        corrupt += "LDAN PAR GYUR CIG ,\n";
        const auto scan = allcore::scanAcipCleanup(corrupt);
        CHECK(scan.slash_corruption && scan.slash_terminated == 30,
              "pervasive line-ending slashes are detected as the Aug-4 "
              "corruption");
        const auto [clean, removed] = allcore::stripLineSlashes(corrupt);
        CHECK(removed == 30, "every line-terminating slash is removed");
        CHECK(clean.find('/') == std::string::npos,
              "the cleaned text carries no stray slashes");
        CHECK(allcore::scanAcipCleanup(clean).slash_terminated == 0,
              "the cleaned text scans clean");
        // the whole point: search across the line break works again
        CHECK(clean.find("DANG \nLDAN PAR") != std::string::npos,
              "cross-line text is contiguous again (searchable)");
    }
    // a legitimate mid-line slash (G-YEN etc.) is never touched
    {
        const std::string doc = "RGYA GAR / BOD KYI SKAD,\nBDEN PA,\n";
        const auto [clean, removed] = allcore::stripLineSlashes(doc);
        CHECK(removed == 0 && clean == doc,
              "a slash inside a line is left alone");
        CHECK(!allcore::scanAcipCleanup(doc).slash_corruption,
              "isolated slashes are not corruption");
    }

    // ---- lowercase runs and pagination -----------------------------------
    {
        const std::string doc =
            "@85A BDEN PA, thissentencewastypedinlowercaseillegally "
            "DANG,\n@86 GNYIS PA,\n@86B GSUM PA,\n";
        const auto scan = allcore::scanAcipCleanup(doc);
        CHECK(scan.lowercase_runs == 1,
              "a 20+ letter lowercase run is flagged (typed illegally)");
        CHECK(scan.folio_marks == 2 && scan.western_page_marks == 1,
              "folio marks and western page marks counted separately - "
              "the typed-book provenance signal");
    }

    if (argc < 2) {
        std::printf("  [SKIP] library measure (no root)\n");
        std::printf("%s\n", failures ? "FAILURES" : "cleanup_smoke OK");
        return failures ? 1 : 0;
    }

    // ---- measure over the installed library ------------------------------
    namespace fs = std::filesystem;
    std::error_code ec;
    int scanned = 0, slashCorrupt = 0, withLower = 0, westernOnly = 0;
    int stride = 0;
    std::vector<std::string> files;
    for (fs::recursive_directory_iterator it(
             argv[1], fs::directory_options::skip_permission_denied, ec),
         end;
         it != end; it.increment(ec)) {
        if (ec) break;
        if (!it->is_regular_file(ec)) continue;
        std::string up = it->path().filename().string();
        for (char& c : up)
            c = static_cast<char>(std::toupper(static_cast<unsigned char>(c)));
        if (up.size() < 5 || up.find(" META.") != std::string::npos)
            continue;
        const std::string ext = up.substr(up.rfind('.') == std::string::npos
                                              ? up.size()
                                              : up.rfind('.'));
        if (ext != ".TXT" && ext != ".ACT" && ext != ".INC") continue;
        files.push_back(it->path().string());
    }
    std::sort(files.begin(), files.end());
    stride = std::max<int>(1, static_cast<int>(files.size() / 400));
    for (size_t i = 0; i < files.size(); i += stride) {
        std::ifstream f(files[i], std::ios::binary);
        std::ostringstream ss;
        ss << f.rdbuf();
        const auto scan = allcore::scanAcipCleanup(ss.str());
        ++scanned;
        if (scan.slash_corruption) ++slashCorrupt;
        if (scan.lowercase_runs > 0) ++withLower;
        if (scan.folio_marks == 0 && scan.western_page_marks > 3)
            ++westernOnly;
    }
    std::printf("  library sample: %d files · %d slash-corrupted · %d "
                "with lowercase runs · %d western-paginated (typed-book "
                "signal)\n",
                scanned, slashCorrupt, withLower, westernOnly);
    CHECK(scanned > 300, "a real sample was scanned");
    CHECK(slashCorrupt * 100 <= scanned * 5,
          "our own library is essentially free of the slash corruption "
          "(the lane exists for intake material like the Jamyang Shepa "
          "case)");

    std::printf("%s\n", failures ? "FAILURES" : "cleanup_smoke OK");
    return failures ? 1 : 0;
}
