// versions_smoke — the document version store battery (F1): store, dedupe,
// the rolling autosave slot, prune caps (pinned and newest never), the path
// filter, FIPS 180-4 SHA-1 vectors, docKey collisions, orphans, damaged
// blobs, malformed records, exact label round-trip, renameKey, byChangeset.
// Fixture-free; deterministic stamps; the temp dir is cleaned at entry and
// exit (the twice-run rule).
#include <cstdio>
#include <filesystem>
#include <fstream>
#include <string>
#include <vector>
#include <unistd.h>

#include "allcore/versions.h"

namespace fs = std::filesystem;
using namespace allcore::versions;

static int failures = 0;
#define CHECK(cond, msg) do { if (cond) std::printf("  [PASS] %s\n", msg); else { std::printf("  [FAIL] %s\n", msg); ++failures; } } while (0)

static void writeFile(const fs::path& p, const std::string& body) {
    fs::create_directories(p.parent_path());
    std::ofstream f(p, std::ios::binary | std::ios::trunc);
    f.write(body.data(), (std::streamsize)body.size());
}
static std::string slurp(const fs::path& p) {
    std::ifstream f(p, std::ios::binary);
    return std::string(std::istreambuf_iterator<char>(f), {});
}
static Meta M(const std::string& stamp, const std::string& path, const std::string& reason, const std::string& content) {
    Meta m;
    m.stamp = stamp; m.path = path; m.kind = "document"; m.reason = reason; m.codec = "raw";
    m.sha1 = sha1Hex(content); m.bytes = (long long)content.size();
    m.encoding = "UTF-8"; m.lineEnding = "LF"; m.savedBy = "Adam"; m.savedBySource = "team-name";
    m.savedAt = "2026-09-09T10:00:00"; m.app = "versions_smoke";
    return m;
}
static PutOutcome putText(const std::string& dir, const std::string& stamp, const std::string& content,
                          const std::string& reason = "save", const std::string& path = "/x/doc.act",
                          const Limits& lim = Limits{}) {
    return put(dir, content, M(stamp, path, reason, content), lim);
}
static int countSuffix(const fs::path& dir, const std::string& suffix) {
    int n = 0;
    std::error_code ec;
    for (const auto& de : fs::directory_iterator(dir, ec)) {
        const std::string nm = de.path().filename().string();
        if (nm.size() >= suffix.size() && nm.compare(nm.size() - suffix.size(), suffix.size(), suffix) == 0) ++n;
    }
    return n;
}
static bool hasStamp(const std::vector<Entry>& v, const std::string& s) { for (const auto& e : v) if (e.meta.stamp == s) return true; return false; }
static int countReason(const std::vector<Entry>& v, const std::string& r) { int n = 0; for (const auto& e : v) if (e.meta.reason == r) ++n; return n; }
static int countOk(const std::vector<Entry>& v, bool ok) { int n = 0; for (const auto& e : v) if (e.meta.metaOk == ok) ++n; return n; }
static const Entry* find(const std::vector<Entry>& v, const std::string& s) { for (const auto& e : v) if (e.meta.stamp == s) return &e; return nullptr; }
static std::string blobOf(const std::string& dir, const std::vector<Entry>& v, const std::string& stamp) {
    const Entry* e = find(v, stamp);
    if (!e) return "<no entry>";
    std::string out;
    return readBlob(dir, *e, out) ? out : "<readBlob false>";
}
static bool isHex8(const std::string& s) {
    if (s.size() != 8) return false;
    for (const char c : s) if (!((c >= '0' && c <= '9') || (c >= 'a' && c <= 'f'))) return false;
    return true;
}

int main() {
    setvbuf(stdout, nullptr, _IONBF, 0);
    const fs::path root = fs::temp_directory_path() / ("all_versions_" + std::to_string(::getpid()));
    std::error_code ec;
    fs::remove_all(root, ec);
    fs::create_directories(root);
    const Limits lim;
    const std::string P = "/x/doc.act";
    const std::string d1 = (root / "doc").string();

    // 1. fresh
    {
        int orph = -1, other = -1;
        auto l0 = list(d1, "", &other, &orph);
        fs::create_directories(d1);
        int orph2 = -1, other2 = -1;
        auto l0b = list(d1, "", &other2, &orph2);
        CHECK(l0.empty() && orph == 0 && other == 0 && l0b.empty() && orph2 == 0 && other2 == 0, "1. list on a fresh dir (absent, then empty) → empty, orphans 0, no error");
    }
    // 2. first store
    {
        auto o1 = putText(d1, "0001", "A\n");
        auto l1 = list(d1);
        CHECK(o1.result == PutResult::Stored && o1.stamp == "0001" && o1.pruned == 0 && l1.size() == 1 &&
              l1[0].meta.stamp == "0001" && l1[0].meta.metaOk && l1[0].meta.problem.empty() &&
              blobOf(d1, l1, "0001") == "A\n" && countSuffix(d1, ".tmp") == 0 && fs::exists(fs::path(d1) / "0001.json"),
              "2. put A → Stored; 1 entry; readBlob returns exactly \"A\\n\"; no *.tmp remains");
        CHECK(l1[0].meta.path == P && l1[0].meta.reason == "save" && l1[0].meta.sha1 == sha1Hex("A\n") &&
              l1[0].meta.bytes == 2 && l1[0].meta.savedBy == "Adam" && l1[0].meta.encoding == "UTF-8",
              "2b. the record carries the provenance that was put");
    }
    // 3. dedupe
    {
        auto o2 = putText(d1, "0002", "A\n");
        auto l2 = list(d1);
        CHECK(o2.result == PutResult::Deduplicated && o2.stamp == "0001" && l2.size() == 1 && countSuffix(d1, ".ver") == 1 &&
              !fs::exists(fs::path(d1) / "0002.json"),
              "3. identical re-save at 0002 → Deduplicated (names 0001); still 1 entry, 1 blob");
    }
    // 4. second distinct version
    {
        auto o3 = putText(d1, "0003", "B\n");
        auto l3 = list(d1);
        CHECK(o3.result == PutResult::Stored && l3.size() == 2 && l3[0].meta.stamp == "0001" && l3[1].meta.stamp == "0003" &&
              blobOf(d1, l3, "0003") == "B\n" && blobOf(d1, l3, "0001") == "A\n",
              "4. put B at 0003 → list == [0001, 0003] ascending; readBlob(0003) == \"B\\n\"");
    }
    // 5. rolling autosave slot
    {
        const std::string d5 = (root / "doc5").string();
        auto s1 = putText(d5, "0001", "S1\n");
        auto a1 = putText(d5, "0002", "X1\n", "autosave");
        auto a2 = putText(d5, "0003", "X2\n", "autosave");
        auto l5a = list(d5);
        const bool slotOk = s1.result == PutResult::Stored && a1.result == PutResult::Stored && a2.result == PutResult::Stored &&
                            l5a.size() == 2 && countReason(l5a, "autosave") == 1 && hasStamp(l5a, "0003") && !hasStamp(l5a, "0002") &&
                            !fs::exists(fs::path(d5) / "0002.ver") && !fs::exists(fs::path(d5) / "0002.json") &&
                            blobOf(d5, l5a, "0003") == "X2\n";
        auto b2 = putText(d5, "0004", "B2\n", "save");
        auto l5b = list(d5);
        CHECK(slotOk && b2.result == PutResult::Stored && l5b.size() == 2 && countReason(l5b, "autosave") == 0 &&
              hasStamp(l5b, "0001") && hasStamp(l5b, "0004") && !fs::exists(fs::path(d5) / "0003.ver") && !fs::exists(fs::path(d5) / "0003.json"),
              "5. autosave x1 then x2 → one autosave entry (x2), x1's blob gone; a save → no autosave remains, 2 total");
        auto a3 = putText(d5, "0005", "B2\n", "autosave");
        CHECK(a3.result == PutResult::Deduplicated && a3.stamp == "0004" && list(d5).size() == 2,
              "5b. an autosave identical to the newest save mints nothing");
    }
    // 6. version cap
    {
        const std::string d6 = (root / "doc6").string();
        Limits l6; l6.maxVersions = 3;
        PutOutcome last;
        const char* stamps[] = {"0001", "0002", "0003", "0004", "0005"};
        for (int i = 0; i < 5; ++i) last = putText(d6, stamps[i], std::string("V") + stamps[i] + "\n", "save", P, l6);
        auto l6l = list(d6);
        const std::string folder = slurp(fs::path(d6) / "_meta.json");
        CHECK(l6l.size() == 3 && l6l[0].meta.stamp == "0003" && l6l[2].meta.stamp == "0005" && last.pruned == 1 &&
              !fs::exists(fs::path(d6) / "0001.ver") && !fs::exists(fs::path(d6) / "0001.json") &&
              !fs::exists(fs::path(d6) / "0002.ver") && !fs::exists(fs::path(d6) / "0002.json") &&
              folder.find("\"schema\":\"all-versions-folder/1\"") != std::string::npos && folder.find("\"pruned\":2") != std::string::npos &&
              folder.find("\"capVersions\":3") != std::string::npos,
              "6. maxVersions=3 after five puts → 3 remain, the oldest two gone (blobs deleted), _meta.json pruned == 2");
    }
    // 7. pinned and newest are never pruned
    {
        const std::string d7 = (root / "doc7").string();
        Meta c = M("0001", P, "save", "C\n"); c.pinned = true;
        put(d7, "C\n", c, lim);
        putText(d7, "0002", "D\n");
        Limits l7; l7.maxVersions = 1;
        const int r7 = prune(d7, l7);
        auto l7l = list(d7);
        CHECK(r7 == 0 && l7l.size() == 2 && l7l[0].meta.pinned && !l7l[1].meta.pinned, "7. [pinned C, newest D] with maxVersions=1 → prune removes nothing, returns 0");
        putText(d7, "0003", "E\n");   // now [pinned C, D, newest E]
        const int r7b = prune(d7, l7);
        auto l7m = list(d7);
        CHECK(r7b == 1 && l7m.size() == 2 && hasStamp(l7m, "0001") && hasStamp(l7m, "0003") && !hasStamp(l7m, "0002"),
              "7b. with an unpinned middle entry only that one goes: pinned and newest survive");
    }
    // 8. byte cap counts UNCOMPRESSED bytes from the records
    {
        const std::string d8 = (root / "doc8").string();
        Limits l8; l8.maxBytes = 2500;
        for (const char* st : {"0001", "0002", "0003"}) {
            Meta m = M(st, P, "save", std::string("doc-") + st);
            m.codec = "qz"; m.bytes = 1000;   // a tiny blob on disk that claims 1000 uncompressed bytes
            put(d8, "xx", m, l8);
        }
        auto l8l = list(d8);
        CHECK(l8l.size() == 2 && l8l[0].meta.stamp == "0002" && l8l[1].meta.stamp == "0003" && !fs::exists(fs::path(d8) / "0001.ver") &&
              fs::file_size(fs::path(d8) / "0002.ver") == 2,
              "8. maxBytes=2500 with three 1000-byte records → oldest pruned until under the cap (on-disk blobs are 2 bytes: it counted `bytes`)");
    }
    // 9. path filter
    {
        const std::string d9 = (root / "doc9").string();
        putText(d9, "0001", "P1\n", "save", "/x/doc.act");
        putText(d9, "0002", "P2\n", "save", "/y/doc.act");
        int other = -1;
        auto l9 = list(d9, "/x/doc.act", &other);
        CHECK(l9.size() == 1 && l9[0].meta.path == "/x/doc.act" && other == 1 && list(d9).size() == 2,
              "9. list(onlyPath=/x/doc.act) excludes the /y/doc.act entry and sets other == 1");
    }
    // 10. SHA-1 vectors
    {
        CHECK(sha1Hex("abc") == "a9993e364706816aba3e25717850c26c9cd0d89d" && sha1Hex("") == "da39a3ee5e6b4b0d3255bfef95601890afd80709",
              "10. sha1Hex: FIPS 180-4 vectors for \"abc\" and \"\"");
        CHECK(sha1Hex("abcdbcdecdefdefgefghfghighijhijkijkljklmklmnlmnomnopnopq") == "84983e441c3bd26ebaae4aa1f95129e5e54670f1" &&
              sha1Hex(std::string(1000000, 'a')) == "34aa973cd4c4daa4f61eeb2bdbad27316534016f" &&
              sha1Hex(std::string(55, 'x')) != sha1Hex(std::string(56, 'x')) && sha1Hex(std::string(64, 'x')).size() == 40,
              "10b. two-block message, the million-'a' vector, and the 55/56/64-byte padding edges");
    }
    // 11. docKey
    {
        const std::string k1 = docKeyFor("/root/library/x/KL0001.ACT", "/root");
        const std::string k2 = docKeyFor("/Volumes/USB/KL0001.ACT", "/root");
        const std::string k3 = docKeyFor("/Volumes/OTHER/KL0001.ACT", "/root");
        CHECK(k1 == "KL0001" && k2.rfind("KL0001~", 0) == 0 && k2.size() == 7 + 8 && isHex8(k2.substr(7)) && k3 != k2 && k3.rfind("KL0001~", 0) == 0,
              "11. docKeyFor: under the root → base name; outside → base~8hex, different for two outside paths");
        CHECK(docKeyFor("/root/KL0001.ACT", "/root/") == "KL0001" && docKeyFor("/rootx/KL0001.ACT", "/root") != "KL0001" &&
              docKeyFor("/root/a/b.tar.gz", "/root") == "b.tar" && docKeyFor("/anywhere/KL0001.ACT", "") != "KL0001" &&
              versionsDirFor("/root", "KL0001") == "/root/library/versions/KL0001",
              "11b. trailing slash on the root, /rootx is not /root, completeBaseName keeps inner dots, empty root = outside, versionsDirFor");
    }
    // 12. orphan
    {
        writeFile(fs::path(d1) / "0006.ver", "orphan\n");
        int orph = -1;
        auto l12 = list(d1, "", nullptr, &orph);
        CHECK(orph == 1 && !hasStamp(l12, "0006") && l12.size() == 2, "12. a 0006.ver with no 0006.json → not an entry, orphans == 1");
    }
    // 13. damaged blob
    {
        auto before = list(d1);
        writeFile(fs::path(d1) / "0003.ver", "B tampered\n");
        auto l13 = list(d1);
        std::string out = "untouched";
        const Entry* e = find(l13, "0003");
        const bool refused = e && !readBlob(d1, *e, out);
        CHECK(before.size() == 2 && refused && out.empty() && e->meta.metaOk, "13. a tampered .ver → readBlob returns false (SHA-1 mismatch); the record still lists");
        writeFile(fs::path(d1) / "0003.ver", "B\n");
        CHECK(blobOf(d1, list(d1), "0003") == "B\n", "13b. with the bytes restored readBlob succeeds again");
    }
    // 14. malformed record
    {
        writeFile(fs::path(d1) / "0007.json", "not json");
        writeFile(fs::path(d1) / "0007.ver", "Q\n");
        auto l14 = list(d1);
        const Entry* bad = find(l14, "0007");
        std::string out;
        CHECK(l14.size() == 3 && bad && !bad->meta.metaOk && !bad->meta.problem.empty() && countOk(l14, true) == 2 &&
              !readBlob(d1, *bad, out) && blobOf(d1, l14, "0001") == "A\n",
              "14. 0007.json = 'not json' → listed with metaOk=false and a problem; unreadable, unrestorable; the others still load");
        writeFile(fs::path(d1) / "0008.json", "{\"schema\":\"all-version/1\",\"stamp\":\"0008\",\"sha1\":\"" + sha1Hex("R\n") + "\"}");
        writeFile(fs::path(d1) / "0008.ver", "R\n");
        auto l14b = list(d1);
        const Entry* sparse = find(l14b, "0008");
        std::string out8;
        CHECK(sparse && sparse->meta.metaOk && sparse->meta.problem.empty() && sparse->meta.reason.empty() && readBlob(d1, *sparse, out8) && out8 == "R\n",
              "14b. a sparse but valid record parses (missing keys keep defaults)");
        writeFile(fs::path(d1) / "0009.json", "{\"schema\":\"all-version/2\",\"stamp\":\"0009\"}");
        writeFile(fs::path(d1) / "0009.ver", "S\n");
        const Entry* future = find(list(d1), "0009");
        CHECK(future && !future->meta.metaOk, "14c. a record of an unknown schema is refused wholesale, not half-read");
        for (const char* n : {"0007.json", "0007.ver", "0008.json", "0008.ver", "0009.json", "0009.ver", "0006.ver"}) fs::remove(fs::path(d1) / n, ec);
    }
    // 15. exact round-trip
    {
        Meta m = M("20260908-214512-337", "/Users/adam/library/kangyur/KL0032.ACT", "milestone", "body\n");
        m.label = "sent\tto GMR\n\"quoted\" \\ slash/ \xE0\xBD\xA6\xE0\xBD\xBA\xE0\xBD\x98\xE0\xBD\xA6 \x01";
        m.pinned = true; m.bom = true; m.revision = 12; m.bytes = 48213; m.changeset = "rif-20260908-214512";
        m.kind = "document"; m.encoding = "Windows-1252"; m.lineEnding = "CRLF"; m.codec = "qz";
        m.statisticsJson = "{\"folios\":12,\"syllables\":4021,\"note\":\"a }\\\" brace\"}";
        m.statisticsNote = "";
        const std::string js = serializeMeta(m);
        Meta back;
        const bool ok = parseMeta(js, back);
        CHECK(ok && back.label == m.label && back.stamp == m.stamp && back.path == m.path && back.kind == m.kind &&
              back.reason == m.reason && back.changeset == m.changeset && back.revision == 12 && back.bytes == 48213 &&
              back.codec == "qz" && back.sha1 == m.sha1 && back.encoding == m.encoding && back.lineEnding == "CRLF" &&
              back.bom && back.pinned && back.savedBy == m.savedBy && back.savedBySource == m.savedBySource &&
              back.savedAt == m.savedAt && back.app == m.app && back.statisticsJson == m.statisticsJson &&
              back.statisticsNote.empty() && back.metaOk && back.problem.empty(),
              "15. a label with a tab, a newline, quotes, a backslash, Tibetan and a control byte round-trips byte-identically; every field survives");
        Meta n = M("0001", P, "save", "x\n");
        n.statisticsJson = ""; n.statisticsNote = "not computed: file too large";
        const std::string js2 = serializeMeta(n);
        Meta back2;
        CHECK(js2.find("\"statistics\": null") != std::string::npos && parseMeta(js2, back2) && back2.statisticsJson.empty() &&
              back2.statisticsNote == n.statisticsNote && !parseMeta("{\"schema\":\"all-version/1\",\"revision\":\"12\"}", back2) &&
              !parseMeta("{\"schema\":\"all-version/1\"} trailing", back2) && parseMeta("{\"schema\":\"all-version/1\",\"label\":\"\\u0f66\\ud83d\\ude00\"}", back2) &&
              back2.label == "\xE0\xBD\xA6\xF0\x9F\x98\x80",
              "15b. empty statistics serialize as null; a wrong value type or trailing text is refused; \\u escapes incl. surrogate pairs decode");
    }
    // 16. renameKey
    {
        const std::string vroot = (root / "vroot").string();
        putText(vroot + "/doc", "0001", "A\n");
        putText(vroot + "/doc", "0002", "B\n");
        const bool moved = renameKey(vroot, "doc", "doc2");
        auto l16a = list(vroot + "/doc");
        auto l16b = list(vroot + "/doc2");
        CHECK(moved && l16a.empty() && !fs::exists(fs::path(vroot) / "doc") && l16b.size() == 2 && blobOf(vroot + "/doc2", l16b, "0002") == "B\n",
              "16. renameKey(doc, doc2) → list(doc) empty, list(doc2) complete and readable");
        putText(vroot + "/doc", "0001", "Z\n");
        const bool refused = renameKey(vroot, "doc", "doc2");
        CHECK(!refused && list(vroot + "/doc").size() == 1 && list(vroot + "/doc2").size() == 2 && blobOf(vroot + "/doc", list(vroot + "/doc"), "0001") == "Z\n",
              "16b. onto an existing key it returns false and changes nothing");
        CHECK(renameKey(vroot, "nothing-here", "also-nothing") && !fs::exists(fs::path(vroot) / "also-nothing") && renameKey(vroot, "doc", "doc"),
              "16c. no history to carry, or the same key → true without creating anything");
    }
    // 17. byChangeset across docKeys
    {
        const std::string vroot = (root / "vroot17").string();
        Meta a1 = M("0001", "/a/KL0001.ACT", "pre-replace", "a1\n"); a1.changeset = "rif-1";
        put(vroot + "/KL0001", "a1\n", a1, lim);
        putText(vroot + "/KL0001", "0002", "a2\n");
        Meta b1 = M("0003", "/b/KL0002.ACT", "pre-replace", "b1\n"); b1.changeset = "rif-1";
        put(vroot + "/KL0002", "b1\n", b1, lim);
        Meta b2 = M("0004", "/b/KL0002.ACT", "pre-replace", "b2\n"); b2.changeset = "rif-2";
        put(vroot + "/KL0002", "b2\n", b2, lim);
        writeFile(fs::path(vroot) / "_changesets" / "rif-1.json", "{\"schema\":\"all-changeset/1\",\"id\":\"rif-1\"}");
        auto cs = byChangeset(vroot, "rif-1");
        CHECK(cs.size() == 2 && cs[0].meta.stamp == "0001" && cs[0].meta.path == "/a/KL0001.ACT" && cs[1].meta.stamp == "0003" &&
              cs[1].meta.path == "/b/KL0002.ACT" && cs[0].meta.changeset == "rif-1" && cs[1].meta.changeset == "rif-1" &&
              byChangeset(vroot, "rif-2").size() == 1 && byChangeset(vroot, "").empty() && byChangeset(vroot, "rif-9").empty() &&
              byChangeset((root / "no-such-root").string(), "rif-1").empty(),
              "17. byChangeset(rif-1) returns exactly the two entries carrying that id, across docKeys; _changesets/ is skipped");
    }
    // extras: stamp collision and refused puts
    {
        const std::string dx = (root / "docx").string();
        auto c1 = putText(dx, "0001", "one\n");
        auto c2 = putText(dx, "0001", "two\n");
        auto c3 = putText(dx, "0001", "three\n");
        auto lx = list(dx);
        CHECK(c1.stamp == "0001" && c2.stamp == "0001-1" && c3.stamp == "0001-2" && lx.size() == 3 && lx[0].meta.stamp == "0001" &&
              lx[1].meta.stamp == "0001-1" && lx[2].meta.stamp == "0001-2" && blobOf(dx, lx, "0001-1") == "two\n",
              "18. a stamp collision appends -N and keeps chronological order");
        Meta noSha = M("0002", P, "save", "z\n"); noSha.sha1.clear();
        Meta badStamp = M("with/slash", P, "save", "z\n");
        Meta reserved = M("_meta", P, "save", "z\n");
        CHECK(put(dx, "z\n", noSha, lim).result == PutResult::Failed && put(dx, "z\n", badStamp, lim).result == PutResult::Failed &&
              put(dx, "z\n", reserved, lim).result == PutResult::Failed && list(dx).size() == 3 && countSuffix(dx, ".tmp") == 0,
              "19. a put without a sha1, with a path-like stamp, or with a reserved stamp is refused and writes nothing");
    }

    fs::remove_all(root, ec);
    CHECK(!fs::exists(root), "temp dir removed at exit");
    std::printf("versions_smoke: %d failure(s)\n", failures);
    return failures ? 1 : 0;
}
