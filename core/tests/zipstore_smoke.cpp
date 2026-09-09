// zipstore_smoke — the STORED-zip container battery (analysis-suite F6).
// Fixture-free: everything it needs it builds.
#include <cstdint>
#include <cstdio>
#include <string>
#include <vector>

#include "allcore/zipstore.h"

using namespace allcore::zipstore;
static int failures = 0;
#define CHECK(cond, msg) do { if (cond) std::printf("  [PASS] %s\n", msg); else { std::printf("  [FAIL] %s\n", msg); ++failures; } } while (0)

static std::uint16_t u16(const std::string& s, std::size_t at) {
    return (std::uint16_t)((unsigned char)s[at] | ((unsigned char)s[at + 1] << 8));
}
static std::uint32_t u32(const std::string& s, std::size_t at) {
    return (std::uint32_t)((unsigned char)s[at]) | ((std::uint32_t)(unsigned char)s[at + 1] << 8) |
           ((std::uint32_t)(unsigned char)s[at + 2] << 16) | ((std::uint32_t)(unsigned char)s[at + 3] << 24);
}

// Walk the local file headers from the front, the way an unzipper does.
struct Local { std::string name; std::uint16_t method = 0, flags = 0; std::size_t dataOffset = 0; std::uint32_t size = 0, crc = 0; };
static std::vector<Local> walkLocals(const std::string& z) {
    std::vector<Local> v;
    std::size_t at = 0;
    while (at + 30 <= z.size() && u32(z, at) == 0x04034B50u) {
        Local l;
        l.flags = u16(z, at + 6);
        l.method = u16(z, at + 8);
        l.crc = u32(z, at + 14);
        l.size = u32(z, at + 22);
        const std::uint16_t nameLen = u16(z, at + 26), extraLen = u16(z, at + 28);
        if (at + 30 + nameLen + extraLen + l.size > z.size()) break;
        l.name = z.substr(at + 30, nameLen);
        l.dataOffset = at + 30 + nameLen + extraLen;
        v.push_back(l);
        at = l.dataOffset + l.size;
    }
    return v;
}

int main() {
    setvbuf(stdout, nullptr, _IONBF, 0);

    // 1. CRC-32, reflected polynomial 0xEDB88320: the published check value.
    CHECK(crc32("123456789") == 0xCBF43926u && crc32("") == 0u &&
              crc32("a") == 0xE8B7BE43u && crc32(std::string("\x00\x00", 2)) == 0x41D912FFu,
          "1 crc32 matches the published check values (\"123456789\" = 0xCBF43926, \"\" = 0)");

    // 2. write → list round-trip.
    const std::vector<Part> parts = {{"a.txt", "AB"}, {"b/c.xml", "<x/>"}};
    const std::string z = writeStored(parts);
    {
        bool ok = z.size() > 4 && z.compare(0, 4, "PK\x03\x04", 4) == 0;
        const auto e = listStored(z);
        ok = ok && e.size() == 2;
        if (ok) {
            for (std::size_t i = 0; i < e.size(); ++i) {
                ok = ok && e[i].name == parts[i].name;
                ok = ok && e[i].size == (std::uint32_t)parts[i].bytes.size();
                ok = ok && e[i].crc == crc32(parts[i].bytes);
                ok = ok && e[i].dataOffset + e[i].size <= z.size();
                ok = ok && z.compare(e[i].dataOffset, parts[i].bytes.size(), parts[i].bytes) == 0;
            }
        }
        const auto locals = walkLocals(z);
        ok = ok && locals.size() == 2;
        if (locals.size() == 2) {
            for (std::size_t i = 0; i < 2; ++i) {
                ok = ok && locals[i].method == 0;                    // stored
                ok = ok && (locals[i].flags & 0x0800u) != 0;         // bit 11: UTF-8 names
                ok = ok && locals[i].name == parts[i].name;
                ok = ok && locals[i].dataOffset == e[i].dataOffset;
                ok = ok && locals[i].crc == e[i].crc && locals[i].size == e[i].size;
            }
        }
        // EOCD: no archive comment, so it is the last 22 bytes.
        const std::size_t eocd = z.size() - 22;
        ok = ok && u32(z, eocd) == 0x06054B50u && u16(z, eocd + 8) == 2 && u16(z, eocd + 10) == 2;
        ok = ok && u32(z, eocd + 16) + u32(z, eocd + 12) == (std::uint32_t)eocd &&
             u32(z, (std::size_t)u32(z, eocd + 16)) == 0x02014B50u;
        CHECK(ok, "2 writeStored/listStored round-trip: names, sizes, CRCs, data offsets, method 0, UTF-8 flag, EOCD count 2");
    }

    // 3. Determinism — the whole point of the fixed DOS timestamps.
    CHECK(writeStored(parts) == z && writeStored(parts).size() == z.size(),
          "3 two writes of the same parts are byte-identical");

    // 4. Edges: no parts, an empty part, a UTF-8 name, and a part big enough
    //    to leave the tiny-buffer path behind.
    {
        const std::string empty = writeStored({});
        bool ok = listStored(empty).empty() && empty.size() == 22 && u32(empty, 0) == 0x06054B50u;

        std::string big(70000, 'x');
        for (std::size_t i = 0; i < big.size(); ++i) big[i] = (char)('a' + (i % 26));
        const std::vector<Part> odd = {{"\xE0\xBD\x82.txt", ""}, {"word/document.xml", big}};
        const std::string z2 = writeStored(odd);
        const auto e2 = listStored(z2);
        ok = ok && e2.size() == 2 && e2[0].size == 0 && e2[0].crc == 0u &&
             e2[0].name == odd[0].name && e2[1].size == (std::uint32_t)big.size() &&
             z2.compare(e2[1].dataOffset, big.size(), big) == 0;
        CHECK(ok, "4 empty archive, empty part, UTF-8 name and a 70 KB part all survive the round-trip");
    }

    // 5. Junk in, nothing out: a reader that guesses is worse than one that
    //    says it found nothing.
    {
        bool ok = listStored("").empty() && listStored("not a zip at all").empty();
        std::string truncated = z.substr(0, z.size() - 10);   // EOCD chopped
        ok = ok && listStored(truncated).empty();
        CHECK(ok, "5 empty, non-zip and truncated buffers yield no entries rather than a guess");
    }

    std::printf("zipstore_smoke: %d failure(s)\n", failures);
    return failures ? 1 : 0;
}
