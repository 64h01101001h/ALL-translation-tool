// app/speak_out.inc — reading Tibetan aloud from the Kawachen recordings
// (2026-09-10). One place that turns a stretch of Tibetan into sound, and the
// only place in the app that opens an audio file.
//
// Compiled ONLY into a private build. -DALL_AUDIO=OFF removes it entirely,
// and a machine without Qt6::Multimedia builds the public product unaffected.
// Kawachen's permission covers in-house use, not redistribution.
//
// WHY DECODE RATHER THAN CHAIN. A syllable is 0.29-0.65s. Handing a sequence
// of them to QMediaPlayer one after another puts that player's start latency
// between every pair, which is audible and, worse, VARIABLE — the rhythm
// wanders. So each file is decoded to PCM once, cached, and the sequence is
// concatenated into a single buffer with a gap we choose. Playback is then one
// QAudioSink over one buffer, and the timing is exactly what was asked for.
//
// WHAT THIS IS. Syllable-accurate reading aloud in a real human voice — which
// is how Tibetan has always been taught. It is NOT a native speaker reading a
// sentence: each recording carries its own pitch contour and final release,
// and connected speech has a rise and fall across a phrase that separate
// atoms cannot supply. Every caller is handed the tier of every syllable so
// the interface can say which it is doing. Nothing here may imply fluency.
#pragma once

namespace speak {

#if defined(ALL_HAVE_AUDIO_OUT)

// The bank, opened once. Absent is the normal state of a machine that has not
// harvested the recordings, and is not an error.
// Resolved ONCE, to a pointer. The first version returned the developer
// fallback only while a `tried` flag was still false, so the very first call
// got the working bank and every call after it got the empty bundled one —
// audio played once per launch and then silently stopped. A one-shot branch
// that decides which of two objects to return has to store the decision, not
// the fact that it was made.
inline allcore::SpeakBank& bank() {
    static allcore::SpeakBank bundled(
        QDir(QCoreApplication::applicationDirPath())
            .absoluteFilePath("../Resources/kawachen")
            .toStdString());
    static allcore::SpeakBank dev("data/audio/kawachen");   // developer tree
    static allcore::SpeakBank* chosen =
        bundled.available() ? &bundled : &dev;
    return *chosen;
}

inline bool available() { return bank().available(); }

// One decoded recording. Kept as raw PCM in the format the sink was opened
// with, so concatenation is a memcpy and never a resample.
struct Decoded {
    QByteArray pcm;
    QAudioFormat fmt;
};

inline QHash<QString, Decoded>& cache() {
    static QHash<QString, Decoded> c;
    return c;
}

// Decode one file to PCM, synchronously. The files are tiny (4-10 KB) and the
// decoder is fast; doing this lazily on first use keeps startup free and the
// cache means a syllable is decoded at most once per session.
inline Decoded decodeOne(const QString& path) {
    if (cache().contains(path)) return cache().value(path);
    Decoded out;
    QAudioDecoder dec;
    dec.setSource(QUrl::fromLocalFile(path));
    QEventLoop loop;
    bool done = false, failed = false;
    QObject::connect(&dec, &QAudioDecoder::bufferReady, [&] {
        const QAudioBuffer b = dec.read();
        if (b.isValid()) {
            if (!out.fmt.isValid()) out.fmt = b.format();
            out.pcm.append(b.constData<char>(), b.byteCount());
        }
    });
    QObject::connect(&dec, &QAudioDecoder::finished, [&] { done = true; loop.quit(); });
    // QAudioDecoder has both an error() getter and an error(Error)
    // signal, so the overload has to be named explicitly.
    QObject::connect(&dec,
                     QOverload<QAudioDecoder::Error>::of(&QAudioDecoder::error),
                     [&](QAudioDecoder::Error) { failed = true; loop.quit(); });
    dec.start();
    QTimer::singleShot(3000, &loop, &QEventLoop::quit);   // never hang a click
    if (!done && !failed) loop.exec();
    if (failed) out = Decoded{};
    cache().insert(path, out);
    return out;
}

// What a passage will sound like, before anything is played: one entry per
// syllable with the tier that will be used. A surface shows this so a reader
// knows which syllables are exact and which are approximations, and which
// cannot be spoken at all.
inline std::vector<allcore::SpokenSyllable> plan(const QString& wylie) {
    return bank().readAloud(wylie.toStdString());
}

// Play a planned sequence. `gapMs` is silence between syllables — the reason
// this is a parameter and not a constant is that a beginner reading and a
// proofreader scanning want different tempos, and neither is "correct".
inline void play(const std::vector<allcore::SpokenSyllable>& seq, int gapMs = 60) {
    static QAudioSink* sink = nullptr;
    static QBuffer* buf = nullptr;
    if (sink) { sink->stop(); sink->deleteLater(); sink = nullptr; }
    if (buf) { buf->close(); buf->deleteLater(); buf = nullptr; }

    QByteArray all;
    QAudioFormat fmt;
    for (const auto& s : seq) {
        for (const std::string& f : s.files) {
            const Decoded d = decodeOne(QString::fromStdString(f));
            if (d.pcm.isEmpty()) continue;
            if (!fmt.isValid()) fmt = d.fmt;
            all.append(d.pcm);
        }
        if (gapMs > 0 && fmt.isValid())
            all.append(QByteArray(fmt.bytesForDuration(gapMs * 1000), '\0'));
    }
    if (all.isEmpty() || !fmt.isValid()) return;
    buf = new QBuffer;
    buf->setData(all);
    buf->open(QIODevice::ReadOnly);
    sink = new QAudioSink(fmt);
    sink->start(buf);
}

// The sentence a surface shows beside the button, so the claim is made once
// and identically everywhere rather than reworded per pane.
inline QString honestyLine(const std::vector<allcore::SpokenSyllable>& seq) {
    int exact = 0, approx = 0, none = 0;
    for (const auto& s : seq) {
        switch (s.tier) {
            case allcore::SpeakTier::Exact:
            case allcore::SpeakTier::SilentDropped:
            case allcore::SpeakTier::BaseEnding: ++exact; break;
            case allcore::SpeakTier::Reduced: ++approx; break;
            default: ++none; break;
        }
    }
    QString s = QString("%1 syllables read one at a time in a native voice "
                        "— not a fluent reading.").arg(exact);
    if (approx) s += QString("  %1 approximated.").arg(approx);
    if (none) s += QString("  %1 not held, and skipped.").arg(none);
    return s;
}

#else   // public build: no audio code at all

inline bool available() { return false; }

#endif

// ---- the one way in, from anywhere ---------------------------------------
// Adam, 2026-09-10: "highlight any string of tibetan text ... right click and
// have an option to pronounce". Written once and called from every surface
// that shows Tibetan, so the honesty line, the refusals and the wording
// cannot drift between panes.
//
// Three refusals, all silent by design — a menu that offers Pronounce and
// then cannot is worse than one that does not offer it:
//   * no audio in this build, or no recordings harvested → no entry at all;
//   * the selection holds no Tibetan → no entry;
//   * the bank cannot say a single syllable of it → the entry appears
//     DISABLED, with the reason in its tooltip, because "nothing happened"
//     is the one outcome a reader must never be left to interpret.
// TWO input shapes are accepted, and lowercase wylie is deliberately NOT one
// of them. The first attempt took "letters, spaces and an apostrophe" as
// wylie, and the selftest caught it offering to pronounce "hello world this
// is english prose" — every one of those letters is wylie-legal, so the
// inventory cannot tell the two apart and only real syllable parsing could.
// Rather than half-parse, the rule is a shape the reader can see:
//   * Tibetan script — what Adam actually asked for;
//   * ACIP, which is written UPPERCASE by convention and is what the Input
//     pane and the Draft source column hold.
// A lowercase Latin selection is refused. The cost is that someone who types
// wylie by hand gets no entry; the alternative was offering to pronounce
// English, which is the kind of confident wrongness this project exists to
// avoid.
inline bool isTibetanScript(const QString& sel) {
    for (QChar c : sel)
        if (c.unicode() >= 0x0F00 && c.unicode() <= 0x0FFF) return true;
    return false;
}

inline bool isAcip(const QString& sel) {
    bool anyLetter = false;
    for (QChar c : sel) {
        if (c.isLetter()) {
            if (!c.isUpper()) return false;   // one lowercase letter and it is not ACIP
            anyLetter = true;
        } else if (!c.isSpace() && !QString("'.,-()[]/*@\u2026").contains(c) &&
                   !c.isDigit()) {
            return false;
        }
    }
    return anyLetter;
}

inline QString wylieOf(const QString& sel) {
    if (isTibetanScript(sel))
        return QString::fromStdString(
            allcore::unicodeToWylie(sel.toStdString()).wylie);
    if (isAcip(sel))
        return QString::fromStdString(
            allcore::acipToEwts(sel.toStdString()));
    return QString();
}

inline bool looksTibetan(const QString& sel) {
    return isTibetanScript(sel) || isAcip(sel);
}

#if defined(ALL_HAVE_AUDIO_OUT)
inline QAction* addPronounce(QMenu* m, const QString& selRaw, QWidget* owner,
                             const std::function<void(const QString&)>& say = {}) {
    if (!m || !available()) return nullptr;
    const QString sel = selRaw.simplified();
    if (sel.isEmpty() || sel.size() > 400 || !looksTibetan(sel)) return nullptr;
    const QString wylie = wylieOf(sel);
    if (wylie.trimmed().isEmpty()) return nullptr;
    auto seq = bank().readAloud(wylie.toStdString());
    int sayable = 0;
    for (const auto& x : seq)
        if (x.tier != allcore::SpeakTier::None) ++sayable;
    m->addSeparator();
    QAction* a = m->addAction(QString("Pronounce \u201c%1\u201d")
                                  .arg(sel.left(24) +
                                       (sel.size() > 24 ? QString("\u2026")
                                                        : QString())));
    if (sayable == 0) {
        a->setEnabled(false);
        a->setToolTip("No recording is held for any syllable of this "
                      "selection. Nothing is approximated.");
        return a;
    }
    a->setToolTip(honestyLine(seq));
    QObject::connect(a, &QAction::triggered, owner, [seq, say] {
        play(seq);
        if (say) say(honestyLine(seq));
    });
    return a;
}
#else
inline QAction* addPronounce(QMenu*, const QString&, QWidget*,
                             const std::function<void(const QString&)>& = {}) {
    return nullptr;   // public build: the menu never mentions audio
}
#endif

}  // namespace speak
