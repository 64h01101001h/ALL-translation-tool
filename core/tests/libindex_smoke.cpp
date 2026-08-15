// libindex_smoke — the prebuilt library full-text index: build, search,
// incremental update, removal.
#include <cstdio>
#include <filesystem>
#include <fstream>

#include "allcore/libindex.h"
#include "allcore/searchnorm.h"

static int failures = 0;
#define CHECK(cond, msg)                                        \
    do {                                                        \
        if (cond) std::printf("  [PASS] %s\n", msg);            \
        else { std::printf("  [FAIL] %s\n", msg); ++failures; } \
    } while (0)

namespace fs = std::filesystem;

static void write(const fs::path& p, const char* text) {
    std::ofstream f(p);
    f << text;
}

int main() {
    setvbuf(stdout, nullptr, _IONBF, 0);
    const fs::path root = fs::temp_directory_path() / "libindex_smoke_root";
    fs::remove_all(root);
    fs::create_directories(root / "kangyur");
    write(root / "kangyur" / "KL0001MA.ACT",
          "BDEN PA BZHI NI\nSDUG BSNGAL DANG\nKUN 'BYUNG DANG\n'GOG PA DANG "
          "LAM MO\nBDEN PA'I DON LA\n");
    write(root / "notes.txt",
          "line one\nBDEN PA appears here\nfar away\nfar away\nSDUG BSNGAL "
          "here\n");
    write(root / "skip.pdf", "not indexed");

    const std::string dbp = (root / ".index.db").string();
    allcore::LibraryIndex ix(dbp);
    auto st = ix.update(root.string());
    CHECK(st.added == 2 && st.removed == 0,
          "initial update indexes the two eligible files (.pdf skipped)");
    CHECK(ix.fileCount() == 2 && ix.lineCount() == 10,
          "file and line counts recorded");

    auto hits = ix.search("\"bden pa\"");
    CHECK(hits.size() == 3, "phrase found in both files (3 windows — "
                            "incl. the affixed line's own)");
    // affix tolerance: the BDEN PA'I line matches a bden pa search via
    // the normalized column (same authority as the corpus spine)
    bool affixLine = false;
    for (auto& h : hits)
        for (auto& l : h.lines)
            affixLine |= (l.find("PA'I DON") != std::string::npos);
    CHECK(affixLine, "BDEN PA'I line found by the bden pa search");
    bool actHit = false;
    for (auto& h : hits) actHit |= h.file.find("KL0001MA.ACT") != std::string::npos;
    CHECK(actHit, ".ACT collection files are searchable");

    auto near1 = ix.search("\"BDEN PA\" NEAR/3 \"SDUG BSNGAL\"");
    auto near2 = ix.search("\"BDEN PA\" NEAR/1 \"SDUG BSNGAL\"");
    CHECK(near1.size() == 3 && near2.size() == 1,
          "NEAR windows honor the line bound per file (affixed line "
          "joins the /3 window)");
    CHECK(!near1[0].lines.empty(), "hit windows are hydrated with line text");

    auto orHits = ix.search("\"KUN 'BYUNG\" OR \"far away\"");
    CHECK(orHits.size() == 3, "OR unions across files");

    // incremental: unchanged files skipped; edits re-index; deletions purge
    auto st2 = ix.update(root.string());
    CHECK(st2.unchanged == 2 && st2.added == 0 && st2.updated == 0,
          "second update touches nothing");
    // fold-generation stamp: configuring the lemma fold after an
    // index was built forces one full reindex, so norms built
    // without the fold heal — and a past-form line becomes
    // reachable from its present stem
    const fs::path verbs = fs::temp_directory_path() / "libindex_verbs.tsv";
    write(verbs, "bkag\t'gog\n");
    allcore::configureLemmaFold(verbs.string());
    write(root / "past.txt", "BKAG PA YIN\n");
    auto stf = ix.update(root.string());
    CHECK(stf.added == 1 && stf.updated == 2 && stf.unchanged == 0,
          "lemma-fold generation change reindexes every file once");
    bool pastHit = false;
    for (auto& h : ix.search("\"'gog\""))
        for (auto& l : h.lines)
            pastHit |= l.find("BKAG") != std::string::npos;
    CHECK(pastHit, "past form BKAG found by its present stem 'gog");
    auto stf2 = ix.update(root.string());
    CHECK(stf2.unchanged == 3 && stf2.updated == 0,
          "stamp settles: next update touches nothing");
    fs::remove(verbs);

    fs::last_write_time(root / "notes.txt",
                        fs::file_time_type::clock::now());
    write(root / "notes.txt", "completely new content\n");
    auto st3 = ix.update(root.string());
    CHECK(st3.updated == 1, "edited file re-indexes");
    CHECK(ix.search("\"far away\"").empty(),
          "stale lines are purged from the FTS index");
    fs::remove(root / "kangyur" / "KL0001MA.ACT");
    auto st4 = ix.update(root.string());
    CHECK(st4.removed == 1 && ix.fileCount() == 2,
          "deleted file leaves the index");
    CHECK(ix.search("\"bden pa\"").empty(), "its lines are gone too");

    fs::remove_all(root);
    std::printf("%s (%d failures)\n",
                failures ? "LIBINDEX SMOKE FAILED" : "LIBINDEX SMOKE OK",
                failures);
    return failures ? 1 : 0;
}
