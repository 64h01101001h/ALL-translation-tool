// ALL Translation Tool — Qt 6 shell.
// Pane 1 "Lookup": headword search over the spine (walking skeleton kept).
// Pane 2 "Analysis": passage → engine pre-pass → Claude API (streamed) →
//                    rendered report → machine QC panel.
#include <QApplication>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QNetworkAccessManager>
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
#include <QDirIterator>

#include <functional>
#include <QDateTime>

#include "allcore/analysis.h"
#include "allcore/drills.h"
#include "allcore/engines.h"
#include "allcore/gofer.h"
#include "allcore/lattice.h"
#include "allcore/particles.h"
#include "allcore/progress.h"
#include "allcore/qc.h"
#include "allcore/reader.h"
#include "allcore/refdict.h"
#include "allcore/sanskrit.h"
#include "allcore/spellcheck.h"
#include "allcore/spine.h"
#include "allcore/outline.h"
#include "allcore/verse.h"
#include "allcore/wilsonparse.h"
#include "allcore/terminology.h"
#include "allcore/tibexport.h"

// What an entry card shows — the Overlay's display toggles feed this; other
// panes use the defaults (which match the historical behavior).
struct EntryDisplay {
    bool phonetics = true;
    bool glosses = true;
    bool sanskrit = true;
    bool hopkins = false;
};

static QString entryHtml(const allcore::Entry& e,
                         const EntryDisplay& d = EntryDisplay{}) {
    QString h;
    h += "<div style='margin-bottom:14px'>";
    h += "<span style='font-size:22px'>" +
         QString::fromStdString(e.tibetan).toHtmlEscaped() + "</span> ";
    h += "<b>" + QString::fromStdString(e.wylie).toHtmlEscaped() + "</b>";
    if (!e.tibetan_source.empty())
        h += " <i style='color:#888'>[generated script]</i>";
    if (d.phonetics && !e.pronunciation.empty()) {
        h += "<br>pron: " + QString::fromStdString(e.pronunciation).toHtmlEscaped();
        if (e.pronunciation_card_attested) h += " ⟪card⟫";
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

static QWidget* makeLookupPane(allcore::Spine& spine, allcore::RefDict* ref) {
    auto* pane = new QWidget;
    auto* layout = new QVBoxLayout(pane);
    auto* box = new QLineEdit;
    box->setPlaceholderText("wylie · Tibetan · ACIP headword…");
    auto* results = new QTextBrowser;
    layout->addWidget(box);
    layout->addWidget(results);
    QObject::connect(box, &QLineEdit::returnPressed, [&spine, ref, box, results] {
        const std::string raw = box->text().trimmed().toStdString();
        if (raw.empty()) return;
        auto entries = spine.lookup(raw);
        if (entries.empty()) entries = spine.headwordSearch('"' + raw + '"', 10);
        QString h;
        if (entries.empty()) h = "<i>no HGM match</i>";
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
        results->setHtml(h);
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
    void openFile(const QString& fn) {
        QFile f(fn);
        if (!f.open(QIODevice::ReadOnly)) return;
        input_->setPlainText(QString::fromUtf8(f.readAll()));
        loadDoc();
        auto info = allcore::decodeAcipFilename(fn.toStdString());
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
                allcore::RefDict* ref, allcore::Progress* progress)
        : spine_(spine), checker_(checker), ref_(ref), progress_(progress),
          index_(spine) {
        auto* outer = new QVBoxLayout(this);
        auto* split = new QSplitter(Qt::Horizontal);

        auto* left = new QWidget;
        auto* ll = new QVBoxLayout(left);
        ll->addWidget(new QLabel("<b>Document (ACIP)</b>"));
        input_ = new QPlainTextEdit;
        input_->setPlaceholderText("Paste an ACIP document…");
        ll->addWidget(input_, 1);
        auto* open = new QPushButton("Open ACIP file…");
        ll->addWidget(open);
        auto* load = new QPushButton("Load into overlay");
        ll->addWidget(load);
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
            settings.value("overlay/scriptMode", 0).toInt());
        scriptRow->addWidget(scriptMode_, 1);
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
        hint_ = new QLabel("Click a shaded word to see its context; click again "
                           "to cycle outward through the containing phrases.");
        hint_->setWordWrap(true);
        ll->addWidget(hint_);

        auto* right = new QSplitter(Qt::Vertical);
        view_ = new QTextEdit;
        view_->setReadOnly(true);
        view_->setFontPointSize(15);
        context_ = new QTextBrowser;
        right->addWidget(view_);
        right->addWidget(context_);
        right->setStretchFactor(0, 3);
        right->setStretchFactor(1, 2);

        split->addWidget(left);
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
        connect(view_, &QTextEdit::cursorPositionChanged, [this] { onClick(); });
    }

private:
    static QColor washColor(int depth) {
        switch (depth) {
            case 0: return QColor(0, 0, 0, 0);
            case 1: return QColor(0xEE, 0xED, 0xFE);
            case 2: return QColor(0xCE, 0xCB, 0xF6);
            default: return QColor(0xAF, 0xA9, 0xEC);
        }
    }

    void loadDoc() {
        doc_ = allcore::buildOverlay(spine_, index_,
                                     input_->toPlainText().toStdString());
        tokBeg_.clear();
        tokEnd_.clear();
        const int mode = scriptMode_->currentIndex();   // 0 script, 1 ACIP, 2 wylie
        QString text;
        for (size_t i = 0; i < doc_.tokens.size(); ++i) {
            QString disp;
            if (mode == 0) {
                // ACIP → EWTS → unicode through the battery-proven ports;
                // failures render as ⟨wylie⟩ markers, never guessed
                auto [u, ok] =
                    allcore::wylieToUnicode(allcore::acipToEwts(doc_.tokens[i]));
                disp = QString::fromStdString(u);
                if (disp.isEmpty()) disp = QString::fromStdString(doc_.tokens[i]);
            } else if (mode == 2) {
                disp = QString::fromStdString(allcore::acipToEwts(doc_.tokens[i]));
            } else {
                disp = QString::fromStdString(doc_.tokens[i]);
            }
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
        }
        loading_ = true;
        view_->setPlainText(text);
        auto depth = doc_.coverDepth(3);
        int spellFlags = 0, agreeFlags = 0;
        QTextCursor cur(view_->document());
        for (size_t i = 0; i < doc_.tokens.size(); ++i) {
            QTextCharFormat fmt;
            bool touched = false;
            if (depth[i] > 0) {
                fmt.setBackground(washColor(depth[i]));
                touched = true;
                // provisional innermost gloss → dashed amber underline
                auto at = doc_.spansAt((int)i);
                if (!at.empty() &&
                    doc_.entries[doc_.spans[at.front()].entry_ix].provisional()) {
                    fmt.setUnderlineStyle(QTextCharFormat::DashUnderline);
                    fmt.setUnderlineColor(QColor(0xBA, 0x75, 0x17));
                }
            }
            // Wilson suffix-agreement diagnostic: dotted amber underline on a
            // variant particle that disagrees with the preceding suffix.
            // Particles inside a longer matched phrase are skipped — the
            // dictionary already vouches for those. (Soft flag: spellcheck red
            // below intentionally overrides it.)
            if (showGrammar_->isChecked() && i > 0 && !doc_.barrier_after[i - 1]) {
                bool inPhrase = false;
                for (int sx : doc_.spansAt((int)i))
                    inPhrase |= (doc_.spans[sx].end - doc_.spans[sx].beg > 1);
                if (!inPhrase) {
                    auto ag = allcore::checkAgreement(doc_.tokens[i - 1],
                                                     doc_.tokens[i]);
                    if (ag.verdict == allcore::Agreement::Disagrees) {
                        fmt.setUnderlineStyle(QTextCharFormat::DotLine);
                        fmt.setUnderlineColor(QColor(0xD9, 0x77, 0x06));
                        touched = true;
                        ++agreeFlags;
                    }
                }
            }
            // spellcheck: red wave underline on illegal syllables (item 5)
            if (checker_ &&
                !checker_->legalWylie(allcore::acipToEwts(doc_.tokens[i]))) {
                fmt.setUnderlineStyle(QTextCharFormat::WaveUnderline);
                fmt.setUnderlineColor(QColor(0xE2, 0x4B, 0x4A));
                touched = true;
                ++spellFlags;
            }
            if (!touched) continue;
            cur.setPosition(tokBeg_[i]);
            cur.setPosition(tokEnd_[i], QTextCursor::KeepAnchor);
            cur.setCharFormat(fmt);
        }
        loading_ = false;
        lastTok_ = -1;
        hint_->setText(QString("%1 tokens · %2 spans · %3 entries · %4 spelling "
                               "flag(s) · %5 particle-agreement flag(s) — click "
                               "any shaded word.")
                           .arg(doc_.tokens.size())
                           .arg(doc_.spans.size())
                           .arg(doc_.entries.size())
                           .arg(spellFlags)
                           .arg(agreeFlags));
        context_->clear();
    }

    int tokenAt(int charPos) const {
        for (size_t i = 0; i < tokBeg_.size(); ++i)
            if (charPos >= tokBeg_[i] && charPos <= tokEnd_[i]) return (int)i;
        return -1;
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
                    QString("<b>%1</b> — grammatical particle<br>%2<br>"
                            "<small style='color:#555'>paradigm: %3</small>")
                        .arg(QString::fromUtf8(p->wylie).toHtmlEscaped(),
                             QString::fromUtf8(p->function).toHtmlEscaped(),
                             QString::fromUtf8(p->group).toHtmlEscaped()) +
                    agreementHtml(tok));
            } else {
                context_->setHtml("<i>no dictionary span here</i>");
            }
            return;
        }
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
        sel.format.setBackground(QColor(0x7F, 0x77, 0xDD, 110));
        view_->setExtraSelections({sel});

        // breadcrumb of the nesting chain + entry + corpus count
        QString h = "<div style='color:#555;font-size:12px'>nesting: ";
        for (size_t i = 0; i < at.size(); ++i) {
            const auto& s = doc_.spans[at[i]];
            QString w = QString::fromStdString(doc_.entries[s.entry_ix].wylie).toHtmlEscaped();
            h += (i ? " ‹ " : "");
            h += (int(i) == cycle_ ? "<b>" + w + "</b>" : w);
        }
        h += QString(" &nbsp;(click again to cycle)</div><hr>");
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
    QTextEdit* view_ = nullptr;
    QTextBrowser* context_ = nullptr;
    QLabel* hint_ = nullptr;
    QComboBox* scriptMode_ = nullptr;
    QCheckBox* showPhon_ = nullptr;
    QCheckBox* showGloss_ = nullptr;
    QCheckBox* showCorpus_ = nullptr;
    QCheckBox* showGrammar_ = nullptr;
    QCheckBox* showSanskrit_ = nullptr;
    QCheckBox* showHopkins_ = nullptr;
    QCheckBox* showRefs_ = nullptr;
    bool loading_ = false;
    int lastTok_ = -1;
    int cycle_ = 0;
};

// ---- Search pane: Gofer grammar over the corpus ----------------------------
static QWidget* makeSearchPane(allcore::Spine& spine) {
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
    layout->addLayout(row);

    auto* row2 = new QHBoxLayout;
    auto* dirBox = new QLineEdit;
    dirBox->setPlaceholderText(
        "…or a folder of raw ACIP files (leave empty to search the corpus)");
    row2->addWidget(dirBox, 1);
    layout->addLayout(row2);

    auto* results = new QTextBrowser;
    layout->addWidget(results);
    auto* status = new QLabel(
        "Terms are phrases (quote or just type). OR = either. NEAR/N = both "
        "within N lines of the same source. Parentheses group.");
    status->setWordWrap(true);
    layout->addWidget(status);

    auto runSearch = [&spine, box, courseBox, dirBox, results, status] {
        const std::string q = box->text().trimmed().toStdString();
        if (q.empty()) return;
        const QString dir = dirBox->text().trimmed();
        if (!dir.isEmpty()) {
            try {
                auto hits = allcore::goferSearchFiles(dir.toStdString(), q, 60);
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
static QWidget* makeConvertPane() {
    auto* pane = new QWidget;
    auto* layout = new QVBoxLayout(pane);
    layout->addWidget(new QLabel(
        "<b>Input</b> (ACIP, EWTS wylie, or Sanskrit IAST — auto-detected)"));
    auto* input = new QPlainTextEdit;
    input->setPlaceholderText(
        "BSOD NAMS   ·   bsod nams   ·   SNGA DRO'I KA BA …   ·   pramāṇa");
    layout->addWidget(input, 1);
    auto* out = new QTextBrowser;
    layout->addWidget(out, 2);

    auto convert = [input, out] {
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
        out->setHtml(h);
    };
    QObject::connect(input, &QPlainTextEdit::textChanged, convert);
    return pane;
}

// ---- Trainer pane: progressive-reveal reading tutor (docs/TRAINER_DESIGN.md)
class TrainerPane : public QWidget {
public:
    explicit TrainerPane(allcore::Spine& spine)
        : spine_(spine), index_(spine) {
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

private:
    void loadDoc() {
        doc_ = allcore::buildOverlay(spine_, index_,
                                     input_->toPlainText().toStdString());
        clauses_ = allcore::refineClauses(
            doc_, allcore::splitClauses(doc_.tokens, doc_.barrier_after));
        selClause_ = selChunk_ = -1;
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
                auto [u, ok] = allcore::wylieToUnicode(allcore::acipToEwts(tok));
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
        QString h;
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
                auto parses = allcore::wilsonParse(spine_, doc_, {cl});
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
                    wylie += allcore::acipToEwts(doc_.tokens[t]);
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
    allcore::HeadwordIndex index_;
    allcore::OverlayDoc doc_;
    std::vector<allcore::Clause> clauses_;
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
                         "Vocabulary review (SRS)"});
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
            newDrill();
        });
        QObject::connect(script_, &QCheckBox::toggled, [this] { renderQuestion(); });
        refreshStats();
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
    DraftPane(allcore::Spine& spine, allcore::Progress* progress)
        : spine_(spine), progress_(progress), index_(spine) {
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
        auto* outlineBtn = new QPushButton("Extract outline (sa bcad)");
        srcCol->addWidget(outlineBtn);
        auto* verseBtn = new QPushButton("Verse meter");
        srcCol->addWidget(verseBtn);
        tl->addLayout(srcCol, 1);
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
        aiBtn_ = new QPushButton("AI back-check (API, labeled AI)");
        if (qgetenv("ANTHROPIC_API_KEY").isEmpty()) {
            aiBtn_->setEnabled(false);
            aiBtn_->setToolTip("ANTHROPIC_API_KEY is not set");
        }
        draftCol->addWidget(aiBtn_);
        bl->addLayout(draftCol, 1);
        report_ = new QTextBrowser;
        bl->addWidget(report_, 1);
        split->addWidget(bottom);
        layout->addWidget(split, 1);

        QObject::connect(loadBtn, &QPushButton::clicked, [this] { load(); });
        QObject::connect(outlineBtn, &QPushButton::clicked,
                         [this] { outline(); });
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
                             if (s.startsWith("t:"))
                                 showConcordance(s.mid(2).toStdString());
                             else if (s == "back" && lastClause_ >= 0)
                                 showAnchors(lastClause_);
                         });
    }

private:
    void load() {
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
        QString h = QString("<div style='color:#777'>anchors — clause %1"
                            "</div><hr>").arg(ci + 1);
        for (int ix : termSpans(clauses_[ci])) {
            const auto& e = doc_.entries[doc_.spans[ix].entry_ix];
            QString w = QString::fromStdString(e.wylie).toHtmlEscaped();
            h += "<div style='margin:4px 0'><a href='t:" + w +
                 "'><b>" + w + "</b></a>";
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

    allcore::Spine& spine_;
    allcore::Progress* progress_ = nullptr;
    allcore::HeadwordIndex index_;
    allcore::OverlayDoc doc_;
    std::vector<allcore::Clause> clauses_;
    int lastClause_ = -1;
    QPlainTextEdit* source_ = nullptr;
    QPlainTextEdit* draft_ = nullptr;
    QTextBrowser* clauseView_ = nullptr;
    QTextBrowser* anchors_ = nullptr;
    QTextBrowser* report_ = nullptr;
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
    LibraryPane(const QString& root, std::function<void(const QString&)> open)
        : libRoot_(root + "/library"), open_(std::move(open)) {
        QDir().mkpath(libRoot_);
        QDir().mkpath(libRoot_ + "/my_materials");
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
        row->addWidget(installBtn);
        row->addWidget(importBtn);
        row->addStretch();
        layout->addLayout(row);

        auto* split = new QSplitter(Qt::Horizontal);
        model_ = new QFileSystemModel(this);
        model_->setRootPath(libRoot_);
        tree_ = new QTreeView;
        tree_->setModel(model_);
        tree_->setRootIndex(model_->index(libRoot_));
        tree_->setSortingEnabled(true);
        tree_->sortByColumn(0, Qt::AscendingOrder);
        tree_->setColumnWidth(0, 260);
        split->addWidget(tree_);
        info_ = new QTextBrowser;
        info_->setOpenExternalLinks(true);
        split->addWidget(info_);
        split->setStretchFactor(0, 2);
        split->setStretchFactor(1, 1);
        layout->addWidget(split, 1);

        connect(installBtn, &QPushButton::clicked, [this] { installZip(); });
        connect(importBtn, &QPushButton::clicked, [this] { importFiles(); });
        connect(tree_->selectionModel(), &QItemSelectionModel::currentChanged,
                [this](const QModelIndex& ix, const QModelIndex&) {
                    showInfo(model_->filePath(ix));
                });
        connect(tree_, &QTreeView::doubleClicked, [this](const QModelIndex& ix) {
            const QString p = model_->filePath(ix);
            if (QFileInfo(p).isFile() && open_) open_(p);
        });
    }

private:
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

    void showInfo(const QString& path) {
        const QFileInfo fi(path);
        if (!fi.isFile()) { info_->clear(); return; }
        QString h = "<b>" + fi.fileName().toHtmlEscaped() + "</b><br>" +
                    QString("%1 KB · %2<br>")
                        .arg(fi.size() / 1024)
                        .arg(fi.lastModified().toString("yyyy-MM-dd"));
        auto acip = allcore::decodeAcipFilename(path.toStdString());
        if (acip.recognized) {
            h += "<div style='margin-top:4px;color:#4A3FBF'>" +
                 QString::fromStdString(acip.collection).toHtmlEscaped() +
                 " · text " + QString::fromStdString(acip.number);
            if (!acip.part.empty())
                h += " part " + QString::fromStdString(acip.part);
            h += "<br>" + QString::fromStdString(acip.status).toHtmlEscaped() +
                 (acip.status.empty() ? "" : " · ") +
                 QString::fromStdString(acip.language).toHtmlEscaped();
            if (acip.incomplete) h += " · <b>INCOMPLETE</b>";
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

    QString libRoot_;
    std::function<void(const QString&)> open_;
    QFileSystemModel* model_ = nullptr;
    QTreeView* tree_ = nullptr;
    QTextBrowser* info_ = nullptr;
};

}  // namespace

// Find the project data root: works from terminal (cwd) AND from Finder
// (walk up from the .app bundle looking for build/hgm_spine_v27_2.db).
static QString findDataRoot() {
    QStringList cands = {QDir::currentPath()};
    QDir d(QCoreApplication::applicationDirPath());
    for (int i = 0; i < 7; ++i) {
        cands << d.absolutePath();
        if (!d.cdUp()) break;
    }
    for (const auto& c : cands)
        if (QFileInfo::exists(c + "/build/hgm_spine_v27_2.db")) return c;
    return QDir::currentPath();
}

int main(int argc, char** argv) {
    QApplication app(argc, argv);
    const QString root = findDataRoot();
    QString dbPath = argc > 1 ? argv[1] : root + "/build/hgm_spine_v27_2.db";
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

    // the learner's own progress/SRS data — local file, optional
    allcore::Progress* progress = nullptr;
    try {
        static allcore::Progress pr((root + "/progress.db").toStdString());
        progress = &pr;
    } catch (const std::exception&) {}

    QTabWidget tabs;
    tabs.setWindowTitle(
        QString("ALL Translation Tool — HGM v%1")
            .arg(QString::fromStdString(spine.metaValue("release_version"))));
    auto* overlay = new OverlayPane(spine, checker, refdict, progress);
    tabs.addTab(overlay, "Overlay");
    tabs.addTab(new AnalysisPane(spine, tplPath, root + "/analyses"), "Analysis");
    tabs.addTab(new TrainerPane(spine), "Trainer");
    tabs.addTab(new DrillsPane(spine, progress), "Drills");
    tabs.addTab(new DraftPane(spine, progress), "Draft");
    tabs.addTab(new LibraryPane(root,
                                [&tabs, overlay](const QString& path) {
                                    overlay->openFile(path);
                                    tabs.setCurrentIndex(0);
                                }),
                "Library");
    tabs.addTab(makeSearchPane(spine), "Search");
    tabs.addTab(makeConvertPane(), "Convert");
    tabs.addTab(makeLookupPane(spine, refdict), "Lookup");
    tabs.resize(1180, 760);
    tabs.show();
    return app.exec();
}
