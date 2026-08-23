#include "allcore/proposals.h"

#include <cctype>
#include <filesystem>
#include <fstream>
#include <sstream>

namespace allcore {

const char* Proposal::kindName(ProposalKind k) {
    switch (k) {
        case ProposalKind::Honorific: return "honorific";
        case ProposalKind::HighHonorific: return "high-honorific";
        case ProposalKind::Humilific: return "humilific";
        case ProposalKind::DoubleHonorific: return "double-honorific";
        case ProposalKind::Pronunciation: return "pronunciation";
        case ProposalKind::Abbreviation: return "abbreviation";
        case ProposalKind::WordRendering: return "word";
        case ProposalKind::PhraseRendering: return "phrase";
        case ProposalKind::Note: return "note";
        case ProposalKind::Idiom: return "idiom";
        case ProposalKind::Spelling: return "spelling";
        case ProposalKind::CatalogIdentity: return "catalog-identity";
        default: return "unknown";
    }
}

ProposalKind Proposal::kindFromName(const std::string& s) {
    if (s == "honorific") return ProposalKind::Honorific;
    if (s == "high-honorific") return ProposalKind::HighHonorific;
    if (s == "humilific") return ProposalKind::Humilific;
    if (s == "double-honorific") return ProposalKind::DoubleHonorific;
    if (s == "pronunciation") return ProposalKind::Pronunciation;
    if (s == "abbreviation") return ProposalKind::Abbreviation;
    if (s == "word") return ProposalKind::WordRendering;
    if (s == "phrase") return ProposalKind::PhraseRendering;
    if (s == "note") return ProposalKind::Note;
    if (s == "idiom") return ProposalKind::Idiom;
    if (s == "spelling") return ProposalKind::Spelling;
    if (s == "catalog-identity") return ProposalKind::CatalogIdentity;
    return ProposalKind::Unknown;
}

const char* Proposal::statusName(ProposalStatus s) {
    switch (s) {
        case ProposalStatus::Pending: return "pending";
        case ProposalStatus::Approved: return "approved";
        case ProposalStatus::Declined: return "declined";
        case ProposalStatus::Deferred: return "deferred";
    }
    return "pending";
}

ProposalStatus Proposal::statusFromName(const std::string& s) {
    if (s == "approved") return ProposalStatus::Approved;
    if (s == "declined") return ProposalStatus::Declined;
    if (s == "deferred") return ProposalStatus::Deferred;
    return ProposalStatus::Pending;
}

std::string tsvEscape(const std::string& s) {
    std::string out;
    out.reserve(s.size());
    for (char c : s) {
        if (c == '\\') out += "\\\\";
        else if (c == '\t') out += "\\t";
        else if (c == '\n') out += "\\n";
        else if (c == '\r') out += "\\r";
        else out += c;
    }
    return out;
}

std::string tsvUnescape(const std::string& s) {
    std::string out;
    out.reserve(s.size());
    for (size_t i = 0; i < s.size(); ++i) {
        if (s[i] == '\\' && i + 1 < s.size()) {
            char n = s[++i];
            if (n == '\\') out += '\\';
            else if (n == 't') out += '\t';
            else if (n == 'n') out += '\n';
            else if (n == 'r') out += '\r';
            else out += n;
        } else {
            out += s[i];
        }
    }
    return out;
}

namespace {
const char* kHeader =
    "# id\tkind\tstatus\tproposer\tcreated\twylie\tvalue\tfield\t"
    "evidence\tapprover\truled\tcomment";

std::vector<std::string> splitTab(const std::string& line) {
    std::vector<std::string> cols;
    std::string cur;
    for (char c : line) {
        if (c == '\t') { cols.push_back(cur); cur.clear(); }
        else cur += c;
    }
    cols.push_back(cur);
    return cols;
}
}  // namespace

ProposalStore::ProposalStore(const std::string& dir) : dir_(dir) {}

namespace {
bool parseProposalsFile(const std::string& path,
                        std::vector<Proposal>& out) {
    std::ifstream f(path);
    if (!f) return false;
    std::string line;
    while (std::getline(f, line)) {
        if (!line.empty() && line.back() == '\r') line.pop_back();
        if (line.empty() || line[0] == '#') continue;
        auto c = splitTab(line);
        if (c.size() < 12) continue;
        Proposal p;
        p.id = tsvUnescape(c[0]);
        p.kind = Proposal::kindFromName(c[1]);
        p.status = Proposal::statusFromName(c[2]);
        p.proposer = tsvUnescape(c[3]);
        p.created = tsvUnescape(c[4]);
        p.wylie = tsvUnescape(c[5]);
        p.value = tsvUnescape(c[6]);
        p.field = tsvUnescape(c[7]);
        p.evidence = tsvUnescape(c[8]);
        p.approver = tsvUnescape(c[9]);
        p.ruled = tsvUnescape(c[10]);
        p.comment = tsvUnescape(c[11]);
        out.push_back(std::move(p));
    }
    return true;
}
std::string rowFingerprint(const Proposal& p) {
    return p.kindName(p.kind) + std::string("|") +
           Proposal::statusName(p.status) + "|" + p.proposer + "|" +
           p.created + "|" + p.wylie + "|" + p.value + "|" + p.field +
           "|" + p.evidence + "|" + p.approver + "|" + p.ruled + "|" +
           p.comment;
}
}  // namespace

bool ProposalStore::load() {
    items_.clear();
    conflictFiles_.clear();
    absorbed_ = 0;
    divergent_ = 0;
    const bool ok = parseProposalsFile(dir_ + "/proposals.tsv", items_);
    // S1: Dropbox conflict siblings — absorb unseen rows, count (not
    // guess-merge) diverging ones, leave the files on disk untouched
    std::error_code ec;
    for (const auto& e :
         std::filesystem::directory_iterator(dir_, ec)) {
        const std::string name = e.path().filename().string();
        if (name.rfind("proposals", 0) != 0 ||
            name.find("conflicted copy") == std::string::npos ||
            name.size() < 4 ||
            name.substr(name.size() - 4) != ".tsv")
            continue;
        std::vector<Proposal> rows;
        if (!parseProposalsFile(e.path().string(), rows)) continue;
        conflictFiles_.push_back(name);
        for (auto& r : rows) {
            const Proposal* mine = nullptr;
            for (const auto& p : items_)
                if (p.id == r.id) { mine = &p; break; }
            if (!mine) {
                items_.push_back(std::move(r));
                ++absorbed_;
            } else if (rowFingerprint(*mine) != rowFingerprint(r)) {
                ++divergent_;
            }
        }
    }
    return ok;
}

bool ProposalStore::save() {
    // S1: another machine may have appended since our load — keep any
    // ids we have never seen (our own rows stay authoritative for
    // their ids; the single-authority model rules same-id edits)
    {
        std::vector<Proposal> disk;
        if (parseProposalsFile(dir_ + "/proposals.tsv", disk))
            for (auto& d : disk) {
                bool have = false;
                for (const auto& p : items_)
                    if (p.id == d.id) { have = true; break; }
                if (!have) items_.push_back(std::move(d));
            }
    }
    std::ofstream f(dir_ + "/proposals.tsv", std::ios::trunc);
    if (!f) return false;
    f << kHeader << "\n";
    for (const auto& p : items_) {
        f << tsvEscape(p.id) << '\t' << Proposal::kindName(p.kind) << '\t'
          << Proposal::statusName(p.status) << '\t'
          << tsvEscape(p.proposer) << '\t' << tsvEscape(p.created) << '\t'
          << tsvEscape(p.wylie) << '\t' << tsvEscape(p.value) << '\t'
          << tsvEscape(p.field) << '\t' << tsvEscape(p.evidence) << '\t'
          << tsvEscape(p.approver) << '\t' << tsvEscape(p.ruled) << '\t'
          << tsvEscape(p.comment) << "\n";
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
    f.flush();
    f.close();
    return !f.fail();
}

std::string ProposalStore::propose(ProposalKind kind,
                                   const std::string& proposer,
                                   const std::string& wylie,
                                   const std::string& value,
                                   const std::string& field,
                                   const std::string& evidence,
                                   const std::string& isoDate) {
    Proposal p;
    // id: kind + date + running count — stable, human-legible, unique
    // enough for an in-house queue (the store rewrites by id on ruling)
    std::ostringstream id;
    std::string ini;
    bool word = true;
    for (char ch : proposer) {
        if (word && std::isalpha((unsigned char)ch))
            ini += (char)std::tolower((unsigned char)ch);
        word = (ch == ' ' || ch == '-' || ch == '.');
    }
    if (ini.empty()) ini = "x";
    // S1: two machines proposing the same day no longer collide —
    // the proposer's initials join the id (kind-date-initials-N)
    id << Proposal::kindName(kind) << "-" << isoDate << "-" << ini
       << "-" << (items_.size() + 1);
    p.id = id.str();
    p.kind = kind;
    p.status = ProposalStatus::Pending;
    p.proposer = proposer;
    p.created = isoDate;
    p.wylie = wylie;
    p.value = value;
    p.field = field;
    p.evidence = evidence;
    items_.push_back(p);
    return p.id;
}

bool ProposalStore::rule(const std::string& id, ProposalStatus status,
                         const std::string& approver,
                         const std::string& comment,
                         const std::string& isoDate) {
    for (auto& p : items_) {
        if (p.id == id) {
            p.status = status;
            p.approver = approver;
            p.comment = comment;
            p.ruled = isoDate;
            return true;
        }
    }
    return false;
}

size_t ProposalStore::pendingCount() const {
    size_t n = 0;
    for (const auto& p : items_)
        if (p.status == ProposalStatus::Pending) ++n;
    return n;
}

}  // namespace allcore
