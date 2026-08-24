#include "allcore/comments.h"

#include <filesystem>
#include <fstream>
#include <sstream>

namespace fs = std::filesystem;

namespace allcore {

namespace {
// same single-line TSV escaping contract as the proposals store
std::string esc(const std::string& s) {
    std::string o;
    for (char c : s) {
        if (c == '\t') o += "\\t";
        else if (c == '\n') o += "\\n";
        else if (c == '\\') o += "\\\\";
        else o += c;
    }
    return o;
}
std::string unesc(const std::string& s) {
    std::string o;
    for (size_t i = 0; i < s.size(); ++i) {
        if (s[i] == '\\' && i + 1 < s.size()) {
            const char n = s[++i];
            o += n == 't' ? '\t' : n == 'n' ? '\n' : n;
        } else {
            o += s[i];
        }
    }
    return o;
}
std::vector<std::string> splitTabs(const std::string& line) {
    std::vector<std::string> cols;
    std::string cur;
    for (char c : line) {
        if (c == '\t') { cols.push_back(cur); cur.clear(); }
        else cur += c;
    }
    cols.push_back(cur);
    return cols;
}
bool parseFile(const std::string& path,
               std::vector<TextComment>& out) {
    std::ifstream f(path);
    if (!f) return false;
    std::string line;
    while (std::getline(f, line)) {
        if (!line.empty() && line.back() == '\r') line.pop_back();
        if (line.empty() || line[0] == '#') continue;
        auto c = splitTabs(line);
        if (c.size() < 6) continue;
        TextComment t;
        t.id = unesc(c[0]);
        t.file = unesc(c[1]);
        t.line = std::atoi(c[2].c_str());
        t.author = unesc(c[3]);
        t.created = unesc(c[4]);
        t.text = unesc(c[5]);
        out.push_back(std::move(t));
    }
    return true;
}
}  // namespace

CommentStore::CommentStore(const std::string& dir) : dir_(dir) {}

bool CommentStore::load() {
    items_.clear();
    conflictFiles_ = 0;
    const bool ok = parseFile(dir_ + "/comments.tsv", items_);
    std::error_code ec;
    for (const auto& e : fs::directory_iterator(dir_, ec)) {
        const std::string name = e.path().filename().string();
        if (name.rfind("comments", 0) != 0 ||
            name.find("conflicted copy") == std::string::npos ||
            name.size() < 4 ||
            name.substr(name.size() - 4) != ".tsv")
            continue;
        std::vector<TextComment> rows;
        if (!parseFile(e.path().string(), rows)) continue;
        ++conflictFiles_;
        for (auto& r : rows) {
            bool have = false;
            for (const auto& p : items_)
                if (p.id == r.id) { have = true; break; }
            if (!have) items_.push_back(std::move(r));
        }
    }
    return ok;
}

bool CommentStore::add(const std::string& file, int line,
                       const std::string& author,
                       const std::string& text,
                       const std::string& createdIso) {
    std::ostringstream id;
    id << file << "|" << line << "|" << author << "|" << createdIso
       << "|" << (items_.size() + 1);
    const bool fresh = !fs::exists(dir_ + "/comments.tsv");
    std::ofstream f(dir_ + "/comments.tsv", std::ios::app);
    if (!f) return false;
    if (fresh)
        f << "# team comments - id, file, line, author, created, "
             "text (append-only; never rewritten)\n";
    f << esc(id.str()) << '\t' << esc(file) << '\t' << line << '\t'
      << esc(author) << '\t' << esc(createdIso) << '\t' << esc(text)
      << '\n';
    // SQA FAIL-1 (2026-08-23): this `if (!f)` sat before any flush,
    // and an ofstream does not set failbit until the buffer is
    // pushed - so a single appended comment, far too small to fill
    // it, reported success having written nothing. This store is
    // explicitly for a shared Dropbox folder (comments.h:4-10), so a
    // disconnected share is squarely in scope, and the header
    // promises the record is "append-only; never rewritten".
    //
    // Judge the write BEFORE the in-memory push: a comment that did
    // not reach disk must not sit in items_ looking saved for the
    // rest of the session. Pinned by core/tests/storeflush_smoke.cpp.
    f.flush();
    f.close();
    if (f.fail()) return false;
    TextComment t;
    t.id = id.str();
    t.file = file;
    t.line = line;
    t.author = author;
    t.created = createdIso;
    t.text = text;
    items_.push_back(std::move(t));
    return true;
}

std::vector<const TextComment*> CommentStore::byFile(
    const std::string& file) const {
    std::vector<const TextComment*> out;
    for (const auto& c : items_)
        if (c.file == file) out.push_back(&c);
    return out;
}

}  // namespace allcore
