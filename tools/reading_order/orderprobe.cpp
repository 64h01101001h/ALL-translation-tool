// orderprobe — expose the SHIPPING reading-order engine over spans on stdin.
//
// v2: walk EVERY clause, not just the first. v1 processed cls[0] only, which
// made 523 of 730 alignment spans unmappable — any child sitting in a second
// clause had no chunk to map onto, and the measurement's middle rows rested on
// 57 spans because of a limitation in the probe rather than in the data.
//
// Output per line:  id \t c<clause>:<order>|<chunk text> \t ...
// The predicted global order is (clause index, plan order): clause order is
// written order, which the measurement says is right 95% of the time, and the
// engine's ruling applies only inside a clause.
#include <cstdio>
#include <iostream>
#include <string>
#include <vector>
#include "allcore/engines.h"
#include "allcore/lattice.h"
#include "allcore/reader.h"
#include "allcore/spine.h"

int main(int argc, char** argv) {
    if (argc < 2) { std::fprintf(stderr, "usage: orderprobe <spine.db>\n"); return 2; }
    allcore::Spine spine(argv[1]);
    std::string line;
    while (std::getline(std::cin, line)) {
        if (line.empty()) continue;
        auto tab = line.find('\t');
        if (tab == std::string::npos) continue;
        std::string id = line.substr(0, tab), wylie = line.substr(tab + 1);
        auto doc = allcore::buildOverlay(spine, allcore::ewtsToAcip(wylie));
        auto cls = allcore::splitClauses(doc.tokens, doc.barrier_after);
        cls = allcore::refineClauses(doc, cls);
        std::printf("%s", id.c_str());
        if (cls.empty()) { std::printf("\tNOCLAUSE\n"); continue; }
        for (size_t ci = 0; ci < cls.size(); ++ci) {
            auto chunks = allcore::chunkClause(doc, cls[ci]);
            auto plan = allcore::planReading(chunks, allcore::spotVerb(doc, chunks));
            for (size_t i = 0; i < chunks.size() && i < plan.size(); ++i) {
                std::string txt;
                for (int t = chunks[i].beg; t < chunks[i].end && t < (int)doc.tokens.size(); ++t)
                    txt += (txt.empty() ? "" : " ") + doc.tokens[t];
                if (txt.empty()) continue;
                std::printf("\tc%zu:%d|%s", ci, plan[i].order, txt.c_str());
            }
        }
        std::printf("\n");
    }
    return 0;
}
