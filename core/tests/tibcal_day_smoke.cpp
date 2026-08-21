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

    // ---- year battery: every month header + every day of the
    // original's printed years (argv[2] = kck_year_fixtures.tsv) ----
    bool ok3 = true;
    if (argc > 2) {
        std::ifstream fy(argv[2]);
        std::string ln;
        long curYear = 0;
        std::vector<allcore::KckMonth> months;
        int mOk = 0, mTot = 0, dOk = 0, dTot = 0;
        struct DayRow { long y, ix, tt, jd; };
        std::vector<DayRow> dayRows;
        while (std::getline(fy, ln)) {
            if (ln.empty() || ln[0] == '#') continue;
            std::istringstream in(ln);
            std::string kind;
            in >> kind;
            if (kind == "MONTH") {
                long y, ix, disp, ical;
                std::string el, gen, an;
                in >> y >> ix >> disp >> ical >> el >> gen >> an;
                if (y != curYear) {
                    curYear = y;
                    months = allcore::kckYearMonths(y);
                }
                ++mTot;
                if (ix < (long)months.size()) {
                    const auto& m = months[ix];
                    if (m.display_month == disp &&
                        (long)m.intercalary == ical &&
                        m.element_en == el && m.animal_en == an &&
                        m.female == (gen == "female"))
                        ++mOk;
                    else
                        std::printf("  [FAIL] month %ld/%ld: got %d%s "
                                    "%s-%s\n",
                                    y, ix, m.display_month,
                                    m.intercalary ? "i" : "",
                                    m.element_en.c_str(),
                                    m.animal_en.c_str());
                }
            } else if (kind == "DAY") {
                long y, ix, tt, dd, mm, yy;
                in >> y >> ix >> tt >> dd >> mm >> yy;
                dayRows.push_back({y, ix, tt,
                                   allcore::julianDay(dd, mm, yy)});
            }
        }
        // group day rows: a tshes printed TWICE is a duplicated
        // (lhag) lunar day — the engine's single JD is the pair's
        // second member, and the first must be exactly JD-1
        int lhag = 0;
        for (size_t i = 0; i < dayRows.size();) {
            const auto& r = dayRows[i];
            size_t j = i + 1;
            while (j < dayRows.size() && dayRows[j].y == r.y &&
                   dayRows[j].ix == r.ix && dayRows[j].tt == r.tt)
                ++j;
            const long lastJd = dayRows[j - 1].jd;
            ++dTot;
            if (r.y != curYear) {
                curYear = r.y;
                months = allcore::kckYearMonths(r.y);
            }
            bool ok = false;
            if (r.ix < (long)months.size()) {
                auto day = allcore::kckDayForTrueMonth(
                    months[r.ix].true_month, r.tt);
                ok = day.valid && day.jd == lastJd;
                if (ok && j - i == 2) {
                    ok = (dayRows[i].jd == lastJd - 1);
                    ++lhag;
                }
            }
            if (ok)
                ++dOk;
            else
                std::printf("  [FAIL] %ld m#%ld tshes %ld\n", r.y,
                            r.ix, r.tt);
            i = j;
        }
        std::printf("%d/%d month headers, %d/%d days match the "
                    "original's printed years (%d doubled lhag days "
                    "verified as pairs)\n", mOk, mTot, dOk, dTot,
                    lhag);
        ok3 = (mOk == mTot && dOk == dTot && mTot > 0);
    }

    std::printf("%s\n", (pass == total && ok2 && ok3)
                             ? "TIBCAL_DAY SMOKE OK"
                             : "TIBCAL_DAY SMOKE FAILED");
    return (pass == total && ok2 && ok3) ? 0 : 1;
}
