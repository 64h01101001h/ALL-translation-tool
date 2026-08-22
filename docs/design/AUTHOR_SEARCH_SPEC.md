# Author search — the specification

*Produced 2026-08-22 by an 8-agent design workflow: four parallel
readers over the integration surface, three independent UX proposals,
one judging synthesis. Backlog #33. Mockups published for Adam's
ruling; the six open questions at the end are his to answer, and the
recommendations beside them are the defaults taken if he does not.*

*One correction the workflow made to itself is worth keeping: all three
proposals independently keyed their index on the author STRING, and
the judge rejected all three in favour of `matchPeople` over the BDRC
person id. That was the same conclusion reached independently from the
data side while the agents were running.*

---

## Chosen base

**Proposal 1 ("Works by author — a first-class STUDY control in the Library ribbon") is the base.**

The deliverable is TEXTS. Texts live in LibraryPane, and LibraryPane is the only place in the app where both render targets this feature needs already sit side by side: `info_` (app/main.cpp:19955), the HTML surface that carries the house honesty grammar (`ux::sourceBadge`, `zoneLabel`, `openfile:` anchors), and `list_` (app/main.cpp:19944), a sortable table whose column 0 carries the path in `Qt::UserRole` and whose double-click is already wired to `logOpen(p); open_(p);`. P1 also correctly identifies that `personHtml()`'s "Their works in your Library" block (app/main.cpp:20944-20991) is this feature already half-built — same data path, capped at 10, labelled by bare work key, reachable only after you select a text by that author. The work is to lift the cap, give the links real titles, and add a front door.

P1 additionally wins on the smallest-surface test: no new class, no new dialog, no new `stack_` page. Two widgets in an existing ribbon group, three anchor schemes in an existing handler, one lazy index.

**BUT the base is amended on its single most important technical choice — see `rejected`. P1's `authorGroups_` keyed on `allcore::authorCanonical` is deleted. The searchable unit is the PERSON (`allcore::matchPeople` over `data/extracted/author_index.json`), not the canonicalized author string.**

## Grafted from the runners-up

**From Proposal 3 (card-first) — the best honesty ideas in the three:**

1. **The route strip.** The single best idea proposed. A weak arrival must survive the navigation: landing on a ledger reached by a pronunciation fold must not launder the guess into fact. Adopted verbatim in shape (vermilion-washed banner at the top of the ledger, `MACHINE` badge, names the query and the evidence), including its two-sided discipline — an Exact arrival emits **no** strip, because an exact hit must not be slandered with a weak label either.
2. **The four-channel tier encoding** (left-rule weight *and* style, type size *and* weight, background wash on machine tiers only, `≈`/`≈?` glyph). P1 used three channels, P2 used three. Four survives greyscale, zoom, and colour-blindness with margin. P3's `AuthorTierSkin` struct is adopted as the concrete implementation.
3. **Modeless, never `exec()`** — the UI sweep (app/main.cpp:31902-31921) clicks every enabled QPushButton and a modal would hang the battery. (Moot once P3's dialog is rejected, but the rule stands for anything added later.)
4. **Per-row provenance in the table** — P3's "Source bank" column, simplified to a per-row marker.
5. **`author:` must also be added to the Lookup pane's anchor handler**, so `g_personCardByName`'s deferral line becomes live. P3 caught this; P1 and P2 did not.

**From Proposal 2 (palette-first):**

6. **⌘K as an accelerator that routes INTO the library surface.** P2 is right that the existing persons lane (app/main.cpp:29139-29157) is a live honesty regression — an unannounced Contains fold rendered at exact-hit weight, capped at the *alphabetically* first 4, matching neither ACIP nor pronunciation. It gets rewritten. But P2's own diagnosis convicts its destination: the lane returns BDRC/ToL *web links* and "answers 'who is this' and sends you to a browser", while Adam asked for texts. So the lane is rebuilt on `matchPeople` and its rows route to the Library ledger via a new hook — the palette is the doorway, the Library is the room. The two BDRC/ToL URL rows are **deleted**; those links live on the ledger.
7. **The multimap files-per-work fix.** `fileByWork_` is first-wins by directory-walk order, so 460 of 5,607 keys resolve to a `… META.TXT` sidecar, and its filter `{*.txt,*.TXT,*.acip}` excludes `.ACT/.INC/.ACE`. Uncapping a 139-work ledger multiplies that error ~14×. Adopted: one walk fills a `QMultiMap`, a `bestFile()` picker prefers a non-META text file, and the surplus is disclosed as `(+N more files)`.
8. **The lazy-guard antipattern must not propagate.** Both existing guards (`personsLoaded_` :29131, `filesIndexed_` :29209) set their flag `true` *before* the load attempt, so a missing file silently kills the lane for the session. `buildAuthorIndex()` sets its flag only after a successful read and banks the failure reason for display.
9. **Index staleness must be invalidated**, not tolerated — `installZip` (:20209) and `importFiles` (:20482) clear `authorIndexBuilt_`.

**From Proposal 1, kept and carried:**

10. Uncapped ledger; the `authortable:` handoff into the sortable `list_` with `viewBtn->setChecked(true)` kept in sync; the permanent 8th `Author (catalog)` column reading `not catalogued` rather than blank; the live-computed **Library ledger footer**; the FIND-box cross-link mitigation; the `.conflicts` disclosure for S6387; the primer/miss/empty copy in house voice.

## Rejected

**1. REJECTED IN ALL THREE: matching author STRINGS via `allcore::matchAuthors`.**

This is the ruling that reshapes the feature. Every proposal built its index by keying author strings on `allcore::authorCanonical` — P1's `authorGroups_`, P2's `authorNames_`, P3's `findableAuthors_`. All three then wrote risk sections about the collisions this causes.

`core/include/allcore/authorsearch.h` already ships the answer, and says so explicitly:

> "BDRC person id P64 carries SEVEN spellings of Tsongkhapa … Matching author STRINGS returns the same man seven times with his 161 works split between the copies, which is useless to someone asking what he wrote. So the searchable unit is the PERSON … The grouping comes from BDRC's own per-text linkage, not from name similarity, so it is evidence rather than inference."

`std::vector<PersonHit> matchPeople(const std::string&, const std::vector<PersonRef>&)` is implemented in `core/src/authorsearch.cpp`, and its backing bank `data/extracted/author_index.json` (155 KB, 462 people, built by `tools/build_author_index.py`) exists on disk right now. **`matchPeople`, `PersonRef` and `author_index.json` have zero references in `app/main.cpp`.** Verified against the live data: P64 banks 6 alias spellings of Tsongkhapa (`RJE TZ ONG KHA PA…`, `RJE TZONG KHA PA BLO BZANG GRAGS P A`, …) across 161 works; P289 6 spellings / 157 works; P423 9 spellings / 129 works. 36 of 462 people are multi-alias.

Consequences of the rejection:
- P1's canonical grouping is *name-similarity inference* and its own risk section admits it: "two genuinely distinct people who share a spelling collapse into one node." `matchPeople` cannot make that error — the pid distinguishes them.
- P2's kind-7 "⌥ also include 5 variant spellings" opt-in row is **deleted entirely**. It exists only to paper over string-splitting. The catalog's six mangled Tsongkhapa spellings are not five extra people awaiting a user's permission to merge; they are one man, and BDRC's per-text linkage already says so. Asking the user to authorize that merge is asking them to ratify a bug.
- P3's "phantom homonyms" risk (Tsongkhapa returning twice from two banks) dissolves — both banks feed one pid.
- The two-sided rule becomes statable: **two people who sound alike stay two rows (the machine must not merge); one person spelled six ways is one row (the machine must not split).** The pid is what tells the cases apart.

`matchAuthors` is not discarded — it is demoted to the one job it is right for (see spec §3b: the 442 library work keys with a catalog author name but no pid).

**2. REJECTED: Proposal 2's in-palette drill mode (kinds 5/6/7, `drillAuthor_`, `drillIncludeVariants_`, `lastQuery_`).** P2's own risk section is the case against it: entering the drill sets `box_` text, which fires `textChanged`, which must clear `drillAuthor_` or the user is trapped; forget the inverse and every later query renders the stale shelf; `openPalette()` grows from resetting three members to six. It also overloads Esc with a third meaning and produces two ledgers for one number (a capped stage-1 count beside an uncapped stage-2 list). A ⌘K palette is for *getting somewhere fast*, not for holding a 139-row bibliography.

**3. REJECTED: Proposal 3's `AuthorFinder : QDialog` and its `stack_` index 2.** A new dialog class duplicates a debounce, a query normalizer, a renderer and a `selfTest` that the ribbon line edit + `info_` already provide. Worse, P3's third `stack_` page breaks two existing readers that assume two pages — `surveySelected()` (:20756) tests `currentIndex()==1` and otherwise reads `model_->filePath(tree_->currentIndex())`, so in author mode it would survey whatever was last selected in a hidden tree; and `viewBtn`'s toggled handler (:19982) hard-sets 0/1. P3 lists both as risks it would have to mitigate. Rendering candidates *and* the ledger into `info_`, and reusing the existing `list_` for the table, costs nothing and breaks nothing.

**4. REJECTED: Proposal 2's ink-only palette ladder as the feature's primary tier display.** P2 is correct that a `QListWidget` cannot carry the badge/zone/caveat grammar and that an HTML delegate fights the palette's `::item:selected` stylesheet. But the conclusion is not "ship a second, weaker tier vocabulary in the palette" — `docs/CLOSER.md:40` forbids exactly that flattening. The conclusion is that the palette must not be the surface that ranks five tiers. It carries at most 4 rows with a plain trailing tier word and routes to the surface that does the ranking properly.

**5. REJECTED: displaying `persons_bdrc.json`'s `works` integer, or `author_index.json`'s `works_local` array, as the local count.** `works_local` is a build-time snapshot (the builder walks `library/` itself at :47-50). Presence is derived **live** from the app's own walk, because "can I open it" is a filesystem question. `works` (the banked total) supplies the "of M" denominator. The JSON's `works_local` is used for one thing only: nothing displayed — it is not read. This continues `personHtml`'s existing and correct discipline of ignoring the banked `works` integer, and avoids printing two truths.

**6. REJECTED in all three: green `#1E6B4E` and `#b00` anywhere on this surface.** All three proposals reached this independently; it is confirmed, and pinned by test.

## Specification

## AUTHOR SEARCH — implementable specification

### 0. The one-line shape
A person-scoped author finder in **LibraryPane** (Read → Library). Type a name in ACIP, wylie, Tibetan unicode, or English phonetics → a tier-ranked list of **people** (not name-strings) → click a person → their complete uncapped bibliography with live `openfile:` links → double-click opens the text in the Overlay. Entry points: ribbon STUDY group, and a rebuilt ⌘K lane that routes here.

---

### 1. PANE AND WIDGETS

All inside `class LibraryPane` (app/main.cpp:19746-21422). **No new class. No new pane. No new `stack_` page. No dialog.**

**1a. Two additions to the ribbon's STUDY group** (built at :19770-19865; STUDY holds only `surveyBtn` today at :19856):

```cpp
authorBtn_ = new QPushButton("Works by author…");
authorBtn_->setToolTip(
    "Every text in your Library by one person. Type the name in ACIP "
    "(TZONG KHA PA), in wylie (tsong kha pa), in Tibetan, or the way it "
    "sounds (\"tsongkapa\"). Names that only SOUND alike are labeled as "
    "guesses — you pick the person.");
connect(authorBtn_, &QPushButton::clicked, [this]{
    buildAuthorIndex();
    authorBox_->setFocus(); authorBox_->selectAll();
    info_->setHtml(authorPrimerHtml());
});
gStudy->addBig(authorBtn_, "people");          // miniIcon vocabulary, :3323

authorBox_ = new QLineEdit;
authorBox_->setPlaceholderText("author: ACIP, wylie, Tibetan, or how it sounds… (Enter)");
authorBox_->setMinimumWidth(240);
connect(authorBox_, &QLineEdit::returnPressed, [this]{
    showAuthorCandidates(authorBox_->text());
});
gStudy->add(authorBox_);                       // raw widget, exactly like gFind->add(search_) at :19863
```

`addBig()` reparents and **hides** the source button — `selfTest` must never assert `isVisible()`; copy the parent-chain walk at :20036-20059 looking for `objectName()=="ribbonBar"`.

`authorBtn_` is auto-enumerated into the menu-bar mirror (:30895-30935) and auto-clicked by the UI sweep (:31902-31921). Because its first click triggers the library walk, **add the literal label `"Works by author…"` to the sweep's `stallGuard` set.**

**1b. `list_` gains a permanent 8th column.** Change the header at :19944 from 7 to 8 columns, appending `"Author (catalog)"`. Filled in `fillList()` (:20154) from the person index; a work key with no person link and no catalog author reads **`not catalogued`** — never blank. Permanent, not author-mode-only: dynamic column counts break anything pinning 7, and a permanently author-sortable catalog is a second, cheaper answer to the same question.

**1c. Enter-to-search, not search-as-you-type.** `matchPeople` calls `matchAuthors` per person, which calls `authorPhoneticFold` → `pronounce()` for every alias falling past Spacing — ~500 alias folds per invocation with no cache (`core/src/authorsearch.cpp`). That does not belong on a keystroke. **Precompute the phonetic fold once per alias at index-build time** into `PersonRef` — but since `matchAuthors` recomputes internally, the v1 ruling is: **Enter only.** If profiling later shows a query under ~50 ms, add a 220 ms `QTimer` mirroring HuntPalette (:28892). No `textChanged` connection ships in v1.

**1d. New members** (append at :21400-21421):
```cpp
QPushButton* authorBtn_ = nullptr;
QLineEdit*   authorBox_ = nullptr;
bool authorIndexBuilt_ = false;
QString authorIndexError_;                          // path of a bank that failed to load
std::vector<allcore::PersonRef> people_;            // fed to matchPeople
QMap<QString, QJsonObject> personByPid_;            // pid -> author_index record
QMap<QString, allcore::PersonHit> lastHitByPid_;    // arrival tier/evidence carrier
std::vector<std::string> nameOnlyAuthors_;          // catalog authors with no pid
QMultiMap<QString, QString> filesByWork_;           // workKey -> EVERY path
QString lastAuthorQuery_;
int nWorkKeys_ = 0, nWithPid_ = 0, nNameOnly_ = 0, nUnattributed_ = 0;
```

---

### 2. THE INDEX — `buildAuthorIndex()`

Lazy, guarded by `authorIndexBuilt_`, called by `authorBtn_`, `authorBox_`'s Enter, the `g_libraryAuthorWorks` hook, and `selfTest`. It calls `loadPersons()` (:20778) first.

**One directory walk, three consumers.** Replace the walk inside `personHtml` (:20948) with a shared `ensureFileIndex()` that runs `QDirIterator(libRoot_, {"*.txt","*.TXT","*.acip","*.ACT","*.act","*.INC","*.inc","*.ACE","*.ace"}, QDir::Files, QDirIterator::Subdirectories)` — note the **widened filter**; `.ACT/.INC/.ACE` were invisible before. Key each file with the existing `QRegularExpression("^([A-Za-z]+)0*(\\d+)")` → uppercase prefix + zero-stripped number. Fill:
- `filesByWork_` — **multimap, every file**, and
- `fileByWork_` — one path per key, now chosen by `bestFile()` rather than first-wins.

```cpp
// Prefer something a reader can actually read. First-wins by walk order
// pointed 460 of 5,607 keys at a "<KEY> META.TXT" sidecar.
static QString bestFile(const QList<QString>& paths) {
    QStringList sorted = paths; sorted.sort();
    for (const QString& p : sorted) {              // pass 1: real text, not a sidecar
        const QString n = QFileInfo(p).fileName().toUpper();
        if (!n.contains(" META.") && (n.endsWith(".TXT") || n.endsWith(".ACT") ||
                                      n.endsWith(".INC") || n.endsWith(".ACE")))
            return p;
    }
    for (const QString& p : sorted)                // pass 2: anything not a sidecar
        if (!QFileInfo(p).fileName().toUpper().contains(" META.")) return p;
    return sorted.isEmpty() ? QString() : sorted.first();   // pass 3: the sidecar
}
```
This is a strict improvement to `personHtml` and keeps its existing pin at :20095 (`ph.contains("openfile:")`) passing.

**Load `data/extracted/author_index.json`.** Data root by `libRoot_.chopped(8)` (the `/library` strip, valid because of :19750). Schema, verified on disk — top level `{_meta, people}`, `people` is 462 records keyed by pid:
```
"P64": { display: "RJE TZONG KHA PA BLO BZANG GRAGS PA",
         name_recorded: true,
         aliases: ["RJE T ZONG KHA PA BLO BZANG GRAGS PA", … 6 total],
         works:       ["D12207", … 161 keys],
         works_local: [… build-time snapshot, NOT READ …],
         sources:     ["acip-person-links","persons-bdrc"],
         dates: "1357-1419",        // present on 166 of 462
         tol: "577", tol_source: "wikidata-P4138" }   // tol on 193 of 462
```
Fields `dates`, `tol`, `tol_source`, `name_recorded` are **optional** — read with `.value()`, never assume presence (36 records have `display: ""` and empty `aliases`).

Build `people_` — one `PersonRef` per record, `localWorks` filled from the **live** walk:
```cpp
for (auto it = obj.begin(); it != obj.end(); ++it) {
    const QJsonObject r = it.value().toObject();
    allcore::PersonRef pr;
    pr.pid     = it.key().toStdString();
    pr.display = r.value("display").toString().toStdString();
    for (const auto& a : r.value("aliases").toArray())
        pr.aliases.push_back(a.toString().toStdString());
    int local = 0;
    for (const auto& w : r.value("works").toArray())
        if (filesByWork_.contains(w.toString())) ++local;
    pr.localWorks = local;                    // live, never the JSON's works_local
    people_.push_back(std::move(pr));
    personByPid_[it.key()] = r;
}
```
`matchPeople` skips `aliases.empty()`, so the 36 nameless records are unreachable by search by construction — correct, and disclosed in the ledger footer rather than shown as blank rows.

**Failure is stated, never silent.** Set `authorIndexBuilt_ = true` **only after** a successful parse. On failure record `authorIndexError_ = <absolute path> + ": " + <reason>` and render it in place of results — a missing bank must never read as "no such person". This is the deliberate break from `personsLoaded_` (:29131) and `filesIndexed_` (:29209), both of which set their flag before the load attempt and silently disable themselves for the session.

**Invalidation.** `installZip` (:20209), `downloadAndInstall` (:20416) and `importFiles` (:20482) each clear `authorIndexBuilt_`, `filesByWork_`, `fileByWork_` and `people_`. "Find every text by an author" is precisely the claim staleness falsifies.

**Live ledger counts**, computed during the walk, never hardcoded (current values, measured):
`nWorkKeys_` 5,607 · `nWithPid_` 1,596 · `nNameOnly_` 442 · `nUnattributed_` 3,569.

---

### 3. MATCHING

**3a. People (the primary path).**
```cpp
const auto hits = allcore::matchPeople(query.trimmed().toStdString(), people_);
```
`matchPeople` returns **one hit per person, best tier wins**, sorted strongest tier → most local works → name. ACIP/wylie/Tibetan duality, the phonetic fold, the `kMinPhoneticFold`/`kMinContainsLen` floors and the homonym rule are all already inside the engine. The app adds no matching logic.

Populate `lastHitByPid_` from the result — this is how the arrival tier and evidence reach the ledger without inventing an anchor encoding.

**3b. Name-only attributions (the residual path).** 442 live work keys carry a `catalog_works` author name but no pid, so `matchPeople` cannot see them. After the person pass, run the string matcher over exactly that residue:
```cpp
const auto nameHits = allcore::matchAuthors(q, nameOnlyAuthors_);
```
`nameOnlyAuthors_` = the distinct `authorByWork_` values whose work keys appear in no person record. These render **in their own zone, below every person zone**, headed:

> `[REFERENCE]` **NAME ONLY · NO CATALOG PERSON RECORD · N**
> *grouped by the spelling of the name, because the catalog banks no person id for these texts. Two people who share a spelling would share a row here, and one person spelled two ways would appear twice — the rows above are grouped by BDRC's per-text linkage and do not have that weakness.*

This is the honest use of `matchAuthors`: bare strings, no pid available, and the limitation named.

---

### 4. INTERACTION — TYPING TO OPENING A TEXT

**Step 1.** User clicks STUDY → "Works by author…" (or presses Enter in `authorBox_`, or ⌘K, or clicks an author name on any text card). The button focuses the box and renders the primer into `info_`. Nothing else changes; the left pane keeps showing whatever the user was browsing.

**Step 2.** User types `tsongkapa`, presses **Enter**. `showAuthorCandidates(query)` runs `buildAuthorIndex()`, stores `lastAuthorQuery_`, runs §3a then §3b, and renders tier-grouped candidates into `info_`. **Nothing is auto-selected; the left pane is untouched.**

**The one auto-advance, and its exact rule:** if `hits.size() == 1 && hits[0].tier == AuthorTier::Exact && nameHits.empty()`, chain directly into `showAuthorWorks(hits[0].pid)`. One exactly-named person is not a choice. **Any other shape stops at the candidate list — including a single Phonetic hit**, because a phonetic match is a guess about identity and confirming it is the human's job.

**Step 3.** Each candidate row is `<a href='author:' + anchorEnc(pid) + "'>`. New branches in the `info_->anchorClicked` lambda at :19957-19976 — **there is no fallback dispatcher; a scheme not added here is dead.** Prefix lengths **include the colon**:
```cpp
} else if (s.startsWith("authortable:")) {          // len 12
    fillListForPerson(anchorPayload(s, 12));
    viewBtn->setChecked(true);                       // or the ribbon toggle lies
    stack_->setCurrentIndex(1);
} else if (s.startsWith("authorback:")) {           // len 11
    showAuthorCandidates(anchorPayload(s, 11));
} else if (s.startsWith("author:")) {               // len 7  — payload is a PID
    showAuthorWorks(anchorPayload(s, 7));
}
```
Every payload through `anchorEnc()` (:221), every handler through `anchorPayload()` (:225). Percent-encoding is the Library vocabulary; base64 belongs to the citation family (`citefile:`/`citeopen:`/`citefind:`, :9993-10005) and must not be mixed in. Pids are ASCII-safe but pass through `anchorEnc` anyway for uniformity. **Also add `author:` to the Lookup pane's anchor handler** so `g_personCardByName`'s deferral line becomes live.

**Step 4.** `showAuthorWorks(pid)` renders the ledger into `info_` (§7). Arrival tier and evidence come from `lastHitByPid_.value(pid)`; absent (a direct arrival from a text card) means Exact-grade and no route strip.

**Step 5.** Every present work is `<a href='openfile:' + anchorEnc(bestFile(...)) + "'>` labelled by `englishTitle()` (:20110), not by bare work key. Click → the existing branch at :19962-19965 → `logOpen(p); open_(p);` → the text opens in the Overlay and `g_raisePane` raises it. **`open_` (member :21408) is the injected opener — LibraryPane has no `openPath()`, that is FilesPane's (:18471) — and it must always be paired with `logOpen(p)` (:21239) or the file never reaches Recently opened.**

**Step 6 (optional).** The ledger header carries `<a href='authortable:…'>show these 139 in the sortable list →</a>`. `fillListForPerson(pid)` is `fillList()` restricted to that person's present work keys; the user gets the familiar table, sortable by catalog number, verification, language, size, English title and now Author, with double-click-to-open already wired (:19988). **It is a link and not automatic** — silently swapping the left pane out from under someone violates user control.

**Step 7.** `<a href='authorback:…'>← other candidates for "tsongkapa"</a>` sits at the top of every ledger, so drilling into one Changkya never strands you from the other.

**The ⌘K accelerator.** Rewrite HuntPalette's persons lane (:29128-29202) on `matchPeople`. Load `author_index.json` into the palette's own lazy copy. Emit **at most 4 rows, kind 5**, payload = pid:
`👤  <display>  (<dates>)  ·  139 of 161 works in your Library   —  SOUNDS LIKE`
followed by the honest overflow row when more matched (`— showing 4 of 9 people; keep typing to narrow`, kind -1, `ux::kFaint`). Tier is carried by the trailing word plus item ink/weight/slant (`Exact` `ux::kInk` bold · `Contains` `ux::kMuted` · `Spacing` `ux::kSoft` · `Phonetic` `ux::kMachine` italic · `PhoneticNear` `ux::kSoft` italic), set via `Qt::ForegroundRole`/`Qt::FontRole` — **not** an HTML delegate, which would fight the palette's `::item:selected { background:#8C2F2B }` and render grey text on dark red. Use `list_->font()` as the base, not `item->font()`. Row text must contain no markup, and must use a plain `cut()` helper, **not `ux::snip()`, which HTML-escapes** and would print literal `&#39;` in the list. The two BDRC/ToL URL rows are deleted; those links live on the ledger.

New file-scope hook beside `g_goferQuery` (:2660):
```cpp
static std::function<void(const QString& pid, int tier, const QString& evidence)>
    g_libraryAuthorWorks;
```
In `activate()` (:29236), placed before the kind-1 test to match existing order, **checking the hook before `hide()`** — the existing kind-3 branch has the defect of hiding first and then silently swallowing the activation on a null hook:
```cpp
} else if (a.kind == 5) {
    if (!g_libraryAuthorWorks) return;
    hide();
    g_libraryAuthorWorks(a.payload, tierOf(a), evidenceOf(a));
}
```
Also fix the stale `// 0=Lookup 1=Search(Gofer) 2=open URL` comment on `struct Act` (:28983) to document kinds 3, 4 and 5.

**Install the hook in LibraryPane's CONSTRUCTOR, not in `loadPersons()`.** `loadPersons()` is reachable only through `personHtml()` (:20886), which is why `g_personCardByName` and `g_personBadgeForFile` are dead on a fresh launch today. The hook body calls `buildAuthorIndex()` itself:
```cpp
g_libraryAuthorWorks = [this](const QString& pid, int tier, const QString& ev) {
    buildAuthorIndex();
    showAuthorWorks(pid, tier, ev);
    if (g_raisePane) g_raisePane(this);
};
```

**Existing callers wired the same day:** `personHtml()`'s bolded author name (:20895) becomes `<a href='author:<pid>'>` when `acipPersonLinks_[workKey]["pid"]` exists (Exact arrival, evidence `"from this text's own ACIP↔BDRC catalog link"`, no route strip); `personHtml()`'s dead-end `"… and N more"` (:20977-20981) becomes that same link; `g_personCardByName`'s deferral line (:20853-20855) becomes it too.

---

### 5. HOMONYMS AND ALIASES — the two-sided rule

**Two people who sound alike stay two rows. One person spelled six ways is one row.** The pid is what tells the cases apart, and the grouping comes from BDRC's per-text linkage — evidence, not name inference.

- **Homonyms.** `matchPeople` emits one hit per pid. `changkya` returns P209 `LCANG SKYA NGAG DBANG BLO BZANG CHOS LDAN` (1642-1714, 106 local) **and** P210 `LCANG SKYA ROL PA'I RDO RJE` (1717-1786) as two rows, each with its own pid, dates and local count. Nothing merges them, nothing ranks one as correct. The machine never picks between people who share a name (inviolable rule 3).
- **Aliases.** When `hit.matchedAlias != hit.display`, the row discloses which spelling was hit:
  > `<small>matched the spelling “RJE TZ ONG KHA PA BLO BZANG GRAGS PA” — one of 6 the catalog banks for this person</small>`
  and when `hit.aliasCount > 1` on an exact-display hit:
  > `<small>the catalog banks this person's name 6 ways; all 6 are searched and all 161 works are counted here</small>`
  The ledger repeats the full alias list under a **`ALSO BANKED AS`** sub-heading with the line: *"the same person, spelled differently in the catalog — grouped by BDRC's per-text linkage, not by this app guessing that the names look alike."*
- **The conflict record.** `acip_person_links.json`'s `.conflicts` subtree (one key, `S6387`, two competing pid/author claims — P289 Dngul chu Dharma Bhadra vs P268 Dbyangs can grub pa'i rdo rje) is **never read by the app today**, so S6387 currently shows one attribution with a green "confirmed" label and no hint a conflict exists. Read it: any work key present in `.conflicts` renders in the ledger with `<small style='color:#935800'>two competing attributions recorded — this person is one claim, not a ruling</small>`, on **both** people's ledgers.
- **The confirmed-pid green line** `(confirmed per-text link — ACIP↔BDRC catalog)` in `#2C5B2E` (:20899-20909) may appear **only** for a genuine per-text link, **never** for a search tier. Likewise the amber `N candidates — homonyms, you disambiguate:` in `#8a6d1a` (:20914-20920) — reused verbatim, since both strings are already audited house language.

---

### 6. EMPTY STATES

**6a. Primer** (button clicked, box empty):
> **Works by author**
> Type a name in ACIP (`TZONG KHA PA`), in wylie (`tsong kha pa`), in Tibetan, or the way it sounds (`"tsongkapa"`, `"gendun drup"`). Case picks the reading: ACIP is UPPERCASE, wylie lowercase. Every person the catalog banks comes back as their own row, labeled with the route it matched by. Names that only sound alike are candidates — the machine never picks between people who share a name.

**6b. Miss** — modeled on the terminal Lookup miss (:1698-1710): name every fold actually tried, then point at lower layers without promising them.
```cpp
h = ux::sourceBadge(ux::Epistemic::Reference) + zoneLabel("AUTHORS IN THE CATALOG") +
    QString("<div style='color:%1'><b>no catalogued person matches “%2”</b></div>"
            "<div style='color:%3;font-size:12px;margin-top:2px'>tried: exact name · "
            "name contains · syllable spacing · pronunciation · near-pronunciation — "
            "across %4 catalogued people (%5 spellings) and %6 name-only attributions, "
            "nothing matched. ACIP is UPPERCASE, wylie lowercase — case picks the "
            "reading; you can also spell the name the way it SOUNDS. %7 of the %8 texts "
            "in your Library carry no author in any bank, so a miss here is not evidence "
            "the author is absent from your shelf. The FIND box searches file names "
            "instead.</div>")
        .arg(ux::kMuted).arg(query.toHtmlEscaped()).arg(ux::kSoft)
        .arg(nPeople).arg(nAliases).arg((int)nameOnlyAuthors_.size())
        .arg(nUnattributed_).arg(nWorkKeys_);
```

**6c. Bank missing** (`!authorIndexError_.isEmpty()`) — never a "no match":
> `[MACHINE]` **AUTHOR BANK NOT LOADED**
> author search is unavailable because a data bank could not be read: `<path>` — `<reason>`. This is a missing file, not an empty result: nothing here says the author is absent. Read → Library → Import data release… installs the bank.

**6d. Zero rows in a tier** — the zone is omitted entirely. A zone with zero rows is noise; the ladder is named in the miss state instead.

---

### 7. AUTHOR MATCHED, NO LOCAL TEXTS — three distinct states

**This is the majority case and must be built as three states, not one.** Measured on the shipped bank: of 462 people, **127** have texts installed, **37** have banked works with none installed, and **298** have no banked works at all. 335 of 462 (72%) reach one of the two "no texts" states. A single generic message would misreport 298 people as a shelf gap when they are biography records, and would misreport 37 real gaps as nothing at all.

Every candidate row states its count up front, so the state is visible *before* the click: `· 139 of 161 works in your Library` / `· 0 of 14 in your Library` / `· biography record — no works banked`.

**(a) Works banked, some installed** — the normal ledger (§8).

**(b) Works banked (N), none installed** — a real gap in the shelf, stated as a gap with a remedy:
```cpp
h += ux::sourceBadge(ux::Epistemic::Reference) + zoneLabel("WORKS · NONE INSTALLED HERE") +
     QString(" <small style='color:%1'>0 of %2</small>").arg(ux::kFaint).arg(banked);
h += QString("<div style='font-size:12px;color:%1;margin-top:3px'>"
             "<b>%2 works are catalogued under this person. None of them are "
             "installed in your Library.</b> They are listed below with no link — "
             "there is no file to open. Shelve → <b>Get collections…</b> installs "
             "the official Kangyur, Tengyur and Sungbum releases; "
             "Shelve → <b>Install collection…</b> takes a zip you already have.</div>")
        .arg(ux::kMuted).arg(banked);
// then every banked work key, greyed #78706A, NO anchor, with englishTitle() where known
```
The work keys are still listed. Showing the titles you do not have is how the user learns what the collection would give them.

**(c) No works banked at all** — a biography record, not a search failure:
```cpp
h += ux::sourceBadge(ux::Epistemic::Reference) + zoneLabel("BIOGRAPHY RECORD · NO WORKS BANKED");
h += QString("<div style='font-size:12px;color:%1;margin-top:3px'>"
             "<b>This person is banked as a biography, not as an author.</b> "
             "The catalog links no texts to them, so there is nothing to open here "
             "and nothing is missing from your shelf. %2 of the %3 people in the "
             "author bank are biography records of this kind — they come from BDRC "
             "and the Treasury of Lives, which record people whether or not the ACIP "
             "catalog holds their works. <i>This is not a search failure, and it is "
             "not evidence that they wrote nothing.</i></div>")
        .arg(ux::kMuted).arg(nBioOnly).arg(nPeople);
// then the BDRC and Treasury of Lives links, which ARE the answer for this person
```
BDRC via `https://library.bdrc.io/show/bdr:<pid>`; Treasury of Lives via the banked `tol` through `tolBiographyUrl()` (:3108-3115, returns empty rather than guessing) falling back to `tolLinkForPid(pid, libRoot_.chopped(8))` (:3130-3157), keeping its `(matched via Wikidata)` disclosure intact.

**In all three states the "← other candidates" link and the Library ledger footer render.** No state is a dead end, and no state is silent.

---

### 8. THE LEDGER (`showAuthorWorks`)

Order, top to bottom:
1. `← other candidates for "<lastAuthorQuery_>"` (`authorback:`), when a query preceded this.
2. **Route strip** (§9) — only when arrival tier ≥ Phonetic.
3. Name at `ux::kFsLead`, dates, alias disclosure, `ALSO BANKED AS` list when `aliasCount > 1`.
4. **IDENTITY zone** — pid, BDRC link, ToL link, and `personHtml`'s existing confirmed/candidates strings verbatim.
5. **WORKS zone** — §7 (a), (b) or (c).
6. For (a): every present work, **uncapped**, `<a href='openfile:…'>` labelled `<work key> — <englishTitle()>`, plus per-row marks: `(+N more files for this work)` from `filesByWork_`; `<small style='color:#935800'>metadata sidecar</small>` when `bestFile` fell through to a `META.TXT`; `<small style='color:#935800'>two competing attributions recorded</small>` for a `.conflicts` key. Then banked-but-absent keys, greyed, unlinked, under `<b>N</b> more catalogued but not installed`.
7. `show these N in the sortable list →` (`authortable:`).
8. **Library ledger footer**, always, on every ledger and every candidate list:
```cpp
h += QString("<div style='margin-top:10px;border-top:1px solid #E2DACB;padding-top:6px;"
             "font-size:11px;color:%1'>Library ledger: %2 texts carry a catalog work key · "
             "%3 are linked to a catalogued person · %4 carry an author name but no person "
             "record · %5 carry no author at all. A text with no catalogued author cannot be "
             "found by author here, and its absence from this list is not evidence about "
             "who wrote it.</div>")
        .arg(ux::kFaint).arg(nWorkKeys_).arg(nWithPid_).arg(nNameOnly_).arg(nUnattributed_);
```
On the current library that reads **5,607 · 1,596 · 442 · 3,569**. Without it, a user who searches Tsongkhapa and gets 139 texts has no way to know that nearly two-thirds of their library is invisible to author search. **This footer is not polish; it is the line that keeps the feature from lying by omission**, and it is the Phase-3 ledger rule (`docs/UX_AUDIT.md:263`) applied to the one number that matters.

**No cap on the ledger.** Every other capped list in the app prints "N of M" and stops; this one does not stop, because the request was *all available texts*. The `personHtml` cap of 10 (:20977) is exactly what makes today's version useless for the six people with >89 local works. The 139-row page is the correct output, and `authortable:` is one click away for anyone who wants columns.

---

### 9. THE ROUTE STRIP

A weak arrival must survive the navigation. Rendered at the top of `showAuthorWorks` whenever `tier >= AuthorTier::Phonetic`:
```cpp
h += QString("<div style='background:#FBF1E6;border-left:3px solid %1;padding:5px 9px;"
             "margin:2px 0 8px 0;border-radius:4px;font-size:11px;color:#8A4A18'>%2"
             "you reached this page by a <b>pronunciation fold</b> of “%3” (%4). "
             "That is a guess about WHICH person you meant — nothing on this page "
             "confirms it. Tibetan names are homonym-dense and two different people "
             "can sound identical. Check the dates and the biography before you cite "
             "anything found this way.</div>")
        .arg(ux::kMachine)
        .arg(ux::sourceBadge(ux::Epistemic::Machine))
        .arg(lastAuthorQuery_.toHtmlEscaped())
        .arg(evidence.toHtmlEscaped());
```
An Exact, Contains or Spacing arrival emits **no strip** — the F1 discipline is two-sided (:33435-33500): weak is never rendered strong, and strong is never slandered weak.

---

### 10. NON-GOALS
No editing of author records, no merge/split UI, no writing back to the banks. The surface reads four banks and the filesystem; disagreements between them are **displayed, never resolved**. Resolving them is a data-project job.

## Tier styling

All five tiers ride **four independent channels at once** — left-rule weight *and* style, type size *and* weight, ink, and (machine tiers only) a background wash plus a glyph — so the ranking survives greyscale printing, zoom, and colour-blindness rather than depending on hue.

**Reservations honored.** Green `#1E6B4E` (`ux::kAct`) appears nowhere on this surface: it is HGM's binding authority and, in the badge grammar, attested EVIDENCE — and `docs/UX_AUDIT_2.md:54` logs W2-11 as a P1 defect for spending it on a non-success. `#b00` appears nowhere: it is the dictionary's `PROVISIONAL (auto-aligned)` mark (13 sites, pinned verbatim by the F1 sweep at :33446) and borrowing it would collide two unrelated meanings. `ux::Epistemic::Evidence` is deliberately unused — a name in a catalog is a REFERENCE record, not attested evidence of identity. **All author chips are outline-only**, because solid fill means *binding* in `ux::sourceBadge` (:289) and nothing here is binding.

**Two grammars, stacked and kept apart.** `ux::sourceBadge` opens each tier zone and names the **source class** (who vouches); the chip rides on each **row** and names the **route** by which the typed name reached the banked one. Because the badge follows the tier, the MACHINE badge lands physically on the machine tiers. `sourceBadge` emits a trailing space and is **always** immediately followed by `zoneLabel` — a badge without a zone label breaks the grammar.

| tier | badge | left rule | name ink | px / weight | wash | glyph | chip word |
|---|---|---|---|---|---|---|---|
| **Exact** | Reference `#2E629E` | `3px solid #2E629E` | `ux::kInk` `#2B2118` | 14 / 700 | — | — | `EXACT NAME` |
| **Contains** | Reference | `2px solid #6E675D` | `ux::kInk` `#2B2118` | 13 / 600 | — | — | `NAME CONTAINS` |
| **Spacing** | Reference | `2px dashed #6E675D` | `ux::kMuted` `#6E675D` | 13 / 500 | — | — | `SPACING VARIANT` |
| **Phonetic** | Machine `#B4540A` | `2px dashed #B4540A` | `#8A4A18` | 13 / 500 | `#FBF1E6` | `≈` | `SOUNDS LIKE` |
| **PhoneticNear** | Machine | `2px dotted #D89A6A` | `ux::kSoft` `#78706A` | 12 / 400 | `#FDF7F0` | `≈?` | `SOUNDS CLOSE` |

`#D89A6A` is the one new hex — a lightened `kMachine`, needed because Qt rich text does not honor `rgba()`. Everything else is an existing `ux::` token. Per `docs/CLOSER.md:40` (GUI revamps deferred), this **extends** the shipped grammar rather than inventing a visual language.

```cpp
// ---- author-match tier skins (five tiers, two authority classes) ----
// Ordering is monotone in rule-weight, type size, weight AND ink at
// once, so the ramp never depends on hue alone. #1E6B4E is HGM's and
// is not spent here; #b00 is the dictionary's PROVISIONAL mark and is
// not borrowed for search confidence.
struct AuthorTierSkin {
    const char* border; const char* wash; const char* nameCol;
    int px; int weight; const char* glyph;
    const char* chip; const char* zone; const char* caveat;
    ux::Epistemic badge;
};

static AuthorTierSkin authorTierSkin(allcore::AuthorTier t) {
    using T = allcore::AuthorTier;
    switch (t) {
    case T::Exact: return {
        "3px solid #2E629E", "", ux::kInk, 14, 700, "",
        "EXACT NAME", "EXACT · THE NAME AS BANKED", "",
        ux::Epistemic::Reference };
    case T::Contains: return {
        "2px solid #6E675D", "", ux::kInk, 13, 600, "",
        "NAME CONTAINS", "CONTAINS · YOUR WORDS INSIDE A LONGER NAME",
        "your words appear inside a longer banked name — orthographic, "
        "still the catalog's own spelling.",
        ux::Epistemic::Reference };
    case T::Spacing: return {
        "2px dashed #6E675D", "", ux::kMuted, 13, 500, "",
        "SPACING VARIANT", "SPACING · SAME LETTERS, DIFFERENT SYLLABLE BREAKS",
        "the same letters with the syllables broken differently "
        "(“pandi ta” and “pandita”). Tibetan names are "
        "written both ways, so this is a normalization — it claims "
        "nothing about sound.",
        ux::Epistemic::Reference };
    case T::Phonetic: return {
        "2px dashed #B4540A", "#FBF1E6", "#8A4A18", 13, 500, "≈",
        "SOUNDS LIKE", "SOUNDS THE SAME · MACHINE-LOCATED CANDIDATES",
        "the pronunciations agree (GMR convention) — a guess about "
        "WHICH person you meant, not evidence that it is them. Tibetan "
        "names are homonym-dense and two different people can sound "
        "identical. Candidates; the catalog record is the authority.",
        ux::Epistemic::Machine };
    case T::PhoneticNear: return {
        "2px dotted #D89A6A", "#FDF7F0", ux::kSoft, 12, 400, "≈?",
        "SOUNDS CLOSE", "SOUNDS CLOSE · WEAKEST · DISTANCE SHOWN",
        "the pronunciations are CLOSE, not equal — how far apart "
        "travels in each row below. The weakest route in this pane; "
        "confirm the name against the dates before citing anything "
        "found this way.",
        ux::Epistemic::Machine };
    }
    return {};
}

// The chip: same geometry as ux::sourceBadge (9px, ls 1px, weight 700,
// 1px border, radius 3, padding 0 4px, trailing space) so it reads as
// the same family — but outline-only across all five, because solid
// fill keeps meaning BINDING.
static QString authorTierChip(allcore::AuthorTier t) {
    const AuthorTierSkin s = authorTierSkin(t);
    const bool mach = t >= allcore::AuthorTier::Phonetic;
    return QString("<span style='font-family:-apple-system,Arial,sans-serif;"
                   "font-size:9px;letter-spacing:1px;font-weight:700;"
                   "border:1px %1 %2;border-radius:3px;padding:0 4px;color:%2;"
                   "%3'>%4</span> ")
        .arg(t == allcore::AuthorTier::PhoneticNear ? "dotted"
             : t >= allcore::AuthorTier::Spacing    ? "dashed" : "solid")
        .arg(s.nameCol == ux::kInk ? (t == allcore::AuthorTier::Exact
                                          ? "#2E629E" : "#6E675D")
                                   : s.nameCol)
        .arg(mach ? "font-style:italic;" : "")
        .arg(s.chip);
}

// One zone per NON-EMPTY tier. shown/total is the Phase-3 ledger rule:
// no cap may pose as the whole. A zone with zero rows is omitted.
static QString authorTierZone(allcore::AuthorTier t, int shown, int total) {
    const AuthorTierSkin s = authorTierSkin(t);
    QString h = ux::sourceBadge(s.badge) + zoneLabel(s.zone) +
                QString(" <small style='color:%1'>%2 of %3</small>")
                    .arg(ux::kFaint).arg(shown).arg(total);
    if (*s.caveat)
        h += QString("<div style='font-size:12px'><i style='color:%1'>%2</i></div>")
                 .arg(ux::kSoft).arg(s.caveat);
    return h;
}

// One PERSON row. `here`/`banked` = works installed / works catalogued.
static QString personHitRow(const allcore::PersonHit& hit, const QString& dates,
                            int here, int banked) {
    const AuthorTierSkin s = authorTierSkin(hit.tier);
    const QString disp = QString::fromStdString(hit.display);
    QString h = QString("<div style='margin:5px 0 0 0;padding:3px 0 3px 10px;"
                        "border-left:%1;%2'>")
                    .arg(s.border)
                    .arg(*s.wash ? QString("background:%1;").arg(s.wash) : QString());
    h += authorTierChip(hit.tier);
    h += QString("<a href='author:%1' style='color:%2;font-size:%3px;"
                 "font-weight:%4;text-decoration:none'>%5%6</a>")
             .arg(anchorEnc(QString::fromStdString(hit.pid)))
             .arg(s.nameCol).arg(s.px).arg(s.weight)
             .arg(*s.glyph ? QString("<span style='color:%1;font-weight:700'>%2</span> ")
                                 .arg(ux::kMachine).arg(QString::fromUtf8(s.glyph))
                           : QString())
             .arg(disp.toHtmlEscaped());
    if (!dates.isEmpty() && dates != "Unknown")
        h += QString(" <small style='color:%1'>(%2)</small>")
                 .arg(ux::kFaint).arg(dates.toHtmlEscaped());
    // the engine's own sentence, verbatim, never paraphrased — for
    // PhoneticNear it carries the edit distance
    h += QString("<div style='font-size:11px;color:%1;font-style:italic'>%2</div>")
             .arg(*s.wash ? ux::kMachine : ux::kSoft)
             .arg(ux::snipStd(hit.evidence, 120));
    // which spelling was hit, when it is not the display form
    if (hit.matchedAlias != hit.display)
        h += QString("<div style='font-size:11px;color:%1'>matched the spelling "
                     "“%2” — one of %3 the catalog banks for this "
                     "person</div>")
                 .arg(ux::kFaint)
                 .arg(QString::fromStdString(hit.matchedAlias).toHtmlEscaped())
                 .arg(hit.aliasCount);
    // §7: the count is visible BEFORE the click, in all three states
    h += QString("<div style='font-size:11px;color:%1'>%2</div>")
             .arg(ux::kFaint)
             .arg(banked == 0
                      ? QString("biography record — no works banked under this person")
                      : QString("%1 of %2 works in your Library").arg(here).arg(banked));
    h += "</div>";
    return h;
}
```

**Rendered, for `tsongkapa`:** the ladder reads a 14px bold ink name on a solid 3px blue rule at the top, down to a 12px grey-brown name on a dotted peach rule inside a peach wash prefixed `≈?` at the bottom. Nobody mistakes the fifth for the first, in colour or out of it. Both Changkyas come back as two `EXACT NAME` rows with their own pids, dates and counts; Tsongkhapa comes back **once** with all 139 local works, not six times with them split.

**In the ledger**, the chosen person's chip repeats at the head so the route never gets lost between screens, the zone badge follows the same rule (a ledger reached phonetically opens `[MACHINE]`, one reached exactly opens `[REFERENCE]`), and the route strip (§9) carries the full caveat.

**In ⌘K**, the ladder degrades honestly to ink + weight + slant + the trailing chip word, set through `Qt::ForegroundRole`/`Qt::FontRole` — `QStyledItemDelegate::initStyleOption` maps `ForegroundRole` onto `QPalette::Text` only, never `HighlightedText`, so a selected row still paints cream on `#8C2F2B` and stays legible.

## Test pins

- ONE PERSON, NOT SIX ROWS (the ruling): candidatesHtml("RJE TZ ONG KHA PA BLO BZANG GRAGS PA") — a mangled alias — yields exactly one "author:" anchor for pid P64, discloses "one of 6", and worksHtml("P64") counts >=130 "openfile:" anchors. Pins that alias spellings never split a man's works.
- HOMONYMS STAY APART: candidatesHtml("changkya") and candidatesHtml("lcang skya") each yield >=2 DISTINCT author: pids. The machine never merges two people who share a name.
- EXACT NOT SLANDERED (F1 direction 1): candidatesHtml("RJE TZONG KHA PA BLO BZANG GRAGS PA") contains "EXACT NAME", and contains NEITHER "SOUNDS" NOR the machine ink "#B4540A" NOR the route-strip phrase "pronunciation fold".
- WEAK NOT DRESSED STRONG (F1 direction 2): candidatesHtml("tsongkapa") contains "SOUNDS LIKE", "#B4540A", the glyph "≈", and the caveat "a guess about WHICH person you meant" — and does NOT contain "EXACT NAME".
- NO RESERVED INK BORROWED: neither candidatesHtml("tsongkapa") nor worksHtml("P64") contains "#1E6B4E" or "#b00", on any tier.
- ACIP/WYLIE/UNICODE DUALITY: the set of author: pids from candidatesHtml("TZONG KHA PA"), candidatesHtml("tsong kha pa") and the Tibetan-unicode form are identical and non-empty.
- ROUTE STRIP IS TWO-SIDED: worksHtml("P64", AuthorTier::Phonetic, "sounds like") contains "pronunciation fold" and "#FBF1E6"; worksHtml("P64", AuthorTier::Exact, "") contains NEITHER.
- LEDGER IS UNCAPPED: worksHtml("P64") contains >100 "openfile:" anchors and does NOT contain "and 10 more" or "… and " — the personHtml cap must not have followed the data over.
- BIOGRAPHY-ONLY STATE (298 people): for a pid whose works array is empty, worksHtml contains "banked as a biography, not as an author" and "not a search failure", contains NO "openfile:", and DOES contain a bdrc.io link. It must not read as a miss.
- BANKED-BUT-ABSENT STATE (37 people): for a pid with works and zero live files, worksHtml contains "None of them are installed in your Library", the true banked count, and "Get collections" — and lists the work keys with no openfile: anchor.
- MISS NAMES EVERY ROUTE: candidatesHtml("zzzznotaname") contains "exact name", "name contains", "syllable spacing", "near-pronunciation", and "carry no author in any bank" — and does NOT contain "fuzzy".
- BANK FAILURE IS NOT A MISS: with authorIndexError_ set, candidatesHtml(anything) contains "AUTHOR BANK NOT LOADED" and the failing path, and does NOT contain "no catalogued person matches". Plus: buildAuthorIndex() leaves authorIndexBuilt_ FALSE after a failed read (the flag-before-load antipattern must not propagate).

## Open questions for Adam

1. The 442 gap. 442 live library work keys carry a catalog_works author NAME but no BDRC pid, so matchPeople cannot reach them; my spec handles them in-app with a second, clearly-labeled matchAuthors pass (§3b). The cleaner fix is upstream: should tools/build_author_index.py absorb catalog_works name-only attributions as pid-less person records, so the app needs one matcher instead of two? That is a data-project change and needs your call on whether a name-only group may sit in the same bank as pid-grouped people.
2. author_index.json's works_local is a build-time snapshot of your library (built 2026-08-22 13:40). I ruled that the app ignores it and derives presence live from its own walk, so the count is always true for THIS machine. Confirm that ruling is permanent — the alternative is re-running the builder on every collection install, which couples the app to a Python tool.
3. 36 of the 462 people have no recorded name (display empty, aliases empty). They are unreachable by search by construction and their works are therefore unfindable by author. Should they surface anywhere — e.g. an "N author records with no name banked" line in the ledger footer — or stay entirely silent?
4. acip_person_links.json's .conflicts subtree holds exactly one entry (S6387: P289 Dngul chu Dharma Bhadra vs P268 Dbyangs can grub pa'i rdo rje) and is never read by the app today. I spec'd showing the work on BOTH people's ledgers with an amber "two competing attributions recorded" mark. Confirm that is the disposition you want, rather than suppressing the work from both.
5. The display name for each person is chosen by the builder's "most frequent spelling" rule (the catalog's own mode). For P64 that yields "RJE TZONG KHA PA BLO BZANG GRAGS PA". Do you want an authoritative per-person display form (a small override table) instead of letting frequency decide what a reader sees?
6. I deleted the two BDRC / Treasury of Lives URL rows from the ⌘K persons lane, on the grounds that you asked for texts and those rows send you to a browser; those links now live on the ledger instead. For the 298 biography-only people, though, the browser IS the answer. Should ⌘K keep a single BDRC row for people with zero banked works?
