// worksheet_smoke — the cataloging worksheet (9g step 3) on the REAL
// 52-column schema from the team's live spreadsheet. Pins the schema,
// the sidecar round trip, and the CSV row export.
#include <cstdio>
#include <string>

#include "allcore/worksheet.h"

static int failures = 0;
#define CHECK(cond, msg)                                        \
    do {                                                        \
        if (cond) std::printf("  [PASS] %s\n", msg);            \
        else { std::printf("  [FAIL] %s\n", msg); ++failures; } \
    } while (0)

int main() {
    setvbuf(stdout, nullptr, _IONBF, 0);
    const auto& schema = allcore::worksheetSchema();
    CHECK(schema.size() == 52,
          "the schema carries all 52 columns of the live sheet");
    CHECK(schema.front() == "ACIP Number" &&
              schema.back() == "ISBN number:",
          "column order is the sheet's own (ACIP Number ... ISBN)");
    bool colo = false, dims = false;
    for (const auto& f : schema) {
        if (f == "Colophon:") colo = true;
        if (f == "Size of printed area:") dims = true;
    }
    CHECK(colo && dims, "the colophon and the St. Petersburg "
                        "physical-dims fields are present");

    allcore::Worksheet w;
    w["ACIP Number"] = "S05002-1";
    w["Tohoku"] = "5002, (A)";
    w["Colophon:"] = "line one\nline two\twith a tab";
    w["Author / Authorship Statement"] = "BU STON RIN CHEN GRUB";
    w["A Future Field"] = "unknown fields survive";
    const std::string side = allcore::serializeWorksheet(w);
    CHECK(side.find("working data, not the official catalog") !=
              std::string::npos,
          "the sidecar states what it is");
    const auto back = allcore::parseWorksheet(side);
    CHECK(back == w, "the sidecar round-trips exactly (escapes, "
                     "unknown fields and all)");

    const std::string csv = allcore::worksheetCsvRow(w);
    CHECK(csv.find("\"5002, (A)\"") != std::string::npos,
          "CSV cells with commas are quoted (RFC 4180)");
    CHECK(csv.find("ACIP Number,In Database,Tohoku") == 0,
          "the CSV header row is the sheet's own column order");
    // 52 columns = 51 commas outside quotes on the data row
    const std::string dataRow = csv.substr(csv.find('\n') + 1);
    int commas = 0;
    bool q = false;
    for (char c : dataRow) {
        if (c == '"') q = !q;
        else if (c == ',' && !q) ++commas;
    }
    CHECK(commas == 51, "the data row has exactly 52 cells");

    std::printf("%s\n", failures ? "FAILURES" : "worksheet_smoke OK");
    return failures ? 1 : 0;
}
