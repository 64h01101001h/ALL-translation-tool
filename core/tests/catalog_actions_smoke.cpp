// catalog_actions_smoke — the staged-approval pipeline (Adam's
// in-house directive, 2026-08-20). A scratch "official root" stands
// in for the team's Dropbox folder; every pin exercises the real
// filesystem contract: stage COPIES (mother copy untouched), the
// ledger is plain readable TSV, approve MOVES onto the proposed
// shelf, reject moves to REJECTED, nothing ever overwrites.
// argv[1] = scratch dir.
#include <filesystem>
#include <fstream>
#include <cstdio>
#include <string>

#include "allcore/catalog_actions.h"

namespace fs = std::filesystem;
static int failures = 0;
#define CHECK(cond, msg)                                        \
    do {                                                        \
        if (cond) std::printf("  [PASS] %s\n", msg);            \
        else { std::printf("  [FAIL] %s\n", msg); ++failures; } \
    } while (0)

static void write(const fs::path& p, const std::string& t) {
    std::ofstream(p) << t;
}

int main(int argc, char** argv) {
    setvbuf(stdout, nullptr, _IONBF, 0);
    if (argc < 2) return 2;
    const fs::path wd = fs::path(argv[1]) / "actions_probe";
    fs::remove_all(wd);
    const fs::path root = wd / "official";     // "the Dropbox folder"
    const fs::path intake = wd / "intake";
    fs::create_directories(root);
    fs::create_directories(intake);
    write(intake / "S99991_TEST WORK_A TEST_AUTHOR.TXT",
          "@001A TEST BODY\n");
    write(intake / "S99991 META.TXT", "continuation");

    allcore::ActionLedger led(root.string());
    CHECK(led.load(), "an absent ledger loads as empty");

    const std::string id = led.stage(
        (intake / "S99991_TEST WORK_A TEST_AUTHOR.TXT").string(),
        "S99991_TEST WORK_A TEST_AUTHOR.TXT",
        "Sungbum/TEST SHELF", "Nick Cataloger", "2026-08-20 12:00",
        "identity 92% vs TD03995");
    CHECK(!id.empty(), "staging returns a row id");
    CHECK(fs::exists(intake / "S99991_TEST WORK_A TEST_AUTHOR.TXT"),
          "the mother copy is UNTOUCHED (stage copies, never moves)");
    CHECK(fs::exists(root / "AWAITING APPROVAL" /
                     "S99991_TEST WORK_A TEST_AUTHOR.TXT"),
          "the staged copy sits in AWAITING APPROVAL");
    CHECK(fs::exists(root / "AWAITING APPROVAL" / "S99991 META.TXT"),
          "the META companion travels with the stage");
    {   // the ledger is readable plain text with a comment header
        std::ifstream in(led.ledgerPath());
        std::string all((std::istreambuf_iterator<char>(in)),
                        std::istreambuf_iterator<char>());
        CHECK(all.find("# Cataloging actions") == 0 &&
                  all.find("Nick Cataloger") != std::string::npos &&
                  all.find("pending") != std::string::npos,
              "the ledger is plain readable ASCII TSV");
    }
    // double-stage the same name: refused (never overwrite)
    CHECK(led.stage((intake / "S99991_TEST WORK_A TEST_AUTHOR.TXT")
                        .string(),
                    "S99991_TEST WORK_A TEST_AUTHOR.TXT", "Other",
                    "Nick Cataloger", "2026-08-20 12:01", "") == "",
          "a second stage under the same name is refused");

    // reload from disk (a second machine on the Dropbox share)
    allcore::ActionLedger led2(root.string());
    CHECK(led2.load() && led2.pendingCount() == 1,
          "the ledger round-trips (a teammate's app sees 1 pending)");

    // approve: file lands on the proposed shelf, row marked
    CHECK(led2.approve(id, "Geshe Michael", "2026-08-20") == "",
          "approve succeeds");
    CHECK(fs::exists(root / "Sungbum" / "TEST SHELF" /
                     "S99991_TEST WORK_A TEST_AUTHOR.TXT"),
          "the approved file sits on its proposed shelf");
    CHECK(!fs::exists(root / "AWAITING APPROVAL" /
                      "S99991_TEST WORK_A TEST_AUTHOR.TXT"),
          "…and has left the staging area");
    CHECK(led2.approve(id, "Geshe Michael", "2026-08-20") ==
              "already approved",
          "a second approve is refused, not repeated");

    // a rejected action
    write(intake / "S99992_WRONG_WRONG_X.TXT", "body\n");
    const std::string id2 =
        led2.stage((intake / "S99992_WRONG_WRONG_X.TXT").string(),
                   "S99992_WRONG_WRONG_X.TXT", "Sungbum/TEST SHELF",
                   "Nick Cataloger", "2026-08-20 12:02", "");
    CHECK(led2.reject(id2, "Geshe Michael", "2026-08-20",
                      "wrong identity - recheck the colophon") == "",
          "reject succeeds");
    CHECK(fs::exists(root / "AWAITING APPROVAL" / "REJECTED" /
                     "S99992_WRONG_WRONG_X.TXT"),
          "the rejected file moves to REJECTED, not deleted");
    {
        allcore::ActionLedger led3(root.string());
        led3.load();
        bool found = false;
        for (const auto& r : led3.all())
            if (r.id == id2)
                found = r.status == "rejected" &&
                        r.note.find("recheck the colophon") !=
                            std::string::npos;
        CHECK(found, "the rejection reason survives the round trip");
    }
    // ---- SQA STATIC-1: approve() must not report success over a
    // ledger it could not write. Measured with ACTIONS.tsv chmod 400:
    // approve() returned "" (success), the file sat on the shelf, the
    // ledger on disk still said pending, and the row became
    // UNCLEARABLE because the next approve() found the staged file
    // gone. stage() had always rolled back correctly on the same
    // failure — the rule held only where its author remembered it.
    {
        const fs::path wd2 = fs::temp_directory_path() /
                             "sqa_static1_probe";
        std::error_code ec;
        fs::remove_all(wd2, ec);
        fs::create_directories(wd2 / "intake", ec);
        {
            std::ofstream src(wd2 / "intake" / "S00001_A_B_C.TXT");
            src << "body\n";
        }
        allcore::ActionLedger led(wd2.string());
        led.load();
        const std::string sid =
            led.stage((wd2 / "intake" / "S00001_A_B_C.TXT").string(),
                      "S00001_A_B_C.TXT", "shelf", "Tenzin",
                      "2026-08-23", "note");
        CHECK(!sid.empty() && sid.find(' ') == std::string::npos,
              "staged an action to approve");

        // make the ledger unwritable WITHOUT touching the staged file
        const fs::path ledger = wd2 / "AWAITING APPROVAL" / "ACTIONS.tsv";
        const bool haveLedger = fs::exists(ledger, ec);
        CHECK(haveLedger, "the ledger file exists before the probe");
        fs::permissions(ledger, fs::perms::owner_read,
                        fs::perm_options::replace, ec);

        const std::string err =
            led.approve(sid, "GMR", "2026-08-23");
        CHECK(!err.empty(),
              "approve() REFUSES when the ledger cannot be written "
              "(it once returned success and diverged permanently)");

        // and the rollback: the item must still be staged, not
        // stranded on the shelf with a pending ledger row
        fs::permissions(ledger, fs::perms::owner_all,
                        fs::perm_options::replace, ec);
        const bool stillStaged =
            fs::exists(wd2 / "AWAITING APPROVAL" / "S00001_A_B_C.TXT", ec);
        const bool notOnShelf =
            !fs::exists(wd2 / "shelf" / "S00001_A_B_C.TXT", ec);
        CHECK(stillStaged && notOnShelf,
              "a refused approval leaves the file staged, not "
              "stranded on the shelf");
        bool pending = false;
        for (const auto& r : led.all())
            if (r.id == sid) pending = (r.status == "pending");
        CHECK(pending,
              "a refused approval leaves the row pending, so it can "
              "still be cleared");
        fs::remove_all(wd2, ec);
    }

    fs::remove_all(wd);
    std::printf("%s: %d failure(s)\n", failures ? "FAIL" : "OK",
                failures);
    return failures ? 1 : 0;
}
