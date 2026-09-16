// app/idioms.inc — the idiom bank (2026-09-09, Adam: "a bank of previously
// used idioms, and a way of adding new ones to the idiom register that need
// to be approved by Geshe Michael. Idioms can be big in our translation
// group — I feel like Geshe Michael uses them a lot").
//
// One surface, two doors: the Draft's EVIDENCE group (translating) and the
// Overlay's REVIEW menu (reading). Both open the same bank, and the bank
// always knows which text is open, so "where does this occur" is answerable.
//
// What the bank shows, and the line it does not cross. The register marks
// WHICH Tibetan forms are idioms; it holds no English of its own, because
// English is Geshe Michael's (rule 1). The renderings beside an idiom are
// fetched, at the moment of asking, from his own parallel corpus — attested,
// counted, and labelled HGM. A translator's own English exists here in
// exactly one place: as the payload of a PROPOSAL that is waiting for his
// ruling, and it is labelled as such everywhere it appears.
//
// Two register files, and only one of them is ours to write:
//   <root>/data/idioms/idioms.json      the shipped seed — read-only. This
//                                       repo consumes releases; the app
//                                       never edits data.
//   <dataRoot>/library/idioms.json      the team register — where rulings
//                                       and new forms land. Loaded second,
//                                       so a ruling always overrides a seed.

namespace idiombank {
using allcore::idioms::Entry;
using allcore::idioms::Register;
using allcore::idioms::Status;

inline QString seedPath(const QString& root) { return root + "/data/idioms/idioms.json"; }
inline QString teamPath(const QString& dataRoot) { return dataRoot + "/library/idioms.json"; }

inline QString statusLabel(Status s) {
    switch (s) {
        case Status::Approved: return "approved";
        case Status::Declined: return "ruled not an idiom";
        case Status::Proposed: break;
    }
    return "awaiting Geshe Michael's ruling";
}
inline QColor statusColour(Status s) {
    return s == Status::Approved ? QColor(0x2E, 0x5D, 0x34) : s == Status::Declined ? QColor(0x6B, 0x5E, 0x4E) : QColor(0x9A, 0x7A, 0x33);
}

// Read one register file. A file that is absent is not an error (the team
// register does not exist until the first form is added); a file that is
// present and unreadable IS, and is reported rather than read as empty.
inline std::vector<Entry> readFile(const QString& path, const QString& source, QString* problem) {
    std::vector<Entry> out;
    QFile f(path);
    if (!QFileInfo::exists(path)) return out;
    if (!f.open(QIODevice::ReadOnly)) { if (problem) *problem = "could not be read: " + path; return out; }
    QJsonParseError pe{};
    const QJsonDocument d = QJsonDocument::fromJson(f.readAll(), &pe);
    if (pe.error != QJsonParseError::NoError) { if (problem) *problem = QString("is not valid JSON (%1 at offset %2): %3").arg(pe.errorString()).arg(pe.offset).arg(path); return out; }
    for (const auto& v : d.object().value("idioms").toArray()) {
        const QJsonObject o = v.toObject();
        Entry e;
        e.wylie = o.value("wylie").toString().toStdString();
        e.status = allcore::idioms::statusFromName(o.value("status").toString().toStdString());
        e.evidence = o.value("evidence").toString().toStdString();
        e.proposedBy = o.value("proposed_by").toString().toStdString();
        e.proposed = o.value("proposed").toString().toStdString();
        e.ruledBy = o.value("ruled_by").toString().toStdString();
        e.ruled = o.value("ruled").toString().toStdString();
        e.note = o.value("note").toString().toStdString();
        e.source = source.toStdString();
        if (!e.wylie.empty()) out.push_back(e);
    }
    return out;
}

struct Loaded { Register reg; int seed = 0, team = 0; QStringList problems; };
inline Loaded load(const QString& root, const QString& dataRoot) {
    Loaded L;
    QString p1, p2;
    const auto seed = readFile(seedPath(root), "shipped seed", &p1);
    const auto team = readFile(teamPath(dataRoot), "team register", &p2);
    if (!p1.isEmpty()) L.problems << "The shipped idiom seed " + p1;
    if (!p2.isEmpty()) L.problems << "The team idiom register " + p2;
    L.seed = (int)seed.size(); L.team = (int)team.size();
    L.reg.addAll(seed);
    L.reg.addAll(team);   // second: a ruling here overrides the seed
    return L;
}

// Write the team register. Everything the app knows goes in, including the
// seed forms once they carry a ruling, so a ruling survives a release that
// rewrites the seed.
inline bool saveTeam(QWidget* parent, const QString& dataRoot, const Register& reg) {
    QJsonObject root;
    QJsonObject meta;
    meta["schema"] = "all-idioms/1";
    meta["written"] = QDateTime::currentDateTime().toString(Qt::ISODate);
    meta["note"] = "The team idiom register. It marks WHICH Tibetan forms are idioms and carries no English of its own — the English is Geshe Michael's, shown from the corpus. Forms arrive through the Propose channel (kind: idiom) and carry Geshe Michael's ruling.";
    root["meta"] = meta;
    QJsonArray arr;
    for (const Entry& e : reg.all()) {
        QJsonObject o;
        o["wylie"] = QString::fromStdString(e.wylie);
        o["status"] = QString::fromStdString(allcore::idioms::statusName(e.status));
        if (!e.evidence.empty()) o["evidence"] = QString::fromStdString(e.evidence);
        if (!e.proposedBy.empty()) o["proposed_by"] = QString::fromStdString(e.proposedBy);
        if (!e.proposed.empty()) o["proposed"] = QString::fromStdString(e.proposed);
        if (!e.ruledBy.empty()) o["ruled_by"] = QString::fromStdString(e.ruledBy);
        if (!e.ruled.empty()) o["ruled"] = QString::fromStdString(e.ruled);
        if (!e.note.empty()) o["note"] = QString::fromStdString(e.note);
        arr.push_back(o);
    }
    root["idioms"] = arr;
    QDir().mkpath(QFileInfo(teamPath(dataRoot)).path());
    return saveOrWarn(parent, teamPath(dataRoot), QJsonDocument(root).toJson(), "the team idiom register");
}

// Bring the authority's rulings across from the shared proposals folder.
// A proposal that is still pending changes nothing here: an unruled form
// stays "awaiting Geshe Michael's ruling", which is the honest state.
struct RulingSummary { int approved = 0, declined = 0, added = 0, pending = 0; bool ok = false; QString problem; };
inline RulingSummary applyRulings(Register& reg, const QString& proposalsDir) {
    RulingSummary S;
    if (proposalsDir.isEmpty()) { S.problem = "No shared proposals folder is set (Propose tab)."; return S; }
    allcore::ProposalStore store(proposalsDir.toStdString());
    if (!store.load()) { S.problem = "The proposals folder could not be read: " + proposalsDir; return S; }
    for (const auto& p : store.all()) {
        if (p.kind != allcore::ProposalKind::Idiom) continue;
        if (p.status == allcore::ProposalStatus::Pending || p.status == allcore::ProposalStatus::Deferred) { ++S.pending; continue; }
        const bool approved = p.status == allcore::ProposalStatus::Approved;
        Entry e;
        const Entry* had = reg.find(p.wylie);
        if (had) e = *had; else { e.wylie = p.wylie; ++S.added; }
        e.status = approved ? Status::Approved : Status::Declined;
        e.evidence = p.evidence.empty() ? e.evidence : p.evidence;
        e.proposedBy = p.proposer; e.proposed = p.created;
        e.ruledBy = p.approver; e.ruled = p.ruled; e.note = p.comment;
        e.source = "ruling";
        reg.add(e);
        approved ? ++S.approved : ++S.declined;
    }
    S.ok = true;
    return S;
}

// The syllables of a text, in the register's own canonical form — one rule
// for the register and the text, so a match cannot be an artefact of two
// spellings of the same idea.
inline std::vector<std::string> syllablesOf(const QString& text) {
    QString t = text;
    t.replace(QChar(0x2029), '\n');
    bool upper = false, lower = false;
    for (const QChar c : t) { if (c.isUpper()) upper = true; else if (c.isLower()) lower = true; }
    const std::string wy = (upper && !lower) ? allcore::acipToEwts(t.toStdString()) : t.toLower().toStdString();
    return allcore::idioms::splitPattern(wy);
}
inline QString toWylie(const QString& sel) {
    QString s = sel; s.replace(QChar(0x2029), ' '); s = s.simplified();
    bool upper = false, lower = false;
    for (const QChar c : s) { if (c.isUpper()) upper = true; else if (c.isLower()) lower = true; }
    return (upper && !lower) ? QString::fromStdString(allcore::acipToEwts(s.toStdString())) : s.toLower();
}

// ---------------------------------------------------------------- the bank
class IdiomBankDialog : public QDialog {
public:
    IdiomBankDialog(const QString& root, const QString& dataRoot, allcore::Spine& spine, QWidget* parent = nullptr)
        : QDialog(parent), root_(root), dataRoot_(dataRoot), spine_(spine) {
        setWindowFlag(Qt::Window);
        setWindowTitle("Idioms");
        resize(940, 620);
        auto* v = new QVBoxLayout(this);
        auto* banner = new QLabel(ux::sourceBadge(ux::Epistemic::Reference) +
            "The register marks which Tibetan forms are idioms — fixed expressions not to be read word by word. "
            "It holds no English of its own: the renderings below are Geshe Michael's, fetched from the courses and counted. "
            "A new form is a proposal until Geshe Michael rules on it.");
        banner->setTextFormat(Qt::RichText); banner->setWordWrap(true); ux::themedStyle(banner, [] { return QString("color:%1; background:%2; border:1px solid %3; padding:5px 8px;")
            .arg(ux::chromeInk()).arg(ux::chromePlaque()).arg(ux::chromeRule()); });
        v->addWidget(banner);
        auto* row = new QHBoxLayout;
        filter_ = new QLineEdit; filter_->setPlaceholderText("Filter the forms (wylie)");
        statusBox_ = new QComboBox; statusBox_->addItems({"All", "Approved", "Awaiting Geshe Michael's ruling", "Ruled not an idiom"});
        inText_ = new QCheckBox("Only the forms in the open text"); inText_->setEnabled(false);
        row->addWidget(filter_, 1); row->addWidget(new QLabel("Show:")); row->addWidget(statusBox_); row->addWidget(inText_);
        v->addLayout(row);
        connect(filter_, &QLineEdit::textChanged, this, [this](const QString&) { render(); });
        connect(statusBox_, qOverload<int>(&QComboBox::currentIndexChanged), this, [this](int) { render(); });
        connect(inText_, &QCheckBox::toggled, this, [this](bool) { render(); });
        auto* split = new QSplitter(Qt::Vertical);
        table_ = new QTreeWidget; table_->setRootIsDecorated(false); table_->setAlternatingRowColors(true);
        table_->setHeaderLabels({"Form (wylie)", "Status", "In the open text", "Ruled by", "Why it was put forward"});
        table_->header()->setSectionResizeMode(QHeaderView::ResizeToContents);
        table_->header()->setStretchLastSection(true);
        split->addWidget(table_);
        detail_ = new QTextBrowser; detail_->setOpenLinks(false); detail_->setMinimumHeight(150);
        split->addWidget(detail_); split->setStretchFactor(0, 3); split->setStretchFactor(1, 2);
        v->addWidget(split, 1);
        connect(table_, &QTreeWidget::itemSelectionChanged, this, [this] { showDetail(); });
        auto* acts = new QHBoxLayout;
        auto btn = [&](const QString& label, const QString& tip, std::function<void()> fn) { auto* b = new QPushButton(label); b->setToolTip(tip); connect(b, &QPushButton::clicked, this, std::move(fn)); acts->addWidget(b); return b; };
        proposeBtn_ = btn("Propose an idiom…", "Put a Tibetan form forward for Geshe Michael's ruling, with the passage you found it in as evidence", [this] { propose(); });
        btn("Update from rulings", "Bring across every ruling Geshe Michael has made in the shared proposals folder", [this] { pullRulings(); });
        btn("Copy the list", "The visible forms and their status, as text", [this] { copyList(); });
        acts->addStretch(1);
        auto* close = new QPushButton("Close"); connect(close, &QPushButton::clicked, this, [this] { accept(); }); acts->addWidget(close);
        v->addLayout(acts);
        status_ = new QLabel; status_->setWordWrap(true); status_->setTextFormat(Qt::RichText); ux::themedStyle(status_, [] { return QString("color:%1;")
            .arg(ux::chromeInk()); });
        v->addWidget(status_);
        reload();
    }

    // The text the bank reports occurrences in.
    void setText(const QString& name, const QString& text) {
        textName_ = name; textSyllables_ = syllablesOf(text);
        hits_ = reg_.findIn(textSyllables_);
        inText_->setEnabled(!textSyllables_.empty());
        inText_->setText(textName_.isEmpty() ? "Only the forms in the open text" : QString("Only the forms in %1").arg(textName_));
        render();
    }
    void setSelection(const QString& sel) { selection_ = toWylie(sel); if (!selection_.isEmpty()) filter_->setText(selection_); }
    QString selection() const { return selection_; }

    void reload() {
        const Loaded L = load(root_, dataRoot_);
        reg_ = L.reg; problems_ = L.problems; seedN_ = L.seed; teamN_ = L.team;
        hits_ = reg_.findIn(textSyllables_);
        render();
    }
    void render() {
        table_->clear();
        const QString f = filter_->text().trimmed().toLower();
        const int mode = statusBox_->currentIndex();
        QMap<QString, int> counts;
        for (const auto& h : hits_) counts[QString::fromStdString(h.wylie)]++;
        int shown = 0;
        for (const Entry& e : reg_.all()) {
            const QString w = QString::fromStdString(e.wylie);
            if (!f.isEmpty() && !w.toLower().contains(f)) continue;
            if (mode == 1 && e.status != Status::Approved) continue;
            if (mode == 2 && e.status != Status::Proposed) continue;
            if (mode == 3 && e.status != Status::Declined) continue;
            const int n = counts.value(w, 0);
            if (inText_->isChecked() && n == 0) continue;
            auto* it = new QTreeWidgetItem({w, statusLabel(e.status), n ? QString::number(n) + (n == 1 ? " time" : " times") : QString("—"),
                                            QString::fromStdString(e.ruledBy.empty() ? std::string("—") : e.ruledBy), QString::fromStdString(e.evidence)});
            it->setForeground(1, statusColour(e.status));
            it->setData(0, Qt::UserRole, w);
            table_->addTopLevelItem(it); ++shown;
        }
        QStringList notes;
        notes << QString("%1 form(s) shown of %2 in the register — %3 approved, %4 awaiting Geshe Michael's ruling, %5 ruled not an idiom.")
                     .arg(shown).arg(reg_.size()).arg(reg_.countByStatus(Status::Approved)).arg(reg_.countByStatus(Status::Proposed)).arg(reg_.countByStatus(Status::Declined));
        notes << QString("Read from the shipped seed (%1 form(s)) and the team register (%2).").arg(seedN_).arg(teamN_);
        if (!textSyllables_.empty()) notes << QString("%1 occurrence(s) of registered forms in %2. A form the register does not know is not marked — this list is of the idioms the register holds, not of every idiom in the text.").arg(hits_.size()).arg(textName_.isEmpty() ? "the open text" : textName_);
        for (const QString& p : problems_) notes << p;
        status_->setText(ux::sourceBadge(ux::Epistemic::Reference) + notes.join(" ").toHtmlEscaped());
        if (table_->topLevelItemCount()) { table_->topLevelItem(0)->setSelected(true); showDetail(); } else detail_->clear();
    }

    // The renderings: Geshe Michael's own, from his courses, counted — never composed here.
    void showDetail() {
        const auto sel = table_->selectedItems();
        if (sel.isEmpty()) { detail_->clear(); return; }
        const QString w = sel.first()->data(0, Qt::UserRole).toString();
        const Entry* e = reg_.find(w.toStdString());
        QString h = "<div style='font-size:14px'><b>" + w.toHtmlEscaped() + "</b> &nbsp; " + ux::sourceBadge(ux::Epistemic::Reference) + statusLabel(e ? e->status : Status::Proposed) + "</div>";
        if (e) {
            if (!e->evidence.empty()) h += "<div style='color:#6B5E4E'>put forward because: " + QString::fromStdString(e->evidence).toHtmlEscaped() + "</div>";
            if (!e->proposedBy.empty()) h += "<div style='color:#6B5E4E'>put forward by " + QString::fromStdString(e->proposedBy).toHtmlEscaped() + (e->proposed.empty() ? QString() : " on " + QString::fromStdString(e->proposed)) + "</div>";
            if (!e->ruledBy.empty()) h += "<div style='color:#1E6B4E'>ruled " + QString::fromStdString(allcore::idioms::statusName(e->status)) + " by " + QString::fromStdString(e->ruledBy).toHtmlEscaped() + (e->ruled.empty() ? QString() : " on " + QString::fromStdString(e->ruled)) + (e->note.empty() ? QString() : " — “" + QString::fromStdString(e->note).toHtmlEscaped() + "”") + "</div>";
            if (e->status == Status::Proposed) h += "<div style='color:#9A7A33'>No ruling yet. Until Geshe Michael rules, this form is a proposal: it is marked in a text as a proposal and is never presented as settled.</div>";
        }
        h += "<hr><div>" + ux::sourceBadge(ux::Epistemic::Binding) + "<b>How Geshe Michael has rendered it</b></div>";
        const std::string q = "\"" + w.toStdString() + "\"";
        const long total = spine_.corpusCount(q);
        const auto segs = spine_.corpusSearch(q, "", 8);
        if (segs.empty()) {
            h += "<div><i>No segment of Geshe Michael's courses contains this exact form. That is not evidence against the idiom: the corpus is 42,199 aligned segments of the courses we hold, not everything Geshe Michael has said.</i></div>";
        } else {
            h += QString("<div style='color:#6B5E4E'>%1 segment(s) contain it%2. These are Geshe Michael's renderings in their own contexts, not a definition.</div>")
                     .arg(total < 0 ? QString::number(segs.size()) : QString::number(total))
                     .arg(total > (long)segs.size() ? QString(" · showing the first %1").arg(segs.size()) : QString());
            for (const auto& s : segs) {
                h += "<div style='margin:6px 0'><i style='color:#6B5E4E'>" + QString::fromStdString(s.wylie).left(160).toHtmlEscaped() + "</i><br>" +
                     QString::fromStdString(s.english).left(240).toHtmlEscaped() +
                     "<span style='color:#8A8074;font-size:11px'> &nbsp;[" + QString::fromStdString(s.course).toHtmlEscaped() + ":" + QString::number(s.seq) + "]</span></div>";
            }
        }
        if (!hits_.empty()) {
            QStringList where;
            for (const auto& hit : hits_) if (QString::fromStdString(hit.wylie) == w) where << QString("syllable %1").arg(hit.begSyllable + 1);
            if (!where.isEmpty()) h += "<hr><div><b>In " + (textName_.isEmpty() ? QString("the open text") : textName_.toHtmlEscaped()) + "</b>: " + where.join(", ") + "</div>";
        }
        detail_->setHtml(h);
    }

    // Put a form forward. The English a translator types here is theirs, and
    // is labelled as theirs until he rules.
    bool propose(const QString& forcedWylie = QString(), const QString& forcedEnglish = QString(), const QString& forcedEvidence = QString()) {
        QString w = forcedWylie.isEmpty() ? selection_ : forcedWylie;
        if (w.isEmpty() && !table_->selectedItems().isEmpty()) w = table_->selectedItems().first()->data(0, Qt::UserRole).toString();
        if (w.isEmpty()) w = docprops::askName(this, "Propose an idiom", "The Tibetan form, in wylie or ACIP:", QString());
        w = toWylie(w);
        if (w.isEmpty()) { say("Nothing proposed: no Tibetan form was given."); return false; }
        if (const Entry* had = reg_.find(w.toStdString()); had && had->status == Status::Approved) { say("“" + w + "” is already in the register, approved. Nothing was proposed."); return false; }
        const QString eng = forcedEnglish.isEmpty()
                                ? docprops::askName(this, "Propose an idiom", "The English you have been using for it (optional — it is yours until Geshe Michael rules):", QString())
                                : forcedEnglish;
        const QString ev = forcedEvidence.isEmpty()
                               ? (textName_.isEmpty() ? QString("proposed from the idiom bank") : QString("found in %1").arg(textName_))
                               : forcedEvidence;
        if (g_proposalsDir.isEmpty() || g_userName.isEmpty()) { say("Set your name and the shared proposals folder in the Propose tab first — a proposal has to carry who made it."); return false; }
        allcore::ProposalStore store(g_proposalsDir.toStdString());
        store.load();
        store.propose(allcore::ProposalKind::Idiom, g_userName.toStdString(), w.toStdString(), eng.toStdString(), "idiom", ev.toStdString(), isoToday().toStdString());
        if (!store.save()) { say("The proposals folder could not be written — nothing was filed."); return false; }
        lastProposed_ = w;
        say("“" + w + "” was filed for Geshe Michael's ruling" + (eng.isEmpty() ? QString() : QString(", with your English “%1” recorded as yours").arg(eng)) + ". It stays a proposal until Geshe Michael rules.");
        return true;
    }
    void pullRulings() {
        const RulingSummary S = applyRulings(reg_, g_proposalsDir);
        if (!S.ok) { say(S.problem); return; }
        if (S.approved + S.declined == 0) { say(QString("No new rulings. %1 idiom proposal(s) are still waiting for Geshe Michael.").arg(S.pending)); render(); return; }
        if (!saveTeam(this, dataRoot_, reg_)) { say("The rulings were read but the team register could not be written."); return; }
        hits_ = reg_.findIn(textSyllables_);
        render();
        say(QString("Brought across %1 approval(s) and %2 decline(s); %3 form(s) were new to the register. %4 proposal(s) are still waiting for Geshe Michael's ruling.").arg(S.approved).arg(S.declined).arg(S.added).arg(S.pending));
    }
    void copyList() {
        QStringList lines;
        for (int i = 0; i < table_->topLevelItemCount(); ++i) { auto* it = table_->topLevelItem(i); lines << QString("%1\t%2\t%3").arg(it->text(0), it->text(1), it->text(2)); }
        QApplication::clipboard()->setText("# form\tstatus\tin the open text\n" + lines.join("\n") + "\n");
        say(QString("%1 form(s) copied.").arg(lines.size()));
    }

    // ---- accessors for the battery
    const Register& reg() const { return reg_; }
    int rowCount() const { return table_->topLevelItemCount(); }
    QString statusText() const { return QTextDocumentFragment::fromHtml(status_->text()).toPlainText(); }
    QString detailText() const { return detail_->toPlainText(); }
    QString lastProposed() const { return lastProposed_; }
    int hitCount() const { return (int)hits_.size(); }
    void setStatusFilter(int i) { statusBox_->setCurrentIndex(i); }
    void setOnlyInText(bool on) { inText_->setChecked(on); }

private:
    void say(const QString& m) { status_->setText(ux::sourceBadge(ux::Epistemic::Reference) + m.toHtmlEscaped()); }
    QString root_, dataRoot_, textName_, selection_, lastProposed_;
    allcore::Spine& spine_;
    Register reg_;
    std::vector<std::string> textSyllables_;
    std::vector<allcore::idioms::Hit> hits_;
    QStringList problems_;
    int seedN_ = 0, teamN_ = 0;
    QLineEdit* filter_ = nullptr; QComboBox* statusBox_ = nullptr; QCheckBox* inText_ = nullptr;
    QTreeWidget* table_ = nullptr; QTextBrowser* detail_ = nullptr; QLabel* status_ = nullptr; QPushButton* proposeBtn_ = nullptr;
};

// One bank, opened from wherever the translator is.
static QPointer<IdiomBankDialog> g_bank;
inline IdiomBankDialog* openBank(const QString& root, const QString& dataRoot, allcore::Spine& spine, QWidget* parent,
                                 const QString& textName, const QString& text, const QString& selection) {
    if (!g_bank) g_bank = new IdiomBankDialog(root, dataRoot, spine, parent);
    g_bank->reload();
    g_bank->setText(textName, text);
    g_bank->setSelection(selection);
    if (!g_harnessRun) { g_bank->show(); g_bank->raise(); g_bank->activateWindow(); }
    return g_bank;
}

// ---- selftests
inline int selfTest(QStringList& log, const QString& root, const QString& dataRoot, allcore::Spine& spine) {
    int fails = 0;
    auto check = [&](bool ok, const char* what) { log << QString("  [%1] Idioms: %2").arg(ok ? "PASS" : "FAIL").arg(what); if (!ok) ++fails; };
    // the shipped seed is real data: it must read, and its five forms must be proposals, not rulings
    const Loaded L = load(root, dataRoot);
    check(L.problems.isEmpty(), "the register files read without a problem (an unreadable one would be reported, never read as empty)");
    check(L.seed >= 5 && L.reg.size() >= 5, "the shipped seed's forms are in the register");
    check(L.reg.countByStatus(Status::Approved) == 0 || L.reg.countByStatus(Status::Proposed) > 0,
          "seed forms carry no ruling they were never given");
    IdiomBankDialog d(root, dataRoot, spine);
    d.setText("fixture", "SANGS RGYAS BLA NA MED PA YIN , DPER NA CHOS ,");
    check(d.hitCount() >= 1, "an idiom in the open text is found by the matcher");
    check(d.statusText().contains("occurrence(s) of registered forms") && d.statusText().contains("not of every idiom in the text"),
          "the bank says plainly that it lists the register's forms, not every idiom in the text");
    const int all = d.rowCount();
    d.setStatusFilter(2);   // awaiting a ruling
    check(d.rowCount() > 0 && d.rowCount() <= all, "the status filter narrows the list to the forms awaiting a ruling");
    d.setStatusFilter(0);
    d.setOnlyInText(true);
    check(d.rowCount() >= 1 && d.rowCount() <= all, "“only the forms in the open text” narrows the list to what the text contains");
    d.setOnlyInText(false);
    check(d.detailText().contains("Geshe Michael") || d.detailText().contains("courses"),
          "the detail names whose English is being shown");
    {   // proposing without an identity is refused, and files nothing
        const QString keepDir = g_proposalsDir, keepUser = g_userName;
        g_proposalsDir.clear();
        const bool filed = d.propose("bla ma'i thugs rje", "the kindness of the lama", "battery");
        check(!filed && d.statusText().contains("Propose tab"), "a proposal without a name and a shared folder is refused, and says where to set them");
        g_proposalsDir = keepDir; g_userName = keepUser;
    }
    {   // a real proposal round-trip through a temp proposals folder, then a ruling
        const QString keepDir = g_proposalsDir, keepUser = g_userName;
        const QString td = QDir::tempPath() + "/dct_idioms_" + QString::number(QCoreApplication::applicationPid());
        QDir(td).removeRecursively(); QDir().mkpath(td);
        g_proposalsDir = td; g_userName = "Battery";
        const bool filed = d.propose("rang lag na yod", "in our own two hands", "battery fixture");
        allcore::ProposalStore st(td.toStdString()); st.load();
        bool found = false; std::string id;
        for (const auto& p : st.all()) if (p.kind == allcore::ProposalKind::Idiom && p.wylie == "rang lag na yod") { found = true; id = p.id; }
        check(filed && found, "a proposed idiom is filed to the shared folder as an idiom proposal");
        check(d.statusText().contains("stays a proposal until Geshe Michael rules"), "the translator is told plainly that it is not settled");
        Register r2 = d.reg();
        const RulingSummary before = applyRulings(r2, td);
        check(before.ok && before.approved == 0 && before.pending >= 1, "while it is pending, no ruling is brought across");
        check(st.rule(id, allcore::ProposalStatus::Approved, "Geshe Michael", "yes, an idiom", isoToday().toStdString()) && st.save(),
              "fixture: the authority rules on it");
        Register r3 = d.reg();
        const RulingSummary after = applyRulings(r3, td);
        const auto* e = r3.find("rang lag na yod");
        check(after.approved == 1 && e && e->status == Status::Approved && e->ruledBy == "Geshe Michael",
              "Geshe Michael's ruling is brought across and the form becomes approved, with Geshe Michael's name on it");
        QDir(td).removeRecursively();
        g_proposalsDir = keepDir; g_userName = keepUser;
    }
    return fails;
}

}  // namespace idiombank
