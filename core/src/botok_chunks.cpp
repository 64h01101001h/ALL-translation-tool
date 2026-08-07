// botok_chunks.cpp — C++ port of Botok's chunking layer (see botok.h header
// comment for the canonical source files). Apache-2.0 upstream.
//
// Porting rules honored here (project rule 2 — port from the file, never
// reconstruct from spec): every upstream quirk is preserved and marked
// [UPSTREAM QUIRK], because the battery proves equivalence with the Python
// behavior, not with what the Python "meant to do".
#include "allcore/botok.h"

#include <fstream>
#include <sstream>
#include <stdexcept>

namespace allcore {
namespace botok {

// ---------------------------------------------------------------- utf8/utf32

std::u32string u8to32(const std::string& s) {
    std::u32string out;
    out.reserve(s.size());
    size_t i = 0;
    while (i < s.size()) {
        unsigned char c = s[i];
        char32_t cp = 0;
        int extra = 0;
        if (c < 0x80) { cp = c; }
        else if ((c & 0xE0) == 0xC0) { cp = c & 0x1F; extra = 1; }
        else if ((c & 0xF0) == 0xE0) { cp = c & 0x0F; extra = 2; }
        else if ((c & 0xF8) == 0xF0) { cp = c & 0x07; extra = 3; }
        else { ++i; continue; }  // stray continuation byte: skip
        if (i + extra >= s.size()) break;  // truncated sequence at end
        bool ok = true;
        for (int k = 1; k <= extra; ++k) {
            unsigned char cc = s[i + k];
            if ((cc & 0xC0) != 0x80) { ok = false; break; }
            cp = (cp << 6) | (cc & 0x3F);
        }
        if (!ok) { ++i; continue; }
        out.push_back(cp);
        i += extra + 1;
    }
    return out;
}

std::string u32to8(const std::u32string& s) {
    std::string out;
    out.reserve(s.size() * 3);
    for (char32_t cp : s) {
        if (cp < 0x80) out.push_back(static_cast<char>(cp));
        else if (cp < 0x800) {
            out.push_back(static_cast<char>(0xC0 | (cp >> 6)));
            out.push_back(static_cast<char>(0x80 | (cp & 0x3F)));
        } else if (cp < 0x10000) {
            out.push_back(static_cast<char>(0xE0 | (cp >> 12)));
            out.push_back(static_cast<char>(0x80 | ((cp >> 6) & 0x3F)));
            out.push_back(static_cast<char>(0x80 | (cp & 0x3F)));
        } else {
            out.push_back(static_cast<char>(0xF0 | (cp >> 18)));
            out.push_back(static_cast<char>(0x80 | ((cp >> 12) & 0x3F)));
            out.push_back(static_cast<char>(0x80 | ((cp >> 6) & 0x3F)));
            out.push_back(static_cast<char>(0x80 | (cp & 0x3F)));
        }
    }
    return out;
}

const char* chunkMarkName(int mark) {
    switch (mark) {
        case BO: return "BO";
        case LATIN_C: return "LATIN";
        case CJK_C: return "CJK";
        case OTHER_C: return "OTHER";
        case TEXT: return "TEXT";
        case PUNCT: return "PUNCT";
        case NON_PUNCT: return "NON_PUNCT";
        case SPACE: return "SPACE";
        case NON_SPACE: return "NON_SPACE";
        case SYM: return "SYM";
        case NON_SYM: return "NON_SYM";
        case NUM: return "NUM";
        case NON_NUM: return "NON_NUM";
        default: return "?";
    }
}

// ---------------------------------------------------------------- CharTable

static int charMarkValue(const std::string& name) {
    static const std::unordered_map<std::string, int> kMap = {
        {"CONS", CONS}, {"SUB_CONS", SUB_CONS}, {"VOW", VOW}, {"TSEK", TSEK},
        {"NORMAL_PUNCT", NORMAL_PUNCT}, {"SPECIAL_PUNCT", SPECIAL_PUNCT},
        {"NUMERAL", NUMERAL}, {"SYMBOL", SYMBOL}, {"IN_SYL_MARK", IN_SYL_MARK},
        {"NON_BO_NON_SKRT", NON_BO_NON_SKRT}, {"SKRT_CONS", SKRT_CONS},
        {"SKRT_SUB_CONS", SKRT_SUB_CONS}, {"SKRT_VOW", SKRT_VOW},
        {"SKRT_LONG_VOW", SKRT_LONG_VOW}, {"CJK", CJK}, {"LATIN", LATIN},
        {"OTHER", OTHER}, {"TRANSPARENT", TRANSPARENT}, {"NFC", NFC},
    };
    auto it = kMap.find(name);
    if (it == kMap.end())
        throw std::runtime_error("botok CharTable: unknown category " + name);
    return it->second;
}

CharTable::CharTable(const std::string& dir) {
    const std::string path = dir + "/bo_uni_table.csv";
    std::ifstream in(path, std::ios::binary);
    if (!in) throw std::runtime_error("botok CharTable: cannot open " + path);
    std::string line;
    bool first = true;
    while (std::getline(in, line)) {
        if (!line.empty() && line.back() == '\r') line.pop_back();
        if (first) {  // header (skip; also strips the utf-8-sig BOM row)
            first = false;
            continue;
        }
        if (line.empty()) continue;
        // cols: 0 hex codepoint, 1 —char—, 2 category name. No quoted commas
        // appear in cols 0-2 of the table; naive split is exact here.
        std::vector<std::string> cols;
        std::stringstream ss(line);
        std::string col;
        while (std::getline(ss, col, ',')) cols.push_back(col);
        if (cols.size() < 3) continue;
        // Upstream reads the char from col 1 with the em-dashes stripped
        // (charcategories.py). We do the same, falling back on the hex col
        // only to cross-check; a mismatch is a data error and throws.
        std::u32string raw = u8to32(cols[1]);
        std::u32string ch;
        for (char32_t c : raw)
            if (c != U'—') ch.push_back(c);  // strip "—"
        if (ch.size() != 1) continue;  // empty/multi never matches a char upstream
        char32_t cp = ch[0];
        char32_t hexCp = static_cast<char32_t>(std::stoul(cols[0], nullptr, 16));
        if (cp != hexCp)
            throw std::runtime_error("botok CharTable: col0/col1 mismatch at " + cols[0]);
        tib_[cp] = charMarkValue(cols[2]);
    }
    if (tib_.size() < 200)
        throw std::runtime_error("botok CharTable: table too small — bad file?");
}

int CharTable::category(char32_t c) const {
    // transparent list (charcategories.transparent), checked first
    switch (c) {
        case U' ': case U'\t': case U'\n':
        case 0x180E: case 0x2000: case 0x2001: case 0x2002: case 0x2003:
        case 0x2004: case 0x2005: case 0x2006: case 0x2007: case 0x2008:
        case 0x2009: case 0x200A: case 0x200B: case 0x202F: case 0x205F:
        case 0x3000: case 0xFEFF:
            return TRANSPARENT;
        default: break;
    }
    // Tibetan block: table lookup, unknown char is an error (never guess)
    if (c >= 0x0F00 && c <= 0x0FFF) {
        auto it = tib_.find(c);
        if (it != tib_.end()) return it->second;
        throw std::runtime_error("botok: Tibetan-range char missing from table");
    }
    // [UPSTREAM QUIRK] the third CJK condition in charcategories.py is
    // eval('" 0"') <= char <= eval('"⾡f"'), which in Python builds
    // the TWO-char strings " "+"0" and "⾡"+"f" (the \u escape only
    // eats 4 hex digits). Lexicographic string comparison then makes the
    // condition equivalent to 0x2000 < c <= 0x2FA1 — NOT the supplementary
    // CJK plane they intended. Real U+20000..U+2FA1F falls through to OTHER,
    // and BMP chars 0x2001..0x2FA1 (not caught earlier) come out CJK, before
    // the LATIN ranges are ever tested. Preserved exactly.
    if ((c >= 0x2E80 && c <= 0xFAFF) || (c >= 0xFE30 && c <= 0xFE4F) ||
        (c > 0x2000 && c <= 0x2FA1))
        return CJK;
    if ((c >= 0x0020 && c <= 0x036F) || (c >= 0x1E00 && c <= 0x20CF))
        return LATIN;
    return OTHER;
}

// ------------------------------------------------------------ ChunkFramework

// truthiness of an intermediate mark under Python rules:
// False(0) and None(-1) are falsy; True(1) and real markers (>=100) truthy.
static inline bool truthy(int mark) { return mark == 1 || mark >= 100; }

ChunkFramework::ChunkFramework(const CharTable& table, const std::u32string& text,
                               const std::u32string& ignoreChars)
    : text_(text), len_(static_cast<int>(text.size())) {
    base_.resize(text_.size());
    for (int i = 0; i < len_; ++i) {
        int cat = table.category(text_[i]);
        if (cat == NFC) ++nfcCount_;  // upstream warns; we count and go on
        bool ignored = ignoreChars.find(text_[i]) != std::u32string::npos;
        base_[i] = ignored ? TRANSPARENT : cat;
    }
}

std::vector<Chunk> ChunkFramework::chunk(int startIdx, int endIdx,
                                         const std::function<bool(int)>& cond) {
    std::vector<Chunk> chunked;
    int start = startIdx;
    int length = 0;
    int prevState = -1;
    int currentState = -1;
    for (int i = startIdx; i < endIdx; ++i) {
        currentState = cond(i) ? 1 : 0;
        if (prevState == -1) prevState = currentState;
        if (currentState == prevState) {
            ++length;
        } else {
            chunked.push_back({prevState, start, length});
            prevState = currentState;
            start += length;
            length = 1;
        }
    }
    // [UPSTREAM QUIRK] tail branch kept verbatim, including the
    // (start+length < end_idx) length bump, though it looks unreachable.
    if (length != 0) {
        if (currentState == prevState && start + length < endIdx) ++length;
        chunked.push_back({prevState, start, length});
    }
    return chunked;
}

std::vector<Chunk> ChunkFramework::chunkUsing(const std::function<bool(int)>& cond,
                                              int start, int end, int yes,
                                              int no) const {
    // Python: `if not start and not end: start, end = 0, len` — callers
    // either pass both or neither; the full-string overloads pass (0, len_).
    auto indices = chunk(start, end, cond);
    std::vector<Chunk> out;
    out.reserve(indices.size());
    for (const Chunk& i : indices)
        out.push_back({i.mark ? yes : no, i.start, i.len});
    return out;
}

std::vector<Chunk> ChunkFramework::mergeChunks(
    std::vector<Chunk> chunks,
    const std::function<bool(const Chunk&, const Chunk&)>& mergeCond) {
    // (the upstream `while not previous` placeholder loop is dead code —
    // a 3-tuple is always truthy — and is omitted)
    int num = 0;
    while (num <= static_cast<int>(chunks.size()) - 1) {
        Chunk current = chunks[num];
        if (num - 1 >= 0) {
            Chunk previous = chunks[num - 1];
            if (mergeCond(previous, current)) {
                chunks[num - 1] = {previous.mark, previous.start,
                                   previous.len + current.len};
                chunks.erase(chunks.begin() + num);
                --num;
            }
        }
        ++num;
    }
    return chunks;
}

bool ChunkFramework::mergeCondition(const Chunk& c,
                                    const std::function<bool(int)>& cond) const {
    for (int i = c.start; i < c.start + c.len; ++i)
        if (!cond(i)) return false;
    return true;
}

std::vector<Chunk> ChunkFramework::pipeChunk(std::vector<Chunk> chunks,
                                             const PipedFn& fn, int toChunkMarker,
                                             int yes) {
    // [UPSTREAM QUIRK] Python iterates `enumerate(chunks)` over the LIVE list
    // while deleting/inserting at i, so iteration continues at i+1 inside the
    // freshly inserted region (re-examining inserted chunks that kept the
    // original marker). The index loop below reproduces that exactly.
    for (size_t i = 0; i < chunks.size(); ++i) {
        Chunk chunk = chunks[i];
        if (chunk.mark != toChunkMarker) continue;
        auto fresh = fn(chunk.start, chunk.start + chunk.len, yes);
        if (fresh.empty()) continue;
        chunks.erase(chunks.begin() + i);
        for (size_t j = 0; j < fresh.size(); ++j) {
            if (fresh[j].mark != yes)
                chunks.insert(chunks.begin() + i + j,
                              {chunk.mark, fresh[j].start, fresh[j].len});
            else
                chunks.insert(chunks.begin() + i + j, fresh[j]);
        }
    }
    return chunks;
}

std::vector<Chunk> ChunkFramework::cleanChunks(std::vector<Chunk> chunks) const {
    chunks = mergeSpaces(std::move(chunks));
    return mergeSimilarChunks(std::move(chunks));
}

std::vector<Chunk> ChunkFramework::mergeSpaces(std::vector<Chunk> chunks) const {
    return mergeChunks(std::move(chunks), [this](const Chunk&, const Chunk& cur) {
        return mergeCondition(cur, [this](int i) { return isSpace(i); });
    });
}

std::vector<Chunk> ChunkFramework::mergeSimilarChunks(std::vector<Chunk> chunks) {
    return mergeChunks(std::move(chunks), [](const Chunk& prev, const Chunk& cur) {
        return prev.mark != TEXT && cur.mark != TEXT && prev.mark == cur.mark;
    });
}

std::vector<Chunk> ChunkFramework::mergeSkippablePunct(
    std::vector<Chunk> chunks) const {
    int i = 0;
    while (i <= static_cast<int>(chunks.size()) - 1) {
        Chunk current = chunks[i];
        // first element: absorb a fully-skippable chunk 0 into chunk 1.
        if (i == 0 && static_cast<int>(chunks.size()) - 1 >= 1) {
            bool toDel = mergeCondition(
                current, [this](int ci) { return isSkippablePunct(ci); });
            if (toDel) {
                // [UPSTREAM QUIRK] the merged length is written as
                // next.start + next.len — an absolute end index, which equals
                // the correct length only because chunk 0 starts at 0.
                // Preserved verbatim.
                chunks[i + 1] = {chunks[i + 1].mark, current.start,
                                 chunks[i + 1].start + chunks[i + 1].len};
                chunks.erase(chunks.begin() + i);
                --i;
            }
        }
        // remaining ones: absorb a fully-skippable chunk into its predecessor
        // (note: `current` stays the snapshot from the top, as in Python).
        if (i - 1 >= 0) {
            bool toDel = mergeCondition(
                current, [this](int ci) { return isSkippablePunct(ci); });
            if (toDel) {
                chunks[i - 1] = {chunks[i - 1].mark, chunks[i - 1].start,
                                 current.len + chunks[i - 1].len};
                chunks.erase(chunks.begin() + i);
                --i;
            }
        }
        ++i;
    }
    return mergeSimilarChunks(std::move(chunks));
}

// ------------------------------------------------------------- test methods

bool ChunkFramework::isBoUnicode(int i) const {
    return base_[i] != OTHER && base_[i] != LATIN && base_[i] != CJK;
}
bool ChunkFramework::isLatin(int i) const {
    return base_[i] == LATIN || base_[i] == TRANSPARENT;
}
bool ChunkFramework::isCjk(int i) const {
    return base_[i] == CJK || base_[i] == TRANSPARENT;
}
bool ChunkFramework::isPunct(int i) const {
    // exact port incl. the look-behind clause (chunkframework.__is_punct)
    if (i && (base_[i - 1] == SYMBOL || base_[i - 1] == NUMERAL ||
              base_[i - 1] == OTHER || base_[i - 1] == NORMAL_PUNCT ||
              base_[i - 1] == SPECIAL_PUNCT || base_[i - 1] == TSEK ||
              base_[i - 1] == TRANSPARENT) &&
        (base_[i] == TSEK || base_[i] == TRANSPARENT ||
         base_[i] == NORMAL_PUNCT))
        return true;
    return base_[i] == NORMAL_PUNCT || base_[i] == SPECIAL_PUNCT ||
           base_[i] == TRANSPARENT;
}
bool ChunkFramework::isSym(int i) const {
    return base_[i] == SYMBOL || base_[i] == TRANSPARENT || base_[i] == NFC;
}
bool ChunkFramework::isNum(int i) const {
    return base_[i] == NUMERAL || base_[i] == TRANSPARENT;
}
bool ChunkFramework::isSpace(int i) const { return base_[i] == TRANSPARENT; }
bool ChunkFramework::isTsekOrLongSkrtVowel(int i) const {
    return base_[i] == TSEK || base_[i] == SKRT_LONG_VOW;
}
bool ChunkFramework::isSkippablePunct(int i) const {
    return base_[i] == TSEK || isSpace(i);
}
bool ChunkFramework::isSylText(int i) const {
    // literal port of TokChunks.__is_syl_text (the redundant OR keeps
    // SKRT_LONG_VOW in the cleaned syllable)
    return (base_[i] != TSEK && base_[i] != TRANSPARENT &&
            base_[i] != SKRT_LONG_VOW) ||
           base_[i] == SKRT_LONG_VOW;
}

// ------------------------------------------------------------- chunkers

std::vector<Chunk> ChunkFramework::chunkBoChars(int start, int end, int yes, int no) {
    return chunkUsing([this](int i) { return isBoUnicode(i); }, start, end, yes, no);
}
std::vector<Chunk> ChunkFramework::chunkLatin(int start, int end, int yes, int no) {
    return chunkUsing([this](int i) { return isLatin(i); }, start, end, yes, no);
}
std::vector<Chunk> ChunkFramework::chunkCjk(int start, int end, int yes, int no) {
    return chunkUsing([this](int i) { return isCjk(i); }, start, end, yes, no);
}
std::vector<Chunk> ChunkFramework::chunkPunct(int start, int end, int yes, int no) {
    return chunkUsing([this](int i) { return isPunct(i); }, start, end, yes, no);
}
std::vector<Chunk> ChunkFramework::chunkSymbol(int start, int end, int yes, int no) {
    return chunkUsing([this](int i) { return isSym(i); }, start, end, yes, no);
}
std::vector<Chunk> ChunkFramework::chunkNumber(int start, int end, int yes, int no) {
    return chunkUsing([this](int i) { return isNum(i); }, start, end, yes, no);
}
std::vector<Chunk> ChunkFramework::chunkSpaces(int start, int end, int yes, int no) {
    return chunkUsing([this](int i) { return isSpace(i); }, start, end, yes, no);
}

std::vector<Chunk> ChunkFramework::syllabify(int start, int end, int yes) {
    auto indices = chunk(start, end,
                         [this](int i) { return isTsekOrLongSkrtVowel(i); });
    for (size_t num = 0; num < indices.size(); ++num) {
        if (indices[num].mark && num >= 1 && !indices[num - 1].mark)
            indices[num - 1].len += indices[num].len;
    }
    std::vector<Chunk> out;
    for (const Chunk& i : indices)
        if (!i.mark) out.push_back({yes, i.start, i.len});
    return out;
}

std::vector<Chunk> ChunkFramework::adjustSyls(int start, int end, int yes) {
    // exact port of chunkframework.adjust_syls: splits a syllable after an
    // internal space only when what precedes ends in ཀ/ག/ཤ (bare or +ི) —
    // the no-shad consonants whose syllable can legally end there; other
    // internal spaces are absorbed into one syllable with what follows.
    auto indices = chunk(start, end, [this](int i) { return isSpace(i); });
    const int n = static_cast<int>(indices.size());
    for (int num = 0; num < n; ++num) {
        if (n - 1 > num && num > 0 && truthy(indices[num].mark)) {
            int s = indices[num - 1].start;
            int e = indices[num - 1].len;
            bool endsSplittable =
                (e >= 2 && text_[s + e - 1] == U'ི' &&
                 (text_[s + e - 2] == U'ཀ' || text_[s + e - 2] == U'ག' ||
                  text_[s + e - 2] == U'ཤ')) ||
                (e >= 1 && (text_[s + e - 1] == U'ཀ' || text_[s + e - 1] == U'ག' ||
                            text_[s + e - 1] == U'ཤ'));
            if (endsSplittable) {
                indices[num - 1] = {yes, indices[num - 1].start,
                                    indices[num - 1].len + indices[num].len};
            } else {
                indices[num - 1] = {indices[num - 1].mark, indices[num - 1].start,
                                    indices[num - 1].len + indices[num].len +
                                        indices[num + 1].len};
                indices[num + 1] = {kNoneMark, indices[num + 1].start,
                                    indices[num + 1].len};
            }
        } else if (indices[num].mark == 0) {  // `is False`
            indices[num].mark = yes;
        } else if ((num == 0 || num == n - 1) && indices[num].mark == 1) {  // `is True`
            indices[num].mark = PUNCT;
        }
    }
    std::vector<Chunk> out;
    for (const Chunk& i : indices)
        if (i.mark != 1 && i.mark != kNoneMark) out.push_back(i);
    if (out.size() > 1) return out;
    return {};
}

// ------------------------------------------------------------- pipelines

std::vector<Chunk> ChunkFramework::makeChunks(bool spaceAsPunct) {
    auto chunks = chunkBoChars();
    if (spaceAsPunct)
        chunks = pipeChunk(chunks,
                           [this](int s, int e, int y) {
                               return chunkSpaces(s, e, y, NON_SPACE);
                           },
                           BO, PUNCT);
    chunks = pipeChunk(chunks,
                       [this](int s, int e, int y) {
                           return chunkPunct(s, e, y, NON_PUNCT);
                       },
                       BO, PUNCT);
    chunks = pipeChunk(chunks,
                       [this](int s, int e, int y) {
                           return chunkSymbol(s, e, y, NON_SYM);
                       },
                       BO, SYM);
    chunks = pipeChunk(chunks,
                       [this](int s, int e, int y) {
                           return chunkNumber(s, e, y, NON_NUM);
                       },
                       BO, NUM);
    if (!spaceAsPunct)
        chunks = mergeSkippablePunct(std::move(chunks));  // correctly built syls
    chunks = pipeChunk(chunks,
                       [this](int s, int e, int y) { return syllabify(s, e, y); },
                       BO, TEXT);
    chunks = pipeChunk(chunks,
                       [this](int s, int e, int y) { return adjustSyls(s, e, y); },
                       TEXT, TEXT);
    chunks = pipeChunk(chunks,
                       [this](int s, int e, int y) {
                           return chunkCjk(s, e, y, OTHER_C);
                       },
                       OTHER_C, CJK_C);
    chunks = pipeChunk(chunks,
                       [this](int s, int e, int y) {
                           return chunkLatin(s, e, y, OTHER_C);
                       },
                       OTHER_C, LATIN_C);
    if (!spaceAsPunct) chunks = mergeSkippablePunct(std::move(chunks));
    return chunks;
}

std::vector<TokChunk> ChunkFramework::serveSylsToTrie(bool spaceAsPunct) {
    std::vector<TokChunk> out;
    for (const Chunk& c : makeChunks(spaceAsPunct)) {
        TokChunk tc;
        tc.chunk = c;
        if (c.mark == TEXT) {
            tc.isSyl = true;
            for (int i = c.start; i < c.start + c.len; ++i)
                if (isSylText(i)) tc.sylChars.push_back(i);
        }
        out.push_back(std::move(tc));
    }
    return out;
}

std::vector<std::string> ChunkFramework::getSyls(bool spaceAsPunct) {
    std::vector<std::string> syls;
    for (const Chunk& c : makeChunks(spaceAsPunct)) {
        if (c.mark != TEXT) continue;
        std::u32string syl;
        for (int i = c.start; i < c.start + c.len; ++i)
            if (isSylText(i)) syl.push_back(text_[i]);
        syls.push_back(u32to8(syl));
    }
    return syls;
}

std::string ChunkFramework::chunkString(const Chunk& c) const {
    return u32to8(text_.substr(c.start, c.len));
}

std::vector<std::pair<std::string, std::string>> ChunkFramework::readable(
    const std::vector<Chunk>& chunks) const {
    std::vector<std::pair<std::string, std::string>> out;
    out.reserve(chunks.size());
    for (const Chunk& c : chunks)
        out.emplace_back(chunkMarkName(c.mark), chunkString(c));
    return out;
}

std::vector<std::pair<std::string, std::string>> chunkTokenize(
    const CharTable& table, const std::string& utf8Text) {
    ChunkFramework cf(table, u8to32(utf8Text));
    return cf.readable(cf.makeChunks());
}

}  // namespace botok
}  // namespace allcore
