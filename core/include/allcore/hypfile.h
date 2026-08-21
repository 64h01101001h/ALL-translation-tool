// allcore/hypfile.h — Hypercontext .hyp file parser (the legacy import
// half of the Align pane; docs/ACIP_DEV_FOLDER_SURVEY.md).
//
// Format, pinned from Adam's own tutorial.hyp (hypercontext-master):
//   <t> / <e>   switch script mode (Tibetan ACIP / English)
//   <t$> <e$>   literal escapes — render as the text "<t>" / "<e>"
//   <fs…>       font-size styling — stripped
//   <N …>       numbered link open (digits, then a space); closed by
//               the next unmatched '>'. The same N appears once in a
//               Tibetan region and once in an English region — that
//               pair IS the alignment. Links nest (sublinks).
//   <!…>        a note (annotation) — collected, not part of the text
// Unknown tag-like sequences pass through literally and are counted,
// never silently dropped (rule 3).
#pragma once

#include <string>
#include <vector>

namespace allcore {

struct HypLink {
    int id = 0;
    // byte ranges into HypFile::tibetan / english; -1 = side missing
    int tibBeg = -1, tibEnd = -1;
    int engBeg = -1, engEnd = -1;
};

struct HypFile {
    std::string tibetan;             // concatenated <t> runs (ACIP)
    std::string english;             // concatenated <e> runs
    std::vector<HypLink> links;      // ids present on BOTH sides
    std::vector<HypLink> onesided;   // ids missing a side (surfaced)
    std::vector<std::string> notes;  // <!…> annotations, in order
    int unknownTags = 0;             // tag-like text passed through
};

HypFile parseHypFile(const std::string& raw);

}  // namespace allcore
