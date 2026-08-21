// dossier_smoke — TDD RED first: the Translation Dossier store.
// A dossier is a translator's desk for ONE text: it POINTS at the
// source, remembers the position, and carries stamps — it never
// duplicates data. Local store (a desk, not the shared record).
// Self-cleaning at entry and exit (constitution R6).
#include <cstdio>
#include <filesystem>
#include <string>

#include "allcore/dossier.h"

namespace fs = std::filesystem;

static int failures = 0;
#define CHECK(cond, what)                                   \
    do {                                                    \
        const bool ok = (cond);                             \
        std::printf("  [%s] %s\n", ok ? "PASS" : "FAIL",    \
                    what);                                  \
        if (!ok) ++failures;                                \
    } while (0)

int main() {
    const fs::path dir =
        fs::temp_directory_path() / "all_dossier_smoke";
    std::error_code ec;
    fs::remove_all(dir, ec);
    fs::create_directories(dir);
    const std::string d = dir.string();

    {
        allcore::DossierStore st(d);
        st.load();
        CHECK(st.all().empty(), "fresh store is empty");
        const std::string slug = st.create(
            "Diamond Cutter", "/lib/S0134I.txt", 1,
            "2026-08-21T14:00");
        CHECK(!slug.empty(), "create returns a slug");
        CHECK(st.all().size() == 1, "one dossier exists");
        CHECK(st.save(), "store saves");
    }
    {
        allcore::DossierStore st(d);
        CHECK(st.load(), "store reloads");
        CHECK(st.all().size() == 1, "dossier round-trips");
        const auto& x = st.all().front();
        CHECK(x.title == "Diamond Cutter" &&
                  x.textPath == "/lib/S0134I.txt" && x.line == 1,
              "fields round-trip");
        CHECK(st.touch(x.slug, 451, "2026-08-21T15:30"),
              "touch updates position + stamp");
        CHECK(st.save(), "touched store saves");
    }
    {
        allcore::DossierStore st(d);
        st.load();
        CHECK(st.all().front().line == 451 &&
                  st.all().front().touched == "2026-08-21T15:30",
              "touch persisted");
        // a second dossier; ordering = most recently touched first
        st.create("Sungbum vol 2", "/lib/S5275.txt", 1,
                  "2026-08-21T16:00");
        st.save();
    }
    {
        allcore::DossierStore st(d);
        st.load();
        CHECK(st.all().size() == 2 &&
                  st.all().front().title == "Sungbum vol 2",
              "most-recently-touched orders first");
        // titles with tabs/newlines survive escaping
        st.create("weird\ttitle\nhere", "/x.txt", 3,
                  "2026-08-21T17:00");
        st.save();
    }
    {
        allcore::DossierStore st(d);
        st.load();
        CHECK(st.all().size() == 3 &&
                  st.all().front().title.find('\t') !=
                      std::string::npos,
              "escaping round-trips");
        CHECK(st.remove(st.all().front().slug), "remove works");
        st.save();
    }
    {
        allcore::DossierStore st(d);
        st.load();
        CHECK(st.all().size() == 2, "removal persisted");
        CHECK(!st.touch("no-such-slug", 1, "2026-08-21T18:00"),
              "touching a ghost fails honestly");
    }

    fs::remove_all(dir, ec);
    std::printf("dossier_smoke: %s (%d failures)\n",
                failures ? "FAIL" : "ALL PASS", failures);
    return failures ? 1 : 0;
}
