// allcore/stardict.cpp — see the header for scope. Format facts from
// the StarDict format spec (idx: word\0 + 32-bit BE offset + size);
// verified against generated fixtures in stardict_smoke.
#include "allcore/stardict.h"

#include <zlib.h>

#include <algorithm>
#include <cctype>
#include <cstring>
#include <fstream>
#include <sstream>

namespace allcore {
namespace {

std::string lower(const std::string& s) {
    std::string o = s;
    for (char& c : o)
        c = (char)std::tolower((unsigned char)c);
    return o;
}

bool readFile(const std::string& path, std::string& out) {
    std::ifstream f(path, std::ios::binary);
    if (!f) return false;
    std::ostringstream ss;
    ss << f.rdbuf();
    out = ss.str();
    return true;
}

// full gzip/dictzip decompress (dictzip is a valid gzip stream when
// read end-to-end)
bool gunzipAll(const std::string& in, std::string& out) {
    z_stream zs;
    std::memset(&zs, 0, sizeof zs);
    if (inflateInit2(&zs, 15 + 32) != Z_OK) return false;
    zs.next_in = (Bytef*)in.data();
    zs.avail_in = (uInt)in.size();
    char buf[1 << 16];
    int rc = Z_OK;
    while (rc != Z_STREAM_END) {
        zs.next_out = (Bytef*)buf;
        zs.avail_out = sizeof buf;
        rc = inflate(&zs, Z_NO_FLUSH);
        if (rc != Z_OK && rc != Z_STREAM_END) {
            inflateEnd(&zs);
            return false;
        }
        out.append(buf, sizeof buf - zs.avail_out);
    }
    inflateEnd(&zs);
    return true;
}

}  // namespace

StarDict::StarDict(const std::string& ifo_path) {
    std::string ifo;
    if (!readFile(ifo_path, ifo)) {
        error_ = "cannot read " + ifo_path;
        return;
    }
    // key=value lines; the magic first line is ignored
    std::istringstream ls(ifo);
    std::string line;
    while (std::getline(ls, line)) {
        if (!line.empty() && line.back() == '\r') line.pop_back();
        const auto eq = line.find('=');
        if (eq == std::string::npos) continue;
        const std::string k = line.substr(0, eq);
        const std::string v = line.substr(eq + 1);
        if (k == "bookname") bookname_ = v;
        else if (k == "sametypesequence") sametypesequence_ = v;
        else if (k == "idxoffsetbits" && v == "64") {
            error_ = "idxoffsetbits=64 not supported (refused, "
                     "not guessed)";
            return;
        }
    }
    const std::string base =
        ifo_path.substr(0, ifo_path.rfind('.'));
    std::string idx;
    if (!readFile(base + ".idx", idx)) {
        std::string gz;
        if (readFile(base + ".idx.gz", gz) && gunzipAll(gz, idx)) {
        } else {
            error_ = "cannot read " + base + ".idx";
            return;
        }
    }
    // parse idx: word\0 off(4,BE) size(4,BE)
    size_t i = 0;
    while (i < idx.size()) {
        const size_t z = idx.find('\0', i);
        if (z == std::string::npos || z + 8 > idx.size()) break;
        Entry e;
        e.word = idx.substr(i, z - i);
        const unsigned char* p = (const unsigned char*)idx.data() + z + 1;
        e.off = (uint32_t(p[0]) << 24) | (uint32_t(p[1]) << 16) |
                (uint32_t(p[2]) << 8) | uint32_t(p[3]);
        e.size = (uint32_t(p[4]) << 24) | (uint32_t(p[5]) << 16) |
                 (uint32_t(p[6]) << 8) | uint32_t(p[7]);
        entries_.push_back(std::move(e));
        i = z + 9;
    }
    if (entries_.empty()) {
        error_ = "empty or unparsable idx";
        return;
    }
    if (!readFile(base + ".dict", dict_)) {
        std::string dz;
        if (!readFile(base + ".dict.dz", dz) ||
            !gunzipAll(dz, dict_)) {
            error_ = "cannot read " + base + ".dict[.dz]";
            return;
        }
    }
    sorted_.resize(entries_.size());
    for (size_t k = 0; k < sorted_.size(); ++k) sorted_[k] = k;
    std::stable_sort(sorted_.begin(), sorted_.end(),
                     [this](size_t a, size_t b) {
                         return lower(entries_[a].word) <
                                lower(entries_[b].word);
                     });
    ok_ = true;
}

std::string StarDict::defText(const Entry& e) const {
    if (e.off + (uint64_t)e.size > dict_.size()) return {};
    const std::string raw = dict_.substr(e.off, e.size);
    // sametypesequence of pure text kinds → the block IS the text
    if (!sametypesequence_.empty()) {
        bool textual = true;
        for (char c : sametypesequence_)
            textual &= (std::strchr("mlgtxykwh", c) != nullptr);
        if (textual) return raw;
    }
    // typed segments: lowercase type = null-terminated text;
    // uppercase = 4-byte-size binary (skipped)
    std::string out;
    size_t i = 0;
    while (i < raw.size()) {
        const char t = raw[i++];
        if (std::isupper((unsigned char)t)) {
            if (i + 4 > raw.size()) break;
            const unsigned char* p = (const unsigned char*)raw.data() + i;
            const uint32_t n = (uint32_t(p[0]) << 24) |
                               (uint32_t(p[1]) << 16) |
                               (uint32_t(p[2]) << 8) | uint32_t(p[3]);
            i += 4 + n;
            continue;
        }
        const size_t z = raw.find('\0', i);
        const std::string seg =
            raw.substr(i, z == std::string::npos ? std::string::npos
                                                 : z - i);
        if (std::strchr("mlgtxykwh", t)) {
            if (!out.empty()) out += "\n";
            out += seg;
        }
        if (z == std::string::npos) break;
        i = z + 1;
    }
    return out;
}

std::vector<std::string> StarDict::lookup(
    const std::string& word) const {
    std::vector<std::string> out;
    const std::string w = lower(word);
    auto it = std::lower_bound(
        sorted_.begin(), sorted_.end(), w,
        [this](size_t a, const std::string& v) {
            return lower(entries_[a].word) < v;
        });
    for (; it != sorted_.end() && lower(entries_[*it].word) == w;
         ++it) {
        std::string d = defText(entries_[*it]);
        if (!d.empty()) out.push_back(std::move(d));
    }
    return out;
}

std::vector<std::string> StarDict::prefix(const std::string& p,
                                          int limit) const {
    std::vector<std::string> out;
    const std::string w = lower(p);
    auto it = std::lower_bound(
        sorted_.begin(), sorted_.end(), w,
        [this](size_t a, const std::string& v) {
            return lower(entries_[a].word) < v;
        });
    for (; it != sorted_.end() && (int)out.size() < limit; ++it) {
        const std::string lw = lower(entries_[*it].word);
        if (lw.compare(0, w.size(), w) != 0) break;
        out.push_back(entries_[*it].word);
    }
    return out;
}

}  // namespace allcore
