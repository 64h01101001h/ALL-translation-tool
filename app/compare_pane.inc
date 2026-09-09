// app/compare_pane.inc — the Compare & Merge pane (2026-09-08). Included
// from main.cpp before FilesPane; relies on NumberedEdit, RibbonBar,
// miniIcon, saveOrWarn, safeGet*, editops, docprops::askName defined above.
// (Qt/allcore includes for this file live at the top of main.cpp — this
// file is included inside main.cpp's anonymous namespace.)

static std::function<void(const QString&, const QString&)> g_compareFiles;     // (leftPath, rightPath)
static std::function<void(const QString&, const QString&)> g_compareFolders;   // (leftDir, rightDir)
static std::function<void(const QString&, const QString&, const QString&, const QString&)> g_compareTexts;  // (aName,aText,bName,bText)

namespace cmp {
using namespace allcore::textdiff;

inline std::vector<std::string> toStd(const QStringList& l) { std::vector<std::string> v; v.reserve(l.size()); for (const auto& s : l) v.push_back(s.toStdString()); return v; }
inline QStringList fromStd(const std::vector<std::string>& v) { QStringList l; l.reserve(int(v.size())); for (const auto& s : v) l << QString::fromStdString(s); return l; }
inline QStringList linesOf(const QString& text) { return fromStd(splitLines(text.toStdString())); }
inline int u16(const std::string& s, int byteOff) { return QString::fromUtf8(s.data(), std::max(0, std::min(byteOff, (int)s.size()))).size(); }

struct Loaded { QString text; QString eol = "\n"; bool ok = false; bool binary = false; qint64 bytes = 0; int firstDiffOffset = -1; QByteArray raw; };
inline Loaded readText(const QString& path) {
    Loaded L; QFile f(path);
    if (!f.open(QIODevice::ReadOnly)) return L;
    L.raw = f.readAll(); L.bytes = L.raw.size(); L.ok = true;
    L.binary = L.raw.left(8192).contains('\0');
    if (L.binary) return L;
    L.eol = L.raw.contains("\r\n") ? "\r\n" : (L.raw.contains('\r') ? "\r" : "\n");
    L.text = QString::fromUtf8(L.raw);
    return L;
}
inline int firstDifferingByte(const QByteArray& a, const QByteArray& b) {
    const int n = std::min(a.size(), b.size());
    for (int i = 0; i < n; ++i) if (a[i] != b[i]) return i;
    return a.size() == b.size() ? -1 : n;
}

inline QColor colChange() { return QColor(0xFF, 0xF3, 0xC4); }
inline QColor colInsert() { return QColor(0xD9, 0xF5, 0xD3); }
inline QColor colDelete() { return QColor(0xF8, 0xD7, 0xD5); }
inline QColor colMinor()  { return QColor(0xEC, 0xEC, 0xEC); }
inline QColor colMoved()  { return QColor(0xDB, 0xE7, 0xFF); }
inline QColor colFiller() { return QColor(0xF4, 0xF1, 0xEA); }
inline QColor colMark()   { return QColor(0xFF, 0xD5, 0x4A); }
inline QColor colFor(const Hunk& h) {
    if (h.kind == Kind::Equal) return Qt::transparent;
    if (h.unimportant) return colMinor();
    if (h.movedPeer >= 0) return colMoved();
    return h.kind == Kind::Change ? colChange() : h.kind == Kind::Insert ? colInsert() : colDelete();
}
inline QString kindName(const Hunk& h) {
    if (h.unimportant) return "minor difference";
    if (h.movedPeer >= 0) return "moved block";
    return h.kind == Kind::Change ? "changed" : h.kind == Kind::Insert ? "only on the right" : "only on the left";
}

// Display rows: aligned pairs of line indices (-1 = filler), or a gap row.
struct Row { int a = -1, b = -1, hunk = -1, gap = 0; };
enum class Show { All, DiffsOnly, Context };
inline std::vector<Row> buildRows(const Result& r, Show show, int ctx, bool minorAsDiff) {
    std::vector<Row> rows;
    auto isDiff = [&](const Hunk& h) { return h.kind != Kind::Equal && (minorAsDiff || !h.unimportant); };
    for (size_t i = 0; i < r.hunks.size(); ++i) {
        const Hunk& h = r.hunks[i];
        if (!isDiff(h)) {
            const int n = std::max(h.aEnd - h.aBeg, h.bEnd - h.bBeg);   // equal (or minor-as-equal): 1:1
            if (show == Show::All) { for (int k = 0; k < n; ++k) rows.push_back({h.aBeg + k, h.bBeg + k, (int)i, 0}); continue; }
            const int c = show == Show::DiffsOnly ? 0 : ctx;
            bool prevDiff = false, nextDiff = false;
            for (size_t j = i; j-- > 0;) { if (isDiff(r.hunks[j])) { prevDiff = true; break; } }
            for (size_t j = i + 1; j < r.hunks.size(); ++j) { if (isDiff(r.hunks[j])) { nextDiff = true; break; } }
            int head = prevDiff ? std::min(n, c) : 0, tail = nextDiff ? std::min(n, c) : 0;
            if (head + tail >= n) { head = n; tail = 0; }
            for (int k = 0; k < head; ++k) rows.push_back({h.aBeg + k, h.bBeg + k, (int)i, 0});
            if (n - head - tail > 0) rows.push_back({-1, -1, (int)i, n - head - tail});
            for (int k = n - tail; k < n; ++k) rows.push_back({h.aBeg + k, h.bBeg + k, (int)i, 0});
            continue;
        }
        const int n = std::max(h.aEnd - h.aBeg, h.bEnd - h.bBeg);
        for (int k = 0; k < n; ++k) rows.push_back({h.aBeg + k < h.aEnd ? h.aBeg + k : -1, h.bBeg + k < h.bEnd ? h.bBeg + k : -1, (int)i, 0});
    }
    return rows;
}

// Session / options ↔ JSON
inline QJsonObject optionsJson(const Options& o) {
    QJsonObject j;
    j["ignoreCase"] = o.ignoreCase; j["ignoreWhitespace"] = o.ignoreWhitespace; j["ignoreWhitespaceChange"] = o.ignoreWhitespaceChange;
    j["ignoreBlankLines"] = o.ignoreBlankLines; j["ignoreTibetanPunct"] = o.ignoreTibetanPunct; j["ignoreFolioMarkers"] = o.ignoreFolioMarkers;
    j["ignoreApparatus"] = o.ignoreApparatus; j["scriptAgnostic"] = o.scriptAgnostic; j["detectMoves"] = o.detectMoves;
    QJsonArray rx; for (const auto& r : o.ignoreRegex) rx.append(QString::fromStdString(r)); j["ignoreRegex"] = rx;
    QJsonArray sb; for (const auto& s : o.substitutions) { QJsonObject x; x["from"] = QString::fromStdString(s.first); x["to"] = QString::fromStdString(s.second); sb.append(x); } j["substitutions"] = sb;
    return j;
}
inline Options optionsFrom(const QJsonObject& j) {
    Options o;
    o.ignoreCase = j["ignoreCase"].toBool(); o.ignoreWhitespace = j["ignoreWhitespace"].toBool(); o.ignoreWhitespaceChange = j["ignoreWhitespaceChange"].toBool();
    o.ignoreBlankLines = j["ignoreBlankLines"].toBool(); o.ignoreTibetanPunct = j["ignoreTibetanPunct"].toBool(); o.ignoreFolioMarkers = j["ignoreFolioMarkers"].toBool();
    o.ignoreApparatus = j["ignoreApparatus"].toBool(); o.scriptAgnostic = j["scriptAgnostic"].toBool(); o.detectMoves = j.contains("detectMoves") ? j["detectMoves"].toBool() : true;
    for (const auto& v : j["ignoreRegex"].toArray()) o.ignoreRegex.push_back(v.toString().toStdString());
    for (const auto& v : j["substitutions"].toArray()) o.substitutions.push_back({v.toObject()["from"].toString().toStdString(), v.toObject()["to"].toString().toStdString()});
    return o;
}
}  // namespace cmp

// An editor that paints per-row change bars at its left edge and exposes
// the block geometry the link strip and location pane need.
class DiffEdit : public NumberedEdit {
public:
    explicit DiffEdit(QWidget* parent = nullptr) : NumberedEdit(parent) {
        setLineWrapMode(QPlainTextEdit::NoWrap);
        QFont f = font(); f.setPointSize(std::max(11, f.pointSize())); setFont(f);
    }
    void setRowColors(QVector<QColor> c) { rowColors_ = std::move(c); viewport()->update(); }
    QTextBlock firstVisible() const { return firstVisibleBlock(); }
    QRectF blockRect(const QTextBlock& b) const { return blockBoundingGeometry(b).translated(contentOffset()); }
    QRectF blockBounds(const QTextBlock& b) const { return blockBoundingRect(b); }
protected:
    void paintEvent(QPaintEvent* e) override {
        QPlainTextEdit::paintEvent(e);
        if (rowColors_.isEmpty()) return;
        QPainter p(viewport());
        QTextBlock b = firstVisibleBlock();
        while (b.isValid()) {
            const QRectF r = blockRect(b);
            if (r.top() > viewport()->height()) break;
            if (b.blockNumber() < rowColors_.size() && rowColors_[b.blockNumber()].alpha() > 0)
                p.fillRect(QRectF(0, r.top(), 4, r.height()), rowColors_[b.blockNumber()].darker(130));
            b = b.next();
        }
    }
private:
    QVector<QColor> rowColors_;
};

class ComparePane : public QWidget {
public:
    using Options = allcore::textdiff::Options;
    using Result = allcore::textdiff::Result;
    using Hunk = allcore::textdiff::Hunk;
    using Kind = allcore::textdiff::Kind;

    explicit ComparePane(const QString& root, QWidget* parent = nullptr) : QWidget(parent), root_(root) {
        auto* outer = new QVBoxLayout(this);
        outer->setContentsMargins(0, 0, 0, 0); outer->setSpacing(0);
        buildRibbon();
        // ---- page switcher (Text · Folders · Three-Way) + stacked pages
        auto* switcher = new QHBoxLayout; switcher->setContentsMargins(6, 3, 6, 0); switcher->setSpacing(4);
        pageBtns_ = new QButtonGroup(this);
        auto pageBtn = [&](const QString& label, int id) { auto* b = new QToolButton; b->setText(label); b->setCheckable(true); b->setAutoRaise(true); pageBtns_->addButton(b, id); switcher->addWidget(b); return b; };
        pageBtn("Text Compare", 0)->setChecked(true); pageBtn("Folder Compare", 1); pageBtn("Three-Way Merge", 2);
        switcher->addStretch(1);
        outer->addLayout(switcher);
        pages_ = new QStackedWidget;
        textPage_ = new QWidget; auto* tp = new QVBoxLayout(textPage_); tp->setContentsMargins(0, 0, 0, 0); tp->setSpacing(0);
        pages_->addWidget(textPage_);
        outer->addWidget(pages_, 1);
        connect(pageBtns_, &QButtonGroup::idClicked, this, [this](int id) { if (id < pages_->count()) pages_->setCurrentIndex(id); });
        // ---- header: the two names
        auto* head = new QHBoxLayout;
        nameL_ = new QLabel("(left) — Compare Files… or Compare with Clipboard to begin"); nameL_->setTextInteractionFlags(Qt::TextSelectableByMouse);
        nameR_ = new QLabel("(right)"); nameR_->setTextInteractionFlags(Qt::TextSelectableByMouse);
        for (auto* l : {nameL_, nameR_}) { l->setStyleSheet("font-weight:600; padding:3px 6px;"); l->setWordWrap(false); }
        head->addWidget(nameL_, 1); head->addSpacing(48); head->addWidget(nameR_, 1); head->addSpacing(16);
        tp->addLayout(head);
        // ---- body: left | link strip | right | location pane
        auto* body = new QHBoxLayout; body->setSpacing(0); body->setContentsMargins(0, 0, 0, 0);
        left_ = new DiffEdit; right_ = new DiffEdit;
        link_ = new LinkStrip(this); loc_ = new LocationPane(this);
        body->addWidget(left_, 1); body->addWidget(link_); body->addWidget(right_, 1); body->addWidget(loc_);
        auto* bodyW = new QWidget; bodyW->setLayout(body);
        // ---- line details
        details_ = new QWidget;
        {
            auto* dv = new QVBoxLayout(details_); dv->setContentsMargins(4, 2, 4, 2); dv->setSpacing(2);
            detailLbl_ = new QLabel("No difference selected."); detailLbl_->setStyleSheet("color:#6B5E4E; font-size:11px;");
            dv->addWidget(detailLbl_);
            auto* dh = new QHBoxLayout;
            detL_ = new QPlainTextEdit; detR_ = new QPlainTextEdit;
            for (auto* d : {detL_, detR_}) { d->setReadOnly(true); d->setMaximumHeight(96); d->setLineWrapMode(QPlainTextEdit::WidgetWidth); }
            dh->addWidget(detL_, 1); dh->addWidget(detR_, 1);
            dv->addLayout(dh);
        }
        auto* split = new QSplitter(Qt::Vertical);
        split->addWidget(bodyW); split->addWidget(details_); split->setStretchFactor(0, 1); split->setStretchFactor(1, 0);
        tp->addWidget(split, 1);
        // ---- status
        status_ = new QLabel("Ready."); status_->setStyleSheet("padding:2px 6px; color:#4A3F33;");
        outer->addWidget(status_);

        // scroll sync
        auto sync = [this](QScrollBar* from, QScrollBar* to) {
            connect(from, &QScrollBar::valueChanged, this, [this, to](int v) { if (syncing_) return; syncing_ = true; to->setValue(v); syncing_ = false; link_->update(); loc_->update(); });
        };
        sync(left_->verticalScrollBar(), right_->verticalScrollBar()); sync(right_->verticalScrollBar(), left_->verticalScrollBar());
        sync(left_->horizontalScrollBar(), right_->horizontalScrollBar()); sync(right_->horizontalScrollBar(), left_->horizontalScrollBar());
        // edits → rescan when idle
        rescan_.setSingleShot(true); rescan_.setInterval(900);
        connect(&rescan_, &QTimer::timeout, this, [this] { rescanFromEditors(); });
        for (auto* e : {left_, right_}) connect(e, &QPlainTextEdit::textChanged, this, [this] { if (!rendering_ && show_ == cmp::Show::All) { dirtyEditors_ = true; rescan_.start(); } });
        connect(left_, &QPlainTextEdit::cursorPositionChanged, this, [this] { if (!rendering_) selectHunkAtCursor(left_); });
        connect(right_, &QPlainTextEdit::cursorPositionChanged, this, [this] { if (!rendering_) selectHunkAtCursor(right_); });
        // shortcuts (pane-local)
        auto sc = [this](const char* k, std::function<void()> f) { auto* s = new QShortcut(QKeySequence(k), this); s->setContext(Qt::WidgetWithChildrenShortcut); connect(s, &QShortcut::activated, this, std::move(f)); };
        sc("F8", [this] { step(+1); }); sc("F7", [this] { step(-1); });
        sc("Alt+Right", [this] { copyCurrent(true, false); }); sc("Alt+Left", [this] { copyCurrent(false, false); });
        sc("F5", [this] { rescanFromEditors(); });
        setLines({}, {}, "", "");
    }

    // ------------------------------------------------------------- pages
    void addPage(QWidget* w) { pages_->addWidget(w); }
    void showPage(int id) { if (id < pages_->count()) { pages_->setCurrentIndex(id); if (auto* b = pageBtns_->button(id)) b->setChecked(true); } }
    int currentPage() const { return pages_->currentIndex(); }
    QLabel* statusLabel() const { return status_; }
    const QString& root() const { return root_; }

    // ------------------------------------------------------------- API
    void compareFiles(const QString& a, const QString& b) {
        showPage(0);
        const auto A = cmp::readText(a), B = cmp::readText(b);
        if (!A.ok || !B.ok) { status_->setText("Could not read " + (A.ok ? b : a)); return; }
        pathL_ = a; pathR_ = b; eolL_ = A.eol; eolR_ = B.eol;
        if (A.binary || B.binary) {
            const int off = cmp::firstDifferingByte(A.raw, B.raw);
            binary_ = true;
            setLines({}, {}, a, b);
            status_->setText(off < 0 ? QString("Binary files, identical (%1 bytes).").arg(A.bytes)
                                     : QString("Binary files differ; first differing byte at offset %1 (%2 vs %3 bytes).").arg(off).arg(A.bytes).arg(B.bytes));
            return;
        }
        binary_ = false;
        setLines(cmp::linesOf(A.text), cmp::linesOf(B.text), a, b);
        remember("text", a, b);
    }
    void compareTexts(const QString& aName, const QString& aText, const QString& bName, const QString& bText) {
        showPage(0);
        pathL_.clear(); pathR_.clear(); binary_ = false; eolL_ = eolR_ = "\n";
        setLines(cmp::linesOf(aText), cmp::linesOf(bText), aName, bName);
    }
    void pickAndCompareFiles() {
        const QString a = safeGetOpenFileName(this, "Left file", QString(), "Texts (*.txt *.act *.inc *.ace *.md);;All files (*)"); if (a.isEmpty()) return;
        const QString b = safeGetOpenFileName(this, "Right file", QFileInfo(a).path(), "Texts (*.txt *.act *.inc *.ace *.md);;All files (*)"); if (b.isEmpty()) return;
        compareFiles(a, b);
    }
    void pickAndCompareFolders() {
        if (!g_compareFolders) { status_->setText("Folder compare is not in this build yet (next batch)."); return; }
        const QString a = safeGetExistingDirectory(this, "Left folder", QString()); if (a.isEmpty()) return;
        const QString b = safeGetExistingDirectory(this, "Right folder", QString()); if (b.isEmpty()) return;
        g_compareFolders(a, b);
    }
    void openThreeWay() { if (openThreeWay_) openThreeWay_(); else status_->setText("Three-way merge is not in this build yet (next batch)."); }
    // Manual alignment (WinMerge sync points / Araxis links / BC alignment
    // overrides): pin the left caret's line to the right caret's line; the
    // diff then runs separately above and below every pin.
    bool addPinAtCursors() {
        if (show_ != cmp::Show::All) { status_->setText("Switch View to Show All to pin an alignment."); return false; }
        const int ra = left_->textCursor().blockNumber(), rb = right_->textCursor().blockNumber();
        if (ra < 0 || rb < 0 || ra >= (int)rows_.size() || rb >= (int)rows_.size()) return false;
        const int a = rows_[ra].a, b = rows_[rb].b;
        if (a < 0 || b < 0) { status_->setText("Put both carets on real lines (not filler) to pin them together."); return false; }
        for (const auto& p : pins_) if ((p.first <= a) != (p.second <= b)) { status_->setText("That pin would cross an existing one."); return false; }
        pins_.push_back({a, b}); std::sort(pins_.begin(), pins_.end());
        recompute(true); status_->setText(QString("Aligned left line %1 with right line %2 (%3 pin%4).").arg(a + 1).arg(b + 1).arg(pins_.size()).arg(pins_.size() == 1 ? "" : "s"));
        return true;
    }
    void clearPins() { pins_.clear(); recompute(true); status_->setText("Alignment pins cleared."); }
    int pinCount() const { return int(pins_.size()); }
    // A file with <<<<<<< / ======= / >>>>>>> markers (a merge that stopped)
    // opens as a two-sided comparison: "ours" left, "theirs" right.
    static bool splitConflictText(const QString& text, QString& ours, QString& theirs, int* conflicts = nullptr) {
        QStringList L, R; int state = 0, n = 0;
        for (const QString& line : cmp::linesOf(text)) {
            if (state == 0 && line.startsWith("<<<<<<<")) { state = 1; ++n; continue; }
            if (state == 1 && line.startsWith("=======")) { state = 2; continue; }
            if (state == 2 && line.startsWith(">>>>>>>")) { state = 0; continue; }
            if (state == 0) { L << line; R << line; } else if (state == 1) L << line; else R << line;
        }
        if (conflicts) *conflicts = n;
        if (n == 0) return false;
        ours = L.join('\n') + "\n"; theirs = R.join('\n') + "\n"; return true;
    }
    bool openConflictFile(const QString& path) {
        const auto F = cmp::readText(path);
        if (!F.ok || F.binary) { status_->setText("Could not read " + path); return false; }
        QString ours, theirs; int n = 0;
        if (!splitConflictText(F.text, ours, theirs, &n)) { status_->setText("No conflict markers (<<<<<<< ======= >>>>>>>) in " + QFileInfo(path).fileName()); return false; }
        compareTexts(QFileInfo(path).fileName() + " (ours)", ours, QFileInfo(path).fileName() + " (theirs)", theirs);
        pathL_ = path;   // Save Left writes the resolved file back over the conflict file
        updateNames(); status_->setText(QString("%1 conflict%2 opened as a two-sided comparison; resolve with Copy Left/Right, then Save Left.").arg(n).arg(n == 1 ? "" : "s"));
        return true;
    }
    void swapSides() { std::swap(aLines_, bLines_); std::swap(pathL_, pathR_); std::swap(nameLText_, nameRText_); std::swap(eolL_, eolR_); pushUndo(); recompute(true); }
    void reload() { if (!pathL_.isEmpty() && !pathR_.isEmpty()) compareFiles(pathL_, pathR_); else recompute(true); }

    const Result& result() const { return res_; }
    const QStringList& leftLines() const { return aLines_; }
    const QStringList& rightLines() const { return bLines_; }
    int rowCount() const { return int(rows_.size()); }
    int currentHunk() const { return cur_; }
    Options& options() { return opts_; }
    void setShow(cmp::Show s, int ctx = 3) { show_ = s; ctx_ = ctx; syncViewButtons(); recompute(false); }
    void setMinorAsDifference(bool on) { minorAsDiff_ = on; recompute(false); }

    // navigation over differences (minor included only when shown as differences)
    QVector<int> navigable() const {
        QVector<int> v;
        for (int i = 0; i < (int)res_.hunks.size(); ++i) { const auto& h = res_.hunks[i]; if (h.kind != Kind::Equal && (minorAsDiff_ || !h.unimportant)) v << i; }
        return v;
    }
    bool step(int dir) {
        const auto nav = navigable(); if (nav.isEmpty()) { status_->setText("No differences to step through."); return false; }
        int pos = nav.indexOf(cur_);
        if (pos < 0) { pos = 0; for (int i = 0; i < nav.size(); ++i) if (nav[i] > cur_) { pos = i; break; } if (dir < 0 && cur_ >= 0) pos = std::max(0, pos - 1); }
        else pos = std::clamp(pos + dir, 0, int(nav.size()) - 1);
        goToHunk(nav[pos]); return true;
    }
    void first() { const auto nav = navigable(); if (!nav.isEmpty()) goToHunk(nav.first()); }
    void last()  { const auto nav = navigable(); if (!nav.isEmpty()) goToHunk(nav.last()); }
    void goToHunk(int idx) {
        if (idx < 0 || idx >= (int)res_.hunks.size()) return;
        cur_ = idx;
        for (size_t r = 0; r < rows_.size(); ++r) if (rows_[r].hunk == idx) {
            rendering_ = true;
            QTextCursor c(left_->document()->findBlockByNumber((int)r)); left_->setTextCursor(c); left_->centerCursor();
            QTextCursor d(right_->document()->findBlockByNumber((int)r)); right_->setTextCursor(d); right_->centerCursor();
            rendering_ = false; break;
        }
        showDetails(); link_->update(); loc_->update();
        const auto nav = navigable();
        status_->setText(QString("Difference %1 of %2 · %3%4").arg(nav.indexOf(idx) + 1).arg(nav.size()).arg(cmp::kindName(res_.hunks[idx]),
                         res_.hunks[idx].rawNormalised ? "" : " · compared raw (a line here could not be converted)"));
    }
    void goToMovedPeer() { if (cur_ >= 0 && cur_ < (int)res_.hunks.size() && res_.hunks[cur_].movedPeer >= 0) goToHunk(res_.hunks[cur_].movedPeer); else status_->setText("The current difference is not a moved block."); }

    // merge operations (on the line vectors; the editors re-render)
    bool copyHunk(int idx, bool toRight) {
        if (idx < 0 || idx >= (int)res_.hunks.size() || res_.hunks[idx].kind == Kind::Equal) return false;
        if (show_ != cmp::Show::All) { status_->setText("Switch View to Show All to merge (the filtered view hides lines)."); return false; }
        pushUndo();
        const Hunk h = res_.hunks[idx];
        if (toRight) { QStringList src = aLines_.mid(h.aBeg, h.aEnd - h.aBeg); bLines_.erase(bLines_.begin() + h.bBeg, bLines_.begin() + h.bEnd); for (int k = 0; k < src.size(); ++k) bLines_.insert(h.bBeg + k, src[k]); dirtyR_ = true; }
        else { QStringList src = bLines_.mid(h.bBeg, h.bEnd - h.bBeg); aLines_.erase(aLines_.begin() + h.aBeg, aLines_.begin() + h.aEnd); for (int k = 0; k < src.size(); ++k) aLines_.insert(h.aBeg + k, src[k]); dirtyL_ = true; }
        const int keepA = h.aBeg;
        recompute(true);
        // land on the next difference after the merged spot
        const auto nav = navigable(); int next = -1;
        for (int i : nav) if (res_.hunks[i].aBeg >= keepA) { next = i; break; }
        if (next >= 0) goToHunk(next); else { cur_ = -1; showDetails(); status_->setText(QString::fromStdString(allcore::textdiff::summary(res_))); }
        return true;
    }
    bool copyCurrent(bool toRight, bool /*advance*/) { return copyHunk(cur_, toRight); }
    void copyAll(bool toRight) { if (show_ != cmp::Show::All) { status_->setText("Switch View to Show All to merge."); return; } pushUndo(); if (toRight) { bLines_ = aLines_; dirtyR_ = true; } else { aLines_ = bLines_; dirtyL_ = true; } recompute(true); }
    bool undo() { if (undo_.empty()) return false; redo_.push_back({aLines_, bLines_}); auto s = undo_.back(); undo_.pop_back(); aLines_ = s.first; bLines_ = s.second; dirtyL_ = dirtyR_ = true; recompute(true); return true; }
    bool redo() { if (redo_.empty()) return false; undo_.push_back({aLines_, bLines_}); auto s = redo_.back(); redo_.pop_back(); aLines_ = s.first; bLines_ = s.second; dirtyL_ = dirtyR_ = true; recompute(true); return true; }

    bool saveSide(bool leftSide, QString path = QString()) {
        if (show_ == cmp::Show::All) rescanFromEditors();
        if (path.isEmpty()) path = leftSide ? pathL_ : pathR_;
        if (path.isEmpty()) { path = safeGetSaveFileName(this, leftSide ? "Save left as" : "Save right as", QString(), "Texts (*.txt *.act *.inc *.md);;All files (*)"); if (path.isEmpty()) return false; }
        const QString eol = leftSide ? eolL_ : eolR_;
        const QByteArray bytes = (leftSide ? aLines_ : bLines_).join(eol).toUtf8() + eol.toUtf8();
        if (!saveOrWarn(this, path, bytes, leftSide ? "left text" : "right text")) return false;
        if (leftSide) { pathL_ = path; dirtyL_ = false; } else { pathR_ = path; dirtyR_ = false; }
        updateNames(); status_->setText("Saved " + path); return true;
    }

    // reports
    QString report(const QString& kind) const {
        using namespace allcore::textdiff;
        const auto a = cmp::toStd(aLines_), b = cmp::toStd(bLines_);
        const std::string an = nameLText_.toStdString(), bn = nameRText_.toStdString();
        if (kind == "html") return QString::fromStdString(sideBySideHtml(an, bn, a, b, res_, show_ != cmp::Show::All, ctx_));
        if (kind == "patch") return QString::fromStdString(unifiedDiff(an, bn, a, b, res_, 3));
        if (kind == "md") return QString::fromStdString(apparatusMarkdown(an, bn, apparatus(a, b, res_)));
        if (kind == "csv") return QString::fromStdString(apparatusCsv(apparatus(a, b, res_)));
        return QString::fromStdString(summary(res_));
    }
    bool saveReport() {
        const QString fn = safeGetSaveFileName(this, "Save comparison report", QString(),
            "HTML side-by-side (*.html);;Unified patch (*.patch);;Apparatus criticus, Markdown (*.md);;Apparatus criticus, CSV (*.csv);;Summary (*.txt)");
        if (fn.isEmpty()) return false;
        const QString sfx = QFileInfo(fn).suffix().toLower();
        const QString kind = sfx == "html" ? "html" : sfx == "patch" || sfx == "diff" ? "patch" : sfx == "md" ? "md" : sfx == "csv" ? "csv" : "txt";
        if (!saveOrWarn(this, fn, report(kind).toUtf8(), "report")) return false;
        status_->setText("Report saved: " + fn); return true;
    }

    // sessions
    QString sessionsDir() const { return root_ + "/library/compare_sessions"; }
    QJsonObject sessionJson() const { QJsonObject j; j["mode"] = "text"; j["left"] = pathL_; j["right"] = pathR_; j["options"] = cmp::optionsJson(opts_); j["show"] = int(show_); j["context"] = ctx_; j["minorAsDiff"] = minorAsDiff_; return j; }
    bool saveSessionAs(const QString& name) {
        if (pathL_.isEmpty() || pathR_.isEmpty()) { status_->setText("Sessions hold file paths; this comparison has none (clipboard or unsaved text)."); return false; }
        QDir().mkpath(sessionsDir());
        QString safe = name.trimmed(); safe.replace(QRegularExpression("[/\\\\:]"), "-"); if (safe.isEmpty()) return false;
        return saveOrWarn(this, sessionsDir() + "/" + safe + ".json", QJsonDocument(sessionJson()).toJson(), "session");
    }
    bool openSessionJson(const QJsonObject& j) {
        opts_ = cmp::optionsFrom(j["options"].toObject()); syncRuleButtons();
        show_ = cmp::Show(j["show"].toInt()); ctx_ = j.contains("context") ? j["context"].toInt() : 3; minorAsDiff_ = j["minorAsDiff"].toBool(); syncViewButtons();
        if (j["mode"].toString() == "folders" && g_compareFolders) { g_compareFolders(j["left"].toString(), j["right"].toString()); return true; }
        compareFiles(j["left"].toString(), j["right"].toString()); return !aLines_.isEmpty() || !bLines_.isEmpty();
    }
    bool openSessionFile(const QString& path) { QFile f(path); if (!f.open(QIODevice::ReadOnly)) return false; return openSessionJson(QJsonDocument::fromJson(f.readAll()).object()); }
    QStringList sessionNames() const { QStringList v; for (const auto& fi : QDir(sessionsDir()).entryInfoList({"*.json"}, QDir::Files, QDir::Name)) v << fi.completeBaseName(); return v; }
    static QStringList recent() { return QSettings("ALL", "TranslationTool").value("compare/recent").toStringList(); }
    void remember(const QString& mode, const QString& a, const QString& b) {
        QSettings st("ALL", "TranslationTool"); QStringList r = st.value("compare/recent").toStringList();
        QJsonObject j; j["mode"] = mode; j["left"] = a; j["right"] = b; const QString s = QString::fromUtf8(QJsonDocument(j).toJson(QJsonDocument::Compact));
        r.removeAll(s); r.prepend(s); while (r.size() > 10) r.removeLast(); st.setValue("compare/recent", r);
    }
    void applyRules() { syncRuleButtons(); recompute(true); }

    // ------------------------------------------------------- selftest
    int selfTest(QStringList& log) {
        int fails = 0;
        auto check = [&](bool ok, const char* what) { log << QString("  [%1] Compare: %2").arg(ok ? "PASS" : "FAIL").arg(what); if (!ok) ++fails; };
        opts_ = Options(); minorAsDiff_ = false; show_ = cmp::Show::All; syncRuleButtons(); syncViewButtons();
        compareTexts("A", "@001A\nBLA MA LA\nPHYAG 'TSHAL LO\nSEMS CAN THAMS CAD\nBDE BA\n", "B", "@001A\nBLA MA LA\nPHYAG 'TSHAL LO\nSEMS CAN KUN\nBDE BA\n");
        check(res_.differences() == 1 && res_.changes == 1, "two ACIP texts differing in one line → one changed difference");
        check(rowCount() == 5 && left_->document()->blockCount() == 5 && right_->document()->blockCount() == 5, "Show All renders one aligned row per line");
        check(step(+1) && cur_ >= 0 && res_.hunks[cur_].kind == Kind::Change, "Next Difference lands on the change");
        check(detL_->toPlainText() == "SEMS CAN THAMS CAD" && detR_->toPlainText() == "SEMS CAN KUN", "Line Details shows the two readings");
        setShow(cmp::Show::DiffsOnly);
        check(rowCount() == 3 && rows_[0].gap == 3 && rows_[2].gap == 1, "Differences Only collapses identical runs into gap rows");
        check(!copyHunk(cur_, true), "merging is refused while the view is filtered");
        setShow(cmp::Show::Context, 1);
        check(rowCount() == 4 && rows_[0].gap == 2, "Context 1 keeps one identical line each side of the change");
        setShow(cmp::Show::All);
        check(copyHunk(navigable().value(0), true) && res_.differences() == 0 && bLines_[3] == "SEMS CAN THAMS CAD", "Copy to Right takes the left reading; texts now agree");
        check(undo() && res_.differences() == 1 && bLines_[3] == "SEMS CAN KUN", "Undo restores the right text");
        check(redo() && res_.differences() == 0, "Redo re-applies the merge");
        undo();
        copyAll(false);
        check(res_.differences() == 0 && aLines_[3] == "SEMS CAN KUN", "All to Left makes the left match the right");
        undo();
        // minor differences under a rule
        compareTexts("A", "SEMS  CAN\nTHAMS CAD\n", "B", "SEMS CAN\nTHAMS CAD\n");
        check(res_.differences() == 1, "spacing difference is important with no rules on");
        opts_.ignoreWhitespaceChange = true; applyRules();
        check(res_.differences() == 0 && res_.unimportant == 1 && navigable().isEmpty(), "…and minor under Ignore Whitespace Change, skipped by navigation");
        setMinorAsDifference(true);
        check(navigable().size() == 1, "Show Minor as Differences brings it back into navigation");
        setMinorAsDifference(false); opts_ = Options(); syncRuleButtons();
        // reports
        compareTexts("A.act", "@001A\nSEMS CAN THAMS CAD\n", "B.act", "@001A\nSEMS CAN KUN\n");
        check(report("patch").startsWith("--- A.act\n+++ B.act\n") && report("html").contains("<mark>KUN</mark>") && report("md").contains("**@001A.2** A: THAMS CAD ] B: KUN") && report("csv").startsWith("cite,kind"), "patch, HTML, apparatus Markdown and CSV reports render");
        // editing in place → rescan
        {
            rendering_ = true; QTextCursor c(right_->document()->findBlockByNumber(1)); c.select(QTextCursor::LineUnderCursor); c.insertText("SEMS CAN THAMS CAD"); rendering_ = false;
            dirtyEditors_ = true; rescanFromEditors();
            check(res_.differences() == 0 && bLines_[1] == "SEMS CAN THAMS CAD", "typing in the right pane and rescanning re-diffs from the edited text");
        }
        // sessions round-trip through a temp root
        {
            const QString tmp = QDir::temp().filePath("all_cmp_" + QString::number(QCoreApplication::applicationPid()));
            QDir().mkpath(tmp);
            const QString fa = tmp + "/a.act", fb = tmp + "/b.act";
            QFile f(fa); f.open(QIODevice::WriteOnly); f.write("@001A\nSEMS CAN\n"); f.close();
            QFile g(fb); g.open(QIODevice::WriteOnly); g.write("@001A\r\nSEMS CAN KUN\r\n"); g.close();
            compareFiles(fa, fb);
            check(res_.differences() == 1 && eolR_ == "\r\n" && eolL_ == "\n", "Compare Files reads both, detecting each side's line ending");
            check(recent().size() >= 1 && recent().first().contains("a.act"), "the comparison is remembered as recent");
            const QString keepRoot = root_; root_ = tmp;
            check(saveSessionAs("probe/session") && sessionNames() == QStringList{"probe-session"}, "Save Session writes library/compare_sessions/<safe>.json");
            compareTexts("x", "", "y", "");
            check(openSessionFile(sessionsDir() + "/probe-session.json") && pathL_ == fa && res_.differences() == 1, "Open Session restores the comparison");
            root_ = keepRoot;
            check(copyHunk(navigable().value(0), false) && saveSide(true) && QFile(fa).exists() && cmp::readText(fa).text == "@001A\nSEMS CAN KUN\n", "Save Left writes the merged left text with its own line ending");
            QDir(tmp).removeRecursively();
        }
        // binary
        {
            const QString tmp = QDir::temp().filePath("all_cmpb_" + QString::number(QCoreApplication::applicationPid()));
            QDir().mkpath(tmp);
            QFile f(tmp + "/x.bin"); f.open(QIODevice::WriteOnly); f.write(QByteArray("ab\0cd", 5)); f.close();
            QFile g(tmp + "/y.bin"); g.open(QIODevice::WriteOnly); g.write(QByteArray("ab\0ce", 5)); g.close();
            compareFiles(tmp + "/x.bin", tmp + "/y.bin");
            check(binary_ && status_->text().contains("offset 4"), "binary files report the first differing byte, never a text diff");
            QDir(tmp).removeRecursively();
        }
        // alignment pins: force left line 2 (y) to align with right line 2 (q)
        compareTexts("A", "x\ny\nSEMS CAN\nz\n", "B", "SEMS CAN\nq\nr\ns\n");
        check(res_.hunks.size() == 3 && res_.hunks[1].kind == Kind::Equal, "without pins the diff aligns SEMS CAN with SEMS CAN");
        { rendering_ = true; QTextCursor ca(left_->document()->findBlockByNumber(1)); left_->setTextCursor(ca); QTextCursor cb(right_->document()->findBlockByNumber(3)); right_->setTextCursor(cb); rendering_ = false; }
        check(rows_[1].a == 1 && rows_[3].b == 1, "the carets sit on left line 2 and right line 2 in the aligned view");
        check(addPinAtCursors() && pinCount() == 1, "Align Caret Lines pins them together");
        check(res_.hunks.size() == 2 && res_.hunks[0].kind == Kind::Change && res_.hunks[1].aBeg == 1 && res_.hunks[1].bBeg == 1, "the diff now runs above and below the pin: x↔SEMS CAN, then y/SEMS CAN/z ↔ q/r/s");
        clearPins(); check(pinCount() == 0 && res_.hunks.size() == 3, "Clear Alignment Pins restores the free alignment");
        // conflict files
        {
            QString ours, theirs; int n = 0;
            check(splitConflictText("a\n<<<<<<< MINE\nX\n=======\nY\n>>>>>>> THEIRS\nb\n", ours, theirs, &n) && n == 1 && ours == "a\nX\nb\n" && theirs == "a\nY\nb\n", "a conflict file splits into ours / theirs");
            check(!splitConflictText("plain\n", ours, theirs, &n), "a file without markers is refused as a conflict file");
        }
        compareTexts("", "", "", "");
        return fails;
    }

private:
    // ------------------------------------------------------- internals
    void setLines(QStringList a, QStringList b, const QString& nameA, const QString& nameB) {
        aLines_ = std::move(a); bLines_ = std::move(b); nameLText_ = nameA; nameRText_ = nameB;
        undo_.clear(); redo_.clear(); dirtyL_ = dirtyR_ = false; cur_ = -1;
        recompute(true);
    }
    void pushUndo() { undo_.push_back({aLines_, bLines_}); redo_.clear(); if (undo_.size() > 200) undo_.erase(undo_.begin()); }
    void recompute(bool rediff) {
        if (rediff) res_ = diffWithPins();
        rows_ = cmp::buildRows(res_, show_, ctx_, minorAsDiff_);
        render();
        if (cur_ >= (int)res_.hunks.size()) cur_ = -1;
        updateNames(); showDetails();
        if (!binary_) status_->setText(QString::fromStdString(allcore::textdiff::summary(res_)));
        link_->update(); loc_->update();
    }
    Result diffWithPins() {
        using namespace allcore::textdiff;
        const auto A = cmp::toStd(aLines_), B = cmp::toStd(bLines_);
        // drop pins that no longer fit the texts
        std::vector<std::pair<int, int>> pins;
        for (const auto& p : pins_) if (p.first <= (int)A.size() && p.second <= (int)B.size()) pins.push_back(p);
        pins_ = pins;
        if (pins.empty()) return diffLines(A, B, opts_);
        Result R; R.aLines = int(A.size()); R.bLines = int(B.size());
        int a0 = 0, b0 = 0;
        auto segment = [&](int a1, int b1) {
            std::vector<std::string> sa(A.begin() + a0, A.begin() + a1), sb(B.begin() + b0, B.begin() + b1);
            Result r = diffLines(sa, sb, opts_);
            const int base = int(R.hunks.size());
            for (auto h : r.hunks) { h.aBeg += a0; h.aEnd += a0; h.bBeg += b0; h.bEnd += b0; if (h.movedPeer >= 0) h.movedPeer += base; R.hunks.push_back(h); }
            R.changes += r.changes; R.inserts += r.inserts; R.deletes += r.deletes; R.unimportant += r.unimportant; R.moved += r.moved; R.unnormalised += r.unnormalised;
            a0 = a1; b0 = b1;
        };
        for (const auto& p : pins) segment(p.first, p.second);
        segment(int(A.size()), int(B.size()));
        return R;
    }
    void render() {
        rendering_ = true;
        const int scroll = left_->verticalScrollBar()->value();
        QString ta, tb; ta.reserve(4096); tb.reserve(4096);
        for (size_t r = 0; r < rows_.size(); ++r) {
            const auto& row = rows_[r];
            if (row.gap) { const QString g = QString("⋯ %1 identical line%2 ⋯").arg(row.gap).arg(row.gap == 1 ? "" : "s"); ta += g; tb += g; }
            else { if (row.a >= 0) ta += aLines_[row.a]; if (row.b >= 0) tb += bLines_[row.b]; }
            if (r + 1 < rows_.size()) { ta += '\n'; tb += '\n'; }
        }
        left_->setPlainText(ta); right_->setPlainText(tb);
        const bool editable = show_ == cmp::Show::All && !binary_;
        left_->setReadOnly(!editable); right_->setReadOnly(!editable);
        QList<QTextEdit::ExtraSelection> selA, selB; QVector<QColor> barA(int(rows_.size()), Qt::transparent), barB(int(rows_.size()), Qt::transparent);
        for (size_t r = 0; r < rows_.size(); ++r) {
            const auto& row = rows_[r];
            QTextBlock ba = left_->document()->findBlockByNumber((int)r), bb = right_->document()->findBlockByNumber((int)r);
            ba.setUserState(row.gap ? -3 : row.a < 0 ? -2 : row.a); bb.setUserState(row.gap ? -3 : row.b < 0 ? -2 : row.b);
            if (row.gap) { addBg(selA, ba, QColor(0xFA, 0xF8, 0xF3)); addBg(selB, bb, QColor(0xFA, 0xF8, 0xF3)); continue; }
            const Hunk& h = res_.hunks[row.hunk];
            const QColor c = cmp::colFor(h);
            if (c.alpha() == 0) continue;
            barA[(int)r] = c; barB[(int)r] = c;
            if (row.a < 0) addBg(selA, ba, cmp::colFiller()); else addBg(selA, ba, c);
            if (row.b < 0) addBg(selB, bb, cmp::colFiller()); else addBg(selB, bb, c);
            if (h.kind == Kind::Change && !h.unimportant && row.a >= 0 && row.b >= 0) {
                const std::string sa = aLines_[row.a].toStdString(), sb = bLines_[row.b].toStdString();
                for (const auto& sp : allcore::textdiff::diffInline(sa, sb)) {
                    if (sp.equal) continue;
                    addMark(selA, ba, cmp::u16(sa, sp.aBeg), cmp::u16(sa, sp.aEnd));
                    addMark(selB, bb, cmp::u16(sb, sp.bBeg), cmp::u16(sb, sp.bEnd));
                }
            }
        }
        left_->setExtraSelections(selA); right_->setExtraSelections(selB);
        left_->setRowColors(barA); right_->setRowColors(barB);
        left_->verticalScrollBar()->setValue(scroll); right_->verticalScrollBar()->setValue(scroll);
        rendering_ = false; dirtyEditors_ = false;
    }
    static void addBg(QList<QTextEdit::ExtraSelection>& out, const QTextBlock& b, const QColor& c) {
        QTextEdit::ExtraSelection s; s.cursor = QTextCursor(b); s.format.setBackground(c); s.format.setProperty(QTextFormat::FullWidthSelection, true); out << s;
    }
    static void addMark(QList<QTextEdit::ExtraSelection>& out, const QTextBlock& b, int from, int to) {
        if (to <= from) return;
        QTextEdit::ExtraSelection s; s.cursor = QTextCursor(b); s.cursor.setPosition(b.position() + from); s.cursor.setPosition(b.position() + std::min(to, b.length() - 1), QTextCursor::KeepAnchor);
        s.format.setBackground(cmp::colMark()); out << s;
    }
    void rescanFromEditors() {
        if (show_ != cmp::Show::All || binary_ || !dirtyEditors_) return;
        auto collect = [](QPlainTextEdit* e) { QStringList v; for (QTextBlock b = e->document()->begin(); b.isValid(); b = b.next()) { if (b.userState() == -2 && b.text().isEmpty()) continue; if (b.userState() == -3) continue; v << b.text(); } return v; };
        const QStringList na = collect(left_), nb = collect(right_);
        if (na == aLines_ && nb == bLines_) { dirtyEditors_ = false; return; }
        pushUndo();
        if (na != aLines_) dirtyL_ = true; if (nb != bLines_) dirtyR_ = true;
        aLines_ = na; bLines_ = nb;
        recompute(true);
    }
    void selectHunkAtCursor(DiffEdit* e) {
        const int r = e->textCursor().blockNumber();
        if (r < 0 || r >= (int)rows_.size()) return;
        const int h = rows_[r].hunk;
        if (h != cur_ && h >= 0 && res_.hunks[h].kind != Kind::Equal) { cur_ = h; showDetails(); link_->update(); loc_->update(); }
    }
    void showDetails() {
        if (cur_ < 0 || cur_ >= (int)res_.hunks.size()) { detailLbl_->setText("No difference selected."); detL_->clear(); detR_->clear(); return; }
        const Hunk& h = res_.hunks[cur_];
        detailLbl_->setText(QString("%1 · left lines %2–%3 · right lines %4–%5%6").arg(cmp::kindName(h)).arg(h.aBeg + 1).arg(std::max(h.aBeg + 1, h.aEnd)).arg(h.bBeg + 1).arg(std::max(h.bBeg + 1, h.bEnd)).arg(h.movedPeer >= 0 ? "  (Goto Moved Peer to see the other copy)" : ""));
        const QString la = aLines_.mid(h.aBeg, h.aEnd - h.aBeg).join('\n'), lb = bLines_.mid(h.bBeg, h.bEnd - h.bBeg).join('\n');
        detL_->setPlainText(la); detR_->setPlainText(lb);
        QList<QTextEdit::ExtraSelection> sa, sb;
        const std::string ssa = la.toStdString(), ssb = lb.toStdString();
        for (const auto& sp : allcore::textdiff::diffInline(ssa, ssb)) {
            if (sp.equal) continue;
            QTextEdit::ExtraSelection x; x.cursor = QTextCursor(detL_->document()); x.cursor.setPosition(cmp::u16(ssa, sp.aBeg)); x.cursor.setPosition(cmp::u16(ssa, sp.aEnd), QTextCursor::KeepAnchor); x.format.setBackground(cmp::colMark()); sa << x;
            QTextEdit::ExtraSelection y; y.cursor = QTextCursor(detR_->document()); y.cursor.setPosition(cmp::u16(ssb, sp.bBeg)); y.cursor.setPosition(cmp::u16(ssb, sp.bEnd), QTextCursor::KeepAnchor); y.format.setBackground(cmp::colMark()); sb << y;
        }
        detL_->setExtraSelections(sa); detR_->setExtraSelections(sb);
    }
    void updateNames() {
        auto nm = [](const QString& path, const QString& fallback, bool dirty, int lines, const QString& eol) {
            const QString base = path.isEmpty() ? fallback : QFileInfo(path).fileName();
            return QString("%1%2  <span style='color:#6B5E4E;font-weight:400'>%3 lines · %4</span>").arg(base.toHtmlEscaped(), dirty ? " •" : "").arg(lines).arg(eol == "\r\n" ? "CRLF" : eol == "\r" ? "CR" : "LF");
        };
        nameL_->setText(nm(pathL_, nameLText_.isEmpty() ? "(left)" : nameLText_, dirtyL_, aLines_.size(), eolL_));
        nameR_->setText(nm(pathR_, nameRText_.isEmpty() ? "(right)" : nameRText_, dirtyR_, bLines_.size(), eolR_));
        nameL_->setToolTip(pathL_); nameR_->setToolTip(pathR_);
    }

    // ---- ribbon (compact: small buttons in columns; the fit ratchet
    // caps every pane's ribbon width)
    void buildRibbon() {
        auto* ribbon = new RibbonBar;
        auto* gSess = ribbon->group("SESSION");
        auto* gView = ribbon->group("VIEW");
        auto* gNav = ribbon->group("NAVIGATE");
        auto* gMerge = ribbon->group("MERGE");
        auto* gRules = ribbon->group("RULES");
        auto* gFile = ribbon->group("FILE");
        auto* gRep = ribbon->group("REPORT");
        auto btn = [&](const QString& label, const QString& tip, std::function<void()> fn) {
            auto* b = new QPushButton(label); b->setToolTip(tip); b->setStyleSheet("padding:1px 6px;"); connect(b, &QPushButton::clicked, this, std::move(fn)); return b;
        };
        auto big = [&](RibbonGroup* g, const QString& label, const QString& icon, const QString& tip, std::function<void()> fn) {
            auto* b = new QPushButton(label); b->setToolTip(tip); connect(b, &QPushButton::clicked, this, std::move(fn)); g->addBig(b, icon); return b;
        };
        big(gSess, "Compare Files…", "diff", "Pick two text files (ACIP, Wylie, Unicode or English) and compare them line by line.", [this] { pickAndCompareFiles(); });
        big(gSess, "Folders…", "folder", "Compare two folders: identical, different, only-left, only-right files; sync with confirmation.", [this] { pickAndCompareFolders(); });
        big(gSess, "Three-Way…", "diff", "Base text plus two edited copies → one merged text, conflicts shown.", [this] { openThreeWay(); });
        auto* sess = new QPushButton("Sessions ▾"); sess->setToolTip("Save or reopen a comparison (both paths + rules)."); sess->setStyleSheet("padding:1px 6px;");
        {
            auto* menu = new QMenu(sess); sess->setMenu(menu);
            connect(menu, &QMenu::aboutToShow, this, [this, menu] {
                menu->clear();
                connect(menu->addAction("Open Conflict File…"), &QAction::triggered, this, [this] {
                    const QString f = safeGetOpenFileName(this, "Open a file with <<<<<<< conflict markers", QString(), "Texts (*.txt *.act *.inc *.md);;All files (*)");
                    if (!f.isEmpty()) openConflictFile(f);
                });
                connect(menu->addAction("Align Caret Lines (pin)"), &QAction::triggered, this, [this] { addPinAtCursors(); });
                connect(menu->addAction("Clear Alignment Pins"), &QAction::triggered, this, [this] { clearPins(); });
                menu->addSeparator();
                connect(menu->addAction("Save Session…"), &QAction::triggered, this, [this] {
                    const QString n = docprops::askName(this, "Save Session", "Session name:", QFileInfo(pathL_).completeBaseName() + " vs " + QFileInfo(pathR_).completeBaseName());
                    if (!n.isEmpty() && saveSessionAs(n)) status_->setText("Session saved: " + n);
                });
                QMenu* open = menu->addMenu("Open Session");
                for (const QString& n : sessionNames()) connect(open->addAction(n), &QAction::triggered, this, [this, n] { if (!openSessionFile(sessionsDir() + "/" + n + ".json")) status_->setText("Could not open session " + n); });
                if (sessionNames().isEmpty()) open->addAction("(none saved)")->setEnabled(false);
                QMenu* rec = menu->addMenu("Recent Comparisons");
                for (const QString& s : recent()) {
                    const QJsonObject j = QJsonDocument::fromJson(s.toUtf8()).object();
                    const QString label = QFileInfo(j["left"].toString()).fileName() + "  ↔  " + QFileInfo(j["right"].toString()).fileName() + (j["mode"].toString() == "folders" ? "  (folders)" : "");
                    connect(rec->addAction(label), &QAction::triggered, this, [this, j] { if (j["mode"].toString() == "folders") { if (g_compareFolders) g_compareFolders(j["left"].toString(), j["right"].toString()); } else compareFiles(j["left"].toString(), j["right"].toString()); });
                }
                if (recent().isEmpty()) rec->addAction("(none yet)")->setEnabled(false);
            });
        }
        gSess->addColumn({btn("Swap Sides", "Exchange left and right.", [this] { swapSides(); }),
                          btn("Reload", "Re-read both files from disk (F5 rescans edited text).", [this] { reload(); }),
                          sess});
        // VIEW
        showAll_ = new QRadioButton("Show All"); showDiff_ = new QRadioButton("Differences"); showCtx_ = new QRadioButton("+ Context");
        ctxSpin_ = new QSpinBox; ctxSpin_->setRange(0, 20); ctxSpin_->setValue(3); ctxSpin_->setToolTip("Identical lines kept around each difference"); ctxSpin_->setMaximumWidth(48);
        showAll_->setChecked(true);
        connect(showAll_, &QRadioButton::toggled, this, [this](bool on) { if (on && !syncingView_) setShow(cmp::Show::All, ctxSpin_->value()); });
        connect(showDiff_, &QRadioButton::toggled, this, [this](bool on) { if (on && !syncingView_) setShow(cmp::Show::DiffsOnly, ctxSpin_->value()); });
        connect(showCtx_, &QRadioButton::toggled, this, [this](bool on) { if (on && !syncingView_) setShow(cmp::Show::Context, ctxSpin_->value()); });
        connect(ctxSpin_, qOverload<int>(&QSpinBox::valueChanged), this, [this](int v) { if (show_ == cmp::Show::Context && !syncingView_) setShow(cmp::Show::Context, v); });
        auto* ctxRow = new QWidget; { auto* h = new QHBoxLayout(ctxRow); h->setContentsMargins(0, 0, 0, 0); h->setSpacing(2); h->addWidget(showCtx_); h->addWidget(ctxSpin_); }
        gView->addColumn({showAll_, showDiff_, ctxRow});
        minorT_ = new QCheckBox("Minor = differences"); minorT_->setToolTip("Count differences the rules call minor (whitespace, punctuation…) as differences to step through and merge.");
        connect(minorT_, &QCheckBox::toggled, this, [this](bool on) { if (!syncingView_) setMinorAsDifference(on); });
        auto* detT = new QCheckBox("Line Details"); detT->setChecked(true); connect(detT, &QCheckBox::toggled, this, [this](bool on) { details_->setVisible(on); });
        auto* wrapT = new QCheckBox("Wrap"); connect(wrapT, &QCheckBox::toggled, this, [this](bool on) { for (auto* e : {left_, right_}) e->setLineWrapMode(on ? QPlainTextEdit::WidgetWidth : QPlainTextEdit::NoWrap); });
        gView->addColumn({minorT_, detT, wrapT});
        // NAVIGATE
        gNav->addColumn({btn("First", "First difference", [this] { first(); }),
                         btn("Previous (F7)", "Previous difference", [this] { step(-1); }),
                         btn("Next (F8)", "Next difference", [this] { step(+1); })});
        gNav->addColumn({btn("Last", "Last difference", [this] { last(); }),
                         btn("Moved Peer", "Jump to the other copy of a moved block", [this] { goToMovedPeer(); })});
        // MERGE
        gMerge->addColumn({btn("◀ Copy Left", "Replace the left side of the current difference with the right (⌥←)", [this] { copyCurrent(false, false); }),
                           btn("Copy Right ▶", "Replace the right side of the current difference with the left (⌥→)", [this] { copyCurrent(true, false); }),
                           btn("Undo Merge", "Undo the last merge, rescan or swap (unlimited)", [this] { if (!undo()) status_->setText("Nothing to undo."); })});
        gMerge->addColumn({btn("◀ All Left", "Make the left text identical to the right", [this] { copyAll(false); }),
                           btn("All Right ▶", "Make the right text identical to the left", [this] { copyAll(true); }),
                           btn("Redo", "Redo", [this] { if (!redo()) status_->setText("Nothing to redo."); })});
        // RULES
        auto rule = [&](const QString& label, const QString& tip, bool Options::*field) {
            auto* c = new QCheckBox(label); c->setToolTip(tip);
            connect(c, &QCheckBox::toggled, this, [this, field](bool on) { if (syncingRules_) return; opts_.*field = on; recompute(true); });
            rules_.push_back({c, field}); return c;
        };
        gRules->addColumn({rule("Ignore case", "ACIP is upper-case, Wylie lower-case; English capitals", &Options::ignoreCase),
                           rule("Ignore spacing", "Runs of spaces compare equal", &Options::ignoreWhitespaceChange),
                           rule("Ignore blank lines", "Blank-only differences are minor", &Options::ignoreBlankLines)});
        gRules->addColumn({rule("Ignore shad / tsheg marks", "shad / nyis shad / gter tsheg variants, ACIP , ,, * # %  and Wylie / | !", &Options::ignoreTibetanPunct),
                           rule("Ignore folio markers", "@012A-style markers", &Options::ignoreFolioMarkers),
                           rule("Ignore [ ] { } apparatus", "[...] and {...} editorial spans", &Options::ignoreApparatus)});
        gRules->addColumn({rule("Script-agnostic", "ACIP = Wylie = Tibetan script: every line is converted to Wylie by the canonical converters before comparing. A line that will not convert cleanly is compared raw and reported.", &Options::scriptAgnostic),
                           rule("Detect moves", "A block deleted here and inserted there is one move, not two differences", &Options::detectMoves)});
        gRules->addColumn({btn("Line Filters…", "Regular expressions; a difference whose lines all match is minor (WinMerge's line filters)", [this] { editLineFilters(); }),
                           btn("Substitutions…", "regex => replacement, applied to both sides before comparing (WinMerge's substitution filters)", [this] { editSubstitutions(); })});
        // FILE
        gFile->addColumn({btn("Save Left", "Write the left text back to its file", [this] { saveSide(true); }),
                          btn("Save Right", "Write the right text back to its file", [this] { saveSide(false); })});
        gFile->addColumn({btn("Save Left As…", "", [this] { saveSide(true, safeGetSaveFileName(this, "Save left as", pathL_, "Texts (*.txt *.act *.inc *.md);;All files (*)")); }),
                          btn("Save Right As…", "", [this] { saveSide(false, safeGetSaveFileName(this, "Save right as", pathR_, "Texts (*.txt *.act *.inc *.md);;All files (*)")); })});
        gFile->addColumn({btn("Left → Overlay", "Read the left text in the Overlay", [this] { if (openInOverlay_ && !pathL_.isEmpty()) openInOverlay_(pathL_); else status_->setText("The left side is not a file on disk."); }),
                          btn("Right → Overlay", "Read the right text in the Overlay", [this] { if (openInOverlay_ && !pathR_.isEmpty()) openInOverlay_(pathR_); else status_->setText("The right side is not a file on disk."); })});
        // REPORT
        gRep->addColumn({btn("Save Report…", "HTML side-by-side · unified patch · apparatus criticus (Markdown / CSV) · summary", [this] { saveReport(); }),
                         btn("Copy Summary", "Copy the one-line summary to the clipboard", [this] { QApplication::clipboard()->setText(report("txt")); status_->setText("Summary copied."); }),
                         btn("Copy Apparatus", "Copy the apparatus criticus (Markdown) to the clipboard", [this] { QApplication::clipboard()->setText(report("md")); status_->setText("Apparatus copied."); })});
        ribbon->finish();
        ribbon->attachTo(this);   // the window's ribbon band hosts it (like every pane)
    }
    void editLineFilters() {
        if (g_harnessRun) return;
        QDialog d(this); d.setWindowTitle("Line Filters"); auto* v = new QVBoxLayout(&d);
        v->addWidget(new QLabel("One regular expression per line. Differences made only of matching lines count as minor.\nExample: ^@[0-9]+[AB]   (folio marker lines)"));
        auto* ed = new QPlainTextEdit; QStringList cur; for (const auto& r : opts_.ignoreRegex) cur << QString::fromStdString(r); ed->setPlainText(cur.join('\n')); v->addWidget(ed, 1);
        auto* bb = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel); v->addWidget(bb);
        connect(bb, &QDialogButtonBox::accepted, &d, &QDialog::accept); connect(bb, &QDialogButtonBox::rejected, &d, &QDialog::reject);
        if (d.exec() != QDialog::Accepted) return;
        opts_.ignoreRegex.clear(); for (const QString& l : ed->toPlainText().split('\n')) if (!l.trimmed().isEmpty()) opts_.ignoreRegex.push_back(l.trimmed().toStdString());
        recompute(true);
    }
    void editSubstitutions() {
        if (g_harnessRun) return;
        QDialog d(this); d.setWindowTitle("Substitutions"); auto* v = new QVBoxLayout(&d);
        v->addWidget(new QLabel("One per line:  <regex> => <replacement>\nExample:  NGO BO => NGO-BO"));
        auto* ed = new QPlainTextEdit; QStringList cur; for (const auto& s : opts_.substitutions) cur << QString::fromStdString(s.first) + " => " + QString::fromStdString(s.second); ed->setPlainText(cur.join('\n')); v->addWidget(ed, 1);
        auto* bb = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel); v->addWidget(bb);
        connect(bb, &QDialogButtonBox::accepted, &d, &QDialog::accept); connect(bb, &QDialogButtonBox::rejected, &d, &QDialog::reject);
        if (d.exec() != QDialog::Accepted) return;
        opts_.substitutions.clear();
        for (const QString& l : ed->toPlainText().split('\n')) { const int k = l.indexOf("=>"); if (k > 0) opts_.substitutions.push_back({l.left(k).trimmed().toStdString(), l.mid(k + 2).trimmed().toStdString()}); }
        recompute(true);
    }
    void syncRuleButtons() { syncingRules_ = true; for (auto& r : rules_) r.first->setChecked(opts_.*(r.second)); syncingRules_ = false; }
    void syncViewButtons() { syncingView_ = true; showAll_->setChecked(show_ == cmp::Show::All); showDiff_->setChecked(show_ == cmp::Show::DiffsOnly); showCtx_->setChecked(show_ == cmp::Show::Context); ctxSpin_->setValue(ctx_); minorT_->setChecked(minorAsDiff_); syncingView_ = false; }

public:
    std::function<void(const QString&)> openInOverlay_;
    std::function<void()> openThreeWay_;

private:
    // ---- link strip: bands between the panes with per-hunk copy buttons
    class LinkStrip : public QWidget {
    public:
        explicit LinkStrip(ComparePane* p) : QWidget(p), p_(p) { setFixedWidth(48); setMouseTracking(true); }
    protected:
        void paintEvent(QPaintEvent*) override {
            QPainter g(this); g.fillRect(rect(), QColor(0xF7, 0xF4, 0xEC));
            hits_.clear();
            if (p_->rows_.empty()) return;
            const QTextBlock first = p_->left_->firstVisible();
            int lastHunk = -1; double top = 0, bottom = 0;
            auto flush = [&](int hunk) {
                if (hunk < 0) return;
                const Hunk& h = p_->res_.hunks[hunk];
                if (h.kind == Kind::Equal) return;
                const QColor c = cmp::colFor(h);
                if (c.alpha() == 0) return;
                QRectF band(0, top, width(), std::max(4.0, bottom - top));
                g.fillRect(band, c);
                if (hunk == p_->cur_) { g.setPen(QPen(QColor(0x9A, 0x7A, 0x33), 2)); g.drawRect(band.adjusted(1, 1, -1, -1)); }
                if (p_->show_ == cmp::Show::All && band.height() >= 10) {
                    const QRectF bl(3, band.center().y() - 6, 18, 12), br(width() - 21, band.center().y() - 6, 18, 12);
                    g.setPen(QColor(0x4A, 0x3F, 0x33)); g.setBrush(Qt::white); g.drawRoundedRect(bl, 3, 3); g.drawRoundedRect(br, 3, 3);
                    g.drawText(bl, Qt::AlignCenter, "◀"); g.drawText(br, Qt::AlignCenter, "▶");
                    hits_.push_back({bl, hunk, false}); hits_.push_back({br, hunk, true});
                }
            };
            for (QTextBlock b = first; b.isValid(); b = b.next()) {
                const QRectF r = p_->left_->blockRect(b);
                if (r.top() > height()) break;
                const int row = b.blockNumber(); if (row >= (int)p_->rows_.size()) break;
                const int hunk = p_->rows_[row].gap ? -1 : p_->rows_[row].hunk;
                if (hunk != lastHunk) { flush(lastHunk); lastHunk = hunk; top = r.top(); }
                bottom = r.bottom();
            }
            flush(lastHunk);
        }
        void mousePressEvent(QMouseEvent* e) override {
            for (const auto& h : hits_) if (h.rect.contains(e->position())) { p_->cur_ = h.hunk; p_->copyHunk(h.hunk, h.toRight); return; }
            // click on a band selects the hunk
            const QTextBlock b = p_->left_->cursorForPosition(QPoint(2, int(e->position().y()))).block();
            if (b.isValid() && b.blockNumber() < (int)p_->rows_.size()) { const int h = p_->rows_[b.blockNumber()].hunk; if (h >= 0 && p_->res_.hunks[h].kind != Kind::Equal) p_->goToHunk(h); }
        }
    private:
        struct Hit { QRectF rect; int hunk; bool toRight; };
        ComparePane* p_; std::vector<Hit> hits_;
    };
    // ---- location pane: the whole comparison as a map
    class LocationPane : public QWidget {
    public:
        explicit LocationPane(ComparePane* p) : QWidget(p), p_(p) { setFixedWidth(16); setToolTip("Map of the whole comparison — click to jump"); }
    protected:
        void paintEvent(QPaintEvent*) override {
            QPainter g(this); g.fillRect(rect(), QColor(0xEF, 0xEA, 0xDF));
            const int R = (int)p_->rows_.size(); if (R == 0) return;
            const double scale = double(height()) / R;
            int i = 0;
            while (i < R) {
                const int hunk = p_->rows_[i].hunk; int j = i; while (j < R && p_->rows_[j].hunk == hunk) ++j;
                if (hunk >= 0 && !p_->rows_[i].gap) { const Hunk& h = p_->res_.hunks[hunk]; const QColor c = cmp::colFor(h); if (c.alpha()) g.fillRect(QRectF(2, i * scale, width() - 4, std::max(2.0, (j - i) * scale)), hunk == p_->cur_ ? c.darker(150) : c.darker(115)); }
                i = j;
            }
            // viewport indicator
            const QTextBlock f = p_->left_->firstVisible(); int vis = 0;
            for (QTextBlock b = f; b.isValid() && p_->left_->blockRect(b).top() < p_->left_->viewport()->height(); b = b.next()) ++vis;
            g.fillRect(QRectF(0, f.blockNumber() * scale, width(), std::max(3.0, vis * scale)), QColor(0x9A, 0x7A, 0x33, 60));
        }
        void mousePressEvent(QMouseEvent* e) override {
            const int R = (int)p_->rows_.size(); if (!R) return;
            const int row = std::clamp(int(e->position().y() / height() * R), 0, R - 1);
            QTextCursor c(p_->left_->document()->findBlockByNumber(row)); p_->left_->setTextCursor(c); p_->left_->centerCursor();
        }
    private:
        ComparePane* p_;
    };

    QString root_;
    QStackedWidget* pages_ = nullptr; QWidget* textPage_ = nullptr; QButtonGroup* pageBtns_ = nullptr;
    std::vector<std::pair<int, int>> pins_;
    QStringList aLines_, bLines_;
    QString pathL_, pathR_, nameLText_, nameRText_, eolL_ = "\n", eolR_ = "\n";
    bool dirtyL_ = false, dirtyR_ = false, dirtyEditors_ = false, binary_ = false;
    Options opts_;
    Result res_;
    std::vector<cmp::Row> rows_;
    cmp::Show show_ = cmp::Show::All; int ctx_ = 3; bool minorAsDiff_ = false;
    int cur_ = -1;
    std::vector<std::pair<QStringList, QStringList>> undo_, redo_;
    bool rendering_ = false, syncing_ = false, syncingRules_ = false, syncingView_ = false;
    QTimer rescan_;
    DiffEdit* left_ = nullptr; DiffEdit* right_ = nullptr;
    LinkStrip* link_ = nullptr; LocationPane* loc_ = nullptr;
    QLabel* nameL_ = nullptr; QLabel* nameR_ = nullptr; QLabel* status_ = nullptr; QLabel* detailLbl_ = nullptr;
    QWidget* details_ = nullptr; QPlainTextEdit* detL_ = nullptr; QPlainTextEdit* detR_ = nullptr;
    QRadioButton* showAll_ = nullptr; QRadioButton* showDiff_ = nullptr; QRadioButton* showCtx_ = nullptr; QSpinBox* ctxSpin_ = nullptr; QCheckBox* minorT_ = nullptr;
    std::vector<std::pair<QCheckBox*, bool Options::*>> rules_;
};

// =====================================================================
// Folder Compare page (Beyond Compare / WinMerge / Araxis folder views):
// two trees classified Identical · Different · Minor only · Left only ·
// Right only; criteria bytes / rules-based text / size; sync copies with
// a preview and confirmation; snapshots (paths + sizes + SHA-1) for later
// comparison; HTML / CSV reports. Nothing here ever deletes a file.
class FolderComparePage : public QWidget {
public:
    enum Status { Identical, Different, MinorOnly, LeftOnly, RightOnly, Unreadable };
    struct Entry { QString rel, leftAbs, rightAbs; qint64 lsize = -1, rsize = -1; QDateTime lmod, rmod; Status status = Identical; };

    explicit FolderComparePage(ComparePane* owner) : QWidget(owner), owner_(owner) {
        auto* v = new QVBoxLayout(this); v->setContentsMargins(6, 4, 6, 4); v->setSpacing(4);
        auto* top = new QGridLayout;
        leftEd_ = new QLineEdit; rightEd_ = new QLineEdit; leftEd_->setPlaceholderText("Left folder"); rightEd_->setPlaceholderText("Right folder");
        auto* bl = new QPushButton("Browse…"); auto* br = new QPushButton("Browse…");
        connect(bl, &QPushButton::clicked, this, [this] { const QString d = safeGetExistingDirectory(this, "Left folder", leftEd_->text()); if (!d.isEmpty()) leftEd_->setText(d); });
        connect(br, &QPushButton::clicked, this, [this] { const QString d = safeGetExistingDirectory(this, "Right folder", rightEd_->text()); if (!d.isEmpty()) rightEd_->setText(d); });
        top->addWidget(new QLabel("Left:"), 0, 0); top->addWidget(leftEd_, 0, 1); top->addWidget(bl, 0, 2);
        top->addWidget(new QLabel("Right:"), 1, 0); top->addWidget(rightEd_, 1, 1); top->addWidget(br, 1, 2);
        v->addLayout(top);
        auto* opts = new QHBoxLayout;
        filter_ = new QLineEdit("*"); filter_->setMaximumWidth(160); filter_->setToolTip("File name patterns, ; separated (e.g. *.act;*.txt)");
        crit_ = new QComboBox; crit_->addItems({"Compare bytes (exact)", "Rules-based text (the Compare rules)", "Size only (fast)"}); crit_->setToolTip("How two files with the same name are judged");
        recurse_ = new QCheckBox("Subfolders"); recurse_->setChecked(true);
        hidden_ = new QCheckBox("Hidden files");
        show_ = new QComboBox; show_->addItems({"Show all", "Differences", "Left only", "Right only", "Identical", "Minor only"});
        connect(show_, qOverload<int>(&QComboBox::currentIndexChanged), this, [this](int) { refreshTable(); });
        auto* go = new QPushButton("Compare"); go->setDefault(true);
        connect(go, &QPushButton::clicked, this, [this] { compare(leftEd_->text().trimmed(), rightEd_->text().trimmed()); });
        auto* swap = new QPushButton("Swap"); connect(swap, &QPushButton::clicked, this, [this] { const QString l = leftEd_->text(); leftEd_->setText(rightEd_->text()); rightEd_->setText(l); if (!entries_.empty()) compare(leftEd_->text(), rightEd_->text()); });
        opts->addWidget(new QLabel("Files:")); opts->addWidget(filter_); opts->addWidget(crit_); opts->addWidget(recurse_); opts->addWidget(hidden_); opts->addStretch(1); opts->addWidget(show_); opts->addWidget(go); opts->addWidget(swap);
        v->addLayout(opts);
        table_ = new QTreeWidget; table_->setRootIsDecorated(false); table_->setAlternatingRowColors(true); table_->setSelectionMode(QAbstractItemView::ExtendedSelection);
        table_->setHeaderLabels({"Name", "Status", "Left size", "Right size", "Left modified", "Right modified"});
        table_->header()->setSectionResizeMode(0, QHeaderView::Stretch);
        connect(table_, &QTreeWidget::itemDoubleClicked, this, [this](QTreeWidgetItem* it, int) { openEntry(it->data(0, Qt::UserRole).toInt()); });
        v->addWidget(table_, 1);
        auto* acts = new QHBoxLayout;
        auto act = [&](const QString& label, const QString& tip, std::function<void()> fn) { auto* b = new QPushButton(label); b->setToolTip(tip); connect(b, &QPushButton::clicked, this, std::move(fn)); acts->addWidget(b); return b; };
        act("Compare Files", "Open the selected pair in Text Compare (or double-click)", [this] { const auto sel = selectedRows(); if (!sel.empty()) openEntry(sel.front()); });
        act("← Copy to Left", "Copy the selected right-side files to the left folder (preview first; replaces same-named files; deletes nothing)", [this] { copySelected(false); });
        act("Copy to Right →", "Copy the selected left-side files to the right folder (preview first)", [this] { copySelected(true); });
        act("← Update Left", "Copy every right-only and different file to the left (preview first)", [this] { updateSide(false); });
        act("Update Right →", "Copy every left-only and different file to the right (preview first)", [this] { updateSide(true); });
        acts->addStretch(1);
        act("Save Snapshot…", "Record the LEFT folder's paths, sizes and SHA-1 hashes to a file, to compare against later", [this] {
            if (leftEd_->text().isEmpty()) { status("Choose a left folder first."); return; }
            const QString out = safeGetSaveFileName(this, "Save folder snapshot", QString(), "Folder snapshot (*.snapshot.json)");
            if (!out.isEmpty() && saveSnapshot(leftEd_->text(), out)) status("Snapshot saved: " + out);
        });
        act("Compare with Snapshot…", "Compare the LEFT folder against a saved snapshot (hashes; nothing can be copied from a snapshot)", [this] {
            if (leftEd_->text().isEmpty()) { status("Choose a left folder first."); return; }
            const QString in = safeGetOpenFileName(this, "Open folder snapshot", QString(), "Folder snapshot (*.snapshot.json)");
            if (!in.isEmpty()) compareWithSnapshot(leftEd_->text(), in);
        });
        act("Save Report…", "HTML or CSV listing of every entry and its status", [this] {
            const QString out = safeGetSaveFileName(this, "Save folder report", QString(), "HTML (*.html);;CSV (*.csv)");
            if (out.isEmpty()) return;
            if (saveOrWarn(this, out, report(QFileInfo(out).suffix().toLower() == "csv" ? "csv" : "html").toUtf8(), "folder report")) status("Report saved: " + out);
        });
        v->addLayout(acts);
        summary_ = new QLabel("Choose two folders and press Compare."); summary_->setStyleSheet("color:#4A3F33; padding:2px;"); v->addWidget(summary_);
    }

    // ---------------------------------------------------------- API
    void compare(const QString& left, const QString& right) {
        snapMode_ = false; leftEd_->setText(left); rightEd_->setText(right);
        if (!QFileInfo(left).isDir() || !QFileInfo(right).isDir()) { status("Both sides must be folders."); return; }
        QApplication::setOverrideCursor(g_busyCursor());
        const auto L = scan(left), R = scan(right);
        entries_.clear();
        QSet<QString> keys; for (auto it = L.cbegin(); it != L.cend(); ++it) keys << it.key(); for (auto it = R.cbegin(); it != R.cend(); ++it) keys << it.key();
        QStringList sorted(keys.begin(), keys.end()); sorted.sort(Qt::CaseInsensitive);
        int n = 0;
        for (const QString& k : sorted) {
            Entry e; e.rel = k;
            if (L.contains(k)) { e.leftAbs = L[k].absoluteFilePath(); e.lsize = L[k].size(); e.lmod = L[k].lastModified(); }
            if (R.contains(k)) { e.rightAbs = R[k].absoluteFilePath(); e.rsize = R[k].size(); e.rmod = R[k].lastModified(); }
            e.status = e.leftAbs.isEmpty() ? RightOnly : e.rightAbs.isEmpty() ? LeftOnly : classify(e);
            entries_.push_back(e);
            if (++n % 50 == 0) QCoreApplication::processEvents(QEventLoop::ExcludeUserInputEvents);
        }
        QApplication::restoreOverrideCursor();
        refreshTable();
        owner_->remember("folders", left, right);
    }
    bool saveSnapshot(const QString& dir, const QString& outPath) {
        if (!QFileInfo(dir).isDir()) return false;
        QJsonArray arr;
        const auto files = scan(dir);
        for (auto it = files.cbegin(); it != files.cend(); ++it) {
            QJsonObject o; o["path"] = it.key(); o["size"] = it.value().size(); o["mtime"] = it.value().lastModified().toString(Qt::ISODate); o["sha1"] = sha1Of(it.value().absoluteFilePath());
            arr.append(o);
        }
        QJsonObject root; root["root"] = dir; root["taken"] = QDateTime::currentDateTime().toString(Qt::ISODate); root["entries"] = arr;
        return saveOrWarn(this, outPath, QJsonDocument(root).toJson(), "folder snapshot");
    }
    bool compareWithSnapshot(const QString& left, const QString& snapshotPath) {
        QFile f(snapshotPath); if (!f.open(QIODevice::ReadOnly)) { status("Could not read " + snapshotPath); return false; }
        const QJsonObject root = QJsonDocument::fromJson(f.readAll()).object();
        if (!root.contains("entries")) { status("Not a folder snapshot: " + snapshotPath); return false; }
        snapMode_ = true; leftEd_->setText(left); rightEd_->setText("snapshot: " + QFileInfo(snapshotPath).fileName());
        const auto L = scan(left);
        QMap<QString, QJsonObject> S; for (const auto& v : root["entries"].toArray()) S[v.toObject()["path"].toString()] = v.toObject();
        entries_.clear();
        QSet<QString> keys; for (auto it = L.cbegin(); it != L.cend(); ++it) keys << it.key(); for (auto it = S.cbegin(); it != S.cend(); ++it) keys << it.key();
        QStringList sorted(keys.begin(), keys.end()); sorted.sort(Qt::CaseInsensitive);
        for (const QString& k : sorted) {
            Entry e; e.rel = k;
            if (L.contains(k)) { e.leftAbs = L[k].absoluteFilePath(); e.lsize = L[k].size(); e.lmod = L[k].lastModified(); }
            if (S.contains(k)) { e.rightAbs = "(snapshot)"; e.rsize = qint64(S[k]["size"].toDouble()); e.rmod = QDateTime::fromString(S[k]["mtime"].toString(), Qt::ISODate); }
            if (e.leftAbs.isEmpty()) e.status = RightOnly;
            else if (S.contains(k)) e.status = sha1Of(e.leftAbs) == S[k]["sha1"].toString() ? Identical : Different;
            else e.status = LeftOnly;
            entries_.push_back(e);
        }
        refreshTable();
        return true;
    }
    const std::vector<Entry>& entries() const { return entries_; }
    int count(Status s) const { int n = 0; for (const auto& e : entries_) if (e.status == s) ++n; return n; }
    std::vector<int> visibleRows() const {
        std::vector<int> v;
        const int mode = show_->currentIndex();
        for (int i = 0; i < (int)entries_.size(); ++i) {
            const Status s = entries_[i].status;
            const bool ok = mode == 0 || (mode == 1 && s != Identical) || (mode == 2 && s == LeftOnly) || (mode == 3 && s == RightOnly) || (mode == 4 && s == Identical) || (mode == 5 && s == MinorOnly);
            if (ok) v.push_back(i);
        }
        return v;
    }
    void setShowMode(int m) { show_->setCurrentIndex(m); }
    void setCriteria(int c) { crit_->setCurrentIndex(c); }

    // sync operations: preview, confirm, copy; never delete
    int copySelected(bool toRight) { std::vector<int> rows = selectedRows(); return copyRows(rows, toRight); }
    int updateSide(bool toRight) {
        std::vector<int> rows;
        for (int i = 0; i < (int)entries_.size(); ++i) { const Status s = entries_[i].status; if (s == Different || s == MinorOnly || (toRight ? s == LeftOnly : s == RightOnly)) rows.push_back(i); }
        return copyRows(rows, toRight);
    }
    QString report(const QString& kind) const {
        QString o;
        if (kind == "csv") {
            o += "path,status,left_size,right_size,left_modified,right_modified\n";
            for (const auto& e : entries_) o += QString("\"%1\",%2,%3,%4,%5,%6\n").arg(QString(e.rel).replace('"', "\"\""), statusName(e.status)).arg(e.lsize).arg(e.rsize).arg(e.lmod.toString(Qt::ISODate), e.rmod.toString(Qt::ISODate));
            return o;
        }
        o += "<!doctype html><meta charset='utf-8'><title>Folder compare</title><style>body{font-family:-apple-system,Helvetica,sans-serif;font-size:13px}table{border-collapse:collapse}td,th{padding:2px 8px;border-bottom:1px solid #eee;text-align:left}"
             "tr.d td{background:#fff3c4}tr.l td{background:#f8d7d5}tr.r td{background:#d9f5d3}tr.m td{background:#eeeeee}</style>";
        o += "<h2>Folder compare</h2><p>" + leftEd_->text().toHtmlEscaped() + " &nbsp;↔&nbsp; " + rightEd_->text().toHtmlEscaped() + "</p><p>" + summaryText().toHtmlEscaped() + "</p><table><tr><th>Name</th><th>Status</th><th>Left size</th><th>Right size</th><th>Left modified</th><th>Right modified</th></tr>";
        for (const auto& e : entries_) {
            const char* cls = e.status == Different ? "d" : e.status == LeftOnly ? "l" : e.status == RightOnly ? "r" : e.status == MinorOnly ? "m" : "";
            o += QString("<tr class='%1'><td>%2</td><td>%3</td><td>%4</td><td>%5</td><td>%6</td><td>%7</td></tr>").arg(cls, e.rel.toHtmlEscaped(), statusName(e.status), e.lsize >= 0 ? QString::number(e.lsize) : "", e.rsize >= 0 ? QString::number(e.rsize) : "", e.lmod.isValid() ? e.lmod.toString("yyyy-MM-dd HH:mm") : "", e.rmod.isValid() ? e.rmod.toString("yyyy-MM-dd HH:mm") : "");
        }
        return o + "</table>";
    }
    static QString statusName(Status s) { switch (s) { case Identical: return "Identical"; case Different: return "Different"; case MinorOnly: return "Minor only"; case LeftOnly: return "Left only"; case RightOnly: return "Right only"; default: return "Unreadable"; } }
    bool harnessAutoConfirm_ = false;

    int selfTest(QStringList& log) {
        int fails = 0;
        auto check = [&](bool ok, const char* what) { log << QString("  [%1] Compare/Folders: %2").arg(ok ? "PASS" : "FAIL").arg(what); if (!ok) ++fails; };
        const QString tmp = QDir::temp().filePath("all_fold_" + QString::number(QCoreApplication::applicationPid()));
        QDir(tmp).removeRecursively(); QDir().mkpath(tmp + "/L/sub"); QDir().mkpath(tmp + "/R/sub");
        auto put = [](const QString& p, const QByteArray& b) { QFile f(p); f.open(QIODevice::WriteOnly); f.write(b); f.close(); };
        put(tmp + "/L/same.act", "@001A\nSEMS CAN\n"); put(tmp + "/R/same.act", "@001A\nSEMS CAN\n");
        put(tmp + "/L/diff.act", "@001A\nSEMS CAN\n"); put(tmp + "/R/diff.act", "@001A\nSEMS CAN KUN\n");
        put(tmp + "/L/onlyL.act", "x\n"); put(tmp + "/R/onlyR.act", "y\n");
        put(tmp + "/L/minor.act", "SEMS  CAN\n"); put(tmp + "/R/minor.act", "SEMS CAN\n");
        put(tmp + "/L/sub/nested.txt", "n\n"); put(tmp + "/R/sub/nested.txt", "n\n");
        setCriteria(0); setShowMode(0);
        compare(tmp + "/L", tmp + "/R");
        check(entries_.size() == 6 && count(Identical) == 2 && count(Different) == 2 && count(LeftOnly) == 1 && count(RightOnly) == 1, "bytes criterion: 2 identical (one nested), 2 different, 1 left-only, 1 right-only");
        setShowMode(1); check(visibleRows().size() == 4, "Show ▸ Differences hides the identical pairs");
        setShowMode(2); check(visibleRows().size() == 1 && entries_[visibleRows()[0]].rel == "onlyL.act", "Show ▸ Left only lists onlyL.act");
        setShowMode(0);
        const bool keepWs = owner_->options().ignoreWhitespaceChange; owner_->options().ignoreWhitespaceChange = true;
        setCriteria(1); compare(tmp + "/L", tmp + "/R");
        check(count(MinorOnly) == 1 && count(Different) == 1, "rules-based criterion: the spacing-only pair is Minor only under Ignore spacing");
        owner_->options().ignoreWhitespaceChange = keepWs; setCriteria(0);
        check(report("csv").contains("\"onlyR.act\",Right only") && report("html").contains("diff.act"), "CSV and HTML folder reports list entries with status");
        check(saveSnapshot(tmp + "/L", tmp + "/L.snapshot.json") && QFile(tmp + "/L.snapshot.json").exists(), "Save Snapshot writes paths, sizes and SHA-1 hashes");
        put(tmp + "/L/diff.act", "@001A\nCHANGED\n");
        check(compareWithSnapshot(tmp + "/L", tmp + "/L.snapshot.json") && count(Different) == 1 && count(Identical) == 4 && snapMode_, "Compare with Snapshot finds exactly the file changed since the snapshot");
        compare(tmp + "/L", tmp + "/R");
        harnessAutoConfirm_ = true;
        const int copied = updateSide(true);
        harnessAutoConfirm_ = false;
        check(copied == 3 && QFile(tmp + "/R/onlyL.act").exists() && cmp::readText(tmp + "/R/diff.act").text == "@001A\nCHANGED\n" && QFile(tmp + "/R/onlyR.act").exists(),
              "Update Right copies left-only and different files, replaces, and deletes nothing");
        check(count(LeftOnly) == 0 && count(Different) == 0 && count(RightOnly) == 1, "after the update the folders differ only by the right-only file");
        harnessAutoConfirm_ = false;
        check(updateSide(false) == 0, "without confirmation (harness default) no file is copied");
        QDir(tmp).removeRecursively();
        entries_.clear(); refreshTable();
        return fails;
    }

private:
    QMap<QString, QFileInfo> scan(const QString& dir) const {
        QMap<QString, QFileInfo> out;
        const QStringList globs = filter_->text().split(';', Qt::SkipEmptyParts);
        QDir::Filters f = QDir::Files | QDir::NoDotAndDotDot; if (hidden_->isChecked()) f |= QDir::Hidden;
        QDirIterator it(dir, f, recurse_->isChecked() ? QDirIterator::Subdirectories : QDirIterator::NoIteratorFlags);
        const QDir base(dir);
        while (it.hasNext()) {
            it.next(); const QFileInfo fi = it.fileInfo();
            if (!globs.isEmpty() && !(globs.size() == 1 && globs[0].trimmed() == "*") && !QDir::match(globs, fi.fileName())) continue;
            out.insert(base.relativeFilePath(fi.absoluteFilePath()), fi);
        }
        return out;
    }
    Status classify(const Entry& e) const {
        const int crit = crit_->currentIndex();
        if (crit == 2) return e.lsize == e.rsize ? Identical : Different;
        if (crit == 0) {
            if (e.lsize != e.rsize) return Different;
            QFile a(e.leftAbs), b(e.rightAbs);
            if (!a.open(QIODevice::ReadOnly) || !b.open(QIODevice::ReadOnly)) return Unreadable;
            while (!a.atEnd() || !b.atEnd()) if (a.read(65536) != b.read(65536)) return Different;
            return Identical;
        }
        const auto A = cmp::readText(e.leftAbs), B = cmp::readText(e.rightAbs);
        if (!A.ok || !B.ok) return Unreadable;
        if (A.binary || B.binary) return A.raw == B.raw ? Identical : Different;
        const auto r = allcore::textdiff::diffLines(allcore::textdiff::splitLines(A.text.toStdString()), allcore::textdiff::splitLines(B.text.toStdString()), owner_->options());
        if (r.differences()) return Different;
        return r.unimportant ? MinorOnly : (A.raw == B.raw ? Identical : MinorOnly);
    }
    static QString sha1Of(const QString& path) {
        QFile f(path); if (!f.open(QIODevice::ReadOnly)) return {};
        QCryptographicHash h(QCryptographicHash::Sha1); h.addData(&f); return QString::fromLatin1(h.result().toHex());
    }
    QString summaryText() const {
        if (entries_.empty()) return "No entries.";
        return QString("%1 entries: %2 identical · %3 different · %4 minor only · %5 left only · %6 right only%7").arg(entries_.size()).arg(count(Identical)).arg(count(Different)).arg(count(MinorOnly)).arg(count(LeftOnly)).arg(count(RightOnly)).arg(count(Unreadable) ? QString(" · %1 unreadable").arg(count(Unreadable)) : "");
    }
    void refreshTable() {
        table_->clear();
        for (int i : visibleRows()) {
            const Entry& e = entries_[i];
            auto* it = new QTreeWidgetItem({e.rel, statusName(e.status), e.lsize >= 0 ? QString::number(e.lsize) : "", e.rsize >= 0 ? QString::number(e.rsize) : "",
                                            e.lmod.isValid() ? e.lmod.toString("yyyy-MM-dd HH:mm") : "", e.rmod.isValid() ? e.rmod.toString("yyyy-MM-dd HH:mm") : ""});
            it->setData(0, Qt::UserRole, i);
            const QColor c = e.status == Different ? cmp::colChange() : e.status == LeftOnly ? cmp::colDelete() : e.status == RightOnly ? cmp::colInsert() : e.status == MinorOnly ? cmp::colMinor() : QColor(Qt::transparent);
            if (c.alpha()) for (int col = 0; col < 6; ++col) it->setBackground(col, c);
            table_->addTopLevelItem(it);
        }
        summary_->setText(summaryText() + (snapMode_ ? "  (snapshot: hashes only; nothing can be copied)" : ""));
    }
    std::vector<int> selectedRows() const { std::vector<int> v; for (auto* it : table_->selectedItems()) v.push_back(it->data(0, Qt::UserRole).toInt()); std::sort(v.begin(), v.end()); v.erase(std::unique(v.begin(), v.end()), v.end()); return v; }
    void openEntry(int i) {
        if (i < 0 || i >= (int)entries_.size()) return;
        const Entry& e = entries_[i];
        if (snapMode_) { status("A snapshot holds hashes, not contents; open the folder pair to compare files."); return; }
        if (e.leftAbs.isEmpty() || e.rightAbs.isEmpty()) { status(e.rel + " exists on one side only."); return; }
        owner_->compareFiles(e.leftAbs, e.rightAbs);
    }
    bool confirmOps(const QStringList& lines, int replaced, bool toRight) {
        if (g_harnessRun) return harnessAutoConfirm_;
        QDialog d(this); d.setWindowTitle(toRight ? "Copy to the right folder" : "Copy to the left folder");
        auto* v = new QVBoxLayout(&d);
        v->addWidget(new QLabel(QString("%1 file%2 will be copied; %3 existing file%4 will be replaced. Nothing is deleted.").arg(lines.size()).arg(lines.size() == 1 ? "" : "s").arg(replaced).arg(replaced == 1 ? "" : "s")));
        auto* list = new QListWidget; list->addItems(lines); v->addWidget(list, 1);
        auto* bb = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel); bb->button(QDialogButtonBox::Ok)->setText("Copy"); v->addWidget(bb);
        connect(bb, &QDialogButtonBox::accepted, &d, &QDialog::accept); connect(bb, &QDialogButtonBox::rejected, &d, &QDialog::reject);
        d.resize(560, 400);
        return d.exec() == QDialog::Accepted;
    }
    int copyRows(const std::vector<int>& rows, bool toRight) {
        if (snapMode_) { status("A snapshot holds hashes, not contents; nothing can be copied from it."); return 0; }
        const QString srcRoot = toRight ? leftEd_->text() : rightEd_->text(), dstRoot = toRight ? rightEd_->text() : leftEd_->text();
        QStringList lines; std::vector<std::pair<QString, QString>> ops; int replaced = 0;
        for (int i : rows) {
            if (i < 0 || i >= (int)entries_.size()) continue;
            const Entry& e = entries_[i];
            const QString src = toRight ? e.leftAbs : e.rightAbs; if (src.isEmpty()) continue;
            const QString dst = QDir(dstRoot).filePath(e.rel);
            const bool exists = QFile::exists(dst); if (exists) ++replaced;
            lines << (e.rel + (exists ? "   (replaces)" : "   (new)")); ops.push_back({src, dst});
        }
        if (ops.empty()) { status("Nothing to copy for that selection."); return 0; }
        if (!confirmOps(lines, replaced, toRight)) { status("Copy cancelled."); return 0; }
        int done = 0, failed = 0;
        for (const auto& op : ops) {
            QDir().mkpath(QFileInfo(op.second).path());
            if (QFile::exists(op.second) && !QFile::remove(op.second)) { ++failed; continue; }
            if (QFile::copy(op.first, op.second)) ++done; else ++failed;
        }
        compare(leftEd_->text(), rightEd_->text());
        status(QString("Copied %1 file%2%3.").arg(done).arg(done == 1 ? "" : "s").arg(failed ? QString("; %1 FAILED (check permissions)").arg(failed) : ""));
        Q_UNUSED(srcRoot);
        return done;
    }
    void status(const QString& m) { owner_->statusLabel()->setText(m); }

    ComparePane* owner_;
    QLineEdit* leftEd_ = nullptr; QLineEdit* rightEd_ = nullptr; QLineEdit* filter_ = nullptr;
    QComboBox* crit_ = nullptr; QComboBox* show_ = nullptr; QCheckBox* recurse_ = nullptr; QCheckBox* hidden_ = nullptr;
    QTreeWidget* table_ = nullptr; QLabel* summary_ = nullptr;
    std::vector<Entry> entries_; bool snapMode_ = false;
};

// =====================================================================
// Three-Way Merge page (diff3 / Meld / Araxis / WinMerge 3-way): Left ·
// Base · Right above, the merged text below; non-conflicting changes are
// merged automatically, conflicts are listed and resolved one by one.
class ThreeWayPage : public QWidget {
public:
    using Chunk = allcore::textdiff::Chunk; using ChunkKind = allcore::textdiff::ChunkKind;
    explicit ThreeWayPage(ComparePane* owner) : QWidget(owner), owner_(owner) {
        auto* v = new QVBoxLayout(this); v->setContentsMargins(6, 4, 6, 4); v->setSpacing(4);
        auto* top = new QGridLayout;
        const char* labels[3] = {"Left (mine):", "Base (common ancestor):", "Right (theirs):"};
        for (int i = 0; i < 3; ++i) {
            paths_[i] = new QLineEdit; auto* b = new QPushButton("Browse…");
            connect(b, &QPushButton::clicked, this, [this, i] { const QString f = safeGetOpenFileName(this, "Choose file", paths_[i]->text(), "Texts (*.txt *.act *.inc *.md);;All files (*)"); if (!f.isEmpty()) paths_[i]->setText(f); });
            top->addWidget(new QLabel(labels[i]), i, 0); top->addWidget(paths_[i], i, 1); top->addWidget(b, i, 2);
        }
        auto* go = new QPushButton("Merge"); connect(go, &QPushButton::clicked, this, [this] { mergeFiles(paths_[1]->text().trimmed(), paths_[0]->text().trimmed(), paths_[2]->text().trimmed()); });
        top->addWidget(go, 1, 3);
        v->addLayout(top);
        auto* split = new QSplitter(Qt::Vertical);
        auto* threeW = new QWidget; auto* th = new QHBoxLayout(threeW); th->setContentsMargins(0, 0, 0, 0); th->setSpacing(2);
        for (int i = 0; i < 3; ++i) { auto* col = new QVBoxLayout; col->setContentsMargins(0, 0, 0, 0); heads_[i] = new QLabel(labels[i]); heads_[i]->setStyleSheet("font-weight:600;"); eds_[i] = new DiffEdit; eds_[i]->setReadOnly(true); col->addWidget(heads_[i]); col->addWidget(eds_[i], 1); th->addLayout(col, 1); }
        split->addWidget(threeW);
        auto* bottom = new QWidget; auto* bh = new QHBoxLayout(bottom); bh->setContentsMargins(0, 0, 0, 0);
        auto* mergedCol = new QVBoxLayout; mergedHead_ = new QLabel("Merged result"); mergedHead_->setStyleSheet("font-weight:600;"); merged_ = new DiffEdit; mergedCol->addWidget(mergedHead_); mergedCol->addWidget(merged_, 1);
        bh->addLayout(mergedCol, 3);
        auto* side = new QVBoxLayout; side->addWidget(new QLabel("Conflicts"));
        list_ = new QListWidget; connect(list_, &QListWidget::currentRowChanged, this, [this](int r) { focusConflict(r); }); side->addWidget(list_, 1);
        auto row = [&](std::initializer_list<std::pair<const char*, std::function<void()>>> items) { auto* h = new QHBoxLayout; for (auto& it : items) { auto* b = new QPushButton(it.first); connect(b, &QPushButton::clicked, this, it.second); h->addWidget(b); } side->addLayout(h); };
        row({{"Take Left", [this] { resolveSelected(1); }}, {"Take Right", [this] { resolveSelected(2); }}});
        row({{"Take Both", [this] { resolveSelected(3); }}, {"Take Base", [this] { resolveSelected(4); }}});
        row({{"All Left", [this] { resolveAll(1); }}, {"All Right", [this] { resolveAll(2); }}});
        row({{"◀ Prev", [this] { if (list_->currentRow() > 0) list_->setCurrentRow(list_->currentRow() - 1); }}, {"Next ▶", [this] { if (list_->currentRow() + 1 < list_->count()) list_->setCurrentRow(list_->currentRow() + 1); }}});
        row({{"Save Merged As…", [this] { saveMergedAs(); }}, {"Merged vs Base", [this] { owner_->compareTexts("Base", baseText(), "Merged", mergedText()); }}});
        bh->addLayout(side, 1);
        split->addWidget(bottom);
        v->addWidget(split, 1);
        status_ = new QLabel("Choose Left, Base and Right, then Merge."); status_->setStyleSheet("color:#4A3F33; padding:2px;"); v->addWidget(status_);
    }

    // ---------------------------------------------------------- API
    void mergeFiles(const QString& base, const QString& left, const QString& right) {
        const auto B = cmp::readText(base), L = cmp::readText(left), R = cmp::readText(right);
        if (!B.ok || !L.ok || !R.ok) { status_->setText("Could not read one of the three files."); return; }
        if (B.binary || L.binary || R.binary) { status_->setText("Three-way merge works on text files; one of these is binary."); return; }
        paths_[0]->setText(left); paths_[1]->setText(base); paths_[2]->setText(right);
        eol_ = L.eol;
        mergeTexts(QFileInfo(base).fileName(), B.text, QFileInfo(left).fileName(), L.text, QFileInfo(right).fileName(), R.text);
    }
    void mergeTexts(const QString& baseName, const QString& baseText, const QString& leftName, const QString& leftText, const QString& rightName, const QString& rightText) {
        names_[0] = leftName; names_[1] = baseName; names_[2] = rightName;
        lines_[0] = cmp::linesOf(leftText); lines_[1] = cmp::linesOf(baseText); lines_[2] = cmp::linesOf(rightText);
        merge_ = allcore::textdiff::merge3(cmp::toStd(lines_[1]), cmp::toStd(lines_[0]), cmp::toStd(lines_[2]), owner_->options());
        render();
        list_->setCurrentRow(list_->count() ? 0 : -1);
    }
    QString mergedText() const { return merged_->toPlainText(); }
    QString baseText() const { return lines_[1].join('\n') + (lines_[1].isEmpty() ? "" : "\n"); }
    int conflicts() const { return merge_.conflicts; }
    int unresolved() const { int n = 0; for (const auto& c : merge_.chunks) if (c.kind == ChunkKind::Conflict && c.resolution == 0) ++n; return n; }
    bool resolve(int conflictIndex, int resolution) {
        int k = 0;
        for (auto& c : merge_.chunks) if (c.kind == ChunkKind::Conflict) { if (k == conflictIndex) { c.resolution = resolution; render(); list_->setCurrentRow(conflictIndex); return true; } ++k; }
        return false;
    }
    void resolveAll(int resolution) { for (auto& c : merge_.chunks) if (c.kind == ChunkKind::Conflict) c.resolution = resolution; render(); }
    bool saveMergedAs(QString path = QString()) {
        if (path.isEmpty()) path = safeGetSaveFileName(this, "Save merged text as", paths_[0]->text(), "Texts (*.txt *.act *.inc *.md);;All files (*)");
        if (path.isEmpty()) return false;
        if (unresolved()) status_->setText(QString("Saved with %1 unresolved conflict%2 still marked <<<<<<< ======= >>>>>>>.").arg(unresolved()).arg(unresolved() == 1 ? "" : "s"));
        QString text = mergedText(); if (eol_ != "\n") text.replace("\n", eol_);
        if (!saveOrWarn(this, path, text.toUtf8(), "merged text")) return false;
        if (!unresolved()) status_->setText("Merged text saved: " + path);
        return true;
    }
    void pickFiles() { if (paths_[1]->text().isEmpty()) paths_[1]->setFocus(); }

    int selfTest(QStringList& log) {
        int fails = 0;
        auto check = [&](bool ok, const char* what) { log << QString("  [%1] Compare/Three-way: %2").arg(ok ? "PASS" : "FAIL").arg(what); if (!ok) ++fails; };
        mergeTexts("base", "a\nb\nc\nd\ne\n", "left", "a\nB\nc\nd\ne\n", "right", "a\nb\nc\nd\nE\n");
        check(conflicts() == 0 && mergedText() == "a\nB\nc\nd\nE\n", "non-overlapping edits merge automatically into one text");
        mergeTexts("base", "a\nb\nc\nd\ne\n", "left", "a\nX\nc\nd\nE\n", "right", "a\nY\nc\nd\nE\n");
        check(conflicts() == 1 && unresolved() == 1 && list_->count() == 1 && mergedText().contains("<<<<<<< left\nX\n=======\nY\n>>>>>>> right"), "one conflict listed and marked; the shared edit E is taken once");
        check(resolve(0, 2) && unresolved() == 0 && mergedText() == "a\nY\nc\nd\nE\n", "Take Right resolves it and re-renders the merged text");
        resolveAll(3); check(mergedText() == "a\nX\nY\nc\nd\nE\n", "Take Both keeps both readings in order");
        const QString tmp = QDir::temp().filePath("all_3w_" + QString::number(QCoreApplication::applicationPid()));
        QDir().mkpath(tmp);
        auto put = [](const QString& p, const QByteArray& b) { QFile f(p); f.open(QIODevice::WriteOnly); f.write(b); f.close(); };
        put(tmp + "/base.act", "@001A\r\nSEMS CAN\r\nBDE BA\r\n"); put(tmp + "/mine.act", "@001A\r\nSEMS CAN THAMS CAD\r\nBDE BA\r\n"); put(tmp + "/theirs.act", "@001A\r\nSEMS CAN\r\nBDE BA CHEN PO\r\n");
        mergeFiles(tmp + "/base.act", tmp + "/mine.act", tmp + "/theirs.act");
        check(conflicts() == 0 && mergedText() == "@001A\nSEMS CAN THAMS CAD\nBDE BA CHEN PO\n", "three ACIP files: each side's edit lands");
        check(saveMergedAs(tmp + "/merged.act") && cmp::readText(tmp + "/merged.act").raw == QByteArray("@001A\r\nSEMS CAN THAMS CAD\r\nBDE BA CHEN PO\r\n"), "Save Merged As writes the left file's line ending (CRLF)");
        QDir(tmp).removeRecursively();
        mergeTexts("", "", "", "", "", "");
        return fails;
    }

private:
    void render() {
        for (int i = 0; i < 3; ++i) { eds_[i]->setPlainText(lines_[i].join('\n')); heads_[i]->setText((i == 0 ? "Left: " : i == 1 ? "Base: " : "Right: ") + names_[i]); }
        QList<QTextEdit::ExtraSelection> sel[3]; QVector<QColor> bars[3];
        for (int i = 0; i < 3; ++i) bars[i] = QVector<QColor>(lines_[i].size(), Qt::transparent);
        auto paint = [&](int ed, int lo, int hi, const QColor& c) { for (int k = lo; k < hi && k < lines_[ed].size(); ++k) { QTextBlock b = eds_[ed]->document()->findBlockByNumber(k); QTextEdit::ExtraSelection s; s.cursor = QTextCursor(b); s.format.setBackground(c); s.format.setProperty(QTextFormat::FullWidthSelection, true); sel[ed] << s; bars[ed][k] = c; } };
        for (const auto& c : merge_.chunks) {
            switch (c.kind) {
            case ChunkKind::Left: paint(0, c.leftBeg, c.leftEnd, cmp::colChange()); paint(1, c.baseBeg, c.baseEnd, cmp::colMinor()); break;
            case ChunkKind::Right: paint(2, c.rightBeg, c.rightEnd, cmp::colChange()); paint(1, c.baseBeg, c.baseEnd, cmp::colMinor()); break;
            case ChunkKind::Both: paint(0, c.leftBeg, c.leftEnd, cmp::colMoved()); paint(2, c.rightBeg, c.rightEnd, cmp::colMoved()); paint(1, c.baseBeg, c.baseEnd, cmp::colMinor()); break;
            case ChunkKind::Conflict: paint(0, c.leftBeg, c.leftEnd, cmp::colDelete()); paint(2, c.rightBeg, c.rightEnd, cmp::colDelete()); paint(1, c.baseBeg, c.baseEnd, cmp::colDelete()); break;
            default: break;
            }
        }
        for (int i = 0; i < 3; ++i) { eds_[i]->setExtraSelections(sel[i]); eds_[i]->setRowColors(bars[i]); }
        const auto out = allcore::textdiff::renderMerge(merge_, cmp::toStd(lines_[1]), cmp::toStd(lines_[0]), cmp::toStd(lines_[2]), names_[0].toStdString(), names_[2].toStdString());
        merged_->setPlainText(cmp::fromStd(out).join('\n') + (out.empty() ? "" : "\n"));
        // mark unresolved conflict blocks in the merged text
        QList<QTextEdit::ExtraSelection> ms; QVector<QColor> mb(merged_->document()->blockCount(), Qt::transparent);
        bool in = false;
        for (QTextBlock b = merged_->document()->begin(); b.isValid(); b = b.next()) {
            const QString t = b.text();
            if (t.startsWith("<<<<<<<")) in = true;
            if (in) { QTextEdit::ExtraSelection s; s.cursor = QTextCursor(b); s.format.setBackground(cmp::colDelete()); s.format.setProperty(QTextFormat::FullWidthSelection, true); ms << s; if (b.blockNumber() < mb.size()) mb[b.blockNumber()] = cmp::colDelete(); }
            if (t.startsWith(">>>>>>>")) in = false;
        }
        merged_->setExtraSelections(ms); merged_->setRowColors(mb);
        list_->clear(); int k = 0;
        for (const auto& c : merge_.chunks) if (c.kind == ChunkKind::Conflict) {
            ++k;
            const char* res = c.resolution == 1 ? "Left" : c.resolution == 2 ? "Right" : c.resolution == 3 ? "Both" : c.resolution == 4 ? "Base" : "unresolved";
            list_->addItem(QString("Conflict %1 — base lines %2–%3 · left %4 line%5 · right %6 line%7 · %8").arg(k).arg(c.baseBeg + 1).arg(std::max(c.baseBeg + 1, c.baseEnd)).arg(c.leftEnd - c.leftBeg).arg(c.leftEnd - c.leftBeg == 1 ? "" : "s").arg(c.rightEnd - c.rightBeg).arg(c.rightEnd - c.rightBeg == 1 ? "" : "s").arg(res));
        }
        mergedHead_->setText(QString("Merged result — %1 conflict%2, %3 unresolved, %4 change%5 merged automatically").arg(merge_.conflicts).arg(merge_.conflicts == 1 ? "" : "s").arg(unresolved()).arg(merge_.autoMerged).arg(merge_.autoMerged == 1 ? "" : "s"));
        status_->setText(merge_.conflicts ? "Select a conflict and Take Left / Right / Both / Base; resolving re-renders the merged text (edit it by hand last)." : "No conflicts — the merged text is ready to save.");
    }
    void focusConflict(int row) {
        if (row < 0) return;
        int k = 0;
        for (const auto& c : merge_.chunks) if (c.kind == ChunkKind::Conflict) {
            if (k == row) {
                auto centre = [](DiffEdit* e, int line) { QTextCursor cur(e->document()->findBlockByNumber(std::max(0, line))); e->setTextCursor(cur); e->centerCursor(); };
                centre(eds_[0], c.leftBeg); centre(eds_[1], c.baseBeg); centre(eds_[2], c.rightBeg);
                return;
            }
            ++k;
        }
    }
    void resolveSelected(int r) { if (list_->currentRow() < 0) { status_->setText("Select a conflict first."); return; } resolve(list_->currentRow(), r); }

    ComparePane* owner_;
    QLineEdit* paths_[3] = {nullptr, nullptr, nullptr}; QLabel* heads_[3] = {nullptr, nullptr, nullptr}; DiffEdit* eds_[3] = {nullptr, nullptr, nullptr};
    QLabel* mergedHead_ = nullptr; DiffEdit* merged_ = nullptr; QListWidget* list_ = nullptr; QLabel* status_ = nullptr;
    QString names_[3]; QStringList lines_[3]; QString eol_ = "\n";
    allcore::textdiff::MergeResult merge_;
};

struct ComparePages { FolderComparePage* folders = nullptr; ThreeWayPage* three = nullptr; };
inline ComparePages installComparePages(ComparePane* p) {
    ComparePages pg;
    pg.folders = new FolderComparePage(p); p->addPage(pg.folders);
    pg.three = new ThreeWayPage(p); p->addPage(pg.three);
    g_compareFolders = [p, f = pg.folders](const QString& a, const QString& b) { p->showPage(1); f->compare(a, b); if (g_raisePane) g_raisePane(p); };
    p->openThreeWay_ = [p, t = pg.three] { p->showPage(2); t->pickFiles(); };
    return pg;
}
