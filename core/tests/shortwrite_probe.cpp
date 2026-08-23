// shortwrite_probe — writes a ProposalStore to a directory chosen by
// the caller and reports, on stdout, whether save() CLAIMED success and
// how many bytes actually reached the disk.
//
// It exists because of the 2026-08-22 SQA assessment's sharpest
// finding: not one test in this repository could detect a write
// failure. Five independent "report success when the write failed"
// mutants survived a full green ctest.
//
// The subtle half is what makes this probe necessary. An unwritable
// DIRECTORY is caught by the `if (!f) return false;` at the top of
// save() — that guard was always correct, so a test using a chmod'd
// folder passes with or without the fix and proves nothing. (I wrote
// exactly that test first, mutation-tested it, and watched the mutant
// survive.) The path the fix actually changed is a write that fails
// AFTER a successful open, which needs a volume that runs out of room
// mid-stream — hence tools/test_shortwrite.sh and its RAM disk.
//
// Usage: shortwrite_probe <dir> <rows>
// Prints: CLAIMED=TRUE|FALSE BYTES=<n>
// Exit:   0 always (the shell script judges; a probe that exits
//         non-zero on the interesting case is indistinguishable from a
//         probe that crashed).
#include <cstdio>
#include <cstdlib>
#include <filesystem>
#include <string>

#include "allcore/proposals.h"

int main(int argc, char** argv) {
    if (argc < 3) {
        std::printf("usage: shortwrite_probe <dir> <rows>\n");
        return 2;
    }
    const std::string dir = argv[1];
    const int rows = std::atoi(argv[2]);

    allcore::ProposalStore store(dir);
    for (int i = 0; i < rows; ++i) {
        // long fields on purpose: the row has to be big enough that
        // the serialised store exceeds the hole left on the volume,
        // and big enough to overrun the ofstream's own buffer, which
        // is where a buffered short write hides.
        // Review finding: discarding this in a probe about discarded
        // returns. A refused propose() silently shrinks the payload
        // until it FITS, and the script then blames FAIL-2 for a
        // regression that is really an empty store.
        const std::string pid = store.propose(
                      allcore::ProposalKind::Honorific,
                      "proposer-" + std::to_string(i),
                      "bsod nams kyi phung po " + std::to_string(i),
                      std::string(200, 'x'), "field",
                      std::string(400, 'e'), "2026-08-22");

        if (pid.empty()) {
            std::printf("PROBE BROKEN: propose() refused a row; the "
                        "store is too small to test a short write\n");
            return 3;
        }
    }

    const bool claimed = store.save();

    std::error_code ec;
    const auto path = std::filesystem::path(dir) / "proposals.tsv";
    const auto sz = std::filesystem::file_size(path, ec);
    const long long bytes = ec ? -1 : (long long)sz;

    std::printf("CLAIMED=%s BYTES=%lld\n", claimed ? "TRUE" : "FALSE",
                bytes);
    return 0;
}
