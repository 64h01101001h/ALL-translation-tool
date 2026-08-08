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
#include <map>
#include <memory>
#include <optional>
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

// Human-readable CharMark enum name ("CONS", "TSEK", ...).
const char* charMarkName(int mark);

// ======================= increment 2: trie + tokenizer =======================
// Port sources: botok/tries/basictrie.py, botok/tries/trie.py (in-memory
// logic only — no pickling/dialect packs), botok/textunits/sylcomponents.py
// + bosyl.py (SylComponents.json banked at data/botok/),
// botok/third_party/has_skrt_syl.py (char classes expanded to codepoint
// ranges via sre_parse — see botok_trie.cpp), botok/tokenizers/token.py +
// tokenize.py.

// one sense entry (a Python dict with these possible keys; absent = nullopt)
struct Sense {
    std::optional<std::string> pos, lemma, sense;
    std::optional<long> freq;
    std::optional<bool> affixed;
};

struct Affixation {
    int len = 0;
    std::string type;
    bool aa = false;
};

// a trie node's user data (the Python node.data dict, minus the '_' slot)
struct NodeData {
    bool hasSenses = false;
    std::vector<Sense> senses;
    bool hasAffixation = false;
    Affixation affixation;
    bool skrt = false;       // present-and-true only when set
    bool hasFormFreq = false;
    long formFreq = 0;
};

class TrieNode {
public:
    bool leaf = false;
    NodeData data;
    std::map<std::string, std::unique_ptr<TrieNode>> children;  // syl -> node
    bool canWalk() const { return !children.empty(); }
    bool isMatch() const { return leaf; }
};

// data patch for BasicTrie::add (Python's dict.update semantics)
struct DataPatch {
    std::optional<Affixation> affixation;
    std::optional<bool> skrt;
};

class BasicTrie {
public:
    BasicTrie() : head_(new TrieNode()) {}
    TrieNode* head() { return head_.get(); }

    // word = list of cleaned syllables (UTF-8, no tsek)
    void add(const std::vector<std::string>& word, const DataPatch* data = nullptr);
    TrieNode* walk(const std::string& syl, TrieNode* current) const;
    // add_data with a sense dict / a form-frequency int
    bool addData(const std::vector<std::string>& word, const Sense& sense);
    bool addData(const std::vector<std::string>& word, long formFreq);
    bool deactivate(const std::vector<std::string>& word, bool rev = false);

    static bool isDiffMeaning(const Sense& m1, const Sense& m2);
    static bool addMeaning(std::vector<Sense>& meanings, const Sense& meaning);

private:
    std::unique_ptr<TrieNode> head_;
};

// SylComponents (syllable anatomy from SylComponents.json) + BoSyl affixes.
class BoSyl {
public:
    // dir must contain SylComponents.json
    explicit BoSyl(const std::string& dir);

    struct Parts {
        enum Kind { NotWellFormed, Single, Multiple } kind = NotWellFormed;
        std::u32string root, suffix;  // when Single
    };
    Parts getParts(const std::u32string& syl) const;
    // empty = None (no unambiguous mingzhi)
    std::u32string getMingzhi(const std::u32string& syl) const;
    // "dadrag", "thame", the syllable itself, or "" (= Python None)
    std::u32string getInfo(const std::u32string& syl) const;
    bool isThame(const std::u32string& syl) const;
    bool isAffixable(const std::u32string& syl) const;
    // affixed forms in botok's fixed order; empty = not affixable
    std::vector<std::pair<std::u32string, Affixation>> getAllAffixed(
        const std::u32string& syl) const;

private:
    std::vector<std::u32string> dadrag_, suffixes_, csuffixes_, exceptions_;
    std::map<std::u32string, std::string> roots_;        // root -> class
    std::map<std::u32string, std::u32string> mingzhis_;  // root -> mingzhi
    std::vector<std::u32string> ambiguous_;              // list-valued: mingzhi-less
};

// has_skrt_syl port (third_party): any syllable matching the Sanskrit regexes
bool hasSkrtSyl(const std::u32string& word);

// Trie: BasicTrie + affix inflection (in-memory; no dialect packs/pickles)
class Trie : public BasicTrie {
public:
    Trie(const CharTable& table, const BoSyl& bosyl)
        : table_(table), bosyl_(bosyl) {}

    void inflectNModifyTrie(const std::string& word, bool deactivate = false,
                            bool skrt = false);
    // line = "form\tpos\tlemma\tsense\tfreq" (or comma-separated / bare form)
    void inflectNAddData(const std::string& line);
    void addNonInflectible(const std::string& word);

private:
    struct Inflected {
        std::vector<std::string> syls;  // UTF-8 cleaned syllables
        std::optional<Affixation> affixation;
    };
    const std::vector<Inflected>* getInflected(const std::string& word);

    const CharTable& table_;
    const BoSyl& bosyl_;
    std::map<std::string, std::vector<Inflected>> tmpInflected_;
};

// Token (tokenizers/token.py, the fields Tokenize fills)
struct BotokToken {
    std::string text;
    std::vector<std::string> charTypes;   // CharMark names
    std::string chunkType;                // ChunkMark name
    int start = 0;
    int len = 0;
    bool hasSyls = false;
    std::vector<std::vector<int>> sylsIdx;             // token-relative
    std::vector<std::pair<int, int>> sylsStartEnd;     // token-relative {start,end}
    NodeData data;   // senses/affixation/skrt/formFreq (copied after injection)
    bool skrt = false;
};

// Tokenize (tokenizers/tokenize.py): maximal match with backtracking
class Tokenize {
public:
    explicit Tokenize(Trie& trie) : trie_(trie) {}
    std::vector<BotokToken> tokenize(ChunkFramework& preProcessed,
                                     const std::vector<TokChunk>& chunks);

private:
    int addFoundWordOrNonWord(int cIdx, const std::map<int, TrieNode*>& matchData,
                              const std::vector<int>& syls,
                              std::vector<BotokToken>& tokens) const;
    BotokToken chunksToToken(const std::vector<int>& syls, NodeData* data,
                             const char* ttype) const;
    BotokToken createToken(int ttype, int start, int length,
                           const std::vector<std::vector<int>>* sylChars,
                           const std::vector<std::pair<int, int>>& sylStartEnd,
                           NodeData* data) const;

    Trie& trie_;
    ChunkFramework* pre_ = nullptr;
    const std::vector<TokChunk>* chunks_ = nullptr;
};

// Segmenter — the app-facing facade: a lexicon-fed Trie + Tokenize behind
// one call. REFERENCE ONLY in the UI: segmentation output is displayed as
// a labeled layer and never drives the Overlay's dictionary-bound spans.
struct SegWord {
    std::string text;     // the token's surface text (UTF-8, incl. tsheks)
    bool tibetan = false; // TEXT chunk (vs punct/latin/other)
    bool word = false;    // the cleaned syllables reach a lexicon leaf
    // when the matched leaf is an inflected (affixed) lexicon form:
    std::string affixType;  // "gi", "gis", "la", ... (empty = base form)
    bool affixAa = false;   // the host lost a final འ to take the affix
    bool operator==(const SegWord& o) const {
        return text == o.text && tibetan == o.tibetan && word == o.word &&
               affixType == o.affixType && affixAa == o.affixAa;
    }
};

class Segmenter {
public:
    // dataDir = data/botok (char table + SylComponents.json)
    explicit Segmenter(const std::string& dataDir)
        : table_(dataDir), bosyl_(dataDir), trie_(table_, bosyl_) {}

    // unicodeWord: Tibetan-script word; all affixed forms enter the trie
    void addWord(const std::string& unicodeWord) {
        trie_.inflectNModifyTrie(unicodeWord);
        ++words_;
    }
    size_t wordCount() const { return words_; }

    std::vector<SegWord> segment(const std::string& unicodeText);

private:
    CharTable table_;
    BoSyl bosyl_;
    Trie trie_;
    size_t words_ = 0;
};

// SegTrie — the memory-lean segmentation trie for app-scale lexica
// (spine headwords + the 449k Monlam forms × affix inflection would be
// GB-scale in the ported TrieNode layout). Structure: interned syllable
// ids, one flat edge hash map, two bytes of flags per node (leaf +
// affix-type + aa). Segmentation runs the SAME maximal-match walk as the
// ported Tokenize, minus node-data bookkeeping (which cannot affect
// boundaries); the battery proves SegTrie::segment == Segmenter::segment
// word-for-word at corpus scale, so the ported path remains the anchor.
class SegTrie {
public:
    SegTrie(const std::string& dataDir)
        : table_(dataDir), bosyl_(dataDir) {
        nodes_.push_back(0);  // root
    }

    void addWord(const std::string& unicodeWord);  // word + all affixed forms
    std::vector<SegWord> segment(const std::string& unicodeText) const;

    size_t wordCount() const { return words_; }
    size_t nodeCount() const { return nodes_.size(); }
    size_t edgeCount() const { return edges_.size(); }

private:
    static constexpr uint32_t kNoNode = 0xFFFFFFFFu;
    uint32_t walkSyl(const std::string& syl, uint32_t node) const;
    void add(const std::vector<std::string>& syls, int affixTypeIdx, bool aa);

    CharTable table_;
    BoSyl bosyl_;
    std::unordered_map<std::string, uint32_t> sylIds_;
    std::unordered_map<uint64_t, uint32_t> edges_;  // (parent<<24|sylId) -> child
    // per node: bit0 leaf · bits1-4 affix type (0 none, 1..11 botok order) ·
    // bit5 aa
    std::vector<uint16_t> nodes_;
    size_t words_ = 0;
};

// affix-type names in botok's fixed order (1-based index into this table)
const char* segAffixTypeName(int idx);

}  // namespace botok
}  // namespace allcore
