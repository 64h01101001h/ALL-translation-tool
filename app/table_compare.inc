// app/table_compare.inc — F7 Table Compare (analysis suite, batch 5, 2026-09-09):
// the Compare pane's fourth page. Two moments: a new dictionary release arrives
// and the translator needs the headwords whose values changed, side by side,
// not a 63 MB text diff; two glossaries have drifted and the rows where they
// disagree are wanted — with the app REFUSING to guess when the same headword
// appears twice. The engine is allcore::table (the one delimited reader) +
// allcore::tablediff (a relational full outer join by key; battery
// tablediff_smoke). JSON shapes are adapted HERE, so allcore gains no JSON
// parser. Included from main.cpp right after compare_pane.inc.
//
// Honesty rules this page enforces: a duplicated key is AMBIGUOUS and never
// paired by position; no key chosen → a refusal listing the column names,
// never column 0; a column present on one side never reads as "changed to
// empty"; a row whose cell count disagrees with the header is refused and
// listed, never padded; the release TSV is declared lossy; an alignment
// file's own TENTATIVE tier is carried verbatim; both sides are identified
// (path, size, sha256, mtime, the file's own version); nothing is written —
// this page compares, it does not merge.

namespace tablecmp {
using allcore::table::Table;
using allcore::table::Format;

struct Preset {
    QString name, hint, keyCol, key2Col, listCols, bannerLossy, bannerTier;
    Format format = Format::Auto;
    QStringList impliedColumns;        // headerless house TSVs
    QStringList pinned;                // provenance columns that cannot be unticked
    QString filter;                    // file dialog filter
    QString jsonShape;                 // "" for delimited; else the adapter name
};
inline const std::vector<Preset>& presets() {
    static const std::vector<Preset> P = {
        {"Generic", "column names read from the file; choose the key", "", "", "", "", "", Format::Auto, {}, {}, "Tables (*.tsv *.csv *.txt);;All files (*)", ""},
        {"Per-text glossary (TSV, key wylie)", "<dataRoot>/library/glossaries/<base>.tsv — headerless, # comments", "wylie", "", "", "", "", Format::Tsv, {"wylie", "gloss"}, {}, "Glossaries (*.tsv)", ""},
        {"Release dictionary TSV (key wylie)", "data/hgm_dictionary_v27_2.tsv", "wylie", "", "wylie_variants; hgm_gloss_alternatives", "The release TSV does not carry tibetan_source, wylie_variants or the corpus-occurrence detail, and hgm_evidence is reduced to a tier string. This comparison is lossy — compare the JSON master for a provenance-faithful answer.", "", Format::Tsv, {}, {"hgm_evidence", "tier", "source_wave"}, "Release TSV (*.tsv)", ""},
        {"Alignment evidence JSON (key headword+eng)", "data/alignment/*evidence*.json", "headword", "eng", "", "", "TENTATIVE (machine-matched, unreviewed). This compares an evidence layer. Nothing here is a change to an HGM gloss.", Format::Auto, {}, {"tier", "source"}, "Alignment JSON (*.json)", "alignment-evidence"},
        {"Alignment links JSON (key course+page+id)", "data/alignment/*links*.json", "course", "page_id", "", "", "TENTATIVE (machine-matched, unreviewed). This compares an evidence layer. Nothing here is a change to an HGM gloss.", Format::Auto, {}, {"tier"}, "Alignment JSON (*.json)", "alignment-links"},
        {"Reverse index JSON (key english)", "hgm_reverse_index_v27_2.json", "english", "", "entries", "", "", Format::Auto, {}, {}, "Reverse index (*.json)", "reverse-index"},
        {"Proposals TSV (key id)", "<proposalsDir>/proposals.tsv — headerless, # comment header", "id", "", "", "", "", Format::Tsv, {"id", "kind", "status", "proposer", "created", "wylie", "value", "field", "evidence", "approver", "ruled", "comment"}, {"proposer", "created", "approver", "ruled"}, "Proposals (*.tsv)", ""},
        {"Build manifest JSON (key path)", "dist/stage/BUILD_MANIFEST.json", "path", "", "", "", "", Format::Auto, {}, {"sha256"}, "Manifest (*.json)", "manifest"},
        {"Folder snapshot JSON (key path)", "*.snapshot.json", "path", "", "", "", "", Format::Auto, {}, {"sha1", "size"}, "Snapshot (*.json)", "snapshot"},
        {"Registrar CSV (key = detected number column)", "any registrar CSV/TSV", "", "", "", "", "", Format::Auto, {}, {}, "Registrar (*.csv *.tsv)", ""},
        {"Worksheet CSV (key ACIP Number)", "worksheet export", "ACIP Number", "", "", "", "", Format::Csv, {}, {}, "Worksheet (*.csv)", ""},
    };
    return P;
}

// One side of a comparison, with its identity (rule 4: both sides are named).
struct Side {
    QString path; qint64 bytes = 0; QString sha256, mtime, identity, note;
    Table table; bool ok = false; QString error;
};
inline QString sha256Of(const QByteArray& b) { return QString::fromLatin1(QCryptographicHash::hash(b, QCryptographicHash::Sha256).toHex()); }

// ---- JSON adapters (app-side; three known shapes + two generic list shapes)
inline Table tableFromRows(const QStringList& columns, const QList<QStringList>& rows) {
    Table t; for (const QString& c : columns) t.columns.push_back(c.toStdString());
    int ln = 1; for (const QStringList& r : rows) { allcore::table::Row row; row.sourceLine = ln++; for (const QString& c : r) row.cells.push_back(c.toStdString()); t.rows.push_back(row); }
    t.headerFromFile = false; t.detectionNote = "column names come from the JSON adapter, not from a header"; return t;
}
inline QString jsonScalar(const QJsonValue& v) {
    if (v.isString()) return v.toString(); if (v.isDouble()) return QString::number(v.toDouble(), 'g', 15); if (v.isBool()) return v.toBool() ? "true" : "false";
    if (v.isArray()) { QStringList parts; for (const auto& x : v.toArray()) parts << jsonScalar(x); return parts.join("; "); }
    if (v.isObject()) return QString::fromUtf8(QJsonDocument(v.toObject()).toJson(QJsonDocument::Compact));
    return {};
}
// A JSON array of flat objects (or an object whose values are objects) → a table whose columns are the union of keys, in first-seen order.
inline Table tableFromObjectList(const QJsonDocument& d, const QString& arrayKey, const QString& idColumn, QString* identity) {
    QJsonArray arr; QJsonObject meta;
    if (d.isArray()) arr = d.array();
    else { const QJsonObject o = d.object(); meta = o.value("meta").toObject(); if (!arrayKey.isEmpty() && o.value(arrayKey).isArray()) arr = o.value(arrayKey).toArray(); else if (o.value(arrayKey).isObject()) { for (auto it = o.value(arrayKey).toObject().begin(); it != o.value(arrayKey).toObject().end(); ++it) { QJsonObject row = it.value().toObject(); row.insert(idColumn, it.key()); arr.push_back(row); } } else { for (const QString& k : {"entries", "items", "rows", "files", "links", "evidence", "records"}) if (o.value(k).isArray()) { arr = o.value(k).toArray(); break; } } }
    QStringList cols; QList<QStringList> rows;
    for (const auto& x : arr) { const QJsonObject r = x.toObject(); for (auto it = r.begin(); it != r.end(); ++it) if (!cols.contains(it.key())) cols << it.key(); }
    for (const auto& x : arr) { const QJsonObject r = x.toObject(); QStringList cells; for (const QString& c : cols) cells << jsonScalar(r.value(c)); rows << cells; }
    if (identity) { QStringList id; for (const QString& k : {"version", "date", "tier", "generated", "built", "release_version"}) if (meta.contains(k)) id << k + "=" + jsonScalar(meta.value(k)); *identity = id.join(" · "); }
    return tableFromRows(cols, rows);
}
inline Side readSide(const QString& path, const Preset& pr, allcore::Spine* spine) {
    Side S; S.path = path;
    QFile f(path); if (!f.open(QIODevice::ReadOnly)) { S.error = "Could not read " + path; return S; }
    const QByteArray raw = f.readAll(); S.bytes = raw.size(); S.sha256 = sha256Of(raw); S.mtime = QFileInfo(path).lastModified().toString(Qt::ISODate);
    if (raw.left(8192).contains('\0')) { S.error = QFileInfo(path).fileName() + " is binary."; return S; }
    if (path.endsWith(".json", Qt::CaseInsensitive) || !pr.jsonShape.isEmpty()) {
        QJsonParseError pe; const QJsonDocument d = QJsonDocument::fromJson(raw, &pe);
        if (pe.error != QJsonParseError::NoError) { S.error = QString("Not valid JSON: %1 at offset %2").arg(pe.errorString()).arg(pe.offset); return S; }
        QString ident;
        if (pr.jsonShape == "alignment-evidence") S.table = tableFromObjectList(d, "evidence", "headword", &ident);
        else if (pr.jsonShape == "alignment-links") S.table = tableFromObjectList(d, "links", "id", &ident);
        else if (pr.jsonShape == "reverse-index") { const QJsonObject o = d.object(); QJsonObject body = o.value("index").isObject() ? o.value("index").toObject() : o; QStringList cols{"english", "entries"}; QList<QStringList> rows; for (auto it = body.begin(); it != body.end(); ++it) if (it.key() != "meta") rows << QStringList{it.key(), jsonScalar(it.value())}; S.table = tableFromRows(cols, rows); ident = jsonScalar(o.value("meta").toObject().value("version")); }
        else if (pr.jsonShape == "manifest") S.table = tableFromObjectList(d, "files", "path", &ident);
        else if (pr.jsonShape == "snapshot") S.table = tableFromObjectList(d, "files", "path", &ident);
        else S.table = tableFromObjectList(d, "", "key", &ident);
        S.identity = ident.isEmpty() ? "no version recorded in the file" : ident;
        if (!pr.bannerTier.isEmpty()) { const QString tier = d.object().value("meta").toObject().value("tier").toString(); if (!tier.isEmpty()) S.note = tier; }
    } else {
        std::vector<std::string> implied; for (const QString& c : pr.impliedColumns) implied.push_back(c.toStdString());
        S.table = allcore::table::readDelimited(QString::fromUtf8(raw).toStdString(), pr.format, implied);
        S.identity = "no version recorded in the file";
        if (spine && pr.name.startsWith("Release dictionary")) S.identity = QString("spine release_version %1 · source_master_sha256 %2").arg(QString::fromStdString(spine->metaValue("release_version")), QString::fromStdString(spine->metaValue("source_master_sha256")).left(12));
    }
    S.ok = true; return S;
}
inline QString identityLine(const Side& S) {
    return QString("%1 · %L2 bytes · sha256 %3 · modified %4 · %5").arg(S.path).arg(S.bytes).arg(S.sha256.left(12), S.mtime, S.identity);
}

// ---- the page
class TableComparePage : public QWidget {
public:
    TableComparePage(ComparePane* owner, const QString& root, allcore::Spine* spine) : QWidget(owner), owner_(owner), root_(root), spine_(spine) {
        auto* v = new QVBoxLayout(this); v->setContentsMargins(6, 4, 6, 4); v->setSpacing(4);
        auto* banner = new QLabel(ux::sourceBadge(ux::Epistemic::Machine) + "Table Compare pairs records by a key you choose and lists the fields that differ. Nothing is written. This page compares; it does not merge."); banner->setTextFormat(Qt::RichText); banner->setWordWrap(true); banner->setStyleSheet("color:#4A4036;"); v->addWidget(banner);
        auto* top = new QGridLayout;
        leftEd_ = new QLineEdit; rightEd_ = new QLineEdit; leftEd_->setPlaceholderText("Left table (TSV, CSV or JSON)"); rightEd_->setPlaceholderText("Right table");
        auto* bl = new QPushButton("Browse…"); auto* br = new QPushButton("Browse…");
        connect(bl, &QPushButton::clicked, this, [this] { const QString f = safeGetOpenFileName(this, "Left table", QString(), preset().filter); if (!f.isEmpty()) { leftEd_->setText(f); autoPreset(f); } });
        connect(br, &QPushButton::clicked, this, [this] { const QString f = safeGetOpenFileName(this, "Right table", QString(), preset().filter); if (!f.isEmpty()) rightEd_->setText(f); });
        top->addWidget(new QLabel("Left:"), 0, 0); top->addWidget(leftEd_, 0, 1); top->addWidget(bl, 0, 2);
        top->addWidget(new QLabel("Right:"), 1, 0); top->addWidget(rightEd_, 1, 1); top->addWidget(br, 1, 2);
        v->addLayout(top);
        auto* row2 = new QHBoxLayout;
        presetBox_ = new QComboBox; for (const auto& p : presets()) presetBox_->addItem(p.name); presetBox_->setToolTip("Fills format, implied column names, key, list columns, pinned provenance columns and the banner");
        formatBox_ = new QComboBox; formatBox_->addItems({"Detect", "Comma-separated", "Tab-separated"});
        keyBox_ = new QComboBox; keyBox_->setEditable(true); keyBox_->setMinimumWidth(140); key2Box_ = new QComboBox; key2Box_->setEditable(true); key2Box_->setMinimumWidth(120); key2Box_->setToolTip("A second key column: composite keys rescue duplicated single keys");
        listEd_ = new QLineEdit; listEd_->setPlaceholderText("list columns, e.g. wylie_variants; hgm_gloss_alternatives"); listEd_->setToolTip("Treat as lists (split on \"; \"): a reordered list is Same, an added item is Changed with the item named");
        row2->addWidget(new QLabel("Preset:")); row2->addWidget(presetBox_); row2->addWidget(new QLabel("Format:")); row2->addWidget(formatBox_); row2->addWidget(new QLabel("Key column:")); row2->addWidget(keyBox_); row2->addWidget(new QLabel("second:")); row2->addWidget(key2Box_); row2->addStretch(1);
        v->addLayout(row2);
        auto* row3 = new QHBoxLayout;
        colsBtn_ = new QPushButton("Compare columns…"); colsBtn_->setToolTip("Tick the shared columns to compare; provenance columns carry a lock and cannot be unticked");
        connect(colsBtn_, &QPushButton::clicked, this, [this] { colsList_->setVisible(!colsList_->isVisible()); });
        row3->addWidget(colsBtn_); row3->addWidget(new QLabel("Treat as lists:")); row3->addWidget(listEd_, 1);
        auto* run = new QPushButton("Compare"); connect(run, &QPushButton::clicked, this, [this] { compare(); });
        auto* swap = new QPushButton("Swap sides"); connect(swap, &QPushButton::clicked, this, [this] { const QString l = leftEd_->text(); leftEd_->setText(rightEd_->text()); rightEd_->setText(l); if (res_) compare(); });
        row3->addWidget(run); row3->addWidget(swap);
        row3->addWidget(new QLabel("Show:")); showBox_ = new QComboBox; showBox_->addItems({"All", "Changed", "Left only", "Right only", "Ambiguous keys", "Identical"}); connect(showBox_, qOverload<int>(&QComboBox::currentIndexChanged), this, [this](int) { render(); }); row3->addWidget(showBox_);
        v->addLayout(row3);
        colsList_ = new QListWidget; colsList_->setMaximumHeight(110); colsList_->setVisible(false); v->addWidget(colsList_);
        connect(presetBox_, qOverload<int>(&QComboBox::currentIndexChanged), this, [this](int) { applyPreset(); });
        banners_ = new QLabel; banners_->setWordWrap(true); banners_->setTextFormat(Qt::RichText); banners_->setStyleSheet("color:#9A2B1E;"); banners_->setVisible(false); v->addWidget(banners_);
        ident_ = new QLabel; ident_->setWordWrap(true); ident_->setStyleSheet("color:#6B5E4E; font-size:11px;"); ident_->setTextInteractionFlags(Qt::TextSelectableByMouse); v->addWidget(ident_);
        tree_ = new QTreeWidget; tree_->setHeaderLabels({"Status", "Key", "Column", "Left", "Right"}); tree_->header()->setSectionResizeMode(QHeaderView::ResizeToContents); tree_->header()->setStretchLastSection(true); tree_->setRootIsDecorated(true); tree_->setContextMenuPolicy(Qt::CustomContextMenu); v->addWidget(tree_, 1);
        connect(tree_, &QTreeWidget::customContextMenuRequested, this, [this](const QPoint& p) {
            auto* it = tree_->itemAt(p); if (!it) return; QTreeWidgetItem* top = it->parent() ? it->parent() : it; const QString key = top->text(1);
            QMenu m; m.addAction("Open in Lookup", [key] { if (g_lookupQuery) g_lookupQuery(key); });
            m.addAction("Compare these two rows as text", [this, top] { const int i = top->data(0, Qt::UserRole).toInt(); if (!res_ || i < 0 || i >= (int)res_->rows.size()) return; const auto& r = res_->rows[i]; if (g_compareTexts) g_compareTexts(QFileInfo(leftEd_->text()).fileName() + " · " + QString::fromStdString(r.key), rowText(left_.table, r.leftRow), QFileInfo(rightEd_->text()).fileName() + " · " + QString::fromStdString(r.key), rowText(right_.table, r.rightRow)); owner_->showPage(0); });
            m.exec(tree_->viewport()->mapToGlobal(p));
        });
        summary_ = new QLabel; summary_->setWordWrap(true); summary_->setTextFormat(Qt::RichText); v->addWidget(summary_);
        auto* rep = new QHBoxLayout;
        auto btn = [&](const QString& l, const QString& tip, std::function<void()> fn) { auto* b = new QPushButton(l); b->setToolTip(tip); connect(b, &QPushButton::clicked, this, std::move(fn)); rep->addWidget(b); return b; };
        btn("Save report…", "CSV (status,key,column,left,right), Markdown or HTML — routed by the filter you pick", [this] { saveReport(); });
        btn("Copy summary", "The summary line and the ambiguous-key list to the clipboard", [this] { QApplication::clipboard()->setText(summaryText()); status("Summary copied."); });
        btn("Save session…", "Both paths, preset, key and show mode under library/compare_sessions (mode: table)", [this] { saveSession(); });
        btn("Open session…", "Reopen a saved table comparison", [this] { openSession(); });
        rep->addStretch(1); v->addLayout(rep);
        applyPreset();
    }
    void focusLeft() { leftEd_->setFocus(); }
    const Preset& preset() const { return presets()[std::max(0, presetBox_->currentIndex())]; }
    void setPreset(const QString& name) { for (int i = 0; i < presetBox_->count(); ++i) if (presetBox_->itemText(i) == name) { presetBox_->setCurrentIndex(i); return; } }
    void setPaths(const QString& l, const QString& r) { leftEd_->setText(l); rightEd_->setText(r); }
    void setKey(const QString& k, const QString& k2 = QString()) { keyBox_->setCurrentText(k); key2Box_->setCurrentText(k2); }
    const std::optional<allcore::tablediff::DiffResult>& result() const { return res_; }
    QString summaryText() const { return summary_->text().isEmpty() ? QString() : QTextDocumentFragment::fromHtml(summary_->text()).toPlainText(); }
    QString bannerText() const { return QTextDocumentFragment::fromHtml(banners_->text()).toPlainText(); }
    int visibleTopRows() const { int n = 0; for (int i = 0; i < tree_->topLevelItemCount(); ++i) n += tree_->topLevelItem(i)->isHidden() ? 0 : 1; return n; }
    bool columnItemEnabled(const QString& col) const { for (int i = 0; i < colsList_->count(); ++i) if (colsList_->item(i)->data(Qt::UserRole).toString() == col) return colsList_->item(i)->flags() & Qt::ItemIsEnabled; return true; }
    QString showMode() const { return showBox_->currentText(); }
    void setShowMode(const QString& m) { showBox_->setCurrentText(m); }

    // ---- the comparison (the engine does the join; this reads, adapts and renders)
    bool compare() {
        res_.reset(); tree_->clear(); banners_->clear(); banners_->setVisible(false);
        const Preset& pr = preset();
        left_ = readSide(leftEd_->text().trimmed(), pr, spine_); right_ = readSide(rightEd_->text().trimmed(), pr, spine_);
        if (!left_.ok || !right_.ok) { status((left_.ok ? right_.error : left_.error) + " — nothing compared."); ident_->setText(""); return false; }
        ident_->setText("Left: " + identityLine(left_).toHtmlEscaped() + "<br>Right: " + identityLine(right_).toHtmlEscaped()); ident_->setTextFormat(Qt::RichText);
        refreshColumnList();
        allcore::tablediff::DiffOptions o;
        const QString k1 = keyBox_->currentText().trimmed(), k2 = key2Box_->currentText().trimmed();
        if (!k1.isEmpty()) o.keyColumns.push_back(k1.toStdString()); if (!k2.isEmpty()) o.keyColumns.push_back(k2.toStdString());
        for (int i = 0; i < colsList_->count(); ++i) if (colsList_->item(i)->checkState() == Qt::Checked) o.compareColumns.push_back(colsList_->item(i)->data(Qt::UserRole).toString().toStdString());
        for (const QString& c : listEd_->text().split(';', Qt::SkipEmptyParts)) o.listColumns.push_back(c.trimmed().toStdString());
        for (const QString& c : pr.pinned) o.pinnedColumns.push_back(c.toStdString());
        o.cellRules = owner_->options();   // the pane's RULES ribbon — no second rule set
        // keys that are ordinals are refused up front (entries.id is per-release; COURSE:seq is positional)
        for (const QString& k : {k1, k2}) if (k == "id" && pr.name.startsWith("Release") ) { status("The spine's entries.id is a per-release ordinal and is not offered as a key; use wylie (with tibetan as a second key)."); return false; }
        if (k1 == "seq" || k2 == "seq") { status("COURSE:seq is positional and is renumbered by any corpus insertion; use a content key."); return false; }
        res_ = allcore::tablediff::diffTables(left_.table, right_.table, o);
        QStringList bans;
        if (!pr.bannerLossy.isEmpty()) bans << pr.bannerLossy;
        if (!pr.bannerTier.isEmpty()) bans << (left_.note.isEmpty() && right_.note.isEmpty() ? pr.bannerTier : (left_.note.isEmpty() ? right_.note : left_.note) + ". This compares an evidence layer. Nothing here is a change to an HGM gloss.");
        if (!left_.table.headerFromFile || !right_.table.headerFromFile) bans << "Column names were not read from the file; they come from the preset or the JSON adapter.";
        if (!res_->leftOnlyColumns.empty() || !res_->rightOnlyColumns.empty()) bans << QString("Columns present on only one side are never reported as changed-to-empty. Only on the left: %1. Only on the right: %2.").arg(join(res_->leftOnlyColumns), join(res_->rightOnlyColumns));
        const int refused = left_.table.refusedRows + right_.table.refusedRows;
        if (refused) bans << QString("%1 row(s) had a cell count that does not match the header. They were refused and are listed with their line numbers below — nothing was padded silently.").arg(refused);
        if (!res_->refusals.empty()) bans << QString::fromStdString(res_->refusals.front());
        {   // the binding-gloss census made us look at this: a compared column can BE the English
            bool glossy = false;
            for (const auto& c : res_->columns) if (QString::fromStdString(c).contains("gloss", Qt::CaseInsensitive)) glossy = true;
            if (glossy) bans << "One of the columns compared holds Geshe Michael's English. These are the two files' own cells, shown as they stand, with the tier column pinned beside them so a tier can never be dropped from a reading. This page compares data; it neither composes English nor promotes anything into his.";
        }
        if (res_->ambiguous) bans << "Rows whose key occurs more than once are listed as AMBIGUOUS KEY and are never paired by position — pairing them would be a guess. Choose a second key column, or resolve them in the source.";
        banners_->setText(bans.join("<br>").toHtmlEscaped().replace("&lt;br&gt;", "<br>")); banners_->setVisible(!bans.isEmpty());
        render();
        return res_->refusals.empty();
    }
    QString report(const QString& kind) const {
        if (!res_) return {};
        using namespace allcore::tablediff;
        if (kind.contains("CSV")) return QString::fromStdString(diffCsv(*res_));
        const QString md = QString::fromStdString(diffMarkdown(*res_, leftEd_->text().toStdString(), rightEd_->text().toStdString()));
        const QString head = "Left: " + identityLine(left_) + "\nRight: " + identityLine(right_) + "\nRules in force: " + QString::fromStdString(allcore::textdiff::optionsDescription(owner_->options())) + "\n" + bannerText() + "\n\n";
        if (kind.contains("HTML")) { QTextDocument d; d.setMarkdown(head + md); return d.toHtml(); }
        return head + md;
    }
    bool saveReport() {
        if (!res_) { status("Compare first — there is no report yet."); return false; }
        QString sel;
        const QString fn = safeGetSaveFileName(this, "Save table comparison report", QFileInfo(leftEd_->text()).path() + "/table-compare.csv", "CSV (*.csv);;Markdown (*.md);;HTML (*.html)", &sel);
        if (fn.isEmpty()) { status("Report not saved."); return false; }
        if (!saveOrWarn(this, fn, report(sel.isEmpty() ? "CSV" : sel).toUtf8(), "the table report")) return false;
        status("Report saved: " + fn); return true;
    }
    QJsonObject sessionJson() const { QJsonObject j; j["mode"] = "table"; j["left"] = leftEd_->text(); j["right"] = rightEd_->text(); j["preset"] = preset().name; j["key"] = QJsonArray{keyBox_->currentText(), key2Box_->currentText()}; j["show"] = showBox_->currentText(); j["listColumns"] = listEd_->text(); j["options"] = cmp::optionsJson(owner_->options()); return j; }
    bool saveSessionAs(const QString& name) {
        QDir().mkpath(owner_->sessionsDirPublic()); QString safe = name; safe.replace(QRegularExpression("[^A-Za-z0-9_.-]+"), "-");
        return saveOrWarn(this, owner_->sessionsDirPublic() + "/" + safe + ".json", QJsonDocument(sessionJson()).toJson(), "the table session");
    }
    bool openSessionFile(const QString& path) {
        QFile f(path); if (!f.open(QIODevice::ReadOnly)) return false;
        const QJsonObject j = QJsonDocument::fromJson(f.readAll()).object(); if (j["mode"].toString() != "table") { status("That session is not a table comparison."); return false; }
        setPreset(j["preset"].toString()); leftEd_->setText(j["left"].toString()); rightEd_->setText(j["right"].toString()); listEd_->setText(j["listColumns"].toString());
        const QJsonArray k = j["key"].toArray(); setKey(k.size() > 0 ? k[0].toString() : QString(), k.size() > 1 ? k[1].toString() : QString());
        const bool ok = compare(); showBox_->setCurrentText(j["show"].toString()); return ok;
    }
    void saveSession() { if (!res_) { status("Compare first."); return; } const QString n = docprops::askName(this, "Save Table Session", "Session name:", QFileInfo(leftEd_->text()).completeBaseName() + " vs " + QFileInfo(rightEd_->text()).completeBaseName()); if (!n.isEmpty() && saveSessionAs(n)) status("Session saved: " + n); }
    void openSession() { const QString f = safeGetOpenFileName(this, "Open a table session", owner_->sessionsDirPublic(), "Sessions (*.json)"); if (!f.isEmpty() && !openSessionFile(f)) status("Could not open that session."); }

    // ---- selftests 17–24 (fixture-free: temp TSVs; the JSON adapter over a stub)
    int selfTest(QStringList& log) {
        int fails = 0; auto check = [&](bool ok, const char* what) { log << QString("  [%1] TableCompare: %2").arg(ok ? "PASS" : "FAIL").arg(what); if (!ok) ++fails; };
        const QString td = QDir::tempPath() + "/dct_f7_" + QString::number(QCoreApplication::applicationPid()); QDir(td).removeRecursively(); QDir().mkpath(td);
        auto w = [&](const QString& rel, const QString& t) { QFile f(td + "/" + rel); if (f.open(QIODevice::WriteOnly)) f.write(t.toUtf8()); };
        w("a.tsv", "wylie\tgloss\thgm_evidence\nsems\tmind\tcurated\nchos\tdharma\tglossary\nrgyal\tking\tcurated\nrgyal\tvictor\tcurated\nsgra\tsound\tcurated\n");
        w("b.tsv", "wylie\tgloss\thgm_evidence\nsems\tmind\tcurated\nchos\tDharma\tglossary\nrgyal\tking\tcurated\nsnying\theart\tcurated\n");
        const QDateTime ma = QFileInfo(td + "/a.tsv").lastModified(), mb = QFileInfo(td + "/b.tsv").lastModified();
        setPreset("Generic"); setPaths(td + "/a.tsv", td + "/b.tsv"); setKey("wylie");
        const bool ok = compare();
        check(ok && res_ && res_->changed == 1 && res_->ambiguous == 1 && res_->rightOnly == 1 && res_->leftOnly == 1 && res_->same == 1, ("F7-17 visible counts match the engine (1 changed, 1 ambiguous, 1 left-only, 1 right-only, 1 same) [" + summaryText() + "]").toUtf8().constData());
        check(summaryText().contains("never paired") || bannerText().contains("never paired"), "F7-17b the summary says ambiguous keys are never paired");
        setShowMode("Changed"); check(visibleTopRows() == 1, "F7-21 Show ▸ Changed leaves exactly the changed rows visible"); setShowMode("All");
        check(saveSessionAs("probe-table") && QFile::exists(owner_->sessionsDirPublic() + "/probe-table.json"), "F7-21b a table session is written under library/compare_sessions with mode table");
        setKey(""); setPaths("", ""); res_.reset();
        check(openSessionFile(owner_->sessionsDirPublic() + "/probe-table.json") && keyBox_->currentText() == "wylie" && res_ && res_->changed == 1, "F7-21c the session round-trips preset, key and show mode");
        QFile::remove(owner_->sessionsDirPublic() + "/probe-table.json");
        check(QFileInfo(td + "/a.tsv").lastModified() == ma && QFileInfo(td + "/b.tsv").lastModified() == mb, "F7-22 no input file is written by a compare");
        check(!saveReport() && summary_->text().contains("not saved"), "F7-23 Save report… returns false with a status line under the harness, no modal");
        {   // presets
            setPreset("Per-text glossary (TSV, key wylie)"); check(keyBox_->currentText() == "wylie" && preset().impliedColumns == QStringList{"wylie", "gloss"}, "F7-18 the glossary preset selects the key and names the implied columns");
            setPreset("Release dictionary TSV (key wylie)"); setPaths(td + "/a.tsv", td + "/b.tsv"); compare();
            check(keyBox_->currentText() == "wylie" && bannerText().contains("lossy"), "F7-18b the release preset keys on wylie and shows the lossy banner");
            check(!columnItemEnabled("hgm_evidence"), "F7-20 a provenance column's picker item is disabled (locked)");
            w("ev.json", "{\"meta\":{\"tier\":\"TENTATIVE (machine-matched, unreviewed)\",\"version\":\"v1\"},\"evidence\":[{\"headword\":\"sems\",\"eng\":\"mind\",\"tier\":\"tentative\",\"source\":\"DWTC\"},{\"headword\":\"chos\",\"eng\":\"dharma\",\"tier\":\"tentative\",\"source\":\"DWTC\"}]}");
            setPreset("Alignment evidence JSON (key headword+eng)"); setPaths(td + "/ev.json", td + "/ev.json"); compare();
            check(left_.ok && left_.table.rows.size() == 2 && left_.table.columns.size() == 4 && bannerText().contains("TENTATIVE (machine-matched, unreviewed)"), "F7-18c/19 the alignment adapter yields 2 rows and 4 columns and renders the file's own TENTATIVE tier");
        }
        {   // CLI
            const int rc0 = cli({"--table-compare", td + "/b.tsv", td + "/b.tsv", "wylie"}, 0);   // b has no duplicated key, so identity is certifiable
            const int rc1 = cli({"--table-compare", td + "/a.tsv", td + "/b.tsv", "wylie", td + "/r.csv"}, 0);
            QFile rf(td + "/r.csv"); const QString first = rf.open(QIODevice::ReadOnly) ? QString::fromUtf8(rf.readLine()).trimmed() : QString();
            const int rc2 = cli({"--table-compare", td + "/a.tsv", td + "/missing.tsv", "wylie"}, 0);
            const int rcNoKey = cli({"--table-compare", td + "/a.tsv", td + "/b.tsv"}, 0);
            const int rcDup = cli({"--table-compare", td + "/a.tsv", td + "/a.tsv", "wylie"}, 0);   // a duplicated key cannot be certified identical
            check(rc0 == 0 && rc1 == 1 && first == "status,key,column,left,right" && rc2 == 2 && rcNoKey == 2 && rcDup == 1, ("F7-24 the CLI exits 0 identical / 1 different / 2 unreadable or no key, writes the CSV header, and refuses to call a file with a duplicated key identical to itself [" + QString::number(rc0) + QString::number(rc1) + QString::number(rc2) + QString::number(rcNoKey) + QString::number(rcDup) + "]").toUtf8().constData());
        }
        setPreset("Generic"); setPaths("", ""); res_.reset(); tree_->clear(); banners_->setVisible(false); summary_->clear(); ident_->clear();
        QDir(td).removeRecursively();
        return fails;
    }

    // CLI: --table-compare A B <key> [report.csv]; without a key: exit 2 and the column names on stdout.
    static int cli(const QStringList& args, int ix) {
        if (ix + 2 >= args.size()) { fprintf(stderr, "table-compare: usage --table-compare A B <key> [report.csv]\n"); return 2; }
        const Preset& pr = presets()[0];
        Side A = readSide(args[ix + 1], pr, nullptr), B = readSide(args[ix + 2], pr, nullptr);
        if (!A.ok || !B.ok) { fprintf(stderr, "table-compare: %s\n", (A.ok ? B.error : A.error).toUtf8().constData()); return 2; }
        if (ix + 3 >= args.size()) { printf("table-compare: no key column given. Columns on the left: %s. Columns on the right: %s.\n", join(A.table.columns).toUtf8().constData(), join(B.table.columns).toUtf8().constData()); return 2; }
        allcore::tablediff::DiffOptions o; o.keyColumns.push_back(args[ix + 3].toStdString());
        const auto r = allcore::tablediff::diffTables(A.table, B.table, o);
        if (!r.refusals.empty()) { printf("table-compare: %s\n", r.refusals.front().c_str()); return 2; }
        printf("%s\n", summaryFor(r).toUtf8().constData());
        if (ix + 4 < args.size()) { QFile f(args[ix + 4]); const std::string body = allcore::tablediff::diffCsv(r); if (!f.open(QIODevice::WriteOnly) || f.write(body.data(), (qint64)body.size()) != (qint64)body.size()) { fprintf(stderr, "table-compare: could not write %s\n", args[ix + 4].toUtf8().constData()); return 2; } }
        return (r.changed + r.leftOnly + r.rightOnly + r.ambiguous) ? 1 : 0;
    }
    static QString summaryFor(const allcore::tablediff::DiffResult& r) {
        return QString("%L1 rows left · %L2 right · %3 only on the right · %4 only on the left · %5 changed · %6 identical · %7 keys ambiguous (never paired by position). Rows read %L8, compared %L9, refused %10.")
            .arg(r.leftRows).arg(r.rightRows).arg(r.rightOnly).arg(r.leftOnly).arg(r.changed).arg(r.same).arg(r.ambiguous).arg(r.leftRows + r.rightRows).arg(r.same + r.changed).arg((int)(r.leftProv.units_refused + r.rightProv.units_refused));
    }

private:
    static QString join(const std::vector<std::string>& v) { QStringList l; for (const auto& s : v) l << QString::fromStdString(s); return l.isEmpty() ? QString("none") : l.join(", "); }
    static QString rowText(const Table& t, int row) { if (row < 0 || row >= (int)t.rows.size()) return {}; QStringList l; for (size_t i = 0; i < t.columns.size() && i < t.rows[row].cells.size(); ++i) l << QString::fromStdString(t.columns[i]) + ": " + QString::fromStdString(t.rows[row].cells[i]); return l.join('\n') + "\n"; }
    void status(const QString& m) { summary_->setText(ux::sourceBadge(ux::Epistemic::Machine) + m.toHtmlEscaped()); owner_->statusLabel()->setText(m); }
    void applyPreset() {
        const Preset& pr = preset(); keyBox_->setCurrentText(pr.keyCol); key2Box_->setCurrentText(pr.key2Col); listEd_->setText(pr.listCols);
        formatBox_->setCurrentIndex(pr.format == Format::Csv ? 1 : pr.format == Format::Tsv ? 2 : 0);
        banners_->setText(pr.bannerLossy.toHtmlEscaped()); banners_->setVisible(!pr.bannerLossy.isEmpty());
        if (pr.name.startsWith("Registrar")) status("Registrar preset: the key is the detected number column (normalised with normalizeCatalogKey) — set it in Key column after the first compare lists the columns.");
    }
    void autoPreset(const QString& path) {
        const QString b = QFileInfo(path).fileName().toLower();
        if (b.contains("hgm_dictionary") && b.endsWith(".tsv")) setPreset("Release dictionary TSV (key wylie)");
        else if (path.contains("/library/glossaries/")) setPreset("Per-text glossary (TSV, key wylie)");
        else if (b.contains("evidence") && b.endsWith(".json")) setPreset("Alignment evidence JSON (key headword+eng)");
        else if (b.contains("links") && b.endsWith(".json")) setPreset("Alignment links JSON (key course+page+id)");
        else if (b.contains("reverse_index")) setPreset("Reverse index JSON (key english)");
        else if (b == "build_manifest.json") setPreset("Build manifest JSON (key path)");
        else if (b.endsWith(".snapshot.json")) setPreset("Folder snapshot JSON (key path)");
        else if (b == "proposals.tsv") setPreset("Proposals TSV (key id)");
    }
    void refreshColumnList() {
        QSet<QString> shared; for (const auto& c : left_.table.columns) for (const auto& d : right_.table.columns) if (c == d) shared.insert(QString::fromStdString(c));
        QSet<QString> was; for (int i = 0; i < colsList_->count(); ++i) if (colsList_->item(i)->checkState() == Qt::Unchecked) was.insert(colsList_->item(i)->data(Qt::UserRole).toString());
        colsList_->clear();
        for (const auto& c : left_.table.columns) { const QString col = QString::fromStdString(c); if (!shared.contains(col)) continue; auto* it = new QListWidgetItem(col); it->setData(Qt::UserRole, col); it->setFlags(it->flags() | Qt::ItemIsUserCheckable);
            const bool pinned = preset().pinned.contains(col); if (pinned) { it->setText("🔒 " + col + " (provenance — always compared)"); it->setFlags(it->flags() & ~Qt::ItemIsEnabled); it->setCheckState(Qt::Checked); } else it->setCheckState(was.contains(col) ? Qt::Unchecked : Qt::Checked);
            colsList_->addItem(it); }
    }
    void render() {
        tree_->clear(); if (!res_) return;
        using allcore::tablediff::RowStatus;
        const QString mode = showBox_->currentText();
        auto name = [](RowStatus s) { switch (s) { case RowStatus::Same: return "Identical"; case RowStatus::Changed: return "Changed"; case RowStatus::LeftOnly: return "Left only"; case RowStatus::RightOnly: return "Right only"; case RowStatus::AmbiguousKey: return "AMBIGUOUS KEY"; } return "?"; };
        int shown = 0;
        for (size_t i = 0; i < res_->rows.size(); ++i) {
            const auto& r = res_->rows[i]; const QString st = name(r.status);
            const bool show = mode == "All" || (mode == "Changed" && r.status == RowStatus::Changed) || (mode == "Left only" && r.status == RowStatus::LeftOnly) || (mode == "Right only" && r.status == RowStatus::RightOnly) || (mode == "Ambiguous keys" && r.status == RowStatus::AmbiguousKey) || (mode == "Identical" && r.status == RowStatus::Same);
            if (!show) continue; if (++shown > 5000) break;
            auto* it = new QTreeWidgetItem({st, QString::fromStdString(r.key), r.status == RowStatus::AmbiguousKey ? QString("%1 row(s) left, %2 right — not paired").arg(r.leftDupes).arg(r.rightDupes) : QString("%1 field(s) changed").arg(r.fields.size()), "", ""});
            it->setData(0, Qt::UserRole, (int)i);
            if (r.status == RowStatus::AmbiguousKey) for (int c = 0; c < 5; ++c) it->setForeground(c, QColor(0x9A, 0x2B, 0x1E));
            for (const auto& fd : r.fields) { auto* ch = new QTreeWidgetItem(it, {"", "", QString::fromStdString(fd.column), QString::fromStdString(fd.left), QString::fromStdString(fd.right)}); if (!fd.addedItems.empty() || !fd.removedItems.empty()) ch->setToolTip(2, "added: " + join(fd.addedItems) + " · removed: " + join(fd.removedItems)); }
            tree_->addTopLevelItem(it);
        }
        QString sm = summaryFor(*res_); if (shown > 5000) sm += QString(" Showing the first 5,000 of %1 rows in this view.").arg(res_->rows.size());
        summary_->setText(ux::sourceBadge(ux::Epistemic::Machine) + sm.toHtmlEscaped());
        owner_->statusLabel()->setText(sm);
    }

    ComparePane* owner_; QString root_; allcore::Spine* spine_;
    QLineEdit *leftEd_ = nullptr, *rightEd_ = nullptr, *listEd_ = nullptr; QComboBox *presetBox_ = nullptr, *formatBox_ = nullptr, *keyBox_ = nullptr, *key2Box_ = nullptr, *showBox_ = nullptr;
    QPushButton* colsBtn_ = nullptr; QListWidget* colsList_ = nullptr; QLabel *banners_ = nullptr, *ident_ = nullptr, *summary_ = nullptr; QTreeWidget* tree_ = nullptr;
    Side left_, right_; std::optional<allcore::tablediff::DiffResult> res_;
};

inline TableComparePage* installTableComparePage(ComparePane* p, const QString& root, allcore::Spine* spine) {
    auto* page = new TableComparePage(p, root, spine);
    p->addPage(page);                       // page index 3
    p->addPageButton("Table Compare", 3);
    return page;
}

}  // namespace tablecmp
using tablecmp::TableComparePage;
using tablecmp::installTableComparePage;
