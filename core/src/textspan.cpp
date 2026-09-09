#include "allcore/textspan.h"

#include <cctype>
#include <regex>

namespace allcore {
namespace textspan {

static const std::regex& folioRe() {
    static const std::regex re(kFolioMarkerPattern);
    return re;
}

std::vector<FolioMarker> folioMarkers(const std::string& line) {
    std::vector<FolioMarker> out;
    for (auto it = std::sregex_iterator(line.begin(), line.end(), folioRe()); it != std::sregex_iterator(); ++it) {
        FolioMarker m; m.begin = (int)it->position(0); m.end = m.begin + (int)it->length(0);
        m.id = (*it)[1].str();
        for (auto& c : m.id) c = (char)std::toupper((unsigned char)c);
        out.push_back(m);
    }
    return out;
}

int folioMarkerEnd(const std::string& line, std::string* folioOut) {
    const auto ms = folioMarkers(line);
    if (ms.empty()) return -1;
    if (folioOut) *folioOut = ms.front().id;
    return ms.front().end;
}

std::string lastFolio(const std::string& line) {
    const auto ms = folioMarkers(line);
    return ms.empty() ? std::string() : ms.back().id;
}

bool folioParts(const std::string& id, int& number, char& side, int& digitWidth) {
    if (id.empty()) return false;
    size_t i = 0; std::string digits;
    while (i < id.size() && std::isdigit((unsigned char)id[i])) digits += id[i++];
    if (digits.empty()) return false;
    number = std::stoi(digits); digitWidth = (int)digits.size();
    side = i < id.size() ? (char)std::toupper((unsigned char)id[i]) : '\0';
    return true;
}

std::vector<Span> apparatusSpans(const std::string& line, bool* balanced) {
    std::vector<Span> out; bool ok = true;
    int openAt = -1; char openKind = 0;
    for (int i = 0; i < (int)line.size(); ++i) {
        const char c = line[i];
        if (c == '[' || c == '{') {
            if (openAt >= 0) { ok = false; break; }   // nested or unclosed → refuse
            openAt = i; openKind = c;
        } else if (c == ']' || c == '}') {
            const char want = c == ']' ? '[' : '{';
            if (openAt < 0 || openKind != want) { ok = false; break; }
            out.push_back({openAt, i + 1, openKind}); openAt = -1; openKind = 0;
        }
    }
    if (openAt >= 0) ok = false;
    if (balanced) *balanced = ok;
    if (!ok) out.clear();
    return out;
}

std::string stripApparatus(const std::string& line, bool* balanced) {
    bool ok = true;
    const auto spans = apparatusSpans(line, &ok);
    if (balanced) *balanced = ok;
    if (!ok) return line;
    std::string o; int pos = 0;
    for (const auto& s : spans) { o.append(line, pos, s.begin - pos); pos = s.end; }
    o.append(line, pos, std::string::npos);
    std::string c; bool ws = false;
    for (unsigned char ch : o) { if (std::isspace(ch)) { ws = true; continue; } if (ws && !c.empty()) c += ' '; ws = false; c += (char)ch; }
    return c;
}

}  // namespace textspan
}  // namespace allcore
