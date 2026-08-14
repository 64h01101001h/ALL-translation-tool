// Faithful port of the Modern Karana chain in Henning's KCK
// (third_party/kck_henning: T2.C zla_dag/rilchaf/nyi_dru/gza_dru/
// nyi_lon/tse_dru/nyi_dag/gza_dag/spi_zagf, UTILS.C radix helpers,
// driver order from T4.C). Ported from the files, not from spec;
// proven against the original's own outputs.
#include "allcore/tibcal_day.h"

#include <cstdlib>

namespace allcore {

namespace {

using L5 = std::array<long, 5>;

// ---- radix helpers (UTILS.C): places units; 60; 60; 6; n2 ----------
L5 addg(const L5& a2, const L5& a3, long n1, long n2) {
    L5 a1{};
    long r = a2[4] + a3[4];
    a1[4] = r % n2;
    r = a2[3] + a3[3] + r / n2;
    a1[3] = r % 6;
    r = a2[2] + a3[2] + r / 6;
    a1[2] = r % 60;
    r = a2[1] + a3[1] + r / 60;
    a1[1] = r % 60;
    r = a2[0] + a3[0] + r / 60;
    a1[0] = r % n1;
    return a1;
}

L5 subg(const L5& a2, const L5& a3, long n1, long n2) {
    L5 a1{};
    L5 a4 = a2;
    a1[4] = a4[4] - a3[4];
    if (a1[4] < 0) { a1[4] += n2; a4[3] -= 1; }
    a1[3] = a4[3] - a3[3];
    if (a1[3] < 0) { a1[3] += 6; a4[2] -= 1; }
    a1[2] = a4[2] - a3[2];
    if (a1[2] < 0) { a1[2] += 60; a4[1] -= 1; }
    a1[1] = a4[1] - a3[1];
    if (a1[1] < 0) { a1[1] += 60; a4[0] -= 1; }
    a1[0] = a4[0] - a3[0];
    if (a1[0] < 0) a1[0] += n1;
    return a1;
}

L5 mulg(const L5& lst, long x, long n1, long n2) {
    // x is non-negative in every ported call
    long long p0 = (long long)lst[0] * x, p1 = (long long)lst[1] * x,
              p2 = (long long)lst[2] * x, p3 = (long long)lst[3] * x,
              p4 = (long long)lst[4] * x;
    p3 += p4 / n2; p4 %= n2;
    p2 += p3 / 6;  p3 %= 6;
    p1 += p2 / 60; p2 %= 60;
    p0 += p1 / 60; p1 %= 60;
    p0 %= n1;
    return {(long)p0, (long)p1, (long)p2, (long)p3, (long)p4};
}

// div_g6 with frac5 == 1 collapses to a 5-place divide (the only form
// the ported chain uses: div_g6(lista, 135, 13, 1))
L5 divg(const L5& a, long x, long frac4) {
    long long v0 = a[0], v1 = a[1], v2 = a[2], v3 = a[3], v4 = a[4];
    long long r;
    r = v0 % x; v0 /= x; v1 += r * 60;
    r = v1 % x; v1 /= x; v2 += r * 60;
    r = v2 % x; v2 /= x; v3 += r * 6;
    r = v3 % x; v3 /= x; v4 += r * frac4;
    v4 /= x;
    return {(long)v0, (long)v1, (long)v2, (long)v3, (long)v4};
}

// ---- Modern Karana epoch constants (set_epoch, chr=='3') ----------
constexpr long kEpochYear = 2009, kEpochMonth = 2;
constexpr long kIcalInd = 55, kRilA = 4, kRilB = 73;
constexpr long kSpzC = 27, kSpzB = 54, kSpzF = 3, kSpzJ = 2454887;
constexpr long kSunF = 13, kGzaF = 13;   // set_byed

// bases (T2.C statics; byed rtsis daily/monthly means + epoch values)
constexpr L5 kNyidm{2, 10, 58, 2, 10};    // monthly mean sun motion
constexpr L5 kGzadm{1, 31, 50, 0, 0};     // monthly mean weekday motion
constexpr L5 kTsedm{0, 59, 3, 4, 0};      // per-lunar-day weekday motion
constexpr L5 kNyilm{0, 4, 21, 5, 9};      // per-lunar-day sun motion
constexpr L5 kGda3{3, 34, 37, 0, 0};      // weekday at epoch
constexpr L5 kNda3{25, 5, 29, 3, 2};      // sun at epoch
constexpr L5 kNyihaf{13, 30, 0, 0, 0};    // half circle
constexpr long kNyibye[6] = {4, 1, 1, 4, 6, 6};
constexpr long kNyidom[6] = {6, 10, 11, 10, 6, 0};
constexpr long kGzabye[14] = {5, 5, 4, 3, 2, 1, -1, -2, -3, -4, -5, -5, -5, 5};
constexpr long kGzadom[14] = {5, 10, 15, 19, 22, 24, 25, 24, 22, 19, 15, 10, 5, 0};
constexpr L5 kNyifac1{7, 44, 0, 0, 0};   // nyi ma skyes khyim (T3.C)

}  // namespace

KckDay kckDayForTrueMonth(long true_month, long tshes) {
    KckDay out;
    if (true_month < 0) return out;
    out.zla0 = true_month;
    out.zla1 = -1;   // not derived from a (year, month) request
    const long curMth = true_month;

    // ---- monthly means + anomaly ----------------------------------
    const L5 gzadru =
        addg(mulg(kGzadm, curMth, 7, kGzaF), kGda3, 7, kGzaF);
    const L5 nyidru =
        addg(mulg(kNyidm, curMth, 27, kSunF), kNda3, 27, kSunF);
    long rilcha0, rilcha1;
    {
        const long rb = curMth + kRilB;
        const long ra = 2 * curMth + kRilA + rb / 126;
        rilcha1 = rb % 126;
        rilcha0 = ra % 28;
    }

    // ---- daily means ----------------------------------------------
    const L5 tsedru = mulg(kTsedm, tshes, 7, kGzaF);
    const L5 nyilon = mulg(kNyilm, tshes, 27, kSunF);
    const L5 tsebar = addg(gzadru, tsedru, 7, kGzaF);
    const L5 nyibar = addg(nyidru, nyilon, 27, kSunF);

    // ---- nyi_dag: true sun (must run first — sets sol_cor/nyidor) --
    L5 solCor{};
    int nyidor = 0;
    {
        L5 nyiwor = subg(nyibar, kNyifac1, 27, kSunF);
        long test = 60 * nyiwor[0] + nyiwor[1];
        if (test >= 810) {
            nyidor = 1;
            nyiwor = subg(nyiwor, kNyihaf, 27, kSunF);
            test = 60 * nyiwor[0] + nyiwor[1];
        }
        const long trem = test % 135;
        long tquo = test / 135;
        if (tquo == 0) tquo = 6;
        L5 lista{};
        lista[2] = (trem * 60 + nyiwor[2]) * kNyibye[tquo - 1];
        lista[3] = nyiwor[3] * kNyibye[tquo - 1];
        lista[4] = nyiwor[4] * kNyibye[tquo - 1];
        lista = divg(lista, 135, kSunF);
        lista = addg(L5{}, lista, 27, kSunF);   // normalize carries
        L5 listb{};
        listb[1] = kNyidom[tquo - 1];
        if (tquo == 3 || tquo == 4 || tquo == 5)
            solCor = subg(listb, lista, 27, kSunF);
        else
            solCor = addg(listb, lista, 27, kSunF);
        if (nyidor == 0)
            out.nyi = subg(nyibar, solCor, 27, kSunF);
        else
            out.nyi = addg(nyibar, solCor, 27, kSunF);
    }

    // ---- gza_dag: true weekday ------------------------------------
    {
        const long rilpo = rilcha0 + tshes;
        long trem = rilpo % 14;
        if (trem == 0) trem = 14;
        L5 list2{};
        list2[1] = kGzadom[trem - 1];
        // the "more normal in Tibetan systems" branch (gza_short_flg=0)
        const long long tot =
            (long long)(30 * rilcha1 + tshes) * kGzabye[trem - 1] *
            360LL * kGzaF / 3780LL;
        L5 list1{};
        list1[4] = std::labs((long)tot);
        list1 = addg(list1, L5{}, 7, kGzaF);   // normalize carries
        L5 list3 = (tot < 0) ? subg(list2, list1, 7, kGzaF)
                             : addg(list2, list1, 7, kGzaF);
        L5 gzawor = ((rilpo / 14) % 2 == 0)
                        ? addg(tsebar, list3, 7, kGzaF)
                        : subg(tsebar, list3, 7, kGzaF);
        if (nyidor == 0)
            out.gza = subg(gzawor, solCor, 7, kSunF);
        else
            out.gza = addg(gzawor, solCor, 7, kSunF);
    }

    // ---- spi_zagf: general day → Julian day ------------------------
    {
        long spizag = curMth * 30 + tshes;
        const long c = spizag + kSpzC;
        long bb = spizag + kSpzB;
        bb = bb + c / 707;
        spizag = spizag - bb / 64;
        long cc = (spizag + kSpzF) % 7;
        long bw = out.gza[0];
        if (cc != bw) {
            if (cc > 4 && bw < 2) bw += 7;
            else if (bw > 4 && cc < 2) cc += 7;
            spizag = spizag + bw - cc;
        }
        out.spyi = spizag;
        out.jd = spizag + kSpzJ;
    }
    out.valid = true;
    return out;
}

KckDay kckModernKarana(long year, long month, long tshes) {
    // zla_dag from (year, month), then the shared true-month path
    const long yr = year - kEpochYear;
    const long a = 12 * yr + month - kEpochMonth;
    if (a < 0) return {};   // the original refuses pre-epoch dates
    const long b = 2 * a + kIcalInd;
    KckDay out = kckDayForTrueMonth(a + b / 65, tshes);
    out.zla1 = b % 65;
    return out;
}

// ---- month enumeration + names (T4.C, tsurlug==1 path) -------------
namespace {
// ORDER IS: "Fire", "Earth", "Iron", "Water", "Wood" (T4.C comment)
const char* kMElem[5] = {"Fire", "Earth", "Iron", "Water", "Wood"};
// cycanim reconstructed exactly from mt_animx=(m+10)%12 against the
// fixture months (kc_2010/kc_2012): index 11 = Tiger (month 1)
const char* kMAnim[12] = {"Rabbit", "Dragon", "Snake",  "Horse",
                          "Sheep",  "Monkey", "Bird",   "Dog",
                          "Pig",    "Mouse",  "Ox",     "Tiger"};

void monthName(long ty, long mthnum, KckMonth& m) {
    const long yrElem = (ty / 2 - 3) % 5;
    const int yrGender = (int)(ty % 2);   // 1 = female, odd CE
    m.animal_en = kMAnim[(mthnum + 10) % 12];
    long e = -1;
    const long g = yrGender;
    const long ye = yrElem;
    if ((ye == 4 && g == 0) || (ye == 1 && g == 1))
        e = 0 + (mthnum - 1) / 2;
    if ((ye == 4 && g == 1) || (ye == 2 && g == 0))
        e = 1 + (mthnum - 1) / 2;
    if ((ye == 0 && g == 0) || (ye == 2 && g == 1))
        e = 2 + (mthnum - 1) / 2;
    if ((ye == 1 && g == 0) || (ye == 3 && g == 1))
        e = 4 + (mthnum - 1) / 2;
    if ((ye == 0 && g == 1) || (ye == 3 && g == 0))
        e = 3 + (mthnum - 1) / 2;
    if (yrGender == 0 && (mthnum == 11 || mthnum == 12))
        e = ye + (mthnum + 1) / 2;   // the male-year exception (T4.C)
    m.element_en = kMElem[e % 5];
    m.female = ((mthnum + 1) % 2) == 1;
}
}  // namespace

std::vector<KckMonth> kckYearMonths(long year) {
    std::vector<KckMonth> out;
    // the epoch year (2009) mixes pre-epoch months with epoch-forward
    // ones, and the original program's own print of it is boundary-
    // corrupted — refused rather than guessed (fixtures prove 2010+)
    if (year < 2010) return out;
    for (long tm = 1; tm <= 12; ++tm) {
        const long a = 12 * (year - kEpochYear) + tm - kEpochMonth;
        if (a < 0) continue;
        const long b = 2 * a + kIcalInd;
        const long zla1 = b % 65;
        const long zla0 = a + b / 65;
        if (zla1 == 0 || zla1 == 1) {
            // intercalary first (previous display number), then the
            // regular month — the original's print order (kc_2012)
            KckMonth mi;
            mi.display_month = (int)(tm - 1 == 0 ? 12 : tm - 1);
            mi.intercalary = true;
            mi.true_month = zla0 - 1;
            monthName(year, mi.display_month, mi);
            out.push_back(mi);
        }
        KckMonth m;
        m.display_month = (int)tm;
        m.intercalary = false;
        m.true_month = zla0;
        monthName(year, m.display_month, m);
        out.push_back(m);
    }
    return out;
}

long julianDay(long D, long M, long Y) {
    int gregorian;
    if (Y > 1582) gregorian = 1;
    else if (Y < 1582) gregorian = 0;
    else if (M > 10) gregorian = 1;
    else if (M < 10) gregorian = 0;
    else gregorian = (D >= 15) ? 1 : 0;
    if (gregorian)
        return D + (1461 * (Y + 4800 + (M - 14) / 12)) / 4 +
               (367 * (M - 2 - 12 * ((M - 14) / 12))) / 12 -
               (3 * ((Y + 4900 + (M - 14) / 12) / 100)) / 4 - 32075;
    return 367 * Y - (7 * (Y + 5001 + (M - 9) / 7)) / 4 +
           (275 * M) / 9 + D + 1729777;
}

WesternDate westernFromJd(long jd) {
    // standard inverse of the ESAA formula (Gregorian; valid for the
    // epoch range 2009+ this engine serves)
    long l = jd + 68569;
    const long n = (4 * l) / 146097;
    l = l - (146097 * n + 3) / 4;
    const long i = (4000 * (l + 1)) / 1461001;
    l = l - (1461 * i) / 4 + 31;
    const long j = (80 * l) / 2447;
    WesternDate w;
    w.day = l - (2447 * j) / 80;
    l = j / 11;
    w.month = j + 2 - 12 * l;
    w.year = 100 * (n - 49) + i + l;
    return w;
}

std::vector<KckMatch> kckFromJulianDay(long jd) {
    std::vector<KckMatch> out;
    // locate candidate months by mean rate (~30.44 civil days per
    // true month of 30 lunar days), then scan exactly
    const long approx = (jd - 2454887) * 64 / 63 / 30;   // ~true months
    for (long cm = approx - 2; cm <= approx + 2; ++cm) {
        if (cm < 0) continue;
        // invert zla0 -> (year, month): find a with a + (2a+55)/65 == cm
        for (long a = cm - (cm / 32) - 2; a <= cm + 1; ++a) {
            if (a < 0) continue;
            if (a + (2 * a + 55) / 65 != cm) continue;
            const long year = 2009 + (a + 1) / 12;   // month back-calc
            const long month = (a + 1) % 12 + 1;
            // re-derive exactly as forward does
            for (long tt = 1; tt <= 30; ++tt) {
                auto d = kckModernKarana(year, month, tt);
                if (d.valid && d.jd == jd)
                    out.push_back({year, month, tt, d});
            }
            break;
        }
    }
    return out;
}

}  // namespace allcore
