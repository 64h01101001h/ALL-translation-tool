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
            // a silent skip hides coverage loss — fail loudly so
            // a seed without an ACIP field gets noticed
            std::printf("  [FAIL] seed has no ACIP field — the "
                        "ACIP-conversion lane was NOT "
                        "exercised\n");
            ++failures;
        }
    }

    // 5. a quotation spanning two consecutive corpus lines yields ONE
    //    match covering the whole span (cross-segment merge). The seed
    //    pair is chosen so that NO single segment contains the line
    //    boundary — the merge path is the only way to one match.
    {
        auto sylsOf = [](const std::string& w) {
            std::vector<std::string> v;
            std::string cur;
            for (char c : w + " ") {
                if (c == ' ') {
                    if (!cur.empty()) v.push_back(cur);
                    cur.clear();
                } else {
                    cur += c;
                }
            }
            return v;
        };
        allcore::CorpusSegment a, b;
        for (long long id = 1; id + 1 < spine.corpusMaxId() && a.id == 0;
             ++id) {
            auto s1 = spine.corpusSegmentById(id);
            auto s2 = spine.corpusSegmentById(id + 1);
            if (!s1.id || !s2.id || s1.course != s2.course ||
                s2.seq != s1.seq + 1)
                continue;
            auto v1 = sylsOf(s1.wylie), v2 = sylsOf(s2.wylie);
            if (v1.size() < 8 || v2.size() < 8) continue;
            std::string boundary = "\"";
            for (size_t k = v1.size() - 4; k < v1.size(); ++k)
                boundary += v1[k] + " ";
            for (size_t k = 0; k < 3; ++k)
                boundary += v2[k] + (k == 2 ? "" : " ");
            boundary += "\"";
            if (spine.corpusSearch(boundary, "", 1).empty()) {
                a = s1;
                b = s2;
            }
        }
        CHECK(a.id != 0,
              "found a consecutive pair whose boundary is in no single "
              "segment");
        if (a.id != 0) {
            const int total = (int)(sylsOf(a.wylie).size() +
                                    sylsOf(b.wylie).size());
            auto m = allcore::detectQuotations(
                spine, a.wylie + " " + b.wylie, false, 7);
            CHECK(m.size() == 1 && m[0].start_syllable == 0 &&
                      m[0].syllable_count == total &&
                      m[0].course == a.course && m[0].seq == b.seq,
                  "boundary-spanning quotation merges into one "
                  "whole-span match");
        }
    }

    std::printf("%s (%d failures)\n",
                failures ? "QUOTATION SMOKE FAILED" : "QUOTATION SMOKE OK",
                failures);
    return failures ? 1 : 0;
}
