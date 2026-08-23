// proposals_smoke — the in-house proposal & approval channel: the
// file-backed store round-trips, categories route register-vs-export
// correctly, rulings stamp provenance, and TSV escaping survives tabs
// and newlines inside evidence.
#include <cstdio>
#include <cstdlib>
#include <string>

#include <filesystem>
#include <fstream>
#include <sstream>

#include "allcore/colloquial.h"
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
        // start clean EXPLICITLY: save() now merge-preserves rows on
        // disk (S1 — a never-loaded store must not wipe the queue),
        // so a deliberate reset removes the file itself
        std::remove((dir + "/proposals.tsv").c_str());
        std::remove((dir + "/proposals (test's conflicted copy "
                           "2026-08-20).tsv")
                        .c_str());
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

    // ---- the authority's ruling applied to the register file ----
    {
        const std::string reg = dir + "/ruling_register.tsv";
        auto write = [&] {
            std::ofstream f(reg, std::ios::trunc);
            f << "# colloquial\twylie\tgmr_pron\tclass\n"
                 "tulku\tsprul sku\ttrulku\tcommunity\n"
                 "kamdir\tskabs 'dir\tkap-dir\tprenasal-derived\n"
                 "kyamdro\tskyabs 'gro\tkyap-dro\tprenasal-derived\n"
                 "gendun\tdge 'dun\tgendun\thgm-attested\n";
        };
        auto slurp = [&] {
            std::ifstream f(reg);
            std::stringstream ss;
            ss << f.rdbuf();
            return ss.str();
        };
        write();
        // approve: in-place class upgrade with the ruler's stamp
        CHECK(allcore::applyPronunciationRuling(
                  reg, "kamdir", "skabs 'dir", true, "Geshe Michael",
                  "2026-08-09"),
              "approve finds the derived row");
        std::string s1 = slurp();
        CHECK(s1.find("kamdir\tskabs 'dir\tkap-dir\tapproved\t# ruled "
                      "by Geshe Michael 2026-08-09") != std::string::npos,
              "approve upgrades class in place, stamped");
        CHECK(s1.find("prenasal-derived\n") != std::string::npos,
              "the other derived row is untouched");
        CHECK(s1.find("community") != std::string::npos &&
                  s1.find("hgm-attested") != std::string::npos,
              "community and hgm-attested rows survive verbatim");
        // decline: the derived row is removed
        CHECK(allcore::applyPronunciationRuling(
                  reg, "kyamdro", "skyabs 'gro", false, "Adam",
                  "2026-08-09"),
              "decline finds the derived row");
        std::string s2 = slurp();
        CHECK(s2.find("kyamdro") == std::string::npos,
              "declined derived row is removed");
        // protected classes are structurally untouchable
        CHECK(!allcore::applyPronunciationRuling(
                  reg, "tulku", "sprul sku", false, "x", "2026-08-09"),
              "a community row cannot be removed by a ruling");
        CHECK(!allcore::applyPronunciationRuling(
                  reg, "gendun", "dge 'dun", true, "x", "2026-08-09"),
              "an hgm-attested row cannot be re-classed by a ruling");
        // novel form: no match, file byte-identical
        std::string before = slurp();
        CHECK(!allcore::applyPronunciationRuling(
                  reg, "nowhere", "med pa", true, "x", "2026-08-09"),
              "a novel form returns false (caller appends fresh)");
        CHECK(slurp() == before, "a miss leaves the file byte-identical");
    }

    CHECK(allcore::tsvUnescape(allcore::tsvEscape("a\tb\\c\nd")) ==
              "a\tb\\c\nd",
          "escape/unescape is a clean round trip");
    CHECK(std::string(allcore::Proposal::kindName(
              allcore::Proposal::kindFromName("high-honorific"))) ==
              "high-honorific",
          "kind name round-trips");
    CHECK(std::string(allcore::Proposal::kindName(
              allcore::Proposal::kindFromName("humilific"))) ==
              "humilific",
          "humilific kind round-trips");
    CHECK(std::string(allcore::Proposal::kindName(
              allcore::Proposal::kindFromName("double-honorific"))) ==
              "double-honorific",
          "double-honorific kind round-trips");
    {
        allcore::Proposal p;
        p.kind = allcore::ProposalKind::Humilific;
        CHECK(p.isRegister(), "humilific routes to the register lane");
        p.kind = allcore::ProposalKind::DoubleHonorific;
        CHECK(p.isRegister(),
              "double-honorific routes to the register lane");
    }

    CHECK(std::string(allcore::Proposal::kindName(
              allcore::Proposal::kindFromName("spelling"))) == "spelling",
          "spelling kind round-trips");
    {
        allcore::Proposal p;
        p.kind = allcore::ProposalKind::Spelling;
        CHECK(!p.isRegister(),
              "spelling routes to the export lane (app never edits texts)");
    }

    CHECK(std::string(allcore::Proposal::kindName(
              allcore::Proposal::kindFromName("idiom"))) == "idiom",
          "idiom kind round-trips");
    {
        allcore::Proposal p;
        p.kind = allcore::ProposalKind::Idiom;
        CHECK(!p.isRegister(), "idiom routes to the export lane");
    }


    // the catalog-identity kind (loop 14): name round-trip + export class
    {
        using allcore::Proposal;
        using allcore::ProposalKind;
        CHECK(std::string(Proposal::kindName(
                  ProposalKind::CatalogIdentity)) == "catalog-identity",
              "catalog-identity kind has a stable name");
        CHECK(Proposal::kindFromName("catalog-identity") ==
                  ProposalKind::CatalogIdentity,
              "catalog-identity parses back");
        Proposal p;
        p.kind = ProposalKind::CatalogIdentity;
        CHECK(!p.isRegister(),
              "catalog-identity is an EXPORT kind - approvals leave "
              "through the candidates file, never applied in-app");
    }

    // ---- S1 (stewardship): the Dropbox conflicted-copy drill ----
    {
        std::remove((dir + "/proposals.tsv").c_str());
        std::remove((dir + "/proposals (test's conflicted copy "
                           "2026-08-20).tsv")
                        .c_str());
        allcore::ProposalStore a(dir);
        auto keep = a.propose(allcore::ProposalKind::PhraseRendering,
                              "Tenzin", "bden pa", "truth", "",
                              "ev", "2026-08-20");
        CHECK(a.save(), "main store saves");
        // simulate Dropbox: a sibling holding one NEW row and one
        // DIVERGENT copy of the same id
        {
            allcore::ProposalStore b(dir);
            b.load();
            b.propose(allcore::ProposalKind::PhraseRendering,
                      "Dorje", "sdug bsngal", "suffering", "",
                      "ev2", "2026-08-20");
            for (auto& p :
                 const_cast<std::vector<allcore::Proposal>&>(b.all()))
                if (p.id == keep) p.comment = "diverged";
            const std::string conflictPath =
                dir + "/proposals (test's conflicted copy "
                      "2026-08-20).tsv";
            // write the sibling by saving to a temp store dir is
            // overkill - reuse save() then rename
            allcore::ProposalStore c(dir);
            c.load();
            std::remove((dir + "/proposals.tsv").c_str());
            b.save();
            std::rename((dir + "/proposals.tsv").c_str(),
                        conflictPath.c_str());
            // restore the main file (store a's view)
            a.save();
        }
        allcore::ProposalStore merged(dir);
        CHECK(merged.load(), "load with a conflicted sibling present");
        CHECK(merged.conflictFiles().size() == 1,
              "the conflicted copy is detected and named");
        CHECK(merged.absorbedRows() == 1,
              "the sibling's NEW row joins the queue");
        CHECK(merged.divergentRows() == 1,
              "the diverged same-id row is counted, not guess-merged");
        bool sawNew = false, mainWins = false;
        for (const auto& p : merged.all()) {
            if (p.proposer == "Dorje") sawNew = true;
            if (p.id == keep) mainWins = (p.comment != "diverged");
        }
        CHECK(sawNew, "absorbed row is queryable");
        CHECK(mainWins, "the main file's row wins on divergence");
    }

    // ---- SQA FAIL-2: a save that did not land must SAY so ----
    // The 2026-08-22 assessment's sharpest finding was that not one
    // test in this repository could detect a write failure: five
    // independent "report success when the write failed" mutants all
    // survived a full green ctest. This is the first test that can.
    //
    // ProposalStore::save() ended with a bare `return true;` after an
    // unchecked ofstream and was MEASURED returning TRUE having
    // written 16,384 of 123,576 bytes. proposals.tsv is the
    // PROPOSE/APPROVE channel — a truncated write drops the tail,
    // which is exactly where the newest rulings sit.
    //
    // An unwritable directory is the reachable half of that bug (a
    // read-only or missing data folder is enough — no full disk
    // needed), and it needs no privileges to stage.
    {
        const std::string ro = dir + "/sqa_readonly_probe";
        std::error_code pre;
        std::filesystem::remove_all(ro, pre);
        std::filesystem::create_directories(ro, pre);
        allcore::ProposalStore store(ro);
        store.propose(allcore::ProposalKind::Honorific, "Tenzin",
                      "bsod nams", "merit", "field", "evidence",
                      "2026-08-22");
        CHECK(store.save(), "save succeeds while the folder is writable");
        // Unlink FIRST: removing a file needs write permission on its
        // DIRECTORY, so stripping the directory first makes the
        // cleanup itself impossible. Every call here takes the
        // error_code overload — a test that throws on its own setup
        // reports "aborted", which reads like a product defect.
        std::error_code ec;
        std::filesystem::remove(ro + "/proposals.tsv", ec);
        std::filesystem::permissions(
            ro, std::filesystem::perms::owner_write |
                    std::filesystem::perms::group_write |
                    std::filesystem::perms::others_write,
            std::filesystem::perm_options::remove, ec);
        const bool honest = !store.save();
        // restore before asserting, so a failed assertion cannot
        // leave an undeletable tree behind for the next run
        std::filesystem::permissions(
            ro, std::filesystem::perms::owner_all,
            std::filesystem::perm_options::add, ec);
        std::filesystem::remove_all(ro, ec);
        // HONEST LABEL. This exercises the OPEN guard, which was
        // always correct — it passes with or without the SQA FAIL-2
        // fix, and I proved that by planting the mutant and watching
        // it survive. It is kept as a cheap guard against someone
        // deleting `if (!f) return false;`, NOT as evidence about
        // short writes. The short-write path needs a volume that
        // fills mid-stream: tools/test_shortwrite.sh (ctest target
        // `shortwrite`), which does kill the mutant.
        CHECK(honest,
              "save() reports FALSE when the file cannot be OPENED "
              "(the open guard; short writes are covered by the "
              "`shortwrite` test, not by this one)");
    }

    std::printf("proposals_smoke: %s (%d failures)\n",
                failures ? "FAIL" : "ALL PASS", failures);
    return failures ? 1 : 0;
}
