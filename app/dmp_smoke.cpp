// dmp_smoke — the vendored google diff-match-patch (Apache-2.0,
// app/thirdparty/) that powers the Input pane's double-keying
// comparison. Integration checks on ACIP text: the exact discrepancy
// classes the input-center correction pass needs.
#include <cstdio>

#include "thirdparty/diff_match_patch.h"

static int failures = 0;
#define CHECK(cond, msg)                                        \
    do {                                                        \
        if (cond) std::printf("  [PASS] %s\n", msg);            \
        else { std::printf("  [FAIL] %s\n", msg); ++failures; } \
    } while (0)

int main() {
    diff_match_patch dmp;

    // identical partner files: one EQUAL span, zero discrepancies
    auto same = dmp.diff_main("BSOD NAMS KYI TSOGS,", "BSOD NAMS KYI TSOGS,");
    CHECK(same.size() == 1 && same.front().operation == EQUAL,
          "identical texts: single EQUAL span");

    // a dropped suffix letter (the classic typo the correction pass
    // exists to catch): NAMS vs NAM
    auto d1 = dmp.diff_main("BSOD NAMS KYI", "BSOD NAM KYI");
    dmp.diff_cleanupSemantic(d1);
    int dels = 0, ins = 0;
    QString delText;
    for (const auto& d : d1) {
        if (d.operation == DELETE) { ++dels; delText = d.text; }
        if (d.operation == INSERT) ++ins;
    }
    CHECK(dels == 1 && ins == 0 && delText.contains("S"),
          "dropped letter: one DELETE span containing S");

    // a substituted syllable: DGE vs DGA'
    auto d2 = dmp.diff_main("DGE BSHES", "DGA' BSHES");
    dmp.diff_cleanupSemantic(d2);
    bool hasDel = false, hasIns = false;
    for (const auto& d : d2) {
        hasDel |= d.operation == DELETE;
        hasIns |= d.operation == INSERT;
    }
    CHECK(hasDel && hasIns, "substitution: DELETE + INSERT pair");

    // offsets recoverable by walking EQUAL/DELETE through 'mine'
    {
        auto d3 = dmp.diff_main("KA KHA GA NGA", "KA KHA NGA");
        dmp.diff_cleanupSemantic(d3);
        int minePos = 0, delAt = -1;
        QString delTxt;
        for (const auto& d : d3) {
            if (d.operation == DELETE) {
                delAt = minePos;
                delTxt = d.text;
            }
            if (d.operation != INSERT) minePos += d.text.length();
        }
        // either alignment (" GA" at 6 / "GA " at 7) is a valid diff
        CHECK(delTxt.contains("GA") && (delAt == 6 || delAt == 7),
              "discrepancy position maps into my text (GA around 6-7)");
    }

    std::printf("dmp_smoke: %s (%d failures)\n",
                failures ? "FAIL" : "ALL PASS", failures);
    return failures ? 1 : 0;
}
