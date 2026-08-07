// botok_smoke — battery for the Botok chunker port (allcore/botok.h).
// Expected values in botok_expected.h are GENERATED from the live Python
// botok oracle (v0.9.0) over botok's own chunk-test inputs; the upstream
// pytest suite (tests/chunks, 13 tests) was run green first, so oracle ==
// upstream-documented behavior. This battery proves C++ == oracle.
#include <cstdio>
#include <cstring>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>

#include "allcore/botok.h"
#include "botok_expected.h"

using namespace allcore::botok;
namespace ex = botok_expected;

static int failures = 0;
#define CHECK(cond, msg)                                        \
    do {                                                        \
        if (cond) std::printf("  [PASS] %s\n", msg);            \
        else { std::printf("  [FAIL] %s\n", msg); ++failures; } \
    } while (0)

static bool samePairs(const char* label,
                      const std::vector<std::pair<std::string, std::string>>& got,
                      const ex::Exp* want, int wantN) {
    bool ok = static_cast<int>(got.size()) == wantN;
    for (int i = 0; ok && i < wantN; ++i)
        ok = got[i].first == want[i].mark && got[i].second == want[i].text;
    if (!ok) {
        std::printf("    %s mismatch: got %zu chunks (want %d)\n", label,
                    got.size(), wantN);
        for (size_t i = 0; i < got.size(); ++i)
            std::printf("      got  [%zu] %s '%s'\n", i, got[i].first.c_str(),
                        got[i].second.c_str());
        for (int i = 0; i < wantN; ++i)
            std::printf("      want [%d] %s '%s'\n", i, want[i].mark, want[i].text);
    }
    return ok;
}

static std::vector<int> parseIdxs(const char* s) {
    std::vector<int> out;
    std::stringstream ss(s);
    std::string tok;
    while (std::getline(ss, tok, ','))
        if (!tok.empty()) out.push_back(std::stoi(tok));
    return out;
}

int main(int argc, char** argv) {
    if (argc < 2) {
        std::fprintf(stderr, "usage: botok_smoke <data/botok dir>\n");
        return 2;
    }
    CharTable table(argv[1]);
    std::printf("  char table: %zu Tibetan-block entries\n", table.tableSize());
    CHECK(table.tableSize() >= 200, "char table loaded");

    // category spot checks, incl. the preserved upstream quirks
    CHECK(table.category(U'་') == TSEK, "category: tsek");
    CHECK(table.category(U'བ') == CONS, "category: cons");
    CHECK(table.category(U' ') == TRANSPARENT, "category: space transparent");
    CHECK(table.category(U'这') == CJK, "category: CJK han");
    CHECK(table.category(U'T') == LATIN, "category: latin");
    CHECK(table.category(U'ก') == OTHER, "category: thai = OTHER");
    // [UPSTREAM QUIRK] locked in: the Python string-comparison artifact makes
    // U+2014 EM DASH "CJK", and true supplementary CJK (U+20000) "OTHER".
    CHECK(table.category(static_cast<char32_t>(0x2014)) == CJK,
          "quirk: em dash classified CJK (upstream artifact preserved)");
    CHECK(table.category(static_cast<char32_t>(0x20000)) == OTHER,
          "quirk: U+20000 falls to OTHER (upstream artifact preserved)");

    struct Case {
        const char* name;
        const char* input;
        const ex::Exp* want;
        int wantN;
        int which;  // 0 bo, 1 punct, 2 sym, 3 num, 4 space, 5 syllabify, 6 latin, 7 cjk
    };
    const Case cases[] = {
        {"bo/non-bo", ex::kBoNonboInput, ex::kBoNonbo, ex::kBoNonbo_n, 0},
        {"punct", ex::kPunctNonpunctInput, ex::kPunctNonpunct, ex::kPunctNonpunct_n, 1},
        {"sym", ex::kSymNonsymInput, ex::kSymNonsym, ex::kSymNonsym_n, 2},
        {"num", ex::kNumNonnumInput, ex::kNumNonnum, ex::kNumNonnum_n, 3},
        {"space", ex::kSpaceNonspaceInput, ex::kSpaceNonspace, ex::kSpaceNonspace_n, 4},
        {"syllabify", ex::kTextInput, ex::kText, ex::kText_n, 5},
        {"latin", ex::kLatinInput, ex::kLatin, ex::kLatin_n, 6},
        {"cjk", ex::kCjkInput, ex::kCjk, ex::kCjk_n, 7},
        {"other", ex::kOtherInput, ex::kOther, ex::kOther_n, 0},
    };
    for (const Case& c : cases) {
        ChunkFramework cf(table, u8to32(c.input));
        std::vector<Chunk> chunks;
        switch (c.which) {
            case 0: chunks = cf.chunkBoChars(); break;
            case 1: chunks = cf.chunkPunct(); break;
            case 2: chunks = cf.chunkSymbol(); break;
            case 3: chunks = cf.chunkNumber(); break;
            case 4: chunks = cf.chunkSpaces(); break;
            case 5: chunks = cf.syllabify(); break;
            case 6: chunks = cf.chunkLatin(); break;
            case 7: chunks = cf.chunkCjk(); break;
        }
        CHECK(samePairs(c.name, cf.readable(chunks), c.want, c.wantN),
              (std::string("chunker: ") + c.name).c_str());
    }

    // the staged full example (pipe_chunk + clean_chunks after every stage)
    {
        ChunkFramework cf(table, u8to32(ex::kFullInput));
        auto chunks = cf.cleanChunks(cf.chunkBoChars());
        CHECK(samePairs("full stage1", cf.readable(chunks), ex::kFullStage1,
                        ex::kFullStage1_n), "full example: bo/other");
        struct Stage {
            const char* name;
            int marker, yes;
            const ex::Exp* want;
            int wantN;
        };
        const Stage stages[] = {
            {"punct", BO, PUNCT, ex::kFullStage2, ex::kFullStage2_n},
            {"num", BO, NUM, ex::kFullStage3, ex::kFullStage3_n},
            {"sym", BO, SYM, ex::kFullStage4, ex::kFullStage4_n},
            {"text", BO, TEXT, ex::kFullStage5, ex::kFullStage5_n},
            {"cjk", OTHER_C, CJK_C, ex::kFullStage6, ex::kFullStage6_n},
            {"latin", OTHER_C, LATIN_C, ex::kFullStage7, ex::kFullStage7_n},
        };
        for (const Stage& st : stages) {
            ChunkFramework::PipedFn fn;
            if (std::strcmp(st.name, "punct") == 0)
                fn = [&](int s, int e, int y) { return cf.chunkPunct(s, e, y, NON_PUNCT); };
            else if (std::strcmp(st.name, "num") == 0)
                fn = [&](int s, int e, int y) { return cf.chunkNumber(s, e, y, NON_NUM); };
            else if (std::strcmp(st.name, "sym") == 0)
                fn = [&](int s, int e, int y) { return cf.chunkSymbol(s, e, y, NON_SYM); };
            else if (std::strcmp(st.name, "text") == 0)
                fn = [&](int s, int e, int y) { return cf.syllabify(s, e, y); };
            else if (std::strcmp(st.name, "cjk") == 0)
                fn = [&](int s, int e, int y) { return cf.chunkCjk(s, e, y, OTHER_C); };
            else
                fn = [&](int s, int e, int y) { return cf.chunkLatin(s, e, y, OTHER_C); };
            chunks = ChunkFramework::pipeChunk(chunks, fn, st.marker, st.yes);
            chunks = cf.cleanChunks(chunks);
            CHECK(samePairs(st.name, cf.readable(chunks), st.want, st.wantN),
                  (std::string("full example: ") + st.name).c_str());
        }
    }

    // Chunks.make_chunks
    {
        ChunkFramework cf(table, u8to32(ex::kChunksInput));
        CHECK(samePairs("make_chunks", cf.readable(cf.makeChunks()),
                        ex::kMakeChunks, ex::kMakeChunks_n),
              "Chunks: make_chunks pipeline");
    }

    // TokChunks.serve_syls_to_trie: structure + cleaned syllables
    {
        ChunkFramework cf(table, u8to32(ex::kChunksInput));
        auto toks = cf.serveSylsToTrie();
        bool ok = static_cast<int>(toks.size()) == ex::kTokChunks_n;
        for (int i = 0; ok && i < ex::kTokChunks_n; ++i) {
            const auto& want = ex::kTokChunks[i];
            const auto& got = toks[i];
            ok = got.isSyl == want.isSyl && got.chunk.mark == want.mark &&
                 got.chunk.start == want.start && got.chunk.len == want.len &&
                 got.sylChars == parseIdxs(want.sylIdxs);
        }
        CHECK(ok, "TokChunks: serve_syls_to_trie structure");

        // cleaned syllables + tsek, as in the upstream test
        std::vector<std::string> sylsTsek;
        std::u32string text = u8to32(ex::kChunksInput);
        for (const auto& t : toks) {
            if (!t.isSyl) continue;
            std::u32string s;
            for (int i : t.sylChars) s.push_back(text[i]);
            s.push_back(U'་');
            sylsTsek.push_back(u32to8(s));
        }
        bool sOk = static_cast<int>(sylsTsek.size()) == ex::kTokSylsTsek_n;
        for (int i = 0; sOk && i < ex::kTokSylsTsek_n; ++i)
            sOk = sylsTsek[i] == ex::kTokSylsTsek[i];
        CHECK(sOk, "TokChunks: cleaned syllables + tsek");

        auto gs = cf.getSyls();
        bool gOk = static_cast<int>(gs.size()) == ex::kGetSyls_n;
        for (int i = 0; gOk && i < ex::kGetSyls_n; ++i)
            gOk = gs[i] == ex::kGetSyls[i];
        CHECK(gOk, "TokChunks: get_syls");
    }

    // ChunkTokenizer — exercises adjust_syls (spaces inside syllables)
    {
        auto got = chunkTokenize(table, ex::kChunkTokInput);
        bool ok = static_cast<int>(got.size()) == ex::kChunkTok_n;
        for (int i = 0; ok && i < ex::kChunkTok_n; ++i)
            ok = got[i].first == ex::kChunkTok[i].mark &&
                 got[i].second == ex::kChunkTok[i].text;
        if (!ok) {
            std::printf("    chunkTokenize mismatch (%zu vs %d):\n", got.size(),
                        ex::kChunkTok_n);
            for (size_t i = 0; i < got.size(); ++i)
                std::printf("      got  [%zu] %s '%s'\n", i, got[i].first.c_str(),
                            got[i].second.c_str());
        }
        CHECK(ok, "ChunkTokenizer: full tokenize incl. adjust_syls");
    }

    // optional corpus-scale diff-battery: argv[2] = TSV dumped by
    // tools/build_botok_reference.py (Python oracle get_syls over every
    // distinct spine tibetan string). Asserts 100.000% identity.
    if (argc >= 3) {
        std::ifstream ref(argv[2]);
        CHECK(bool(ref), "syllable reference opened");
        std::string line;
        long total = 0, match = 0, shown = 0;
        while (std::getline(ref, line)) {
            if (line.empty()) continue;
            size_t tab = line.find('\t');
            if (tab == std::string::npos) continue;
            std::string text = line.substr(0, tab);
            std::string want = line.substr(tab + 1);
            ChunkFramework cf(table, u8to32(text));
            auto syls = cf.getSyls();
            std::string got;
            for (size_t i = 0; i < syls.size(); ++i) {
                if (i) got += '|';
                got += syls[i];
            }
            ++total;
            if (got == want) ++match;
            else if (shown++ < 5)
                std::printf("    diff: '%s' got '%s' want '%s'\n", text.c_str(),
                            got.c_str(), want.c_str());
        }
        std::printf("  syllable oracle: %ld/%ld = %.3f%%\n", match, total,
                    total ? 100.0 * match / total : 0.0);
        CHECK(total > 100000 && match == total,
              "corpus-scale get_syls identical to Python oracle");
    }

    std::printf("botok_smoke: %s (%d failures)\n",
                failures ? "FAIL" : "ALL PASS", failures);
    return failures ? 1 : 0;
}
