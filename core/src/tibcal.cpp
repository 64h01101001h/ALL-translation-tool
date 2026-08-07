#include "allcore/tibcal.h"

#include <algorithm>
#include <cctype>

namespace allcore {

namespace {

const char* kElemEn[5] = {"fire", "earth", "iron", "water", "wood"};
const char* kElemWy[5] = {"me", "sa", "lcags", "chu", "shing"};
// animal cycle in cycle order starting at the rabjung base year (hare)
const char* kAnimEn[12] = {"hare", "dragon", "snake",  "horse",
                           "sheep", "monkey", "bird",   "dog",
                           "pig",   "rat",    "ox",     "tiger"};
const char* kAnimWy[12] = {"yos",   "'brug",  "sbrul",  "rta",
                           "lug",   "spre'u", "bya",    "khyi",
                           "phag",  "byi",    "glang",  "stag"};

std::string lower(std::string s) {
    for (auto& c : s) c = (char)std::tolower((unsigned char)c);
    return s;
}

}  // namespace

TibetanYear tibetanYear(int g) {
    TibetanYear y;
    y.gregorian = g;
    if (g < 1027) return y;   // rabjung 0: before the first cycle
    const int off60 = (g - 1027) % 60;
    y.rabjung = (g - 1027) / 60 + 1;
    y.year_in_cycle = off60 + 1;
    y.animal_en = kAnimEn[off60 % 12];
    y.animal_wylie = kAnimWy[off60 % 12];
    // element pairs advance from fire-male-tiger 1026: offset from 1026
    const int e = ((g - 1026) % 10) / 2;
    y.element_en = kElemEn[e];
    y.element_wylie = kElemWy[e];
    y.female = ((g - 1026) % 2) == 1;
    return y;
}

std::vector<int> yearsFor(const std::string& element,
                          const std::string& animal, int rabjung,
                          int from, int to) {
    std::vector<int> out;
    const std::string el = lower(element), an = lower(animal);
    int ei = -1, ai = -1;
    for (int i = 0; i < 5; ++i)
        if (el == kElemEn[i] || el == kElemWy[i]) ei = i;
    for (int i = 0; i < 12; ++i)
        if (an == kAnimEn[i] || an == kAnimWy[i]) ai = i;
    if (ei < 0 || ai < 0) return out;
    int lo = std::max(from, 1027), hi = std::min(to, 9999);
    if (rabjung > 0) {
        lo = std::max(lo, 1027 + 60 * (rabjung - 1));
        hi = std::min(hi, 1027 + 60 * rabjung - 1);
    }
    for (int g = lo; g <= hi; ++g) {
        if ((g - 1027) % 12 == ai && ((g - 1026) % 10) / 2 == ei)
            out.push_back(g);
    }
    return out;
}

}  // namespace allcore
