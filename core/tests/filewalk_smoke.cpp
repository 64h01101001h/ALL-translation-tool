// filewalk_smoke — the one eligibility rule and the glob walk (F0).
#include <cstdio>
#include <filesystem>
#include <fstream>
#include <string>
#include <vector>
#include <unistd.h>

#include "allcore/filewalk.h"

namespace fs = std::filesystem;
static int failures = 0;
#define CHECK(cond, msg) do { if (cond) std::printf("  [PASS] %s\n", msg); else { std::printf("  [FAIL] %s\n", msg); ++failures; } } while (0)

static void put(const fs::path& p, size_t bytes) { fs::create_directories(p.parent_path()); std::ofstream f(p, std::ios::binary); std::string s(bytes, 'x'); f.write(s.data(), (std::streamsize)s.size()); }
static bool has(const std::vector<std::string>& v, const char* name) { for (const auto& s : v) if (fs::path(s).filename() == name) return true; return false; }

int main() {
    setvbuf(stdout, nullptr, _IONBF, 0);
    using namespace allcore;
    CHECK(eligibleTextFile("x.ACE", 5) && eligibleTextFile("x.txt", 5) && eligibleTextFile("dir/y.inc", 5), "eligible: .ACE .txt .inc (case-insensitive)");
    CHECK(!eligibleTextFile("x.pdf", 5) && !eligibleTextFile("x", 5), "not eligible: .pdf, no extension");
    CHECK(!eligibleTextFile("x.txt", (11u << 20)), "not eligible: over 10 MB");
    CHECK(globMatch("*.txt", "a.txt") && globMatch("*.txt", "sub/deep/a.TXT") && !globMatch("*.txt", "a.act"), "a pattern without '/' matches the file name anywhere, case-insensitively");
    CHECK(globMatch("sub/*", "sub/d.inc") && !globMatch("sub/*", "sub/deeper/d.inc") && globMatch("sub/**", "sub/deeper/d.inc"), "'*' stays within a directory; '**' crosses");
    CHECK(globMatch("[ab].txt", "b.txt") && !globMatch("[!ab].txt", "b.txt") && globMatch("?.txt", "b.txt"), "classes and '?'");

    const fs::path root = fs::temp_directory_path() / ("all_filewalk_" + std::to_string(::getpid()));
    fs::remove_all(root);
    put(root / "a.txt", 10); put(root / "b.ACT", 10); put(root / "c.bin", 10); put(root / "sub" / "d.inc", 10);
    put(root / "big.txt", (11u << 20)); put(root / "x.bak", 3);
    WalkStats st;
    auto v = listEligibleFiles(root.string(), "*.txt;*.act;*.inc", "sub/*", true, 100, &st);
    CHECK(v.size() == 2 && has(v, "a.txt") && has(v, "b.ACT"), "include globs + exclude: exactly a.txt and b.ACT");
    CHECK(st.ineligibleExt == 2 && st.tooLarge == 1 && st.excluded == 1 && st.capped == 0, "stats: 2 wrong extension (c.bin, x.bak), 1 too large, 1 excluded, 0 capped");
    WalkStats st2;
    auto v2 = listEligibleFiles(root.string(), "*.txt;*.act;*.inc", "sub/*", true, 1, &st2);
    CHECK(v2.size() == 1 && st2.capped == 1, "a file cap of 1 returns one path and COUNTS the other as capped");
    WalkStats st3;
    auto v3 = listEligibleFiles(root.string(), "*.txt;!*.bak", "", false, 100, &st3);
    CHECK(has(v3, "a.txt") && !has(v3, "x.bak") && !has(v3, "d.inc"), "a '!' include excludes; non-recursive skips sub/");
    WalkStats st4;
    auto v4 = listEligibleFiles(root.string(), "", "", true, 100, &st4);
    CHECK(v4.size() == 3 && has(v4, "d.inc"), "an empty include list means every eligible file (a.txt, b.ACT, sub/d.inc)");
    fs::remove_all(root);
    std::printf("filewalk_smoke: %d failure(s)\n", failures);
    return failures ? 1 : 0;
}
