// tablediff_smoke — the table-differ battery (F7 Table Compare,
// acceptance tests 7-16). Fixture-free: every table is built here, so
// the battery runs anywhere.
#include <cstdio>
#include <string>
#include <vector>

#include "allcore/tablediff.h"

using allcore::table::Row;
using allcore::table::Table;
using namespace allcore::tablediff;

static int failures = 0;
#define CHECK(cond, msg) do { if (cond) std::printf("  [PASS] %s\n", msg); else { std::printf("  [FAIL] %s\n", msg); ++failures; } } while (0)

static bool has(const std::string& hay, const std::string& needle) {
    return hay.find(needle) != std::string::npos;
}

// A table straight from the caller: header row at line 1, data from 2.
static Table mk(const std::vector<std::string>& cols,
                const std::vector<std::vector<std::string>>& rows) {
    Table t;
    t.columns = cols;
    t.headerFromFile = true;
    int ln = 2;
    for (const auto& r : rows) {
        Row row;
        row.cells = r;
        row.sourceLine = ln++;
        t.rows.push_back(row);
    }
    return t;
}

static const RowDiff* firstWith(const DiffResult& r, RowStatus s) {
    for (const auto& d : r.rows)
        if (d.status == s) return &d;
    return nullptr;
}

int main() {
    setvbuf(stdout, nullptr, _IONBF, 0);

    // 7 — the plain full outer join
    {
        const Table A = mk({"k", "v"}, {{"a", "1"}, {"b", "2"}});
        const Table B = mk({"k", "v"}, {{"a", "1"}, {"b", "3"}, {"c", "4"}});
        DiffOptions o;
        o.keyColumns = {"k"};
        const DiffResult r = diffTables(A, B, o);
        CHECK(r.same == 1 && r.changed == 1 && r.rightOnly == 1 && r.leftOnly == 0 &&
                  r.ambiguous == 0 && r.rows.size() == 3,
              "T7 one identical, one changed, one only on the right, none only on the "
              "left");
        const RowDiff* ch = firstWith(r, RowStatus::Changed);
        CHECK(ch && ch->key == "b" && ch->fields.size() == 1 &&
                  ch->fields[0].column == "v" && ch->fields[0].left == "2" &&
                  ch->fields[0].right == "3",
              "T7 the changed row carries one FieldDiff naming the column and both "
              "readings");
        const RowDiff* ro = firstWith(r, RowStatus::RightOnly);
        CHECK(ro && ro->key == "c" && ro->leftRow == -1 && ro->rightRow == 2 &&
                  r.leftRows == 2 && r.rightRows == 3 && r.columns.size() == 2,
              "T7 the right-only row has no left row, and both row counts are recorded");
    }

    // 8 — multiplicity is checked FIRST: a duplicated key is never paired
    {
        const Table A = mk({"k", "v"}, {{"k", "1"}, {"k", "2"}});
        const Table B = mk({"k", "v"}, {{"k", "1"}});
        DiffOptions o;
        o.keyColumns = {"k"};
        const DiffResult r = diffTables(A, B, o);
        CHECK(r.rows.size() == 1 && r.rows[0].status == RowStatus::AmbiguousKey &&
                  r.ambiguous == 1,
              "T8 the duplicated key yields exactly ONE row, AmbiguousKey");
        CHECK(r.rows[0].leftDupes == 2 && r.rows[0].rightDupes == 1 &&
                  r.rows[0].fields.empty() && r.leftDupGroups == 1 &&
                  r.rightDupGroups == 0,
              "T8 both group sizes are reported and no field diff is offered");
        CHECK(r.same == 0 && r.changed == 0 && r.leftOnly == 0 && r.rightOnly == 0 &&
                  r.rows[0].leftRow == -1 && r.rows[0].rightRow == -1,
              "T8 no row of A is paired with B's — not even by position");
    }

    // 9 — composite-key rescue: the same fixture, keyed on both columns
    {
        const Table A = mk({"k", "v"}, {{"k", "1"}, {"k", "2"}});
        const Table B = mk({"k", "v"}, {{"k", "1"}, {"k", "2"}});
        DiffOptions one;
        one.keyColumns = {"k"};
        const DiffResult amb = diffTables(A, B, one);
        CHECK(amb.rows.size() == 1 && amb.ambiguous == 1 && amb.rows[0].leftDupes == 2 &&
                  amb.rows[0].rightDupes == 2,
              "T9 keyed on one column the fixture is one ambiguous group");
        DiffOptions two;
        two.keyColumns = {"k", "v"};
        const DiffResult r = diffTables(A, B, two);
        CHECK(r.rows.size() == 2 && r.same == 2 && r.ambiguous == 0 &&
                  r.rows[0].leftRow == 0 && r.rows[0].rightRow == 0 &&
                  r.rows[1].leftRow == 1 && r.rows[1].rightRow == 1,
              "T9 keyed on both columns the same fixture is two paired records, "
              "ambiguous == 0");
    }

    // 10 — a column on one side only is never a change to empty
    {
        const Table A = mk({"k", "v", "tibetan_source"},
                           {{"a", "1", "generated-ewts-v27-1"}});
        const Table B = mk({"k", "v"}, {{"a", "1"}});
        DiffOptions o;
        o.keyColumns = {"k"};
        const DiffResult r = diffTables(A, B, o);
        CHECK(r.leftOnlyColumns == std::vector<std::string>({"tibetan_source"}) &&
                  r.rightOnlyColumns.empty() &&
                  r.columns == std::vector<std::string>({"k", "v"}),
              "T10 the one-sided column is reported as drift; the shared set is the "
              "left order");
        CHECK(r.rows.size() == 1 && r.rows[0].status == RowStatus::Same &&
                  r.rows[0].fields.empty() && r.changed == 0,
              "T10 no FieldDiff is produced for it — never 'changed to empty'");
        DiffOptions named = o;
        named.compareColumns = {"v", "tibetan_source"};
        named.pinnedColumns = {"tibetan_source"};
        const DiffResult r2 = diffTables(A, B, named);
        CHECK(r2.changed == 0 && r2.same == 1 && r2.rows[0].fields.empty() &&
                  !r2.leftProv.refusals.empty() &&
                  has(r2.leftProv.refusals.back(), "tibetan_source") &&
                  r2.refusals.empty(),
              "T10 naming it (or pinning it) does not resurrect it; the provenance "
              "says it was not compared");
    }

    // 11 — list columns are multisets
    {
        DiffOptions o;
        o.keyColumns = {"k"};
        o.listColumns = {"g"};
        const Table A = mk({"k", "g"}, {{"a", "a; b"}});
        const Table B = mk({"k", "g"}, {{"a", "b; a"}});
        const DiffResult reordered = diffTables(A, B, o);
        CHECK(reordered.same == 1 && reordered.changed == 0 &&
                  reordered.rows[0].fields.empty(),
              "T11 a reordered list is Same");
        const Table C = mk({"k", "g"}, {{"a", "a; b; c"}});
        const DiffResult added = diffTables(A, C, o);
        CHECK(added.changed == 1 && added.rows[0].fields.size() == 1 &&
                  added.rows[0].fields[0].addedItems ==
                      std::vector<std::string>({"c"}) &&
                  added.rows[0].fields[0].removedItems.empty(),
              "T11 an added item is Changed and the item is named");
        CHECK(added.rows[0].fields[0].left == "a; b" &&
                  added.rows[0].fields[0].right == "a; b; c",
              "T11 the whole cells are still exposed for the app's in-cell marks");
        DiffOptions plain;
        plain.keyColumns = {"k"};
        const DiffResult noList = diffTables(A, B, plain);
        CHECK(noList.changed == 1 && noList.same == 0,
              "T11 with listColumns empty the reordered list is Changed");
    }

    // 12 — cell equality is the caller's rule set, nothing else
    {
        const Table A = mk({"k", "v"}, {{"a", "Chos"}});
        const Table B = mk({"k", "v"}, {{"a", "chos"}});
        DiffOptions o;
        o.keyColumns = {"k"};
        const DiffResult strict = diffTables(A, B, o);
        o.cellRules.ignoreCase = true;
        const DiffResult folded = diffTables(A, B, o);
        CHECK(strict.changed == 1 && strict.same == 0,
              "T12 without ignoreCase, \"Chos\" and \"chos\" are Changed");
        CHECK(folded.same == 1 && folded.changed == 0,
              "T12 cellRules.ignoreCase makes them Same");
        CHECK(has(folded.leftProv.norm, "ignore case") &&
                  has(folded.leftProv.norm, "case-sensitive"),
              "T12 the provenance states both halves of the rule (cells folded, key "
              "not)");
    }

    // 13 — key case sensitivity is its own decision
    {
        const Table A = mk({"k", "v"}, {{"paN chen", "x"}, {"pan chen", "y"}});
        const Table B = mk({"k", "v"}, {{"paN chen", "x"}, {"pan chen", "y"}});
        DiffOptions o;
        o.keyColumns = {"k"};
        const DiffResult sensitive = diffTables(A, B, o);
        CHECK(sensitive.rows.size() == 2 && sensitive.same == 2 &&
                  sensitive.ambiguous == 0,
              "T13 keyCaseSensitive == true: two records");
        o.keyCaseSensitive = false;
        const DiffResult folded = diffTables(A, B, o);
        CHECK(folded.rows.size() == 1 && folded.ambiguous == 1 &&
                  folded.rows[0].status == RowStatus::AmbiguousKey &&
                  folded.rows[0].leftDupes == 2 && folded.rows[0].rightDupes == 2,
              "T13 keyCaseSensitive == false: one AmbiguousKey group, never merged "
              "silently");
    }

    // 14 — pinned provenance columns are compared whatever the picker says
    {
        const Table A = mk({"wylie", "gloss", "hgm_evidence", "note"},
                           {{"chos", "dharma", "curated", "left note"}});
        const Table B = mk({"wylie", "gloss", "hgm_evidence", "note"},
                           {{"chos", "dharma", "auto-aligned provisional", "right note"}});
        DiffOptions o;
        o.keyColumns = {"wylie"};
        o.compareColumns = {"gloss"};
        o.pinnedColumns = {"hgm_evidence"};
        const DiffResult r = diffTables(A, B, o);
        CHECK(r.changed == 1 && r.rows[0].fields.size() == 1 &&
                  r.rows[0].fields[0].column == "hgm_evidence" &&
                  r.rows[0].fields[0].left == "curated" &&
                  r.rows[0].fields[0].right == "auto-aligned provisional",
              "T14 a difference in the pinned provenance column is reported even "
              "though only gloss was ticked");
        CHECK(r.rows[0].fields.size() == 1,
              "T14 the unticked, unpinned 'note' column is still not compared");
    }

    // 15 — refusal over guess
    {
        const Table A = mk({"wylie", "gloss"}, {{"chos", "dharma"}});
        const Table B = mk({"wylie", "gloss"}, {{"chos", "the Dharma"}});
        DiffOptions o;   // no key columns at all
        const DiffResult r = diffTables(A, B, o);
        CHECK(r.rows.empty() && r.same == 0 && r.changed == 0 && r.leftOnly == 0 &&
                  r.rightOnly == 0,
              "T15 with no key column nothing is compared — no fallback to column 0");
        CHECK(!r.refusals.empty() && r.refusals[0] == "no key column chosen",
              "T15 refusals[0] is exactly \"no key column chosen\"");
        bool named = false;
        for (const auto& s : r.refusals)
            if (has(s, "wylie") && has(s, "gloss")) named = true;
        CHECK(named && r.refusals.size() >= 2,
              "T15 the available column names are given with the refusal");
        DiffOptions missing;
        missing.keyColumns = {"headword"};
        const DiffResult r2 = diffTables(A, B, missing);
        CHECK(r2.rows.empty() && !r2.refusals.empty() &&
                  has(r2.refusals[0], "headword") && has(r2.refusals[0], "not present"),
              "T15 a key column that is not there is refused too, by name");
    }

    // 16 — the report writers
    {
        const Table A = mk({"k", "v"}, {{"a", "plain"}, {"b", "same"}, {"d", "gone"}});
        const Table B = mk({"k", "v"}, {{"a", "has, commas"}, {"b", "same"}, {"e", "new"}});
        DiffOptions o;
        o.keyColumns = {"k"};
        const DiffResult r = diffTables(A, B, o);
        const std::string csv = diffCsv(r);
        CHECK(csv.substr(0, csv.find('\n')) == "status,key,column,left,right",
              "T16 the first line is exactly status,key,column,left,right");
        CHECK(has(csv, "\"has, commas\"") && has(csv, "changed,\"a\",\"v\","),
              "T16 a value containing a comma is quoted");
        CHECK(has(csv, "same,\"b\",\"\",\"\",\"\"") &&
                  has(csv, "left-only,\"d\",\"\",\"\",\"\"") &&
                  has(csv, "right-only,\"e\",\"\",\"\",\"\""),
              "T16 one line with an empty column/left/right for the unchanged and "
              "one-sided rows");
        const std::string md = diffMarkdown(r, "left.tsv", "right.tsv");
        CHECK(has(md, "engine: tablediff/1") && has(md, "left.tsv") &&
                  has(md, "right.tsv") && has(md, "key: k, case-sensitive"),
              "T16 the Markdown carries both provenance blocks and the key");
        CHECK(has(md, "Rows read: 3 on the left, 3 on the right") &&
                  has(md, "Rows compared: 2") && has(md, "Rows refused: 0") &&
                  has(md, "| Key | Column | Left | Right |") &&
                  has(md, "never paired"),
              "T16 read / compared / refused are three separate numbers, and the "
              "ambiguous-key rule is stated");
    }

    std::printf("tablediff_smoke: %d failure(s)\n", failures);
    return failures ? 1 : 0;
}
