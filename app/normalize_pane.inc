// app/normalize_pane.inc — F3 Normalize… with preview (analysis suite, batch 4,
// 2026-09-09). The compare rules offered as TRANSFORMATIONS of a selection, a
// document, one side of a comparison, or a file: collapse spacing, trailing
// blanks, blank-line runs, the double-shad spelling, spacing around the shad,
// apparatus spans, folio markers, line endings, final newline, house-style
// spacing for English lines. Every run previews as a diff in the Compare pane
// before anything is applied; the result label is counted from that diff.
// The engine is allcore::textnorm (Qt-free, battery textnorm_smoke). This file
// is the app glue: the target abstraction, the pure driver the selftests call,
// the file path (strict UTF-8, bank a pre-normalize version first), and the
// dialog. Included from main.cpp after compare_pane.inc.
//
// Honesty rules enforced here: nothing is guessed — a line the rules cannot
// classify is left byte-identical and listed with its reason; the double-shad
// and space-before-shad forms have NO default (awaiting the house ruling);
// lossy switches say "removes text"; Apply is enabled only after a preview
// computed for exactly these settings and this text; the Manuscript is refused
// (rich text); a file target must be UTF-8 and is banked as a version first.

namespace normalize {
using allcore::textnorm::Options;
using allcore::textnorm::Eol;
using allcore::textnorm::ShadSpace;

inline QString eolName(const QString& eol) { return eol == "\r\n" ? "CRLF" : eol == "\r" ? "CR" : "LF"; }
inline QString eolFor(Eol e, const QString& detected) { return e == Eol::LF ? "\n" : e == Eol::CRLF ? "\r\n" : e == Eol::CR ? "\r" : detected; }
inline QJsonObject toJson(const Options& o) {
    QJsonObject j;
    j["collapseSpaces"] = o.collapseSpaces; j["trimTrailing"] = o.trimTrailing; j["maxBlankLines"] = o.maxBlankLines;
    j["doubleShad"] = QString::fromStdString(o.doubleShad);
    j["spaceBeforeShad"] = o.spaceBeforeShad == ShadSpace::None ? "none" : o.spaceBeforeShad == ShadSpace::One ? "one" : "keep";
    j["spaceAfterShad"] = o.spaceAfterShad; j["stripApparatus"] = o.stripApparatus; j["stripFolioMarkers"] = o.stripFolioMarkers;
    j["eol"] = o.eol == Eol::LF ? "LF" : o.eol == Eol::CRLF ? "CRLF" : o.eol == Eol::CR ? "CR" : "keep";
    j["ensureFinalNewline"] = o.ensureFinalNewline;
    return j;
}
inline Options fromJson(const QJsonObject& j) {
    Options o;
    o.collapseSpaces = j["collapseSpaces"].toBool(); o.trimTrailing = j["trimTrailing"].toBool();
    o.maxBlankLines = j.contains("maxBlankLines") ? j["maxBlankLines"].toInt() : -1;
    o.doubleShad = j["doubleShad"].toString().toStdString();
    const QString sb = j["spaceBeforeShad"].toString(); o.spaceBeforeShad = sb == "none" ? ShadSpace::None : sb == "one" ? ShadSpace::One : ShadSpace::Keep;
    o.spaceAfterShad = j["spaceAfterShad"].toBool(); o.stripApparatus = j["stripApparatus"].toBool(); o.stripFolioMarkers = j["stripFolioMarkers"].toBool();
    const QString e = j["eol"].toString(); o.eol = e == "LF" ? Eol::LF : e == "CRLF" ? Eol::CRLF : e == "CR" ? Eol::CR : Eol::Keep;
    o.ensureFinalNewline = j["ensureFinalNewline"].toBool();
    return o;
}
inline bool anyRule(const Options& o, bool houseStyle) {
    return o.collapseSpaces || o.trimTrailing || o.maxBlankLines >= 0 || !o.doubleShad.empty() || o.spaceBeforeShad != ShadSpace::Keep || o.spaceAfterShad ||
           o.stripApparatus || o.stripFolioMarkers || o.eol != Eol::Keep || o.ensureFinalNewline || houseStyle;
}

// What Normalize acts on. Editor targets change the editor only (Save mints
// the version); compare targets change one side (one undo snapshot); file
// targets write the file after banking a pre-normalize version.
struct Target {
    QString kind;                                  // "editor" | "compare" | "file"
    QString label;                                 // "Document box — KL0032.ACT, 4,212 lines"
    QString path, dataRoot, docKind = "document";  // file targets (and the editor's file when known)
    std::function<QString()> text;                 // the whole text
    std::function<bool()> hasSelection;
    std::function<QString()> selectionText;        // extended to whole lines
    std::function<bool(const QString& newText, bool selectionScope)> apply;
    std::function<void(int line1)> jumpToLine;
    bool isManuscript = false;
};
struct RunResult {
    bool ok = false, applied = false;
    QString before, after, message, eolFrom, eolTo, versionNotice;
    int changedLines = 0;
    QStringList refused;   // "line 12: mixed script"
    std::vector<int> refusedLines;   // 1-based
};

// Extend a QPlainTextEdit selection to whole lines and return it.
inline QString wholeLineSelection(QPlainTextEdit* pe) {
    QTextCursor c = pe->textCursor(); if (!c.hasSelection()) return {};
    QTextCursor s(pe->document()); s.setPosition(std::min(c.anchor(), c.position())); s.movePosition(QTextCursor::StartOfBlock);
    QTextCursor e(pe->document()); e.setPosition(std::max(c.anchor(), c.position())); if (!e.atBlockStart() || e.position() == std::min(c.anchor(), c.position())) e.movePosition(QTextCursor::EndOfBlock);
    QTextCursor r(pe->document()); r.setPosition(s.position()); r.setPosition(e.position(), QTextCursor::KeepAnchor);
    return r.selectedText().replace(QChar(0x2029), '\n');
}
// Replace the whole document or the whole-line selection in ONE edit block.
inline bool replaceInEditor(QPlainTextEdit* pe, const QString& newText, bool selectionScope) {
    QTextCursor c = pe->textCursor();
    QTextCursor r(pe->document());
    if (selectionScope && c.hasSelection()) {
        QTextCursor s(pe->document()); s.setPosition(std::min(c.anchor(), c.position())); s.movePosition(QTextCursor::StartOfBlock);
        QTextCursor e(pe->document()); e.setPosition(std::max(c.anchor(), c.position())); if (!e.atBlockStart() || e.position() == std::min(c.anchor(), c.position())) e.movePosition(QTextCursor::EndOfBlock);
        r.setPosition(s.position()); r.setPosition(e.position(), QTextCursor::KeepAnchor);
    } else { r.select(QTextCursor::Document); }
    r.beginEditBlock(); r.insertText(newText); r.endEditBlock();
    return true;
}
inline Target editorTarget(QPlainTextEdit* pe, const QString& name, const QString& path = QString(), const QString& dataRoot = QString()) {
    Target t; t.kind = "editor"; t.path = path; t.dataRoot = dataRoot;
    t.label = name + (path.isEmpty() ? "" : " — " + QFileInfo(path).fileName()) + QString(", %L1 lines").arg(pe->document()->blockCount());
    QPointer<QPlainTextEdit> p(pe);
    t.text = [p] { return p ? p->toPlainText() : QString(); };
    t.hasSelection = [p] { return p && p->textCursor().hasSelection(); };
    t.selectionText = [p] { return p ? wholeLineSelection(p) : QString(); };
    t.apply = [p](const QString& s, bool sel) { return p && replaceInEditor(p, s, sel); };
    t.jumpToLine = [p](int line1) { if (!p) return; QTextCursor c(p->document()->findBlockByNumber(std::max(0, line1 - 1))); p->setTextCursor(c); p->centerCursor(); p->setFocus(); };
    return t;
}
inline Target fileTarget(const QString& path, const QString& dataRoot) {
    Target t; t.kind = "file"; t.path = path; t.dataRoot = dataRoot;
    const auto L = cmp::readText(path);
    t.label = "File — " + path + (L.ok && !L.binary ? QString(" (%1, UTF-8)").arg(eolName(L.eol)) : QString());
    t.text = [path] { const auto L2 = cmp::readText(path); if (!L2.ok || L2.binary) return QString(); QByteArray raw = L2.raw; if (raw.startsWith("\xEF\xBB\xBF")) raw.remove(0, 3); return QString::fromUtf8(raw); };
    t.hasSelection = [] { return false; };
    t.selectionText = [] { return QString(); };
    t.apply = [](const QString&, bool) { return false; };   // files go through applyToFile (bank first)
    return t;
}

// The pure driver: normalize `before`, count the change from the diff, and
// (optionally) apply. Never touches disk for editor/compare targets.
inline RunResult run(const Target& t, const Options& o, bool houseStyle, bool apply, bool selectionScope) {
    using namespace allcore::textdiff;
    RunResult R;
    if (t.isManuscript) { R.message = "The Manuscript is rich text; Normalize works on plain text — use the Overlay document, the Draft, or a file in Compare. Format › Apply House Style Spacing handles the Manuscript's spacing."; return R; }
    if (!anyRule(o, houseStyle)) { R.message = "No rule is switched on — nothing to preview."; return R; }
    const QString before = selectionScope && t.hasSelection && t.hasSelection() ? t.selectionText() : (t.text ? t.text() : QString());
    R.before = before;
    const std::string detected = allcore::textnorm::detectEol(before.toStdString());
    const auto lines = splitLines(before.toStdString());
    const auto res = allcore::textnorm::normalizeText(lines, detected, o);
    // join: house style on LF text first (it is a QTextDocument pass), then the EOL
    QString joined = cmp::fromStd(res.lines).join('\n');   // explicit: no trailing break unless the text had one (or the rule adds it)
    const bool endedWithEol = before.endsWith('\n') || before.endsWith('\r');
    if (endedWithEol || res.finalNewlineAdded) joined += '\n';
    if (houseStyle) { QTextDocument tmp; tmp.setPlainText(joined); const auto hs = editops::HouseStyle::fromSettings(); if (hs.any()) editops::applyHouseStyleSpacing(&tmp, hs); joined = tmp.toPlainText(); if ((endedWithEol || res.finalNewlineAdded) && !joined.endsWith('\n')) joined += '\n'; }
    const QString eolTo = res.eol.empty() ? QString::fromStdString(detected) : QString::fromStdString(res.eol);
    R.eolFrom = eolName(QString::fromStdString(detected)); R.eolTo = eolName(eolTo);
    if (eolTo != "\n") joined.replace("\n", eolTo);
    R.after = joined;
    // counted from the diff the user will look at
    const Result d = diffLines(splitLines(before.toStdString()), splitLines(joined.toStdString()), allcore::textdiff::Options());
    R.changedLines = d.differences();
    for (const auto& rf : res.refused) { R.refused << QString("line %1: %2").arg(rf.line + 1).arg(QString::fromStdString(rf.reason)); R.refusedLines.push_back(rf.line + 1); }
    QString m = QString("%1 line%2 would change").arg(R.changedLines).arg(R.changedLines == 1 ? "" : "s");
    if (!res.refused.empty()) m += QString(" · %1 line%2 left alone (listed)").arg(res.refused.size()).arg(res.refused.size() == 1 ? "" : "s");
    if (res.shadRulesSkipped) m += QString(" · shad rules skipped on %1 non-Tibetan line%2").arg(res.shadRulesSkipped).arg(res.shadRulesSkipped == 1 ? "" : "s");
    if (R.eolFrom != R.eolTo) m += QString(" · line endings %1 → %2").arg(R.eolFrom, R.eolTo);
    if (res.finalNewlineAdded && !endedWithEol) m += " · final line break added";   // the engine guarantees one; only the caller knows whether it was missing
    R.message = m; R.ok = true;
    if (apply && t.kind != "file") {
        if (R.changedLines == 0 && R.eolFrom == R.eolTo && !res.finalNewlineAdded) { R.message += " — nothing applied"; return R; }
        R.applied = t.apply && t.apply(joined, selectionScope && t.hasSelection && t.hasSelection());
        if (R.applied) R.message += t.kind == "editor" ? " — applied in the editor (one Undo reverses it; Save keeps a version)" : " — applied to this side (Undo Merge reverts; Save writes)";
    }
    return R;
}

static std::function<void(const QString&)> g_fileNormalized;   // set in main(): reload the Overlay when it holds the file and is clean

// A file: strict UTF-8, BOM kept, EOL as chosen, pre-normalize version FIRST.
inline RunResult applyToFile(const Target& t, const Options& o, bool houseStyle) {
    RunResult R;
    const auto L = cmp::readText(t.path);
    if (!L.ok) { R.message = "Could not read " + t.path; return R; }
    if (L.binary) { R.message = "Refused: " + QFileInfo(t.path).fileName() + " is binary."; return R; }
    QByteArray raw = L.raw; const bool bom = raw.startsWith("\xEF\xBB\xBF"); if (bom) raw.remove(0, 3);
    int bad = 0; enc::decode(raw, "UTF-8", &bad);
    if (bad > 0) { R.message = QString("Refused: %1 byte(s) are not UTF-8 — open it in the Overlay and choose the encoding first.").arg(bad); return R; }
    R = run(t, o, houseStyle, false, false);
    if (!R.ok) return R;
    if (R.changedLines == 0 && R.eolFrom == R.eolTo && R.after == R.before) { R.message += " — nothing written"; return R; }
    const QString vn = docprops::noteVersion(nullptr, t.dataRoot, t.path, L.raw, t.docKind, "pre-normalize", "", "UTF-8", eolName(L.eol), bom, QJsonObject());
    if (vn.startsWith("Version NOT kept") || (vn.isEmpty() && !t.dataRoot.isEmpty() && docprops::versionsEnabled())) { R.message = "Refused: the original could not be kept as a version (" + (vn.isEmpty() ? QString("no version written") : vn) + ") — nothing changed."; R.ok = false; return R; }
    R.versionNotice = vn;
    QByteArray out; if (bom) out.append("\xEF\xBB\xBF", 3); out += R.after.toUtf8();
    if (!saveOrWarn(nullptr, t.path, out, "the normalized file")) { R.message = "The file could not be written."; R.ok = false; return R; }
    R.applied = true;
    R.message += vn.isEmpty() ? " — written (versions are off, so no copy of the original was kept)" : " — written; the original is kept as a version (" + vn + ")";
    if (g_fileNormalized) g_fileNormalized(t.path);
    return R;
}

// Presets: release-owned <dataRoot>/data/house_style/normalize_presets.json plus the user's own.
struct Preset { QString name, source; Options options; bool user = false; };
inline std::vector<Preset> presets(const QString& dataRoot) {
    std::vector<Preset> v;
    QFile f(dataRoot + "/data/house_style/normalize_presets.json");
    if (f.open(QIODevice::ReadOnly)) for (const auto& x : QJsonDocument::fromJson(f.readAll()).object().value("presets").toArray()) { const auto j = x.toObject(); v.push_back({j["name"].toString(), j["source"].toString(), fromJson(j["options"].toObject()), false}); }
    for (const auto& x : QJsonDocument::fromJson(QSettings("ALL", "TranslationTool").value("normalize/presets").toByteArray()).array()) { const auto j = x.toObject(); v.push_back({j["name"].toString(), "saved by you", fromJson(j["options"].toObject()), true}); }
    return v;
}
inline void saveUserPreset(const QString& name, const Options& o) {
    QSettings st("ALL", "TranslationTool");
    QJsonArray arr = QJsonDocument::fromJson(st.value("normalize/presets").toByteArray()).array();
    QJsonArray out; for (const auto& x : arr) if (x.toObject()["name"].toString() != name) out.push_back(x);
    QJsonObject j; j["name"] = name; j["options"] = toJson(o); out.push_back(j);
    st.setValue("normalize/presets", QJsonDocument(out).toJson(QJsonDocument::Compact));
}

class NormalizeDialog : public QDialog {
public:
    NormalizeDialog(const Target& t, QWidget* parent = nullptr) : QDialog(parent), t_(t) {
        setWindowFlag(Qt::Window); setWindowTitle("Normalize Text"); setModal(false); resize(720, 640);
        auto* v = new QVBoxLayout(this);
        auto* banner = new QLabel("Every rule is a mechanical transformation, described beside it. The GMR / input-centre formatting sheet has not been supplied; when it is, its rulings become presets here. "
                                  "Nothing is guessed: a line the rules cannot classify is left byte-identical and listed. Normalize works strictly per line and never joins or splits lines.");
        banner->setWordWrap(true); ux::themedStyle(banner, [] { return QString("color:%1;")
            .arg(ux::chromeMuted()); }); v->addWidget(banner);
        target_ = new QLabel(t.label); target_->setWordWrap(true); v->addWidget(target_);
        auto* scope = new QHBoxLayout; whole_ = new QRadioButton("Whole document"); sel_ = new QRadioButton("Selection"); whole_->setChecked(true);
        const bool hasSel = t.hasSelection && t.hasSelection(); sel_->setEnabled(hasSel); if (hasSel) { sel_->setChecked(true); sel_->setText("Selection (extended to whole lines — every rule is line-based)"); }
        if (t.kind != "editor") { whole_->setText(t.kind == "file" ? "Whole file" : "Whole side"); sel_->setVisible(false); }
        scope->addWidget(whole_); scope->addWidget(sel_); scope->addStretch(1); v->addLayout(scope);
        auto* f = new QFormLayout;
        collapse_ = new QCheckBox("Collapse runs of spaces and tabs to one space"); f->addRow(collapse_);
        trim_ = new QCheckBox("Remove trailing spaces"); f->addRow(trim_);
        auto* bl = new QHBoxLayout; blank_ = new QCheckBox("Reduce runs of blank lines to at most"); blankN_ = new QSpinBox; blankN_->setRange(0, 5); blankN_->setValue(1); blankN_->setEnabled(false); bl->addWidget(blank_); bl->addWidget(blankN_); bl->addStretch(1); f->addRow(bl);
        auto* ds = new QHBoxLayout; dshad_ = new QCheckBox("Write the double shad as"); dshadForm_ = new QComboBox; dshadForm_->addItems({",,", ", ,", "།།", "༎"}); dshadForm_->setEnabled(false);
        auto* dsNote = new QLabel("awaiting the house ruling (Preferences › House Style)"); ux::themedStyle(dsNote, [] { return QString("color:%1;")
            .arg(ux::chromeMuted()); }); ds->addWidget(dshad_); ds->addWidget(dshadForm_); ds->addWidget(dsNote); ds->addStretch(1); f->addRow(ds);
        auto* sb = new QHBoxLayout; sbefore_ = new QCheckBox("Space before a shad:"); sbeforeForm_ = new QComboBox; sbeforeForm_->addItems({"none (GA, / ཀ།)", "one (GA , / ཀ །)"}); sbeforeForm_->setEnabled(false);
        auto* sbNote = new QLabel("awaiting the house ruling"); ux::themedStyle(sbNote, [] { return QString("color:%1;")
            .arg(ux::chromeMuted()); }); sb->addWidget(sbefore_); sb->addWidget(sbeforeForm_); sb->addWidget(sbNote); sb->addStretch(1); f->addRow(sb);
        safter_ = new QCheckBox("One space after a shad"); f->addRow(safter_);
        appar_ = new QCheckBox("Remove [ ] and { } apparatus spans — removes text; the original is kept as a version"); ux::themedStyle(appar_, [] { return QString("color:%1;")
            .arg(ux::chromeError()); }); f->addRow(appar_);
        folio_ = new QCheckBox("Remove folio markers — removes text; folio citations in reports depend on these markers"); ux::themedStyle(folio_, [] { return QString("color:%1;")
            .arg(ux::chromeError()); }); f->addRow(folio_);
        eol_ = new QComboBox; eol_->addItems({"keep", "LF", "CRLF", "CR"}); f->addRow("Line endings:", eol_);
        final_ = new QCheckBox("Ensure the file ends with a line break"); f->addRow(final_);
        house_ = new QCheckBox("Apply house-style spacing to English lines (after the rules above)");
        if (!editops::HouseStyle::fromSettings().any()) { house_->setEnabled(false); house_->setToolTip("No House Style switch is on (Preferences › House Style)"); }
        f->addRow(house_);
        v->addLayout(f);
        auto* pr = new QHBoxLayout; presetBox_ = new QComboBox; presetBox_->setVisible(false); pr->addWidget(presetBox_);
        auto* savePreset = new QPushButton("Save as preset…"); pr->addWidget(savePreset); pr->addStretch(1); v->addLayout(pr);
        connect(savePreset, &QPushButton::clicked, this, [this] { const QString n = docprops::askName(this, "Save Normalize Preset", "Preset name:", QString()); if (n.isEmpty()) return; saveUserPreset(n, options()); loadPresets(); result_->setText("Preset saved: " + n); });
        auto* acts = new QHBoxLayout;
        preview_ = new QPushButton("Preview in Compare"); apply_ = new QPushButton("Apply"); apply_->setEnabled(false); apply_->setToolTip("Preview first: Apply is enabled only for a preview computed with exactly these settings and this text");
        listBtn_ = new QPushButton("List untouched lines"); listBtn_->setCheckable(true); listBtn_->setEnabled(false);
        auto* close = new QPushButton("Close");
        acts->addWidget(preview_); acts->addWidget(apply_); acts->addWidget(listBtn_); acts->addStretch(1); acts->addWidget(close); v->addLayout(acts);
        result_ = new QLabel; result_->setWordWrap(true); v->addWidget(result_);
        untouched_ = new QListWidget; untouched_->setVisible(false); untouched_->setMaximumHeight(140); v->addWidget(untouched_);
        connect(untouched_, &QListWidget::itemActivated, this, [this](QListWidgetItem* it) { if (t_.jumpToLine) t_.jumpToLine(it->data(Qt::UserRole).toInt()); });
        connect(listBtn_, &QPushButton::toggled, this, [this](bool on) { untouched_->setVisible(on); });
        connect(preview_, &QPushButton::clicked, this, [this] { preview(); });
        connect(apply_, &QPushButton::clicked, this, [this] { apply(); });
        connect(close, &QPushButton::clicked, this, [this] { remember(); this->close(); });
        connect(blank_, &QCheckBox::toggled, blankN_, &QSpinBox::setEnabled);
        connect(dshad_, &QCheckBox::toggled, dshadForm_, &QComboBox::setEnabled);
        connect(sbefore_, &QCheckBox::toggled, sbeforeForm_, &QComboBox::setEnabled);
        for (auto* cb : {collapse_, trim_, blank_, dshad_, sbefore_, safter_, appar_, folio_, final_, house_}) connect(cb, &QCheckBox::toggled, this, [this](bool) { invalidate(); });
        for (auto* c : {dshadForm_, sbeforeForm_, eol_}) connect(c, qOverload<int>(&QComboBox::currentIndexChanged), this, [this](int) { invalidate(); });
        connect(blankN_, qOverload<int>(&QSpinBox::valueChanged), this, [this](int) { invalidate(); });
        connect(whole_, &QRadioButton::toggled, this, [this](bool) { invalidate(); });
        setOptions(fromJson(QJsonDocument::fromJson(QSettings("ALL", "TranslationTool").value("normalize/last").toByteArray()).object()));
        loadPresets();
        invalidate();
    }
    Options options() const {
        Options o;
        o.collapseSpaces = collapse_->isChecked(); o.trimTrailing = trim_->isChecked(); o.maxBlankLines = blank_->isChecked() ? blankN_->value() : -1;
        if (dshad_->isChecked()) o.doubleShad = dshadForm_->currentText().toStdString();
        o.spaceBeforeShad = !sbefore_->isChecked() ? ShadSpace::Keep : sbeforeForm_->currentIndex() == 0 ? ShadSpace::None : ShadSpace::One;
        o.spaceAfterShad = safter_->isChecked(); o.stripApparatus = appar_->isChecked(); o.stripFolioMarkers = folio_->isChecked();
        o.eol = Eol(eol_->currentIndex()); o.ensureFinalNewline = final_->isChecked();
        return o;
    }
    void setOptions(const Options& o) {
        collapse_->setChecked(o.collapseSpaces); trim_->setChecked(o.trimTrailing); blank_->setChecked(o.maxBlankLines >= 0); if (o.maxBlankLines >= 0) blankN_->setValue(o.maxBlankLines);
        dshad_->setChecked(!o.doubleShad.empty()); if (!o.doubleShad.empty()) dshadForm_->setCurrentText(QString::fromStdString(o.doubleShad));
        sbefore_->setChecked(o.spaceBeforeShad != ShadSpace::Keep); sbeforeForm_->setCurrentIndex(o.spaceBeforeShad == ShadSpace::One ? 1 : 0);
        safter_->setChecked(o.spaceAfterShad); appar_->setChecked(o.stripApparatus); folio_->setChecked(o.stripFolioMarkers); eol_->setCurrentIndex(int(o.eol)); final_->setChecked(o.ensureFinalNewline);
    }
    bool houseStyle() const { return house_->isEnabled() && house_->isChecked(); }
    bool selectionScope() const { return sel_->isVisible() && sel_->isEnabled() && sel_->isChecked(); }
    bool applyEnabled() const { return apply_->isEnabled(); }
    QString resultText() const { return result_->text(); }
    QString settingsKey() const {
        const QString text = selectionScope() && t_.selectionText ? t_.selectionText() : (t_.text ? t_.text() : QString());
        return QString::fromLatin1(QCryptographicHash::hash((QJsonDocument(toJson(options())).toJson(QJsonDocument::Compact) + (houseStyle() ? "H" : "-") + (selectionScope() ? "S" : "W") + text.toUtf8()), QCryptographicHash::Sha1).toHex());
    }
    void invalidate() { apply_->setEnabled(false); previewKey_.clear(); }
    RunResult preview() {
        last_ = run(t_, options(), houseStyle(), false, selectionScope());
        result_->setText(last_.message);
        untouched_->clear(); for (int i = 0; i < last_.refused.size(); ++i) { auto* it = new QListWidgetItem(last_.refused[i]); it->setData(Qt::UserRole, last_.refusedLines[i]); untouched_->addItem(it); }
        listBtn_->setEnabled(!last_.refused.isEmpty());
        if (!last_.ok) { invalidate(); return last_; }
        if (g_compareTexts) g_compareTexts(t_.label + " (current)", last_.before, t_.label + " (normalized)", last_.after);
        previewKey_ = settingsKey(); apply_->setEnabled(true);
        return last_;
    }
    RunResult apply() {
        if (!apply_->isEnabled() || previewKey_ != settingsKey()) { invalidate(); result_->setText("The text or the settings changed since the preview — preview again before applying."); RunResult r; r.message = result_->text(); return r; }
        RunResult r = t_.kind == "file" ? applyToFile(t_, options(), houseStyle()) : run(t_, options(), houseStyle(), true, selectionScope());
        result_->setText(r.message); remember(); invalidate();
        return r;
    }
    void remember() { QSettings("ALL", "TranslationTool").setValue("normalize/last", QJsonDocument(toJson(options())).toJson(QJsonDocument::Compact)); }
    QComboBox* presetBox() const { return presetBox_; }
    // F4 embeds the rule panel inside Replace in Files (Normalize mode): no own target, scope or actions
    void setEmbedded() {
        setWindowFlags(Qt::Widget);
        target_->setVisible(false); whole_->setVisible(false); sel_->setVisible(false);
        preview_->setVisible(false); apply_->setVisible(false); listBtn_->setVisible(false); result_->setVisible(false); untouched_->setVisible(false);
        for (QObject* c : children()) if (auto* b = qobject_cast<QPushButton*>(c)) if (b->text() == "Close") b->setVisible(false);
    }

private:
    void loadPresets() {
        const auto ps = presets(t_.dataRoot);
        presetBox_->blockSignals(true); presetBox_->clear();
        presetBox_->setVisible(!ps.empty());   // no empty control on first run: appears when the sheet arrives or the user saves one
        if (ps.empty()) { presetBox_->blockSignals(false); return; }
        presetBox_->addItem("Presets…");
        for (const auto& p : ps) { presetBox_->addItem(p.name); presetBox_->setItemData(presetBox_->count() - 1, "Source: " + p.source, Qt::ToolTipRole); }
        presetBox_->blockSignals(false);
        connect(presetBox_, qOverload<int>(&QComboBox::activated), this, [this, ps](int i) { if (i >= 1 && i - 1 < (int)ps.size()) { setOptions(ps[i - 1].options); result_->setText("Preset loaded: " + ps[i - 1].name + " (" + ps[i - 1].source + ")"); } }, Qt::UniqueConnection);
    }
    Target t_;
    QLabel* target_ = nullptr; QRadioButton* whole_ = nullptr; QRadioButton* sel_ = nullptr;
    QCheckBox *collapse_ = nullptr, *trim_ = nullptr, *blank_ = nullptr, *dshad_ = nullptr, *sbefore_ = nullptr, *safter_ = nullptr, *appar_ = nullptr, *folio_ = nullptr, *final_ = nullptr, *house_ = nullptr;
    QSpinBox* blankN_ = nullptr; QComboBox *dshadForm_ = nullptr, *sbeforeForm_ = nullptr, *eol_ = nullptr, *presetBox_ = nullptr;
    QPushButton *preview_ = nullptr, *apply_ = nullptr, *listBtn_ = nullptr; QLabel* result_ = nullptr; QListWidget* untouched_ = nullptr;
    QString previewKey_; RunResult last_;
};

// Open the dialog for a target (no-op under the harness: the selftests drive run()/the dialog directly).
inline void open(const Target& t, QWidget* parent, const std::function<void(const QString&)>& msg) {
    if (t.isManuscript) { if (msg) msg(run(t, Options(), false, false, false).message); return; }
    if (g_harnessRun) return;
    auto* d = new NormalizeDialog(t, parent); d->setAttribute(Qt::WA_DeleteOnClose); d->show(); d->raise(); d->activateWindow();
}

// ---- selftests 17–23 (called from the main selftest driver, where every pane exists)
inline void selfTest(OverlayPane* overlay, ComparePane* comparePane, const std::function<QString()>& manuscriptText, const QString& dataRoot,
                     const std::function<void(bool, const char*)>& check) {   // the Manuscript is defined later in the TU, so it arrives as a hook
    auto* pe = overlay->documentEditor();
    const QString keepText = pe->toPlainText();
    pe->setPlainText("BLA  MA ,LA\n");
    Options o; o.collapseSpaces = true; o.spaceBeforeShad = ShadSpace::None; o.spaceAfterShad = true;
    Target t = editorTarget(pe, "Document box");
    RunResult r = run(t, o, false, false, false);
    if (g_compareTexts) g_compareTexts(t.label + " (current)", r.before, t.label + " (normalized)", r.after);
    check(r.ok && r.after == "BLA MA, LA\n" && pe->toPlainText() == "BLA  MA ,LA\n" && r.changedLines == 1, "F3-17 preview returns the normalized text without changing the editor, counted from the diff");
    check(comparePane->sideName(true).endsWith("(current)") && comparePane->sideName(false).endsWith("(normalized)") && comparePane->result().differences() == 1, "F3-17b the Compare pane shows current vs normalized with one difference");
    r = run(t, o, false, true, false);
    check(r.applied && pe->toPlainText() == "BLA MA, LA\n" && overlay->isDirty(), "F3-18 apply changes the editor in one edit block and the document is dirty");
    pe->document()->undo();
    check(pe->toPlainText() == "BLA  MA ,LA\n", "F3-18b a single Undo restores the original");
    {   // file target: CRLF + BOM → LF, BOM kept, pre-normalize version banked
        const QString td = QDir::tempPath() + "/dct_f3_" + QString::number(QCoreApplication::applicationPid()); QDir().mkpath(td);
        const QString fp = td + "/crlf.act"; const QByteArray orig("\xEF\xBB\xBF" "KA  KHA\r\nGA\r\n");   // 16 bytes: BOM + two CRLF lines
        { QFile f(fp); if (f.open(QIODevice::WriteOnly)) f.write(orig); }
        QSettings vst("ALL", "TranslationTool"); const QVariant was = vst.value("versions/enabled"); vst.setValue("versions/enabled", true);
        const QString vdir = docprops::versionsDir(dataRoot, fp); QDir(vdir).removeRecursively();
        Options fo; fo.collapseSpaces = true; fo.eol = Eol::LF;
        RunResult fr = applyToFile(fileTarget(fp, dataRoot), fo, false);
        QFile rf(fp); const QByteArray got = rf.open(QIODevice::ReadOnly) ? rf.readAll() : QByteArray();
        check(fr.applied && got.startsWith("\xEF\xBB\xBF") && !got.contains('\r') && got.endsWith('\n') && got == QByteArray("\xEF\xBB\xBF" "KA KHA\nGA\n"), ("F3-19 a CRLF+BOM file becomes LF with the BOM kept [" + fr.message + "]").toUtf8().constData());
        auto ents = allcore::versions::list(vdir.toStdString()); QByteArray banked; bool okB = !ents.empty() && ents[0].meta.reason == "pre-normalize" && docprops::versionBytes(vdir, ents[0], banked) && banked == orig;
        check(okB, "F3-19b the pre-normalize version holds the original bytes");
        const QString bp = td + "/bin.dat"; { QFile f(bp); if (f.open(QIODevice::WriteOnly)) f.write(QByteArray("ab\0cd", 5)); }
        RunResult br = applyToFile(fileTarget(bp, dataRoot), fo, false);
        const QString lp = td + "/latin1.txt"; { QFile f(lp); if (f.open(QIODevice::WriteOnly)) f.write(QByteArray("caf\xE9  x\n")); }
        RunResult lr = applyToFile(fileTarget(lp, dataRoot), fo, false);
        QFile lf(lp); const QByteArray lfNow = lf.open(QIODevice::ReadOnly) ? lf.readAll() : QByteArray();
        check(!br.applied && br.message.contains("binary") && !lr.applied && lr.message.contains("not UTF-8") && lfNow == QByteArray("caf\xE9  x\n"), "F3-20 binary and non-UTF-8 files are refused and left untouched");
        if (was.isValid()) vst.setValue("versions/enabled", was); else vst.remove("versions/enabled");
        QDir(vdir).removeRecursively(); QDir(td).removeRecursively();
    }
    {   // Manuscript refused
        Target m; m.isManuscript = true; m.text = manuscriptText;
        const QString before = manuscriptText();
        RunResult mr = run(m, o, false, true, false);
        check(!mr.ok && mr.message.contains("rich text") && manuscriptText() == before, "F3-21 the Manuscript is refused with the stated message and nothing changes");
    }
    {   // Compare pane side
        comparePane->compareTexts("L", "KA\n", "R", "KA  KHA\n");
        const int depth = comparePane->undoDepth();
        Options co; co.collapseSpaces = true;
        Target ct; ct.kind = "compare"; ct.text = [comparePane] { return comparePane->sideLines(false).join('\n') + "\n"; };
        ct.hasSelection = [] { return false; }; ct.selectionText = [] { return QString(); };
        ct.apply = [comparePane](const QString& s, bool) { comparePane->setSideLines(false, cmp::linesOf(s)); return true; };
        RunResult cr = run(ct, co, false, true, false);
        check(cr.applied && comparePane->sideLines(false) == QStringList{"KA KHA"} && comparePane->undoDepth() == depth + 1, "F3-22 Normalize Right pushes exactly one undo snapshot");
        check(comparePane->undo() && comparePane->sideLines(false) == QStringList{"KA  KHA"}, "F3-22b Undo restores the right side");
    }
    {   // Apply gating
        pe->setPlainText("A  B\n");
        NormalizeDialog d(editorTarget(pe, "Document box"));
        d.setOptions(Options()); Options go; go.collapseSpaces = true; d.setOptions(go);
        check(!d.applyEnabled(), "F3-23 Apply is disabled before any preview");
        d.preview();
        check(d.applyEnabled(), "F3-23b …enabled after a preview for these settings");
        Options go2 = go; go2.trimTrailing = true; d.setOptions(go2);
        check(!d.applyEnabled(), "F3-23c …and disabled again the moment a switch changes");
        d.preview(); pe->setPlainText("A  B  C\n");
        RunResult ar = d.apply();
        check(!ar.applied && ar.message.contains("preview again"), "F3-23d a changed text since the preview refuses to apply");
    }
    pe->setPlainText(keepText);
}

}  // namespace normalize
