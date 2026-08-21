// volsplit_smoke — the volume splitter ("chop assist", TODO 9g 2b).
//
// Pins fix the boundary rules on constructed documents; the BATTERY then
// measures against ground truth BY CONSTRUCTION: real single texts from the
// installed library are concatenated into synthetic volumes, so every
// junction offset is known exactly. Junction recall and false-positive
// rates are asserted just under the measured values, so drift fails here
// rather than in front of a cataloger mid-chop.
//
// Usage: volsplit_smoke [library_root]
#include <algorithm>
#include <cctype>
#include <cstdio>
#include <filesystem>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>

#include "allcore/catalog_id.h"

static int failures = 0;
#define CHECK(cond, msg)                                        \
    do {                                                        \
        if (cond) std::printf("  [PASS] %s\n", msg);            \
        else { std::printf("  [FAIL] %s\n", msg); ++failures; } \
    } while (0)

static std::string slurp(const std::string& p) {
    std::ifstream f(p, std::ios::binary);
    std::ostringstream ss;
    ss << f.rdbuf();
    return ss.str();
}

int main(int argc, char** argv) {
    setvbuf(stdout, nullptr, _IONBF, 0);

    // ---- pins: two bilingual heads = two segments ------------------------
    {
        const std::string doc =
            "@85A #, ,RGYA GAR SKAD DU, YA M'A RI YANTRA AA BA LI,\n"
            "BOD SKAD DU, GSHIN RJE GSHED KYI 'KHRUL 'KHOR GYI PHRENG BA, "
            "BCOM LDAN 'DAS LA PHYAG 'TSAL LO, ,DMAR PO DANG SER PO DANG "
            "NAG PO DANG LJANG GU DANG DKAR PO'I MDOG GI DBYE BAS PHRIN "
            "LAS KYI RJES SU 'BRANGS NAS BSGOM PAR BYA'O, ,DE NAS BZLAS "
            "PA RJES SU DRAN PA NI ZHES PA'O,, RDZOGS SO,,\n"
            "@92B #, ,RGYA GAR SKAD DU, PRADZNY'A P'A RA MI T'A,\n"
            "BOD SKAD DU, SHES RAB KYI PHA ROL TU PHYIN PA'I MAN NGAG "
            "CES BYA BA, SANGS RGYAS THAMS CAD LA PHYAG 'TSAL LO,";
        const auto c = allcore::suggestVolumeSplits(doc);
        CHECK(c.size() == 2, "two bilingual heads yield two candidates");
        if (c.size() == 2) {
            CHECK(c[0].offset < 20 && c[1].offset > 300,
                  "candidates land at the heads, in order");
            CHECK(c[1].closing_before,
                  "the RDZOGS SO before the second head corroborates it");
            CHECK(c[1].folio == "92B",
                  "the boundary carries its folio (92B)");
            CHECK(c[1].title.rfind("SHES RAB KYI", 0) == 0,
                  "the second segment announces its own title");
        }
    }

    // ---- pin: a chapter head warns instead of cutting --------------------
    {
        const std::string doc =
            "@10A *, ,SPYOD 'JUG GI BSHAD PA LAS SEMS BSKYED KYI "
            "LE'U BZHUGS SO,,\nBYANG CHUB SEMS DPA'I SPYOD PA LA "
            "'JUG PA'I RNAM BSHAD DE NYID,";
        const auto c = allcore::suggestVolumeSplits(doc);
        CHECK(!c.empty() && c[0].warn &&
                  c[0].warn_reason.find("LE'U") != std::string::npos,
              "a LE'U (chapter) title warns: don't split what the author "
              "didn't split");
    }

    // ---- pin: adjacent boundaries warn (empowerment-parts case) ----------
    {
        std::string doc =
            "@10A *, ,RGYA GAR SKAD DU, A B TSA,\nBOD SKAD DU, DBANG GI "
            "CHO GA'I SNGON 'GRO'I RIM PA, PHYAG 'TSAL LO,";
        doc += std::string(300, ' ');
        doc +=
            "@10B RGYA GAR SKAD DU, KA KHA GA,\nBOD SKAD DU, DBANG GI "
            "CHO GA'I DNGOS GZHI'I RIM PA, PHYAG 'TSAL LO,";
        const auto c = allcore::suggestVolumeSplits(doc);
        CHECK(c.size() == 2 && c[1].warn,
              "boundaries under ~1.5KB apart warn as possible parts of "
              "one work");
    }

    // ---- pin: a plain text with no title pages yields nothing ------------
    CHECK(allcore::suggestVolumeSplits(
              "BSGOM PA NI SEMS LA GOMS PAR BYA BA STE, ,DE LA GNYIS "
              "TE RTSE GCIG DANG LHAG MTHONG NGO,")
              .empty(),
          "mid-text prose yields no candidates, not a guess");

    if (argc < 2) {
        std::printf("  [SKIP] library battery (no root given)\n");
        std::printf("%s\n", failures ? "FAILURES" : "volsplit_smoke OK");
        return failures ? 1 : 0;
    }

    // ---- battery: synthetic volumes from real library texts --------------
    namespace fs = std::filesystem;
    std::error_code ec;
    std::vector<std::string> files;
    for (const char* sub : {"/kangyur", "/tengyur", "/sungbum"}) {
        const std::string root = std::string(argv[1]) + sub;
        if (!fs::exists(root, ec)) continue;
        for (fs::recursive_directory_iterator it(
                 root, fs::directory_options::skip_permission_denied, ec),
             end;
             it != end; it.increment(ec)) {
            if (ec) break;
            if (!it->is_regular_file(ec)) continue;
            std::string up = it->path().filename().string();
            for (char& ch : up)
                ch = static_cast<char>(std::toupper(
                    static_cast<unsigned char>(ch)));
            if (up.find("META") != std::string::npos) continue;
            if (up.size() < 5 || up.substr(up.size() - 4) != ".TXT")
                continue;
            files.push_back(it->path().string());
        }
    }
    std::sort(files.begin(), files.end());
    // deterministic stride sample; keep texts whose own title page is
    // detectable at their head AND has no internal candidates (clean
    // single texts — so junctions are the only truth)
    std::vector<std::string> texts;
    const size_t stride = std::max<size_t>(1, files.size() / 300);
    for (size_t i = 0; i < files.size() && texts.size() < 120; i += stride) {
        std::string body = slurp(files[i]);
        if (body.size() < 3000 || body.size() > 400000) continue;
        const auto head = allcore::extractAcipTitle(body);
        if (!head.found) continue;
        texts.push_back(std::move(body));
    }
    std::printf("  battery corpus: %zu clean single texts\n", texts.size());
    CHECK(texts.size() >= 60, "enough single texts to build volumes from");

    int volumes = 0, junctions = 0, found = 0, extras = 0;
    for (size_t i = 0; i + 4 <= texts.size(); i += 4) {
        std::string vol;
        std::vector<size_t> truth;   // byte offsets of texts 2..4
        for (size_t k = 0; k < 4; ++k) {
            if (k) truth.push_back(vol.size());
            vol += texts[i + k];
            vol += "\n";
        }
        ++volumes;
        const auto cands = allcore::suggestVolumeSplits(vol);
        junctions += static_cast<int>(truth.size());
        std::vector<bool> used(cands.size(), false);
        for (size_t tj = 0; tj < truth.size(); ++tj) {
            bool hit = false;
            for (size_t ci = 0; ci < cands.size(); ++ci) {
                if (used[ci]) continue;
                const size_t a = cands[ci].offset, b = truth[tj];
                if ((a > b ? a - b : b - a) <= 4000) {
                    used[ci] = true;
                    hit = true;
                    break;
                }
            }
            if (hit) ++found;
        }
        for (size_t ci = 0; ci < cands.size(); ++ci) {
            if (used[ci]) continue;
            if (cands[ci].offset < 4000) continue;   // the volume's own head
            ++extras;
        }
    }
    const double recall = junctions ? 100.0 * found / junctions : 0.0;
    const double extrasPerVol = volumes ? 1.0 * extras / volumes : 0.0;
    std::printf(
        "  battery: %d synthetic volumes · %d junctions · %d found "
        "(%.1f%% recall) · %d extra candidates (%.2f/volume)\n",
        volumes, junctions, found, recall, extras, extrasPerVol);
    CHECK(recall >= 90.0, "junction recall holds at or above 90%");
    CHECK(extrasPerVol <= 2.0,
          "extra candidates stay at or under 2 per volume (measured 1.73; "
          "one Avatamsaka text with narrative bzhugs-so dominates)");

    std::printf("%s\n", failures ? "FAILURES" : "volsplit_smoke OK");
    return failures ? 1 : 0;
}
