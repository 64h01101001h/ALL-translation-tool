// allanalyze — headless Analysis pipeline: passage file → engine pre-pass →
// Claude API (streaming SSE) → markdown report on stdout.
//
// Usage: allanalyze <spine.db> <template.md> <passage.txt> [draft.txt]
// Auth:  ANTHROPIC_API_KEY environment variable (never stored by the app).
#include <QCoreApplication>
#include <QFile>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QTextStream>

#include <cstdio>
#include <string>

#include "allcore/analysis.h"
#include "allcore/qc.h"
#include "allcore/spine.h"

static QString readAll(const QString& path) {
    QFile f(path);
    if (!f.open(QIODevice::ReadOnly | QIODevice::Text)) {
        std::fprintf(stderr, "cannot read %s\n", qPrintable(path));
        std::exit(2);
    }
    return QString::fromUtf8(f.readAll());
}

int main(int argc, char** argv) {
    QCoreApplication app(argc, argv);
    if (argc < 4) {
        std::fprintf(stderr,
                     "usage: allanalyze <spine.db> <template.md> <passage.txt> [draft.txt]\n");
        return 2;
    }
    const QByteArray key = qgetenv("ANTHROPIC_API_KEY");
    if (key.isEmpty()) {
        std::fprintf(stderr, "ANTHROPIC_API_KEY is not set\n");
        return 2;
    }

    const std::string passage = readAll(argv[3]).toStdString();
    const std::string draft = argc > 4 ? readAll(argv[4]).toStdString() : std::string();

    allcore::Spine spine(argv[1]);
    auto pre = allcore::runPrePass(spine, passage);
    std::fprintf(stderr, "[engine] %zu anchors, %zu corpus hits\n", pre.anchors.size(),
                 pre.corpus_hits.size());
    auto prompt = allcore::buildAnalysisPrompt(argv[2], pre, passage, draft);
    const std::string body = allcore::buildMessagesRequestJson(prompt);

    QNetworkRequest req(QUrl("https://api.anthropic.com/v1/messages"));
    req.setRawHeader("x-api-key", key);
    req.setRawHeader("anthropic-version", "2023-06-01");
    req.setRawHeader("anthropic-beta", "server-side-fallback-2026-07-01");
    req.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

    QNetworkAccessManager net;
    QNetworkReply* reply = net.post(req, QByteArray::fromStdString(body));

    // Minimal SSE consumer: emit text deltas as they arrive; surface refusal
    // and error events; everything else is ignored.
    QByteArray buf;
    std::string accum;
    QTextStream out(stdout);
    QObject::connect(reply, &QNetworkReply::readyRead, [&] {
        buf += reply->readAll();
        int nl;
        while ((nl = buf.indexOf('\n')) >= 0) {
            const QByteArray line = buf.left(nl).trimmed();
            buf.remove(0, nl + 1);
            if (!line.startsWith("data:")) continue;
            const QByteArray data = line.mid(5).trimmed();
            const std::string t = allcore::sseTextDelta(data.toStdString());
            if (!t.empty()) {
                accum += t;
                out << QString::fromStdString(t);
                out.flush();
            } else if (data.contains("\"stop_reason\":\"refusal\"")) {
                std::fprintf(stderr,
                             "\n[refusal] the safety classifiers declined this request "
                             "and no fallback served it\n");
            } else if (data.contains("\"type\":\"error\"")) {
                std::fprintf(stderr, "\n[api error] %s\n", data.constData());
            }
        }
    });
    QObject::connect(reply, &QNetworkReply::finished, [&] {
        if (reply->error() != QNetworkReply::NoError)
            std::fprintf(stderr, "\n[network error] %s\n",
                         qPrintable(reply->errorString()));
        out << "\n";
        out.flush();
        if (!accum.empty()) {
            auto findings = allcore::validateReport(spine, accum, passage);
            std::fprintf(stderr, "\n%s",
                         allcore::findingsToMarkdown(findings).c_str());
        }
        app.exit(reply->error() == QNetworkReply::NoError ? 0 : 1);
    });
    return app.exec();
}
