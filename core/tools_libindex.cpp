// libindex_cli — build/update the Library search index headless.
// Usage: libindex_cli <index.db> <library-root>
// Same engine as the app's "Update search index" button; useful for
// scripted installs (e.g. after unpacking a collection).
#include <cstdio>
#include "allcore/libindex.h"
#include "allcore/searchnorm.h"

int main(int argc, char** argv) {
    if (argc < 3) {
        std::fprintf(stderr,
                     "usage: libindex_cli <index.db> <library-root> "
                     "[verb_lemmas.tsv]\n");
        return 2;
    }
    if (argc > 3) allcore::configureLemmaFold(argv[3]);
    allcore::LibraryIndex ix(argv[1]);
    auto st = ix.update(argv[2]);
    std::printf("added=%d updated=%d unchanged=%d removed=%d "
                "files=%lld lines=%lld\n",
                st.added, st.updated, st.unchanged, st.removed,
                (long long)ix.fileCount(), (long long)ix.lineCount());
    return 0;
}
