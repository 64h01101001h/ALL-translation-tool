#include "allcore/drills.h"

#include <algorithm>

#include "allcore/particles.h"

namespace allcore {

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

CorpusSegment DrillFactory::uniformSegment(std::mt19937& rng) const {
    const long long maxId = spine_.corpusMaxId();
    for (int i = 0; i < 8; ++i) {
        const long long id = 1 + (long long)(rng() % (unsigned long)maxId);
        auto seg = spine_.corpusSegmentById(id);
        if (seg.id && !seg.acip.empty() && !seg.english.empty()) return seg;
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
            if (seg.id && !seg.acip.empty() && !seg.english.empty())
                return seg;
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

std::optional<ClozeDrill> DrillFactory::makeCloze(std::mt19937& rng) const {
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
        const std::string answer = chunkText(doc, picked->chunks[blank]);
        // distractors: marked chunks from other random segments
        std::vector<std::string> distractors;
        for (int tries = 0; tries < 30 && distractors.size() < 3; ++tries) {
            auto other = randomSegment(rng);
            if (!other.id || other.id == seg.id) continue;
            auto odoc = buildOverlay(spine_, index_, other.acip);
            auto ocls = refineClauses(
                odoc, splitClauses(odoc.tokens, odoc.barrier_after));
            auto opick = pickClause(odoc, ocls, 2, 7);
            if (!opick) continue;
            for (const auto& c : opick->chunks) {
                if (c.marker.empty()) continue;
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
    std::mt19937& rng) const {
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

}  // namespace allcore
