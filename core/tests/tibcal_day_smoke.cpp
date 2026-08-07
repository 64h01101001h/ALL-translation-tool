// tibcal_day_smoke — the day-level port vs the original program's own
// outputs: all 76 fixtures must match zla-dag, gza-dag, nyi-dag, and
// the Julian day exactly.
#include <cstdio>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>

#include "allcore/tibcal_day.h"

int main(int argc, char** argv) {
    if (argc < 2) {
        std::fprintf(stderr,
                     "usage: tibcal_day_smoke <kck_fixtures.tsv>\n");
        return 2;
    }
    std::ifstream f(argv[1]);
    if (!f) { std::fprintf(stderr, "no fixtures\n"); return 2; }
    std::string line;
    int total = 0, pass = 0;
    while (std::getline(f, line)) {
        if (line.empty() || line[0] == '#') continue;
        std::istringstream in(line);
        std::string ys, ms, ts, z0s, z1s, gs, ns, js;
        std::getline(in, ys, '\t');
        std::getline(in, ms, '\t');
        std::getline(in, ts, '\t');
        std::getline(in, z0s, '\t');
        std::getline(in, z1s, '\t');
        std::getline(in, gs, '\t');
        std::getline(in, ns, '\t');
        std::getline(in, js, '\t');
        ++total;
        auto d = allcore::kckModernKarana(std::stol(ys), std::stol(ms),
                                          std::stol(ts));
        long g[5], n[5];
        {
            std::istringstream gi(gs), ni(ns);
            for (auto& v : g) gi >> v;
            for (auto& v : n) ni >> v;
        }
        bool ok = d.valid && d.zla0 == std::stol(z0s) &&
                  d.zla1 == std::stol(z1s) && d.jd == std::stol(js);
        for (int i = 0; i < 5; ++i)
            ok = ok && d.gza[i] == g[i] && d.nyi[i] == n[i];
        if (ok) {
            ++pass;
        } else {
            std::printf(
                "  [FAIL] %s-%s tshes %s: got zla %ld;%ld gza "
                "%ld;%ld,%ld,%ld,%ld nyi %ld;%ld,%ld,%ld,%ld jd %ld\n",
                ys.c_str(), ms.c_str(), ts.c_str(), d.zla0, d.zla1,
                d.gza[0], d.gza[1], d.gza[2], d.gza[3], d.gza[4],
                d.nyi[0], d.nyi[1], d.nyi[2], d.nyi[3], d.nyi[4], d.jd);
        }
    }
    std::printf("%d/%d fixtures match the original exactly\n", pass,
                total);
    // pre-epoch honesty
    if (allcore::kckModernKarana(2008, 1, 1).valid) {
        std::printf("  [FAIL] pre-epoch date claimed valid\n");
        return 1;
    }
    // western-date round trip + inverse on every fixture
    std::ifstream f2(argv[1]);
    int rt = 0, rtot = 0, invOk = 0;
    while (std::getline(f2, line)) {
        if (line.empty() || line[0] == '#') continue;
        std::istringstream in(line);
        std::string ys, ms, ts, z0s, z1s, gs, ns, js;
        std::getline(in, ys, '\t'); std::getline(in, ms, '\t');
        std::getline(in, ts, '\t'); std::getline(in, z0s, '\t');
        std::getline(in, z1s, '\t'); std::getline(in, gs, '\t');
        std::getline(in, ns, '\t'); std::getline(in, js, '\t');
        ++rtot;
        const long jd = std::stol(js);
        auto w = allcore::westernFromJd(jd);
        if (allcore::julianDay(w.day, w.month, w.year) == jd) ++rt;
        auto inv = allcore::kckFromJulianDay(jd);
        for (auto& m : inv)
            if (m.year == std::stol(ys) && m.month == std::stol(ms) &&
                m.tshes == std::stol(ts))
                ++invOk;
    }
    std::printf("%d/%d western round-trips; %d/%d inverse lookups "
                "recover the fixture date\n", rt, rtot, invOk, rtot);
    const bool ok2 = (rt == rtot && invOk == rtot);

    std::printf("%s\n", (pass == total && ok2)
                             ? "TIBCAL_DAY SMOKE OK"
                             : "TIBCAL_DAY SMOKE FAILED");
    return (pass == total && ok2) ? 0 : 1;
}
