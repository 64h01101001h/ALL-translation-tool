// catalog_actions.cpp — see allcore/catalog_actions.h.
#include "allcore/catalog_actions.h"

#include <filesystem>
#include <fstream>
#include <sstream>

namespace allcore {
namespace fs = std::filesystem;
namespace {

const char* kStagingDirName = "AWAITING APPROVAL";
const char* kLedgerName = "ACTIONS.tsv";
const char* kRejectedDirName = "REJECTED";

std::string sanitizeField(std::string s) {
    for (char& c : s)
        if (c == '\t' || c == '\n' || c == '\r') c = ' ';
    return s;
}

// the "NNN META.TXT" companion convention (catalog_name.cpp):
// "<stem up to first space or full stem> META.TXT", uppercase — find
// a companion that actually exists beside src
std::string metaCompanionFor(const fs::path& file) {
    std::string stem = file.stem().string();
    // the number is the first '_'-field of the name
    const size_t u = stem.find('_');
    if (u != std::string::npos) stem = stem.substr(0, u);
    for (const char* pat : {" META.TXT", " META.txt", " META.RTF"}) {
        fs::path cand = file.parent_path() / (stem + pat);
        std::error_code ec;
        if (fs::exists(cand, ec)) return cand.string();
    }
    return "";
}

}  // namespace

ActionLedger::ActionLedger(const std::string& official_root)
    : root_(official_root) {}

std::string ActionLedger::stagingDir() const {
    return (fs::path(root_) / kStagingDirName).string();
}

std::string ActionLedger::ledgerPath() const {
    return (fs::path(stagingDir()) / kLedgerName).string();
}

int ActionLedger::pendingCount() const {
    int n = 0;
    for (const auto& r : rows_)
        if (r.status == "pending") ++n;
    return n;
}

CatalogAction* ActionLedger::find(const std::string& id) {
    for (auto& r : rows_)
        if (r.id == id) return &r;
    return nullptr;
}

bool ActionLedger::load() {
    rows_.clear();
    std::ifstream in(ledgerPath());
    if (!in) return true;   // no ledger yet = empty, fine
    std::string line;
    bool header = false;
    while (std::getline(in, line)) {
        if (!line.empty() && line.back() == '\r') line.pop_back();
        if (line.empty() || line[0] == '#') continue;
        if (!header) { header = true; continue; }   // column names
        std::vector<std::string> f;
        size_t start = 0;
        while (true) {
            const size_t t = line.find('\t', start);
            if (t == std::string::npos) {
                f.push_back(line.substr(start));
                break;
            }
            f.push_back(line.substr(start, t - start));
            start = t + 1;
        }
        if (f.size() < 11) return false;   // malformed row
        CatalogAction a;
        a.id = f[0]; a.ts = f[1]; a.actor = f[2]; a.action = f[3];
        a.source = f[4]; a.staged_name = f[5]; a.proposed_shelf = f[6];
        a.status = f[7]; a.decided_by = f[8]; a.decided_on = f[9];
        a.note = f[10];
        rows_.push_back(std::move(a));
    }
    return true;
}

bool ActionLedger::save() const {
    std::error_code ec;
    fs::create_directories(stagingDir(), ec);
    std::ofstream out(ledgerPath(), std::ios::trunc);
    if (!out) return false;
    out << "# Cataloging actions awaiting approval - plain ASCII, "
           "one row per action.\n"
        << "# Staged files sit beside this ledger; approval moves "
           "them onto their proposed shelf.\n"
        << "# The mother copy is never touched: staging COPIES from "
           "the intake, it does not move.\n"
        << "id\tts\tactor\taction\tsource\tstaged_name\t"
           "proposed_shelf\tstatus\tdecided_by\tdecided_on\tnote\n";
    for (const auto& r : rows_) {
        out << sanitizeField(r.id) << '\t' << sanitizeField(r.ts)
            << '\t' << sanitizeField(r.actor) << '\t'
            << sanitizeField(r.action) << '\t'
            << sanitizeField(r.source) << '\t'
            << sanitizeField(r.staged_name) << '\t'
            << sanitizeField(r.proposed_shelf) << '\t'
            << sanitizeField(r.status) << '\t'
            << sanitizeField(r.decided_by) << '\t'
            << sanitizeField(r.decided_on) << '\t'
            << sanitizeField(r.note) << '\n';
    }
    // SQA FAIL-2 (critical): this ended `return true;` after an
    // UNCHECKED ofstream, and was measured returning TRUE having
    // written 16,384 of 123,576 bytes.
    //
    // Flush AND CLOSE before judging. flush() alone is NOT enough —
    // measured on a full 2 MB volume, it reported success after
    // writing 155,648 of ~240,000 bytes, because the failure does not
    // surface until the filebuf is closed. Caught by
    // tools/test_shortwrite.sh, which exists for this.
    // (House rule 4: nothing reports success it did not verify.)
    out.flush();
    out.close();
    return !out.fail();
}

std::string ActionLedger::stage(const std::string& src,
                                const std::string& staged_name,
                                const std::string& proposed_shelf,
                                const std::string& actor,
                                const std::string& ts,
                                const std::string& note) {
    std::error_code ec;
    if (!fs::exists(src, ec)) return "";
    fs::create_directories(stagingDir(), ec);
    const fs::path dest = fs::path(stagingDir()) / staged_name;
    if (fs::exists(dest, ec)) return "";   // never overwrite a stage
    fs::copy_file(src, dest, ec);
    if (ec) return "";
    // the META companion travels as a copy too
    const std::string meta = metaCompanionFor(src);
    if (!meta.empty()) {
        const fs::path md =
            fs::path(stagingDir()) / fs::path(meta).filename();
        if (!fs::exists(md, ec)) fs::copy_file(meta, md, ec);
    }
    int mx = 0;
    for (const auto& r : rows_)
        if (r.id.size() > 1 && r.id[0] == 'A')
            mx = std::max(mx, std::atoi(r.id.c_str() + 1));
    CatalogAction a;
    char idb[16];
    std::snprintf(idb, sizeof idb, "A%04d", mx + 1);
    a.id = idb;
    a.ts = ts;
    a.actor = actor;
    a.action = "shelve";
    a.source = src;
    a.staged_name = staged_name;
    a.proposed_shelf = proposed_shelf;
    a.status = "pending";
    a.note = note;
    rows_.push_back(a);
    if (!save()) {
        rows_.pop_back();
        fs::remove(dest, ec);
        return "";
    }
    return a.id;
}

// A rollback rename that says whether it worked. fs::rename with an
// error_code silently does nothing on EXDEV or a read-only parent, and
// a rollback nobody checked is how a "nothing was filed" message ends
// up over a file sitting on the shelf.
static bool restore(const std::filesystem::path& from,
                    const std::filesystem::path& to) {
    std::error_code rc;
    std::filesystem::rename(from, to, rc);
    if (!rc) return true;
    return !std::filesystem::exists(from, rc) &&
           std::filesystem::exists(to, rc);
}

std::string ActionLedger::approve(const std::string& id,
                                  const std::string& approver,
                                  const std::string& date) {
    CatalogAction* a = find(id);
    if (!a) return "no such action";
    if (a->status != "pending") return "already " + a->status;
    std::error_code ec;
    const fs::path staged = fs::path(stagingDir()) / a->staged_name;
    if (!fs::exists(staged, ec)) return "staged file is missing";
    const fs::path shelf = fs::path(root_) / a->proposed_shelf;
    fs::create_directories(shelf, ec);
    const fs::path dest = shelf / a->staged_name;
    if (fs::exists(dest, ec)) return "a file of that name is already on the shelf";
    fs::rename(staged, dest, ec);
    if (ec) return "move failed: " + ec.message();
    // SQA STATIC-1: fs::rename OVERWRITES. metaCompanionFor truncates
    // the stem at the first '_', so any two items numbered 001
    // collide, and an existing shelf META was silently clobbered with
    // no permission trickery at all. Refuse instead, and put the
    // file back where it came from so nothing is half-moved.
    const std::string meta = metaCompanionFor(staged);
    fs::path metaDest;
    if (!meta.empty()) {
        metaDest = shelf / fs::path(meta).filename();
        if (fs::exists(metaDest, ec)) {
            fs::rename(dest, staged, ec);   // undo the move
            return "a META companion of that name is already on the "
                   "shelf; resolve it before approving";
        }
        fs::rename(meta, metaDest, ec);
        if (ec) {
            fs::rename(dest, staged, ec);
            return "the META companion could not be moved; nothing "
                   "was filed";
        }
    }
    const std::string prevStatus = a->status;
    a->status = "approved";
    a->decided_by = approver;
    a->decided_on = date;
    // SQA STATIC-1: save()'s return was DISCARDED here. Measured with
    // ACTIONS.tsv chmod 400: approve() returned "" (success to every
    // caller) while the file sat on the shelf and the ledger on disk
    // still said pending — permanently divergent, and the row became
    // UNCLEARABLE because a second approve() then found the staged
    // file missing. stage(), twelve lines above, has always rolled
    // back correctly on the same failure.
    if (!save()) {
        // Review finding 2026-08-23: these renames ignored their own
        // error_code while the message below asserted the item was
        // still staged — the very "report what you did not verify"
        // shape this function was being fixed for.
        const bool metaBack =
            metaDest.empty() ? true : restore(metaDest, meta);
        const bool fileBack = restore(dest, staged);
        a->status = prevStatus;
        a->decided_by.clear();
        a->decided_on.clear();
        if (fileBack && metaBack)
            return "the ledger could not be written, so nothing was "
                   "filed \u2014 the item is still staged and pending";
        return "the ledger could not be written AND the item could "
               "not be put back; it is now split between the shelf "
               "and the staging area. Move it back by hand before "
               "acting on this row again.";
    }
    return "";
}

std::string ActionLedger::reject(const std::string& id,
                                 const std::string& approver,
                                 const std::string& date,
                                 const std::string& reason) {
    CatalogAction* a = find(id);
    if (!a) return "no such action";
    if (a->status != "pending") return "already " + a->status;
    std::error_code ec;
    const fs::path staged = fs::path(stagingDir()) / a->staged_name;
    const fs::path rejDir = fs::path(stagingDir()) / kRejectedDirName;
    fs::create_directories(rejDir, ec);
    std::string metaSrc;
    fs::path metaDest;
    if (fs::exists(staged, ec)) {
        fs::rename(staged, rejDir / a->staged_name, ec);
        if (ec) return "move failed: " + ec.message();
        metaSrc = metaCompanionFor(staged);
        if (!metaSrc.empty()) {
            metaDest = rejDir / fs::path(metaSrc).filename();
            fs::rename(metaSrc, metaDest, ec);
            if (ec) metaDest.clear();   // nothing to roll back
        }
    }
    const std::string prevStatus = a->status;
    const std::string prevNote = a->note;
    a->status = "rejected";
    a->decided_by = approver;
    a->decided_on = date;
    a->note = reason.empty() ? a->note : reason;
    // SQA STATIC-1, same shape as approve(): a discarded save() left
    // the file moved into rejected/ while the ledger still read
    // pending.
    if (!save()) {
        // Review finding 2026-08-23, two defects in this rollback:
        // the META moved above was NOT brought back, so the pair
        // split and a later approve() filed the text with its catalog
        // metadata stranded in REJECTED/; and the rename results were
        // never checked while the message asserted the item was still
        // staged. Roll back BOTH, then report what is actually true.
        const bool fileBack = restore(rejDir / a->staged_name, staged);
        const bool metaBack =
            metaDest.empty() ? true : restore(metaDest, metaSrc);
        a->status = prevStatus;
        a->note = prevNote;
        a->decided_by.clear();
        a->decided_on.clear();
        if (fileBack && metaBack)
            return "the ledger could not be written, so nothing was "
                   "rejected \u2014 the item is still staged and pending";
        return "the ledger could not be written AND the item could "
               "not be put back; it is now split between the staging "
               "area and REJECTED/. Move it back by hand before "
               "acting on this row again.";
    }
    return "";
}

}  // namespace allcore
