// botok_tok_smoke — battery for the Botok trie + tokenizer port.
// References are GENERATED from the live Python botok oracle:
//   scenarios file — small tries driven through the upstream tests' exact
//     operations (affix inflection, add_data senses, deactivate, skrt,
//     non-max backtracking, pos-injection persistence across repeat runs);
//   corpus file — trie built from every distinct spine Tibetan string,
//     inputs = corpus segments converted by the canonical engine.
// This battery proves token-stream identity: text, type, offsets, syllable
// indices, senses, affixation, skrt — field for field.
#include <sqlite3.h>

#include <cstdio>
#include <fstream>
#include <memory>
#include <sstream>
#include <string>
#include <vector>

#include "allcore/botok.h"

using namespace allcore::botok;

static int failures = 0;
#define CHECK(cond, msg)                                        \
    do {                                                        \
        if (cond) std::printf("  [PASS] %s\n", msg);            \
        else { std::printf("  [FAIL] %s\n", msg); ++failures; } \
    } while (0)

static const char US = '\x1f';
static const char RS = '\x1e';

static std::string serToken(const BotokToken& t) {
    std::ostringstream o;
    o << "text=" << t.text << US << "type=" << t.chunkType << US
      << "start=" << t.start << US << "len=" << t.len << US;
    if (t.hasSyls) {
        o << "syls=";
        for (size_t i = 0; i < t.sylsIdx.size(); ++i) {
            if (i) o << ';';
            for (size_t j = 0; j < t.sylsIdx[i].size(); ++j) {
                if (j) o << ',';
                o << t.sylsIdx[i][j];
            }
        }
        o << US << "sse=";
        for (size_t i = 0; i < t.sylsStartEnd.size(); ++i) {
            if (i) o << ';';
            o << t.sylsStartEnd[i].first << '-' << t.sylsStartEnd[i].second;
        }
        o << US;
    } else {
        o << "syls=-" << US << "sse=-" << US;
    }
    o << "chars=";
    for (size_t i = 0; i < t.charTypes.size(); ++i) {
        if (i) o << ',';
        o << t.charTypes[i];
    }
    o << US;
    if (t.data.hasSenses) {
        o << "senses=";
        for (size_t i = 0; i < t.data.senses.size(); ++i) {
            if (i) o << ';';
            const Sense& m = t.data.senses[i];
            bool first = true;
            auto kv = [&](const char* k, const std::string& v) {
                if (!first) o << ',';
                first = false;
                o << k << ':' << v;
            };
            // alphabetical key order, matching the oracle's sorted(m.keys())
            if (m.affixed) kv("affixed", *m.affixed ? "True" : "False");
            if (m.freq) kv("freq", std::to_string(*m.freq));
            if (m.lemma) kv("lemma", *m.lemma);
            if (m.pos) kv("pos", *m.pos);
            if (m.sense) kv("sense", *m.sense);
        }
    } else {
        o << "senses=-";
    }
    o << US;
    if (t.data.hasAffixation) {
        o << "affix=len:" << t.data.affixation.len
          << ",type:" << t.data.affixation.type
          << ",aa:" << (t.data.affixation.aa ? "True" : "False");
    } else {
        o << "affix=-";
    }
    o << US << "skrt=" << (t.skrt ? "True" : "False") << US << "formfreq=";
    if (t.data.hasFormFreq) o << t.data.formFreq;
    else o << '-';
    return o.str();
}

static std::vector<std::string> tokenizeSer(const CharTable& table, Trie& trie,
                                            const std::string& input) {
    ChunkFramework cf(table, u8to32(input));
    auto chunks = cf.serveSylsToTrie();
    Tokenize tok(trie);
    std::vector<std::string> out;
    for (const BotokToken& t : tok.tokenize(cf, chunks))
        out.push_back(serToken(t));
    return out;
}

int main(int argc, char** argv) {
    if (argc < 3) {
        std::fprintf(stderr,
                     "usage: botok_tok_smoke <data/botok dir> <scenarios file> "
                     "[<spine.db> <corpus reference>]\n");
        return 2;
    }
    CharTable table(argv[1]);
    BoSyl bosyl(argv[1]);

    // spot checks from sylcomponents.py's own __main__ examples
    {
        auto p = bosyl.getParts(U"བཀྲིས");
        CHECK(p.kind == BoSyl::Parts::Single && p.root == U"བཀྲ" &&
              p.suffix == U"ིས", "get_parts(bkris) == (bkr, is)");
        CHECK(bosyl.getInfo(U"དེའིའམ") == U"thame", "get_info(de'i'am) thame");
        CHECK(bosyl.getInfo(U"དེའི") == U"thame", "get_info(de'i) thame");
        CHECK(bosyl.getAllAffixed(U"མཐའ").size() == 11,
              "mtha' affixable with 11 forms");
        CHECK(bosyl.getAllAffixed(U"མཐའ")[0].second.aa,
              "mtha' affixation records aa");
        CHECK(bosyl.getAllAffixed(U"ལེགས").empty(), "legs not affixable");
    }

    // ---- scenario battery ---------------------------------------------------
    {
        std::ifstream scen(argv[2]);
        CHECK(bool(scen), "scenarios file opened");
        std::string line;
        std::unique_ptr<Trie> trie;
        std::string scName;
        std::vector<std::string> got;
        std::vector<std::string> want;
        int inputNo = 0;
        bool allOk = true;
        int compared = 0;

        auto flush = [&]() {
            if (got.empty() && want.empty()) return;
            bool ok = got == want;
            if (!ok) {
                allOk = false;
                std::printf("    scenario '%s' input %d mismatch (%zu vs %zu tokens)\n",
                            scName.c_str(), inputNo, got.size(), want.size());
                for (size_t i = 0; i < got.size() || i < want.size(); ++i) {
                    if (i < got.size()) std::printf("      got  [%zu] %s\n", i, got[i].c_str());
                    if (i < want.size()) std::printf("      want [%zu] %s\n", i, want[i].c_str());
                }
            }
            ++compared;
            got.clear();
            want.clear();
        };

        while (std::getline(scen, line)) {
            if (line.rfind("#SC\t", 0) == 0) {
                flush();
                scName = line.substr(4);
                trie = std::make_unique<Trie>(table, bosyl);
                inputNo = 0;
            } else if (line.rfind("#OP ", 0) == 0) {
                flush();
                size_t tab = line.find('\t');
                std::string op = line.substr(4, tab - 4);
                std::string arg = line.substr(tab + 1);
                if (op == "mod") trie->inflectNModifyTrie(arg);
                else if (op == "mod_deactivate") trie->inflectNModifyTrie(arg, true);
                else if (op == "mod_skrt") trie->inflectNModifyTrie(arg, false, true);
                else if (op == "data") trie->inflectNAddData(arg);
                else if (op == "add_non_inflectible") trie->addNonInflectible(arg);
                else { std::printf("    unknown op %s\n", op.c_str()); allOk = false; }
            } else if (line.rfind("#IN\t", 0) == 0) {
                flush();
                ++inputNo;
                got = tokenizeSer(table, *trie, line.substr(4));
            } else if (line.rfind("#TK\t", 0) == 0) {
                want.push_back(line.substr(4));
            }
        }
        flush();
        std::printf("  scenario inputs compared: %d\n", compared);
        CHECK(allOk && compared >= 10, "scenario token streams identical to oracle");
    }

    // ---- Segmenter facade ---------------------------------------------------
    {
        Segmenter seg(argv[1]);
        seg.addWord("བཀྲ་ཤིས་");
        seg.addWord("བདེ་ལེགས་");
        seg.addWord("མཐའ་");
        // the compact trie must agree word-for-word (incl. affix info)
        SegTrie compact(argv[1]);
        compact.addWord("བཀྲ་ཤིས་");
        compact.addWord("བདེ་ལེགས་");
        compact.addWord("མཐའ་");
        const char* inp = "བཀྲ་ཤིས་བདེ་ལེགས། ཀཀ abc མཐའི་མཐའ།";
        CHECK(seg.segment(inp) == compact.segment(inp),
              "SegTrie == ported Segmenter on the facade scenario");
        auto cw = compact.segment(inp);
        CHECK(cw.size() == 8 && cw[5].affixType == "gi" && cw[5].affixAa,
              "SegTrie affix info: mtha'i = gi affix with aa");
        auto words = seg.segment("བཀྲ་ཤིས་བདེ་ལེགས། ཀཀ abc མཐའི་མཐའ།");
        CHECK(words.size() == 8, "segmenter: token count");
        auto is = [&](size_t i, const char* text, bool tib, bool word) {
            return i < words.size() && words[i].text == text &&
                   words[i].tibetan == tib && words[i].word == word;
        };
        CHECK(is(0, "བཀྲ་ཤིས་", true, true), "segmenter: word 1 matched");
        CHECK(is(1, "བདེ་ལེགས", true, true), "segmenter: word 2 matched");
        CHECK(is(2, "། ", false, false), "segmenter: punct not a word");
        CHECK(is(3, "ཀཀ ", true, false), "segmenter: OOV marked non-word");
        CHECK(is(4, "abc ", false, false), "segmenter: latin not tibetan");
        CHECK(is(5, "མཐའི་", true, true), "segmenter: affixed form matched");
        CHECK(is(6, "མཐའ", true, true), "segmenter: base form matched");
        CHECK(is(7, "།", false, false), "segmenter: final shad");
    }

    // ---- corpus-scale battery ----------------------------------------------
    if (argc >= 5) {
        {  // scoped: the big ported trie frees before the equivalence pass
        Trie trie(table, bosyl);
        sqlite3* db = nullptr;
        CHECK(sqlite3_open_v2(argv[3], &db, SQLITE_OPEN_READONLY, nullptr) ==
                  SQLITE_OK, "spine opened");
        sqlite3_stmt* s = nullptr;
        sqlite3_prepare_v2(db,
                           "SELECT DISTINCT tibetan FROM entries WHERE tibetan "
                           "IS NOT NULL AND tibetan != '' ORDER BY tibetan",
                           -1, &s, nullptr);
        long words = 0;
        while (sqlite3_step(s) == SQLITE_ROW) {
            trie.inflectNModifyTrie(
                reinterpret_cast<const char*>(sqlite3_column_text(s, 0)));
            ++words;
        }
        sqlite3_finalize(s);
        sqlite3_close(db);
        std::printf("  trie built from %ld spine words\n", words);
        CHECK(words > 100000, "corpus trie built");

        std::ifstream ref(argv[4]);
        CHECK(bool(ref), "corpus reference opened");
        std::string line;
        long total = 0, match = 0, shown = 0;
        while (std::getline(ref, line)) {
            size_t tab = line.find('\t');
            if (tab == std::string::npos) continue;
            std::string input = line.substr(0, tab);
            std::string wantJoined = line.substr(tab + 1);
            auto toks = tokenizeSer(table, trie, input);
            std::string gotJoined;
            for (size_t i = 0; i < toks.size(); ++i) {
                if (i) gotJoined += RS;
                gotJoined += toks[i];
            }
            ++total;
            if (gotJoined == wantJoined) ++match;
            else if (shown++ < 3)
                std::printf("    corpus diff on: %.60s...\n      got  %.200s\n      want %.200s\n",
                            input.c_str(), gotJoined.c_str(), wantJoined.c_str());
        }
        std::printf("  corpus token streams: %ld/%ld = %.3f%%\n", match, total,
                    total ? 100.0 * match / total : 0.0);
        CHECK(total > 0 && match == total,
              "corpus-scale token streams identical to oracle");
        }  // ported trie freed

        // ---- SegTrie corpus equivalence: compact path == ported path ------
        {
            Segmenter ported(argv[1]);
            SegTrie compact(argv[1]);
            sqlite3* db = nullptr;
            sqlite3_open_v2(argv[3], &db, SQLITE_OPEN_READONLY, nullptr);
            sqlite3_stmt* s = nullptr;
            sqlite3_prepare_v2(
                db,
                "SELECT DISTINCT tibetan FROM entries WHERE tibetan IS NOT "
                "NULL AND tibetan != '' ORDER BY tibetan",
                -1, &s, nullptr);
            while (sqlite3_step(s) == SQLITE_ROW) {
                const char* w =
                    reinterpret_cast<const char*>(sqlite3_column_text(s, 0));
                ported.addWord(w);
                compact.addWord(w);
            }
            sqlite3_finalize(s);
            sqlite3_close(db);
            std::printf("  SegTrie: %zu words, %zu nodes, %zu edges "
                        "(~%.0f MB flat)\n",
                        compact.wordCount(), compact.nodeCount(),
                        compact.edgeCount(),
                        (compact.nodeCount() * 2.0 +
                         compact.edgeCount() * 24.0) / 1e6);

            std::ifstream ref(argv[4]);
            std::string line;
            long total = 0, same = 0, shown = 0;
            while (std::getline(ref, line)) {
                size_t tab = line.find('\t');
                if (tab == std::string::npos) continue;
                std::string input = line.substr(0, tab);
                auto a = ported.segment(input);
                auto b = compact.segment(input);
                ++total;
                if (a == b) ++same;
                else if (shown++ < 3) {
                    std::printf("    seg diff on: %.60s...\n", input.c_str());
                    for (size_t i = 0; i < a.size() || i < b.size(); ++i) {
                        if (i < a.size())
                            std::printf("      ported [%zu] '%s' t%d w%d %s\n",
                                        i, a[i].text.c_str(), a[i].tibetan,
                                        a[i].word, a[i].affixType.c_str());
                        if (i < b.size())
                            std::printf("      compact[%zu] '%s' t%d w%d %s\n",
                                        i, b[i].text.c_str(), b[i].tibetan,
                                        b[i].word, b[i].affixType.c_str());
                    }
                }
            }
            std::printf("  SegTrie corpus equivalence: %ld/%ld = %.3f%%\n",
                        same, total, total ? 100.0 * same / total : 0.0);
            CHECK(total > 0 && same == total,
                  "SegTrie segmentation identical to the ported path at "
                  "corpus scale");
        }
    }

    std::printf("botok_tok_smoke: %s (%d failures)\n",
                failures ? "FAIL" : "ALL PASS", failures);
    return failures ? 1 : 0;
}
