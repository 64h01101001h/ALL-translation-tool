#include "allcore/zipstore.h"

namespace allcore {
namespace zipstore {

// ---------------------------------------------------------------- CRC-32
static const std::uint32_t* crcTable() {
    static std::uint32_t t[256];
    static const bool built = [] {
        for (std::uint32_t i = 0; i < 256; ++i) {
            std::uint32_t c = i;
            for (int k = 0; k < 8; ++k) c = (c & 1u) ? (0xEDB88320u ^ (c >> 1)) : (c >> 1);
            t[i] = c;
        }
        return true;
    }();
    (void)built;
    return t;
}

std::uint32_t crc32(const std::string& bytes) {
    const std::uint32_t* t = crcTable();
    std::uint32_t c = 0xFFFFFFFFu;
    for (unsigned char ch : bytes) c = t[(c ^ ch) & 0xFFu] ^ (c >> 8);
    return c ^ 0xFFFFFFFFu;
}

// ------------------------------------------------------- little-endian io
static void put16(std::string& out, std::uint16_t v) {
    out += (char)(v & 0xFFu);
    out += (char)((v >> 8) & 0xFFu);
}
static void put32(std::string& out, std::uint32_t v) {
    out += (char)(v & 0xFFu);
    out += (char)((v >> 8) & 0xFFu);
    out += (char)((v >> 16) & 0xFFu);
    out += (char)((v >> 24) & 0xFFu);
}
static std::uint16_t get16(const std::string& s, std::size_t at) {
    return (std::uint16_t)((unsigned char)s[at] | ((unsigned char)s[at + 1] << 8));
}
static std::uint32_t get32(const std::string& s, std::size_t at) {
    return (std::uint32_t)((unsigned char)s[at]) | ((std::uint32_t)(unsigned char)s[at + 1] << 8) |
           ((std::uint32_t)(unsigned char)s[at + 2] << 16) | ((std::uint32_t)(unsigned char)s[at + 3] << 24);
}

// The fixed timestamp: 1980-01-01 00:00:00, the earliest a DOS date can
// express. Deterministic output matters more than a real clock here — the
// same two drafts must always produce the same bytes.
static const std::uint16_t kDosTime = 0x0000;
static const std::uint16_t kDosDate = 0x0021;   // year 1980, month 1, day 1

static const std::uint16_t kFlagUtf8 = 0x0800;   // general-purpose bit 11
static const std::uint16_t kVersion = 20;        // 2.0: the stored/deflate baseline

// ---------------------------------------------------------------- writer
std::string writeStored(const std::vector<Part>& parts) {
    std::string out;
    struct Central { std::uint32_t crc, size, offset; const Part* p; };
    std::vector<Central> dir;
    dir.reserve(parts.size());

    for (const Part& p : parts) {
        Central c{crc32(p.bytes), (std::uint32_t)p.bytes.size(), (std::uint32_t)out.size(), &p};
        put32(out, 0x04034B50u);                     // local file header signature
        put16(out, kVersion);                        // version needed to extract
        put16(out, kFlagUtf8);                       // general purpose bit flag
        put16(out, 0);                               // compression method: stored
        put16(out, kDosTime);
        put16(out, kDosDate);
        put32(out, c.crc);
        put32(out, c.size);                          // compressed size == uncompressed
        put32(out, c.size);
        put16(out, (std::uint16_t)p.name.size());
        put16(out, 0);                               // extra field length
        out += p.name;
        out += p.bytes;
        dir.push_back(c);
    }

    const std::uint32_t dirOffset = (std::uint32_t)out.size();
    for (const Central& c : dir) {
        put32(out, 0x02014B50u);                     // central directory header signature
        put16(out, kVersion);                        // version made by (host 0 = MS-DOS)
        put16(out, kVersion);                        // version needed to extract
        put16(out, kFlagUtf8);
        put16(out, 0);                               // stored
        put16(out, kDosTime);
        put16(out, kDosDate);
        put32(out, c.crc);
        put32(out, c.size);
        put32(out, c.size);
        put16(out, (std::uint16_t)c.p->name.size());
        put16(out, 0);                               // extra field length
        put16(out, 0);                               // file comment length
        put16(out, 0);                               // disk number start
        put16(out, 0);                               // internal file attributes
        put32(out, 0);                               // external file attributes
        put32(out, c.offset);                        // offset of the local header
        out += c.p->name;
    }
    const std::uint32_t dirSize = (std::uint32_t)out.size() - dirOffset;

    put32(out, 0x06054B50u);                         // end of central directory
    put16(out, 0);                                   // this disk
    put16(out, 0);                                   // disk with the central directory
    put16(out, (std::uint16_t)dir.size());           // entries on this disk
    put16(out, (std::uint16_t)dir.size());           // entries total
    put32(out, dirSize);
    put32(out, dirOffset);
    put16(out, 0);                                   // archive comment length
    return out;
}

// ---------------------------------------------------------------- reader
std::vector<EntryView> listStored(const std::string& archive) {
    std::vector<EntryView> out;
    if (archive.size() < 22) return out;

    // Find the EOCD: scan back from the end over the 64 KB the comment
    // field could occupy. Nothing is guessed — no signature, no entries.
    std::size_t eocd = std::string::npos;
    const std::size_t lowest = archive.size() > 22 + 65535u ? archive.size() - (22 + 65535u) : 0;
    for (std::size_t i = archive.size() - 22 + 1; i-- > lowest;) {
        if (get32(archive, i) == 0x06054B50u) { eocd = i; break; }
    }
    if (eocd == std::string::npos) return out;

    const std::uint16_t count = get16(archive, eocd + 10);
    const std::uint32_t dirSize = get32(archive, eocd + 12);
    const std::uint32_t dirOffset = get32(archive, eocd + 16);
    if ((std::size_t)dirOffset + dirSize > archive.size()) return out;

    std::size_t at = dirOffset;
    for (std::uint16_t i = 0; i < count; ++i) {
        if (at + 46 > archive.size() || get32(archive, at) != 0x02014B50u) break;
        const std::uint16_t method = get16(archive, at + 10);
        const std::uint32_t crc = get32(archive, at + 16);
        const std::uint32_t csize = get32(archive, at + 20);
        const std::uint32_t usize = get32(archive, at + 24);
        const std::uint16_t nameLen = get16(archive, at + 28);
        const std::uint16_t extraLen = get16(archive, at + 30);
        const std::uint16_t commentLen = get16(archive, at + 32);
        const std::uint32_t local = get32(archive, at + 42);
        if (at + 46 + nameLen > archive.size()) break;

        if (method == 0 && (std::size_t)local + 30 <= archive.size() && get32(archive, local) == 0x04034B50u) {
            // The local header's own name/extra lengths decide where the
            // data starts; the central copy may differ in the extra field.
            const std::uint16_t lname = get16(archive, local + 26);
            const std::uint16_t lextra = get16(archive, local + 28);
            const std::size_t data = (std::size_t)local + 30 + lname + lextra;
            if (data + csize <= archive.size()) {
                EntryView e;
                e.name = archive.substr(at + 46, nameLen);
                e.dataOffset = data;
                e.size = usize;
                e.crc = crc;
                out.push_back(e);
            }
        }
        at += 46u + nameLen + extraLen + commentLen;
    }
    return out;
}

}  // namespace zipstore
}  // namespace allcore
