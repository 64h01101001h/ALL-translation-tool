#pragma once
// app/ux_tokens.h - the design tokens, lifted out of main.cpp on
// 2026-08-25 as the first step against RISK_REGISTER R7.
//
// main.cpp was 39,581 lines - over half the codebase - and app/ held
// no header at all. That is the whole of R7: there was no pattern for
// putting anything anywhere else. This is the pattern, and these
// constants earn the first move because they have NO dependency on
// anything in main.cpp: colours with their measured WCAG contrast
// ratios, type sizes, and three inline helpers that read only the Qt
// palette.
//
// The reserved greens live here too. #1E6B4E (kAct) and #2E7D32 are
// binding-authority colours by house rule 1, and having them in one
// named place is worth more than the line count.

#include <cmath>

#include <QApplication>
#include <QPalette>
#include <QString>

namespace ux {
// grounds
[[maybe_unused]] constexpr const char* kPaper = "#FAF6EE";   // reading surface
[[maybe_unused]] constexpr const char* kInk = "#2B2118";     // primary text on paper
// structure & identity
[[maybe_unused]] constexpr const char* kGold = "#82672A";    // eyebrows, identity
                                            // (AA 4.97 on paper; the
                                            // chrome keeps #9A7A33
                                            // for borders/washes)
[[maybe_unused]] constexpr const char* kMuted = "#6E675D";   // secondary text
[[maybe_unused]] constexpr const char* kFaint = "#6F6F6F";   // tertiary/hints (AA 4.66)
[[maybe_unused]] constexpr const char* kSoft = "#78706A";    // caption italic (AA 4.51)
// semantic
[[maybe_unused]] constexpr const char* kAct = "#1E6B4E";     // actions / success
[[maybe_unused]] constexpr const char* kWarn = "#935800";    // advisories (AA 5.36)
[[maybe_unused]] constexpr const char* kMachine = "#B4540A"; // machine-derived marks
[[maybe_unused]] constexpr const char* kError = "#8C2F2B";   // errors / refusals
[[maybe_unused]] constexpr const char* kDoc = "#2E629E";     // documents / links-int
[[maybe_unused]] constexpr const char* kPeople = "#6E3E8E";  // people / access
// type roles (px, in rendered HTML)
[[maybe_unused]] constexpr int kFsMeta = 11;     // metadata, badges, eyebrows
[[maybe_unused]] constexpr int kFsBody = 12;     // supporting prose
[[maybe_unused]] constexpr int kFsCard = 13;     // card body
[[maybe_unused]] constexpr int kFsHead = 15;     // section heads
[[maybe_unused]] constexpr int kFsLead = 18;     // leading content (pron etc.)
[[maybe_unused]] constexpr int kFsTibetan = 22;  // Tibetan headword script
[[maybe_unused]] constexpr int kFsTibetanInline = 16;   // W6 floor: stacked glyphs in
                                       // evidence rows never render
                                       // at Latin body sizes

// ---- epistemic badge grammar (phase 2; audit C2) ----------------
// Every evidence block declares its authority class visually, in a
// small chip BEFORE its label. Wording of existing honesty labels
// stays verbatim — the badge adds a visual grammar, replaces
// nothing. Classes:
//   Binding   — HGM's own English; the point of the project
//   Evidence  — primary attested material (corpus, colophons)
//   Reference — external authorities (84000, Das, MVP, ref dicts)
//   Machine   — machine-located/derived candidates (OCR, auto-
//               aligned, suggestions); always review material
//   Ai        — model-composed, always labeled
enum class Epistemic { Binding, Evidence, Reference, Machine, Ai };
inline QString sourceBadge(Epistemic e) {
    const char* txt = nullptr;
    const char* col = nullptr;
    bool solid = false;
    switch (e) {
        case Epistemic::Binding:
            txt = "HGM"; col = kGold; solid = true; break;
        case Epistemic::Evidence:
            txt = "EVIDENCE"; col = kAct; break;
        case Epistemic::Reference:
            txt = "REFERENCE"; col = kDoc; break;
        case Epistemic::Machine:
            txt = "MACHINE"; col = kMachine; break;
        case Epistemic::Ai:
            txt = "AI"; col = kError; break;
    }
    return QString("<span style='font-family:-apple-system,Arial,"
                   "sans-serif;font-size:9px;letter-spacing:1px;"
                   "font-weight:700;border:1px solid %1;"
                   "border-radius:3px;padding:0 4px;%2'>%3</span> ")
        .arg(col)
        .arg(solid ? QString("background:%1;color:#FFF8EC;")
                         .arg(col)
                   : QString("color:%1;").arg(col))
        .arg(txt);
}
// honest elision (L-tier): evidence snippets never end mid-word
// pretending to be whole — a cut is marked with a visible ellipsis
static QString snip(const QString& t, int cap) {
    if (t.size() <= cap) return t.toHtmlEscaped();
    // This comment promised "never ends mid-word" while the code did
    // a flat left(cap). Adam's 2026-08-22 screenshot is the proof:
    // 84000 definitions cut off at «...ldan to "possessing" the g».
    // Back off to the last space, but only when that costs little,
    // so a long unbroken token still gets cut rather than vanishing.
    QString cut = t.left(cap);
    static const QRegularExpression ws(R"(\s)");
    const int sp = cut.lastIndexOf(ws);
    if (sp > cap * 3 / 4) cut = cut.left(sp);
    while (!cut.isEmpty() &&
           (cut.endsWith(' ') || cut.endsWith(',') || cut.endsWith(';')))
        cut.chop(1);
    return cut.toHtmlEscaped() + QString::fromUtf8("\u2026");
}
static QString snipStd(const std::string& t, int cap) {
    return snip(QString::fromStdString(t), cap);
}
// chrome-aware inks (W7): the chrome follows the system appearance,
// so widget labels on it measure their ink at use time. Values are
// WCAG-checked on both grounds (dark #2D2D2D · light #ECECEC).
inline bool darkChrome() {
    return QApplication::palette().color(QPalette::Window)
               .lightness() < 128;
}
inline const char* chromeMuted() {
    return darkChrome() ? "#A8A29A" : "#5E574D";
}
inline const char* chromeGold() {
    return darkChrome() ? "#C9A55C" : "#82672A";
}
// WCAG 2.x relative luminance and contrast ratio. Here rather than in
// a test so the tokens and the arithmetic that judges them live
// together - the 2026-08-25 audit found every ratio annotated above to
// be accurate, and the way to keep that true is to check it in the
// battery rather than re-audit it in a year.
inline double srgbLin(int c) {
    const double v = c / 255.0;
    return v <= 0.04045 ? v / 12.92
                        : std::pow((v + 0.055) / 1.055, 2.4);
}
inline double luminance(const char* hex) {
    const QString h = QString::fromLatin1(hex).mid(1);
    const int r = h.mid(0, 2).toInt(nullptr, 16);
    const int g = h.mid(2, 2).toInt(nullptr, 16);
    const int b = h.mid(4, 2).toInt(nullptr, 16);
    return 0.2126 * srgbLin(r) + 0.7152 * srgbLin(g) +
           0.0722 * srgbLin(b);
}
inline double contrastRatio(const char* a, const char* b) {
    const double la = luminance(a), lb = luminance(b);
    const double hi = la > lb ? la : lb, lo = la > lb ? lb : la;
    return (hi + 0.05) / (lo + 0.05);
}
}   // namespace ux
