// allcore/analysis.h — the Analysis pane's engine pre-pass and prompt assembly.
//
// Pipeline stage 1 of docs/analysis/PASSAGE_ANALYSIS_TEMPLATE.md: before the
// model runs, the passage is matched against the spine so HGM term anchors
// (rule 1: never model-composed) and corpus concordance hits are INJECTED into
// the prompt. Assembly is offline and unit-tested; the API call is stage 2.
#pragma once

#include <string>
#include <vector>

#include "allcore/spine.h"

namespace allcore {

// One dictionary anchor found in the passage (longest ACIP match wins;
// nested shorter matches are also reported — the template's items 8/9 want
// both the phrase and its parts).
struct TermAnchor {
    std::string acip;                    // as matched, spine orthography
    std::string wylie;
    std::string tibetan;
    std::string pronunciation;
    std::vector<std::string> hgm_gloss;  // binding layer, verbatim
    std::string tier;                    // curated | glossary | auto-aligned | ""
    int beg_tok = 0;                     // token span in the passage
    int end_tok = 0;                     // exclusive
};

struct CorpusHit {
    std::string course;
    int seq = 0;
    std::string wylie;
    std::string english;
};

struct AnalysisPrePass {
    std::vector<std::string> tokens;     // normalized ACIP syllable tokens
    std::vector<TermAnchor> anchors;     // all matches, longest-first per position
    std::vector<CorpusHit> corpus_hits;  // passages HGM translated that overlap
};

// Normalize raw ACIP input: straighten curly apostrophes, drop bracketed
// folio markers like [598], and split on whitespace/commas/shad markers.
std::vector<std::string> tokenizeAcip(const std::string& raw);

// Run the engine pre-pass against the spine. max_phrase_syllables caps the
// greedy longest-match scan; corpus_limit caps concordance hits.
AnalysisPrePass runPrePass(const Spine& spine, const std::string& acip_passage,
                           int max_phrase_syllables = 14, int corpus_limit = 6);

// Assemble the two prompt halves. `system` = the verbatim template (loaded
// from template_path, the canonical file) — stable, cache-friendly.
// `user` = the engine-verified anchors + corpus hits + the passage/draft.
struct AssembledPrompt {
    std::string system;
    std::string user;
};
AssembledPrompt buildAnalysisPrompt(const std::string& template_path,
                                    const AnalysisPrePass& pre,
                                    const std::string& acip_passage,
                                    const std::string& draft_english);

// Back-translation QC (docs/PEDAGOGY_ROADMAP.md item 7): a coverage diff of
// an English draft against its Tibetan source — omissions, additions, and
// anchor mismatches only. The model is instructed to DIFF, never to
// retranslate or rewrite; output is always presented as AI, and the term
// anchors it diffs against are engine-verified HGM data (rule 1).
AssembledPrompt buildBackTranslationPrompt(const AnalysisPrePass& pre,
                                           const std::string& acip_source,
                                           const std::string& english_draft);

// Serialize a complete /v1/messages request body (Claude API, streaming,
// prompt-cached system block, server-side fallbacks enabled).
std::string buildMessagesRequestJson(const AssembledPrompt& prompt,
                                     const std::string& model = "claude-opus-5",
                                     int max_tokens = 32000);

std::string jsonEscape(const std::string& s);

// Extract the text payload from one SSE `data:` JSON line if it is a
// content_block_delta with a text_delta; returns "" otherwise. Minimal,
// dependency-free — both the CLI and the GUI stream through this.
std::string sseTextDelta(const std::string& sse_data_json);

}  // namespace allcore
