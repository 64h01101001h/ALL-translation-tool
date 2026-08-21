// allcore/catalog_actions.h — the in-house approval pipeline for
// cataloging actions (Adam's directive, 2026-08-20).
//
// The rule extends the charter: catalogers other than the approver
// (Geshe Michael) never place material on the official shelves
// directly. Their actions STAGE — the file is COPIED (mother copy
// untouched) into "<official root>/AWAITING APPROVAL/" under its
// proposed catalog name, and an append-style ledger row records who
// did what, when, from where, onto which proposed shelf, with the
// evidence summary. The approver reads the ledger, then approves
// (the staged file moves onto its proposed shelf) or rejects (it
// moves to AWAITING APPROVAL/REJECTED/, row annotated). Because the
// official root is the team's synced Dropbox folder, the staged
// files, the ledger, and every approved placement propagate through
// the share by themselves — the app never talks to Dropbox, it just
// works inside the folder Dropbox carries.
//
// The ledger is plain ASCII TSV ("ACTIONS.tsv" beside the staged
// files) — the future-proofing doctrine applied to the approval
// trail itself; GMR can read the list in any editor, forever.
#pragma once

#include <string>
#include <vector>

namespace allcore {

struct CatalogAction {
    std::string id;             // stable row id ("A0001", …)
    std::string ts;             // ISO date-time, minute precision
    std::string actor;          // roster name of who staged it
    std::string action;         // "shelve" (v1; kinds may grow)
    std::string source;         // where the file came from (intake path)
    std::string staged_name;    // filename inside AWAITING APPROVAL
    std::string proposed_shelf; // shelf path RELATIVE to official root
    std::string status;         // pending | approved | rejected
    std::string decided_by;     // approver name ("" while pending)
    std::string decided_on;     // ISO date ("" while pending)
    std::string note;           // evidence summary / rejection reason
};

// The staging area + ledger under one official root.
class ActionLedger {
public:
    // official_root: the team's shared (Dropbox-synced) library root.
    explicit ActionLedger(const std::string& official_root);

    // Loads ACTIONS.tsv if present. Returns false only on a
    // malformed file (missing file = empty ledger, fine).
    bool load();
    bool save() const;

    const std::vector<CatalogAction>& all() const { return rows_; }
    int pendingCount() const;

    // Stage a copy of src (plus its "NNN META.TXT" companion when one
    // exists) into AWAITING APPROVAL under staged_name, and append a
    // pending row. ts is supplied by the caller (minute ISO). Returns
    // the new row id, or "" on failure (nothing half-done: a failed
    // copy leaves no row).
    std::string stage(const std::string& src,
                      const std::string& staged_name,
                      const std::string& proposed_shelf,
                      const std::string& actor, const std::string& ts,
                      const std::string& note);

    // Approve row `id`: move the staged file (+ META companion) onto
    // its proposed shelf (created under the official root if absent),
    // mark the row. Returns "" on success, else the error.
    std::string approve(const std::string& id,
                        const std::string& approver,
                        const std::string& date);

    // Reject row `id`: move the staged file (+ companion) into
    // AWAITING APPROVAL/REJECTED/, mark the row with the reason.
    std::string reject(const std::string& id,
                       const std::string& approver,
                       const std::string& date,
                       const std::string& reason);

    std::string stagingDir() const;   // <root>/AWAITING APPROVAL
    std::string ledgerPath() const;   // …/ACTIONS.tsv

private:
    std::string root_;
    std::vector<CatalogAction> rows_;
    CatalogAction* find(const std::string& id);
};

}  // namespace allcore
