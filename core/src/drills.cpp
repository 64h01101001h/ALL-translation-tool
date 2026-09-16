#include "allcore/drills.h"

#include "allcore/chantline.h"

#include "allcore/tibdisplay.h"

#include <algorithm>

#include "allcore/particles.h"

namespace allcore {

DebateStatement parseDebate(const std::string& acip) {
    DebateStatement d;
    const size_t cc = acip.find("CHOS CAN");
    if (cc == std::string::npos) return d;
    const size_t th = acip.find("THAL", cc);
    if (th == std::string::npos) return d;
    const size_t ph = acip.find("PHYIR", th);
    if (ph == std::string::npos) return d;
    auto trim = [](std::string v) {
        while (!v.empty() && (v.front() == ' ' || v.front() == ',')) v.erase(0, 1);
        while (!v.empty() && (v.back() == ' ' || v.back() == ',')) v.pop_back();
        return v;
    };
    // The subject is the clause immediately before CHOS CAN, not everything
    // before it: these statements often open with a conditional or a citation.
    {
        const std::string head = acip.substr(0, cc);
        const size_t comma = head.find_last_of(',');
        if (comma != std::string::npos) {
            d.preamble = trim(head.substr(0, comma));
            d.subject = trim(head.substr(comma + 1));
        } else {
            d.subject = trim(head);
        }
    }
    d.consequence = trim(acip.substr(cc + 8, th - (cc + 8)));
    d.reason = trim(acip.substr(th + 4, ph - (th + 4)));
    // A template whose elements are empty is not a statement; refuse rather
    // than hand back blanks that would render as an unanswerable card.
    d.ok = !d.subject.empty() && !d.consequence.empty() && !d.reason.empty();
    return d;
}

std::string debateReplyTarget(const std::string& reply) {
    if (reply == "MA GRUB NA") return "subject";
    if (reply == "RTAGS MA GRUB") return "reason";
    if (reply == "MA KHYAB NA") return "entailment";
    if (reply == "RTZA BAR 'DOD NA") return "accept";
    return std::string();
}

bool targetRefused(const DrillTarget& t, std::string* why) {
    if (t.kind != DrillTarget::Kind::ClozeRole) return false;
    const std::string head = t.skill.substr(0, t.skill.find(' '));
    if (head == "unmarked" || head == "predicate") {
        if (why)
            *why = "The cloze drill blanks a chunk that carries a role "
                   "marker, because the marker is what makes the question "
                   "fairly answerable from the English. Training on \"" +
                   head +
                   "\" would force questions this generator already treats as "
                   "unfair, so a weakness measured there would say more about "
                   "the generator than about the reader.";
        return true;
    }
    return false;
}

std::string clozeSkill(const ClozeDrill& d) {
    // the same string the miss taxonomy files under, so the two cannot drift
    const std::string r = d.role;
    return r.substr(0, r.find(' '));
}

std::string particleSkill(const ParticleDrill& d) {
    return d.options.empty() ? std::string() : d.options[0] + "-family";
}



namespace {

constexpr int kAttempts = 60;   // segment draws before giving up

std::string chunkText(const OverlayDoc& doc, const Chunk& c) {
    std::string s;
    for (int t = c.beg; t < c.end; ++t) {
        if (t > c.beg) s += ' ';
        s += doc.tokens[t];
    }
    return s;
}

// pick the "best" clause of a segment: most chunks within [lo, hi]
struct PickedClause {
    Clause clause;
    std::vector<Chunk> chunks;
};

std::optional<PickedClause> pickClause(const OverlayDoc& doc,
                                       const std::vector<Clause>& clauses,
                                       size_t lo, size_t hi) {
    std::optional<PickedClause> best;
    for (const auto& cl : clauses) {
        auto chunks = chunkClause(doc, cl);
        if (chunks.size() < lo || chunks.size() > hi) continue;
        if (!best || chunks.size() > best->chunks.size())
            best = PickedClause{cl, std::move(chunks)};
    }
    return best;
}

}  // namespace

// A drill must come from teaching material. Two classes of corpus segment are
// not that, and both were reaching learners (audit, 2026-09-09):
//
//  1. THE TITLE CATALOGUE. 4,010 of the 42,199 segments (9.5%) live in the
//     courses TITLK, TITLR, TITLS, TITLT, plus AUTH and SUBJ — they are a
//     catalogue of work titles, author names and subject headings, not prose.
//     They are also the SHORTEST things in the corpus (10-15 ACIP words
//     against a corpus mean near 36), and randomSegment's adaptive branch
//     prefers short segments, so it selected them at 2-3x their share:
//     measured at 20.8-25.0% of adaptive draws, with TITLS the single
//     most-drawn course of all 75. Roughly a quarter of Adam's drills were
//     title lines, which is exactly what he kept screenshotting.
//
//  2. ENGLISH SITTING IN THE ACIP FIELD. 254 segments (0.60%) carry English
//     prose where the Tibetan should be — front matter that was never
//     translated. Fed to the script converter it renders as plausible-looking
//     nonsense (the Asian Classics Institute becomes ཐེ་ཨསིན་ཅླསྶིཅས་…), which
//     is fabrication, not a display bug. 13 of the 75 courses OPEN on one.
//
// Both are refused here, at the draw, so every mode is covered at once.
bool DrillFactory::isDrillable(const CorpusSegment& seg) {
    if (!seg.id || seg.acip.empty() || seg.english.empty()) return false;
    std::string c = seg.course;
    for (auto& ch : c) ch = (char)std::toupper((unsigned char)ch);
    if (c.rfind("TITL", 0) == 0 || c == "AUTH" || c == "SUBJ") return false;
    // C13:63-82 -- the documented column offset. Twenty segments whose Tibetan
    // is paired with the English of a DIFFERENT catalogue item: C13:72 has the
    // Tree of Sandalwood against "12) The Servant of Gentle Voice (Manjushri)",
    // the previous item's title. See docs/upstream/C13_COLUMN_OFFSET.md.
    //
    // This refusal existed in exactly two places, both inline in a single
    // pool's loop: the desktop's debate path and the pack builder's. Every
    // OTHER draw -- cloze, trainer, order, particle, boundary, vocab -- came
    // through here, where there was no rule, so 8 of the 20 pass every other
    // test and were eligible for any of them. A card built from one would put
    // another text's title on screen under "Geshe Michael Roach's English".
    // The rule belongs at the one gate every pool already asks.
    if (c == "C13" && seg.seq >= 63 && seg.seq <= 82) return false;
    // ACIP is written in capitals. A run of lower-case letters in the Tibetan
    // field means the field holds English, not transliteration.
    int lower = 0, upper = 0;
    for (unsigned char ch : seg.acip) {
        if (ch >= 'a' && ch <= 'z') ++lower;
        else if (ch >= 'A' && ch <= 'Z') ++upper;
    }
    if (lower > upper) return false;
    //  2b. SYLLABLES THE CONVERTER REFUSES. Ritual texts carry Sanskrit
    //      mantras written in Tibetan letters — hrI:, ShTrI:, hUM^ — which the
    //      Tibetan converter will not render, and which it therefore flags as
    //      ⟨wylie⟩ rather than guess at (rule 3). That flag is correct, and
    //      a passage full of them is still not a reading exercise: it has no
    //      verb, no particles and no reading order, so the Trainer's own
    //      layers have nothing true to say about it. Found by Adam on the
    //      phone, 2026-09-10, on a Kali Wang empowerment text; measured at
    //      9.8% of trainer passages and 9.6% of cloze drills.
    //
    //      The mantras themselves are not the problem and are not being
    //      dismissed — the Sanskrit workbench reads them properly. They are
    //      simply the wrong material for a drill about Tibetan grammar.
    {
        const std::string t = acipToTibetanPlain(acipStripMarkup(seg.acip));
        if (t.find("\u27e8") != std::string::npos) return false;
    }
    //  2c. AND THE MIRROR OF THAT: the Tibetan converts perfectly, but the
    //      ENGLISH was never English. 1,066 of the 42,199 segments carrying an
    //      English field hold a chant transliteration ("sashi pukyi jukshing
    //      metok tram," is the SOUND of its own Tibetan) or a Sanskrit mantra
    //      carried over untranslated ("Om argham praticha sva ha."). Every
    //      drill that shows one of these prints it as "Geshe Michael Roach's
    //      English for this segment", which is a false claim -- 64 of them
    //      were in the shipped pack when this was found, 2026-09-13.
    //
    //      2b caught the mantras whose TIBETAN will not convert. This catches
    //      the ones where only the English side is wrong, which 2b cannot see.
    if (englishIsNotEnglish(seg.english, seg.acip)) return false;
    //  3. ENGLISH WILDLY OUT OF PROPORTION TO THE TIBETAN. Some segments carry
    //     a whole explanatory passage against a line or two of Tibetan — one
    //     pairs 5,180 English words with 5 ACIP words. As a drill that is
    //     useless: the learner is given three hundred words of English as the
    //     "hint" for three syllables. Measured over all 42,199 segments, the
    //     median is 1.43 English words per ACIP word and the 95th percentile
    //     is 3.11, so a ceiling of 6:1 sits far above normal prose and refuses
    //     only 774 segments (1.8%). Found on the iPhone build, 2026-09-09,
    //     where the disproportion is impossible to miss on a small screen.
    auto words = [](const std::string& t) {
        size_t n = 0;
        bool in = false;
        for (char ch : t) {
            const bool sp = (ch == ' ' || ch == '\t' || ch == '\n');
            if (!sp && !in) { ++n; in = true; }
            else if (sp) in = false;
        }
        return n;
    };
    //     The ratio is measured on the English that actually PARALLELS the
    //     Tibetan. Material in [square brackets] is supplied by the translator
    //     — a debate restatement of the previous move, a gloss, a page number —
    //     and has no Tibetan in this segment at all. 3,045 segments (7.2%)
    //     carry some. Counting it makes an ordinary segment look lopsided:
    //     Adam's C15 example measures 4.3 English words per ACIP word with the
    //     brackets and 1.4 without, and 1.4 is the corpus median exactly. So
    //     the brackets are stripped before the ratio is taken, and a good
    //     segment is not refused for carrying honest supplied context.
    std::string parallel;
    {
        int depth = 0;
        for (char ch : seg.english) {
            if (ch == '[') { ++depth; parallel += ' '; continue; }
            if (ch == ']') { if (depth) --depth; parallel += ' '; continue; }
            if (!depth) parallel += ch;
        }
    }
    const size_t wa = words(seg.acip), we = words(parallel);
    if (wa == 0 || we == 0) return false;
    if (we > wa * 6) return false;
    return true;
}

CorpusSegment DrillFactory::uniformSegment(std::mt19937& rng) const {
    const long long maxId = spine_.corpusMaxId();
    for (int i = 0; i < 16; ++i) {
        const long long id = 1 + (long long)(rng() % (unsigned long)maxId);
        auto seg = spine_.corpusSegmentById(id);
        if (isDrillable(seg)) return seg;
    }
    return {};
}

CorpusSegment DrillFactory::randomSegment(std::mt19937& rng) const {
    if (!adaptive_ || !progress_) return uniformSegment(rng);
    // resurfacing: roughly one draw in four revisits a peeked segment
    if (rng() % 4 == 0) {
        auto peeked = progress_->peekedSegments(20);
        if (!peeked.empty()) {
            auto seg =
                spine_.corpusSegmentById(peeked[rng() % peeked.size()]);
            if (isDrillable(seg)) return seg;
        }
    }
    // level targeting: sample a few candidates, keep the one whose
    // known-vocabulary coverage is closest to the ~0.65 sweet spot (with a
    // mild penalty for very long segments). A brand-new learner (coverage 0
    // everywhere) simply gets the shortest candidate — a gentle start.
    CorpusSegment best;
    double bestScore = -1e9;
    for (int c = 0; c < 5; ++c) {
        auto seg = uniformSegment(rng);
        if (!seg.id) continue;
        auto doc = buildOverlay(spine_, index_, seg.acip);
        std::vector<std::string> words;
        for (const auto& e : doc.entries)
            if (!e.hgm_gloss.empty()) words.push_back(e.wylie);
        const double cov = progress_->coverage(words);
        double score = -std::abs(cov - 0.65);
        score -= 0.01 * std::max<int>(0, (int)doc.tokens.size() - 25);
        if (score > bestScore) {
            bestScore = score;
            best = std::move(seg);
        }
    }
    return best.id ? best : uniformSegment(rng);
}

std::optional<OrderDrill> DrillFactory::makeOrder(std::mt19937& rng) const {
    for (int attempt = 0; attempt < kAttempts; ++attempt) {
        auto seg = randomSegment(rng);
        if (!seg.id) continue;
        auto doc = buildOverlay(spine_, index_, seg.acip);
        auto clauses =
            refineClauses(doc, splitClauses(doc.tokens, doc.barrier_after));
        auto picked = pickClause(doc, clauses, 3, 7);
        if (!picked) continue;
        OrderDrill d;
        d.segment = std::move(seg);
        for (const auto& c : picked->chunks) {
            d.chunks.push_back(chunkText(doc, c));
            d.markers.push_back(c.marker);
        }
        d.boundary = picked->clause.boundary;
        d.verb = spotVerb(doc, picked->chunks);
        d.plan = planReading(picked->chunks, d.verb);
        d.presented.resize(d.chunks.size());
        for (size_t i = 0; i < d.presented.size(); ++i) d.presented[i] = (int)i;
        // shuffle until not the identity (n>=3 guarantees termination)
        do {
            std::shuffle(d.presented.begin(), d.presented.end(), rng);
        } while (std::is_sorted(d.presented.begin(), d.presented.end()));
        return d;
    }
    return std::nullopt;
}

std::optional<ClozeDrill> DrillFactory::makeCloze(
    std::mt19937& rng, const DrillTarget& target) const {
    // A refused target never becomes a silent off-target draw.
    if (targetRefused(target)) return std::nullopt;
    const bool aiming =
        target.kind == DrillTarget::Kind::ClozeRole && !target.skill.empty();
    for (int attempt = 0; attempt < kAttempts; ++attempt) {
        auto seg = randomSegment(rng);
        if (!seg.id) continue;
        auto doc = buildOverlay(spine_, index_, seg.acip);
        auto clauses =
            refineClauses(doc, splitClauses(doc.tokens, doc.barrier_after));
        auto picked = pickClause(doc, clauses, 2, 7);
        if (!picked) continue;
        // blank a MARKED chunk when one exists (its role makes the question
        // answerable from the English); else any chunk
        std::vector<int> marked;
        for (size_t i = 0; i < picked->chunks.size(); ++i)
            if (!picked->chunks[i].marker.empty()) marked.push_back((int)i);
        const int blank =
            !marked.empty()
                ? marked[rng() % marked.size()]
                : (int)(rng() % picked->chunks.size());
        // A targeted draw keeps looking until the blanked chunk exercises the
        // skill asked for. The loop's own attempt budget bounds this, and
        // running out returns nullopt so the caller can say so.
        if (aiming) {
            const std::string& r = picked->chunks[blank].role;
            if (r.substr(0, r.find(' ')) != target.skill) continue;
        }
        const std::string answer = chunkText(doc, picked->chunks[blank]);
        // Distractors must carry THE SAME role marker as the answer.
        //
        // They used to be any marked chunk from any other segment, and the
        // consequence was measured over a shipped 4,000-drill pack: in 72.2%
        // of drills exactly one option's case particle fitted the slot, so the
        // answer could be picked without reading the Tibetan or the English at
        // all. Only 22.6% required meaning. The drill was training particle
        // recognition while appearing to test reading — which is exactly what
        // Adam reported: answering correctly and still not knowing what the
        // chunk meant.
        //
        // Matching the marker removes the grammatical give-away, so the four
        // options are all syntactically possible and only sense decides. A
        // drill that cannot be filled this way is REJECTED rather than filled
        // with an easier distractor: fewer honest drills beat more guessable
        // ones, and the pack builder draws until it has its count.
        const std::string wantMarker = picked->chunks[blank].marker;
        std::vector<std::string> distractors;
        for (int tries = 0; tries < 120 && distractors.size() < 3; ++tries) {
            auto other = randomSegment(rng);
            if (!other.id || other.id == seg.id) continue;
            auto odoc = buildOverlay(spine_, index_, other.acip);
            auto ocls = refineClauses(
                odoc, splitClauses(odoc.tokens, odoc.barrier_after));
            auto opick = pickClause(odoc, ocls, 2, 7);
            if (!opick) continue;
            for (const auto& c : opick->chunks) {
                if (c.marker.empty()) continue;
                if (c.marker != wantMarker) continue;   // same slot, or no use
                std::string t = chunkText(odoc, c);
                if (t == answer) continue;
                bool dup = false;
                for (auto& x : distractors) dup |= (x == t);
                if (!dup) {
                    distractors.push_back(std::move(t));
                    break;
                }
            }
        }
        if (distractors.size() < 3) continue;
        ClozeDrill d;
        d.segment = std::move(seg);
        for (size_t i = 0; i < picked->chunks.size(); ++i)
            d.chunks.push_back((int)i == blank
                                   ? std::string("[ ... ]")
                                   : chunkText(doc, picked->chunks[i]));
        d.blank = blank;
        d.role = picked->chunks[blank].role;
        d.options = distractors;
        d.options.insert(d.options.begin() + (rng() % 4), answer);
        for (size_t i = 0; i < d.options.size(); ++i)
            if (d.options[i] == answer) d.correct = (int)i;
        return d;
    }
    return std::nullopt;
}

std::optional<ParticleDrill> DrillFactory::makeParticle(
    std::mt19937& rng, const DrillTarget& target) const {
    const bool aiming =
        target.kind == DrillTarget::Kind::ParticleFamily && !target.skill.empty();
    // families whose members a learner must choose between
    static const std::vector<std::vector<std::string>> kFamilies = {
        {"gi", "kyi", "gyi", "yi"},
        {"gis", "kyis", "gyis", "yis"},
        {"kyang", "yang"},
        {"tu", "du", "su", "ru"},
        {"ste", "te", "de"},
        {"cing", "zhing", "shing"},
        {"ces", "zhes", "shes"},
    };
    for (int attempt = 0; attempt < kAttempts; ++attempt) {
        auto seg = randomSegment(rng);
        if (!seg.id) continue;
        std::vector<std::string> toks;
        std::vector<bool> barriers;
        tokenizeDocument(seg.acip, toks, barriers);
        // collect candidate positions: token is a family member, not
        // clause-initial
        struct Cand {
            int tok;
            int family;
        };
        std::vector<Cand> cands;
        for (int i = 1; i < (int)toks.size(); ++i) {
            if (barriers[i - 1]) continue;
            std::string low = toks[i];
            for (auto& ch : low)
                if (ch >= 'A' && ch <= 'Z') ch = (char)(ch - 'A' + 'a');
            for (size_t f = 0; f < kFamilies.size(); ++f)
                for (const auto& m : kFamilies[f]) {
                    // de/shes are usually the demonstrative / the verb "to
                    // know" — never blank them (they stay as options)
                    if (m == "de" || m == "shes") continue;
                    if (low == m) cands.push_back({i, (int)f});
                }
        }
        if (cands.empty()) continue;
        const Cand pick = cands[rng() % cands.size()];
        ParticleDrill d;
        d.segment = seg;
        d.tokens = toks;
        d.blank_tok = pick.tok;
        std::string actual = toks[pick.tok];
        for (auto& ch : actual)
            if (ch >= 'A' && ch <= 'Z') ch = (char)(ch - 'A' + 'a');
        d.tokens[pick.tok] = "▢";
        // targeted: only serve the family the learner is weak in
        if (aiming && !kFamilies[pick.family].empty() &&
            kFamilies[pick.family][0] + "-family" != target.skill)
            continue;
        d.options = kFamilies[pick.family];
        for (size_t i = 0; i < d.options.size(); ++i)
            if (d.options[i] == actual) d.correct = (int)i;
        // agreement-table explanation against the preceding syllable
        std::string up = actual;
        for (auto& ch : up)
            if (ch >= 'a' && ch <= 'z') ch = (char)(ch - 'a' + 'A');
        auto ag = checkAgreement(toks[pick.tok - 1], up);
        switch (ag.verdict) {
            case Agreement::Agrees:
                d.explanation = "the tables call for \"" + ag.expected +
                                "\" after " + toks[pick.tok - 1] +
                                " — and that is what the text has";
                break;
            case Agreement::DaDragPossible:
                d.explanation = "the tables call for \"" + ag.expected +
                                "\" — this stem carries historical da-drag, "
                                "so the da-class variant is correct here";
                break;
            case Agreement::Disagrees:
                d.explanation = "the tables call for \"" + ag.expected +
                                "\", but the source text itself uses \"" +
                                actual + "\" — an attested variant";
                break;
            default:
                d.explanation = "this form is not suffix-selected; the text "
                                "uses \"" + actual + "\"";
                break;
        }
        return d;
    }
    return std::nullopt;
}

std::optional<BoundaryDrill> DrillFactory::makeBoundary(
    std::mt19937& rng, bool want_unpunctuated) const {
    for (int attempt = 0; attempt < kAttempts; ++attempt) {
        auto seg = randomSegment(rng);
        if (!seg.id) continue;
        auto doc = buildOverlay(spine_, index_, seg.acip);
        if (doc.tokens.size() < 6 || doc.tokens.size() > 40) continue;

        // The pool is a property of the SOURCE, so it is read before any
        // stripping: a barrier anywhere but the very end means the scribe
        // marked a boundary inside this segment.
        bool punctuated = false;
        for (size_t i = 0; i + 1 < doc.barrier_after.size(); ++i)
            if (doc.barrier_after[i]) { punctuated = true; break; }
        if (punctuated != !want_unpunctuated) continue;

        std::vector<int> merged;
        auto clauses = refineClauses(
            doc, splitClauses(doc.tokens, doc.barrier_after), &merged);
        // One clause means there is nothing to find. Every token a boundary
        // means the question is not a question either.
        if (clauses.size() < 2 || clauses.size() > doc.tokens.size() / 2)
            continue;

        BoundaryDrill d;
        d.segment = std::move(seg);
        d.tokens = doc.tokens;
        d.punctuated = punctuated;
        for (size_t i = 0; i + 1 < clauses.size(); ++i) {   // not the final end
            d.ends.push_back(clauses[i].end - 1);
            d.attested.push_back(clauses[i].boundary == "barrier");
            const std::string fn = clauses[i].boundary_function
                                       ? clauses[i].boundary_function : "";
            d.functions.push_back(
                clauses[i].boundary == "barrier"
                    ? "punctuation"
                    : (fn.empty() ? clauses[i].boundary
                                  : clauses[i].boundary + " \u2014 " + fn));
        }
        if (d.ends.empty()) continue;
        // A merged na that coincides with a real end is scored; only the ones
        // that are NOT in the key are the unknowable ones.
        for (int m : merged)
            if (std::find(d.ends.begin(), d.ends.end(), m) == d.ends.end())
                d.unscored.push_back(m);
        return d;
    }
    return std::nullopt;
}

BlankSplit placeBlank(const std::string& seg,
                      const std::vector<std::string>& chunks,
                      const std::string& answer) {
    BlankSplit r;
    if (seg.empty() || answer.empty()) {
        r.why = "the segment or the answer is empty";
        return r;
    }
    // Start from the clause's own position, so a phrase that repeats earlier
    // in the segment cannot capture the blank.
    std::string prefix;
    for (const auto& c : chunks) {
        if (c == "[ ... ]") break;
        if (!prefix.empty()) prefix += " ";
        prefix += c;
    }
    size_t from = 0;
    if (!prefix.empty()) {
        const size_t p = seg.find(prefix);
        if (p != std::string::npos) from = p;
    }
    size_t at = seg.find(answer, from);
    if (at == std::string::npos) at = seg.find(answer);
    if (at == std::string::npos) {
        r.why = "the answer does not appear in the segment verbatim";
        return r;
    }
    r.ok = true;
    r.before = seg.substr(0, at);
    r.after = seg.substr(at + answer.size());
    return r;
}

}  // namespace allcore
