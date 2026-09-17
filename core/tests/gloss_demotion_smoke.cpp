// gloss_demotion_smoke — the headline equivalent, and what must not move.
//
// Adam asked why GUS rendered as "i", it was measured and reported and not
// fixed, and he asked again. The loader now moves two established artifacts
// out of the headline slot. This proves the two that move, the ones that must
// NOT move, and the invariant that matters most: nothing is ever deleted.
//
// Rule 1 is why the invariant is here. hgm_gloss is binding, so a change that
// quietly dropped an equivalent would be a worse fault than the one it fixed.
#include <cstdio>
#include <algorithm>
#include <string>
#include <vector>

#include <stdexcept>

#include "allcore/spine.h"

static int run(allcore::Spine& sp);

static int failures = 0;
#define CHECK(cond, msg)                                        \
    do {                                                        \
        if (cond) std::printf("  [PASS] %s\n", msg);            \
        else { std::printf("  [FAIL] %s\n", msg); ++failures; } \
    } while (0)

static bool has(const std::vector<std::string>& v, const std::string& s) {
    return std::find(v.begin(), v.end(), s) != v.end();
}

int main(int argc, char** argv) {
    const std::string db = argc > 1 ? argv[1] : "build/hgm_spine_v27_2.db";
    std::vector<allcore::Entry> probe;
    try {
        allcore::Spine sp(db);
        probe = sp.lookup("gus");
        if (probe.empty()) {
            std::printf("gloss demotion: SKIPPED — spine has no entries at %s\n",
                        db.c_str());
            return 77;
        }
        return run(sp);
    } catch (const std::exception& e) {
        std::printf("gloss demotion: SKIPPED — cannot open spine db %s (%s)\n",
                    db.c_str(), e.what());
        return 77;
    }
}

static const allcore::Entry* pick(const std::vector<allcore::Entry>& v,
                                  const std::string& w) {
    for (const auto& e : v)
        if (e.wylie == w) return &e;
    return nullptr;
}

static int run(allcore::Spine& sp) {

    struct Case {
        const char* wylie;
        const char* want_front;   // what must be headlined
        const char* must_keep;    // what must still be present
        const char* why;
    };
    const Case cases[] = {
        {"gus", "respect", "i",
         "gus headlines \"respect\", and \"i\" is kept, not deleted"},
        {"bdag", "i", "self",
         "bdag is UNTOUCHED — its \"i\" is correct and attested six times"},
        {"rnams", "s", "all",
         "rnams is UNTOUCHED — \"s\" is the English plural for a plural marker"},
        {"rang", "i", "own",
         "rang is UNTOUCHED — it means I"},
    };
    for (const auto& c : cases) {
        auto rows = sp.lookup(c.wylie);
        const allcore::Entry* e = pick(rows, c.wylie);
        if (!e || e->hgm_gloss.empty()) {
            std::printf("  [FAIL] %s (no entry or no gloss)\n", c.why);
            ++failures;
            continue;
        }
        CHECK(e->hgm_gloss.front() == c.want_front, c.why);
        CHECK(has(e->hgm_gloss, c.must_keep),
              "  ...and it still carries its other equivalent");
    }

    // gis: "'ll" must not headline, and must still be there.
    auto gis_rows = sp.lookup("gis");
    if (const allcore::Entry* e = pick(gis_rows, "gis")) {
        CHECK(!e->hgm_gloss.empty() && e->hgm_gloss.front() != "'ll",
              "gis does not headline \"'ll\" (an English future cannot render "
              "an instrumental)");
        CHECK(has(e->hgm_gloss, "'ll"),
              "  ...and \"'ll\" is kept, moved rather than removed");
    } else {
        std::printf("  [FAIL] gis has no entry\n");
        ++failures;
    }

    std::printf("gloss demotion: %s (%d failure%s)\n",
                failures == 0 ? "ALL PASS" : "FAIL", failures,
                failures == 1 ? "" : "s");
    return failures;
}
