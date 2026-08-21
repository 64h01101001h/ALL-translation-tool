#include "allcore/qc.h"

#include <algorithm>
#include <cctype>
#include <map>
#include <sstream>

#include "allcore/analysis.h"

namespace allcore {
namespace {

// section number -> body text, keyed on lines like "## 6)" (tolerates "## 6) …").
std::map<int, std::string> splitSections(const std::string& md) {
    std::map<int, std::string> out;
    std::istringstream in(md);
    std::string line;
    int cur = 0;
    while (std::getline(in, line)) {
        if (line.rfind("## ", 0) == 0) {
            size_t i = 3;
            while (i < line.size() && std::isspace((unsigned char)line[i])) ++i;
            size_t j = i;
            while (j < line.size() && std::isdigit((unsigned char)line[j])) ++j;
            if (j > i && j < line.size() && line[j] == ')') {
                cur = std::stoi(line.substr(i, j - i));
                continue;
            }
            cur = 0;  // some other heading
            continue;
        }
        if (cur) out[cur] += line + "\n";
    }
    return out;
}

// keep letters+digits only, lowercased — the "same words" comparison space
std::string normEnglish(const std::string& s) {
    std::string o;
    for (unsigned char c : s) {
        if (std::isalnum(c)) o += (char)std::tolower(c);
    }
    return o;
}

std::string stripMd(const std::string& s) {
    std::string o;
    for (char c : s)
        if (c != '*' && c != '`' && c != '_') o += c;
    // trim
    size_t b = o.find_first_not_of(" \t");
    size_t e = o.find_last_not_of(" \t");
    return b == std::string::npos ? "" : o.substr(b, e - b + 1);
}

bool acipToken(const std::string& t) {
    if (t.empty()) return false;
    bool upper = false;
    for (unsigned char c : t) {
        if (std::islower(c)) return false;
        if (std::isupper(c)) upper = true;
        else if (c != '\'' && c != '+' && c != '-' && c != '@' && c != ':' && c != ',')
            return false;
    }
    return upper;
}

std::vector<std::string> acipTokensOf(const std::string& s) {
    std::vector<std::string> toks;
    for (auto& t : tokenizeAcip(s))
        if (acipToken(t)) toks.push_back(t);
    return toks;
}

// join tokens with no separators — comparison space that forgives the
// template's legitimate morpheme splitting (SNGA DRO 'I vs SNGA DRO'I)
std::string glue(const std::vector<std::string>& toks, size_t beg, size_t end) {
    std::string o;
    for (size_t i = beg; i < end; ++i) o += toks[i];
    return o;
}

// maximal runs of CONSECUTIVE ACIP tokens in a line; English words, dashes,
// and punctuation break runs, so separate mentions never get glued together.
// Leading single-letter clause labels (A/B) are dropped.
std::vector<std::vector<std::string>> acipRunsOf(const std::string& line) {
    std::vector<std::vector<std::string>> runs;
    std::vector<std::string> cur;
    std::istringstream in(line);
    std::string t;
    while (in >> t) {
        while (!t.empty() && (t.back() == ',' || t.back() == '.' || t.back() == ':'))
            t.pop_back();
        if (t == "A" || t == "B") {           // clause labels / formula variables
            if (!cur.empty()) { runs.push_back(cur); cur.clear(); }
        } else if (acipToken(t)) {
            cur.push_back(t);
        } else if (!cur.empty()) {
            runs.push_back(cur);
            cur.clear();
        }
    }
    if (!cur.empty()) runs.push_back(cur);
    return runs;
}

// "SNGA DRO'I KA BA = "the morning pillar's"" -> {acip, english}
struct SegLine { std::string acip, english; };
std::vector<SegLine> parseSegmentLines(const std::string& sec) {
    std::vector<SegLine> out;
    std::istringstream in(sec);
    std::string line;
    while (std::getline(in, line)) {
        auto eq = line.find(" = ");
        if (eq == std::string::npos) continue;
        std::string left = stripMd(line.substr(0, eq));
        if (acipTokensOf(left).empty()) continue;
        std::string right = line.substr(eq + 3);
        auto q1 = right.find_first_of("\"“");
        auto q2 = right.find_last_of("\"”");
        if (q1 != std::string::npos && q2 != std::string::npos && q2 > q1)
            right = right.substr(q1 + 1, q2 - q1 - 1);
        out.push_back({left, right});
    }
    return out;
}

// base translation X out of section 5: text of the "> ..." blockquote after "X)"
std::string baseTranslation(const std::string& sec5, char which) {
    std::istringstream in(sec5);
    std::string line, out;
    bool inTarget = false, collecting = false;
    const std::string tagA = "A)", tagB = "B)";
    const std::string& tag = which == 'A' ? tagA : tagB;
    const std::string& other = which == 'A' ? tagB : tagA;
    while (std::getline(in, line)) {
        std::string plain = stripMd(line);
        if (plain.rfind(tag, 0) == 0) { inTarget = true; continue; }
        if (plain.rfind(other, 0) == 0) inTarget = false;
        if (inTarget && line.rfind(">", 0) == 0) {
            collecting = true;
            out += line.substr(1) + " ";
        } else if (collecting && line.rfind(">", 0) != 0) {
            break;
        }
    }
    return out;
}

void checkReconstruction(std::vector<QcFinding>& out, const std::string& label,
                         const std::string& check, const std::string& sec,
                         const std::string& base,
                         const std::vector<std::string>& srcToks) {
    auto segs = parseSegmentLines(sec);
    if (segs.empty()) {
        out.push_back({check, "warn", "item " + label + ": no ACIP = \"english\" "
                                      "segment lines found (format drift?)"});
        return;
    }
    if (base.empty()) {
        out.push_back({check, "warn",
                       "item 5: Base Translation " + label +
                           " blockquote not found — cannot verify reconstruction"});
        return;
    }
    std::string concat;
    std::vector<std::string> acipConcat;
    for (const auto& s : segs) {
        concat += s.english + " ";
        for (auto& t : acipTokensOf(s.acip)) acipConcat.push_back(t);
    }
    const std::string nc = normEnglish(concat), nb = normEnglish(base);
    if (nc == nb) {
        out.push_back({check, "info",
                       "item " + label + ": segments reconstruct Base Translation " +
                           label + " exactly (" + std::to_string(segs.size()) +
                           " segments)"});
    } else {
        // word-level diff so the reader sees WHERE the wording diverged
        auto words = [](const std::string& s) {
            std::vector<std::string> w;
            std::string cur;
            for (unsigned char c : s) {
                if (std::isalnum(c)) cur += (char)std::tolower(c);
                else if (!cur.empty()) { w.push_back(cur); cur.clear(); }
            }
            if (!cur.empty()) w.push_back(cur);
            return w;
        };
        auto ws = words(concat), wb = words(base);
        size_t i = 0;
        while (i < ws.size() && i < wb.size() && ws[i] == wb[i]) ++i;
        auto excerpt = [&](const std::vector<std::string>& w) {
            std::string o;
            for (size_t j = i; j < w.size() && j < i + 5; ++j)
                o += (j > i ? " " : "") + w[j];
            return o.empty() ? std::string("(end)") : o;
        };
        out.push_back({check, "error",
                       "item " + label + ": segments do NOT reconstruct Base "
                       "Translation " + label + " — first divergence: segments say \"" +
                           excerpt(ws) + "…\" but the base says \"" + excerpt(wb) +
                           "…\" (added/dropped/altered wording)"});
    }
    // Translation B may legitimately reorder segments (read-order note), so
    // coverage is order-insensitive there; A must match in order.
    bool covered;
    if (label == "B") {
        auto a = acipConcat, b = srcToks;
        std::sort(a.begin(), a.end());
        std::sort(b.begin(), b.end());
        covered = (a == b);
    } else {
        covered = (acipConcat == srcToks);
    }
    if (!covered) {
        std::string msg = "item " + label + ": ACIP segments do not cover the "
                          "source passage (";
        msg += std::to_string(acipConcat.size()) + " vs " +
               std::to_string(srcToks.size()) + " tokens)";
        out.push_back({"acip-coverage", "warn", msg});
    } else {
        out.push_back({"acip-coverage", "info",
                       "item " + label + ": ACIP segments cover the source" +
                           (label == "B" ? " (order-insensitive)" : " exactly")});
    }
}

std::vector<std::string> splitCells(const std::string& row) {
    std::vector<std::string> cells;
    std::string cur;
    for (size_t i = 1; i < row.size(); ++i) {   // skip leading '|'
        if (row[i] == '|') { cells.push_back(cur); cur.clear(); }
        else cur += row[i];
    }
    return cells;
}

bool separatorRow(const std::string& s) {
    for (char c : s)
        if (c != '|' && c != '-' && c != ':' && c != ' ') return false;
    return true;
}

std::string lower(std::string s) {
    for (auto& c : s) c = (char)std::tolower((unsigned char)c);
    return s;
}

}  // namespace

std::vector<QcFinding> validateReport(const Spine& spine,
                                      const std::string& report_markdown,
                                      const std::string& source_acip) {
    std::vector<QcFinding> out;
    auto secs = splitSections(report_markdown);
    if (secs.empty()) {
        out.push_back({"parse", "error",
                       "no '## N)' sections found — report format unrecognized"});
        return out;
    }
    const auto srcToks = acipTokensOf(source_acip);

    // ---- items 6/7 reconstruction ----
    checkReconstruction(out, "A", "reconstruction-a", secs[6],
                        baseTranslation(secs[5], 'A'), srcToks);
    checkReconstruction(out, "B", "reconstruction-b", secs[7],
                        baseTranslation(secs[5], 'B'), srcToks);

    // ---- item 8: HGM anchors ----
    {
        std::istringstream in(secs[8]);
        std::string line;
        int rows = 0, verified = 0;
        while (std::getline(in, line)) {
            if (line.empty() || line[0] != '|' || separatorRow(line)) continue;
            auto cells = splitCells(line);
            if (cells.size() < 3) continue;
            std::string term = stripMd(cells[0]);
            if (lower(term).find("acip") != std::string::npos) continue;  // header
            // strip trailing annotations after the ACIP run
            auto toks = acipTokensOf(term);
            if (toks.empty()) continue;
            std::string key;
            for (size_t i = 0; i < toks.size(); ++i) {
                if (i) key += ' ';
                key += toks[i];
            }
            ++rows;
            auto hits = spine.lookup(key);
            if (hits.empty()) {
                out.push_back({"hgm-anchor", "info",
                               "item 8: '" + key + "' not found in the spine — "
                               "HGM default cannot be verified"});
                continue;
            }
            const Entry& e = hits.front();
            const std::string cell = lower(cells.size() > 1 ? cells[1] : "");
            const std::string chosen = lower(cells.size() > 2 ? cells[2] : "");
            const std::string notes = lower(cells.size() > 3 ? cells[3] : "");
            if (e.hgm_gloss.empty()) {
                if (cell.find("no hgm") == std::string::npos &&
                    cell.find("—") == std::string::npos && cell.find("-") == std::string::npos)
                    out.push_back({"hgm-anchor", "warn",
                                   "item 8: '" + key + "' has NO HGM equivalent in the "
                                   "spine, but the table shows a default"});
                continue;
            }
            bool quoted = false, chosenIsGloss = false;
            for (const auto& g : e.hgm_gloss) {
                if (cell.find(lower(g)) != std::string::npos) quoted = true;
                if (!chosen.empty() && chosen.find(lower(g)) != std::string::npos)
                    chosenIsGloss = true;
            }
            if (!quoted) {
                std::string have;
                for (size_t i = 0; i < e.hgm_gloss.size() && i < 4; ++i)
                    have += (i ? "; " : "") + e.hgm_gloss[i];
                out.push_back({"hgm-anchor", "error",
                               "item 8: '" + key + "' HGM default cell does not quote "
                               "the spine's gloss (spine has: " + have + ")"});
            } else {
                ++verified;
            }
            if (!chosenIsGloss && notes.find("deviation") == std::string::npos)
                out.push_back({"deviation-mark", "warn",
                               "item 8: '" + key + "' chosen English is not an HGM "
                               "gloss and the notes carry no DEVIATION mark"});
        }
        out.push_back({"hgm-anchor", "info",
                       "item 8: " + std::to_string(verified) + "/" +
                           std::to_string(rows) + " rows verified against the spine"});
    }

    // ---- ACIP integrity over structural sections ----
    // character-level containment (spaces removed) forgives morpheme splitting;
    // consecutive-run extraction prevents gluing separate mentions.
    const std::string srcGlued = glue(srcToks, 0, srcToks.size());
    for (int n : {2, 6, 7, 9, 10}) {
        std::istringstream in(secs[n]);
        std::string line;
        while (std::getline(in, line)) {
            auto eq = line.find(" = ");
            std::string scan = stripMd(eq == std::string::npos ? line : line.substr(0, eq));
            for (const auto& toks : acipRunsOf(scan)) {
                if (toks.size() < 2) continue;
                // only meaningful if it overlaps the source vocabulary
                int inSrc = 0;
                for (auto& t : toks)
                    if (std::find(srcToks.begin(), srcToks.end(), t) != srcToks.end())
                        ++inSrc;
                if (inSrc * 2 < (int)toks.size()) continue;
                if (srcGlued.find(glue(toks, 0, toks.size())) == std::string::npos) {
                    std::string run;
                    for (size_t i = 0; i < toks.size(); ++i)
                        run += (i ? " " : "") + toks[i];
                    out.push_back({"acip-integrity", "error",
                                   "item " + std::to_string(n) + ": quoted ACIP not in "
                                   "the source passage (silent fix?): " + run});
                }
            }
        }
    }
    return out;
}

std::string findingsToMarkdown(const std::vector<QcFinding>& findings) {
    int err = 0, warn = 0;
    for (const auto& f : findings) {
        if (f.severity == "error") ++err;
        else if (f.severity == "warn") ++warn;
    }
    std::string md = "## Machine QC — " + std::to_string(err) + " error(s), " +
                     std::to_string(warn) + " warning(s)\n";
    for (const auto& f : findings) {
        std::string icon = f.severity == "error" ? "❌" :
                           f.severity == "warn" ? "⚠️" : "✓";
        md += "- " + icon + " [" + f.check + "] " + f.message + "\n";
    }
    return md;
}

}  // namespace allcore
