// app/versions_pane.inc — F1 Versions (analysis suite, batch 4, 2026-09-09).
// Every save of a document, draft or manuscript keeps a compressed copy
// with who saved it and when, under <dataRoot>/library/versions/<docKey>/.
// This file holds the app glue (docprops::noteVersion, decoding, restore
// with the bank-first rule) and the Versions window. The store itself is
// allcore::versions (Qt-free, battery versions_smoke). Included from
// main.cpp after compare_pane.inc, inside its anonymous namespace; the Qt
// and allcore includes live at the top of main.cpp.
//
// Honesty rules this file enforces: a restore banks the file as it is on
// disk first, so it can never lose anything; a version whose bytes do not
// decode in the recorded encoding refuses Compare and Restore (nothing is
// guessed); a version whose hash does not match its record is shown as
// damaged and excluded; who-saved-it states whether it came from the Team
// name or the login; and the window says plainly that there is no Track
// Changes toggle — accept/reject is Compare + Copy Left/Right + Save.

namespace docprops {

inline QString versionsRoot(const QString& dataRoot) { return dataRoot + "/library/versions"; }
inline QString versionDocKey(const QString& docPath, const QString& dataRoot) {
    return QString::fromStdString(allcore::versions::docKeyFor(QFileInfo(docPath).absoluteFilePath().toStdString(), dataRoot.toStdString()));
}
inline QString versionsDir(const QString& dataRoot, const QString& docPath) { return versionsRoot(dataRoot) + "/" + versionDocKey(docPath, dataRoot); }
inline allcore::versions::Limits versionLimits() {
    QSettings st("ALL", "TranslationTool");
    allcore::versions::Limits l;
    l.maxVersions = st.value("versions/capCount", 200).toInt();
    l.maxBytes = st.value("versions/capMB", 64).toLongLong() << 20;
    return l;
}
inline int versionCount(const QString& dataRoot, const QString& docPath) {
    if (dataRoot.isEmpty() || docPath.isEmpty()) return -1;
    const QString dir = versionsDir(dataRoot, docPath);
    if (!QFileInfo(dir).isDir()) return 0;
    return (int)allcore::versions::list(dir.toStdString(), QFileInfo(docPath).absoluteFilePath().toStdString()).size();
}
inline bool versionsEnabled() { return QSettings("ALL", "TranslationTool").value("versions/enabled", true).toBool(); }
inline QString whoSaves(QString* source) {
    if (!g_userName.isEmpty()) { if (source) *source = "team-name"; return g_userName; }
    if (source) *source = "login";
    return qEnvironmentVariable("USER", "unknown");
}

// Keep a version of the bytes that were just written. Returns the notice
// for the pane's hint line ("" when there is nothing to say). No-op when
// versions are off or there is no data root. Called AFTER noteSave so the
// revision recorded is the one the save produced.
inline QString noteVersion(QWidget* /*parent*/, const QString& dataRoot, const QString& docPath,
                           const QByteArray& savedBytes, const QString& kind, const QString& reason,
                           const QString& changeset, const QString& encoding, const QString& eol, bool bom,
                           const QJsonObject& statistics) {
    if (!versionsEnabled() || dataRoot.isEmpty() || docPath.isEmpty()) return {};
    QSettings st("ALL", "TranslationTool");
    if (reason == "autosave" && !st.value("versions/autosaveSlot", true).toBool()) return {};
    const QString dir = versionsDir(dataRoot, docPath);
    QDir().mkpath(dir);
    allcore::versions::Meta m;
    m.stamp = QDateTime::currentDateTime().toString("yyyyMMdd-HHmmss-zzz").toStdString();
    m.path = QFileInfo(docPath).absoluteFilePath().toStdString();
    m.kind = kind.toStdString(); m.reason = reason.toStdString(); m.changeset = changeset.toStdString();
    m.encoding = encoding.toStdString(); m.lineEnding = eol.toStdString(); m.bom = bom;
    m.codec = "qz"; m.bytes = savedBytes.size();
    m.sha1 = QString::fromLatin1(QCryptographicHash::hash(savedBytes, QCryptographicHash::Sha1).toHex()).toStdString();
    QString src; m.savedBy = whoSaves(&src).toStdString(); m.savedBySource = src.toStdString();
    m.savedAt = QDateTime::currentDateTime().toString(Qt::ISODate).toStdString();
    m.app = QString("DiamondCutterTranslationTool %1").arg(QStringLiteral(ALL_APP_VERSION)).toStdString();
    m.revision = load(sidecarPath(dataRoot, docPath)).value("revision").toInt();
    if (statistics.isEmpty()) m.statisticsNote = "statistics not computed for this save";
    else m.statisticsJson = QString::fromUtf8(QJsonDocument(statistics).toJson(QJsonDocument::Compact)).toStdString();
    const QByteArray blob = qCompress(savedBytes);
    const auto out = allcore::versions::put(dir.toStdString(), std::string(blob.constData(), (size_t)blob.size()), m, versionLimits());
    if (out.result == allcore::versions::PutResult::Failed) return "Version NOT kept — " + dir + " is not writable";
    if (out.result == allcore::versions::PutResult::Deduplicated) return "Unchanged since the last version — nothing new was kept";
    QString n = reason == "autosave" ? "Autosave version kept (one rolling slot)" : "Version kept";
    if (out.pruned) n += QString(" · %1 older removed to stay within %2 versions / %3 MB (Preferences ▸ Versions)").arg(out.pruned).arg(versionLimits().maxVersions).arg(versionLimits().maxBytes >> 20);
    return n;
}

// The bytes of a version as written to disk (decompressed). False when the
// blob is missing, unreadable, or its hash does not match its record.
inline bool versionBytes(const QString& dir, const allcore::versions::Entry& e, QByteArray& out) {
    std::string blob;
    if (!allcore::versions::readBlob(dir.toStdString(), e, blob)) return false;
    QByteArray raw(blob.data(), (int)blob.size());
    if (e.meta.codec == "qz") { out = qUncompress(raw); if (out.isEmpty() && e.meta.bytes > 0) return false; }
    else out = raw;
    const QString sha = QString::fromLatin1(QCryptographicHash::hash(out, QCryptographicHash::Sha1).toHex());
    return sha.toStdString() == e.meta.sha1;
}
// The version as text in its recorded encoding; bad = undecodable bytes.
inline bool versionText(const QString& dir, const allcore::versions::Entry& e, QString& text, int* bad) {
    QByteArray bytes;
    if (!versionBytes(dir, e, bytes)) return false;
    if (e.meta.lineEnding == "HTML") { text = editops::htmlToPlain(QString::fromUtf8(bytes)); if (bad) *bad = 0; return true; }
    int b = 0;
    text = enc::decode(bytes, QString::fromStdString(e.meta.encoding.empty() ? std::string("UTF-8") : e.meta.encoding), &b);
    if (bad) *bad = b;
    return true;
}

}  // namespace docprops

// The Versions window. One per document; modeless; every action returns
// honestly; no QMessageBox (two-step buttons carry the confirmation, and
// under the harness harnessAutoConfirm_ decides).
class VersionsWindow : public QDialog {
public:
    struct Hooks {
        QString dataRoot, docPath, kind;                       // kind: document | draft | manuscript
        std::function<QString()> currentText;                  // what is being edited now (plain text)
        std::function<QByteArray()> currentBytesOnDisk;        // reads the file (empty when unsaved)
        std::function<bool()> isDirty;
        std::function<bool(const QString& encoding)> reloadWithEncoding;   // the pane's revert path
        std::function<int()> caretLine;                        // 1-based, or 0
    };
    explicit VersionsWindow(const Hooks& h, QWidget* parent = nullptr) : QDialog(parent), h_(h) {
        setWindowFlag(Qt::Window); setAttribute(Qt::WA_DeleteOnClose, false);
        setWindowTitle("Versions — " + QFileInfo(h.docPath).fileName());
        resize(980, 620);
        auto* v = new QVBoxLayout(this);
        auto* top = new QHBoxLayout;
        filter_ = new QComboBox; filter_->addItems({"All", "Saves only", "Milestones", "Batches"});
        showAuto_ = new QCheckBox("Show autosaves");
        showElse_ = new QCheckBox("Show versions of a file with this name in another folder");
        showElse_->setVisible(false);
        top->addWidget(new QLabel("Show:")); top->addWidget(filter_); top->addWidget(showAuto_); top->addWidget(showElse_); top->addStretch(1);
        auto* openDir = new QPushButton("Open Versions Folder"); top->addWidget(openDir);
        connect(openDir, &QPushButton::clicked, this, [this] { QDir().mkpath(dir()); QDesktopServices::openUrl(QUrl::fromLocalFile(dir())); });
        v->addLayout(top);
        table_ = new QTreeWidget; table_->setRootIsDecorated(false); table_->setSelectionMode(QAbstractItemView::ExtendedSelection); table_->setAlternatingRowColors(true);
        table_->setHeaderLabels({"When", "Who", "Kind", "Name", "Rev", "Size", "Folios", "Syllables", "Shads", "Words", "Lines"});
        table_->header()->setSectionResizeMode(QHeaderView::ResizeToContents);   // dates, names and counts show whole
        table_->header()->setSectionResizeMode(3, QHeaderView::Stretch);           // the version's name takes the rest
        table_->header()->setMinimumSectionSize(48);
        v->addWidget(table_, 1);
        auto* acts = new QHBoxLayout;
        auto btn = [&](const QString& label, const QString& tip, std::function<void()> fn) { auto* b = new QPushButton(label); b->setToolTip(tip); connect(b, &QPushButton::clicked, this, std::move(fn)); acts->addWidget(b); return b; };
        cmpCur_ = btn("Compare with Current", "The selected version on the left, what you are editing on the right, in the Compare pane", [this] { compareWithCurrent(); });
        cmpTwo_ = btn("Compare Two Selected", "Older on the left, newer on the right", [this] { compareTwo(); });
        restore_ = btn("Restore…", "Replace the file on disk with this version. The file as it is now is kept as a version first.", [this] { restoreStep(); });
        btn("Save Version As…", "Write this version's bytes to a file of your choosing without touching the current file", [this] { saveVersionAs(); });
        btn("Name this Version…", "Give the version a name and pin it: pinned versions are never pruned", [this] { nameVersion(); });
        btn("Line Origins…", "Which version first introduced the line under the caret, walked with the diff engine", [this] { lineOrigins(); });
        acts->addStretch(1);
        v->addLayout(acts);
        notice_ = new QLabel; notice_->setWordWrap(true); ux::themedStyle(notice_, [] { return QString("color:%1;")
            .arg(ux::chromeMuted()); }); v->addWidget(notice_);
        auto* footer = new QLabel("Versions exist only for saves made in this app; edits made in other programs, and saves from before this feature existed, left no version. "
                                  "There is no Track Changes toggle for texts: to accept or reject a change, compare a version with what you are editing and use MERGE ▸ Copy Left / Copy Right, then Save."
                                  + QString(h.kind == "manuscript" ? " Manuscript versions are compared as text; formatting is not compared." : ""));
        footer->setWordWrap(true); ux::themedStyle(footer, [] { return QString("color:%1; font-size:11px;")
            .arg(ux::chromeMuted()); }); v->addWidget(footer);
        connect(filter_, qOverload<int>(&QComboBox::currentIndexChanged), this, [this](int) { refresh(); });
        connect(showAuto_, &QCheckBox::toggled, this, [this](bool) { refresh(); });
        connect(showElse_, &QCheckBox::toggled, this, [this](bool) { refresh(); });
        connect(table_, &QTreeWidget::itemSelectionChanged, this, [this] { syncButtons(); });
        connect(table_, &QTreeWidget::itemActivated, this, [this](QTreeWidgetItem*, int) { compareWithCurrent(); });
        restoreTimer_.setSingleShot(true); restoreTimer_.setInterval(8000);
        connect(&restoreTimer_, &QTimer::timeout, this, [this] { restoreArmed_ = false; restore_->setText("Restore…"); });
        refresh();
    }

    QString dir() const { return docprops::versionsDir(h_.dataRoot, h_.docPath); }
    int rowCount() const { return table_->topLevelItemCount(); }
    bool harnessAutoConfirm_ = false;
    QString lastMessage() const { return notice_->text(); }

    void watchChrome() {
        static bool armed = false;
        if (armed) return;
        armed = true;
        ux::onChromeChange(this, [this] { refresh(); });
    }
    void refresh() {
        watchChrome();
        entries_.clear(); table_->clear();
        int other = 0, orphans = 0;
        const std::string only = QFileInfo(h_.docPath).absoluteFilePath().toStdString();
        auto all = allcore::versions::list(dir().toStdString(), showElse_->isChecked() ? std::string() : only, &other, &orphans);
        showElse_->setVisible(other > 0 || showElse_->isChecked());
        if (other > 0) showElse_->setText(QString("Show versions of a file with this name in another folder (%1)").arg(other));
        // newest first
        std::reverse(all.begin(), all.end());
        const int mode = filter_->currentIndex();
        int damaged = 0, hidden = 0;
        for (const auto& e : all) {
            const QString reason = QString::fromStdString(e.meta.reason);
            const bool isAuto = reason == "autosave", isBatch = reason.startsWith("pre-") && !e.meta.changeset.empty();
            if (isAuto && !showAuto_->isChecked()) { ++hidden; continue; }
            if (mode == 1 && reason != "save") continue;
            if (mode == 2 && !e.meta.pinned) continue;
            if (mode == 3 && !isBatch) continue;
            entries_.push_back(e);
            QJsonObject stats = e.meta.statisticsJson.empty() ? QJsonObject() : QJsonDocument::fromJson(QByteArray::fromStdString(e.meta.statisticsJson)).object();
            auto num = [&](const char* k) { return stats.contains(k) ? QString::number(stats.value(k).toInt()) : (e.meta.statisticsNote.empty() ? QString("") : QString("—")); };
            const QString when = QDateTime::fromString(QString::fromStdString(e.meta.savedAt), Qt::ISODate).toString("d MMM yyyy HH:mm:ss");
            QString who = QString::fromStdString(e.meta.savedBy); if (e.meta.savedBySource == "login") who += " (login; no Team name set)";
            QString kind = reason == "save" ? "Saved" : reason == "autosave" ? "Autosaved" : reason == "milestone" ? "Milestone" : reason == "pre-restore" ? "Before restore" : reason == "pre-replace" ? "Before replace" : reason == "pre-patch" ? "Before patch" : reason == "pre-normalize" ? "Before normalize" : reason;
            if (!e.meta.metaOk) kind = "unreadable record";
            QString name = QString::fromStdString(e.meta.label); if (e.meta.pinned) name = "\U0001F4CC " + name;
            auto* it = new QTreeWidgetItem({when.isEmpty() ? QString::fromStdString(e.meta.stamp) : when, who, kind, name, QString::number(e.meta.revision), QString("%1 KB").arg(e.meta.bytes / 1024.0, 0, 'f', 1),
                                            num("folios"), num("syllables"), num("shads"), num("words"), num("lines")});
            it->setToolTip(0, QString("stamp %1 · %2 · %3 · %4").arg(QString::fromStdString(e.meta.stamp), QString::fromStdString(e.meta.encoding), QString::fromStdString(e.meta.lineEnding), QString::fromStdString(e.meta.path)));
            QByteArray probe; const bool okBytes = e.meta.metaOk && docprops::versionBytes(dir(), e, probe);
            if (!okBytes) { ++damaged; it->setText(2, e.meta.metaOk ? "damaged — cannot be restored" : ("unreadable record: " + QString::fromStdString(e.meta.problem))); for (int c = 0; c < 11; ++c) it->setForeground(c, QColor(ux::chromeError())); }
            // ORDER MATTERS: this used to run unconditionally, so a damaged
            // AUTOSAVE was painted quiet grey a line after being painted
            // warning red — the one row that most needed to stand out was the
            // one row that lost its colour. Damage outranks de-emphasis.
            if (isAuto && okBytes) for (int c = 0; c < 11; ++c) it->setForeground(c, QColor(ux::chromeMuted()));
            if (e.meta.path != only) it->setText(3, name + "  (elsewhere)");
            table_->addTopLevelItem(it);
        }
        QStringList notes;
        const int pruned = prunedCount();
        if (pruned) notes << QString("%1 older version%2 removed to stay within %3 versions / %4 MB (Preferences ▸ Versions).").arg(pruned).arg(pruned == 1 ? "" : "s").arg(docprops::versionLimits().maxVersions).arg(docprops::versionLimits().maxBytes >> 20);
        if (hidden) notes << "Autosaves keep one rolling entry until you press Save (Show autosaves).";
        if (orphans) notes << QString("%1 record%2 with no readable provenance %3 not offered as a version (orphan).").arg(orphans).arg(orphans == 1 ? "" : "s").arg(orphans == 1 ? "is" : "are");
        if (damaged) notes << QString("%1 damaged — excluded from Compare and Restore.").arg(damaged);
        if (entries_.empty()) notes << "No versions yet: the first Save of this text will keep one.";
        notice_->setText(notes.join("  "));
        syncButtons();
    }

    // ---- actions (public so the selftest can drive them)
    bool compareWithCurrent(int row = -1) {
        if (row < 0) row = selectedRow(); if (row < 0) { say("Select a version first."); return false; }
        QString text; int bad = 0;
        if (!docprops::versionText(dir(), entries_[row], text, &bad)) { say("This version cannot be read (missing or damaged) — nothing was guessed."); return false; }
        if (bad > 0) { say(QString("%1 byte(s) of this version cannot be read as %2. Nothing was guessed — use Save Version As… to keep the raw bytes.").arg(bad).arg(QString::fromStdString(entries_[row].meta.encoding))); return false; }
        const QString cur = h_.currentText ? h_.currentText() : QString();
        if (g_compareTexts) g_compareTexts(label(row), text, QFileInfo(h_.docPath).fileName() + " (editing)", cur);
        return true;
    }
    bool compareTwo() {
        auto rows = selectedRows(); if (rows.size() != 2) { say("Select exactly two versions."); return false; }
        int older = rows[0], newer = rows[1];   // table is newest-first
        if (entries_[older].meta.stamp > entries_[newer].meta.stamp) std::swap(older, newer);
        QString a, b; int ba = 0, bb = 0;
        if (!docprops::versionText(dir(), entries_[older], a, &ba) || !docprops::versionText(dir(), entries_[newer], b, &bb)) { say("One of the versions cannot be read."); return false; }
        if (ba || bb) { say(QString("%1 undecodable byte(s) — nothing was guessed.").arg(ba + bb)); return false; }
        if (g_compareTexts) g_compareTexts(label(older), a, label(newer), b);
        return true;
    }
    // Two-step: first click arms and relabels for 8 s; the second runs it.
    bool restoreStep() {
        const int row = selectedRow(); if (row < 0) { say("Select a version first."); return false; }
        if (h_.isDirty && h_.isDirty()) { say("Save or revert your unsaved edits first — Restore replaces the file on disk."); return false; }
        if (g_harnessRun) { if (!harnessAutoConfirm_) { say("Restore refused (no confirmation)."); return false; } return doRestore(row); }
        if (!restoreArmed_) { restoreArmed_ = true; restore_->setText(QString("Confirm: restore the version of %1 by %2 — the file as it is now is kept first").arg(QString::fromStdString(entries_[row].meta.savedAt).left(16), QString::fromStdString(entries_[row].meta.savedBy))); restoreTimer_.start(); return false; }
        restoreArmed_ = false; restore_->setText("Restore…"); restoreTimer_.stop();
        return doRestore(row);
    }
    bool doRestore(int row) {
        QByteArray bytes;
        if (!docprops::versionBytes(dir(), entries_[row], bytes)) { say("This version is damaged or missing — nothing restored."); return false; }
        // bank the file as it is on disk first
        const QByteArray onDisk = h_.currentBytesOnDisk ? h_.currentBytesOnDisk() : QByteArray();
        QString banked;
        if (!onDisk.isEmpty()) {
            const auto cur = currentMetaFor();
            banked = docprops::noteVersion(this, h_.dataRoot, h_.docPath, onDisk, h_.kind, "pre-restore", "", cur.first, cur.second, false, QJsonObject());
            if (banked.startsWith("Version NOT kept")) { say(banked + " — restore stopped, nothing written."); return false; }
        }
        if (!saveOrWarn(this, h_.docPath, bytes, "the restored version")) { say("The restore could not be written."); return false; }
        const QString encoding = QString::fromStdString(entries_[row].meta.encoding);
        if (h_.reloadWithEncoding) h_.reloadWithEncoding(encoding);
        refresh();
        const QString kept = onDisk.isEmpty() ? "there was no file on disk to keep"
                           : banked.startsWith("Unchanged") ? "the text that was on disk was already kept as a version (identical bytes), so nothing new was written"
                           : "the text that was on disk was kept as a version first (before restore)";
        say(QString("Restored the version of %1; %2.").arg(QString::fromStdString(entries_[row].meta.savedAt), kept));
        return true;
    }
    bool saveVersionAs(QString path = QString()) {
        const int row = selectedRow(); if (row < 0) { say("Select a version first."); return false; }
        QByteArray bytes; if (!docprops::versionBytes(dir(), entries_[row], bytes)) { say("This version is damaged or missing."); return false; }
        if (path.isEmpty()) path = safeGetSaveFileName(this, "Save this version as", QFileInfo(h_.docPath).path(), "All files (*)");
        if (path.isEmpty()) return false;
        if (!saveOrWarn(this, path, bytes, "the version copy")) return false;
        say("Version written to " + path); return true;
    }
    bool nameVersion(QString name = QString(), bool ask = true) {
        const int row = selectedRow(); if (row < 0) { say("Select a version first."); return false; }
        if (ask) name = docprops::askName(this, "Name this Version", "Name (pinned versions are never pruned):", QString::fromStdString(entries_[row].meta.label));
        auto m = entries_[row].meta; m.label = name.toStdString(); m.pinned = true;
        QFile f(QString::fromStdString(entries_[row].jsonPath));
        if (!f.open(QIODevice::WriteOnly | QIODevice::Truncate)) { say("The version record could not be written."); return false; }
        const std::string json = allcore::versions::serializeMeta(m);
        f.write(json.data(), (qint64)json.size()); f.close();
        refresh(); say(name.isEmpty() ? "Version pinned." : "Version named and pinned: " + name); return true;
    }
    // Which version first introduced the current line `line1` (1-based).
    QString lineOriginsFor(int line1) {
        std::vector<std::vector<std::string>> vs;
        std::vector<allcore::versions::Entry> used;
        // oldest first, saves and milestones only (autosaves and pre-* copies are not points in the text's history)
        for (int i = (int)entries_.size() - 1; i >= 0; --i) {
            const auto& e = entries_[i];
            if (e.meta.reason != "save" && e.meta.reason != "milestone") continue;
            QString t; int bad = 0;
            if (!docprops::versionText(dir(), e, t, &bad) || bad) continue;
            vs.push_back(allcore::textdiff::splitLines(t.toStdString())); used.push_back(e);
        }
        const auto cur = allcore::textdiff::splitLines((h_.currentText ? h_.currentText() : QString()).toStdString());
        if (line1 < 1 || line1 > (int)cur.size()) return "No such line in the current text.";
        const auto orig = allcore::textdiff::lineOrigins(vs, cur);
        const int o = orig[line1 - 1];
        QString basis = QString("Basis: exact line match, walked through %1 kept version%2 with the diff engine; pruned versions and edits made in other programs are not seen.").arg(vs.size()).arg(vs.size() == 1 ? "" : "s");
        if (o >= (int)vs.size()) return QString("Line %1 has changed since the newest version (unsaved edits). ").arg(line1) + basis;
        return QString("Line %1 first appears in the version of %2 (%3, version %4 of %5) and is unchanged since. ").arg(line1).arg(QString::fromStdString(used[o].meta.savedAt), QString::fromStdString(used[o].meta.savedBy)).arg(o + 1).arg(vs.size()) + basis;
    }
    void lineOrigins() {
        const int def = h_.caretLine ? h_.caretLine() : 1;
        QString s = g_harnessRun ? QString::number(def) : docprops::askName(this, "Line Origins", "Line number in the current text:", QString::number(def));
        if (s.isEmpty()) return;
        say("MACHINE · " + lineOriginsFor(s.toInt()));
    }
    const std::vector<allcore::versions::Entry>& entries() const { return entries_; }
    void selectRow(int r) { table_->clearSelection(); if (r >= 0 && r < table_->topLevelItemCount()) table_->topLevelItem(r)->setSelected(true); }

private:
    int prunedCount() const {
        QFile f(dir() + "/_meta.json"); if (!f.open(QIODevice::ReadOnly)) return 0;
        return QJsonDocument::fromJson(f.readAll()).object().value("pruned").toInt();
    }
    std::pair<QString, QString> currentMetaFor() const {
        // encoding / line ending of the file as saved: taken from the newest record
        if (!entries_.empty()) return {QString::fromStdString(entries_.front().meta.encoding), QString::fromStdString(entries_.front().meta.lineEnding)};
        return {"UTF-8", h_.kind == "manuscript" ? "HTML" : "LF"};
    }
    QString label(int row) const { return QFileInfo(h_.docPath).fileName() + QString(" (version %1, %2)").arg(QString::fromStdString(entries_[row].meta.savedAt).left(16), QString::fromStdString(entries_[row].meta.savedBy)); }
    int selectedRow() const { const auto r = selectedRows(); return r.empty() ? -1 : r.front(); }
    std::vector<int> selectedRows() const { std::vector<int> v; for (auto* it : table_->selectedItems()) v.push_back(table_->indexOfTopLevelItem(it)); std::sort(v.begin(), v.end()); v.erase(std::unique(v.begin(), v.end()), v.end()); return v; }
    void syncButtons() { const int n = (int)selectedRows().size(); cmpCur_->setEnabled(n == 1); cmpTwo_->setEnabled(n == 2); restore_->setEnabled(n == 1); if (n != 1 && restoreArmed_) { restoreArmed_ = false; restore_->setText("Restore…"); } }
    void say(const QString& m) { notice_->setText(m); }

    Hooks h_;
    QTreeWidget* table_ = nullptr; QComboBox* filter_ = nullptr; QCheckBox* showAuto_ = nullptr; QCheckBox* showElse_ = nullptr;
    QPushButton* cmpCur_ = nullptr; QPushButton* cmpTwo_ = nullptr; QPushButton* restore_ = nullptr; QLabel* notice_ = nullptr;
    std::vector<allcore::versions::Entry> entries_;
    bool restoreArmed_ = false; QTimer restoreTimer_;
};
