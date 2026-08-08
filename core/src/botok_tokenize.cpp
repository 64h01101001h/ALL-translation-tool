// botok_tokenize.cpp — C++ port of Botok's Tokenize (tokenizers/tokenize.py
// + the Token fields it fills from tokenizers/token.py). Apache-2.0 upstream.
// The maximal-match walk, the non-max backtracking and the in-place pos
// injection into trie node data (an upstream side effect) are preserved
// exactly; the battery proves stream equivalence against the live oracle.
#include "allcore/botok.h"

namespace allcore {
namespace botok {

namespace {

// ttype = NO_POS when the node data has no senses or none of them has a pos
bool needsNoPos(const NodeData& data) {
    if (!data.hasSenses) return true;
    for (const Sense& m : data.senses)
        if (m.pos) return false;
    return true;
}

bool hasSkrtChar(const std::vector<int>& groups) {
    for (int g : groups)
        if (g == SKRT_VOW || g == SKRT_CONS || g == SKRT_SUB_CONS) return true;
    return false;
}

}  // namespace

std::vector<BotokToken> Tokenize::tokenize(ChunkFramework& preProcessed,
                                           const std::vector<TokChunk>& chunks) {
    pre_ = &preProcessed;
    chunks_ = &chunks;
    std::vector<BotokToken> tokens;
    const int n = static_cast<int>(chunks.size());

    int cIdx = 0;
    while (cIdx < n) {
        int walker = cIdx;
        std::vector<int> syls;
        std::vector<std::vector<int>> maxMatch;
        std::map<int, TrieNode*> matchData;
        TrieNode* currentNode = nullptr;
        bool foundMaxMatch = false;

        while (true) {
            const TokChunk& cur = chunks[walker];
            if (cur.isSyl) {  // CHUNK IS SYLLABLE
                std::u32string syl32;
                for (int i : cur.sylChars) syl32.push_back(pre_->text()[i]);
                currentNode = trie_.walk(u32to8(syl32), currentNode);
                if (currentNode) {
                    syls.push_back(walker);
                    if (currentNode->isMatch()) {
                        matchData[walker] = currentNode;
                        maxMatch.push_back(syls);
                        if (walker + 1 == n) foundMaxMatch = true;
                    } else {
                        if (walker + 1 == n) {
                            if (!maxMatch.empty()) {
                                foundMaxMatch = true;
                            } else {
                                addFoundWordOrNonWord(walker, matchData, syls,
                                                      tokens);
                                cIdx += static_cast<int>(syls.size());
                                break;
                            }
                        }
                    }
                } else {  // CAN'T CONTINUE WALKING
                    if (!maxMatch.empty()) {
                        foundMaxMatch = true;
                    } else if (!syls.empty()) {
                        cIdx = addFoundWordOrNonWord(walker, matchData, syls,
                                                     tokens);
                        break;
                    } else {
                        tokens.push_back(
                            chunksToToken({walker}, nullptr, "NON_WORD"));
                        cIdx += 1;
                        break;
                    }
                }
            } else {  // CHUNK IS NON-SYLLABLE
                if (!maxMatch.empty()) {
                    foundMaxMatch = true;
                } else if (!syls.empty()) {
                    cIdx = addFoundWordOrNonWord(walker, matchData, syls, tokens);
                    if (syls.size() == 1) cIdx += 1;
                    break;
                } else {
                    tokens.push_back(chunksToToken({cIdx}, nullptr, nullptr));
                    cIdx += 1;
                    break;
                }
            }

            if (foundMaxMatch) {
                addFoundWordOrNonWord(
                    cIdx + static_cast<int>(maxMatch.back().size()) - 1,
                    matchData, maxMatch.back(), tokens);
                cIdx += static_cast<int>(maxMatch.back().size());
                break;
            }
            ++walker;
        }
    }

    pre_ = nullptr;
    chunks_ = nullptr;
    return tokens;
}

int Tokenize::addFoundWordOrNonWord(int cIdx,
                                    const std::map<int, TrieNode*>& matchData,
                                    const std::vector<int>& syls,
                                    std::vector<BotokToken>& tokens) const {
    auto it = matchData.find(cIdx);
    if (it != matchData.end()) {  // there is a match
        NodeData* data = &it->second->data;
        const char* ttype = needsNoPos(*data) ? "NO_POS" : nullptr;
        tokens.push_back(chunksToToken(syls, data, ttype));
    } else if (!matchData.empty()) {  // non-max match: back off to the last one
        int nonMaxIdx = matchData.rbegin()->first;
        std::vector<int> nonMaxSyls;
        for (int syl : syls)
            if (syl <= nonMaxIdx) nonMaxSyls.push_back(syl);
        NodeData* data = &matchData.at(nonMaxIdx)->data;
        const char* ttype = needsNoPos(*data) ? "NO_POS" : nullptr;
        tokens.push_back(chunksToToken(nonMaxSyls, data, ttype));
        cIdx = nonMaxIdx;
    } else {
        // add first syl as non-word, then decrement for a fresh attempt
        tokens.push_back(chunksToToken({syls[0]}, nullptr, "NO_POS"));
        if (syls.size() >= 2)
            cIdx -= static_cast<int>(syls.size()) - 2;  // len(syls[1:]) - 1
        // has_decremented is always False upstream (callers never pass it)
        if ((cIdx < static_cast<int>(chunks_->size()) &&
             !(*chunks_)[cIdx].isSyl) ||
            syls.size() > 1)
            cIdx -= 1;
    }
    return cIdx;
}

BotokToken Tokenize::chunksToToken(const std::vector<int>& syls, NodeData* data,
                                   const char* ttype) const {
    NodeData fresh;  // Python's literal {} argument
    if (!data) data = &fresh;
    // pos injection — mutates the passed-in data (trie node data for
    // matches), exactly like upstream
    if (ttype) {
        if (!data->hasSenses) {
            data->hasSenses = true;
            Sense s;
            s.pos = ttype;
            data->senses = {s};
        } else {
            for (Sense& m : data->senses)
                if (!m.pos) m.pos = ttype;
        }
    }

    const auto& C = *chunks_;
    if (syls.size() == 1) {
        const TokChunk& tc = C[syls[0]];
        std::vector<std::vector<int>> tokenSyls;
        if (tc.isSyl) tokenSyls.push_back(tc.sylChars);
        std::vector<std::pair<int, int>> sylStartEnd = {
            {tc.chunk.start, tc.chunk.len}};
        return createToken(tc.chunk.mark, tc.chunk.start, tc.chunk.len,
                           tc.isSyl ? &tokenSyls : nullptr, sylStartEnd, data);
    }
    // multi-syllable: all chunks are syllable chunks
    std::vector<std::vector<int>> tokenSyls;
    std::vector<std::pair<int, int>> sylStartEnd;
    int tokenLength = 0;
    for (int idx : syls) {
        tokenSyls.push_back(C[idx].sylChars);
        tokenLength += C[idx].chunk.len;
        sylStartEnd.emplace_back(C[idx].chunk.start, C[idx].chunk.len);
    }
    int tokenType = C[syls.back()].chunk.mark;
    int tokenStart = C[syls.front()].chunk.start;
    return createToken(tokenType, tokenStart, tokenLength, &tokenSyls,
                       sylStartEnd, data);
}

BotokToken Tokenize::createToken(
    int ttype, int start, int length,
    const std::vector<std::vector<int>>* sylChars,
    const std::vector<std::pair<int, int>>& sylStartEnd, NodeData* data) const {
    BotokToken token;
    token.text = u32to8(pre_->text().substr(start, length));
    token.chunkType = chunkMarkName(ttype);
    token.start = start;
    token.len = length;
    if (sylChars) {  // Python: if syls != [None]
        token.hasSyls = true;
        for (const auto& syl : *sylChars) {
            std::vector<int> rel;
            for (int s : syl) rel.push_back(s - start);
            token.sylsIdx.push_back(rel);
        }
        for (const auto& [s, l] : sylStartEnd)
            token.sylsStartEnd.emplace_back(s - start, s - start + l);
    }
    std::vector<int> charGroups;
    for (int i = start; i < start + length; ++i)
        charGroups.push_back(pre_->baseStructure()[i]);
    for (int g : charGroups) token.charTypes.push_back(charMarkName(g));
    token.data = *data;  // copy after injection (see header notes)
    token.skrt = data->skrt;
    if (!token.skrt)
        token.skrt = hasSkrtChar(charGroups) ||
                     hasSkrtSyl(pre_->text().substr(start, length));
    return token;
}

// ------------------------------------------------------------------ Segmenter

std::vector<SegWord> Segmenter::segment(const std::string& unicodeText) {
    ChunkFramework cf(table_, u8to32(unicodeText));
    auto chunks = cf.serveSylsToTrie();
    Tokenize tok(trie_);
    std::vector<SegWord> out;
    for (const BotokToken& t : tok.tokenize(cf, chunks)) {
        SegWord w;
        w.text = t.text;
        w.tibetan = t.chunkType == "TEXT";
        if (w.tibetan && t.hasSyls) {
            // word iff the cleaned syllables reach a lexicon leaf — checked
            // against the trie directly (pos labels can't distinguish a
            // matched-lexicon token from a backtracked non-word)
            std::u32string text32 = u8to32(t.text);
            TrieNode* node = nullptr;
            bool ok = true;
            for (const auto& syl : t.sylsIdx) {
                std::u32string s32;
                for (int i : syl) s32.push_back(text32[i]);
                node = trie_.walk(u32to8(s32), node);
                if (!node) { ok = false; break; }
            }
            w.word = ok && node && node->isMatch();
            if (w.word && node->data.hasAffixation) {
                w.affixType = node->data.affixation.type;
                w.affixAa = node->data.affixation.aa;
            }
        }
        out.push_back(std::move(w));
    }
    return out;
}

}  // namespace botok
}  // namespace allcore
