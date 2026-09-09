// allcore/zipstore.h — a minimal STORED-only ZIP container (PKWARE APPNOTE
// 6.3), written for the tracked-changes .docx export (analysis-suite plan
// F6) and kept general because an OPC package is just a zip. Created
// 2026-09-09.
//
// No zlib, no third-party code: allcore only links `z` under
// USE_SYSTEM_SQLITE=ON, and an export path must not acquire a link
// dependency it can do without. Method 0 (stored) is a byte-for-byte copy,
// so a review document is roughly the size of its text — acceptable, and
// the only honest alternative would be a raw DEFLATE stream (NOT Qt's
// qCompress container, which is a different thing wearing the same name).
//
// DOS timestamps are fixed at 1980-01-01 00:00:00 so that two writes of the
// same parts produce byte-identical archives; the smoke asserts it.
// listStored is public on purpose: it is the first brick of the .docx
// READER, not a test-only convenience.
#pragma once

#include <cstddef>
#include <cstdint>
#include <string>
#include <vector>

namespace allcore {
namespace zipstore {

// CRC-32, reflected polynomial 0xEDB88320 (ISO 3309 / ITU-T V.42) — the
// one the zip format specifies. crc32("123456789") == 0xCBF43926.
std::uint32_t crc32(const std::string& bytes);

struct Part {
    std::string name;    // the path inside the archive, forward slashes, UTF-8
    std::string bytes;   // the part's contents, verbatim
};

// Local file headers (0x04034b50) + central directory (0x02014b50) + EOCD
// (0x06054b50). Method 0, no data descriptors, no ZIP64, no archive
// comment, general-purpose bit 11 set so the UTF-8 names are read as such.
// Parts are written in the order given.
std::string writeStored(const std::vector<Part>& parts);

struct EntryView {
    std::string name;
    std::size_t dataOffset = 0;   // byte offset of the entry's data in the archive
    std::uint32_t size = 0;       // uncompressed == compressed (stored)
    std::uint32_t crc = 0;
};

// Parses the central directory (not the local headers' order) and reports
// each entry. A truncated or non-zip buffer yields an empty vector rather
// than a guess; entries that are not stored (method != 0) are skipped, so a
// caller never mistakes compressed bytes for the part's contents.
std::vector<EntryView> listStored(const std::string& archive);

}  // namespace zipstore
}  // namespace allcore
