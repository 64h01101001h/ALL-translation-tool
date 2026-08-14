// stardict_smoke — the StarDict reader against generated fixtures:
// a two-word dictionary written in the real byte format (ifo + idx +
// dict), then the same dictionary with a gzip-compressed .dict.dz,
// then the refusal paths (64-bit idx, missing files). The fixture is
// built by this test so the battery carries its own ground truth.
#include <zlib.h>

#include <cstdio>
#include <cstring>
#include <filesystem>
#include <fstream>
#include <string>

#include "allcore/stardict.h"

static int failures = 0;
#define CHECK(cond, what)                                     \
    do {                                                      \
        if (cond) {                                           \
            std::printf("  [PASS] %s\n", what);               \
        } else {                                              \
            std::printf("  [FAIL] %s\n", what);               \
            ++failures;                                       \
        }                                                     \
    } while (0)

namespace fs = std::filesystem;

static void writeBytes(const fs::path& p, const std::string& b) {
    std::ofstream f(p, std::ios::binary);
    f.write(b.data(), (std::streamsize)b.size());
}

static void be32(std::string& s, uint32_t v) {
    s.push_back(char(v >> 24));
    s.push_back(char(v >> 16));
    s.push_back(char(v >> 8));
    s.push_back(char(v));
}

static std::string gzipAll(const std::string& in) {
    z_stream zs;
    std::memset(&zs, 0, sizeof zs);
    deflateInit2(&zs, Z_BEST_SPEED, Z_DEFLATED, 15 + 16, 8,
                 Z_DEFAULT_STRATEGY);
    std::string out;
    out.resize(deflateBound(&zs, (uLong)in.size()));
    zs.next_in = (Bytef*)in.data();
    zs.avail_in = (uInt)in.size();
    zs.next_out = (Bytef*)out.data();
    zs.avail_out = (uInt)out.size();
    deflate(&zs, Z_FINISH);
    out.resize(zs.total_out);
    deflateEnd(&zs);
    return out;
}

int main() {
    const fs::path dir =
        fs::temp_directory_path() / "all_stardict_smoke";
    fs::create_directories(dir);

    // the dictionary: "bsod nams" -> "merit (fixture)",
    // "ka" -> "pillar (fixture)"; sametypesequence=m (plain text)
    const std::string defA = "merit (fixture)";
    const std::string defB = "pillar (fixture)";
    std::string dict = defA + defB;
    std::string idx;
    idx += "bsod nams";
    idx.push_back('\0');
    be32(idx, 0);
    be32(idx, (uint32_t)defA.size());
    idx += "ka";
    idx.push_back('\0');
    be32(idx, (uint32_t)defA.size());
    be32(idx, (uint32_t)defB.size());
    const std::string ifo =
        "StarDict's dict ifo file\nversion=2.4.2\n"
        "bookname=Fixture Tibetan Dictionary\nwordcount=2\n"
        "idxfilesize=" + std::to_string(idx.size()) +
        "\nsametypesequence=m\n";
    writeBytes(dir / "fix.ifo", ifo);
    writeBytes(dir / "fix.idx", idx);
    writeBytes(dir / "fix.dict", dict);

    {
        allcore::StarDict d((dir / "fix.ifo").string());
        CHECK(d.ok(), "fixture dictionary opens");
        CHECK(d.bookname() == "Fixture Tibetan Dictionary",
              "bookname parsed from the ifo");
        CHECK(d.wordCount() == 2, "both index entries parsed");
        auto r = d.lookup("bsod nams");
        CHECK(r.size() == 1 && r[0] == defA,
              "exact lookup returns the definition bytes");
        auto rc = d.lookup("BSOD NAMS");
        CHECK(rc.size() == 1 && rc[0] == defA,
              "lookup is case-insensitive");
        auto p = d.prefix("bso");
        CHECK(p.size() == 1 && p[0] == "bsod nams",
              "prefix scan finds the headword");
        CHECK(d.lookup("nonexistent").empty(),
              "missing word returns empty, never guesses");
    }

    // the .dict.dz variant (gzip whole-stream, as dictzip is)
    fs::remove(dir / "fix.dict");
    writeBytes(dir / "fix.dict.dz", gzipAll(dict));
    {
        allcore::StarDict d((dir / "fix.ifo").string());
        CHECK(d.ok(), "dict.dz (gzip) variant opens");
        auto r = d.lookup("ka");
        CHECK(r.size() == 1 && r[0] == defB,
              "lookup reads the decompressed data");
    }

    // refusal: 64-bit idx offsets are not guessed at
    writeBytes(dir / "wide.ifo",
               "StarDict's dict ifo file\nversion=3.0.0\n"
               "bookname=Wide\nidxoffsetbits=64\n");
    writeBytes(dir / "wide.idx", idx);
    writeBytes(dir / "wide.dict", dict);
    {
        allcore::StarDict d((dir / "wide.ifo").string());
        CHECK(!d.ok() &&
                  d.error().find("64") != std::string::npos,
              "64-bit idx is refused with a reason");
    }
    {
        allcore::StarDict d((dir / "absent.ifo").string());
        CHECK(!d.ok(), "missing files fail cleanly");
    }

    std::printf(failures ? "STARDICT SMOKE FAILED\n"
                         : "STARDICT SMOKE OK\n");
    return failures;
}
