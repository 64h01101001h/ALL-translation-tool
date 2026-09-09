#include "allcore/docx_redline.h"

#include <sstream>
#include <string>
#include <vector>

#include "allcore/zipstore.h"

namespace allcore {
namespace docx {

using textdiff::Hunk;
using textdiff::Kind;
using textdiff::Result;

static const char* kWns = "http://schemas.openxmlformats.org/wordprocessingml/2006/main";

// ------------------------------------------------------------- escaping
// A byte-wise pass is safe for UTF-8: every byte of a multi-byte sequence
// is >= 0x80, so only ASCII can match the specials or the control range.
static std::string escapeCounting(const std::string& s, int* unrep) {
    std::string o;
    o.reserve(s.size() + 8);
    for (unsigned char c : s) {
        switch (c) {
            case '&': o += "&amp;"; break;
            case '<': o += "&lt;"; break;
            case '>': o += "&gt;"; break;
            case '"': o += "&quot;"; break;
            default:
                if (c < 0x20 && c != '\t') {
                    o += "\xEF\xBF\xBD";   // U+FFFD — never dropped silently
                    if (unrep) ++*unrep;
                } else {
                    o += (char)c;
                }
        }
    }
    return o;
}

std::string xmlEscape(const std::string& s) { return escapeCounting(s, nullptr); }

// --------------------------------------------------------------- Tibetan
// U+0F00–U+0FFF is exactly 0xE0 followed by 0xBC–0xBF in UTF-8, and 0xE0
// can only ever be a lead byte, so this is an exact test, not a guess.
static bool hasTibetan(const std::string& s) {
    for (std::size_t i = 0; i + 2 < s.size(); ++i) {
        if ((unsigned char)s[i] == 0xE0) {
            const unsigned char c1 = (unsigned char)s[i + 1];
            if (c1 >= 0xBC && c1 <= 0xBF) return true;
        }
    }
    return false;
}

// -------------------------------------------------------------- the date
// Word wants a zoned ISO stamp on every revision.
static std::string wordDate(const std::string& iso) {
    if (iso.empty()) return "1980-01-01T00:00:00Z";
    if (iso.find('T') == std::string::npos) return iso + "T00:00:00Z";
    if (iso.back() == 'Z' || iso.find('+', 11) != std::string::npos) return iso;
    return iso + "Z";
}
static std::string dayOf(const std::string& iso) {
    const std::size_t t = iso.find('T');
    return t == std::string::npos ? iso : iso.substr(0, t);
}

// ----------------------------------------------------------- the writer
namespace {

struct Writer {
    const RedlineOptions& o;
    RedlineStats& st;
    std::string author, date;
    int nextId = 1;

    Writer(const RedlineOptions& opt, RedlineStats& s)
        : o(opt), st(s), author(xmlEscape(opt.author)), date(xmlEscape(wordDate(opt.dateIso))) {}

    // w:id must be unique and increasing across the whole document —
    // paragraph marks and content runs draw from the one counter.
    std::string trackAttrs() {
        std::ostringstream a;
        a << " w:id=\"" << nextId++ << "\" w:author=\"" << author << "\" w:date=\"" << date << "\"";
        return a.str();
    }

    // The complex-script slot is what Word consults for Tibetan shaping;
    // ASCII (ACIP/Wylie) runs get no override at all.
    std::string runProps(const std::string& text) const {
        if (!hasTibetan(text)) return std::string();
        return "<w:rPr><w:rFonts w:cs=\"" + xmlEscape(o.tibetanFont) + "\"/><w:cs/></w:rPr>";
    }

    std::string run(const std::string& text, const char* textTag) {
        std::ostringstream r;
        r << "<w:r>" << runProps(text) << "<" << textTag << " xml:space=\"preserve\">"
          << escapeCounting(text, &st.unrepresentable) << "</" << textTag << ">"
          << "</w:r>";
        return r.str();
    }

    std::string insRun(const std::string& text) {
        ++st.insertions;
        return "<w:ins" + trackAttrs() + ">" + run(text, "w:t") + "</w:ins>";
    }
    std::string delRun(const std::string& text) {
        ++st.deletions;
        return "<w:del" + trackAttrs() + ">" + run(text, "w:delText") + "</w:del>";
    }

    // mark: 0 unchanged, 1 inserted paragraph mark, 2 deleted paragraph
    // mark (ECMA-376 §17.13.5.15/.18) — so Accept All and Reject All
    // merge the paragraphs the way the line-level edit meant.
    std::string paragraph(int mark, const std::string& content) {
        std::ostringstream p;
        p << "<w:p>";
        if (mark == 1) p << "<w:pPr><w:rPr><w:ins" << trackAttrs() << "/></w:rPr></w:pPr>";
        else if (mark == 2) p << "<w:pPr><w:rPr><w:del" << trackAttrs() << "/></w:rPr></w:pPr>";
        p << content << "</w:p>";
        return p.str();
    }

    std::string plainLine(const std::string& line) {
        return paragraph(0, line.empty() ? std::string() : run(line, "w:t"));
    }
    std::string deletedLine(const std::string& line) {
        return paragraph(2, line.empty() ? std::string() : delRun(line));
    }
    std::string insertedLine(const std::string& line) {
        return paragraph(1, line.empty() ? std::string() : insRun(line));
    }

    // A 1:1 Change pair: equal spans stay plain, unequal spans become a
    // deletion followed by an insertion inside the one paragraph, so Word
    // renders struck-through then underlined text in place.
    std::string refinedPair(const std::string& la, const std::string& lb) {
        std::string content;
        for (const auto& s : textdiff::diffInline(la, lb, textdiff::Mode::Auto)) {
            if (s.equal) {
                if (s.aEnd > s.aBeg) content += run(la.substr((std::size_t)s.aBeg, (std::size_t)(s.aEnd - s.aBeg)), "w:t");
                continue;
            }
            if (s.aEnd > s.aBeg) content += delRun(la.substr((std::size_t)s.aBeg, (std::size_t)(s.aEnd - s.aBeg)));
            if (s.bEnd > s.bBeg) content += insRun(lb.substr((std::size_t)s.bBeg, (std::size_t)(s.bEnd - s.bBeg)));
        }
        return paragraph(0, content);
    }
};

}  // namespace

// ------------------------------------------------------------- the note
static std::string plural(int n, const char* one, const char* many) { return n == 1 ? one : many; }

static std::string noteText(const Result& r, const RedlineOptions& o, const RedlineStats& st) {
    std::ostringstream n;
    n << "Revision marks computed by the Diamond Cutter Translation Tool from A = "
      << (o.aName.empty() ? std::string("(unnamed)") : o.aName) << " and B = "
      << (o.bName.empty() ? std::string("(unnamed)") : o.bName)
      << " on " << (o.dateIso.empty() ? std::string("(no date)") : dayOf(o.dateIso)) << ". ";
    n << "Rules in force: " << (o.provenance.empty() ? std::string("none") : o.provenance) << ". ";
    n << textdiff::summary(r) << " ";
    n << "Under those rules " << st.minorMarked << " minor " << plural(st.minorMarked, "difference is", "differences are")
      << " included as " << plural(st.minorMarked, "a revision", "revisions")
      << " — a redline is exact, so nothing is quietly omitted. ";
    n << st.comparedRaw << " " << plural(st.comparedRaw, "line was", "lines were")
      << " compared raw (could not be converted). ";
    n << "Where lines did not pair one to one, whole lines are marked. ";
    n << "Moved blocks appear as a deletion and an insertion";
    if (st.moved) n << " (" << st.moved << " " << plural(st.moved, "block", "blocks") << " here)";
    n << ". ";
    if (st.unrepresentable)
        n << st.unrepresentable << " control " << plural(st.unrepresentable, "character", "characters")
          << " could not be represented and " << plural(st.unrepresentable, "appears", "appear")
          << " as the replacement character U+FFFD. ";
    n << "Tibetan runs are tagged " << o.tibetanFont
      << " in the complex-script slot; Word substitutes another font if it is not installed. ";
    n << "Both drafts are compared as plain text; formatting was not compared. ";
    n << "Export run by " << (o.author.empty() ? std::string("(unnamed)") : o.author) << ". ";
    n << "These marks are machine-computed; they are not an editor's edits.";
    return n.str();
}

// --------------------------------------------------------- document.xml
std::string documentXml(const std::vector<std::string>& a, const std::vector<std::string>& b,
                        const Result& r, const RedlineOptions& o, RedlineStats* stOut) {
    RedlineStats st;
    // The engine's own counts; the writer does not re-derive them.
    st.comparedRaw = r.unnormalised;
    st.moved = r.moved;

    Writer w(o, st);
    std::ostringstream body;

    for (const Hunk& h : r.hunks) {
        const int na = h.aEnd - h.aBeg, nb = h.bEnd - h.bBeg;
        switch (h.kind) {
            case Kind::Equal:
                for (int i = h.aBeg; i < h.aEnd && i < (int)a.size(); ++i) body << w.plainLine(a[(std::size_t)i]);
                break;
            case Kind::Delete:
                for (int i = h.aBeg; i < h.aEnd && i < (int)a.size(); ++i) body << w.deletedLine(a[(std::size_t)i]);
                break;
            case Kind::Insert:
                for (int j = h.bBeg; j < h.bEnd && j < (int)b.size(); ++j) body << w.insertedLine(b[(std::size_t)j]);
                break;
            case Kind::Change:
                if (o.inlineRefine && na == nb) {
                    for (int k = 0; k < na; ++k) {
                        const int i = h.aBeg + k, j = h.bBeg + k;
                        if (i >= (int)a.size() || j >= (int)b.size()) break;
                        body << w.refinedPair(a[(std::size_t)i], b[(std::size_t)j]);
                    }
                } else {
                    // The apparatus() block policy: whole lines out, whole
                    // lines in, no inline refinement across a mismatch.
                    for (int i = h.aBeg; i < h.aEnd && i < (int)a.size(); ++i) body << w.deletedLine(a[(std::size_t)i]);
                    for (int j = h.bBeg; j < h.bEnd && j < (int)b.size(); ++j) body << w.insertedLine(b[(std::size_t)j]);
                }
                break;
        }
        // Minor differences are marked like any other and counted; the
        // note reports the count. One policy, not a checkbox.
        if (h.kind != Kind::Equal && h.unimportant) ++st.minorMarked;
    }

    // The note is written last and placed first: its counts only exist
    // once the body has been walked.
    const std::string note = noteText(r, o, st);

    std::ostringstream d;
    d << "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"yes\"?>\r\n"
      << "<w:document xmlns:w=\"" << kWns << "\"><w:body>"
      << "<w:p><w:r><w:t xml:space=\"preserve\">" << xmlEscape(note) << "</w:t></w:r></w:p>"
      << body.str()
      << "<w:sectPr><w:pgSz w:w=\"12240\" w:h=\"15840\"/>"
         "<w:pgMar w:top=\"1440\" w:right=\"1440\" w:bottom=\"1440\" w:left=\"1440\" "
         "w:header=\"720\" w:footer=\"720\" w:gutter=\"0\"/></w:sectPr>"
      << "</w:body></w:document>";

    if (stOut) *stOut = st;
    return d.str();
}

// ------------------------------------------------------------ the parts
std::string stylesXml(const RedlineOptions& o) {
    std::ostringstream s;
    s << "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"yes\"?>\r\n"
      << "<w:styles xmlns:w=\"" << kWns << "\">"
      << "<w:docDefaults><w:rPrDefault><w:rPr>"
         "<w:rFonts w:ascii=\"Times New Roman\" w:hAnsi=\"Times New Roman\"/>"
         "<w:sz w:val=\"24\"/><w:szCs w:val=\"24\"/>"
         "</w:rPr></w:rPrDefault>"
         "<w:pPrDefault><w:pPr><w:spacing w:after=\"120\"/></w:pPr></w:pPrDefault></w:docDefaults>"
      << "<w:style w:type=\"paragraph\" w:default=\"1\" w:styleId=\"Normal\">"
         "<w:name w:val=\"Normal\"/><w:qFormat/>"
         "<w:rPr><w:rFonts w:ascii=\"Times New Roman\" w:hAnsi=\"Times New Roman\"/><w:sz w:val=\"24\"/></w:rPr>"
         "</w:style>"
      << "<w:style w:type=\"character\" w:styleId=\"Tibetan\">"
         "<w:name w:val=\"Tibetan\"/>"
         "<w:rPr><w:rFonts w:cs=\"" << xmlEscape(o.tibetanFont) << "\"/><w:cs/></w:rPr>"
         "</w:style>"
      << "</w:styles>";
    return s.str();
}

std::string settingsXml() {
    std::ostringstream s;
    s << "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"yes\"?>\r\n"
      << "<w:settings xmlns:w=\"" << kWns << "\">"
      // Further edits the reviewer makes in Word are tracked too.
      << "<w:trackRevisions/>"
      << "<w:defaultTabStop w:val=\"720\"/>"
      << "<w:characterSpacingControl w:val=\"doNotCompress\"/>"
      << "</w:settings>";
    return s.str();
}

std::string coreXml(const RedlineOptions& o) {
    const std::string date = xmlEscape(wordDate(o.dateIso));
    std::ostringstream desc;
    desc << "Revision marks computed by the Diamond Cutter Translation Tool from A = "
         << (o.aName.empty() ? std::string("(unnamed)") : o.aName) << " and B = "
         << (o.bName.empty() ? std::string("(unnamed)") : o.bName) << ". Rules in force: "
         << (o.provenance.empty() ? std::string("none") : o.provenance)
         << ". These marks are machine-computed; they are not an editor's edits.";
    std::ostringstream s;
    s << "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"yes\"?>\r\n"
      << "<cp:coreProperties "
         "xmlns:cp=\"http://schemas.openxmlformats.org/package/2006/metadata/core-properties\" "
         "xmlns:dc=\"http://purl.org/dc/elements/1.1/\" "
         "xmlns:dcterms=\"http://purl.org/dc/terms/\" "
         "xmlns:dcmitype=\"http://purl.org/dc/dcmitype/\" "
         "xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\">"
      << "<dc:title>" << xmlEscape(o.title) << "</dc:title>"
      << "<dc:creator>" << xmlEscape(o.author) << "</dc:creator>"
      << "<cp:lastModifiedBy>" << xmlEscape(o.author) << "</cp:lastModifiedBy>"
      << "<dcterms:created xsi:type=\"dcterms:W3CDTF\">" << date << "</dcterms:created>"
      << "<dcterms:modified xsi:type=\"dcterms:W3CDTF\">" << date << "</dcterms:modified>"
      << "<dc:description>" << xmlEscape(desc.str()) << "</dc:description>"
      << "</cp:coreProperties>";
    return s.str();
}

std::string appXml() {
    return "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"yes\"?>\r\n"
           "<Properties "
           "xmlns=\"http://schemas.openxmlformats.org/officeDocument/2006/extended-properties\" "
           "xmlns:vt=\"http://schemas.openxmlformats.org/officeDocument/2006/docPropsVTypes\">"
           "<Application>Diamond Cutter Translation Tool</Application>"
           "<DocSecurity>0</DocSecurity>"
           "<ScaleCrop>false</ScaleCrop>"
           "<Company>Asian Legacy Library</Company>"
           "<LinksUpToDate>false</LinksUpToDate>"
           "<SharedDoc>false</SharedDoc>"
           "<HyperlinksChanged>false</HyperlinksChanged>"
           "<AppVersion>1.0000</AppVersion>"
           "</Properties>";
}

std::string contentTypesXml() {
    return "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"yes\"?>\r\n"
           "<Types xmlns=\"http://schemas.openxmlformats.org/package/2006/content-types\">"
           "<Default Extension=\"rels\" ContentType=\"application/vnd.openxmlformats-package.relationships+xml\"/>"
           "<Default Extension=\"xml\" ContentType=\"application/xml\"/>"
           "<Override PartName=\"/word/document.xml\" "
           "ContentType=\"application/vnd.openxmlformats-officedocument.wordprocessingml.document.main+xml\"/>"
           "<Override PartName=\"/word/styles.xml\" "
           "ContentType=\"application/vnd.openxmlformats-officedocument.wordprocessingml.styles+xml\"/>"
           "<Override PartName=\"/word/settings.xml\" "
           "ContentType=\"application/vnd.openxmlformats-officedocument.wordprocessingml.settings+xml\"/>"
           "<Override PartName=\"/docProps/core.xml\" "
           "ContentType=\"application/vnd.openxmlformats-package.core-properties+xml\"/>"
           "<Override PartName=\"/docProps/app.xml\" "
           "ContentType=\"application/vnd.openxmlformats-officedocument.extended-properties+xml\"/>"
           "</Types>";
}

std::string relsXml() {
    return "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"yes\"?>\r\n"
           "<Relationships xmlns=\"http://schemas.openxmlformats.org/package/2006/relationships\">"
           "<Relationship Id=\"rId1\" "
           "Type=\"http://schemas.openxmlformats.org/officeDocument/2006/relationships/officeDocument\" "
           "Target=\"word/document.xml\"/>"
           "<Relationship Id=\"rId2\" "
           "Type=\"http://schemas.openxmlformats.org/package/2006/relationships/metadata/core-properties\" "
           "Target=\"docProps/core.xml\"/>"
           "<Relationship Id=\"rId3\" "
           "Type=\"http://schemas.openxmlformats.org/officeDocument/2006/relationships/extended-properties\" "
           "Target=\"docProps/app.xml\"/>"
           "</Relationships>";
}

std::string documentRelsXml() {
    return "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"yes\"?>\r\n"
           "<Relationships xmlns=\"http://schemas.openxmlformats.org/package/2006/relationships\">"
           "<Relationship Id=\"rId1\" "
           "Type=\"http://schemas.openxmlformats.org/officeDocument/2006/relationships/styles\" "
           "Target=\"styles.xml\"/>"
           "<Relationship Id=\"rId2\" "
           "Type=\"http://schemas.openxmlformats.org/officeDocument/2006/relationships/settings\" "
           "Target=\"settings.xml\"/>"
           "</Relationships>";
}

// ----------------------------------------------------------- the package
std::string buildRedlineDocx(const std::vector<std::string>& a, const std::vector<std::string>& b,
                             const Result& r, const RedlineOptions& o, RedlineStats* stOut) {
    RedlineStats st;
    const std::string doc = documentXml(a, b, r, o, &st);
    // [Content_Types].xml first, as OPC requires; the rest in the order of
    // the plan's part table.
    std::vector<zipstore::Part> parts = {
        {"[Content_Types].xml", contentTypesXml()},
        {"_rels/.rels", relsXml()},
        {"word/_rels/document.xml.rels", documentRelsXml()},
        {"word/document.xml", doc},
        {"word/styles.xml", stylesXml(o)},
        {"word/settings.xml", settingsXml()},
        {"docProps/core.xml", coreXml(o)},
        {"docProps/app.xml", appXml()},
    };
    if (stOut) *stOut = st;
    return zipstore::writeStored(parts);
}

}  // namespace docx
}  // namespace allcore
