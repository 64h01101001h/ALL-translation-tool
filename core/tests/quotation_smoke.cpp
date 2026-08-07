// quotation_smoke — deterministic quotation detection against the spine.
// A real corpus line quoted verbatim must be found and attributed to its
// source; sub-threshold and out-of-corpus text must find NOTHING.
#include <cstdio>
#include <string>

#include "allcore/quotation.h"
#include "allcore/spine.h"

static int failures = 0;
#define CHECK(cond, msg)                                        \
    do {                                                        \
        if (cond) std::printf("  [PASS] %s\n", msg);            \
        else { std::printf("  [FAIL] %s\n", msg); ++failures; } \
    } while (0)

int main(int argc, char** argv) {
    if (argc < 2) {
        std::fprintf(stderr, "usage: quotation_smoke <hgm_spine.db>\n");
        return 2;
    }
    allcore::Spine spine(argv[1]);

    // take a real corpus segment as the "quotation" ground truth
    allcore::CorpusSegment seed;
    for (long long id = 1; id < spine.corpusMaxId() && seed.id == 0; ++id) {
        auto s = spine.corpusSegmentById(id);
        int syl = 0;
        for (char c : s.wylie)
            if (c == ' ') ++syl;
        if (s.id != 0 && syl >= 10 && !s.english.empty()) seed = s;
    }
    CHECK(seed.id != 0, "found a seed segment with 10+ syllables");
    std::printf("  seed: %s #%d: %.60s…\n", seed.course.c_str(), seed.seq,
                seed.wylie.c_str());

    // 1. the segment quoted verbatim inside surrounding text is detected
    //    and attributed to its own course
    {
        const std::string passage =
            "zhes bya ba dang " + seed.wylie + " zhes gsungs so";
        auto m = allcore::detectQuotations(spine, passage, false, 7);
        bool hit = false;
        for (const auto& q : m)
            hit |= (q.course == seed.course && q.syllable_count >= 7);
        CHECK(!m.empty(), "verbatim quotation detected");
        CHECK(hit, "attributed to the correct source course");
        if (!m.empty())
            CHECK(!m[0].english.empty(),
                  "published English travels with the match");
    }

    // 2. below the syllable threshold: silence, not guesses
    {
        std::string firstFew;
        int c = 0;
        for (char ch : seed.wylie) {
            if (ch == ' ' && ++c == 4) break;
            firstFew += ch;
        }
        auto m = allcore::detectQuotations(spine, firstFew, false, 7);
        CHECK(m.empty(), "4-syllable snippet is below threshold: no match");
    }

    // 3. legal-but-invented wylie finds nothing (attested only)
    {
        auto m = allcore::detectQuotations(
            spine,
            "kag dag kag dag kag dag kag dag kag dag kag dag kag dag",
            false, 7);
        CHECK(m.empty(), "invented text matches nothing");
    }

    // 4. ACIP input goes through the canonical converter
    {
        // uppercase the seed's wylie? No — build ACIP from a known pair:
        // the converter battery guarantees acipToEwts; we just prove the
        // path is wired by round-tripping the seed's ACIP field if present
        if (!seed.acip.empty()) {
            const std::string passage = "KA BA DANG , " + seed.acip;
            auto m = allcore::detectQuotations(spine, passage, true, 7);
            bool hit = false;
            for (const auto& q : m) hit |= (q.course == seed.course);
            CHECK(hit, "ACIP passage detected via canonical conversion");
        } else {
            std::printf("  [SKIP] seed has no ACIP field\n");
        }
    }

    std::printf("%s (%d failures)\n",
                failures ? "QUOTATION SMOKE FAILED" : "QUOTATION SMOKE OK",
                failures);
    return failures ? 1 : 0;
}
