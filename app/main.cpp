// ALL Translation Tool — Qt 6 shell.
// Pane 1 "Lookup": headword search over the spine (walking skeleton kept).
// Pane 2 "Analysis": passage → engine pre-pass → Claude API (streamed) →
//                    rendered report → machine QC panel.
#include <QApplication>
#include <QCollator>
#include <QElapsedTimer>
#include <QHBoxLayout>
#include <QInputDialog>
#include <QTextDocumentFragment>
#include <QListView>
#include <QScrollArea>
#include <QShortcut>
#include <QStringListModel>
#include <QTextStream>
#include <QLabel>
#include <QLineEdit>
#include <QNetworkAccessManager>
#include <QDialog>
#include <QDialogButtonBox>
#include <QFontDatabase>
#include <QFormLayout>
#include <QGroupBox>
#include <QMessageBox>
#include <QListWidget>
#include <QStackedWidget>
#include <QTableWidget>
#include <QHeaderView>
#include <QPainter>
#include <QPen>
#include <QRegularExpression>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QPlainTextEdit>
#include <QPushButton>
#include <QScrollBar>
#include <QSplitter>
#include <QTabWidget>
#include <QTextBrowser>
#include <QVBoxLayout>
#include <QWidget>

#include <QCheckBox>
#include <QComboBox>
#include <QRadioButton>
#include <QSettings>
#include <QTextEdit>

#include <ctime>
#include <optional>
#include <random>

#include <set>
#include <string>

#include <QCoreApplication>
#include <QDir>
#include <QFile>
#include <QFileDialog>
#include <QFileInfo>
#include <QFileSystemModel>
#include <QTreeView>
#include <QProcess>
#ifdef ALL_HAVE_QTPDF
#include <QPdfDocument>
#include <QPdfView>
#include <QPdfPageNavigator>
#endif
#include <QSpinBox>
#include <QTime>
#include <QStringDecoder>
#include <QTextBlock>
#include <QDirIterator>
#include <QDesktopServices>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QUrl>
#include <QProgressDialog>

#include <functional>
#include <QDateTime>
#include <QStandardPaths>
#include <QTimer>
#include <QStyleHints>
#include <QMainWindow>
#include <QMenuBar>
#include <QMenu>

#include "allcore/abbr.h"
#include "allcore/analysis.h"
#ifdef ALL_HAVE_OCR
#include "allocr/linebuild.h"
#include "allocr/recognize.h"
#endif
#include "allcore/contractions.h"
#include "allcore/botok.h"
#include "allcore/drills.h"
#include "allcore/engines.h"
#include "allcore/gofer.h"
#include "allcore/lattice.h"
#include "allcore/mvp.h"
#include "allcore/poslex.h"
#include "allcore/libindex.h"
#include "allcore/particles.h"
#include "allcore/progress.h"
#include "allcore/qc.h"
#include "allcore/reader.h"
#include "allcore/refdict.h"
#include "allcore/sanskrit.h"
#include "allcore/spellcheck.h"
#include "allcore/quotation.h"
#include "allcore/spine.h"
#include "allcore/affixnorm.h"
#include "allcore/lexicon.h"
#include "allcore/verbstems.h"
#include "allcore/outline.h"
#include "allcore/hypfile.h"
#include "allcore/whitney.h"
#include "allcore/colloquial.h"
#include "allcore/proposals.h"

// the in-house proposal channel (defined before main; forward-declared
// here so the panes above can offer "propose from card")
static void loadIdentity();
static void fileProposal(QWidget* parent, allcore::ProposalKind kind,
                         const QString& wylie, const QString& value,
                         const QString& field, const QString& evidence);
// batch spelling flags from the Overlay's spelling-doubts panel; rows
// are {wylie, value, field, evidence}. Returns count filed, -1 on
// failure (the helper explains the failure to the user itself).
static int fileSpellingProposals(QWidget* parent,
                                 const QList<QStringList>& rows);
// #62 illustration-candidate geometry (defined before ScanPane)
static std::vector<QRect> illustrationCandidates(
    int w, int h, const std::vector<QRect>& lines);
static void runIllustrationGallery(QWidget* parent, const QString& root,
                                   const QStringList& paths,
                                   const QString& title);
#include "allcore/verse.h"
#include "allcore/wilsonparse.h"
#include "allcore/terminology.h"
#include "allcore/tibcal.h"
#include "allcore/tibcal_day.h"
#include "allcore/tibexport.h"

// What an entry card shows — the Overlay's display toggles feed this; other
// panes use the defaults (which match the historical behavior).
struct EntryDisplay {
    bool phonetics = true;
    bool glosses = true;
    bool sanskrit = true;
    bool hopkins = false;
};

// honorific register rows: wylie -> (ordinary, domain, level); loaded
// once in main(), consulted by every entry card in every pane
using HonorificMap = std::map<std::string, std::array<QString, 3>>;
static const HonorificMap* g_honorifics = nullptr;

// the idioms register (Adam, 2026-08-10): wylie -> (status, evidence).
// Marks WHICH strings are idioms/fixed expressions — English stays
// the dictionary's (rule 1). Grows via Propose (kind: idiom) + the
// authority's ruling; proposed entries LOOK proposed.
using IdiomMap = std::map<std::string, std::pair<QString, QString>>;
static const IdiomMap* g_idioms = nullptr;

// teaching moments (#63, Adam-authorized 2026-08-11): normalized HGM
// gloss -> up to 5 timecoded links to Geshe Michael's recorded
// classes (DCC YouTube). Captions were the FINDING AID only; every
// link opens the recording, which is the authority.
struct TeachingMoment {
    QString title, url, lang, src, snippet;
    int t = 0;
};
using TeachingMap = std::map<std::string, std::vector<TeachingMoment>>;
static const TeachingMap* g_teaching = nullptr;
// spoken-Tibetan tier: WYLIE -> moments where he SAYS the word
static const TeachingMap* g_teachingTib = nullptr;

// Das (1902) reference layer (#61): syllable-onset -> PDF page from
// the team's own bookmarked copy (public-domain text). Reference
// comparanda only, never HGM material.
static std::vector<std::pair<std::string, int>>* g_dasSections =
    nullptr;
static QString g_dasPdfPath;
static int dasPageFor(const std::string& wylie) {
    if (!g_dasSections || g_dasSections->empty()) return -1;
    std::string syl = wylie.substr(0, wylie.find(' '));
    int best = -1;
    size_t bestLen = 0;
    for (const auto& [onset, page] : *g_dasSections)
        if (syl.compare(0, onset.size(), onset) == 0 &&
            onset.size() > bestLen) {
            bestLen = onset.size();
            best = page;
        }
    return best;
}
static void showDasPage(QWidget* parent, int page);

// normalization MUST mirror tools/build_teaching_index.py terms.txt
static std::string teachingKey(std::string g) {
    std::string t;
    bool inParen = false;
    for (char c : g) {
        if (c == '(') inParen = true;
        else if (c == ')') inParen = false;
        else if (!inParen) t += (char)std::tolower((unsigned char)c);
    }
    std::string u;
    for (char c : t)
        u += (std::isalpha((unsigned char)c) || c == ' ' || c == '-' ||
              c == '\'')
                 ? c
                 : ' ';
    std::istringstream is(u);
    std::string w, out;
    bool first = true;
    while (is >> w) {
        if (first && w == "to") { first = false; continue; }
        first = false;
        out += (out.empty() ? "" : " ") + w;
    }
    return out;
}

// spelling forms the authority has ruled VALID (a DECLINED spelling
// flag means "this form is fine") — the Overlay legality check treats
// them as legal. Loaded from the proposal store once per launch.
static const std::set<std::string>* g_spellingValid = nullptr;

// The published apparatus (GMR-approved, mined from released books):
// 344 footnotes keyed by English lemma + 138 bibliography entries.
// Loaded once; flags terms wherever they appear.
struct ApparatusNote { QString lemma, text, source; int num = 0; };
struct ApparatusBib { QString source, section, id, text; };
static const std::vector<ApparatusNote>* g_appNotes = nullptr;
static const std::vector<ApparatusBib>* g_appBib = nullptr;
// the colloquial/prenasal register, for the cards' "also heard" line
static const allcore::ColloquialPron* g_colloquial = nullptr;

static QString entryHtml(const allcore::Entry& e,
                         const EntryDisplay& d = EntryDisplay{}) {
    QString h;
    h += "<div style='margin-bottom:14px'>";
    h += "<span style='font-size:22px'>" +
         QString::fromStdString(e.tibetan).toHtmlEscaped() + "</span> ";
    h += "<b>" + QString::fromStdString(e.wylie).toHtmlEscaped() + "</b>";
    if (g_appNotes && !e.hgm_gloss.empty()) {
        for (const auto& note : *g_appNotes) {
            bool hit = false;
            for (const auto& g : e.hgm_gloss)
                if (!note.lemma.isEmpty() &&
                    QString::fromStdString(g).contains(note.lemma,
                                                       Qt::CaseInsensitive))
                    hit = true;
            if (hit) {
                h += "<div style='background:#F3EDDF;padding:2px 6px;"
                     "border-radius:4px;margin:2px 0;font-size:11px'>"
                     "\U0001F4CE dealt with in published footnote " +
                     QString::number(note.num) + " \u2014 <i>" +
                     note.source.toHtmlEscaped() + "</i></div>";
                break;
            }
        }
    }
    if (g_honorifics) {
        auto it = g_honorifics->find(e.wylie);
        if (it != g_honorifics->end()) {
            const auto& [ord, dom, lvl] = it->second;
            h += " <span style='background:#EADFF7;color:#4A2A6B;"
                 "padding:1px 7px;border-radius:8px;font-size:11px'>" +
                 QString(lvl == "high" ? "HIGH honorific"
                         : lvl == "humilific" ? "humilific"
                         : lvl == "double" ? "double honorific"
                                           : "honorific") +
                 " (zhe sa)</span>";
            if (!ord.isEmpty())
                h += " <small style='color:#555'>ordinary: <b>" +
                     ord.toHtmlEscaped() + "</b></small>";
        }
    }
    if (g_idioms) {
        auto it = g_idioms->find(e.wylie);
        if (it != g_idioms->end()) {
            const bool approved = it->second.first == "approved";
            h += QString(" <span style='background:%1;color:%2;"
                         "padding:1px 7px;border-radius:8px;"
                         "font-size:11px' title='%3'>IDIOM%4</span>")
                     .arg(approved ? "#DDEBDC" : "#F2E8CF")
                     .arg(approved ? "#2C5B2E" : "#6E5A1E")
                     .arg(it->second.second.toHtmlEscaped())
                     .arg(approved ? "" : " (proposed)");
        }
    }
    if (g_dasSections && !e.wylie.empty()) {
        const int pg = dasPageFor(e.wylie);
        if (pg > 0)
            h += QString(" <a href='das:%1' style='font-size:11px'>"
                         "Das 1902 \u00b7 ~p.%1 (reference)</a>")
                     .arg(pg);
    }
    if (g_teaching && !e.hgm_gloss.empty()) {
        std::vector<const TeachingMoment*> ms;
        std::set<QString> seen;
        for (const auto& g : e.hgm_gloss) {
            auto it = g_teaching->find(teachingKey(g));
            if (it == g_teaching->end()) continue;
            for (const auto& m : it->second)
                if (!seen.count(m.url) && ms.size() < 3) {
                    seen.insert(m.url);
                    ms.push_back(&m);
                }
        }
        if (!ms.empty()) {
            h += "<div style='margin-top:6px;font-size:12px'>"
                 "<span style='color:#7C2D26;font-weight:600'>Geshe "
                 "Michael teaching this term</span> <i "
                 "style='color:#888'>(machine-located from class "
                 "captions \u2014 the recording is the "
                 "authority)</i><br>";
            for (const auto* m : ms) {
                const int mm = m->t / 60, ss = m->t % 60;
                h += QString("\u25B6 <a href='%1' title=\"%2\">%3</a>"
                             " @%4:%5%6<br>")
                         .arg(m->url,
                              m->snippet.toHtmlEscaped(),
                              m->title.left(60).toHtmlEscaped())
                         .arg(mm)
                         .arg(ss, 2, 10, QChar('0'))
                         .arg(m->lang == "ENG" || m->lang == "?"
                                  ? QString()
                                  : " <b>[" + m->lang + "]</b>");
            }
            h += "</div>";
        }
    }
    if (g_teachingTib && !e.wylie.empty()) {
        auto it = g_teachingTib->find(e.wylie);
        if (it != g_teachingTib->end() && !it->second.empty()) {
            h += "<div style='margin-top:4px;font-size:12px'>"
                 "<span style='color:#7C2D26;font-weight:600'>He says "
                 "this word</span> <i style='color:#888'>(phonetic "
                 "match on his own convention \u2014 candidates; "
                 "homophones share moments)</i><br>";
            int shown = 0;
            for (const auto& m : it->second) {
                if (++shown > 2) break;
                h += QString("\u25B6 <a href='%1' title=\"%2\">%3</a>"
                             " @%4:%5%6<br>")
                         .arg(m.url, m.snippet.toHtmlEscaped(),
                              m.title.left(60).toHtmlEscaped())
                         .arg(m.t / 60)
                         .arg(m.t % 60, 2, 10, QChar('0'))
                         .arg((m.lang == "ENG" || m.lang == "?"
                                   ? QString()
                                   : " <b>[" + m.lang + "]</b>") +
                              (m.src == "TKB"
                                   ? " <i style='color:#7C2D26'>"
                                     "\u00b7 lam rim</i>"
                                   : QString()));
            }
            h += "</div>";
        }
    }
    if (!e.tibetan_source.empty())
        h += " <i style='color:#888'>[generated script]</i>";
    if (d.phonetics && !e.pronunciation.empty()) {
        h += "<br>pron: " + QString::fromStdString(e.pronunciation).toHtmlEscaped();
        if (e.pronunciation_card_attested) h += " ⟪card⟫";
        // registered variants: community spellings and the prenasal
        // (kamdir) forms — labeled by class, never replacing the
        // GMR-convention pron above
        if (g_colloquial) {
            QString also;
            for (const auto* v : g_colloquial->byWylie(e.wylie)) {
                if (!also.isEmpty()) also += " · ";
                also += QString::fromStdString(v->colloquial)
                            .toHtmlEscaped() +
                        (v->cls == "prenasal-derived"
                             ? " <span style='color:#999'>(prenasal, "
                               "derived)</span>"
                             : " <span style='color:#999'>(community)"
                               "</span>");
            }
            if (!also.isEmpty())
                h += "<br><small style='color:#666'>also heard: " +
                     also + "</small>";
        }
    }
    if (d.glosses) {
        for (const auto& g : e.hgm_gloss) {
            QString tier = e.provisional()
                ? "<span style='color:#b00'>PROVISIONAL (auto-aligned)</span>"
                : QString::fromStdString("HGM (" + e.tier + ")");
            h += "<br>≡ " + QString::fromStdString(g).toHtmlEscaped() +
                 " &nbsp;<small>[" + tier + "]</small>";
        }
        if (e.hgm_gloss.empty())
            h += "<br><i>(no HGM equivalent — " +
                 QString::fromStdString(e.status).toHtmlEscaped() + ")</i>";
    }
    if (d.sanskrit && !e.sanskrit_reference.empty())
        h += "<br><small style='color:#666'>sanskrit (reference): " +
             QString::fromStdString(e.sanskrit_reference).left(160).toHtmlEscaped() +
             "</small>";
    if (d.hopkins && !e.hopkins_reference.empty())
        h += "<br><small style='color:#666'>Hopkins (reference only): " +
             QString::fromStdString(e.hopkins_reference).left(200).toHtmlEscaped() +
             "</small>";
    h += "</div>";
    return h;
}

// Link-out tier (survey): external sites searched for a term — links only,
// no ingestion, so restrictive licenses (BY-NC-ND etc.) are never touched.
// Every URL format verified live 2026-08-07; Adarsha omitted (its new site
// broke the old search URLs — recheck later).
static QString linkOutHtml(const std::string& wylie) {
    const QString q = QString::fromUtf8(
        QUrl::toPercentEncoding(QString::fromStdString(wylie)));
    QString h =
        "<div style='color:#777;font-size:11px;margin-top:6px'>search "
        "elsewhere <i>(external sites, links only)</i>: ";
    h += "<a href='https://84000.co/search?query=" + q + "'>84000</a> · ";
    h += "<a href='https://library.bdrc.io/search?q=%22" + q +
         "%22~1&lg=bo-x-ewts&t=Etext'>BDRC etexts</a> · ";
    h += "<a href='https://www2.hf.uio.no/polyglotta/index.php?page=search&q=" +
         q + "'>Bibliotheca Polyglotta</a> · ";
    h += "<a href='https://www.lotsawahouse.org/search?q=" + q +
         "'>Lotsawa House</a></div>";
    return h;
}

static QString mvpHtml(const std::vector<const allcore::MvpEntry*>& hits) {
    if (hits.empty()) return {};
    QString h = "<hr><div style='color:#4A3B7A'><b>Mahāvyutpatti</b> "
                "<small>(DILA TEI, CC BY-SA 3.0 — the classical 9th-century "
                "Skt⇄Tib glossary; reference only)</small></div>";
    for (const auto* e : hits) {
        QString tib;
        for (const auto& t : e->tibetan)
            tib += (tib.isEmpty() ? "" : " · ") +
                   QString::fromStdString(t).toHtmlEscaped();
        QString wy;
        for (const auto& w : e->wylie)
            wy += (wy.isEmpty() ? "" : " · ") +
                  QString::fromStdString(w).toHtmlEscaped();
        h += QString("<div style='margin:5px 0'><b>MVP %1</b> · %2 "
                     "<span style='font-size:18px'>%3</span> · %4</div>")
                 .arg(e->key)
                 .arg(QString::fromStdString(e->iast).toHtmlEscaped(),
                      tib, wy);
    }
    return h;
}

static HonorificMap loadHonorifics(const QString& path) {
    HonorificMap m;
    QFile f(path);
    if (!f.open(QIODevice::ReadOnly | QIODevice::Text)) return m;
    while (!f.atEnd()) {
        const QString line = QString::fromUtf8(f.readLine()).trimmed();
        if (line.isEmpty() || line.startsWith('#')) continue;
        const QStringList c = line.split('\t');
        if (c.size() < 4) continue;
        m[c[0].toStdString()] = {c[1], c[2], c[3]};
    }
    return m;
}

// The Lookup pane's whole stacked search, extracted for the
// selftest: HGM binding layer first, reference layers labeled,
// honorific badges, colloquial variants, link-outs.
static QString lookupResultsHtml(allcore::Spine& spine,
                                 allcore::RefDict* ref,
                                 allcore::Mvp* mvp,
                                 allcore::WhitneyRoots* whitney,
                                 allcore::ColloquialPron* colloq,
                                 const std::string& raw) {
    auto entries = spine.lookup(raw);
    if (entries.empty()) entries = spine.headwordSearch('"' + raw + '"', 10);
    QString h;
    // affix-stripping fallback (lucene-bo rules): po'i finds po
    if (entries.empty()) {
        bool upper = false;
        for (char c : raw) upper |= (c >= 'A' && c <= 'Z');
        const std::string wy = upper ? allcore::acipToEwts(raw) : raw;
        const std::string base =
            allcore::stripAffixedParticlesWylie(wy);
        if (base != wy) {
            entries = spine.lookup(base);
            if (!entries.empty())
                h += QString("<div style='color:#1E6B4E;font-size:"
                             "12px'>no entry for \u201c%1\u201d — "
                             "showing <b>%2</b> (affixed particle "
                             "stripped)</div>")
                         .arg(QString::fromStdString(wy)
                                  .toHtmlEscaped(),
                              QString::fromStdString(base)
                                  .toHtmlEscaped());
        }
    }
    // pronunciation fallback (GMR convention): 'jangchub' finds
    // byang chub — deterministic fold, labeled
    bool viaPron = false;
    if (entries.empty()) {
        entries = spine.lookupByPronunciation(raw, 8);
        viaPron = !entries.empty();
    }
    if (viaPron)
        h += "<div style='color:#1E6B4E;font-size:12px'>matched by "
             "<b>pronunciation</b> (GMR convention) — no exact "
             "headword for \u201c" +
             QString::fromStdString(raw).toHtmlEscaped() +
             "\u201d</div>";
    // colloquial register fallback: community spellings (gonpa,
    // tulku\u2026) and the HGM prenasal forms (kamdir) \u2014 the register
    // only WIDENS lookup, GMR stays canonical
    if (entries.empty() && colloq) {
        for (const auto* ce : colloq->byColloquial(raw)) {
            auto hits = spine.lookup(ce->wylie);
            bool any = false;
            for (auto& e : hits) {
                bool dup = false;
                for (auto& x : entries) dup |= x.id == e.id;
                if (!dup) {
                    entries.push_back(std::move(e));
                    any = true;
                }
            }
            if (any)
                h += QString("<div style='color:#1E6B4E;font-size:"
                             "12px'>matched by <b>colloquial "
                             "pronunciation</b> (%1: %2 = %3%4)"
                             "</div>")
                         .arg(ce->cls == "prenasal-derived"
                                  ? "prenasal rule, derived"
                                  : "community usage register")
                         .arg(QString::fromStdString(ce->colloquial)
                                  .toHtmlEscaped())
                         .arg(QString::fromStdString(ce->wylie)
                                  .toHtmlEscaped())
                         .arg(ce->gmrPron.empty()
                                  ? QString()
                                  : "; GMR convention: " +
                                        QString::fromStdString(
                                            ce->gmrPron)
                                            .toHtmlEscaped());
        }
    }
    if (entries.empty()) h = "<i>no HGM match</i>";
    // (honorific badges render inside entryHtml, every pane alike)
    for (const auto& e : entries) h += entryHtml(e);

    if (ref) {
        // resolve to wylie for the reference layers (they key on wylie)
        std::string wylie = raw;
        if (!entries.empty()) wylie = entries.front().wylie;
        else {
            // uppercase input = ACIP
            bool upper = false;
            for (char c : raw) upper |= (c >= 'A' && c <= 'Z');
            if (upper) wylie = allcore::acipToEwts(raw);
        }
        auto refs = ref->lookup(wylie);
        if (!refs.empty()) {
            h += "<hr><div style='color:#993C1D'><b>Reference layers</b> "
                 "<small>(unlicensed compilations — local lookup only, "
                 "never for release data)</small></div>";
            for (const auto& r : refs) {
                h += "<div style='margin:8px 0'><span style='background:"
                     "#FAEEDA;color:#633806;padding:1px 7px;border-radius:8px;"
                     "font-size:12px'>" +
                     QString::fromStdString(r.layer).toHtmlEscaped() +
                     "</span> " +
                     QString::fromStdString(r.definition)
                         .left(500)
                         .toHtmlEscaped() +
                     "</div>";
            }
        }
    }
    // Mahāvyutpatti: classical Skt⇄Tib reference (exact wylie match)
    if (mvp) {
        std::string wylie = raw;
        if (!entries.empty()) wylie = entries.front().wylie;
        else {
            bool upper = false;
            for (char c : raw) upper |= (c >= 'A' && c <= 'Z');
            if (upper) wylie = allcore::acipToEwts(raw);
        }
        h += mvpHtml(mvp->byWylie(wylie));
    }
    // Whitney reference layer (Roots 1885 + Grammar 1879 citations;
    // public domain, digitization Apache-2.0 — data/whitney/):
    // Latin queries only — try as an IAST root, else as an English
    // meaning word. Reference comparanda, never HGM equivalents.
    {
        bool hasAlpha = false;
        for (unsigned char c : raw)
            hasAlpha |= std::isalpha(c) != 0;
        bool hasTibetan = false;   // U+0F00–U+0FFF = E0 BC/BD/BE/BF …
        for (size_t i = 0; i + 2 < raw.size(); ++i)
            if ((unsigned char)raw[i] == 0xE0 &&
                ((unsigned char)raw[i + 1] & 0xFC) == 0xBC)
                hasTibetan = true;
        if (whitney && hasAlpha && !hasTibetan) {
            auto roots = whitney->byRoot(raw);
            QString matchKind = "root";
            if (roots.empty()) {   // past participle: gata -> gam
                roots = whitney->byPpp(raw);
                if (!roots.empty()) matchKind = "past participle";
            }
            if (roots.empty() && raw.find(' ') == std::string::npos) {
                roots = whitney->byMeaning(raw, 8);
                if (!roots.empty()) matchKind = "English meaning";
            }
            if (!roots.empty()) {
                h += "<hr><div style='color:#4A3A7A'><b>Whitney — "
                     "Roots, Verb-Forms and Primary Derivatives "
                     "(1885)</b> <small>(public-domain reference; "
                     "matched by " + matchKind + ")</small></div>";
                for (const auto* r : roots) {
                    h += "<div style='margin:6px 0'><b>";
                    if (!r->homonym.empty())
                        h += QString::fromStdString(r->homonym) + " ";
                    h += "√" +
                         QString::fromStdString(r->root)
                             .toHtmlEscaped() +
                         "</b> “" +
                         QString::fromStdString(r->meaning)
                             .toHtmlEscaped() +
                         "”";
                    if (!r->classes.empty())
                        h += " · class " +
                             QString::fromStdString(r->classes)
                                 .replace("|", "·")
                                 .toHtmlEscaped();
                    if (!r->classUncertain.empty())
                        h += " <small style='color:#777'>(also? " +
                             QString::fromStdString(r->classUncertain)
                                 .replace("|", "·")
                                 .toHtmlEscaped() +
                             ")</small>";
                    if (!r->ppp.empty())
                        h += " · ppp <b>" +
                             QString::fromStdString(r->ppp)
                                 .toHtmlEscaped() +
                             "</b>";
                    if (!r->dcsClasses.empty() &&
                        r->dcsClasses != "—")
                        h += " · <small style='color:#777'>corpus: " +
                             QString::fromStdString(r->dcsClasses)
                                 .toHtmlEscaped() +
                             "</small>";
                    if (!r->senses.empty())
                        h += "<br><small style='color:#555'>MW: " +
                             QString::fromStdString(r->senses)
                                 .left(140)
                                 .toHtmlEscaped() +
                             "</small>";
                    // link-out: Cologne MW display, SLP1-keyed
                    // (URL format verified live 2026-08-08)
                    if (!r->mwId.empty() && !r->slp1.empty())
                        h += " <small><a href='https://"
                             "sanskrit-lexicon.uni-koeln.de/scans/"
                             "MWScan/2020/web/webtc/getword.php?"
                             "key=" +
                             QString::fromStdString(r->slp1)
                                 .toHtmlEscaped() +
                             "&filter=roman'>MW entry →</a></small>";
                    if (!r->grammarSecs.empty() &&
                        r->grammarSecs != "—")
                        h += "<br><small style='color:#555'>Sanskrit "
                             "Grammar (1879) " +
                             QString::fromStdString(r->grammarSecs)
                                 .left(160)
                                 .toHtmlEscaped() +
                             " <span style='color:#777'>(✦ "
                             "specific · ⚠ exception)"
                             "</span></small>";
                    if (!r->sectionRefs.empty()) {
                        // topical ranges: perfect:781-823|… → a
                        // compact by-topic index into the Grammar
                        QString topics;
                        const QStringList parts =
                            QString::fromStdString(r->sectionRefs)
                                .split('|');
                        int shown = 0;
                        for (const QString& p2 : parts) {
                            if (shown++ >= 6) {
                                topics += QString("· +%1 more ")
                                              .arg(parts.size() - 6);
                                break;
                            }
                            QString t2 = p2;
                            t2.replace(':', " §§");
                            t2.replace('_', ' ');
                            topics += t2.toHtmlEscaped() + " · ";
                        }
                        if (!topics.isEmpty())
                            h += "<br><small style='color:#777'>"
                                 "Grammar by topic: " +
                                 topics + "</small>";
                    }
                    h += "</div>";
                }
                h += "<div style='font-size:11px;color:#777'>read "
                     "the Grammar itself: <a href='https://archive."
                     "org/details/sanskritgrammari00whituoft'>"
                     "archive.org scan (public domain)</a></div>";
            }
        }
    }
    // link-out tier: external searches for the resolved term
    {
        std::string wylie = raw;
        if (!entries.empty()) wylie = entries.front().wylie;
        else {
            bool upper = false;
            for (char c : raw) upper |= (c >= 'A' && c <= 'Z');
            if (upper) wylie = allcore::acipToEwts(raw);
        }
        h += linkOutHtml(wylie);
    }
    // English reverse lookup (release reverse index, binding layer)
    {
        std::string eng = raw;
        for (auto& c : eng) c = (char)std::tolower((unsigned char)c);
        auto rev = spine.reverseIndex(eng);
        if (!rev.empty()) {
            h += "<hr><div><b>English \u2192 Tibetan</b> <small>(HGM reverse "
                 "index)</small></div>";
            for (const auto& r : rev) {
                QString tier = r.tier == "auto-aligned"
                    ? "<span style='color:#b00'>PROVISIONAL</span>"
                    : QString::fromStdString("HGM (" + r.tier + ")");
                h += "<div style='margin:5px 0'><b>" +
                     QString::fromStdString(r.wylie).toHtmlEscaped() + "</b> \u00b7 " +
                     QString::fromStdString(r.pronunciation).toHtmlEscaped() +
                     " &nbsp;<small>[" + tier + "]</small></div>";
            }
        }
    }
    // the proposal channel, reachable while looking things up
    h += QString("<div style='margin-top:8px;font-size:12px'>"
                 "<a href='propose:%1'>propose to the authority\u2026"
                 "</a></div>")
             .arg(QString::fromStdString(raw).toHtmlEscaped());

    return h;
}

// propose a term to the authority from anywhere a card shows it —
// kind picker + proposed value, filed through the shared channel
static void proposeTermDialog(QWidget* parent, const QString& wylie,
                              const QString& evidence) {
    const QStringList kinds = {
        "Honorific term (\u2194 ordinary)", "HIGH honorific marking",
        "Pronunciation exception", "Abbreviation / contraction candidate",
        "Rendering for this word (\u2192 dictionary)",
        "Idiom / fixed expression", "Note about this term"};
    bool ok = false;
    const QString choice = QInputDialog::getItem(
        parent, "Propose to the authority",
        "What are you proposing for \u201c" + wylie + "\u201d?", kinds, 0,
        false, &ok);
    if (!ok) return;
    allcore::ProposalKind kind = allcore::ProposalKind::WordRendering;
    if (choice.startsWith("Honorific")) kind = allcore::ProposalKind::Honorific;
    else if (choice.startsWith("HIGH"))
        kind = allcore::ProposalKind::HighHonorific;
    else if (choice.startsWith("Pronunciation"))
        kind = allcore::ProposalKind::Pronunciation;
    else if (choice.startsWith("Abbreviation"))
        kind = allcore::ProposalKind::Abbreviation;
    else if (choice.startsWith("Idiom"))
        kind = allcore::ProposalKind::Idiom;
    else if (choice.startsWith("Note")) kind = allcore::ProposalKind::Note;
    QString value;
    if (kind != allcore::ProposalKind::Note) {
        value = QInputDialog::getText(
            parent, "Proposed value",
            kind == allcore::ProposalKind::Pronunciation
                ? "Proposed pronunciation:"
                : (kind == allcore::ProposalKind::Honorific
                       ? "Ordinary counterpart (or leave blank):"
                       : "Proposed English / expansion:"),
            QLineEdit::Normal, "", &ok);
        if (!ok) return;
    }
    QString field;
    if (kind == allcore::ProposalKind::Honorific ||
        kind == allcore::ProposalKind::HighHonorific ||
        kind == allcore::ProposalKind::Humilific ||
        kind == allcore::ProposalKind::DoubleHonorific) {
        field = value;
        value.clear();
    }
    loadIdentity();
    fileProposal(parent, kind, wylie, value, field, evidence);
}

static QWidget* makeLookupPane(allcore::Spine& spine, allcore::RefDict* ref,
                               allcore::Mvp* mvp,
                               allcore::WhitneyRoots* whitney = nullptr,
                               allcore::ColloquialPron* colloq = nullptr) {
    auto* pane = new QWidget;
    auto* outer = new QHBoxLayout(pane);
    auto* split = new QSplitter(Qt::Horizontal);
    outer->addWidget(split);

    // left: alphabetical browse in Tibetan dictionary order (ICU/CLDR "bo"
    // rules — collation_smoke pins the traditional properties)
    auto* left = new QWidget;
    auto* leftL = new QVBoxLayout(left);
    auto* browseBtn =
        new QPushButton("Browse the dictionary (Tibetan order)");
    auto* browseList = new QListView;
    browseList->setEditTriggers(QAbstractItemView::NoEditTriggers);
    auto* browseModel = new QStringListModel(browseList);
    browseList->setModel(browseModel);
    leftL->addWidget(browseBtn);
    leftL->addWidget(browseList);
    auto browseWylie = std::make_shared<QStringList>();  // row -> wylie

    auto* right = new QWidget;
    auto* layout = new QVBoxLayout(right);
    auto* box = new QLineEdit;
    box->setPlaceholderText("wylie · Tibetan · ACIP headword…");
    auto* results = new QTextBrowser;
    results->setOpenExternalLinks(true);  // link-out tier opens the browser
    QObject::connect(
        results, &QTextBrowser::anchorClicked,
        [pane, box](const QUrl& u) {
            const QString s = u.toString();
            if (s.startsWith("das:")) {
                showDasPage(pane, s.mid(4).toInt());
                QMetaObject::invokeMethod(box, "returnPressed");
            } else if (s.startsWith("propose:")) {
                proposeTermDialog(
                    pane, s.mid(8),
                    "looked up in the dictionary (Lookup pane)");
                // re-run the search: the browser navigated away on click
                QMetaObject::invokeMethod(box, "returnPressed");
            }
        });
    layout->addWidget(box);
    layout->addWidget(results);
    split->addWidget(left);
    split->addWidget(right);
    split->setStretchFactor(0, 1);
    split->setStretchFactor(1, 2);

    QObject::connect(browseBtn, &QPushButton::clicked, [&spine, browseBtn,
                                                        browseModel,
                                                        browseWylie] {
        if (!browseWylie->isEmpty()) return;  // built once
        browseBtn->setEnabled(false);
        browseBtn->setText("sorting the dictionary…");
        QCoreApplication::processEvents();
        // (display unicode, sort key from it; wylie kept for the click)
        struct Row { QString uni; QString wylie; QCollatorSortKey key; };
        QCollator bo(QLocale("bo"));
        std::vector<Row> rows;
        for (const auto& [id, acip] : spine.allAcipHeadwords()) {
            const std::string wylie = allcore::acipToEwts(acip);
            auto [uni, ok] = allcore::wylieToUnicode(wylie);
            QString wy = QString::fromStdString(wylie);
            QString un = ok ? QString::fromStdString(uni) : wy;
            rows.push_back(Row{un, wy, bo.sortKey(un)});
        }
        std::sort(rows.begin(), rows.end(),
                  [](const Row& a, const Row& b) { return a.key < b.key; });
        QStringList display;
        display.reserve((int)rows.size());
        for (const auto& r : rows) {
            display << r.uni;
            *browseWylie << r.wylie;
        }
        browseModel->setStringList(display);
        browseBtn->setText(
            QString("%1 headwords, Tibetan dictionary order").arg(rows.size()));
    });
    QObject::connect(browseList, &QListView::clicked,
                     [box, browseWylie](const QModelIndex& ix) {
                         if (ix.row() < 0 || ix.row() >= browseWylie->size())
                             return;
                         box->setText(browseWylie->at(ix.row()));
                         // invoke the signal to run the normal lookup path
                         QMetaObject::invokeMethod(box, "returnPressed");
                     });

    QObject::connect(box, &QLineEdit::returnPressed, [&spine, ref, mvp,
                                                      whitney, colloq, box,
                                                      results] {
        const std::string raw = box->text().trimmed().toStdString();
        if (raw.empty()) return;
        results->setHtml(lookupResultsHtml(spine, ref, mvp, whitney,
                                           colloq, raw));
    });
    return pane;
}

namespace {

class AnalysisPane : public QWidget {
public:
    AnalysisPane(allcore::Spine& spine, QString templatePath, QString saveDir)
        : spine_(spine), templatePath_(std::move(templatePath)),
          saveDir_(std::move(saveDir)) {
        auto* outer = new QVBoxLayout(this);
        auto* split = new QSplitter(Qt::Horizontal);

        auto* left = new QWidget;
        auto* ll = new QVBoxLayout(left);
        ll->addWidget(new QLabel("<b>Passage (ACIP)</b>"));
        passage_ = new QPlainTextEdit;
        passage_->setPlaceholderText("Paste the ACIP passage here…");
        ll->addWidget(passage_, 3);
        ll->addWidget(new QLabel("Draft English (optional)"));
        draft_ = new QPlainTextEdit;
        ll->addWidget(draft_, 1);
        analyze_ = new QPushButton("Analyze");
        status_ = new QLabel;
        status_->setWordWrap(true);
        ll->addWidget(analyze_);
        ll->addWidget(status_);

        auto* right = new QSplitter(Qt::Vertical);
        report_ = new QTextBrowser;
        report_->setOpenExternalLinks(false);
        qc_ = new QTextBrowser;
        right->addWidget(report_);
        right->addWidget(qc_);
        right->setStretchFactor(0, 4);
        right->setStretchFactor(1, 1);

        split->addWidget(left);
        split->addWidget(right);
        split->setStretchFactor(0, 1);
        split->setStretchFactor(1, 2);
        outer->addWidget(split);

        if (qgetenv("ANTHROPIC_API_KEY").isEmpty()) {
            analyze_->setEnabled(false);
            status_->setText("ANTHROPIC_API_KEY is not set — start the app from a "
                             "terminal where the key is exported.");
        }
        connect(analyze_, &QPushButton::clicked, [this] { run(); });
    }

private:
    void run() {
        const std::string passage = passage_->toPlainText().toStdString();
        if (passage.empty()) {
            status_->setText("Paste a passage first.");
            return;
        }
        analyze_->setEnabled(false);
        report_->clear();
        qc_->clear();
        accum_.clear();
        try {
            auto pre = allcore::runPrePass(spine_, passage);
            status_->setText(QString("engine: %1 anchors, %2 corpus hits — asking "
                                     "Claude…")
                                 .arg(pre.anchors.size())
                                 .arg(pre.corpus_hits.size()));
            auto prompt = allcore::buildAnalysisPrompt(
                templatePath_.toStdString(), pre, passage,
                draft_->toPlainText().toStdString());
            const std::string body = allcore::buildMessagesRequestJson(prompt);

            QNetworkRequest req(QUrl("https://api.anthropic.com/v1/messages"));
            req.setRawHeader("x-api-key", qgetenv("ANTHROPIC_API_KEY"));
            req.setRawHeader("anthropic-version", "2023-06-01");
            req.setRawHeader("anthropic-beta", "server-side-fallback-2026-07-01");
            req.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
            reply_ = net_.post(req, QByteArray::fromStdString(body));

            connect(reply_, &QNetworkReply::readyRead, [this] { onData(); });
            connect(reply_, &QNetworkReply::finished, [this, passage] {
                onDone(passage);
            });
        } catch (const std::exception& ex) {
            status_->setText(QString("error: %1").arg(ex.what()));
            analyze_->setEnabled(true);
        }
    }

    void onData() {
        buf_ += reply_->readAll();
        int nl;
        while ((nl = buf_.indexOf('\n')) >= 0) {
            const QByteArray line = buf_.left(nl).trimmed();
            buf_.remove(0, nl + 1);
            if (!line.startsWith("data:")) continue;
            const std::string data = line.mid(5).trimmed().toStdString();
            const std::string t = allcore::sseTextDelta(data);
            if (!t.empty()) {
                accum_ += t;
                // stream as plain text; the pretty markdown render lands at the end
                report_->setPlainText(QString::fromStdString(accum_));
                report_->verticalScrollBar()->setValue(
                    report_->verticalScrollBar()->maximum());
            } else if (data.find("\"stop_reason\":\"refusal\"") != std::string::npos) {
                status_->setText("The safety classifiers declined this request and "
                                 "no fallback served it.");
            } else if (data.find("\"type\":\"error\"") != std::string::npos) {
                status_->setText(QString("API error: %1")
                                     .arg(QString::fromStdString(data).left(300)));
            }
        }
    }

    void onDone(const std::string& passage) {
        if (reply_->error() != QNetworkReply::NoError && accum_.empty()) {
            status_->setText("network error: " + reply_->errorString());
        } else {
            report_->setMarkdown(QString::fromStdString(accum_));
            auto findings = allcore::validateReport(spine_, accum_, passage);
            qc_->setMarkdown(
                QString::fromStdString(allcore::findingsToMarkdown(findings)));
            int errs = 0;
            for (const auto& f : findings)
                if (f.severity == "error") ++errs;
            // auto-save: report + source passage + QC verdict, timestamped
            QString saved;
            QDir().mkpath(saveDir_);
            const QString fn = saveDir_ + "/analysis-" +
                QDateTime::currentDateTime().toString("yyyyMMdd-HHmmss") + ".md";
            QFile f(fn);
            if (f.open(QIODevice::WriteOnly)) {
                f.write("<!-- source passage:\n");
                f.write(QByteArray::fromStdString(passage));
                f.write("\n-->\n\n");
                f.write(QByteArray::fromStdString(accum_));
                f.write("\n\n---\n\n");
                f.write(QByteArray::fromStdString(
                    allcore::findingsToMarkdown(findings)));
                saved = " · saved " + QFileInfo(fn).fileName();
            }
            status_->setText((errs ? QString("done — QC flagged %1 error(s), see "
                                             "the panel below")
                                         .arg(errs)
                                   : QString("done — machine QC passed")) + saved);
        }
        reply_->deleteLater();
        reply_ = nullptr;
        analyze_->setEnabled(true);
    }

    allcore::Spine& spine_;
    QString templatePath_;
    QString saveDir_;
    QPlainTextEdit* passage_ = nullptr;
    QPlainTextEdit* draft_ = nullptr;
    QPushButton* analyze_ = nullptr;
    QLabel* status_ = nullptr;
    QTextBrowser* report_ = nullptr;
    QTextBrowser* qc_ = nullptr;
    QNetworkAccessManager net_;
    QNetworkReply* reply_ = nullptr;
    QByteArray buf_;
    std::string accum_;
};

// ---- Overlay pane: document view with nested depth shading -----------------
class OverlayPane : public QWidget {
public:
    // open a file into the overlay (used by the Library pane's browser)
    // --selftest: exercise this pane's wiring against the real spine
    int selfTest(QStringList& log, const QString& root) {
        selfTestRoot_ = root;
        int fails = 0;
        auto check = [&](bool ok, const char* what) {
            log << QString("  [%1] Overlay: %2")
                       .arg(ok ? "PASS" : "FAIL")
                       .arg(what);
            if (!ok) ++fails;
        };
        const QString acip =
            "SEMS CAN THAMS CAD BDE BA DANG LDAN PAR GYUR CIG,";
        scriptMode_->setCurrentIndex(0);
        input_->setPlainText(acip);
        loadDoc();
        check(!docIsWylie_, "ACIP sample detected as ACIP");
        const size_t nTok = doc_.tokens.size();
        check(nTok >= 8, "tokens parsed");
        check(!doc_.spans.empty(), "dictionary spans found");
        check(!doc_.entries.empty(), "entries resolved");
        check(!view_->toPlainText().contains(QChar(0x27E8)),
              "script conversion clean (no failure markers)");
        bool glossed = false;
        for (const auto& e : doc_.entries)
            if (e.wylie == "sems can" && !e.hgm_gloss.empty())
                glossed = true;
        check(glossed, "known term carries an HGM gloss");
        input_->setPlainText(acip.toLower());
        loadDoc();
        check(docIsWylie_, "lowercase sample detected as wylie");
        check(doc_.tokens.size() == nTok,
              "wylie tokenization matches ACIP");
        check(!view_->toPlainText().contains(QChar(0x27E8)),
              "wylie script conversion clean");
        input_->setPlainText("SEMS CAN QQZ BDE BA,");
        loadDoc();
        check(hint_->text().contains("1 spelling"),
              "illegal syllable raises one spellcheck flag");
        spellToggle_->setChecked(true);   // opens + populates the panel
        check(spellList_->rowCount() == 1 &&
                  spellList_->item(0, 1)->text() == "1",
              "spelling-doubts panel lists the doubted form");
        spellToggle_->setChecked(false);
        {
            // #62 logic: a left side-panel and a big inter-line gap
            // are found; a fully-texted page yields no candidates
            std::vector<QRect> tight = {{50, 10, 900, 40},
                                        {50, 60, 900, 40}};
            std::vector<QRect> art = {{300, 10, 650, 40},
                                      {300, 200, 650, 40}};
            const auto none = illustrationCandidates(1000, 250, tight);
            const auto some = illustrationCandidates(1000, 250, art);
            bool leftPanel = false, gap = false;
            for (const auto& r : some) {
                if (r.left() == 0 && r.width() == 300) leftPanel = true;
                if (r.top() == 49 && r.height() == 151) gap = true;
            }
            check(none.empty() && leftPanel && gap,
                  "illustration candidates: side panel + gap found, "
                  "clean page yields none");
        }
        input_->setPlainText("BDEN PA , SDUG BSNGAL ,");
        loadDoc();
        check(teachingsReportHtml().contains("youtube.com"),
              "teachings-for-this-text report reaches the corpus");
        input_->setPlainText("@04B ,GDAB ,");
        loadDoc();
        check(collectSpellHits().empty() &&
                  !hint_->text().contains("1 spelling"),
              "folio markers and dictionary-attested forms are not "
              "doubted");
        {
            // double-click must not auto-select (the line-highlight
            // bug): a synthetic double-click leaves no selection
            QMouseEvent dbl(QEvent::MouseButtonDblClick,
                            QPointF(5, 5), QPointF(5, 5),
                            Qt::LeftButton, Qt::LeftButton,
                            Qt::NoModifier);
            QCoreApplication::sendEvent(view_->viewport(), &dbl);
            check(!view_->textCursor().hasSelection() ||
                      view_->textCursor().selectedText().length() <
                          40,
                  "double-click does not select the line");
        }
        {
            shadeMode_->setCurrentIndex(0);   // GMR click mode
            input_->setPlainText("'PHAGS PA'I BDEN PA BZHI");
            loadDoc();
            int bden = -1;
            for (size_t i = 0; i < doc_.tokens.size(); ++i)
                if (doc_.tokens[i] == "BDEN") bden = (int)i;
            bool ok = bden >= 0;
            if (ok) {
                QTextCursor c(view_->document());
                c.setPosition(tokBeg_[bden]);
                view_->setTextCursor(c);   // triggers onClick
                const auto sels = view_->extraSelections();
                ok = !sels.empty() &&
                     sels[0].cursor.selectionStart() == tokBeg_[0] &&
                     sels[0].cursor.selectionEnd() >=
                         tokEnd_[doc_.tokens.size() - 1];
            }
            check(ok, "click mode highlights the LONGEST phrase "
                      "first (GMR directive)");
        }
        input_->setPlainText("GANG ZAG ,\n\nBDEN PA ,");
        loadDoc();
        check(view_->toPlainText().count('\n') == 2,
              "display mirrors the source's line breaks");

        // ---- robustness: hostile input must never crash, and the
        // token bookkeeping must stay consistent. ACIP display mode:
        // the offscreen platform lacks lazy text layout, so script
        // mode on large inputs is quadratic (a test-harness artifact,
        // not an app behavior — the small cases above cover script)
        scriptMode_->setCurrentIndex(1);
        auto consistent = [&] {
            return tokBeg_.size() == doc_.tokens.size() &&
                   tokEnd_.size() == doc_.tokens.size();
        };
        const struct { const char* name; const char* text; } hostile[] = {
            {"empty document", ""},
            {"punctuation only", ",,,///;;;   ,,,"},
            {"emoji and symbols", "\U0001F64F \u2764 \u262F test \U0001F304"},
            {"raw Tibetan script pasted",
             "\u0F66\u0F7A\u0F58\u0F66\u0F0B\u0F45\u0F53\u0F0B"
             "\u0F50\u0F58\u0F66\u0F0B\u0F45\u0F51\u0F0D"},
            {"mixed transliterations",
             "SEMS CAN thams cad BDE ba dang LDAN par"},
            {"lone 'a chungs and shads", "' '' ' / // '"},
        };
        for (const auto& hcase : hostile) {
            input_->setPlainText(QString::fromUtf8(hcase.text));
            loadDoc();
            check(consistent(),
                  (QString("survives ") + hcase.name).toUtf8().constData());
        }
        {
            // a large single line through the full pane path (the
            // offscreen platform lays out per format change, so the
            // widget-level case stays at tens of kilobytes; the
            // megabyte case runs against the engine core below)
            QString big;
            big.reserve(70000);
            for (int k = 0; k < 4000; ++k) big += "SEMS CAN BDE BA ";
            input_->setPlainText(big);
            loadDoc();
            check(consistent(), "survives a 64k single line");
        }

        // ---- performance regression guard on the ENGINE CORE: a
        // full canonical volume through buildOverlay (tokenize +
        // lattice + spans) — the algorithmic path a regression would
        // hit. Display timing is a GUI concern (viewport-lazy there,
        // pathological offscreen), covered by the manual checklist.
        {
            QFile v(selfTestRoot_ +
                    "/library/acip_release6/TD10199_T.TXT");
            if (v.open(QIODevice::ReadOnly)) {
                const std::string vol =
                    QString::fromUtf8(v.readAll()).toStdString();
                QElapsedTimer t;
                t.start();
                auto vdoc = allcore::buildOverlay(spine_, index_, vol);
                const qint64 ms = t.elapsed();
                log << QString("  [info] Overlay: full volume "
                               "(%1 tokens, %2 spans) through "
                               "buildOverlay in %3 ms")
                           .arg(vdoc.tokens.size())
                           .arg(vdoc.spans.size())
                           .arg(ms);
                check(vdoc.tokens.size() > 100000,
                      "full volume tokenizes at scale");
                check(ms < 15000,
                      "full-volume engine pass under the regression "
                      "ceiling");
            } else {
                log << "  [info] Overlay: volume file absent — perf "
                       "guard skipped";
            }
        }
        // widget-level open ceiling: the FULL pipeline (openFile ->
        // loadDoc -> lazy layout) on the largest volume — this is the
        // check that would have caught the Library-open hang
        {
            QFile v(selfTestRoot_ +
                    "/library/acip_release6/TD3811I_inc_t.txt");
            if (v.exists()) {
                QElapsedTimer t;
                t.start();
                openFile(v.fileName());
                const qint64 ms = t.elapsed();
                log << QString("  [info] Overlay: 1.2MB volume "
                               "openFile in %1 ms")
                           .arg(ms);
                check(ms < 20000,
                      "full-volume openFile under the widget ceiling");
            }
        }
        input_->clear();
        loadDoc();
        return fails;
    }

    void openFile(const QString& fn) {
        QFile f(fn);
        if (!f.open(QIODevice::ReadOnly)) return;
        input_->setPlainText(QString::fromUtf8(f.readAll()));
        docFile_ = fn;
        loadGlossary();
        loadDoc();
        auto info = allcore::decodeAcipFilename(fn.toStdString());
        setScanTarget(info, fn);
        // OCR outputs are review material until verified (OCR_DESIGN):
        // banner + first-pass QC via the syllable-legality layer
        if (fn.contains("/ocr_out/") && checker_) {
            int bad = 0, tot = 0;
            for (const auto& t : doc_.tokens) {
                bool alpha = false;
                for (char ch : t)
                    alpha |= std::isalpha((unsigned char)ch) != 0;
                if (!alpha) continue;
                ++tot;
                if (!checker_->legalWylie(allcore::acipToEwts(t))) ++bad;
            }
            hint_->setText(
                QString("⚠ OCR-DERIVED — unverified review material. "
                        "First-pass QC: %1 of %2 syllables fail the "
                        "legality check (red waves below). Verify before "
                        "any further use.\n")
                        .arg(bad)
                        .arg(tot) +
                hint_->text());
        }
        if (info.recognized)
            hint_->setText(
                QString("%1 · text %2 · %3\n")
                    .arg(QString::fromStdString(info.collection),
                         QString::fromStdString(info.number),
                         QString::fromStdString(info.status.empty()
                                                    ? info.language
                                                    : info.status)) +
                hint_->text());
    }

    OverlayPane(allcore::Spine& spine, allcore::SyllableChecker* checker,
                allcore::RefDict* ref, allcore::Progress* progress,
                const QString& root = QString())
        : spine_(spine), checker_(checker), ref_(ref), progress_(progress),
          index_(spine),
          scanCache_(root.isEmpty() ? QString()
                                    : root + "/library/scan_cache"),
          dataRoot_(root) {
        auto* outer = new QVBoxLayout(this);
        auto* split = new QSplitter(Qt::Horizontal);

        auto* left = new QWidget;
        auto* ll = new QVBoxLayout(left);
        ll->addWidget(new QLabel("<b>Document (ACIP)</b>"));
        input_ = new QPlainTextEdit;
        input_->setPlaceholderText("Paste an ACIP document…");
        input_->setMinimumHeight(90);
        ll->addWidget(input_, 1);
        auto* open = new QPushButton("Open ACIP file…");
        ll->addWidget(open);
        auto* load = new QPushButton("Load into overlay");
        ll->addWidget(load);
auto* secRev = new QLabel("<span style='color:#9A7A33;font-size:10px;letter-spacing:2px;font-weight:600'>REVIEW</span>");
        secRev->setContentsMargins(0, 8, 0, 0);
        ll->addWidget(secRev);
        spellToggle_ = new QCheckBox("Show spelling doubts");
        spellToggle_->setToolTip(
            "Open a small list of every syllable in this text that "
            "fails the classical legality rules \u2014 likely "
            "input-operator errors. Click a row to jump to it in the "
            "text; file checked rows for the authority's ruling.");
        ll->addWidget(spellToggle_);
        spellPanel_ = new QWidget;
        spellPanel_->setVisible(false);
        spellPanel_->setMaximumHeight(230);
        {
            auto* spv = new QVBoxLayout(spellPanel_);
            spv->setContentsMargins(0, 0, 0, 0);
            spv->setSpacing(4);
            spellList_ = new QTableWidget(0, 3);
            spellList_->setHorizontalHeaderLabels(
                {"Form", "\u00d7", "Wylie"});
            spellList_->verticalHeader()->setVisible(false);
            spellList_->setEditTriggers(
                QAbstractItemView::NoEditTriggers);
            spellList_->setSelectionBehavior(
                QAbstractItemView::SelectRows);
            spellList_->horizontalHeader()->setStretchLastSection(true);
            spv->addWidget(spellList_);
            auto* spRow = new QHBoxLayout;
            auto* spAll = new QPushButton("Check all");
            spAll->setToolTip(
                "Check every doubted form in the list at once.");
            auto* spFile = new QPushButton("File for ruling");
            spFile->setToolTip(
                "File the checked forms into the authority's Approval "
                "queue as potential input errors, with their context "
                "attached as evidence.");
            spRow->addWidget(spAll);
            spRow->addWidget(spFile);
            spv->addLayout(spRow);
            connect(spAll, &QPushButton::clicked, [this] {
                for (int r = 0; r < spellList_->rowCount(); ++r)
                    spellList_->item(r, 0)->setCheckState(Qt::Checked);
            });
            connect(spFile, &QPushButton::clicked,
                    [this] { fileCheckedSpellHits(); });
            connect(spellList_, &QTableWidget::cellClicked,
                    [this](int r, int) { jumpToSpellHit(r); });
        }
        ll->addWidget(spellPanel_);
        auto* teachBtn = new QPushButton("Teachings for this text\u2026");
        teachBtn->setToolTip(
            "Sweep every term this text contains and list where "
            "Geshe Michael taught each one \u2014 timecoded links "
            "into the recorded classes, both by his English and by "
            "the spoken Tibetan. Machine-located candidates; the "
            "recordings are the authority.");
        ll->addWidget(teachBtn);
        connect(teachBtn, &QPushButton::clicked, [this] {
            if (doc_.tokens.empty()) {
                QMessageBox::information(this, "Teachings",
                                         "Load a document first.");
                return;
            }
            const QString html = teachingsReportHtml();
            auto* dlg = new QDialog(this);
            dlg->setAttribute(Qt::WA_DeleteOnClose);
            dlg->setWindowTitle(
                "Teachings for this text \u2014 machine-located; "
                "the recordings are the authority");
            dlg->resize(760, 640);
            auto* v = new QVBoxLayout(dlg);
            auto* b = new QTextBrowser;
            b->setOpenExternalLinks(true);
            b->setHtml(html);
            v->addWidget(b);
            dlg->show();
        });
        connect(spellToggle_, &QCheckBox::toggled, [this](bool on) {
            spellPanel_->setVisible(on);
            if (on) rebuildSpellPanel();
        });
auto* secScan = new QLabel("<span style='color:#9A7A33;font-size:10px;letter-spacing:2px;font-weight:600'>SCANS</span>");
        secScan->setContentsMargins(0, 8, 0, 0);
        ll->addWidget(secScan);
                scanBtn_ = new QPushButton("Follow along in scans (BDRC)");
        scanBtn_->setEnabled(false);
        scanBtn_->setToolTip(
            "Opens the woodblock scans of this text from BDRC and keeps "
            "the page in step with the @folio marker at your cursor. "
            "Folio\u2194image mapping comes from BDRC's own IIIF manifest "
            "labels — nothing is guessed.");
        ll->addWidget(scanBtn_);
        connect(scanBtn_, &QPushButton::clicked, [this] {
            if (titleSearchMode_) titleSearchDialog();
            else followScans();
        });
        scanCap_ = new QLabel;
        scanCap_->setWordWrap(true);
        scanCap_->hide();
        ll->addWidget(scanCap_);
        scanImg_ = new QLabel;
        scanImg_->setScaledContents(false);
        scanImg_->setAlignment(Qt::AlignCenter);
        scanImg_->hide();
        ll->addWidget(scanImg_);
        auto* nav = new QWidget;
        auto* navl = new QHBoxLayout(nav);
        navl->setContentsMargins(0, 0, 0, 0);
        auto* prevB = new QPushButton("◀ folio");
        auto* nextB = new QPushButton("folio ▶");
        navl->addWidget(prevB);
        navl->addWidget(nextB);
#ifdef ALL_HAVE_OCR
        wordLocB_ = new QPushButton("locate word (OCR)");
        wordLocB_->setCheckable(true);
        wordLocB_->setToolTip(
            "Run OCR on this folio scan (BDRC models, CC BY-NC, with "
            "permission) and highlight the cursor's word on the "
            "woodblock itself. Deskew is disabled here so highlight "
            "coordinates stay in the original image (a labeled deviation "
            "from the BDRC pipeline). OCR output is used only to LOCATE "
            "— never as text.");
        navl->addWidget(wordLocB_);
        connect(wordLocB_, &QPushButton::toggled, [this](bool) {
            if (!basePx_.isNull()) setScanPixmap(basePx_, curFolio_);
        });
#endif
        scanNav_ = nav;
        scanNav_->hide();
        ll->addWidget(nav);
        connect(prevB, &QPushButton::clicked, [this] { stepFolio(-1); });
        connect(nextB, &QPushButton::clicked, [this] { stepFolio(+1); });
        connect(input_, &QPlainTextEdit::cursorPositionChanged,
                [this] { syncFolioToCursor(); });
        connect(open, &QPushButton::clicked, [this] {
            const QString fn = QFileDialog::getOpenFileName(
                this, "Open ACIP document", QString(),
                "ACIP/text files (*.txt *.acip *.act *.inc *.md);;All files (*)");
            if (fn.isEmpty()) return;
            QFile f(fn);
            if (f.open(QIODevice::ReadOnly)) {
                input_->setPlainText(QString::fromUtf8(f.readAll()));
                loadDoc();
                // ACIP file nomenclature: show the file's own provenance
                auto info = allcore::decodeAcipFilename(fn.toStdString());
                setScanTarget(info, fn);
                if (info.recognized)
                    hint_->setText(
                        QString("%1 · text %2%3%4 · %5 — %6")
                            .arg(QString::fromStdString(info.collection),
                                 QString::fromStdString(info.number),
                                 info.part.empty()
                                     ? ""
                                     : " part " + QString::fromStdString(
                                                      info.part),
                                 info.incomplete ? " · INCOMPLETE" : "",
                                 QString::fromStdString(info.language),
                                 QString::fromStdString(
                                     info.status.empty() ? "status unknown"
                                                         : info.status)) +
                        "\n" + hint_->text());
            }
        });
auto* illusVolBtn = new QPushButton("Illustration gallery (cached scans)");
        illusVolBtn->setToolTip(
            "Search this text's already-downloaded woodblock pages "
            "(the scan cache) for illustration candidates and show "
            "them as a gallery. Candidates only \u2014 pages you have "
            "not viewed are not searched.");
        ll->addWidget(illusVolBtn);
        connect(illusVolBtn, &QPushButton::clicked, [this, root] {
            if (scanCache_.isEmpty() || scanWork_.isEmpty()) {
                QMessageBox::information(
                    this, "No linked volume",
                    "Link a scan volume first (SCANS \u2192 Follow "
                    "along / Search BDRC).");
                return;
            }
            QStringList paths;
            for (const QString& f :
                 QDir(scanCache_).entryList(QDir::Files))
                if (f.contains(scanWork_)) paths << scanCache_ + "/" + f;
            if (paths.isEmpty()) {
                QMessageBox::information(
                    this, "No cached pages",
                    "No pages of this volume are in the scan cache "
                    "yet \u2014 pages cache as you view them in "
                    "Follow along.");
                return;
            }
            runIllustrationGallery(this, root, paths,
                                   scanWork_ + " cached pages");
        });
auto* secFmt = new QLabel("<span style='color:#9A7A33;font-size:10px;letter-spacing:2px;font-weight:600'>FORMAT &amp; EXPORT</span>");
        secFmt->setContentsMargins(0, 8, 0, 0);
        ll->addWidget(secFmt);
                auto* prepBtn = new QPushButton("Prepare for translation (Mixed Nuts)…");
        ll->addWidget(prepBtn);
        connect(prepBtn, &QPushButton::clicked, [this] {
            const std::string src = input_->toPlainText().toStdString();
            if (src.empty()) return;
            auto prep = allcore::formatForTranslation(src);
            std::string outText = prep.text;
            if (!prep.notes.empty()) {
                outText += "\n\n---- NOTES (input-operator brackets) ----\n";
                for (size_t i = 0; i < prep.notes.size(); ++i)
                    outText += "[" + std::to_string(i + 1) + "] " +
                               prep.notes[i] + "\n";
            }
            const QString fn = QFileDialog::getSaveFileName(
                this, "Save translation-prep text", "translation_prep.txt",
                "Text files (*.txt)");
            if (!fn.isEmpty()) {
                QFile f(fn);
                if (f.open(QIODevice::WriteOnly))
                    f.write(QByteArray::fromStdString(outText));
            }
            context_->setHtml(
                QString("<b>Translation prep (GMR workflow)</b>: %1 "
                        "paragraphs at double shads, %2 bracket note(s) "
                        "folio-tagged. Verse lineation + house style follow "
                        "the Mixed Nuts style guide (pending upload).")
                    .arg(prep.paragraphs)
                    .arg(prep.notes.size()));
        });
        auto* exportBtn = new QPushButton("Export print Tibetan (Unicode)…");
        ll->addWidget(exportBtn);
        connect(exportBtn, &QPushButton::clicked, [this] {
            const std::string src = input_->toPlainText().toStdString();
            if (src.empty()) return;
            auto res = allcore::exportTibetanUnicode(src);
            const QString fn = QFileDialog::getSaveFileName(
                this, "Save print Tibetan", "tibetan_unicode.txt",
                "Text files (*.txt)");
            if (!fn.isEmpty()) {
                QFile f(fn);
                if (f.open(QIODevice::WriteOnly))
                    f.write(QByteArray::fromStdString(res.unicode));
            }
            QString h = QString("<b>Print export</b>: %1 syllables, "
                                "%2 flagged ⟨…⟩ (never guessed).")
                            .arg(res.syllables)
                            .arg(res.failures);
            if (!res.failed.empty()) {
                h += "<br><small>flagged: ";
                int shown = 0;
                for (const auto& s : res.failed) {
                    if (shown++ >= 12) { h += "…"; break; }
                    h += QString::fromStdString(s).toHtmlEscaped() + " ";
                }
                h += "</small>";
            }
            h += "<div style='font-size:20px;margin-top:6px'>" +
                 QString::fromStdString(res.unicode)
                     .left(600)
                     .toHtmlEscaped()
                     .replace("\n", "<br>") +
                 "</div>";
            context_->setHtml(h);
        });
        // ---- display toggles: the reader chooses their information density;
        // choices persist across sessions ----
        QSettings settings("ALL", "TranslationTool");
        ll->addWidget(new QLabel("<b>Display</b>"));
        auto* scriptRow = new QHBoxLayout;
        scriptRow->addWidget(new QLabel("text as"));
        scriptMode_ = new QComboBox;
        scriptMode_->addItems({"Tibetan script", "ACIP", "Wylie"});
        scriptMode_->setCurrentIndex(
            // default: ACIP transliteration — the input centers'
            // native script (Adam, 2026-08-10); Tibetan script and
            // wylie stay one click away, remembered per user
            settings.value("overlay/scriptMode", 1).toInt());
        scriptRow->addWidget(scriptMode_, 1);
        auto* shadeRow = new QHBoxLayout;
        shadeRow->addWidget(new QLabel("shading"));
        shadeMode_ = new QComboBox;
        // GMR's directive (class of 2026-08-10, via Adam): the page
        // opens CLEAN; clicking finds and highlights the LONGEST
        // known phrase, backing off to shorter ones on each click.
        // The full always-shaded wash stays one click away as the
        // coverage/QC view.
        shadeMode_->addItems(
            {"on click (longest phrase first)", "all matches (wash)"});
        shadeMode_->setToolTip(
            "How dictionary matches show. ON CLICK (the default, per "
            "Geshe Michael's direction): the text opens clean; click "
            "any word and the longest phrase the dictionary knows "
            "there is highlighted \u2014 click again to step down to "
            "the shorter phrases inside it. ALL MATCHES: every known "
            "word and phrase is washed with color, nested and "
            "tier-tinted \u2014 the coverage view.");
        shadeMode_->setCurrentIndex(
            settings.value("overlay/shading", 0).toInt());
        shadeRow->addWidget(shadeMode_, 1);
        ll->addLayout(shadeRow);
        // Tibetan typeface for script mode: SambhotaDege first when the
        // system has it (Adam's preference), then the bundled OFL faces
        // (data/fonts/FONTS.md); persisted by family name, graceful if a
        // face goes missing
        tibFont_ = new QComboBox;
        {
            // curated Tibetan faces (fonts survey, data/fonts/FONTS.md):
            // Adam's preference first, then the vetted Unicode faces in
            // popularity order — only those actually present. Fake
            // Tibetan-declarers (Katari/Phosphate/Songti) and the legacy
            // custom-encoded TibetanMachineWeb family are excluded by
            // construction.
            static const char* kVetted[] = {
                "SambhotaDege",          // Adam's preferred (Sambhota Dege)
                "Noto Serif Tibetan",    // the modern default (bundled, OFL)
                "Jomolhari",             // classic dbu-can (bundled, OFL)
                "Monlam-Bodyig",         // community favorite (bundled, OFL)
                "TibetanMachineUnicode", // THL's scholarly standard
                "BabelStone Tibetan Slim", // widest rare-stack coverage (bundled)
                "TibetanClassicUnicode",
                "Kailasa",               // Apple system face
                "Kokonor",               // Apple system face
                "Microsoft Himalaya",    // the Windows default
                // style faces registered per-session from this machine's
                // font files (main(); never bundled) — display variety
                "TibetanChogyalUnicode",      // chos-rgyal
                "TibetanYigchung",            // yig-chung cursive
                "TibetanCalligraphicUnicode",
                "TCRC Youtso Unicode",
                "Tib-US Unicode",
            };
            QStringList opts;
            for (const char* f2 : kVetted)
                if (QFontDatabase::hasFamily(f2)) opts << f2;
            opts << "system";
            tibFont_->addItems(opts);
            const QString saved =
                settings.value("overlay/tibFontFamily", opts.first())
                    .toString();
            const int ix = tibFont_->findText(saved);
            if (ix >= 0) tibFont_->setCurrentIndex(ix);
        }
        scriptRow->addWidget(tibFont_);
        connect(tibFont_, &QComboBox::currentIndexChanged, [this](int) {
            QSettings s("ALL", "TranslationTool");
            s.setValue("overlay/tibFontFamily", tibFont_->currentText());
            if (!doc_.tokens.empty()) loadDoc();
        });
        ll->addLayout(scriptRow);
        auto mkToggle = [&](const char* key, const QString& label,
                            bool defOn) {
            auto* cb = new QCheckBox(label);
            cb->setChecked(settings.value(QString("overlay/") + key, defOn)
                               .toBool());
            ll->addWidget(cb);
            connect(cb, &QCheckBox::toggled, [this, key](bool on) {
                QSettings s("ALL", "TranslationTool");
                s.setValue(QString("overlay/") + key, on);
                if (!doc_.tokens.empty()) loadDoc();
            });
            return cb;
        };
        showPhon_ = mkToggle("phonetics", "phonetics", true);
        showGloss_ = mkToggle("glosses", "HGM definitions", true);
        showCorpus_ = mkToggle("corpus", "corpus usage (contextual)", true);
        showGrammar_ = mkToggle("grammar", "grammar marks && particle notes",
                                true);
        showSanskrit_ = mkToggle("sanskrit", "Sanskrit reference", false);
        showHopkins_ = mkToggle("hopkins", "Hopkins reference", false);
        showRefs_ = mkToggle("refs", "reference dictionaries (LC/TD/THL)",
                             true);
        showSeg_ = mkToggle("segmentation", "Botok segmentation (reference)",
                            false);
        showAttest_ = mkToggle("attestation",
                               "unattested-word hints (segmenter + Monlam)",
                               false);
        hint_ = new QLabel("Click a shaded word to see its context; click again "
                           "to cycle outward through the containing phrases.");
        hint_->setWordWrap(true);
        ll->addWidget(hint_);

        auto* right = new QSplitter(Qt::Vertical);
        // QPlainTextEdit: LAZY per-block layout — a full volume's
        // setPlainText was paying whole-document layout in QTextEdit
        // (~3s on 1.2MB; final hotspot of the Library-open fix)
        view_ = new QPlainTextEdit;
        view_->setReadOnly(true);
        // rapid clicks must WALK THE PHRASE CHAIN, never trigger
        // Qt's double-click word-select / triple-click line-select
        // (the whole line lit up after cycling — Adam, 2026-08-11)
        view_->viewport()->installEventFilter(this);
        { QFont vf = view_->font(); vf.setPointSize(15); view_->setFont(vf); }
        context_ = new QTextBrowser;
        context_->setOpenLinks(false);
        connect(context_, &QTextBrowser::anchorClicked,
                [this](const QUrl& u) {
                    const QString s = u.toString();
                    if (s.startsWith("das:")) {
                        showDasPage(this, s.mid(4).toInt());
                        return;
                    }
                    if (s.startsWith("gloss:")) {
                        const std::string wylie = s.mid(6).toStdString();
                        auto it = glossary_.find(wylie);
                        bool ok = false;
                        const QString g = QInputDialog::getText(
                            this, "Per-text glossary",
                            QString::fromStdString(wylie) + " =",
                            QLineEdit::Normal,
                            it == glossary_.end()
                                ? QString()
                                : QString::fromStdString(it->second),
                            &ok);
                        if (ok && !g.trimmed().isEmpty()) {
                            saveGlossaryEntry(wylie,
                                              g.trimmed().toStdString());
                            lastTok_ = -1;   // re-show the card fresh
                            cycle_ = 0;
                            onClick();
                        }
                    } else if (s.startsWith("propose:")) {
                        proposeFromCard(s.mid(8));
                    } else if (s.startsWith("http")) {
                        QDesktopServices::openUrl(u);
                    }
                });
        right->addWidget(view_);
        right->addWidget(context_);
        right->setStretchFactor(0, 3);
        right->setStretchFactor(1, 2);

        // the control column outgrew short windows — widgets were
        // being crushed (squashed input box, clipped headings and
        // folio buttons; Adam's screenshot, 2026-08-11). Scroll
        // instead of squeeze: nothing compresses below natural size.
        auto* leftScroll = new QScrollArea;
        leftScroll->setWidget(left);
        leftScroll->setWidgetResizable(true);
        leftScroll->setFrameShape(QFrame::NoFrame);
        leftScroll->setHorizontalScrollBarPolicy(
            Qt::ScrollBarAlwaysOff);
        split->addWidget(leftScroll);
        split->addWidget(right);
        split->setStretchFactor(0, 1);
        split->setStretchFactor(1, 3);
        outer->addWidget(split);

        connect(load, &QPushButton::clicked, [this] { loadDoc(); });
        connect(scriptMode_, &QComboBox::currentIndexChanged, [this](int m) {
            QSettings s("ALL", "TranslationTool");
            s.setValue("overlay/scriptMode", m);
            if (!doc_.tokens.empty()) loadDoc();
        });
        connect(shadeMode_, &QComboBox::currentIndexChanged, [this](int m) {
            QSettings s("ALL", "TranslationTool");
            s.setValue("overlay/shading", m);
            if (!doc_.tokens.empty()) loadDoc();
        });
        connect(view_, &QPlainTextEdit::cursorPositionChanged, [this] { onClick(); });
    }

private:
    // Phrase shading: adjacent top-level phrases cycle through soft
    // hues (so boundaries read by color, not by gaps), and nesting
    // darkens within the phrase's own hue. Index 0..4 = mint, powder
    // blue, lavender, peach, rose.
    static QColor washColor(int hue, int depth, bool provisional = false) {
        if (depth <= 0) return QColor(0, 0, 0, 0);
        static const QColor base[5] = {
            QColor(0xD9, 0xEF, 0xDA), QColor(0xD8, 0xE9, 0xF7),
            QColor(0xF1, 0xF0, 0xFB),   // the singles' whisper-neutral
            QColor(0xFA, 0xE8, 0xD8), QColor(0xF7, 0xE3, 0xEA)};
        QColor c = base[((hue % 5) + 5) % 5];
        if (depth == 2) c = c.darker(108);
        else if (depth >= 3) c = c.darker(118);
        if (provisional) {
            // provisional MUST look provisional (rule 2) — an amber
            // lean of the wash, tint not underline, so large volumes
            // format as merged runs instead of per-token ops
            c = QColor((c.red() * 2 + 0xE8) / 3,
                       (c.green() * 2 + 0xC9) / 3,
                       (c.blue() * 2 + 0x8A) / 3);
        }
        return c;
    }

    // token -> EWTS: ACIP documents convert; wylie documents are
    // already EWTS (docIsWylie_ set per load by the battery-covered
    // detector). Fixes lowercase-wylie files garbling in script mode.
    std::string tokEwts(const std::string& tok) const {
        return docIsWylie_ ? tok : allcore::acipToEwts(tok);
    }

    void loadDoc() {
        docIsWylie_ =
            allcore::looksLikeWylie(input_->toPlainText().toStdString());
        doc_ = allcore::buildOverlay(spine_, index_,
                                     input_->toPlainText().toStdString());
        tokBeg_.clear();
        tokEnd_.clear();
        const int mode = scriptMode_->currentIndex();   // 0 script, 1 ACIP, 2 wylie
        // Tibetan script renders in the bundled Noto Serif Tibetan
        // when available; ACIP/wylie stay in the default face
        {
            QFont f = view_->font();
            const QString want = tibFont_ ? tibFont_->currentText()
                                          : QStringLiteral("system");
            if (mode == 0 && want != "system" &&
                QFontDatabase::hasFamily(want))
                f.setFamilies({want});
            else
                f.setFamilies(QFontDatabase::systemFont(
                                  QFontDatabase::GeneralFont)
                                  .families());
            view_->setFont(f);
        }
        // Mirror the source's line structure (Adam, 2026-08-10): the
        // input box keeps the ACIP file's returns; the display follows
        // them, so the shading reads congruently with the source
        // instead of one endless run. Counted by tokenizing each
        // source line with the same tokenizer; if the counts do not
        // reconcile with the document's tokens, fall back to the
        // continuous layout rather than guessing line ends.
        std::vector<int> nlAfter(doc_.tokens.size(), 0);
        {
            const std::string raw = input_->toPlainText().toStdString();
            size_t tokIdx = 0, pos = 0;
            long prevLast = -1;
            int blanks = 0;
            while (pos <= raw.size()) {
                const size_t e = raw.find('\n', pos);
                const std::string line = raw.substr(
                    pos, e == std::string::npos ? std::string::npos
                                                : e - pos);
                std::vector<std::string> ltoks;
                std::vector<bool> lbar;
                allcore::tokenizeDocument(line, ltoks, lbar);
                if (ltoks.empty()) {
                    ++blanks;
                } else {
                    if (prevLast >= 0 &&
                        prevLast < (long)nlAfter.size())
                        nlAfter[prevLast] = 1 + blanks;
                    blanks = 0;
                    tokIdx += ltoks.size();
                    prevLast = (long)tokIdx - 1;
                }
                if (e == std::string::npos) break;
                pos = e + 1;
            }
            if (tokIdx != doc_.tokens.size())
                std::fill(nlAfter.begin(), nlAfter.end(), 0);
        }
        QString text;
        text.reserve((int)(input_->toPlainText().size() * 2));
        std::unordered_map<std::string, QString> dispCache;
        dispCache.reserve(4096);
        for (size_t i = 0; i < doc_.tokens.size(); ++i) {
            auto cached = dispCache.find(doc_.tokens[i]);
            QString disp;
            if (cached != dispCache.end()) {
                disp = cached->second;
            } else if (mode == 0) {
                // ACIP → EWTS → unicode through the battery-proven ports;
                // failures render as ⟨wylie⟩ markers, never guessed
                auto [u, ok] =
                    allcore::wylieToUnicode(tokEwts(doc_.tokens[i]));
                disp = QString::fromStdString(u);
                if (disp.isEmpty()) disp = QString::fromStdString(doc_.tokens[i]);
            } else if (mode == 2) {
                disp = QString::fromStdString(tokEwts(doc_.tokens[i]));
            } else {
                disp = QString::fromStdString(
                    docIsWylie_ ? allcore::ewtsToAcip(doc_.tokens[i])
                                : doc_.tokens[i]);
            }
            if (cached == dispCache.end())
                dispCache.emplace(doc_.tokens[i], disp);
            tokBeg_.push_back(text.size());
            text += disp;
            tokEnd_.push_back(text.size());
            if (mode == 0) {
                text += doc_.barrier_after[i] ? QString::fromUtf8("། ")
                                              : QString::fromUtf8("་");
            } else {
                if (doc_.barrier_after[i]) text += mode == 2 ? " /" : ",";
                text += " ";
            }
            if (nlAfter[i] > 0) {
                while (text.endsWith(' ')) text.chop(1);
                for (int k = 0; k < nlAfter[i]; ++k) text += '\n';
            }
        }
        loading_ = true;
        view_->setPlainText(text);
        auto depth = doc_.coverDepth(3);
        // assign each token its OUTERMOST covering span (longest), then
        // cycle the wash hue whenever that phrase changes — adjacent
        // phrases get different colors and read without gaps
        std::vector<int> outerSpan(doc_.tokens.size(), -1);
        std::vector<int> outerLen(doc_.tokens.size(), 0);
        // innermost (shortest) covering span + phrase membership,
        // precomputed in the same pass: the per-token spansAt() scans
        // here were O(tokens x spans) — the Library-open hang on full
        // volumes (2026-08-10, caught by stack sample)
        std::vector<int> innerSpan(doc_.tokens.size(), -1);
        std::vector<int> innerLen(doc_.tokens.size(), INT_MAX);
        std::vector<char> inPhrase_(doc_.tokens.size(), 0);
        for (int si = 0; si < (int)doc_.spans.size(); ++si) {
            const auto& sp = doc_.spans[si];
            const int len = sp.end - sp.beg;
            for (int t = sp.beg; t < sp.end && t < (int)outerSpan.size();
                 ++t) {
                if (len > outerLen[t]) { outerLen[t] = len; outerSpan[t] = si; }
                if (len < innerLen[t]) { innerLen[t] = len; innerSpan[t] = si; }
                if (len > 1) inPhrase_[t] = 1;
            }
        }
        // single-word matches keep one quiet neutral (lavender); the
        // cycling hues are reserved for MULTI-syllable phrases, so long
        // dictionary phrases pop as solid color runs against a calm
        // page instead of every word advancing the palette
        std::vector<int> tokHue(doc_.tokens.size(), 2);
        {
            static const int cycle[4] = {0, 1, 3, 4};
            int hue = -1, last = -2;
            for (size_t t = 0; t < outerSpan.size(); ++t) {
                if (outerLen[t] <= 1) continue;   // stays lavender
                if (outerSpan[t] != last) {
                    last = outerSpan[t];
                    ++hue;
                }
                tokHue[t] = cycle[hue % 4];
            }
        }
        int spellFlags = 0, agreeFlags = 0, attestFlags = 0;
        // word-level attestation pass (toggle): maximal runs of tokens the
        // dictionary lattice does NOT cover (and that are not grammatical
        // particles) are segmented by the proven segmenter over the
        // HGM + Monlam lexicon; only segmenter-OOV words are hinted —
        // never single syllables of words the lexicon does know.
        std::vector<bool> unattested(doc_.tokens.size(), false);
        if (showAttest_->isChecked()) {
            ensureSegmenter();
            if (segmenter_) {
                size_t i = 0;
                const size_t n = doc_.tokens.size();
                while (i < n) {
                    if (depth[i] > 0 ||
                        allcore::classifyParticle(doc_.tokens[i])) {
                        ++i;
                        continue;
                    }
                    // extend the uncovered run
                    size_t j = i;
                    std::vector<std::string> unis;
                    bool convOk = true;
                    while (j < n && depth[j] == 0 &&
                           !allcore::classifyParticle(doc_.tokens[j])) {
                        auto [u, ok] = allcore::wylieToUnicode(
                            tokEwts(doc_.tokens[j]));
                        if (!ok) { convOk = false; break; }
                        unis.push_back(u);
                        bool barrier = doc_.barrier_after[j];
                        ++j;
                        if (barrier) break;
                    }
                    if (!convOk || unis.empty()) { i = j + (convOk ? 0 : 1); continue; }
                    std::string run;
                    for (const auto& u : unis) {
                        if (!run.empty()) run += "་";
                        run += u;
                    }
                    // map segmenter words back to tokens by syllable count
                    size_t tokIx = i;
                    for (const auto& w : segmenter_->segment(run)) {
                        if (!w.tibetan) continue;
                        // syllables in this word = tsheg-separated pieces
                        int syls = 0;
                        bool inSyl = false;
                        for (const QChar qc : QString::fromStdString(w.text)) {
                            if (qc == QChar(0x0F0B) || qc == QChar(' ')) {
                                inSyl = false;
                            } else if (!inSyl) {
                                inSyl = true;
                                ++syls;
                            }
                        }
                        for (int s = 0; s < syls && tokIx < j; ++s, ++tokIx) {
                            if (!w.word) {
                                unattested[tokIx] = true;
                                ++attestFlags;
                            }
                        }
                    }
                    i = j;
                }
            }
        }
        QTextCursor cur(view_->document());
        // one edit block for the whole pass: ~10^5 per-token format
        // ops otherwise each trigger a relayout — minutes for a full
        // volume (the Library-open hang, 2026-08-10)
        view_->setUpdatesEnabled(false);
        cur.beginEditBlock();
        // Pass 1 — compute each token's full format key, then emit
        // MERGED RUNS (adjacent tokens + colored separators with an
        // identical format become one setCharFormat call): Qt pays
        // an engine invalidation per call, so runs — not tokens —
        // are what make full volumes open fast (2026-08-10).
        const size_t nTok = doc_.tokens.size();
        struct TokFmt { QRgb bg = 0; int ul = 0; QRgb ulc = 0; };
        std::vector<TokFmt> tf(nTok);
        std::unordered_map<std::string, bool> legalCache;
        legalCache.reserve(4096);
        for (size_t i = 0; i < nTok; ++i) {
            TokFmt f;
            const bool washAll =
                !shadeMode_ || shadeMode_->currentIndex() == 1;
            if (washAll && depth[i] > 0) {
                const bool prov =
                    innerSpan[i] >= 0 &&
                    doc_.entries[doc_.spans[innerSpan[i]].entry_ix]
                        .provisional();
                f.bg = washColor(tokHue[i], depth[i], prov).rgba();
            }
            if (showGrammar_->isChecked() && i > 0 &&
                !doc_.barrier_after[i - 1] && !inPhrase_[i]) {
                auto ag = allcore::checkAgreement(doc_.tokens[i - 1],
                                                  doc_.tokens[i]);
                if (ag.verdict == allcore::Agreement::Disagrees) {
                    f.ul = QTextCharFormat::DotLine;
                    f.ulc = qRgb(0xD9, 0x77, 0x06);
                    ++agreeFlags;
                }
            }
            if (unattested[i]) {
                f.ul = QTextCharFormat::DashDotLine;
                f.ulc = qRgb(0x5B, 0x7C, 0x99);
            }
            // spelling doubt only for tokens the dictionary does
            // NOT cover (a matched span attests the spelling as the
            // curated dictionary writes it — e.g. gdab, a gap in the
            // CC0 rule data) and never for @folio markers (structure,
            // not spelling). Adam's screenshot, 2026-08-11.
            if (checker_ && depth[i] == 0 &&
                !doc_.tokens[i].empty() && doc_.tokens[i][0] != '@') {
                auto lc = legalCache.find(doc_.tokens[i]);
                bool legal;
                if (lc != legalCache.end()) {
                    legal = lc->second;
                } else {
                    const std::string ew = tokEwts(doc_.tokens[i]);
                    legal = checker_->legalWylie(ew) ||
                            (g_spellingValid &&
                             g_spellingValid->count(ew) > 0);
                    legalCache.emplace(doc_.tokens[i], legal);
                }
                if (!legal) {
                    f.ul = QTextCharFormat::WaveUnderline;
                    f.ulc = qRgb(0xE2, 0x4B, 0x4A);
                    ++spellFlags;
                }
            }
            tf[i] = f;
        }
        auto sepColored = [&](size_t i) {
            // the tsheg after token i inherits its wash (no white
            // gaps inside phrases; never across a clause barrier)
            if (shadeMode_ && shadeMode_->currentIndex() == 0)
                return false;
            return depth[i] > 0 && i + 1 < nTok && depth[i + 1] > 0 &&
                   !doc_.barrier_after[i] && tokEnd_[i] < tokBeg_[i + 1];
        };
        struct Run { int beg; int end; QRgb bg; int ul; QRgb ulc; };
        std::vector<Run> runs;
        runs.reserve(4096);
        size_t i = 0;
        while (i < nTok) {
            const TokFmt f = tf[i];
            if (!f.bg && !f.ul) { ++i; continue; }
            size_t j = i;
            // extend the run while the next token has the identical
            // format AND the separator between them carries the same
            // background (colored sep needs f.bg; plain-gap merging
            // would paint the gap, so only merge across colored seps
            // or zero-width gaps)
            while (j + 1 < nTok && tf[j + 1].bg == f.bg &&
                   tf[j + 1].ul == f.ul && tf[j + 1].ulc == f.ulc &&
                   (tokEnd_[j] == tokBeg_[j + 1] ||
                    (sepColored(j) && f.bg && !f.ul)))
                ++j;
            const int endPos = sepColored(j) ? tokBeg_[j + 1]
                                             : tokEnd_[j];
            runs.push_back({tokBeg_[i], endPos, f.bg, f.ul, f.ulc});
            i = j + 1;
        }
        // progressive apply: the first slice lands before the text
        // appears (covers several screens); the rest is applied in
        // background slices so a full volume OPENS INSTANTLY instead
        // of stalling on ~10^5 format ops (2026-08-10)
        ++formatGen_;
        const int gen = formatGen_;
        auto applySlice = [this](size_t from, size_t to,
                                 const std::vector<Run>& rs) {
            QTextCursor c(view_->document());
            c.beginEditBlock();
            for (size_t k = from; k < to && k < rs.size(); ++k) {
                QTextCharFormat fmt;
                if (rs[k].bg)
                    fmt.setBackground(QColor::fromRgba(rs[k].bg));
                if (rs[k].ul) {
                    fmt.setUnderlineStyle(
                        QTextCharFormat::UnderlineStyle(rs[k].ul));
                    fmt.setUnderlineColor(QColor::fromRgb(rs[k].ulc));
                }
                c.setPosition(rs[k].beg);
                c.setPosition(rs[k].end, QTextCursor::KeepAnchor);
                c.setCharFormat(fmt);
            }
            c.endEditBlock();
        };
        const size_t kFirst = 4000, kSlice = 6000;
        applySlice(0, kFirst, runs);
        if (runs.size() > kFirst) {
            auto rest = std::make_shared<std::vector<Run>>(
                runs.begin() + kFirst, runs.end());
            auto cont = std::make_shared<std::function<void(size_t)>>();
            *cont = [this, gen, rest, applySlice, cont,
                     kSlice](size_t at) {
                if (gen != formatGen_) return;   // doc reloaded
                applySlice(at, at + kSlice, *rest);
                if (at + kSlice < rest->size())
                    QTimer::singleShot(0, view_, [cont, at, kSlice] {
                        (*cont)(at + kSlice);
                    });
            };
            QTimer::singleShot(0, view_, [cont] { (*cont)(0); });
        }
        cur.endEditBlock();
        view_->setUpdatesEnabled(true);
        loading_ = false;
        lastTok_ = -1;
        QString attestNote =
            showAttest_->isChecked()
                ? QString(" · %1 unattested-word hint(s)").arg(attestFlags)
                : QString();
        if (!doc_.tokens.empty() && doc_.spans.empty())
            hint_->setText(QString("%1 tokens, but NO dictionary "
                                   "matches — is this Tibetan "
                                   "transliteration? (English or "
                                   "unknown text shows no shading)")
                               .arg(doc_.tokens.size()));
        else
        hint_->setText(QString("%1 tokens · %2 spans · %3 entries · %4 spelling "
                               "flag(s) · %5 particle-agreement flag(s)%6 — click "
                               "any shaded word.")
                           .arg(doc_.tokens.size())
                           .arg(doc_.spans.size())
                           .arg(doc_.entries.size())
                           .arg(spellFlags)
                           .arg(agreeFlags)
                           .arg(attestNote));
        context_->clear();
        if (spellToggle_ && spellToggle_->isChecked())
            rebuildSpellPanel();
    }

    // The spelling-doubts layer: the same legality check that draws
    // the red waves, but as a working LIST — one row per distinct
    // doubted form with count, occurrences, and first context. Forms
    // the authority has ruled valid (declined flags) are excluded.
    struct SpellHit {
        QString form, ewts, context;
        int count = 0;
        std::vector<int> toks;   // token indices of every occurrence
    };
    std::vector<SpellHit> collectSpellHits() const {
        std::vector<SpellHit> out;
        if (!checker_) return out;
        const auto depth = doc_.coverDepth(1);
        std::map<std::string, size_t> ix;
        for (size_t i = 0; i < doc_.tokens.size(); ++i) {
            const std::string& t = doc_.tokens[i];
            if (!t.empty() && t[0] == '@') continue;   // folio marker
            if (i < depth.size() && depth[i] > 0)
                continue;   // dictionary-attested spelling
            bool alpha = false;
            for (char ch : t)
                alpha |= std::isalpha((unsigned char)ch) != 0;
            if (!alpha) continue;
            const std::string ew = tokEwts(t);
            if (checker_->legalWylie(ew)) continue;
            if (g_spellingValid && g_spellingValid->count(ew)) continue;
            auto f = ix.find(t);
            if (f != ix.end()) {
                out[f->second].count++;
                out[f->second].toks.push_back((int)i);
                continue;
            }
            QString ctx;
            const size_t b = i >= 3 ? i - 3 : 0;
            const size_t e = std::min(doc_.tokens.size(), i + 4);
            for (size_t j = b; j < e; ++j) {
                if (!ctx.isEmpty()) ctx += ' ';
                ctx += (j == i)
                    ? "[" + QString::fromStdString(doc_.tokens[j]) + "]"
                    : QString::fromStdString(doc_.tokens[j]);
            }
            ix.emplace(t, out.size());
            out.push_back({QString::fromStdString(t),
                           QString::fromStdString(ew), ctx, 1,
                           {(int)i}});
        }
        return out;
    }

    void rebuildSpellPanel() {
        spellHits_ = collectSpellHits();
        spellCycle_.assign(spellHits_.size(), 0);
        spellList_->setRowCount((int)spellHits_.size());
        for (int r = 0; r < (int)spellHits_.size(); ++r) {
            auto* c0 = new QTableWidgetItem(spellHits_[r].form);
            c0->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable |
                         Qt::ItemIsUserCheckable);
            c0->setCheckState(Qt::Unchecked);
            c0->setToolTip(spellHits_[r].context);
            spellList_->setItem(r, 0, c0);
            spellList_->setItem(
                r, 1,
                new QTableWidgetItem(
                    QString::number(spellHits_[r].count)));
            auto* c2 = new QTableWidgetItem(spellHits_[r].ewts);
            c2->setToolTip(spellHits_[r].context);
            spellList_->setItem(r, 2, c2);
        }
        spellList_->resizeColumnsToContents();
    }

    void jumpToSpellHit(int row) {
        // click a row → the text scrolls to the doubted syllable,
        // selected; click again → the next occurrence, cycling
        if (row < 0 || row >= (int)spellHits_.size()) return;
        const auto& toks = spellHits_[row].toks;
        if (toks.empty()) return;
        int& cyc = spellCycle_[row];
        const int tok = toks[cyc % (int)toks.size()];
        ++cyc;
        if (tok < 0 || tok >= (int)tokBeg_.size()) return;
        QTextCursor c(view_->document());
        c.setPosition(tokBeg_[tok]);
        c.setPosition(tokEnd_[tok], QTextCursor::KeepAnchor);
        view_->setTextCursor(c);
        view_->ensureCursorVisible();
    }

    void fileCheckedSpellHits() {
        QList<QStringList> rows;
        const QString base = docFile_.isEmpty()
            ? QString("pasted text")
            : QFileInfo(docFile_).fileName();
        for (int r = 0; r < spellList_->rowCount() &&
                        r < (int)spellHits_.size(); ++r)
            if (spellList_->item(r, 0)->checkState() == Qt::Checked)
                rows.push_back(
                    {spellHits_[r].ewts,
                     QString("possible input error \u2014 fails "
                             "syllable legality (seen %1\u00d7 as "
                             "\u201c%2\u201d)")
                         .arg(spellHits_[r].count)
                         .arg(spellHits_[r].form),
                     base, spellHits_[r].context});
        if (rows.isEmpty()) {
            QMessageBox::information(
                this, "Nothing checked",
                "Check at least one doubted form first (the checkbox "
                "on its row).");
            return;
        }
        fileSpellingProposals(this, rows);
    }

    QCheckBox* spellToggle_ = nullptr;
    QWidget* spellPanel_ = nullptr;
    QTableWidget* spellList_ = nullptr;
    std::vector<SpellHit> spellHits_;
    std::vector<int> spellCycle_;

    // #64 stage D (v1): every matched entry in the loaded text with
    // its teaching moments, in text order — the translator's
    // companion sheet. Context-fit ranking is the next increment.
    // #64 stage D (v2): every matched entry in the loaded text with
    // its teaching moments — moments RANKED BY CONTEXT FIT: a moment
    // whose snippet mentions other terms of this same text outranks
    // one that merely uses the word. Machine-located candidates; the
    // recordings are the authority.
    QString teachingsReportHtml() const {
        // the text's own vocabulary, as normalized gloss keys
        std::set<std::string> docKeys;
        for (const auto& e : doc_.entries)
            for (const auto& g : e.hgm_gloss)
                docKeys.insert(teachingKey(g));
        auto contextScore = [&docKeys](const QString& snippet,
                                       const std::string& self) {
            const std::string low = snippet.toLower().toStdString();
            int score = 0;
            for (const auto& k : docKeys)
                if (k != self && k.size() >= 5 &&
                    low.find(k) != std::string::npos)
                    ++score;
            return score;
        };
        QString h =
            "<i style='color:#888'>Candidates located from class "
            "captions, ranked by context fit (moments mentioning "
            "other terms of this text rank first); click a link and "
            "the recording itself teaches.</i><hr>";
        int shown = 0;
        std::set<std::string> done;
        for (const auto& e : doc_.entries) {
            if (done.count(e.wylie)) continue;
            done.insert(e.wylie);
            QString sec;
            if (g_teaching) {
                std::vector<const TeachingMoment*> cand;
                std::set<QString> seen;
                std::string selfKey;
                for (const auto& g : e.hgm_gloss) {
                    const std::string k = teachingKey(g);
                    if (selfKey.empty()) selfKey = k;
                    auto it = g_teaching->find(k);
                    if (it == g_teaching->end()) continue;
                    for (const auto& m : it->second)
                        if (!seen.count(m.url)) {
                            seen.insert(m.url);
                            cand.push_back(&m);
                        }
                }
                std::stable_sort(
                    cand.begin(), cand.end(),
                    [&](const TeachingMoment* a,
                        const TeachingMoment* b) {
                        return contextScore(a->snippet, selfKey) >
                               contextScore(b->snippet, selfKey);
                    });
                int n = 0;
                for (const auto* mp : cand) {
                    if (++n > 2) break;
                    const auto& m = *mp;
                    sec += QString(
                               "&nbsp;&nbsp;\u25B6 <a href='%1'>"
                               "%2</a> @%3:%4%5<br>")
                               .arg(m.url,
                                    m.title.left(56).toHtmlEscaped())
                               .arg(m.t / 60)
                               .arg(m.t % 60, 2, 10, QChar('0'))
                               .arg((m.lang == "ENG" || m.lang == "?"
                                         ? QString()
                                         : " [" + m.lang + "]") +
                                    (m.src == "TKB" ? " (lam rim)"
                                                    : QString()));
                }
            }
            if (g_teachingTib) {
                auto it = g_teachingTib->find(e.wylie);
                if (it != g_teachingTib->end() &&
                    !it->second.empty()) {
                    const auto& m = it->second.front();
                    sec += QString("&nbsp;&nbsp;\u25B6 <a href='%1'>"
                                   "%2</a> @%3:%4 <i style="
                                   "'color:#888'>(he says the "
                                   "word)</i><br>")
                               .arg(m.url,
                                    m.title.left(56).toHtmlEscaped())
                               .arg(m.t / 60)
                               .arg(m.t % 60, 2, 10, QChar('0'));
                }
            }
            if (sec.isEmpty()) continue;
            QString gloss;
            if (!e.hgm_gloss.empty())
                gloss = " \u2014 " +
                        QString::fromStdString(e.hgm_gloss.front())
                            .toHtmlEscaped();
            h += "<b>" +
                 QString::fromStdString(e.wylie).toHtmlEscaped() +
                 "</b><small style='color:#555'>" + gloss +
                 "</small><br>" + sec;
            if (++shown >= 300) {
                h += "<i>\u2026 report capped at 300 terms</i>";
                break;
            }
        }
        if (!shown)
            h += "<i>No teaching moments found for this text's "
                 "vocabulary (yet \u2014 the corpus grows as more "
                 "classes are indexed).</i>";
        return h;
    }

    bool eventFilter(QObject* w, QEvent* ev) override {
        if (view_ && w == view_->viewport() &&
            ev->type() == QEvent::MouseButtonDblClick) {
            onClick();      // count it as one more step of the chain
            return true;    // and swallow the auto-selection
        }
        return QWidget::eventFilter(w, ev);
    }

    int tokenAt(int charPos) const {
        // binary search — this runs on every click/cursor move, and a
        // linear scan over 300k tokens made interaction sluggish
        auto it = std::upper_bound(tokBeg_.begin(), tokBeg_.end(),
                                   charPos);
        if (it == tokBeg_.begin()) return -1;
        const size_t i = (it - tokBeg_.begin()) - 1;
        return (charPos <= tokEnd_[i]) ? (int)i : -1;
    }

    void onClick() {
        if (loading_ || doc_.tokens.empty()) return;
        const int tok = tokenAt(view_->textCursor().position());
        if (tok < 0) return;
        auto at = doc_.spansAt(tok);
        // suffix-agreement line for the clicked token (Wilson sandhi tables)
        auto agreementHtml = [&](int t) -> QString {
            if (t <= 0 || doc_.barrier_after[t - 1]) return {};
            auto ag = allcore::checkAgreement(doc_.tokens[t - 1], doc_.tokens[t]);
            using A = allcore::Agreement;
            if (ag.verdict == A::NotAParticleVariant) return {};
            const QString prev =
                QString::fromStdString(doc_.tokens[t - 1]).toHtmlEscaped();
            const QString exp =
                QString::fromStdString(ag.expected).toHtmlEscaped();
            if (ag.verdict == A::Agrees)
                return "<div style='color:#3B7A3B;font-size:12px'>suffix "
                       "agreement: agrees with preceding " + prev + "</div>";
            if (ag.verdict == A::DaDragPossible)
                return "<div style='color:#7A5A00;font-size:12px'>suffix "
                       "agreement after " + prev + ": tables call for <b>" +
                       exp + "</b> — " +
                       QString::fromUtf8(ag.note).toHtmlEscaped() + "</div>";
            return "<div style='color:#B4540A;font-size:12px'>suffix agreement: "
                   "after " + prev + " the tables call for <b>" + exp +
                   "</b> — flagged for review</div>";
        };
        if (at.empty()) {
            // no dictionary span — but it may be a freestanding grammatical
            // particle (Wilson rule tables)
            if (const auto* p = showGrammar_->isChecked()
                                    ? allcore::classifyParticle(doc_.tokens[tok])
                                    : nullptr) {
                context_->setHtml(
                    segmentationHtml(tok) +
                    QString("<b>%1</b> — grammatical particle<br>%2<br>"
                            "<small style='color:#555'>paradigm: %3</small>")
                        .arg(QString::fromUtf8(p->wylie).toHtmlEscaped(),
                             QString::fromUtf8(p->function).toHtmlEscaped(),
                             QString::fromUtf8(p->group).toHtmlEscaped()) +
                    agreementHtml(tok));
            } else {
                QString extra;
                const std::string tokWylie = tokEwts(doc_.tokens[tok]);
                auto [u1, ok1] = allcore::wylieToUnicode(tokWylie);
                if (ok1) {
                    loadLexicon();
                    const auto att = lexicon_.attested(u1);
                    if (!att.empty())
                        extra = QString(
                                    "<br><small style='color:#1E6B4E'>"
                                    "not in the HGM dictionary, but "
                                    "attested in the %1 word list "
                                    "<i>(Apache-2.0, reference)</i>"
                                    "</small>")
                                    .arg(QString::fromStdString(att)
                                             .toHtmlEscaped());
                }
                // bskungs yig: a known orthographic abbreviation — the
                // companion to the red wave (legal abbreviation, not a typo)
                if (!abbrTried_) {
                    abbrTried_ = true;
                    abbr_.load((dataRoot_ +
                                "/data/abbreviations/tibschol_abbr.csv")
                                   .toStdString());
                }
                auto hits = abbr_.byWylie(tokWylie);
                if (hits.empty() && ok1) hits = abbr_.byUnicode(u1);
                for (const auto* a : hits)
                    extra += QString(
                                 "<br><small style='color:#7A3B5E'>"
                                 "orthographic abbreviation (bskungs yig): "
                                 "<b>%1</b> = %2 <span style='font-size:15px'>"
                                 "%3</span> <i>(TibSchol table / rKTs, CC0 — "
                                 "reference; not a typo)</i></small>")
                                 .arg(QString::fromStdString(a->abbrWylie)
                                          .toHtmlEscaped(),
                                      QString::fromStdString(a->expWylie)
                                          .toHtmlEscaped(),
                                      QString::fromStdString(a->expUnicode)
                                          .toHtmlEscaped());
                context_->setHtml(segmentationHtml(tok) +
                                  glossaryHtml(tokWylie) +
                                  "<i>no dictionary span here</i>" + extra);
            }
            return;
        }
        const bool clickMode =
            shadeMode_ && shadeMode_->currentIndex() == 0;
        if (clickMode)
            std::reverse(at.begin(), at.end());   // longest first,
                                                  // backing off shorter
        if (tok == lastTok_) cycle_ = (cycle_ + 1) % (int)at.size();
        else cycle_ = 0;
        lastTok_ = tok;
        const auto& span = doc_.spans[at[cycle_]];
        const auto& e = doc_.entries[span.entry_ix];
        // the learner met this word: it enters (or bumps) their SRS deck
        if (progress_) progress_->touchWord(e.wylie, (long long)time(nullptr));

        // active-span highlight via extra selections (never touches stored formats)
        QTextEdit::ExtraSelection sel;
        sel.cursor = QTextCursor(view_->document());
        sel.cursor.setPosition(tokBeg_[span.beg]);
        sel.cursor.setPosition(tokEnd_[span.end - 1], QTextCursor::KeepAnchor);
        if (clickMode)
            sel.format.setBackground(
                e.provisional() ? QColor(0xE8, 0xC9, 0x8A, 190)
                                : QColor(0xE9, 0xC4, 0x6A, 170));
        else
            sel.format.setBackground(QColor(0x7F, 0x77, 0xDD, 110));
        view_->setExtraSelections({sel});

        // breadcrumb of the nesting chain + entry + corpus count
        QString h = segmentationHtml(tok);
        h += "<div style='color:#555;font-size:12px'>nesting: ";
        for (size_t i = 0; i < at.size(); ++i) {
            const auto& s = doc_.spans[at[i]];
            QString w = QString::fromStdString(doc_.entries[s.entry_ix].wylie).toHtmlEscaped();
            h += (i ? " ‹ " : "");
            h += (int(i) == cycle_ ? "<b>" + w + "</b>" : w);
        }
        h += QString(" &nbsp;(click again to %1)</div><hr>")
                 .arg(clickMode ? "step down to the shorter phrase"
                                : "cycle");
        // the per-text glossary outranks the general dictionary here
        h += glossaryHtml(e.wylie);
        EntryDisplay disp;
        disp.phonetics = showPhon_->isChecked();
        disp.glosses = showGloss_->isChecked();
        disp.sanskrit = showSanskrit_->isChecked();
        disp.hopkins = showHopkins_->isChecked();
        h += entryHtml(e, disp);
        if (showGrammar_->isChecked() && !span.clitic.empty()) {
            // the span matched through a fused ending the Wilson layer split off
            h += QString("<div style='color:#7A5A00;font-size:12px'>fused "
                         "particle split off: <b>%1</b> — %2</div>")
                     .arg(QString::fromStdString(span.clitic).toHtmlEscaped(),
                          QString::fromUtf8(
                              allcore::fusedEndingFunction(span.clitic))
                              .toHtmlEscaped());
        }
        if (showGrammar_->isChecked()) h += agreementHtml(tok);
        // Monlam word-list attestation: labeled reference, display only
        {
            loadLexicon();
            auto [uw, okw] = allcore::wylieToUnicode(e.wylie);
            const auto att = okw ? lexicon_.attested(uw) : std::string();
            if (!att.empty())
                h += QString("<div style='color:#1E6B4E;font-size:12px'>"
                             "attested in the %1 word list <i>(Apache-"
                             "2.0, reference)</i></div>")
                         .arg(QString::fromStdString(att)
                                  .toHtmlEscaped());
        }
        // bsdus tshig: a registered syllabic contraction — expansion shown
        // with its derived tier (the OM-dot register, Adam 2026-08-07)
        if (showGrammar_->isChecked()) {
            if (!contrTried_) {
                contrTried_ = true;
                contr_.load((dataRoot_ + "/data/extracted/bsdus_tshig.tsv")
                                .toStdString());
            }
            for (const auto* c : contr_.expansions(e.wylie))
                h += QString("<div style='color:#7A3B5E;font-size:12px'>"
                             "bsdus tshig <i>(derived register, %1/%2)</i>: "
                             "contraction of <b>%3</b> — dropped %4; shared "
                             "gloss “%5”</div>")
                         .arg(QString::fromStdString(c->cls).toHtmlEscaped(),
                              QString::fromStdString(c->glossKind)
                                  .toHtmlEscaped(),
                              QString::fromStdString(c->longWylie)
                                  .toHtmlEscaped(),
                              QString::fromStdString(c->dropped).toHtmlEscaped(),
                              QString::fromStdString(c->gloss).toHtmlEscaped());
        }
        // CC0 verbs-database: labeled reference comparanda, display only
        if (showGrammar_->isChecked()) {
            if (!verbStemsTried_) {
                verbStemsTried_ = true;
                verbStems_.load((dataRoot_ +
                                 "/data/extracted/verb_stems.tsv")
                                    .toStdString());
            }
            const auto& tokTxt = doc_.tokens[tok];
            auto [uni, okc] = allcore::wylieToUnicode(tokEwts(tokTxt));
            if (okc) {
                const auto hits = verbStems_.lookup(uni);
                if (!hits.empty()) {
                    QString vh =
                        "<div style='margin-top:6px;color:#1E6B4E;"
                        "font-size:12px'><b>Verbs DB</b> "
                        "<small>(tibetan-nlp, CC0 — reference "
                        "only)</small>: ";
                    for (size_t i = 0; i < hits.size() && i < 3; ++i) {
                        if (i) vh += " · ";
                        vh += QString::fromStdString(hits[i].roles)
                                  .toHtmlEscaped() +
                              " stem of " +
                              QString::fromStdString(hits[i].paradigm)
                                  .toHtmlEscaped() +
                              (hits[i].sources.empty()
                                   ? QString()
                                   : " [" +
                                         QString::fromStdString(
                                             hits[i].sources)
                                             .toHtmlEscaped() +
                                         "]");
                    }
                    h += vh + "</div>";
                }
            }
        }
        auto segs = showCorpus_->isChecked()
                        ? spine_.corpusSearch('"' + e.wylie + '"', "", 3)
                        : std::vector<allcore::CorpusSegment>{};
        if (!segs.empty()) {
            h += "<div style='margin-top:8px'><b>From the corpus:</b></div>";
            for (const auto& s : segs) {
                h += "<div style='margin:6px 0'><small>[" +
                     QString::fromStdString(s.course) + ":" + QString::number(s.seq) +
                     "]</small><br><i>" +
                     QString::fromStdString(s.wylie).left(180).toHtmlEscaped() +
                     "</i><br>" +
                     QString::fromStdString(s.english).left(220).toHtmlEscaped() +
                     "</div>";
            }
        }
        if (ref_ && showRefs_->isChecked()) {
            auto refs = ref_->lookup(e.wylie, 6);
            if (!refs.empty()) {
                h += "<div style='margin-top:8px;color:#993C1D'><b>Reference</b> "
                     "<small>(local only)</small></div>";
                for (const auto& r : refs)
                    h += "<div style='margin:4px 0'><span style='background:#FAEEDA;"
                         "color:#633806;padding:0 6px;border-radius:8px;font-size:11px'>" +
                         QString::fromStdString(r.layer).toHtmlEscaped() + "</span> " +
                         QString::fromStdString(r.definition).left(220).toHtmlEscaped() +
                         "</div>";
            }
        }
        context_->setHtml(h);
    }

    allcore::Spine& spine_;
    allcore::SyllableChecker* checker_ = nullptr;
    allcore::RefDict* ref_ = nullptr;
    allcore::Progress* progress_ = nullptr;
    allcore::HeadwordIndex index_;
    allcore::OverlayDoc doc_;
    std::vector<int> tokBeg_, tokEnd_;
    QPlainTextEdit* input_ = nullptr;
    QPlainTextEdit* view_ = nullptr;
    QTextBrowser* context_ = nullptr;
    // ---- BDRC scan follow-along (APPARATUS_DESIGN §3) ----------------
    void setScanTarget(const allcore::AcipFileInfo& info,
                       const QString& fileName = QString()) {
        ++fetchEpoch_;   // invalidates any in-flight scan fetches
        scanWork_.clear();
        fileKey_ = QFileInfo(fileName).fileName();
        folioUrl_.clear();
        folioOrder_.clear();
        curFolio_.clear();
        scanImg_->hide(); scanCap_->hide(); scanNav_->hide();
        const std::string url = allcore::bdrcScanUrl(info);
        auto pos = url.rfind("bdr:");
        if (pos != std::string::npos)
            scanWork_ = QString::fromStdString(url.substr(pos));
        // no deterministic mapping (Sungbum etc.): use a saved,
        // user-confirmed link if one exists
        if (scanWork_.isEmpty() && !fileKey_.isEmpty()) {
            QFile f(linksFile());
            if (f.open(QIODevice::ReadOnly)) {
                const auto o =
                    QJsonDocument::fromJson(f.readAll()).object();
                scanWork_ = o[fileKey_].toString();
            }
        }
        titleSearchMode_ = scanWork_.isEmpty();
        scanBtn_->setEnabled(titleSearchMode_ ? !fileKey_.isEmpty() : true);
        scanBtn_->setText(
            titleSearchMode_
                ? "Find scans on BDRC (title search)…"
                : "Follow along in scans (" + scanWork_ + ")");
    }

    QString linksFile() const {
        return scanCache_.isEmpty()
                   ? QString()
                   : QFileInfo(scanCache_).path() + "/bdrc_links.json";
    }

    // pre-fill for the title query: the text's own opening line,
    // through the canonical ACIP→EWTS converter (transliteration rule)
    QString deriveTitleQuery() const {
        QString head = input_->toPlainText().left(400);
        head.replace(QRegularExpression("@[A-Za-z0-9]+"), " ");
        head.replace(QRegularExpression("\\[[^\\]]*\\]"), " ");
        head.replace(QRegularExpression("[*,;/]"), " ");
        head = head.simplified();
        QStringList syl = head.split(' ', Qt::SkipEmptyParts);
        QStringList take;
        for (const auto& t : syl) {
            if (t.compare("BZHUGS", Qt::CaseInsensitive) == 0) break;
            take << t;
            if (take.size() >= 12) break;
        }
        return QString::fromStdString(
                   allcore::acipToEwts(take.join(' ').toStdString()))
            .trimmed();
    }

    void titleSearchDialog() {
        QDialog dlg(this);
        dlg.setWindowTitle("Find scans on BDRC by title");
        auto* v = new QVBoxLayout(&dlg);
        v->addWidget(new QLabel(
            "No catalog mapping exists for this file, so the scans must "
            "be found by TITLE SEARCH and confirmed by you — the match "
            "is never asserted automatically. Scans follow the edition "
            "you choose; its folio numbers are that edition's own."));
        auto* q = new QLineEdit(deriveTitleQuery());
        v->addWidget(q);
        auto* go = new QPushButton("Search BDRC");
        v->addWidget(go);
        auto* list = new QListWidget;
        v->addWidget(list, 1);
        auto* use = new QPushButton("Use selected (saves the link)");
        use->setEnabled(false);
        v->addWidget(use);
        QObject::connect(list, &QListWidget::itemSelectionChanged,
                         [&] { use->setEnabled(!list->selectedItems()
                                                    .isEmpty()); });
        auto runSearch = [&] {
            list->clear();
            list->addItem("searching…");
            const QString url =
                "https://purl.bdrc.io/query/table/BLMP?L_NAME=%22" +
                QString(QUrl::toPercentEncoding(q->text().trimmed())) +
                "%22&LG_NAME=bo-x-ewts&I_LIM=20&format=json";
            auto* rep = nam_.get(QNetworkRequest(QUrl(url)));
            QObject::connect(rep, &QNetworkReply::finished, &dlg, [&, rep] {
                rep->deleteLater();
                list->clear();
                if (rep->error() != QNetworkReply::NoError) {
                    list->addItem("BDRC unreachable: " +
                                  rep->errorString());
                    return;
                }
                const auto d =
                    QJsonDocument::fromJson(rep->readAll()).object();
                const auto rows = d["results"]
                                      .toObject()["bindings"]
                                      .toArray();
                for (const auto& rv : rows) {
                    const auto r = rv.toObject();
                    const QString id = r["s"]
                                           .toObject()["value"]
                                           .toString()
                                           .section('/', -1);
                    if (!id.startsWith("MW")) continue;
                    const QString lit =
                        r["lit"].toObject()["value"].toString();
                    auto* it = new QListWidgetItem(
                        lit + "   [bdr:" + id + "]");
                    it->setData(Qt::UserRole, "bdr:" + id);
                    list->addItem(it);
                }
                if (!list->count())
                    list->addItem("no instances matched — edit the "
                                  "title and search again");
            });
        };
        QObject::connect(go, &QPushButton::clicked, runSearch);
        QObject::connect(q, &QLineEdit::returnPressed, runSearch);
        QObject::connect(use, &QPushButton::clicked, &dlg,
                         &QDialog::accept);
        runSearch();
        if (dlg.exec() != QDialog::Accepted ||
            list->selectedItems().isEmpty())
            return;
        scanWork_ =
            list->selectedItems().first()->data(Qt::UserRole).toString();
        if (scanWork_.isEmpty()) return;
        // persist the user-confirmed link
        QFile f(linksFile());
        QJsonObject o;
        if (f.open(QIODevice::ReadOnly)) {
            o = QJsonDocument::fromJson(f.readAll()).object();
            f.close();
        }
        o[fileKey_] = scanWork_;
        QDir().mkpath(QFileInfo(linksFile()).path());
        if (f.open(QIODevice::WriteOnly))
            f.write(QJsonDocument(o).toJson());
        titleSearchMode_ = false;
        scanBtn_->setText("Follow along in scans (" + scanWork_ + ")");
        followScans();
    }

    void followScans() {
        if (scanWork_.isEmpty()) return;
        scanCap_->setText("<i>fetching the scan outline from BDRC…</i>");
        scanCap_->show();
        const QUrl u("https://iiifpres.bdrc.io/collection/wio:" + scanWork_);
        auto* rep = nam_.get(QNetworkRequest(u));
        const int epoch = fetchEpoch_;
        connect(rep, &QNetworkReply::finished, [this, rep, epoch] {
            rep->deleteLater();
            if (epoch != fetchEpoch_) return;   // a newer file took over
            if (rep->error() != QNetworkReply::NoError) {
                scanCap_->setText("BDRC unreachable: " + rep->errorString() +
                                  " — <a href='https://library.bdrc.io/show/" +
                                  scanWork_ + "'>open in the browser</a>");
                scanCap_->setOpenExternalLinks(true);
                return;
            }
            const auto o = QJsonDocument::fromJson(rep->readAll()).object();
            QString lic = o["license"].toString();
            scanLicense_ = lic;
            QStringList manifests;
            for (const auto& m : o["manifests"].toArray())
                manifests << m.toObject()["@id"].toString();
            if (manifests.isEmpty()) {
                scanCap_->setText("BDRC lists no scan volumes for " +
                                  scanWork_);
                return;
            }
            pendingManifests_ = manifests;
            fetchNextManifest();
        });
    }

    void fetchNextManifest() {
        if (pendingManifests_.isEmpty()) {
            scanCap_->setText(
                QString("Scans: Buddhist Digital Resource Center · %1 "
                        "folio sides mapped%2. Move the cursor in the "
                        "document — the page follows its @folio marker.")
                    .arg(folioOrder_.size())
                    .arg(scanLicense_.contains("publicdomain")
                             ? " · public domain"
                             : ""));
            scanNav_->show();
            syncFolioToCursor();
            if (curFolio_.isEmpty() && !folioOrder_.isEmpty())
                showFolio(folioOrder_.front());
            return;
        }
        const QUrl u(pendingManifests_.takeFirst());
        auto* rep = nam_.get(QNetworkRequest(u));
        const int epoch = fetchEpoch_;
        connect(rep, &QNetworkReply::finished, [this, rep, epoch] {
            rep->deleteLater();
            if (epoch != fetchEpoch_) return;
            if (rep->error() == QNetworkReply::NoError) {
                const auto man =
                    QJsonDocument::fromJson(rep->readAll()).object();
                for (const auto& sq : man["sequences"].toArray()) {
                    for (const auto& cv :
                         sq.toObject()["canvases"].toArray()) {
                        const auto c = cv.toObject();
                        QString folio;
                        for (const auto& lb : c["label"].toArray()) {
                            const auto l = lb.toObject();
                            const QString v = l["@value"].toString();
                            // BDRC's own folio label: "94a" / "94b"
                            if (l["@language"].toString() == "en" &&
                                QRegularExpression("^\\d+[ab]$")
                                    .match(v)
                                    .hasMatch())
                                folio = v;
                        }
                        if (folio.isEmpty()) continue;
                        const auto imgs = c["images"].toArray();
                        if (imgs.isEmpty()) continue;
                        const QString iurl = imgs.first()
                                                 .toObject()["resource"]
                                                 .toObject()["@id"]
                                                 .toString();
                        if (!iurl.isEmpty() && !folioUrl_.contains(folio)) {
                            folioUrl_[folio] = iurl;
                            folioOrder_ << folio;
                        }
                    }
                }
            }
            fetchNextManifest();
        });
    }

    void syncFolioToCursor() {
        if (folioUrl_.isEmpty()) return;
        const QString all = input_->toPlainText();
        const int pos = input_->textCursor().position();
        QRegularExpression re("@0*(\\d+)([AB])");
        QString folio;
        int folioEnd = -1;
        auto it = re.globalMatch(all.left(pos));
        while (it.hasNext()) {
            const auto m = it.next();
            folio = m.captured(1) + m.captured(2).toLower();
            folioEnd = m.capturedEnd();
        }
        if (folio.isEmpty()) return;
        // the ACIP token under the cursor, as wylie (drives word-locate)
        {
            auto isTok = [](QChar c) {
                return c.isLetter() || c == '\'' || c == '+';
            };
            int b = pos, e = pos;
            while (b > 0 && isTok(all[b - 1])) --b;
            while (e < all.size() && isTok(all[e])) ++e;
            const QString tok = all.mid(b, e - b);
            bool hasLower = false;
            for (QChar c : tok) hasLower |= c.isLower();
            // lowercase files (Lhasa _inc etc.) are already wylie;
            // uppercase ACIP goes through the proven converter
            curWordWylie_ = tok.isEmpty()
                                ? QString()
                                : hasLower
                                      ? tok.toLower()
                                      : QString::fromStdString(
                                            allcore::acipToEwts(
                                                tok.toStdString()));
        }
        // line within the folio side: the input centers preserved the
        // woodblock's own line breaks, so newline counting is exact
        curLine_ = 1 + (int)all.mid(folioEnd, pos - folioEnd).count('\n');
        const auto next = re.match(all, pos);
        const int sideEnd =
            next.hasMatch() ? next.capturedStart() : all.size();
        curLineTotal_ =
            1 + (int)all.mid(folioEnd, sideEnd - folioEnd)
                    .trimmed()
                    .count('\n');
        if (folio != curFolio_ && folioUrl_.contains(folio))
            showFolio(folio);
        else if (folio == curFolio_ && !basePx_.isNull())
            setScanPixmap(basePx_, curFolio_);
    }

    void stepFolio(int d) {
        int ix = folioOrder_.indexOf(curFolio_);
        ix = (ix < 0) ? 0 : ix + d;
        if (ix >= 0 && ix < folioOrder_.size()) showFolio(folioOrder_[ix]);
    }

    void showFolio(const QString& folio) {
        curFolio_ = folio;
        const QString url = folioUrl_.value(folio);
        scanCap_->setText(QString("<b>folio %1</b> · BDRC scan").arg(folio));
        scanCap_->show();
        if (!scanCache_.isEmpty()) {
            QDir().mkpath(scanCache_);
            const QString fn = scanCache_ + "/" +
                               QString(url).replace(
                                   QRegularExpression("[^A-Za-z0-9._-]"),
                                   "_");
            if (QFile::exists(fn)) {
                QPixmap px(fn);
                if (!px.isNull()) { setScanPixmap(px, folio); return; }
            }
            fetchScan(url, fn, folio);
        } else {
            fetchScan(url, QString(), folio);
        }
    }

    void fetchScan(const QString& url, const QString& cacheFn,
                   const QString& folio) {
        auto* rep = nam_.get(QNetworkRequest(QUrl(url)));
        const int epoch = fetchEpoch_;
        connect(rep, &QNetworkReply::finished,
                [this, rep, cacheFn, folio, epoch] {
                    rep->deleteLater();
                    if (epoch != fetchEpoch_) return;
                    if (rep->error() != QNetworkReply::NoError) {
                        scanCap_->setText("folio " + folio +
                                          ": image fetch failed (" +
                                          rep->errorString() + ")");
                        return;
                    }
                    const QByteArray data = rep->readAll();
                    if (!cacheFn.isEmpty()) {
                        QFile f(cacheFn);
                        if (f.open(QIODevice::WriteOnly)) f.write(data);
                    }
                    QPixmap px;
                    if (px.loadFromData(data) && folio == curFolio_)
                        setScanPixmap(px, folio);
                });
    }

#ifdef ALL_HAVE_OCR
    // per-folio OCR (word-locate lane): the canonical pipeline with
    // deskew FORCED to 0° — a labeled deviation — so line/word boxes
    // stay in the original image's coordinate space. Used to LOCATE
    // the cursor word on the woodblock; the recognized text itself is
    // never surfaced as text (review-material rule stands).
    struct FolioOcr {
        allocr::PageLines pl;
        std::vector<std::vector<allocr::WordSpan>> words;
        std::vector<std::string> lineText;
    };

    bool ensureFolioOcrModels() {
        if (fdet_ && frec_) return true;
        const QString lm = dataRoot_ +
            "/library/ocr_models/BDRC_PhotiLines/PhotiLines.onnx";
        const QString od = dataRoot_ + "/library/ocr_models/BDRC_Woodblock";
        if (!QFile::exists(lm) || !QFile::exists(od + "/OCRModel.onnx"))
            return false;
        try {
            fdet_ = std::make_unique<allocr::LineDetector>(lm.toStdString());
            frec_ = std::make_unique<allocr::TextRecognizer>(od.toStdString());
        } catch (const std::exception&) {
            return false;
        }
        return true;
    }

    const FolioOcr* folioOcrFor(const QString& folio) {
        auto it = folioOcr_.find(folio);
        if (it != folioOcr_.end()) return &it->second;
        if (folioOcrBusy_ || basePx_.isNull()) return nullptr;
        if (!ensureFolioOcrModels()) {
            scanCap_->setText(scanCap_->text() +
                              " · word-locate needs the OCR models "
                              "(see the Scan pane)");
            return nullptr;
        }
        folioOcrBusy_ = true;
        scanCap_->setText("folio " + folio +
                          ": running OCR to locate words…");
        QCoreApplication::processEvents();
        FolioOcr fo;
        QImage img = basePx_.toImage().convertToFormat(QImage::Format_RGB888);
        const int w = img.width(), h = img.height();
        std::vector<uint8_t> rgb(static_cast<size_t>(w) * h * 3);
        for (int y = 0; y < h; ++y)
            std::copy(img.constScanLine(y), img.constScanLine(y) + w * 3,
                      rgb.begin() + static_cast<size_t>(y) * w * 3);
        try {
            auto mask = fdet_->detect(rgb.data(), w, h);
            const double zero = 0.0;   // labeled deviation (see tooltip)
            fo.pl = allocr::buildLines(rgb.data(), w, h, mask, 2.5, 4.0,
                                       true, &zero);
            fo.words.resize(fo.pl.images.size());
            for (size_t i = 0; i < fo.pl.images.size(); ++i) {
                fo.lineText.push_back(
                    frec_->recognize(fo.pl.images[i], true, &fo.words[i]));
                QCoreApplication::processEvents();
            }
        } catch (const std::exception&) {
            folioOcrBusy_ = false;
            return nullptr;
        }
        folioOcrBusy_ = false;
        // each FolioOcr holds the full deskewed page plus line strips —
        // several MB; cap the cache so a long reading session doesn't
        // accumulate the whole volume in memory
        while (folioOcr_.size() >= 4 &&
               folioOcr_.find(folio) == folioOcr_.end())
            folioOcr_.erase(folioOcr_.begin());
        auto [ins, ok] = folioOcr_.emplace(folio, std::move(fo));
        (void)ok;
        return &ins->second;
    }

    // strip-x -> original-image x via the line's colMap
    static int folioPageX(const FolioOcr& fo, size_t line, double x) {
        const auto& cm = fo.pl.images[line].colMap;
        if (cm.empty()) return fo.pl.lines[line].x + int(x);
        const int ix = std::min(std::max(int(std::lround(x)), 0),
                                int(cm.size()) - 1);
        return cm[ix];
    }
#endif

    void setScanPixmap(const QPixmap& px, const QString& folio) {
        basePx_ = px;
        QPixmap shown = px;
        QString lineNote;
        if (curLine_ > 0 && curLineTotal_ >= curLine_ &&
            curLineTotal_ > 1) {
            // approximate band: woodblock lines are regular, but the
            // division is uniform arithmetic — labeled approximate
            shown = px.copy();
            QPainter p(&shown);
            const int y0 = px.height() * (curLine_ - 1) / curLineTotal_;
            const int y1 = px.height() * curLine_ / curLineTotal_;
            p.fillRect(0, y0, px.width(), y1 - y0,
                       QColor(255, 190, 0, 55));
            p.setPen(QPen(QColor(230, 150, 0, 180), 2));
            p.drawLine(0, y0, px.width(), y0);
            p.drawLine(0, y1, px.width(), y1);
            p.end();
            lineNote = QString(" · line %1/%2 (approximate band)")
                           .arg(curLine_)
                           .arg(curLineTotal_);
        }
#ifdef ALL_HAVE_OCR
        if (wordLocB_ && wordLocB_->isChecked() &&
            !curWordWylie_.isEmpty()) {
            const FolioOcr* fo = folioOcrFor(folio);
            if (fo && !fo->pl.lines.empty()) {
                // QPixmap is implicitly shared; QPainter detaches `shown`
                QPainter p(&shown);
                const std::string needle =
                    curWordWylie_.toLower().toStdString();
                // search the follow-along's expected line first, then all
                std::vector<size_t> order;
                if (curLine_ >= 1 &&
                    curLine_ <= (int)fo->lineText.size())
                    order.push_back(size_t(curLine_ - 1));
                for (size_t i = 0; i < fo->lineText.size(); ++i)
                    if (order.empty() || i != order[0]) order.push_back(i);
                int hits = 0;
                for (size_t oi = 0; oi < order.size() && !hits; ++oi) {
                    const size_t li = order[oi];
                    const std::string& text = fo->lineText[li];
                    // char offset -> covering word via prefix lengths
                    std::vector<size_t> pref{0};
                    for (const auto& ws : fo->words[li])
                        pref.push_back(pref.back() + ws.text.size());
                    for (size_t at = text.find(needle);
                         at != std::string::npos;
                         at = text.find(needle, at + 1)) {
                        size_t k0 = 0, k1 = 0;
                        for (size_t k = 0; k + 1 < pref.size(); ++k) {
                            if (pref[k] <= at && at < pref[k + 1]) k0 = k;
                            if (pref[k] < at + needle.size() &&
                                at + needle.size() <= pref[k + 1])
                                k1 = k;
                        }
                        const int x0 =
                            folioPageX(*fo, li, fo->words[li][k0].x0);
                        const int x1 =
                            folioPageX(*fo, li, fo->words[li][k1].x1);
                        const auto& l = fo->pl.lines[li];
                        p.fillRect(QRect(x0, l.y, std::max(x1 - x0, 3),
                                         l.h),
                                   QColor(0xE8, 0x50, 0x20, 90));
                        p.setPen(QPen(QColor(0xB4, 0x2A, 0x0A), 3));
                        p.drawRect(x0, l.y, std::max(x1 - x0, 3), l.h);
                        ++hits;
                    }
                }
                p.end();
                lineNote += hits
                    ? QString(" · “%1” located ×%2 (OCR word boxes — "
                              "locator only)")
                          .arg(curWordWylie_)
                          .arg(hits)
                    : QString(" · “%1” not found by OCR on this side")
                          .arg(curWordWylie_);
            }
        }
#endif
        const int w = qMax(300, input_->width() - 12);
        scanImg_->setPixmap(
            shown.width() > w
                ? shown.scaledToWidth(w, Qt::SmoothTransformation)
                : shown);
        scanImg_->show();
        scanCap_->setText(
            QString("<b>folio %1</b>%2 · %3×%4 · Buddhist Digital "
                    "Resource Center%5")
                .arg(folio)
                .arg(lineNote)
                .arg(px.width())
                .arg(px.height())
                .arg(scanLicense_.contains("publicdomain")
                         ? " · public domain"
                         : ""));
    }

    QLabel* hint_ = nullptr;
    // scan follow-along state
    QNetworkAccessManager nam_;
    QPushButton* scanBtn_ = nullptr;
    QLabel* scanImg_ = nullptr;
    QLabel* scanCap_ = nullptr;
    QWidget* scanNav_ = nullptr;
    QString scanWork_, scanCache_, curFolio_, scanLicense_, fileKey_;
    int fetchEpoch_ = 0;
    QString curWordWylie_;   // ACIP token at cursor, as wylie
#ifdef ALL_HAVE_OCR
    // word-locate lane (OCR used as a LOCATOR only)
    QPushButton* wordLocB_ = nullptr;
    std::unique_ptr<allocr::LineDetector> fdet_;
    std::unique_ptr<allocr::TextRecognizer> frec_;
    std::map<QString, FolioOcr> folioOcr_;
    bool folioOcrBusy_ = false;
#endif
    QString dataRoot_;
    allcore::VerbStems verbStems_;
    bool verbStemsTried_ = false;
    allcore::RefLexicon lexicon_;
    bool lexiconTried_ = false;

    void loadLexicon() {
        if (lexiconTried_) return;
        lexiconTried_ = true;
        lexicon_.load((dataRoot_ +
                       "/data/extracted/monlam_lexicon_1.txt")
                          .toStdString(),
                      "Monlam Dictionary");
        lexicon_.load((dataRoot_ +
                       "/data/extracted/monlam_lexicon_2.txt")
                          .toStdString(),
                      "Monlam Grand Dictionary");
    }
    bool titleSearchMode_ = false;
    QPixmap basePx_;
    int curLine_ = 0, curLineTotal_ = 0;
    QMap<QString, QString> folioUrl_;
    QStringList folioOrder_, pendingManifests_;
    QComboBox* scriptMode_ = nullptr;
    QComboBox* shadeMode_ = nullptr;
    QComboBox* tibFont_ = nullptr;
    QCheckBox* showPhon_ = nullptr;
    QCheckBox* showGloss_ = nullptr;
    QCheckBox* showCorpus_ = nullptr;
    QCheckBox* showGrammar_ = nullptr;
    QCheckBox* showSanskrit_ = nullptr;
    QCheckBox* showHopkins_ = nullptr;
    QCheckBox* showRefs_ = nullptr;
    QCheckBox* showSeg_ = nullptr;
    QCheckBox* showAttest_ = nullptr;
    allcore::AbbrTable abbr_;
    bool abbrTried_ = false;
    allcore::Contractions contr_;
    bool contrTried_ = false;
    QString docFile_;
    bool docIsWylie_ = false;
    QString selfTestRoot_;
    int formatGen_ = 0;
    std::map<std::string, std::string> glossary_;
    std::unique_ptr<allcore::botok::SegTrie> segmenter_;
    bool segTried_ = false;
    QString segInfo_;
    bool loading_ = false;
    int lastTok_ = -1;
    int cycle_ = 0;

    // ---- per-text glossary (Ven. Phil's Translation Tool + Hypercontext
    // "FROM THIS GLOSSARY:" — the ancestor feature; Adam approved
    // 2026-08-07). A plain TSV per opened text at
    // library/glossaries/<basename>.tsv (wylie <TAB> gloss, # comments,
    // human-editable); glossary glosses OUTRANK the general dictionary in
    // the click card, clearly labeled as the translator's own.
    QString glossaryPath() const {
        if (docFile_.isEmpty()) return {};
        return dataRoot_ + "/library/glossaries/" +
               QFileInfo(docFile_).completeBaseName() + ".tsv";
    }
    void loadGlossary() {
        glossary_.clear();
        const QString p = glossaryPath();
        if (p.isEmpty()) return;
        QFile f(p);
        if (!f.open(QIODevice::ReadOnly | QIODevice::Text)) return;
        while (!f.atEnd()) {
            const QString line = QString::fromUtf8(f.readLine()).trimmed();
            if (line.isEmpty() || line.startsWith('#')) continue;
            const int tab = line.indexOf('\t');
            if (tab <= 0) continue;
            glossary_[line.left(tab).trimmed().toStdString()] =
                line.mid(tab + 1).trimmed().toStdString();
        }
    }
    // propose a term to the authority straight from its card, with the
    // surrounding passage auto-captured as evidence (proposal channel
    // v2). The kind is chosen by the proposer; the term and evidence
    // come pre-filled so it takes one dialog, not a form re-entry.
    void proposeFromCard(const QString& wylieHtml) {
        const QString wylie =
            QTextDocumentFragment::fromHtml(wylieHtml).toPlainText();
        // category
        const QStringList kinds = {
            "Honorific term (↔ ordinary)",
            "HIGH honorific marking",
            "Humilific (self-lowering) form",
            "DOUBLE honorific (subject + recipient)",
            "Pronunciation exception",
            "Abbreviation / contraction candidate",
            "Rendering for this word (→ dictionary)",
            "Idiom / fixed expression",
            "Note about this passage"};
        bool ok = false;
        const QString choice = QInputDialog::getItem(
            this, "Propose to the authority",
            "What are you proposing for “" + wylie + "”?", kinds, 0, false,
            &ok);
        if (!ok) return;
        allcore::ProposalKind kind = allcore::ProposalKind::WordRendering;
        if (choice.startsWith("Honorific"))
            kind = allcore::ProposalKind::Honorific;
        else if (choice.startsWith("HIGH"))
            kind = allcore::ProposalKind::HighHonorific;
        else if (choice.startsWith("Humilific"))
            kind = allcore::ProposalKind::Humilific;
        else if (choice.startsWith("DOUBLE"))
            kind = allcore::ProposalKind::DoubleHonorific;
        else if (choice.startsWith("Pronunciation"))
            kind = allcore::ProposalKind::Pronunciation;
        else if (choice.startsWith("Abbreviation"))
            kind = allcore::ProposalKind::Abbreviation;
        else if (choice.startsWith("Idiom"))
            kind = allcore::ProposalKind::Idiom;
        else if (choice.startsWith("Note"))
            kind = allcore::ProposalKind::Note;
        // the proposed value
        QString value;
        if (kind != allcore::ProposalKind::Note) {
            value = QInputDialog::getText(
                this,
                "Proposed value",
                kind == allcore::ProposalKind::Pronunciation
                    ? "Proposed pronunciation:"
                    : (kind == allcore::ProposalKind::Honorific
                           ? "Ordinary counterpart (or leave blank):"
                           : "Proposed English / expansion:"),
                QLineEdit::Normal, "", &ok);
            if (!ok) return;
        }
        // evidence: the sentence around the cursor in the source
        QString evidence;
        if (input_) {
            const QString all = input_->toPlainText();
            const int pos = input_->textCursor().position();
            int b = pos, e = pos;
            for (int n = 0; b > 0 && all[b - 1] != '\n'; --b) {}
            for (; e < all.size() && all[e] != '\n'; ++e) {}
            evidence = all.mid(b, e - b).trimmed().left(300);
        }
        if (!docFile_.isEmpty())
            evidence += "  [" + QFileInfo(docFile_).fileName() + "]";
        // honorific: the term itself is the honorific, the value is the
        // ordinary counterpart (matching the register file's columns)
        QString field;
        if (kind == allcore::ProposalKind::Honorific ||
            kind == allcore::ProposalKind::HighHonorific ||
            kind == allcore::ProposalKind::Humilific ||
            kind == allcore::ProposalKind::DoubleHonorific) {
            field = value;   // ordinary form goes in the secondary slot
            value.clear();
        }
        loadIdentity();
        fileProposal(this, kind, wylie, value, field, evidence);
    }

    void saveGlossaryEntry(const std::string& wylie, const std::string& gloss) {
        const QString p = glossaryPath();
        if (p.isEmpty()) return;
        QDir().mkpath(QFileInfo(p).path());
        glossary_[wylie] = gloss;
        QFile f(p);
        if (!f.open(QIODevice::WriteOnly | QIODevice::Text)) return;
        QTextStream ts(&f);
        ts << "# per-text glossary for " << QFileInfo(docFile_).fileName()
           << " — wylie <TAB> gloss; the translator's own, binding for "
              "this text's display\n";
        for (const auto& [w, g] : glossary_)
            ts << QString::fromStdString(w) << "\t"
               << QString::fromStdString(g) << "\n";
    }
    QString glossaryHtml(const std::string& wylie) const {
        auto it = glossary_.find(wylie);
        QString edit = QString("<a href='gloss:%1'>%2</a>")
                           .arg(QString::fromStdString(wylie).toHtmlEscaped(),
                                it == glossary_.end()
                                    ? "＋ add to this text's glossary"
                                    : "edit");
        // propose-from-card (proposal channel v2): offer this term to
        // the authority, evidence auto-captured from the open passage
        edit += QString(" &nbsp;·&nbsp; <a href='propose:%1'>propose to "
                        "the authority…</a>")
                    .arg(QString::fromStdString(wylie).toHtmlEscaped());
        if (it == glossary_.end())
            return docFile_.isEmpty()
                       ? QString()
                       : "<div style='font-size:11px;color:#999'>" + edit +
                             "</div>";
        return QString("<div style='background:#EAF6EF;padding:4px 7px;"
                       "border-radius:6px;margin:3px 0'><b>FROM THIS TEXT'S "
                       "GLOSSARY:</b> %1 <small style='color:#555'>"
                       "(the translator's own — outranks the general "
                       "dictionary here)</small> <small>%2</small></div>")
            .arg(QString::fromStdString(it->second).toHtmlEscaped(), edit);
    }

    // Botok segmenter: lazy one-time lexicon build — HGM dictionary headwords
    // (battery-proven conversion chain) + the Monlam word lists, in the
    // compact SegTrie (proven segmentation-identical to the ported path at
    // corpus scale). Reference display only — the Overlay's spans stay
    // dictionary-lattice-bound.
    void ensureSegmenter() {
        if (segTried_) return;
        segTried_ = true;
        QElapsedTimer timer;
        timer.start();
        try {
            auto seg = std::make_unique<allcore::botok::SegTrie>(
                (dataRoot_ + "/data/botok").toStdString());
            for (const auto& [id, acip] : spine_.allAcipHeadwords()) {
                auto [uni, ok] =
                    allcore::wylieToUnicode(allcore::acipToEwts(acip));
                if (ok) seg->addWord(uni);
            }
            size_t hgmWords = seg->wordCount();
            loadLexicon();
            lexicon_.eachWord(
                [&seg](const std::string& w) { seg->addWord(w); });
            segmenter_ = std::move(seg);
            segInfo_ = QString("%1 HGM + %2 Monlam forms, built in %3s")
                           .arg(hgmWords)
                           .arg(segmenter_->wordCount() - hgmWords)
                           .arg(timer.elapsed() / 1000.0, 0, 'f', 1);
        } catch (const std::exception& e) {
            segInfo_ = QString("unavailable: %1").arg(e.what());
        }
    }

    QString segmentationHtml(int tok) {
        if (!showSeg_ || !showSeg_->isChecked()) return {};
        ensureSegmenter();
        if (!segmenter_)
            return "<div style='color:#B4540A;font-size:12px'>Botok "
                   "segmentation " + segInfo_.toHtmlEscaped() + "</div>";
        // the clicked token's clause (between barriers)
        int beg = tok;
        while (beg > 0 && !doc_.barrier_after[beg - 1]) --beg;
        int end = tok;
        while (end + 1 < (int)doc_.tokens.size() && !doc_.barrier_after[end])
            ++end;
        std::string uni;
        for (int i = beg; i <= end; ++i) {
            auto [u, ok] =
                allcore::wylieToUnicode(allcore::acipToEwts(doc_.tokens[i]));
            if (!ok)
                return "<div style='color:#B4540A;font-size:12px'>Botok "
                       "segmentation skipped: unconvertible token in this "
                       "clause</div>";
            if (!uni.empty()) uni += "་";
            uni += u;
        }
        QString h =
            "<div style='font-size:12px'><span style='color:#555'>Botok "
            "segmentation <i>(Apache-2.0 port, reference only — lexicon: " +
            segInfo_.toHtmlEscaped() +
            ")</i>:</span><br><span style='font-size:15px'>";
        bool first = true;
        for (const auto& w : segmenter_->segment(uni)) {
            QString txt = QString::fromStdString(w.text).toHtmlEscaped();
            if (!first) h += " <span style='color:#AAA'>·</span> ";
            first = false;
            if (w.tibetan && w.word) {
                h += "<span style='color:#1E4E6B'>" + txt + "</span>";
                if (!w.affixType.empty())  // matched through an affixed form:
                    // show the split (base + particle), splitaffixed-style
                    h += QString("<small style='color:#7A5A00'> = %1 + %2 "
                                 "(%3)</small>")
                             .arg(QString::fromStdString(
                                      allcore::botok::segWordBaseForm(w))
                                      .toHtmlEscaped(),
                                  QString::fromStdString(
                                      allcore::botok::segWordAffixSurface(w))
                                      .toHtmlEscaped(),
                                  QString::fromStdString(w.affixType)
                                      .toHtmlEscaped());
            } else if (w.tibetan) {  // honest: not in the lexicon
                h += "<span style='color:#B4540A'>⟨" + txt + "⟩</span>";
            } else {
                h += "<span style='color:#888'>" + txt + "</span>";
            }
        }
        h += "</span></div><hr>";
        return h;
    }
};

// ---- Search pane: Gofer grammar over the corpus ----------------------------

// ---- GoferPane: the original Gofer GUI reborn (Adam, 2026-08-10 —
// GMR's daily tool; fidelity to the original layout and workflow).
// The ENGINE is the proven allcore port of Ted Lemon's lex/expr/near
// semantics (gofer_smoke); this pane rebuilds the face: eight
// "go for" fields, combiner + proximity, fold mode, a checkable
// list of search targets (the aligned corpus + any folders, added
// exactly as in the original), Add/Remove/Duplicate/Save/Stop/Find,
// and the Search Setting / Saved Search / Search Results tabs.
class GoferPane : public QWidget {
public:
    GoferPane(allcore::Spine& spine, const QString& root)
        : spine_(spine), root_(root) {
        auto* outer = new QVBoxLayout(this);
        inner_ = new QTabWidget;
        outer->addWidget(inner_);

        // ---- Search Setting tab
        auto* setting = new QWidget;
        auto* sl = new QVBoxLayout(setting);
        sl->addWidget(new QLabel("Enter Text to Go For:"));
        auto* grid1 = new QGridLayout;
        for (int i = 0; i < 4; ++i) {
            fields_[i] = new QLineEdit;
            grid1->addWidget(fields_[i], i / 2, i % 2);
        }
        sl->addLayout(grid1);
        auto* opRow = new QHBoxLayout;
        combiner_ = new QComboBox;
        combiner_->addItems({"OR", "AND (same file)", "NEAR"});
        opRow->addWidget(combiner_);
        opRow->addWidget(new QLabel("proximity:"));
        proximity_ = new QLineEdit("0");
        proximity_->setMaximumWidth(120);
        opRow->addWidget(proximity_);
        opRow->addStretch();
        sl->addLayout(opRow);
        auto* grid2 = new QGridLayout;
        for (int i = 4; i < 8; ++i) {
            fields_[i] = new QLineEdit;
            grid2->addWidget(fields_[i], (i - 4) / 2, (i - 4) % 2);
        }
        sl->addLayout(grid2);
        fold_ = new QComboBox;
        fold_->addItems({"Ignore space and capitalization when searching",
                         "Ignore space when searching",
                         "Ignore nothing when searching"});
        sl->addWidget(fold_);
        dirs_ = new QListWidget;
        loadDirs();
        sl->addWidget(dirs_, 1);
        auto* btnRow = new QHBoxLayout;
        auto* addB = new QPushButton("Add");
        auto* remB = new QPushButton("Remove");
        auto* dupB = new QPushButton("Duplicate");
        auto* saveB = new QPushButton("Save");
        stopB_ = new QPushButton("Stop");
        stopB_->setEnabled(false);
        findB_ = new QPushButton("Find");
        for (auto* b : {addB, remB, dupB, saveB}) btnRow->addWidget(b);
        btnRow->addStretch();
        btnRow->addWidget(stopB_);
        btnRow->addWidget(findB_);
        sl->addLayout(btnRow);
        inner_->addTab(setting, "Search Setting");

        // ---- Saved Search tab
        auto* savedPage = new QWidget;
        auto* vl = new QVBoxLayout(savedPage);
        saved_ = new QListWidget;
        vl->addWidget(saved_, 1);
        auto* svRow = new QHBoxLayout;
        auto* loadB = new QPushButton("Load");
        auto* delB = new QPushButton("Delete");
        svRow->addWidget(loadB);
        svRow->addWidget(delB);
        svRow->addStretch();
        vl->addLayout(svRow);
        inner_->addTab(savedPage, "Saved Search");
        refreshSaved();

        // ---- Search Results tab
        results_ = new QTextBrowser;
        results_->setOpenLinks(false);
        inner_->addTab(results_, "Search Results");

        connect(addB, &QPushButton::clicked, [this] {
            const QString d = QFileDialog::getExistingDirectory(
                this, "Add a folder to search");
            if (!d.isEmpty()) { addDirRow(d, true); saveDirs(); }
        });
        connect(remB, &QPushButton::clicked, [this] {
            const int r = dirs_->currentRow();
            if (r > 1) { delete dirs_->takeItem(r); saveDirs(); }
        });
        connect(dupB, &QPushButton::clicked, [this] {
            auto* it = dirs_->currentItem();
            if (it && dirs_->currentRow() > 1) {
                addDirRow(it->text(), it->checkState() == Qt::Checked);
                saveDirs();
            }
        });
        connect(saveB, &QPushButton::clicked, [this] { saveSearch(); });
        connect(loadB, &QPushButton::clicked, [this] { loadSearch(); });
        connect(saved_, &QListWidget::itemDoubleClicked,
                [this](QListWidgetItem*) { loadSearch(); });
        connect(delB, &QPushButton::clicked, [this] {
            auto* it = saved_->currentItem();
            if (!it) return;
            QSettings s("ALL", "TranslationTool");
            s.remove("gofer/saved/" + it->text());
            refreshSaved();
        });
        connect(findB_, &QPushButton::clicked, [this] { find(); });
    }

    int selfTest(QStringList& log) {
        int fails = 0;
        auto check = [&](bool ok, const char* what) {
            log << QString("  [%1] Gofer: %2")
                       .arg(ok ? "PASS" : "FAIL").arg(what);
            if (!ok) ++fails;
        };
        int nf = 0;
        for (auto* f : fields_) nf += f != nullptr;
        check(nf == 8, "eight go-for fields present");
        check(dirs_->count() >= 2 &&
                  dirs_->item(0)->checkState() == Qt::Checked,
              "corpus row present and checked");
        for (auto* f : fields_) f->clear();
        fields_[0]->setText("sems can");
        combiner_->setCurrentIndex(0);
        find();
        check(results_->toPlainText().contains("hit"),
              "Find reaches the corpus through the Gofer engine");
        fields_[0]->clear();
        return fails;
    }

private:
    static constexpr const char* kCorpusRow =
        "HGM aligned corpus (all courses)";
    static constexpr const char* kApparatusRow =
        "Published apparatus (footnotes + bibliography)";

    void addDirRow(const QString& text, bool checked) {
        auto* it = new QListWidgetItem(text);
        it->setFlags(it->flags() | Qt::ItemIsUserCheckable);
        it->setCheckState(checked ? Qt::Checked : Qt::Unchecked);
        dirs_->addItem(it);
    }
    void loadDirs() {
        QSettings s("ALL", "TranslationTool");
        addDirRow(kCorpusRow, s.value("gofer/corpusChecked", true).toBool());
        addDirRow(kApparatusRow,
                  s.value("gofer/apparatusChecked", true).toBool());
        const auto list = s.value("gofer/dirs").toStringList();
        const auto checked = s.value("gofer/dirsChecked").toStringList();
        if (list.isEmpty()) {
            addDirRow(root_ + "/library", true);
        } else {
            for (const auto& d : list) addDirRow(d, checked.contains(d));
        }
    }
    void saveDirs() {
        QSettings s("ALL", "TranslationTool");
        QStringList list, checked;
        s.setValue("gofer/corpusChecked",
                   dirs_->item(0)->checkState() == Qt::Checked);
        s.setValue("gofer/apparatusChecked",
                   dirs_->item(1)->checkState() == Qt::Checked);
        for (int i = 2; i < dirs_->count(); ++i) {
            list << dirs_->item(i)->text();
            if (dirs_->item(i)->checkState() == Qt::Checked)
                checked << dirs_->item(i)->text();
        }
        s.setValue("gofer/dirs", list);
        s.setValue("gofer/dirsChecked", checked);
    }

    QString buildQuery() const {
        QStringList terms;
        for (auto* f : fields_) {
            const QString t = f->text().trimmed();
            if (!t.isEmpty()) terms << "\"" + t + "\"";
        }
        if (terms.isEmpty()) return {};
        const int mode = combiner_->currentIndex();
        if (mode == 0) return terms.join(" OR ");
        if (mode == 1) return terms.join(" NEAR/1000000 ");
        int n = proximity_->text().toInt();
        if (n <= 0) n = 3;
        return terms.join(QString(" NEAR/%1 ").arg(n));
    }

    void find() {
        const QString q = buildQuery();
        if (q.isEmpty()) {
            results_->setHtml("<i>enter at least one term</i>");
            inner_->setCurrentIndex(2);
            return;
        }
        saveDirs();
        results_->setHtml("<i>searching\u2026</i>");
        inner_->setCurrentIndex(2);
        QCoreApplication::processEvents();
        QString h = QString("<div style='color:#777'>query: <code>%1"
                            "</code></div><hr>")
                        .arg(q.toHtmlEscaped());
        int total = 0;
        if (dirs_->item(0)->checkState() == Qt::Checked) {
            try {
                auto hits = allcore::goferSearch(spine_, q.toStdString(),
                                                 "", 60);
                h += QString("<div><b>%1</b> \u2014 %2 hit(s)</div>")
                         .arg(kCorpusRow)
                         .arg(hits.size());
                total += (int)hits.size();
                int shown = 0;
                for (const auto& g : hits) {
                    if (++shown > 20) break;
                    h += "<div style='margin:4px 0'><small>[" +
                         QString::fromStdString(g.course) + ":" +
                         QString::number(g.seq_lo) + "]</small> ";
                    for (const auto& seg : g.window)
                        h += QString::fromStdString(seg.wylie)
                                 .left(120)
                                 .toHtmlEscaped() +
                             " <i>" +
                             QString::fromStdString(seg.english)
                                 .left(120)
                                 .toHtmlEscaped() +
                             "</i> ";
                    h += "</div>";
                }
                h += "<hr>";
            } catch (const std::exception& e) {
                h += QString("<div style='color:#8C2F2B'>corpus: %1"
                             "</div><hr>")
                         .arg(QString::fromUtf8(e.what()).toHtmlEscaped());
            }
        }
        if (dirs_->item(1)->checkState() == Qt::Checked &&
            (g_appNotes || g_appBib)) {
            QStringList terms;
            for (auto* f : fields_) {
                const QString t = f->text().trimmed();
                if (!t.isEmpty()) terms << t;
            }
            const bool anyMode = combiner_->currentIndex() == 0;
            auto matches = [&](const QString& hay) {
                int found = 0;
                for (const auto& t : terms)
                    if (hay.contains(t, Qt::CaseInsensitive)) ++found;
                return anyMode ? found > 0
                               : found == (int)terms.size();
            };
            int n = 0;
            QString ah;
            if (g_appNotes)
                for (const auto& note : *g_appNotes) {
                    if (!matches(note.lemma + " " + note.text)) continue;
                    if (++n <= 20)
                        ah += "<div style='margin:4px 0'><b>note " +
                              QString::number(note.num) + "</b> — " +
                              note.lemma.toHtmlEscaped() + " <small>(" +
                              note.source.toHtmlEscaped() +
                              ")</small><br>" +
                              note.text.left(200).toHtmlEscaped() +
                              "</div>";
                }
            if (g_appBib)
                for (const auto& b : *g_appBib) {
                    if (!matches(b.source + " " + b.text)) continue;
                    if (++n <= 40)
                        ah += "<div style='margin:4px 0'><b>" +
                              b.id.toHtmlEscaped() + "</b> " +
                              b.text.left(220).toHtmlEscaped() +
                              " <small>(" + b.source.toHtmlEscaped() +
                              ")</small></div>";
                }
            h += QString("<div><b>%1</b> — %2 hit(s)</div>")
                     .arg(kApparatusRow)
                     .arg(n) +
                 ah + "<hr>";
            total += n;
        }
        for (int i = 2; i < dirs_->count(); ++i) {
            if (dirs_->item(i)->checkState() != Qt::Checked) continue;
            const QString dir = dirs_->item(i)->text();
            try {
                std::vector<allcore::FileGoferHit> hits;
                const QString ix = dir + "/.index.db";
                if (QFileInfo::exists(ix)) {
                    allcore::LibraryIndex li(ix.toStdString());
                    hits = li.search(q.toStdString(), 60);
                } else {
                    hits = allcore::goferSearchFiles(dir.toStdString(),
                                                     q.toStdString(), 60);
                }
                h += QString("<div><b>%1</b> \u2014 %2 hit(s)</div>")
                         .arg(dir.toHtmlEscaped())
                         .arg(hits.size());
                total += (int)hits.size();
                int shown = 0;
                for (const auto& f : hits) {
                    if (++shown > 20) break;
                    h += "<div style='margin:4px 0'><small>" +
                         QString::fromStdString(f.file).toHtmlEscaped() +
                         ":" + QString::number(f.line_lo) + "</small> ";
                    for (const auto& ln : f.lines)
                        h += QString::fromStdString(ln)
                                 .left(140)
                                 .toHtmlEscaped() +
                             " ";
                    h += "</div>";
                }
                h += "<hr>";
            } catch (const std::exception& e) {
                h += QString("<div style='color:#8C2F2B'>%1: %2</div><hr>")
                         .arg(dir.toHtmlEscaped())
                         .arg(QString::fromUtf8(e.what()).toHtmlEscaped());
            }
        }
        h += QString("<div><b>%1 total hit(s)</b></div>").arg(total);
        results_->setHtml(h);
    }

    void saveSearch() {
        bool ok = false;
        const QString name = QInputDialog::getText(
            this, "Save search", "Name:", QLineEdit::Normal, "", &ok);
        if (!ok || name.trimmed().isEmpty()) return;
        QSettings s("ALL", "TranslationTool");
        s.beginGroup("gofer/saved/" + name.trimmed());
        for (int i = 0; i < 8; ++i)
            s.setValue(QString("f%1").arg(i), fields_[i]->text());
        s.setValue("combiner", combiner_->currentIndex());
        s.setValue("proximity", proximity_->text());
        s.setValue("fold", fold_->currentIndex());
        s.endGroup();
        refreshSaved();
    }
    void loadSearch() {
        auto* it = saved_->currentItem();
        if (!it) return;
        QSettings s("ALL", "TranslationTool");
        s.beginGroup("gofer/saved/" + it->text());
        for (int i = 0; i < 8; ++i)
            fields_[i]->setText(
                s.value(QString("f%1").arg(i)).toString());
        combiner_->setCurrentIndex(s.value("combiner", 0).toInt());
        proximity_->setText(s.value("proximity", "0").toString());
        fold_->setCurrentIndex(s.value("fold", 0).toInt());
        s.endGroup();
        inner_->setCurrentIndex(0);
    }
    void refreshSaved() {
        saved_->clear();
        QSettings s("ALL", "TranslationTool");
        s.beginGroup("gofer/saved");
        for (const auto& g : s.childGroups()) saved_->addItem(g);
        s.endGroup();
    }

    allcore::Spine& spine_;
    QString root_;
    QTabWidget* inner_ = nullptr;
    QLineEdit* fields_[8] = {};
    QComboBox* combiner_ = nullptr;
    QLineEdit* proximity_ = nullptr;
    QComboBox* fold_ = nullptr;
    QListWidget* dirs_ = nullptr;
    QListWidget* saved_ = nullptr;
    QTextBrowser* results_ = nullptr;
    QPushButton* stopB_ = nullptr;
    QPushButton* findB_ = nullptr;
};

static QWidget* makeSearchPane(allcore::Spine& spine,
                               const QString& libraryRoot = QString()) {
    auto* pane = new QWidget;
    auto* layout = new QVBoxLayout(pane);

    auto* row = new QHBoxLayout;
    auto* box = new QLineEdit;
    box->setPlaceholderText(
        "Gofer query…   e.g.  \"kun rdzob bden pa\" NEAR/3 \"don dam bden pa\"   ·   stong OR bden");
    auto* courseBox = new QComboBox;
    courseBox->addItem("all courses", "");
    for (const auto& c : spine.corpusCourses())
        courseBox->addItem(QString::fromStdString(c), QString::fromStdString(c));
    row->addWidget(box, 1);
    row->addWidget(courseBox);
    // saved searches (Gofer's own SavedSearches feature, restored — Adam
    // approved 2026-08-07): named queries persisted across sessions
    auto* savedBox = new QComboBox;
    savedBox->setPlaceholderText("saved searches…");
    savedBox->setMinimumWidth(150);
    {
        QSettings settings("ALL", "TranslationTool");
        const QStringList saved =
            settings.value("search/saved").toStringList();
        for (const QString& s : saved) {
            const int sep = s.indexOf('\x1f');
            if (sep > 0)
                savedBox->addItem(s.left(sep), s.mid(sep + 1));
        }
    }
    auto* saveBtn = new QPushButton("Save…");
    row->addWidget(savedBox);
    row->addWidget(saveBtn);
    QObject::connect(savedBox, &QComboBox::activated, [savedBox, box](int ix) {
        box->setText(savedBox->itemData(ix).toString());
    });
    QObject::connect(saveBtn, &QPushButton::clicked, [pane, box, savedBox] {
        const QString q = box->text().trimmed();
        if (q.isEmpty()) return;
        bool ok = false;
        const QString name = QInputDialog::getText(
            pane, "Save search", "Name for this query:", QLineEdit::Normal,
            q.left(30), &ok);
        if (!ok || name.trimmed().isEmpty()) return;
        savedBox->addItem(name.trimmed(), q);
        QSettings settings("ALL", "TranslationTool");
        QStringList saved = settings.value("search/saved").toStringList();
        saved << name.trimmed() + QChar('\x1f') + q;
        settings.setValue("search/saved", saved);
    });
    layout->addLayout(row);

    auto* row2 = new QHBoxLayout;
    auto* dirBox = new QLineEdit;
    dirBox->setPlaceholderText(
        "…or a folder of raw ACIP files (leave empty to search the corpus)");
    row2->addWidget(dirBox, 1);
    // one-click file-tree search over the installed library
    if (!libraryRoot.isEmpty() && QDir(libraryRoot).exists()) {
        auto* libBtn = new QPushButton("search the Library");
        QObject::connect(libBtn, &QPushButton::clicked,
                         [dirBox, libraryRoot] { dirBox->setText(libraryRoot); });
        row2->addWidget(libBtn);
    }
    // federated search of the user's own machine (the wysearch idea from
    // the ACIP Development survey): Spotlight already indexes every text,
    // PDF, and Word file on the Mac — probe it with the query as typed
    // PLUS its Tibetan-unicode conversions, since files on disk may be in
    // wylie, ACIP, or Tibetan script
    auto* macBtn = new QPushButton("Search this Mac (Spotlight)");
    macBtn->setToolTip(
        "Ask macOS Spotlight for documents anywhere on this machine "
        "containing the query — searched as typed, and (when it converts) "
        "as Tibetan unicode via the proven wylie and ACIP chains.");
    row2->addWidget(macBtn);
    layout->addLayout(row2);

    auto* results = new QTextBrowser;
    results->setOpenLinks(false);   // anchors handled below, never navigated
    layout->addWidget(results);
    auto* status = new QLabel(
        "Terms are phrases (quote or just type). OR = either. NEAR/N = both "
        "within N lines of the same source. Parentheses group.");
    status->setWordWrap(true);
    layout->addWidget(status);
    QObject::connect(macBtn, &QPushButton::clicked, [box, results, status] {
        // strip Gofer operators — Spotlight gets plain phrases
        QString raw = box->text().trimmed();
        raw.replace(QRegularExpression("\\bOR\\b|NEAR/\\d+|[()\"]"), " ");
        raw = raw.simplified();
        if (raw.isEmpty()) {
            status->setText("type a query first (plain words work best "
                            "for Spotlight)");
            return;
        }
        QStringList variants{raw};
        if (auto [uni, ok] =
                allcore::wylieToUnicode(raw.toLower().toStdString());
            ok)
            variants << QString::fromStdString(uni);
        if (auto [uni, ok] = allcore::wylieToUnicode(
                allcore::acipToEwts(raw.toUpper().toStdString()));
            ok) {
            const QString u = QString::fromStdString(uni);
            if (!variants.contains(u)) variants << u;
        }
        status->setText("asking Spotlight…");
        QCoreApplication::processEvents();
        QStringList found;   // ordered, deduplicated
        for (const QString& term : variants) {
            QProcess p;
            p.start("/usr/bin/mdfind",
                    {"-literal", QString("kMDItemTextContent == \"*%1*\"cd")
                                     .arg(term)});
            // bounded: a cold Spotlight index can stall — 6s per term
            // keeps the worst case short (the UI notes the wait above)
            if (!p.waitForFinished(6000)) { p.kill(); continue; }
            const QStringList lines =
                QString::fromUtf8(p.readAllStandardOutput())
                    .split('\n', Qt::SkipEmptyParts);
            for (const QString& f : lines) {
                if (found.contains(f)) continue;
                found << f;
                if (found.size() >= 60) break;
            }
            if (found.size() >= 60) break;
        }
        QString h = "<div><b>FROM THIS MAC (Spotlight)</b> — searched as: ";
        for (const QString& v : variants)
            h += "<code>" + v.toHtmlEscaped() + "</code> ";
        h += "</div><hr>";
        if (found.isEmpty())
            h += "<i>Spotlight found no documents (its index skips "
                 "unindexed volumes and some file types)</i>";
        for (const QString& f : found) {
            const QFileInfo fi(f);
            h += "<div style='margin:3px 0'><a href='" +
                 QUrl::fromLocalFile(f).toString() + "'>" +
                 fi.fileName().toHtmlEscaped() + "</a> <small "
                 "style='color:#777'>" +
                 fi.path().toHtmlEscaped() + "</small></div>";
        }
        results->setHtml(h);
        status->setText(QString("%1 document(s) on this machine — click "
                                "to open in the default app")
                            .arg(found.size()));
    });
    QObject::connect(results, &QTextBrowser::anchorClicked,
                     [](const QUrl& u) {
                         if (u.isLocalFile() || u.scheme().startsWith("http"))
                             QDesktopServices::openUrl(u);
                     });

    auto runSearch = [&spine, box, courseBox, dirBox, results, status,
                      libraryRoot] {
        const std::string q = box->text().trimmed().toStdString();
        if (q.empty()) return;
        const QString dir = dirBox->text().trimmed();
        if (!dir.isEmpty()) {
            try {
                // the library has a prebuilt index — use it when targeting
                // the library root (token/phrase semantics, like the corpus)
                std::vector<allcore::FileGoferHit> hits;
                const QString ixPath = dir + "/.index.db";
                if (!libraryRoot.isEmpty() &&
                    QDir(dir) == QDir(libraryRoot) &&
                    QFileInfo::exists(ixPath)) {
                    allcore::LibraryIndex ix(ixPath.toStdString());
                    hits = ix.search(q, 60);
                    status->setText(
                        QString("searched the library INDEX (%1 files, %2 "
                                "lines) — rebuild it from the Library pane "
                                "after installing new texts.")
                            .arg(ix.fileCount())
                            .arg(ix.lineCount()));
                } else {
                    hits = allcore::goferSearchFiles(dir.toStdString(), q, 60);
                }
                QString h;
                if (hits.empty()) h = "<i>no matches in files</i>";
                for (const auto& hit : hits) {
                    h += "<div style='margin-bottom:12px'><b>" +
                         QString::fromStdString(hit.file).toHtmlEscaped() + ":" +
                         QString::number(hit.line_lo) +
                         (hit.line_hi != hit.line_lo
                              ? "–" + QString::number(hit.line_hi)
                              : QString()) +
                         "</b>";
                    for (const auto& l : hit.lines)
                        h += "<br>" + QString::fromStdString(l).left(300).toHtmlEscaped();
                    h += "</div>";
                }
                results->setHtml(h);
                status->setText(QString("%1 file hit window(s)").arg(hits.size()));
            } catch (const std::exception& ex) {
                status->setText(QString("file search error: %1").arg(ex.what()));
            }
            return;
        }
        try {
            auto hits = allcore::goferSearch(
                spine, q, courseBox->currentData().toString().toStdString(), 60);
            QString h;
            if (hits.empty()) h = "<i>no matches</i>";
            for (const auto& hit : hits) {
                h += "<div style='margin-bottom:12px'><b>[" +
                     QString::fromStdString(hit.course) + ":" +
                     QString::number(hit.seq_lo) +
                     (hit.seq_hi != hit.seq_lo
                          ? "–" + QString::number(hit.seq_hi)
                          : QString()) +
                     "]</b>";
                for (const auto& s : hit.window) {
                    h += "<br><i>" +
                         QString::fromStdString(s.wylie).left(240).toHtmlEscaped() +
                         "</i><br>" +
                         QString::fromStdString(s.english).left(300).toHtmlEscaped();
                }
                h += "</div>";
            }
            results->setHtml(h);
            status->setText(QString("%1 hit window(s)").arg(hits.size()));
        } catch (const std::exception& ex) {
            status->setText(QString("query error: %1").arg(ex.what()));
        }
    };
    QObject::connect(box, &QLineEdit::returnPressed, runSearch);
    QObject::connect(courseBox, &QComboBox::activated, runSearch);
    return pane;
}

// ---- Convert pane: ACIP/wylie → everything, via the battery-proven ports ----

// ---- SanskritPane: the whole Sanskrit stack in one workbench
// (Adam, 2026-08-10) — every notation, Whitney's roots, the
// Mahavyutpatti bridge to Tibetan, and Devanagari OCR.
class SanskritPane : public QWidget {
public:
    SanskritPane(allcore::Mvp* mvp, allcore::WhitneyRoots* whitney)
        : mvp_(mvp), whitney_(whitney) {
        auto* l = new QVBoxLayout(this);
        l->addWidget(new QLabel(
            "<b>Sanskrit workbench</b> — IAST, Devanagari, or ACIP "
            "input-code (a#/n%) auto-detected. Every notation, "
            "Whitney's roots, and the Mah\u0101vyutpatti bridge."));
        auto* row = new QHBoxLayout;
        in_ = new QLineEdit;
        in_->setPlaceholderText(
            "prama\u0304n\u0323a \u00b7 \u0928\u092e\u0903 \u00b7 "
            "prama#n%a \u00b7 root: bh\u016b");
        row->addWidget(in_, 1);
        auto* go = new QPushButton("Analyze");
        row->addWidget(go);
        auto* ocr = new QPushButton("Sanskrit OCR\u2026");
        ocr->setToolTip("Recognize a Devanagari image (tesseract + san "
                        "model, optional install) \u2014 review "
                        "material, never trusted text");
        row->addWidget(ocr);
        l->addLayout(row);
        out_ = new QTextBrowser;
        out_->setOpenExternalLinks(true);
        l->addWidget(out_, 1);
        connect(go, &QPushButton::clicked, [this] { analyze(); });
        connect(in_, &QLineEdit::returnPressed, [this] { analyze(); });
        connect(ocr, &QPushButton::clicked, [this] { runOcr(); });
    }

    int selfTest(QStringList& log) {
        int fails = 0;
        auto check = [&](bool ok, const char* what) {
            log << QString("  [%1] Sanskrit: %2")
                       .arg(ok ? "PASS" : "FAIL").arg(what);
            if (!ok) ++fails;
        };
        in_->setText(QString::fromUtf8("pram\u0101\u1e47a"));
        analyze();
        const QString t = out_->toPlainText();
        check(t.contains("prama#n%a"), "input-code style renders");
        check(out_->toHtml().contains(QString::fromUtf8("\u092A")) ||
                  t.contains(QString::fromUtf8("\u092A")),
              "Devanagari renders");
        if (whitney_) {
            in_->setText(QString::fromUtf8("bh\u016b"));
            analyze();
            check(out_->toPlainText().contains("Whitney"),
                  "Whitney roots reachable");
        }
        in_->clear();
        return fails;
    }

private:
    void analyze() {
        const QString raw = in_->text().trimmed();
        if (raw.isEmpty()) return;
        std::string iast = raw.toStdString();
        bool hasDeva = false;
        for (QChar c : raw)
            hasDeva |= (c.unicode() >= 0x0900 && c.unicode() <= 0x097F);
        QString note;
        if (hasDeva) {
            auto [ia, ok] = allcore::devanagariToIast(iast);
            iast = ia;
            if (!ok) note = " <small>(partial Devanagari decode)</small>";
        } else if (raw.contains('#') || raw.contains('%') ||
                   raw.contains('~')) {
            iast = allcore::inputcodeToIast(iast);
            note = " <small>(from ACIP input-code)</small>";
        }
        auto row = [](const QString& k, const QString& v) {
            return "<tr><td style='color:#777;padding-right:12px'>" + k +
                   "</td><td>" + v + "</td></tr>";
        };
        auto [acip, aok] = allcore::iastToAcip(iast);
        auto [tib, tok2] = allcore::iastToTibetan(iast);
        auto [deva, dok] = allcore::iastToDevanagari(iast);
        auto [ipa, iok] = allcore::iastToIpa(iast);
        QString h = "<div><b>" +
                    QString::fromStdString(iast).toHtmlEscaped() +
                    "</b>" + note + "</div><table>";
        h += row("input-code",
                 QString::fromStdString(allcore::iastToInputcode(iast))
                     .toHtmlEscaped());
        h += row("next-letter",
                 QString::fromStdString(allcore::iastToNextletter(iast))
                     .toHtmlEscaped());
        h += row("Tibetanized ACIP",
                 QString::fromStdString(acip).toHtmlEscaped() +
                     (aok ? "" : " \u26a0"));
        h += row("Tibetan script",
                 "<span style='font-size:20px'>" +
                     QString::fromStdString(tib).toHtmlEscaped() +
                     "</span>" + (tok2 ? "" : " \u26a0"));
        h += row("Devanagari",
                 "<span style='font-size:18px'>" +
                     QString::fromStdString(deva).toHtmlEscaped() +
                     "</span>" + (dok ? "" : " \u26a0"));
        h += row("IPA", QString::fromStdString(ipa).toHtmlEscaped() +
                            (iok ? "" : " \u26a0"));
        h += "</table>";
        // Whitney: exact root, then PPP reverse, then meaning search
        if (whitney_) {
            auto roots = whitney_->byRoot(iast);
            if (roots.empty()) roots = whitney_->byPpp(iast);
            if (!roots.empty()) {
                h += "<hr><div style='color:#5A3B22'><b>Whitney, roots "
                     "(1885)</b> <small>(reference)</small></div>";
                int n = 0;
                for (const auto* r : roots) {
                    if (++n > 6) break;
                    h += "<div style='margin:3px 0'><b>" +
                         QString::fromStdString(r->root).toHtmlEscaped() +
                         "</b> \u2014 " +
                         QString::fromStdString(r->meaning)
                             .toHtmlEscaped() +
                         "</div>";
                }
            }
        }
        if (mvp_) {
            auto hits = mvp_->byIast(iast);
            h += mvpHtml(hits);
        }
        const QString q = QString::fromUtf8(QUrl::toPercentEncoding(
            QString::fromStdString(iast)));
        h += "<div style='color:#777;font-size:11px;margin-top:8px'>"
             "search elsewhere: <a href='https://www.sanskrit-lexicon."
             "uni-koeln.de/scans/MWScan/2020/web/webtc/indexcaller.php?"
             "key=" + q + "&input=SLP1&output=IAST'>Monier-Williams "
             "(Cologne)</a></div>";
        out_->setHtml(h);
    }

    void runOcr() {
        QString tess = QStandardPaths::findExecutable("tesseract");
        if (tess.isEmpty())
            for (const char* c : {"/opt/homebrew/bin/tesseract",
                                  "/usr/local/bin/tesseract"})
                if (QFileInfo::exists(c)) { tess = c; break; }
        const bool hasSan =
            !tess.isEmpty() &&
            QFileInfo::exists("/opt/homebrew/share/tessdata/"
                              "san.traineddata");
        if (!hasSan) {
            out_->setHtml("<div style='color:#8C2F2B'>needs tesseract + "
                          "san.traineddata (see Convert pane note)</div>");
            return;
        }
        const QString img = QFileDialog::getOpenFileName(
            this, "Devanagari image", QString(),
            "Images (*.png *.jpg *.jpeg *.tif *.tiff)");
        if (img.isEmpty()) return;
        QProcess p;
        p.start(tess, {img, "stdout", "-l", "san"});
        p.waitForFinished(60000);
        const QString deva =
            QString::fromUtf8(p.readAllStandardOutput()).trimmed();
        if (deva.isEmpty()) {
            out_->setHtml("<div style='color:#8C2F2B'>nothing "
                          "recognized</div>");
            return;
        }
        in_->setText(deva);
        analyze();
        out_->setHtml("<div style='background:#FDEEDC;padding:6px'>"
                      "\u26A0 OCR-DERIVED \u2014 review material</div>" +
                      out_->toHtml());
    }

    allcore::Mvp* mvp_;
    allcore::WhitneyRoots* whitney_;
    QLineEdit* in_ = nullptr;
    QTextBrowser* out_ = nullptr;
};

static QWidget* makeConvertPane(allcore::Mvp* mvp,
                                allcore::WhitneyRoots* whitney = nullptr) {
    auto* pane = new QWidget;
    auto* layout = new QVBoxLayout(pane);

    // colophon-date helper (allcore tibcal): year ⇄ element-animal
    auto* calRow = new QHBoxLayout;
    calRow->addWidget(new QLabel("<b>Colophon year</b>"));
    auto* calIn = new QLineEdit;
    calIn->setPlaceholderText(
        "1357 · me bya · fire bird 6 · 3 8 2011 · 2011-08-31 · "
        "2012 months · 2012 m3i");
    calRow->addWidget(calIn, 1);
    auto* calOut = new QLabel;
    calOut->setWordWrap(true);
    layout->addLayout(calRow);
    layout->addWidget(calOut);
    QObject::connect(calIn, &QLineEdit::textChanged, [calIn, calOut] {
        const QString q = calIn->text().trimmed();
        if (q.isEmpty()) { calOut->clear(); return; }
        // western calendar date (ISO): -> Tibetan date(s)
        if (q.contains('-') && q.count('-') == 2) {
            const auto p = q.split('-');
            bool ok1, ok2, ok3;
            const long Y = p[0].toLong(&ok1), M = p[1].toLong(&ok2),
                       D = p[2].toLong(&ok3);
            if (ok1 && ok2 && ok3) {
                const long jd = allcore::julianDay(D, M, Y);
                const auto ms = allcore::kckFromJulianDay(jd);
                if (ms.empty()) {
                    calOut->setText(
                        "no lunar day carries this date — an omitted "
                        "(chad) day, or outside the Modern Karana "
                        "epoch (2009 onward)");
                    return;
                }
                QStringList parts;
                for (const auto& m : ms)
                    parts << QString("tshes <b>%1</b>, month %2, %3")
                                 .arg(m.tshes)
                                 .arg(m.month)
                                 .arg(m.year);
                calOut->setText(
                    parts.join(" · ") +
                    QString(" <small>(JD %1%2)</small>")
                        .arg(jd)
                        .arg(ms.size() > 1 ? " — a duplicated (lhag) "
                                             "lunar day"
                                           : ""));
                return;
            }
        }
        // month list: "YYYY months" — the year's months with names
        // and intercalation (proven 25/25 vs the original's years)
        {
            const auto p = q.split(' ', Qt::SkipEmptyParts);
            if (p.size() == 2 && p[1].compare("months",
                                              Qt::CaseInsensitive) == 0) {
                bool oky = false;
                const long y = p[0].toLong(&oky);
                if (oky && y >= 2009) {
                    QStringList ms;
                    for (const auto& m : allcore::kckYearMonths(y))
                        ms << QString("%1%2 %3-%4%5")
                                  .arg(m.display_month)
                                  .arg(m.intercalary ? "ᵢ" : "")
                                  .arg(QString::fromStdString(
                                      m.element_en))
                                  .arg(QString::fromStdString(
                                      m.animal_en))
                                  .arg(m.female ? " (f)" : " (m)");
                    calOut->setText(
                        ms.join(" · ") +
                        " <small>(ᵢ = intercalary; type e.g. \u201c" +
                        QString::number(y) +
                        " m3\u201d for the month's days)</small>");
                    return;
                }
            }
            // month table: "YYYY mN" or "YYYY mNi" (intercalary)
            if (p.size() == 2 && p[1].startsWith('m') &&
                p[1].size() >= 2) {
                bool oky = false, okm = false;
                const long y = p[0].toLong(&oky);
                QString mm = p[1].mid(1);
                const bool wantIcal = mm.endsWith('i');
                if (wantIcal) mm.chop(1);
                const long mno = mm.toLong(&okm);
                if (oky && okm && y >= 2009 && mno >= 1 && mno <= 12) {
                    long cm = -1;
                    for (const auto& m : allcore::kckYearMonths(y))
                        if (m.display_month == mno &&
                            m.intercalary == wantIcal)
                            cm = m.true_month;
                    if (cm < 0) {
                        calOut->setText(wantIcal
                                            ? "no intercalary month "
                                              "with that number this "
                                              "year"
                                            : "month not found");
                        return;
                    }
                    QStringList rows;
                    long prevJd = -1;
                    for (long tt = 1; tt <= 30; ++tt) {
                        auto d = allcore::kckDayForTrueMonth(cm, tt);
                        auto w = allcore::westernFromJd(d.jd);
                        QString note;
                        if (prevJd >= 0 && d.jd == prevJd)
                            note = " ⟨chad — shares the civil day⟩";
                        else if (prevJd >= 0 && d.jd == prevJd + 2)
                            note = " ⟨lhag — spans two civil days⟩";
                        rows << QString("%1: %2-%3-%4%5")
                                    .arg(tt)
                                    .arg(w.year)
                                    .arg(w.month, 2, 10, QChar('0'))
                                    .arg(w.day, 2, 10, QChar('0'))
                                    .arg(note);
                        prevJd = d.jd;
                    }
                    calOut->setText(
                        rows.join(" · ") +
                        " <small>(dates verified 721/721 against the "
                        "original program's printed years)</small>");
                    return;
                }
            }
        }
        // full Tibetan date: tshes month year -> western
        {
            const auto p = q.split(' ', Qt::SkipEmptyParts);
            if (p.size() == 3) {
                bool ok1, ok2, ok3;
                const long tt = p[0].toLong(&ok1),
                           mm = p[1].toLong(&ok2),
                           yy = p[2].toLong(&ok3);
                if (ok1 && ok2 && ok3 && yy > 100) {
                    const auto d = allcore::kckModernKarana(yy, mm, tt);
                    if (!d.valid) {
                        calOut->setText(
                            "before the Modern Karana epoch (2009) — "
                            "this engine refuses rather than guesses");
                        return;
                    }
                    static const char* kDays[7] = {
                        "Saturday", "Sunday",   "Monday", "Tuesday",
                        "Wednesday", "Thursday", "Friday"};
                    const auto w = allcore::westernFromJd(d.jd);
                    calOut->setText(
                        QString("tshes %1, month %2, %3 = <b>%4, "
                                "%5-%6-%7</b> <small>(JD %8 · zla-dag "
                                "%9;%10 · gza-dag %11;%12,%13,%14,%15 — "
                                "76/76 verified against Henning's "
                                "original)</small>")
                            .arg(tt)
                            .arg(mm)
                            .arg(yy)
                            .arg(kDays[d.gza[0] % 7])
                            .arg(w.year)
                            .arg(w.month, 2, 10, QChar('0'))
                            .arg(w.day, 2, 10, QChar('0'))
                            .arg(d.jd)
                            .arg(d.zla0)
                            .arg(d.zla1)
                            .arg(d.gza[0])
                            .arg(d.gza[1])
                            .arg(d.gza[2])
                            .arg(d.gza[3])
                            .arg(d.gza[4]));
                    return;
                }
            }
        }
        bool isYear = false;
        const int g = q.toInt(&isYear);
        if (isYear && g > 0) {
            const auto y = allcore::tibetanYear(g);
            if (y.rabjung == 0) {
                calOut->setText(
                    QString("%1 is before the first rabjung (1027) — "
                            "no cycle designation.")
                        .arg(g));
                return;
            }
            calOut->setText(
                QString("<b>%1</b> begins the Tibetan year "
                        "<b>%2-%3%4</b> (%5 %6) — rabjung %7, year "
                        "%8 of 60. <small>Losar falls in Feb/Mar; "
                        "early-year western dates may belong to the "
                        "previous Tibetan year.</small>")
                    .arg(g)
                    .arg(QString::fromStdString(y.element_en),
                         QString::fromStdString(y.animal_en),
                         y.female ? " (female)" : " (male)",
                         QString::fromStdString(y.element_wylie),
                         QString::fromStdString(y.animal_wylie))
                    .arg(y.rabjung)
                    .arg(y.year_in_cycle));
            return;
        }
        const QStringList parts =
            q.split(' ', Qt::SkipEmptyParts);
        if (parts.size() < 2) {
            calOut->setText("<i>enter a western year, or: element "
                            "animal [rabjung]</i>");
            return;
        }
        const int rj =
            parts.size() > 2 ? parts[2].toInt() : 0;
        const auto years = allcore::yearsFor(
            parts[0].toStdString(), parts[1].toStdString(), rj);
        if (years.empty()) {
            calOut->setText(
                "<i>no such element-animal combination" +
                QString(rj ? " in that rabjung" : "") +
                " (parity: each element pairs with only half the "
                "animals)</i>");
            return;
        }
        QStringList ys;
        for (size_t i = 0; i < years.size() && i < 20; ++i) {
            const auto y = allcore::tibetanYear(years[i]);
            ys << QString("<b>%1</b> (rabjung %2)")
                      .arg(years[i])
                      .arg(y.rabjung);
        }
        calOut->setText(ys.join(" · ") +
                        (years.size() > 20 ? " …" : ""));
    });

    layout->addWidget(new QLabel(
        "<b>Input</b> (ACIP, EWTS wylie, or Sanskrit IAST — auto-detected)"));
    // Sanskrit OCR (optional capability, like the Tibetan OCR model):
    // enabled when tesseract + the san traineddata are installed.
    // Output is REVIEW MATERIAL — recognized Devanagari lands in the
    // input for the proven converter chain (→ IAST, Whitney), never
    // trusted as text.
    auto* saOcrBtn = new QPushButton(
        "Sanskrit OCR — recognize a Devanagari image\u2026");
    layout->addWidget(saOcrBtn);
    auto* input = new QPlainTextEdit;
    input->setPlaceholderText(
        "BSOD NAMS   ·   bsod nams   ·   SNGA DRO'I KA BA …   ·   pramāṇa");
    layout->addWidget(input, 1);
    auto* out = new QTextBrowser;
    layout->addWidget(out, 2);
    QObject::connect(saOcrBtn, &QPushButton::clicked, [pane, input, out] {
        QString tess = QStandardPaths::findExecutable("tesseract");
        if (tess.isEmpty())
            for (const char* c : {"/opt/homebrew/bin/tesseract",
                                  "/usr/local/bin/tesseract"})
                if (QFileInfo::exists(c)) { tess = c; break; }
        const bool hasSan =
            !tess.isEmpty() &&
            (QFileInfo::exists(QFileInfo(tess).path() +
                               "/../share/tessdata/san.traineddata") ||
             QFileInfo::exists("/opt/homebrew/share/tessdata/"
                               "san.traineddata"));
        if (!hasSan) {
            out->setHtml(
                "<div style='color:#8C2F2B'>Sanskrit OCR needs the "
                "tesseract engine with the Sanskrit model:<br>"
                "<code>brew install tesseract</code> and place "
                "<code>san.traineddata</code> (tessdata_best, Apache-2.0) "
                "in the tessdata folder. Recognition stays optional — "
                "the converter works without it.</div>");
            return;
        }
        const QString img = QFileDialog::getOpenFileName(
            pane, "Devanagari image", QString(),
            "Images (*.png *.jpg *.jpeg *.tif *.tiff)");
        if (img.isEmpty()) return;
        out->setHtml("<i>recognizing\u2026</i>");
        QCoreApplication::processEvents();
        QProcess p;
        p.start(tess, {img, "stdout", "-l", "san"});
        p.waitForFinished(60000);
        const QString deva =
            QString::fromUtf8(p.readAllStandardOutput()).trimmed();
        if (deva.isEmpty()) {
            out->setHtml("<div style='color:#8C2F2B'>nothing "
                         "recognized</div>");
            return;
        }
        // review-material banner + feed the proven converter
        input->setPlainText(deva);
        auto [ia, ok] = allcore::devanagariToIast(deva.toStdString());
        out->setHtml(
            "<div style='background:#FDEEDC;padding:6px'>\u26A0 "
            "OCR-DERIVED \u2014 unverified review material. Verify "
            "against the image before any further use.</div>"
            "<div style='margin-top:6px'><b>Devanagari (recognized):"
            "</b><br>" + deva.toHtmlEscaped() +
            "</div><div style='margin-top:6px'><b>IAST"
            "</b>" + QString(ok ? "" : " <small>(partial \u2014 some "
                                       "signs unconverted)</small>") +
            ":<br>" + QString::fromStdString(ia).toHtmlEscaped() +
            "</div><div style='color:#777;font-size:11px;margin-top:6px'>"
            "edit the input above and convert as usual for the full "
            "chain (ACIP styles, Tibetan, Whitney roots).</div>");
    });

    auto convert = [input, out, mvp, whitney] {
        const QString raw = input->toPlainText().trimmed();
        if (raw.isEmpty()) { out->clear(); return; }
        // Devanagari input → IAST first, then the full Sanskrit card
        bool isDeva = false;
        for (QChar c : raw)
            isDeva |= (c.unicode() >= 0x0900 && c.unicode() <= 0x097F);
        // IAST diacritics present = Sanskrit input → full converter
        static const QString kIast =
            QString::fromUtf8("āīūṛṝḷḹṅñṭḍṇśṣṃṁḥĀĪŪṚṜḶḸṄÑṬḌṆŚṢṂṀḤ");
        bool isIast = false;
        for (QChar c : raw) isIast |= kIast.contains(c);
        if (isDeva || isIast) {
            std::string t = raw.toStdString();
            bool devaOk = true;
            if (isDeva) {
                auto [ia, ok] = allcore::devanagariToIast(t);
                devaOk = ok;
                if (ok) t = ia;
            }
            auto [acip, aok] = allcore::iastToAcip(t);
            auto [tib, tok] = allcore::iastToTibetan(t);
            auto [deva, dok] = allcore::iastToDevanagari(t);
            auto [ipa, iok] = allcore::iastToIpa(t);
            QString h = "<table cellspacing='6'>";
            h += QString("<tr><td><b>detected</b></td><td>%1</td></tr>")
                     .arg(isDeva ? "Sanskrit Devanagari" : "Sanskrit IAST");
            if (isDeva)
                h += QString("<tr><td><b>IAST</b></td><td>%1</td></tr>")
                         .arg(devaOk ? QString::fromStdString(t).toHtmlEscaped()
                                     : "⟨not Sanskrit Devanagari⟩");
            auto row = [&h](const char* label, const QString& val,
                            const char* style = "") {
                h += QString("<tr><td><b>%1</b></td><td style='%2'>%3</td></tr>")
                         .arg(label, style, val.toHtmlEscaped());
            };
            row("ACIP (Tibetanized)", aok ? QString::fromStdString(acip)
                                          : "⟨cannot tokenize⟩");
            row("input-code", QString::fromStdString(allcore::iastToInputcode(t)));
            row("next-letter", QString::fromStdString(allcore::iastToNextletter(t)));
            if (aok)
                row("wylie", QString::fromStdString(allcore::acipToEwts(acip)));
            row("Tibetan", tok ? QString::fromStdString(tib) : "⟨…⟩",
                "font-size:26px");
            row("Devanagari", dok ? QString::fromStdString(deva) : "⟨…⟩",
                "font-size:22px");
            if (iok)
                row("IPA (classical)", QString::fromStdString(ipa),
                    "font-size:18px");
            row("simplified pron.",
                QString::fromStdString(allcore::iastToPronunciation(t)),
                "font-size:16px");
            h += "<tr><td></td><td><i style='color:#777;font-size:11px'>"
                 "IPA follows the standard Classical-Sanskrit mapping; the "
                 "simplified style follows the ACIP pronunciation standard "
                 "(Release IV manual, banked in docs/standards)</i></td></tr>";
            auto guide = allcore::sanskritLetterGuide(t);
            if (!guide.empty()) {
                h += "<tr><td style='vertical-align:top'><b>how to say it"
                     "</b></td><td>";
                for (auto& [letter, hint] : guide)
                    h += "<b>" + QString::fromStdString(letter).toHtmlEscaped() +
                         "</b> — " + QString::fromUtf8(hint).toHtmlEscaped() +
                         "<br>";
                h += "<i style='color:#777;font-size:11px'>letter guide per "
                     "the FPMT transliteration standard</i></td></tr>";
            }
            h += "</table>";
            if (mvp) h += mvpHtml(mvp->byIast(t));
            // Whitney roots: annotate any input token that IS a root
            // form (no stemming — inflected words are not guessed at)
            if (whitney) {
                QString wh;
                std::stringstream ts2(t);
                std::string tok2;
                std::set<std::string> seen2;
                while (ts2 >> tok2) {
                    if (!seen2.insert(tok2).second) continue;
                    for (const auto* r : whitney->byRoot(tok2)) {
                        wh += "<div style='margin:3px 0'><b>";
                        if (!r->homonym.empty())
                            wh += QString::fromStdString(r->homonym) +
                                  " ";
                        wh += "√" +
                              QString::fromStdString(r->root)
                                  .toHtmlEscaped() +
                              "</b> “" +
                              QString::fromStdString(r->meaning)
                                  .toHtmlEscaped() +
                              "”";
                        if (!r->classes.empty())
                            wh += " · class " +
                                  QString::fromStdString(r->classes)
                                      .replace("|", "·")
                                      .toHtmlEscaped();
                        if (!r->ppp.empty())
                            wh += " · ppp <b>" +
                                  QString::fromStdString(r->ppp)
                                      .toHtmlEscaped() +
                                  "</b>";
                        wh += "</div>";
                    }
                }
                if (!wh.isEmpty())
                    h += "<div style='color:#4A3A7A;margin-top:6px'><b>"
                         "Whitney — Roots (1885)</b> <small>(exact root "
                         "form in the input; reference only)</small>"
                         "</div>" +
                         wh;
            }
            out->setHtml(h);
            return;
        }
        // uppercase-dominant input = ACIP
        int upper = 0, letters = 0;
        for (QChar c : raw) {
            if (c.isLetter()) { ++letters; if (c.isUpper()) ++upper; }
        }
        const bool isAcip = letters > 0 && upper * 10 > letters * 6;
        const std::string wylie =
            isAcip ? allcore::acipToEwts(raw.toStdString()) : raw.toStdString();
        auto [uni, ok] = allcore::wylieToUnicode(wylie);
        const std::string pron = allcore::pronounce(wylie);

        QString h;
        h += "<table cellspacing='6'>";
        h += "<tr><td><b>detected</b></td><td>" +
             QString(isAcip ? "ACIP" : "EWTS wylie") + "</td></tr>";
        h += "<tr><td><b>wylie</b></td><td>" +
             QString::fromStdString(wylie).toHtmlEscaped() + "</td></tr>";
        h += "<tr><td><b>Tibetan</b></td><td style='font-size:26px'>" +
             QString::fromStdString(uni).toHtmlEscaped() + "</td></tr>";
        if (!ok)
            h += "<tr><td></td><td><i style='color:#b00'>⟨…⟩ marks syllables the "
                 "engine cannot legally convert — flagged, never guessed</i></td></tr>";
        h += "<tr><td><b>phonetics</b></td><td style='font-size:18px'>" +
             QString::fromStdString(pron).toHtmlEscaped() + "</td></tr>";
        h += "</table>";
        if (mvp) h += mvpHtml(mvp->byWylie(wylie));
        out->setHtml(h);
    };
    QObject::connect(input, &QPlainTextEdit::textChanged, convert);
    return pane;
}

// ---- Trainer pane: progressive-reveal reading tutor (docs/TRAINER_DESIGN.md)
class TrainerPane : public QWidget {
public:
    explicit TrainerPane(allcore::Spine& spine,
                         allcore::Progress* progress = nullptr,
                         allcore::PosLexicon* pos = nullptr,
                         allcore::Contractions* contractions = nullptr)
        : spine_(spine), progress_(progress), pos_(pos),
          contractions_(contractions), index_(spine) {
        auto* layout = new QVBoxLayout(this);
        layout->addWidget(new QLabel(
            "<b>Translation Trainer</b> — paste a passage, try to read it "
            "yourself, then reveal one layer at a time. Engine guidance is "
            "labeled guidance; only HGM corpus lines are answers."));
        input_ = new QPlainTextEdit;
        input_->setPlaceholderText(
            "Paste ACIP Tibetan…   e.g.  SANGS RGYAS KYIS CHOS BSTAN");
        input_->setMaximumHeight(90);
        layout->addWidget(input_);
        auto* row = new QHBoxLayout;
        auto* load = new QPushButton("Load");
        row->addWidget(load);
        const char* labels[6] = {"1 · chunks", "2 · particle roles",
                                 "3 · reading order", "4 · vocabulary",
                                 "5 · answer key", "6 · full parse (Wilson)"};
        for (int i = 0; i < 6; ++i) {
            reveal_[i] = new QCheckBox(labels[i]);
            row->addWidget(reveal_[i]);
            QObject::connect(reveal_[i], &QCheckBox::toggled, [this] { render(); });
        }
        script_ = new QCheckBox("Tibetan script");
        row->addWidget(script_);
        QObject::connect(script_, &QCheckBox::toggled, [this] { render(); });
        row->addStretch();
        layout->addLayout(row);
        view_ = new QTextBrowser;
        view_->setOpenLinks(false);   // chunk links are handled in-pane
        QObject::connect(view_, &QTextBrowser::anchorClicked,
                         [this](const QUrl& u) { onAnchor(u); });
        layout->addWidget(view_, 1);
        QObject::connect(load, &QPushButton::clicked, [this] { loadDoc(); });
    }

    int selfTest(QStringList& log) {
        int fails = 0;
        auto check = [&](bool ok, const char* what) {
            log << QString("  [%1] Trainer: %2")
                       .arg(ok ? "PASS" : "FAIL").arg(what);
            if (!ok) ++fails;
        };
        for (int k = 0; k < 6; ++k) reveal_[k]->setChecked(false);
        input_->setPlainText(
            "SEMS CAN THAMS CAD BDE BA DANG LDAN PAR GYUR CIG,");
        loadDoc();
        check(!clauses_.empty(), "passage splits into clauses");
        const int bare = view_->toPlainText().size();
        check(bare > 0, "bare layer renders");
        for (int k = 0; k < 6; ++k) reveal_[k]->setChecked(true);
        render();
        check(view_->toPlainText().size() > bare,
              "revealed layers add guidance");
        check(view_->toPlainText().contains("verb") ||
                  view_->toHtml().contains("verb"),
              "verb-first guidance present");
        for (int k = 0; k < 6; ++k) reveal_[k]->setChecked(false);
        input_->clear();
        loadDoc();
        return fails;
    }

private:
    std::string tokEwts(const std::string& tok) const {
        return docIsWylie_ ? tok : allcore::acipToEwts(tok);
    }

    void loadDoc() {
        docIsWylie_ =
            allcore::looksLikeWylie(input_->toPlainText().toStdString());
        doc_ = allcore::buildOverlay(spine_, index_,
                                     input_->toPlainText().toStdString());
        clauses_ = allcore::refineClauses(
            doc_, allcore::splitClauses(doc_.tokens, doc_.barrier_after));
        selClause_ = selChunk_ = -1;
        // "readable for you": how much of this text's dictionary vocabulary
        // the learner's own deck already covers (local progress data)
        coverageNote_.clear();
        if (progress_) {
            std::vector<std::string> words;
            for (const auto& e : doc_.entries)
                if (!e.hgm_gloss.empty()) words.push_back(e.wylie);
            std::sort(words.begin(), words.end());
            words.erase(std::unique(words.begin(), words.end()), words.end());
            if (!words.empty()) {
                const double cov = progress_->coverage(words);
                coverageNote_ = QString(
                    "<div style='color:#4A3FBF'><small>readable for you? "
                    "your deck covers ~%1%% of this text's %2 dictionary "
                    "words — words you click join the deck.</small></div>")
                                    .arg((int)(cov * 100.0 + 0.5))
                                    .arg(words.size());
            }
        }
        render();
    }

    void onAnchor(const QUrl& u) {
        const QStringList parts = u.toString().split(':');
        if (parts.size() != 3 || parts[0] != "c") return;
        const int cl = parts[1].toInt(), ch = parts[2].toInt();
        if (cl == selClause_ && ch == selChunk_) {
            selClause_ = selChunk_ = -1;   // click again to collapse
        } else {
            selClause_ = cl;
            selChunk_ = ch;
        }
        render();
    }

    QString tokensText(int beg, int end) const {
        const bool script = script_->isChecked();
        QString s;
        for (int t = beg; t < end; ++t) {
            const std::string& tok = doc_.tokens[t];
            if (script) {
                auto [u, ok] = allcore::wylieToUnicode(tokEwts(tok));
                QString disp = QString::fromStdString(u);
                if (disp.isEmpty()) disp = QString::fromStdString(tok);
                if (t > beg) s += QString::fromUtf8("་");
                s += disp;
            } else {
                if (t > beg) s += ' ';
                s += QString::fromStdString(tok);
            }
        }
        return s.toHtmlEscaped();
    }

    void render() {
        if (doc_.tokens.empty()) { view_->clear(); return; }
        const bool chunksOn = reveal_[0]->isChecked();
        const bool rolesOn = reveal_[1]->isChecked();
        const bool orderOn = reveal_[2]->isChecked();
        const bool glossOn = reveal_[3]->isChecked();
        const bool answerOn = reveal_[4]->isChecked();
        const bool parseOn = reveal_[5]->isChecked();
        QString h = coverageNote_;
        int cn = 0;
        for (const auto& cl : clauses_) {
            ++cn;
            h += QString("<div style='margin:10px 0;padding:8px;border-left:"
                         "3px solid #7F77DD'><small style='color:#777'>clause "
                         "%1</small><br>").arg(cn);
            auto chunks = allcore::chunkClause(doc_, cl);
            auto verb = allcore::spotVerb(doc_, chunks);
            auto plan = allcore::planReading(chunks, verb);
            if (!chunksOn || chunks.empty()) {
                h += "<span style='font-size:16px'>" +
                     tokensText(cl.beg, cl.end) + "</span>";
            } else {
                for (size_t i = 0; i < chunks.size(); ++i) {
                    const auto& c = chunks[i];
                    const bool sel = (cn - 1 == selClause_ &&
                                      (int)i == selChunk_);
                    h += QString("<a href='c:%1:%2' style='text-decoration:"
                                 "none;color:inherit'>").arg(cn - 1).arg(i);
                    h += QString("<span style='border:1px solid %1;"
                                 "border-radius:6px;padding:2px 6px;"
                                 "margin-right:6px;font-size:16px;"
                                 "white-space:nowrap%2'>")
                             .arg(sel ? "#4A3FBF" : "#B9B4E8",
                                  sel ? ";background:#EDEBFB" : "");
                    if (orderOn && plan[i].order > 0)
                        h += QString("<b style='color:#4A3FBF'>%1&nbsp;</b>")
                                 .arg(plan[i].order);
                    if (orderOn && plan[i].order == 0)
                        h += "<b style='color:#B4540A'>+&nbsp;</b>";
                    h += tokensText(c.beg, c.end) + "</span></a>";
                }
                // the boundary particle sits outside every chunk
                if (!cl.boundary.empty() && cl.boundary != "barrier")
                    h += "<span style='color:#7A5A00;font-size:16px'>" +
                         tokensText(cl.end - 1, cl.end) + "</span>";
                if (rolesOn) {
                    h += "<div style='margin-top:4px'>";
                    for (size_t i = 0; i < chunks.size(); ++i) {
                        h += QString("<small style='color:#555'>· <b>%1</b>"
                                     "%2 — %3</small><br>")
                                 .arg(tokensText(chunks[i].beg, chunks[i].end),
                                      chunks[i].marker.empty()
                                          ? ""
                                          : QString(" ⟨%1%2⟩").arg(
                                                QString::fromStdString(
                                                    chunks[i].marker),
                                                chunks[i].marker_fused
                                                    ? " fused" : ""),
                                      QString::fromUtf8(chunks[i].role)
                                          .toHtmlEscaped());
                    }
                    if (!cl.boundary.empty() && cl.boundary != "barrier")
                        h += QString("<small style='color:#7A5A00'>· boundary "
                                     "⟨%1⟩ — %2</small><br>")
                                 .arg(QString::fromStdString(cl.boundary),
                                      QString::fromUtf8(cl.boundary_function)
                                          .toHtmlEscaped());
                    h += "</div>";
                }
                if (orderOn) {
                    h += "<div style='margin-top:4px'>";
                    if (verb.chunk >= 0 && verb.confident) {
                        h += QString("<small style='color:#3B7A3B'>verb: "
                                     "<b>%1</b> (%2)</small><br>")
                                 .arg(QString::fromStdString(verb.wylie),
                                      QString::fromStdString(verb.evidence)
                                          .toHtmlEscaped());
                        if (verb.cls)
                            h += QString("<small style='color:#3B7A3B'>%1 — "
                                         "%2</small><br>")
                                     .arg(QString::fromUtf8(verb.cls->label)
                                              .toHtmlEscaped(),
                                          QString::fromUtf8(verb.cls->expects)
                                              .toHtmlEscaped());
                    } else
                        h += "<small style='color:#B4540A'>verb: no HGM verb "
                             "evidence in this clause — candidate unverified"
                             "</small><br>";
                    for (const auto& p : plan)
                        h += QString("<small style='color:#555'>%1 %2</small>"
                                     "<br>")
                                 .arg(p.order > 0
                                          ? QString("<b>%1.</b>").arg(p.order)
                                          : QString("+"),
                                      QString::fromUtf8(p.how).toHtmlEscaped());
                    h += "</div>";
                }
            }
            // clicked chunk: full tier-labeled entries inline
            if (cn - 1 == selClause_ && selChunk_ >= 0 &&
                selChunk_ < (int)chunks.size()) {
                const auto& c = chunks[selChunk_];
                h += "<div style='margin-top:6px;padding:6px;background:"
                     "#F5F4FB'>";
                std::set<long long> seenSel;
                for (int t = c.beg; t < c.end; ++t) {
                    for (int ix : doc_.spansAt(t)) {
                        const auto& e =
                            doc_.entries[doc_.spans[ix].entry_ix];
                        if (seenSel.insert(e.id).second) h += entryHtml(e);
                    }
                }
                if (seenSel.empty())
                    h += "<i>no dictionary entries in this chunk</i>";
                h += "</div>";
            }
            if (glossOn) {
                h += "<div style='margin-top:4px'>";
                std::set<long long> seen;
                for (int t = cl.beg; t < cl.end; ++t) {
                    auto at = doc_.spansAt(t);
                    if (at.empty()) continue;
                    const auto& e =
                        doc_.entries[doc_.spans[at.front()].entry_ix];
                    if (!seen.insert(e.id).second) continue;
                    QString gl = e.hgm_gloss.empty()
                        ? "<i>(no HGM equivalent)</i>"
                        : QString::fromStdString(e.hgm_gloss.front())
                              .toHtmlEscaped();
                    QString tier = e.provisional()
                        ? " <span style='color:#b00'>[PROVISIONAL]</span>"
                        : "";
                    h += "<small>≡ <b>" +
                         QString::fromStdString(e.wylie).toHtmlEscaped() +
                         "</b>: " + gl + tier + "</small><br>";
                }
                h += "</div>";
            }
            if (parseOn) {
                auto parses =
                    allcore::wilsonParse(spine_, doc_, {cl}, pos_, contractions_);
                if (!parses.empty()) {
                    h += "<div style='margin-top:6px;padding:6px;background:"
                         "#F4F8F4'><small style='color:#3B7A3B'><b>full parse "
                         "(Wilson designations)</b></small><br>";
                    for (const auto& u : parses[0].units) {
                        h += "<small><b>" +
                             QString::fromStdString(u.text).toHtmlEscaped() +
                             "</b> — " +
                             QString::fromStdString(u.category).toHtmlEscaped();
                        if (!u.detail.empty())
                            h += " <span style='color:#777'>(" +
                                 QString::fromStdString(u.detail)
                                     .left(90)
                                     .toHtmlEscaped() + ")</span>";
                        h += "</small><br>";
                    }
                    // the Science of the Dots line: token ·[label] token …
                    if (!parses[0].dots.empty()) {
                        h += "<small style='color:#555'><b>dots:</b> ";
                        for (const auto& d : parses[0].dots) {
                            h += QString::fromStdString(
                                     doc_.tokens[d.after_tok])
                                     .toHtmlEscaped() +
                                 " <span style='color:#4A3FBF'>·" +
                                 QString::fromStdString(d.label)
                                     .toHtmlEscaped() +
                                 "</span> ";
                        }
                        h += "</small><br>";
                    }
                    h += "</div>";
                }
            }
            if (answerOn) {
                std::string wylie;
                for (int t = cl.beg; t < cl.end; ++t) {
                    if (t > cl.beg) wylie += ' ';
                    wylie += tokEwts(doc_.tokens[t]);
                }
                auto segs = spine_.corpusSearch('"' + wylie + '"', "", 2);
                if (!segs.empty()) {
                    for (const auto& s : segs)
                        h += "<div style='margin-top:4px;background:#EEF6EE;"
                             "padding:4px'><small>[" +
                             QString::fromStdString(s.course) + ":" +
                             QString::number(s.seq) + "] <b>HGM:</b> " +
                             QString::fromStdString(s.english)
                                 .toHtmlEscaped() + "</small></div>";
                } else {
                    h += "<div style='margin-top:4px'><small style="
                         "'color:#777'>not in the corpus — the Analysis pane "
                         "can give an AI reading (labeled AI)</small></div>";
                }
            }
            h += "</div>";
        }
        view_->setHtml(h);
    }

    allcore::Spine& spine_;
    allcore::Progress* progress_ = nullptr;
    allcore::PosLexicon* pos_ = nullptr;
    allcore::Contractions* contractions_ = nullptr;
    allcore::HeadwordIndex index_;
    allcore::OverlayDoc doc_;
    bool docIsWylie_ = false;
    std::vector<allcore::Clause> clauses_;
    QString coverageNote_;
    QPlainTextEdit* input_ = nullptr;
    QTextBrowser* view_ = nullptr;
    QCheckBox* reveal_[6] = {};
    QCheckBox* script_ = nullptr;
    int selClause_ = -1, selChunk_ = -1;
};

// ---- Drills pane: corpus-generated exercises (docs/PEDAGOGY_ROADMAP.md) ----
class DrillsPane : public QWidget {
public:
    DrillsPane(allcore::Spine& spine, allcore::Progress* progress)
        : spine_(spine), progress_(progress), index_(spine),
          factory_(spine, index_, progress),
          rng_((unsigned)QDateTime::currentMSecsSinceEpoch()) {
        auto* layout = new QVBoxLayout(this);
        layout->addWidget(new QLabel(
            "<b>Drills</b> — every exercise comes from a real corpus segment; "
            "every answer is HGM's own text. Engine guidance is labeled "
            "guidance."));
        stats_ = new QLabel;
        layout->addWidget(stats_);
        auto* row = new QHBoxLayout;
        mode_ = new QComboBox;
        mode_->addItems({"Chunk order", "Cloze (fill the blank)",
                         "Particle choice", "Parallel reading",
                         "Vocabulary review (SRS)",
                         "Translate & compare"});
        row->addWidget(mode_);
        auto* newBtn = new QPushButton("New drill");
        row->addWidget(newBtn);
        script_ = new QCheckBox("Tibetan script");
        row->addWidget(script_);
        adaptive_ = new QCheckBox("adapt to my level");
        adaptive_->setChecked(progress_ != nullptr);
        adaptive_->setEnabled(progress_ != nullptr);
        factory_.setAdaptive(progress_ != nullptr);
        QObject::connect(adaptive_, &QCheckBox::toggled,
                         [this](bool on) { factory_.setAdaptive(on); });
        row->addWidget(adaptive_);
        course_ = new QComboBox;
        for (const auto& c : spine_.corpusCourses())
            course_->addItem(QString::fromStdString(c));
        course_->setVisible(false);
        row->addWidget(course_);
        row->addStretch();
        layout->addLayout(row);
        question_ = new QTextBrowser;
        layout->addWidget(question_, 2);
        answerRow_ = new QWidget;
        answerRow_->setLayout(new QVBoxLayout);
        layout->addWidget(answerRow_);
        transDraft_ = new QPlainTextEdit;
        transDraft_->setPlaceholderText(
            "your translation — write it before revealing HGM's…");
        transDraft_->setMaximumHeight(110);
        transDraft_->setVisible(false);
        layout->addWidget(transDraft_);
        auto* row2 = new QHBoxLayout;
        input_ = new QLineEdit;
        input_->setPlaceholderText("your order, e.g.  C A B  (chunk-order drills)");
        row2->addWidget(input_, 1);
        check_ = new QPushButton("Check / Reveal");
        row2->addWidget(check_);
        layout->addLayout(row2);
        result_ = new QTextBrowser;
        layout->addWidget(result_, 2);

        QObject::connect(newBtn, &QPushButton::clicked, [this] { newDrill(); });
        QObject::connect(check_, &QPushButton::clicked, [this] { checkDrill(); });
        QObject::connect(mode_, &QComboBox::currentIndexChanged, [this](int m) {
            course_->setVisible(m == 3);
            input_->setVisible(m == 0);
            transDraft_->setVisible(m == 5);
            newDrill();
        });
        QObject::connect(script_, &QCheckBox::toggled, [this] { renderQuestion(); });
        refreshStats();
    }

    int selfTest(QStringList& log) {
        int fails = 0;
        auto check = [&](bool ok, const char* what) {
            log << QString("  [%1] Drills: %2")
                       .arg(ok ? "PASS" : "FAIL").arg(what);
            if (!ok) ++fails;
        };
        mode_->setCurrentIndex(0);
        newDrill();
        check(order_.has_value(), "order drill generated from corpus");
        mode_->setCurrentIndex(1);
        newDrill();
        check(cloze_.has_value() && !cloze_->options.empty(),
              "cloze drill offers options");
        mode_->setCurrentIndex(2);
        newDrill();
        check(part_.has_value(), "particle drill generated");
        return fails;
    }

private:
    QString disp(const std::string& acipText) const {
        if (!script_->isChecked())
            return QString::fromStdString(acipText).toHtmlEscaped();
        // per-token ACIP → script via the battery-proven chain
        QString out;
        QString cur;
        auto flush = [&] {
            if (cur.isEmpty()) return;
            auto [u, ok] =
                allcore::wylieToUnicode(allcore::acipToEwts(cur.toStdString()));
            out += ok && !u.empty() ? QString::fromStdString(u) : cur;
            out += QString::fromUtf8("་");
            cur.clear();
        };
        for (QChar c : QString::fromStdString(acipText)) {
            if (c == ' ') flush();
            else cur += c;
        }
        flush();
        return out.toHtmlEscaped();
    }

    void clearAnswers() {
        auto* lay = answerRow_->layout();
        while (auto* item = lay->takeAt(0)) {
            delete item->widget();
            delete item;
        }
        radios_.clear();
    }

    void addRadios(const std::vector<std::string>& options, bool asAcip) {
        for (const auto& o : options) {
            auto* rb = new QRadioButton(
                asAcip ? disp(o) : QString::fromStdString(o));
            radios_.push_back(rb);
            answerRow_->layout()->addWidget(rb);
        }
    }

    void refreshStats() {
        if (!progress_) { stats_->clear(); return; }
        auto st = progress_->stats((long long)time(nullptr));
        QString weak;
        auto misses = progress_->topMisses(3);
        for (size_t i = 0; i < misses.size(); ++i) {
            QString name = QString::fromStdString(misses[i].first);
            name.remove(0, name.indexOf(':') + 1);   // drop the "miss:" prefix
            weak += (i ? ", " : " · weak spots: ") + name +
                    QString(" ×%1").arg(misses[i].second);
        }
        stats_->setText(
            QString("<small style='color:#555'>deck: %1 words (%2 due) · "
                    "drills: %3 (%4 right) · reading: %5 segments (%6 "
                    "peeked)%7</small>")
                .arg(st.words_tracked)
                .arg(st.due_now)
                .arg(st.drills_done)
                .arg(st.drills_correct)
                .arg(st.segments_read)
                .arg(st.segments_peeked)
                .arg(weak));
    }

    void newDrill() {
        clearAnswers();
        result_->clear();
        order_.reset();
        cloze_.reset();
        part_.reset();
        vocab_.clear();
        trans_ = allcore::CorpusSegment{};
        const int m = mode_->currentIndex();
        if (m == 0) {
            order_ = factory_.makeOrder(rng_);
        } else if (m == 1) {
            cloze_ = factory_.makeCloze(rng_);
            if (cloze_) addRadios(cloze_->options, true);
        } else if (m == 2) {
            part_ = factory_.makeParticle(rng_);
            if (part_) addRadios(part_->options, false);
        } else if (m == 3) {
            // parallel reading: load the course lazily; moving on without a
            // reveal counts as a clean read
            const std::string course = course_->currentText().toStdString();
            if (course != readCourse_) {
                readCourse_ = course;
                readSegs_ = spine_.corpusWindow(course, 0, 9999999);
                readPos_ = 0;
                revealed_ = false;
            } else if (readPos_ < readSegs_.size()) {
                if (progress_ && !revealed_)
                    progress_->recordSegmentRead(readSegs_[readPos_].id, false,
                                                 (long long)time(nullptr));
                ++readPos_;
                revealed_ = false;
            }
            if (readPos_ >= readSegs_.size()) readPos_ = 0;
        } else if (m == 5) {
            // translate & compare: a level-appropriate unseen segment
            trans_ = factory_.pickSegment(rng_);
            transDraft_->clear();
        } else if (m == 4) {
            // SRS review: next due word from the learner's own deck
            if (progress_) {
                auto due = progress_->dueWords(1, (long long)time(nullptr));
                if (!due.empty()) vocab_ = due.front();
            }
            if (!vocab_.empty())
                addRadios({"I knew it", "I did not know it"}, false);
        }
        renderQuestion();
        refreshStats();
    }

    void renderQuestion() {
        const int m = mode_->currentIndex();
        QString h;
        if (m == 0 && order_) {
            h += "<div style='color:#555'>Restore the original order of these "
                 "chunks (enter letters, e.g. <b>C A B</b>). From [" +
                 QString::fromStdString(order_->segment.course) + ":" +
                 QString::number(order_->segment.seq) + "]</div><hr>";
            for (size_t i = 0; i < order_->presented.size(); ++i)
                h += QString("<div style='font-size:18px'><b>%1)</b> %2</div>")
                         .arg(QChar('A' + (int)i))
                         .arg(disp(order_->chunks[order_->presented[i]]));
        } else if (m == 1 && cloze_) {
            h += "<div style='color:#555'>Which chunk fills the blank? HGM's "
                 "English for the whole segment:</div>"
                 "<div style='background:#EEF6EE;padding:6px'><i>" +
                 QString::fromStdString(cloze_->segment.english).toHtmlEscaped() +
                 "</i></div><hr><div style='font-size:18px'>";
            for (const auto& c : cloze_->chunks)
                h += (c == "[ ... ]" ? "<b style='color:#B4540A'>[ … ]</b>  "
                                     : disp(c) + "  ");
            h += "</div>";
        } else if (m == 2 && part_) {
            h += "<div style='color:#555'>Which particle belongs in the blank?"
                 "</div><hr><div style='font-size:18px'>";
            for (size_t i = 0; i < part_->tokens.size(); ++i) {
                const auto& t = part_->tokens[i];
                h += (t == "▢" ? "<b style='color:#B4540A'>▢</b> "
                               : disp(t) + " ");
            }
            h += "</div>";
        } else if (m == 3 && readPos_ < readSegs_.size()) {
            const auto& s = readSegs_[readPos_];
            h += "<div style='color:#555'>Read first; Check/Reveal shows "
                 "HGM's English (and counts as a peek); New drill moves on. "
                 "[" + QString::fromStdString(s.course) + ":" +
                 QString::number(s.seq) + "]</div><hr>" +
                 "<div style='font-size:20px'>" + disp(s.acip) + "</div>";
        } else if (m == 5 && trans_.id) {
            h += "<div style='color:#555'>Translate this yourself below — "
                 "then Check reveals HGM's own rendering and a terminology "
                 "diff. Nothing grades your style; the master's version "
                 "teaches. [" + QString::fromStdString(trans_.course) + ":" +
                 QString::number(trans_.seq) + "]</div><hr>" +
                 "<div style='font-size:20px'>" + disp(trans_.acip) +
                 "</div>";
        } else if (m == 4) {
            if (vocab_.empty()) {
                h += "<div style='color:#3B7A3B'>Nothing due for review — "
                     "read in the Overlay (clicked words join your deck) or "
                     "come back later.</div>";
            } else {
                std::string acip = vocab_;
                auto es = spine_.lookup(vocab_);
                if (!es.empty() && !es.front().acip.empty())
                    acip = es.front().acip;
                h += "<div style='color:#555'>Do you know this word? Grade "
                     "yourself, then Check reveals the entry.</div><hr>"
                     "<div style='font-size:24px'>" + disp(acip) + "</div>" +
                     "<div style='font-size:14px;color:#777'>" +
                     QString::fromStdString(vocab_).toHtmlEscaped() + "</div>";
            }
        } else {
            h = "<i>no drill available — press New drill</i>";
        }
        question_->setHtml(h);
    }

    int pickedRadio() const {
        for (size_t i = 0; i < radios_.size(); ++i)
            if (radios_[i]->isChecked()) return (int)i;
        return -1;
    }

    void checkDrill() {
        QString h;
        const int m = mode_->currentIndex();
        if (m == 0 && order_) {
            // parse letters like "C A B"
            std::vector<int> given;
            for (QChar c : input_->text().toUpper())
                if (c >= 'A' && c < QChar('A' + (int)order_->presented.size()))
                    given.push_back(order_->presented[c.unicode() - 'A']);
            bool right = given.size() == order_->chunks.size();
            if (right)
                for (size_t i = 0; i < given.size(); ++i)
                    right &= (given[i] == (int)i);
            h += right ? "<b style='color:#3B7A3B'>Correct.</b>"
                       : "<b style='color:#B4540A'>Not yet.</b>";
            h += "<div>Original order:</div>";
            for (size_t i = 0; i < order_->chunks.size(); ++i)
                h += QString("<div><b>%1.</b> %2</div>")
                         .arg(i + 1)
                         .arg(disp(order_->chunks[i]));
            h += "<div style='background:#EEF6EE;padding:6px;margin-top:6px'>"
                 "<b>HGM:</b> " +
                 QString::fromStdString(order_->segment.english).toHtmlEscaped() +
                 "</div>";
            if (order_->verb.confident)
                h += "<small style='color:#3B7A3B'>verb: " +
                     QString::fromStdString(order_->verb.wylie) + " (" +
                     QString::fromStdString(order_->verb.evidence).toHtmlEscaped() +
                     ")</small><br>";
            for (const auto& p : order_->plan)
                h += QString("<small style='color:#555'>%1 %2</small><br>")
                         .arg(p.order > 0 ? QString("<b>%1.</b>").arg(p.order)
                                          : QString("+"),
                              QString::fromUtf8(p.how).toHtmlEscaped());
        } else if (m == 1 && cloze_) {
            const int pick = pickedRadio();
            h += (pick == cloze_->correct)
                     ? "<b style='color:#3B7A3B'>Correct.</b>"
                     : QString("<b style='color:#B4540A'>Not yet — the answer "
                               "is %1.</b>")
                           .arg(disp(cloze_->options[cloze_->correct]));
            h += "<div><small>role of the blanked chunk: " +
                 QString::fromUtf8(cloze_->role).toHtmlEscaped() +
                 "</small></div>";
        } else if (m == 2 && part_) {
            const int pick = pickedRadio();
            h += (pick == part_->correct)
                     ? "<b style='color:#3B7A3B'>Correct.</b>"
                     : QString("<b style='color:#B4540A'>Not yet — the text "
                               "has \"%1\".</b>")
                           .arg(QString::fromStdString(
                               part_->options[part_->correct]));
            h += "<div><small>" +
                 QString::fromStdString(part_->explanation).toHtmlEscaped() +
                 "</small></div>";
            h += "<div style='background:#EEF6EE;padding:6px;margin-top:6px'>"
                 "<b>HGM:</b> " +
                 QString::fromStdString(part_->segment.english).toHtmlEscaped() +
                 "</div>";
        } else if (m == 3 && readPos_ < readSegs_.size()) {
            const auto& s = readSegs_[readPos_];
            h += "<div style='background:#EEF6EE;padding:6px'><b>HGM:</b> " +
                 QString::fromStdString(s.english).toHtmlEscaped() + "</div>";
            if (progress_ && !revealed_)
                progress_->recordSegmentRead(s.id, true,
                                             (long long)time(nullptr));
            revealed_ = true;
        } else if (m == 5 && trans_.id) {
            h += "<div style='background:#EEF6EE;padding:6px'><b>HGM:</b> " +
                 QString::fromStdString(trans_.english).toHtmlEscaped() +
                 "</div>";
            const std::string draft = transDraft_->toPlainText().toStdString();
            if (!draft.empty()) {
                auto rep = allcore::checkTerminology(spine_, index_,
                                                     trans_.acip, draft);
                int matched = 0;
                QString un;
                for (const auto& t : rep.terms) {
                    if (!t.matched.empty()) { ++matched; continue; }
                    if (un.count('\n') < 6) {
                        QString gl;
                        int shown = 0;
                        for (const auto& g : t.glosses) {
                            if (shown++ >= 2) break;
                            gl += (shown > 1 ? " · " : "") +
                                  QString::fromStdString(g).toHtmlEscaped();
                        }
                        un += "<br><small>○ <b>" +
                              QString::fromStdString(t.wylie).toHtmlEscaped() +
                              "</b> — HGM has: " + gl + "</small>";
                    }
                }
                h += QString("<div style='margin-top:4px'><small>terminology: "
                             "%1 of %2 terms matched an HGM equivalent in "
                             "your draft</small>%3</div>")
                         .arg(matched)
                         .arg(rep.terms.size())
                         .arg(un);
                if (progress_)
                    progress_->recordDrill(
                        "translate", std::to_string(trans_.id),
                        rep.terms.empty() ||
                            matched * 2 >= (int)rep.terms.size(),
                        (long long)time(nullptr));
            } else {
                h += "<small style='color:#777'>write your translation above "
                     "before checking — the comparison is the exercise.</small>";
            }
        } else if (m == 4 && !vocab_.empty()) {
            const int pick = pickedRadio();
            auto es = spine_.lookup(vocab_);
            if (!es.empty()) h += entryHtml(es.front());
            if (progress_ && pick >= 0) {
                progress_->reviewWord(vocab_, pick == 0,
                                      (long long)time(nullptr));
                h += pick == 0 ? "<small style='color:#3B7A3B'>scheduled "
                                 "further out</small>"
                               : "<small style='color:#B4540A'>will retry "
                                 "soon</small>";
            } else if (pick < 0) {
                h += "<small style='color:#777'>grade yourself (pick one) to "
                     "schedule the next review</small>";
            }
        }
        // drill results feed the progress log; misses are classified by the
        // skill they reveal (the taxonomy that names the weak spot)
        if (progress_ && pickedRadio() >= 0) {
            const long long now = (long long)time(nullptr);
            if (m == 1 && cloze_) {
                const bool right = pickedRadio() == cloze_->correct;
                progress_->recordDrill("cloze",
                                       std::to_string(cloze_->segment.id),
                                       right, now);
                if (!right) {
                    std::string role = cloze_->role;
                    role = role.substr(0, role.find(' '));
                    progress_->recordDrill("miss:cloze-role:" + role,
                                           std::to_string(cloze_->segment.id),
                                           false, now);
                }
            } else if (m == 2 && part_) {
                const bool right = pickedRadio() == part_->correct;
                progress_->recordDrill("particle",
                                       std::to_string(part_->segment.id),
                                       right, now);
                if (!right)
                    progress_->recordDrill(
                        "miss:particle:" + part_->options[0] + "-family",
                        std::to_string(part_->segment.id), false, now);
            }
        }
        if (progress_ && m == 0 && order_ && !input_->text().isEmpty()) {
            std::vector<int> given;
            for (QChar c : input_->text().toUpper())
                if (c >= 'A' && c < QChar('A' + (int)order_->presented.size()))
                    given.push_back(order_->presented[c.unicode() - 'A']);
            bool right = given.size() == order_->chunks.size();
            int firstWrong = -1;
            if (right)
                for (size_t i = 0; i < given.size(); ++i)
                    if (given[i] != (int)i) {
                        right = false;
                        firstWrong = (int)i;
                        break;
                    }
            const long long now = (long long)time(nullptr);
            progress_->recordDrill("order", std::to_string(order_->segment.id),
                                   right, now);
            if (!right && firstWrong >= 0 &&
                firstWrong < (int)order_->markers.size()) {
                const std::string& mk = order_->markers[firstWrong];
                std::string skill = "chunk-order";
                if (mk.empty() &&
                    firstWrong == (int)order_->chunks.size() - 1)
                    skill = "verb-position";
                else if (mk == "gi" || mk == "kyi" || mk == "gyi" ||
                         mk == "yi" || mk == "'i")
                    skill = "genitive-attach";
                else if (mk == "gis" || mk == "kyis" || mk == "gyis" ||
                         mk == "yis" || mk == "s")
                    skill = "agent-chunk";
                else if (mk == "la" || mk == "du" || mk == "ru" ||
                         mk == "su" || mk == "tu" || mk == "r" || mk == "na")
                    skill = "ladon-chunk";
                progress_->recordDrill("miss:order:" + skill,
                                       std::to_string(order_->segment.id),
                                       false, now);
            }
        }
        refreshStats();
        result_->setHtml(h);
    }

    allcore::Spine& spine_;
    allcore::Progress* progress_ = nullptr;
    allcore::HeadwordIndex index_;
    allcore::DrillFactory factory_;
    std::mt19937 rng_;
    std::string vocab_;
    allcore::CorpusSegment trans_;
    QPlainTextEdit* transDraft_ = nullptr;
    bool revealed_ = false;
    QLabel* stats_ = nullptr;
    std::optional<allcore::OrderDrill> order_;
    std::optional<allcore::ClozeDrill> cloze_;
    std::optional<allcore::ParticleDrill> part_;
    std::vector<allcore::CorpusSegment> readSegs_;
    std::string readCourse_;
    size_t readPos_ = 0;
    QComboBox* mode_ = nullptr;
    QComboBox* course_ = nullptr;
    QCheckBox* script_ = nullptr;
    QCheckBox* adaptive_ = nullptr;
    QTextBrowser* question_ = nullptr;
    QWidget* answerRow_ = nullptr;
    std::vector<QRadioButton*> radios_;
    QLineEdit* input_ = nullptr;
    QPushButton* check_ = nullptr;
    QTextBrowser* result_ = nullptr;
};

// ---- Draft pane: the working translator's workspace ------------------------
// Source clauses + tier-labeled anchors + concordance on the left; the
// English draft + terminology report below. The engine anchors and checks —
// it never composes English (rule 1).
class DraftPane : public QWidget {
public:
    DraftPane(allcore::Spine& spine, allcore::Progress* progress,
              const QString& root = QString())
        : spine_(spine), progress_(progress), index_(spine), root_(root) {
        auto* layout = new QVBoxLayout(this);
        auto* row = new QHBoxLayout;
        row->addWidget(new QLabel(
            "<b>Draft workspace</b> — paste the source, click a clause for "
            "its anchors, click a term for its corpus concordance. The "
            "terminology check matches your draft against HGM's equivalents; "
            "it never writes English for you."));
        layout->addLayout(row);
        auto* split = new QSplitter(Qt::Vertical);

        auto* top = new QWidget;
        auto* tl = new QHBoxLayout(top);
        auto* srcCol = new QVBoxLayout;
        source_ = new QPlainTextEdit;
        source_->setPlaceholderText("Source ACIP…");
        srcCol->addWidget(source_);
        auto* loadBtn = new QPushButton("Load source");
        srcCol->addWidget(loadBtn);
auto* secStruct = new QLabel("<span style='color:#9A7A33;font-size:10px;letter-spacing:2px;font-weight:600'>STRUCTURE</span>");
        secStruct->setContentsMargins(0, 8, 0, 0);
        srcCol->addWidget(secStruct);
                auto* outlineBtn = new QPushButton("Extract outline (sa bcad)");
        srcCol->addWidget(outlineBtn);
                auto* structBtn = new QPushButton("Structural units (bam po / le'u)");
        structBtn->setToolTip(
            "Explicit BAM PO and numbered LE'U markers written in the text "
            "(authoritative), plus a syllable-derived shloka/bampo ESTIMATE "
            "(30-syllable prose shloka, 300-shloka bampo).");
        srcCol->addWidget(structBtn);
        auto* verseBtn = new QPushButton("Verse meter");
        srcCol->addWidget(verseBtn);
        auto* verseReadBtn = new QPushButton("Verse reading order");
        verseReadBtn->setToolTip(
            "Verse does not read line by line: the meter forces the "
            "poet to displace words and drop understood particles, so "
            "the reading unit is the STANZA. This groups the lines into "
            "shlokas (four verse lines, or the poet's own double-shad "
            "closes) and applies the verb-first reading order to each "
            "stanza as a whole.");
        srcCol->addWidget(verseReadBtn);
        QObject::connect(verseReadBtn, &QPushButton::clicked,
                         [this] { verseReading(); });
auto* secEvid = new QLabel("<span style='color:#9A7A33;font-size:10px;letter-spacing:2px;font-weight:600'>EVIDENCE</span>");
        secEvid->setContentsMargins(0, 8, 0, 0);
        srcCol->addWidget(secEvid);
                auto* quoteBtn = new QPushButton("Detect quotations");
        quoteBtn->setToolTip(
            "Finds passages that exactly match corpus segments (7+ "
            "syllables) — attested quotations, never inferred. Matched "
            "works recommend their published bibliography entries.");
        srcCol->addWidget(quoteBtn);
        auto* memBtn = new QPushButton("Phrase memory");
        memBtn->setToolTip(
            "How has this phrase been rendered before? Select Tibetan "
            "in the source and press: every corpus segment containing "
            "it (HGM's own renderings, binding) plus any matches from "
            "the Align pane's harvest (translator-authored, PENDING — "
            "labeled). The beginnings of a Project translation memory, "
            "built only from attested work.");
        srcCol->addWidget(memBtn);
        QObject::connect(memBtn, &QPushButton::clicked,
                         [this] { phraseMemory(); });
        QObject::connect(quoteBtn, &QPushButton::clicked,
                         [this] { detectQuotes(); });
        // same crush class as the Overlay column (fit sweep,
        // 2026-08-11): the source column demands >800px — scroll,
        // never squeeze
        auto* srcWrap = new QWidget;
        srcWrap->setLayout(srcCol);
        auto* srcScroll = new QScrollArea;
        srcScroll->setWidget(srcWrap);
        srcScroll->setWidgetResizable(true);
        srcScroll->setFrameShape(QFrame::NoFrame);
        srcScroll->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        tl->addWidget(srcScroll, 1);
        clauseView_ = new QTextBrowser;
        clauseView_->setOpenLinks(false);
        tl->addWidget(clauseView_, 1);
        anchors_ = new QTextBrowser;
        anchors_->setOpenLinks(false);
        tl->addWidget(anchors_, 1);
        split->addWidget(top);

        auto* bottom = new QWidget;
        auto* bl = new QHBoxLayout(bottom);
        auto* draftCol = new QVBoxLayout;
        draft_ = new QPlainTextEdit;
        draft_->setPlaceholderText("Your English draft…");
        draftCol->addWidget(draft_);
        auto* checkBtn = new QPushButton("Check terminology");
        draftCol->addWidget(checkBtn);
        notesSearch_ = new QLineEdit;
        notesSearch_->setPlaceholderText(
            "search the shared apparatus: footnotes + bibliography "
            "(GMR: reuse released work)…");
        draftCol->addWidget(notesSearch_);
        QObject::connect(notesSearch_, &QLineEdit::returnPressed,
                         [this] { searchNotesBank(); });
auto* secPub = new QLabel("<span style='color:#9A7A33;font-size:10px;letter-spacing:2px;font-weight:600'>PUBLISH</span>");
        secPub->setContentsMargins(0, 8, 0, 0);
        draftCol->addWidget(secPub);
                auto* rtfBtn = new QPushButton("Export draft (RTF, *italics*)…");
        rtfBtn->setToolTip(
            "Writes the draft as RTF for the publishing workflow. Text "
            "between *asterisks* becomes italic — STD-004: name-parts "
            "woven into a verse are italicized in the English.");
        draftCol->addWidget(rtfBtn);
        QObject::connect(rtfBtn, &QPushButton::clicked,
                         [this] { exportRtf(); });
        auto* composeBib = new QPushButton("Compose bibliography entry…");
        composeBib->setToolTip(
            "Assembles an entry in the published house format (STD-007). "
            "Fields are used exactly as typed — enter technical "
            "spelling as it should print.");
        draftCol->addWidget(composeBib);
        QObject::connect(composeBib, &QPushButton::clicked,
                         [this] { composeBibDialog(); });
        auto* proposeBtn =
            new QPushButton("Propose footnote (pending GMR approval)");
        proposeBtn->setToolTip(
            "Saves the selected draft text as a CANDIDATE note. Candidates "
            "stay in the pending queue — the official MAIN apparatus only "
            "ever contains published, GMR-approved notes.");
        draftCol->addWidget(proposeBtn);
        QObject::connect(proposeBtn, &QPushButton::clicked,
                         [this] { proposeNote(); });
        aiBtn_ = new QPushButton("AI back-check (API, labeled AI)");
        if (qgetenv("ANTHROPIC_API_KEY").isEmpty()) {
            aiBtn_->setEnabled(false);
            aiBtn_->setToolTip("ANTHROPIC_API_KEY is not set");
        }
        draftCol->addWidget(aiBtn_);
        bl->addLayout(draftCol, 1);
        report_ = new QTextBrowser;
        report_->setOpenLinks(false);
        QObject::connect(report_, &QTextBrowser::anchorClicked,
                         [this](const QUrl& u) {
                             const QString s = u.toString();
                             if (s.startsWith("note:"))
                                 insertNote(s.mid(5).toInt());
                             else if (s.startsWith("bib:"))
                                 insertBibEntry(s.mid(4).toInt());
                             else if (s.startsWith("cand:"))
                                 insertCandidate(s.mid(5).toInt());
                         });
        bl->addWidget(report_, 1);
        split->addWidget(bottom);
        layout->addWidget(split, 1);

        QObject::connect(loadBtn, &QPushButton::clicked, [this] { load(); });
        QObject::connect(outlineBtn, &QPushButton::clicked,
                         [this] { outline(); });
        QObject::connect(structBtn, &QPushButton::clicked,
                         [this] { structuralUnits(); });
        QObject::connect(verseBtn, &QPushButton::clicked, [this] { verse(); });
        QObject::connect(aiBtn_, &QPushButton::clicked, [this] { aiCheck(); });
        QObject::connect(checkBtn, &QPushButton::clicked, [this] { check(); });
        QObject::connect(clauseView_, &QTextBrowser::anchorClicked,
                         [this](const QUrl& u) {
                             auto parts = u.toString().split(':');
                             if (parts.size() == 2 && parts[0] == "c")
                                 showAnchors(parts[1].toInt());
                         });
        QObject::connect(anchors_, &QTextBrowser::anchorClicked,
                         [this](const QUrl& u) {
                             auto s = u.toString();
                             if (s.startsWith("ts:")) {
                                 // STD-002: insert the apparatus form at the
                                 // draft cursor — pronunciation spelling +
                                 // "(technical spelling: …)"
                                 const std::string wylie =
                                     s.mid(3).toStdString();
                                 std::string pron;
                                 auto es = spine_.lookup(wylie);
                                 if (!es.empty()) pron = es.front().pronunciation;
                                 QString ins =
                                     (pron.empty()
                                          ? ""
                                          : QString::fromStdString(pron) + " ") +
                                     "(technical spelling: " +
                                     QString::fromStdString(
                                         allcore::hgmTechnicalSpelling(wylie)) +
                                     ")";
                                 draft_->textCursor().insertText(ins);
                                 draft_->setFocus();
                             } else if (s.startsWith("t:"))
                                 showConcordance(s.mid(2).toStdString());
                             else if (s == "back" && lastClause_ >= 0)
                                 showAnchors(lastClause_);
                             else if (s.startsWith("http"))
                                 QDesktopServices::openUrl(u);  // link-out tier
                         });
    }

    // demo/screenshot hook: load a text and open the first clause's
    // color-linked anchors
    void demo(const QString& text) {
        source_->setPlainText(text);
        load();
        if (!clauses_.empty()) showAnchors(0);
    }

    int selfTest(QStringList& log) {
        int fails = 0;
        auto check = [&](bool ok, const char* what) {
            log << QString("  [%1] Draft: %2")
                       .arg(ok ? "PASS" : "FAIL")
                       .arg(what);
            if (!ok) ++fails;
        };
        demo("/ /blo sbyong snyan brgyud chen mo'i 'khrid yig "
             "/sems can thams cad bde ba dang ldan par gyur cig /");
        check(!clauses_.empty(), "clauses split");
        const QString a = anchors_->toHtml().toLower();
        check(a.contains("d9efda") || a.contains("d8e9f7") ||
                  a.contains("fae8d8") || a.contains("f7e3ea"),
              "anchor hue bands render");
        check(anchors_->toPlainText().contains(QString::fromUtf8("≡")),
              "HGM glosses listed");
        showConcordance("sems can");
        check(anchors_->toPlainText().contains("corpus hit"),
              "term concordance reaches the corpus");
        source_->setPlainText("sems can thams cad");
        QTextCursor c(source_->document());
        c.select(QTextCursor::Document);
        source_->setTextCursor(c);
        phraseMemory();
        check(anchors_->toPlainText().contains("HGM corpus"),
              "phrase memory finds corpus renderings");
        source_->clear();
        return fails;
    }

private:
    std::string tokEwts(const std::string& tok) const {
        return docIsWylie_ ? tok : allcore::acipToEwts(tok);
    }

    void load() {
        docIsWylie_ =
            allcore::looksLikeWylie(source_->toPlainText().toStdString());
        doc_ = allcore::buildOverlay(spine_, index_,
                                     source_->toPlainText().toStdString());
        clauses_ = allcore::refineClauses(
            doc_, allcore::splitClauses(doc_.tokens, doc_.barrier_after));
        QString h;
        for (size_t i = 0; i < clauses_.size(); ++i) {
            const auto& c = clauses_[i];
            QString text;
            for (int t = c.beg; t < c.end; ++t) {
                if (t > c.beg) text += ' ';
                text += QString::fromStdString(doc_.tokens[t]);
            }
            h += QString("<div style='margin:4px 0'><a href='c:%1' "
                         "style='text-decoration:none'><small style="
                         "'color:#777'>clause %2</small> %3</a></div>")
                     .arg(i)
                     .arg(i + 1)
                     .arg(text.toHtmlEscaped());
        }
        clauseView_->setHtml(h.isEmpty() ? "<i>no clauses</i>" : h);
        anchors_->clear();
        lastClause_ = -1;
    }

    // maximal glossed, non-particle spans overlapping the clause
    std::vector<int> termSpans(const allcore::Clause& cl) const {
        std::vector<int> out;
        for (int i = 0; i < (int)doc_.spans.size(); ++i) {
            const auto& s = doc_.spans[i];
            if (s.end <= cl.beg || s.beg >= cl.end) continue;
            const auto& e = doc_.entries[s.entry_ix];
            if (e.hgm_gloss.empty()) continue;
            if (s.end - s.beg == 1) {
                std::string up = e.wylie;
                for (auto& c : up)
                    if (c >= 'a' && c <= 'z') c = (char)(c - 'a' + 'A');
                if (allcore::classifyParticle(up)) continue;
            }
            bool contained = false;
            for (const auto& t : doc_.spans) {
                if (&t == &s ||
                    doc_.entries[t.entry_ix].hgm_gloss.empty())
                    continue;
                if (t.beg <= s.beg && s.end <= t.end &&
                    (t.end - t.beg) > (s.end - s.beg))
                    contained = true;
            }
            if (!contained) out.push_back(i);
        }
        return out;
    }

    void showAnchors(int ci) {
        if (ci < 0 || ci >= (int)clauses_.size()) return;
        lastClause_ = ci;
        const auto terms = termSpans(clauses_[ci]);
        // color link (GMR course-material style): the k-th anchored
        // term and its gloss entry share one hue — which English goes
        // with which Tibetan reads by color
        static const char* hues[4] = {"#D9EFDA", "#D8E9F7", "#FAE8D8",
                                      "#F7E3EA"};
        auto hueOf = [&](int spanIx) -> const char* {
            for (size_t k = 0; k < terms.size(); ++k)
                if (terms[k] == spanIx) return hues[k % 4];
            return nullptr;
        };
        QString h = QString("<div style='color:#777'>anchors — clause %1"
                            "</div>").arg(ci + 1);
        // the clause itself in script, terms banded in their hues
        {
            const auto& cl = clauses_[ci];
            QString line;
            for (int t = cl.beg; t < cl.end; ++t) {
                // the innermost-of-the-maximal terms covering t
                const char* hue = nullptr;
                bool opens = false, closes = false;
                for (size_t k = 0; k < terms.size(); ++k) {
                    const auto& sp = doc_.spans[terms[k]];
                    if (sp.beg <= t && t < sp.end) {
                        hue = hues[k % 4];
                        opens = (t == sp.beg);
                        closes = (t == sp.end - 1);
                        break;
                    }
                }
                auto [u, okc] =
                    allcore::wylieToUnicode(tokEwts(doc_.tokens[t]));
                QString disp = okc && !u.empty()
                                   ? QString::fromStdString(u)
                                   : QString::fromStdString(doc_.tokens[t])
                                         .toHtmlEscaped();
                const QString tsheg = QString::fromUtf8("་");
                if (hue) {
                    if (opens)
                        line += QString("<span style='background:%1'>")
                                    .arg(hue);
                    line += disp;
                    line += closes ? QString("</span>") + tsheg
                                   : tsheg;
                } else {
                    line += disp + tsheg;
                }
            }
            h += "<div style='font-size:20px;margin:6px 0'>" + line +
                 "</div>";
        }
        h += "<hr>";
        for (int ix : terms) {
            const auto& e = doc_.entries[doc_.spans[ix].entry_ix];
            QString w = QString::fromStdString(e.wylie).toHtmlEscaped();
            const char* hue = hueOf(ix);
            h += "<div style='margin:4px 0'><a href='t:" + w +
                 QString("'><b style='background:%1;padding:1px 4px;"
                         "border-radius:3px'>")
                     .arg(hue ? hue : "transparent") +
                 w + "</b></a> <a href='ts:" + w +
                 "' style='font-size:11px;color:#7A5A00'>[+ technical "
                 "spelling]</a>";
            QString tier = e.provisional()
                ? " <span style='color:#b00;font-size:11px'>[PROVISIONAL]"
                  "</span>"
                : QString(" <small>[%1]</small>")
                      .arg(QString::fromStdString(e.tier));
            h += tier + "<br>";
            int shown = 0;
            for (const auto& g : e.hgm_gloss) {
                if (shown++ >= 3) break;
                h += "<small>≡ " + QString::fromStdString(g).toHtmlEscaped() +
                     "</small><br>";
            }
            h += "</div>";
        }
        anchors_->setHtml(h);
    }

    void showConcordance(const std::string& wylie) {
        if (progress_)
            progress_->touchWord(wylie, (long long)time(nullptr));
        auto segs = spine_.corpusSearch('"' + wylie + '"', "", 200);
        std::map<std::string, int> byCourse;
        for (const auto& s : segs) ++byCourse[s.course];
        QString h = "<div><a href='back'>← back to anchors</a></div>"
                    "<div style='margin-top:4px'><b>" +
                    QString::fromStdString(wylie).toHtmlEscaped() +
                    "</b> — " + QString::number(segs.size()) +
                    " corpus hit(s)</div>";
        h += "<div style='color:#777;font-size:11px'>";
        for (auto& [c, n] : byCourse)
            h += QString::fromStdString(c) + " ×" + QString::number(n) + " · ";
        h += "</div><hr>";
        int shown = 0;
        for (const auto& s : segs) {
            if (shown++ >= 8) break;
            h += "<div style='margin:5px 0'><small>[" +
                 QString::fromStdString(s.course) + ":" +
                 QString::number(s.seq) + "]</small><br><i>" +
                 QString::fromStdString(s.wylie).left(160).toHtmlEscaped() +
                 "</i><br>" +
                 QString::fromStdString(s.english).left(200).toHtmlEscaped() +
                 "</div>";
        }
        h += linkOutHtml(wylie);
        anchors_->setHtml(h);
    }

    void outline() {
        std::vector<std::string> toks;
        std::vector<bool> bars;
        allcore::tokenizeDocument(source_->toPlainText().toStdString(), toks,
                                  bars);
        auto root = allcore::extractOutline(toks, bars);
        QString h = "<div><b>sa bcad — topical outline</b></div>";
        if (!root.heading.empty())
            h += "<div style='color:#555'>" +
                 QString::fromStdString(root.heading).toHtmlEscaped() +
                 QString(" — %1 part(s) announced</div>").arg(root.announced);
        h += "<hr>";
        int total = 0;
        std::function<void(const allcore::OutlineNode&, int)> dump =
            [&](const allcore::OutlineNode& node, int depth) {
                if (depth > 0) {
                    ++total;
                    h += QString("<div style='margin-left:%1px'>"
                                 "<b>%2</b>%3 %4%5</div>")
                             .arg(depth * 18)
                             .arg(QString::fromStdString(node.label))
                             .arg(node.irregular
                                      ? " <span style='color:#B4540A'>"
                                        "[out of sequence]</span>"
                                      : "")
                             .arg(QString::fromStdString(node.heading)
                                      .left(90)
                                      .toHtmlEscaped())
                             .arg(node.announced > 0
                                      ? QString(" <small style='color:#777'>"
                                                "(→ %1 parts)</small>")
                                            .arg(node.announced)
                                      : QString());
                }
                for (const auto& c : node.children) dump(c, depth + 1);
            };
        dump(root, 0);
        if (total == 0)
            h += "<i>no sa bcad markers found in this text</i>";
        report_->setHtml(h);
    }

    // Canonical structural units (survey ⑥): explicit BAM PO / LE'U
    // markers are authoritative; the shloka/bampo numbers derived from
    // syllable counts are ESTIMATES (segmentation-units.md: shloka = 30
    // syllables of prose or 4 verse lines; bampo = 300 shlokas) and are
    // labeled as such — never presented as counts.
    void structuralUnits() {
        std::vector<std::string> toks;
        std::vector<bool> bars;
        allcore::tokenizeDocument(source_->toPlainText().toStdString(), toks,
                                  bars);
        auto st = allcore::extractStructure(toks, bars);
        QString h = "<div><b>Structural units — bam po / le'u</b></div>";
        h += QString("<div style='color:#555'>%1 syllables · shloka "
                     "<i>estimate</i> %2 · bampo <i>estimate</i> %3 "
                     "<small style='color:#777'>(30-syllable prose shloka, "
                     "300-shloka bampo — edition-dependent estimates, not "
                     "counts)</small></div><hr>")
                 .arg(st.syllables)
                 .arg(st.shlokaEstimate(), 0, 'f', 1)
                 .arg(st.bampoEstimate(), 0, 'f', 2);
        if (!st.bampos.empty()) {
            h += QString("<div><b>%1 explicit bam po marker(s)</b> "
                         "(written in the text — authoritative)</div>")
                     .arg(st.bampos.size());
            if (st.preamble_syllables > 0)
                h += QString("<div style='color:#777'>preamble before "
                             "first marker: %1 syllables</div>")
                         .arg(st.preamble_syllables);
            for (const auto& m : st.bampos)
                h += QString("<div style='margin-left:14px'>%1%2 "
                             "<small style='color:#777'>%3 syllables"
                             "</small>%4</div>")
                         .arg(m.number > 0
                                  ? QString("<b>bam po %1</b>").arg(m.number)
                                  : "<b>bam po ?</b>")
                         .arg(QString(" — <i>%1</i>")
                                  .arg(QString::fromStdString(m.label)
                                           .toHtmlEscaped()))
                         .arg(m.syllables)
                         .arg(m.irregular
                                  ? " <span style='color:#B4540A'>[out of "
                                    "sequence — gap or omission in the "
                                    "source]</span>"
                                  : "");
        } else {
            h += "<div><i>no explicit bam po markers in this text</i></div>";
        }
        if (!st.chapters.empty()) {
            h += QString("<hr><div><b>%1 numbered le'u (chapter) "
                         "colophon(s)</b></div>")
                     .arg(st.chapters.size());
            for (const auto& m : st.chapters)
                h += QString("<div style='margin-left:14px'><b>le'u %1</b> "
                             "— <i>%2</i>%3</div>")
                         .arg(m.number)
                         .arg(QString::fromStdString(m.label).toHtmlEscaped())
                         .arg(m.irregular
                                  ? " <span style='color:#B4540A'>[out of "
                                    "sequence]</span>"
                                  : "");
        }
        report_->setHtml(h);
    }

    void verse() {
        auto v = allcore::analyzeVerse(source_->toPlainText().toStdString());
        QString h = "<div><b>Verse meter</b></div>";
        if (v.is_verse)
            h += QString("<div style='color:#3B7A3B'>verse detected — "
                         "%1-syllable meter, %2 line(s), %3 irregular</div>"
                         "<div style='color:#777;font-size:11px'>register "
                         "note: verse translates differently than prose "
                         "(single-register warnings apply)</div>")
                     .arg(v.meter)
                     .arg(v.lines.size())
                     .arg(v.irregular_count);
        else
            h += QString("<div style='color:#777'>no dominant meter — reads "
                         "as prose (%1 line(s))</div>")
                     .arg(v.lines.size());
        h += "<hr>";
        for (const auto& l : v.lines)
            h += QString("<div%1><small style='color:#777'>%2 · %3 syl</small> "
                         "%4</div>")
                     .arg(l.irregular
                              ? " style='background:#FDEEDC'"
                              : "")
                     .arg(l.number)
                     .arg(l.syllables)
                     .arg(QString::fromStdString(l.text).toHtmlEscaped());
        report_->setHtml(h);
    }

    // Verse reading order (leadership rec #4): verse reads by the
    // STANZA, not the line — the meter displaces words and drops
    // understood particles. Group into shlokas, then run the verb-first
    // reading plan over each assembled stanza.
    void verseReading() {
        const std::string src = source_->toPlainText().toStdString();
        auto v = allcore::analyzeVerse(src);
        QString h = "<div><b>Verse reading order</b></div>";
        if (!v.is_verse) {
            h += "<div style='color:#777'>no dominant meter — this reads "
                 "as prose; use the Trainer's reading order instead.</div>";
            report_->setHtml(h);
            return;
        }
        auto stanzas = allcore::groupStanzas(v, src);
        h += QString("<div style='color:#3B7A3B'>%1-syllable verse · %2 "
                     "reading unit(s)</div><div style='color:#777;"
                     "font-size:11px'>Read each stanza as ONE unit, not "
                     "line by line: the meter moves words out of their "
                     "prose order, and case particles are often dropped "
                     "(understood) to fit — supply them as you "
                     "assemble.</div><hr>")
                     .arg(v.meter)
                     .arg(stanzas.size());
        int sn = 0;
        for (const auto& st : stanzas) {
            ++sn;
            h += QString("<div style='margin-top:8px'><b>Stanza %1</b> "
                         "<small style='color:#777'>(lines %2–%3%4)"
                         "</small></div>")
                     .arg(sn)
                     .arg(st.first_line)
                     .arg(st.last_line)
                     .arg(st.regular ? "" : ", irregular");
            // assemble the stanza's lines into one span and read it
            std::string joined;
            for (const auto& ln : st.lines) {
                if (!joined.empty()) joined += " ";
                joined += ln;
            }
            allcore::OverlayDoc doc =
                allcore::buildOverlay(spine_, index_, joined);
            auto clauses = allcore::refineClauses(
                doc, allcore::splitClauses(doc.tokens, doc.barrier_after));
            if (clauses.empty()) continue;
            // read the whole stanza's chunks: treat it as one reading span
            allcore::Clause whole;
            whole.beg = 0;
            whole.end = (int)doc.tokens.size();
            auto chunks = allcore::chunkClause(doc, whole);
            auto verb = allcore::spotVerb(doc, chunks);
            auto plan = allcore::planReading(chunks, verb);
            if (verb.chunk >= 0)
                h += QString("<div style='margin-left:10px'>verb: <b>%1</b>"
                             "%2</div>")
                         .arg(QString::fromStdString(verb.wylie)
                                  .toHtmlEscaped())
                         .arg(verb.confident
                                  ? ""
                                  : " <small style='color:#B4540A'>"
                                    "(unverified — no tense evidence)"
                                    "</small>");
            // ordered chunks
            std::vector<std::pair<int, QString>> ordered;
            int drops = 0;
            for (size_t ci = 0; ci < chunks.size(); ++ci) {
                const auto& c = chunks[ci];
                QString txt;
                for (int t = c.beg; t < c.end; ++t) {
                    if (!txt.isEmpty()) txt += " ";
                    txt += QString::fromStdString(doc.tokens[t]);
                }
                // a nominal chunk with no case marker inside a metered
                // line = a likely understood (dropped) particle
                if (c.marker.empty() && std::string(c.role) != "predicate" &&
                    ci + 1 < chunks.size())
                    ++drops;
                const int ord = ci < plan.size() ? plan[ci].order : 0;
                ordered.push_back({ord ? ord : 99,
                                   txt.toHtmlEscaped() +
                                       (ord ? "" :
                                        " <small style='color:#999'>"
                                        "(reads with the next)</small>")});
            }
            std::stable_sort(ordered.begin(), ordered.end(),
                             [](auto& a, auto& b) { return a.first < b.first; });
            h += "<div style='margin-left:10px'>reading order: ";
            for (size_t i = 0; i < ordered.size(); ++i) {
                if (i) h += " → ";
                h += ordered[i].second;
            }
            h += "</div>";
            if (drops > 0)
                h += QString("<div style='margin-left:10px;color:#8C2F2B;"
                             "font-size:12px'>~%1 case particle(s) likely "
                             "understood (dropped for meter) — supply the "
                             "relationship as you read</div>")
                         .arg(drops);
        }
        report_->setHtml(h);
    }

    // STD-008: the shared footnote bank — released Mixed Nuts notes are
    // meant for reuse; every insertion carries its source citation
    void loadNotesBank() {
        if (notesLoaded_) return;
        notesLoaded_ = true;
        QFile f(dataFile("extracted/mixed_nuts_notes.json"));
        if (f.open(QIODevice::ReadOnly)) {
            for (const auto& v :
                 QJsonDocument::fromJson(f.readAll()).array()) {
                const auto o = v.toObject();
                notesBank_.push_back({o["source"].toString(),
                                      o["note"].toInt(),
                                      o["lemma"].toString(),
                                      o["text"].toString()});
            }
        }
        QFile fb(dataFile("extracted/mixed_nuts_bibliography.json"));
        if (fb.open(QIODevice::ReadOnly)) {
            for (const auto& v :
                 QJsonDocument::fromJson(fb.readAll()).array()) {
                const auto o = v.toObject();
                QStringList refs;
                for (const auto& r : o["acip_refs"].toArray())
                    refs << r.toString();
                bibBank_.push_back({o["id"].toString(),
                                    o["source"].toString(),
                                    o["text"].toString(),
                                    refs.join(", ")});
            }
        }
        loadCandidates();
    }

    QString dataFile(const char* name) const {
        QString p = QCoreApplication::applicationDirPath() +
                    "/../../../../../data/" + name;
        if (QFile::exists(p)) return p;
        return QDir::currentPath() + "/data/" + name;
    }

    void searchNotesBank() {
        loadNotesBank();
        const QString q = notesSearch_->text().trimmed();
        if (q.isEmpty()) return;
        if (notesBank_.empty() && bibBank_.empty()) {
            report_->setHtml("<i>apparatus banks not found — run "
                             "tools/extract_mixed_nuts_notes.py and "
                             "tools/extract_mixed_nuts_bibliography.py</i>");
            return;
        }
        QString h = "<b>Shared apparatus</b> <small>(official tier: "
                    "published + GMR-approved only; insertions carry their "
                    "citation)</small><br>";
        int found = 0;
        for (int i = 0; i < (int)notesBank_.size() && found < 10; ++i) {
            const auto& n = notesBank_[i];
            if (!n.lemma.contains(q, Qt::CaseInsensitive) &&
                !n.text.contains(q, Qt::CaseInsensitive))
                continue;
            ++found;
            h += QString("<div style='margin:6px 0'><a href='note:%1'>"
                         "[insert]</a> <b>%2</b> <small style='color:#777'>"
                         "(%3, n.%4)</small><br><small>%5</small></div>")
                     .arg(i)
                     .arg(n.lemma.toHtmlEscaped())
                     .arg(n.source.toHtmlEscaped())
                     .arg(n.note)
                     .arg(n.text.left(220).toHtmlEscaped());
        }
        int bibFound = 0;
        for (int i = 0; i < (int)bibBank_.size() && bibFound < 8; ++i) {
            const auto& b = bibBank_[i];
            if (!b.text.contains(q, Qt::CaseInsensitive) &&
                !b.acipRefs.contains(q, Qt::CaseInsensitive))
                continue;
            ++bibFound;
            h += QString("<div style='margin:6px 0'><a href='bib:%1'>"
                         "[insert entry]</a> <b>bibliography %2</b> "
                         "<small style='color:#777'>(%3%4)</small><br>"
                         "<small>%5</small></div>")
                     .arg(i)
                     .arg(b.id.toHtmlEscaped())
                     .arg(b.source.toHtmlEscaped())
                     .arg(b.acipRefs.isEmpty()
                              ? QString()
                              : " · ACIP " + b.acipRefs.toHtmlEscaped())
                     .arg(b.text.left(220).toHtmlEscaped());
        }
        int candFound = 0;
        for (int i = 0; i < (int)candBank_.size() && candFound < 5; ++i) {
            const auto& n = candBank_[i];
            if (!n.lemma.contains(q, Qt::CaseInsensitive) &&
                !n.text.contains(q, Qt::CaseInsensitive))
                continue;
            ++candFound;
            h += QString("<div style='margin:6px 0;border-left:3px solid "
                         "#c80;padding-left:6px'><a href='cand:%1'>"
                         "[insert]</a> <b>%2</b> <b style='color:#c80'>"
                         "⚠ PENDING — not GMR-approved</b><br>"
                         "<small>%3</small></div>")
                     .arg(i)
                     .arg(n.lemma.toHtmlEscaped())
                     .arg(n.text.left(220).toHtmlEscaped());
        }
        if (!found && !bibFound && !candFound)
            h += "<i>no matching apparatus</i>";
        report_->setHtml(h);
    }

    void loadCandidates() {
        candBank_.clear();
        QFile f(dataFile("candidate_notes.json"));
        if (!f.open(QIODevice::ReadOnly)) return;
        for (const auto& v : QJsonDocument::fromJson(f.readAll()).array()) {
            const auto o = v.toObject();
            candBank_.push_back({o["proposed"].toString(), 0,
                                 o["lemma"].toString(),
                                 o["text"].toString()});
        }
    }

    void proposeNote() {
        const QString sel = draft_->textCursor().selectedText().trimmed();
        if (sel.isEmpty()) {
            report_->setHtml("<i>select the note text in the draft first "
                             "(lemma: text…)</i>");
            return;
        }
        QString lemma = sel.section(':', 0, 0).trimmed();
        QString text = sel.section(':', 1).trimmed();
        if (text.isEmpty()) { lemma = sel.left(40); text = sel; }
        QFile f(dataFile("candidate_notes.json"));
        QJsonArray arr;
        if (f.open(QIODevice::ReadOnly)) {
            arr = QJsonDocument::fromJson(f.readAll()).array();
            f.close();
        }
        QJsonObject o;
        o["lemma"] = lemma;
        o["text"] = text;
        o["proposed"] = QDateTime::currentDateTime().toString(Qt::ISODate);
        o["status"] = "pending GMR approval";
        arr.append(o);
        if (f.open(QIODevice::WriteOnly)) {
            f.write(QJsonDocument(arr).toJson());
            f.close();
        }
        loadCandidates();
        report_->setHtml(
            QString("<b>Candidate saved</b> — <b>%1</b> is now in the "
                    "pending queue (%2 candidates). It stays PENDING until "
                    "published in a released volume and 100%% approved by "
                    "GMR; only then does it enter the MAIN apparatus.")
                .arg(lemma.toHtmlEscaped())
                .arg(candBank_.size()));
    }

    void insertNote(int ix) {
        if (ix < 0 || ix >= (int)notesBank_.size()) return;
        const auto& n = notesBank_[ix];
        draft_->textCursor().insertText(
            "[NOTE: " + n.lemma + ": " + n.text + " — reused from " +
            n.source + ", n." + QString::number(n.note) + "]");
        draft_->setFocus();
    }

    void detectQuotes() {
        const std::string src = source_->toPlainText().toStdString();
        if (src.empty()) {
            report_->setHtml("<i>paste the source text first</i>");
            return;
        }
        loadNotesBank();
        // heuristic-free input typing: ACIP is uppercase by standard
        bool isAcip = false;
        for (char c : src)
            if (c >= 'A' && c <= 'Z') { isAcip = true; break; }
        const auto qs =
            allcore::detectQuotations(spine_, src, isAcip, 7);
        QString h =
            "<b>Quotation detection</b> <small>(exact corpus match over "
            "7+ syllables — attested, never inferred; lesser overlaps "
            "belong to the concordance)</small><br>";
        if (qs.empty()) {
            h += "<i>no attested quotations found</i>";
            report_->setHtml(h);
            return;
        }
        for (const auto& q : qs) {
            h += QString("<div style='margin:8px 0'><b>%1</b> segment #%2 "
                         "(%3 syllables attested)<br><small>%4</small><br>"
                         "<small><i>published English:</i> %5</small>")
                     .arg(QString::fromStdString(q.course).toHtmlEscaped())
                     .arg(q.seq)
                     .arg(q.syllable_count)
                     .arg(QString::fromStdString(q.matched_wylie)
                              .toHtmlEscaped())
                     .arg(QString::fromStdString(q.english)
                              .left(220)
                              .toHtmlEscaped());
            // published footnotes whose lemma appears in the segment's
            // published English are offered too (official tier only)
            int recs = 0;
            for (int i = 0; i < (int)notesBank_.size() && recs < 3; ++i) {
                const auto& n = notesBank_[i];
                if (n.lemma.size() < 5 ||
                    !QString::fromStdString(q.english)
                         .contains(n.lemma, Qt::CaseInsensitive))
                    continue;
                ++recs;
                h += QString("<br><a href='note:%1'>[insert footnote: "
                             "%2]</a> <small style='color:#777'>(%3, "
                             "n.%4)</small>")
                         .arg(i)
                         .arg(n.lemma.toHtmlEscaped())
                         .arg(n.source.toHtmlEscaped())
                         .arg(n.note);
            }
            // a matched work recommends its published bibliography
            // entries: match course id against the bank's ACIP refs
            const QString cq =
                QString::fromStdString(q.course).toUpper();
            for (int i = 0; i < (int)bibBank_.size(); ++i) {
                if (!bibBank_[i].acipRefs.isEmpty() && cq.size() >= 6 &&
                    bibBank_[i].acipRefs.toUpper().contains(cq))
                    h += QString("<br><a href='bib:%1'>[insert "
                                 "bibliography entry %2]</a>")
                             .arg(i)
                             .arg(bibBank_[i].id.toHtmlEscaped());
            }
            h += "</div>";
        }
        report_->setHtml(h);
    }

    void exportRtf() {
        const QString text = draft_->toPlainText();
        if (text.trimmed().isEmpty()) {
            report_->setHtml("<i>nothing to export</i>");
            return;
        }
        const QString fn = QFileDialog::getSaveFileName(
            this, "Export draft as RTF", "draft.rtf", "RTF (*.rtf)");
        if (fn.isEmpty()) return;
        QString body;
        bool italic = false;
        int italicRuns = 0;
        const auto u16 = text.utf16();
        for (int i = 0; i < text.size(); ++i) {
            const QChar c = text.at(i);
            if (c == '*') {
                body += italic ? "}" : "{\\i ";
                if (!italic) ++italicRuns;
                italic = !italic;
                continue;
            }
            if (c == '\\' || c == '{' || c == '}') {
                body += '\\';
                body += c;
            } else if (c == '\n') {
                body += "\\par\n";
            } else if (u16[i] < 128) {
                body += c;
            } else {
                body += "\\u" + QString::number((short)u16[i]) + "?";
            }
        }
        if (italic) body += "}";   // unclosed marker: close, then warn
        QFile f(fn);
        if (!f.open(QIODevice::WriteOnly)) {
            report_->setHtml("<i>could not write " + fn.toHtmlEscaped() +
                             "</i>");
            return;
        }
        f.write(("{\\rtf1\\ansi\\deff0{\\fonttbl{\\f0 Times New "
                 "Roman;}}\\f0\\fs24\n" +
                 body + "\n}")
                    .toUtf8());
        f.close();
        report_->setHtml(
            QString("<b>Exported</b> %1 — %2 italic run%3 (STD-004)%4")
                .arg(QFileInfo(fn).fileName().toHtmlEscaped())
                .arg(italicRuns)
                .arg(italicRuns == 1 ? "" : "s")
                .arg(italic ? " · <b style='color:#B26B00'>warning: an "
                              "asterisk was left unclosed</b>"
                            : ""));
    }

    void composeBibDialog() {
        QDialog dlg(this);
        dlg.setWindowTitle("Compose bibliography entry (STD-007)");
        auto* form = new QFormLayout(&dlg);
        auto* ep = new QLineEdit;
        auto* au = new QLineEdit;
        auto* dt = new QLineEdit;
        auto* et = new QLineEdit;
        auto* tt = new QLineEdit;
        auto* ac = new QLineEdit;
        auto* fo = new QLineEdit;
        ep->setPlaceholderText("Co-ne bla-ma (optional)");
        au->setPlaceholderText("Grags-pa bshad-sgrub — technical spelling");
        dt->setPlaceholderText("1675-1748 / c. 200AD (optional)");
        et->setPlaceholderText("A Brief Clarification of Heart: …");
        tt->setPlaceholderText(
            "Lam-rim bsdus-don gyi tsig-'grel … (optional)");
        ac->setPlaceholderText("S00184 / TD04158 (optional)");
        fo->setPlaceholderText("1a-11a (optional)");
        form->addRow("Epithets", ep);
        form->addRow("Author", au);
        form->addRow("Dates", dt);
        form->addRow("English title", et);
        form->addRow("Tibetan title", tt);
        form->addRow("ACIP number", ac);
        form->addRow("Folios", fo);
        auto* fillBtn = new QPushButton(
            "Auto-fill from catalog (by ACIP number — review result)");
        fillBtn->setToolTip(
            "Fills Tibetan title, author, dates, and a draft English "
            "title from the ACIP catalog (v29 title wave). Wylie comes "
            "from the canonical converter in plain lowercase — house "
            "technical-spelling hyphens and capitals are yours to "
            "apply.");
        form->addRow(fillBtn);
        QObject::connect(fillBtn, &QPushButton::clicked,
                         [this, ep, au, dt, et, tt, ac] {
                             autoFillBib(ep, au, dt, et, tt, ac);
                         });
        auto* hyBtn = new QPushButton(
            "Pair-hyphenate author (STD-002 helper — review result)");
        form->addRow(hyBtn);
        auto* preview = new QLabel;
        preview->setWordWrap(true);
        preview->setStyleSheet("QLabel{background:#00000010;padding:6px}");
        form->addRow(preview);
        auto* scanLink = new QLabel;
        form->addRow(scanLink);
        auto assemble = [=] {
            allcore::BibliographyFields f;
            f.epithets = ep->text().trimmed().toStdString();
            f.author = au->text().trimmed().toStdString();
            f.dates = dt->text().trimmed().toStdString();
            f.english_title = et->text().trimmed().toStdString();
            f.tibetan_title = tt->text().trimmed().toStdString();
            f.acip_number = ac->text().trimmed().toStdString();
            f.folios = fo->text().trimmed().toStdString();
            return f;
        };
        auto refresh = [=] {
            preview->setText(QString::fromStdString(
                allcore::composeBibliographyEntry(assemble())));
            const auto info = allcore::decodeAcipFilename(
                ac->text().trimmed().toStdString() + ".ACT");
            const auto url = allcore::bdrcScanUrl(info);
            scanLink->setText(
                url.empty() ? ""
                            : "scans: " + QString::fromStdString(url));
        };
        for (auto* w : {ep, au, dt, et, tt, ac, fo})
            QObject::connect(w, &QLineEdit::textChanged, refresh);
        QObject::connect(hyBtn, &QPushButton::clicked, [au, refresh] {
            au->setText(QString::fromStdString(allcore::hgmTechnicalSpelling(
                au->text().trimmed().toStdString())));
            refresh();
        });
        auto* buttons = new QDialogButtonBox;
        auto* insertB =
            buttons->addButton("Insert into draft", QDialogButtonBox::AcceptRole);
        auto* candB = buttons->addButton(
            "Save as candidate (pending GMR approval)",
            QDialogButtonBox::ActionRole);
        QObject::connect(candB, &QPushButton::clicked, [this, assemble] {
            const auto f = assemble();
            const QString entry = QString::fromStdString(
                allcore::composeBibliographyEntry(f));
            if (entry == ".") return;
            QFile fj(dataFile("candidate_bib.json"));
            QJsonArray arr;
            if (fj.open(QIODevice::ReadOnly)) {
                arr = QJsonDocument::fromJson(fj.readAll()).array();
                fj.close();
            }
            QJsonObject o;
            o["entry"] = entry;
            o["acip_no"] = QString::fromStdString(f.acip_number);
            o["proposed"] =
                QDateTime::currentDateTime().toString(Qt::ISODate);
            o["status"] = "pending GMR approval";
            arr.append(o);
            if (fj.open(QIODevice::WriteOnly)) {
                fj.write(QJsonDocument(arr).toJson());
                fj.close();
            }
            report_->setHtml(
                "<b>Bibliography candidate saved</b> — pending until "
                "published and approved; it appears in the review "
                "sheet (tools/build_pending_review.py).");
        });
        buttons->addButton(QDialogButtonBox::Cancel);
        form->addRow(buttons);
        QObject::connect(buttons, &QDialogButtonBox::accepted, &dlg,
                         &QDialog::accept);
        QObject::connect(buttons, &QDialogButtonBox::rejected, &dlg,
                         &QDialog::reject);
        (void)insertB;
        refresh();
        if (dlg.exec() != QDialog::Accepted) return;
        const QString entry = QString::fromStdString(
            allcore::composeBibliographyEntry(assemble()));
        if (entry == ".") return;
        draft_->textCursor().insertText(
            "[BIBLIOGRAPHY — NEW ENTRY, house format (STD-007), "
            "pending publication: " + entry + "]");
        draft_->setFocus();
    }

    void loadCatalogWorks() {
        if (worksLoaded_) return;
        worksLoaded_ = true;
        QFile f(dataFile("extracted/catalog_works.json"));
        if (!f.open(QIODevice::ReadOnly)) return;
        worksDoc_ = QJsonDocument::fromJson(f.readAll()).object();
    }

    void autoFillBib(QLineEdit* ep, QLineEdit* au, QLineEdit* dt,
                     QLineEdit* et, QLineEdit* tt, QLineEdit* ac) {
        loadCatalogWorks();
        QRegularExpression re("^([A-Za-z]+)0*(\\d+)");
        const auto m = re.match(ac->text().trimmed());
        if (!m.hasMatch()) return;
        const auto w = worksDoc_[m.captured(1).toUpper() + m.captured(2)]
                           .toObject();
        if (w.isEmpty()) {
            ac->setToolTip("no catalog record for this number");
            return;
        }
        (void)ep;
        auto toWylie = [](const QJsonValue& v) {
            return QString::fromStdString(
                       allcore::acipToEwts(v.toString().toStdString()))
                .trimmed();
        };
        if (au->text().isEmpty() && !w["author"].toString().isEmpty())
            au->setText(toWylie(w["author"]));
        if (dt->text().isEmpty()) dt->setText(w["dates"].toString());
        if (et->text().isEmpty()) et->setText(w["eng"].toString());
        if (tt->text().isEmpty() && !w["tib"].toString().isEmpty())
            tt->setText(toWylie(w["tib"]));
    }

    void insertBibEntry(int ix) {
        if (ix < 0 || ix >= (int)bibBank_.size()) return;
        const auto& b = bibBank_[ix];
        draft_->textCursor().insertText(
            "[BIBLIOGRAPHY: " + b.text + " — entry " + b.id +
            " as published in " + b.source + "]");
        draft_->setFocus();
    }

    void insertCandidate(int ix) {
        if (ix < 0 || ix >= (int)candBank_.size()) return;
        const auto& n = candBank_[ix];
        draft_->textCursor().insertText(
            "[NOTE — PENDING, not GMR-approved: " + n.lemma + ": " +
            n.text + "]");
        draft_->setFocus();
    }

    void aiCheck() {
        const std::string src = source_->toPlainText().toStdString();
        const std::string dr = draft_->toPlainText().toStdString();
        if (src.empty() || dr.empty()) {
            report_->setHtml("<i>paste both the source and a draft first</i>");
            return;
        }
        aiBtn_->setEnabled(false);
        aiAccum_.clear();
        aiBuf_.clear();
        report_->setHtml("<i>asking Claude for a coverage diff…</i>");
        auto pre = allcore::runPrePass(spine_, src);
        auto prompt = allcore::buildBackTranslationPrompt(pre, src, dr);
        const std::string body = allcore::buildMessagesRequestJson(prompt);
        QNetworkRequest req(QUrl("https://api.anthropic.com/v1/messages"));
        req.setRawHeader("x-api-key", qgetenv("ANTHROPIC_API_KEY"));
        req.setRawHeader("anthropic-version", "2023-06-01");
        req.setRawHeader("anthropic-beta", "server-side-fallback-2026-07-01");
        req.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
        aiReply_ = net_.post(req, QByteArray::fromStdString(body));
        QObject::connect(aiReply_, &QNetworkReply::readyRead, [this] {
            aiBuf_ += aiReply_->readAll();
            int nl;
            while ((nl = aiBuf_.indexOf('\n')) >= 0) {
                const QByteArray line = aiBuf_.left(nl).trimmed();
                aiBuf_.remove(0, nl + 1);
                if (!line.startsWith("data:")) continue;
                const std::string t =
                    allcore::sseTextDelta(line.mid(5).trimmed().toStdString());
                if (!t.empty()) aiAccum_ += t;
            }
            report_->setPlainText(
                "AI COVERAGE DIFF (model output — not engine data)\n\n" +
                QString::fromStdString(aiAccum_));
        });
        QObject::connect(aiReply_, &QNetworkReply::finished, [this] {
            QString h = "<div style='background:#FBEFEF;padding:4px'><b>AI "
                        "coverage diff</b> — model output, labeled AI; the "
                        "term anchors it cites are engine-verified.</div><hr>"
                        "<pre style='white-space:pre-wrap'>" +
                        QString::fromStdString(aiAccum_).toHtmlEscaped() +
                        "</pre>";
            if (aiReply_->error() != QNetworkReply::NoError && aiAccum_.empty())
                h += "<div style='color:#b00'>network/API error: " +
                     aiReply_->errorString().toHtmlEscaped() + "</div>";
            report_->setHtml(h);
            aiReply_->deleteLater();
            aiReply_ = nullptr;
            aiBtn_->setEnabled(true);
        });
    }

    void check() {
        auto rep = allcore::checkTerminology(
            spine_, index_, source_->toPlainText().toStdString(),
            draft_->toPlainText().toStdString());
        int unmatched = 0, mixed = 0, provisionalUsed = 0;
        for (const auto& t : rep.terms) {
            unmatched += t.matched.empty();
            mixed += t.matched.size() > 1;
            provisionalUsed += (!t.matched.empty() && t.provisional);
        }
        QString h = QString("<div><b>%1 term(s)</b> · %2 without an HGM "
                            "equivalent in the draft · %3 with mixed "
                            "renderings · %4 drafted from PROVISIONAL "
                            "glosses</div><hr>")
                        .arg(rep.terms.size())
                        .arg(unmatched)
                        .arg(mixed)
                        .arg(provisionalUsed);
        for (const auto& t : rep.terms) {
            const bool un = t.matched.empty();
            h += "<div style='margin:4px 0'>";
            h += (un ? "<b style='color:#B4540A'>○ " : "<b style='color:#3B7A3B'>● ");
            h += QString::fromStdString(t.wylie).toHtmlEscaped() + "</b>";
            if (t.occurrences > 1)
                h += QString(" ×%1").arg(t.occurrences);
            if (t.provisional)
                h += " <span style='color:#b00;font-size:11px'>[PROVISIONAL]"
                     "</span>";
            if (un) {
                QString gl;
                int shown = 0;
                for (const auto& g : t.glosses) {
                    if (shown++ >= 3) break;
                    gl += (shown > 1 ? " · " : "") +
                          QString::fromStdString(g).toHtmlEscaped();
                }
                h += "<br><small>no HGM equivalent found in the draft — HGM "
                     "has: " + gl + "</small>";
            } else {
                QString m;
                for (size_t i = 0; i < t.matched.size(); ++i)
                    m += (i ? " · " : "") +
                         QString::fromStdString(t.matched[i]).toHtmlEscaped();
                h += "<br><small>draft uses: " + m +
                     (t.matched.size() > 1
                          ? " <span style='color:#B4540A'>(mixed — is the "
                            "variation intended?)</span>"
                          : "") + "</small>";
            }
            h += "</div>";
        }
        if (!rep.shared.empty()) {
            h += "<hr><div><b>One English word serving several terms:</b></div>";
            for (const auto& sh : rep.shared) {
                QString ts;
                for (size_t i = 0; i < sh.term_wylies.size(); ++i)
                    ts += (i ? ", " : "") +
                          QString::fromStdString(sh.term_wylies[i]).toHtmlEscaped();
                h += "<div><small>\"" +
                     QString::fromStdString(sh.english).toHtmlEscaped() +
                     "\" ← " + ts + " — possible collapsed distinction</small>"
                     "</div>";
            }
        }
        report_->setHtml(h);
    }

    // ---- phrase memory (leadership rec #3): every prior rendering of
    // a selected phrase — HGM corpus (binding) + Align harvest (PENDING)
    void phraseMemory() {
        QString sel = source_->textCursor().selectedText().trimmed();
        sel.replace(QChar(0x2029), ' ');   // paragraph separators
        if (sel.isEmpty()) {
            anchors_->setHtml("<i>select a Tibetan phrase in the source "
                              "first, then Phrase memory</i>");
            return;
        }
        bool upper = false;
        for (QChar c : sel) upper |= c.isUpper();
        const std::string wy =
            upper ? allcore::acipToEwts(sel.toStdString())
                  : sel.toLower().toStdString();
        QString h = "<div><b>Phrase memory</b> — “" +
                    QString::fromStdString(wy).toHtmlEscaped() +
                    "”</div><hr>";
        // 1. the corpus: HGM's own renderings (binding layer)
        auto segs = spine_.corpusSearch('"' + wy + '"', "", 12);
        if (!segs.empty()) {
            h += QString("<div><b>HGM corpus</b> — %1 segment(s) "
                         "containing the phrase</div>")
                     .arg(segs.size());
            for (const auto& s : segs)
                h += "<div style='margin:5px 0'><small>[" +
                     QString::fromStdString(s.course) + ":" +
                     QString::number(s.seq) + "]</small><br><i>" +
                     QString::fromStdString(s.wylie)
                         .left(180)
                         .toHtmlEscaped() +
                     "</i><br>" +
                     QString::fromStdString(s.english)
                         .left(240)
                         .toHtmlEscaped() +
                     "</div>";
        } else {
            h += "<div><i>no corpus segment contains this exact "
                 "phrase</i></div>";
        }
        // 2. the Align harvest: translator-authored pairs (PENDING)
        int harvestHits = 0;
        QString hh;
        if (!root_.isEmpty()) {
            const std::string needle = " " + wy + " ";
            for (const QString& dir :
                 {root_ + "/library/links",
                  root_ + "/data/candidate_alignments"}) {
                for (const QString& fn : QDir(dir).entryList(
                         {"*.tsv"}, QDir::Files)) {
                    QFile f(dir + "/" + fn);
                    if (!f.open(QIODevice::ReadOnly | QIODevice::Text))
                        continue;
                    while (!f.atEnd() && harvestHits < 12) {
                        const QString line =
                            QString::fromUtf8(f.readLine()).trimmed();
                        if (line.isEmpty() || line.startsWith('#'))
                            continue;
                        const QStringList c = line.split('\t');
                        // links: tibBeg tibEnd wylie engBeg engEnd eng
                        // pairs: wylie eng
                        QString lw, le;
                        if (c.size() >= 6) { lw = c[2]; le = c[5]; }
                        else if (c.size() == 2) { lw = c[0]; le = c[1]; }
                        else continue;
                        const std::string hay =
                            " " + lw.toStdString() + " ";
                        if (hay.find(needle) == std::string::npos &&
                            lw.toStdString() != wy)
                            continue;
                        ++harvestHits;
                        hh += "<div style='margin:4px 0'><i>" +
                              lw.toHtmlEscaped() + "</i> — “" +
                              le.toHtmlEscaped() +
                              "” <small style='color:#777'>(" +
                              fn.toHtmlEscaped() + ")</small></div>";
                    }
                }
            }
        }
        if (harvestHits)
            h += QString("<hr><div><b>Align harvest</b> — %1 "
                         "translator-authored pair(s) <span style='"
                         "color:#B4540A'>(PENDING tier — not yet "
                         "reviewed into the dictionary)</span></div>")
                     .arg(harvestHits) +
                 hh;
        anchors_->setHtml(h);
    }

    allcore::Spine& spine_;
    allcore::Progress* progress_ = nullptr;
    QString root_;
    allcore::HeadwordIndex index_;
    allcore::OverlayDoc doc_;
    bool docIsWylie_ = false;
    std::vector<allcore::Clause> clauses_;
    int lastClause_ = -1;
    QPlainTextEdit* source_ = nullptr;
    QPlainTextEdit* draft_ = nullptr;
    QTextBrowser* clauseView_ = nullptr;
    QTextBrowser* anchors_ = nullptr;
    QTextBrowser* report_ = nullptr;
    struct BankNote {
        QString source;
        int note = 0;
        QString lemma;
        QString text;
    };
    struct BibEntry {
        QString id;
        QString source;
        QString text;
        QString acipRefs;
    };
    std::vector<BankNote> notesBank_;
    std::vector<BibEntry> bibBank_;
    std::vector<BankNote> candBank_;   // pending tier — never official
    QJsonObject worksDoc_;
    bool worksLoaded_ = false;
    bool notesLoaded_ = false;
    QLineEdit* notesSearch_ = nullptr;
    QPushButton* aiBtn_ = nullptr;
    QNetworkAccessManager net_;
    QNetworkReply* aiReply_ = nullptr;
    QByteArray aiBuf_;
    std::string aiAccum_;
};

// ---- Library pane: collection installs, personal imports, tree browser -----
// Collections (Kangyur / Tengyur / Sungbum ZIPs) come from
// https://asianlegacylibrary.org/library/ — the user downloads a ZIP there
// and installs it here; personal materials (.docx/.txt/.acip/.md) import
// into their own folder, with .docx auto-converted to text via macOS
// textutil. Everything lands under <root>/library/ and is browsable in a
// sortable tree; ACIP file names decode to their catalog provenance.
class LibraryPane : public QWidget {
public:
    LibraryPane(const QString& root, allcore::Progress* progress,
                std::function<void(const QString&)> open)
        : libRoot_(root + "/library"), progress_(progress),
          open_(std::move(open)) {
        QDir().mkpath(libRoot_);
        QDir().mkpath(libRoot_ + "/my_materials");
        QDir().mkpath(libRoot_ + "/ocr_out");
        auto* layout = new QVBoxLayout(this);
        layout->addWidget(new QLabel(
            "<b>Library</b> — install the ACIP collections (download the "
            "Kangyur / Tengyur / Sungbum ZIPs from "
            "<a href='https://asianlegacylibrary.org/library/'>"
            "asianlegacylibrary.org/library</a>), or import your own "
            "materials. Double-click a text to open it in the Overlay."));
        auto* row = new QHBoxLayout;
        auto* installBtn = new QPushButton("Install collection ZIP…");
        auto* importBtn = new QPushButton("Import my materials…");
        auto* ocrBtn = new QPushButton("Send to OCR…");
        auto* utfcBtn = new QPushButton("Legacy font rescue (UTFC)…");
        utfcBtn->setToolTip(
            "Convert a document typed in a pre-Unicode Tibetan font "
            "encoding (TibetanMachineWeb, LTibetan, Sambhota…) to "
            "Unicode text, via the Universal Tibetan Font Converter — "
            "run as a separate external tool (GPL v3, never linked).");
        auto* viewBtn = new QPushButton("List view");
        viewBtn->setCheckable(true);
        viewBtn->setToolTip(
            "Flat catalog table of every file — sort by catalog number, "
            "verification level, or language across all folders.");
        auto* indexBtn = new QPushButton("Update search index");
        row->addWidget(installBtn);
        row->addWidget(importBtn);
        row->addWidget(viewBtn);
        row->addWidget(ocrBtn);
        row->addWidget(utfcBtn);
        row->addWidget(indexBtn);
        search_ = new QLineEdit;
        search_->setPlaceholderText("find in library by name… (Enter)");
        row->addWidget(search_, 1);
        layout->addLayout(row);
        // decoder-driven filters: browse the library by what the ACIP file
        // names themselves declare (collection, verification level, language)
        auto* frow = new QHBoxLayout;
        colFilter_ = new QComboBox;
        colFilter_->addItems({"any collection", "Kangyur", "Tengyur",
                              "Sungbum", "Reference", "Graphics"});
        statusFilter_ = new QComboBox;
        statusFilter_->addItems({"any status", "high (L/M/N/F)",
                                 "mid (C/D/E)", "raw (A/B)", "incomplete"});
        langFilter_ = new QComboBox;
        langFilter_->addItems({"any language", "Tibetan", "English",
                               "Sanskrit", "mixed"});
        for (auto* cb : {colFilter_, statusFilter_, langFilter_}) {
            frow->addWidget(cb);
            connect(cb, &QComboBox::currentIndexChanged,
                    [this](int) { searchNames(); });
        }
        frow->addStretch();
        layout->addLayout(frow);

        auto* split = new QSplitter(Qt::Horizontal);
        model_ = new QFileSystemModel(this);
        model_->setRootPath(libRoot_);
        tree_ = new QTreeView;
        tree_->setModel(model_);
        tree_->setRootIndex(model_->index(libRoot_));
        tree_->setSortingEnabled(true);
        tree_->sortByColumn(0, Qt::AscendingOrder);
        tree_->setColumnWidth(0, 260);
        // remember which folders were open across restarts (Adam,
        // 2026-08-10): persist expanded paths, restore on load
        {
            QSettings st("ALL", "TranslationTool");
            for (const QString& p :
                 st.value("library/expanded").toStringList())
                if (QFileInfo::exists(p))
                    tree_->expand(model_->index(p));
            auto persist = [this] {
                QSettings s2("ALL", "TranslationTool");
                QStringList open = s2.value("library/expanded")
                                       .toStringList();
                // rebuild from live state of known + new paths
                QStringList out;
                for (const QString& p : open)
                    if (tree_->isExpanded(model_->index(p)))
                        out << p;
                s2.setValue("library/expanded", out);
            };
            connect(tree_, &QTreeView::expanded,
                    [this](const QModelIndex& ix) {
                        QSettings s2("ALL", "TranslationTool");
                        QStringList open =
                            s2.value("library/expanded").toStringList();
                        const QString p = model_->filePath(ix);
                        if (!open.contains(p)) {
                            open << p;
                            s2.setValue("library/expanded", open);
                        }
                    });
            connect(tree_, &QTreeView::collapsed,
                    [this](const QModelIndex& ix) {
                        QSettings s2("ALL", "TranslationTool");
                        QStringList open =
                            s2.value("library/expanded").toStringList();
                        open.removeAll(model_->filePath(ix));
                        s2.setValue("library/expanded", open);
                    });
            Q_UNUSED(persist);
        }
        auto* stack = new QStackedWidget;
        stack->addWidget(tree_);
        list_ = new QTableWidget;
        list_->setColumnCount(7);
        list_->setHorizontalHeaderLabels(
            {"File", "Collection", "No.", "English title (catalog)",
             "Verification", "Language", "KB"});
        list_->setEditTriggers(QAbstractItemView::NoEditTriggers);
        list_->setSelectionBehavior(QAbstractItemView::SelectRows);
        list_->setSortingEnabled(true);
        list_->verticalHeader()->setVisible(false);
        stack->addWidget(list_);
        stack_ = stack;
        split->addWidget(stack);
        info_ = new QTextBrowser;
        info_->setOpenLinks(false);
        connect(info_, &QTextBrowser::anchorClicked, [this](const QUrl& u) {
            const QString s = u.toString();
            if (s.startsWith("openfile:")) {
                const QString p = s.mid(9);
                logOpen(p);
                if (open_) open_(p);
            } else if (s.startsWith("http")) {
                QDesktopServices::openUrl(u);
            }
        });
        split->addWidget(info_);
        split->setStretchFactor(0, 2);
        split->setStretchFactor(1, 1);
        layout->addWidget(split, 1);

        connect(viewBtn, &QPushButton::toggled, [this](bool on) {
            stack_->setCurrentIndex(on ? 1 : 0);
            if (on) fillList();
        });
        connect(list_, &QTableWidget::cellDoubleClicked,
                [this](int r, int) {
                    const QString p =
                        list_->item(r, 0)->data(Qt::UserRole).toString();
                    if (!p.isEmpty() && open_) {
                        logOpen(p);
                        open_(p);
                    }
                });
        connect(list_, &QTableWidget::currentCellChanged,
                [this](int r, int, int, int) {
                    if (r >= 0 && list_->item(r, 0))
                        showInfo(list_->item(r, 0)
                                     ->data(Qt::UserRole)
                                     .toString());
                });
        connect(installBtn, &QPushButton::clicked, [this] { installZip(); });
        connect(importBtn, &QPushButton::clicked, [this] { importFiles(); });
        connect(ocrBtn, &QPushButton::clicked, [this] { sendToOcr(); });
        connect(utfcBtn, &QPushButton::clicked, [this] { convertLegacy(); });
        connect(indexBtn, &QPushButton::clicked, [this] { updateIndex(); });
        connect(search_, &QLineEdit::returnPressed, [this] { searchNames(); });
        connect(tree_->selectionModel(), &QItemSelectionModel::currentChanged,
                [this](const QModelIndex& ix, const QModelIndex&) {
                    showInfo(model_->filePath(ix));
                });
        connect(tree_, &QTreeView::doubleClicked, [this](const QModelIndex& ix) {
            const QString p = model_->filePath(ix);
            if (QFileInfo(p).isFile() && open_) {
                logOpen(p);
                open_(p);
            }
        });
        showRecents();
    }

    int selfTest(QStringList& log) {
        int fails = 0;
        auto check = [&](bool ok, const char* what) {
            log << QString("  [%1] Library: %2")
                       .arg(ok ? "PASS" : "FAIL").arg(what);
            if (!ok) ++fails;
        };
        check(list_->rowCount() > 1000 ||
                  model_->rowCount(model_->index(libRoot_)) > 0,
              "catalog populated from the library");
        return fails;
    }

private:
    // catalog-number → published catalog English title (v29 title wave)
    QString englishTitle(const QString& fileName) {
        if (!titlesLoaded_) {
            titlesLoaded_ = true;
            QFile f(QFileInfo(libRoot_).path() +
                    "/data/extracted/catalog_titles.json");
            if (f.open(QIODevice::ReadOnly)) {
                const auto o =
                    QJsonDocument::fromJson(f.readAll()).object();
                for (auto it = o.begin(); it != o.end(); ++it)
                    titles_[it.key()] = it.value().toString();
            }
            QFile fs(QFileInfo(libRoot_).path() +
                     "/data/extracted/sungbum_subjects.json");
            if (fs.open(QIODevice::ReadOnly)) {
                const auto o =
                    QJsonDocument::fromJson(fs.readAll()).object();
                for (auto it = o.begin(); it != o.end(); ++it)
                    subjects_[it.key()] = it.value().toString();
            }
            // ACIP Release 6 subject memberships (bilingual names,
            // covers Tengyur too)
            QFile fr(QFileInfo(libRoot_).path() +
                     "/data/extracted/r6_work_subjects.json");
            if (fr.open(QIODevice::ReadOnly)) {
                const auto o =
                    QJsonDocument::fromJson(fr.readAll()).object();
                for (auto it = o.begin(); it != o.end(); ++it) {
                    QStringList ss;
                    for (const auto& v : it.value().toArray()) {
                        QString n = v.toString();
                        n.replace("--", " — ").replace('_', ' ');
                        ss << n;
                    }
                    if (!ss.isEmpty())
                        r6subjects_[it.key()] = ss.join(" · ");
                }
            }
        }
        QRegularExpression re("^([A-Za-z]+)0*(\\d+)");
        const auto m = re.match(fileName);
        if (!m.hasMatch()) return QString();
        return titles_.value(m.captured(1).toUpper() + m.captured(2));
    }

    void fillList() {
        list_->setSortingEnabled(false);
        list_->setRowCount(0);
        QDirIterator it(libRoot_, QDir::Files,
                        QDirIterator::Subdirectories);
        while (it.hasNext()) {
            const QString p = it.next();
            const QFileInfo fi(p);
            if (fi.fileName().startsWith('.')) continue;
            const auto a = allcore::decodeAcipFilename(p.toStdString());
            const int r = list_->rowCount();
            list_->insertRow(r);
            auto* name = new QTableWidgetItem(fi.fileName());
            name->setData(Qt::UserRole, p);
            list_->setItem(r, 0, name);
            list_->setItem(
                r, 1,
                new QTableWidgetItem(
                    a.recognized
                        ? QString::fromStdString(a.collection)
                        : (p.contains("/ocr_out/") ? "ocr-derived"
                           : p.contains("/my_materials/")
                               ? "my materials"
                               : "")));
            auto* num = new QTableWidgetItem;
            num->setData(Qt::EditRole,
                         a.recognized
                             ? QString::fromStdString(a.number).toInt()
                             : -1);
            list_->setItem(r, 2, num);
            list_->setItem(r, 3,
                           new QTableWidgetItem(englishTitle(
                               fi.fileName())));
            list_->setItem(r, 4,
                           new QTableWidgetItem(QString::fromStdString(
                               a.status)));
            list_->setItem(r, 5,
                           new QTableWidgetItem(QString::fromStdString(
                               a.language)));
            auto* kb = new QTableWidgetItem;
            kb->setData(Qt::EditRole, (int)(fi.size() / 1024));
            list_->setItem(r, 6, kb);
        }
        list_->setSortingEnabled(true);
        list_->resizeColumnsToContents();
        list_->setColumnWidth(0, qMin(list_->columnWidth(0), 220));
    }

    void installZip() {
        const QString zip = QFileDialog::getOpenFileName(
            this, "Install collection ZIP", QString(), "ZIP archives (*.zip)");
        if (zip.isEmpty()) return;
        QString name = QFileInfo(zip).completeBaseName().toLower();
        for (const char* c : {"kangyur", "tengyur", "sungbum", "varanasi"})
            if (name.contains(c)) { name = c; break; }
        const QString dest = libRoot_ + "/" + name;
        QDir().mkpath(dest);
        info_->setHtml("<i>unpacking " + QFileInfo(zip).fileName().toHtmlEscaped() +
                       " …</i>");
        QCoreApplication::processEvents();
        QProcess p;
        p.start("/usr/bin/unzip", {"-o", zip, "-d", dest});
        p.waitForFinished(10 * 60 * 1000);
        int files = 0;
        QDirIterator it(dest, QDir::Files, QDirIterator::Subdirectories);
        while (it.hasNext()) { it.next(); ++files; }
        info_->setHtml(
            p.exitCode() == 0
                ? QString("<b>Installed.</b> %1 file(s) now under library/%2/.")
                      .arg(files)
                      .arg(name)
                : "<b style='color:#b00'>unzip failed</b> (exit " +
                      QString::number(p.exitCode()) + ") — is the ZIP intact?");
    }

    void importFiles() {
        const QStringList files = QFileDialog::getOpenFileNames(
            this, "Import materials", QString(),
            "Documents (*.docx *.txt *.acip *.act *.md *.rtf);;All files (*)");
        if (files.isEmpty()) return;
        const QString dest = libRoot_ + "/my_materials";
        QStringList report;
        for (const QString& f : files) {
            const QFileInfo fi(f);
            const QString target = dest + "/" + fi.fileName();
            QFile::remove(target);
            QFile::copy(f, target);
            report << fi.fileName();
            const QString suffix = fi.suffix().toLower();
            if (suffix == "docx" || suffix == "rtf") {
                // macOS textutil converts to plain text alongside
                const QString txt =
                    dest + "/" + fi.completeBaseName() + ".txt";
                QProcess p;
                p.start("/usr/bin/textutil",
                        {"-convert", "txt", "-output", txt, target});
                p.waitForFinished(120000);
                if (p.exitCode() == 0)
                    report.last() += " (+ .txt conversion)";
            }
        }
        info_->setHtml("<b>Imported into library/my_materials/:</b><br>" +
                       report.join("<br>").toHtmlEscaped());
    }

    // Legacy-font rescue lane: the Universal Tibetan Font Converter
    // (Tashi Tsering / Trace Foundation, GPL v3) run as an EXTERNAL
    // PROCESS — never linked into the app (GPL isolation; survey Tier 1).
    // Rescues documents typed in pre-Unicode font encodings — exactly
    // the TibetanMachineWeb/LTibetan/Sambhota faces the font survey
    // found installed here as custom-encoded legacy fonts.
    void convertLegacy() {
        const QString root = QFileInfo(libRoot_).path();
        const QString bin = root + "/build/utfc/utfc";
        if (!QFileInfo::exists(bin)) {
            info_->setHtml(
                "<b>UTFC not built yet.</b><br>Run <code>sh "
                "tools/setup_utfc.sh</code> once in the project folder — "
                "it compiles the Universal Tibetan Font Converter from "
                "the UTFC-master sources (GPL v3; kept as a separate "
                "external tool, never linked into this app).");
            return;
        }
        // the binary looks up its .tbl mapping tables relative to cwd,
        // so it must run inside the UTFC source folder
        QSettings st("ALL", "TranslationTool");
        QString utfcDir = st.value("utfc/dir").toString();
        if (utfcDir.isEmpty() || !QFileInfo::exists(utfcDir + "/Converter.c")) {
            QFile f(root + "/build/utfc/utfc_dir.txt");
            if (f.open(QIODevice::ReadOnly))
                utfcDir = QString::fromUtf8(f.readAll()).trimmed();
        }
        if (!QFileInfo::exists(utfcDir + "/Converter.c")) {
            utfcDir = QFileDialog::getExistingDirectory(
                this, "Locate the UTFC-master folder (conversion tables)");
            if (utfcDir.isEmpty()) return;
        }
        st.setValue("utfc/dir", utfcDir);
        const QString src = QFileDialog::getOpenFileName(
            this, "Legacy document to rescue", QString(),
            "Text documents (*.txt *.rtf);;All files (*)");
        if (src.isEmpty()) return;
        // source encodings UTFC ships tables for (Converter.c
        // EncodingNames[], IDs 1–16; 0 = Unicode is the target)
        const QStringList encs = {
            "TMW — TibetanMachineWeb (THL)",
            "TM — TibetanMachine (old THL)",
            "LTibetan — LTibetan/Larashi",
            "OldSambhota — Sambhota Jinkhyi",
            "NewSambhota — Sambhota Dege etc.",
            "ACIP — ACIP transcription",
            "Wylie — classic Wylie",
            "THDLWylie — THL extended Wylie",
            "LCWylie — Lokesh Chandra Wylie",
            "TCRCBodYig — TCRC Bod-Yig",
            "Fz — Founder (PRC)",
            "Hg — Huaguang (PRC)",
            "Bzd — Beijing Zangwen Duo",
            "Ty — Tongyong",
            "NS — Namsel",
            "Jamyang"};
        bool ok = false;
        QString enc = QInputDialog::getItem(
            this, "Source encoding",
            "Which legacy font/encoding is the document typed in?\n"
            "(If unsure and the font on the old machine was "
            "\"TibetanMachineWeb1–9\", pick TMW.)",
            encs, 0, false, &ok);
        if (!ok) return;
        enc = enc.section(QChar(0x2014), 0, 0).trimmed();
        enc = enc.section(' ', 0, 0);
        const QFileInfo fi(src);
        const QString dest = libRoot_ + "/my_materials";
        const QString mid = dest + "/" + fi.completeBaseName() + ".utfc16";
        QProcess p;
        p.setWorkingDirectory(utfcDir);
        p.start(bin, {src, mid, enc, "Unicode", "UnicodeTXT", "none"});
        p.waitForFinished(300000);
        const QString plog = QString::fromUtf8(p.readAllStandardOutput()) +
                             QString::fromUtf8(p.readAllStandardError());
        if (!QFileInfo::exists(mid) || QFileInfo(mid).size() == 0 ||
            !plog.contains("Done")) {
            QFile::remove(mid);
            info_->setHtml("<b>UTFC conversion failed.</b> Its output:"
                           "<br><pre>" + plog.toHtmlEscaped() + "</pre>");
            return;
        }
        // UTFC's UnicodeTXT output is UTF-16LE with BOM → normalize UTF-8
        QString text;
        {
            QFile f(mid);
            f.open(QIODevice::ReadOnly);
            QStringDecoder dec(QStringDecoder::Utf16LE);
            text = dec(f.readAll());
        }
        QFile::remove(mid);
        if (!text.isEmpty() && text.front() == QChar(0xFEFF))
            text.remove(0, 1);
        const QString target =
            dest + "/" + fi.completeBaseName() + ".utfc.txt";
        {
            QFile f(target);
            f.open(QIODevice::WriteOnly | QIODevice::Truncate);
            f.write(text.toUtf8());
        }
        info_->setHtml(
            "<b>Rescued to Unicode:</b> <a href='openfile:" + target +
            "'>" + QFileInfo(target).fileName().toHtmlEscaped() +
            "</a><br>" +
            QString("%1 characters · source encoding %2 · converter: "
                    "UTFC (Trace Foundation)<br>")
                .arg(text.size())
                .arg(enc.toHtmlEscaped()) +
            "<div style='color:#B26B00'><b>utfc-derived</b> — "
            "machine-converted from a legacy font encoding; review "
            "before treating as canonical. Wrong-encoding guesses "
            "produce garbage, not silent errors — if it looks wrong, "
            "re-run with another source encoding.</div>" +
            (text.contains(QChar(0x0F0B))
                 ? QString()
                 : "<div style='color:#B00020'><b>warning:</b> no "
                   "Tibetan tsheg found in the output — the source "
                   "encoding chosen is probably wrong.</div>"));
    }

    void showInfo(const QString& path) {
        const QFileInfo fi(path);
        if (!fi.isFile()) { info_->clear(); return; }
        QString h = "<b>" + fi.fileName().toHtmlEscaped() + "</b><br>" +
                    QString("%1 KB · %2<br>")
                        .arg(fi.size() / 1024)
                        .arg(fi.lastModified().toString("yyyy-MM-dd"));
        if (path.contains("/ocr_out/"))
            h += "<div style='color:#B26B00'><b>ocr-derived</b> — "
                 "unverified review material; opening it runs the "
                 "syllable-legality first-pass QC</div>";
        auto acip = allcore::decodeAcipFilename(path.toStdString());
        const QString etitle = englishTitle(fi.fileName());
        if (!etitle.isEmpty())
            h += "<div style='margin-top:2px'><i>" + etitle.toHtmlEscaped() +
                 "</i> <small style='color:#777'>(catalog title)</small>"
                 "</div>";
        {
            QRegularExpression re("^([A-Za-z]+)0*(\\d+)");
            const auto m = re.match(fi.fileName());
            const QString subj =
                m.hasMatch() ? subjects_.value(m.captured(1).toUpper() +
                                               m.captured(2))
                             : QString();
            if (!subj.isEmpty())
                h += "<div><span style='font-size:13px'>" +
                     subj.toHtmlEscaped() +
                     "</span> <small style='color:#777'>(subject, "
                     "Sungbum catalog)</small></div>";
            const QString r6 =
                m.hasMatch() ? r6subjects_.value(m.captured(1).toUpper() +
                                                 m.captured(2))
                             : QString();
            if (!r6.isEmpty())
                h += "<div style='font-size:12px'>" +
                     r6.toHtmlEscaped() +
                     " <small style='color:#777'>(subject, ACIP "
                     "Release 6)</small></div>";
        }
        if (acip.recognized) {
            h += "<div style='margin-top:4px;color:#4A3FBF'>" +
                 QString::fromStdString(acip.collection).toHtmlEscaped() +
                 " · text " + QString::fromStdString(acip.number);
            if (!acip.part.empty())
                h += " part " + QString::fromStdString(acip.part);
            if (!acip.subNumber.empty())
                h += " <small style='color:#777'>(sub-number " +
                     QString::fromStdString(acip.subNumber)
                         .toHtmlEscaped() +
                     ", toolchain grammar)</small>";
            h += "<br>" + QString::fromStdString(acip.status).toHtmlEscaped() +
                 (acip.status.empty() ? "" : " · ") +
                 QString::fromStdString(acip.language).toHtmlEscaped();
            if (acip.incomplete) h += " · <b>INCOMPLETE</b>";
            const std::string scan = allcore::bdrcScanUrl(acip);
            if (!scan.empty())
                h += "<br><a href='" + QString::fromStdString(scan) +
                     "'>view the original scans on BDRC</a>";
            h += "</div>";
        }
        // text preview for reasonable files
        const QString suffix = fi.suffix().toLower();
        if (fi.size() < 4 * 1024 * 1024 &&
            (suffix == "txt" || suffix == "act" || suffix == "acip" ||
             suffix == "md" || suffix == "inc" || suffix.isEmpty())) {
            QFile f(path);
            if (f.open(QIODevice::ReadOnly))
                h += "<hr><pre style='white-space:pre-wrap;font-size:12px'>" +
                     QString::fromUtf8(f.read(700)).toHtmlEscaped() + "…</pre>";
        }
        h += "<div style='margin-top:6px;color:#777'><small>double-click to "
             "open in the Overlay</small></div>";
        info_->setHtml(h);
    }

    void updateIndex() {
        info_->setHtml("<i>indexing the library… (first run over a full "
                       "collection can take a while)</i>");
        QCoreApplication::processEvents();
        try {
            allcore::LibraryIndex ix((libRoot_ + "/.index.db").toStdString());
            auto st = ix.update(libRoot_.toStdString());
            info_->setHtml(
                QString("<b>Search index up to date.</b><br>%1 added · %2 "
                        "updated · %3 removed · %4 unchanged<br>%5 file(s), "
                        "%6 line(s) indexed.<br><small>The Search pane's "
                        "\"search the Library\" now answers from this index "
                        "instantly.</small>")
                    .arg(st.added)
                    .arg(st.updated)
                    .arg(st.removed)
                    .arg(st.unchanged)
                    .arg(ix.fileCount())
                    .arg(ix.lineCount()));
        } catch (const std::exception& ex) {
            info_->setHtml(QString("<b style='color:#b00'>indexing failed:"
                                   "</b> %1")
                               .arg(QString::fromUtf8(ex.what()).toHtmlEscaped()));
        }
    }

    void logOpen(const QString& path) {
        if (progress_)
            progress_->recordDrill("openfile", path.toStdString(), true,
                                   (long long)time(nullptr));
    }

    static QString fileLink(const QString& path, const QString& base) {
        QString label = path;
        if (label.startsWith(base)) label = label.mid(base.size() + 1);
        return "<a href='openfile:" + path.toHtmlEscaped() + "'>" +
               label.toHtmlEscaped() + "</a>";
    }

    void showRecents() {
        if (!progress_) return;
        auto rec = progress_->recentKeys("openfile", 10);
        if (rec.empty()) return;
        QString h = "<b>Recently opened</b><br>";
        for (const auto& r : rec) {
            const QString p = QString::fromStdString(r);
            if (QFileInfo::exists(p)) h += fileLink(p, libRoot_) + "<br>";
        }
        info_->setHtml(h);
    }

    bool passesFilters(const QString& path) const {
        const int ci = colFilter_->currentIndex();
        const int si = statusFilter_->currentIndex();
        const int li = langFilter_->currentIndex();
        if (ci == 0 && si == 0 && li == 0) return true;
        auto info = allcore::decodeAcipFilename(path.toStdString());
        if (!info.recognized) return false;   // filters imply catalog files
        if (ci > 0 &&
            QString::fromStdString(info.collection)
                    .indexOf(colFilter_->currentText(), 0,
                             Qt::CaseInsensitive) < 0)
            return false;
        if (si > 0) {
            const QString st = QString::fromStdString(info.status);
            switch (si) {
                case 1:
                    if (!st.contains("typo") && !st.contains("statistical") &&
                        !st.contains("proofreading"))
                        return false;
                    break;
                case 2:
                    if (!st.contains("automated comparison")) return false;
                    break;
                case 3:
                    if (!st.contains("typing")) return false;
                    break;
                case 4:
                    if (!info.incomplete) return false;
                    break;
            }
        }
        if (li > 0 &&
            QString::fromStdString(info.language)
                    .indexOf(langFilter_->currentText(), 0,
                             Qt::CaseInsensitive) < 0)
            return false;
        return true;
    }

    void searchNames() {
        const QString q = search_->text().trimmed();
        const bool filtered = colFilter_->currentIndex() ||
                              statusFilter_->currentIndex() ||
                              langFilter_->currentIndex();
        if (q.isEmpty() && !filtered) { showRecents(); return; }
        QString h = filtered
            ? "<b>Filtered browse</b> <small>(" +
                  colFilter_->currentText() + " · " +
                  statusFilter_->currentText() + " · " +
                  langFilter_->currentText() +
                  (q.isEmpty() ? "" : " · name “" + q.toHtmlEscaped() + "”") +
                  ")</small><br>"
            : "<b>Name matches for “" + q.toHtmlEscaped() + "”</b><br>";
        int found = 0;
        QDirIterator it(libRoot_, QDir::Files, QDirIterator::Subdirectories);
        while (it.hasNext() && found < 60) {
            it.next();
            if (!q.isEmpty() &&
                !it.fileName().contains(q, Qt::CaseInsensitive))
                continue;
            if (!passesFilters(it.fileName())) continue;
            h += fileLink(it.filePath(), libRoot_) + "<br>";
            ++found;
        }
        h += found ? QString("<small>%1 match(es)%2</small>")
                         .arg(found)
                         .arg(found == 60 ? " (capped)" : "")
                   : "<i>no matches</i>";
        h += "<div style='margin-top:6px'><small>content search: use the "
             "Search pane — its folder box points at the library.</small>"
             "</div>";
        info_->setHtml(h);
    }

    void sendToOcr() {
        // hand-off to BDRC's open-source Tibetan OCR app (docs/OCR_DESIGN.md
        // stage 1): open the app if installed, else the download page; OCR
        // text output belongs in library/ocr_out/ where it is browsable and
        // importable like any material
        const QString sel = model_->filePath(tree_->currentIndex());
        QString app;
        for (const QString& cand :
             QDir("/Applications").entryList(QDir::Dirs)) {
            if (cand.contains("OCR", Qt::CaseInsensitive) &&
                cand.contains("Tib", Qt::CaseInsensitive))
                app = "/Applications/" + cand;
        }
        QString h = "<b>Send to OCR</b><br>";
        if (!app.isEmpty()) {
            QStringList args = {app};
            if (!sel.isEmpty() && QFileInfo(sel).isFile()) args << sel;
            QProcess::startDetached("/usr/bin/open", args.size() > 1
                                                        ? QStringList{"-a", app, sel}
                                                        : QStringList{app});
            h += "launched " + QFileInfo(app).fileName().toHtmlEscaped() +
                 (sel.isEmpty() ? "" : " with the selected file") + ".<br>";
        } else {
            h += "BDRC's free offline Tibetan OCR app is not installed. "
                 "Download it here: <a href='https://buda-base.github.io/"
                 "tibetan-ocr-app/'>buda-base.github.io/tibetan-ocr-app</a> "
                 "(5 script-style models incl. Woodblock).<br>";
        }
        h += "<small>Save OCR text output into <b>library/ocr_out/</b> — "
             "OCR-derived text is review material until verified (rule 3); "
             "run it through the Overlay's spellcheck as first-pass QC."
             "</small>";
        info_->setHtml(h);
    }

    QString libRoot_;
    allcore::Progress* progress_ = nullptr;
    std::function<void(const QString&)> open_;
    QFileSystemModel* model_ = nullptr;
    QTreeView* tree_ = nullptr;
    QTableWidget* list_ = nullptr;
    QStackedWidget* stack_ = nullptr;
    QHash<QString, QString> titles_;
    QHash<QString, QString> subjects_;
    QHash<QString, QString> r6subjects_;
    bool titlesLoaded_ = false;
    QTextBrowser* info_ = nullptr;
    QLineEdit* search_ = nullptr;
    QComboBox* colFilter_ = nullptr;
    QComboBox* statusFilter_ = nullptr;
    QComboBox* langFilter_ = nullptr;
};

}  // namespace

// Find the project data root: works from terminal (cwd) AND from Finder
// (walk up from the .app bundle looking for build/hgm_spine_v27_2.db).
// ---- Align pane: Hypercontext's authoring half, modernized ---------------
// The original Hypercontext (tutorial.hyp, Adam's own fork) was an
// ALIGNMENT tool: click a Tibetan span (look-ahead word-finder), click its
// English, spacebar to link — nested sublinks for titles/phrases — and the
// hypertexted files fed the dictionary. Here: the word-finder is the
// proven lattice, links persist per text, and the harvest exports as
// PENDING alignment candidates for the data project (rule 1: these are
// the TRANSLATOR'S OWN attestations, clearly tiered, never auto-bound).
// ---- Review pane: translator oversight (roadmap; leadership-doc
// recommendation #2) — the reviewer's side-by-side view over a
// FINISHED draft: register-sensitive terms, provisional-tier reliance,
// unmatched equivalents, collapsed distinctions, and sense-spread
// attention flags. Everything is review GUIDANCE — flags, never
// verdicts; the checker matches against HGM's glosses and never
// composes English (rule 1).
class ReviewPane : public QWidget {
public:
    ReviewPane(allcore::Spine& spine) : spine_(spine), index_(spine) {
        auto* outer = new QVBoxLayout(this);
        outer->addWidget(new QLabel(
            "<b>Review (oversight)</b> — paste the Tibetan source and the "
            "finished English draft under review, then Run review. Flags "
            "guide the reviewer's attention; nothing is auto-corrected."));
        auto* row = new QHBoxLayout;
        auto* openSrc = new QPushButton("Open source…");
        auto* openDraft = new QPushButton("Open draft…");
        auto* runB = new QPushButton("Run review");
        row->addWidget(openSrc);
        row->addWidget(openDraft);
        row->addWidget(runB);
        row->addStretch();
        outer->addLayout(row);
        auto* split = new QSplitter(Qt::Horizontal);
        src_ = new QPlainTextEdit;
        src_->setPlaceholderText("Tibetan source (ACIP)…");
        draft_ = new QPlainTextEdit;
        draft_->setPlaceholderText("The finished English draft…");
        split->addWidget(src_);
        split->addWidget(draft_);
        outer->addWidget(split, 1);
        report_ = new QTextBrowser;
        outer->addWidget(report_, 1);
        auto openInto = [this](QPlainTextEdit* into) {
            const QString f = QFileDialog::getOpenFileName(
                this, "Open file", QString(),
                "Text (*.txt *.act *.inc *.md);;All files (*)");
            if (f.isEmpty()) return;
            QFile qf(f);
            if (qf.open(QIODevice::ReadOnly))
                into->setPlainText(QString::fromUtf8(qf.readAll()));
        };
        connect(openSrc, &QPushButton::clicked,
                [openInto, this] { openInto(src_); });
        connect(openDraft, &QPushButton::clicked,
                [openInto, this] { openInto(draft_); });
        connect(runB, &QPushButton::clicked, [this] { review(); });
    }

    int selfTest(QStringList& log) {
        int fails = 0;
        auto check = [&](bool ok, const char* what) {
            log << QString("  [%1] Review: %2")
                       .arg(ok ? "PASS" : "FAIL")
                       .arg(what);
            if (!ok) ++fails;
        };
        // register-sensitive term with a matched rendering
        src_->setPlainText("BSOD NAMS");
        draft_->setPlainText("this merit is dedicated");
        review();
        const QString r1 = report_->toPlainText().toLower();
        check(r1.contains("register"),
              "register-sensitive term raises the register warning");
        // established term absent from the draft is surfaced
        src_->setPlainText("SEMS CAN THAMS CAD");
        draft_->setPlainText("every single one of them");
        review();
        check(report_->toPlainText().contains("sems can"),
              "unmatched established term is surfaced");
        // honorific in the source raises the register-of-respect note
        src_->setPlainText("GZIGS PA");
        draft_->setPlainText("he saw it");
        review();
        check(report_->toPlainText().contains("honorific"),
              "honorific source term raises the respect advisory");
        src_->clear();
        draft_->clear();
        return fails;
    }

private:
    void review() {
        const std::string source = src_->toPlainText().toStdString();
        const std::string draft = draft_->toPlainText().toStdString();
        if (source.empty() || draft.empty()) {
            report_->setHtml("<i>both the source and the draft are "
                             "needed</i>");
            return;
        }
        report_->setHtml("<i>reviewing…</i>");
        QCoreApplication::processEvents();
        auto rep = allcore::checkTerminology(spine_, index_, source, draft);

        // honorific register advisory: terms of respect in the source
        // deserve English that carries the dignity — surfaced for the
        // reviewer's judgment, never auto-resolved
        QString honHtml;
        if (g_honorifics) {
            // scan the source tokens directly (unigram + bigram): the
            // register keys on the honorific stem, which a longer
            // matched dictionary term can hide
            std::vector<std::string> toks;
            std::vector<bool> bars;
            allcore::tokenizeDocument(source, toks, bars);
            std::vector<std::string> wy;
            wy.reserve(toks.size());
            for (const auto& t : toks)
                wy.push_back(allcore::tokenToEwts(t));
            std::set<std::string> seen;
            std::vector<std::string> cands;
            for (size_t i = 0; i < wy.size(); ++i) {
                cands.push_back(wy[i]);
                if (i + 1 < wy.size() && !bars[i])
                    cands.push_back(wy[i] + " " + wy[i + 1]);
            }
            for (const auto& c : cands) {
                auto it = g_honorifics->find(c);
                if (it == g_honorifics->end() ||
                    !seen.insert(c).second)
                    continue;
                const auto& [ordinary, domain, level] = it->second;
                honHtml += "<div style='margin:4px 0'><b>" +
                           QString::fromStdString(c).toHtmlEscaped() +
                           "</b> — " +
                           (level == "high"
                                ? QString("<b>HIGH honorific</b>")
                                : level == "humilific"
                                    ? QString("humilific")
                                    : level == "double"
                                        ? QString("double honorific")
                                        : QString("honorific")) +
                           (ordinary.isEmpty()
                                ? QString()
                                : " (ordinary: " +
                                      ordinary.toHtmlEscaped() + ")") +
                           (domain.isEmpty()
                                ? QString()
                                : " <small style='color:#777'>" +
                                      domain.toHtmlEscaped() +
                                      "</small>") +
                           " — check that the English carries the "
                           "register of respect</div>";
            }
        }

        QString noteHtml;
        if (g_appNotes) {
            const QString d = QString::fromStdString(draft);
            int shown = 0;
            for (const auto& note : *g_appNotes) {
                if (note.lemma.size() < 4 ||
                    !d.contains(note.lemma, Qt::CaseInsensitive))
                    continue;
                if (++shown > 8) break;
                noteHtml += "<div style='margin:4px 0'><b>" +
                            note.lemma.toHtmlEscaped() +
                            "</b> — published footnote " +
                            QString::number(note.num) + " <small>(" +
                            note.source.toHtmlEscaped() +
                            ")</small></div>";
            }
        }
        int provisionalUsed = 0, unmatched = 0;
        QString regHtml, provHtml, unmHtml, spreadHtml;
        int spreadShown = 0;
        for (const auto& t : rep.terms) {
            // distinct gloss alternatives (sense spread)
            std::set<std::string> alts;
            for (const auto& g : t.glosses) {
                std::string low;
                for (char c : g)
                    low += (char)std::tolower((unsigned char)c);
                alts.insert(low);
            }
            const bool registerMarked = [&] {
                for (const auto& g : t.glosses)
                    if (g.find("register)") != std::string::npos)
                        return true;
                return false;
            }();
            if (registerMarked) {
                regHtml += "<div style='margin:4px 0'><b>" +
                           QString::fromStdString(t.wylie)
                               .toHtmlEscaped() +
                           "</b> — established senses differ by "
                           "register: <i>";
                QString gl;
                for (const auto& g : t.glosses) {
                    if (!gl.isEmpty()) gl += " · ";
                    gl += QString::fromStdString(g).toHtmlEscaped();
                }
                regHtml += gl + "</i> — confirm the rendering matches "
                           "this text's register.</div>";
            }
            if (t.provisional && !t.matched.empty()) {
                ++provisionalUsed;
                provHtml += "<div style='margin:3px 0'><b>" +
                            QString::fromStdString(t.wylie)
                                .toHtmlEscaped() +
                            "</b> rendered “" +
                            QString::fromStdString(t.matched.front())
                                .toHtmlEscaped() +
                            "” — this equivalent is <span style='color:"
                            "#b00'>PROVISIONAL</span> tier; verify "
                            "against the corpus.</div>";
            }
            if (t.matched.empty()) {
                ++unmatched;
                if (unmatched <= 12)
                    unmHtml += "<div style='margin:2px 0'><b>" +
                               QString::fromStdString(t.wylie)
                                   .toHtmlEscaped() +
                               "</b> ×" + QString::number(t.occurrences) +
                               " — none of HGM's equivalents (<i>" +
                               QString::fromStdString(
                                   t.glosses.empty() ? ""
                                                     : t.glosses.front())
                                   .toHtmlEscaped() +
                               "…</i>) appear in the draft. May be a "
                               "legitimate choice — worth a look.</div>";
            } else if (alts.size() >= 3 && spreadShown < 10) {
                ++spreadShown;
                spreadHtml += "<div style='margin:2px 0'><b>" +
                              QString::fromStdString(t.wylie)
                                  .toHtmlEscaped() +
                              "</b> has " +
                              QString::number(alts.size()) +
                              " established senses — confirm “" +
                              QString::fromStdString(t.matched.front())
                                  .toHtmlEscaped() +
                              "” is the right one here.</div>";
            }
        }
        QString sharedHtml;
        for (const auto& s : rep.shared) {
            QString ws;
            for (const auto& w : s.term_wylies) {
                if (!ws.isEmpty()) ws += ", ";
                ws += QString::fromStdString(w).toHtmlEscaped();
            }
            sharedHtml += "<div style='margin:3px 0'>“" +
                          QString::fromStdString(s.english)
                              .toHtmlEscaped() +
                          "” serves <b>" + ws +
                          "</b> — a distinction may have collapsed."
                          "</div>";
        }

        QString h = QString(
            "<div><b>Reviewer's report</b> — %1 term(s) anchored · %2 "
            "unmatched · %3 provisional-supported · %4 shared-English "
            "collapse(s)</div><div style='color:#777;font-size:12px'>"
            "flags are attention guidance, not errors; the reviewer "
            "decides</div><hr>")
                        .arg(rep.terms.size())
                        .arg(unmatched)
                        .arg(provisionalUsed)
                        .arg(rep.shared.size());
        if (!regHtml.isEmpty())
            h += "<div style='color:#8C2F2B'><b>Register-sensitive "
                 "terms</b></div>" + regHtml + "<hr>";
        if (!noteHtml.isEmpty())
            h += QString("<div style='color:#5A3B22'><b>published "
                         "footnotes apply (reuse, don't rewrite)</b>"
                         "</div>") +
                 noteHtml + "<hr>";
        if (!honHtml.isEmpty())
            h += QString("<div style='color:#7A5A00'><b>⚑ honorific "
                         "terms in the source (zhe sa)</b></div>") +
                 honHtml + "<hr>";
        if (!provHtml.isEmpty())
            h += "<div><b>Renderings resting on PROVISIONAL glosses"
                 "</b></div>" + provHtml + "<hr>";
        if (!sharedHtml.isEmpty())
            h += "<div><b>Possible collapsed distinctions</b></div>" +
                 sharedHtml + "<hr>";
        if (!unmHtml.isEmpty())
            h += "<div><b>Terms without an HGM equivalent in the draft"
                 "</b> <small>(first 12)</small></div>" + unmHtml +
                 "<hr>";
        if (!spreadHtml.isEmpty())
            h += "<div><b>Sense-spread attention</b> <small>(3+ "
                 "established senses; first 10)</small></div>" +
                 spreadHtml;
        report_->setHtml(h);
    }

    allcore::Spine& spine_;
    allcore::HeadwordIndex index_;
    QPlainTextEdit* src_ = nullptr;
    QPlainTextEdit* draft_ = nullptr;
    QTextBrowser* report_ = nullptr;
};

class AlignPane : public QWidget {
public:
    AlignPane(allcore::Spine& spine, const QString& root)
        : spine_(spine), root_(root), index_(spine) {
        auto* outer = new QVBoxLayout(this);
        outer->addWidget(new QLabel(
            "<b>Align (hypertext)</b> — the Hypercontext workflow: click a "
            "Tibetan word (click again to cycle the word-finder), select its "
            "English, press <b>space</b> or Link. Sublinks inside longer "
            "links show green. Links save per text; the harvest exports as "
            "PENDING candidates for the dictionary project."));
        auto* row = new QHBoxLayout;
        auto* open = new QPushButton("Open ACIP file…");
        row->addWidget(open);
        auto* hypBtn = new QPushButton("Import .hyp…");
        hypBtn->setToolTip(
            "Import a legacy Hypercontext hypertexted file: both texts "
            "and every numbered link come in as Align-pane links "
            "(sub-syllable links are widened to whole syllables and "
            "counted).");
        row->addWidget(hypBtn);
        connect(hypBtn, &QPushButton::clicked, [this] { importHyp(); });
        auto* load = new QPushButton("Load pasted texts");
        row->addWidget(load);
        linkBtn_ = new QPushButton("Link (space)");
        linkBtn_->setEnabled(false);
        row->addWidget(linkBtn_);
        auto* unlink = new QPushButton("Delete last link");
        row->addWidget(unlink);
        auto* exportBtn = new QPushButton("Export aligned pairs (PENDING)…");
        row->addWidget(exportBtn);
        row->addStretch();
        outer->addLayout(row);

        auto* split = new QSplitter(Qt::Horizontal);
        tib_ = new QTextEdit;
        tib_->setPlaceholderText("Tibetan (ACIP) — paste here, then Load");
        eng_ = new QTextEdit;
        eng_->setPlaceholderText("English translation — paste here, then Load");
        split->addWidget(tib_);
        split->addWidget(eng_);
        outer->addWidget(split, 2);
        links_ = new QTextBrowser;
        links_->setOpenLinks(false);
        links_->setMaximumHeight(150);
        outer->addWidget(links_);

        auto* sc = new QShortcut(QKeySequence(Qt::Key_Space), this);
        sc->setContext(Qt::WidgetWithChildrenShortcut);
        connect(sc, &QShortcut::activated, [this] { makeLink(); });
        connect(linkBtn_, &QPushButton::clicked, [this] { makeLink(); });
        connect(open, &QPushButton::clicked, [this] {
            const QString f = QFileDialog::getOpenFileName(
                this, "Open ACIP file", root_ + "/library");
            if (f.isEmpty()) return;
            QFile qf(f);
            if (!qf.open(QIODevice::ReadOnly)) return;
            tib_->setPlainText(QString::fromUtf8(qf.readAll()));
            docFile_ = f;
            loadTexts();
        });
        connect(load, &QPushButton::clicked, [this] {
            if (docFile_.isEmpty()) docFile_ = "untitled";
            loadTexts();
        });
        connect(unlink, &QPushButton::clicked, [this] {
            if (linksList_.empty()) return;
            linksList_.pop_back();
            saveLinks();
            repaint_();
        });
        connect(exportBtn, &QPushButton::clicked, [this] { exportPairs(); });
        connect(tib_, &QTextEdit::cursorPositionChanged, [this] {
            if (loading_ || doc_.tokens.empty()) return;
            onTibClick();
        });
        connect(links_, &QTextBrowser::anchorClicked, [this](const QUrl& u) {
            const QString s = u.toString();
            if (s.startsWith("del:")) {
                const int i = s.mid(4).toInt();
                if (i >= 0 && i < (int)linksList_.size()) {
                    linksList_.erase(linksList_.begin() + i);
                    saveLinks();
                    repaint_();
                }
            }
        });
    }

    int selfTest(QStringList& log) {
        int fails = 0;
        auto check = [&](bool ok, const char* what) {
            log << QString("  [%1] Align: %2")
                       .arg(ok ? "PASS" : "FAIL").arg(what);
            if (!ok) ++fails;
        };
        tib_->setPlainText("sems can thams cad bde ba dang ldan par "
                           "gyur cig");
        loadTexts();
        check(!doc_.tokens.empty(), "Tibetan side tokenized");
        check(docIsWylie_, "wylie input detected");
        check(tokEwts(doc_.tokens.front()) ==
                  doc_.tokens.front(),
              "harvest wylie stays lowercase for wylie docs");
        tib_->clear();
        loadTexts();
        return fails;
    }

private:
    struct Link {
        int tibBeg = 0, tibEnd = 0;   // token span [beg, end)
        std::string wylie;
        int engBeg = 0, engEnd = 0;   // char range in the English text
        std::string english;
    };

    QString linksPath() const {
        if (docFile_.isEmpty() || docFile_ == "untitled") return {};
        return root_ + "/library/links/" +
               QFileInfo(docFile_).completeBaseName() + ".tsv";
    }

    std::string tokEwts(const std::string& tok) const {
        return docIsWylie_ ? tok : allcore::acipToEwts(tok);
    }

    void loadTexts() {
        docIsWylie_ =
            allcore::looksLikeWylie(tib_->toPlainText().toStdString());
        doc_ = allcore::buildOverlay(spine_, index_,
                                     tib_->toPlainText().toStdString());
        // fixed-layout ACIP rendering with per-token positions
        loading_ = true;
        QString text;
        tokBeg_.clear();
        tokEnd_.clear();
        for (size_t i = 0; i < doc_.tokens.size(); ++i) {
            tokBeg_.push_back(text.size());
            text += QString::fromStdString(doc_.tokens[i]);
            tokEnd_.push_back(text.size());
            text += doc_.barrier_after[i] ? ",  " : " ";
        }
        tib_->setPlainText(text);
        tib_->setReadOnly(true);
        eng_->setReadOnly(true);
        loading_ = false;
        // load persisted links
        linksList_.clear();
        const QString p = linksPath();
        if (!p.isEmpty()) {
            QFile f(p);
            if (f.open(QIODevice::ReadOnly | QIODevice::Text)) {
                while (!f.atEnd()) {
                    const QStringList c =
                        QString::fromUtf8(f.readLine()).trimmed().split('\t');
                    if (c.size() < 6 || c[0].startsWith('#')) continue;
                    Link l;
                    l.tibBeg = c[0].toInt();
                    l.tibEnd = c[1].toInt();
                    l.wylie = c[2].toStdString();
                    l.engBeg = c[3].toInt();
                    l.engEnd = c[4].toInt();
                    l.english = c[5].toStdString();
                    linksList_.push_back(l);
                }
            }
        }
        lastTok_ = -1;
        cycle_ = 0;
        selBeg_ = selEnd_ = -1;
        repaint_();
    }

    void onTibClick() {
        const int pos = tib_->textCursor().position();
        int tok = -1;
        for (size_t i = 0; i < tokBeg_.size(); ++i)
            if (pos >= tokBeg_[i] && pos <= tokEnd_[i]) { tok = (int)i; break; }
        if (tok < 0) return;
        auto at = doc_.spansAt(tok);
        if (tok == lastTok_) cycle_ = (cycle_ + 1) % (int)(at.size() + 1);
        else cycle_ = 0;
        lastTok_ = tok;
        if (!at.empty() && cycle_ < (int)at.size()) {
            const auto& sp = doc_.spans[at[cycle_]];
            selBeg_ = sp.beg;
            selEnd_ = sp.end;
        } else {  // bare token (the word-finder found nothing / cycled past)
            selBeg_ = tok;
            selEnd_ = tok + 1;
        }
        linkBtn_->setEnabled(true);
        repaint_();
    }

    static QColor linkColor(bool nested) {
        return nested ? QColor(0x9E, 0xD9, 0xA0) : QColor(0xAB, 0xCD, 0xEF);
    }

    void repaint_() {
        // permanent link tints + current selection, via extraSelections
        QList<QTextEdit::ExtraSelection> tsel, esel;
        for (size_t i = 0; i < linksList_.size(); ++i) {
            const Link& l = linksList_[i];
            bool nested = false;
            for (const Link& o : linksList_)
                if (&o != &l && l.tibBeg >= o.tibBeg && l.tibEnd <= o.tibEnd)
                    nested = true;
            if (l.tibBeg < (int)tokBeg_.size() &&
                l.tibEnd <= (int)tokEnd_.size() && l.tibEnd > l.tibBeg) {
                QTextEdit::ExtraSelection s;
                s.cursor = QTextCursor(tib_->document());
                s.cursor.setPosition(tokBeg_[l.tibBeg]);
                s.cursor.setPosition(tokEnd_[l.tibEnd - 1],
                                     QTextCursor::KeepAnchor);
                s.format.setBackground(linkColor(nested));
                tsel << s;
            }
            QTextEdit::ExtraSelection s2;
            s2.cursor = QTextCursor(eng_->document());
            s2.cursor.setPosition(l.engBeg);
            s2.cursor.setPosition(std::min(l.engEnd,
                                           (int)eng_->toPlainText().size()),
                                  QTextCursor::KeepAnchor);
            s2.format.setBackground(linkColor(nested));
            esel << s2;
        }
        if (selBeg_ >= 0 && selBeg_ < (int)tokBeg_.size() && selEnd_ > selBeg_) {
            QTextEdit::ExtraSelection s;
            s.cursor = QTextCursor(tib_->document());
            s.cursor.setPosition(tokBeg_[selBeg_]);
            s.cursor.setPosition(tokEnd_[selEnd_ - 1], QTextCursor::KeepAnchor);
            s.format.setBackground(QColor(0xC9, 0xA9, 0xE9));  // purple
            tsel << s;
        }
        tib_->setExtraSelections(tsel);
        eng_->setExtraSelections(esel);
        // links list
        QString h = QString("<b>%1 link(s)</b> — ").arg(linksList_.size());
        for (size_t i = 0; i < linksList_.size(); ++i)
            h += QString("<span style='white-space:nowrap'>[%1 = %2 "
                         "<a href='del:%3'>×</a>]</span>  ")
                     .arg(QString::fromStdString(linksList_[i].wylie)
                              .toHtmlEscaped(),
                          QString::fromStdString(linksList_[i].english)
                              .left(40)
                              .toHtmlEscaped())
                     .arg(i);
        links_->setHtml(h);
    }

    void makeLink() {
        if (selBeg_ < 0 || doc_.tokens.empty()) return;
        const QTextCursor ec = eng_->textCursor();
        if (!ec.hasSelection()) {
            links_->setHtml("<i>select the matching English first, then "
                            "space/Link</i>");
            return;
        }
        Link l;
        l.tibBeg = selBeg_;
        l.tibEnd = selEnd_;
        std::string wy;
        for (int t = selBeg_; t < selEnd_; ++t) {
            if (!wy.empty()) wy += " ";
            wy += tokEwts(doc_.tokens[t]);
        }
        l.wylie = wy;
        l.engBeg = ec.selectionStart();
        l.engEnd = ec.selectionEnd();
        l.english = ec.selectedText().toStdString();
        linksList_.push_back(l);
        saveLinks();
        selBeg_ = selEnd_ = -1;
        linkBtn_->setEnabled(false);
        repaint_();
    }

    // Import a legacy Hypercontext .hyp file (allcore::parseHypFile,
    // proven on tutorial.hyp): texts split by script, numbered links
    // become Align links. Token mapping is honest — sub-syllable links
    // widen to whole tokens (counted), unmappable links are skipped
    // (counted), never guessed.
    void importHyp() {
        const QString fn = QFileDialog::getOpenFileName(
            this, "Import Hypercontext file", QString(),
            "Hypercontext (*.hyp);;All files (*)");
        if (fn.isEmpty()) return;
        QFile qf(fn);
        if (!qf.open(QIODevice::ReadOnly)) return;
        const auto hyp =
            allcore::parseHypFile(qf.readAll().toStdString());
        if (hyp.tibetan.empty()) {
            links_->setHtml("<b>no Tibetan text in that .hyp file</b>");
            return;
        }
        tib_->setReadOnly(false);
        tib_->setPlainText(QString::fromStdString(hyp.tibetan));
        eng_->setPlainText(QString::fromStdString(hyp.english));
        docFile_ = fn;
        loadTexts();
        // token -> byte range in the .hyp Tibetan text (in-order find;
        // ACIP is plain ASCII so byte alignment is exact)
        const std::string& raw = hyp.tibetan;
        std::vector<int> rb, re2;
        size_t cur = 0;
        bool mapOk = true;
        for (const auto& tok : doc_.tokens) {
            const size_t p = raw.find(tok, cur);
            if (p == std::string::npos) {
                mapOk = false;
                break;
            }
            rb.push_back(static_cast<int>(p));
            re2.push_back(static_cast<int>(p + tok.size()));
            cur = p + tok.size();
        }
        int imported = 0, widened = 0, skippedN = 0;
        if (mapOk) {
            linksList_.clear();
            for (const auto& hl : hyp.links) {
                int t0 = -1, t1 = -1;
                for (size_t i = 0; i < rb.size(); ++i)
                    if (re2[i] > hl.tibBeg && rb[i] < hl.tibEnd) {
                        if (t0 < 0) t0 = static_cast<int>(i);
                        t1 = static_cast<int>(i) + 1;
                    }
                if (t0 < 0) {
                    ++skippedN;
                    continue;
                }
                if (rb[t0] < hl.tibBeg || re2[t1 - 1] > hl.tibEnd)
                    ++widened;   // sub-syllable link -> whole token(s)
                Link l;
                l.tibBeg = t0;
                l.tibEnd = t1;
                std::string wy;
                for (int t = t0; t < t1; ++t) {
                    if (!wy.empty()) wy += " ";
                    wy += tokEwts(doc_.tokens[t]);
                }
                l.wylie = wy;
                // byte offsets -> QString (UTF-16) positions
                l.engBeg = QString::fromUtf8(hyp.english.c_str(),
                                             hl.engBeg).size();
                l.engEnd = QString::fromUtf8(hyp.english.c_str(),
                                             hl.engEnd).size();
                l.english = hyp.english.substr(
                    hl.engBeg, hl.engEnd - hl.engBeg);
                linksList_.push_back(std::move(l));
                ++imported;
            }
            saveLinks();
        }
        repaint_();
        links_->setHtml(
            QString("<div><b>.hyp import:</b> %1 — %2 link(s) imported"
                    "%3%4%5%6%7</div>")
                .arg(QFileInfo(fn).fileName().toHtmlEscaped())
                .arg(imported)
                .arg(widened
                         ? QString(" · %1 sub-syllable link(s) widened "
                                   "to whole syllables").arg(widened)
                         : QString())
                .arg(skippedN
                         ? QString(" · <span style='color:#B4540A'>%1 "
                                   "unmappable, skipped</span>")
                               .arg(skippedN)
                         : QString())
                .arg(hyp.onesided.empty()
                         ? QString()
                         : QString(" · <span style='color:#B4540A'>%1 "
                                   "one-sided link id(s) ignored</span>")
                               .arg(hyp.onesided.size()))
                .arg(hyp.notes.empty()
                         ? QString()
                         : QString(" · %1 note(s) in the file")
                               .arg(hyp.notes.size()))
                .arg(mapOk ? QString()
                           : " · <span style='color:#B00020'><b>token "
                             "mapping failed — links not imported"
                             "</b></span>") +
            links_->toHtml());
    }

    void saveLinks() {
        const QString p = linksPath();
        if (p.isEmpty()) return;
        QDir().mkpath(QFileInfo(p).path());
        QFile f(p);
        if (!f.open(QIODevice::WriteOnly | QIODevice::Text)) return;
        QTextStream ts(&f);
        ts << "# align links for " << QFileInfo(docFile_).fileName()
           << " — tibBeg tibEnd wylie engBeg engEnd english (translator's "
              "own; PENDING tier)\n";
        for (const Link& l : linksList_)
            ts << l.tibBeg << "\t" << l.tibEnd << "\t"
               << QString::fromStdString(l.wylie) << "\t" << l.engBeg << "\t"
               << l.engEnd << "\t"
               << QString::fromStdString(l.english).replace('\t', ' ')
               << "\n";
    }

    void exportPairs() {
        if (linksList_.empty()) return;
        QDir().mkpath(root_ + "/data/candidate_alignments");
        const QString base = docFile_ == "untitled"
                                 ? "untitled"
                                 : QFileInfo(docFile_).completeBaseName();
        const QString out = QFileDialog::getSaveFileName(
            this, "Export aligned pairs",
            root_ + "/data/candidate_alignments/" + base + "-pairs.tsv",
            "TSV (*.tsv)");
        if (out.isEmpty()) return;
        QFile f(out);
        if (!f.open(QIODevice::WriteOnly | QIODevice::Text)) return;
        QTextStream ts(&f);
        ts << "# PENDING alignment candidates (translator-authored in the "
              "Align pane) — for the data project's review; never "
              "auto-ingested\n# source: "
           << docFile_ << "\n";
        for (const Link& l : linksList_)
            ts << QString::fromStdString(l.wylie) << "\t"
               << QString::fromStdString(l.english).replace('\t', ' ')
               << "\n";
        links_->setHtml(links_->toHtml() +
                        "<div style='color:#3B7A3B'>exported " +
                        QString::number(linksList_.size()) + " pair(s) to " +
                        out.toHtmlEscaped() + "</div>");
    }

    allcore::Spine& spine_;
    QString root_, docFile_;
    allcore::HeadwordIndex index_;
    allcore::OverlayDoc doc_;
    bool docIsWylie_ = false;
    std::vector<int> tokBeg_, tokEnd_;
    std::vector<Link> linksList_;
    QTextEdit* tib_ = nullptr;
    QTextEdit* eng_ = nullptr;
    QTextBrowser* links_ = nullptr;
    QPushButton* linkBtn_ = nullptr;
    bool loading_ = false;
    int lastTok_ = -1, cycle_ = 0, selBeg_ = -1, selEnd_ = -1;
};

// ---- Input pane: the input-center workflow — ACE reborn -------------------
// Aaron Cram's ACIP input-center module (acip-support, Apache-2.0;
// docs/ACE_RECOVERY.md) rebuilt on our stack: scan above, ACIP editor
// below, caret-linked scan scroll (ACE's proportional mode, upgraded to
// exact OCR line bands when models are present), live syllable-legality
// underlines, and the double-keying comparison (vendored google
// diff-match-patch, Apache-2.0) that the correction pass ran "until the
// text input by both input operators match exactly".
#include "thirdparty/diff_match_patch.h"

class InputPane : public QWidget {
public:
    InputPane(allcore::SyllableChecker* checker, const QString& root)
        : checker_(checker), root_(root) {
        auto* outer = new QVBoxLayout(this);
        auto* banner = new QLabel(
            "<b>Input (type from scan)</b> — the input-center workflow "
            "(ACE reborn; Aaron Cram's acip-support design, Apache-2.0). "
            "Open the page scan, type the ACIP below; the scan follows "
            "your cursor. Compare against your partner's file to run the "
            "double-keying correction pass.");
        banner->setWordWrap(true);
        outer->addWidget(banner);

        auto* row = new QHBoxLayout;
        auto* openScanB = new QPushButton("Open scan…");
        row->addWidget(openScanB);
        auto* openFolderB = new QPushButton("Open scan folder…");
        openFolderB->setToolTip(
            "The block workflow: open a whole folder of page scans and "
            "work through them in order — each page's typing saves to "
            "its own file in library/input_work/<folder>/ and reloads "
            "when you return to the page.");
        row->addWidget(openFolderB);
        prevPageB_ = new QPushButton("◀ page");
        nextPageB_ = new QPushButton("page ▶");
        pageLbl_ = new QLabel;
        prevPageB_->setEnabled(false);
        nextPageB_->setEnabled(false);
        row->addWidget(prevPageB_);
        row->addWidget(pageLbl_);
        row->addWidget(nextPageB_);
        connect(openFolderB, &QPushButton::clicked,
                [this] { openFolder(); });
        connect(prevPageB_, &QPushButton::clicked,
                [this] { gotoPage(pageIx_ - 1); });
        connect(nextPageB_, &QPushButton::clicked,
                [this] { gotoPage(pageIx_ + 1); });
        followBox_ = new QCheckBox("scan follows cursor");
        followBox_->setChecked(true);
        row->addWidget(followBox_);
#ifdef ALL_HAVE_OCR
        auto* bandsB = new QPushButton("Detect lines (OCR)");
        bandsB->setToolTip(
            "Run line detection on the scan (BDRC PhotiLines model) so "
            "cursor-following jumps to the EXACT line band instead of "
            "ACE's proportional estimate. Detection only — no text is "
            "recognized.");
        row->addWidget(bandsB);
        connect(bandsB, &QPushButton::clicked, [this] { detectBands(); });
        auto* prefillB = new QPushButton("Pre-fill from OCR (draft)");
        prefillB->setToolTip(
            "OCR-assisted input: recognize this page (BDRC Woodblock "
            "model, CC BY-NC, with permission) and fill the EMPTY "
            "editor with the draft in ACIP for you to CORRECT against "
            "the scan — every syllable the spellchecker doubts is "
            "underlined. The draft is ocr-derived: correcting it is "
            "your typing pass; the double-keying comparison against "
            "your partner still applies unchanged.");
        row->addWidget(prefillB);
        connect(prefillB, &QPushButton::clicked, [this] { prefill(); });
#endif
        auto* zoomL = new QLabel("zoom");
        row->addWidget(zoomL);
        zoom_ = new QSlider(Qt::Horizontal);
        zoom_->setRange(25, 200);
        zoom_->setValue(100);
        zoom_->setMaximumWidth(120);
        row->addWidget(zoom_);
        auto* folioB = new QPushButton("@ next folio");
        folioB->setToolTip(
            "Insert the next folio marker per the ACIP spec (@ in "
            "column 1, 3-digit zero-padded, side A then B): after "
            "@001A comes @001B, then @002A. Starts at @001A in an "
            "empty document.");
        row->addWidget(folioB);
        connect(folioB, &QPushButton::clicked, [this] { nextFolio(); });
        auto* partnerB = new QPushButton("Compare with partner file…");
        row->addWidget(partnerB);
        auto* saveB = new QPushButton("Save…");
        row->addWidget(saveB);
        row->addStretch();
        outer->addLayout(row);

        auto* split = new QSplitter(Qt::Vertical);
        scroll_ = new QScrollArea;
        scanImg_ = new QLabel("open a page scan (PNG/JPG/TIFF)");
        scanImg_->setAlignment(Qt::AlignCenter);
        scroll_->setWidget(scanImg_);
        scroll_->setWidgetResizable(true);
        split->addWidget(scroll_);
        editor_ = new QPlainTextEdit;
        editor_->setPlaceholderText(
            "Type the ACIP transliteration here, one line per woodblock "
            "line…");
        split->addWidget(editor_);
        split->setStretchFactor(0, 7);
        split->setStretchFactor(1, 3);
        outer->addWidget(split, 1);
        status_ = new QLabel;
        status_->setWordWrap(true);
        outer->addWidget(status_);

        connect(openScanB, &QPushButton::clicked, [this] { openScan(); });
        connect(partnerB, &QPushButton::clicked, [this] { compare(); });
        connect(saveB, &QPushButton::clicked, [this] { save(); });
        connect(zoom_, &QSlider::valueChanged, [this](int) { render(); });
        connect(editor_, &QPlainTextEdit::cursorPositionChanged,
                [this] { follow(); });
        connect(editor_, &QPlainTextEdit::textChanged,
                [this] { spellcheck(); });
    }

    int selfTest(QStringList& log) {
        int fails = 0;
        auto check = [&](bool ok, const char* what) {
            log << QString("  [%1] Input: %2")
                       .arg(ok ? "PASS" : "FAIL").arg(what);
            if (!ok) ++fails;
        };
        editor_->setPlainText("SEMS CAN THAMS CAD BDE BA DANG,");
        compareWith("SEMS CAN THAMS CAD BDE BA DANG,");
        check(status_->text().contains("match exactly"),
              "identical double-keying reports clean");
        compareWith("SEMS CAN THAMS CAD BDE MA DANG,");
        check(status_->text().contains("1") ||
                  status_->text().contains("discrep"),
              "planted difference is flagged");
        check(!canPrefill(),
              "pre-fill refuses a non-empty editor (never overwrites)");
        editor_->clear();
        check(canPrefill(), "pre-fill allowed on an empty page");
        return fails;
    }

private:
    void openScan() {
        const QString f = QFileDialog::getOpenFileName(
            this, "Open page scan", root_ + "/library",
            "Images (*.png *.jpg *.jpeg *.tif *.tiff)");
        if (f.isEmpty()) return;
        base_ = QPixmap(f);
        if (base_.isNull()) {
            status_->setText("cannot read that image");
            return;
        }
        scanFile_ = f;
#ifdef ALL_HAVE_OCR
        bands_.clear();
#endif
        curBand_ = -1;
        render();
        status_->setText(QString("%1 · %2×%3 — type below; the scan "
                                 "follows your cursor (%4)")
                             .arg(QFileInfo(f).fileName())
                             .arg(base_.width())
                             .arg(base_.height())
                             .arg(bandNote()));
    }

    QString bandNote() const {
#ifdef ALL_HAVE_OCR
        if (!bands_.empty())
            return QString("exact OCR line bands, %1 line(s)")
                .arg(bands_.size());
#endif
        return "proportional — ACE mode, approximate";
    }

    void render() {
        if (base_.isNull()) return;
        const double z = zoom_->value() / 100.0;
        QPixmap shown = base_.scaled(
            int(base_.width() * z), int(base_.height() * z),
            Qt::KeepAspectRatio, Qt::SmoothTransformation);
        QPainter p(&shown);
#ifdef ALL_HAVE_OCR
        for (size_t i = 0; i < bands_.size(); ++i) {
            const auto& b = bands_[i];
            const QRect r(int(b.x * z), int(b.y * z), int(b.w * z),
                          int(b.h * z));
            if (int(i) == curBand_) {
                p.fillRect(r, QColor(255, 190, 0, 60));
                p.setPen(QPen(QColor(230, 150, 0), 2));
            } else {
                p.setPen(QPen(QColor(0x7F, 0x77, 0xDD, 120), 1));
            }
            p.drawRect(r);
        }
#endif
        p.end();
        scanImg_->setPixmap(shown);
        scanImg_->adjustSize();
    }

#ifdef ALL_HAVE_OCR
    void detectBands() {
        if (base_.isNull()) {
            status_->setText("open a scan first");
            return;
        }
        const QString lm = root_ +
            "/library/ocr_models/BDRC_PhotiLines/PhotiLines.onnx";
        if (!QFile::exists(lm)) {
            status_->setText("line model missing (see the Scan pane for "
                             "download instructions)");
            return;
        }
        status_->setText("detecting lines…");
        QCoreApplication::processEvents();
        try {
            if (!det_)
                det_ = std::make_unique<allocr::LineDetector>(
                    lm.toStdString());
            QImage img =
                base_.toImage().convertToFormat(QImage::Format_RGB888);
            const int w = img.width(), h = img.height();
            std::vector<uint8_t> rgb(size_t(w) * h * 3);
            for (int y = 0; y < h; ++y)
                std::copy(img.constScanLine(y),
                          img.constScanLine(y) + w * 3,
                          rgb.begin() + size_t(y) * w * 3);
            auto mask = det_->detect(rgb.data(), w, h);
            const double zero = 0.0;   // original-image coordinates
            auto pl = allocr::buildLines(rgb.data(), w, h, mask, 2.5, 4.0,
                                         true, &zero);
            bands_.clear();
            for (const auto& l : pl.lines) bands_.push_back(l);
        } catch (const std::exception& e) {
            status_->setText(QString("line detection failed: %1")
                                 .arg(e.what()));
            return;
        }
        render();
        status_->setText(QString("%1 line band(s) — cursor now follows "
                                 "exact lines (typed line N = scan line "
                                 "N, per the input convention)")
                             .arg(bands_.size()));
    }

    // OCR-assisted input: recognize the page and fill the EMPTY editor
    // with the ACIP draft for the operator to CORRECT. The draft is
    // ocr-derived; correcting it is the typing pass, and double-keying
    // against the partner applies unchanged on top.
    // pre-fill only ever starts from an empty page (never overwrites
    // typing) — the guard is a named predicate so the selftest pins it
    bool canPrefill() const {
        return editor_->toPlainText().trimmed().isEmpty();
    }

    void prefill() {
        if (base_.isNull()) {
            status_->setText("open a scan first");
            return;
        }
        if (!canPrefill()) {
            status_->setText(
                "the editor already has typing — pre-fill only starts "
                "from an empty page (it never overwrites your work)");
            return;
        }
        const QString od = root_ + "/library/ocr_models/BDRC_Woodblock";
        if (!QFile::exists(od + "/OCRModel.onnx")) {
            status_->setText("recognition model missing (see the Scan "
                             "pane for download instructions)");
            return;
        }
        if (bands_.empty()) detectBands();
        if (bands_.empty()) return;
        try {
            if (!rec_)
                rec_ = std::make_unique<allocr::TextRecognizer>(
                    od.toStdString());
            // rebuild the full line strips for recognition
            QImage img =
                base_.toImage().convertToFormat(QImage::Format_RGB888);
            const int w = img.width(), h = img.height();
            std::vector<uint8_t> rgb(size_t(w) * h * 3);
            for (int y = 0; y < h; ++y)
                std::copy(img.constScanLine(y),
                          img.constScanLine(y) + w * 3,
                          rgb.begin() + size_t(y) * w * 3);
            auto mask = det_->detect(rgb.data(), w, h);
            const double zero = 0.0;
            auto pl = allocr::buildLines(rgb.data(), w, h, mask, 2.5,
                                         4.0, true, &zero);
            QStringList lines;
            for (size_t i = 0; i < pl.images.size(); ++i) {
                status_->setText(QString("recognizing line %1/%2…")
                                     .arg(i + 1)
                                     .arg(pl.images.size()));
                QCoreApplication::processEvents();
                const std::string wy = rec_->recognize(pl.images[i]);
                // wylie -> ACIP via the round-trip-proven reverse
                lines << QString::fromStdString(
                    allcore::ewtsToAcip(wy));
            }
            editor_->setPlainText(lines.join("\n"));
        } catch (const std::exception& e) {
            status_->setText(QString("pre-fill failed: %1").arg(e.what()));
            return;
        }
        status_->setText(
            QString("OCR draft filled: %1 line(s), ocr-derived — "
                    "CORRECT it against the scan (doubtful syllables "
                    "are underlined); the double-keying pass vs your "
                    "partner applies as always")
                .arg(bands_.size()));
    }
#endif

    void follow() {
        if (base_.isNull() || !followBox_->isChecked()) return;
        const QTextCursor c = editor_->textCursor();
        const int line = c.blockNumber();
        const int total = std::max(editor_->document()->blockCount(), 1);
        const int col = c.positionInBlock();
        const double z = zoom_->value() / 100.0;
#ifdef ALL_HAVE_OCR
        if (!bands_.empty()) {
            const int i =
                std::min(line, int(bands_.size()) - 1);
            if (i != curBand_) {
                curBand_ = i;
                render();
            }
            const auto& b = bands_[size_t(i)];
            const int lineLen =
                std::max(int(c.block().text().size()), 1);
            const int x = int((b.x + b.w * std::min(1.0,
                              double(col) / lineLen)) * z);
            scroll_->ensureVisible(x, int((b.y + b.h / 2) * z), 160, 120);
            return;
        }
#endif
        // ACE's proportional mode (acip-support ScanPanel CARET mode) —
        // approximate by design, labeled in the banner
        const int y = int(base_.height() * z * line / total);
        const int x = int(base_.width() * z *
                          std::min(1.0, col / 80.0));
        scroll_->ensureVisible(x, y, 160, 120);
    }

    void spellcheck() {
        spellSels_.clear();
        if (checker_) {
            const QString text = editor_->toPlainText();
            // full-document re-scan per keystroke: fine at page scale,
            // pathological on a pasted book — guard and say so
            if (text.size() > 200000) {
                status_->setText(
                    "live spellcheck paused (document over 200k "
                    "characters — the Overlay's checker handles full "
                    "texts)");
                editor_->setExtraSelections(diffSels_);
                return;
            }
            int tokBeg = -1;
            auto isTok = [](QChar ch) {
                return ch.isLetter() || ch == '\'' || ch == '+';
            };
            for (int i = 0; i <= text.size(); ++i) {
                const bool in = i < text.size() && isTok(text[i]);
                if (in && tokBeg < 0) tokBeg = i;
                if (!in && tokBeg >= 0) {
                    const QString tok = text.mid(tokBeg, i - tokBeg);
                    if (!checker_->legalWylie(allcore::acipToEwts(
                            tok.toStdString()))) {
                        QTextEdit::ExtraSelection sel;
                        sel.cursor = QTextCursor(editor_->document());
                        sel.cursor.setPosition(tokBeg);
                        sel.cursor.setPosition(
                            i, QTextCursor::KeepAnchor);
                        sel.format.setUnderlineStyle(
                            QTextCharFormat::WaveUnderline);
                        sel.format.setUnderlineColor(QColor(0xC0281C));
                        spellSels_.push_back(sel);
                    }
                    tokBeg = -1;
                }
            }
        }
        editor_->setExtraSelections(spellSels_ + diffSels_);
    }

    void compare() {
        const QString f = QFileDialog::getOpenFileName(
            this, "Partner's input file", root_ + "/library",
            "Text (*.txt *.act *.inc);;All files (*)");
        if (f.isEmpty()) return;
        QFile qf(f);
        if (!qf.open(QIODevice::ReadOnly)) return;
        compareWith(QString::fromUtf8(qf.readAll()));
    }

    // the double-keying diff itself, dialog-free (selftest-covered)
    void compareWith(const QString& partner) {
        const QString mine = editor_->toPlainText();
        // diff cost grows with divergence; keep the correction pass at
        // block scale (the input workflow compares blocks, not volumes)
        if (mine.size() > 500000 || partner.size() > 500000) {
            status_->setText(
                "compare is for input blocks — one of these texts is "
                "over 500k characters; split it first");
            return;
        }
        status_->setText("comparing…");
        QCoreApplication::processEvents();
        diff_match_patch dmp;
        auto diffs = dmp.diff_main(mine, partner);
        dmp.diff_cleanupSemantic(diffs);
        diffSels_.clear();
        int minePos = 0, nDisc = 0;
        for (const auto& d : diffs) {
            if (d.operation == DELETE) {
                // in my text, not the partner's
                QTextEdit::ExtraSelection sel;
                sel.cursor = QTextCursor(editor_->document());
                sel.cursor.setPosition(minePos);
                sel.cursor.setPosition(
                    minePos + int(d.text.length()),
                    QTextCursor::KeepAnchor);
                sel.format.setBackground(QColor(0xF5, 0xC0, 0x66, 150));
                diffSels_.push_back(sel);
                ++nDisc;
            } else if (d.operation == INSERT) {
                // in the partner's text, missing here: mark the seam
                QTextEdit::ExtraSelection sel;
                sel.cursor = QTextCursor(editor_->document());
                sel.cursor.setPosition(std::max(minePos - 1, 0));
                sel.cursor.setPosition(
                    std::min(minePos + 1,
                             int(editor_->toPlainText().size())),
                    QTextCursor::KeepAnchor);
                sel.format.setBackground(QColor(0x9E, 0xD9, 0xA0, 150));
                diffSels_.push_back(sel);
                ++nDisc;
            }
            if (d.operation != INSERT) minePos += int(d.text.length());
        }
        editor_->setExtraSelections(spellSels_ + diffSels_);
        status_->setText(
            nDisc == 0
                ? QString("double-keying PASS — your text and the "
                          "partner file match exactly")
                : QString("%1 discrepancy region(s) — orange = here "
                          "only, green seam = partner has extra text "
                          "there. Correct until the texts match "
                          "exactly (the input-center rule).")
                      .arg(nDisc));
    }

    // ---- the block workflow: a folder of pages, typed in order ----
    void openFolder() {
        const QString dir = QFileDialog::getExistingDirectory(
            this, "Folder of page scans", root_ + "/library");
        if (dir.isEmpty()) return;
        pages_.clear();
        for (const QString& e : QDir(dir).entryList(
                 {"*.png", "*.jpg", "*.jpeg", "*.tif", "*.tiff"},
                 QDir::Files, QDir::Name))
            pages_ << dir + "/" + e;
        if (pages_.isEmpty()) {
            status_->setText("no page images in that folder");
            return;
        }
        workDir_ = root_ + "/library/input_work/" +
                   QFileInfo(dir).fileName();
        QDir().mkpath(workDir_);
        pageIx_ = -1;
        gotoPage(0);
        status_->setText(
            QString("%1 page(s) — typing saves per page into %2; use "
                    "◀ ▶ to move through the block")
                .arg(pages_.size())
                .arg(workDir_));
    }

    QString pageTextFile(int ix) const {
        return workDir_ +
               QString("/page_%1_%2.txt")
                   .arg(ix + 1, 3, 10, QChar('0'))
                   .arg(QFileInfo(pages_[ix]).completeBaseName());
    }

    void gotoPage(int ix) {
        if (ix < 0 || ix >= pages_.size()) return;
        // save the current page's typing before moving
        if (pageIx_ >= 0 && pageIx_ < pages_.size()) {
            QFile f(pageTextFile(pageIx_));
            if (f.open(QIODevice::WriteOnly | QIODevice::Text))
                f.write(editor_->toPlainText().toUtf8());
        }
        pageIx_ = ix;
        base_ = QPixmap(pages_[ix]);
        scanFile_ = pages_[ix];
#ifdef ALL_HAVE_OCR
        bands_.clear();
#endif
        curBand_ = -1;
        render();
        // load this page's typing, if any
        QFile f(pageTextFile(ix));
        editor_->setPlainText(
            f.open(QIODevice::ReadOnly | QIODevice::Text)
                ? QString::fromUtf8(f.readAll())
                : QString());
        pageLbl_->setText(QString("page %1/%2").arg(ix + 1)
                              .arg(pages_.size()));
        prevPageB_->setEnabled(ix > 0);
        nextPageB_->setEnabled(ix + 1 < pages_.size());
        status_->setText(QString("%1 (%2) — %3")
                             .arg(QFileInfo(pages_[ix]).fileName())
                             .arg(pageLbl_->text())
                             .arg(bandNote()));
    }

    // insert the next @dddX folio marker (ACIP spec: column 1,
    // 3-digit zero-padded, sides A -> B -> next number's A)
    void nextFolio() {
        const QString text = editor_->toPlainText();
        QRegularExpression re("@0*(\\d+)([AB])");
        int num = 0;
        QChar side = 'B';   // so an empty doc starts at 001A
        auto it = re.globalMatch(text);
        while (it.hasNext()) {
            const auto m = it.next();
            num = m.captured(1).toInt();
            side = m.captured(2)[0];
        }
        QString marker;
        if (side == 'A')
            marker = QString("@%1B").arg(num, 3, 10, QChar('0'));
        else
            marker = QString("@%1A").arg(num + 1, 3, 10, QChar('0'));
        QTextCursor c = editor_->textCursor();
        // the spec puts @ in column 1: open a fresh line when needed
        const bool atLineStart = c.positionInBlock() == 0;
        c.insertText((atLineStart ? "" : "\n") + marker + "\n");
        editor_->setTextCursor(c);
        editor_->setFocus();
    }

    void save() {
        QDir().mkpath(root_ + "/library/input_work");
        // block mode: flush the current page, then export the whole
        // block combined in page order
        if (!pages_.isEmpty() && pageIx_ >= 0) {
            gotoPage(pageIx_);   // persists the current page's text
            const QString out = QFileDialog::getSaveFileName(
                this, "Export combined block",
                workDir_ + "-combined.txt", "Text (*.txt)");
            if (out.isEmpty()) return;
            QFile f(out);
            if (!f.open(QIODevice::WriteOnly | QIODevice::Text)) return;
            int nonEmpty = 0;
            for (int i = 0; i < pages_.size(); ++i) {
                QFile pf(pageTextFile(i));
                if (pf.open(QIODevice::ReadOnly | QIODevice::Text)) {
                    const QByteArray t = pf.readAll();
                    if (!t.trimmed().isEmpty()) {
                        f.write(t);
                        if (!t.endsWith('\n')) f.write("\n");
                        ++nonEmpty;
                    }
                }
            }
            status_->setText(
                QString("combined %1 typed page(s) of %2 into %3")
                    .arg(nonEmpty)
                    .arg(pages_.size())
                    .arg(out));
            return;
        }
        const QString base =
            scanFile_.isEmpty() ? "input"
                                : QFileInfo(scanFile_).completeBaseName();
        const QString out = QFileDialog::getSaveFileName(
            this, "Save input work",
            root_ + "/library/input_work/" + base + ".txt",
            "Text (*.txt)");
        if (out.isEmpty()) return;
        QFile f(out);
        if (!f.open(QIODevice::WriteOnly | QIODevice::Text)) return;
        f.write(editor_->toPlainText().toUtf8());
        status_->setText("saved " + out);
    }

    allcore::SyllableChecker* checker_ = nullptr;
    QString root_, scanFile_;
    QPixmap base_;
    QLabel* scanImg_ = nullptr;
    QScrollArea* scroll_ = nullptr;
    QPlainTextEdit* editor_ = nullptr;
    QCheckBox* followBox_ = nullptr;
    QSlider* zoom_ = nullptr;
    QLabel* status_ = nullptr;
    QList<QTextEdit::ExtraSelection> spellSels_, diffSels_;
    int curBand_ = -1;
    // block workflow state
    QStringList pages_;
    int pageIx_ = -1;
    QString workDir_;
    QPushButton* prevPageB_ = nullptr;
    QPushButton* nextPageB_ = nullptr;
    QLabel* pageLbl_ = nullptr;
#ifdef ALL_HAVE_OCR
    std::unique_ptr<allocr::LineDetector> det_;
    std::unique_ptr<allocr::TextRecognizer> rec_;
    std::vector<allocr::OcrLine> bands_;
#endif
};

#ifdef ALL_HAVE_OCR
// ---- Scan pane: OCR stage 2 in the app (OCR_DESIGN.md) --------------------
// The proven allocr pipeline end to end: line detection (PhotiLines) →
// line building → CTC recognition (Woodblock) → OUR unicode chain +
// syllable-legality QC. Everything ocr-derived is labeled and review-
// gated; output feeds library/ocr_out/ (the existing review flow).
// ---- #62 woodblock illustration candidates (logic layer; UI next
// increment): regions of a folio NOT covered by detected text lines,
// big enough to be art. Pure geometry — testable without models.
// Returns candidate rects: side panels beyond the text envelope and
// vertical gaps taller than 2x the median line height. CANDIDATES
// only, labeled so wherever shown; never claimed complete.
static std::vector<QRect> illustrationCandidates(
    int w, int h, const std::vector<QRect>& lines) {
    std::vector<QRect> out;
    if (w <= 0 || h <= 0 || lines.empty()) return out;
    int minX = w, maxX = 0, minY = h, maxY = 0;
    std::vector<int> hs;
    for (const auto& r : lines) {
        minX = std::min(minX, r.left());
        maxX = std::max(maxX, r.right());
        minY = std::min(minY, r.top());
        maxY = std::max(maxY, r.bottom());
        hs.push_back(r.height());
    }
    std::sort(hs.begin(), hs.end());
    const int med = hs[hs.size() / 2];
    // side panels (where miniatures live on first/last folios)
    if (minX > w / 8)
        out.push_back(QRect(0, minY, minX, maxY - minY));
    if (w - maxX > w / 8)
        out.push_back(QRect(maxX, minY, w - maxX, maxY - minY));
    // vertical gaps between consecutive lines
    std::vector<QRect> sorted = lines;
    std::sort(sorted.begin(), sorted.end(),
              [](const QRect& a, const QRect& b) {
                  return a.top() < b.top();
              });
    for (size_t i = 1; i < sorted.size(); ++i) {
        const int gap = sorted[i].top() - sorted[i - 1].bottom();
        if (gap > 2 * med)
            out.push_back(QRect(minX, sorted[i - 1].bottom(),
                                maxX - minX, gap));
    }
    return out;
}

// shared gallery runner (#62): detect lines on each image, compute
// candidate regions, show crops. Used by the Scan pane (any folder)
// and the Overlay (the linked volume's cached scans).
static void runIllustrationGallery(QWidget* parent, const QString& root,
                                   const QStringList& paths,
                                   const QString& title) {
    const QString lm =
        root + "/library/ocr_models/BDRC_PhotiLines/PhotiLines.onnx";
    if (!QFile::exists(lm)) {
        QMessageBox::information(
            parent, "Model missing",
            "The line-detection model is not installed (see the Scan "
            "pane's model note).");
        return;
    }
    std::unique_ptr<allocr::LineDetector> det;
    try {
        det = std::make_unique<allocr::LineDetector>(lm.toStdString());
    } catch (const std::exception& e) {
        QMessageBox::warning(parent, "Model failed", e.what());
        return;
    }
    QProgressDialog prog("Scanning pages for illustration "
                         "candidates\u2026",
                         "Stop", 0, paths.size(), parent);
    prog.setWindowModality(Qt::WindowModal);
    struct Crop { QImage img; QString from; };
    std::vector<Crop> crops;
    int done = 0;
    for (const QString& path : paths) {
        prog.setValue(done++);
        QCoreApplication::processEvents();
        if (prog.wasCanceled()) break;
        QImage img(path);
        if (img.isNull()) continue;
        img = img.convertToFormat(QImage::Format_RGB888);
        const int w = img.width(), h = img.height();
        std::vector<uint8_t> rgb(size_t(w) * h * 3);
        for (int y = 0; y < h; ++y)
            std::copy(img.constScanLine(y), img.constScanLine(y) + w * 3,
                      rgb.begin() + size_t(y) * w * 3);
        try {
            auto mask = det->detect(rgb.data(), w, h);
            const double zero = 0.0;
            auto pl = allocr::buildLines(rgb.data(), w, h, mask, 2.5,
                                         4.0, true, &zero);
            std::vector<QRect> lrects;
            for (const auto& l : pl.lines)
                lrects.push_back(QRect(l.x, l.y, l.w, l.h));
            for (const auto& r : illustrationCandidates(w, h, lrects))
                crops.push_back({img.copy(r), QFileInfo(path).fileName()});
        } catch (const std::exception&) {
            continue;
        }
    }
    prog.setValue(paths.size());
    if (crops.empty()) {
        QMessageBox::information(
            parent, "Illustration gallery",
            "No candidates found (candidates only \u2014 a miniature "
            "the geometry cannot see is not disproven).");
        return;
    }
    QDialog dlg(parent);
    dlg.setWindowTitle(QString("%1 \u2014 %2 region(s), machine-found, "
                               "review material")
                           .arg(title)
                           .arg(crops.size()));
    dlg.resize(900, 600);
    auto* v = new QVBoxLayout(&dlg);
    auto* list = new QListWidget;
    list->setViewMode(QListView::IconMode);
    list->setIconSize(QSize(220, 160));
    list->setResizeMode(QListView::Adjust);
    for (const auto& c : crops) {
        auto* it = new QListWidgetItem(
            QIcon(QPixmap::fromImage(c.img).scaled(
                220, 160, Qt::KeepAspectRatio,
                Qt::SmoothTransformation)),
            c.from);
        list->addItem(it);
    }
    v->addWidget(list, 1);
    auto* rowb = new QHBoxLayout;
    auto* saveB = new QPushButton("Save crops\u2026");
    auto* closeB = new QPushButton("Close");
    rowb->addStretch();
    rowb->addWidget(saveB);
    rowb->addWidget(closeB);
    v->addLayout(rowb);
    QObject::connect(closeB, &QPushButton::clicked, &dlg,
                     &QDialog::accept);
    QObject::connect(
        list, &QListWidget::itemDoubleClicked,
        [&](QListWidgetItem* it) {
            const int ix = list->row(it);
            QDialog full(&dlg);
            full.setWindowTitle(crops[ix].from + " \u2014 candidate crop");
            auto* fv = new QVBoxLayout(&full);
            auto* lbl = new QLabel;
            lbl->setPixmap(QPixmap::fromImage(crops[ix].img.scaled(
                1000, 700, Qt::KeepAspectRatio,
                Qt::SmoothTransformation)));
            fv->addWidget(lbl);
            full.exec();
        });
    QObject::connect(saveB, &QPushButton::clicked, [&] {
        const QString out = QFileDialog::getExistingDirectory(
            &dlg, "Save crops into folder");
        if (out.isEmpty()) return;
        int n = 0;
        for (const auto& c : crops)
            c.img.save(QString("%1/%2-illus%3.png")
                           .arg(out, QFileInfo(c.from).completeBaseName())
                           .arg(++n));
        QMessageBox::information(
            &dlg, "Saved",
            QString("%1 crop(s) saved \u2014 named by source page.")
                .arg(n));
    });
    dlg.exec();
}

class ScanPane : public QWidget {
public:
    ScanPane(allcore::SyllableChecker* checker, const QString& root)
        : checker_(checker), root_(root) {
        auto* outer = new QVBoxLayout(this);
        auto* banner = new QLabel(
            "<b>Scan → text (OCR)</b> — models by the Buddhist Digital "
            "Resource Center (CC BY-NC 4.0, used with BDRC's permission). "
            "<span style='color:#B4540A'>All output is OCR-DERIVED review "
            "material: verify before any other use; it is never "
            "corpus-bound.</span>");
        banner->setWordWrap(true);
        outer->addWidget(banner);
        auto* row = new QHBoxLayout;
        auto* open = new QPushButton("Open scan image…");
        row->addWidget(open);
        deskewOverride_ = new QCheckBox(
            "override deskew to 0° (DEVIATION from the BDRC pipeline — "
            "workaround for its angle bug on straight pages)");
        row->addWidget(deskewOverride_);
        run_ = new QPushButton("Run OCR");
        run_->setEnabled(false);
        row->addWidget(run_);
        illusToggle_ = new QCheckBox("mark illustration candidates");
        illusToggle_->setToolTip(
            "Outline regions of the folio NOT covered by detected "
            "text lines \u2014 side panels and large gaps, where "
            "woodblock miniatures and diagrams live. CANDIDATES "
            "only, machine-found from the line geometry; never "
            "claimed complete.");
        row->addWidget(illusToggle_);
        auto* galleryBtn = new QPushButton("Illustration gallery\u2026");
        galleryBtn->setToolTip(
            "Scan a whole folder of page images for illustration "
            "candidates (regions not covered by detected text lines) "
            "and show them as a gallery of crops with their page "
            "provenance. Candidates only \u2014 never claimed "
            "complete.");
        row->addWidget(galleryBtn);
        connect(galleryBtn, &QPushButton::clicked,
                [this] { illusGallery(); });
        connect(illusToggle_, &QCheckBox::toggled, [this] {
            if (!pl_.lines.empty()) drawPage(-1, -1);
        });
        save_ = new QPushButton("Save to ocr_out…");
        save_->setEnabled(false);
        row->addWidget(save_);
        auto* batchBtn = new QPushButton("Batch folder…");
        batchBtn->setToolTip(
            "OCR every page image in a folder (a scanned volume) — one "
            "-ocr.txt per page into library/ocr_out/<folder>/, all "
            "headered OCR-DERIVED. The Than Grove batch-volume pattern "
            "from the ACIP Development survey.");
        row->addWidget(batchBtn);
        row->addStretch();
        outer->addLayout(row);
        connect(batchBtn, &QPushButton::clicked, [this] { batchFolder(); });
        auto* split = new QSplitter(Qt::Vertical);
        auto* scroll = new QScrollArea;
        pageView_ = new QLabel("open a scan image (PNG/JPG/TIFF)");
        pageView_->setAlignment(Qt::AlignCenter);
        scroll->setWidget(pageView_);
        scroll->setWidgetResizable(true);
        split->addWidget(scroll);
        scroll_ = scroll;
        results_ = new QTextBrowser;
        results_->setOpenLinks(false);
        connect(results_, &QTextBrowser::anchorClicked, [this](const QUrl& u) {
            const QStringList p = u.toString().split(':');
            if (p.size() != 3 || p[0] != "w") return;
            const int li = p[1].toInt(), wi = p[2].toInt();
            if (li < 0 || li >= int(words_.size()) || wi < 0 ||
                wi >= int(words_[li].size()))
                return;
            drawPage(li, wi);
            const auto& l = pl_.lines[li];
            scroll_->ensureVisible(pageX(size_t(li), words_[li][wi].x0),
                                   l.y + l.h / 2, 120, 120);
        });
        split->addWidget(results_);
        split->setStretchFactor(0, 1);
        split->setStretchFactor(1, 1);
        outer->addWidget(split, 1);

        connect(open, &QPushButton::clicked, [this] { openImage(); });
        connect(run_, &QPushButton::clicked, [this] { runOcr(); });
        connect(save_, &QPushButton::clicked, [this] { saveOut(); });
    }

private:
    void openImage() {
        const QString f = QFileDialog::getOpenFileName(
            this, "Open scan image", root_ + "/library",
            "Images (*.png *.jpg *.jpeg *.tif *.tiff)");
        if (f.isEmpty()) return;
        QImage img(f);
        if (img.isNull()) {
            results_->setHtml("<i>cannot read that image</i>");
            return;
        }
        img_ = img.convertToFormat(QImage::Format_RGB888);
        file_ = f;
        pageView_->setPixmap(QPixmap::fromImage(img_));
        pageView_->adjustSize();
        run_->setEnabled(true);
        save_->setEnabled(false);
        results_->setHtml("<i>ready — Run OCR</i>");
    }

    bool ensureModels() {
        if (det_ && rec_) return true;
        const QString lineModel =
            root_ + "/library/ocr_models/BDRC_PhotiLines/PhotiLines.onnx";
        const QString ocrDir = root_ + "/library/ocr_models/BDRC_Woodblock";
        if (!QFile::exists(lineModel) ||
            !QFile::exists(ocrDir + "/OCRModel.onnx")) {
            results_->setHtml(
                "<b>models missing.</b> Download from Hugging Face "
                "(huggingface.co/BDRC — CC BY-NC 4.0):<br>"
                "· PhotiLines.onnx + config.json → "
                "library/ocr_models/BDRC_PhotiLines/<br>"
                "· OCRModel.onnx + model_config.json → "
                "library/ocr_models/BDRC_Woodblock/");
            return false;
        }
        try {
            det_ = std::make_unique<allocr::LineDetector>(
                lineModel.toStdString());
            rec_ = std::make_unique<allocr::TextRecognizer>(
                ocrDir.toStdString());
        } catch (const std::exception& e) {
            results_->setHtml(QString("<b>model load failed:</b> %1")
                                  .arg(QString::fromUtf8(e.what())
                                           .toHtmlEscaped()));
            return false;
        }
        return true;
    }

    // map a word's strip-x to deskewed-page x through the line's colMap
    // (mask_n_crop squeezes interior gaps — the map is the exact inverse)
    int pageX(size_t line, double stripX) const {
        const auto& cm = pl_.images[line].colMap;
        if (cm.empty()) return pl_.lines[line].x + int(stripX);
        const int ix = std::min(std::max(int(std::lround(stripX)), 0),
                                int(cm.size()) - 1);
        return cm[ix];
    }

    // deskewed page with line boxes + word boxes; optional highlight
    void drawPage(int hlLine, int hlWord) {
        if (pl_.deskewedRgb.empty()) return;
        QImage page(pl_.deskewedRgb.data(), pl_.width, pl_.height,
                    pl_.width * 3, QImage::Format_RGB888);
        QImage annotated = page.copy();
        {
            QPainter p(&annotated);
            p.setPen(QPen(QColor(0x7F, 0x77, 0xDD), 2));
            for (const auto& l : pl_.lines)
                p.drawRect(l.x, l.y, l.w, l.h);
            for (size_t i = 0; i < words_.size(); ++i) {
                const auto& l = pl_.lines[i];
                for (size_t k = 0; k < words_[i].size(); ++k) {
                    const auto& ws = words_[i][k];
                    const int x0 = pageX(i, ws.x0), x1 = pageX(i, ws.x1);
                    if (int(i) == hlLine && int(k) == hlWord) {
                        p.fillRect(QRect(x0, l.y, std::max(x1 - x0, 2),
                                         l.h),
                                   QColor(0xE8, 0x9C, 0x30, 110));
                        p.setPen(QPen(QColor(0xB4, 0x54, 0x0A), 2));
                        p.drawRect(x0, l.y, std::max(x1 - x0, 2), l.h);
                        p.setPen(QPen(QColor(0x7F, 0x77, 0xDD), 2));
                    } else {
                        p.setPen(QPen(QColor(0x62, 0xA8, 0x62, 150), 1));
                        p.drawRect(x0, l.y + l.h - 6,
                                   std::max(x1 - x0, 2), 5);
                        p.setPen(QPen(QColor(0x7F, 0x77, 0xDD), 2));
                    }
                }
            }
        }
        if (illusToggle_ && illusToggle_->isChecked()) {
            std::vector<QRect> lrects;
            lrects.reserve(pl_.lines.size());
            for (const auto& l : pl_.lines)
                lrects.push_back(QRect(l.x, l.y, l.w, l.h));
            const auto cands = illustrationCandidates(
                pl_.width, pl_.height, lrects);
            QPainter p(&annotated);
            p.setPen(QPen(QColor(0xB8, 0x86, 0x1B), 4));
            QFont f = p.font();
            f.setPointSize(18);
            f.setBold(true);
            p.setFont(f);
            for (const auto& r : cands) {
                p.drawRect(r);
                p.drawText(r.adjusted(8, 8, 0, 0).topLeft() +
                               QPoint(0, 24),
                           "image? (candidate)");
            }
        }
        pageView_->setPixmap(QPixmap::fromImage(annotated));
        pageView_->adjustSize();
    }

    void runOcr() {
        if (img_.isNull() || !ensureModels()) return;
        results_->setHtml("<i>detecting lines…</i>");
        QCoreApplication::processEvents();
        // QImage rows are 4-byte aligned — repack tightly
        const int w = img_.width(), h = img_.height();
        std::vector<uint8_t> rgb(static_cast<size_t>(w) * h * 3);
        for (int y = 0; y < h; ++y)
            std::copy(img_.constScanLine(y), img_.constScanLine(y) + w * 3,
                      rgb.begin() + static_cast<size_t>(y) * w * 3);
        auto mask = det_->detect(rgb.data(), w, h);
        const double zero = 0.0;
        pl_ = allocr::buildLines(
            rgb.data(), w, h, mask, 2.5, 4.0, true,
            deskewOverride_->isChecked() ? &zero : nullptr);
        auto& pl = pl_;
        words_.assign(pl.images.size(), {});
        drawPage(-1, -1);

        // recognize each line; convert via OUR chain; syllable QC
        lastWylie_.clear();
        QString htmlOut =
            QString("<div style='color:#555;font-size:12px'>deskew %1° "
                    "%2 · %3 line(s) · every line OCR-DERIVED (review "
                    "before use) · click a word to see it on the page "
                    "(word boxes: pyctcdecode frame spans)</div>")
                .arg(pl.angle, 0, 'f', 2)
                .arg(deskewOverride_->isChecked()
                         ? "<b>(override — deviation)</b>"
                         : "(BDRC pipeline)")
                .arg(pl.lines.size());
        int flagged = 0;
        for (size_t i = 0; i < pl.images.size(); ++i) {
            results_->setHtml(htmlOut +
                              QString("<i>recognizing line %1/%2…</i>")
                                  .arg(i + 1)
                                  .arg(pl.images.size()));
            QCoreApplication::processEvents();
            const std::string wylie =
                rec_->recognize(pl.images[i], true, &words_[i]);
            lastWylie_.push_back(wylie);
            auto [uni, ok] = allcore::wylieToUnicode(wylie);
            int bad = 0, toks = 0;
            std::stringstream ss(wylie);
            std::string tok;
            while (ss >> tok) {
                bool alpha = !tok.empty();
                for (char c2 : tok)
                    alpha &= (std::isalpha(static_cast<unsigned char>(c2)) ||
                              c2 == '\'' || c2 == '+');
                if (!alpha) continue;
                ++toks;
                if (checker_ && !checker_->legalWylie(tok)) ++bad;
            }
            flagged += bad;
            // wylie rendered as clickable per-word anchors (follow-along:
            // clicking highlights the word's frame-span box on the page)
            QString wylieHtml;
            if (!words_[i].empty()) {
                for (size_t k = 0; k < words_[i].size(); ++k)
                    wylieHtml +=
                        QString("<a href='w:%1:%2' style='text-decoration:"
                                "none;color:#1E4E6B'>%3</a>")
                            .arg(i)
                            .arg(k)
                            .arg(QString::fromStdString(words_[i][k].text)
                                     .toHtmlEscaped());
            } else {
                wylieHtml = QString::fromStdString(wylie).toHtmlEscaped();
            }
            htmlOut += QString(
                           "<div style='margin:6px 0'><b>%1.</b> "
                           "<span style='font-family:monospace'>%2</span>"
                           "<br><span style='font-size:20px'>%3</span>"
                           "<small style='color:%4'> %5/%6 syllable(s) "
                           "fail legality</small></div>")
                           .arg(i + 1)
                           .arg(wylieHtml)
                           .arg(QString::fromStdString(uni).toHtmlEscaped())
                           .arg(bad ? "#B4540A" : "#3B7A3B")
                           .arg(bad)
                           .arg(toks);
            (void)ok;
        }
        htmlOut += QString("<div style='color:#555;font-size:12px'>total "
                           "legality flags: %1 — OCR output is review "
                           "material (models: BDRC, CC BY-NC 4.0)</div>")
                       .arg(flagged);
        results_->setHtml(htmlOut);
        save_->setEnabled(!lastWylie_.empty());
    }

    // Batch-volume OCR (Than Grove's OCRProcessing pattern): every page
    // image in a folder → one headered -ocr.txt per page under
    // library/ocr_out/<folder>/. Same canonical pipeline as single-page,
    // same review-material rule on every output.
    void batchFolder() {
        if (!ensureModels()) return;
        const QString dir = QFileDialog::getExistingDirectory(
            this, "Folder of page scans", root_ + "/library");
        if (dir.isEmpty()) return;
        QStringList imgs;
        for (const QString& e : QDir(dir).entryList(
                 {"*.png", "*.jpg", "*.jpeg", "*.tif", "*.tiff"},
                 QDir::Files, QDir::Name))
            imgs << dir + "/" + e;
        if (imgs.isEmpty()) {
            results_->setHtml("<i>no page images in that folder</i>");
            return;
        }
        const QString outDir =
            root_ + "/library/ocr_out/" + QFileInfo(dir).fileName();
        QDir().mkpath(outDir);
        QProgressDialog prog(
            QString("Batch OCR — %1 page(s)").arg(imgs.size()), "Stop", 0,
            imgs.size(), this);
        prog.setWindowModality(Qt::WindowModal);
        prog.setMinimumDuration(0);
        int done = 0, pagesOk = 0, failed = 0;
        long lineCount = 0, flagCount = 0;
        QString detail;
        for (const QString& f : imgs) {
            prog.setValue(done++);
            prog.setLabelText(QFileInfo(f).fileName());
            QCoreApplication::processEvents();
            if (prog.wasCanceled()) break;
            QImage img(f);
            if (img.isNull()) {
                ++failed;
                detail += "<div style='color:#B00020'>unreadable: " +
                          QFileInfo(f).fileName().toHtmlEscaped() + "</div>";
                continue;
            }
            img = img.convertToFormat(QImage::Format_RGB888);
            const int w = img.width(), h = img.height();
            std::vector<uint8_t> rgb(static_cast<size_t>(w) * h * 3);
            for (int y = 0; y < h; ++y)
                std::copy(img.constScanLine(y),
                          img.constScanLine(y) + w * 3,
                          rgb.begin() + static_cast<size_t>(y) * w * 3);
            try {
                auto mask = det_->detect(rgb.data(), w, h);
                const double zero = 0.0;
                auto pl = allocr::buildLines(
                    rgb.data(), w, h, mask, 2.5, 4.0, true,
                    deskewOverride_->isChecked() ? &zero : nullptr);
                int pageFlags = 0;
                QStringList wylieLines;
                for (const auto& li : pl.images) {
                    const std::string wylie = rec_->recognize(li);
                    wylieLines << QString::fromStdString(wylie);
                    std::stringstream ss(wylie);
                    std::string tok;
                    while (ss >> tok) {
                        bool alpha = !tok.empty();
                        for (char c2 : tok)
                            alpha &= (std::isalpha(
                                          static_cast<unsigned char>(c2)) ||
                                      c2 == '\'' || c2 == '+');
                        if (alpha && checker_ && !checker_->legalWylie(tok))
                            ++pageFlags;
                    }
                }
                const QString out = outDir + "/" +
                                    QFileInfo(f).completeBaseName() +
                                    "-ocr.txt";
                QFile of(out);
                if (of.open(QIODevice::WriteOnly | QIODevice::Text)) {
                    QTextStream ts(&of);
                    ts << "# OCR-DERIVED (unverified review material) — "
                          "source: "
                       << QFileInfo(f).fileName()
                       << " — models: BDRC (CC BY-NC 4.0, with "
                          "permission)\n";
                    for (const QString& w2 : wylieLines) ts << w2 << "\n";
                }
                ++pagesOk;
                lineCount += static_cast<long>(wylieLines.size());
                flagCount += pageFlags;
                detail += QString("<div>%1 — %2 line(s)%3</div>")
                              .arg(QFileInfo(f).fileName().toHtmlEscaped())
                              .arg(wylieLines.size())
                              .arg(pageFlags
                                       ? QString(" · <span style='color:"
                                                 "#B4540A'>%1 legality "
                                                 "flag(s)</span>")
                                             .arg(pageFlags)
                                       : QString());
            } catch (const std::exception& e) {
                ++failed;
                detail += "<div style='color:#B00020'>failed: " +
                          QFileInfo(f).fileName().toHtmlEscaped() + " — " +
                          QString::fromUtf8(e.what()).toHtmlEscaped() +
                          "</div>";
            }
        }
        prog.setValue(imgs.size());
        results_->setHtml(
            QString("<div><b>Batch OCR</b> — %1/%2 page(s) written to "
                    "%3 · %4 line(s) · %5 syllable legality flag(s) · "
                    "%6 failure(s)%7</div>"
                    "<div style='color:#B4540A'>every file is OCR-DERIVED "
                    "review material (models: BDRC, CC BY-NC 4.0)</div>"
                    "<hr>")
                .arg(pagesOk)
                .arg(imgs.size())
                .arg(outDir.toHtmlEscaped())
                .arg(lineCount)
                .arg(flagCount)
                .arg(failed)
                .arg(done < imgs.size() ? " · <b>stopped early</b>"
                                        : QString()) +
            detail);
    }

    void saveOut() {
        if (lastWylie_.empty()) return;
        QDir().mkpath(root_ + "/library/ocr_out");
        const QString base = QFileInfo(file_).completeBaseName();
        const QString out = QFileDialog::getSaveFileName(
            this, "Save OCR text",
            root_ + "/library/ocr_out/" + base + "-ocr.txt",
            "Text (*.txt)");
        if (out.isEmpty()) return;
        QFile f(out);
        if (!f.open(QIODevice::WriteOnly | QIODevice::Text)) return;
        QTextStream ts(&f);
        ts << "# OCR-DERIVED (unverified review material) — source: "
           << QFileInfo(file_).fileName()
           << " — models: BDRC (CC BY-NC 4.0, with permission)\n";
        for (const auto& w2 : lastWylie_)
            ts << QString::fromStdString(w2) << "\n";
        results_->setHtml(results_->toHtml() +
                          "<div style='color:#3B7A3B'>saved to " +
                          out.toHtmlEscaped() +
                          " — opening it in the Overlay runs the ocr-derived "
                          "banner + first-pass QC</div>");
    }

    allcore::SyllableChecker* checker_ = nullptr;
    QString root_, file_;
    QImage img_;
    std::unique_ptr<allocr::LineDetector> det_;
    std::unique_ptr<allocr::TextRecognizer> rec_;
    QLabel* pageView_ = nullptr;
    QScrollArea* scroll_ = nullptr;
    QTextBrowser* results_ = nullptr;
    QPushButton* run_ = nullptr;
    QPushButton* save_ = nullptr;
    void illusGallery() {
        const QString dir = QFileDialog::getExistingDirectory(
            this, "Folder of page scans");
        if (dir.isEmpty()) return;
        QStringList files =
            QDir(dir).entryList({"*.png", "*.jpg", "*.jpeg", "*.tif",
                                 "*.tiff", "*.bmp"},
                                QDir::Files, QDir::Name);
        if (files.isEmpty()) {
            QMessageBox::information(this, "No images",
                                     "No page images in that folder.");
            return;
        }
        QStringList paths;
        for (const QString& f : files) paths << dir + "/" + f;
        runIllustrationGallery(this, root_, paths,
                               "Illustration candidates");
    }

    QCheckBox* deskewOverride_ = nullptr;
    QCheckBox* illusToggle_ = nullptr;
    std::vector<std::string> lastWylie_;
    allocr::PageLines pl_;                            // last processed page
    std::vector<std::vector<allocr::WordSpan>> words_;  // per line
};
#endif  // ALL_HAVE_OCR

static bool isDataRoot(const QString& c) {
    return QFileInfo::exists(c + "/build/hgm_spine_v27_2.db");
}

// Locate the data folder. Development: the repo checkout (walk up from
// the build dir). Distribution: an "ALL Tool Data" folder beside the
// .app (the DMG layout), a previously chosen folder remembered in
// QSettings, or Application Support. Last resort: ask once, remember.
static QString findDataRoot() {
    QSettings s("ALL", "TranslationTool");
    const QString saved = s.value("app/dataRoot").toString();
    if (!saved.isEmpty() && isDataRoot(saved)) return saved;

    QStringList cands = {QDir::currentPath()};
    QDir d(QCoreApplication::applicationDirPath());
    for (int i = 0; i < 7; ++i) {
        cands << d.absolutePath();
        cands << d.absolutePath() + "/ALL Tool Data";   // beside the .app
        if (!d.cdUp()) break;
    }
    cands << QStandardPaths::writableLocation(
                 QStandardPaths::AppDataLocation);      // Application Support
    for (const auto& c : cands)
        if (isDataRoot(c)) return c;

    // not found — ask once and remember (a distributed copy whose data
    // folder was moved). Refusing leaves the app to report the missing
    // spine exactly as before; nothing is guessed.
    QMessageBox::information(
        nullptr, "Locate the data folder",
        "The ALL Tool Data folder (containing build/hgm_spine_…db) was "
        "not found next to the application. Please choose it.");
    const QString picked = QFileDialog::getExistingDirectory(
        nullptr, "Choose the ALL Tool Data folder");
    if (!picked.isEmpty() && isDataRoot(picked)) {
        s.setValue("app/dataRoot", picked);
        return picked;
    }
    return QDir::currentPath();
}

// ---- The in-house proposal & approval channel (Adam's design) ------
// Translators PROPOSE terms and notes; the authority (Geshe Michael,
// or Adam as second admin) APPROVES. Identity is provenance, not a
// login: a name + role set once in QSettings; the store is a folder
// of TSV files that a shared Dropbox syncs. Register approvals apply
// in-app; dictionary/corpus approvals export as a candidates package
// for the data project (the app never mutates the corpus).

// set after the pane groups are built; raises any pane through the
// two-level navigation
static std::function<void(QWidget*)> g_raisePane;

static QString g_userName;
static bool g_isAdmin = false;
static QString g_proposalsDir;
static bool g_identityPinned = false;   // screenshot mode: keep seeds

static QString isoToday() {
    return QDateTime::currentDateTime().toString("yyyy-MM-dd");
}

// establish identity once (or when changed via the Propose pane)
static void loadIdentity() {
    if (g_identityPinned) return;
    QSettings s("ALL", "TranslationTool");
    g_userName = s.value("team/name").toString();
    g_isAdmin = s.value("team/admin", false).toBool();
    g_proposalsDir = s.value("team/proposalsDir").toString();
}

// a small helper any card can call to file a proposal with evidence
static void fileProposal(QWidget* parent, allcore::ProposalKind kind,
                         const QString& wylie, const QString& value,
                         const QString& field, const QString& evidence) {
    if (g_proposalsDir.isEmpty() || g_userName.isEmpty()) {
        QMessageBox::information(
            parent, "Set up proposals",
            "First set your name and the shared proposals folder in the "
            "Propose tab (one-time setup).");
        return;
    }
    allcore::ProposalStore store(g_proposalsDir.toStdString());
    store.load();
    store.propose(kind, g_userName.toStdString(), wylie.toStdString(),
                  value.toStdString(), field.toStdString(),
                  evidence.toStdString(), isoToday().toStdString());
    if (store.save())
        QMessageBox::information(
            parent, "Proposed",
            "Filed for review — it will appear in the authority's "
            "Approval list. Thank you.");
    else
        QMessageBox::warning(parent, "Could not save",
                             "The proposals folder could not be written.");
}

static void showDasPage(QWidget* parent, int page) {
#ifdef ALL_HAVE_QTPDF
    if (g_dasPdfPath.isEmpty() || !QFile::exists(g_dasPdfPath)) {
        QMessageBox::information(
            parent, "Das dictionary",
            "The Das (1902) PDF is not in the data folder "
            "(data/das/das_1902_bookmarked.pdf).");
        return;
    }
    auto* dlg = new QDialog(parent);
    dlg->setAttribute(Qt::WA_DeleteOnClose);
    dlg->setWindowTitle(
        QString("Das, A Tibetan-English Dictionary (1902) \u2014 "
                "p.%1 \u00b7 public domain \u00b7 reference")
            .arg(page));
    dlg->resize(820, 900);
    auto* v = new QVBoxLayout(dlg);
    auto* doc = new QPdfDocument(dlg);
    doc->load(g_dasPdfPath);
    auto* view = new QPdfView(dlg);
    view->setDocument(doc);
    view->setPageMode(QPdfView::PageMode::MultiPage);
    view->setZoomMode(QPdfView::ZoomMode::FitToWidth);
    v->addWidget(view, 1);
    auto* row = new QHBoxLayout;
    auto* back = new QPushButton("\u25C0 page");
    auto* fwd = new QPushButton("page \u25B6");
    auto* closeB = new QPushButton("Close");
    row->addWidget(back);
    row->addWidget(fwd);
    row->addStretch();
    row->addWidget(closeB);
    v->addLayout(row);
    auto* nav = view->pageNavigator();
    QObject::connect(back, &QPushButton::clicked, [nav] {
        if (nav->currentPage() > 0)
            nav->jump(nav->currentPage() - 1, {});
    });
    QObject::connect(fwd, &QPushButton::clicked, [nav, doc] {
        if (nav->currentPage() + 1 < doc->pageCount())
            nav->jump(nav->currentPage() + 1, {});
    });
    QObject::connect(closeB, &QPushButton::clicked, dlg,
                     &QDialog::accept);
    dlg->show();
    QTimer::singleShot(300, dlg, [nav, page, doc] {
        if (page - 1 < doc->pageCount()) nav->jump(page - 1, {});
    });
#else
    QMessageBox::information(parent, "Das dictionary",
                             "This build lacks Qt PDF support.");
    (void)page;
#endif
}

static int fileSpellingProposals(QWidget* parent,
                                 const QList<QStringList>& rows) {
    if (g_proposalsDir.isEmpty() || g_userName.isEmpty()) {
        QMessageBox::information(
            parent, "Set up proposals",
            "First set your name and the shared proposals folder in the "
            "Propose tab or in Settings (one-time setup).");
        return -1;
    }
    allcore::ProposalStore store(g_proposalsDir.toStdString());
    store.load();
    for (const auto& r : rows)
        store.propose(allcore::ProposalKind::Spelling,
                      g_userName.toStdString(), r[0].toStdString(),
                      r[1].toStdString(), r[2].toStdString(),
                      r[3].toStdString(), isoToday().toStdString());
    if (!store.save()) {
        QMessageBox::warning(parent, "Could not save",
                             "The proposals folder could not be written.");
        return -1;
    }
    QMessageBox::information(
        parent, "Filed",
        QString("%1 spelling flag(s) filed for the authority's ruling "
                "\u2014 they will appear in the Approval queue.")
            .arg(rows.size()));
    return (int)rows.size();
}

class ProposePane : public QWidget {
public:
    ProposePane() {
        auto* outer = new QVBoxLayout(this);
        outer->addWidget(new QLabel(
            "<b>Propose</b> — offer a term, marking, pronunciation, or "
            "note for the authority to approve. Your name is recorded "
            "for provenance; nothing is a login."));

        // identity setup
        auto* idBox = new QGroupBox("Who you are (one-time)");
        auto* idl = new QFormLayout(idBox);
        name_ = new QLineEdit(g_userName);
        idl->addRow("Your name", name_);
        admin_ = new QCheckBox("I am an authority (can approve) — "
                               "Geshe Michael / Adam only");
        admin_->setChecked(g_isAdmin);
        idl->addRow("", admin_);
        auto* dirRow = new QHBoxLayout;
        dir_ = new QLineEdit(g_proposalsDir);
        dir_->setPlaceholderText(
            "shared proposals folder (e.g. a Dropbox path)…");
        auto* pick = new QPushButton("Choose…");
        dirRow->addWidget(dir_, 1);
        dirRow->addWidget(pick);
        auto* dirW = new QWidget;
        dirW->setLayout(dirRow);
        idl->addRow("Proposals folder", dirW);
        auto* saveId = new QPushButton("Save identity");
        idl->addRow("", saveId);
        outer->addWidget(idBox);
        connect(pick, &QPushButton::clicked, [this] {
            const QString d = QFileDialog::getExistingDirectory(
                this, "Shared proposals folder (Dropbox-synced is ideal)");
            if (!d.isEmpty()) dir_->setText(d);
        });
        connect(saveId, &QPushButton::clicked, [this] {
            QSettings s("ALL", "TranslationTool");
            s.setValue("team/name", name_->text().trimmed());
            s.setValue("team/admin", admin_->isChecked());
            s.setValue("team/proposalsDir", dir_->text().trimmed());
            loadIdentity();
            QMessageBox::information(
                this, "Saved",
                admin_->isChecked()
                    ? "Identity saved. The Approval tab is now available "
                      "(restart to show it)."
                    : "Identity saved.");
        });

        // the proposal form
        auto* box = new QGroupBox("Make a proposal");
        auto* fl = new QFormLayout(box);
        kind_ = new QComboBox;
        kind_->addItem("Honorific term (↔ ordinary)",
                       int(allcore::ProposalKind::Honorific));
        kind_->addItem("HIGH honorific marking",
                       int(allcore::ProposalKind::HighHonorific));
        kind_->addItem("Humilific (self-lowering) form",
                       int(allcore::ProposalKind::Humilific));
        kind_->addItem("DOUBLE honorific (subject + recipient)",
                       int(allcore::ProposalKind::DoubleHonorific));
        kind_->addItem("Pronunciation exception",
                       int(allcore::ProposalKind::Pronunciation));
        kind_->addItem("Abbreviation / contraction candidate",
                       int(allcore::ProposalKind::Abbreviation));
        kind_->addItem("Rendering for a word (→ dictionary)",
                       int(allcore::ProposalKind::WordRendering));
        kind_->addItem("Rendering for a phrase/clause (→ dictionary)",
                       int(allcore::ProposalKind::PhraseRendering));
        kind_->addItem("Idiom / fixed expression",
                       int(allcore::ProposalKind::Idiom));
        kind_->addItem("Note about a passage",
                       int(allcore::ProposalKind::Note));
        fl->addRow("Kind", kind_);
        wylie_ = new QLineEdit;
        wylie_->setPlaceholderText("the Tibetan (ACIP or wylie)");
        fl->addRow("Tibetan", wylie_);
        value_ = new QLineEdit;
        value_->setPlaceholderText(
            "proposed English / pronunciation / expansion");
        fl->addRow("Proposed", value_);
        field_ = new QLineEdit;
        field_->setPlaceholderText(
            "optional: ordinary form, domain, or note");
        fl->addRow("Secondary", field_);
        evidence_ = new QPlainTextEdit;
        evidence_->setPlaceholderText(
            "evidence — where you saw it, the passage, why…");
        evidence_->setMaximumHeight(90);
        fl->addRow("Evidence", evidence_);
        auto* submit = new QPushButton("File proposal for review");
        fl->addRow("", submit);
        outer->addWidget(box);

        // the feedback half of the loop: what became of what I offered
        auto* mineBtn = new QPushButton("My proposals — what happened?");
        mineBtn->setToolTip(
            "Every proposal you have filed, with its current status and "
            "the authority's comment when there is one.");
        outer->addWidget(mineBtn);
        mine_ = new QTextBrowser;
        mine_->setVisible(false);
        outer->addWidget(mine_, 1);
        outer->addStretch();
        connect(mineBtn, &QPushButton::clicked, [this] { showMine(); });
        connect(submit, &QPushButton::clicked, [this] {
            if (wylie_->text().trimmed().isEmpty() &&
                evidence_->toPlainText().trimmed().isEmpty()) {
                QMessageBox::information(this, "Nothing to propose",
                                         "Add the Tibetan or a note.");
                return;
            }
            loadIdentity();
            fileProposal(
                this,
                allcore::ProposalKind(kind_->currentData().toInt()),
                wylie_->text().trimmed(), value_->text().trimmed(),
                field_->text().trimmed(),
                evidence_->toPlainText().trimmed());
            wylie_->clear();
            value_->clear();
            field_->clear();
            evidence_->clear();
        });
    }

    int selfTest(QStringList& log) {
        int fails = 0;
        auto check = [&](bool ok, const char* what) {
            log << QString("  [%1] Propose: %2")
                       .arg(ok ? "PASS" : "FAIL").arg(what);
            if (!ok) ++fails;
        };
        const QString saveDir = g_proposalsDir;
        const QString tmp = QDir::tempPath() + "/all_selftest_mine";
        QDir(tmp).removeRecursively();
        QDir().mkpath(tmp);
        allcore::ProposalStore st(tmp.toStdString());
        st.propose(allcore::ProposalKind::Honorific,
                   g_userName.toStdString(), "zhal", "mouth (hon.)",
                   "kha", "selftest", "2026-08-09");
        st.save();
        g_proposalsDir = tmp;
        showMine();
        const QString m = mine_->toPlainText();
        g_proposalsDir = saveDir;
        check(m.contains("zhal") && m.toUpper().contains("PENDING"),
              "my-proposals lists own item with status");
        return fails;
    }

private:
    void showMine() {
        loadIdentity();
        mine_->setVisible(true);
        if (g_proposalsDir.isEmpty() || g_userName.isEmpty()) {
            mine_->setHtml("<i>set your name and the proposals folder "
                           "above first</i>");
            return;
        }
        allcore::ProposalStore store(g_proposalsDir.toStdString());
        store.load();
        QString h;
        int n = 0;
        // newest first — the store appends in order
        const auto& all = store.all();
        for (auto it = all.rbegin(); it != all.rend(); ++it) {
            const auto& p = *it;
            if (p.proposer != g_userName.toStdString()) continue;
            ++n;
            static const char* colors[] = {"#B4540A", "#1E6B4E",
                                           "#8C2F2B", "#777777"};
            const char* col = colors[int(p.status)];
            h += QString("<div style='margin:6px 0'>"
                         "<span style='color:%1'><b>%2</b></span> · "
                         "%3 <b>%4</b>%5 <small style='color:#999'>"
                         "(%6)</small>%7</div>")
                     .arg(col)
                     .arg(QString(allcore::Proposal::statusName(p.status))
                              .toUpper())
                     .arg(allcore::Proposal::kindName(p.kind))
                     .arg(QString::fromStdString(p.wylie).toHtmlEscaped())
                     .arg(p.value.empty()
                              ? QString()
                              : " → “" +
                                    QString::fromStdString(p.value)
                                        .toHtmlEscaped() + "”")
                     .arg(QString::fromStdString(p.created))
                     .arg(p.ruled.empty()
                              ? QString()
                              : QString("<br><small>ruled by %1, %2%3"
                                        "</small>")
                                    .arg(QString::fromStdString(p.approver)
                                             .toHtmlEscaped())
                                    .arg(QString::fromStdString(p.ruled))
                                    .arg(p.comment.empty()
                                             ? QString()
                                             : ": <i>" +
                                                   QString::fromStdString(
                                                       p.comment)
                                                       .toHtmlEscaped() +
                                                   "</i>"));
        }
        mine_->setHtml(
            n ? QString("<div><b>%1 proposal(s) by %2</b></div><hr>")
                        .arg(n)
                        .arg(g_userName) + h
              : "<i>you have not filed any proposals yet</i>");
    }

    QLineEdit* name_;
    QCheckBox* admin_;
    QLineEdit* dir_;
    QComboBox* kind_;
    QLineEdit* wylie_;
    QLineEdit* value_;
    QLineEdit* field_;
    QPlainTextEdit* evidence_;
    QTextBrowser* mine_ = nullptr;
};

// The authority's pane — visible only in the admin role. Approve
// applies register items in-app (writing the register file, tiered
// "approved — <approver>, <date>") and marks dictionary items for the
// export package. Every ruling stamps provenance.
class ApprovalPane : public QWidget {
public:
    ApprovalPane(const QString& root) : root_(root) {
        auto* outer = new QVBoxLayout(this);
        outer->addWidget(new QLabel(
            "<b>Approval</b> — the authority's queue. Register approvals "
            "(honorific, pronunciation, abbreviation) apply in the app "
            "immediately, tiered as approved with your name and the "
            "date. Dictionary/corpus approvals are collected for the "
            "data project's next release — the app never edits the "
            "corpus itself."));
        auto* row = new QHBoxLayout;
        auto* refresh = new QPushButton("Refresh queue");
        auto* exportB = new QPushButton("Export approved dictionary "
                                        "candidates…");
        auto* archiveB = new QPushButton("Rulings archive");
        archiveB->setToolTip(
            "The read-only record of every decision — who proposed, who "
            "ruled, when, and the comment. Over the years, a record of "
            "the authority's own judgments.");
        filter_ = new QComboBox;
        filter_->addItem("All kinds", "");
        filter_->addItem("Pronunciations", "pronunciation");
        filter_->addItem("Honorifics", "honorific");
        filter_->addItem("HIGH honorifics", "high-honorific");
        filter_->addItem("Humilifics", "humilific");
        filter_->addItem("Double honorifics", "double-honorific");
        filter_->addItem("Spelling flags", "spelling");
        filter_->addItem("Idioms", "idiom");
        filter_->addItem("Words / phrases / notes", "export");
        row->addWidget(filter_);
        row->addWidget(refresh);
        row->addWidget(exportB);
        row->addWidget(archiveB);
        row->addStretch();
        outer->addLayout(row);
        list_ = new QTextBrowser;
        list_->setOpenLinks(false);
        outer->addWidget(list_, 1);
        connect(refresh, &QPushButton::clicked, [this] { rebuild(); });
        connect(filter_, &QComboBox::currentIndexChanged,
                [this] { rebuild(); });
        connect(exportB, &QPushButton::clicked, [this] { exportApproved(); });
        connect(archiveB, &QPushButton::clicked, [this] { showArchive(); });
        connect(list_, &QTextBrowser::anchorClicked,
                [this](const QUrl& u) { onAction(u.toString()); });
        rebuild();
    }

    int selfTest(QStringList& log) {
        int fails = 0;
        auto check = [&](bool ok, const char* what) {
            log << QString("  [%1] Approval: %2")
                       .arg(ok ? "PASS" : "FAIL")
                       .arg(what);
            if (!ok) ++fails;
        };
        rebuild();   // read-only against the seeded proposals store
        const QString t = list_->toPlainText();
        check(t.contains("pending"), "queue renders the pending count");
        check(t.contains("kamdir"),
              "seeded prenasal queue lists kamdir");
        check(t.contains("Approve") && t.contains("Decline"),
              "ruling actions offered");
        check(filter_ && filter_->count() >= 6,
              "kind filter offers the taxonomy");
        check(t.contains("pronunciation"),
              "per-kind counts shown in the session overview");
        filter_->setCurrentIndex(2);   // Honorifics
        const QString th = list_->toPlainText();
        check(!th.contains("kamdir"),
              "filter hides other kinds (no pronunciations)");
        filter_->setCurrentIndex(0);
        // export path against a throwaway store with one approved item
        {
            const QString saveDir = g_proposalsDir;
            const QString tmp =
                QDir::tempPath() + "/all_selftest_props";
            QDir(tmp).removeRecursively();
            QDir().mkpath(tmp);
            allcore::ProposalStore st(tmp.toStdString());
            const auto id = st.propose(
                allcore::ProposalKind::WordRendering, "selftest",
                "dge ba", "virtue", "", "selftest evidence",
                "2026-08-09");
            st.rule(id, allcore::ProposalStatus::Approved, "selftest",
                    "", "2026-08-09");
            st.save();
            g_proposalsDir = tmp;
            const QString out = tmp + "/export.tsv";
            const int n = writeApprovedExport(out);
            g_proposalsDir = saveDir;
            QFile ef(out);
            ef.open(QIODevice::ReadOnly);
            const QString body = QString::fromUtf8(ef.readAll());
            check(n == 1 && body.contains("APPROVED") &&
                      body.contains("dge ba"),
                  "approved-candidates export writes the ruling");
        }
        return fails;
    }

private:
    void rebuild() {
        loadIdentity();
        if (g_proposalsDir.isEmpty()) {
            list_->setHtml("<i>set the proposals folder in the Propose "
                           "tab first</i>");
            return;
        }
        // a 200+ item ruling session must not lose its place: keep the
        // scroll position across the refresh that follows each ruling
        const int scrollAt =
            list_->verticalScrollBar() ? list_->verticalScrollBar()->value()
                                       : 0;
        allcore::ProposalStore store(g_proposalsDir.toStdString());
        store.load();
        // per-kind pending counts for the session overview
        std::map<std::string, int> byKind;
        for (const auto& p : store.all())
            if (p.status == allcore::ProposalStatus::Pending)
                ++byKind[allcore::Proposal::kindName(p.kind)];
        QString counts;
        for (const auto& [k, n] : byKind)
            counts += QString("%1 %2 · ")
                          .arg(n)
                          .arg(QString::fromStdString(k));
        const QString want =
            filter_ ? filter_->currentData().toString() : QString();
        QString h = QString("<div><b>%1 pending</b> · signed in as %2"
                            "</div><div style='color:#777;font-size:12px'>"
                            "%3</div><hr>")
                        .arg(store.pendingCount())
                        .arg(g_userName.isEmpty() ? "(unnamed)"
                                                  : g_userName)
                        .arg(counts);
        int shown = 0;
        for (const auto& p : store.all()) {
            if (p.status != allcore::ProposalStatus::Pending) continue;
            const QString kindName = allcore::Proposal::kindName(p.kind);
            if (!want.isEmpty()) {
                if (want == "export") {
                    if (p.isRegister()) continue;
                } else if (kindName != want) {
                    continue;
                }
            }
            ++shown;
            h += "<div style='margin:8px 0;padding:6px;border:1px solid "
                 "#DDD'>";
            h += "<b>" +
                 QString(allcore::Proposal::kindName(p.kind)).toUpper() +
                 "</b>" +
                 (p.isRegister()
                      ? " <span style='color:#1E6B4E'>(applies in-app)</span>"
                      : " <span style='color:#8C2F2B'>(→ data project)"
                        "</span>");
            h += "<br><b>" + QString::fromStdString(p.wylie).toHtmlEscaped() +
                 "</b>";
            if (!p.value.empty())
                h += " → “" +
                     QString::fromStdString(p.value).toHtmlEscaped() + "”";
            if (!p.field.empty())
                h += " <small>[" +
                     QString::fromStdString(p.field).toHtmlEscaped() +
                     "]</small>";
            if (!p.evidence.empty())
                h += "<br><small style='color:#555'>" +
                     QString::fromStdString(p.evidence)
                         .left(300)
                         .toHtmlEscaped() +
                     "</small>";
            h += "<br><small style='color:#999'>proposed by " +
                 QString::fromStdString(p.proposer).toHtmlEscaped() +
                 " · " + QString::fromStdString(p.created).toHtmlEscaped() +
                 "</small>";
            const QString id = QString::fromStdString(p.id);
            h += "<br><a href='approve:" + id +
                 "'>✓ Approve</a> &nbsp; <a href='decline:" + id +
                 "'>✗ Decline</a> &nbsp; <a href='defer:" + id +
                 "'>⏸ Defer</a></div>";
        }
        if (!shown)
            h += want.isEmpty()
                     ? QString("<i>nothing pending — the queue is "
                               "clear.</i>")
                     : QString("<i>nothing pending in this kind.</i>");
        list_->setHtml(h);
        if (list_->verticalScrollBar())
            list_->verticalScrollBar()->setValue(scrollAt);
        // keep the tab label's pending count current after each ruling
        if (auto* tw = qobject_cast<QTabWidget*>(
                parentWidget() ? parentWidget()->parentWidget() : nullptr)) {
            const int ix = tw->indexOf(this);
            if (ix >= 0)
                tw->setTabText(ix, store.pendingCount()
                                       ? QString("Approval (%1)")
                                             .arg(store.pendingCount())
                                       : QString("Approval"));
        }
    }

    void onAction(const QString& s) {
        const int c = s.indexOf(':');
        if (c < 0) return;
        const QString verb = s.left(c), id = s.mid(c + 1);
        allcore::ProposalStatus st;
        if (verb == "approve") st = allcore::ProposalStatus::Approved;
        else if (verb == "decline") st = allcore::ProposalStatus::Declined;
        else if (verb == "defer") st = allcore::ProposalStatus::Deferred;
        else return;
        QString comment;
        if (st != allcore::ProposalStatus::Deferred) {
            bool ok = false;
            comment = QInputDialog::getText(
                this,
                verb == "approve" ? "Approve" : "Decline",
                "Optional note:", QLineEdit::Normal, "", &ok);
            if (!ok) return;
        }
        allcore::ProposalStore store(g_proposalsDir.toStdString());
        store.load();
        // find it to know whether to apply in-app before we rule
        const allcore::Proposal* target = nullptr;
        for (const auto& p : store.all())
            if (p.id == id.toStdString()) target = &p;
        if (!target) { rebuild(); return; }
        if (st == allcore::ProposalStatus::Approved && target->isRegister())
            applyRegister(*target);
        // declining a machine-derived pronunciation removes its derived
        // register row (only the prenasal-derived class — community and
        // hgm-attested rows are never touched by a ruling)
        if (st == allcore::ProposalStatus::Declined &&
            target->kind == allcore::ProposalKind::Pronunciation)
            upgradeColloquialRow(
                root_ + "/data/pron_colloquial/colloquial_pron.tsv",
                *target, false);
        store.rule(id.toStdString(), st,
                   g_userName.isEmpty() ? "authority"
                                        : g_userName.toStdString(),
                   comment.toStdString(), isoToday().toStdString());
        store.save();
        rebuild();
    }

    // the ruling's register mutation lives in allcore
    // (applyPronunciationRuling — battery-proven) — this is a thin
    // adapter from the proposal to that call
    bool upgradeColloquialRow(const QString& path,
                              const allcore::Proposal& p, bool approve) {
        return allcore::applyPronunciationRuling(
            path.toStdString(), p.value, p.wylie, approve,
            g_userName.toStdString(), isoToday().toStdString());
    }

    // register approvals write straight into the app's register files,
    // appended with the approved tier + provenance
    void applyRegister(const allcore::Proposal& p) {
        QString path, line;
        if (p.kind == allcore::ProposalKind::Honorific ||
            p.kind == allcore::ProposalKind::HighHonorific ||
            p.kind == allcore::ProposalKind::Humilific ||
            p.kind == allcore::ProposalKind::DoubleHonorific) {
            path = root_ + "/data/honorifics/honorific_register.tsv";
            const QString level =
                p.kind == allcore::ProposalKind::HighHonorific ? "high"
                : p.kind == allcore::ProposalKind::Humilific ? "humilific"
                : p.kind == allcore::ProposalKind::DoubleHonorific
                    ? "double"
                    : "honorific";
            // honorific_wylie \t ordinary \t domain \t level
            line = QString::fromStdString(p.wylie) + "\t" +
                   QString::fromStdString(p.field) + "\t" +
                   QString::fromStdString(p.value) + "\t" + level;
        } else if (p.kind == allcore::ProposalKind::Pronunciation) {
            // a machine-derived row for the same form may already exist
            // (the seeded prenasal review): upgrade it IN PLACE to
            // approved rather than appending a duplicate
            path = root_ + "/data/pron_colloquial/colloquial_pron.tsv";
            if (upgradeColloquialRow(path, p, true)) return;
            // colloquial \t wylie \t gmr_pron \t class
            line = QString::fromStdString(p.value) + "\t" +
                   QString::fromStdString(p.wylie) + "\t\tapproved";
        } else if (p.kind == allcore::ProposalKind::Abbreviation) {
            path = root_ + "/data/abbreviations/approved_abbreviations.tsv";
        }
        if (path.isEmpty()) return;
        QFile f(path);
        if (f.open(QIODevice::Append | QIODevice::Text)) {
            QTextStream ts(&f);
            ts << line << "\t# approved by "
               << (g_userName.isEmpty() ? "authority" : g_userName) << " "
               << isoToday() << "\n";
        }
    }

    // the read-only decisions record: every ruled proposal, newest
    // first — the scholarly artifact the store accumulates over time
    void showArchive() {
        allcore::ProposalStore store(g_proposalsDir.toStdString());
        store.load();
        QString h = "<div><b>Rulings archive</b> — every decision, "
                    "newest first (read-only)</div><hr>";
        int n = 0;
        const auto& all = store.all();
        for (auto it = all.rbegin(); it != all.rend(); ++it) {
            const auto& p = *it;
            if (p.status == allcore::ProposalStatus::Pending) continue;
            ++n;
            const bool app = p.status == allcore::ProposalStatus::Approved;
            h += QString("<div style='margin:7px 0;padding:5px;"
                         "border-left:3px solid %1'>"
                         "<b style='color:%1'>%2</b> · %3 <b>%4</b>%5"
                         "<br><small style='color:#777'>proposed by %6 "
                         "(%7) · ruled by <b>%8</b>, %9%10</small></div>")
                     .arg(app ? "#1E6B4E"
                              : p.status ==
                                        allcore::ProposalStatus::Declined
                                    ? "#8C2F2B"
                                    : "#999999")
                     .arg(QString(allcore::Proposal::statusName(p.status))
                              .toUpper())
                     .arg(allcore::Proposal::kindName(p.kind))
                     .arg(QString::fromStdString(p.wylie).toHtmlEscaped())
                     .arg(p.value.empty()
                              ? QString()
                              : " → “" +
                                    QString::fromStdString(p.value)
                                        .toHtmlEscaped() + "”")
                     .arg(QString::fromStdString(p.proposer).toHtmlEscaped())
                     .arg(QString::fromStdString(p.created))
                     .arg(QString::fromStdString(p.approver).toHtmlEscaped())
                     .arg(QString::fromStdString(p.ruled))
                     .arg(p.comment.empty()
                              ? QString()
                              : " — <i>" +
                                    QString::fromStdString(p.comment)
                                        .toHtmlEscaped() + "</i>");
        }
        if (!n) h += "<i>no rulings yet</i>";
        list_->setHtml(h);
    }

    void exportApproved() {
        const QString out = QFileDialog::getSaveFileName(
            this, "Export approved dictionary candidates",
            root_ + "/data/candidate_alignments/approved_terms.tsv",
            "TSV (*.tsv)");
        if (out.isEmpty()) return;
        writeApprovedExport(out);
    }

    int writeApprovedExport(const QString& out) {
        allcore::ProposalStore store(g_proposalsDir.toStdString());
        store.load();
        QFile f(out);
        if (!f.open(QIODevice::WriteOnly | QIODevice::Text)) return 0;
        QTextStream ts(&f);
        ts << "# APPROVED dictionary/corpus candidates for the data "
              "project — approved in-app, never auto-ingested\n";
        ts << "# kind\twylie\tvalue\tevidence\tapprover\truled\n";
        int n = 0;
        for (const auto& p : store.all()) {
            if (p.status != allcore::ProposalStatus::Approved ||
                p.isRegister())
                continue;
            ts << allcore::Proposal::kindName(p.kind) << "\t"
               << QString::fromStdString(p.wylie) << "\t"
               << QString::fromStdString(p.value) << "\t"
               << QString::fromStdString(p.evidence).replace('\t', ' ')
                      .replace('\n', ' ')
               << "\t" << QString::fromStdString(p.approver) << "\t"
               << QString::fromStdString(p.ruled) << "\n";
            ++n;
        }
        list_->setHtml(list_->toHtml() +
                       QString("<div style='color:#1E6B4E'>exported %1 "
                               "approved dictionary candidate(s) to %2"
                               "</div>")
                           .arg(n)
                           .arg(out.toHtmlEscaped()));
        return n;
    }

    QString root_;
    QTextBrowser* list_ = nullptr;
    QComboBox* filter_ = nullptr;
};


// ---- HelpWindow (Adam, 2026-08-10): searchable help + tutorials.
// Two sources, one search: hand-written chapters (data/help/
// tutorials.md, one ## chapter per pane) and an AUTO-INDEX of every
// button/toggle in the app with its tooltip and location — derived
// like the menu bar, so new features are searchable with zero help
// maintenance. Every hit says where the feature lives and can raise
// the pane.


// ---- ManuscriptPane (task #56, Adam's post-demo Tier 1): the
// translator's writing surface — a rich-text manuscript editor with
// the Gofer search grammar in a sidebar, so the corpus is one
// keystroke away while writing. Files are HTML (lossless round-trip
// through Qt); RTF export goes through macOS textutil.
class ManuscriptPane : public QWidget {
public:
    ManuscriptPane(const allcore::Spine& spine, QWidget* parent = nullptr)
        : QWidget(parent), spine_(spine) {
        auto* outer = new QVBoxLayout(this);
        auto* bar = new QHBoxLayout;
        auto* openB = new QPushButton("Open manuscript\u2026");
        openB->setToolTip("Open a manuscript file (.html) saved from "
                          "this pane.");
        auto* saveB = new QPushButton("Save");
        saveB->setToolTip("Save the manuscript. Autosaves every "
                          "minute once a file exists.");
        auto* saveAsB = new QPushButton("Save as\u2026");
        auto* rtfB = new QPushButton("Export RTF\u2026");
        rtfB->setToolTip("Export the manuscript as RTF for Word or "
                         "Pages (via the Mac's own converter).");
        boldB_ = new QPushButton("B");
        boldB_->setCheckable(true);
        boldB_->setFixedWidth(28);
        boldB_->setToolTip("Bold (applies to the selection)");
        italB_ = new QPushButton("I");
        italB_->setCheckable(true);
        italB_->setFixedWidth(28);
        italB_->setToolTip("Italic (applies to the selection)");
        undB_ = new QPushButton("U");
        undB_->setCheckable(true);
        undB_->setFixedWidth(28);
        undB_->setToolTip("Underline (applies to the selection)");
        sizeSpin_ = new QSpinBox;
        sizeSpin_->setRange(9, 48);
        sizeSpin_->setValue(14);
        sizeSpin_->setToolTip("Font size (applies to the selection)");
        auto* sideToggle = new QCheckBox("Search sidebar");
        sideToggle->setChecked(true);
        sideToggle->setToolTip(
            "Show or hide the Gofer corpus search beside the "
            "manuscript.");
        bar->addWidget(openB);
        bar->addWidget(saveB);
        bar->addWidget(saveAsB);
        bar->addWidget(rtfB);
        bar->addSpacing(12);
        bar->addWidget(boldB_);
        bar->addWidget(italB_);
        bar->addWidget(undB_);
        bar->addWidget(sizeSpin_);
        bar->addStretch();
        bar->addWidget(sideToggle);
        outer->addLayout(bar);

        auto* split = new QSplitter;
        editor_ = new QTextEdit;
        editor_->setAcceptRichText(true);
        {
            QFont f = editor_->font();
            f.setPointSize(14);
            editor_->setFont(f);
        }
        editor_->setPlaceholderText(
            "The manuscript. Write here \u2014 paste Tibetan script "
            "from the Overlay or Convert panes and it keeps its "
            "shape. Search the corpus in the sidebar; \u201cinsert\u201d "
            "places the master's English at your cursor.");
        split->addWidget(editor_);

        side_ = new QWidget;
        auto* sv = new QVBoxLayout(side_);
        sv->setContentsMargins(0, 0, 0, 0);
        query_ = new QLineEdit;
        query_->setPlaceholderText(
            "Gofer: bden pa \u00b7 stong OR bden \u00b7 \"sdug "
            "bsngal\" NEAR/5 \"bden pa\"");
        auto* findB = new QPushButton("Find in corpus");
        findB->setToolTip(
            "Search the aligned corpus with the Gofer grammar (OR, "
            "NEAR/N). Results show the master's English beside the "
            "Tibetan.");
        results_ = new QTextBrowser;
        results_->setOpenLinks(false);
        sv->addWidget(query_);
        sv->addWidget(findB);
        sv->addWidget(results_, 1);
        split->addWidget(side_);
        split->setStretchFactor(0, 3);
        split->setStretchFactor(1, 2);
        outer->addWidget(split, 1);
        status_ = new QLabel("No file \u2014 Save as\u2026 to name the "
                             "manuscript.");
        status_->setStyleSheet("color:#777;font-size:11px");
        outer->addWidget(status_);

        connect(sideToggle, &QCheckBox::toggled, side_,
                &QWidget::setVisible);
        connect(openB, &QPushButton::clicked, [this] {
            const QString fn = QFileDialog::getOpenFileName(
                this, "Open manuscript", QString(),
                "Manuscript (*.html)");
            if (!fn.isEmpty()) openFile(fn);
        });
        connect(saveB, &QPushButton::clicked, [this] { save(); });
        connect(saveAsB, &QPushButton::clicked, [this] { saveAs(); });
        connect(rtfB, &QPushButton::clicked, [this] { exportRtf(); });
        auto applyFmt = [this](auto set) {
            QTextCursor c = editor_->textCursor();
            QTextCharFormat f;
            set(f);
            c.mergeCharFormat(f);
            editor_->mergeCurrentCharFormat(f);
        };
        connect(boldB_, &QPushButton::toggled, [applyFmt](bool on) {
            applyFmt([on](QTextCharFormat& f) {
                f.setFontWeight(on ? QFont::Bold : QFont::Normal);
            });
        });
        connect(italB_, &QPushButton::toggled, [applyFmt](bool on) {
            applyFmt([on](QTextCharFormat& f) { f.setFontItalic(on); });
        });
        connect(undB_, &QPushButton::toggled, [applyFmt](bool on) {
            applyFmt(
                [on](QTextCharFormat& f) { f.setFontUnderline(on); });
        });
        connect(sizeSpin_, &QSpinBox::valueChanged, [applyFmt](int v) {
            applyFmt([v](QTextCharFormat& f) {
                f.setFontPointSize(v);
            });
        });
        connect(editor_, &QTextEdit::textChanged,
                [this] { dirty_ = true; });
        connect(findB, &QPushButton::clicked, [this] { find(); });
        connect(query_, &QLineEdit::returnPressed, [this] { find(); });
        connect(results_, &QTextBrowser::anchorClicked,
                [this](const QUrl& u) {
                    const QString a = u.toString();
                    if (!a.startsWith("ins:")) return;
                    const int ix = a.mid(4).toInt();
                    if (ix >= 0 && ix < (int)inserts_.size())
                        editor_->textCursor().insertText(inserts_[ix]);
                });

        // autosave once a minute when named and dirty
        auto* t = new QTimer(this);
        t->setInterval(60 * 1000);
        connect(t, &QTimer::timeout, [this] {
            if (dirty_ && !path_.isEmpty()) save();
        });
        t->start();
        // reopen last manuscript
        const QString last = QSettings("ALL", "TranslationTool")
                                 .value("manuscript/lastFile")
                                 .toString();
        if (!last.isEmpty() && QFile::exists(last)) openFile(last);
    }

    void openFile(const QString& fn) {
        QFile f(fn);
        if (!f.open(QIODevice::ReadOnly)) return;
        editor_->setHtml(QString::fromUtf8(f.readAll()));
        path_ = fn;
        dirty_ = false;
        QSettings("ALL", "TranslationTool")
            .setValue("manuscript/lastFile", fn);
        status_->setText(QFileInfo(fn).fileName());
    }

    bool save() {
        if (path_.isEmpty()) return saveAs();
        QFile f(path_);
        if (!f.open(QIODevice::WriteOnly)) {
            status_->setText("Could not write " + path_);
            return false;
        }
        f.write(editor_->toHtml().toUtf8());
        dirty_ = false;
        status_->setText(QFileInfo(path_).fileName() + " \u2014 saved " +
                         QTime::currentTime().toString("HH:mm"));
        return true;
    }

    bool saveAs() {
        QString fn = QFileDialog::getSaveFileName(
            this, "Save manuscript", QString(), "Manuscript (*.html)");
        if (fn.isEmpty()) return false;
        if (!fn.endsWith(".html")) fn += ".html";
        path_ = fn;
        QSettings("ALL", "TranslationTool")
            .setValue("manuscript/lastFile", fn);
        return save();
    }

    void exportRtf() {
        if (path_.isEmpty() && !saveAs()) return;
        if (dirty_) save();
        QString fn = QFileDialog::getSaveFileName(
            this, "Export RTF", QString(), "Rich Text (*.rtf)");
        if (fn.isEmpty()) return;
        if (!fn.endsWith(".rtf")) fn += ".rtf";
        QProcess p;
        p.start("textutil", {"-convert", "rtf", path_, "-output", fn});
        p.waitForFinished(15000);
        status_->setText(p.exitCode() == 0
                             ? "Exported " + QFileInfo(fn).fileName()
                             : "RTF export failed \u2014 is this a Mac?");
    }

    void find() {
        const QString q = query_->text().trimmed();
        if (q.isEmpty()) return;
        QString h;
        inserts_.clear();
        try {
            const auto hits =
                allcore::goferSearch(spine_, q.toStdString(), "", 30);
            h += QString("<p><b>%1</b> window(s)</p>").arg(hits.size());
            for (const auto& hit : hits) {
                h += "<div style='margin:6px 0;padding:4px;border-top:"
                     "1px solid #CCC'><i>" +
                     QString::fromStdString(hit.course).toHtmlEscaped() +
                     "</i><br>";
                for (const auto& seg : hit.window) {
                    const QString eng =
                        QString::fromStdString(seg.english);
                    h += QString::fromStdString(seg.wylie)
                             .toHtmlEscaped() +
                         "<br><b>" + eng.toHtmlEscaped() + "</b> ";
                    h += QString("<a href='ins:%1'>insert</a><br>")
                             .arg(inserts_.size());
                    inserts_.push_back(eng);
                }
                h += "</div>";
            }
        } catch (const std::exception& e) {
            h = QString("<p>Query not understood: %1</p>")
                    .arg(QString::fromUtf8(e.what()).toHtmlEscaped());
        }
        results_->setHtml(h);
    }

    int selfTest(QStringList& log) {
        int fails = 0;
        auto check = [&](bool ok, const char* what) {
            log << QString("  [%1] Manuscript: %2")
                       .arg(ok ? "PASS" : "FAIL")
                       .arg(what);
            if (!ok) ++fails;
        };
        editor_->clear();
        editor_->insertPlainText("the four truths");
        editor_->selectAll();
        boldB_->setChecked(true);
        check(editor_->textCursor().charFormat().fontWeight() >=
                  QFont::Bold,
              "bold applies to the selection");
        boldB_->setChecked(false);
        query_->setText("bden pa");
        find();
        check(results_->toPlainText().contains("bden"),
              "Gofer sidebar reaches the corpus");
        const QString tmp =
            QDir::temp().filePath("all_manuscript_selftest.html");
        path_ = tmp;
        editor_->clear();
        editor_->insertPlainText("round trip");
        const bool saved = save();
        editor_->clear();
        openFile(tmp);
        check(saved &&
                  editor_->toPlainText().contains("round trip"),
              "manuscript saves and reopens");
        QFile::remove(tmp);
        path_.clear();
        editor_->clear();
        QSettings("ALL", "TranslationTool")
            .remove("manuscript/lastFile");
        return fails;
    }

private:
    const allcore::Spine& spine_;
    QTextEdit* editor_ = nullptr;
    QWidget* side_ = nullptr;
    QLineEdit* query_ = nullptr;
    QTextBrowser* results_ = nullptr;
    QLabel* status_ = nullptr;
    QPushButton* boldB_ = nullptr;
    QPushButton* italB_ = nullptr;
    QPushButton* undB_ = nullptr;
    QSpinBox* sizeSpin_ = nullptr;
    QString path_;
    bool dirty_ = false;
    std::vector<QString> inserts_;
};

// ---- SettingsDialog (Adam, 2026-08-10): one place for everything the
// app remembers, grouped and explained. Values live in QSettings —
// this dialog only reads/writes the same keys the panes use.
class SettingsDialog : public QDialog {
public:
    SettingsDialog(std::function<void(bool)> applyNight, QWidget* parent)
        : QDialog(parent), applyNight_(applyNight) {
        setWindowTitle("Settings");
        resize(560, 520);
        setStyleSheet(
            "QDialog { background: #F1EBDD; }"
            "QGroupBox { font-weight: 600; color: #8C2F2B;"
            "  border: 1px solid #C9B992; border-radius: 8px;"
            "  margin-top: 12px; padding-top: 8px;"
            "  background: #FAF6EE; }"
            "QGroupBox::title { subcontrol-origin: margin;"
            "  left: 10px; padding: 0 4px; }"
            "QLabel, QCheckBox { color: #2B2118; }"
            "QLineEdit { background: #FFFFFF; color: #2B2118;"
            "  border: 1px solid #C9B992; border-radius: 6px;"
            "  padding: 5px 8px; }");
        QSettings st("ALL", "TranslationTool");
        auto* outer = new QVBoxLayout(this);

        auto* appearance = new QGroupBox("Appearance");
        auto* al = new QFormLayout(appearance);
        night_ = new QCheckBox("Night mode (dark chrome, cream pages)");
        night_->setChecked(st.value("app/nightMode", true).toBool());
        al->addRow(night_);
        outer->addWidget(appearance);

        auto* reading = new QGroupBox("Reading");
        auto* rl = new QFormLayout(reading);
        script_ = new QComboBox;
        script_->addItems({"Tibetan script", "ACIP", "Wylie"});
        script_->setCurrentIndex(
            st.value("overlay/scriptMode", 1).toInt());
        rl->addRow("Overlay displays text as", script_);
        outer->addWidget(reading);

        auto* team = new QGroupBox("Team & proposals");
        auto* tl = new QFormLayout(team);
        name_ = new QLineEdit(st.value("team/name").toString());
        tl->addRow("Your name (provenance)", name_);
        admin_ = new QCheckBox(
            "Authority role (Geshe Michael / Adam) \u2014 shows the "
            "Approval pane");
        admin_->setChecked(st.value("team/admin", false).toBool());
        tl->addRow(admin_);
        dir_ = new QLineEdit(st.value("team/proposalsDir").toString());
        auto* pick = new QPushButton("Choose\u2026");
        auto* dr = new QHBoxLayout;
        dr->addWidget(dir_, 1);
        dr->addWidget(pick);
        auto* dw = new QWidget;
        dw->setLayout(dr);
        tl->addRow("Shared proposals folder", dw);
        connect(pick, &QPushButton::clicked, [this] {
            const QString d = QFileDialog::getExistingDirectory(
                this, "Shared proposals folder (the team Dropbox)");
            if (!d.isEmpty()) dir_->setText(d);
        });
        outer->addWidget(team);

        auto* data = new QGroupBox("Data");
        auto* dl = new QFormLayout(data);
        dataRoot_ = new QLineEdit(st.value("app/dataRoot").toString());
        dataRoot_->setPlaceholderText(
            "auto-detected \u2014 set only to override");
        dl->addRow("Data folder override", dataRoot_);
        dl->addRow(new QLabel(
            "<small style='color:#5C4F40'>Changes to the data folder "
            "or the authority role take effect on the next "
            "launch.</small>"));
        outer->addWidget(data);
        outer->addStretch();

        auto* row = new QHBoxLayout;
        row->addStretch();
        auto* cancel = new QPushButton("Cancel");
        auto* save = new QPushButton("Save");
        save->setDefault(true);
        row->addWidget(cancel);
        row->addWidget(save);
        outer->addLayout(row);
        connect(cancel, &QPushButton::clicked, this, &QDialog::reject);
        connect(save, &QPushButton::clicked, [this] {
            QSettings s2("ALL", "TranslationTool");
            s2.setValue("app/nightMode", night_->isChecked());
            s2.setValue("overlay/scriptMode", script_->currentIndex());
            s2.setValue("team/name", name_->text().trimmed());
            s2.setValue("team/admin", admin_->isChecked());
            s2.setValue("team/proposalsDir", dir_->text().trimmed());
            if (dataRoot_->text().trimmed().isEmpty())
                s2.remove("app/dataRoot");
            else
                s2.setValue("app/dataRoot", dataRoot_->text().trimmed());
            if (applyNight_) applyNight_(night_->isChecked());
            accept();
        });
    }

private:
    std::function<void(bool)> applyNight_;
    QCheckBox* night_ = nullptr;
    QComboBox* script_ = nullptr;
    QLineEdit* name_ = nullptr;
    QCheckBox* admin_ = nullptr;
    QLineEdit* dir_ = nullptr;
    QLineEdit* dataRoot_ = nullptr;
};

class HelpWindow : public QDialog {
public:
    HelpWindow(QTabWidget* tabs, const QString& root, QWidget* parent)
        : QDialog(parent), tabs_(tabs) {
        setWindowTitle("ALL Tool Help & Tutorials");
        resize(1020, 680);
        // the manuscript identity: cream page, maroon heads, gold
        // accents — the same design language as the leadership docs
        setStyleSheet(
            "QDialog { background: #F1EBDD; }"
            "QLineEdit { background: #FFFFFF; color: #2B2118;"
            "  border: 1px solid #C9B992; border-radius: 8px;"
            "  padding: 9px 14px; font-size: 15px; }"
            "QListWidget { background: #FAF6EE; color: #2B2118;"
            "  border: 1px solid #C9B992; border-radius: 8px;"
            "  padding: 6px; font-size: 13px; outline: none; }"
            "QListWidget::item { padding: 7px 8px; border-radius: 5px; }"
            "QListWidget::item:selected { background: #8C2F2B;"
            "  color: #FAF6EE; }"
            "QTextBrowser { background: #FAF6EE; color: #2B2118;"
            "  border: 1px solid #C9B992; border-radius: 8px;"
            "  padding: 18px; }"
            "QSplitter::handle { background: transparent; }");
        auto* outer = new QVBoxLayout(this);
        outer->setContentsMargins(16, 14, 16, 16);
        outer->setSpacing(10);
        auto* title = new QLabel(
            "<span style='color:#9A7A33; letter-spacing:2px;"
            " font-size:11px'>ASIAN LEGACY LIBRARY</span><br>"
            "<span style='color:#8C2F2B; font-size:22px;"
            " font-weight:600'>Help &amp; Tutorials</span>");
        outer->addWidget(title);
        search_ = new QLineEdit;
        search_->setPlaceholderText(
            "Search any feature, button, pane, or topic\u2026");
        outer->addWidget(search_);
        auto* split = new QSplitter;
        results_ = new QListWidget;
        results_->setMinimumWidth(290);
        body_ = new QTextBrowser;
        body_->setOpenLinks(false);
        body_->document()->setDefaultStyleSheet(
            "h1, h2 { color: #8C2F2B; }"
            "h2 { font-size: 21px; margin-bottom: 4px; }"
            "h3 { color: #5A3B22; }"
            "a { color: #9A7A33; }"
            "strong { color: #5A3B22; }"
            "p, li { font-size: 14px; line-height: 1.45; }"
            "code { background: #F1EBDD; }");
        {
            QFont bf = body_->font();
            bf.setFamilies({"Iowan Old Style", "Palatino", "Georgia"});
            bf.setPointSize(14);
            body_->setFont(bf);
        }
        split->addWidget(results_);
        split->addWidget(body_);
        split->setStretchFactor(1, 1);
        outer->addWidget(split, 1);

        // chapters from tutorials.md
        QFile f(root + "/data/help/tutorials.md");
        if (f.open(QIODevice::ReadOnly)) {
            const QString md = QString::fromUtf8(f.readAll());
            for (const QString& part : md.split("\n## ")) {
                const int nl = part.indexOf('\n');
                if (nl < 0) continue;
                QString title = part.left(nl).trimmed();
                title.remove(QRegularExpression("^#+\\s*"));
                chapters_[title] = "## " + part;
                chapterOrder_ << title;
            }
        }
        // auto-index every control in every pane — two-level aware:
        // top tabs are GROUPS holding inner pane tabs
        auto indexPane = [this](QWidget* w, const QString& pane,
                                int loc) {
            for (auto* b : w->findChildren<QPushButton*>())
                if (!b->text().trimmed().isEmpty())
                    controls_.push_back(
                        {b->text(), pane, b->toolTip(), loc});
            for (auto* c : w->findChildren<QCheckBox*>())
                if (!c->text().trimmed().isEmpty())
                    controls_.push_back(
                        {c->text(), pane, c->toolTip(), loc});
            for (auto* cb : w->findChildren<QComboBox*>())
                for (int k = 0; k < cb->count(); ++k)
                    controls_.push_back(
                        {cb->itemText(k), pane, cb->toolTip(), loc});
        };
        for (int i = 0; i < tabs_->count(); ++i) {
            if (auto* inner =
                    qobject_cast<QTabWidget*>(tabs_->widget(i))) {
                for (int j = 0; j < inner->count(); ++j) {
                    locs_.push_back({i, inner, j});
                    indexPane(inner->widget(j), inner->tabText(j),
                              (int)locs_.size() - 1);
                }
            } else {
                locs_.push_back({i, nullptr, -1});
                indexPane(tabs_->widget(i), tabs_->tabText(i),
                          (int)locs_.size() - 1);
            }
        }
        connect(search_, &QLineEdit::textChanged,
                [this](const QString& q) { doSearch(q); });
        connect(results_, &QListWidget::currentRowChanged,
                [this](int r) { showResult(r); });
        connect(body_, &QTextBrowser::anchorClicked,
                [this](const QUrl& u) {
                    const QString a = u.toString();
                    if (a.startsWith("pane:")) {
                        const int k = a.mid(5).toInt();
                        if (k >= 0 && k < (int)locs_.size()) {
                            tabs_->setCurrentIndex(locs_[k].outer);
                            if (locs_[k].inner)
                                locs_[k].inner->setCurrentIndex(
                                    locs_[k].ii);
                        }
                        raise();
                    } else if (a.startsWith("chapter:")) {
                        showChapter(a.mid(8));
                    } else if (a.startsWith("http")) {
                        QDesktopServices::openUrl(u);
                    }
                });
        doSearch("");   // initial: the chapter list
    }

    int indexSize() const {
        return (int)controls_.size() + chapters_.size();
    }
    int hitCount(const QString& q) {
        doSearch(q);
        return results_->count();
    }
    int chapterCount() const { return chapters_.size(); }
    void openChapter(const QString& title) { showChapter(title); }
    // the Workflows window opens listing EVERY workflow by name
    // (Adam, 2026-08-10); clicking one jumps the page to it
    void openWorkflows() {
        const QString ch = "Suggested Workflows";
        showChapter(ch);
        search_->clear();
        results_->clear();
        rows_.clear();
        results_->addItem("\U0001F4D6 " + ch + " (all)");
        rows_.push_back({-1, ch});
        static const QRegularExpression re("^\\*\\*(.+?)\\*\\*");
        for (const QString& raw : chapters_.value(ch).split('\n')) {
            const auto m = re.match(raw.trimmed());
            if (!m.hasMatch()) continue;
            QString t = m.captured(1).trimmed();
            if (t.endsWith('.')) t.chop(1);
            if (t.startsWith("Where do I start")) continue;
            results_->addItem("\u25B8 " + t);
            rows_.push_back({-2, t});
        }
    }
    int openWorkflowsCount() {
        openWorkflows();
        return results_->count();
    }

private:
    struct Ctl { QString label, pane, tip; int tab; };
    struct Loc { int outer; QTabWidget* inner; int ii; };

    void doSearch(const QString& q) {
        results_->clear();
        rows_.clear();
        const QString needle = q.trimmed();
        if (needle.isEmpty()) {
            for (const auto& t : chapterOrder_) {
                results_->addItem("\U0001F4D6 " + t);
                rows_.push_back({-1, t});
            }
            body_->setMarkdown(
                chapters_.value("Getting Started",
                                "Welcome. Pick a chapter."));
            return;
        }
        // chapters whose title or text match
        for (const auto& t : chapterOrder_)
            if (t.contains(needle, Qt::CaseInsensitive) ||
                chapters_[t].contains(needle, Qt::CaseInsensitive)) {
                results_->addItem("\U0001F4D6 " + t);
                rows_.push_back({-1, t});
            }
        // controls whose label/tooltip/pane match
        for (int i = 0; i < (int)controls_.size(); ++i) {
            const auto& c = controls_[i];
            if (c.label.contains(needle, Qt::CaseInsensitive) ||
                c.tip.contains(needle, Qt::CaseInsensitive) ||
                c.pane.contains(needle, Qt::CaseInsensitive)) {
                results_->addItem(c.label + "  \u2014  " + c.pane);
                rows_.push_back({i, QString()});
            }
        }
        if (!results_->count())
            body_->setHtml("<i>no matches \u2014 try another word, "
                           "or browse the chapters (clear the "
                           "search)</i>");
        else
            results_->setCurrentRow(0);
    }

    void showResult(int r) {
        if (r < 0 || r >= (int)rows_.size()) return;
        const auto& row = rows_[r];
        if (row.first == -2) {
            showChapter("Suggested Workflows");
            QTextCursor c = body_->textCursor();
            c.movePosition(QTextCursor::Start);
            body_->setTextCursor(c);
            body_->find(row.second);
            return;
        }
        if (row.first < 0) { showChapter(row.second); return; }
        const auto& c = controls_[row.first];
        QString h = "<h2>" + c.label.toHtmlEscaped() + "</h2>";
        h += "<p>Located in the <b>" + c.pane.toHtmlEscaped() +
             "</b> pane \u2014 <a href='pane:" +
             QString::number(c.tab) + "'>open that pane now</a>.</p>";
        if (!c.tip.isEmpty())
            h += "<p>" + c.tip.toHtmlEscaped() + "</p>";
        QString chapterName;
        for (const auto& t : chapterOrder_)
            if (c.pane.startsWith(t.left(6))) chapterName = t;
        if (!chapterName.isEmpty())
            h += "<p><a href='chapter:" + chapterName +
                 "'>Read the full " + chapterName.toHtmlEscaped() +
                 " tutorial \u2192</a></p>";
        h += "<p style='color:#777'>Tip: every button is also in the "
             "menu bar under its pane\u2019s menu \u2014 and the "
             "Mac\u2019s Help-menu search can point an arrow at "
             "it.</p>";
        body_->setHtml(h);
    }

    void showChapter(const QString& name) {
        if (chapters_.contains(name))
            body_->setMarkdown(chapters_[name]);
    }

    QTabWidget* tabs_;
    QLineEdit* search_ = nullptr;
    QListWidget* results_ = nullptr;
    QTextBrowser* body_ = nullptr;
    QMap<QString, QString> chapters_;
    QStringList chapterOrder_;
    std::vector<Ctl> controls_;
    std::vector<Loc> locs_;
    std::vector<std::pair<int, QString>> rows_;
};

int main(int argc, char** argv) {
    QApplication app(argc, argv);
    const QString root = findDataRoot();
    // bundled Tibetan font (Noto Serif Tibetan, OFL — data/fonts/):
    // consistent complex-stack shaping regardless of system fonts
    QFontDatabase::addApplicationFont(root +
                                      "/data/fonts/NotoSerifTibetan.ttf");
    QFontDatabase::addApplicationFont(
        root + "/data/fonts/BabelStoneTibetanSlim.ttf");
    // Jomolhari (Chris Fynn, OFL 1.0 — bundled): the beloved classic
    // dbu-can bookface, modeled on early Kangyur manuscript letterforms
    QFontDatabase::addApplicationFont(root +
                                      "/data/fonts/Jomolhari-Regular.ttf");
    // Monlam Bodyig (Lobsang Monlam / monlamit.com, OFL 1.1 — bundled):
    // the UI face most widely used in the Tibetan community
    QFontDatabase::addApplicationFont(
        root + "/data/fonts/MonlamBodyig-Regular.ttf");
    // Style faces present as FILES on this machine but not active in Font
    // Book (data/fonts/FONTS.md): register per-session from Adam's own
    // disk — display variety (chos-rgyal, yig-chung cursive, calligraphic),
    // never bundled. All five probed at full working-set coverage.
    for (const char* styleFile :
         {"tibusrfa2.ttf", "TibetanChosgyalUni_2014-12-23-shipped.ttf",
          "TibetanCalligraphicUnicode.1.0.ttf", "TibetanSambhotaYigchung.ttf",
          "TibetanUnicode.ttf"}) {
        const QString p =
            QDir::homePath() + "/Library/Fonts/" + styleFile;
        if (QFileInfo::exists(p)) QFontDatabase::addApplicationFont(p);
    }
    // Prefer SambhotaDege for Tibetan wherever it appears (Adam's request,
    // 2026-08-07): the app-wide font keeps the system face for Latin and
    // falls back per-character to the preferred Tibetan family — so every
    // pane's inline Tibetan (cards, trainer, drills, lookup) picks it up.
    // System-installed only; never bundled (not ours to redistribute).
    {
        QFont appFont = app.font();
        QStringList fams = appFont.families();
        if (fams.isEmpty()) fams << appFont.family();
        fams << (QFontDatabase::hasFamily("SambhotaDege")
                     ? QStringLiteral("SambhotaDege")
                     : QStringLiteral("Noto Serif Tibetan"));
        appFont.setFamilies(fams);
        app.setFont(appFont);
    }
    QString dbPath = (argc > 1 && argv[1][0] != '-')
                         ? argv[1]
                         : root + "/build/hgm_spine_v27_2.db";
    QString tplPath = argc > 2 ? argv[2]
                               : root + "/docs/analysis/PASSAGE_ANALYSIS_TEMPLATE.md";

    allcore::Spine spine(dbPath.toStdString());

    // spellcheck layer is optional: missing data just disables the underlines
    allcore::SyllableChecker* checker = nullptr;
    try {
        static allcore::SyllableChecker chk(
            (argc > 3 ? QString(argv[3]) : QString(root + "/data/spellcheck")).toStdString());
        checker = &chk;
    } catch (const std::exception&) {}

    // reference dictionaries (TD + THL) are optional and LOCAL-ONLY
    allcore::RefDict* refdict = nullptr;
    try {
        static allcore::RefDict rd((root + "/build/reference.db").toStdString());
        refdict = &rd;
    } catch (const std::exception&) {}

    // the Mahāvyutpatti reference table (DILA TEI, CC BY-SA 3.0) — optional
    allcore::Mvp* mvp = nullptr;
    {
        static allcore::Mvp m;
        if (m.load((root + "/data/extracted/mahavyutpatti.tsv").toStdString()))
            mvp = &m;
    }

    // the Whitney layer (Roots 1885 + Grammar 1879 citations; public
    // domain, digitization Apache-2.0) — optional reference comparanda
    allcore::WhitneyRoots* whitney = nullptr;
    {
        static allcore::WhitneyRoots w;
        if (w.load((root + "/data/whitney/whitney_roots.tsv").toStdString()))
            whitney = &w;
    }

    // the colloquial pronunciation register (community spellings +
    // HGM prenasal-rule forms; data/pron_colloquial) — optional
    allcore::ColloquialPron* colloq = nullptr;
    {
        static allcore::ColloquialPron cp;
        if (cp.load((root + "/data/pron_colloquial/colloquial_pron.tsv")
                        .toStdString())) {
            colloq = &cp;
            g_colloquial = &cp;
        }
    }

    // the SOAS POS lexicon (CC BY 4.0) — optional; consulted only where
    // Wilson's rules require POS data, evidence-labeled
    allcore::PosLexicon* poslex = nullptr;
    {
        static allcore::PosLexicon pl;
        if (pl.load((root + "/data/soas_pos/classical-lexicon.txt")
                        .toStdString()))
            poslex = &pl;
    }

    // the derived bsdus-tshig register (attestation-only; docs/
    // BSDUS_TSHIG_REGISTER.md is the review doc) — optional
    allcore::Contractions* contractions = nullptr;
    {
        static allcore::Contractions ct;
        if (ct.load((root + "/data/extracted/bsdus_tshig.tsv").toStdString()))
            contractions = &ct;
    }

    // the learner's own progress/SRS data — local file, optional
    allcore::Progress* progress = nullptr;
    try {
        static allcore::Progress pr((root + "/progress.db").toStdString());
        progress = &pr;
    } catch (const std::exception&) {}

    // Night mode (default on; View menu toggles, remembered):
    // dark chrome via Fusion + palette, while the READING surfaces
    // (text views and cards) stay manuscript-cream so the phrase
    // shading and every card color read exactly as designed —
    // night chrome, paper page.
    QSettings nightSettings("ALL", "TranslationTool");
    const bool nightOn =
        nightSettings.value("app/nightMode", true).toBool();
    // Reading surfaces are manuscript-cream ALWAYS — day, night, and
    // regardless of the macOS system appearance. Previously the cream
    // rule applied only when the app's own night toggle was on, so a
    // system-dark Mac with the toggle off rendered dark pages under
    // the light-tuned phrase washes: light text on light color bands
    // (Adam's screenshot, 2026-08-10). Night mode darkens the CHROME
    // only — night chrome, paper page.
    app.setStyleSheet(
        "QTextEdit, QTextBrowser, QPlainTextEdit { "
        "background: #FAF6EE; color: #2B2118; }");
    auto applyNight = [&app](bool on) {
        // native macOS dark appearance — no style swap, so window
        // resize/decorations stay untouched
        app.styleHints()->setColorScheme(on ? Qt::ColorScheme::Dark
                                            : Qt::ColorScheme::Light);
    };
    if (nightOn) applyNight(true);

    QMainWindow win;
    auto* tabsPtr = new QTabWidget;   // owned by the main window
    QTabWidget& tabs = *tabsPtr;
    win.setCentralWidget(tabsPtr);
    win.setWindowTitle(
        QString("ALL Translation Tool — HGM v%1")
            .arg(QString::fromStdString(spine.metaValue("release_version"))));
    auto* overlay = new OverlayPane(spine, checker, refdict, progress, root);
    tabs.addTab(overlay, "Overlay");
    tabs.addTab(new AnalysisPane(spine, tplPath, root + "/analyses"), "Analysis");
    auto* trainerPane =
        new TrainerPane(spine, progress, poslex, contractions);
    tabs.addTab(trainerPane, "Trainer");
    auto* drillsPane = new DrillsPane(spine, progress);
    tabs.addTab(drillsPane, "Drills");
    auto* draftPane = new DraftPane(spine, progress, root);
    tabs.addTab(draftPane, "Draft");
    auto* manuscriptPane = new ManuscriptPane(spine);
    tabs.addTab(manuscriptPane, "Manuscript");
    auto* reviewPane = new ReviewPane(spine);
    tabs.addTab(reviewPane, "Review");
    auto* alignPane = new AlignPane(spine, root);
    tabs.addTab(alignPane, "Align");
    auto* inputPane = new InputPane(checker, root);
    tabs.addTab(inputPane, "Input");
    auto* libraryPane = new LibraryPane(root, progress,
                                [overlay](const QString& path) {
                                    overlay->openFile(path);
                                    if (g_raisePane) g_raisePane(overlay);
                                });
    tabs.addTab(libraryPane, "Library");
    auto* goferPane = new GoferPane(spine, root);
    tabs.addTab(goferPane, "Search");
    tabs.addTab(makeConvertPane(mvp, whitney), "Convert");
    static std::vector<ApparatusNote> appNotes;
    static std::vector<ApparatusBib> appBib;
    {
        // the unified apparatus store (task #57) supersedes the
        // mixed-nuts pair; fall back to the mined files if absent
        QString notesPath = root + "/data/extracted/apparatus_notes.json";
        if (!QFileInfo::exists(notesPath))
            notesPath = root + "/data/extracted/mixed_nuts_notes.json";
        QFile nf(notesPath);
        if (nf.open(QIODevice::ReadOnly))
            for (const auto& v :
                 QJsonDocument::fromJson(nf.readAll()).array()) {
                const auto o = v.toObject();
                appNotes.push_back({o["lemma"].toString(),
                                    o["text"].toString(),
                                    o["source"].toString(),
                                    o.contains("num")
                                        ? o["num"].toInt()
                                        : o["note"].toInt()});
            }
        QString bibPath =
            root + "/data/extracted/apparatus_bibliography.json";
        if (!QFileInfo::exists(bibPath))
            bibPath = root + "/data/extracted/mixed_nuts_bibliography.json";
        QFile bf(bibPath);
        if (bf.open(QIODevice::ReadOnly))
            for (const auto& v :
                 QJsonDocument::fromJson(bf.readAll()).array()) {
                const auto o = v.toObject();
                appBib.push_back({o["source"].toString(),
                                  o["section"].toString(),
                                  o["id"].toString(),
                                  o["text"].toString()});
            }
        if (!appNotes.empty()) g_appNotes = &appNotes;
        if (!appBib.empty()) g_appBib = &appBib;
    }
    static const HonorificMap honorifics = loadHonorifics(
        root + "/data/honorifics/honorific_register.tsv");
    if (!honorifics.empty()) g_honorifics = &honorifics;
    static IdiomMap idioms;
    {
        QFile f(root + "/data/idioms/idioms.json");
        if (f.open(QIODevice::ReadOnly)) {
            const auto doc = QJsonDocument::fromJson(f.readAll());
            for (const auto& v :
                 doc.object().value("idioms").toArray()) {
                const auto o = v.toObject();
                idioms[o.value("wylie").toString().toStdString()] = {
                    o.value("status").toString(),
                    o.value("evidence").toString()};
            }
        }
        if (!idioms.empty()) g_idioms = &idioms;
    }
    static TeachingMap teaching;
    {
        QFile f(root + "/data/teaching/teaching_moments_card.json");
        if (f.open(QIODevice::ReadOnly)) {
            const auto doc = QJsonDocument::fromJson(f.readAll());
            const auto terms = doc.object().value("terms").toObject();
            for (auto it = terms.begin(); it != terms.end(); ++it) {
                auto& vec = teaching[it.key().toStdString()];
                for (const auto& v : it.value().toArray()) {
                    const auto o = v.toObject();
                    vec.push_back({o.value("title").toString(),
                                   o.value("url").toString(),
                                   o.value("lang").toString(),
                                   o.value("src").toString(),
                                   o.value("snippet").toString(),
                                   o.value("t").toInt()});
                }
            }
        }
        if (!teaching.empty()) g_teaching = &teaching;
        static TeachingMap teachingTib;
        QFile f2(root + "/data/teaching/teaching_moments_card.json");
        if (f2.open(QIODevice::ReadOnly)) {
            const auto doc = QJsonDocument::fromJson(f2.readAll());
            const auto tt =
                doc.object().value("tibetan_terms").toObject();
            for (auto it = tt.begin(); it != tt.end(); ++it) {
                auto& vec = teachingTib[it.key().toStdString()];
                for (const auto& v : it.value().toArray()) {
                    const auto o = v.toObject();
                    vec.push_back({o.value("title").toString(),
                                   o.value("url").toString(),
                                   o.value("lang").toString(),
                                   o.value("src").toString(),
                                   o.value("snippet").toString(),
                                   o.value("t").toInt()});
                }
            }
        }
        if (!teachingTib.empty()) g_teachingTib = &teachingTib;
    }
    static std::vector<std::pair<std::string, int>> dasSections;
    {
        QFile f(root + "/data/extracted/das_pages.json");
        if (f.open(QIODevice::ReadOnly)) {
            const auto doc = QJsonDocument::fromJson(f.readAll());
            for (const auto& v :
                 doc.object().value("sections").toArray()) {
                const auto o = v.toObject();
                dasSections.push_back(
                    {o.value("onset").toString().toStdString(),
                     o.value("pdf_page").toInt()});
            }
        }
        if (!dasSections.empty()) {
            g_dasSections = &dasSections;
            g_dasPdfPath = root + "/data/das/das_1902_bookmarked.pdf";
        }
    }
    tabs.addTab(makeLookupPane(spine, refdict, mvp, whitney, colloq),
                "Lookup");
    auto* sanskritPane = new SanskritPane(mvp, whitney);
    tabs.addTab(sanskritPane, "Sanskrit");
#ifdef ALL_HAVE_OCR
    tabs.addTab(new ScanPane(checker, root), "Scan");
#endif
    // the in-house proposal & approval channel: everyone can Propose;
    // the Approval tab shows only for an authority (Geshe Michael /
    // Adam), gated on the identity role
    loadIdentity();
    // spelling forms the authority ruled valid (declined spelling
    // flags in the shared store) — the Overlay stops doubting them
    static std::set<std::string> spellingValid;
    if (!g_proposalsDir.isEmpty()) {
        allcore::ProposalStore pstore(g_proposalsDir.toStdString());
        if (pstore.load())
            for (const auto& p : pstore.all())
                if (p.kind == allcore::ProposalKind::Spelling &&
                    p.status == allcore::ProposalStatus::Declined)
                    spellingValid.insert(p.wylie);
    }
    g_spellingValid = &spellingValid;
    // --screenshots <dir>: render every pane to PNGs and exit (demo /
    // documentation mode). Shows the Approval queue too, pointed at
    // the seeded proposals — session-only, nothing persisted.
    const QStringList cliArgs = QCoreApplication::arguments();
    const int shotIx = cliArgs.indexOf("--screenshots");
    const bool shotMode = shotIx >= 0 && shotIx + 1 < cliArgs.size();
    const bool selfTestMode = cliArgs.contains("--selftest");
    if (shotMode || selfTestMode) {
        g_isAdmin = true;
        if (g_proposalsDir.isEmpty())
            g_proposalsDir = root + "/data/proposals";
        if (g_userName.isEmpty()) g_userName = "Adam";
        g_identityPinned = true;   // panes reload identity; keep seeds
    }
    auto* proposePane = new ProposePane();
    tabs.addTab(proposePane, "Propose");
    if (g_isAdmin) {
        // the authority sees the pending count on the tab at a glance
        size_t pending = 0;
        if (!g_proposalsDir.isEmpty()) {
            allcore::ProposalStore st(g_proposalsDir.toStdString());
            st.load();
            pending = st.pendingCount();
        }
        tabs.addTab(new ApprovalPane(root),
                    pending ? QString("Approval (%1)").arg(pending)
                            : QString("Approval"));
    }
    // ---- two-level navigation (Adam, 2026-08-10): six workflow
    // groups, each an inner tab row; panes reparented untouched
    struct FlatPane { QWidget* w; QString title; QTabWidget* inner; };
    static std::vector<FlatPane> flatPanes;
    {
        std::vector<std::pair<QWidget*, QString>> all;
        for (int i = 0; i < tabs.count(); ++i)
            all.push_back({tabs.widget(i), tabs.tabText(i)});
        while (tabs.count()) tabs.removeTab(0);
        auto mkGroup = [&](const QString& gname,
                           std::initializer_list<const char*> names) {
            auto* g = new QTabWidget;
            g->setDocumentMode(true);
            for (const char* want : names)
                for (auto& [w, t] : all)
                    if (t == want ||
                        t.startsWith(QString(want) + " ("))
                        {
                            g->addTab(w, t);
                            flatPanes.push_back({w, t, g});
                        }
            if (g->count()) tabs.addTab(g, gname);
            else delete g;
        };
        mkGroup("Read", {"Overlay", "Library"});
        mkGroup("Translate",
                {"Manuscript", "Draft", "Review", "Align"});
        mkGroup("Research",
                {"Search", "Lookup", "Sanskrit", "Convert", "Analysis"});
        mkGroup("Learn", {"Trainer", "Drills"});
        mkGroup("Input", {"Input", "Scan"});
        mkGroup("Community", {"Propose", "Approval"});
        g_raisePane = [&tabs](QWidget* w) {
            for (auto& f : flatPanes)
                if (f.w == w) {
                    tabs.setCurrentWidget(f.inner);
                    f.inner->setCurrentWidget(w);
                    return;
                }
        };
    }

    // ---- the menu bar MIRRORS THE GUI (Adam, 2026-08-10): one menu
    // per workflow group, one submenu per pane — the same structure
    // as the tabs, so anything found in the window is found in the
    // same place in the menus. Actions are still derived GENERICALLY
    // from the panes' own buttons and toggles (raise the pane, then
    // click), so future features appear with no menu maintenance.
    for (int gi = 0; gi < tabs.count(); ++gi) {
        auto* g = qobject_cast<QTabWidget*>(tabs.widget(gi));
        if (!g) continue;
        QMenu* gm = win.menuBar()->addMenu(tabs.tabText(gi));
        for (int pi = 0; pi < g->count(); ++pi) {
            QWidget* pane = g->widget(pi);
            QMenu* menu = gm->addMenu(g->tabText(pi));
            QAction* go = menu->addAction("Show pane");
            QObject::connect(go, &QAction::triggered, [pane] {
                if (g_raisePane) g_raisePane(pane);
            });
            const auto btns = pane->findChildren<QPushButton*>();
            bool sep = false;
            for (QPushButton* b : btns) {
                const QString label = b->text().trimmed();
                if (label.isEmpty()) continue;
                if (!sep) { menu->addSeparator(); sep = true; }
                QAction* a = menu->addAction(label);
                if (!b->toolTip().isEmpty())
                    a->setToolTip(b->toolTip());
                QObject::connect(a, &QAction::triggered, [pane, b] {
                    if (g_raisePane) g_raisePane(pane);
                    b->click();
                });
            }
            const auto checks = pane->findChildren<QCheckBox*>();
            bool csep = false;
            for (QCheckBox* c : checks) {
                if (c->text().trimmed().isEmpty()) continue;
                if (!csep) { menu->addSeparator(); csep = true; }
                QAction* a = menu->addAction(c->text());
                a->setCheckable(true);
                a->setChecked(c->isChecked());
                QObject::connect(a, &QAction::toggled, [c](bool on) {
                    if (c->isChecked() != on) c->setChecked(on);
                });
                QObject::connect(c, &QCheckBox::toggled, [a](bool on) {
                    if (a->isChecked() != on) a->setChecked(on);
                });
            }
        }
    }

    {
        QMenu* view = win.menuBar()->addMenu("View");
        QAction* night = view->addAction("Night mode");
        night->setCheckable(true);
        night->setChecked(nightOn);
        QObject::connect(night, &QAction::toggled, [&, applyNight](bool on) {
            applyNight(on);
            QSettings s("ALL", "TranslationTool");
            s.setValue("app/nightMode", on);
        });
        view->addSeparator();
        QAction* prefs = view->addAction("Settings\u2026");
        prefs->setMenuRole(QAction::PreferencesRole);   // macOS: app menu
        prefs->setShortcut(QKeySequence::Preferences);
        QObject::connect(prefs, &QAction::triggered, [&, applyNight] {
            SettingsDialog dlg(applyNight, &win);
            dlg.exec();
        });
    }

    // Help menu: searchable help + tutorials (and macOS's own
    // Help-search finds every derived menu item)
    HelpWindow* helpWin = nullptr;
    {
        QMenu* helpMenu = win.menuBar()->addMenu("Help");
        QAction* h = helpMenu->addAction("ALL Tool Help && Tutorials\u2026");
        h->setShortcut(QKeySequence::HelpContents);
        QObject::connect(h, &QAction::triggered, [&] {
            if (!helpWin) helpWin = new HelpWindow(&tabs, root, &win);
            helpWin->show();
            helpWin->raise();
            helpWin->activateWindow();
        });
        QAction* wf = helpMenu->addAction("Suggested Workflows…");
        QObject::connect(wf, &QAction::triggered, [&] {
            if (!helpWin) helpWin = new HelpWindow(&tabs, root, &win);
            helpWin->show();
            helpWin->raise();
            helpWin->activateWindow();
            helpWin->openWorkflows();
        });
    }

    // --selftest: suite 38 — the real panes exercised against the
    // real spine, offscreen; nonzero exit on any failure (ctest)
    if (selfTestMode) {
        QStringList log;
        int fails = 0;
        fails += overlay->selfTest(log, root);
        fails += trainerPane->selfTest(log);
        fails += drillsPane->selfTest(log);
        fails += draftPane->selfTest(log);
        fails += manuscriptPane->selfTest(log);
        fails += reviewPane->selfTest(log);
        fails += alignPane->selfTest(log);
        fails += libraryPane->selfTest(log);
        fails += inputPane->selfTest(log);
        fails += goferPane->selfTest(log);
        fails += sanskritPane->selfTest(log);
        fails += proposePane->selfTest(log);
        // Lookup: the extracted stacked search, driver-level
        {
            auto lk = [&](bool ok, const char* what) {
                log << QString("  [%1] Lookup: %2")
                           .arg(ok ? "PASS" : "FAIL").arg(what);
                if (!ok) ++fails;
            };
            const QString h = lookupResultsHtml(spine, refdict, mvp,
                                                whitney, colloq,
                                                "sems can");
            lk(h.contains(QString::fromUtf8("≡")),
               "known term returns HGM equivalents");
            const int hgmAt = h.indexOf(QString::fromUtf8("≡"));
            int refAt = h.indexOf("Chandra");
            if (refAt < 0) refAt = h.indexOf("Hopkins");
            lk(refAt < 0 || hgmAt < refAt,
               "HGM binding layer precedes reference layers");
            const QString hh = lookupResultsHtml(spine, refdict, mvp,
                                                 whitney, colloq,
                                                 "gzigs");
            lk(hh.toLower().contains("honorific"),
               "honorific term carries its badge");
            lk(h.contains("propose:"),
               "propose-to-the-authority action offered");
        }
        // Search: the library's prebuilt line index, driver-level
        {
            auto sk = [&](bool ok, const char* what) {
                log << QString("  [%1] Search: %2")
                           .arg(ok ? "PASS" : "FAIL").arg(what);
                if (!ok) ++fails;
            };
            const QString ix = root + "/library/.index.db";
            if (QFileInfo::exists(ix)) {
                allcore::LibraryIndex li(ix.toStdString());
                auto hits = li.search("sems can", 5);
                sk(!hits.empty(),
                   "library index answers a known query");
            } else {
                sk(true, "library index absent — skipped");
            }
        }
        {
            ApprovalPane ap(root);
            fails += ap.selfTest(log);
        }
        // the help system: chapters + auto-index + search
        {
            HelpWindow hw(&tabs, root, nullptr);
            const bool chapters = hw.chapterCount() >= 16;
            {
                const bool wfHit = hw.hitCount("Suggested Workflows") >= 1 &&
                               hw.openWorkflowsCount() >= 7;
                log << QString("  [%1] Help: Suggested Workflows chapter "
                               "present").arg(wfHit ? "PASS" : "FAIL");
                if (!wfHit) fails++;
            }
            log << QString("  [%1] Help: tutorial chapters for every "
                           "pane (%2)")
                       .arg(chapters ? "PASS" : "FAIL")
                       .arg(hw.chapterCount());
            if (!chapters) ++fails;
            const bool indexed = hw.indexSize() > 150;
            log << QString("  [%1] Help: feature auto-index built "
                           "(%2 entries)")
                       .arg(indexed ? "PASS" : "FAIL")
                       .arg(hw.indexSize());
            if (!indexed) ++fails;
            const bool found = hw.hitCount("Load into overlay") >= 1;
            log << QString("  [%1] Help: search finds a feature by "
                           "name")
                       .arg(found ? "PASS" : "FAIL");
            if (!found) ++fails;
        }
        {
            const bool ok = g_idioms && g_idioms->size() >= 5 &&
                            g_idioms->count("dper na") &&
                            g_idioms->at("dper na").first == "proposed";
            log << QString("  [%1] Idioms: register banked and loaded "
                           "(%2 proposed)")
                       .arg(ok ? "PASS" : "FAIL")
                       .arg(g_idioms ? (int)g_idioms->size() : 0);
            if (!ok) ++fails;
        }
        {
            const bool ok = g_dasSections &&
                            g_dasSections->size() >= 800 &&
                            dasPageFor("khyab pa") > 0 &&
                            dasPageFor("khyab pa") >
                                dasPageFor("ka ba");
            log << QString("  [%1] Das: onset map loaded (%2 "
                           "sections), khyab beyond ka")
                       .arg(ok ? "PASS" : "FAIL")
                       .arg(g_dasSections
                                ? (int)g_dasSections->size() : 0);
            if (!ok) ++fails;
        }
        {
            const bool ok = g_teaching && g_teaching->size() > 1000 &&
                            g_teaching->count("emptiness") &&
                            !g_teaching->at("emptiness").empty() &&
                            g_teaching->at("emptiness")[0].url.contains(
                                "youtube.com");
            const bool tibOk = g_teachingTib &&
                               g_teachingTib->size() > 500 &&
                               g_teachingTib->count("dge bshes");
            log << QString("  [%1] Teaching: spoken-Tibetan tier "
                           "loaded (%2 headwords)")
                       .arg(tibOk ? "PASS" : "FAIL")
                       .arg(g_teachingTib
                                ? (int)g_teachingTib->size() : 0);
            if (!tibOk) ++fails;
            log << QString("  [%1] Teaching: moments index loaded "
                           "(%2 terms)")
                       .arg(ok ? "PASS" : "FAIL")
                       .arg(g_teaching ? (int)g_teaching->size() : 0);
            if (!ok) ++fails;
        }
        {
            const bool cream =
                qApp->styleSheet().contains("#FAF6EE");
            log << QString("  [%1] Night: reading surfaces cream in "
                           "all modes").arg(cream ? "PASS" : "FAIL");
            if (!cream) ++fails;
        }
        // every pane must FIT a small laptop window (the Overlay
        // squeeze bug is a class: any pane demanding more height
        // than ~800px will crush its widgets the same way)
        {
            int worst = 0; QString worstName;
            for (auto& f : flatPanes) {
                const int h = f.w->minimumSizeHint().height();
                log << QString("  [info] fit: %1 needs %2 px")
                           .arg(f.title).arg(h);
                if (h > worst) { worst = h; worstName = f.title; }
            }
            const bool ok = worst <= 800;
            log << QString("  [%1] Fit: every pane fits an 800px "
                           "window (worst: %2 at %3 px)")
                       .arg(ok ? "PASS" : "FAIL")
                       .arg(worstName)
                       .arg(worst);
            if (!ok) ++fails;
        }
        // the menu bar mirrors the GUI: group menus + View + Help
        {
            const auto menus = win.menuBar()->actions();
            bool ok = menus.size() == tabs.count() + 2;
            int overlayActions = 0;
            if (!menus.isEmpty() && menus.first()->menu()) {
                const auto subs = menus.first()->menu()->actions();
                if (!subs.isEmpty() && subs.first()->menu())
                    overlayActions =
                        subs.first()->menu()->actions().size();
            }
            log << QString("  [%1] MenuBar: mirrors the workflow "
                           "groups (%2 menus)")
                       .arg(ok ? "PASS" : "FAIL")
                       .arg(menus.size());
            if (!ok) ++fails;
            const bool rich = overlayActions > 8;
            log << QString("  [%1] MenuBar: Read > Overlay submenu "
                           "carries its actions (%2)")
                       .arg(rich ? "PASS" : "FAIL")
                       .arg(overlayActions);
            if (!rich) ++fails;
        }
        // the authority's tab badge carries the pending count
        {
            bool badge = false;
            for (auto& f : flatPanes)
                if (f.inner->tabText(f.inner->indexOf(f.w))
                        .startsWith("Approval ("))
                    badge = true;
            log << QString("  [%1] Approval: tab badge shows pending "
                           "count").arg(badge ? "PASS" : "FAIL");
            if (!badge) ++fails;
        }
        for (const QString& l : log)
            printf("%s\n", l.toUtf8().constData());
        printf("app selftest: %s (%d failure(s))\n",
               fails ? "FAIL" : "ALL PASS", fails);
        return fails ? 1 : 0;
    }
    // let the window shrink freely: the 15-tab bar must not pin a
    // wide minimum, and the central widget must be allowed to compress
    tabs.tabBar()->setUsesScrollButtons(true);
    tabs.tabBar()->setElideMode(Qt::ElideRight);
    tabs.setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
    win.setMinimumSize(640, 480);
    win.resize(1180, 760);
    win.show();
    const int pasteIx = cliArgs.indexOf("--pasteprobe");
    if (pasteIx >= 0 && pasteIx + 1 < cliArgs.size()) {
        for (auto* b : overlay->findChildren<QPushButton*>())
            if (b->text() == "Load into overlay") {
                for (auto* e : overlay->findChildren<QPlainTextEdit*>()) {
                    if (e->isReadOnly()) continue;   // that's the view
                    e->setPlainText(cliArgs.at(pasteIx + 1));
                    break;
                }
                QElapsedTimer t;
                t.start();
                b->click();
                printf("[paste] loaded in %lld ms\n", t.elapsed());
                for (auto* pv : overlay->findChildren<QPlainTextEdit*>())
                    if (pv->isReadOnly())
                        printf("[paste] view chars=%d\n",
                               (int)pv->toPlainText().size());
                for (auto* lb : overlay->findChildren<QLabel*>()) {
                    const QString t2 = lb->text();
                    if (t2.contains("tokens"))
                        printf("[paste] hint: %s\n",
                               t2.left(120).toUtf8().constData());
                }
                fflush(stdout);
                return 0;
            }
        printf("[paste] BUTTON NOT FOUND\n");
        return 1;
    }
    const int probeIx = cliArgs.indexOf("--openprobe");
    if (probeIx >= 0 && probeIx + 1 < cliArgs.size()) {
        QElapsedTimer t;
        t.start();
        overlay->openFile(cliArgs.at(probeIx + 1));
        printf("[probe] openFile completed in %lld ms\n", t.elapsed());
        for (auto* lb : overlay->findChildren<QLabel*>()) {
            const QString hh = lb->text();
            if (hh.contains("tokens"))
                printf("[probe] hint: %s\n",
                       hh.toUtf8().constData());
        }
        fflush(stdout);
        return 0;
    }
    if (shotMode) {
        const QString outDir = cliArgs.at(shotIx + 1);
        QDir().mkpath(outDir);
        // open a real text in the Overlay so the flagship pane shows
        // its shading, not an empty editor (the Diamond Cutter)
        // Only the first screenful is visible in a capture, and the
        // offscreen platform lacks lazy text layout (every char-format
        // change re-shapes the whole Tibetan document through
        // HarfBuzz — quadratic). So load just the opening of the
        // Diamond Cutter: identical screenshot, instant.
        const QString demo = root + "/library/acip_release6/S0134I_inc_t.txt";
        printf("[shot] opening demo text…\n"); fflush(stdout);
        QFile df(demo);
        if (df.open(QIODevice::ReadOnly)) {
            QString excerpt = QString::fromUtf8(df.read(6000));
            // demo cleanliness: drop the editorial apparatus (bracket
            // comments, parens, page/section markers) so the capture
            // shows the reading experience, not the input-format
            // housekeeping the honest markers would flag
            excerpt.remove(QRegularExpression("\\[[^\\]]*\\]"));
            excerpt.remove(QRegularExpression("@#*[0-9A-Za-z.]*"));
            excerpt.remove(QChar('('));
            excerpt.remove(QChar(')'));
            excerpt.truncate(excerpt.lastIndexOf('\n'));
            // keep pure-ASCII lines only (drop stray unicode
            // annotations); the raw wylie goes in as-is — the Overlay
            // now detects the transliteration itself
            QStringList clean;
            for (const QString& ln : excerpt.split('\n')) {
                bool ascii = !ln.trimmed().isEmpty();
                for (QChar ch : ln) ascii &= ch.unicode() < 128;
                if (ascii) clean << ln;
            }
            excerpt = clean.join('\n');
            const QString tmp = QDir::tempPath() + "/S0134I_inc_t.txt";
            QFile tf(tmp);
            if (tf.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
                tf.write(excerpt.toUtf8());
                tf.close();
                overlay->openFile(tmp);
                draftPane->demo(excerpt);
            }
        }
        printf("[shot] demo open; %d tabs\n", tabs.count()); fflush(stdout);
        for (size_t i = 0; i < flatPanes.size(); ++i) {
            printf("[shot] tab %zu %s\n", i,
                   flatPanes[i].title.toUtf8().constData());
            fflush(stdout);
            if (g_raisePane) g_raisePane(flatPanes[i].w);
            QCoreApplication::processEvents();
            QString name = flatPanes[i].title.toLower();
            name.replace(QRegularExpression("[^a-z]+"), "-");
            name.remove(QRegularExpression("-+$"));
            const QString file = QString("%1/%2-%3.png")
                                     .arg(outDir)
                                     .arg(i + 1, 2, 10, QChar('0'))
                                     .arg(name);
            if (!tabs.grab().save(file))
                fprintf(stderr, "screenshot failed: %s\n",
                        file.toUtf8().constData());
            else
                printf("wrote %s\n", file.toUtf8().constData());
        }
        return 0;
    }
    // ---- lifecycle log (2026-08-10): an intermittent clean-exit-0
    // at startup was observed on staged Release builds — no crash
    // report, no output, unreproducible under observation. These
    // three lines make any future silent death explain itself in
    // ~/Library/Logs/ALLTranslationTool-lifecycle.log.
    {
        static const QString lifePath =
            QDir::home().filePath(
                "Library/Logs/ALLTranslationTool-lifecycle.log");
        auto lifeLog = [](const QString& msg) {
            QFile f(lifePath);
            if (f.open(QIODevice::Append)) {
                f.write((QDateTime::currentDateTime()
                             .toString(Qt::ISODateWithMs) +
                         "  " + msg + "\n")
                            .toUtf8());
            }
        };
        lifeLog(QString("startup ok \u2014 entering event loop (pid %1)")
                    .arg(QCoreApplication::applicationPid()));
        QObject::connect(&app, &QGuiApplication::lastWindowClosed,
                         [lifeLog] { lifeLog("lastWindowClosed fired"); });
        QObject::connect(&app, &QCoreApplication::aboutToQuit,
                         [lifeLog, &win] {
                             lifeLog(QString("aboutToQuit \u2014 window "
                                             "visible=%1 active=%2")
                                         .arg(win.isVisible())
                                         .arg(QApplication::activeWindow()
                                                  != nullptr));
                         });
    }
    return app.exec();
}
