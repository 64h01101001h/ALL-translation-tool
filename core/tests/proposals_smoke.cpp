// proposals_smoke — the in-house proposal & approval channel: the
// file-backed store round-trips, categories route register-vs-export
// correctly, rulings stamp provenance, and TSV escaping survives tabs
// and newlines inside evidence.
#include <cstdio>
#include <cstdlib>
#include <string>

#include "allcore/proposals.h"

static int failures = 0;
#define CHECK(cond, msg)                                        \
    do {                                                        \
        if (cond) std::printf("  [PASS] %s\n", msg);            \
        else { std::printf("  [FAIL] %s\n", msg); ++failures; } \
    } while (0)

int main(int argc, char** argv) {
    const std::string dir = argc > 1 ? argv[1] : "/tmp";

    {
        allcore::ProposalStore store(dir);
        store.load();   // may be empty; fine
        // start clean by rewriting empty then re-adding
        allcore::ProposalStore fresh(dir);
        auto id1 = fresh.propose(
            allcore::ProposalKind::HighHonorific, "Tenzin",
            "zhal snga nas", "", "high",
            "seen in the colophon of X\twith a tab", "2026-08-08");
        auto id2 = fresh.propose(
            allcore::ProposalKind::PhraseRendering, "Dorje",
            "byang chub sems dpa'", "the warrior for enlightenment", "",
            "line 3 of the passage\nsecond line", "2026-08-08");
        CHECK(!id1.empty() && id1 != id2, "propose returns distinct ids");
        CHECK(fresh.pendingCount() == 2, "two pending after two proposals");
        CHECK(fresh.save(), "store saves");

        allcore::ProposalStore reload(dir);
        CHECK(reload.load(), "store reloads");
        CHECK(reload.all().size() >= 2, "reloaded rows present");
        const allcore::Proposal* p1 = nullptr;
        const allcore::Proposal* p2 = nullptr;
        for (const auto& p : reload.all()) {
            if (p.id == id1) p1 = &p;
            if (p.id == id2) p2 = &p;
        }
        CHECK(p1 && p1->field == "high" && p1->proposer == "Tenzin",
              "high-honorific proposal round-trips");
        CHECK(p1 && p1->evidence.find('\t') != std::string::npos,
              "tab inside evidence survives TSV escaping");
        CHECK(p2 && p2->evidence.find('\n') != std::string::npos,
              "newline inside evidence survives TSV escaping");
        CHECK(p1 && p1->isRegister(),
              "high-honorific routes to the in-app register lane");
        CHECK(p2 && !p2->isRegister(),
              "phrase rendering routes to the export lane");

        // rule on one — provenance stamped
        CHECK(reload.rule(id1, allcore::ProposalStatus::Approved,
                          "Geshe Michael", "yes, high honorific",
                          "2026-08-09"),
              "ruling by id succeeds");
        CHECK(!reload.rule("no-such-id",
                           allcore::ProposalStatus::Approved, "x", "",
                           "2026-08-09"),
              "ruling an unknown id fails cleanly");
        reload.save();

        allcore::ProposalStore after(dir);
        after.load();
        const allcore::Proposal* pa = nullptr;
        for (const auto& p : after.all())
            if (p.id == id1) pa = &p;
        CHECK(pa && pa->status == allcore::ProposalStatus::Approved &&
                  pa->approver == "Geshe Michael" &&
                  pa->ruled == "2026-08-09",
              "approval persists with approver + date");
        CHECK(after.pendingCount() == 1,
              "one still pending after one approval");
    }

    CHECK(allcore::tsvUnescape(allcore::tsvEscape("a\tb\\c\nd")) ==
              "a\tb\\c\nd",
          "escape/unescape is a clean round trip");
    CHECK(std::string(allcore::Proposal::kindName(
              allcore::Proposal::kindFromName("high-honorific"))) ==
              "high-honorific",
          "kind name round-trips");

    std::printf("proposals_smoke: %s (%d failures)\n",
                failures ? "FAIL" : "ALL PASS", failures);
    return failures ? 1 : 0;
}
