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
#include "allcore/terminology.h"
#include "allcore/tibdisplay.h"
#include "allcore/tibdisplay.h"

#include <algorithm>
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
// Markup is stripped before conversion, on both the segment and the options.
// Adam, 2026-09-09: an option beginning "(" against a segment ending ")" gives
// the answer away before a word of Tibetan is read. It is a tell, and markup is
// not part of the answer.
std::string tib(const std::string& acip) {
    return allcore::acipToTibetanPlain(allcore::acipStripMarkup(acip));
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
                     "usage: build_drill_pack <spine.db> <out.json> "
                     "[count] [stamp]\n");
        return 2;
    }
    const int want = argc > 3 ? std::atoi(argv[3]) : 4000;
    // A stamp naming the commit and moment this pack was built. Without
    // one a STALE pack is indistinguishable from a fresh one, and the
    // phone will happily show yesterday's drills after a fix landed.
    const char* stamp = argc > 4 ? argv[4] : "unstamped";
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
    field(out, "built_from", stamp);
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

        // The segment converts as a whole — isDrillable already refuses one
        // that does not — but SPLITTING it at the blank can leave a fragment
        // that will not convert on its own, e.g. a bare "s". The two halves
        // are what the reader actually sees, so they are what must be
        // checked. Cheap, and it closes the last 0.1%.
        const std::string beforeT = tib(seg.substr(0, at));
        const std::string afterT = tib(seg.substr(at + ans.size()));
        if (beforeT.find("\u27e8") != std::string::npos ||
            afterT.find("\u27e8") != std::string::npos)
            continue;

        if (n) out += ",";
        out += "{";
        field(out, "before", beforeT);
        field(out, "after", afterT);
        field(out, "english", c->segment.english);
        field(out, "role", c->role);
        // The skill this drill exercises, in the SAME vocabulary the desktop
        // files misses under (allcore::clozeSkill). The phone cannot generate
        // a targeted drill — the pack is built here — but with every drill
        // tagged it can FILTER, which is the same feature reached the other
        // way round. One vocabulary, so a weak spot named on the desktop
        // means the same thing on the phone.
        field(out, "skill", allcore::clozeSkill(*c));
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
        out += "],";
        // What the blanked word means — Adam, 2026-09-10: "I have answered
        // correctly a couple of times and STILL wondered what the English
        // equivalent was." Rule 1 forbids composing English, so this is
        // strictly a MATCH: allcore::checkTerminology takes the blanked chunk
        // and his own English for this segment, and reports which of his
        // recorded equivalents actually occur in it. Evidence that the match
        // is real rather than coincidence: against his English for THIS
        // segment one or more equivalents occur 50.3% of the time; against a
        // random other segment, 14.6%. A lift of 35.7 points.
        //
        // Never "the answer means X" — the median glossed headword has four
        // attested equivalents and the worst has 137, so naming one would be
        // picking a sense, which is a guess. The card shows his recorded range
        // and marks which of them he used here.
        {
            const allcore::TermReport tr = allcore::checkTerminology(
                spine, index, ans, c->segment.english);
            // checkTerminology orders unmatched first, because it was built
            // to flag gaps in a translator's draft. A learner's card wants the
            // opposite: the word he actually used here is the answer to the
            // question being asked, and a content word teaches more than a
            // nominaliser. So: matched first, then the longer headword.
            std::vector<allcore::TermUse> terms = tr.terms;
            std::stable_sort(terms.begin(), terms.end(),
                             [](const allcore::TermUse& a,
                                const allcore::TermUse& b) {
                                 if (a.matched.empty() != b.matched.empty())
                                     return !a.matched.empty();
                                 return a.wylie.size() > b.wylie.size();
                             });
            out += "\"means\":[";
            int emitted = 0;
            for (const auto& t : terms) {
                if (emitted >= 2) break;       // two headwords is a card, not a wall
                if (t.glosses.empty()) continue;
                if (emitted) out += ",";
                out += "{";
                field(out, "wylie", t.wylie);
                field(out, "tier", t.tier);
                out += std::string("\"provisional\":") +
                       (t.provisional ? "true" : "false") + ",";
                out += "\"glosses\":[";
                // twenty carried, eight shown until Look up is pressed — the
                // button must have something real to reveal
                for (size_t k = 0; k < t.glosses.size() && k < 20; ++k) {
                    if (k) out += ",";
                    out += "\""; esc(out, t.glosses[k]); out += "\"";
                }
                out += "],\"more\":" +
                       std::to_string(t.glosses.size() > 20
                                          ? t.glosses.size() - 20 : 0) + ",";
                out += "\"used\":[";
                for (size_t k = 0; k < t.matched.size() && k < 4; ++k) {
                    if (k) out += ",";
                    out += "\""; esc(out, t.matched[k]); out += "\"";
                }
                out += "]}";
                ++emitted;
            }
            out += "]";
        }
        out += "}";
        ++n;
    }
    out += "],";

    // ---- The other drill kinds (Adam, 2026-09-11: "the iOS version just has
    // the cloze option"). He was right, and it was a parity failure against
    // his own standing order. The phone generates nothing — the pack is built
    // here — so every kind it can offer has to be baked, and that is what the
    // rest of this file does now.
    //
    // Three desktop modes are NOT baked, each for a reason:
    //   Parallel reading    — it is corpus segments to read, which the trainer
    //                         array already carries.
    //   Vocabulary (SRS)    — the deck is the LEARNER's, built from what they
    //                         clicked on the desktop, and the two have no way
    //                         to sync. A second unconnected deck on the phone
    //                         would quietly compete with the real one.
    //   Translate & compare — needs checkTerminology run against the whole
    //                         105,634-entry spine, over a draft typed at the
    //                         moment. Not bakeable; genuinely desktop-only.
    {
        std::mt19937 r2(20260911);
        out += "\"order\":[";
        int no = 0;
        for (int i = 0; i < want * 4 && no < want / 4; ++i) {
            auto o = f.makeOrder(r2);
            if (!o) continue;
            bool bad = false;
            for (const auto& ch : o->chunks)
                if (tib(ch).find("\u27e8") != std::string::npos) bad = true;
            if (bad) continue;               // same refusal as the cloze
            if (no) out += ",";
            out += "{";
            field(out, "english", o->segment.english);
            field(out, "course", o->segment.course);
            out += "\"seq\":" + std::to_string(o->segment.seq) + ",";
            out += "\"chunks\":[";
            for (size_t k = 0; k < o->chunks.size(); ++k) {
                if (k) out += ",";
                out += "\""; esc(out, tib(o->chunks[k])); out += "\"";
            }
            out += "],\"presented\":[";
            for (size_t k = 0; k < o->presented.size(); ++k) {
                if (k) out += ",";
                out += std::to_string(o->presented[k]);
            }
            out += "],\"verb\":";
            out += std::string("{") ;
            {
                std::string vb;
                field(vb, "wylie", o->verb.confident ? o->verb.wylie : "");
                field(vb, "evidence", o->verb.evidence);
                vb += std::string("\"confident\":") +
                      (o->verb.confident ? "true" : "false");
                out += vb;
            }
            out += "}}";
            ++no;
        }
        out += "],\"particle\":[";
        int np = 0;
        for (int i = 0; i < want * 4 && np < want / 4; ++i) {
            auto pd = f.makeParticle(r2);
            if (!pd) continue;
            if (np) out += ",";
            out += "{";
            field(out, "english", pd->segment.english);
            field(out, "course", pd->segment.course);
            out += "\"seq\":" + std::to_string(pd->segment.seq) + ",";
            field(out, "explanation", pd->explanation);
            out += "\"correct\":" + std::to_string(pd->correct) + ",";
            out += "\"options\":[";
            for (size_t k = 0; k < pd->options.size(); ++k) {
                if (k) out += ",";
                out += "\""; esc(out, pd->options[k]); out += "\"";
            }
            out += "],\"tokens\":[";
            for (size_t k = 0; k < pd->tokens.size(); ++k) {
                if (k) out += ",";
                out += "\""; esc(out, tib(pd->tokens[k])); out += "\"";
            }
            out += "]}";
            ++np;
        }
        out += "],";

        // Script cards: his own Language Study Guide cards. Everything on one
        // is his — source-attested Tibetan, his pronunciation, his gloss — so
        // it is the beginner's rung and the phone should have it.
        out += "\"script\":[";
        int ns = 0;
        for (const auto& e : spine.scriptCards("", 1200)) {
            if (e.tibetan.empty() || e.pronunciation.empty()) continue;
            if (ns) out += ",";
            out += "{";
            field(out, "tibetan", e.tibetan);
            field(out, "wylie", e.wylie);
            field(out, "pron", e.pronunciation);
            field(out, "tier", e.tier);
            out += std::string("\"provisional\":") +
                   (e.provisional() ? "true" : "false") + ",";
            out += "\"gloss\":[";
            for (size_t k = 0; k < e.hgm_gloss.size() && k < 4; ++k) {
                if (k) out += ",";
                out += "\""; esc(out, e.hgm_gloss[k]); out += "\"";
            }
            out += "]}";
            ++ns;
        }
        out += "],";

        // The Debate Dojo: statements carrying the full template, split into
        // their three elements here so the phone does no parsing.
        out += "\"debate\":[";
        int nd = 0;
        for (const auto& sg : spine.corpusSearch("\"CHOS CAN\" AND \"THAL\"",
                                                 "", 400)) {
            if (nd >= 200) break;
            // the documented C13 window is refused, as on the desktop
            if (sg.course == "C13" && sg.seq >= 63 && sg.seq <= 82) continue;
            const auto db = allcore::parseDebate(sg.acip);
            if (!db.ok) continue;
            const std::string sub = tib(db.subject), con = tib(db.consequence),
                              rea = tib(db.reason);
            if (sub.find("\u27e8") != std::string::npos ||
                con.find("\u27e8") != std::string::npos ||
                rea.find("\u27e8") != std::string::npos) continue;
            if (nd) out += ",";
            out += "{";
            field(out, "preamble", tib(db.preamble));
            field(out, "subject", sub);
            field(out, "consequence", con);
            field(out, "reason", rea);
            field(out, "english", sg.english);
            field(out, "course", sg.course);
            out += "\"seq\":" + std::to_string(sg.seq);
            out += "}";
            ++nd;
        }
        out += "],";
        std::printf("  order %d · particle %d · script %d · debate %d\n",
                    no, np, ns, nd);
    }

    out += "\"trainer\":[";

    // The Trainer: a passage to read yourself, then six layers revealed one at
    // a time — chunks, particle roles, reading order, vocabulary, the answer
    // key, and the full parse. makeOrder already computes all of it, so the
    // phone reveals what the Mac worked out. Adam, 2026-09-09: the iOS app is
    // "a Tibetan Translation Trainer that has both the drills functionality
    // and the Trainer tool".
    int t = 0;
    for (int i = 0; i < want * 8 && t < want / 4; ++i) {
        auto o = f.makeOrder(rng);
        if (!o) continue;
        if (o->chunks.size() < 2) continue;
        if (t) out += ",";
        out += "{";
        field(out, "tibetan", tib(o->segment.acip));
        field(out, "english", o->segment.english);
        field(out, "course", o->segment.course);
        out += "\"seq\":" + std::to_string(o->segment.seq) + ",";
        // layer 1 — the chunks, in their true order
        out += "\"chunks\":[";
        for (size_t k = 0; k < o->chunks.size(); ++k) {
            if (k) out += ",";
            out += "\""; esc(out, tib(o->chunks[k])); out += "\"";
        }
        out += "],";
        // layer 2 — what each chunk is doing
        out += "\"roles\":[";
        for (size_t k = 0; k < o->chunks.size(); ++k) {
            if (k) out += ",";
            out += "\"";
            esc(out, k < o->markers.size() ? o->markers[k] : std::string());
            out += "\"";
        }
        out += "],";
        // layer 3 — the reading order, with the guidance for each step
        out += "\"plan\":[";
        for (size_t k = 0; k < o->plan.size(); ++k) {
            if (k) out += ",";
            const auto& st = o->plan[k];
            out += "{\"chunk\":" + std::to_string(st.chunk) +
                   ",\"order\":" + std::to_string(st.order) + ",";
            field(out, "how", st.how ? st.how : "", false);
            out += "}";
        }
        out += "],";
        // the verb, which frames the clause — layer 6's anchor
        out += "{";
        out.pop_back();
        // How much of the passage on screen the layers below actually
        // describe. The passage is the WHOLE segment; the chunks, roles and
        // plan come from ONE clause of it (drills.cpp picks 3-7 chunks), so
        // the median is about a third. The surface must say so rather than
        // let the layers read as an account of everything above them.
        {
            auto sylCount = [](const std::string& t) {
                int n = 0; bool in = false;
                for (size_t i = 0; i < t.size(); ++i) {
                    const unsigned char ch = (unsigned char)t[i];
                    const bool sep = (ch == ' ') ||
                        (i + 2 < t.size() && ch == 0xE0 &&
                         (unsigned char)t[i+1] == 0xBC &&
                         (unsigned char)t[i+2] == 0x8B);   // U+0F0B tsheg
                    if (sep) { in = false; if (ch != ' ') i += 2; }
                    else if (!in) { in = true; ++n; }
                }
                return n;
            };
            int whole = sylCount(tib(o->segment.acip)), part = 0;
            for (const auto& c : o->chunks) part += sylCount(tib(c));
            const int share = whole ? (part * 100 / whole) : 100;
            out += "\"clause_share\":" + std::to_string(share) + ",";
        }
        field(out, "verb", o->verb.wylie);
        field(out, "verb_evidence", o->verb.evidence);
        out += std::string("\"verb_confident\":") +
               (o->verb.confident ? "true" : "false");
        out += "}";
        ++t;
    }
    out += "]}";

    FILE* fp = std::fopen(argv[2], "wb");
    if (!fp) { std::fprintf(stderr, "cannot write %s\n", argv[2]); return 1; }
    std::fwrite(out.data(), 1, out.size(), fp);
    std::fclose(fp);
    std::printf("wrote %s: %d cloze drills, %.2f MB (%zu bytes each)\n",
                argv[2], n, out.size() / 1048576.0,
                n ? out.size() / (size_t)n : 0);
    std::printf("  trainer passages: %d\n", t);
    std::printf("  title-catalogue drills in the pack: %d (the draw refuses "
                "them; this should read 0)\n", titles);
    return n > 0 ? 0 : 1;
}
