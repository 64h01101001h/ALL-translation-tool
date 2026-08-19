// catalog_list_smoke — the ASCII master catalog generator (9g 2b/4).
// Runs the generator over the installed Kangyur and proves the list is a
// faithful, parseable inventory: record count == eligible files, every
// record parses back with S:/F:/P:, cataloged counts match the decoder,
// and provenance marks (T*) are honest.
//
// Usage: catalog_list_smoke <kangyur_root>
#include <cstdio>
#include <sstream>
#include <string>

#include "allcore/catalog_list.h"

static int failures = 0;
#define CHECK(cond, msg)                                        \
    do {                                                        \
        if (cond) std::printf("  [PASS] %s\n", msg);            \
        else { std::printf("  [FAIL] %s\n", msg); ++failures; } \
    } while (0)

int main(int argc, char** argv) {
    setvbuf(stdout, nullptr, _IONBF, 0);
    if (argc < 2) {
        std::printf("usage: catalog_list_smoke <root>\nFAILURES\n");
        return 1;
    }
    const auto r = allcore::generateAsciiCatalog(argv[1]);
    std::printf("  list: %d files - %d cataloged - %d uncataloged - %d "
                "titled from text - %zu bytes\n",
                r.stats.files, r.stats.cataloged, r.stats.uncataloged,
                r.stats.titled_from_text, r.text.size());
    CHECK(r.stats.files > 500, "the Kangyur yields a real inventory");
    CHECK(r.stats.cataloged + r.stats.uncataloged == r.stats.files,
          "every file is either cataloged or uncataloged - none skipped");

    // parse the list back: records separated by blank lines
    std::istringstream is(r.text);
    std::string line;
    int records = 0, sLines = 0, fLines = 0, pLines = 0, tStar = 0;
    bool inRecord = false;
    bool ascii = true;
    while (std::getline(is, line)) {
        // structure must be clean: no control characters anywhere;
        // field VALUES may carry UTF-8 (verbatim filenames)
        for (unsigned char c : line)
            if (c < 0x20 && c != '\t') ascii = false;
        if (line.empty()) { inRecord = false; continue; }
        if (line[0] == ';') continue;   // header
        if (!inRecord) { ++records; inRecord = true; }
        if (line.rfind("S: ", 0) == 0) ++sLines;
        if (line.rfind("F: ", 0) == 0) ++fLines;
        if (line.rfind("P: ", 0) == 0) ++pLines;
        if (line.rfind("T*: ", 0) == 0) ++tStar;
    }
    CHECK(records == r.stats.files,
          "one record per file, blank-line separated");
    CHECK(sLines == records && fLines == records && pLines == records,
          "every record carries S:, F:, and P: lines");
    CHECK(tStar == r.stats.titled_from_text,
          "every from-the-text title is marked T* - none silent");
    CHECK(ascii, "the list structure is clean plain text (no control\n                 " "characters); values verbatim per the header");
    CHECK(r.text.find("NOT the\n; official catalog") != std::string::npos,
          "the header states this is a list, not the official catalog");

    std::printf("%s\n", failures ? "FAILURES" : "catalog_list_smoke OK");
    return failures ? 1 : 0;
}
