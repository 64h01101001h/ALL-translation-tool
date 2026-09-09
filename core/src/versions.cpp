#include "allcore/versions.h"

#include <algorithm>
#include <cstdint>
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <set>
#include <utility>

namespace allcore::versions {
namespace fs = std::filesystem;

// ---------------------------------------------------------------- SHA-1 ----
// RFC 3174 / FIPS 180-4, written from the standard's description. Full
// 64-byte blocks are hashed straight from the input; only the padded tail
// (one or two blocks) is copied, so a 10 MB document costs no second copy.
namespace {

inline uint32_t rol(uint32_t x, int n) { return (x << n) | (x >> (32 - n)); }

void sha1Block(uint32_t h[5], const unsigned char* p) {
    uint32_t w[80];
    for (int t = 0; t < 16; ++t)
        w[t] = (uint32_t(p[4 * t]) << 24) | (uint32_t(p[4 * t + 1]) << 16) |
               (uint32_t(p[4 * t + 2]) << 8) | uint32_t(p[4 * t + 3]);
    for (int t = 16; t < 80; ++t) w[t] = rol(w[t - 3] ^ w[t - 8] ^ w[t - 14] ^ w[t - 16], 1);
    uint32_t a = h[0], b = h[1], c = h[2], d = h[3], e = h[4];
    for (int t = 0; t < 80; ++t) {
        uint32_t f, k;
        if (t < 20)      { f = (b & c) | (~b & d);            k = 0x5A827999u; }
        else if (t < 40) { f = b ^ c ^ d;                     k = 0x6ED9EBA1u; }
        else if (t < 60) { f = (b & c) | (b & d) | (c & d);   k = 0x8F1BBCDCu; }
        else             { f = b ^ c ^ d;                     k = 0xCA62C1D6u; }
        const uint32_t tmp = rol(a, 5) + f + e + k + w[t];
        e = d; d = c; c = rol(b, 30); b = a; a = tmp;
    }
    h[0] += a; h[1] += b; h[2] += c; h[3] += d; h[4] += e;
}

}  // namespace

std::string sha1Hex(const std::string& bytes) {
    uint32_t h[5] = {0x67452301u, 0xEFCDAB89u, 0x98BADCFEu, 0x10325476u, 0xC3D2E1F0u};
    const auto* data = reinterpret_cast<const unsigned char*>(bytes.data());
    const size_t n = bytes.size();
    size_t off = 0;
    for (; off + 64 <= n; off += 64) sha1Block(h, data + off);
    // tail: remaining bytes, 0x80, zeros to 56 mod 64, then the bit length big-endian
    unsigned char tail[128] = {0};
    const size_t rem = n - off;
    for (size_t i = 0; i < rem; ++i) tail[i] = data[off + i];
    tail[rem] = 0x80;
    const size_t tailLen = rem < 56 ? 64 : 128;
    const uint64_t bits = uint64_t(n) * 8u;
    for (int i = 0; i < 8; ++i) tail[tailLen - 1 - size_t(i)] = (unsigned char)((bits >> (8 * i)) & 0xFFu);
    sha1Block(h, tail);
    if (tailLen == 128) sha1Block(h, tail + 64);
    static const char* hex = "0123456789abcdef";
    std::string out;
    out.reserve(40);
    for (uint32_t v : h)
        for (int s = 28; s >= 0; s -= 4) out += hex[(v >> s) & 0xFu];
    return out;
}

// ------------------------------------------------------------ flat JSON ----
namespace {

// One parsed value. type: 's' decoded string, 'n' number text, 'b' "true"/"false",
// '0' null, 'o' raw object text, 'a' raw array text.
struct JVal { char type = '?'; std::string text; };
using JObj = std::vector<std::pair<std::string, JVal>>;

class Reader {
public:
    explicit Reader(const std::string& s) : s_(s) {}

    // A complete object with nothing but whitespace after it.
    bool object(JObj& out) {
        ws();
        if (!eat('{')) return false;
        ws();
        if (eat('}')) { ws(); return i_ == s_.size(); }
        for (;;) {
            std::string key;
            if (!string(key)) return false;
            ws();
            if (!eat(':')) return false;
            ws();
            JVal v;
            if (!value(v)) return false;
            out.emplace_back(std::move(key), std::move(v));
            ws();
            if (eat(',')) { ws(); continue; }
            if (eat('}')) { ws(); return i_ == s_.size(); }
            return false;
        }
    }

    // A single balanced object/array occupying the whole string.
    bool wholeContainer(char open) {
        ws();
        if (i_ >= s_.size() || s_[i_] != open) return false;
        JVal v;
        if (!balanced(v)) return false;
        ws();
        return i_ == s_.size();
    }

private:
    const std::string& s_;
    size_t i_ = 0;

    void ws() { while (i_ < s_.size() && (s_[i_] == ' ' || s_[i_] == '\t' || s_[i_] == '\n' || s_[i_] == '\r')) ++i_; }
    bool eat(char c) { if (i_ < s_.size() && s_[i_] == c) { ++i_; return true; } return false; }

    bool hex4(uint32_t& cp) {
        if (i_ + 4 > s_.size()) return false;
        cp = 0;
        for (int k = 0; k < 4; ++k) {
            const char c = s_[i_++];
            uint32_t d;
            if (c >= '0' && c <= '9') d = uint32_t(c - '0');
            else if (c >= 'a' && c <= 'f') d = uint32_t(c - 'a' + 10);
            else if (c >= 'A' && c <= 'F') d = uint32_t(c - 'A' + 10);
            else return false;
            cp = (cp << 4) | d;
        }
        return true;
    }
    static void utf8(std::string& out, uint32_t cp) {
        if (cp < 0x80) out += char(cp);
        else if (cp < 0x800) { out += char(0xC0 | (cp >> 6)); out += char(0x80 | (cp & 0x3F)); }
        else if (cp < 0x10000) { out += char(0xE0 | (cp >> 12)); out += char(0x80 | ((cp >> 6) & 0x3F)); out += char(0x80 | (cp & 0x3F)); }
        else { out += char(0xF0 | (cp >> 18)); out += char(0x80 | ((cp >> 12) & 0x3F)); out += char(0x80 | ((cp >> 6) & 0x3F)); out += char(0x80 | (cp & 0x3F)); }
    }
    bool string(std::string& out) {
        if (!eat('"')) return false;
        while (i_ < s_.size()) {
            const char c = s_[i_++];
            if (c == '"') return true;
            if (c == '\\') {
                if (i_ >= s_.size()) return false;
                const char e = s_[i_++];
                switch (e) {
                    case '"': out += '"'; break;
                    case '\\': out += '\\'; break;
                    case '/': out += '/'; break;
                    case 'b': out += '\b'; break;
                    case 'f': out += '\f'; break;
                    case 'n': out += '\n'; break;
                    case 'r': out += '\r'; break;
                    case 't': out += '\t'; break;
                    case 'u': {
                        uint32_t cp;
                        if (!hex4(cp)) return false;
                        if (cp >= 0xD800 && cp <= 0xDBFF) {
                            if (i_ + 1 >= s_.size() || s_[i_] != '\\' || s_[i_ + 1] != 'u') return false;
                            i_ += 2;
                            uint32_t lo;
                            if (!hex4(lo) || lo < 0xDC00 || lo > 0xDFFF) return false;
                            cp = 0x10000 + ((cp - 0xD800) << 10) + (lo - 0xDC00);
                        } else if (cp >= 0xDC00 && cp <= 0xDFFF) {
                            return false;
                        }
                        utf8(out, cp);
                        break;
                    }
                    default: return false;
                }
            } else if ((unsigned char)c < 0x20) {
                return false;   // a raw control character is not JSON
            } else {
                out += c;
            }
        }
        return false;
    }
    bool balanced(JVal& v) {
        const size_t start = i_;
        const char open = s_[i_];
        int depth = 0;
        bool inStr = false;
        while (i_ < s_.size()) {
            const char c = s_[i_++];
            if (inStr) {
                if (c == '\\') { if (i_ < s_.size()) ++i_; }
                else if (c == '"') inStr = false;
                continue;
            }
            if (c == '"') inStr = true;
            else if (c == '{' || c == '[') ++depth;
            else if (c == '}' || c == ']') {
                if (--depth == 0) {
                    v.type = open == '{' ? 'o' : 'a';
                    v.text = s_.substr(start, i_ - start);
                    return true;
                }
            }
        }
        return false;
    }
    bool literal(const char* word, char type, JVal& v) {
        const size_t n = std::char_traits<char>::length(word);
        if (s_.compare(i_, n, word) != 0) return false;
        i_ += n;
        v.type = type;
        v.text = word;
        return true;
    }
    bool value(JVal& v) {
        if (i_ >= s_.size()) return false;
        const char c = s_[i_];
        if (c == '"') { v.type = 's'; return string(v.text); }
        if (c == '{' || c == '[') return balanced(v);
        if (c == 't') return literal("true", 'b', v);
        if (c == 'f') return literal("false", 'b', v);
        if (c == 'n') return literal("null", '0', v);
        const size_t start = i_;
        while (i_ < s_.size() && ((s_[i_] >= '0' && s_[i_] <= '9') || s_[i_] == '-' || s_[i_] == '+' || s_[i_] == '.' || s_[i_] == 'e' || s_[i_] == 'E')) ++i_;
        if (i_ == start) return false;
        v.type = 'n';
        v.text = s_.substr(start, i_ - start);
        return true;
    }
};

std::string quote(const std::string& s) {
    static const char* hex = "0123456789abcdef";
    std::string out;
    out.reserve(s.size() + 2);
    out += '"';
    for (const char c : s) {
        switch (c) {
            case '"': out += "\\\""; break;
            case '\\': out += "\\\\"; break;
            case '\n': out += "\\n"; break;
            case '\t': out += "\\t"; break;
            case '\r': out += "\\r"; break;
            case '\b': out += "\\b"; break;
            case '\f': out += "\\f"; break;
            default:
                if ((unsigned char)c < 0x20) {
                    out += "\\u00";
                    out += hex[((unsigned char)c >> 4) & 0xF];
                    out += hex[(unsigned char)c & 0xF];
                } else {
                    out += c;   // UTF-8 passes through verbatim
                }
        }
    }
    out += '"';
    return out;
}

bool isJsonObjectText(const std::string& s) {
    Reader r(s);
    return r.wholeContainer('{');
}

bool parseFlatObject(const std::string& json, JObj& out) {
    Reader r(json);
    return r.object(out);
}

const JVal* lastKey(const JObj& o, const char* key) {
    const JVal* v = nullptr;
    for (const auto& kv : o) if (kv.first == key) v = &kv.second;
    return v;
}

long long toInt(const std::string& numText) {
    return std::strtoll(numText.c_str(), nullptr, 10);
}

}  // namespace

std::string serializeMeta(const Meta& m) {
    std::vector<std::string> lines;
    auto str = [&](const char* k, const std::string& v) { lines.push_back(std::string("  \"") + k + "\": " + quote(v)); };
    auto num = [&](const char* k, long long v) { lines.push_back(std::string("  \"") + k + "\": " + std::to_string(v)); };
    auto boolean = [&](const char* k, bool v) { lines.push_back(std::string("  \"") + k + "\": " + (v ? "true" : "false")); };
    str("schema", "all-version/1");
    str("stamp", m.stamp);
    str("path", m.path);
    str("kind", m.kind);
    str("reason", m.reason);
    str("changeset", m.changeset);
    num("revision", m.revision);
    num("bytes", m.bytes);
    str("codec", m.codec);
    str("sha1", m.sha1);
    str("encoding", m.encoding);
    str("lineEnding", m.lineEnding);
    boolean("bom", m.bom);
    str("label", m.label);
    boolean("pinned", m.pinned);
    str("savedBy", m.savedBy);
    str("savedBySource", m.savedBySource);
    str("savedAt", m.savedAt);
    str("app", m.app);
    // statistics is the caller's JSON object, carried verbatim; anything that is
    // not a complete object is recorded as null so the record stays readable.
    lines.push_back(std::string("  \"statistics\": ") +
                    (isJsonObjectText(m.statisticsJson) ? m.statisticsJson : std::string("null")));
    str("statisticsNote", m.statisticsNote);
    std::string out = "{\n";
    for (size_t i = 0; i < lines.size(); ++i) {
        out += lines[i];
        out += (i + 1 < lines.size()) ? ",\n" : "\n";
    }
    out += "}\n";
    return out;
}

bool parseMeta(const std::string& json, Meta& out) {
    JObj o;
    if (!parseFlatObject(json, o)) return false;
    const JVal* schema = lastKey(o, "schema");
    if (!schema || schema->type != 's' || schema->text != "all-version/1") return false;
    Meta m;
    auto str = [&](const char* k, std::string& dst) -> bool {
        const JVal* v = lastKey(o, k);
        if (!v) return true;
        if (v->type == 's') { dst = v->text; return true; }
        if (v->type == '0') { dst.clear(); return true; }
        return false;
    };
    auto num = [&](const char* k, long long& dst) -> bool {
        const JVal* v = lastKey(o, k);
        if (!v) return true;
        if (v->type != 'n') return false;
        dst = toInt(v->text);
        return true;
    };
    auto boolean = [&](const char* k, bool& dst) -> bool {
        const JVal* v = lastKey(o, k);
        if (!v) return true;
        if (v->type != 'b') return false;
        dst = v->text == "true";
        return true;
    };
    long long revision = 0;
    if (!str("stamp", m.stamp) || !str("path", m.path) || !str("kind", m.kind) ||
        !str("reason", m.reason) || !str("changeset", m.changeset) ||
        !num("revision", revision) || !num("bytes", m.bytes) ||
        !str("codec", m.codec) || !str("sha1", m.sha1) || !str("encoding", m.encoding) ||
        !str("lineEnding", m.lineEnding) || !boolean("bom", m.bom) ||
        !str("label", m.label) || !boolean("pinned", m.pinned) ||
        !str("savedBy", m.savedBy) || !str("savedBySource", m.savedBySource) ||
        !str("savedAt", m.savedAt) || !str("app", m.app) ||
        !str("statisticsNote", m.statisticsNote))
        return false;
    m.revision = int(revision);
    if (const JVal* st = lastKey(o, "statistics")) {
        if (st->type == 'o') m.statisticsJson = st->text;
        else if (st->type == '0') m.statisticsJson.clear();
        else return false;
    }
    m.metaOk = true;
    m.problem.clear();
    out = m;
    return true;
}

// ------------------------------------------------------------- file I/O ----
namespace {

bool slurp(const fs::path& p, std::string& out) {
    std::ifstream f(p, std::ios::binary);
    if (!f) return false;
    out.assign(std::istreambuf_iterator<char>(f), {});
    return !f.bad();
}

// temp + rename in the same directory: the target is either absent or complete.
bool writeAtomic(const fs::path& target, const std::string& bytes) {
    const fs::path tmp = target.string() + ".tmp";
    std::error_code ec;
    {
        std::ofstream f(tmp, std::ios::binary | std::ios::trunc);
        if (!f) return false;
        f.write(bytes.data(), (std::streamsize)bytes.size());
        f.flush();
        if (!f.good()) { f.close(); fs::remove(tmp, ec); return false; }
    }
    fs::rename(tmp, target, ec);
    if (ec) { fs::remove(tmp, ec); return false; }
    return true;
}

std::string lower(std::string s) {
    for (auto& c : s) if (c >= 'A' && c <= 'Z') c = char(c - 'A' + 'a');
    return s;
}

std::string normalPath(const std::string& p) {
    if (p.empty()) return p;
    std::string s = fs::path(p).lexically_normal().generic_string();
    while (s.size() > 1 && s.back() == '/') s.pop_back();
    return s;
}

bool validStamp(const std::string& s) {
    if (s.empty() || s[0] == '_' || s.size() > 120) return false;
    for (const char c : s)
        if (!((c >= '0' && c <= '9') || (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == '-' || c == '_'))
            return false;
    return true;
}

bool isPre(const std::string& reason) { return reason.rfind("pre-", 0) == 0; }

// Record first, then blob: a half-finished removal leaves an orphan .ver
// (counted, never offered) rather than a record that claims a missing blob.
void removeEntry(const Entry& e) {
    std::error_code ec;
    if (!e.jsonPath.empty()) fs::remove(e.jsonPath, ec);
    if (!e.verPath.empty()) fs::remove(e.verPath, ec);
}

const char* kFolderMeta = "_meta.json";

long long readPrunedCount(const fs::path& dir) {
    std::string text;
    if (!slurp(dir / kFolderMeta, text)) return 0;
    JObj o;
    if (!parseFlatObject(text, o)) return 0;
    const JVal* v = lastKey(o, "pruned");
    return (v && v->type == 'n') ? toInt(v->text) : 0;
}

void writeFolderMeta(const fs::path& dir, long long pruned, const Limits& lim) {
    const std::string text = "{\"schema\":\"all-versions-folder/1\",\"pruned\":" + std::to_string(pruned) +
                             ",\"capVersions\":" + std::to_string(lim.maxVersions) +
                             ",\"capBytes\":" + std::to_string(lim.maxBytes) + "}\n";
    writeAtomic(dir / kFolderMeta, text);   // a failed folder note never blocks a prune
}

}  // namespace

// ------------------------------------------------------------------ keys ----
std::string docKeyFor(const std::string& absPath, const std::string& dataRoot) {
    const fs::path p(absPath);
    std::string base = p.stem().string();
    if (base.empty()) base = p.filename().string();
    if (base.empty()) base = "untitled";
    const std::string abs = normalPath(absPath);
    const std::string root = normalPath(dataRoot);
    bool under = false;
    if (!root.empty()) {
        const std::string prefix = root == "/" ? std::string("/") : root + "/";
        under = abs == root || abs.compare(0, prefix.size(), prefix) == 0;
    }
    if (under) return base;
    return base + "~" + sha1Hex(absPath).substr(0, 8);
}

std::string versionsDirFor(const std::string& dataRoot, const std::string& docKey) {
    return (fs::path(dataRoot) / "library" / "versions" / docKey).string();
}

// ------------------------------------------------------------------ list ----
std::vector<Entry> list(const std::string& dir, const std::string& onlyPath,
                        int* otherPathCount, int* orphans) {
    if (otherPathCount) *otherPathCount = 0;
    if (orphans) *orphans = 0;
    std::vector<Entry> out;
    std::error_code ec;
    if (!fs::is_directory(dir, ec)) return out;
    std::set<std::string> vers;
    std::vector<std::string> jsons;
    for (const auto& de : fs::directory_iterator(dir, ec)) {
        if (!de.is_regular_file(ec)) continue;
        const fs::path p = de.path();
        const std::string name = p.filename().string();
        if (name.empty() || name[0] == '_' || name[0] == '.') continue;   // _meta.json, dotfiles
        const std::string ext = p.extension().string();
        if (ext == ".ver") vers.insert(p.stem().string());
        else if (ext == ".json") jsons.push_back(p.stem().string());
        // anything else (.tmp of an interrupted write included) is not a version
    }
    std::sort(jsons.begin(), jsons.end());
    const std::string wantPath = normalPath(onlyPath);
    for (const auto& stem : jsons) {
        Entry e;
        e.jsonPath = (fs::path(dir) / (stem + ".json")).string();
        const bool hasVer = vers.erase(stem) > 0;
        if (hasVer) e.verPath = (fs::path(dir) / (stem + ".ver")).string();
        std::string text;
        if (!slurp(e.jsonPath, text)) {
            e.meta.metaOk = false;
            e.meta.problem = "record " + stem + ".json could not be read";
        } else if (!parseMeta(text, e.meta)) {
            e.meta.metaOk = false;
            e.meta.problem = "record " + stem + ".json is not a readable all-version/1 record";
        }
        e.meta.stamp = stem;   // the file name is the identity in a directory-scan store
        if (!hasVer && e.meta.problem.empty()) e.meta.problem = "blob " + stem + ".ver is missing";
        if (!wantPath.empty() && e.meta.metaOk && normalPath(e.meta.path) != wantPath) {
            if (otherPathCount) ++*otherPathCount;
            continue;
        }
        out.push_back(std::move(e));
    }
    if (orphans) *orphans = int(vers.size());
    return out;
}

// -------------------------------------------------------------- readBlob ----
bool readBlob(const std::string& dir, const Entry& e, std::string& out) {
    out.clear();
    if (!e.meta.metaOk) return false;   // provenance unreadable → not restorable
    const fs::path p = e.verPath.empty() ? fs::path(dir) / (e.meta.stamp + ".ver") : fs::path(e.verPath);
    std::error_code ec;
    if (!fs::is_regular_file(p, ec)) return false;
    std::string bytes;
    if (!slurp(p, bytes)) return false;
    if (e.meta.sha1.empty()) return false;   // integrity cannot be checked → not restorable
    const bool decodedOnDisk = e.meta.codec.empty() || e.meta.codec == "raw";
    if (decodedOnDisk && sha1Hex(bytes) != lower(e.meta.sha1)) return false;
    out = std::move(bytes);
    return true;
}

// ----------------------------------------------------------------- prune ----
int prune(const std::string& dir, const Limits& lim) {
    std::vector<Entry> entries = list(dir);   // ascending by stamp
    std::vector<const Entry*> ok;
    for (const auto& e : entries) if (e.meta.metaOk) ok.push_back(&e);
    if (ok.empty()) return 0;
    long long total = 0;
    for (const Entry* e : ok) total += std::max(0LL, e->meta.bytes);
    int count = int(ok.size());
    const Entry* newest = ok.back();
    const Entry* newestSave = nullptr;   // the last permanent state must survive even when an autosave is newer
    for (auto it = ok.rbegin(); it != ok.rend(); ++it)
        if ((*it)->meta.reason != "autosave") { newestSave = *it; break; }
    std::vector<const Entry*> cand;
    for (const Entry* e : ok)
        if (e != newest && e != newestSave && !e->meta.pinned) cand.push_back(e);
    auto rank = [](const Entry* e) { return e->meta.reason == "autosave" ? 0 : isPre(e->meta.reason) ? 1 : 2; };
    std::stable_sort(cand.begin(), cand.end(), [&](const Entry* a, const Entry* b) { return rank(a) < rank(b); });
    auto over = [&] {
        return (lim.maxVersions > 0 && count > lim.maxVersions) ||
               (lim.maxBytes > 0 && total > lim.maxBytes);
    };
    int removed = 0;
    for (const Entry* e : cand) {
        if (!over()) break;
        removeEntry(*e);
        --count;
        total -= std::max(0LL, e->meta.bytes);
        ++removed;
    }
    if (removed > 0) writeFolderMeta(dir, readPrunedCount(dir) + removed, lim);
    return removed;
}

// ------------------------------------------------------------------- put ----
PutOutcome put(const std::string& dir, const std::string& blobBytes, Meta meta,
               const Limits& lim) {
    PutOutcome out;
    if (!validStamp(meta.stamp) || meta.sha1.empty()) return out;
    if (meta.codec.empty()) meta.codec = "raw";
    if (meta.bytes <= 0 && meta.codec == "raw") meta.bytes = (long long)blobBytes.size();
    std::error_code ec;
    fs::create_directories(dir, ec);
    if (!fs::is_directory(dir, ec)) return out;

    const std::vector<Entry> existing = list(dir);
    std::vector<Entry> slots;              // the rolling autosave slot (one, in a healthy store)
    const Entry* newestSave = nullptr;     // entries are ascending, so the last one seen is the newest
    for (const auto& e : existing) {
        if (!e.meta.metaOk) continue;
        if (e.meta.reason == "autosave") slots.push_back(e);
        else newestSave = &e;
    }
    const std::string want = lower(meta.sha1);
    if (newestSave && lower(newestSave->meta.sha1) == want) {
        // Identical to the newest permanent version: nothing new to keep, and
        // the autosave slot is stale — the permanent version captures this state.
        for (const auto& s : slots) removeEntry(s);
        out.result = PutResult::Deduplicated;
        out.stamp = newestSave->meta.stamp;
        return out;
    }
    const bool autosave = meta.reason == "autosave";
    if (autosave)
        for (const auto& s : slots)
            if (lower(s.meta.sha1) == want) {   // the slot already holds this state
                out.result = PutResult::Deduplicated;
                out.stamp = s.meta.stamp;
                return out;
            }

    // Stamp collision → "-N", which sorts after the bare stamp.
    std::string stamp = meta.stamp;
    for (int n = 1; fs::exists(fs::path(dir) / (stamp + ".ver"), ec) || fs::exists(fs::path(dir) / (stamp + ".json"), ec); ++n)
        stamp = meta.stamp + "-" + std::to_string(n);
    meta.stamp = stamp;
    const fs::path verPath = fs::path(dir) / (stamp + ".ver");
    const fs::path jsonPath = fs::path(dir) / (stamp + ".json");

    // Blob first, record second: a crash between them leaves an orphan, never a
    // record whose blob is missing.
    if (!writeAtomic(verPath, blobBytes)) return out;
    if (!writeAtomic(jsonPath, serializeMeta(meta))) { fs::remove(verPath, ec); return out; }

    // The slot: an autosave has replaced the previous one; a deliberate put
    // retires it because the permanent version now captures that state.
    for (const auto& s : slots) removeEntry(s);

    out.result = PutResult::Stored;
    out.stamp = stamp;
    out.pruned = prune(dir, lim);
    return out;
}

// ------------------------------------------------------------- renameKey ----
bool renameKey(const std::string& versionsRoot, const std::string& oldKey,
               const std::string& newKey) {
    if (oldKey.empty() || newKey.empty()) return false;
    if (oldKey == newKey) return true;
    std::error_code ec;
    const fs::path from = fs::path(versionsRoot) / oldKey;
    const fs::path to = fs::path(versionsRoot) / newKey;
    if (fs::exists(to, ec)) return false;
    if (!fs::exists(from, ec)) return true;   // no history to carry
    fs::create_directories(versionsRoot, ec);
    fs::rename(from, to, ec);
    return !ec;
}

// ----------------------------------------------------------- byChangeset ----
std::vector<Entry> byChangeset(const std::string& versionsRoot, const std::string& id) {
    std::vector<Entry> out;
    if (id.empty()) return out;
    std::error_code ec;
    if (!fs::is_directory(versionsRoot, ec)) return out;
    std::vector<std::string> dirs;
    for (const auto& de : fs::directory_iterator(versionsRoot, ec)) {
        if (!de.is_directory(ec)) continue;
        const std::string name = de.path().filename().string();
        if (name.empty() || name[0] == '_' || name[0] == '.') continue;   // _changesets/ is not a docKey
        dirs.push_back(de.path().string());
    }
    std::sort(dirs.begin(), dirs.end());
    for (const auto& d : dirs)
        for (auto& e : list(d))
            if (e.meta.metaOk && e.meta.changeset == id) out.push_back(std::move(e));
    std::stable_sort(out.begin(), out.end(),
                     [](const Entry& a, const Entry& b) { return a.meta.stamp < b.meta.stamp; });
    return out;
}

}  // namespace allcore::versions
