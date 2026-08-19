// tree_diff.cpp — see allcore/tree_diff.h.
#include "allcore/tree_diff.h"

#include <algorithm>
#include <cstdint>
#include <filesystem>
#include <fstream>
#include <map>
#include <unordered_map>

namespace allcore {
namespace {

struct FileSig {
    std::uintmax_t size = 0;
    std::uint64_t sample = 0;   // FNV-1a over head 4K + tail 4K + size
};

std::uint64_t fnv1a(std::uint64_t h, const char* p, size_t n) {
    for (size_t i = 0; i < n; ++i) {
        h ^= static_cast<unsigned char>(p[i]);
        h *= 1099511628211ULL;
    }
    return h;
}

FileSig signature(const std::filesystem::path& p, std::uintmax_t size) {
    FileSig s;
    s.size = size;
    std::ifstream f(p, std::ios::binary);
    char buf[4096];
    std::uint64_t h = 1469598103934665603ULL;
    f.read(buf, sizeof buf);
    h = fnv1a(h, buf, static_cast<size_t>(f.gcount()));
    if (size > sizeof buf) {
        f.clear();
        f.seekg(-static_cast<std::streamoff>(
                    std::min<std::uintmax_t>(size, sizeof buf)),
                std::ios::end);
        f.read(buf, sizeof buf);
        h = fnv1a(h, buf, static_cast<size_t>(f.gcount()));
    }
    h = fnv1a(h, reinterpret_cast<const char*>(&size), sizeof size);
    s.sample = h;
    return s;
}

std::map<std::string, FileSig> walk(const std::string& root) {
    namespace fs = std::filesystem;
    std::map<std::string, FileSig> out;
    std::error_code ec;
    if (!fs::exists(root, ec)) return out;
    for (fs::recursive_directory_iterator it(
             root, fs::directory_options::skip_permission_denied, ec), end;
         it != end; it.increment(ec)) {
        if (ec) break;
        if (!it->is_regular_file(ec)) continue;
        const auto size = it->file_size(ec);
        if (ec) continue;
        const std::string rel =
            fs::relative(it->path(), root, ec).string();
        if (ec || rel.empty()) continue;
        out[rel] = signature(it->path(), size);
    }
    return out;
}

}  // namespace

TreeDiffResult diffTrees(const std::string& left_root,
                         const std::string& right_root) {
    TreeDiffResult r;
    const auto L = walk(left_root);
    const auto R = walk(right_root);
    r.files_left = static_cast<int>(L.size());
    r.files_right = static_cast<int>(R.size());

    std::vector<std::pair<std::string, FileSig>> onlyL, onlyR;
    for (const auto& [rel, sig] : L) {
        const auto it = R.find(rel);
        if (it == R.end()) {
            onlyL.push_back({rel, sig});
        } else if (it->second.size == sig.size &&
                   it->second.sample == sig.sample) {
            ++r.identical;
        } else {
            r.modified.push_back(rel);
        }
    }
    for (const auto& [rel, sig] : R)
        if (!L.count(rel)) onlyR.push_back({rel, sig});

    // rename detection: same size + sampled content, different path.
    // Ambiguity is not guessed at: a signature present more than once
    // on either side stays added/removed.
    std::unordered_map<std::uint64_t, int> countL, countR;
    for (const auto& [rel, sig] : onlyL) ++countL[sig.sample];
    for (const auto& [rel, sig] : onlyR) ++countR[sig.sample];
    std::unordered_map<std::uint64_t, std::string> rightBySig;
    for (const auto& [rel, sig] : onlyR)
        if (countR[sig.sample] == 1) rightBySig[sig.sample] = rel;
    std::vector<std::string> stillL;
    std::map<std::string, bool> takenR;
    for (const auto& [rel, sig] : onlyL) {
        const auto it = rightBySig.find(sig.sample);
        if (countL[sig.sample] == 1 && it != rightBySig.end() &&
            !takenR[it->second]) {
            r.renamed.push_back({rel, it->second});
            takenR[it->second] = true;
        } else {
            stillL.push_back(rel);
        }
    }
    for (auto& s : stillL) r.removed.push_back(std::move(s));
    for (const auto& [rel, sig] : onlyR)
        if (!takenR[rel]) r.added.push_back(rel);

    std::sort(r.added.begin(), r.added.end());
    std::sort(r.removed.begin(), r.removed.end());
    std::sort(r.modified.begin(), r.modified.end());
    std::sort(r.renamed.begin(), r.renamed.end());
    return r;
}

}  // namespace allcore
