// allcore/filewalk.h — the ONE definition of "an eligible text file" and
// the walk that lists them (2026-09-09, analysis-suite F0). Search
// (gofer.cpp) and the library index (libindex.cpp) each carried their own
// copy of the rule; Replace in Files would have been a third.
#pragma once

#include <cstdint>
#include <filesystem>
#include <functional>
#include <string>
#include <vector>

namespace allcore {

// .txt .acip .md .act .inc .ace (case-insensitive), at most maxBytes.
bool eligibleTextFile(const std::filesystem::path& p, std::uintmax_t size,
                      std::uintmax_t maxBytes = 10u * 1024 * 1024);

struct WalkStats {
    int scanned = 0;        // regular files seen
    int ineligibleExt = 0;  // wrong extension
    int tooLarge = 0;       // over the size cap
    int excluded = 0;       // eligible but matched an exclude / not an include
    int unreadable = 0;     // could not stat
    int capped = 0;         // eligible but past fileCap — COUNTED, never silently dropped
};

// fnmatch-style glob (*, ?, [...]) against a path relative to the root,
// with '/' as separator; '*' does not cross a '/', "**" does.
bool globMatch(const std::string& pattern, const std::string& relPath);

// Globs are ';'-separated; a leading '!' negates (an exclude). Excludes
// win over includes. An empty include list means "everything eligible".
std::vector<std::string> listEligibleFiles(const std::string& root,
                                           const std::string& includeGlobs,
                                           const std::string& excludeGlobs,
                                           bool recurse, int fileCap, WalkStats* stats,
                                           const std::function<bool()>& keepGoing = {});

}  // namespace allcore
