// botok_segtrie.cpp — the memory-lean segmentation trie (see botok.h).
// The segment() walk is the SAME maximal-match algorithm as the ported
// Tokenize (tokenize.py), with node-data bookkeeping removed — data cannot
// affect token boundaries (only walk() and leaf-ness are consulted). The
// battery proves SegTrie::segment == Segmenter::segment (the ported path)
// word-for-word at corpus scale; the ported path stays the oracle anchor.
#include "allcore/botok.h"

namespace allcore {
namespace botok {

const char* segAffixTypeName(int idx) {
    // botok's affix dict, insertion order, 1-based
    static const char* kNames[] = {"",      "la",    "gis",    "gi",
                                   "am",    "ang",   "o",      "gi+o",
                                   "gi+am", "gi+ang", "o+am",  "o+ang"};
    return (idx >= 1 && idx <= 11) ? kNames[idx] : "";
}

static int affixTypeIndex(const std::string& type) {
    for (int i = 1; i <= 11; ++i)
        if (type == segAffixTypeName(i)) return i;
    return 0;
}

uint32_t SegTrie::walkSyl(const std::string& syl, uint32_t node) const {
    auto sit = sylIds_.find(syl);
    if (sit == sylIds_.end()) return kNoNode;
    auto eit = edges_.find((static_cast<uint64_t>(node) << 24) | sit->second);
    return eit == edges_.end() ? kNoNode : eit->second;
}

void SegTrie::add(const std::vector<std::string>& syls, int affixTypeIdx,
                  bool aa) {
    uint32_t node = 0;
    for (const std::string& syl : syls) {
        auto sit = sylIds_.find(syl);
        uint32_t sylId;
        if (sit == sylIds_.end()) {
            sylId = static_cast<uint32_t>(sylIds_.size());
            if (sylId >= (1u << 24))
                return;  // interning space exhausted (never at real scale)
            sylIds_.emplace(syl, sylId);
        } else {
            sylId = sit->second;
        }
        uint64_t key = (static_cast<uint64_t>(node) << 24) | sylId;
        auto eit = edges_.find(key);
        if (eit == edges_.end()) {
            uint32_t child = static_cast<uint32_t>(nodes_.size());
            nodes_.push_back(0);
            edges_.emplace(key, child);
            node = child;
        } else {
            node = eit->second;
        }
    }
    nodes_[node] |= 1;  // leaf
    if (affixTypeIdx > 0) {
        // inflected-form adds overwrite the affix info (Python dict.update);
        // base-form adds (idx 0) leave any existing affix info in place
        nodes_[node] = static_cast<uint16_t>(
            (nodes_[node] & ~0x3Eu) | ((affixTypeIdx & 0xF) << 1) |
            (aa ? 0x20 : 0));
    }
}

void SegTrie::addWord(const std::string& unicodeWord) {
    ChunkFramework cf(table_, u8to32(unicodeWord));
    std::vector<std::string> syls = cf.getSyls();
    if (syls.empty()) return;
    add(syls, 0, false);
    auto affixed = bosyl_.getAllAffixed(u8to32(syls.back()));
    for (const auto& [infl, meta] : affixed) {
        std::vector<std::string> isyls(syls.begin(), syls.end() - 1);
        isyls.push_back(u32to8(infl));
        add(isyls, affixTypeIndex(meta.type), meta.aa);
    }
    ++words_;
}

std::vector<SegWord> SegTrie::segment(const std::string& unicodeText) const {
    ChunkFramework cf(table_, u8to32(unicodeText));
    const auto chunks = cf.serveSylsToTrie();
    const int n = static_cast<int>(chunks.size());
    std::vector<SegWord> out;

    // emit one SegWord over a run of syllable chunks (contiguous, as in
    // create_token: start of first + summed lengths)
    auto emitSyls = [&](const std::vector<int>& sylIdxs, bool word,
                        uint32_t leafNode) {
        int start = chunks[sylIdxs.front()].chunk.start;
        int len = 0;
        for (int i : sylIdxs) len += chunks[i].chunk.len;
        SegWord w;
        w.text = u32to8(cf.text().substr(start, len));
        w.tibetan = true;  // syllable chunks are TEXT
        w.word = word;
        if (word && leafNode != kNoNode) {
            int affIdx = (nodes_[leafNode] >> 1) & 0xF;
            if (affIdx) {
                w.affixType = segAffixTypeName(affIdx);
                w.affixAa = (nodes_[leafNode] & 0x20) != 0;
            }
        }
        out.push_back(std::move(w));
    };
    auto emitChunk = [&](int idx) {  // non-syllable chunk (punct/latin/...)
        SegWord w;
        w.text = u32to8(cf.text().substr(chunks[idx].chunk.start,
                                         chunks[idx].chunk.len));
        w.tibetan = false;
        out.push_back(std::move(w));
    };
    auto sylText = [&](const TokChunk& tc) {
        std::u32string s;
        for (int i : tc.sylChars) s.push_back(cf.text()[i]);
        return u32to8(s);
    };
    // add_found_word_or_non_word, boundaries only
    auto addFound = [&](int cIdx, const std::map<int, uint32_t>& matchNodes,
                        const std::vector<int>& syls) -> int {
        auto it = matchNodes.find(cIdx);
        if (it != matchNodes.end()) {
            emitSyls(syls, true, it->second);
        } else if (!matchNodes.empty()) {
            int nonMaxIdx = matchNodes.rbegin()->first;
            std::vector<int> nonMaxSyls;
            for (int syl : syls)
                if (syl <= nonMaxIdx) nonMaxSyls.push_back(syl);
            emitSyls(nonMaxSyls, true, matchNodes.at(nonMaxIdx));
            cIdx = nonMaxIdx;
        } else {
            emitSyls({syls[0]}, false, kNoNode);
            if (syls.size() >= 2)
                cIdx -= static_cast<int>(syls.size()) - 2;
            if ((cIdx < n && !chunks[cIdx].isSyl) || syls.size() > 1) cIdx -= 1;
        }
        return cIdx;
    };

    int cIdx = 0;
    while (cIdx < n) {
        int walker = cIdx;
        std::vector<int> syls;
        std::vector<std::vector<int>> maxMatch;
        std::map<int, uint32_t> matchNodes;
        uint32_t current = 0;  // root
        bool foundMaxMatch = false;

        while (true) {
            const TokChunk& cur = chunks[walker];
            if (cur.isSyl) {
                current = walkSyl(sylText(cur), current);
                if (current != kNoNode) {
                    syls.push_back(walker);
                    if (nodes_[current] & 1) {  // is_match
                        matchNodes[walker] = current;
                        maxMatch.push_back(syls);
                        if (walker + 1 == n) foundMaxMatch = true;
                    } else {
                        if (walker + 1 == n) {
                            if (!maxMatch.empty()) {
                                foundMaxMatch = true;
                            } else {
                                addFound(walker, matchNodes, syls);
                                cIdx += static_cast<int>(syls.size());
                                break;
                            }
                        }
                    }
                } else {
                    if (!maxMatch.empty()) {
                        foundMaxMatch = true;
                    } else if (!syls.empty()) {
                        cIdx = addFound(walker, matchNodes, syls);
                        break;
                    } else {
                        emitSyls({walker}, false, kNoNode);  // NON_WORD
                        cIdx += 1;
                        break;
                    }
                }
            } else {
                if (!maxMatch.empty()) {
                    foundMaxMatch = true;
                } else if (!syls.empty()) {
                    cIdx = addFound(walker, matchNodes, syls);
                    if (syls.size() == 1) cIdx += 1;
                    break;
                } else {
                    emitChunk(cIdx);
                    cIdx += 1;
                    break;
                }
            }

            if (foundMaxMatch) {
                addFound(cIdx + static_cast<int>(maxMatch.back().size()) - 1,
                         matchNodes, maxMatch.back());
                cIdx += static_cast<int>(maxMatch.back().size());
                break;
            }
            ++walker;
        }
    }
    return out;
}

}  // namespace botok
}  // namespace allcore
