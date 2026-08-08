// font_smoke — the Tibetan font battery (fonts survey, data/fonts/
// FONTS.md). Coverage corpus: every Tibetan-block codepoint in Adam's
// own 1,973-syllable Hypercontext table plus his fail-to-render
// syllables (converted through the proven ACIP chain). The two BUNDLED
// faces must always pass in full; each additional vetted face is
// asserted only when installed (reported either way). A face vanishing
// or regressing fails loudly.
#include <QDir>
#include <QFontDatabase>
#include <QGuiApplication>
#include <QFileInfo>
#include <QRawFont>

#include <cstdio>
#include <fstream>
#include <set>
#include <sstream>
#include <string>

#include "allcore/engines.h"

static int failures = 0;
#define CHECK(cond, msg)                                        \
    do {                                                        \
        if (cond) std::printf("  [PASS] %s\n", msg);            \
        else { std::printf("  [FAIL] %s\n", msg); ++failures; } \
    } while (0)

static void addCps(const std::string& utf8, std::set<uint>& cps) {
    for (size_t i = 0; i < utf8.size();) {
        unsigned char c = utf8[i];
        uint cp = 0;
        int n = 1;
        if (c < 0x80) cp = c;
        else if ((c & 0xE0) == 0xC0) { cp = c & 0x1F; n = 2; }
        else if ((c & 0xF0) == 0xE0) { cp = c & 0x0F; n = 3; }
        else { cp = c & 0x07; n = 4; }
        for (int k = 1; k < n && i + k < utf8.size(); ++k)
            cp = (cp << 6) | (utf8[i + k] & 0x3F);
        if (cp >= 0x0F00 && cp <= 0x0FFF) cps.insert(cp);
        i += n;
    }
}

int main(int argc, char** argv) {
    qputenv("QT_QPA_PLATFORM", "offscreen");
    QGuiApplication app(argc, argv);
    if (argc < 4) {
        std::fprintf(stderr,
                     "usage: font_smoke <fonts-dir> <syllable-table> "
                     "<fail-list>\n");
        return 2;
    }
    // addApplicationFont needs an absolute path in the offscreen platform
    const QString fontsDir =
        QFileInfo(QString::fromUtf8(argv[1])).absoluteFilePath();
    CHECK(QFontDatabase::addApplicationFont(
              fontsDir + "/NotoSerifTibetan.ttf") >= 0,
          "Noto Serif Tibetan registers");
    CHECK(QFontDatabase::addApplicationFont(
              fontsDir + "/BabelStoneTibetanSlim.ttf") >= 0,
          "BabelStone Tibetan Slim registers");
    CHECK(QFontDatabase::addApplicationFont(
              fontsDir + "/Jomolhari-Regular.ttf") >= 0,
          "Jomolhari registers");

    std::set<uint> cps;
    {
        std::ifstream t(argv[2]);
        CHECK(bool(t), "syllable table opens");
        std::string line;
        while (std::getline(t, line)) addCps(line, cps);
    }
    {
        std::ifstream fl(argv[3]);
        CHECK(bool(fl), "fail-to-render list opens");
        std::string line;
        while (std::getline(fl, line)) {
            // ACIP syllables (strip the <t> marker line prefix)
            std::string acip;
            for (char c : line)
                if (std::isalpha(static_cast<unsigned char>(c)) || c == '\'')
                    acip += c;
                else if (c == '<') break;
            if (acip.empty()) continue;
            auto [uni, ok] = allcore::wylieToUnicode(allcore::acipToEwts(acip));
            if (ok) addCps(uni, cps);
        }
    }
    std::printf("  coverage corpus: %zu Tibetan-block codepoints\n",
                cps.size());
    CHECK(cps.size() >= 60, "corpus covers the block's working set");

    struct Face {
        const char* name;
        bool bundled;  // must always be present and complete
    };
    // style faces the app registers per-session from this machine's font
    // files (never bundled) — same registration here so their coverage is
    // regression-tested; absent files are simply skipped
    for (const char* styleFile :
         {"tibusrfa2.ttf", "TibetanChosgyalUni_2014-12-23-shipped.ttf",
          "TibetanCalligraphicUnicode.1.0.ttf",
          "TibetanSambhotaYigchung.ttf", "TibetanUnicode.ttf"}) {
        const QString p = QDir::homePath() + "/Library/Fonts/" + styleFile;
        if (QFileInfo::exists(p)) QFontDatabase::addApplicationFont(p);
    }

    const Face vetted[] = {
        {"Noto Serif Tibetan", true},
        {"BabelStone Tibetan Slim", true},
        {"Jomolhari", true},
        {"SambhotaDege", false},
        {"TibetanMachineUnicode", false},
        {"TibetanClassicUnicode", false},
        {"Kailasa", false},
        {"Kokonor", false},
        {"Microsoft Himalaya", false},
        {"TibetanChogyalUnicode", false},
        {"TibetanYigchung", false},
        {"TibetanCalligraphicUnicode", false},
        {"TCRC Youtso Unicode", false},
        {"Tib-US Unicode", false},
    };
    for (const Face& face : vetted) {
        if (!QFontDatabase::hasFamily(face.name)) {
            if (face.bundled) {
                std::printf("  [FAIL] bundled face missing: %s\n", face.name);
                ++failures;
            } else {
                std::printf("  (not installed: %s — skipped)\n", face.name);
            }
            continue;
        }
        QRawFont rf = QRawFont::fromFont(QFont(face.name));
        int have = 0;
        for (uint cp : cps)
            if (rf.supportsCharacter(cp)) ++have;
        const bool full = have == static_cast<int>(cps.size());
        std::printf("  %-28s %d/%zu\n", face.name, have, cps.size());
        CHECK(full, (std::string(face.name) + ": full coverage").c_str());
    }

    std::printf("font_smoke: %s (%d failures)\n",
                failures ? "FAIL" : "ALL PASS", failures);
    return failures ? 1 : 0;
}
