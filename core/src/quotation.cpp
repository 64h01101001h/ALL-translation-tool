#include "allcore/quotation.h"

#include <cctype>

#include "allcore/engines.h"

namespace allcore {

namespace {

// syllable tokenization shared by passage and corpus sides — identical
// normalization on both sides is what makes the comparison exact
std::vector<std::string> syllables(const std::string& wylie) {
    std::vector<std::string> out;
    std::string cur;
    auto flush = [&] {
        // strip punctuation from the edges; keep wylie-significant marks
        // ('a-chung apostrophe, +, ., ~) inside the syllable
        size_t b = 0, e = cur.size();
        while (b < e && !std::isalnum((unsigned char)cur[b]) &&
               cur[b] != '\'')
            ++b;
        while (e > b && !std::isalnum((unsigned char)cur[e - 1]) &&
               cur[e - 1] != '\'')
            --e;
        std::string s = cur.substr(b, e - b);
        for (auto& c : s)
            if (c >= 'A' && c <= 'Z') c = (char)(c - 'A' + 'a');
        // page markers (@012a) and pure numbers are not text
        if (!s.empty() && s[0] != '@' &&
            s.find_first_not_of("0123456789") != std::string::npos)
            out.push_back(s);
        cur.clear();
    };
    for (char c : wylie + " ") {
        if (c == ' ' || c == '\t' || c == '\n' || c == '\r' || c == '/' ||
            c == '|' || c == ',' || c == ';')
            flush();
        else
            cur += c;
    }
    return out;
}

}  // namespace

std::vector<QuotationMatch> detectQuotations(const Spine& spine,
                                             const std::string& passage,
                                             bool passage_is_acip,
                                             int min_syllables) {
    std::vector<QuotationMatch> matches;
    if (min_syllables < 3) min_syllables = 3;
    const std::string wylie =
        passage_is_acip ? acipToEwts(passage) : passage;
    const auto syl = syllables(wylie);
    const int n = (int)syl.size();

    for (int i = 0; i + min_syllables <= n;) {
        // FTS phrase query over the minimum window
        std::string phrase = "\"";
        for (int k = 0; k < min_syllables; ++k) {
            if (k) phrase += ' ';
            phrase += syl[i + k];
        }
        phrase += "\"";
        QuotationMatch best;
        for (const auto& seg : spine.corpusSearch(phrase, "", 20)) {
            // verify + extend on the segment's own syllables (FTS
            // tokenization is looser than ours; the corpus text decides)
            const auto ss = syllables(seg.wylie);
            for (size_t p = 0; p + min_syllables <= ss.size(); ++p) {
                int len = 0;
                while (i + len < n && p + len < ss.size() &&
                       syl[i + len] == ss[p + len])
                    ++len;
                if (len >= min_syllables && len > best.syllable_count) {
                    best.segment_id = seg.id;
                    best.course = seg.course;
                    best.seq = seg.seq;
                    best.english = seg.english;
                    best.start_syllable = i;
                    best.syllable_count = len;
                    std::string span;
                    for (int k = 0; k < len; ++k) {
                        if (k) span += ' ';
                        span += ss[p + k];
                    }
                    best.matched_wylie = span;
                }
            }
        }
        if (best.syllable_count > 0) {
            matches.push_back(best);
            i += best.syllable_count;   // non-overlapping, greedy-longest
        } else {
            ++i;
        }
    }
    // merge across segment boundaries: consecutive lines of the same
    // course matched back-to-back in the passage are ONE quotation
    std::vector<QuotationMatch> merged;
    for (auto& m : matches) {
        if (!merged.empty()) {
            auto& p = merged.back();
            if (p.course == m.course && m.seq == p.seq + 1 &&
                m.start_syllable ==
                    p.start_syllable + p.syllable_count) {
                p.seq = m.seq;   // range end; start stays reported first
                p.syllable_count += m.syllable_count;
                p.matched_wylie += " / " + m.matched_wylie;
                if (!m.english.empty())
                    p.english += (p.english.empty() ? "" : " ") + m.english;
                continue;
            }
        }
        merged.push_back(m);
    }
    return merged;
}

}  // namespace allcore
