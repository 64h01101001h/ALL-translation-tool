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
inline allcore::SpeakBank& bank() {
    static allcore::SpeakBank b(QDir(QCoreApplication::applicationDirPath())
                                    .absoluteFilePath("../Resources/kawachen")
                                    .toStdString());
    static bool tried = false;
    if (!tried) {
        tried = true;
        if (!b.available()) {   // developer tree: the repo copy
            static allcore::SpeakBank dev("data/audio/kawachen");
            if (dev.available()) return dev;
        }
    }
    return b;
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

}  // namespace speak
