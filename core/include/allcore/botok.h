// allcore/botok.h — C++ port of Botok's chunking layer (OpenPecha/Botok,
// Apache-2.0; license banked at data/botok/LICENSE-botok).
//
// Port sources (canonical, rule 2 — ported from the files, proven with the
// project's own tests as battery in botok_smoke):
//   botok/textunits/charcategories.py  (char table + ranges)
//   botok/textunits/bostring.py        (per-codepoint categories)
//   botok/chunks/chunkframeworkbase.py (chunk / merge / pipe machinery)
//   botok/chunks/chunkframework.py     (bo/punct/sym/num/space/syl chunkers)
//   botok/chunks/chunks.py             (Chunks pipeline + TokChunks)
//   botok/tokenizers/chunktokenizer.py (ChunkTokenizer)
//
// Python string semantics are codepoint-based, so everything here is
// UTF-32-internal; indices in Chunk are codepoint indices into the input.
// Known upstream quirks are preserved deliberately (never "fixed"):
// see botok_chunks.cpp comments (CJK range string-comparison artifact,
// chunk() tail branch, live-list pipe_chunk iteration).
#pragma once

#include <functional>
#include <string>
#include <unordered_map>
#include <vector>

namespace allcore {
namespace botok {

// CharMarkers — values match botok's IntEnum (start=1).
enum CharMark : int {
    CONS = 1, SUB_CONS, VOW, TSEK,
    NORMAL_PUNCT, SPECIAL_PUNCT,
    NUMERAL, SYMBOL, IN_SYL_MARK, NON_BO_NON_SKRT,
    SKRT_CONS, SKRT_SUB_CONS, SKRT_VOW, SKRT_LONG_VOW,
    CJK, LATIN, OTHER, TRANSPARENT, NFC,
};

// ChunkMarkers — values match botok's IntEnum (start=100).
enum ChunkMark : int {
    BO = 100, LATIN_C, CJK_C, OTHER_C,
    TEXT, PUNCT, NON_PUNCT, SPACE, NON_SPACE,
    SYM, NON_SYM, NUM, NON_NUM,
};

// Human-readable enum name for a ChunkMark value ("BO", "PUNCT", ...).
const char* chunkMarkName(int mark);

std::u32string u8to32(const std::string& s);
std::string u32to8(const std::u32string& s);

// The Tibetan-block char table from bo_uni_table.csv (codepoint -> CharMark).
// Loaded once; pass to every ChunkFramework.
class CharTable {
public:
    // dir must contain bo_uni_table.csv. Throws std::runtime_error on
    // missing file / unknown category name.
    explicit CharTable(const std::string& dir);

    // Exact port of charcategories.get_char_category (incl. its quirks).
    // Throws std::runtime_error for a Tibetan-range codepoint absent from
    // the table (upstream raises ValueError — never guess).
    int category(char32_t c) const;

    size_t tableSize() const { return tib_.size(); }

private:
    std::unordered_map<char32_t, int> tib_;
};

// One chunk: marker + codepoint start + codepoint length.
// During intermediate stages mark can be 0 (Py False), 1 (Py True) or
// kNoneMark (Py None); final pipelines only emit ChunkMark values.
struct Chunk {
    int mark;
    int start;
    int len;
    bool operator==(const Chunk& o) const {
        return mark == o.mark && start == o.start && len == o.len;
    }
};
inline constexpr int kNoneMark = -1;

// TokChunks output: syllable chunks carry the cleaned char indices
// (tsek/space-free), others carry isSyl=false.
struct TokChunk {
    bool isSyl = false;
    std::vector<int> sylChars;  // codepoint indices into the input string
    Chunk chunk{0, 0, 0};
};

// BoString + ChunkFramework + Chunks + TokChunks in one class (the Python
// split is inheritance; the data flow is identical).
class ChunkFramework {
public:
    ChunkFramework(const CharTable& table, const std::u32string& text,
                   const std::u32string& ignoreChars = std::u32string());

    const std::u32string& text() const { return text_; }
    // per-codepoint CharMark values (BoString.base_structure)
    const std::vector<int>& baseStructure() const { return base_; }
    int nfcCount() const { return nfcCount_; }  // upstream warns; we count

    // ---- chunkers (full-string forms use the Python default markers) ----
    std::vector<Chunk> chunkBoChars() { return chunkBoChars(0, len_, BO, OTHER_C); }
    std::vector<Chunk> chunkBoChars(int start, int end, int yes, int no);
    std::vector<Chunk> chunkLatin() { return chunkLatin(0, len_, LATIN_C, OTHER_C); }
    std::vector<Chunk> chunkLatin(int start, int end, int yes, int no);
    std::vector<Chunk> chunkCjk() { return chunkCjk(0, len_, CJK_C, OTHER_C); }
    std::vector<Chunk> chunkCjk(int start, int end, int yes, int no);
    std::vector<Chunk> chunkPunct() { return chunkPunct(0, len_, PUNCT, NON_PUNCT); }
    std::vector<Chunk> chunkPunct(int start, int end, int yes, int no);
    std::vector<Chunk> chunkSymbol() { return chunkSymbol(0, len_, SYM, NON_SYM); }
    std::vector<Chunk> chunkSymbol(int start, int end, int yes, int no);
    std::vector<Chunk> chunkNumber() { return chunkNumber(0, len_, NUM, NON_NUM); }
    std::vector<Chunk> chunkNumber(int start, int end, int yes, int no);
    std::vector<Chunk> chunkSpaces() { return chunkSpaces(0, len_, SPACE, NON_SPACE); }
    std::vector<Chunk> chunkSpaces(int start, int end, int yes, int no);
    std::vector<Chunk> syllabify() { return syllabify(0, len_, TEXT); }
    std::vector<Chunk> syllabify(int start, int end, int yes);
    std::vector<Chunk> adjustSyls(int start, int end, int yes);

    // ---- framework machinery (exact ports) ----
    using PipedFn = std::function<std::vector<Chunk>(int, int, int)>;  // (start,end,yes)
    static std::vector<Chunk> pipeChunk(std::vector<Chunk> chunks, const PipedFn& fn,
                                        int toChunkMarker, int yes);
    std::vector<Chunk> cleanChunks(std::vector<Chunk> chunks) const;
    std::vector<Chunk> mergeSkippablePunct(std::vector<Chunk> chunks) const;

    // ---- Chunks pipeline ----
    std::vector<Chunk> makeChunks(bool spaceAsPunct = false);

    // ---- TokChunks ----
    std::vector<TokChunk> serveSylsToTrie(bool spaceAsPunct = false);
    std::vector<std::string> getSyls(bool spaceAsPunct = false);  // UTF-8, cleaned

    // ---- readable output (get_markers + get_chunked) ----
    std::string chunkString(const Chunk& c) const;  // UTF-8 substring
    // [(marker name, substring)] as in the Python tests
    std::vector<std::pair<std::string, std::string>> readable(
        const std::vector<Chunk>& chunks) const;

private:
    std::vector<Chunk> chunkUsing(const std::function<bool(int)>& cond,
                                  int start, int end, int yes, int no) const;
    static std::vector<Chunk> chunk(int startIdx, int endIdx,
                                    const std::function<bool(int)>& cond);
    static std::vector<Chunk> mergeChunks(
        std::vector<Chunk> chunks,
        const std::function<bool(const Chunk&, const Chunk&)>& mergeCond);
    bool mergeCondition(const Chunk& c, const std::function<bool(int)>& cond) const;
    std::vector<Chunk> mergeSpaces(std::vector<Chunk> chunks) const;
    static std::vector<Chunk> mergeSimilarChunks(std::vector<Chunk> chunks);

    bool isBoUnicode(int i) const;
    bool isLatin(int i) const;
    bool isCjk(int i) const;
    bool isPunct(int i) const;
    bool isSym(int i) const;
    bool isNum(int i) const;
    bool isSpace(int i) const;
    bool isTsekOrLongSkrtVowel(int i) const;
    bool isSkippablePunct(int i) const;
    bool isSylText(int i) const;

    std::u32string text_;
    int len_ = 0;
    std::vector<int> base_;
    int nfcCount_ = 0;
};

// ChunkTokenizer: make_chunks + readable — the coarse syllable/punct pass.
std::vector<std::pair<std::string, std::string>> chunkTokenize(
    const CharTable& table, const std::string& utf8Text);

}  // namespace botok
}  // namespace allcore
