// thl_battery — acceptance battery for allcore::thlPhonetics.
// Every line below is FROM THE PUBLISHED STANDARD itself (THL
// Simplified Phonetics, Germano & Tournadre 2003; captured
// complete in docs/research/THL/language_tools.md §2):
//   - §2.1 general-principle examples
//   - §2.2 the sixteen Special Rules' own examples
//   - §2.8 the full §11 example list (33)
//   - §2.9 the complete §12 List of Exceptions (19)
// Comparison is case-insensitive (capitalization is a proper-noun
// styling rule, not a sound rule).
//
// DOCUMENTED POLICY: the source prints thun>thün, contradicting
// its own rules 2+3 and its other th- examples; we assert the
// rule-derived tün and record the deviation here.
#include <cstdio>
#include <string>
#include <vector>

#include "allcore/engines.h"

static std::string low(std::string s) {
    for (auto& c : s)
        if (c >= 'A' && c <= 'Z') c += 32;
    return s;
}

int main() {
    struct Case {
        const char* wylie;
        const char* expect;
    };
    static const std::vector<Case> cases = {
        // ---- §2.1 general principle ----
        {"dag pa", "dakpa"},
        {"ring po", "ringpo"},
        {"rin chen", "rinchen"},
        {"lab", "lap"},
        {"dum bu", "dumbu"},
        {"dmar po", "marpo"},
        {"ril bu", "rilbu"},
        {"sa skya pa", "sakyapa"},
        {"blo bzang", "lozang"},
        {"rnying ma pa", "nyingmapa"},
        {"dge lugs pa", "gelukpa"},
        {"gzhis ka rtse", "zhikatsé"},
        {"mar me", "marmé"},
        {"dge bshes", "geshé"},
        // ---- rule 1 ----
        {"bcu", "chu"},
        {"gcig pa", "chikpa"},
        {"nag chu", "nakchu"},
        // ---- rule 2 ----
        {"'phag pa", "pakpa"},
        {"gser thang", "sertang"},
        {"khang tshan", "khangtsen"},
        // ---- rule 3 ----
        {"bka' brgyud", "kagyü"},
        {"bsod nams", "sönam"},
        {"yul", "yül"},
        {"dus tshod", "dütsö"},
        {"bon po", "bönpo"},
        // ---- rule 4 ----
        {"sde dge", "degé"},
        {"brgyad", "gyé"},
        {"dge rgan", "gegen"},
        {"ral pa can", "relpachen"},
        {"tshe ring", "tsering"},
        {"byes", "jé"},
        {"bstan 'dzin", "tendzin"},
        {"'jam dpal dbyangs", "jampelyang"},
        // ---- rule 5 ----
        {"dge legs", "gelek"},
        {"kha btags", "khatak"},
        {"sngags pa", "ngakpa"},
        {"byang chub", "jangchup"},
        {"thub bstan", "tupten"},
        {"thabs", "tap"},
        // ---- rule 6 ----
        {"lha sa ba", "lhasawa"},
        {"jo bo", "jowo"},
        {"dpa' bo", "pawo"},
        {"'gro ba'i", "drowé"},
        // ---- rule 7 ----
        {"spyan ras gzigs", "chenrezik"},
        {"phyag", "chak"},
        {"sbyin bdag", "jindak"},
        // ---- rule 8 ----
        {"smyong", "nyong"},
        {"dmyal ba", "nyelwa"},
        // ---- rule 9 ----
        {"sgrol ma", "drölma"},
        {"grub thob", "druptop"},
        {"sprul sku", "trülku"},
        {"'bras spungs", "drepung"},
        {"'phrin las", "trinlé"},
        {"srung ma", "sungma"},
        {"srog rlung", "soklung"},
        {"rdzun smra ba", "dzünmawa"},
        // ---- rule 10 ----
        {"klad pa", "lepa"},
        {"glog", "lok"},
        {"zla ba", "dawa"},
        // ---- rule 11 ----
        {"lha sa", "lhasa"},
        {"lho phyogs", "lhochok"},
        {"lhun grub", "lhündrup"},
        // ---- rule 12 ----
        {"dbang", "wang"},
        {"dbyar kha", "yarkha"},
        {"dbral", "rel"},
        // ---- rule 13 ----
        {"bka' 'gyur", "kangyur"},
        {"dge 'dun", "gendün"},
        {"ngos 'dzin", "ngöndzin"},
        {"rig 'dzin", "rindzin"},
        {"mkha' 'gro", "khandro"},
        {"dkyil 'khor", "kyinkhor"},
        {"chos 'phel", "chömpel"},
        {"dpal 'bar", "pembar"},
        {"sku 'bum", "kumbum"},
        {"dpal 'byor", "penjor"},
        {"rgyu 'bras", "gyundré"},
        // ---- rules 14–15 ----
        {"le'u", "leu"},
        {"khyi'u", "khyiu"},
        {"pa'ang", "pang"},
        {"gri'i", "dri"},
        // ---- rule 16 ----
        {"rgyal bu'i", "gyelbü"},
        {"rin po che'i", "rinpoché"},
        {"bdag po'i", "dakpö"},
        {"le'u'i", "leü"},
        // ---- §2.6 particles / assembled words (per word unit —
        // word SEGMENTATION is the caller's knowledge; the engine
        // never guesses boundaries) ----
        {"chos kyi", "chökyi"},
        {"rnam grangs", "namdrang"},
        {"gtan la", "tenla"},
        {"phab pa", "pappa"},
        {"'gyur med", "gyurmé"},
        {"ma byas pa", "majepa"},
        // ---- §11 example list (whole, minus repeats above) ----
        {"g.yon", "yön"},
        {"rgyal", "gyel"},
        {"bkra shis", "trashi"},
        {"khros ma", "tröma"},
        {"sprul", "trül"},
        {"srid pa", "sipa"},
        {"pad ma", "pema"},
        {"pan chen", "penchen"},
        {"thun", "tün"},   // policy: rules 2+3 over the source typo
        {"dus gsum", "düsum"},
        {"sbed", "bé"},
        {"ces", "ché"},
        {"btsan dbang", "tsenwang"},
        {"tshong khang", "tsongkhang"},
        {"rdzong", "dzong"},
        {"stabs", "tap"},
        {"thug pa", "tukpa"},
        {"debs", "dep"},
        {"sib sib", "sipsip"},
        {"lobs pa", "loppa"},
        {"grub", "drup"},
        {"kla col", "lachöl"},
        {"spyan snga ba", "chenngawa"},
        {"byang chub", "jangchup"},
        {"sems dpa'i", "sempé"},
        {"bon po'i", "bönpö"},
        {"nga'i", "ngé"},
        {"deb", "dep"},
        {"mri tam ga", "mitamga"},
        // ---- §12 the complete List of Exceptions ----
        {"skyabs 'gro", "kyamdro"},
        {"dga' ldan", "ganden"},
        {"rgyal rtse", "gyantsé"},
        {"chab mdo", "chamdo"},
        {"rta mgrin", "tamdrin"},
        {"rten 'brel", "temdrel"},
        {"rdo rje", "dorjé"},
        {"dpal ldan", "penden"},
        {"bla brang", "labrang"},
        {"'bras ljongs", "drenjong"},
        {"'bri ru", "biru"},
        {"sbra nag zhol", "banakzhöl"},
        {"me mda'", "menda"},
        {"lam 'bras", "lamdré"},
        {"lha rje", "lharjé"},
        {"lha bris pa", "lhapripa"},
        {"a mdo", "amdo"},
        {"u rgyan", "urgyen"},
        {"o rgyan", "orgyen"},
        // ---- §7 Sanskrit ----
        {"kaH thog", "katok"},
    };

    int fails = 0;
    for (const auto& c : cases) {
        const std::string got = allcore::thlPhonetics(c.wylie);
        if (low(got) != low(c.expect)) {
            std::printf("FAIL  %-28s expect %-16s got %s\n",
                        c.wylie, c.expect, got.c_str());
            ++fails;
        }
    }
    std::printf("thl_battery: %zu cases, %d failure(s)\n",
                cases.size(), fails);
    return fails ? 1 : 0;
}
