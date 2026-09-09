// textspan_smoke — the shared-substrate battery (analysis-suite F0).
//
// Fixture-free by design: the module it covers is the one every study
// surface depends on, so it must be provable on a clean checkout under
// `ctest -LE fixture`. The numbering follows the plan's acceptance tests
// 1-13; the X-checks below them cover the parts of the deliverable the
// plan did not number (provenanceBlock, and byte offsets over Tibetan
// unicode, which is where a wrong offset actually bites).
#include <cctype>
#include <cstdio>
#include <string>
#include <vector>

#include "allcore/provenance.h"
#include "allcore/textspan.h"

using namespace allcore::textspan;
using allcore::Provenance;
using allcore::provenanceBlock;

static int failures = 0;
#define CHECK(cond, msg)                                        \
    do {                                                        \
        if (cond) std::printf("  [PASS] %s\n", msg);            \
        else { std::printf("  [FAIL] %s\n", msg); ++failures; } \
    } while (0)

static std::vector<std::string> texts(const std::vector<Syllable>& v) {
    std::vector<std::string> o;
    for (const auto& s : v) o.push_back(s.text);
    return o;
}
static bool same(const std::vector<std::string>& a, const std::vector<const char*>& b) {
    if (a.size() != b.size()) return false;
    for (size_t i = 0; i < a.size(); ++i) if (a[i] != b[i]) return false;
    return true;
}
// the tokenizer's own edge rule, for the T2 round trip
static std::string foldEdges(const std::string& in) {
    size_t b = 0, e = in.size();
    while (b < e && !std::isalnum((unsigned char)in[b]) && in[b] != '\'') ++b;
    while (e > b && !std::isalnum((unsigned char)in[e - 1]) && in[e - 1] != '\'') --e;
    std::string s = in.substr(b, e - b);
    for (auto& c : s) if (c >= 'A' && c <= 'Z') c = (char)(c - 'A' + 'a');
    return s;
}
static std::string render(const FolioLoc& f) {
    return f.folio.empty() ? "line " + std::to_string(f.lineInFolio)
                           : "@" + f.folio + "." + std::to_string(f.lineInFolio);
}
static bool has(const std::string& hay, const char* needle) {
    return hay.find(needle) != std::string::npos;
}

// ⟨ ⟩ as bytes, so this file needs no assumption about its own encoding
static const char* kAngleOpen = "\xe2\x9f\xa8";
static const char* kAngleClose = "\xe2\x9f\xa9";

int main() {
    setvbuf(stdout, nullptr, _IONBF, 0);

    // 1. the promoted tokenizer, unchanged
    {
        const auto v = syllablesWylie("@012a SANGS RGYAS , chos 123 pa'i");
        CHECK(same(v, {"012a", "sangs", "rgyas", "chos", "pa'i"}),
              "T1 syllablesWylie: split at ',', case folded, apostrophe kept, "
              "all-digit '123' dropped — and '@012a' survives as '012a' "
              "(the '@' test runs after the edge strip has removed the '@'; "
              "PROMOTED behaviour, pinned here rather than silently changed)");
        CHECK(same(syllablesWylie("@123 ka"), {"ka"}),
              "T1b a digits-only marker IS dropped, by the all-digit rule");
    }

    // 2. offsets are real offsets
    {
        const std::string t = "@001A THAMS CAD , sems can [sic] la ,\nDE LTAR ro ,\n";
        const auto v = syllableSpans(t);
        bool roundTrip = true, increasing = true;
        size_t prev = 0; bool first = true;
        for (const auto& s : v) {
            if (foldEdges(t.substr(s.begByte, s.endByte - s.begByte)) != s.text) roundTrip = false;
            if (!first && s.begByte <= prev) increasing = false;
            prev = s.begByte; first = false;
        }
        CHECK(!v.empty() && roundTrip, "T2 every span's own bytes fold back to its text");
        CHECK(increasing, "T2b offsets are strictly increasing");
    }

    // 3. lines and barriers
    {
        const auto v = syllableSpans("a b , c d\ne");
        bool lines = v.size() == 5 && v[0].line == 1 && v[1].line == 1 &&
                     v[2].line == 1 && v[3].line == 1 && v[4].line == 2;
        bool bars = v.size() == 5 && !v[0].barrierAfter && v[1].barrierAfter &&
                    !v[2].barrierAfter && v[3].barrierAfter && !v[4].barrierAfter;
        CHECK(same(texts(v), {"a", "b", "c", "d", "e"}) && lines, "T3 lines are 1,1,1,1,2");
        CHECK(bars, "T3b barrierAfter on the syllable before ',' and before the "
                    "line break, and nowhere else (an unterminated last line is "
                    "not a barrier)");
    }

    // 4. StripSpan, a mark standing on its own
    {
        Provenance p;
        const auto v = syllableSpans("chos [kyi] nyid", Apparatus::StripSpan, &p);
        CHECK(same(texts(v), {"chos", "nyid"}) && p.apparatus_marks_stripped == 1 &&
                  p.units_refused == 0 && p.units_read == 2 && p.apparatus == std::string("strip-span"),
              "T4 StripSpan removes a whole [ ] span and records it");
    }

    // 5. StripSpan, a mark inside a syllable: refused, never truncated
    {
        Provenance p;
        const auto v = syllableSpans("cho[s] nyid", Apparatus::StripSpan, &p);
        const bool named = p.refusals.size() == 1 && has(p.refusals[0], "line 1") &&
                           has(p.refusals[0], "cho[s]");
        bool noStump = true;
        for (const auto& s : v) if (s.text == "cho") noStump = false;
        CHECK(same(texts(v), {"nyid"}) && p.units_refused == 1 && named && noStump,
              "T5 an intra-syllable mark refuses the syllable, names it by line "
              "and raw form, and never leaves a 'cho' stump");
    }

    // 6. KeepInner reads the supplied letters
    {
        Provenance p;
        const auto v = syllableSpans("cho[s] nyid", Apparatus::KeepInner, &p);
        CHECK(same(texts(v), {"chos", "nyid"}) && p.units_refused == 0 &&
                  p.apparatus == std::string("keep-inner"),
              "T6 KeepInner drops only the delimiters");
    }

    // 7. AsWritten counts the transcription as it stands
    {
        Provenance p;
        const auto v = syllableSpans("cho[s] nyid", Apparatus::AsWritten, &p);
        CHECK(same(texts(v), {"cho[s", "nyid"}) && p.apparatus_marks_stripped == 0 &&
                  p.units_refused == 0 && p.apparatus == std::string("as-written"),
              "T7 AsWritten leaves the marked form as its own token ('cho[s', the "
              "edge strip having taken the ']') and strips nothing");
    }

    // 8. the other bracket kinds, and the round brackets that are not apparatus
    {
        const char* attached[] = {"{x}chos nyid", "cho{x}s nyid", "chos{x} nyid"};
        bool allRefused = true;
        for (const char* t : attached) {
            Provenance p;
            const auto v = syllableSpans(t, Apparatus::StripSpan, &p);
            if (!(same(texts(v), {"nyid"}) && p.units_refused == 1)) allRefused = false;
        }
        CHECK(allRefused, "T8 { } word-initial, medial and final are refused like [ ]");
        Provenance ps;
        const auto vs = syllableSpans("chos {kyi} nyid", Apparatus::StripSpan, &ps);
        CHECK(same(texts(vs), {"chos", "nyid"}) && ps.apparatus_marks_stripped == 1,
              "T8b a free-standing { } span is stripped like [ ]");
        const std::string ang = std::string("chos ") + kAngleOpen + "kyi" + kAngleClose + " nyid";
        const std::string angIn = std::string("cho") + kAngleOpen + "s" + kAngleClose + " nyid";
        Provenance pa, pb;
        const auto va = syllableSpans(ang, Apparatus::StripSpan, &pa);
        const auto vb = syllableSpans(angIn, Apparatus::StripSpan, &pb);
        CHECK(same(texts(va), {"chos", "nyid"}) && pa.apparatus_marks_stripped == 1 &&
                  same(texts(vb), {"nyid"}) && pb.units_refused == 1,
              "T8c the three-byte ⟨ ⟩ pair behaves as (4) free-standing and as (5) attached");
        bool parensKept = true;
        for (Apparatus m : {Apparatus::StripSpan, Apparatus::KeepInner, Apparatus::AsWritten}) {
            Provenance p;
            const auto v = syllableSpans("chos (kyi) nyid", m, &p);
            if (!(same(texts(v), {"chos", "kyi", "nyid"}) && p.apparatus_marks_stripped == 0 &&
                  p.units_refused == 0)) parensKept = false;
        }
        CHECK(parensKept && countParenSpans("chos (kyi) nyid") == 1,
              "T8d ( ) is not house apparatus: retained as text under every mode, "
              "and counted separately for the report");
    }

    // 9. the two folio functions, and their one deliberate difference
    {
        std::string t;
        for (int f = 1; f <= 5; ++f)
            for (const char* side : {"A", "B"}) {
                char buf[16];
                std::snprintf(buf, sizeof buf, "@%03d%s\n", f, side);
                t += buf;
                t += "ka kha\nga nga\n";
            }
        CHECK(countFolioMarkers(t) == 10, "T9 the fixture carries ten well-formed markers");
        bool agree = true;
        std::string firstDisagreement;
        for (size_t off = 0; off < t.size(); ++off)
            if (citeAt(t, off) != render(folioAt(t, off))) {
                agree = false;
                if (firstDisagreement.empty())
                    firstDisagreement = citeAt(t, off) + " vs " + render(folioAt(t, off));
            }
        CHECK(agree, "T9b citeAt and folioAt agree at every byte offset of a "
                     "well-formed text");
        if (!agree) std::printf("        first disagreement: %s\n", firstDisagreement.c_str());
        const std::string bad = "@012\nx\ny\n";
        CHECK(citeAt(bad, 8) == "@012.3" && render(folioAt(bad, 8)) == "line 3",
              "T9c the ONE deliberate difference: on '@012' with no side letter the "
              "lenient citeAt says '@012.3' and the strict folioAt says 'line 3' — "
              "neither invents a folio");
    }

    // 10. a document whose middle line will not convert
    {
        // U+0F72 standing alone is a vowel sign with nothing to sit on:
        // unicodeToWylie warns, so the line is never called converted
        const std::string t = "KA KHA GA\n\xe0\xbd\xb2\nsems can\n";
        const auto c = toEwtsDocument(t);
        CHECK(c.unconvertedLines == 1 && c.unconvertedLineNumbers.size() == 1 &&
                  c.unconvertedLineNumbers[0] == 2 && has(c.wylie, "\xe0\xbd\xb2"),
              "T10 toEwtsDocument lists the line it could not convert (line 2) and "
              "keeps it as written instead of guessing");
        bool ok = true;
        toEwtsLine("\xe0\xbd\xb2", &ok);
        bool ok2 = true;
        const std::string acip = toEwtsLine("SEMS CAN", &ok2);
        CHECK(!ok && ok2 && acip == "sems can",
              "T10b toEwtsLine: ok=false on a warned unicode line, ACIP routed "
              "through acipToEwts");
    }

    // 11. citeAt at a byte offset
    {
        CHECK(citeAt("@012A\nx\ny\n", 8) == "@012A.3", "T11 citeAt: @012A.3");
        CHECK(citeAt("q\nx\ny\n", 4) == "line 3",
              "T11b no marker precedes the offset → 'line 3', never an invented folio");
    }

    // 12. ranges and the Document Properties counts
    {
        const auto r = folioRange("@001A ka kha @014B ga nga");
        CHECK(r.first == "001A" && r.second == "014B", "T12 folioRange: 001A … 014B");
        // the ACIP fixture, and the two numbers docprops::textStatistics
        // produces for it: markers by kFolioMarkerPattern, shads by counting
        // ',' after @-tokens and [ ] / { } spans are blanked
        const std::string acip =
            "@001A THAMS CAD , SEMS [CAN] LA ,\n@001B DE LTAR , {sic} RO ,\n";
        CHECK(countFolioMarkers(acip) == 2 && countShads(acip) == 4,
              "T12b countFolioMarkers/countShads reproduce Document Properties' "
              "own rule on an ACIP fixture (2 folios, 4 shads)");
        CHECK(countShads("KA , [ , ] KHA ,") == 2,
              "T12c a shad inside an apparatus span is not the scribe's shad");
    }

    // 13. the regression statement
    {
        // The real proof is the other batteries: quotation_smoke and
        // textdiff_smoke must pass UNCHANGED after the promotion, and a red
        // one means the promotion was not faithful. What this file can assert
        // without a fixture is that the two entry points ARE one tokenizer.
        const char* corpusish[] = {
            "byang chub sems dpa' sems dpa' chen po",
            "de nas bcom ldan 'das kyis , tshe dang ldan pa rab 'byor la",
            "gang zhig stong pa nyid la mos pa de ni sangs rgyas kyi bstan pa la mos pa'o",
        };
        bool agree = true;
        for (const char* w : corpusish) {
            const auto a = syllablesWylie(w);
            if (texts(syllableSpans(w, Apparatus::AsWritten)) != a) agree = false;
            if (texts(syllableSpans(w, Apparatus::StripSpan)) != a) agree = false;
        }
        CHECK(agree, "T13 syllableSpans reproduces syllablesWylie exactly on "
                     "corpus-shaped wylie — one tokenizer, two entry points");
        CHECK(same(syllablesWylie("byang chub sems dpa'"), {"byang", "chub", "sems", "dpa'"}) &&
                  same(syllablesWylie("BYANG CHUB , SEMS DPA'"),
                       {"byang", "chub", "sems", "dpa'"}),
              "T13b known quotation-side tokenizations are unchanged by the move");
    }

    // X1. the report header
    {
        Provenance p;
        p.engine = "textstats/1"; p.built_utc = "2026-09-09T00:00:00Z";
        p.source = "KL0001.ACT"; p.script_in = "acip"; p.norm = "verbatim";
        syllableSpans("cho[s] nyid", Apparatus::StripSpan, &p);
        const std::string b = provenanceBlock(p);
        CHECK(has(b, "engine: textstats/1\n") && has(b, "apparatus: strip-span\n") &&
                  has(b, "syllables read: 1\n") && has(b, "syllables refused: 1\n") &&
                  has(b, "source sha256: not recorded\n") && has(b, "cho[s]"),
              "X1 provenanceBlock prints one field per line, says 'not recorded' "
              "for an unset field, and lists the refusal");
        Provenance q;
        CHECK(has(provenanceBlock(q), "refusals: none\n"),
              "X1b an empty refusal list prints 'none' — a result, stated");
    }

    // X2. byte offsets over Tibetan unicode (three bytes to the codepoint)
    {
        // "@012A\n" then two Tibetan lines; the offset is inside the third line
        const std::string t = "@012A\n\xe0\xbd\x82\xe0\xbc\x8b\n\xe0\xbd\x84\xe0\xbc\x8b\n";
        CHECK(citeAt(t, t.size() - 2) == "@012A.3" && folioAt(t, t.size() - 2).lineInFolio == 3,
              "X2 a byte offset inside a Tibetan line cites the right line "
              "(a codepoint is three bytes; an unconverted offset cites the wrong one)");
        const auto f = folioAt(t, 0);
        CHECK(f.folio == "012A" && f.linesInFolio == 3,
              "X2b folioAt reports how many lines the folio holds");
    }

    // X3. brackets that do not close: read as written, and SAID
    {
        Provenance p;
        const auto v = syllableSpans("chos [kyi nyid", Apparatus::StripSpan, &p);
        CHECK(same(texts(v), {"chos", "kyi", "nyid"}) && p.units_refused == 0 &&
                  p.refusals.size() == 1 && has(p.refusals[0], "line 1") &&
                  has(p.refusals[0], "unbalanced"),
              "X3 half a bracket is not a span: the line is read as written, no "
              "syllable is dropped, and the header says the line was doubtful");
        Provenance q;
        syllableSpans("chos [a{b}c] nyid", Apparatus::StripSpan, &q);
        CHECK(q.refusals.size() == 1 && q.units_refused == 0,
              "X3b a nested span is not guessed at either");
    }

    std::printf("textspan_smoke: %d failure(s)\n", failures);
    return failures ? 1 : 0;
}
