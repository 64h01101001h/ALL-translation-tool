#include "allcore/chantline.h"

#include <cctype>
#include <set>
#include <string>
#include <vector>

#include "allcore/engines.h"

namespace allcore {
namespace {

std::vector<std::string> words(const std::string& s) {
    std::vector<std::string> out;
    std::string cur;
    for (char c : s) {
        if (std::isalpha((unsigned char)c) || c == '\'')
            cur += (char)std::tolower((unsigned char)c);
        else {
            if (cur.size() >= 2) out.push_back(cur);
            cur.clear();
        }
    }
    if (cur.size() >= 2) out.push_back(cur);
    return out;
}

// The commonest English function words. A sentence of four or more words with
// none of these is not doing English grammar.
const std::set<std::string>& stop() {
    static const std::set<std::string> s = {
        "the","a","an","of","to","and","in","is","are","was","were","that",
        "this","it","for","with","as","on","at","by","be","been","from","or",
        "not","you","we","they","he","she","his","her","its","their","our",
        "my","i","but","if","so","then","than","which","who","what","when",
        "where","how","all","any","can","will","would","should","may","might",
        "there","here","do","does","did","have","has","had"};
    return s;
}

}  // namespace

bool carriesNoEnglishFunctionWord(const std::string& english) {
    const auto w = words(english);
    if (w.size() < 4) return false;
    for (const auto& t : w)
        if (stop().count(t)) return false;
    return true;
}

bool soundsLikeItsOwnTibetan(const std::string& english,
                             const std::string& acip) {
    if (acip.empty() || english.empty()) return false;
    const auto ew = words(english);
    if (ew.size() < 4) return false;
    std::set<std::string> pron;
    for (const auto& t : words(pronounce(acipToEwts(acip)))) pron.insert(t);
    if (pron.empty()) return false;
    int hit = 0;
    for (const auto& t : ew)
        if (pron.count(t)) ++hit;
    return (double)hit / (double)ew.size() >= 0.6;
}

bool englishIsNotEnglish(const std::string& english, const std::string& acip) {
    return carriesNoEnglishFunctionWord(english) ||
           soundsLikeItsOwnTibetan(english, acip);
}

}  // namespace allcore
