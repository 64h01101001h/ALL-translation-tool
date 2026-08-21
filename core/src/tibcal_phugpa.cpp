// Faithful port of the Generalised Phugpa (epoch 1) chain in
// Henning's TCG (third_party/tcg_henning: t2.c set_grub/set_epoch/
// zla_dag/adj_zla/rilchaf/spi_zagf/nyi_dru/gza_dru/nyi_lon/tse_dru/
// nyi_dag/gza_dag, utils.c radix helpers, driver order and the
// chad/lhag day logic from t4.c print_calendar). Ported from the
// files, not from spec; proven against the original's own printed
// calendars (phugpa_smoke, fixtures/pl_2015..2035).
#include "allcore/tibcal_phugpa.h"

#include <array>

#include "allcore/tibcal_day.h"  // julianDay / westernFromJd (proven)

namespace allcore {

namespace {

using L5 = std::array<long long, 5>;

// Phugpa epoch constants (t2.c set_epoch, chr == '1')
constexpr long kEpchYr = -1000;
constexpr long kEyrA = 7;      // intercalation index
constexpr long kRilA = 6, kRilB = 61;
constexpr long kSpzC = 10, kSpzB = 26, kSpzF = 5, kSpzJ = 1355847;
constexpr long kZlaSho1 = 48, kZlaSho2 = 49;
constexpr long kSunF = 67, kGzaF = 707;  // set_grub fractions

// grub rtsis mean motions (t2.c)
constexpr long kNyiDm[5] = {2, 10, 58, 1, 17};    // sun / true month
constexpr long kGzaDm[5] = {1, 31, 50, 0, 480};   // weekday / month
constexpr long kTseDm[5] = {0, 59, 3, 4, 16};     // weekday / tithi
constexpr long kNyiLm[5] = {0, 4, 21, 5, 43};     // sun / tithi
// epoch positions (gda1 / nda1)
constexpr long kGzaDa[5] = {5, 35, 36, 4, 160};
constexpr long kNyiDa[5] = {26, 45, 53, 4, 26};
// sun-equation constants and tables
constexpr long kNyiFac[5] = {6, 45, 0, 0, 0};
constexpr long kNyiHaf[5] = {13, 30, 0, 0, 0};
constexpr long kNyiBye[6] = {4, 1, 1, 4, 6, 6};
constexpr long kNyiDom[6] = {6, 10, 11, 10, 6, 0};
// moon-equation tables
constexpr long kGzaBye[14] = {5, 5, 4,  3,  2,  1, -1,
                              -2, -3, -4, -5, -5, -5, 5};
constexpr long kGzaDom[14] = {5, 10, 15, 19, 22, 24, 25,
                              24, 22, 19, 15, 10, 5, 0};

// ---- radix helpers (utils.c): places units; 60; 60; 6; n2 --------
L5 addg(const L5& a, const L5& b, long n1, long n2) {
    L5 r{};
    long long c = a[4] + b[4];
    r[4] = c % n2;
    c = a[3] + b[3] + c / n2;
    r[3] = c % 6;
    c = a[2] + b[2] + c / 6;
    r[2] = c % 60;
    c = a[1] + b[1] + c / 60;
    r[1] = c % 60;
    c = a[0] + b[0] + c / 60;
    r[0] = c % n1;
    return r;
}

L5 subg(const L5& a, const L5& b, long n1, long n2) {
    L5 r{};
    L5 t = a;
    r[4] = t[4] - b[4];
    if (r[4] < 0) { r[4] += n2; t[3] -= 1; }
    r[3] = t[3] - b[3];
    if (r[3] < 0) { r[3] += 6; t[2] -= 1; }
    r[2] = t[2] - b[2];
    if (r[2] < 0) { r[2] += 60; t[1] -= 1; }
    r[1] = t[1] - b[1];
    if (r[1] < 0) { r[1] += 60; t[0] -= 1; }
    r[0] = t[0] - b[0];
    if (r[0] < 0) r[0] += n1;
    return r;
}

L5 mulg(const L5& l, long long x, long n1, long n2) {
    long long p0 = l[0] * x, p1 = l[1] * x, p2 = l[2] * x,
              p3 = l[3] * x, p4 = l[4] * x;
    p3 += p4 / n2; p4 %= n2;
    p2 += p3 / 6;  p3 %= 6;
    p1 += p2 / 60; p2 %= 60;
    p0 += p1 / 60; p1 %= 60;
    p0 %= n1;
    return {p0, p1, p2, p3, p4};
}

L5 mk(const long (&c)[5]) {
    return {c[0], c[1], c[2], c[3], c[4]};
}

// ---- per-computation state (the original's globals, scoped) ------
struct Chain {
    long long zla0 = 0, zla1 = 0;        // true month
    int zeromthfg = 0;
    long adj_mth = 0;
    long long rilcha0 = 0, rilcha1 = 0;  // lunar anomaly
    L5 gzadru{}, nyidru{};               // month means
    L5 nyidag{}, gzadag{};               // true values
    L5 listc{};                          // sun-equation share
    int nyidor = 0;
    long long juldat = 0;

    void zla_dag(long y, long m) {  // KTC 15 (grub form)
        long long yr = y - kEpchYr;
        long long a = 12 * yr + m - 3;
        if (a >= 0) {
            long long b = 2 * a + kEyrA;
            zla1 = b % 65;
            zla0 = a + b / 65;
        } else {
            zla1 = 0;
            zla0 = 0;
        }
    }

    void adj_zla(long tm) {  // zlapure == 0 branch (Phugpa)
        if (zla1 == kZlaSho1 || zla1 == kZlaSho2) {
            adj_mth = -tm;
        } else if (zla1 > kZlaSho2) {
            adj_mth = tm - 1;
        } else if (zla1 == 0 || zla1 == 1) {
            if (!zeromthfg) {
                zeromthfg = 1;
                adj_mth = tm - 1;
                zla0 -= 1;
            } else {
                zeromthfg = 0;
                adj_mth = tm;
                zla0 += 1;
            }
        } else {  // 1 < zla1 < zlasho1
            adj_mth = tm;
        }
        if (adj_mth == 0) adj_mth = 12;
    }

    void rilchaf(long long x) {  // KTC 21
        long long b = x + kRilB;
        long long a = 2 * x + kRilA + b / 126;
        rilcha1 = b % 126;
        rilcha0 = a % 28;
    }

    void month_means(long long x) {
        gzadru = addg(mulg(mk(kGzaDm), x, 7, kGzaF), mk(kGzaDa), 7,
                      kGzaF);
        nyidru = addg(mulg(mk(kNyiDm), x, 27, kSunF), mk(kNyiDa), 27,
                      kSunF);
    }

    void nyi_dag(const L5& a1) {  // true sun; leaves listc + nyidor
        L5 nyiwor = subg(a1, mk(kNyiFac), 27, kSunF);
        long long test = 60 * nyiwor[0] + nyiwor[1];
        if (test < 810) {
            nyidor = 0;
        } else {
            nyidor = 1;
            nyiwor = subg(nyiwor, mk(kNyiHaf), 27, kSunF);
            test = 60 * nyiwor[0] + nyiwor[1];
        }
        long long trem = test % 135;
        long long tquo = test / 135;
        if (tquo == 0) tquo = 6;
        const long long bye = kNyiBye[tquo - 1];
        // div_g6(lista, 135, sun_f, 1) with places 2..4 populated;
        // the 6th place is (rem*1)/135 == 0, so L5 suffices
        long long v2 = (trem * 60 + nyiwor[2]) * bye;
        long long v3 = nyiwor[3] * bye;
        long long v4 = nyiwor[4] * bye;
        L5 lista{};
        long long rem = v2 % 135;
        lista[2] = v2 / 135;
        v3 += rem * 6;
        rem = v3 % 135;
        lista[3] = v3 / 135;
        v4 += rem * kSunF;
        lista[4] = v4 / 135;
        // normalize carries upward (add_gen with zero)
        lista = addg(lista, L5{}, 27, kSunF);
        L5 listb{};
        listb[1] = kNyiDom[tquo - 1];
        if (tquo == 3 || tquo == 4 || tquo == 5)
            listc = subg(listb, lista, 27, kSunF);
        else
            listc = addg(listb, lista, 27, kSunF);
        if (nyidor == 0)
            nyidag = subg(a1, listc, 27, kSunF);
        else
            nyidag = addg(a1, listc, 27, kSunF);
    }

    void gza_dag(const L5& a1, long long tt) {  // true weekday
        long long rilpo = rilcha0 + tt;
        long long trem = rilpo % 14;
        if (trem == 0) trem = 14;
        L5 list2{};
        list2[1] = kGzaDom[trem - 1];
        // full-accuracy branch (gza_short_flg == 0)
        long long tot = (30 * rilcha1 + tt) * kGzaBye[trem - 1];
        tot = tot * 360 * kGzaF / 3780;
        L5 list3{};
        if (tot < 0) {
            L5 list1{};
            list1[4] = -tot;
            list1 = addg(list1, L5{}, 7, kGzaF);
            list3 = subg(list2, list1, 7, kGzaF);
        } else {
            L5 list1{};
            list1[4] = tot;
            list1 = addg(list1, L5{}, 7, kGzaF);
            list3 = addg(list2, list1, 7, kGzaF);
        }
        L5 gzawor{};
        if ((rilpo / 14) % 2 == 0)
            gzawor = addg(a1, list3, 7, kGzaF);
        else
            gzawor = subg(a1, list3, 7, kGzaF);
        gzawor[4] = kSunF * gzawor[4] / kGzaF;
        if (nyidor == 0)
            gzadag = subg(gzawor, listc, 7, kSunF);
        else
            gzadag = addg(gzawor, listc, 7, kSunF);
        gzadag[4] = kGzaF * gzadag[4] / kSunF;
    }

    void spi_zagf(long long cur_mth, long long tt) {  // KTC 46
        long long spizag = cur_mth * 30 + tt;
        long long c = spizag + kSpzC;
        long long b = spizag + kSpzB + c / kGzaF;
        spizag = spizag - b / 64;
        c = (spizag + kSpzF) % 7;
        b = gzadag[0];
        if (c != b) {
            if (c > 4 && b < 2)
                b += 7;
            else if (b > 4 && c < 2)
                c += 7;
            spizag = spizag + b - c;
        }
        juldat = spizag + kSpzJ;
    }

    // the whole per-tithi chain: JD of the gza' dag day of tithi tt
    long long dayJd(long long cur_mth, long long tt) {
        L5 tsedru = mulg(mk(kTseDm), tt, 7, kGzaF);
        L5 nyilon = mulg(mk(kNyiLm), tt, 27, kSunF);
        L5 tsebar = addg(gzadru, tsedru, 7, kGzaF);
        L5 nyibar = addg(nyidru, nyilon, 27, kSunF);
        nyi_dag(nyibar);  // must precede gza_dag
        gza_dag(tsebar, tt);
        spi_zagf(cur_mth, tt);
        return juldat;
    }
};

}  // namespace

PhugpaYear phugpaYear(int y) {
    PhugpaYear out;
    if (y < 1027) return out;  // before the first rab byung
    Chain ch;
    long ty = y, tm = 1;
    ch.zeromthfg = 0;

    // preamble (t4.c): check we are not at an adjusted 12th month
    ch.zla_dag(ty, tm);
    const long long zla0keep = ch.zla0, zla1keep = ch.zla1;
    ch.adj_zla(tm);
    if (ch.adj_mth == 12) {
        if (!ch.zeromthfg) ++tm;
    } else if (ch.adj_mth == -1) {
        ch.zla0 = zla0keep;
        ch.zla1 = zla1keep;
        ch.zeromthfg = 0;
    }

    bool more = true;
    int guard = 0;
    while (more && ++guard < 20) {
        // month setup
        if (!ch.zeromthfg) ch.zla_dag(ty, tm);
        ch.adj_zla(tm);
        const long long cur_mth = ch.zla0;
        ch.month_means(cur_mth);
        ch.rilchaf(cur_mth);

        PhugpaMonth mo;
        bool delaymth = false;
        if (ch.adj_mth < 0) {
            mo.intercalary = true;
        } else if (ch.zla1 == kZlaSho1 + 2 || ch.zla1 == kZlaSho2 + 2) {
            delaymth = true;
        }
        mo.delayed = delaymth;
        mo.month = int(ch.adj_mth < 0 ? -ch.adj_mth : ch.adj_mth);

        for (long ltt = 1; ltt <= 30; ++ltt) {
            const long long curjd = ch.dayJd(cur_mth, ltt);
            const long long lastjd = ch.dayJd(cur_mth, ltt - 1);
            const long long nextjd = ch.dayJd(cur_mth, ltt + 1);
            PhugpaDay d;
            d.tt = int(ltt);
            if (curjd == lastjd) {
                d.omitted = true;  // chad
            } else if (curjd == lastjd + 2) {
                d.duplicated = true;  // lhag: days curjd-1 and curjd
                d.jd = curjd;
            } else {
                d.jd = curjd;
            }
            (void)nextjd;
            mo.days.push_back(d);
        }
        if (out.months.empty()) {
            for (const auto& d : mo.days)
                if (!d.omitted) {
                    out.losar_jd =
                        d.duplicated ? d.jd - 1 : d.jd;
                    break;
                }
        }
        out.months.push_back(std::move(mo));

        // advance (t4.c epilogue)
        if (!ch.zeromthfg) ++tm;
        if (tm > 12) {
            ++ty;
            tm = 1;
        }
        if (ch.adj_mth == 12) more = false;
    }
    out.valid = !out.months.empty();
    return out;
}

}  // namespace allcore
