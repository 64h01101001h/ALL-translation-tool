#include "allcore/backup.h"

#include <algorithm>
#include <filesystem>
#include <fstream>

namespace fs = std::filesystem;

namespace allcore {

static std::string baseOf(const std::string& path) {
    return fs::path(path).filename().string();
}

bool backupFile(const std::string& src, const std::string& backupDir,
                const std::string& stamp, int keep) {
    std::error_code ec;
    if (!fs::exists(src, ec)) return false;
    fs::create_directories(backupDir, ec);
    const std::string base = baseOf(src);
    const fs::path dst =
        fs::path(backupDir) / (base + "." + stamp + ".bak");
    fs::copy_file(src, dst, fs::copy_options::overwrite_existing, ec);
    if (ec) return false;
    // prune oldest beyond keep (never fatal)
    std::vector<std::string> mine;
    for (const auto& e : fs::directory_iterator(backupDir, ec)) {
        const std::string n = e.path().filename().string();
        if (n.rfind(base + ".", 0) == 0 &&
            n.size() > 4 && n.substr(n.size() - 4) == ".bak")
            mine.push_back(e.path().string());
    }
    std::sort(mine.begin(), mine.end());
    while ((int)mine.size() > keep && keep > 0) {
        fs::remove(mine.front(), ec);
        mine.erase(mine.begin());
    }
    return true;
}

std::string latestBackup(const std::string& backupDir,
                         const std::string& basename) {
    std::error_code ec;
    std::string best;
    for (const auto& e : fs::directory_iterator(backupDir, ec)) {
        const std::string n = e.path().filename().string();
        if (n.rfind(basename + ".", 0) == 0 &&
            n.size() > 4 && n.substr(n.size() - 4) == ".bak" &&
            (best.empty() ||
             e.path().filename().string() > baseOf(best)))
            best = e.path().string();
    }
    return best;
}

bool restoreBackup(const std::string& backupPath,
                   const std::string& dst) {
    std::error_code ec;
    if (!fs::exists(backupPath, ec)) return false;
    fs::copy_file(backupPath, dst,
                  fs::copy_options::overwrite_existing, ec);
    return !ec;
}

}  // namespace allcore
