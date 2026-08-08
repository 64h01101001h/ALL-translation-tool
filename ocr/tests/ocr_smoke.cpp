// ocr_smoke — OCR stage 2 increment A: the allocr line-detection port
// against the CANONICAL BDRC pipeline (tools/build_ocr_reference.py runs
// the actual tibetan-ocr-app functions + the same ONNX model on the same
// banked fixtures). Masks are compared pixel-wise with exact agreement
// reported; the assertion floor allows only the sub-level rounding
// differences of cv2's fixed-point resize/blur vs our float versions.
// Fixtures: two public-domain Pramanavarttika folios (BDRC scans) —
// 99a dense text (the real case), 94a sparse microfilm title page (the
// hardest case, per the stage-2 spike).
#include <cstdio>
#include <fstream>
#include <string>
#include <vector>

#include "allocr/linedet.h"
#ifdef ALLOCR_HAVE_OPENCV
#include "allocr/linebuild.h"
#endif

static int failures = 0;
#define CHECK(cond, msg)                                        \
    do {                                                        \
        if (cond) std::printf("  [PASS] %s\n", msg);            \
        else { std::printf("  [FAIL] %s\n", msg); ++failures; } \
    } while (0)

struct Pnm {
    int w = 0, h = 0, channels = 0;
    std::vector<uint8_t> data;
};

static Pnm readPnm(const std::string& path) {
    std::ifstream f(path, std::ios::binary);
    Pnm p;
    std::string magic;
    f >> magic;
    if (magic == "P6") p.channels = 3;
    else if (magic == "P5") p.channels = 1;
    else return p;
    int maxv = 0;
    f >> p.w >> p.h >> maxv;
    f.get();  // single whitespace after header
    p.data.resize(static_cast<size_t>(p.w) * p.h * p.channels);
    f.read(reinterpret_cast<char*>(p.data.data()),
           static_cast<std::streamsize>(p.data.size()));
    if (!f) p.w = 0;
    return p;
}

int main(int argc, char** argv) {
    if (argc < 3) {
        std::fprintf(stderr, "usage: ocr_smoke <PhotiLines.onnx> <ref-dir>\n");
        return 2;
    }
    allocr::LineDetector det(argv[1]);
    std::printf("  model loaded: %s\n", argv[1]);

    struct Case {
        const char* name;
        double floorPct;   // assertion floor for mask agreement
        int minBands;      // at least this many detected lines
    };
    const Case cases[] = {
        {"pv_folio_99a", 98.0, 3},  // dense text folio: several lines
        {"pv_folio_94a", 98.0, 1},  // sparse title folio: the title line
    };
    for (const Case& c : cases) {
        const std::string base = std::string(argv[2]) + "/" + c.name;
        Pnm in = readPnm(base + ".ppm");
        Pnm want = readPnm(base + ".mask.pgm");
        if (in.w == 0 || want.w == 0) {
            std::printf("  [FAIL] %s: reference files missing — run "
                        "tools/build_ocr_reference.py\n", c.name);
            ++failures;
            continue;
        }
        auto mask = det.detect(in.data.data(), in.w, in.h);
        CHECK(mask.w == want.w && mask.h == want.h,
              (std::string(c.name) + ": mask dimensions match").c_str());
        size_t same = 0, total = mask.data.size();
        for (size_t i = 0; i < total; ++i)
            same += (mask.data[i] > 0) == (want.data[i] > 0);
        const double pct = 100.0 * static_cast<double>(same) /
                           static_cast<double>(total);
        std::printf("  %s: mask agreement %.3f%% (%zu/%zu px)\n", c.name, pct,
                    same, total);
        CHECK(pct >= c.floorPct,
              (std::string(c.name) + ": mask matches the canonical pipeline")
                  .c_str());
        auto bands = allocr::LineDetector::rowBands(mask);
        std::printf("  %s: %zu line band(s)\n", c.name, bands.size());
        CHECK(static_cast<int>(bands.size()) >= c.minBands,
              (std::string(c.name) + ": line bands detected").c_str());
        // the canonical mask must yield the same band count
        allocr::Mask wm;
        wm.w = want.w;
        wm.h = want.h;
        wm.data = want.data;
        auto wantBands = allocr::LineDetector::rowBands(wm);
        CHECK(bands.size() == wantBands.size(),
              (std::string(c.name) + ": band count equals the canonical mask")
                  .c_str());

#ifdef ALLOCR_HAVE_OPENCV
        // ---- increment B: line building on the SAME canonical mask ------
        // (isolated from increment A's tolerances; geometry is the same
        // OpenCV native code the canonical cv2 ran)
        {
            std::ifstream lf(base + ".lines.tsv");
            if (!lf) {
                std::printf("  [FAIL] %s: lines.tsv missing — rerun the "
                            "oracle tool\n", c.name);
                ++failures;
            } else {
                double wantAngle = 0, wantThr = 0;
                struct WantLine { int x, y, w, h, cx, cy; };
                std::vector<WantLine> wantLines;
                std::string key;
                while (lf >> key) {
                    if (key == "angle") lf >> wantAngle;
                    else if (key == "threshold") lf >> wantThr;
                    else if (key == "line") {
                        WantLine wl{};
                        lf >> wl.x >> wl.y >> wl.w >> wl.h >> wl.cx >> wl.cy;
                        wantLines.push_back(wl);
                    }
                }
                auto pl = allocr::buildLines(in.data.data(), in.w, in.h, wm);
                std::printf("  %s: angle %.3f (want %.3f), %zu line(s) "
                            "(want %zu)\n", c.name, pl.angle, wantAngle,
                            pl.lines.size(), wantLines.size());
                CHECK(std::abs(pl.angle - wantAngle) < 0.05,
                      (std::string(c.name) + ": deskew angle matches").c_str());
                CHECK(pl.lines.size() == wantLines.size(),
                      (std::string(c.name) + ": line count matches").c_str());
                bool bboxOk = pl.lines.size() == wantLines.size();
                for (size_t i = 0; bboxOk && i < pl.lines.size(); ++i) {
                    const auto& g = pl.lines[i];
                    const auto& w2 = wantLines[i];
                    bboxOk = std::abs(g.x - w2.x) <= 1 &&
                             std::abs(g.y - w2.y) <= 1 &&
                             std::abs(g.w - w2.w) <= 2 &&
                             std::abs(g.h - w2.h) <= 2;
                    if (!bboxOk)
                        std::printf("    line %zu bbox: got %d,%d %dx%d want "
                                    "%d,%d %dx%d\n", i, g.x, g.y, g.w, g.h,
                                    w2.x, w2.y, w2.w, w2.h);
                }
                CHECK(bboxOk, (std::string(c.name) +
                               ": line bboxes match the canonical").c_str());
                // line images: dimensions + pixel agreement per line
                bool imgOk = true;
                for (size_t i = 0; i < pl.images.size(); ++i) {
                    Pnm li = readPnm(base + ".line" + std::to_string(i) +
                                     ".ppm");
                    if (li.w == 0) { imgOk = false; break; }
                    if (li.w != pl.images[i].w || li.h != pl.images[i].h) {
                        std::printf("    line %zu image: got %dx%d want "
                                    "%dx%d\n", i, pl.images[i].w,
                                    pl.images[i].h, li.w, li.h);
                        imgOk = false;
                        break;
                    }
                    size_t sameB = 0, tot = li.data.size();
                    for (size_t b = 0; b < tot; ++b)
                        sameB += li.data[b] == pl.images[i].rgb[b];
                    double pctB = 100.0 * sameB / tot;
                    if (pctB < 99.9) {
                        std::printf("    line %zu pixels: %.3f%%\n", i, pctB);
                        imgOk = false;
                    }
                }
                CHECK(imgOk, (std::string(c.name) +
                              ": extracted line images match").c_str());
            }
        }
#endif
    }

    std::printf("ocr_smoke: %s (%d failures)\n",
                failures ? "FAIL" : "ALL PASS", failures);
    return failures ? 1 : 0;
}
