#pragma once
// app/textreport.h - the honest-line helpers, lifted out of
// main.cpp on 2026-08-25 as the second step against
// RISK_REGISTER R7 (ux_tokens.h was the first).
//
// These three earn a unit together because they do one job: turn
// numbers into a sentence that does not overstate them. Each was
// written this week against a finding where the old sentence did:
// a display cap printed as a corpus total (DATA-1), a file walk
// that stopped silently (PERF-1), and a diagnostic report that
// must carry no user content at all. All three are pure, all
// three are pinned in app_selftest, and being pure is exactly
// what let them be pinned - which is the argument for this whole
// extraction in miniature.

#include <QFileInfo>
#include <QLocale>
#include <QMap>
#include <QString>
#include <QStringList>

#include "ux_tokens.h"

// SQA PERF-1 (2026-08-24). The unindexed search reads at most
// GoferScan::file_cap files. Before this, the rest were dropped in
// silence, so a rare phrase in the unread 4,988 of 8,988 returned "no
// matches" - a performance cap stated as a fact about the library
// (house rule 3). Pure so the drill can reach it without a pane.
// SQA DATA-1 (2026-08-24). The corpus header collapsed three
// different numbers into one and printed the smallest as if it were
// the largest: rows RENDERED (20), hits FETCHED (60), and the real
// attestation total. It rendered "60 hit(s)" for terms with
// thousands, under the binding layer's own name. A total of -1 means
// the engine could not measure it - FTS5 rejects the pane's own
// NEAR/N grammar at step() - and that must read as unknown, never as
// the fetch cap. Returns the phrase only; the caller supplies the
// layer's name, which is a class member. Pure so the drill can reach
// it without a pane.
// P1 2026-08-24: the diagnostics channel, and the reason it is shaped
// like this rather than like a crash reporter.
//
// docs/PRIVACY.md states that this application has no telemetry, and
// that was verified before it was written - no analytics SDK, no crash
// reporter, no launch beacon anywhere in app/, core/ or ocr/. Being
// able to say that plainly is worth keeping, so the diagnostics
// channel is deliberately NOT a reporter: nothing is transmitted, the
// user asks for a file, and the user decides whether to send it.
//
// What it carries is what answers "which build, on what machine" -
// version, commit, OS, architecture, suite count. What it must NEVER
// carry is anything the translator authored. File paths are reduced to
// their EXTENSION and size: the type is what diagnoses a parser bug,
// while the title of a text can itself be the sensitive part on this
// project, and the home directory carries the user's name.
inline QString buildDiagnosticReport(const QString& version,
                                     const QString& commit,
                                     const QString& osVersion,
                                     const QString& arch,
                                     int suites,
                                     const QStringList& recentPaths) {
    QString h;
    h += "Diamond Cutter Translation Tool - diagnostic report\n";
    h += "==================================================\n\n";
    h += "This file contains no text you have written and no file\n";
    h += "names. It is saved for you to send; nothing is transmitted.\n\n";
    h += "version        " + version + "\n";
    h += "git commit     " + commit + "\n";
    h += "macOS          " + osVersion + "\n";
    h += "architecture   " + arch + "\n";
    h += "test suites    " + QString::number(suites) + "\n";
    h += "\nrecent activity (types only, names redacted)\n";
    QMap<QString, int> byExt;
    for (const QString& p : recentPaths) {
        const QString ext = QFileInfo(p).suffix().toLower();
        byExt[ext.isEmpty() ? QString("(none)") : "." + ext] += 1;
    }
    for (auto it = byExt.constBegin(); it != byExt.constEnd(); ++it)
        h += "  " + it.key() + "   " + QString::number(it.value())
             + " file(s)\n";
    if (byExt.isEmpty()) h += "  (none recorded)\n";
    return h;
}

inline QString corpusCountLine(int rendered, int fetched, long total) {
    if (total < 0)
        return QString("showing %1 of %2 fetched; the total for this "
                       "query is <b>unknown</b> (the engine could not "
                       "count it)")
            .arg(rendered).arg(fetched);
    if (total > (long)fetched)
        return QString("showing %1 of %2 fetched, out of <b>%3</b> in "
                       "the corpus")
            .arg(rendered).arg(fetched)
            .arg(QLocale().toString((qlonglong)total));
    if (rendered < fetched)
        return QString("showing %1 of %2 hit(s)")
            .arg(rendered).arg(fetched);
    return QString("%1 hit(s)").arg(fetched);
}

inline QString goferSkipNotice(int skipped, int scanned) {
    if (skipped <= 0) return {};
    return QString("<div style='color:%1'><small>%2 file(s) in these "
                   "folders were <b>not searched</b> - this sweep read "
                   "%3 of them. A term that appears only in the rest "
                   "will show as no match here. Build the index "
                   "(Update search index) for a complete sweep."
                   "</small></div>")
        .arg(ux::kSoft)
        .arg(QLocale().toString(skipped))
        .arg(QLocale().toString(scanned));
}
