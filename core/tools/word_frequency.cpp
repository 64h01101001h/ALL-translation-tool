// word_frequency — frequency over the corpus under an EXPLICIT policy.
//
// WHY A POLICY AND NOT JUST A COUNT. docs/LEARN_TAB_VISION.md records that two
// independent passes over this database, using two defensible tokenizations,
// produced coverage figures about 2.5x apart — 199 against 380 items for 50%
// coverage, 3,266 against 9,101 for 95% — and rules that "no frequency-coverage
// figure may be printed as fact until a disambiguating segmenter with a
// documented tie-breaking policy exists and is battery-proven." The lattice
// cannot settle it: by design it keeps nested matches and makes no segmentation
// decision at all.
//
// THE POLICY, all three parts stated because all three move the number:
//   segmenter : SegTrie's maximal-match walk. The tie-break is longest-match,
//               and botok_tok_smoke proves SegTrie::segment == Segmenter::segment
//               word for word at corpus scale, so the ported path is the anchor.
//   lexicon   : NAMED ON THE COMMAND LINE, because it decides how long a
//               maximal match can get. Run both and compare; do not assume.
//   unit      : LEMMA. A learner memorises a word, not each of its affixed
//               shapes, and 21% of corpus tokens carry an affix. SegWord gives
//               affixType and affixAa, so the lemma is recovered exactly rather
//               than guessed.
//
// WHAT IT DOES NOT COUNT, and these belong beside every figure it prints:
//   - 6,259 of the 42,199 corpus segments do not render to Tibetan at all and
//     are absent from the input. A flagged rendering is not Tibetan to count.
//   - tokens the lexicon does not know are reported as `unmatched`, never
//     silently folded into a bucket.
//   - this is the ALIGNED CORPUS, which is a SUBSET of Geshe Michael's known
//     translations. Stage 1 of the TODO item asks what "all" means and it is
//     not yet answered, so no figure here describes his whole body of work.
//
//   segmenter : SegTrie's maximal-match walk, battery-proven identical to the
//               ported Tokenize (botok_tok_smoke).
//   lexicon   : the variable under test — it decides how long a maximal match
//               can get, so it moves the number.
//   unit      : LEMMA, not surface form. A learner memorises a word, not each
//               of its affixed shapes. SegWord::affixType names the affix the
//               trie matched, and affixAa says the host lost a final འ to take
//               it, so the lemma is recoverable exactly.
#include <allcore/botok.h>
#include <allcore/spine.h>
#include <allcore/engines.h>
#include <algorithm>
#include <cstdio>
#include <fstream>
#include <map>
#include <string>
#include <vector>

static const std::string TSHEG = "\xe0\xbc\x8b";   // U+0F0B
static const std::string ACHUNG = "\xe0\xbd\xa0";  // U+0F60

// strip the affix syllable the segmenter identified, restoring a dropped འ
static std::string lemmaOf(const allcore::botok::SegWord& w) {
    if (w.affixType.empty()) return w.text;
    std::string t = w.text;
    while (t.size() >= TSHEG.size() &&
           t.compare(t.size() - TSHEG.size(), TSHEG.size(), TSHEG) == 0)
        t.erase(t.size() - TSHEG.size());
    const size_t cut = t.rfind(TSHEG);
    if (cut == std::string::npos) return w.text;   // single syllable: leave it
    std::string base = t.substr(0, cut);
    if (w.affixAa) base += ACHUNG;
    return base + TSHEG;
}

int main(int argc, char** argv) {
    if (argc < 5) {
        std::fprintf(stderr, "usage: freq <data/botok> <spine.db> <corpus_tib.txt>"
                             " <dict|dict+monlam> [m1 m2]\n");
        return 2;
    }
    const std::string dataDir = argv[1], spinePath = argv[2],
                      corpus = argv[3], mode = argv[4];
    allcore::botok::SegTrie seg(dataDir);
    allcore::Spine spine(spinePath);
    long forms = 0;
    for (const auto& h : spine.allAcipHeadwords()) {
        auto r = allcore::wylieToUnicode(allcore::acipToEwts(h.acip));
        if (r.second && !r.first.empty()) { seg.addWord(r.first); ++forms; }
    }
    if (mode == "dict+monlam" && argc >= 7) {
        for (int i = 5; i <= 6; ++i) {
            std::ifstream f(argv[i]);
            std::string line;
            while (std::getline(f, line)) {
                auto tab = line.find('\t');
                std::string w = (tab == std::string::npos) ? line : line.substr(0, tab);
                while (!w.empty() && (w.back() == '\r' || w.back() == ' ')) w.pop_back();
                if (!w.empty()) { seg.addWord(w); ++forms; }
            }
        }
    }
    std::map<std::string, long> surf, lem;
    long tokens = 0, unmatched = 0, affixed = 0;
    {
        std::ifstream f(corpus);
        std::string line;
        while (std::getline(f, line)) {
            if (line.empty()) continue;
            for (const auto& w : seg.segment(line)) {
                if (!w.tibetan) continue;
                if (!w.word) { ++unmatched; continue; }
                ++tokens;
                ++surf[w.text];
                if (!w.affixType.empty()) ++affixed;
                ++lem[lemmaOf(w)];
            }
        }
    }
    auto curve = [&](const std::map<std::string, long>& m, long& c50, long& c95) {
        std::vector<long> v;
        long tot = 0;
        for (const auto& kv : m) { v.push_back(kv.second); tot += kv.second; }
        std::sort(v.begin(), v.end(), std::greater<long>());
        long cum = 0; c50 = c95 = 0;
        for (size_t i = 0; i < v.size(); ++i) {
            cum += v[i];
            if (!c50 && cum * 2 >= tot) c50 = (long)i + 1;
            if (!c95 && cum * 100 >= tot * 95) { c95 = (long)i + 1; break; }
        }
    };
    long s50, s95, l50, l95;
    curve(surf, s50, s95);
    curve(lem, l50, l95);
    std::printf("%-12s lexicon=%-8ld tokens=%-8ld unmatched=%-7ld affixed=%-7ld\n",
                mode.c_str(), forms, tokens, unmatched, affixed);
    std::printf("             surface: types=%-8zu 50%%=%-7ld 95%%=%ld\n",
                surf.size(), s50, s95);
    std::printf("             LEMMA  : types=%-8zu 50%%=%-7ld 95%%=%ld\n",
                lem.size(), l50, l95);
    // Adam's own numbers: what does a memorised core of 2,000 and 4,000 buy?
    {
        std::vector<long> v;
        long tot = 0;
        for (const auto& kv : lem) { v.push_back(kv.second); tot += kv.second; }
        std::sort(v.begin(), v.end(), std::greater<long>());
        for (long n2 : {500L, 1000L, 2000L, 4000L, 8000L}) {
            long cum = 0;
            for (long i = 0; i < n2 && i < (long)v.size(); ++i) cum += v[i];
            std::printf("             top %-6ld lemmas cover %5.1f%% of running text\n",
                        n2, tot ? 100.0 * cum / tot : 0.0);
        }
    }
    return 0;
}
