// storeflush_smoke — SQA FAIL-1 (2026-08-23, critical).
//
// GlossaryStore, DossierStore and CommentStore ended save() with
// `return (bool)f;` (or an `if (!f)` placed before any flush). A
// C++ ofstream does not set failbit until the buffer is pushed, so a
// SMALL store — one that never fills the buffer — reported success
// having written nothing. Measured by the assessment's verifier:
//
//   GlossaryStore rows=50  save()=TRUE  landed=64 of 2842  reload=0
//
// Why the existing tests could not see it, and why this one is shaped
// as it is:
//
//   * An unwritable DIRECTORY is caught by the `if (!f) return false;`
//     at the TOP of save(), which was always correct. A chmod'd-folder
//     test therefore passes with and without the fix and proves
//     nothing — shortwrite_probe's header records that trap being
//     walked into already.
//   * The failure has to happen AFTER a successful open. RLIMIT_FSIZE
//     with SIGXFSZ ignored makes write(2) fail with EFBIG exactly as a
//     full disk does, in process, leaving the file truncated
//     mid-record (the technique app/main.cpp:28694 already uses).
//   * The baseline assessment probed these stores at 600 rows — ABOVE
//     the stream buffer, the one regime where the broken shape works —
//     recorded them honest, and so the remediation excluded them. The
//     row counts here are deliberately SMALL for that reason.
//
// The drill asserts BOTH directions. A save that can never succeed
// would pass the failure half while proving nothing, so each store is
// first saved normally and required to report TRUE.
#include <csignal>
#include <cstdio>
#include <filesystem>
#include <string>
#include <sys/resource.h>

#include "allcore/comments.h"
#include "allcore/dossier.h"
#include "allcore/glossary.h"

static int fails = 0;

static void check(bool ok, const char* what) {
    std::printf("  [%s] %s\n", ok ? "PASS" : "FAIL", what);
    if (!ok) ++fails;
}

// Run fn() with the file-size limit lowered. Returns false if the
// limit could not be applied at all, so the caller can say so rather
// than report a green it did not earn.
template <typename F>
static bool underFileSizeLimit(rlim_t bytes, F&& fn) {
    struct rlimit oldLim {};
    if (getrlimit(RLIMIT_FSIZE, &oldLim) != 0) return false;
    auto* prevH = signal(SIGXFSZ, SIG_IGN);
    struct rlimit lim = oldLim;
    lim.rlim_cur = bytes;
    const bool applied = setrlimit(RLIMIT_FSIZE, &lim) == 0;
    if (applied) fn();
    (void)setrlimit(RLIMIT_FSIZE, &oldLim);
    (void)signal(SIGXFSZ, prevH);
    return applied;
}

int main() {
    namespace fs = std::filesystem;
    const fs::path dir = fs::temp_directory_path() / "all_storeflush";
    std::error_code ec;
    fs::remove_all(dir, ec);
    fs::create_directories(dir, ec);

    std::printf("storeflush_smoke (FAIL-1)\n");

    // ---- GlossaryStore ------------------------------------------
    {
        const std::string p = (dir / "glossary.tsv").string();
        allcore::GlossaryStore g(p);
        for (int i = 0; i < 50; ++i)
            g.set("dge ba " + std::to_string(i), "virtue");
        check(g.save("selftest-doc"),
              "glossary: a normal save reports TRUE "
              "(control - a save that never works proves nothing)");

        bool claimed = true;
        const bool armed = underFileSizeLimit(64, [&] {
            claimed = g.save("selftest-doc");
        });
        check(armed, "glossary: short-write drill armed (RLIMIT_FSIZE)");
        if (armed) {
            check(!claimed,
                  "glossary: save() reports FALSE when the write was "
                  "lost (FAIL-1)");
            allcore::GlossaryStore back(p);
            (void)back.load();
            check(back.all().empty(),
                  "glossary: and the rows really did not land - the "
                  "drill tests a lost write, not a slow one");
        }
    }

    // ---- DossierStore -------------------------------------------
    {
        const std::string d = (dir / "dos").string();
        fs::create_directories(d, ec);
        allcore::DossierStore ds(d);
        for (int i = 0; i < 6; ++i)
            (void)ds.create("desk " + std::to_string(i),
                            "/tmp/text.txt", i + 1, "2026-08-23");
        check(ds.save(),
              "dossier: a normal save reports TRUE (control)");

        bool claimed = true;
        const bool armed =
            underFileSizeLimit(64, [&] { claimed = ds.save(); });
        check(armed, "dossier: short-write drill armed");
        if (armed)
            check(!claimed,
                  "dossier: save() reports FALSE when the write was "
                  "lost (FAIL-1)");
    }

    // ---- CommentStore -------------------------------------------
    // comments.h promises the record is "append-only; never
    // rewritten", and the store is explicitly meant for a shared
    // Dropbox folder - a disconnected share is squarely in scope.
    {
        const std::string c = (dir / "com").string();
        fs::create_directories(c, ec);
        allcore::CommentStore cs(c);
        check(cs.add("text.txt", 1, "selftest", "a first remark",
                     "2026-08-23T00:00:00"),
              "comments: a normal add reports TRUE (control)");

        bool claimed = true;
        const bool armed = underFileSizeLimit(64, [&] {
            claimed = cs.add("text.txt", 2, "selftest",
                             "a remark long enough to overrun the "
                             "lowered file-size limit outright",
                             "2026-08-23T00:00:01");
        });
        check(armed, "comments: short-write drill armed");
        if (armed)
            check(!claimed,
                  "comments: add() reports FALSE when the write was "
                  "lost (FAIL-1)");
    }

    fs::remove_all(dir, ec);
    std::printf("%s (%d failure(s))\n", fails ? "FAILED" : "ok", fails);
    return fails ? 1 : 0;
}
