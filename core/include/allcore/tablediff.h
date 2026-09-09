// allcore/tablediff.h — the table differ (F7 Table Compare, the differ
// half; core/include/allcore/table.h is the reader half).
//
// One job: a relational FULL OUTER JOIN of two tables by key, and an
// honest account of what could not be joined. The record-level form of
// the file-level classification the folder compare already performs.
//
// The house rules this file keeps:
//   * MULTIPLICITY IS CHECKED FIRST. A key that occurs more than once
//     on either side becomes ONE AmbiguousKey row carrying both group
//     sizes and NO field diff. Pairing duplicates by position would be
//     a guess, and the release master has 56 duplicated `wylie` values
//     in 105,634 entries. leftRow/rightRow stay -1 for such a row, so
//     nothing downstream can accidentally present a pairing.
//   * NO KEY MEANS NO ANSWER. Empty keyColumns is refused with the
//     available column names — never a silent fallback to column 0.
//   * A column present on ONE side is never reported as "changed to
//     empty". Column sets are intersected by name; the drift is
//     reported in leftOnlyColumns / rightOnlyColumns.
//   * Cell equality is textdiff::normalizeLine under the CALLER's
//     options — the Compare pane's own RULES ribbon drives it, and
//     there is no second rule set anywhere in this file.
//   * The KEY, though, is matched as written (case-folded only when
//     keyCaseSensitive is false). The cell rules govern what counts as
//     the same VALUE; letting them govern identity would let "ignore
//     punctuation" quietly merge two distinct headwords, which is a
//     pairing guess by another name.
//   * In-cell marks are the app's job: this file exposes the left and
//     right strings; textdiff::diffInline marks them.
//
// Pure C++: no Qt, no JSON (the app adapts JSON shapes into a Table and
// hands the Table here), no product code.
#pragma once

#include <string>
#include <vector>

#include "allcore/provenance.h"
#include "allcore/table.h"
#include "allcore/textdiff.h"

namespace allcore {
namespace tablediff {

// Per-side row cap. Above the release master's 105,634 rows, so the
// intended use is not on the refusal boundary. A side over the cap is
// REFUSED whole — comparing the first 250,000 rows of a larger file and
// calling the result a comparison would be a lie about coverage.
inline constexpr int kMaxRowsPerSide = 250000;

enum class RowStatus { Same, Changed, LeftOnly, RightOnly, AmbiguousKey };

struct FieldDiff {
    std::string column, left, right;   // the whole cells, as written
    // List columns only (see DiffOptions::listColumns): the items that
    // are surplus on one side, as written. A reordered list is Same and
    // produces no FieldDiff at all.
    std::vector<std::string> addedItems, removedItems;
};

struct RowDiff {
    RowStatus status = RowStatus::Same;
    std::string key;               // the key cells joined with " | " for display
    int leftRow = -1, rightRow = -1;   // row indices; -1 = no row on that side,
                                       // and ALWAYS -1 for AmbiguousKey
    int leftDupes = 1, rightDupes = 1; // rows carrying this key, per side
    std::vector<FieldDiff> fields;     // the differing columns only
};

struct DiffOptions {
    // keyColumns: empty is refused, not guessed. compareColumns: empty
    // means every shared column except the key columns. listColumns:
    // split on listSeparator and compared as multisets. pinnedColumns:
    // the provenance columns, unioned into compareColumns
    // unconditionally — a run cannot switch its own provenance off.
    std::vector<std::string> keyColumns, compareColumns, listColumns, pinnedColumns;
    textdiff::Options cellRules;
    bool keyCaseSensitive = true;
    std::string listSeparator = "; ";
};

struct DiffResult {
    // What the differ can know: engine, time, rows read/refused per side
    // and the refusal lines. source / source_sha256 are left empty for
    // the caller to fill — only the app knows the paths.
    Provenance leftProv, rightProv;
    std::vector<RowDiff> rows;
    // columns: the SHARED columns, in the left table's order — the set
    // that was comparable at all. The union, when a caller wants it, is
    // the left table's own columns followed by rightOnlyColumns.
    std::vector<std::string> columns, leftOnlyColumns, rightOnlyColumns;
    int same = 0, changed = 0, leftOnly = 0, rightOnly = 0, ambiguous = 0;
    int leftRows = 0, rightRows = 0, leftDupGroups = 0, rightDupGroups = 0;
    // The DIFFER's own refusals, and only those: no key column chosen, a
    // key column missing on one side, a side over the row cap. Non-empty
    // means the comparison did not happen as asked — which is why the
    // CLI treats it as exit 2. The reader's per-row refusals live in the
    // Tables and in leftProv/rightProv.refusals, where a refused row is
    // a fact about that file rather than about this comparison.
    std::vector<std::string> refusals;
};

DiffResult diffTables(const table::Table& a, const table::Table& b,
                      const DiffOptions& o);

// Report writers.
// diffCsv: header line exactly `status,key,column,left,right`, then one
// line per FieldDiff, and one line with an empty column/left/right for
// every LeftOnly, RightOnly, AmbiguousKey and Same row. RFC-4180
// quoting in the apparatusCsv style.
std::string diffCsv(const DiffResult& r);
// diffMarkdown: both sides' provenance blocks, the key and rules, the
// three counts (rows read / compared / refused), the ambiguous-key list,
// then a section per status.
std::string diffMarkdown(const DiffResult& r, const std::string& aName,
                         const std::string& bName);

// ---- presets ---------------------------------------------------------
// The known table shapes, mirroring the Compare pane's preset table so
// the engine and the pane cannot drift into two truths. A profile with
// an empty keyColumn (Generic, Registrar) is a manual choice and never
// auto-detects: the registrar's number column is found by the app with
// normalizeCatalogKey, which is a catalog rule, not a table rule.
struct TableProfile {
    std::string name, keyColumn, secondKey;
    std::vector<std::string> impliedColumns;   // headerless house TSVs
    std::vector<std::string> listColumns;
    std::vector<std::string> pinnedColumns;    // provenance; never unticked
};
const std::vector<TableProfile>& tableProfiles();

// The profile whose basename hint matches `path` AND whose key column is
// present in `sampled` — nullptr when nothing matches, because naming
// the wrong shape is worse than naming none. A headerless house TSV
// (one with impliedColumns) also matches when the sampled table's
// column COUNT equals the implied count, since a file read without the
// preset takes its first data row for a header.
const TableProfile* detectProfile(const std::string& path,
                                  const table::Table& sampled);

}  // namespace tablediff
}  // namespace allcore
