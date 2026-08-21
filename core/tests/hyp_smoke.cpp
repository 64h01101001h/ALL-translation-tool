// hyp_smoke — the Hypercontext .hyp parser against Adam's own
// tutorial.hyp (banked at data/hypercontext_legacy/). The lesson-1
// example is the ground truth: its nested link structure is described
// IN the tutorial's own prose, so the expected spans are documented by
// the file itself.
#include <cstdio>
#include <fstream>
#include <sstream>
#include <string>

#include "allcore/hypfile.h"

static int failures = 0;
#define CHECK(cond, msg)                                        \
    do {                                                        \
        if (cond) std::printf("  [PASS] %s\n", msg);            \
        else { std::printf("  [FAIL] %s\n", msg); ++failures; } \
    } while (0)

int main(int argc, char** argv) {
    if (argc < 2) {
        std::fprintf(stderr, "usage: hyp_smoke <tutorial.hyp>\n");
        return 2;
    }
    std::ifstream f(argv[1], std::ios::binary);
    CHECK(bool(f), "tutorial.hyp opens");
    std::stringstream ss;
    ss << f.rdbuf();
    const auto hyp = allcore::parseHypFile(ss.str());

    std::printf("  tibetan %zu bytes · english %zu bytes · %zu paired "
                "link(s) · %zu one-sided · %zu note(s) · %d unknown "
                "tag(s)\n",
                hyp.tibetan.size(), hyp.english.size(), hyp.links.size(),
                hyp.onesided.size(), hyp.notes.size(), hyp.unknownTags);
    CHECK(!hyp.tibetan.empty() && !hyp.english.empty(),
          "both scripts extracted");
    CHECK(hyp.links.size() == 15, "all 15 numbered links paired");
    CHECK(hyp.onesided.empty(), "no one-sided links in the tutorial");
    CHECK(hyp.notes.size() == 1 &&
              hyp.notes[0].find("note") != std::string::npos,
          "the tutorial's <!…> note collected");
    CHECK(hyp.unknownTags == 0, "no unknown tags remain");

    auto tibOf = [&](const allcore::HypLink& l) {
        return hyp.tibetan.substr(l.tibBeg, l.tibEnd - l.tibBeg);
    };
    auto engOf = [&](const allcore::HypLink& l) {
        return hyp.english.substr(l.engBeg, l.engEnd - l.engBeg);
    };
    auto byId = [&](int id) -> const allcore::HypLink* {
        for (const auto& l : hyp.links)
            if (l.id == id) return &l;
        return nullptr;
    };

    // the tutorial's own documented example:
    //   <1 <2 DBU MA><3 'I> <4 RTZA BA>><5 'I> <6 'GREL PA>
    //   <7 <8 SHES RAB> <9 SGRON MA>>
    //   "<7 <9 Lamp> of <8 Wisdom>>," a <6 Commentary> <5 to>
    //   "<1 The <4 Root Text> <3 on> the <2 Middle Way>>"
    const auto* l2 = byId(2);
    CHECK(l2 && tibOf(*l2) == "DBU MA" && engOf(*l2) == "Middle Way",
          "link 2: DBU MA = Middle Way");
    const auto* l3 = byId(3);
    CHECK(l3 && tibOf(*l3) == "'I" && engOf(*l3) == "on",
          "link 3: sub-syllable 'I = on");
    const auto* l9 = byId(9);
    CHECK(l9 && tibOf(*l9) == "SGRON MA" && engOf(*l9) == "Lamp",
          "link 9: SGRON MA = Lamp");
    const auto* l1 = byId(1);
    CHECK(l1 && tibOf(*l1) == "DBU MA'I RTZA BA",
          "link 1 tib spans its sublinks: DBU MA'I RTZA BA");
    CHECK(l1 && engOf(*l1) == "The Root Text on the Middle Way",
          "link 1 eng spans its sublinks");
    const auto* l7 = byId(7);
    CHECK(l7 && tibOf(*l7) == "SHES RAB SGRON MA" &&
              engOf(*l7) == "Lamp of Wisdom",
          "link 7: nested pair SHES RAB SGRON MA = Lamp of Wisdom");

    // the <t$>/<e$> escapes render literally in the English text
    CHECK(hyp.english.find("$<t$>DBU MA LA 'JUG PA") == std::string::npos,
          "escape handling: $<t$> sequence not left raw");
    CHECK(hyp.english.find("<t>DBU MA LA 'JUG PA") != std::string::npos ||
              hyp.tibetan.find("DBU MA LA 'JUG PA") != std::string::npos,
          "escaped tag text preserved as literal <t>");

    std::printf("hyp_smoke: %s (%d failures)\n",
                failures ? "FAIL" : "ALL PASS", failures);
    return failures ? 1 : 0;
}
