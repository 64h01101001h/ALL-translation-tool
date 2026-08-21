// allcore/backup.h — the safety net for human judgments (S2).
//
// The data hierarchy (STEWARDSHIP_PROMPT §2): anything a human typed
// outranks anything the tool computed. These stores get rolling,
// dated, bounded backups and a restore path — automatic, quiet, and
// drilled by the battery.
//
// Deterministic by design: the caller supplies the stamp (no clock
// in here), so drills pin exact behavior.
#pragma once

#include <string>
#include <vector>

namespace allcore {

// Copy src into backupDir as "<basename>.<stamp>.bak", then prune the
// oldest same-basename backups beyond `keep`. Returns false when src
// is unreadable or the copy fails; pruning failures are ignored (a
// failed prune never blocks a backup). An identical existing stamp is
// overwritten (idempotent per stamp).
bool backupFile(const std::string& src, const std::string& backupDir,
                const std::string& stamp, int keep);

// Newest same-basename backup in backupDir (lexicographic stamp order
// — the caller's stamps must sort chronologically, e.g. ISO). Empty
// when none exist.
std::string latestBackup(const std::string& backupDir,
                         const std::string& basename);

// Copy the named backup over dst. Never touches the backup itself.
bool restoreBackup(const std::string& backupPath,
                   const std::string& dst);

}  // namespace allcore
