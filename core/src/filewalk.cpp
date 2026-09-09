#include "allcore/filewalk.h"

#include <algorithm>
#include <cctype>

namespace allcore {
namespace fs = std::filesystem;

bool eligibleTextFile(const fs::path& p, std::uintmax_t size, std::uintmax_t maxBytes) {
    if (size > maxBytes) return false;
    std::string ext = p.extension().string();
    for (auto& c : ext) c = (char)std::tolower((unsigned char)c);
    return ext == ".txt" || ext == ".acip" || ext == ".md" || ext == ".act" || ext == ".inc" || ext == ".ace";
}

// Written from the fnmatch(3) description: '*' any run without '/', '?' one
// char that is not '/', '[...]' a class with optional leading '!' or '^'
// and ranges; "**" any run including '/'. Case-insensitive on letters.
static bool matchAt(const std::string& pat, size_t pi, const std::string& s, size_t si) {
    while (pi < pat.size()) {
        const char pc = pat[pi];
        if (pc == '*') {
            const bool deep = pi + 1 < pat.size() && pat[pi + 1] == '*';
            size_t next = deep ? pi + 2 : pi + 1;
            if (deep && next < pat.size() && pat[next] == '/') ++next;   // "**/" also matches zero dirs
            for (size_t k = si; k <= s.size(); ++k) {
                if (matchAt(pat, next, s, k)) return true;
                if (k < s.size() && !deep && s[k] == '/') break;
            }
            return false;
        }
        if (si >= s.size()) return false;
        if (pc == '?') { if (s[si] == '/') return false; ++pi; ++si; continue; }
        if (pc == '[') {
            size_t close = pat.find(']', pi + 2);
            if (close == std::string::npos) return false;
            bool neg = pat[pi + 1] == '!' || pat[pi + 1] == '^';
            size_t j = pi + 1 + (neg ? 1 : 0); bool hit = false;
            const char c = (char)std::tolower((unsigned char)s[si]);
            while (j < close) {
                char lo = (char)std::tolower((unsigned char)pat[j]);
                if (j + 2 < close && pat[j + 1] == '-') { char hi = (char)std::tolower((unsigned char)pat[j + 2]); if (c >= lo && c <= hi) hit = true; j += 3; }
                else { if (c == lo) hit = true; ++j; }
            }
            if (hit == neg) return false;
            pi = close + 1; ++si; continue;
        }
        if (std::tolower((unsigned char)pc) != std::tolower((unsigned char)s[si])) return false;
        ++pi; ++si;
    }
    return si == s.size();
}

bool globMatch(const std::string& pattern, const std::string& relPath) {
    if (pattern.empty()) return false;
    // a pattern without '/' matches the file name in any directory
    if (pattern.find('/') == std::string::npos) {
        const size_t slash = relPath.rfind('/');
        return matchAt(pattern, 0, slash == std::string::npos ? relPath : relPath.substr(slash + 1), 0);
    }
    return matchAt(pattern, 0, relPath, 0);
}

static std::vector<std::string> splitGlobs(const std::string& s) {
    std::vector<std::string> out; std::string cur;
    for (char c : s) { if (c == ';') { if (!cur.empty()) out.push_back(cur); cur.clear(); } else if (!std::isspace((unsigned char)c) || !cur.empty()) cur += c; }
    if (!cur.empty()) out.push_back(cur);
    for (auto& g : out) while (!g.empty() && std::isspace((unsigned char)g.back())) g.pop_back();
    return out;
}

std::vector<std::string> listEligibleFiles(const std::string& root, const std::string& includeGlobs,
                                           const std::string& excludeGlobs, bool recurse, int fileCap,
                                           WalkStats* stats, const std::function<bool()>& keepGoing) {
    WalkStats local; WalkStats& st = stats ? *stats : local; st = WalkStats{};
    std::vector<std::string> includes, excludes;
    for (const auto& g : splitGlobs(includeGlobs)) { if (!g.empty() && g[0] == '!') excludes.push_back(g.substr(1)); else if (g != "*") includes.push_back(g); }
    for (const auto& g : splitGlobs(excludeGlobs)) excludes.push_back(g[0] == '!' ? g.substr(1) : g);
    std::vector<std::string> out;
    std::error_code ec;
    const fs::path rootP(root);
    if (!fs::is_directory(rootP, ec)) return out;
    auto consider = [&](const fs::directory_entry& e) {
        if (keepGoing && !keepGoing()) return false;
        std::error_code ec2;
        if (!e.is_regular_file(ec2)) return true;
        ++st.scanned;
        const auto size = e.file_size(ec2);
        if (ec2) { ++st.unreadable; return true; }
        std::string ext = e.path().extension().string();
        for (auto& c : ext) c = (char)std::tolower((unsigned char)c);
        const bool extOk = ext == ".txt" || ext == ".acip" || ext == ".md" || ext == ".act" || ext == ".inc" || ext == ".ace";
        if (!extOk) { ++st.ineligibleExt; return true; }
        if (!eligibleTextFile(e.path(), size)) { ++st.tooLarge; return true; }
        std::string rel = fs::relative(e.path(), rootP, ec2).generic_string();
        if (ec2) rel = e.path().filename().string();
        bool inc = includes.empty();
        for (const auto& g : includes) if (globMatch(g, rel)) { inc = true; break; }
        bool exc = false;
        for (const auto& g : excludes) if (globMatch(g, rel)) { exc = true; break; }
        if (!inc || exc) { ++st.excluded; return true; }
        if (fileCap > 0 && (int)out.size() >= fileCap) { ++st.capped; return true; }
        out.push_back(e.path().string());
        return true;
    };
    if (recurse) {
        for (auto it = fs::recursive_directory_iterator(rootP, fs::directory_options::skip_permission_denied, ec); it != fs::recursive_directory_iterator(); it.increment(ec)) {
            if (ec) { ec.clear(); continue; }
            if (!consider(*it)) break;
        }
    } else {
        for (auto it = fs::directory_iterator(rootP, fs::directory_options::skip_permission_denied, ec); it != fs::directory_iterator(); it.increment(ec)) {
            if (ec) { ec.clear(); continue; }
            if (!consider(*it)) break;
        }
    }
    std::sort(out.begin(), out.end());
    return out;
}

}  // namespace allcore
