// backup_smoke — the S2 restore drill: a corrupted store comes back.
// Deterministic stamps; self-cleaning at every entry (the twice-run
// rule, R6).
#include <cstdio>
#include <filesystem>
#include <fstream>
#include <string>

#include "allcore/backup.h"

namespace fs = std::filesystem;

static int failures = 0;
#define CHECK(cond, what)                                   \
    do {                                                    \
        const bool ok = (cond);                             \
        std::printf("  [%s] %s\n", ok ? "PASS" : "FAIL",    \
                    what);                                  \
        if (!ok) ++failures;                                \
    } while (0)

static void write(const std::string& p, const std::string& body) {
    std::ofstream f(p, std::ios::trunc);
    f << body;
}
static std::string slurp(const std::string& p) {
    std::ifstream f(p);
    return std::string(std::istreambuf_iterator<char>(f), {});
}

int main() {
    const fs::path dir =
        fs::temp_directory_path() / "all_backup_smoke";
    std::error_code ec;
    fs::remove_all(dir, ec);   // R6: clean at entry, every run
    fs::create_directories(dir);
    const std::string store = (dir / "proposals.tsv").string();
    const std::string bdir = (dir / "backups").string();

    write(store, "# v1\nrow1\n");
    CHECK(allcore::backupFile(store, bdir, "2026-08-20-100000", 14),
          "first backup lands");
    write(store, "# v2\nrow1\nrow2\n");
    CHECK(allcore::backupFile(store, bdir, "2026-08-20-110000", 14),
          "second backup lands");

    // catastrophe: the live store is corrupted
    write(store, "\xff\xfe garbage");
    const std::string latest =
        allcore::latestBackup(bdir, "proposals.tsv");
    CHECK(!latest.empty() &&
              latest.find("110000") != std::string::npos,
          "latest backup is the newest stamp");
    CHECK(allcore::restoreBackup(latest, store),
          "restore succeeds");
    CHECK(slurp(store) == "# v2\nrow1\nrow2\n",
          "restored content is the newest good copy");

    // pruning: keep=2 discards the oldest of three
    CHECK(allcore::backupFile(store, bdir, "2026-08-20-120000", 2),
          "third backup lands with keep=2");
    CHECK(allcore::latestBackup(bdir, "proposals.tsv")
                  .find("120000") != std::string::npos,
          "latest is now the third");
    int count = 0;
    for (const auto& e : fs::directory_iterator(bdir))
        if (e.path().filename().string().rfind("proposals.tsv.", 0) ==
            0)
            ++count;
    CHECK(count == 2, "prune kept exactly 2");

    // a missing source never phantom-backs-up
    CHECK(!allcore::backupFile((dir / "nope.tsv").string(), bdir,
                               "2026-08-20-130000", 2),
          "missing source refuses honestly");

    fs::remove_all(dir, ec);   // R6: clean at exit too
    std::printf("backup_smoke: %s (%d failures)\n",
                failures ? "FAIL" : "ALL PASS", failures);
    return failures ? 1 : 0;
}
