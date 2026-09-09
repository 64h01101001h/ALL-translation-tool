// app/replace_files.inc — F4 Replace in Files… with mandatory preview
// (analysis suite, batch 4, 2026-09-09). One systematic fix across a folder of
// input-centre files: every occurrence listed per file with its line before
// and after, tick/untick, apply only to ticked files, an honest count of what
// changed and what was skipped and why, and an undo of the whole run from the
// pre-replace versions (F1). Normalize mode runs F3's rules instead of a
// find/replace. Included from main.cpp after normalize_pane.inc, so it sees
// cmp::, textdiff, filewalk, versions, editops, docprops, normalize::.
//
// Honesty rules enforced here: counts come from the bytes read here, never
// from the search index; every skip is named (unreadable · binary · not UTF-8
// · over 10 MB · excluded · past the cap · open with unsaved edits · changed
// on disk since the preview · library text without permission · undo copy
// not banked); a script-mismatched zero says so; nothing is written until
// Apply; the preview IS the apply (same function, same text, re-hashed at
// apply); write verdicts come only from saveOrWarn; the undo names what it
// restored and what it skipped.

namespace replf {

struct Spec {
    QString find, with;
    editops::FindOpts opts;
    bool normalizeMode = false;
    normalize::Options norm;
    bool houseStyle = false;
};
struct Scope {
    QStringList folders, files;          // folders to walk, or explicit files (Files pane seed)
    QString include = "*.txt;*.act;*.inc;*.ace;*.acip;*.md", exclude = "*META.TXT;.index.db*";
    bool recurse = true, hidden = false;
    int fileCap = 4000;
};
struct Occ {                       // one match — the tick unit a proofreader reasons about
    int start = 0, len = 0;        // UTF-16 offsets into the file text
    int line = 0;                  // 1-based
    QString cite;                  // "@012B.3" or "line 57"
    QString oldLine, newLine, with;
    bool ticked = true;
};
struct FilePlan {
    QString path, docKey, eol, skipReason;
    bool endsWithEol = false, bom = false, library = false;
    QByteArray preSha1;
    QString text;                  // decoded text (occurrences index into it)
    QStringList before, after;
    QVector<Occ> occs;
    allcore::textdiff::Result diff;   // before → after (all occurrences), for the drill-down
    bool ticked = true;
    int tickedCount() const { int n = 0; for (const auto& o : occs) n += o.ticked ? 1 : 0; return n; }
};
struct Plan {
    QVector<FilePlan> files;       // files WITH matches (or changes), in walk order
    allcore::WalkStats walk;
    int filesRead = 0, skippedBinary = 0, skippedNotUtf8 = 0, skippedUnreadable = 0, withMatches = 0, occurrences = 0;
    bool stoppedEarly = false;
    QString scriptNotice, regexError;
    QStringList skippedNames;      // "path — reason"
    QString key;                   // hash of inputs + per-file sha1s: Apply is valid only for this key
};
struct FileRead { bool ok = false, binary = false, utf8 = true, bom = false, endsWithEol = false; int bad = 0; QByteArray raw; QString text, eol; };
struct ApplyReport {
    int changed = 0, occurrences = 0, writeFailed = 0, skipped = 0, restored = 0;
    QStringList lines;             // human rows; file rows carry the path for anchors
    QString changesetId, indexNote, message;
    bool ok = false;
};

inline QString sha1(const QByteArray& b) { return QString::fromLatin1(QCryptographicHash::hash(b, QCryptographicHash::Sha1).toHex()); }

// The rewrite reader: raw bytes, NUL check, BOM, STRICT UTF-8 (cmp::readText
// is the display reader and cannot report a decode failure).
inline FileRead readForRewrite(const QString& path, qint64 maxBytes = 10LL * 1024 * 1024) {
    FileRead R; QFile f(path);
    if (!f.open(QIODevice::ReadOnly)) return R;
    if (f.size() > maxBytes) { R.ok = false; R.bad = -1; return R; }
    R.raw = f.readAll(); R.ok = true;
    R.binary = R.raw.left(8192).contains('\0');
    if (R.binary) return R;
    QByteArray body = R.raw;
    if (body.startsWith("\xEF\xBB\xBF")) { R.bom = true; body.remove(0, 3); }
    R.text = enc::decode(body, "UTF-8", &R.bad);
    R.utf8 = R.bad == 0;
    R.eol = body.contains("\r\n") ? "\r\n" : body.contains('\r') ? "\r" : "\n";
    R.endsWithEol = body.endsWith('\n') || body.endsWith('\r');
    if (R.eol != "\n") { R.text.replace("\r\n", "\n"); R.text.replace('\r', '\n'); }
    return R;
}
inline QString citeAt(const QStringList& lines, int line0) {
    for (int i = std::min(line0, int(lines.size()) - 1); i >= 0; --i) {
        const std::string last = allcore::textspan::lastFolio(lines[i].toStdString());
        if (!last.empty()) return "@" + QString::fromStdString(last) + "." + QString::number(line0 - i + 1);
    }
    return "line " + QString::number(line0 + 1);
}
// One QRegularExpression pass over the whole text — the same pattern, expansion
// and case shaping the Find bar's Replace All uses, so the two agree exactly.
inline QVector<Occ> planOccurrences(const QString& text, const QString& needle, const QString& with, const editops::FindOpts& o, bool* regexInvalid) {
    QVector<Occ> v; if (regexInvalid) *regexInvalid = false;
    if (needle.isEmpty()) return v;
    const QRegularExpression re = editops::patternFor(needle, o);
    if (!re.isValid()) { if (regexInvalid) *regexInvalid = true; return v; }
    const QStringList lines = text.split('\n');
    QVector<int> lineStart; lineStart.reserve(lines.size()); { int p = 0; for (const auto& l : lines) { lineStart.push_back(p); p += l.size() + 1; } }
    auto lineOf = [&](int pos) { int lo = 0, hi = lineStart.size() - 1; while (lo < hi) { const int mid = (lo + hi + 1) / 2; if (lineStart[mid] <= pos) lo = mid; else hi = mid - 1; } return lo; };
    auto it = re.globalMatch(text);
    while (it.hasNext()) {
        const auto m = it.next();
        Occ oc; oc.start = int(m.capturedStart()); oc.len = int(m.capturedLength());
        QString rep = editops::expandReplacement(m, with, o.regex);
        if (o.preserveCase) rep = editops::shapeCase(m.captured(), rep);
        oc.with = rep;
        const int l0 = lineOf(oc.start); oc.line = l0 + 1; oc.cite = citeAt(lines, l0);
        oc.oldLine = lines[l0];
        const int inLine = oc.start - lineStart[l0];
        oc.newLine = oc.oldLine.left(inLine) + rep + oc.oldLine.mid(inLine + oc.len);
        v.push_back(oc);
    }
    return v;
}
inline QString applyTicked(const QString& text, const QVector<Occ>& occs) {
    QString out = text;
    for (int i = occs.size() - 1; i >= 0; --i) if (occs[i].ticked) out.replace(occs[i].start, occs[i].len, occs[i].with);   // back to front keeps offsets valid
    return out;
}
inline QByteArray bytesForWrite(const QString& text, const QString& eol, bool bom, bool endsWithEol) {
    QString t = text;
    if (endsWithEol && !t.endsWith('\n')) t += '\n';
    if (!endsWithEol && t.endsWith('\n')) t.chop(1);
    if (eol != "\n") t.replace("\n", eol);
    QByteArray b; if (bom) b.append("\xEF\xBB\xBF", 3); b += t.toUtf8(); return b;
}
inline QString dominantScript(const QString& s) {
    int tib = 0, upper = 0, lower = 0;
    for (const QChar c : s) { const ushort u = c.unicode(); if (u >= 0x0F00 && u <= 0x0FFF) ++tib; else if (c.isUpper()) ++upper; else if (c.isLower()) ++lower; }
    if (tib > 0 && tib >= upper + lower) return "Tibetan Unicode";
    if (upper > 0 && lower == 0) return "ACIP";
    if (upper + lower == 0) return "";
    return "Wylie or English";
}

// The plan: scope through the ONE eligibility rule (F0), strict reads, one regex pass per file.
inline Plan buildPlan(const Scope& sc, const Spec& sp, const QString& dataRoot, const std::function<bool(int, int)>& pump = {}) {
    Plan P;
    QStringList paths;
    for (const QString& f : sc.files) if (QFileInfo(f).isFile()) paths << f; else if (QFileInfo(f).isDir()) { allcore::WalkStats w; for (const auto& s : allcore::listEligibleFiles(f.toStdString(), sc.include.toStdString(), sc.exclude.toStdString(), sc.recurse, sc.fileCap, &w)) paths << QString::fromStdString(s); P.walk.scanned += w.scanned; P.walk.capped += w.capped; P.walk.excluded += w.excluded; P.walk.tooLarge += w.tooLarge; P.walk.ineligibleExt += w.ineligibleExt; P.walk.unreadable += w.unreadable; }
    for (const QString& d : sc.folders) { allcore::WalkStats w; for (const auto& s : allcore::listEligibleFiles(d.toStdString(), sc.include.toStdString(), sc.exclude.toStdString(), sc.recurse, sc.fileCap, &w)) paths << QString::fromStdString(s); P.walk.scanned += w.scanned; P.walk.capped += w.capped; P.walk.excluded += w.excluded; P.walk.tooLarge += w.tooLarge; P.walk.ineligibleExt += w.ineligibleExt; P.walk.unreadable += w.unreadable; }
    paths.removeDuplicates();
    if (!sp.normalizeMode) { bool bad = false; editops::patternFor(sp.find, sp.opts); const auto probe = editops::patternFor(sp.find, sp.opts); if (!probe.isValid()) { bad = true; P.regexError = probe.errorString(); } if (bad) return P; }
    QHash<QString, int> fileScripts;
    QCryptographicHash keyHash(QCryptographicHash::Sha1);
    keyHash.addData((sp.find + "\x1f" + sp.with + "\x1f" + QString::number(sp.opts.regex) + QString::number(sp.opts.caseSensitive) + QString::number(sp.opts.wholeWord) + QString::number(sp.opts.preserveCase) + QString::number(sp.normalizeMode) + QJsonDocument(normalize::toJson(sp.norm)).toJson(QJsonDocument::Compact) + QString::number(sp.houseStyle)).toUtf8());
    int done = 0;
    for (const QString& path : paths) {
        if (pump && !pump(done, paths.size())) { P.stoppedEarly = true; break; }
        ++done;
        FileRead R = readForRewrite(path);
        if (!R.ok) { if (R.bad == -1) ++P.walk.tooLarge; else ++P.skippedUnreadable; P.skippedNames << path + (R.bad == -1 ? " — over 10 MB" : " — unreadable"); continue; }
        if (R.binary) { ++P.skippedBinary; P.skippedNames << path + " — binary"; continue; }
        if (!R.utf8) { ++P.skippedNotUtf8; P.skippedNames << path + QString(" — not UTF-8 (%1 byte(s); open it in the Overlay with its encoding and save it as UTF-8 first)").arg(R.bad); continue; }
        ++P.filesRead;
        const QString ds = dominantScript(R.text.left(4000)); if (!ds.isEmpty()) ++fileScripts[ds];
        FilePlan F; F.path = path; F.eol = R.eol; F.bom = R.bom; F.endsWithEol = R.endsWithEol; F.text = R.text;
        F.preSha1 = sha1(R.raw).toLatin1(); F.docKey = dataRoot.isEmpty() ? QFileInfo(path).completeBaseName() : docprops::versionDocKey(path, dataRoot);
        F.library = !dataRoot.isEmpty() && QFileInfo(path).absoluteFilePath().startsWith(QFileInfo(dataRoot).absoluteFilePath() + "/library/");
        F.before = R.text.split('\n'); if (F.before.size() && F.before.last().isEmpty() && R.endsWithEol) F.before.removeLast();
        QString afterText;
        if (sp.normalizeMode) {
            normalize::Target t; t.kind = "file"; t.text = [&R] { return R.text; }; t.hasSelection = [] { return false; }; t.selectionText = [] { return QString(); }; t.apply = [](const QString&, bool) { return false; };
            const auto nr = normalize::run(t, sp.norm, sp.houseStyle, false, false);
            if (!nr.ok) continue;
            afterText = nr.after; afterText.replace("\r\n", "\n"); afterText.replace('\r', '\n');
            // occurrences = changed lines (the tick unit in Normalize mode)
            const QStringList al = afterText.split('\n');
            const auto d = allcore::textdiff::diffLines(cmp::toStd(F.before), cmp::toStd(cmp::linesOf(afterText)), allcore::textdiff::Options());
            for (const auto& h : d.hunks) { if (h.kind == allcore::textdiff::Kind::Equal) continue; Occ oc; oc.line = h.aBeg + 1; oc.cite = citeAt(F.before, std::max(0, h.aBeg)); oc.oldLine = h.aBeg < F.before.size() ? F.before[h.aBeg] : QString(); oc.newLine = h.bBeg < al.size() ? al[h.bBeg] : QString(); oc.with = oc.newLine; F.occs.push_back(oc); }
            F.diff = d;
            if (F.occs.isEmpty() && afterText == R.text) continue;
        } else {
            bool bad = false; F.occs = planOccurrences(R.text, sp.find, sp.with, sp.opts, &bad);
            if (F.occs.isEmpty()) continue;
            afterText = applyTicked(R.text, F.occs);
        }
        F.after = cmp::linesOf(afterText);
        if (!sp.normalizeMode) F.diff = allcore::textdiff::diffLines(cmp::toStd(F.before), cmp::toStd(F.after), allcore::textdiff::Options());
        keyHash.addData(F.path.toUtf8()); keyHash.addData(F.preSha1);
        ++P.withMatches; P.occurrences += F.occs.size();
        P.files.push_back(F);
    }
    P.key = QString::fromLatin1(keyHash.result().toHex());
    if (!sp.normalizeMode && P.occurrences == 0 && !fileScripts.isEmpty()) {
        QString top; int best = 0; for (const QString& k : {QString("Tibetan Unicode"), QString("ACIP"), QString("Wylie or English")}) if (fileScripts.value(k) > best) { best = fileScripts.value(k); top = k; }
        const QString ns = dominantScript(sp.find);
        if (!ns.isEmpty() && !top.isEmpty() && ns != top)
            P.scriptNotice = QString("The search text is %1; the files scanned are %2. A count of 0 here means the two scripts do not match, not that the text is clean. Convert the search text (Format › Tibetan: To %3) or use the Convert pane.").arg(ns, top, top == "ACIP" ? "ACIP" : top == "Tibetan Unicode" ? "Tibetan Script" : "Wylie");
    }
    return P;
}

inline QString footer(const Plan& P, int tickedFiles, int tickedOccs) {
    QStringList parts;
    parts << QString("%L1 files read").arg(P.filesRead) << QString("%1 with matches").arg(P.withMatches) << QString("%1 of %2 occurrences ticked (%3 files)").arg(tickedOccs).arg(P.occurrences).arg(tickedFiles);
    const int sk = P.skippedBinary + P.skippedNotUtf8 + P.skippedUnreadable;
    QString s = QString("%1 skipped").arg(sk); if (sk) s += QString(" (%1 binary, %2 not UTF-8, %3 unreadable)").arg(P.skippedBinary).arg(P.skippedNotUtf8).arg(P.skippedUnreadable);
    parts << s << QString("%1 over 10 MB").arg(P.walk.tooLarge) << QString("%1 past the file cap").arg(P.walk.capped) << QString("stopped early: %1").arg(P.stoppedEarly ? "yes" : "no");
    return parts.join(" · ") + ". Counted in the files' text, not in the search index.";
}

static std::function<QString(const QString&)> g_updateFolderIndex;   // set in main(): rebuilds <folder>/.index.db, returns the indexer's own result line

// Apply — two phases. Verify (re-hash, open-with-edits, library permission,
// confirmation), then per file: bank pre-replace (must be Stored) → write via
// saveOrWarn → reload a clean open document. The changeset record is written
// at the end and is the undo's authority.
inline ApplyReport applyPlan(const QString& dataRoot, Plan& P, const Spec& sp, const Scope& sc, bool allowLibrary, bool confirmed,
                             const std::function<QString(const QString&)>& openDirtyReason, bool updateIndex) {
    ApplyReport R;
    if (!confirmed) { R.message = "Not confirmed — nothing written."; return R; }
    if (dataRoot.isEmpty() || !docprops::versionsEnabled()) { R.message = "Versions are off (Preferences › Versions). Replace in Files needs them for its undo copies — nothing written."; return R; }
    QString id = (sp.normalizeMode ? "nif-" : "rif-") + QDateTime::currentDateTime().toString("yyyyMMdd-HHmmss-zzz");
    { const QString dir = QString::fromStdString(allcore::versions::changesetsDir(docprops::versionsRoot(dataRoot).toStdString())); const QString base = id; for (int n = 2; QFile::exists(dir + "/" + id + ".json"); ++n) id = base + "-" + QString::number(n); }
    allcore::versions::Changeset cs; cs.id = id.toStdString(); cs.kind = sp.normalizeMode ? "normalize-in-files" : "replace-in-files";
    QString src; cs.ranBy = docprops::whoSaves(&src).toStdString(); cs.ranBySource = src.toStdString(); cs.ranAt = QDateTime::currentDateTime().toString(Qt::ISODate).toStdString();
    { QJsonObject spec; spec["mode"] = sp.normalizeMode ? "normalize" : (sp.opts.regex ? "regex" : "text"); spec["find"] = sp.find; spec["replace"] = sp.with; spec["regex"] = sp.opts.regex; spec["caseSensitive"] = sp.opts.caseSensitive; spec["wholeWord"] = sp.opts.wholeWord; spec["preserveCase"] = sp.opts.preserveCase; if (sp.normalizeMode) spec["normalize"] = normalize::toJson(sp.norm); cs.specJson = QJsonDocument(spec).toJson(QJsonDocument::Compact).toStdString(); }
    { QJsonObject scope; scope["folders"] = QJsonArray::fromStringList(sc.folders); scope["files"] = QJsonArray::fromStringList(sc.files); scope["include"] = sc.include; scope["exclude"] = sc.exclude; scope["recurse"] = sc.recurse; scope["hidden"] = sc.hidden; scope["fileCap"] = sc.fileCap; cs.scopeJson = QJsonDocument(scope).toJson(QJsonDocument::Compact).toStdString(); }
    cs.filesRead = P.filesRead; cs.withMatches = P.withMatches;
    QSet<QString> foldersTouched;
    for (auto& F : P.files) {
        allcore::versions::ChangesetFile cf; cf.path = F.path.toStdString(); cf.docKey = F.docKey.toStdString(); cf.preSha1 = F.preSha1.toStdString(); cf.eol = normalize::eolName(F.eol).toStdString(); cf.bom = F.bom;
        cf.occurrences = F.occs.size(); cf.unticked = F.occs.size() - F.tickedCount();
        auto skip = [&](const QString& why) { cf.status = ("skipped:" + why).toStdString(); ++R.skipped; ++cs.skipped; R.lines << F.path + " — skipped: " + why; cs.files.push_back(cf); };
        if (!F.ticked || F.tickedCount() == 0) { skip("not ticked"); continue; }
        if (F.library && !allowLibrary) { skip("canonical library text (allow changes under /library/ first)"); continue; }
        const QString dirty = openDirtyReason ? openDirtyReason(F.path) : QString();
        if (!dirty.isEmpty()) { skip(dirty); continue; }
        FileRead now = readForRewrite(F.path);
        if (!now.ok || now.binary || !now.utf8) { skip("could not re-read the file"); continue; }
        if (sha1(now.raw) != QString::fromLatin1(F.preSha1)) { skip("changed on disk since the preview"); continue; }
        const QString vn = docprops::noteVersion(nullptr, dataRoot, F.path, now.raw, "document", sp.normalizeMode ? "pre-normalize" : "pre-replace", id, "UTF-8", normalize::eolName(F.eol), F.bom, QJsonObject());
        if (!vn.startsWith("Version kept")) { skip("could not bank the undo copy (" + (vn.isEmpty() ? QString("versions off") : vn) + ")"); continue; }
        cf.preStamp = "changeset " + id.toStdString();
        const QString afterText = sp.normalizeMode ? F.after.join('\n') + (F.endsWithEol ? "\n" : "") : applyTicked(F.text, F.occs);
        const QByteArray bytes = bytesForWrite(afterText, F.eol, F.bom, F.endsWithEol);
        if (!saveOrWarn(nullptr, F.path, bytes, "the rewritten file")) { cf.status = "write-failed"; ++R.writeFailed; ++cs.writeFailed; R.lines << F.path + " — NOT WRITTEN"; cs.files.push_back(cf); continue; }
        cf.postSha1 = sha1(bytes).toStdString(); cf.status = "changed";
        ++R.changed; ++cs.changed; R.occurrences += F.tickedCount(); cs.occurrences += F.tickedCount();
        R.lines << F.path + QString(" — changed (%1 occurrence%2)").arg(F.tickedCount()).arg(F.tickedCount() == 1 ? "" : "s");
        foldersTouched.insert(QFileInfo(F.path).path());
        if (normalize::g_fileNormalized) normalize::g_fileNormalized(F.path);
        cs.files.push_back(cf);
    }
    if (!allcore::versions::writeChangeset(docprops::versionsRoot(dataRoot).toStdString(), cs)) R.lines << "The changeset record could not be written — Undo Last Replace in Files will not see this run.";
    R.changesetId = id; R.ok = true;
    R.message = QString("Changed %1 file%2 (%3 occurrence%4) · %5 write failure%6 · %7 skipped").arg(R.changed).arg(R.changed == 1 ? "" : "s").arg(R.occurrences).arg(R.occurrences == 1 ? "" : "s").arg(R.writeFailed).arg(R.writeFailed == 1 ? "" : "s").arg(R.skipped);
    if (R.changed) {
        QStringList notes;
        for (const QString& d : foldersTouched) {
            const QString ix = d + "/.index.db";
            if (!QFileInfo::exists(ix)) continue;
            if (updateIndex && g_updateFolderIndex) notes << g_updateFolderIndex(d);
            else notes << "The search index for " + d + " is out of date until Library › Update search index runs";
        }
        if (!notes.isEmpty()) R.indexNote = notes.join(" · ");
    }
    QSettings("ALL", "TranslationTool").setValue("replacefiles/lastChangeset", id);
    return R;
}

// Undo a run: every recorded file whose current hash still equals postSha1 is
// banked (pre-restore, same changeset) and restored from its pre-replace
// version; a file edited since is skipped with the reason, never clobbered.
inline ApplyReport undoChangeset(const QString& dataRoot, const QString& id, bool confirmed) {
    ApplyReport R;
    if (!confirmed) { R.message = "Not confirmed — nothing restored."; return R; }
    const std::string root = docprops::versionsRoot(dataRoot).toStdString();
    allcore::versions::Changeset cs;
    if (!allcore::versions::readChangeset(root, id.toStdString(), cs)) { R.message = "No changeset record " + id; return R; }
    if (cs.undone) { R.message = "This run was already undone on " + QString::fromStdString(cs.undoneAt); return R; }
    const auto banked = allcore::versions::byChangeset(root, id.toStdString());
    QStringList stamps;
    for (const auto& f : cs.files) {
        if (f.status != "changed") continue;
        const QString path = QString::fromStdString(f.path);
        FileRead now = readForRewrite(path);
        if (!now.ok) { ++R.skipped; R.lines << path + " — skipped: cannot read"; continue; }
        if (sha1(now.raw).toStdString() != f.postSha1) { ++R.skipped; R.lines << path + " — skipped: changed since this run"; continue; }
        const allcore::versions::Entry* pre = nullptr;
        const std::string absPath = QFileInfo(path).absoluteFilePath().toStdString();
        for (const auto& e : banked) if ((e.meta.path == f.path || e.meta.path == absPath) && (e.meta.reason == "pre-replace" || e.meta.reason == "pre-normalize")) pre = &e;
        QByteArray bytes;
        if (!pre || !docprops::versionBytes(QString::fromStdString(allcore::versions::versionsDirFor(root, allcore::versions::docKeyFor(f.path, dataRoot.toStdString()))), *pre, bytes)) { ++R.skipped; R.lines << path + " — skipped: the pre-replace version is missing or damaged"; continue; }
        const QString vn = docprops::noteVersion(nullptr, dataRoot, path, now.raw, "document", "pre-restore", id, "UTF-8", QString::fromStdString(f.eol), f.bom, QJsonObject());
        if (vn.startsWith("Version NOT kept")) { ++R.skipped; R.lines << path + " — skipped: could not bank the current text first"; continue; }
        if (!saveOrWarn(nullptr, path, bytes, "the restored file")) { ++R.writeFailed; R.lines << path + " — NOT WRITTEN"; continue; }
        ++R.restored; stamps << QString::fromStdString(pre->meta.stamp); R.lines << path + " — restored from version " + QString::fromStdString(pre->meta.stamp);
        if (normalize::g_fileNormalized) normalize::g_fileNormalized(path);
    }
    QString who; docprops::whoSaves(&who);
    allcore::versions::markChangesetUndone(root, id.toStdString(), QDateTime::currentDateTime().toString(Qt::ISODate).toStdString(), docprops::whoSaves(nullptr).toStdString(), R.restored, R.skipped);
    R.ok = true; R.changesetId = id;
    R.message = QString("Restored %1 file%2; %3 skipped; %4 failure%5. Versions restored: %6. Each restore kept the file as it was (pre-restore) first.").arg(R.restored).arg(R.restored == 1 ? "" : "s").arg(R.skipped).arg(R.writeFailed).arg(R.writeFailed == 1 ? "" : "s").arg(stamps.isEmpty() ? "none" : stamps.join(", "));
    return R;
}

// ---------------------------------------------------------------- the window
class ReplaceFilesWindow : public QDialog {
public:
    ReplaceFilesWindow(const QString& dataRoot, QWidget* parent = nullptr) : QDialog(parent), dataRoot_(dataRoot) {
        setWindowFlag(Qt::Window); setWindowTitle("Replace in Files"); resize(1040, 760);
        auto* v = new QVBoxLayout(this);
        auto* banner = new QLabel("Nothing is written until you press Apply to ticked files. Counts are made in the files' text, not in the search index."); banner->setStyleSheet("color:#6B5E4E;"); banner->setWordWrap(true); v->addWidget(banner);
        modes_ = new QTabWidget; v->addWidget(modes_);
        {   // Find and replace
            auto* w = new QWidget; auto* f = new QFormLayout(w);
            find_ = new QComboBox; find_->setEditable(true); with_ = new QComboBox; with_->setEditable(true);
            QSettings st("ALL", "TranslationTool");
            find_->addItems(st.value("find/history").toStringList()); with_->addItems(st.value("find/replaceHistory").toStringList()); find_->setCurrentText(""); with_->setCurrentText("");
            f->addRow("Find:", find_); f->addRow("Replace with:", with_);
            auto* o = new QHBoxLayout; regex_ = new QCheckBox("Regular expression"); case_ = new QCheckBox("Match case"); word_ = new QCheckBox("Whole word"); pcase_ = new QCheckBox("Preserve case");
            for (auto* c : {regex_, case_, word_, pcase_}) o->addWidget(c); o->addStretch(1); f->addRow(o);
            connect(find_, &QComboBox::currentTextChanged, this, [this](const QString& t) { bool letters = false; for (const QChar c : t) letters = letters || c.isLetter(); pcase_->setEnabled(letters); if (!letters) pcase_->setChecked(false); invalidate(); });
            modes_->addTab(w, "Find and replace");
        }
        {   // Normalize (rules) — F3's panel embedded
            normalize::Target dummy; dummy.kind = "compare"; dummy.text = [] { return QString(); }; dummy.hasSelection = [] { return false; }; dummy.selectionText = [] { return QString(); }; dummy.apply = [](const QString&, bool) { return false; };
            norm_ = new normalize::NormalizeDialog(dummy, this); norm_->setEmbedded();
            auto* sa = new QScrollArea; sa->setWidgetResizable(true); sa->setWidget(norm_);
            modes_->addTab(sa, "Normalize (rules)");
        }
        connect(modes_, &QTabWidget::currentChanged, this, [this](int) { invalidate(); });
        // scope
        auto* scopeBox = new QGroupBox("In"); auto* sg = new QGridLayout(scopeBox);
        folders_ = new QListWidget; folders_->setMaximumHeight(72); sg->addWidget(folders_, 0, 0, 3, 1);
        seedLabel_ = new QLabel; seedLabel_->setVisible(false); sg->addWidget(seedLabel_, 3, 0);
        auto* addF = new QPushButton("Add folder…"); auto* remF = new QPushButton("Remove"); sg->addWidget(addF, 0, 1); sg->addWidget(remF, 1, 1);
        connect(addF, &QPushButton::clicked, this, [this] { const QString d = safeGetExistingDirectory(this, "Add a folder"); if (!d.isEmpty()) { folders_->addItem(d); invalidate(); } });
        connect(remF, &QPushButton::clicked, this, [this] { delete folders_->takeItem(folders_->currentRow()); invalidate(); });
        recurse_ = new QCheckBox("Subfolders"); recurse_->setChecked(true); hidden_ = new QCheckBox("Hidden"); sg->addWidget(recurse_, 2, 1); sg->addWidget(hidden_, 3, 1);
        include_ = new QLineEdit("*.txt;*.act;*.inc;*.ace;*.acip;*.md"); exclude_ = new QLineEdit("*META.TXT;.index.db*");
        sg->addWidget(new QLabel("Include files:"), 4, 0); sg->addWidget(include_, 5, 0); sg->addWidget(new QLabel("Exclude (! negates):"), 4, 1); sg->addWidget(exclude_, 5, 1);
        auto* capRow = new QHBoxLayout; cap_ = new QSpinBox; cap_->setRange(1, 100000); cap_->setValue(4000); capRow->addWidget(new QLabel("File cap:")); capRow->addWidget(cap_);
        allowLib_ = new QCheckBox("Allow changes to texts under /library/"); allowLib_->setToolTip("Canonical library texts are listed with a library badge and stay unticked until this is on"); capRow->addWidget(allowLib_); capRow->addStretch(1);
        sg->addLayout(capRow, 6, 0, 1, 2);
        v->addWidget(scopeBox);
        for (QObject* c : {static_cast<QObject*>(recurse_), static_cast<QObject*>(hidden_), static_cast<QObject*>(allowLib_)}) connect(qobject_cast<QCheckBox*>(c), &QCheckBox::toggled, this, [this](bool) { invalidate(); });
        connect(include_, &QLineEdit::textChanged, this, [this](const QString&) { invalidate(); }); connect(exclude_, &QLineEdit::textChanged, this, [this](const QString&) { invalidate(); }); connect(cap_, qOverload<int>(&QSpinBox::valueChanged), this, [this](int) { invalidate(); });
        for (auto* c : {regex_, case_, word_, pcase_}) connect(c, &QCheckBox::toggled, this, [this](bool) { invalidate(); });
        connect(with_, &QComboBox::currentTextChanged, this, [this](const QString&) { invalidate(); });
        // preview
        auto* pr = new QHBoxLayout; previewBtn_ = new QPushButton("Preview changes"); stopBtn_ = new QPushButton("Stop"); stopBtn_->setEnabled(false); progress_ = new QLabel;
        pr->addWidget(previewBtn_); pr->addWidget(stopBtn_); pr->addWidget(progress_, 1); v->addLayout(pr);
        connect(previewBtn_, &QPushButton::clicked, this, [this] { preview(); });
        connect(stopBtn_, &QPushButton::clicked, this, [this] { stop_ = true; });
        tree_ = new QTreeWidget; tree_->setHeaderLabels({"File / occurrence", "Detail"}); tree_->header()->setSectionResizeMode(0, QHeaderView::Stretch); tree_->setContextMenuPolicy(Qt::CustomContextMenu);
        v->addWidget(tree_, 1);
        connect(tree_, &QTreeWidget::itemExpanded, this, [this](QTreeWidgetItem* it) { populateChildren(it); });
        connect(tree_, &QTreeWidget::itemChanged, this, [this](QTreeWidgetItem* it, int col) { onTicked(it, col); });
        connect(tree_, &QTreeWidget::itemDoubleClicked, this, [this](QTreeWidgetItem* it, int) { showFileDiff(it); });
        connect(tree_, &QTreeWidget::customContextMenuRequested, this, [this](const QPoint& p) {
            auto* it = tree_->itemAt(p); if (!it) return; QMenu m;
            m.addAction("Open in Overlay at this line", [this, it] { const int fi = fileIndexOf(it); if (fi < 0) return; const int line = it->parent() ? it->data(0, Qt::UserRole + 1).toInt() : 1; if (g_openAtLine) g_openAtLine(plan_.files[fi].path, line); });
            m.exec(tree_->viewport()->mapToGlobal(p));
        });
        auto* tk = new QHBoxLayout; auto* tickAll = new QPushButton("Tick all"); auto* untickAll = new QPushButton("Untick all"); counts_ = new QLabel;
        tk->addWidget(tickAll); tk->addWidget(untickAll); tk->addWidget(counts_, 1); v->addLayout(tk);
        connect(tickAll, &QPushButton::clicked, this, [this] { tickEverything(true); }); connect(untickAll, &QPushButton::clicked, this, [this] { tickEverything(false); });
        footer_ = new QLabel; footer_->setWordWrap(true); footer_->setStyleSheet("color:#6B5E4E;"); v->addWidget(footer_);
        notice_ = new QLabel; notice_->setWordWrap(true); notice_->setStyleSheet("color:#9A2B1E;"); notice_->setVisible(false); v->addWidget(notice_);
        auto* ap = new QHBoxLayout; updIndex_ = new QCheckBox("Update the folder's search index afterwards"); updIndex_->setEnabled(false);
        applyBtn_ = new QPushButton("Apply to ticked files"); applyBtn_->setEnabled(false); undoBtn_ = new QPushButton("Undo this run…"); undoBtn_->setEnabled(false); auto* close = new QPushButton("Close");
        ap->addWidget(updIndex_); ap->addStretch(1); ap->addWidget(applyBtn_); ap->addWidget(undoBtn_); ap->addWidget(close); v->addLayout(ap);
        connect(applyBtn_, &QPushButton::clicked, this, [this] { applyStep(); });
        connect(undoBtn_, &QPushButton::clicked, this, [this] { undoStep(); });
        connect(close, &QPushButton::clicked, this, [this] { remember(); this->close(); });
        result_ = new QTextBrowser; result_->setOpenLinks(false); result_->setMaximumHeight(150); result_->setVisible(false); v->addWidget(result_);
        connect(result_, &QTextBrowser::anchorClicked, this, [](const QUrl& u) { const QString s = u.toString(); if (s.startsWith("rifopen:")) { const QString body = s.mid(8); const int bar = body.lastIndexOf('|'); if (bar > 0 && g_openAtLine) g_openAtLine(QUrl::fromPercentEncoding(body.left(bar).toUtf8()), body.mid(bar + 1).toInt()); } });
        armTimer_.setSingleShot(true); armTimer_.setInterval(8000); connect(&armTimer_, &QTimer::timeout, this, [this] { armed_ = 0; applyBtn_->setText("Apply to ticked files"); undoBtn_->setText("Undo this run…"); });
        // remembered scope
        QSettings st("ALL", "TranslationTool");
        include_->setText(st.value("replacefiles/include", include_->text()).toString()); exclude_->setText(st.value("replacefiles/exclude", exclude_->text()).toString());
        recurse_->setChecked(st.value("replacefiles/subfolders", true).toBool()); hidden_->setChecked(st.value("replacefiles/hidden", false).toBool()); allowLib_->setChecked(st.value("replacefiles/allowLibrary", false).toBool()); updIndex_->setChecked(st.value("replacefiles/updateIndex", false).toBool());
        const QString last = st.value("replacefiles/lastFolder").toString(); if (!last.isEmpty() && QFileInfo(last).isDir()) folders_->addItem(last);
        lastChangeset_ = st.value("replacefiles/lastChangeset").toString(); undoBtn_->setEnabled(!lastChangeset_.isEmpty()); if (!lastChangeset_.isEmpty()) undoBtn_->setText("Undo last run (" + lastChangeset_ + ")…");
    }
    bool harnessAutoConfirm_ = false;
    void seed(const QString& find, const QString& with) { find_->setCurrentText(find); with_->setCurrentText(with); modes_->setCurrentIndex(0); invalidate(); }
    void seedFiles(const QStringList& files) { seedFiles_ = files; folders_->setVisible(files.isEmpty()); seedLabel_->setVisible(!files.isEmpty()); seedLabel_->setText(QString("%1 selected file%2/folder%2 from the Files pane").arg(files.size()).arg(files.size() == 1 ? "" : "s")); invalidate(); }
    void seedFolders(const QStringList& folders) { folders_->clear(); for (const auto& f : folders) folders_->addItem(f); seedFiles_.clear(); folders_->setVisible(true); seedLabel_->setVisible(false); invalidate(); }
    Spec spec() const {
        Spec s; s.normalizeMode = modes_->currentIndex() == 1; s.find = find_->currentText(); s.with = with_->currentText();
        s.opts.regex = regex_->isChecked(); s.opts.caseSensitive = case_->isChecked(); s.opts.wholeWord = word_->isChecked(); s.opts.preserveCase = pcase_->isEnabled() && pcase_->isChecked();
        s.norm = norm_->options(); s.houseStyle = norm_->houseStyle(); return s;
    }
    Scope scope() const { Scope sc; sc.files = seedFiles_; if (seedFiles_.isEmpty()) for (int i = 0; i < folders_->count(); ++i) sc.folders << folders_->item(i)->text(); sc.include = include_->text(); sc.exclude = exclude_->text(); sc.recurse = recurse_->isChecked(); sc.hidden = hidden_->isChecked(); sc.fileCap = cap_->value(); return sc; }
    const Plan& plan() const { return plan_; }
    Plan& planMutable() { return plan_; }
    bool applyEnabled() const { return applyBtn_->isEnabled(); }
    QString footerText() const { return footer_->text(); }
    QString resultText() const { return result_->toPlainText(); }
    void invalidate() { applyBtn_->setEnabled(false); previewKey_.clear(); armed_ = 0; applyBtn_->setText("Apply to ticked files"); }

    Plan preview() {
        const Spec sp = spec(); const Scope sc = scope();
        if (!sp.normalizeMode && sp.find.isEmpty()) { footer_->setText("Type something to find first."); invalidate(); return Plan(); }
        if (sc.files.isEmpty() && sc.folders.isEmpty()) { footer_->setText("Add a folder first."); invalidate(); return Plan(); }
        stop_ = false; stopBtn_->setEnabled(true); previewBtn_->setEnabled(false);
        bool inPump = false;
        plan_ = buildPlan(sc, sp, dataRoot_, [this, &inPump](int done, int total) { if (inPump) return !stop_; inPump = true; progress_->setText(QString("Read %L1 of %L2 files…").arg(done).arg(total)); if (!g_harnessRun) QCoreApplication::processEvents(QEventLoop::ExcludeUserInputEvents); inPump = false; return !stop_; });
        stopBtn_->setEnabled(false); previewBtn_->setEnabled(true); progress_->setText(plan_.stoppedEarly ? "Stopped early." : "");
        if (!plan_.regexError.isEmpty()) { footer_->setText("Invalid regular expression: " + plan_.regexError); tree_->clear(); invalidate(); return plan_; }
        for (auto& F : plan_.files) if (F.library && !allowLib_->isChecked()) { F.ticked = false; }
        rebuildTree();
        notice_->setVisible(!plan_.scriptNotice.isEmpty()); notice_->setText(plan_.scriptNotice);
        bool anyIndex = false; for (const auto& F : plan_.files) anyIndex = anyIndex || QFileInfo::exists(QFileInfo(F.path).path() + "/.index.db"); updIndex_->setEnabled(anyIndex);
        previewKey_ = plan_.key + (sp.normalizeMode ? "N" : "R"); applyBtn_->setEnabled(!plan_.files.isEmpty() && !plan_.stoppedEarly);
        if (plan_.stoppedEarly) footer_->setText(footer_->text() + " Preview again after a complete scan before applying.");
        QSettings st("ALL", "TranslationTool"); if (!sc.folders.isEmpty()) st.setValue("replacefiles/lastFolder", sc.folders.first());
        if (!sp.normalizeMode) { QStringList h = st.value("find/history").toStringList(); h.removeAll(sp.find); h.prepend(sp.find); st.setValue("find/history", h.mid(0, 30)); QStringList rh = st.value("find/replaceHistory").toStringList(); rh.removeAll(sp.with); rh.prepend(sp.with); st.setValue("find/replaceHistory", rh.mid(0, 30)); }
        return plan_;
    }
    // Two-step apply; under the harness harnessAutoConfirm_ decides.
    ApplyReport applyStep() {
        ApplyReport R;
        if (!applyBtn_->isEnabled() || previewKey_ != plan_.key + (spec().normalizeMode ? "N" : "R")) { invalidate(); footer_->setText("The inputs changed since the preview — preview again."); R.message = footer_->text(); return R; }
        int files = 0, lib = 0; for (const auto& F : plan_.files) if (F.ticked && F.tickedCount()) { ++files; lib += F.library ? 1 : 0; }
        if (g_harnessRun) { if (!harnessAutoConfirm_) { R.message = "Refused: no confirmation."; footer_->setText(R.message); return R; } return doApply(); }
        if (armed_ != 1) { armed_ = 1; applyBtn_->setText(QString("Confirm: %1 file%2 will be rewritten%3").arg(files).arg(files == 1 ? "" : "s").arg(lib ? QString(" (%1 of them canonical library texts under /library/)").arg(lib) : QString())); armTimer_.start(); return R; }
        armed_ = 0; applyBtn_->setText("Apply to ticked files"); armTimer_.stop();
        return doApply();
    }
    ApplyReport doApply() {
        ApplyReport R = applyPlan(dataRoot_, plan_, spec(), scope(), allowLib_->isChecked(), true, openDirtyReason_, updIndex_->isChecked());
        showReport(R, false);
        if (R.ok) { lastChangeset_ = R.changesetId; undoBtn_->setEnabled(true); undoBtn_->setText("Undo this run (" + lastChangeset_ + ")…"); }
        invalidate();
        return R;
    }
    ApplyReport undoStep() {
        ApplyReport R;
        if (lastChangeset_.isEmpty()) { R.message = "No run to undo."; return R; }
        if (g_harnessRun) { if (!harnessAutoConfirm_) { R.message = "Refused: no confirmation."; return R; } return doUndo(); }
        if (armed_ != 2) { armed_ = 2; undoBtn_->setText("Confirm: restore the files of " + lastChangeset_ + " from their pre-replace versions"); armTimer_.start(); return R; }
        armed_ = 0; armTimer_.stop();
        return doUndo();
    }
    ApplyReport doUndo() {
        ApplyReport R = undoChangeset(dataRoot_, lastChangeset_, true);
        showReport(R, true);
        undoBtn_->setText("Undo this run…"); undoBtn_->setEnabled(false); lastChangeset_.clear(); QSettings("ALL", "TranslationTool").remove("replacefiles/lastChangeset");
        return R;
    }
    void setOpenDirtyReason(std::function<QString(const QString&)> f) { openDirtyReason_ = std::move(f); }
    void tickEverything(bool on) { for (auto& F : plan_.files) { F.ticked = on && !(F.library && !allowLib_->isChecked()); for (auto& o : F.occs) o.ticked = on; } rebuildTree(); }
    void setLastChangeset(const QString& id) { lastChangeset_ = id; undoBtn_->setEnabled(!id.isEmpty()); }
    void expandFirst() { if (tree_->topLevelItemCount()) tree_->topLevelItem(0)->setExpanded(true); }

private:
    int fileIndexOf(QTreeWidgetItem* it) const { QTreeWidgetItem* top = it->parent() ? it->parent() : it; return top->data(0, Qt::UserRole).toInt(); }
    void rebuildTree() {
        tree_->blockSignals(true); tree_->clear();
        for (int i = 0; i < plan_.files.size(); ++i) {
            const auto& F = plan_.files[i];
            auto* it = new QTreeWidgetItem;
            const QString rel = QFileInfo(F.path).fileName();
            it->setText(0, rel + QString(" — %1 occurrence%2 · %3%4%5").arg(F.occs.size()).arg(F.occs.size() == 1 ? "" : "s").arg(normalize::eolName(F.eol)).arg(F.bom ? " · BOM" : "").arg(F.library ? " · library" : ""));
            it->setText(1, F.path); it->setToolTip(1, F.path);
            it->setFlags(it->flags() | Qt::ItemIsUserCheckable); it->setCheckState(0, F.ticked ? Qt::Checked : Qt::Unchecked);
            if (F.library && !allowLib_->isChecked()) it->setFlags(it->flags() & ~Qt::ItemIsEnabled);
            it->setData(0, Qt::UserRole, i); it->setChildIndicatorPolicy(QTreeWidgetItem::ShowIndicator);
            tree_->addTopLevelItem(it);
        }
        tree_->blockSignals(false);
        updateCounts();
    }
    void populateChildren(QTreeWidgetItem* it) {   // lazily, on expand
        if (it->parent() || it->childCount() > 0) return;
        const int fi = fileIndexOf(it); if (fi < 0 || fi >= plan_.files.size()) return;
        tree_->blockSignals(true);
        const auto& F = plan_.files[fi];
        for (int k = 0; k < F.occs.size(); ++k) {
            const auto& o = F.occs[k];
            auto* c = new QTreeWidgetItem(it);
            c->setText(0, QString("line %1 · %2").arg(o.line).arg(o.cite));
            c->setText(1, o.oldLine.trimmed().left(60) + "  ▸  " + o.newLine.trimmed().left(60));
            c->setToolTip(1, "before: " + o.oldLine + "\nafter:  " + o.newLine);
            c->setFlags(c->flags() | Qt::ItemIsUserCheckable); c->setCheckState(0, o.ticked ? Qt::Checked : Qt::Unchecked);
            c->setData(0, Qt::UserRole + 1, o.line); c->setData(0, Qt::UserRole + 2, k);
        }
        tree_->blockSignals(false);
    }
    void onTicked(QTreeWidgetItem* it, int col) {
        if (col != 0) return;
        const int fi = fileIndexOf(it); if (fi < 0 || fi >= plan_.files.size()) return;
        auto& F = plan_.files[fi];
        tree_->blockSignals(true);
        if (!it->parent()) { F.ticked = it->checkState(0) == Qt::Checked; for (auto& o : F.occs) o.ticked = F.ticked; for (int k = 0; k < it->childCount(); ++k) it->child(k)->setCheckState(0, it->checkState(0)); }
        else { const int k = it->data(0, Qt::UserRole + 2).toInt(); if (k >= 0 && k < F.occs.size()) F.occs[k].ticked = it->checkState(0) == Qt::Checked; if (F.tickedCount() == 0) { F.ticked = false; it->parent()->setCheckState(0, Qt::Unchecked); } else if (!F.ticked) { F.ticked = true; it->parent()->setCheckState(0, Qt::Checked); } }
        tree_->blockSignals(false);
        updateCounts();
    }
    void updateCounts() {
        int tf = 0, to = 0; for (const auto& F : plan_.files) if (F.ticked) { ++tf; to += F.tickedCount(); }
        counts_->setText(QString("%1 files, %2 of %3 occurrences ticked").arg(plan_.files.size()).arg(to).arg(plan_.occurrences));
        footer_->setText(footer(plan_, tf, to));
    }
    void showFileDiff(QTreeWidgetItem* it) {
        const int fi = fileIndexOf(it); if (fi < 0 || fi >= plan_.files.size()) return;
        const auto& F = plan_.files[fi]; const QString name = QFileInfo(F.path).fileName();
        const QString after = spec().normalizeMode ? F.after.join('\n') + "\n" : applyTicked(F.text, F.occs);
        if (g_compareTexts) g_compareTexts(name + " (before)", F.text, name + " (after, ticked occurrences)", after);
    }
    void showReport(const ApplyReport& R, bool undo) {
        QString html = "<b>" + R.message.toHtmlEscaped() + "</b>" + (R.changesetId.isEmpty() ? "" : " · changeset " + R.changesetId) + "<br>";
        for (const QString& l : R.lines) { const int dash = l.indexOf(" — "); const QString path = dash > 0 ? l.left(dash) : QString(); html += path.isEmpty() ? l.toHtmlEscaped() : "<a href='rifopen:" + QString::fromUtf8(QUrl::toPercentEncoding(path)) + "|1'>" + path.toHtmlEscaped() + "</a>" + l.mid(dash).toHtmlEscaped(); html += "<br>"; }
        if (!R.indexNote.isEmpty()) html += "<i>" + R.indexNote.toHtmlEscaped() + "</i><br>";
        if (!undo && R.ok) html += "<i>Undo this run restores each changed file from its pre-replace version (each restore keeps a pre-restore copy first).</i>";
        result_->setHtml(html); result_->setVisible(true);
    }
    void remember() { QSettings st("ALL", "TranslationTool"); st.setValue("replacefiles/include", include_->text()); st.setValue("replacefiles/exclude", exclude_->text()); st.setValue("replacefiles/subfolders", recurse_->isChecked()); st.setValue("replacefiles/hidden", hidden_->isChecked()); st.setValue("replacefiles/allowLibrary", allowLib_->isChecked()); st.setValue("replacefiles/updateIndex", updIndex_->isChecked()); }

    QString dataRoot_; Plan plan_; QString previewKey_, lastChangeset_; QStringList seedFiles_; bool stop_ = false; int armed_ = 0; QTimer armTimer_;
    std::function<QString(const QString&)> openDirtyReason_;
    QTabWidget* modes_ = nullptr; QComboBox *find_ = nullptr, *with_ = nullptr; QCheckBox *regex_ = nullptr, *case_ = nullptr, *word_ = nullptr, *pcase_ = nullptr; normalize::NormalizeDialog* norm_ = nullptr;
    QListWidget* folders_ = nullptr; QLabel* seedLabel_ = nullptr; QCheckBox *recurse_ = nullptr, *hidden_ = nullptr, *allowLib_ = nullptr, *updIndex_ = nullptr; QLineEdit *include_ = nullptr, *exclude_ = nullptr; QSpinBox* cap_ = nullptr;
    QPushButton *previewBtn_ = nullptr, *stopBtn_ = nullptr, *applyBtn_ = nullptr, *undoBtn_ = nullptr; QLabel *progress_ = nullptr, *counts_ = nullptr, *footer_ = nullptr, *notice_ = nullptr; QTreeWidget* tree_ = nullptr; QTextBrowser* result_ = nullptr;
};

static QPointer<ReplaceFilesWindow> g_replaceFilesWin;
inline ReplaceFilesWindow* window(const QString& dataRoot, QWidget* parent, const std::function<QString(const QString&)>& openDirtyReason) {
    if (!g_replaceFilesWin) { g_replaceFilesWin = new ReplaceFilesWindow(dataRoot, parent); g_replaceFilesWin->setOpenDirtyReason(openDirtyReason); }
    if (!g_harnessRun) { g_replaceFilesWin->show(); g_replaceFilesWin->raise(); g_replaceFilesWin->activateWindow(); }
    return g_replaceFilesWin;
}

// ---- selftests (fixture-free; a temp folder under the data root's temp area)
inline void selfTest(const QString& dataRoot, const std::function<void(bool, const char*)>& check) {
    const QString td = QDir::tempPath() + "/dct_f4_" + QString::number(QCoreApplication::applicationPid()); QDir(td).removeRecursively(); QDir().mkpath(td + "/sub");
    auto w = [&](const QString& rel, const QByteArray& b) { QFile f(td + "/" + rel); if (f.open(QIODevice::WriteOnly)) f.write(b); };
    w("a.act", "@001A KHYB PA ,\r\nKHYB PA KHYB\r\n");                       // CRLF, two lines, three occurrences
    w("b.act", QByteArray("\xEF\xBB\xBF" "KA KHYB\n"));                     // BOM
    w("sub/c.txt", "khyb pa\n");                                            // lower case (case test)
    w("bin.act", QByteArray("KHYB\0KHYB", 9));                              // binary
    w("latin.act", QByteArray("KHYB caf\xE9\n"));                           // not UTF-8
    w("notes.md", "nothing here\n");
    Spec sp; sp.find = "KHYB"; sp.with = "KHYAB"; sp.opts.caseSensitive = true;
    Scope sc; sc.folders << td;
    QSettings vst("ALL", "TranslationTool"); const QVariant was = vst.value("versions/enabled"); vst.setValue("versions/enabled", true);
    Plan P = buildPlan(sc, sp, dataRoot);
    check(P.filesRead == 4 && P.withMatches == 2 && P.occurrences == 4 && P.skippedBinary == 1 && P.skippedNotUtf8 == 1, "F4-1 the plan reads the eligible files, counts occurrences in their text, and names binary and non-UTF-8 skips");
    int ai = -1, bi = -1; for (int i = 0; i < P.files.size(); ++i) { if (P.files[i].path.endsWith("a.act")) ai = i; if (P.files[i].path.endsWith("b.act")) bi = i; }
    check(ai >= 0 && P.files[ai].eol == "\r\n" && P.files[ai].occs.size() == 3 && P.files[ai].occs[0].cite == "@001A.1" && P.files[ai].occs[1].line == 2 && P.files[ai].occs[0].newLine == "@001A KHYAB PA ," && bi >= 0 && P.files[bi].bom,
          "F4-2 occurrences carry line, folio cite, before/after lines; EOL and BOM are recorded");
    check(applyTicked(P.files[ai].text, P.files[ai].occs) == "@001A KHYAB PA ,\nKHYAB PA KHYAB\n" && bytesForWrite("x\ny\n", "\r\n", true, true) == QByteArray("\xEF\xBB\xBFx\r\ny\r\n"), "F4-3 applyTicked replaces back to front; bytesForWrite restores EOL, BOM and the final line break");
    P.files[ai].occs[2].ticked = false;   // untick the third occurrence
    QSettings st("ALL", "TranslationTool"); const QVariant lc = st.value("replacefiles/lastChangeset"); QStringList writtenIds;
    ApplyReport R0 = applyPlan(dataRoot, P, sp, sc, false, false, {}, false);
    check(!R0.ok && R0.message.contains("Not confirmed"), "F4-4 nothing is written without confirmation");
    ApplyReport R = applyPlan(dataRoot, P, sp, sc, false, true, [](const QString& p) { return p.endsWith("b.act") ? QString("open in the Overlay with unsaved edits") : QString(); }, false);
    QFile fa(td + "/a.act"); const QByteArray aNow = fa.open(QIODevice::ReadOnly) ? fa.readAll() : QByteArray(); QFile fb(td + "/b.act"); const QByteArray bNow = fb.open(QIODevice::ReadOnly) ? fb.readAll() : QByteArray();
    writtenIds << R.changesetId;
    check(R.ok && R.changed == 1 && R.occurrences == 2 && R.skipped == 1 && aNow == "@001A KHYAB PA ,\r\nKHYAB PA KHYB\r\n" && bNow == QByteArray("\xEF\xBB\xBF" "KA KHYB\n"),
          "F4-5 apply rewrites only ticked occurrences with the file's own CRLF; the open-with-edits file is skipped by name and untouched");
    allcore::versions::Changeset cs; const bool csOk = allcore::versions::readChangeset(docprops::versionsRoot(dataRoot).toStdString(), R.changesetId.toStdString(), cs);
    check(csOk && cs.changed == 1 && cs.skipped == 1 && cs.files.size() == 2 && !cs.undone, "F4-6 the changeset record holds the run (spec, files, summary)");
    const auto banked = allcore::versions::byChangeset(docprops::versionsRoot(dataRoot).toStdString(), R.changesetId.toStdString());
    check(banked.size() == 1 && banked[0].meta.reason == "pre-replace" && banked[0].meta.path == QFileInfo(td + "/a.act").absoluteFilePath().toStdString(), "F4-7 the rewritten file has a pre-replace version carrying the changeset id");
    {   // stale preview: file changed on disk → skipped
        Plan P2 = buildPlan(sc, sp, dataRoot);
        w("a.act", "@001A KHYB ZZ\r\n");
        ApplyReport R2 = applyPlan(dataRoot, P2, sp, sc, false, true, {}, false); writtenIds << R2.changesetId;
        bool aSkipped = false; for (const auto& l : R2.lines) aSkipped = aSkipped || (l.contains("a.act") && l.contains("changed on disk since the preview"));
        check(R2.ok && aSkipped, "F4-8 a file changed since the preview is skipped, never rewritten from stale text");
        w("a.act", aNow);
    }
    {   // undo restores from the pre-replace version, skips an edited file
        w("b.act", "edited since\n");
        allcore::versions::Changeset csDbg; allcore::versions::readChangeset(docprops::versionsRoot(dataRoot).toStdString(), R.changesetId.toStdString(), csDbg);
        QString dbg; for (const auto& f : csDbg.files) dbg += QString::fromStdString(f.path).section('/', -1) + ":" + QString::fromStdString(f.status) + ":" + QString::fromStdString(f.postSha1).left(8) + " ";
        ApplyReport U = undoChangeset(dataRoot, R.changesetId, true);
        QFile fa2(td + "/a.act"); const QByteArray a2 = fa2.open(QIODevice::ReadOnly) ? fa2.readAll() : QByteArray();
        check(U.ok && U.restored == 1 && a2 == "@001A KHYB PA ,\r\nKHYB PA KHYB\r\n" && U.message.contains("pre-restore"), ("F4-9 undo restores the rewritten file byte-for-byte and says each restore banked first [" + U.message + " | " + U.lines.join(" ; ") + " | record: " + dbg + "]").toUtf8().constData());
        allcore::versions::Changeset cs2; allcore::versions::readChangeset(docprops::versionsRoot(dataRoot).toStdString(), R.changesetId.toStdString(), cs2);
        check(cs2.undone && cs2.undoneRestored == 1, "F4-9b the changeset is marked undone");
        ApplyReport U2 = undoChangeset(dataRoot, R.changesetId, true);
        check(!U2.ok && U2.message.contains("already undone"), "F4-9c a second undo is refused");
    }
    {   // regex, preserve case, whole word, zero-length safety, invalid regex
        Spec r; r.find = "khyb"; r.with = "khyab"; r.opts.preserveCase = true;
        Plan Pr = buildPlan(sc, r, dataRoot);
        int ci = -1; for (int i = 0; i < Pr.files.size(); ++i) if (Pr.files[i].path.endsWith("c.txt")) ci = i;
        int aj = -1; for (int i = 0; i < Pr.files.size(); ++i) if (Pr.files[i].path.endsWith("a.act")) aj = i;
        check(ci >= 0 && Pr.files[ci].occs[0].with == "khyab" && aj >= 0 && Pr.files[aj].occs[0].with == "KHYAB", "F4-10 Preserve case shapes the replacement to each match (khyab / KHYAB)");
        Spec z; z.find = "^"; z.with = "> "; z.opts.regex = true;
        Plan Pz = buildPlan(sc, z, dataRoot); int cz = -1; for (int i = 0; i < Pz.files.size(); ++i) if (Pz.files[i].path.endsWith("notes.md")) cz = i;
        check(cz >= 0 && Pz.files[cz].occs.size() == 1 && applyTicked(Pz.files[cz].text, Pz.files[cz].occs) == "> nothing here\n", "F4-11 a zero-length regex match advances and never loops");
        Spec bad; bad.find = "("; bad.opts.regex = true; Plan Pb = buildPlan(sc, bad, dataRoot);
        check(!Pb.regexError.isEmpty() && Pb.files.isEmpty(), "F4-12 an invalid regular expression is reported, not run");
    }
    {   // script mismatch notice (b.act was edited by the undo check above — restore an ACIP body first)
        w("b.act", QByteArray("\xEF\xBB\xBF" "KA KHYB\n"));
        Spec u; u.find = "ཁིབ"; Plan Pu = buildPlan(sc, u, dataRoot);
        check(Pu.occurrences == 0 && Pu.scriptNotice.contains("Tibetan Unicode") && Pu.scriptNotice.contains("ACIP"), "F4-13 a script-mismatched zero says so instead of reading as a clean bill of health");
    }
    {   // file cap counted, library guard, normalize mode
        Scope capped = sc; capped.fileCap = 2; Plan Pc = buildPlan(capped, sp, dataRoot);
        check(Pc.walk.capped >= 1, "F4-14 files past the cap are counted, never silently dropped");
        Spec nm; nm.normalizeMode = true; nm.norm.collapseSpaces = true; w("sub/sp.txt", "A  B\n");
        Plan Pn = buildPlan(sc, nm, dataRoot); int si = -1; for (int i = 0; i < Pn.files.size(); ++i) if (Pn.files[i].path.endsWith("sp.txt")) si = i;
        check(si >= 0 && Pn.files[si].occs.size() == 1 && Pn.files[si].occs[0].newLine == "A B", "F4-15 Normalize mode lists changed lines as the tick unit");
        const QString libDir = dataRoot + "/library/dct_f4_probe"; QDir().mkpath(libDir); { QFile f(libDir + "/lib.act"); if (f.open(QIODevice::WriteOnly)) f.write("KHYB\n"); }
        Scope ls; ls.folders << libDir; Plan Pl = buildPlan(ls, sp, dataRoot);
        ApplyReport Rl = applyPlan(dataRoot, Pl, sp, ls, false, true, {}, false); writtenIds << Rl.changesetId;
        QFile lf(libDir + "/lib.act"); const QByteArray lfNow = lf.open(QIODevice::ReadOnly) ? lf.readAll() : QByteArray();
        check(Pl.files.size() == 1 && Pl.files[0].library && Rl.skipped == 1 && lfNow == "KHYB\n", "F4-16 a canonical library text is badged and skipped unless changes under /library/ are allowed");
        QDir(libDir).removeRecursively(); QDir(docprops::versionsDir(dataRoot, libDir + "/lib.act")).removeRecursively();
    }
    {   // the window: apply gating and harness confirmation
        ReplaceFilesWindow win(dataRoot); win.seedFolders({td}); win.seed("KHYB", "KHYAB");
        check(!win.applyEnabled(), "F4-17 Apply is disabled before a preview");
        win.preview(); check(win.applyEnabled() && win.footerText().contains("files read") && win.footerText().contains("not in the search index"), "F4-17b …enabled after a preview, with the honest footer");
        win.seed("KHYB", "KHYAB2"); check(!win.applyEnabled(), "F4-17c …disabled again when an input changes");
        win.preview(); ApplyReport Rw = win.applyStep(); check(!Rw.ok && Rw.message.contains("no confirmation"), "F4-18 the harness refuses Apply without confirmation");
        win.harnessAutoConfirm_ = true; Rw = win.applyStep(); writtenIds << Rw.changesetId; check(Rw.ok && Rw.changed >= 1 && win.resultText().contains("changeset"), "F4-18b …and applies with it, reporting the changeset");
        ApplyReport Uw = win.undoStep(); check(Uw.ok && Uw.restored == Rw.changed, "F4-18c Undo this run restores what the run changed");
    }
    {   // leave no test records behind in the real data root
        const QString cdir = QString::fromStdString(allcore::versions::changesetsDir(docprops::versionsRoot(dataRoot).toStdString()));
        for (const QString& id : writtenIds) QFile::remove(cdir + "/" + id + ".json");
    }
    if (lc.isValid()) st.setValue("replacefiles/lastChangeset", lc); else st.remove("replacefiles/lastChangeset");
    if (was.isValid()) vst.setValue("versions/enabled", was); else vst.remove("versions/enabled");
    for (const QString& f : {"a.act", "b.act", "sub/c.txt", "notes.md", "sub/sp.txt"}) QDir(docprops::versionsDir(dataRoot, td + "/" + f)).removeRecursively();
    QDir(td).removeRecursively();
}

}  // namespace replf
