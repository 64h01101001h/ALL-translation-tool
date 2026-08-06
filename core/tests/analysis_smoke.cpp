// analysis_smoke — offline proof of the Analysis pre-pass + prompt assembly
// on the real passage [598] (Khedrup Je, determinate time). No API call.
#include <cstdio>
#include <string>

#include "allcore/analysis.h"
#include "allcore/spine.h"

static int failures = 0;
#define CHECK(cond, msg)                                     \
    do {                                                     \
        if (cond) std::printf("  [PASS] %s\n", msg);         \
        else { std::printf("  [FAIL] %s\n", msg); ++failures; } \
    } while (0)

static const char* kPassage =
    "[598]\n"
    "SNGA DRO'I KA BA PHYI DRO MED PA DUS MA 'DRES PA'I DON YIN GYI, "
    "KA BA SNGA DRO YOD NA PHYI DRO MED DGOS PA DUS CHA MA 'DRES PA'I DON MA YIN LA,";

int main(int argc, char** argv) {
    if (argc < 3) {
        std::fprintf(stderr, "usage: analysis_smoke <spine.db> <template.md>\n");
        return 2;
    }
    allcore::Spine spine(argv[1]);

    auto toks = allcore::tokenizeAcip(kPassage);
    CHECK(!toks.empty() && toks[0] == "SNGA", "tokenizer drops [598], first token SNGA");
    bool no_comma = true;
    for (auto& t : toks) no_comma &= (t.find(',') == std::string::npos);
    CHECK(no_comma, "commas stripped from tokens");

    auto pre = allcore::runPrePass(spine, kPassage);
    std::printf("  anchors: %zu, corpus hits: %zu\n", pre.anchors.size(),
                pre.corpus_hits.size());

    auto has_anchor = [&](const std::string& wylie) {
        for (const auto& a : pre.anchors)
            if (a.wylie == wylie) return true;
        return false;
    };
    CHECK(has_anchor("ka ba"), "anchor: ka ba (pillar)");
    CHECK(has_anchor("snga dro"), "anchor: snga dro (morning)");
    CHECK(has_anchor("dus"), "anchor: dus (time)");
    CHECK(has_anchor("yin"), "anchor: yin");

    bool gloss_ok = false;
    for (const auto& a : pre.anchors)
        if (a.wylie == "ka ba")
            for (const auto& g : a.hgm_gloss) gloss_ok |= (g == "pillar");
    CHECK(gloss_ok, "ka ba carries HGM gloss 'pillar' from the spine");

    auto prompt = allcore::buildAnalysisPrompt(argv[2], pre, kPassage, "");
    CHECK(prompt.system.find("Tibetan Passage Analysis") != std::string::npos,
          "system prompt carries the template");
    CHECK(prompt.system.find("How the app uses this template") == std::string::npos,
          "implementation preamble NOT sent to the model");
    CHECK(prompt.user.find("HGM term anchors") != std::string::npos &&
              prompt.user.find("pillar") != std::string::npos,
          "user prompt injects verified anchors");
    CHECK(prompt.user.find("<<INPUT START>>") != std::string::npos &&
              prompt.user.find("SNGA DRO'I KA BA") != std::string::npos,
          "user prompt carries the passage");

    auto body = allcore::buildMessagesRequestJson(prompt);
    CHECK(body.find("\"model\":\"claude-opus-5\"") != std::string::npos,
          "request targets claude-opus-5");
    CHECK(body.find("\"stream\":true") != std::string::npos, "request streams");
    CHECK(body.find("cache_control") != std::string::npos,
          "template block is cache-marked");
    CHECK(body.find("\"fallbacks\":\"default\"") != std::string::npos,
          "server-side fallbacks enabled");

    // back-translation QC prompt (pedagogy item 7): diff-only instructions,
    // anchors + both texts present, never a retranslation request
    {
        auto pre = allcore::runPrePass(spine, "SANGS RGYAS KYIS CHOS BSTAN");
        auto bt = allcore::buildBackTranslationPrompt(
            pre, "SANGS RGYAS KYIS CHOS BSTAN", "The Buddha taught.");
        CHECK(bt.system.find("OMISSIONS") != std::string::npos &&
                  bt.system.find("Never retranslate") != std::string::npos,
              "back-check system prompt is diff-only");
        CHECK(bt.user.find("SANGS RGYAS KYIS") != std::string::npos &&
                  bt.user.find("The Buddha taught.") != std::string::npos &&
                  bt.user.find("term anchors") != std::string::npos,
              "back-check user prompt carries source, draft, anchors");
        auto btBody = allcore::buildMessagesRequestJson(bt);
        CHECK(btBody.find("\"stream\":true") != std::string::npos,
              "back-check request streams");
    }

    std::printf("%s (%d failures)\n", failures ? "ANALYSIS SMOKE FAILED" : "ANALYSIS SMOKE OK",
                failures);
    return failures ? 1 : 0;
}
