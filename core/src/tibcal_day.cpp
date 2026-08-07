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

KckDay kckModernKarana(long year, long month, long tshes) {
    KckDay out;

    // ---- zla_dag: true month --------------------------------------
    const long yr = year - kEpochYear;
    const long a = 12 * yr + month - kEpochMonth;
    if (a < 0) return out;   // the original refuses pre-epoch dates
    const long b = 2 * a + kIcalInd;
    out.zla1 = b % 65;
    out.zla0 = a + b / 65;
    const long curMth = out.zla0;

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

}  // namespace allcore
