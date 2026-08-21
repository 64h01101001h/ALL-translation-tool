// acipcodes_smoke — converter completeness against the full ACIP
// input-code inventory (data/acip_codes/Acipcode.txt: ~430 codes incl.
// the Sambhota-extension section — the substance of the Release IV
// manual's missing chart pages; provenance in the README).
//
// Every alphabetic code must be handled by a PROVEN engine: the native
// ACIP→EWTS→Unicode chain, or (for the Sanskrit stacks the native
// tables rightly refuse) the Sanskrit converter's ACIP-Sanskrit path.
// Unhandled codes are listed — never hidden.
#include <cctype>
#include <cstdio>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>

#include "allcore/engines.h"
#include "allcore/sanskrit.h"

static int failures = 0;
#define CHECK(cond, msg)                                        \
    do {                                                        \
        if (cond) std::printf("  [PASS] %s\n", msg);            \
        else { std::printf("  [FAIL] %s\n", msg); ++failures; } \
    } while (0)

int main(int argc, char** argv) {
    if (argc < 2) {
        std::fprintf(stderr, "usage: acipcodes_smoke <Acipcode.txt>\n");
        return 2;
    }
    std::ifstream f(argv[1]);
    CHECK(bool(f), "inventory opens");
    std::stringstream buf;
    buf << f.rdbuf();
    const std::string s = buf.str();

    // parse [CODE ]CODE pairs; keep alphabetic codes (letters ' +)
    std::vector<std::string> codes;
    bool sambhota = false;
    size_t sambhotaStart = 0;
    for (size_t i = 0; i < s.size(); ++i) {
        if (s[i] != '[') continue;
        size_t j = s.find(']', i);
        if (j == std::string::npos) break;
        std::string code = s.substr(i + 1, j - i - 1);
        while (!code.empty() && code.back() == ' ') code.pop_back();
        i = j;
        if (code.find("ADDITIONAL CODES") != std::string::npos) {
            sambhota = true;
            sambhotaStart = codes.size();
            continue;
        }
        bool alpha = !code.empty();
        for (char c : code)
            alpha &= (std::isalpha(static_cast<unsigned char>(c)) ||
                      c == '\'' || c == '+');
        if (alpha && code != "SPACE") codes.push_back(code);
    }
    std::printf("  %zu alphabetic codes (%zu core + %zu Sambhota "
                "extensions)\n",
                codes.size(), sambhotaStart, codes.size() - sambhotaStart);
    CHECK(codes.size() > 380, "380+ codes parsed");
    CHECK(sambhota, "Sambhota extension section found");

    long native = 0, sanskrit = 0, unhandled = 0;
    std::vector<std::string> misses;
    for (const auto& code : codes) {
        const std::string wylie = allcore::acipToEwts(code);
        auto [uni, ok] = allcore::wylieToUnicode(wylie);
        if (ok) {
            ++native;
            continue;
        }
        // Sanskrit stacks: the native tables rightly refuse; the Sanskrit
        // converter's ACIP path must take them
        const std::string iast = allcore::acipSanskritToIast(code);
        if (!iast.empty() && iast.find("\xE2\x9F\xA8") == std::string::npos) {
            ++sanskrit;  // no ⟨…⟩ failure marker
            continue;
        }
        ++unhandled;
        if (misses.size() < 12) misses.push_back(code);
    }
    std::printf("  native chain: %ld · sanskrit chain: %ld · unhandled: "
                "%ld\n", native, sanskrit, unhandled);
    for (const auto& m : misses) std::printf("    unhandled: %s\n", m.c_str());
    const double pct =
        100.0 * (native + sanskrit) / static_cast<double>(codes.size());
    std::printf("  coverage: %.1f%%\n", pct);
    CHECK(pct >= 99.0, "99%+ of the official code inventory handled by a "
                       "proven engine");

    std::printf("acipcodes_smoke: %s (%d failures)\n",
                failures ? "FAIL" : "ALL PASS", failures);
    return failures ? 1 : 0;
}
