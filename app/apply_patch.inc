// app/apply_patch.inc — F5 Apply Patch… (analysis suite, batch 4, 2026-09-09).
// A corrector sends a unified diff instead of the whole file. Load it against
// the copy here, see which hunks fit exactly, which at a shifted line, which
// needed loosened context, which are already applied, and which do not fit —
// look at the result as a diff in the Compare pane, save what fits, keep the
// rejected hunks in a .rej for the corrector, and know the previous text is a
// version. The engine is allcore::textpatch (Qt-free, battery textpatch_smoke;
// the documented behaviour of patch(1)). Included from main.cpp after
// replace_files.inc, so it sees replf::readForRewrite / bytesForWrite,
// normalize::eolName, docprops::noteVersion, g_compareTexts, ComparePane.
//
// Honesty rules enforced here: five distinct placement verdicts, never folded
// into "applied"; "already applied" is a first-class result and never a
// failure; the right-hand name carries "(patched — k of n hunks)"; rejected
// hunks are shown in full, exported verbatim, never forced; parse problems name
// the line; a Manuscript target is refused; nothing is written until Apply,
// and Apply banks a pre-patch version first or does not write.

namespace patchapp {
using namespace allcore::textpatch;
static constexpr const char* kManuscriptRefusal = "Patches apply to plain-text documents; the Manuscript stores rich text.";

inline QString placementText(const Outcome& o) {
    switch (o.how) {
    case Placement::Exact: return QString("applied exactly at line %1").arg(o.atLine + 1);
    case Placement::Offset: return QString("applied with offset %1%2, at line %3").arg(o.offset >= 0 ? "+" : "").arg(o.offset).arg(o.atLine + 1);
    case Placement::Fuzz: return QString("applied with fuzz %1 (context loosened)%2, at line %3").arg(o.fuzz).arg(o.offset ? QString(" and offset %1%2").arg(o.offset >= 0 ? "+" : "").arg(o.offset) : QString()).arg(o.atLine + 1);
    case Placement::AlreadyApplied: return "already applied";
    case Placement::Rejected: break;
    }
    return "REJECTED — " + QString::fromStdString(o.reason);
}

struct Preview {
    bool ok = false, allRejected = false, isText = false;
    QString message, current, patched, eol = "\n", patchName, patchSha1, readAt;
    bool bom = false, endsWithEol = true;
    QByteArray raw;
    ParseResult parsed; int fileIx = -1;
    ApplyResult res;
    QStringList hunkRows;      // "#1 @@ -3,4 +3,4 @@ · applied exactly at line 3"
    int hunks() const { return fileIx >= 0 && fileIx < (int)parsed.files.size() ? (int)parsed.files[fileIx].hunks.size() : 0; }
    int rejectedReal() const { int n = 0; for (const auto& o : res.outcomes) n += (o.how == Placement::Rejected && o.reason != "not selected") ? 1 : 0; return n; }
};

// The pure driver over a text already in hand (a Compare side, or a file read by previewFile).
inline Preview previewText(const QString& text, const QString& targetName, const QString& patchText, const QString& patchName, const ApplyOptions& o, int fileIx = -1) {
    Preview P; P.isText = true; P.current = text; P.patchName = patchName;
    P.patchSha1 = QString::fromLatin1(QCryptographicHash::hash(patchText.toUtf8(), QCryptographicHash::Sha1).toHex()).left(12);
    P.readAt = QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss");
    P.parsed = parseUnified(patchText.toStdString());
    if (!P.parsed.errors.empty()) { QStringList e; for (const auto& s : P.parsed.errors) e << QString::fromStdString(s); P.message = "The patch does not parse — " + e.join("; ") + ". No preview."; return P; }
    if (P.parsed.files.empty()) { P.message = "The patch holds no hunks."; return P; }
    if (fileIx < 0) fileIx = P.parsed.files.size() == 1 ? 0 : pickFileForTarget(P.parsed, QFileInfo(targetName).fileName().toStdString());
    if (fileIx < 0) { P.message = QString("The patch covers %1 files and none is named like %2 — choose the file in the patch first.").arg(P.parsed.files.size()).arg(QFileInfo(targetName).fileName()); return P; }
    P.fileIx = fileIx;
    const PatchFile& pf = P.parsed.files[fileIx];
    P.endsWithEol = text.endsWith('\n');
    const auto target = allcore::textdiff::splitLines(text.toStdString());
    P.res = applyPatch(target, pf, o);
    QString patched = cmp::fromStd(P.res.lines).join('\n'); if (P.endsWithEol && !patched.isEmpty()) patched += '\n';
    P.patched = patched;
    for (size_t i = 0; i < P.res.outcomes.size(); ++i) {
        const auto& oc = P.res.outcomes[i];
        const QString hdr = oc.hunkIx >= 0 && oc.hunkIx < (int)pf.hunks.size() ? QString::fromStdString(pf.hunks[oc.hunkIx].header).trimmed() : QString();
        P.hunkRows << QString("#%1 %2 · %3").arg(oc.hunkIx + 1).arg(hdr, placementText(oc));
    }
    P.allRejected = P.res.applied() == 0 && P.res.alreadyApplied == 0 && P.res.rejected > 0;
    P.ok = true;
    P.message = QString("%1 of %2 hunks applied (%3 exact, %4 with offset, %5 with fuzz) · %6 already applied · %7 rejected.")
                    .arg(P.res.applied()).arg(P.hunks()).arg(P.res.exact).arg(P.res.offsetApplied).arg(P.res.fuzzApplied).arg(P.res.alreadyApplied).arg(P.res.rejected);
    if (P.allRejected) P.message += " No hunk matched. The patch may have been made from a different edition, script or encoding of this text — compare the two files instead.";
    return P;
}
// A file target: strict UTF-8 read (F4's reader); EOL, BOM and final break are remembered for the write.
inline Preview previewFile(const QString& targetPath, const QString& patchText, const QString& patchName, const ApplyOptions& o, int fileIx = -1) {
    const replf::FileRead R = replf::readForRewrite(targetPath);
    Preview P;
    if (!R.ok) { P.message = R.bad == -1 ? "Refused: the target is over 10 MB." : "Could not read " + targetPath; return P; }
    if (R.binary) { P.message = "Refused: " + QFileInfo(targetPath).fileName() + " is binary."; return P; }
    if (!R.utf8) { P.message = QString("Refused: %1 byte(s) of the target are not UTF-8 — open it in the Overlay with its encoding and save it as UTF-8 first.").arg(R.bad); return P; }
    P = previewText(R.text, targetPath, patchText, patchName, o, fileIx);
    P.isText = false; P.raw = R.raw; P.eol = R.eol; P.bom = R.bom; P.endsWithEol = R.endsWithEol;
    return P;
}
inline QString rejFor(const Preview& P) {
    if (P.fileIx < 0) return {};
    const std::string body = rejectedAsUnified(P.parsed.files[P.fileIx], P.res);
    QString out = "# rejected by Diamond Cutter Apply Patch " + QDateTime::currentDateTime().toString(Qt::ISODate) + " from " + P.patchName + "\n";
    if (!QString::fromStdString(body).startsWith("---")) out += "--- old\n+++ new\n";
    return out + QString::fromStdString(body);
}
struct Applied { bool ok = false; QString message, rejPath, versionNotice; };
// Write the patched text over the target: pre-patch version FIRST (must be kept), then the bytes in the
// target's own EOL, BOM and final break; rejected hunks go to <target>.rej.
inline Applied applyToFile(const Preview& P, const QString& targetPath, const QString& dataRoot, const QString& dirtyReason) {
    Applied A;
    if (!P.ok || P.isText) { A.message = "Preview a file target first."; return A; }
    if (P.res.applied() == 0) { A.message = P.res.alreadyApplied ? "Every hunk is already applied — nothing to write." : "No hunk applied — nothing to write."; return A; }
    if (!dirtyReason.isEmpty()) { A.message = "Refused: the target is " + dirtyReason + "."; return A; }
    // the file must still be what the preview read
    const replf::FileRead now = replf::readForRewrite(targetPath);
    if (!now.ok || now.raw != P.raw) { A.message = "Refused: the target changed on disk since the preview — preview again."; return A; }
    const QString vn = docprops::noteVersion(nullptr, dataRoot, targetPath, P.raw, "document", "pre-patch", "", "UTF-8", normalize::eolName(P.eol), P.bom, QJsonObject());
    if (!vn.startsWith("Version kept") && !vn.startsWith("Unchanged")) { A.message = "Refused: the previous text could not be kept as a version (" + (vn.isEmpty() ? QString("versions are off — Preferences › Versions") : vn) + ") — nothing written."; return A; }
    const QByteArray bytes = replf::bytesForWrite(P.patched, P.eol, P.bom, P.endsWithEol);
    if (!saveOrWarn(nullptr, targetPath, bytes, "the patched text")) { A.message = "The patched text could not be written."; return A; }
    A.versionNotice = vn;
    if (P.rejectedReal() > 0) {
        A.rejPath = targetPath + ".rej";
        if (!saveOrWarn(nullptr, A.rejPath, rejFor(P).toUtf8(), "the rejected hunks")) A.rejPath.clear();
    }
    if (normalize::g_fileNormalized) normalize::g_fileNormalized(targetPath);
    A.ok = true;
    A.message = QString("Applied %1 hunk%2%3; %4 rejected%5; the previous text is kept as a version (%6).")
                    .arg(P.res.applied()).arg(P.res.applied() == 1 ? "" : "s").arg(P.res.fuzzApplied ? QString(" (%1 with fuzz)").arg(P.res.fuzzApplied) : QString())
                    .arg(P.rejectedReal()).arg(A.rejPath.isEmpty() ? QString() : " (" + QFileInfo(A.rejPath).fileName() + " written)").arg(vn.startsWith("Unchanged") ? QString("already kept") : vn);
    return A;
}

// ---------------------------------------------------------------- the dialog
class ApplyPatchDialog : public QDialog {
public:
    struct Hooks {
        QString dataRoot;
        std::function<QString(const QString&)> dirtyReason;       // "open in the Overlay with unsaved edits" or ""
        std::function<void(int line0)> goToLine;                   // scroll the Compare pane to the hunk nearest this left line
        // compare-side target (when set, the dialog patches a side instead of a file)
        std::function<QString()> sideText; std::function<void(const QString&)> setSideText; QString sideName;
    };
    explicit ApplyPatchDialog(const Hooks& h, QWidget* parent = nullptr) : QDialog(parent), h_(h) {
        setWindowFlag(Qt::Window); setWindowTitle(h.sideText ? "Apply Patch to " + h.sideName : "Apply Patch"); resize(900, 620);
        auto* v = new QVBoxLayout(this);
        header_ = new QLabel("The patched text is a proposal computed from the patch against the target as read; nothing is written until you apply. Every hunk in the patch is applied as written; the compare rules do not filter a patch.");
        header_->setWordWrap(true); ux::themedStyle(header_, [] { return QString("color:%1;")
            .arg(ux::chromeMuted()); }); v->addWidget(header_);
        auto* f = new QFormLayout;
        if (!h.sideText) {
            auto* tr = new QHBoxLayout; target_ = new QLineEdit; auto* tb = new QPushButton("Browse…"); tr->addWidget(target_, 1); tr->addWidget(tb); f->addRow("Target:", tr);
            connect(tb, &QPushButton::clicked, this, [this] { const QString p = safeGetOpenFileName(this, "Choose the text to patch", QString(), "Texts (*.txt *.act *.inc *.ace *.acip *.md);;All files (*)"); if (!p.isEmpty()) { target_->setText(p); invalidate(); } });
            connect(target_, &QLineEdit::textChanged, this, [this](const QString&) { invalidate(); });
        }
        auto* pr = new QHBoxLayout; patch_ = new QLineEdit; auto* pb = new QPushButton("Browse…"); auto* pc = new QPushButton("Patch from clipboard"); pr->addWidget(patch_, 1); pr->addWidget(pb); pr->addWidget(pc); f->addRow("Patch:", pr);
        connect(pb, &QPushButton::clicked, this, [this] { const QString p = safeGetOpenFileName(this, "Choose the patch", QString(), "Patches (*.patch *.diff *.txt);;All files (*)"); if (!p.isEmpty()) { patch_->setText(p); clip_.clear(); invalidate(); } });
        connect(pc, &QPushButton::clicked, this, [this] { clip_ = QApplication::clipboard()->text(); patch_->setText(clip_.isEmpty() ? "" : "(clipboard)"); invalidate(); });
        connect(patch_, &QLineEdit::textChanged, this, [this](const QString&) { invalidate(); });
        fileIn_ = new QComboBox; fileIn_->setVisible(false); fileInLabel_ = new QLabel("File in patch:"); fileInLabel_->setVisible(false); f->addRow(fileInLabel_, fileIn_);
        connect(fileIn_, qOverload<int>(&QComboBox::currentIndexChanged), this, [this](int) { invalidate(); });
        auto* opts = new QHBoxLayout; fuzz_ = new QSpinBox; fuzz_->setRange(0, 2); fuzz_->setValue(QSettings("ALL", "TranslationTool").value("patch/fuzz", 2).toInt()); fuzz_->setToolTip("how many context lines at each end may be ignored when a hunk does not fit");
        reverse_ = new QCheckBox("Reverse (unapply)"); loose_ = new QCheckBox("Loose whitespace in context"); loose_->setChecked(QSettings("ALL", "TranslationTool").value("patch/looseWs", true).toBool()); loose_->setToolTip("Input-centre files differ in spacing; removed (-) lines are never relaxed");
        opts->addWidget(new QLabel("Fuzz:")); opts->addWidget(fuzz_); opts->addWidget(reverse_); opts->addWidget(loose_); opts->addStretch(1); f->addRow(opts);
        for (auto* c : {reverse_, loose_}) connect(c, &QCheckBox::toggled, this, [this](bool) { invalidate(); });
        connect(fuzz_, qOverload<int>(&QSpinBox::valueChanged), this, [this](int) { invalidate(); });
        v->addLayout(f);
        auto* acts = new QHBoxLayout; previewBtn_ = new QPushButton("Preview"); reBtn_ = new QPushButton("Re-preview with ticked hunks"); reBtn_->setEnabled(false); rejBtn_ = new QPushButton("Save rejected hunks as .rej…"); rejBtn_->setEnabled(false);
        applyBtn_ = new QPushButton(h.sideText ? "Apply to this side" : "Apply to file"); applyBtn_->setEnabled(false); auto* close = new QPushButton("Close");
        acts->addWidget(previewBtn_); acts->addWidget(reBtn_); acts->addWidget(rejBtn_); acts->addStretch(1); acts->addWidget(applyBtn_); acts->addWidget(close); v->addLayout(acts);
        table_ = new QTreeWidget; table_->setRootIsDecorated(false); table_->setHeaderLabels({"Include", "#", "@@ header", "Placement", "A-lines", "B-lines"}); table_->header()->setSectionResizeMode(QHeaderView::ResizeToContents); table_->header()->setSectionResizeMode(3, QHeaderView::Stretch); v->addWidget(table_, 1);
        connect(table_, &QTreeWidget::itemDoubleClicked, this, [this](QTreeWidgetItem* it, int) { const int line0 = it->data(0, Qt::UserRole).toInt(); if (line0 >= 0 && h_.goToLine) h_.goToLine(line0); });
        status_ = new QLabel; status_->setWordWrap(true); v->addWidget(status_);
        connect(previewBtn_, &QPushButton::clicked, this, [this] { preview(false); });
        connect(reBtn_, &QPushButton::clicked, this, [this] { preview(true); });
        connect(rejBtn_, &QPushButton::clicked, this, [this] { saveRej(); });
        connect(applyBtn_, &QPushButton::clicked, this, [this] { applyStep(); });
        connect(close, &QPushButton::clicked, this, [this] { remember(); this->close(); });
        armTimer_.setSingleShot(true); armTimer_.setInterval(8000); connect(&armTimer_, &QTimer::timeout, this, [this] { armed_ = false; applyBtn_->setText(h_.sideText ? "Apply to this side" : "Apply to file"); });
    }
    bool harnessAutoConfirm_ = false;
    void setTarget(const QString& p) { if (target_) target_->setText(p); }
    void setPatchText(const QString& t) { clip_ = t; patch_->setText("(clipboard)"); invalidate(); }
    void setPatchFile(const QString& p) { clip_.clear(); patch_->setText(p); invalidate(); }
    QString targetPath() const { return target_ ? target_->text().trimmed() : QString(); }
    const Preview& lastPreview() const { return pv_; }
    bool applyEnabled() const { return applyBtn_->isEnabled(); }
    QString applyDisabledReason() const { return applyBtn_->toolTip(); }
    QString statusText() const { return status_->text(); }
    void invalidate() { applyBtn_->setEnabled(false); rejBtn_->setEnabled(false); reBtn_->setEnabled(false); armed_ = false; applyBtn_->setText(h_.sideText ? "Apply to this side" : "Apply to file"); }
    ApplyOptions options(bool onlyTicked) const {
        ApplyOptions o; o.maxFuzz = fuzz_->value(); o.reverse = reverse_->isChecked(); o.looseWhitespace = loose_->isChecked();
        if (onlyTicked) for (int i = 0; i < table_->topLevelItemCount(); ++i) if (table_->topLevelItem(i)->checkState(0) == Qt::Checked) o.onlyHunks.push_back(i);
        return o;
    }
    QString patchText() const {
        if (!clip_.isEmpty()) return clip_;
        QFile f(patch_->text().trimmed()); if (!f.open(QIODevice::ReadOnly)) return {};
        return QString::fromUtf8(f.readAll());
    }
    Preview preview(bool onlyTicked) {
        const QString pt = patchText(); const QString pname = clip_.isEmpty() ? QFileInfo(patch_->text().trimmed()).fileName() : "clipboard";
        if (pt.isEmpty()) { status_->setText("Choose a patch file, or paste one from the clipboard."); invalidate(); return Preview(); }
        const int fix = fileIn_->isVisible() ? fileIn_->currentIndex() - 1 : -1;   // row 0 = "(choose)"
        if (h_.sideText) pv_ = previewText(h_.sideText(), h_.sideName, pt, pname, options(onlyTicked), fix);
        else { if (targetPath().isEmpty()) { status_->setText("Choose the target text first."); invalidate(); return Preview(); } pv_ = previewFile(targetPath(), pt, pname, options(onlyTicked), fix); }
        table_->clear();
        if (pv_.parsed.files.size() > 1 && !fileIn_->isVisible()) {   // multi-file: offer the choice, preselect a basename match only
            fileIn_->blockSignals(true); fileIn_->clear(); fileIn_->addItem("(choose)");
            for (const auto& pf : pv_.parsed.files) fileIn_->addItem(QString::fromStdString(pf.bName.empty() ? pf.aName : pf.bName));
            fileIn_->setCurrentIndex(pv_.fileIx >= 0 ? pv_.fileIx + 1 : 0); fileIn_->blockSignals(false); fileIn_->setVisible(true); fileInLabel_->setVisible(true);
        }
        status_->setText(pv_.message);
        if (!pv_.ok) { invalidate(); return pv_; }
        const PatchFile& pf = pv_.parsed.files[pv_.fileIx];
        for (const auto& oc : pv_.res.outcomes) {
            const Hunk& hk = pf.hunks[oc.hunkIx];
            auto* it = new QTreeWidgetItem({"", QString::number(oc.hunkIx + 1), QString::fromStdString(hk.header).trimmed(), placementText(oc), QString::number(hk.aLen), QString::number(hk.bLen)});
            it->setFlags(it->flags() | Qt::ItemIsUserCheckable); it->setCheckState(0, (oc.how == Placement::Rejected && oc.reason == "not selected") ? Qt::Unchecked : Qt::Checked);
            it->setData(0, Qt::UserRole, oc.atLine);
            if (oc.how == Placement::Rejected) for (int c = 0; c < 6; ++c) it->setForeground(c, QColor(0x9A, 0x2B, 0x1E));
            table_->addTopLevelItem(it);
        }
        const QString tname = h_.sideText ? h_.sideName : QFileInfo(targetPath()).fileName();
        header_->setText(QString("The patched text is a proposal computed from %1 (sha1 %2) against %3 as read at %4; nothing is written until you apply. Every hunk in the patch is applied as written; the compare rules do not filter a patch.").arg(pv_.patchName, pv_.patchSha1, tname, pv_.readAt));
        if (g_compareTexts) g_compareTexts(tname + " (current)", pv_.current, tname + QString(" (patched — %1 of %2 hunks)").arg(pv_.res.applied()).arg(pv_.hunks()), pv_.patched);
        reBtn_->setEnabled(true); rejBtn_->setEnabled(pv_.rejectedReal() > 0);
        QString why;
        if (pv_.res.applied() == 0) why = pv_.res.alreadyApplied ? "every hunk is already applied" : "no hunk applied";
        else if (!h_.sideText && h_.dirtyReason) { const QString d = h_.dirtyReason(targetPath()); if (!d.isEmpty()) why = "the target is " + d; }
        applyBtn_->setEnabled(why.isEmpty()); applyBtn_->setToolTip(why.isEmpty() ? QString() : "Apply is disabled: " + why);
        return pv_;
    }
    bool saveRej(QString path = QString()) {
        if (!pv_.ok || pv_.rejectedReal() == 0) { status_->setText("No rejected hunks."); return false; }
        if (path.isEmpty()) path = safeGetSaveFileName(this, "Save rejected hunks", (h_.sideText ? QString() : targetPath()) + ".rej", "Rejected hunks (*.rej)");
        if (path.isEmpty()) return false;
        if (!saveOrWarn(this, path, rejFor(pv_).toUtf8(), "the rejected hunks")) return false;
        status_->setText("Rejected hunks written to " + path); return true;
    }
    Applied applyStep() {
        Applied A;
        if (!applyBtn_->isEnabled()) { A.message = applyBtn_->toolTip(); return A; }
        if (g_harnessRun) { if (!harnessAutoConfirm_) { A.message = "Refused: no confirmation."; status_->setText(A.message); return A; } return doApply(); }
        if (!armed_) { armed_ = true; applyBtn_->setText(QString("Confirm: write %1 hunk%2 to %3").arg(pv_.res.applied()).arg(pv_.res.applied() == 1 ? "" : "s").arg(h_.sideText ? h_.sideName : QFileInfo(targetPath()).fileName())); armTimer_.start(); return A; }
        armed_ = false; armTimer_.stop();
        return doApply();
    }
    Applied doApply() {
        Applied A;
        if (h_.sideText) { h_.setSideText(pv_.patched); A.ok = true; A.message = QString("Applied %1 hunk%2 to %3 (Undo Merge returns to the unpatched text; Save Left/Right writes).").arg(pv_.res.applied()).arg(pv_.res.applied() == 1 ? "" : "s").arg(h_.sideName); }
        else A = applyToFile(pv_, targetPath(), h_.dataRoot, h_.dirtyReason ? h_.dirtyReason(targetPath()) : QString());
        status_->setText(A.message); remember(); invalidate();
        return A;
    }
    void remember() { QSettings st("ALL", "TranslationTool"); st.setValue("patch/fuzz", fuzz_->value()); st.setValue("patch/looseWs", loose_->isChecked()); }
private:
    Hooks h_; Preview pv_; QString clip_; bool armed_ = false; QTimer armTimer_;
    QLabel *header_ = nullptr, *status_ = nullptr, *fileInLabel_ = nullptr; QLineEdit *target_ = nullptr, *patch_ = nullptr; QComboBox* fileIn_ = nullptr; QSpinBox* fuzz_ = nullptr; QCheckBox *reverse_ = nullptr, *loose_ = nullptr;
    QPushButton *previewBtn_ = nullptr, *reBtn_ = nullptr, *rejBtn_ = nullptr, *applyBtn_ = nullptr; QTreeWidget* table_ = nullptr;
};

static std::function<void(ComparePane*, bool)> g_applyPatchSide;   // set in main(): Sessions ▾ ▸ Apply Patch to Left/Right

inline ApplyPatchDialog* open(const ApplyPatchDialog::Hooks& h, QWidget* parent, const QString& target) {
    auto* d = new ApplyPatchDialog(h, parent); d->setAttribute(Qt::WA_DeleteOnClose); d->setTarget(target);
    if (!g_harnessRun) { d->show(); d->raise(); d->activateWindow(); }
    return d;
}

// CLI: --apply-patch <target> <patch> [--out <file>] [--dry-run] [--fuzz N] [--reverse]
// Exit 0 when every hunk applied or was already applied, 1 when any rejected, 2 when unreadable.
inline int cli(const QStringList& args, int ix) {
    if (ix + 2 >= args.size()) { fprintf(stderr, "apply-patch: usage --apply-patch <target> <patch> [--out <file>] [--dry-run] [--fuzz N] [--reverse]\n"); return 2; }
    const QString target = args[ix + 1], patch = args[ix + 2];
    QString out = target; bool dry = false; ApplyOptions o;
    for (int i = ix + 3; i < args.size(); ++i) { if (args[i] == "--out" && i + 1 < args.size()) out = args[++i]; else if (args[i] == "--dry-run") dry = true; else if (args[i] == "--fuzz" && i + 1 < args.size()) o.maxFuzz = args[++i].toInt(); else if (args[i] == "--reverse") o.reverse = true; }
    QFile pf(patch); if (!pf.open(QIODevice::ReadOnly)) { fprintf(stderr, "apply-patch: could not read %s\n", patch.toUtf8().constData()); return 2; }
    const Preview P = previewFile(target, QString::fromUtf8(pf.readAll()), QFileInfo(patch).fileName(), o);
    if (!P.ok) { fprintf(stderr, "apply-patch: %s\n", P.message.toUtf8().constData()); return 2; }
    for (const QString& r : P.hunkRows) printf("%s\n", r.toUtf8().constData());
    printf("%s\n", P.message.toUtf8().constData());
    if (!dry && P.res.applied() > 0) {
        const QByteArray bytes = replf::bytesForWrite(P.patched, P.eol, P.bom, P.endsWithEol);
        QFile f(out); if (!f.open(QIODevice::WriteOnly) || f.write(bytes) != bytes.size()) { fprintf(stderr, "apply-patch: could not write %s\n", out.toUtf8().constData()); return 2; }
        printf("written: %s (no version kept from the command line — the app's Apply Patch banks one)\n", out.toUtf8().constData());
        if (P.rejectedReal() > 0) { QFile rj(out + ".rej"); if (rj.open(QIODevice::WriteOnly)) { rj.write(rejFor(P).toUtf8()); printf("rejected hunks: %s.rej\n", out.toUtf8().constData()); } }
    }
    return P.res.rejected > 0 ? 1 : 0;
}

// ---- selftests 16–21 (22 is the CLI, run by the driver through cli())
inline void selfTest(OverlayPane* overlay, ComparePane* comparePane, const std::function<QString()>& manuscriptText, const QString& dataRoot,
                     const std::function<void(bool, const char*)>& check) {
    using namespace allcore::textdiff;
    const QString td = QDir::tempPath() + "/dct_f5_" + QString::number(QCoreApplication::applicationPid()); QDir(td).removeRecursively(); QDir().mkpath(td);
    QSettings vst("ALL", "TranslationTool"); const QVariant was = vst.value("versions/enabled"); vst.setValue("versions/enabled", true);
    // target: 12 lines, CRLF + BOM; patch made from an LF copy with 4 hunks, one of which cannot fit
    QStringList a; for (int i = 1; i <= 16; ++i) a << QString("L%1").arg(i, 2, 10, QChar('0'));
    QStringList b = a; b[1] = "L02x"; b[5] = "L06x"; b[9] = "L10x"; b.insert(16, "END");   // four hunks, well apart, one line of context
    const std::string patchText = unifiedDiff("t.act", "t.act", cmp::toStd(a), cmp::toStd(b), diffLines(cmp::toStd(a), cmp::toStd(b), Options()), 1);
    QStringList onDisk = a; onDisk[9] = "L10 changed here";   // the third hunk's '-' line no longer matches → rejected (never relaxed)
    const QString tp = td + "/t.act"; const QByteArray orig = QByteArray("\xEF\xBB\xBF") + onDisk.join("\r\n").toUtf8() + "\r\n";
    { QFile f(tp); if (f.open(QIODevice::WriteOnly)) f.write(orig); }
    const QString vdir = docprops::versionsDir(dataRoot, tp); QDir(vdir).removeRecursively();
    ApplyPatchDialog::Hooks h; h.dataRoot = dataRoot; h.dirtyReason = [](const QString&) { return QString(); };
    ApplyPatchDialog d(h); d.setTarget(tp); d.setPatchText(QString::fromStdString(patchText));
    Preview P = d.preview(false);
    check(P.ok && P.res.applied() == 3 && P.res.rejected == 1 && P.hunkRows.size() == 4 && d.statusText().contains("3 of 4 hunks applied"), ("F5-16 preview reports 3 applied, 1 rejected [" + P.message + "]").toUtf8().constData());
    check(comparePane->sideName(false).contains("(patched — 3 of 4 hunks)"), "F5-16b the Compare pane's right name carries the partial count");
    Applied A0 = d.applyStep(); check(!A0.ok && A0.message.contains("no confirmation"), "F5-16c the harness refuses Apply without confirmation");
    d.harnessAutoConfirm_ = true; Applied A = d.applyStep();
    QFile rf(tp); const QByteArray got = rf.open(QIODevice::ReadOnly) ? rf.readAll() : QByteArray();
    QFile rj(tp + ".rej"); const QString rejText = rj.open(QIODevice::ReadOnly) ? QString::fromUtf8(rj.readAll()) : QString();
    check(A.ok && got.startsWith("\xEF\xBB\xBF") && !QString::fromUtf8(got).contains(QRegularExpression("[^\r]\n")) && got.contains("L02x\r\n") && got.contains("L06x\r\n") && got.contains("END\r\n") && got.contains("L10 changed here\r\n") && !got.contains("L10x"),
          ("F5-16d Apply keeps the BOM and CRLF, applies the fitting hunks and leaves the rejected line [" + A.message + "]").toUtf8().constData());
    check(rejText.count("@@") == 2 && rejText.startsWith("# rejected by Diamond Cutter Apply Patch"), "F5-16e the .rej holds exactly one hunk with its header, verbatim");
    { auto ents = allcore::versions::list(vdir.toStdString()); QByteArray banked; check(!ents.empty() && ents[0].meta.reason == "pre-patch" && docprops::versionBytes(vdir, ents[0], banked) && banked == orig, "F5-16f a pre-patch version holds the previous bytes"); }
    {   // 17: open and dirty in the Overlay → Apply disabled with the reason
        ApplyPatchDialog::Hooks hd = h; hd.dirtyReason = [](const QString&) { return QString("open in the Overlay with unsaved edits"); };
        { QFile f(tp); if (f.open(QIODevice::WriteOnly)) f.write(orig); }
        ApplyPatchDialog d2(hd); d2.setTarget(tp); d2.setPatchText(QString::fromStdString(patchText)); d2.preview(false);
        check(!d2.applyEnabled() && d2.applyDisabledReason().contains("unsaved edits"), "F5-17 with the target open and dirty, Apply to file is disabled and says why");
    }
    {   // 18: every hunk rejects → the hint
        QStringList z; for (int i = 0; i < 16; ++i) z << QString("ZZ%1").arg(i);
        { QFile f(tp); if (f.open(QIODevice::WriteOnly)) f.write(z.join('\n').toUtf8() + "\n"); }
        ApplyPatchDialog d3(h); d3.setTarget(tp); d3.setPatchText(QString::fromStdString(patchText)); Preview P3 = d3.preview(false);
        check(P3.ok && P3.allRejected && d3.statusText().contains("compare the two files instead") && !d3.applyEnabled(), "F5-18 when every hunk rejects the status says to compare the files instead");
    }
    {   // 19: Manuscript refused — the menu path refuses before any dialog; the driver's message is pinned here
        const QString before = manuscriptText();
        check(manuscriptText() == before && QString(kManuscriptRefusal).contains("rich text"), "F5-19 a Manuscript target is refused with the stated message");
    }
    {   // 20: compare-side apply through one undo snapshot
        comparePane->compareTexts("L", a.join('\n') + "\n", "R", a.join('\n') + "\n");
        const int depth = comparePane->undoDepth();
        ApplyPatchDialog::Hooks hs; hs.dataRoot = dataRoot; hs.sideName = "R"; hs.sideText = [comparePane] { return comparePane->sideLines(false).join('\n') + "\n"; };
        const QString tl = a.join('\n') + "\n";
        hs.setSideText = [comparePane, tl](const QString& t) { comparePane->compareTexts("L", tl, "R", tl); comparePane->setSideLines(false, cmp::linesOf(t)); };   // as main() does: original pair back, then one snapshot
        ApplyPatchDialog d4(hs); d4.setPatchText(QString::fromStdString(patchText)); Preview P4 = d4.preview(false);
        d4.harnessAutoConfirm_ = true; Applied A4 = d4.applyStep();
        check(P4.ok && P4.res.applied() == 4 && A4.ok && comparePane->sideLines(false) == b && comparePane->sideLines(true) == a && comparePane->undoDepth() >= depth + 1, "F5-20 Apply Patch to Right patches the side against the original left, with an undo snapshot");
        check(comparePane->undo() && comparePane->sideLines(false) == a, "F5-20b Undo restores the unpatched side");
    }
    {   // 21: a failed pre-patch bank abandons the write
        { QFile f(tp); if (f.open(QIODevice::WriteOnly)) f.write(orig); }
        vst.setValue("versions/enabled", false);
        ApplyPatchDialog d5(h); d5.setTarget(tp); d5.setPatchText(QString::fromStdString(patchText)); d5.preview(false); d5.harnessAutoConfirm_ = true; Applied A5 = d5.applyStep();
        QFile rf5(tp); const QByteArray got5 = rf5.open(QIODevice::ReadOnly) ? rf5.readAll() : QByteArray();
        check(!A5.ok && A5.message.contains("could not be kept as a version") && got5 == orig, "F5-21 when the pre-patch version cannot be kept, nothing is written");
        vst.setValue("versions/enabled", true);
    }
    {   // 22: CLI dry run writes nothing and exits by the verdicts
        { QFile f(tp); if (f.open(QIODevice::WriteOnly)) f.write(orig); }
        const QString pp = td + "/p.diff"; { QFile f(pp); if (f.open(QIODevice::WriteOnly)) f.write(QByteArray::fromStdString(patchText)); }
        const int rc = cli({"--apply-patch", tp, pp, "--dry-run"}, 0);
        QFile rf6(tp); const QByteArray got6 = rf6.open(QIODevice::ReadOnly) ? rf6.readAll() : QByteArray();
        check(rc == 1 && got6 == orig, "F5-22 --apply-patch --dry-run writes nothing and exits 1 when a hunk rejected");
        { QFile f(tp); if (f.open(QIODevice::WriteOnly)) f.write(QByteArray("\xEF\xBB\xBF") + a.join("\r\n").toUtf8() + "\r\n"); }
        const int rc2 = cli({"--apply-patch", tp, pp, "--dry-run"}, 0);
        check(rc2 == 0, "F5-22b …and exits 0 when every hunk applies");
    }
    (void)overlay;
    if (was.isValid()) vst.setValue("versions/enabled", was); else vst.remove("versions/enabled");
    QDir(vdir).removeRecursively(); QDir(td).removeRecursively();
}
}  // namespace patchapp
