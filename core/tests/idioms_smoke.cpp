// idioms_smoke — the idioms register and its matcher (2026-09-09).
// Fixture-free: every register in this battery is built in the test.

#include "allcore/idioms.h"

#include <cstdio>
#include <string>
#include <vector>

using namespace allcore::idioms;

static int failures = 0;
#define CHECK(cond, msg) do { if (cond) std::printf("  [PASS] %s\n", msg); else { std::printf("  [FAIL] %s\n", msg); ++failures; } } while (0)

static Entry mk(const std::string& w, Status s) {
    Entry e; e.wylie = w; e.status = s; e.evidence = "battery fixture"; return e;
}
static std::vector<std::string> syl(std::initializer_list<const char*> v) {
    std::vector<std::string> out; for (const char* s : v) out.push_back(s); return out;
}

int main() {
    std::printf("idioms_smoke\n");

    // 1 — statuses round-trip, and nothing unrecognised is read as a ruling
    CHECK(statusFromName("approved") == Status::Approved && statusFromName("declined") == Status::Declined &&
              statusFromName("proposed") == Status::Proposed && statusFromName("") == Status::Proposed &&
              statusFromName("APPROVED-ish") == Status::Proposed,
          "1 status names round-trip and unknown text reads as proposed, never as a ruling");
    CHECK(std::string(statusName(Status::Approved)) == "approved" && std::string(statusName(Status::Proposed)) == "proposed",
          "1b status names are written back in the register's own vocabulary");

    // 2 — a form is split into syllables, case-folded, with shads and spacing dropped
    CHECK(splitPattern("BLA NA MED PA") == syl({"bla", "na", "med", "pa"}), "2 an ACIP form splits and case-folds to its syllables");
    CHECK(splitPattern("dper na ,") == syl({"dper", "na"}) && splitPattern(" ,, ").empty(),
          "2b shads and spacing are boundaries, not syllables; a form of only punctuation has none");
    CHECK(splitPattern("rang lag na yod") == syl({"rang", "lag", "na", "yod"}), "2c a four-syllable form keeps its four syllables");

    // 3 — the register holds forms, counts them by ruling, and finds them exactly
    Register r;
    r.addAll({mk("dper na", Status::Proposed), mk("bla na med pa", Status::Approved), mk("kho na", Status::Declined)});
    CHECK(r.size() == 3 && r.countByStatus(Status::Approved) == 1 && r.countByStatus(Status::Proposed) == 1 && r.countByStatus(Status::Declined) == 1,
          "3 three forms, counted by their rulings");
    CHECK(r.find("DPER NA") && r.find("dper na")->status == Status::Proposed && !r.find("nonesuch"),
          "3b a form is found however it is spelled in case, and an unregistered form is not");
    CHECK(!r.find(" ,, "), "3c a form of only punctuation is not registered and cannot be found");

    // 4 — later wins by form, so a ruling loaded after the shipped seed governs
    Register r2;
    r2.add(mk("dper na", Status::Proposed));
    Entry ruled = mk("DPER NA", Status::Approved); ruled.ruledBy = "Geshe Michael"; ruled.ruled = "2026-09-09";
    r2.add(ruled);
    CHECK(r2.size() == 1 && r2.find("dper na")->status == Status::Approved && r2.find("dper na")->ruledBy == "Geshe Michael",
          "4 a later entry for the same form replaces the earlier one, so a ruling overrides the seed");

    // 5 — the matcher finds a form in a text, in syllable indices
    const auto text = syl({"sangs", "rgyas", "bla", "na", "med", "pa", "yin", "dper", "na", "chos"});
    const auto hits = r.findIn(text);
    CHECK(hits.size() == 2 && hits[0].wylie == "bla na med pa" && hits[0].begSyllable == 2 && hits[0].endSyllable == 6 &&
              hits[1].wylie == "dper na" && hits[1].begSyllable == 7 && hits[1].endSyllable == 9,
          "5 both registered forms are found, in text order, with their syllable spans");
    CHECK(hits[0].status == Status::Approved && hits[1].status == Status::Proposed,
          "5b each occurrence carries its own ruling, so the reader is never told a proposal is settled");

    // 6 — longest match wins, and an idiom is consumed whole
    Register r3;
    r3.addAll({mk("bla na", Status::Approved), mk("bla na med pa", Status::Approved)});
    const auto h3 = r3.findIn(syl({"bla", "na", "med", "pa", "yin"}));
    CHECK(h3.size() == 1 && h3[0].wylie == "bla na med pa", "6 the longest registered form wins over the shorter one inside it");
    const auto h3b = r3.findIn(syl({"bla", "na", "yin"}));
    CHECK(h3b.size() == 1 && h3b[0].wylie == "bla na", "6b the shorter form still matches where the longer one does not");

    // 7 — a form ruled NOT an idiom is never marked again
    Register r4;
    r4.add(mk("kho na", Status::Declined));
    CHECK(r4.findIn(syl({"chos", "kho", "na", "yin"})).empty() && r4.find("kho na") != nullptr,
          "7 a declined form is kept as a record but is never reported as an idiom");

    // 8 — syllable boundaries are real: no substring accidents
    Register r5;
    r5.add(mk("na", Status::Approved));
    CHECK(r5.findIn(syl({"nang", "gnas", "na"})).size() == 1 && r5.findIn(syl({"nang", "gnas", "na"}))[0].begSyllable == 2,
          "8 a one-syllable form matches only a whole syllable, never inside nang or gnas");

    // 9 — overlapping occurrences are not double-reported
    Register r6;
    r6.add(mk("na na", Status::Approved));
    const auto h6 = r6.findIn(syl({"na", "na", "na", "na"}));
    CHECK(h6.size() == 2 && h6[0].begSyllable == 0 && h6[1].begSyllable == 2, "9 repeated forms are reported once each, not once per overlap");

    // 10 — an empty register and an empty text are answers, not crashes
    Register r7;
    CHECK(r7.findIn(text).empty() && r.findIn({}).empty() && r7.size() == 0,
          "10 an empty register finds nothing and an empty text has nothing to find");

    std::printf("idioms_smoke: %d failure(s)\n", failures);
    return failures ? 1 : 0;
}
