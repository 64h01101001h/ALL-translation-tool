// Build a self-contained drill pack for the iOS app.
//
// Path A (Adam, 2026-09-09): the phone carries no spine and no converter. Every
// drill is generated HERE, on the Mac, by the same allcore that the desktop
// uses — so the phone inherits today's fixes for free: the whole segment shown
// against the English rather than a quarter of it, the title catalogue and the
// English-in-ACIP segments refused at the draw, and editorial markup separated
// from script instead of leaking raw.
//
// Tibetan is converted here too, through allcore::acipDisplayPieces, so the app
// only ever renders Unicode it was handed. Anything that would not convert
// arrives already flagged in ⟨ ⟩ and can never be mistaken for script.
#include "allcore/drills.h"
#include "allcore/lattice.h"
#include "allcore/spine.h"
#include "allcore/tibdisplay.h"

#include <cstdio>
#include <random>
#include <string>
#include <vector>

namespace {

void esc(std::string& o, const std::string& s) {
    for (char c : s) {
        switch (c) {
            case '"': o += "\\\""; break;
            case '\\': o += "\\\\"; break;
            case '\n': o += "\\n"; break;
            case '\r': break;
            case '\t': o += " "; break;
            default:
                if ((unsigned char)c < 0x20) o += ' ';
                else o += c;
        }
    }
}
void field(std::string& o, const char* k, const std::string& v, bool comma = true) {
    o += "\""; o += k; o += "\":\"";
    esc(o, v);
    o += comma ? "\"," : "\"";
}
std::string tib(const std::string& acip) {
    return allcore::acipToTibetanPlain(acip);
}
// A title line still gets badged on the phone, for the rare one that is a
// legitimate heading rather than catalogue.
bool isTitle(const std::string& course) {
    std::string u = course;
    for (auto& c : u) c = (char)std::toupper((unsigned char)c);
    return u.rfind("TITL", 0) == 0 || u == "AUTH" || u == "SUBJ";
}

}  // namespace

int main(int argc, char** argv) {
    if (argc < 3) {
        std::fprintf(stderr,
                     "usage: build_drill_pack <spine.db> <out.json> [count]\n");
        return 2;
    }
    const int want = argc > 3 ? std::atoi(argv[3]) : 4000;
    allcore::Spine spine(argv[1]);
    allcore::HeadwordIndex index(spine);
    allcore::DrillFactory f(spine, index);
    std::mt19937 rng(20260909);

    std::string out = "{\"meta\":{";
    field(out, "built_by", "allcore, on the Mac; the phone generates nothing");
    field(out, "source", spine.metaValue("source_corpus"));
    field(out, "tier",
          "Every English line is Geshe Michael Roach's own text from his "
          "courses. Engine guidance is labeled guidance.");
    out += "\"schema\":1},\"cloze\":[";

    int n = 0, titles = 0;
    for (int i = 0; i < want * 8 && n < want; ++i) {
        auto c = f.makeCloze(rng);
        if (!c) continue;
        if (c->correct < 0 || c->correct >= (int)c->options.size()) continue;
        const std::string& seg = c->segment.acip;
        const std::string ans = c->options[c->correct];
        // the whole segment, with the answer blanked where it stands
        std::string prefix;
        for (const auto& ch : c->chunks) {
            if (ch == "[ ... ]") break;
            if (!prefix.empty()) prefix += " ";
            prefix += ch;
        }
        size_t from = 0;
        if (!prefix.empty()) {
            const size_t p = seg.find(prefix);
            if (p != std::string::npos) from = p;
        }
        size_t at = seg.find(ans, from);
        if (at == std::string::npos) at = seg.find(ans);
        if (at == std::string::npos) continue;   // refuse rather than approximate

        if (n) out += ",";
        out += "{";
        field(out, "before", tib(seg.substr(0, at)));
        field(out, "after", tib(seg.substr(at + ans.size())));
        field(out, "english", c->segment.english);
        field(out, "role", c->role);
        field(out, "course", c->segment.course);
        out += "\"seq\":" + std::to_string(c->segment.seq) + ",";
        out += std::string("\"title\":") + (isTitle(c->segment.course) ? "true" : "false") + ",";
        if (isTitle(c->segment.course)) ++titles;
        out += "\"answer\":" + std::to_string(c->correct) + ",";
        out += "\"options\":[";
        for (size_t k = 0; k < c->options.size(); ++k) {
            if (k) out += ",";
            out += "\"";
            esc(out, tib(c->options[k]));
            out += "\"";
        }
        out += "]}";
        ++n;
    }
    out += "]}";

    FILE* fp = std::fopen(argv[2], "wb");
    if (!fp) { std::fprintf(stderr, "cannot write %s\n", argv[2]); return 1; }
    std::fwrite(out.data(), 1, out.size(), fp);
    std::fclose(fp);
    std::printf("wrote %s: %d cloze drills, %.2f MB (%zu bytes each)\n",
                argv[2], n, out.size() / 1048576.0,
                n ? out.size() / (size_t)n : 0);
    std::printf("  title-catalogue drills in the pack: %d (the draw refuses "
                "them; this should read 0)\n", titles);
    return n > 0 ? 0 : 1;
}
