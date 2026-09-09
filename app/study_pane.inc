// app/study_pane.inc — the Study pane (analysis suite, batch 5, F1 shell —
// 2026-09-09). One place to measure a text, find what repeats in it, see
// which other texts share material with it, and list the people, texts and
// dates it names — four pages of one pane in the Research group. This file
// is the shell: page switcher, ribbon (STUDY · SCOPE · REPORT), the scope
// label built once at the pane boundary through allcore::textspan (one
// tokenizer, one script routing, one folio walk, one apparatus policy), the
// status line, hooks, reports and the selftest. The four pages arrive with
// their engines (F2 Passages, F5 Statistics, F4 Similarity, F6 Entities);
// until then each page says plainly what it is and what it does not yet do.
// Included from main.cpp after apply_patch.inc (it uses cmp::readText,
// saveOrWarn, docprops, RibbonBar, sess::remember, ux::sourceBadge).
//
// Honesty rules this pane enforces: Study measures, counts and matches; it
// never composes English and never proposes a reading (D2: no hgm_gloss is
// rendered here — meanings are one click away in Lookup). Every figure comes
// from the syllables produced once here, with the lines that could not be
// converted and the syllables refused for editorial marks counted and named.

class StudyPane : public QWidget {
public:
    // What every page works on: the text as read, converted ONCE at this
    // boundary, with its refusals travelling alongside.
    struct Doc {
        QString name, text, path, encoding = "UTF-8";
        std::string wylie;                                  // toEwtsDocument
        std::vector<allcore::textspan::Syllable> syl;      // under the current apparatus policy
        allcore::Provenance prov;
        int unconvertedLines = 0; std::vector<int> unconvertedLineNumbers;
        int folios = 0, lines = 0, shads = 0;
        std::pair<std::string, std::string> folioRange;
        bool loaded = false;
    };

    class Page : public QWidget {
    public:
        explicit Page(const QString& title, const QString& what, const QString& notYet) : title_(title) {
            auto* v = new QVBoxLayout(this); v->setContentsMargins(6, 4, 6, 4); v->setSpacing(6);
            head_ = new QLabel(ux::sourceBadge(ux::Epistemic::Machine) + "<b>" + title + "</b> — " + what); head_->setWordWrap(true); head_->setTextFormat(Qt::RichText); v->addWidget(head_);
            body_ = new QLabel(notYet); body_->setWordWrap(true); ux::themedStyle(body_, [] { return QString("color:%1;")
            .arg(ux::chromeMuted()); }); v->addWidget(body_);
            v->addStretch(1);
        }
        virtual ~Page() = default;
        const QString& title() const { return title_; }
        // Re-run the page over the document. The shell pages have nothing to run yet and say so.
        virtual QString run(const Doc& d) {
            if (!d.loaded) return title_ + ": no text is being studied — choose Text… or Use the front document.";
            return QString("%1: %2 syllables read from %3; this page's engine is not in this build yet, so there is nothing to show — no number here is a placeholder.").arg(title_).arg((qulonglong)d.syl.size(), 0, 10).arg(d.name);
        }
        virtual QString report(const Doc& d, const QString& /*kind*/) { return run(d) + "\n"; }
        virtual QStringList reportFilters() const { return {"Markdown (*.md)"}; }
    protected:
        QString title_; QLabel* head_ = nullptr; QLabel* body_ = nullptr;
    };

    StudyPane(const QString& root, allcore::Spine& spine, QWidget* parent = nullptr) : QWidget(parent), root_(root) {
        (void)spine;   // the pages take the spine when their engines land (F5 coverage columns, F6 banks)
        auto* outer = new QVBoxLayout(this); outer->setContentsMargins(0, 0, 0, 0); outer->setSpacing(0);
        // ---- ribbon: three compact groups
        auto* ribbon = new RibbonBar;
        auto* gStudy = ribbon->group("STUDY");
        auto* gScope = ribbon->group("SCOPE");
        auto* gRep = ribbon->group("REPORT");
        auto btn = [&](const QString& label, const QString& tip, std::function<void()> fn) { auto* b = new QPushButton(label); b->setToolTip(tip); b->setStyleSheet("padding:1px 6px;"); connect(b, &QPushButton::clicked, this, std::move(fn)); return b; };
        frontBtn_ = btn("Use the front document", "Study the text open in front (Overlay document, Draft or Manuscript), with the encoding it was opened in", [] { if (g_studyFrontDoc) g_studyFrontDoc(); });
        gStudy->addColumn({btn("Text…", "Pick a text file to study (read as UTF-8; line endings detected; binary refused)", [this] { pickText(); }), frontBtn_,
                           btn("Refresh", "Re-run the current page against the current text", [this] { rerun(); })});
        scope_ = new QLabel("Studying: nothing yet"); scope_->setWordWrap(true); scope_->setMinimumWidth(240); scope_->setMaximumWidth(360); ux::themedStyle(scope_, [] { return QString("color:%1;")
            .arg(ux::chromeInk()); });
        apparatus_ = new QComboBox; apparatus_->addItems({"Strip the marked span", "Keep the letters inside the marks", "Leave the text as it stands"});
        apparatus_->setToolTip("Editorial marks: [ ] { } ⟨ ⟩ spans are stripped before counting (default); a mark inside a syllable refuses that syllable rather than truncating it. ( ) is always kept and counted.");
        auto* appRow = new QWidget; auto* al = new QHBoxLayout(appRow); al->setContentsMargins(0, 0, 0, 0); al->addWidget(new QLabel("Editorial marks:")); al->addWidget(apparatus_);
        gScope->addColumn({scope_, appRow});
        connect(apparatus_, qOverload<int>(&QComboBox::currentIndexChanged), this, [this](int) { retokenize(); rerun(); });
        gRep->addColumn({btn("Save report…", "The current page's report as Markdown or CSV", [this] { saveReport(); }), btn("Copy", "The current page's report to the clipboard", [this] { copyReport(); })});
        ribbon->finish(); ribbon->attachTo(this);
        // ---- banner
        auto* banner = new QLabel(ux::sourceBadge(ux::Epistemic::Machine) + "Study measures, counts and matches. It never composes English and never proposes a reading. For what a term means, send it to the Lookup pane.");
        banner->setTextFormat(Qt::RichText); banner->setWordWrap(true); ux::themedStyle(banner, [] { return QString("padding:4px 8px; color:%1; background:%2; border-bottom:1px solid %3;")
            .arg(ux::chromeInk()).arg(ux::chromePlaque()).arg(ux::chromeRule()); }); outer->addWidget(banner);
        // ---- page switcher
        auto* switcher = new QHBoxLayout; switcher->setContentsMargins(6, 3, 6, 0); switcher->setSpacing(4);
        pageBtns_ = new QButtonGroup(this);
        auto pageBtn = [&](const QString& label, int id) { auto* b = new QToolButton; b->setText(label); b->setCheckable(true); b->setAutoRaise(true); pageBtns_->addButton(b, id); switcher->addWidget(b); return b; };
        pageBtn("Statistics", 0)->setChecked(true); pageBtn("Passages", 1); pageBtn("Similarity", 2); pageBtn("Entities", 3);
        switcher->addStretch(1); outer->addLayout(switcher);
        pages_ = new QStackedWidget; outer->addWidget(pages_, 1);
        connect(pageBtns_, &QButtonGroup::idClicked, this, [this](int id) { if (id < pages_->count()) { pages_->setCurrentIndex(id); rerun(); } });
        addPage(new Page("Statistics", "counts of the text as written: syllables, types, once-only forms, unit lengths, particles, trends, nearby words, repeating phrases, distinctive terms.", "Arrives with the statistics engine (batch 5, F5). Nothing here is a reading."));
        addPage(new Page("Passages", "passages this text repeats inside itself or shares with other texts — shared wording only, every run re-checked syllable by syllable.", "Arrives with the passages engine (batch 5, F2). It will not detect paraphrase or allusion."));
        addPage(new Page("Similarity", "which other texts share material with this one, ranked, with the shared syllables verified.", "Arrives after Passages (batch 5, F4)."));
        addPage(new Page("Entities", "the people, texts and dates this text names, as candidates for a human to confirm — never places.", "Arrives last (batch 5, F6)."));
        // ---- refusal strip + status
        refusals_ = new QLabel; refusals_->setWordWrap(true); ux::themedStyle(refusals_, [] { return QString("color:%1; padding:2px 8px;")
            .arg(ux::chromeError()); }); refusals_->setVisible(false); outer->addWidget(refusals_);
        status_ = new QLabel(ux::sourceBadge(ux::Epistemic::Machine) + "Choose a text to study."); status_->setTextFormat(Qt::RichText); status_->setWordWrap(true); ux::themedStyle(status_, [] { return QString("padding:3px 8px; color:%1; border-top:1px solid %2;")
            .arg(ux::chromeInk()).arg(ux::chromeRule()); }); outer->addWidget(status_);
        setFrontAvailable(false);
    }

    // ---- API
    void setText(const QString& name, const QString& text, const QString& path, const QString& encoding = "UTF-8") {
        doc_ = Doc(); doc_.name = name; doc_.text = text; doc_.path = path; doc_.encoding = encoding; doc_.loaded = true;
        retokenize(); rerun();
    }
    bool openPath(const QString& path) {
        const auto L = cmp::readText(path);
        if (!L.ok) { say("Could not read " + path); return false; }
        if (L.binary) { say(QFileInfo(path).fileName() + " is binary and cannot be studied."); return false; }
        setText(QFileInfo(path).fileName(), L.text, path, "UTF-8");
        return true;
    }
    void showPage(int id) { if (id < pages_->count()) { pages_->setCurrentIndex(id); if (auto* b = pageBtns_->button(id)) b->setChecked(true); rerun(); } }
    int currentPage() const { return pages_->currentIndex(); }
    QLabel* statusLabel() const { return status_; }
    const Doc& doc() const { return doc_; }
    void setFrontAvailable(bool on) { frontBtn_->setEnabled(on); frontBtn_->setToolTip(on ? "Study the text open in front (Overlay document, Draft or Manuscript), with the encoding it was opened in" : "no document is open"); }
    allcore::textspan::Apparatus apparatusMode() const { return apparatus_->currentIndex() == 1 ? allcore::textspan::Apparatus::KeepInner : apparatus_->currentIndex() == 2 ? allcore::textspan::Apparatus::AsWritten : allcore::textspan::Apparatus::StripSpan; }
    QString scopeText() const { return scope_->text(); }
    QString refusalText() const { return refusals_->text(); }
    Page* page(int i) const { return i >= 0 && i < (int)pageObjs_.size() ? pageObjs_[i] : nullptr; }

    // The page's report as text; kind is the filter picked in Save report… (routed by name, not suffix).
    QString report(const QString& kind = "Markdown (*.md)") {
        Page* p = page(currentPage()); if (!p) return {};
        return QString::fromStdString(allcore::provenanceBlock(doc_.prov)) + "\n" + p->report(doc_, kind);
    }
    bool saveReport() {
        Page* p = page(currentPage()); if (!p) return false;
        if (!doc_.loaded) { say("Nothing to save: choose a text to study first."); return false; }
        QString sel;
        const QString fn = safeGetSaveFileName(this, "Save study report", (doc_.path.isEmpty() ? QDir::homePath() : QFileInfo(doc_.path).path()) + "/" + (doc_.path.isEmpty() ? QString("study") : QFileInfo(doc_.path).completeBaseName()) + "-" + p->title().toLower() + ".md", p->reportFilters().join(";;"), &sel);
        if (fn.isEmpty()) { say("Report not saved."); return false; }
        if (!saveOrWarn(this, fn, report(sel.isEmpty() ? p->reportFilters().first() : sel).toUtf8(), "the study report")) return false;
        say("Report saved: " + fn); return true;
    }
    void copyReport() { if (!doc_.loaded) { say("Nothing to copy: choose a text to study first."); return; } QApplication::clipboard()->setText(report()); say("Report copied to the clipboard."); }

    // ---- selftest (F1 shell: tests 4–8 of the spec; page-specific checks arrive with the pages)
    int selfTest(QStringList& log) {
        int fails = 0;
        auto check = [&](bool ok, const char* what) { log << QString("  [%1] Study: %2").arg(ok ? "PASS" : "FAIL").arg(what); if (!ok) ++fails; };
        const QString fixture = "@001A\nSANGS RGYAS DANG BYANG CHUB SEMS DPA' THAMS CAD LA PHYAG 'TSAL LO,,\nCHOS THAMS CAD NI STONG PA NYID DO,,\n@001B\nCHO[S] KYI [SKU] NYID\n";
        setText("fixture", fixture, "");
        check(doc_.loaded && doc_.syl.size() > 10 && doc_.folios == 2, "setText converts once at the boundary: syllables and folios counted");
        for (int i = 0; i < 4; ++i) { showPage(i); check(!status_->text().isEmpty() && pages_->currentIndex() == i, "page renders with a status line"); }
        check(!saveReport() && status_->text().contains("not saved"), "Save report… returns false under the harness with a status line, no modal");
        const QString before = refusals_->text();
        apparatus_->setCurrentIndex(1);   // keep the letters inside the marks
        check(refusals_->text() != before && !refusals_->text().contains("editorial mark inside"), "switching Editorial marks re-runs and changes the refused count on the bracket fixture");
        apparatus_->setCurrentIndex(0);
        check(refusals_->text().contains("editorial mark inside"), "…and back to Strip the marked span the refusal is stated again");
        check(minimumSizeHint().width() <= 800, "fit gate: the Study pane's minimum width stays within 800 px");
        check(scope_->text().contains("syllables") && scope_->text().contains("folios"), "the scope label states syllables, lines and folios");
        check(!report().isEmpty() && report().contains("engine:"), "the report opens with the provenance block");
        showPage(0);
        return fails;
    }

private:
    void addPage(Page* p) { auto* sc = new QScrollArea; sc->setWidgetResizable(true); sc->setFrameShape(QFrame::NoFrame); sc->setWidget(p); pages_->addWidget(sc); pageObjs_.push_back(p); }
    void say(const QString& m) { status_->setText(ux::sourceBadge(ux::Epistemic::Machine) + m.toHtmlEscaped()); }
    void pickText() {
        const QString f = safeGetOpenFileName(this, "Choose a text to study", QString(), "Texts (*.txt *.act *.inc *.ace *.acip *.md);;All files (*)");
        if (f.isEmpty()) { say("No text chosen."); return; }
        openPath(f);
    }
    // ONE conversion, ONE tokenization, under the current apparatus policy; the refusals travel with the doc.
    void retokenize() {
        if (!doc_.loaded) return;
        using namespace allcore::textspan;
        const std::string raw = doc_.text.toStdString();
        const Converted cv = toEwtsDocument(raw);
        doc_.wylie = cv.wylie; doc_.unconvertedLines = cv.unconvertedLines; doc_.unconvertedLineNumbers = cv.unconvertedLineNumbers;
        doc_.prov = allcore::Provenance(); doc_.prov.engine = "study/1"; doc_.prov.built_utc = QDateTime::currentDateTimeUtc().toString(Qt::ISODate).toStdString();
        doc_.prov.source = doc_.path.isEmpty() ? doc_.name.toStdString() : doc_.path.toStdString();
        doc_.prov.source_sha256 = QString::fromLatin1(QCryptographicHash::hash(doc_.text.toUtf8(), QCryptographicHash::Sha256).toHex()).toStdString();
        doc_.prov.script_in = scriptOf(doc_.text); doc_.prov.norm = "verbatim";
        doc_.prov.apparatus = apparatusMode() == Apparatus::StripSpan ? "strip-span" : apparatusMode() == Apparatus::KeepInner ? "keep-inner" : "as-written";
        doc_.syl = syllableSpans(doc_.wylie, apparatusMode(), &doc_.prov);
        doc_.prov.units_read = (long long)doc_.syl.size();
        for (int ln : cv.unconvertedLineNumbers) doc_.prov.refusals.push_back("line " + std::to_string(ln) + ": could not be converted");
        doc_.folios = countFolioMarkers(raw); doc_.shads = countShads(raw); doc_.folioRange = folioRange(raw);
        doc_.lines = doc_.text.isEmpty() ? 0 : doc_.text.count('\n') + (doc_.text.endsWith('\n') ? 0 : 1);
        QString sc = QString("Studying: %1 — %L2 syllables · %L3 lines").arg(doc_.name).arg((qulonglong)doc_.syl.size()).arg(doc_.lines);
        if (doc_.folios) sc += QString(" · folios %1–%2").arg(QString::fromStdString(doc_.folioRange.first), QString::fromStdString(doc_.folioRange.second)); else sc += " · no folio markers";
        sc += doc_.path.isEmpty() ? QString(" · the document open in front (encoding %1)").arg(doc_.encoding) : QString(" · read as %1").arg(doc_.encoding);
        scope_->setText(sc);
        QStringList notes;
        if (doc_.unconvertedLines) { QStringList ls; for (int ln : cv.unconvertedLineNumbers) { if (ls.size() < 12) ls << QString::number(ln); } notes << QString("%1 line(s) could not be converted and are not counted — lines %2%3.").arg(doc_.unconvertedLines).arg(ls.join(", ")).arg(cv.unconvertedLineNumbers.size() > 12 ? ", …" : ""); }
        if (doc_.prov.units_refused) notes << QString("%1 syllable(s) carry an editorial mark inside them and are not counted — switch Editorial marks to “Keep the letters inside the marks” to include them.").arg(doc_.prov.units_refused);
        refusals_->setText(notes.join("  ")); refusals_->setVisible(!notes.isEmpty());
    }
    static std::string scriptOf(const QString& t) {
        int tib = 0, up = 0, low = 0;
        for (const QChar c : t) { const ushort u = c.unicode(); if (u >= 0x0F00 && u <= 0x0FFF) ++tib; else if (c.isUpper()) ++up; else if (c.isLower()) ++low; }
        if (tib && !up && !low) return "unicode"; if (up && !low && !tib) return "acip"; if (low && !up && !tib) return "wylie"; return "mixed";
    }
    void rerun() { Page* p = page(currentPage()); if (!p) return; say(p->run(doc_)); }

    QString root_; Doc doc_;
    QLabel *scope_ = nullptr, *refusals_ = nullptr, *status_ = nullptr; QComboBox* apparatus_ = nullptr; QPushButton* frontBtn_ = nullptr;
    QStackedWidget* pages_ = nullptr; QButtonGroup* pageBtns_ = nullptr; std::vector<Page*> pageObjs_;
public:
    static inline std::function<void()> g_studyFrontDoc;   // set in main(): hands the front document to the pane
};
// Hooks other panes call without knowing the Study pane (declared here; set in main()).
static std::function<void(const QString& name, const QString& text, const QString& path, int page)> g_studyText;
