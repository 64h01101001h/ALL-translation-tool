// allcore/versions.h — the document version store (2026-09-09, analysis-suite
// F1). Every Save of a source text, a draft or a manuscript keeps a copy under
// <dataRoot>/library/versions/<docKey>/<stamp>.ver + <stamp>.json, so a
// translator can list the history, compare any kept state with the editor,
// name the one sent to Geshe Michael, and restore — and so every later batch
// write (pre-replace, pre-patch, pre-normalize, pre-restore) has an undo it
// can lean on. No index file: the listing is a directory scan, so nothing
// goes stale.
//
// Qt-free, std::filesystem only, deterministic: the caller supplies stamps and
// clock strings (as backup.h does), so the battery pins exact behaviour.
// Blob bytes are OPAQUE here — the app compresses (codec "qz" = qCompress
// container) or not (codec "raw"); allcore gains no zlib.
//
// Honesty rules this store enforces:
//  - Blob first, record second, each through temp + rename: a crash never
//    leaves a record whose blob is missing. The reverse — a .ver with no
//    .json — is an ORPHAN: list() counts it and never returns it as a version.
//  - Dedupe by SHA-1 against the newest non-autosave entry: an identical
//    re-save mints nothing (Deduplicated, with that entry's stamp), so the
//    version count and the sidecar revision are seen to differ honestly.
//  - One rolling autosave slot per document: an autosave replaces the previous
//    autosave record and blob; any deliberate put (save, milestone, pre-*)
//    removes the slot, because the permanent version now captures that state.
//  - prune() never removes the newest entry, never a pinned one; otherwise
//    oldest-first — autosave, then pre-*, then everything else — until both
//    caps hold. The byte cap counts the UNCOMPRESSED `bytes` from the records,
//    so the user's "MB" matches what Properties shows. Records that cannot be
//    read, and orphan blobs, are never touched and never counted.
//  - readBlob() returns false when the blob is missing, or when the bytes on
//    disk do not hash to the record's sha1 (codec "raw"/empty, where the blob
//    IS the decoded bytes), or when the record itself is unreadable: a restore
//    is a write, and a version whose integrity or provenance cannot be read
//    must not be restorable. For codec "qz" the sha1 is over the DECOMPRESSED
//    bytes, which core cannot see — the app verifies after qUncompress.
//  - A record that cannot be parsed is still listed, with metaOk=false and a
//    problem string — never silently dropped, never guessed at.
#pragma once

#include <string>
#include <vector>

namespace allcore::versions {

// One version's record (<stamp>.json, schema "all-version/1"). Every field is
// provenance written by the caller at save time; the store never invents one.
// statisticsJson is the raw JSON object text ("" = null, with statisticsNote
// saying why — never a silent zero).
struct Meta {
    std::string stamp, path, kind, reason, changeset, encoding, lineEnding,
                codec, sha1, savedBy, savedBySource, savedAt, app, label,
                statisticsJson, statisticsNote;
    int  revision = 0;
    long long bytes = 0;             // UNCOMPRESSED size of the document bytes
    bool pinned = false, bom = false;
    bool metaOk = true;              // false = unreadable record, still listed
    std::string problem;             // why it is unusable ("" when it is fine)
};
// meta.stamp is always the file stem — the identity in a directory-scan store.
// verPath is "" when the record has no blob (problem says so).
struct Entry { Meta meta; std::string verPath, jsonPath; };
// <= 0 on either cap means "no cap" (a zero read from unset settings must
// never wipe a history).
struct Limits { int maxVersions = 200; long long maxBytes = 64LL << 20; };
enum class PutResult { Stored, Deduplicated, Failed };
// stamp: the stamp written (Stored), the matching entry's stamp
// (Deduplicated), "" (Failed). pruned: entries removed by the prune that
// follows a store.
struct PutOutcome { PutResult result = PutResult::Failed; std::string stamp; int pruned = 0; };

// Under dataRoot → completeBaseName (parity with the properties/glossary
// sidecars, so Rename carries the history). Outside → completeBaseName + "~"
// + first 8 hex of sha1(absPath), so two KL0001.ACT from different volumes
// can never share an undo set. An empty dataRoot puts everything "outside".
std::string docKeyFor(const std::string& absPath, const std::string& dataRoot);
std::string versionsDirFor(const std::string& dataRoot, const std::string& docKey);

// blobBytes are OPAQUE to core (the app compresses). meta.sha1 must be the hash
// of the DECODED bytes: dedupe and integrity are both defined on what lands on disk.
// Requirements: a stamp of [A-Za-z0-9_-] not starting with '_' (stamps must
// sort chronologically; on a collision "-N" is appended), a non-empty sha1.
// An empty codec is recorded as "raw"; for raw blobs a bytes of 0 is filled
// from blobBytes.size(). Stores, then prunes with lim.
PutOutcome put(const std::string& dir, const std::string& blobBytes, Meta meta,
               const Limits& lim);

// Ascending by stamp. Records that cannot be parsed come back with metaOk=false
// and a problem string (never dropped, and never filtered out by onlyPath —
// they have no readable path). A .ver with no .json is counted in *orphans
// and is NOT returned as an entry. onlyPath ("" = all) keeps entries whose
// recorded path equals it and counts the rest in *otherPathCount.
std::vector<Entry> list(const std::string& dir, const std::string& onlyPath = "",
                        int* otherPathCount = nullptr, int* orphans = nullptr);

bool readBlob(const std::string& dir, const Entry& e, std::string& out);  // false on missing/unreadable/damaged
int  prune(const std::string& dir, const Limits& lim);   // returns removed count; bumps _meta.json "pruned"
// Moves <versionsRoot>/<oldKey> to <versionsRoot>/<newKey>. False if the
// target exists (or the move fails); true when moved, when the two keys are
// the same, or when there was no history to carry. Record `path` fields keep
// the path recorded at save time — provenance is not rewritten.
bool renameKey(const std::string& versionsRoot, const std::string& oldKey,
               const std::string& newKey);
// Every readable entry under every docKey directory of versionsRoot whose
// record carries this changeset id ("" matches nothing), ascending by stamp.
// The _changesets/ folder (F4's records) is not a docKey and is skipped.
std::vector<Entry> byChangeset(const std::string& versionsRoot, const std::string& id);
std::string sha1Hex(const std::string& bytes);          // RFC 3174 / FIPS 180-4, in-house, lower-case hex

// Flat JSON, no library. Escapes are exact: a label with a tab and a newline
// round-trips byte-identically. parseMeta is strict — a syntax error, a wrong
// value type or a schema other than "all-version/1" returns false and leaves
// `out` untouched (never a half-read record).
std::string serializeMeta(const Meta&);
bool parseMeta(const std::string& json, Meta& out);

// ------------------------------------------------------------ changesets ----
// One batch run — Replace in Files…, Normalize in Files… — as a durable audit
// object: <versionsRoot>/_changesets/<id>.json, schema "all-changeset/1".
// The '_' keeps the folder out of every docKey scan (list() ignores '_' names,
// byChangeset() skips '_' directories), so the record and the pre-replace
// Versions it names can share one root without either seeing the other as its
// own kind. The two find each other from either end: the record lists each
// file's preStamp, and each banked Version carries this id in meta.changeset.
//
// Honesty rules this record enforces:
//  - specJson / scopeJson are the CALLER's spec and scope objects, carried
//    verbatim as raw JSON object text and never interpreted here — core knows
//    nothing of needles, globs or file caps, and must not pretend to. Anything
//    that is not a complete JSON object is written as null, never half-kept.
//  - Every file that was read into the plan is listed with its own `status`
//    (changed | skipped:<reason> | write-failed) — a skip is named, never
//    dropped — plus the pre/post SHA-1s an undo needs to prove the file has
//    not moved since the run.
//  - `undone` is written once, by markChangesetUndone, with the counts the
//    undo actually achieved: a run whose undo skipped a file says so forever.
struct ChangesetFile { std::string path, docKey, preStamp, preSha1, postSha1, eol, status; int occurrences = 0, unticked = 0; bool bom = false; };
struct Changeset {
    std::string id, kind, ranBy, ranBySource, ranAt;      // kind: replace-in-files | normalize-in-files
    std::string specJson, scopeJson;                      // the caller's spec/scope objects as raw JSON object text (opaque here)
    std::vector<ChangesetFile> files;
    int filesRead = 0, withMatches = 0, changed = 0, skipped = 0, writeFailed = 0, occurrences = 0;
    bool undone = false; std::string undoneAt, undoneBy; int undoneRestored = 0, undoneSkipped = 0;
};

std::string changesetsDir(const std::string& versionsRoot);                 // "<root>/_changesets"
// An id obeys the stamp rule — [A-Za-z0-9_-], never leading '_' (that prefix
// is the store's own) — so it is a safe file stem and sorts chronologically.
// Creates the directory; temp + rename, so a reader never sees half a record.
bool writeChangeset(const std::string& versionsRoot, const Changeset& c);
// Strict, exactly as parseMeta is: a missing file, a syntax error, a wrong
// value type or a schema other than "all-changeset/1" returns false and leaves
// `out` untouched.
bool readChangeset(const std::string& versionsRoot, const std::string& id, Changeset& out);
// Ascending (ids sort chronologically). Only well-formed <id>.json stems count:
// _meta.json-style strays, dotfiles and non-.json files are not changesets.
std::vector<std::string> listChangesetIds(const std::string& versionsRoot);
// Reads, fills the undone block, writes back. False when the record cannot be
// read or cannot be written — an undo that could not be recorded is not told.
bool markChangesetUndone(const std::string& versionsRoot, const std::string& id,
                         const std::string& at, const std::string& by, int restored, int skipped);
std::string serializeChangeset(const Changeset&);
bool parseChangeset(const std::string& json, Changeset& out);

}  // namespace allcore::versions
