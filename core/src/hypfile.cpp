#include "allcore/hypfile.h"

#include <algorithm>
#include <cctype>
#include <map>

namespace allcore {

HypFile parseHypFile(const std::string& raw) {
    HypFile out;
    char mode = 'e';   // tutorial.hyp opens in English; <t>/<e> switch
    struct Open {
        int id;
        char mode;
        int beg;   // buffer length at open, in the opening mode's buffer
    };
    std::vector<Open> stack;
    std::map<int, HypLink> byId;

    auto buf = [&](char m) -> std::string& {
        return m == 't' ? out.tibetan : out.english;
    };

    for (size_t i = 0; i < raw.size();) {
        const char c = raw[i];
        if (c == '<') {
            // <t> / <e> — mode switch
            if (i + 2 < raw.size() && (raw[i + 1] == 't' || raw[i + 1] == 'e') &&
                raw[i + 2] == '>') {
                mode = raw[i + 1];
                i += 3;
                continue;
            }
            // <t$> / <e$> — literal escape for the tag text itself
            if (i + 3 < raw.size() && (raw[i + 1] == 't' || raw[i + 1] == 'e') &&
                raw[i + 2] == '$' && raw[i + 3] == '>') {
                buf(mode) += std::string("<") + raw[i + 1] + ">";
                i += 4;
                continue;
            }
            // <fs…> — font styling, stripped
            if (i + 2 < raw.size() && raw[i + 1] == 'f' && raw[i + 2] == 's') {
                size_t j = raw.find('>', i);
                if (j != std::string::npos) {
                    i = j + 1;
                    continue;
                }
            }
            // <!…> — a note (annotation): collected, not text
            if (i + 1 < raw.size() && raw[i + 1] == '!') {
                size_t j = raw.find('>', i);
                if (j != std::string::npos) {
                    out.notes.push_back(raw.substr(i + 2, j - i - 2));
                    i = j + 1;
                    continue;
                }
            }
            // <N … — numbered link open (digits then a space)
            {
                size_t j = i + 1;
                while (j < raw.size() &&
                       std::isdigit(static_cast<unsigned char>(raw[j])))
                    ++j;
                if (j > i + 1 && j < raw.size() && raw[j] == ' ') {
                    const int id = std::stoi(raw.substr(i + 1, j - i - 1));
                    stack.push_back(
                        {id, mode, static_cast<int>(buf(mode).size())});
                    i = j + 1;
                    continue;
                }
            }
            // anything else tag-like: literal, counted
            ++out.unknownTags;
            buf(mode) += c;
            ++i;
            continue;
        }
        if (c == '>' && !stack.empty()) {
            const Open o = stack.back();
            stack.pop_back();
            HypLink& l = byId[o.id];
            l.id = o.id;
            const int end = static_cast<int>(buf(o.mode).size());
            if (o.mode == 't') {
                l.tibBeg = o.beg;
                l.tibEnd = end;
            } else {
                l.engBeg = o.beg;
                l.engEnd = end;
            }
            ++i;
            continue;
        }
        buf(mode) += c;
        ++i;
    }
    // unclosed opens: close at end of their buffer (surfaced as normal
    // links — the span is what the file actually delimited)
    while (!stack.empty()) {
        const Open o = stack.back();
        stack.pop_back();
        HypLink& l = byId[o.id];
        l.id = o.id;
        const int end = static_cast<int>(buf(o.mode).size());
        if (o.mode == 't') {
            l.tibBeg = o.beg;
            l.tibEnd = end;
        } else {
            l.engBeg = o.beg;
            l.engEnd = end;
        }
        ++out.unknownTags;   // an unclosed link is a defect worth noticing
    }

    for (auto& [id, l] : byId) {
        if (l.tibBeg >= 0 && l.engBeg >= 0)
            out.links.push_back(l);
        else
            out.onesided.push_back(l);
    }
    auto byIdLess = [](const HypLink& a, const HypLink& b) {
        return a.id < b.id;
    };
    std::sort(out.links.begin(), out.links.end(), byIdLess);
    std::sort(out.onesided.begin(), out.onesided.end(), byIdLess);
    return out;
}

}  // namespace allcore
