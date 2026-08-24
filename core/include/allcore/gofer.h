// allcore/gofer.h — Gofer-style proximity search over the corpus (roadmap
// item 2). The SEMANTICS of Ted Lemon's Gofer rewrite (lex.c/expr.c/near.c),
// not the C code: quoted or bare terms combined with OR and NEAR-within-N-lines,
// evaluated over corpus segments where `seq` is the line number within a course.
//
// Grammar:   query   := orExpr
//            orExpr  := nearExpr ( OR nearExpr )*
//            nearExpr:= term ( NEAR/<n> term )*        (n = lines, default 3)
//            term    := "quoted phrase" | bare-word | ( query )
// Examples:  bden pa                       — phrase (bare words join)
//            stong OR bden                 — either term
//            "sdug bsngal" NEAR/5 "bden pa" — both within 5 lines, same course
#pragma once

#include <string>
#include <vector>

#include "allcore/spine.h"

namespace allcore {

struct GoferHit {
    std::string course;
    int seq_lo = 0;                        // matched line window, inclusive
    int seq_hi = 0;
    std::vector<CorpusSegment> window;     // hydrated segments in the window
};

// Parse and evaluate; throws std::runtime_error on malformed queries.
// `course` empty = all courses. Hits are sorted by (course, seq_lo).
std::vector<GoferHit> goferSearch(const Spine& spine, const std::string& query,
                                  const std::string& course = "",
                                  int limit = 60);

// The same grammar over a directory tree of raw ACIP text files (.txt, .acip,
// .md; ≤10 MB each) — Gofer's original habitat. Term matching is
// case-insensitive substring per line; NEAR/N = within N file lines, never
// across files. line_lo/line_hi are 1-based.
struct FileGoferHit {
    std::string file;                  // path relative to root
    int line_lo = 0;
    int line_hi = 0;
    std::vector<std::string> lines;    // the matched window's text
};
// SQA PERF-1 (2026-08-23). The unindexed search now says what it
// actually did. Before this, NEAR was an uncapped cross product
// (measured: 115,589 ms, 23.5 GB peak RSS on three ordinary words) and
// the file walk stopped at 4,000 of 8,988 files in silence - so a rare
// phrase in the unread 4,988 came back as "no matches", turning a
// performance cap into a claim about the library. Both are bounded
// now, and both report. Window cap matches LibraryIndex::kScanCap
// (libindex.h:76) so the two evaluators cannot drift apart again.
struct GoferScan {
    int file_cap = 4000;        // in:  files READ before the walk stops
    bool cut = false;           // out: a node hit the window cap
    int files_scanned = 0;      // out: files actually read
    int files_skipped = 0;      // out: eligible files left unread
};

std::vector<FileGoferHit> goferSearchFiles(const std::string& root_dir,
                                           const std::string& query,
                                           int limit = 60,
                                           GoferScan* scan = nullptr);

}  // namespace allcore
