// docx_redline_smoke — the tracked-changes .docx writer battery
// (analysis-suite F6). Fixture-free: the texts, the XML scanner and the
// Accept-All / Reject-All reader all live here, so the test proves the
// document by reading it back rather than by trusting the writer.
#include <cstdio>
#include <cstdlib>
#include <set>
#include <string>
#include <vector>

#include "allcore/docx_redline.h"
#include "allcore/textdiff.h"
#include "allcore/zipstore.h"

#if defined(__APPLE__)
#include <fstream>
#include <iterator>
#endif

using namespace allcore;
using allcore::docx::RedlineOptions;
using allcore::docx::RedlineStats;

static int failures = 0;
#define CHECK(cond, msg) do { if (cond) std::printf("  [PASS] %s\n", msg); else { std::printf("  [FAIL] %s\n", msg); ++failures; } } while (0)

static bool has(const std::string& hay, const std::string& needle) { return hay.find(needle) != std::string::npos; }
static int count(const std::string& hay, const std::string& needle) {
    int n = 0;
    for (std::size_t p = hay.find(needle); p != std::string::npos; p = hay.find(needle, p + needle.size())) ++n;
    return n;
}

// ------------------------------------------------------------- a tag walk
struct Tag { std::string name; bool closing = false, selfClosing = false; std::size_t begin = 0, end = 0; };
// Returns false when the XML is malformed enough to stop reading.
static bool nextTag(const std::string& s, std::size_t from, Tag* t) {
    const std::size_t lt = s.find('<', from);
    if (lt == std::string::npos) return false;
    const std::size_t gt = s.find('>', lt);
    if (gt == std::string::npos) return false;
    std::string body = s.substr(lt + 1, gt - lt - 1);
    t->begin = lt; t->end = gt;
    t->closing = !body.empty() && body[0] == '/';
    t->selfClosing = !body.empty() && body.back() == '/';
    std::size_t i = t->closing ? 1 : 0;
    std::size_t j = i;
    while (j < body.size() && body[j] != ' ' && body[j] != '/' && body[j] != '\t') ++j;
    t->name = body.substr(i, j - i);
    return true;
}

// Every w: element opens and closes in order. A stack, not a regex.
static bool tagsBalanced(const std::string& xml) {
    std::vector<std::string> stack;
    Tag t;
    for (std::size_t at = 0; nextTag(xml, at, &t); at = t.end + 1) {
        if (!t.name.empty() && (t.name[0] == '?' || t.name[0] == '!')) continue;
        if (t.name.compare(0, 2, "w:") != 0) continue;
        if (t.selfClosing) continue;
        if (t.closing) {
            if (stack.empty() || stack.back() != t.name) return false;
            stack.pop_back();
        } else {
            stack.push_back(t.name);
        }
    }
    return stack.empty();
}

static std::string unescape(const std::string& s) {
    std::string o;
    for (std::size_t i = 0; i < s.size();) {
        if (s[i] == '&') {
            if (s.compare(i, 5, "&amp;") == 0) { o += '&'; i += 5; continue; }
            if (s.compare(i, 4, "&lt;") == 0) { o += '<'; i += 4; continue; }
            if (s.compare(i, 4, "&gt;") == 0) { o += '>'; i += 4; continue; }
            if (s.compare(i, 6, "&quot;") == 0) { o += '"'; i += 6; continue; }
        }
        o += s[i++];
    }
    return o;
}

// ------------------------------------ what Word would show after Accept /
// Reject All. accept keeps w:t and drops w:delText; reject keeps w:t
// outside w:ins plus w:delText. An inserted paragraph mark disappears on
// reject (the paragraph merges with the next), a deleted one on accept.
struct Para { int mark = 0; std::string accept, reject; };

static std::vector<Para> paragraphs(const std::string& doc) {
    std::vector<Para> out;
    for (std::size_t p = doc.find("<w:p>"); p != std::string::npos; p = doc.find("<w:p>", p + 5)) {
        const std::size_t e = doc.find("</w:p>", p);
        if (e == std::string::npos) break;
        std::string inner = doc.substr(p + 5, e - p - 5);
        Para para;
        if (inner.compare(0, 7, "<w:pPr>") == 0) {
            const std::size_t pe = inner.find("</w:pPr>");
            if (pe != std::string::npos) {
                const std::string pPr = inner.substr(0, pe);
                if (has(pPr, "<w:ins ")) para.mark = 1;
                else if (has(pPr, "<w:del ")) para.mark = 2;
                inner = inner.substr(pe + 8);
            }
        }
        int insDepth = 0, delDepth = 0;
        Tag t;
        std::size_t at = 0;
        while (nextTag(inner, at, &t)) {
            at = t.end + 1;
            if (t.name == "w:ins" && !t.selfClosing) insDepth += t.closing ? -1 : 1;
            if (t.name == "w:del" && !t.selfClosing) delDepth += t.closing ? -1 : 1;
            if ((t.name == "w:t" || t.name == "w:delText") && !t.closing && !t.selfClosing) {
                const std::string close = "</" + t.name + ">";
                const std::size_t ce = inner.find(close, t.end + 1);
                if (ce == std::string::npos) break;
                const std::string text = unescape(inner.substr(t.end + 1, ce - t.end - 1));
                if (t.name == "w:t") {
                    if (delDepth == 0) para.accept += text;
                    if (insDepth == 0) para.reject += text;
                } else {
                    para.reject += text;
                }
                at = ce + close.size();
            }
        }
        out.push_back(para);
    }
    return out;
}

static std::vector<std::string> rebuild(const std::string& doc, bool accept) {
    std::vector<std::string> lines;
    std::string cur;
    bool pending = false;
    for (const Para& p : paragraphs(doc)) {
        cur += accept ? p.accept : p.reject;
        pending = true;
        const bool merge = accept ? (p.mark == 2) : (p.mark == 1);
        if (!merge) { lines.push_back(cur); cur.clear(); pending = false; }
    }
    // Word never removes the document's LAST paragraph mark, so a pending
    // merge simply ends the document.
    if (pending) lines.push_back(cur);
    if (!lines.empty()) lines.erase(lines.begin());   // the provenance note
    // A revision on the last paragraph's mark leaves one empty paragraph
    // behind in Word — the trailing-newline of a text file. Documented.
    if (!lines.empty() && lines.back().empty()) lines.pop_back();
    return lines;
}

static std::vector<int> trackIds(const std::string& doc) {
    std::vector<int> ids;
    Tag t;
    for (std::size_t at = 0; nextTag(doc, at, &t); at = t.end + 1) {
        if (t.closing || (t.name != "w:ins" && t.name != "w:del")) continue;
        const std::string tag = doc.substr(t.begin, t.end - t.begin + 1);
        const std::size_t k = tag.find("w:id=\"");
        if (k == std::string::npos) { ids.push_back(-1); continue; }
        ids.push_back(std::atoi(tag.c_str() + k + 6));
        if (!has(tag, "w:author=\"") || !has(tag, "w:date=\"")) ids.push_back(-1);
    }
    return ids;
}

static RedlineOptions baseOptions() {
    RedlineOptions o;
    o.author = "Adam via Diamond Cutter compare";
    o.dateIso = "2026-09-08T12:00:00Z";
    o.aName = "draft3.act";
    o.bName = "draft4.act";
    o.title = "draft3.act vs draft4.act";
    return o;
}

int main() {
    setvbuf(stdout, nullptr, _IONBF, 0);
    const RedlineOptions o = baseOptions();

    // The plan's fixture, with one line added. As the plan writes it
    // (a = 3 lines, b = those 3 with "BDE BA" appended), diffLines merges
    // the whole tail into ONE Change hunk of 1 left line against 2 right
    // lines — the unequal case, which the plan's own algorithm says must be
    // marked as whole lines. So the plan's fixture cannot show the in-line
    // refinement the plan's test 5 asserts. A trailing shared line
    // ("RDZOGS SO") separates the change from the insertion and the hunks
    // come out 1:1 Change + Insert, which is what test 5 describes. The
    // plan's literal fixture is exercised below, on the path it actually
    // takes.
    const std::vector<std::string> a = {"@001A", "BLA MA LA", "SEMS CAN THAMS CAD", "RDZOGS SO"};
    const std::vector<std::string> b = {"@001A", "BLA MA LA", "SEMS CAN KUN", "RDZOGS SO", "BDE BA"};
    const textdiff::Result r = textdiff::diffLines(a, b, textdiff::Options());
    RedlineStats st;
    const std::string doc = docx::documentXml(a, b, r, o, &st);

    // 1 — the marks themselves.
    {
        const std::size_t d = doc.find("<w:delText xml:space=\"preserve\">THAMS CAD</w:delText>");
        bool inDel = false;
        if (d != std::string::npos) {
            const std::size_t open = doc.rfind("<w:del ", d);
            const std::size_t close = doc.rfind("</w:del>", d);
            inDel = open != std::string::npos && (close == std::string::npos || close < open) &&
                    has(doc.substr(open, d - open), "w:author=\"Adam via Diamond Cutter compare\"") &&
                    has(doc.substr(open, d - open), "w:date=\"2026-09-08T12:00:00Z\"");
        }
        const std::size_t i = doc.find("<w:t xml:space=\"preserve\">KUN</w:t>");
        bool inIns = false;
        if (i != std::string::npos) {
            const std::size_t open = doc.rfind("<w:ins ", i);
            const std::size_t close = doc.rfind("</w:ins>", i);
            inIns = open != std::string::npos && (close == std::string::npos || close < open);
        }
        // "BDE BA" is a new last line: its own paragraph mark is inserted.
        const std::size_t bde = doc.find("BDE BA");
        const std::size_t pOpen = bde == std::string::npos ? std::string::npos : doc.rfind("<w:p>", bde);
        const bool markIns = pOpen != std::string::npos &&
                             has(doc.substr(pOpen, bde - pOpen), "<w:pPr><w:rPr><w:ins ");
        const bool plainBla = count(doc, "BLA MA LA") == 1 &&
                              has(doc, "<w:r><w:t xml:space=\"preserve\">BLA MA LA</w:t></w:r>");
        CHECK(inDel && inIns && markIns && plainBla && st.insertions == 2 && st.deletions == 1,
              "1 THAMS CAD deleted and KUN inserted in place, BDE BA's paragraph mark inserted, BLA MA LA plain; 2 insertions / 1 deletion");

        // The plan's literal fixture: one Change hunk, 1 left line against
        // 2 right lines. Whole lines out, whole lines in — no inline spans,
        // which is what the note promises for a mismatched pair.
        const std::vector<std::string> pa = {"@001A", "BLA MA LA", "SEMS CAN THAMS CAD"};
        const std::vector<std::string> pb = {"@001A", "BLA MA LA", "SEMS CAN KUN", "BDE BA"};
        const textdiff::Result pr = textdiff::diffLines(pa, pb, textdiff::Options());
        RedlineStats pst;
        const std::string pd = docx::documentXml(pa, pb, pr, o, &pst);
        CHECK(pr.hunks.size() == 2 && pr.hunks[1].kind == textdiff::Kind::Change &&
                  pr.hunks[1].aEnd - pr.hunks[1].aBeg == 1 && pr.hunks[1].bEnd - pr.hunks[1].bBeg == 2 &&
                  pst.deletions == 1 && pst.insertions == 2 &&
                  has(pd, "<w:delText xml:space=\"preserve\">SEMS CAN THAMS CAD</w:delText>") &&
                  !has(pd, ">THAMS CAD<") && rebuild(pd, true) == pb && rebuild(pd, false) == pa,
              "1b the plan's own 3-vs-4 fixture is a 1:2 Change, so whole lines are marked and Accept/Reject still round-trip");
    }

    // 2 — every revision is attributable, and the ids are unique from 1.
    {
        const auto ids = trackIds(doc);
        std::set<int> uniq(ids.begin(), ids.end());
        bool ok = !ids.empty() && uniq.size() == ids.size() && uniq.count(-1) == 0 && *uniq.begin() == 1;
        ok = ok && count(doc, "w:author=\"") == count(doc, "<w:ins ") + count(doc, "<w:del ");
        ok = ok && count(doc, "w:date=\"") == count(doc, "<w:ins ") + count(doc, "<w:del ");
        CHECK(ok, "2 every w:ins / w:del carries w:author and w:date; w:id values are unique and start at 1");
    }

    // 3 — structural well-formedness.
    CHECK(tagsBalanced(doc) && count(doc, "<w:p>") + count(doc, "<w:p ") == count(doc, "</w:p>") &&
              count(doc, "<w:p>") == 6 && has(doc, "<w:sectPr>") &&
              doc.find("<w:sectPr>") > doc.rfind("</w:p>"),
          "3 all w: tags balance; w:p opens match closes (6 paragraphs); w:sectPr closes the body");

    // 4 — escaping, and control characters counted rather than dropped.
    {
        bool ok = docx::xmlEscape("a<b&c>\"d\"") == "a&lt;b&amp;c&gt;&quot;d&quot;";
        const std::vector<std::string> ea = {"plain & simple"}, eb = {"plain & simple <x>"};
        RedlineStats est;
        const std::string ed = docx::documentXml(ea, eb, textdiff::diffLines(ea, eb, textdiff::Options()), o, &est);
        ok = ok && has(ed, "&amp;") && !has(ed, " & ") && est.unrepresentable == 0;
        ok = ok && rebuild(ed, true) == eb && rebuild(ed, false) == ea;

        const std::vector<std::string> ca = {std::string("bad\x01char")}, cb = {"good char"};
        RedlineStats cst;
        const std::string cd = docx::documentXml(ca, cb, textdiff::diffLines(ca, cb, textdiff::Options()), o, &cst);
        ok = ok && cst.unrepresentable == 1 && has(cd, "\xEF\xBF\xBD") &&
             has(cd.substr(0, cd.find("</w:p>")), "1 control character could not be represented");
        CHECK(ok, "4 xmlEscape covers & < > \"; a literal & round-trips; U+0001 becomes U+FFFD, is counted, and the note says so");
    }

    // 5 — an unequal Change: whole lines out, whole lines in.
    {
        const std::vector<std::string> ua = {"HEAD", "p", "q", "TAIL"};
        const std::vector<std::string> ub = {"HEAD", "r", "s", "t", "TAIL"};
        const textdiff::Result ur = textdiff::diffLines(ua, ub, textdiff::Options());
        RedlineStats ust;
        const std::string ud = docx::documentXml(ua, ub, ur, o, &ust);
        const auto ps = paragraphs(ud);
        // note, HEAD, p, q, r, s, t, TAIL
        bool ok = ps.size() == 8 && ps[2].mark == 2 && ps[3].mark == 2 &&
                  ps[4].mark == 1 && ps[5].mark == 1 && ps[6].mark == 1 &&
                  ust.deletions == 2 && ust.insertions == 3;
        // no inline refinement: each of those paragraphs is one whole line
        ok = ok && ps[2].reject == "p" && ps[2].accept.empty() && ps[4].accept == "r" && ps[4].reject.empty();
        ok = ok && rebuild(ud, true) == ub && rebuild(ud, false) == ua;
        CHECK(ok, "5 a 2-vs-3 Change writes 2 deleted then 3 inserted whole-line paragraphs, no inline spans");
    }

    // 6 — the minor-difference policy: one policy, not a checkbox.
    {
        const std::vector<std::string> ma = {"KA  KHA"}, mb = {"KA KHA"};
        textdiff::Options mo; mo.ignoreWhitespaceChange = true;
        const textdiff::Result mr = textdiff::diffLines(ma, mb, mo);
        RedlineStats mst;
        const std::string md = docx::documentXml(ma, mb, mr, o, &mst);
        const std::string mnote = md.substr(0, md.find("</w:p>"));
        CHECK(mr.unimportant == 1 && mst.minorMarked == 1 && count(md, "<w:ins ") + count(md, "<w:del ") > 0 &&
                  has(mnote, "1 minor difference is included as a revision") &&
                  has(mnote, "nothing is quietly omitted") &&
                  rebuild(md, true) == mb && rebuild(md, false) == ma,
              "6 a minor hunk is marked like any other, counted, and the note says so");
    }

    // 7 — raw-compared lines and moved blocks are reported, not hidden.
    {
        textdiff::Options so; so.scriptAgnostic = true;
        const std::string bad = "\u0F40\u0FDB\u0F71";   // the converter cannot map this
        const std::vector<std::string> ra = {"ka", bad, "ga"}, rb = {"ka", bad + "x", "ga"};
        const textdiff::Result rr = textdiff::diffLines(ra, rb, so);
        RedlineStats rst;
        const std::string rd = docx::documentXml(ra, rb, rr, o, &rst);
        const std::string rnote = rd.substr(0, rd.find("</w:p>"));
        bool ok = rr.unnormalised >= 1 && rst.comparedRaw == rr.unnormalised && has(rnote, "compared raw");

        const std::vector<std::string> va = {"one", "two", "three", "four"};
        const std::vector<std::string> vb = {"three", "four", "one", "two"};
        const textdiff::Result vr = textdiff::diffLines(va, vb, textdiff::Options());
        RedlineStats vst;
        const std::string vd = docx::documentXml(va, vb, vr, o, &vst);
        const std::string vnote = vd.substr(0, vd.find("</w:p>"));
        ok = ok && vr.moved == 1 && vst.moved == 1 &&
             has(vnote, "Moved blocks appear as a deletion and an insertion (1 block here)") &&
             rebuild(vd, true) == vb && rebuild(vd, false) == va;
        CHECK(ok, "7 unconvertible lines are counted as compared raw and moved blocks are declared as delete + insert");
    }

    // 8 — the complex-script slot, and only where it belongs.
    {
        const std::vector<std::string> ta = {"BKRA SHIS", "བཀྲ"};
        const std::vector<std::string> tb = {"BKRA SHIS", "བཀྲ་"};
        RedlineStats tst;
        const std::string td = docx::documentXml(ta, tb, textdiff::diffLines(ta, tb, textdiff::Options()), o, &tst);
        const std::size_t tib = td.find("བཀྲ");
        const std::size_t acip = td.find(">BKRA SHIS<");
        const std::size_t tibRun = tib == std::string::npos ? std::string::npos : td.rfind("<w:r>", tib);
        const std::size_t acipRun = acip == std::string::npos ? std::string::npos : td.rfind("<w:r>", acip);
        const bool tibHas = tibRun != std::string::npos &&
                            has(td.substr(tibRun, tib - tibRun), "<w:rPr><w:rFonts w:cs=\"Noto Serif Tibetan\"/><w:cs/></w:rPr>");
        const bool acipPlain = acipRun != std::string::npos && !has(td.substr(acipRun, acip - acipRun), "w:rFonts");
        CHECK(tibHas && acipPlain, "8 a Tibetan run carries w:rFonts w:cs and <w:cs/>; an ACIP run carries no font override");
    }

    // 9 — the note comes first and is not itself a revision.
    {
        const std::size_t first = doc.find("<w:p>");
        const std::size_t firstEnd = doc.find("</w:p>", first);
        const std::string p1 = doc.substr(first, firstEnd - first);
        CHECK(first != std::string::npos && has(p1, "draft3.act") && has(p1, "draft4.act") &&
                  has(p1, "Adam via Diamond Cutter compare") && has(p1, "machine-computed") &&
                  has(p1, "not an editor's edits") && has(p1, "Noto Serif Tibetan") &&
                  has(p1, "compared as plain text") &&
                  !has(p1, "<w:ins") && !has(p1, "<w:del"),
              "9 the first paragraph is the untracked provenance note naming A, B, the author, the font and the honesty line");
    }

    // 10 — the supporting parts.
    {
        const std::string ct = docx::contentTypesXml(), rl = docx::relsXml(), drl = docx::documentRelsXml();
        const std::string sx = docx::stylesXml(o), sg = docx::settingsXml(), cx = docx::coreXml(o), ax = docx::appXml();
        CHECK(has(ct, "PartName=\"/word/document.xml\"") && has(ct, "PartName=\"/word/styles.xml\"") &&
                  has(ct, "PartName=\"/word/settings.xml\"") && has(ct, "Extension=\"rels\"") &&
                  has(rl, "Target=\"word/document.xml\"") && has(rl, "Target=\"docProps/core.xml\"") &&
                  has(drl, "Target=\"styles.xml\"") && has(drl, "Target=\"settings.xml\"") &&
                  has(sg, "<w:trackRevisions/>") && has(sx, "w:styleId=\"Normal\"") &&
                  has(sx, "Times New Roman") && has(sx, "w:styleId=\"Tibetan\"") &&
                  has(cx, "<dc:creator>Adam via Diamond Cutter compare</dc:creator>") &&
                  has(ax, "<Application>Diamond Cutter Translation Tool</Application>") &&
                  tagsBalanced(sx) && tagsBalanced(sg),
              "10 content types, both rels, styles, settings (<w:trackRevisions/>), core (dc:creator = author) and app are as specified");
    }

    // 11 — the package.
    {
        RedlineStats pst;
        const std::string pkg = docx::buildRedlineDocx(a, b, r, o, &pst);
        const auto entries = zipstore::listStored(pkg);
        const std::vector<std::string> want = {
            "[Content_Types].xml", "_rels/.rels", "word/_rels/document.xml.rels", "word/document.xml",
            "word/styles.xml", "word/settings.xml", "docProps/core.xml", "docProps/app.xml"};
        std::vector<std::string> got;
        for (const auto& e : entries) got.push_back(e.name);
        bool ok = got == want && pkg.compare(0, 2, "PK") == 0 && pst.insertions == st.insertions;
        for (const auto& e : entries)
            ok = ok && zipstore::crc32(pkg.substr(e.dataOffset, e.size)) == e.crc;
        ok = ok && docx::buildRedlineDocx(a, b, r, o, nullptr) == pkg;
        CHECK(ok, "11 buildRedlineDocx writes exactly the eight parts, each CRC checks, and two builds are byte-identical");
    }

    // 12 — the whole point: Accept All is text B, Reject All is text A.
    CHECK(rebuild(doc, true) == b && rebuild(doc, false) == a,
          "12 Accept All reconstructs text B and Reject All reconstructs text A");

    // 13 — a second reader, where one exists.
#if defined(__APPLE__)
    {
        const std::string path = "/tmp/allcore_docx_redline_smoke.docx";
        const std::string out = "/tmp/allcore_docx_redline_smoke.txt";
        {
            std::ofstream f(path, std::ios::binary);
            const std::string pkg = docx::buildRedlineDocx(a, b, r, o, nullptr);
            f.write(pkg.data(), (std::streamsize)pkg.size());
        }
        const int rc = std::system(("/usr/bin/textutil -convert txt -output '" + out + "' '" + path + "' >/dev/null 2>&1").c_str());
        if (rc != 0) {
            std::printf("  [SKIP] 13 textutil could not read the package (exit %d) — the Word gate stands\n", rc);
        } else {
            std::ifstream f(out, std::ios::binary);
            const std::string txt((std::istreambuf_iterator<char>(f)), std::istreambuf_iterator<char>());
            CHECK(has(txt, "BLA MA LA") && has(txt, "BDE BA"), "13 macOS textutil reads the package back and finds both texts");
        }
        std::remove(path.c_str());
        std::remove(out.c_str());
    }
#else
    std::printf("  [SKIP] 13 textutil second-reader check is macOS only\n");
#endif

    std::printf("docx_redline_smoke: %d failure(s)\n", failures);
    return failures ? 1 : 0;
}
