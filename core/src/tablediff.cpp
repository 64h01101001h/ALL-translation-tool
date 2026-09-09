// tablediff.cpp — see allcore/tablediff.h.
#include "allcore/tablediff.h"

#include <cctype>
#include <ctime>
#include <map>
#include <set>
#include <sstream>
#include <unordered_map>
#include <utility>

namespace allcore {
namespace tablediff {

namespace {

std::string nowUtc() {
    const std::time_t t = std::time(nullptr);
    std::tm g{};
#if defined(_WIN32)
    gmtime_s(&g, &t);
#else
    gmtime_r(&t, &g);
#endif
    char buf[32] = {0};
    std::strftime(buf, sizeof buf, "%Y-%m-%dT%H:%M:%SZ", &g);
    return std::string(buf);
}

std::string trimEnds(const std::string& s) {
    size_t b = 0, e = s.size();
    while (b < e && (s[b] == ' ' || s[b] == '\t')) ++b;
    while (e > b && (s[e - 1] == ' ' || s[e - 1] == '\t')) --e;
    return s.substr(b, e - b);
}

std::string lower(const std::string& s) {
    std::string o = s;
    for (auto& c : o) c = (char)std::tolower((unsigned char)c);
    return o;
}

int columnIndex(const table::Table& t, const std::string& name) {
    for (size_t i = 0; i < t.columns.size(); ++i)
        if (t.columns[i] == name) return (int)i;
    return -1;
}

// A cell that is not there reads as empty rather than crashing: the
// reader refuses ragged rows, but a caller may hand-build a Table.
const std::string& cellAt(const table::Table& t, int row, int col) {
    static const std::string kEmpty;
    if (row < 0 || row >= (int)t.rows.size() || col < 0) return kEmpty;
    const auto& cells = t.rows[(size_t)row].cells;
    if (col >= (int)cells.size()) return kEmpty;
    return cells[(size_t)col];
}

// A list cell → its items. An empty cell is NO items, not one empty
// item, so "" against "" is Same and "" against "a" adds exactly "a".
std::vector<std::string> splitList(const std::string& text, const std::string& sep) {
    std::vector<std::string> out;
    if (trimEnds(text).empty()) return out;
    if (sep.empty()) {   // no separator = nothing to split on; treat it as one item
        out.push_back(text);
        return out;
    }
    size_t pos = 0;
    for (;;) {
        const size_t hit = text.find(sep, pos);
        if (hit == std::string::npos) {
            out.push_back(trimEnds(text.substr(pos)));
            break;
        }
        out.push_back(trimEnds(text.substr(pos, hit - pos)));
        pos = hit + sep.size();
    }
    return out;
}

std::string statusName(RowStatus s) {
    switch (s) {
        case RowStatus::Same: return "same";
        case RowStatus::Changed: return "changed";
        case RowStatus::LeftOnly: return "left-only";
        case RowStatus::RightOnly: return "right-only";
        case RowStatus::AmbiguousKey: return "ambiguous-key";
    }
    return "unknown";
}

std::string joinNames(const std::vector<std::string>& v) {
    if (v.empty()) return "none";
    std::string s;
    for (size_t i = 0; i < v.size(); ++i) {
        if (i) s += ", ";
        s += v[i];
    }
    return s;
}

// The comparison rule in one sentence, banked in both sides' Provenance
// so a saved report says how rows were matched and how cells were judged.
std::string ruleSentence(const DiffOptions& o) {
    std::string s = "key: ";
    if (o.keyColumns.empty()) {
        s += "(none chosen)";
    } else {
        for (size_t i = 0; i < o.keyColumns.size(); ++i) {
            if (i) s += " + ";
            s += o.keyColumns[i];
        }
    }
    s += o.keyCaseSensitive ? ", case-sensitive" : ", case-insensitive";
    s += "; cells: " + textdiff::optionsDescription(o.cellRules);
    return s;
}

void fillSide(Provenance& p, const table::Table& t, const DiffOptions& o) {
    p.engine = "tablediff/1";
    p.built_utc = nowUtc();
    p.norm = ruleSentence(o);
    p.units_read = (long long)t.rows.size();
    p.units_refused = t.refusedRows;
    p.refusals = t.refusals;   // the reader's own line-numbered refusals
    // source / source_sha256 stay empty: only the app knows the paths,
    // and provenanceBlock prints an unset field as "not recorded".
}

// Multiset comparison for a declared list column: a reordered list is
// the same list; an added item is named. Returns true when they differ.
bool listDiffers(const std::string& lv, const std::string& rv,
                 const DiffOptions& o, FieldDiff& f) {
    const std::vector<std::string> L = splitList(lv, o.listSeparator);
    const std::vector<std::string> R = splitList(rv, o.listSeparator);
    std::vector<std::string> nL, nR;
    std::map<std::string, int> haveL, haveR;
    nL.reserve(L.size());
    nR.reserve(R.size());
    for (const auto& s : L) {
        nL.push_back(textdiff::normalizeLine(s, o.cellRules));
        ++haveL[nL.back()];
    }
    for (const auto& s : R) {
        nR.push_back(textdiff::normalizeLine(s, o.cellRules));
        ++haveR[nR.back()];
    }
    for (size_t i = 0; i < R.size(); ++i) {   // surplus on the right = added
        auto it = haveL.find(nR[i]);
        if (it != haveL.end() && it->second > 0) --it->second;
        else f.addedItems.push_back(R[i]);
    }
    for (size_t i = 0; i < L.size(); ++i) {   // surplus on the left = removed
        auto it = haveR.find(nL[i]);
        if (it != haveR.end() && it->second > 0) --it->second;
        else f.removedItems.push_back(L[i]);
    }
    return !(f.addedItems.empty() && f.removedItems.empty());
}

}  // namespace

// ---------------------------------------------------------------- join
DiffResult diffTables(const table::Table& a, const table::Table& b,
                      const DiffOptions& o) {
    DiffResult r;
    r.leftRows = (int)a.rows.size();
    r.rightRows = (int)b.rows.size();
    fillSide(r.leftProv, a, o);
    fillSide(r.rightProv, b, o);

    // Column drift, first: worth reporting even when the join below is
    // refused, because it is usually the answer to "why is nothing here".
    std::set<std::string> inA, inB;
    for (const auto& c : a.columns)
        if (!inA.insert(c).second)
            r.leftProv.refusals.push_back("column \"" + c +
                                          "\" appears more than once; the first was used");
    for (const auto& c : b.columns)
        if (!inB.insert(c).second)
            r.rightProv.refusals.push_back("column \"" + c +
                                           "\" appears more than once; the first was used");
    std::set<std::string> seen;
    for (const auto& c : a.columns)
        if (inB.count(c) && seen.insert(c).second) r.columns.push_back(c);
    seen.clear();
    for (const auto& c : a.columns)
        if (!inB.count(c) && seen.insert(c).second) r.leftOnlyColumns.push_back(c);
    seen.clear();
    for (const auto& c : b.columns)
        if (!inA.count(c) && seen.insert(c).second) r.rightOnlyColumns.push_back(c);

    // The cap. A side over it is refused WHOLE: comparing its first
    // 250,000 rows and calling that the answer would misstate coverage.
    if (r.leftRows > kMaxRowsPerSide || r.rightRows > kMaxRowsPerSide) {
        const std::string cap = std::to_string(kMaxRowsPerSide);
        if (r.leftRows > kMaxRowsPerSide) {
            const std::string m = "the left table has " + std::to_string(r.leftRows) +
                                  " rows, over the per-side cap of " + cap +
                                  " — refused whole rather than compared in part";
            r.refusals.push_back(m);
            r.leftProv.refusals.push_back(m);
            r.leftProv.units_refused += r.leftRows;
        }
        if (r.rightRows > kMaxRowsPerSide) {
            const std::string m = "the right table has " + std::to_string(r.rightRows) +
                                  " rows, over the per-side cap of " + cap +
                                  " — refused whole rather than compared in part";
            r.refusals.push_back(m);
            r.rightProv.refusals.push_back(m);
            r.rightProv.units_refused += r.rightRows;
        }
        return r;
    }

    // No key, no answer. Column 0 would be a guess dressed as a default.
    if (o.keyColumns.empty()) {
        r.refusals.push_back("no key column chosen");
        r.refusals.push_back("available columns — left: " + joinNames(a.columns) +
                             "; right: " + joinNames(b.columns));
        return r;
    }
    std::vector<int> keyA, keyB;
    bool keyOk = true;
    for (const auto& k : o.keyColumns) {
        const int ia = columnIndex(a, k), ib = columnIndex(b, k);
        if (ia < 0) {
            r.refusals.push_back("key column \"" + k +
                                 "\" is not present on the left; available columns: " +
                                 joinNames(a.columns));
            keyOk = false;
        }
        if (ib < 0) {
            r.refusals.push_back("key column \"" + k +
                                 "\" is not present on the right; available columns: " +
                                 joinNames(b.columns));
            keyOk = false;
        }
        keyA.push_back(ia);
        keyB.push_back(ib);
    }
    if (!keyOk) return r;

    // The compare set: the shared columns the caller asked for, plus the
    // pinned provenance columns unconditionally. A column that lives on
    // one side only is dropped here and said so in the provenance — it
    // must never surface as "changed to empty".
    const std::set<std::string> shared(r.columns.begin(), r.columns.end());
    const std::set<std::string> keySet(o.keyColumns.begin(), o.keyColumns.end());
    std::vector<std::string> cmp;
    std::set<std::string> inCmp;
    auto want = [&](const std::string& c) {
        if (shared.count(c) && inCmp.insert(c).second) cmp.push_back(c);
    };
    auto noteOneSided = [&](const std::string& c, const char* why) {
        const std::string where = inA.count(c)   ? "only on the left"
                                  : inB.count(c) ? "only on the right"
                                                 : "on neither side";
        const std::string m = "column \"" + c + "\" was " + why + " but is present " + where +
                              "; it was not compared (a one-sided column is never reported "
                              "as changed to empty)";
        r.leftProv.refusals.push_back(m);
        r.rightProv.refusals.push_back(m);
    };
    if (o.compareColumns.empty()) {
        // The default: every shared column except the key itself, whose
        // two sides are equal by construction.
        for (const auto& c : r.columns)
            if (!keySet.count(c)) want(c);
    } else {
        for (const auto& c : o.compareColumns) {
            if (!shared.count(c)) { noteOneSided(c, "named for comparison"); continue; }
            want(c);
        }
    }
    for (const auto& c : o.pinnedColumns) {
        if (!shared.count(c)) { noteOneSided(c, "pinned as provenance"); continue; }
        want(c);   // a run cannot switch its own provenance off
    }
    std::vector<int> cmpA, cmpB;
    std::vector<char> cmpIsList;
    const std::set<std::string> listSet(o.listColumns.begin(), o.listColumns.end());
    for (const auto& c : cmp) {
        cmpA.push_back(columnIndex(a, c));
        cmpB.push_back(columnIndex(b, c));
        cmpIsList.push_back(listSet.count(c) ? (char)1 : (char)0);
    }

    // Build the key groups: left rows in file order, then the keys only
    // the right side has, in its file order. The result reads in the
    // order the user knows their file in.
    struct Group {
        std::string display;
        std::vector<int> left, right;
    };
    std::vector<Group> groups;
    std::unordered_map<std::string, size_t> where;
    // The matching key is joined with a unit separator so a composite
    // key cannot collide ("a|b" + "c" is not "a" + "b|c"); the DISPLAY
    // key is the readable join, and the first side to create the group
    // supplies it.
    auto keyOf = [&](const table::Table& t, const std::vector<int>& idx, int row,
                     std::string& display) {
        std::string k;
        display.clear();
        for (size_t i = 0; i < idx.size(); ++i) {
            const std::string& v = cellAt(t, row, idx[i]);
            if (i) display += " | ";
            display += v;
            k += o.keyCaseSensitive ? v : lower(v);
            k += '\x1f';
        }
        return k;
    };
    for (int i = 0; i < (int)a.rows.size(); ++i) {
        std::string disp;
        const std::string k = keyOf(a, keyA, i, disp);
        auto it = where.find(k);
        if (it == where.end()) {
            where.emplace(k, groups.size());
            groups.push_back({disp, {i}, {}});
        } else {
            groups[it->second].left.push_back(i);
        }
    }
    for (int i = 0; i < (int)b.rows.size(); ++i) {
        std::string disp;
        const std::string k = keyOf(b, keyB, i, disp);
        auto it = where.find(k);
        if (it == where.end()) {
            where.emplace(k, groups.size());
            groups.push_back({disp, {}, {i}});
        } else {
            groups[it->second].right.push_back(i);
        }
    }

    // Classify. MULTIPLICITY FIRST — a duplicated key is answered with
    // the group sizes and nothing else.
    r.rows.reserve(groups.size());
    for (const auto& g : groups) {
        RowDiff d;
        d.key = g.display;
        d.leftDupes = (int)g.left.size();
        d.rightDupes = (int)g.right.size();
        if (g.left.size() > 1) ++r.leftDupGroups;
        if (g.right.size() > 1) ++r.rightDupGroups;
        if (g.left.size() > 1 || g.right.size() > 1) {
            d.status = RowStatus::AmbiguousKey;   // leftRow/rightRow stay -1
            ++r.ambiguous;
        } else if (g.right.empty()) {
            d.status = RowStatus::LeftOnly;
            d.leftRow = g.left[0];
            ++r.leftOnly;
        } else if (g.left.empty()) {
            d.status = RowStatus::RightOnly;
            d.rightRow = g.right[0];
            ++r.rightOnly;
        } else {
            d.leftRow = g.left[0];
            d.rightRow = g.right[0];
            for (size_t c = 0; c < cmp.size(); ++c) {
                const std::string& lv = cellAt(a, d.leftRow, cmpA[c]);
                const std::string& rv = cellAt(b, d.rightRow, cmpB[c]);
                FieldDiff f;
                if (cmpIsList[c]) {
                    if (!listDiffers(lv, rv, o, f)) continue;
                } else if (textdiff::normalizeLine(lv, o.cellRules) ==
                           textdiff::normalizeLine(rv, o.cellRules)) {
                    continue;
                }
                f.column = cmp[c];
                f.left = lv;
                f.right = rv;
                d.fields.push_back(std::move(f));
            }
            if (d.fields.empty()) {
                d.status = RowStatus::Same;
                ++r.same;
            } else {
                d.status = RowStatus::Changed;
                ++r.changed;
            }
        }
        r.rows.push_back(std::move(d));
    }
    return r;
}

// ------------------------------------------------------------- reports
std::string diffCsv(const DiffResult& r) {
    // apparatusCsv's idiom: every text field quoted, its own quotes
    // doubled. A newline inside a quoted field is RFC-4180 legal.
    auto q = [](const std::string& s) {
        std::string o = "\"";
        for (char c : s) {
            if (c == '"') o += "\"\"";
            else o += c;
        }
        return o + "\"";
    };
    std::ostringstream o;
    o << "status,key,column,left,right\n";
    for (const auto& d : r.rows) {
        const std::string st = statusName(d.status);
        if (d.fields.empty()) {
            o << st << "," << q(d.key) << "," << q("") << "," << q("") << "," << q("") << "\n";
            continue;
        }
        for (const auto& f : d.fields)
            o << st << "," << q(d.key) << "," << q(f.column) << "," << q(f.left) << ","
              << q(f.right) << "\n";
    }
    return o.str();
}

namespace {

std::string mdCell(const std::string& s) {
    std::string o;
    for (char c : s) {
        if (c == '|') o += "\\|";
        else if (c == '\n' || c == '\r') o += ' ';
        else o += c;
    }
    return o;
}

void mdSection(std::ostringstream& out, const DiffResult& r, RowStatus s,
               const char* heading) {
    int n = 0;
    for (const auto& d : r.rows)
        if (d.status == s) ++n;
    out << "\n## " << heading << " (" << n << ")\n\n";
    if (n == 0) {
        out << "None.\n";
        return;
    }
    out << "| Key | Column | Left | Right |\n|---|---|---|---|\n";
    for (const auto& d : r.rows) {
        if (d.status != s) continue;
        if (d.fields.empty()) {
            out << "| " << mdCell(d.key) << " |  |  |  |\n";
            continue;
        }
        for (const auto& f : d.fields)
            out << "| " << mdCell(d.key) << " | " << mdCell(f.column) << " | "
                << mdCell(f.left) << " | " << mdCell(f.right) << " |\n";
    }
}

}  // namespace

std::string diffMarkdown(const DiffResult& r, const std::string& aName,
                         const std::string& bName) {
    std::ostringstream o;
    o << "# Table compare\n\n";
    o << "**Left:** " << (aName.empty() ? "not recorded" : aName) << "  \n";
    o << "**Right:** " << (bName.empty() ? "not recorded" : bName) << "  \n";
    o << "**Key and rules:** " << (r.leftProv.norm.empty() ? "not recorded" : r.leftProv.norm)
      << "\n";
    o << "\nNothing is written by a comparison. This report compares; it does not merge.\n";

    o << "\n## Provenance — left\n\n```\n" << provenanceBlock(r.leftProv) << "```\n";
    o << "\n## Provenance — right\n\n```\n" << provenanceBlock(r.rightProv) << "```\n";

    // Three separate numbers, as the pane states them: read, compared, refused.
    const long long refused = r.leftProv.units_refused + r.rightProv.units_refused;
    o << "\n## Summary\n\n";
    o << "- Rows read: " << r.leftRows << " on the left, " << r.rightRows
      << " on the right\n";
    o << "- Rows compared: " << (r.same + r.changed)
      << " (paired records — one row on each side. Ambiguous, left-only and "
         "right-only rows are classified, not compared.)\n";
    o << "- Rows refused: " << refused << " (" << r.leftProv.units_refused
      << " on the left, " << r.rightProv.units_refused
      << " on the right) — listed in the provenance blocks above\n";
    o << "- " << r.same << " identical · " << r.changed << " changed · " << r.leftOnly
      << " only on the left · " << r.rightOnly << " only on the right · " << r.ambiguous
      << " key(s) ambiguous\n";
    o << "- Columns compared on both sides: " << joinNames(r.columns) << "\n";
    o << "- Columns only on the left: " << joinNames(r.leftOnlyColumns) << "\n";
    o << "- Columns only on the right: " << joinNames(r.rightOnlyColumns) << "\n";
    if (!r.leftOnlyColumns.empty() || !r.rightOnlyColumns.empty())
        o << "- Columns present on only one side are never reported as changed to empty.\n";
    if (!r.refusals.empty()) {
        o << "\n## Refused\n\n";
        for (const auto& s : r.refusals) o << "- " << s << "\n";
    }

    o << "\n## Ambiguous keys — never paired (" << r.ambiguous << ")\n\n";
    o << "Rows whose key occurs more than once are listed as AMBIGUOUS KEY and are never "
         "paired by position — pairing them would be a guess. Choose a second key column, "
         "or resolve them in the source.\n\n";
    if (r.ambiguous == 0) {
        o << "None.\n";
    } else {
        o << "| Key | Rows on the left | Rows on the right |\n|---|---|---|\n";
        for (const auto& d : r.rows)
            if (d.status == RowStatus::AmbiguousKey)
                o << "| " << mdCell(d.key) << " | " << d.leftDupes << " | " << d.rightDupes
                  << " |\n";
    }
    o << "\nDuplicate-key groups: " << r.leftDupGroups << " on the left, " << r.rightDupGroups
      << " on the right.\n";

    mdSection(o, r, RowStatus::Changed, "Changed");
    mdSection(o, r, RowStatus::LeftOnly, "Only on the left");
    mdSection(o, r, RowStatus::RightOnly, "Only on the right");
    mdSection(o, r, RowStatus::Same, "Identical");
    return o.str();
}

// ------------------------------------------------------------ presets
const std::vector<TableProfile>& tableProfiles() {
    // Mirrors the Compare pane's preset table (app/table_compare.inc)
    // in the same order, so the engine and the pane state one shape per
    // file rather than two. Names are the pane's names verbatim.
    static const std::vector<TableProfile> P = {
        {"Generic", "", "", {}, {}, {}},
        {"Per-text glossary (TSV, key wylie)", "wylie", "", {"wylie", "gloss"}, {}, {}},
        {"Release dictionary TSV (key wylie)", "wylie", "", {},
         {"wylie_variants", "hgm_gloss_alternatives", "hgm_glosses", "corpus_courses"},
         {"hgm_evidence", "tier", "source_wave"}},
        {"Alignment evidence JSON (key headword+eng)", "headword", "eng", {}, {"refs"},
         {"tier", "source"}},
        // The links adapter emits a course-qualified page ("C01/c1L1",
        // the shape the file's own notes map uses) as page_id, so the
        // pane's two key boxes can carry course+page+id.
        {"Alignment links JSON (key course+page+id)", "course", "page_id", {}, {}, {"tier"}},
        {"Reverse index JSON (key english)", "english", "", {}, {"entries"}, {}},
        // proposals.tsv writes its header as a '#' comment, so the names
        // come from the preset; they are proposals.cpp's own, in order.
        {"Proposals TSV (key id)", "id", "",
         {"id", "kind", "status", "proposer", "created", "wylie", "value", "field",
          "evidence", "approver", "ruled", "comment"},
         {}, {"proposer", "created", "evidence", "approver", "ruled"}},
        {"Build manifest JSON (key path)", "path", "", {}, {}, {"sha256"}},
        {"Folder snapshot JSON (key path)", "path", "", {}, {}, {"sha1", "size"}},
        // The registrar's number column is found by the app with
        // normalizeCatalogKey — a catalog rule, not a table rule — so no
        // key is named here and the profile never auto-detects.
        {"Registrar CSV (key = detected number column)", "", "", {}, {}, {}},
        {"Worksheet CSV (key ACIP Number)", "ACIP Number", "", {}, {}, {}},
    };
    return P;
}

namespace {

bool keyUsable(const TableProfile& p, const table::Table& t) {
    if (p.keyColumn.empty()) return false;   // a manual choice, never a detection
    if (columnIndex(t, p.keyColumn) >= 0 &&
        (p.secondKey.empty() || columnIndex(t, p.secondKey) >= 0))
        return true;
    // A headerless house TSV read WITHOUT its preset takes its first
    // data row for a header, so the key column is not there to find;
    // the implied column count is the only honest signal left.
    return !p.impliedColumns.empty() && t.columns.size() == p.impliedColumns.size();
}

}  // namespace

const TableProfile* detectProfile(const std::string& path, const table::Table& sampled) {
    const std::string lowPath = lower(path);
    std::string base = lowPath;
    const size_t slash = base.find_last_of("/\\");
    if (slash != std::string::npos) base = base.substr(slash + 1);
    auto has = [&](const char* s) { return base.find(s) != std::string::npos; };
    auto ends = [&](const std::string& s) {
        return base.size() >= s.size() &&
               base.compare(base.size() - s.size(), s.size(), s) == 0;
    };

    std::string want;
    if (has("hgm_dictionary") && ends(".tsv")) want = "Release dictionary TSV (key wylie)";
    else if (lowPath.find("/library/glossaries/") != std::string::npos)
        want = "Per-text glossary (TSV, key wylie)";
    else if (has("evidence") && ends(".json"))
        want = "Alignment evidence JSON (key headword+eng)";
    else if (has("links") && ends(".json")) want = "Alignment links JSON (key course+page+id)";
    else if (has("reverse_index")) want = "Reverse index JSON (key english)";
    else if (base == "build_manifest.json") want = "Build manifest JSON (key path)";
    else if (ends(".snapshot.json")) want = "Folder snapshot JSON (key path)";
    else if (base == "proposals.tsv") want = "Proposals TSV (key id)";
    else if (columnIndex(sampled, "ACIP Number") >= 0) want = "Worksheet CSV (key ACIP Number)";
    if (want.empty()) return nullptr;

    for (const auto& p : tableProfiles()) {
        if (p.name != want) continue;
        return keyUsable(p, sampled) ? &p : nullptr;
    }
    return nullptr;
}

}  // namespace tablediff
}  // namespace allcore
