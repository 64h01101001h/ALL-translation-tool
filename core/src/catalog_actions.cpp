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
    // UNCHECKED ofstream. Measured against the real allcore on a
    // volume with a 16,384-byte hole: save() returned TRUE having
    // written 16,384 of 123,576 bytes. The stores either side of it
    // in the same probe reported honestly, because they end with
    // `return (bool)f;` — the rule was right and held only where its
    // author remembered it. Flushing first turns a buffered short
    // write into a stream error we can actually report (house rule 4:
    // nothing reports success that did not verify the bytes landed).
    // Flush AND close before judging. Measured 2026-08-23 on a full
    // 2 MB volume: flush() alone reported success after writing
    // 155,648 of ~240,000 bytes — the failure only surfaces when the
    // filebuf is closed, so a check before close() is still a lie.
    // Caught by tools/test_shortwrite.sh, which is the whole reason
    // that test exists.
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
    const std::string meta = metaCompanionFor(staged);
    if (!meta.empty())
        fs::rename(meta, shelf / fs::path(meta).filename(), ec);
    a->status = "approved";
    a->decided_by = approver;
    a->decided_on = date;
    save();
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
    if (fs::exists(staged, ec)) {
        fs::rename(staged, rejDir / a->staged_name, ec);
        if (ec) return "move failed: " + ec.message();
        const std::string meta = metaCompanionFor(staged);
        if (!meta.empty())
            fs::rename(meta, rejDir / fs::path(meta).filename(), ec);
    }
    a->status = "rejected";
    a->decided_by = approver;
    a->decided_on = date;
    a->note = reason.empty() ? a->note : reason;
    save();
    return "";
}

}  // namespace allcore
