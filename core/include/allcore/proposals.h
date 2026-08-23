// allcore/proposals.h — the in-house proposal & approval channel
// (Adam's design, 2026-08-08). Translators PROPOSE; the authority
// (Geshe Michael, or Adam as second admin) APPROVES. Deliberately NOT
// a login server: identity is provenance, not protection, and the
// store is a folder of small TSV files — point it at the shared
// Dropbox and the whole team's queue synchronizes with no
// infrastructure, offline-capable, fully audit-trailed.
//
// Boundary (rule: the app imports releases, never owns the data):
//  - REGISTER categories (honorific, high-honorific, pronunciation,
//    abbreviation) apply IN-APP on approval — they are app-native
//    layers;
//  - DICTIONARY/CORPUS categories (word/phrase/clause renderings) are
//    approved in-app but EXPORTED as an approved-candidates package
//    for the data project — never written into the corpus here.
#pragma once

#include <string>
#include <vector>

namespace allcore {

// what is being proposed
enum class ProposalKind {
    Honorific,       // honorific ↔ ordinary pair (register, app-native)
    HighHonorific,   // high-honorific marking (register, app-native)
    Humilific,       // humilific: self-lowering forms (register)
    DoubleHonorific, // both subject and recipient honored (register)
    Pronunciation,   // a pronunciation exception (register, app-native)
    Abbreviation,    // a bsdus-tshig / bskungs-yig candidate (register)
    WordRendering,   // English for a word (dictionary — export)
    PhraseRendering, // English for a phrase/clause (dictionary — export)
    Note,            // free note about a passage (export)
    Idiom,           // idiom / fixed-expression candidate for the
                     //   idioms register (export — the banked list
                     //   updates from rulings, app never edits data)
    Spelling,        // doubted spelling / input error (export;
                     //   a DECLINED flag = ruled valid, and the
                     //   app stops doubting that form)
    CatalogIdentity, // a proposed identity for an uncataloged text
                     //   (export — approvals become a candidates
                     //   file for the data project; the app NEVER
                     //   writes the catalog, numbers stay
                     //   registrar-issued)
    Unknown
};

enum class ProposalStatus { Pending, Approved, Declined, Deferred };

struct Proposal {
    std::string id;          // stable unique id (kind+timestamp+seq)
    ProposalKind kind = ProposalKind::Unknown;
    ProposalStatus status = ProposalStatus::Pending;
    std::string proposer;    // provenance: who proposed
    std::string created;     // ISO date the proposal was made
    // the payload — meaning depends on kind:
    std::string wylie;       // the Tibetan (ACIP or wylie) in question
    std::string value;       // proposed English / pronunciation / expansion
    std::string field;       // secondary slot: ordinary form, level, domain…
    std::string evidence;    // auto-captured: source text / passage / where
    // ruling (set on approve/decline/defer):
    std::string approver;    // who ruled
    std::string ruled;       // ISO date of the ruling
    std::string comment;     // the authority's optional note

    // register categories apply in-app; dictionary categories export
    bool isRegister() const {
        return kind == ProposalKind::Honorific ||
               kind == ProposalKind::HighHonorific ||
               kind == ProposalKind::Humilific ||
               kind == ProposalKind::DoubleHonorific ||
               kind == ProposalKind::Pronunciation ||
               kind == ProposalKind::Abbreviation;
    }
    static const char* kindName(ProposalKind k);
    static ProposalKind kindFromName(const std::string& s);
    static const char* statusName(ProposalStatus s);
    static ProposalStatus statusFromName(const std::string& s);
};

// A folder-backed store: one TSV row per proposal in <dir>/proposals.tsv
// (append-only for adds; rewritten on ruling). Safe to point at a
// Dropbox-synced folder; concurrent writers are rare in-house and the
// last write wins per row by id.
class ProposalStore {
public:
    explicit ProposalStore(const std::string& dir);
    bool load();
    // [[nodiscard]] for the same reason as ActionLedger::save:
    // a discarded false here WAS the critical SQA FAIL-2.
    [[nodiscard]]  bool save();
    const std::vector<Proposal>& all() const { return items_; }
    // add a fresh pending proposal; returns its id
    std::string propose(ProposalKind kind, const std::string& proposer,
                        const std::string& wylie, const std::string& value,
                        const std::string& field,
                        const std::string& evidence,
                        const std::string& isoDate);
    // rule on one by id; returns false if not found
    bool rule(const std::string& id, ProposalStatus status,
              const std::string& approver, const std::string& comment,
              const std::string& isoDate);
    size_t pendingCount() const;

    // S1 (stewardship): the many-hands surface. load() also absorbs
    // Dropbox "conflicted copy" siblings — rows with unseen ids join
    // the queue; same-id rows that DIFFER are counted, never merged
    // by guess (the main file wins; the conflicted file is left on
    // disk for a human). save() reloads the file first and keeps any
    // rows another machine added while we held ours in memory.
    const std::vector<std::string>& conflictFiles() const {
        return conflictFiles_;
    }
    size_t absorbedRows() const { return absorbed_; }
    size_t divergentRows() const { return divergent_; }

private:
    std::string dir_;
    std::vector<Proposal> items_;
    std::vector<std::string> conflictFiles_;
    size_t absorbed_ = 0;
    size_t divergent_ = 0;
};

// escape/unescape a field for single-line TSV storage
std::string tsvEscape(const std::string& s);
std::string tsvUnescape(const std::string& s);

}  // namespace allcore
