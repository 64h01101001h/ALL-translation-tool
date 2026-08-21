// phugpa_smoke — the true-Phugpa port against 21 full years the
// unmodified TCG oracle printed (fixtures/pl_2015..2035): month
// sequence with intercalary/delayed labels, per-day tithi sequence
// including omitted (chad) and duplicated (lhag) days, and every
// dated day's Gregorian date and weekday.
#include <cstdio>
#include <cstring>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>

#include "allcore/tibcal_day.h"
#include "allcore/tibcal_phugpa.h"

static int failures = 0;
#define CHECK(cond, msg)                                        \
    do {                                                        \
        if (cond) std::printf("  [PASS] %s\n", msg);            \
        else { std::printf("  [FAIL] %s\n", msg); ++failures; } \
    } while (0)

struct FixDay {
    int tt;
    bool omitted;
    int d = 0, mo = 0, y = 0;   // gregorian (dated lines)
    std::string wday;
};
struct FixMonth {
    int month;
    bool intercalary, delayed;
    std::vector<FixDay> days;   // duplicated tithis appear TWICE
};

static const char* kMon[] = {"Jan", "Feb", "Mar", "Apr", "May",
                             "Jun", "Jul", "Aug", "Sep", "Oct",
                             "Nov", "Dec"};
static const char* kWd[] = {"Sun", "Mon", "Tue", "Wed",
                            "Thu", "Fri", "Sat"};

static std::vector<FixMonth> parseFixture(const std::string& path) {
    std::vector<FixMonth> out;
    std::ifstream f(path);
    std::string line;
    while (std::getline(f, line)) {
        if (line.rfind("Tibetan Lunar Month:", 0) == 0) {
            FixMonth m{};
            m.month = std::atoi(line.c_str() + 20);
            m.intercalary =
                line.find("(Intercalary)") != std::string::npos;
            m.delayed = line.find("(Delayed)") != std::string::npos;
            out.push_back(m);
            continue;
        }
        if (out.empty()) continue;
        // "12. Omitted: ..." or "12: Fri. ...; 28 Feb 2025"
        size_t i = 0;
        while (i < line.size() && isdigit((unsigned char)line[i]))
            ++i;
        if (i == 0 || i > 2) continue;
        if (line[i] == '.' && line.find("Omitted:") != std::string::npos) {
            FixDay d{};
            d.tt = std::atoi(line.c_str());
            d.omitted = true;
            out.back().days.push_back(d);
        } else if (line[i] == ':') {
            FixDay d{};
            d.tt = std::atoi(line.c_str());
            d.wday = line.substr(i + 2, 3);
            const size_t semi = line.rfind(';');
            if (semi == std::string::npos) continue;
            int day, year;
            char mon[8] = {0};
            if (std::sscanf(line.c_str() + semi + 1, " %d %7s %d",
                            &day, mon, &year) == 3) {
                d.d = day;
                d.y = year;
                for (int k = 0; k < 12; ++k)
                    if (!std::strcmp(mon, kMon[k])) d.mo = k + 1;
                out.back().days.push_back(d);
            }
        }
    }
    return out;
}

int main(int argc, char** argv) {
    setvbuf(stdout, nullptr, _IONBF, 0);
    if (argc < 2) {
        std::fprintf(stderr, "usage: phugpa_smoke <fixtures dir>\n");
        return 2;
    }
    const std::string dir = argv[1];
    long months = 0, dayLines = 0, dups = 0, omits = 0;
    for (int y = 2015; y <= 2035; ++y) {
        char pathBuf[512];
        std::snprintf(pathBuf, sizeof pathBuf, "%s/pl_%d.txt",
                      dir.c_str(), y);
        auto fix = parseFixture(pathBuf);
        if (fix.empty()) {
            std::printf("  [FAIL] fixture %d missing/unparsed\n", y);
            ++failures;
            continue;
        }
        auto port = allcore::phugpaYear(y);
        if (!port.valid || port.months.size() != fix.size()) {
            std::printf("  [FAIL] %d: month count port=%zu fix=%zu\n",
                        y, port.months.size(), fix.size());
            ++failures;
            continue;
        }
        for (size_t mi = 0; mi < fix.size(); ++mi) {
            const auto& fm = fix[mi];
            const auto& pm = port.months[mi];
            ++months;
            if (fm.month != pm.month ||
                fm.intercalary != pm.intercalary ||
                fm.delayed != pm.delayed) {
                std::printf("  [FAIL] %d m#%zu: label port=%d/%d/%d "
                            "fix=%d/%d/%d\n",
                            y, mi, pm.month, pm.intercalary,
                            pm.delayed, fm.month, fm.intercalary,
                            fm.delayed);
                ++failures;
                continue;
            }
            // expand the port month into printed-day order
            struct P {
                int tt;
                bool omitted;
                long jd;
            };
            std::vector<P> pd;
            for (const auto& d : pm.days) {
                if (d.omitted) {
                    pd.push_back({d.tt, true, 0});
                } else if (d.duplicated) {
                    pd.push_back({d.tt, false, d.jd - 1});
                    pd.push_back({d.tt, false, d.jd});
                    ++dups;
                } else {
                    pd.push_back({d.tt, false, d.jd});
                }
            }
            if (pd.size() != fm.days.size()) {
                std::printf("  [FAIL] %d m%d: day-line count "
                            "port=%zu fix=%zu\n",
                            y, fm.month, pd.size(), fm.days.size());
                ++failures;
                continue;
            }
            for (size_t di = 0; di < pd.size(); ++di) {
                const auto& a = pd[di];
                const auto& b = fm.days[di];
                ++dayLines;
                if (a.tt != b.tt || a.omitted != b.omitted) {
                    std::printf("  [FAIL] %d m%d line %zu: tithi "
                                "port=%d/%d fix=%d/%d\n",
                                y, fm.month, di, a.tt, a.omitted,
                                b.tt, b.omitted);
                    ++failures;
                    break;
                }
                if (a.omitted) {
                    ++omits;
                    continue;
                }
                const auto w = allcore::westernFromJd(a.jd);
                const long wd = (a.jd + 1) % 7;
                if (w.day != b.d || w.month != b.mo ||
                    w.year != b.y || b.wday != kWd[wd]) {
                    std::printf(
                        "  [FAIL] %d m%d tt%d: port %ld-%ld-%ld "
                        "%s vs fix %d-%d-%d %s\n",
                        y, fm.month, a.tt, w.year, w.month, w.day,
                        kWd[wd], b.y, b.mo, b.d, b.wday.c_str());
                    ++failures;
                    break;
                }
            }
        }
    }
    std::printf("phugpa_smoke: 21 years, %ld months, %ld day lines "
                "(%ld duplicated pairs, %ld omitted), %d failure(s)\n",
                months, dayLines, dups, omits, failures);
    CHECK(months > 250 && dayLines > 7000 && dups > 50 && omits > 50,
          "coverage floor: full span exercised");
    return failures ? 1 : 0;
}
