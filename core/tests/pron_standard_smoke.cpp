// pron_standard_smoke — battery H (queued in TODO since the ACIP
// pronunciation standard was banked): the standard's own explicit
// example pairs (docs/standards/ACIP_PRONUNCIATION_STANDARD.md,
// from ACIP Catalog Release IV pp. 173–178), each pre-validated
// against the canonical Python engine before pinning here.
// Comparison is whitespace/hyphen-normalized, as the standard's
// own 2026-08-06 measurement was. The Sanskrit-letter and
// diphthong exceptions documented in the standard are NOT pinned
// (they are reference findings, not engine expectations).
#include <cstdio>
#include <string>

#include "allcore/engines.h"

static int failures = 0;

static std::string norm(std::string s) {
    std::string o;
    for (char c : s)
        if (c != ' ' && c != '-') o += (char)std::tolower((unsigned char)c);
    return o;
}

int main() {
    struct Pair { const char* acip; const char* expect; };
    static const Pair pairs[] = {
        {"KA BA", "kawa"},          {"TSONG KHA PA", "tsongkapa"},
        {"RE 'GA'", "renga"},       {"DGE LUGS PA", "gelukpa"},
        {"DKA' NGAL", "kangel"},    {"CHOS", "chu"},
        {"MTHA' MJUG", "tanjuk"},   {"GTAN", "ten"},
        {"THAR PA", "tarpa"},       {"DGA' LDAN", "ganden"},
        {"GNAS", "ne"},             {"LAN", "len"},
        {"DPA' BO", "pawo"},        {"PHAN THOGS", "pentok"},
        {"BAR DO", "bardo"},        {"CHU 'BEBS", "chumbep"},
        {"RAB TU TSA BA", "raptutsawa"}, {"SMAN PA", "menpa"},
        {"RIM PA", "rimpa"},        {"GTZUG PHUD", "tsukpu"},
        {"RDZAS", "dze"},           {"RANG DBANG", "rangwang"},
        {"LAN PA", "lenpa"},        {"YID", "yi"},
        {"LUS", "lu"},              {"SLEB PA", "leppa"},
        {"DGONGS PA", "gongpa"},    {"BSOD NAMS", "sunam"},
        {"G-YAR BA", "yarwa"},      {"BSHAD PA", "shepa"},
        {"SRID PA", "sipa"},        {"LHA", "hla"},
    };
    int n = 0;
    for (const auto& p : pairs) {
        ++n;
        const std::string got =
            allcore::pronounce(allcore::acipToEwts(p.acip));
        if (norm(got) != norm(p.expect)) {
            std::printf("FAIL  %-16s expect %-12s got %s\n",
                        p.acip, p.expect, got.c_str());
            ++failures;
        }
    }
    std::printf("pron_standard: %d standard example pairs, %d "
                "failure(s)\n", n, failures);
    return failures;
}
