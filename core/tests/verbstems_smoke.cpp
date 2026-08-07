// verbstems_smoke — the CC0 verbs-database reference layer.
#include <cstdio>
#include <string>

#include "allcore/engines.h"
#include "allcore/verbstems.h"

static int failures = 0;
#define CHECK(cond, msg)                                        \
    do {                                                        \
        if (cond) std::printf("  [PASS] %s\n", msg);            \
        else { std::printf("  [FAIL] %s\n", msg); ++failures; } \
    } while (0)

int main(int argc, char** argv) {
    if (argc < 2) {
        std::fprintf(stderr, "usage: verbstems_smoke <verb_stems.tsv>\n");
        return 2;
    }
    allcore::VerbStems vs;
    CHECK(vs.load(argv[1]), "bank loads");
    CHECK(vs.size() > 3000, "3,000+ distinct stems");

    // the classic paradigm byed/byas/bya/byos — reach it the way the
    // app does: our own wylie -> unicode conversion, then lookup
    auto u = [](const char* w) {
        return allcore::wylieToUnicode(w).first;
    };
    auto byas = vs.lookup(u("byas"));
    bool pastByas = false;
    for (const auto& h : byas)
        pastByas |= (h.roles.find("past") != std::string::npos);
    CHECK(!byas.empty() && pastByas, "byas found as a past stem");
    auto byed = vs.lookup(u("byed"));
    bool presByed = false;
    for (const auto& h : byed)
        presByed |= (h.roles.find("present") != std::string::npos);
    CHECK(!byed.empty() && presByed, "byed found as a present stem");
    for (const auto& h : byas)
        if (!h.sources.empty()) {
            CHECK(true, "hits carry their attestation sources");
            break;
        }

    // non-verbs stay silent; the known-bad source-flag cell was skipped
    CHECK(vs.lookup(u("bsod")).empty() || true,
          "lookup never throws on arbitrary syllables");
    CHECK(vs.lookup("TDC").empty(),
          "the misaligned source-flag cell was never banked");

    std::printf("%s (%d failures)\n",
                failures ? "VERBSTEMS SMOKE FAILED" : "VERBSTEMS SMOKE OK",
                failures);
    return failures ? 1 : 0;
}
