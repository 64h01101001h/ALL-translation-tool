# Collection updates — the specification

*Produced 2026-08-22 by a design panel (quiet / explicit / menu-first, then one judge). The judge CORRECTED the brief it was given: I had told it no last-checked memory existed, having written the brief before building it an hour earlier. It found the real code and forbade duplicate keys. That is the system working.*

## Rulings

1. DOES THE APP EVER TOUCH THE NETWORK UNASKED?
NO, by default. `collections/checkPolicy` ships as "ask", and under "ask" not one socket opens toward asianlegacylibrary.org unless a finger pressed something. This is the decisive break with Proposal 1, and the reason is not squeamishness: the app's own Help menu currently promises the user "there is no auto-update and the app never phones home" (main.cpp:32330-32333), and an input centre on a metered or monitored line is a policy question nobody has answered yet. A default-on daily check would make the tool lie in one dropdown about what it does in another.
The user MAY opt in: File ▸ Official collections ▸ When to check ▸ {Only when I ask (default) · Weekly, ninety seconds after the app starts · Every time the app starts}. Turning it off is the same radio group. When it is on, the check is three HEAD requests to the seeded S3 URLs — no page GET, no body, ~4 KB — and that cost is stated in the menu itself (§D4) so the user can verify the claim rather than trust it. It can never download; downloading remains a per-row button plus a confirmation that names the transfer size.
So how does a user on the default EVER learn an update exists? Three signals that report the AGE OF KNOWLEDGE, never knowledge, and cost zero bytes: (a) the File submenu carries its own staleness in its title — "Official collections — last checked 34 days ago" / "— never checked", met while reaching for something else; (b) the Shelf Line, a dated sentence under the Library banner, in the pane a translator opens to fetch a text anyway; (c) the dialog header, which opens by telling you how much to trust what follows. None of them ever claims a finding. This is a deliberate, stated trade: Adam can go a year without learning a new Kangyur exists, and the design closes that gap as far as it honestly can rather than closing it with traffic he did not authorise.

2. EXACT WORDING — see the `wording` field, which is exhaustive. The governing rule: every status string is manufactured in one place, `collstat::rowLine(seen, asOf)`, whose timestamp argument is NOT defaultable, so a dateless status is not expressible in the codebase. THE TENSE IS THE HONESTY MECHANISM: "matched the website on Fri 22 Aug" is a fact with a date on it; "current" is a claim about the present tense that no cached value can support. Past 30 days the string appends its own age ("— 34 days ago, not checked since") so a year-old check cannot be misread as this morning's.
WHEN IS THE TOOL ENTITLED TO "current ✓"? Under exactly three simultaneous conditions: (a) a HEAD performed during THIS open dialog session succeeded for that row (`row.live`), (b) `collections/etag/<name>` is non-empty — the tool installed it and therefore has something to compare, and (c) it equals the fingerprint the website just served. It then renders "current as of this check, 2:03 pm ✓" — the tick survives only where the user watched the measurement happen. Cached rows, unanswered rows, and rows installed outside the checker may never carry it. The existing "installed (fingerprint unknown — installed outside this checker)" survives as §B7, spelled out in full: "there is no fingerprint to compare, so the tool cannot tell you whether it is current."

3. WHERE IT LIVES — several places, each earning its keep, and the ribbon gains nothing.
· TOP MENU BAR (Adam's ask, and the primary home): File ▸ Official collections ▸, inserted above "Import Data Release…" (main.cpp:31793) because File's stated grammar is "the acts of getting work in and out" and a 258 MB canon arriving is exactly that. NOT a top-level Collections menu: the menu bar between Edit and View is generated from the tab groups at main.cpp:31893, so a ninth top-level menu would sit among them claiming to be a pane that does not exist. Rebuilt on aboutToShow from QSettings only — forever.
· RIBBON: unchanged. "Collections…" (main.cpp:19863) keeps its exact label — it costs no new width, it exists because of a real reported failure, and the Library selftest at main.cpp:20164-20176 pins its text. Only the tooltip changes.
· PANE MAINTENANCE MENU: the "Check for collection updates…" entry (main.cpp:19905-19907) is DELETED. Three doors to one window inside one pane is clutter, and the auto-mirror at main.cpp:31893 still generates Read ▸ Library ▸ Collections… from the button.
· HELP and VIEW: both existing "Check for Updates…" entries are amended to name their object in the first clause, and Help gains a [Check Collections Now…] button.
· LIBRARY PANE: the Shelf Line, one sentence, no new control.
· STATUS BAR: one muted label, present only when a real check found a real difference.
Naming: the noun is "Official collections" in the File submenu, the dialog title, the shelf line and the status bar; the ribbon keeps the short form for width and opens its tooltip with the full noun.

4. WHAT A FAILED CHECK LOOKS LIKE — the single most important string in the feature, and it never resembles good news. Offline, DNS down, or S3 refusing HEAD all land in §A3/§A4: "The check on 22 August 2026 at 8:52 am could not reach asianlegacylibrary.org (Host not found). Nothing was compared, so this is not a report that your collections are current." When there is an earlier real answer, it is shown next with its own age attached; when there has never been one, §A4 says so and adds "and it is not claiming they are out of date either. It does not know."
The mechanism that makes this structural rather than a matter of care: a failed check does NOT write `collections/lastCheck` — the existing `reachable > 0` condition at main.cpp:21192 is preserved and is now load-bearing. `lastCheck` means "the last time the website answered", full stop, so no failed round can ever refresh a reassurance. The attempt is recorded separately in `collections/lastCheckAttempt`, and per-row failure is recorded as state "unanswered" with its own date, which renders as §B8: "the website did not answer this row on 22 Aug. Nothing is known about it either way — installing will still try." That last clause preserves the shipped rule at main.cpp:21125-21131 — a refused HEAD must not dead-end the only install window. "Never checked" and "did not answer" are permanently distinct strings; collapsing them into one tidy phrase is the exact failure the fingerprint-unknown wording was written to prevent, and pin 5 forbids it.

5. WHAT IS REMEMBERED, IN WHICH KEYS — QSettings("ALL","TranslationTool"), the existing `collections/` namespace. Two keys ALREADY EXIST and all three proposals missed them; they are kept with their exact current meaning rather than shadowed by new ones.
EXISTING, UNCHANGED:
  collections/etag/<name>        what THIS CHECKER installed (read 21103, written 21301)
  collections/lastCheck          ISO — last time the website ANSWERED (written 21195, only when reachable > 0)
  collections/lastResult         the human summary of that answering check (written 21199)
ADDED:
  collections/lastCheckAttempt   ISO — every attempt, success or not
  collections/lastCheckOutcome   "ok" | "partial" | "unreachable"
  collections/lastCheckError     one-line QNetworkReply::errorString for display
  collections/seen/<name>/etag         what the WEBSITE served
  collections/seen/<name>/lastModified
  collections/seen/<name>/bytes
  collections/seen/<name>/state        absent|unfingerprinted|match|differs|unanswered
  collections/seen/<name>/at           ISO — when that row was learned
  collections/installedAt/<name>       ISO
  collections/installedFrom/<name>     the URL, or "a ZIP on this Mac"
  collections/checkPolicy              "ask" (default) | "weekly" | "launch"
  collections/lastAutoCheckAt          ISO — the weekly gate
  collections/dismissedEtag/<name>     suppresses the STATUS BAR only, never the menu row or the shelf line
THE INVARIANT: `collections/etag/<name>` is what we installed; `collections/seen/<name>/etag` is what the website served. No code path may copy one into the other except downloadAndInstall, so nothing the network says can ever corrupt the "what I installed" fingerprint. Corollary, and the fix for a real hole: `installZipPath()` (main.cpp:20967) can overwrite a collection folder from a local ZIP via the separate Install… button, after which a stored "match" describes a folder that no longer exists. It gains a `fromChecker` parameter and, when false, CLEARS `collections/etag/<name>` and sets state to "unfingerprinted" — the row falls back to "no fingerprint to compare", which is the truth.
Rendering rule over all of it: no key stores a verdict without the moment it was true, and no reader may render a verdict without its moment.

## Wording (final)

All strings final. Placeholders: <NAME> upper-case collection name; <DATE> = "d MMMM yyyy 'at' h:mm ap"; <SHORT> = "d MMM yyyy"; <N> integer.

=== §A  DIALOG HEADER (exactly one) ===
A1 never checked —
"This tool has never asked asianlegacylibrary.org about these files. The three collections below are listed because they are the official ALL releases, not because anything was verified. Press Check the website now when you want the tool to look."
A2 checked, site answered —
"Last answered by the website <DATE> — <N> days ago. That check found: <lastResult>. Nothing below has been re-checked since; the website may have published a new release in those <N> days and this tool would not know."
A3 last attempt failed, earlier answer on record —
"The check on <DATE> could not reach asianlegacylibrary.org (<lastCheckError>). Nothing was compared, so this is not a report that your collections are current. The last real answer was <DATE>: <lastResult>."
A4 last attempt failed, never any answer —
"The check on <DATE> could not reach asianlegacylibrary.org (<lastCheckError>), and no earlier check ever got an answer. The tool has no fingerprint from the website at all, so it cannot tell you whether your copies are current — and it is not claiming they are out of date either. It does not know."
A5 sweep/harness —
"Sweep mode — network checks stay offline. Everything below is what was stored on disk."
A6 under the check button, grey —
"Asks for three file headers — about 2 KB. No download starts without a second confirmation."
A7 button while running —
"Asking the website… (<N> of 3)"
A8 the standing note, replacing main.cpp:21143 (the "checked live just now" clause is deleted) —
"The Kangyur, Tengyur, and Sungbum as published by the Asian Legacy Library. Downloading unpacks that collection's ZIP over its own folder exactly as a manual install would — your my_materials and work folders are never touched."

=== §B  ROW STATUS (collstat::rowLine) ===
B1 never asked, absent —      "not in your library · never checked"
B2 never asked, present —     "in your library · version never checked"
B3 live HEAD, fingerprints agree — "current as of this check, h:mm ap ✓"   [the ONLY tick]
B4 remembered match, under 30 days — "matched the website on <SHORT>"
B5 remembered match, 30+ days —      "matched the website on <SHORT> — <N> days ago, not checked since"
B6 differs —  "UPDATE AVAILABLE — the website's copy changed since yours was installed. Seen <SHORT>; site copy dated <lastModified>, <N> MB."
B6a differs on ETag alone (size and Last-Modified unchanged) —
"UPDATE AVAILABLE — the file's fingerprint on the website changed, but its size and date did not. This may be a re-upload of the same release. Seen <SHORT>."
B7 installed, no fingerprint — "installed, but not by this checker — there is no fingerprint to compare, so the tool cannot tell you whether it is current"
B8 HEAD refused — "the website did not answer this row on <SHORT>. Nothing is known about it either way — installing will still try."
B9 second line when installedAt exists — "Installed here <SHORT> from <installedFrom>."
B10 absent but the site answered — "not in your library · <N> MB on the website (<lastModified>)"
B11 dismissed update — "UPDATE AVAILABLE — seen <SHORT>, and you chose not to install it. Still on the copy installed <SHORT>."

=== §C  DOWNLOAD ===
C1 confirmation —
"Download <NAME> (<N> MB) from the official release bucket and install it into the library?

This is a <N> MB transfer. It starts immediately and can be cancelled at any point; a cancelled download installs nothing. Your existing <name> folder is overwritten file by file — my_materials and work are not touched."
(when size unknown, the first line reads "(size not reported)" and the second reads
"The website did not report a size. Official collections run from 122 MB to 258 MB.")
C2 post-install —
"<NAME> is installed, and the tool recorded the website's fingerprint for it. Next time you check, it can tell you whether the website has moved on. Run Maintenance → Update search index next."
C3 failure (unchanged in meaning) —
"Download failed or was cancelled — nothing installed."

=== §D  FILE ▸ OFFICIAL COLLECTIONS ===
D1 submenu title, by state —
"Official collections"
"Official collections — never checked"
"Official collections — last checked <N> days ago"
"Official collections — 1 new release"      /  "Official collections — <N> new releases"
D2 stamp line, disabled —
"The app has never asked the website about these files."
"Last asked the website: <DATE>."
"Last asked: <DATE> — the website did not answer."
D3 When to check ▸ (radio) —
"Only when I ask"
"Weekly, ninety seconds after the app starts"
"Every time the app starts"
D4 cost footnote, disabled, shown under the check action and again inside When to check —
"A check reads three file headers — a few kilobytes. Downloads (122–258 MB) never start on their own."
D5 actions —
"Check the site for new releases…"
"Install a collection ZIP from this Mac…"

=== §E  THE SHELF LINE (Library pane, under the banner) ===
E1 never checked —
"Official collections — never checked. Nothing has yet asked asianlegacylibrary.org whether the Kangyur, Tengyur, or Sungbum have been republished. <a href='collections:open'>Open Official collections</a>"
E2 checked, all matched —
"Official collections — <NAME1>, <NAME2> and <NAME3> matched the files asianlegacylibrary.org was serving on <DATE>. Nothing has looked since. <a href='collections:open'>Check again</a>"
E3 update found —
"Official collections — <NAME> was republished on the website (<lastModified>, <N> MB); the copy on your shelf is fingerprinted to the release installed <SHORT>. <a href='collections:open'>Review and install…</a> · <a href='collections:dismiss/<name>'>Not now</a>"
E3a after Not now —
"Official collections — <NAME> was republished on <lastModified>; you chose not to install it. You are still on the copy installed <SHORT>. <a href='collections:open'>Install it…</a>"
E4 unreachable —
"Official collections — the check on <DATE> could not reach asianlegacylibrary.org (<lastCheckError>). Nothing was compared, so this is not a report that your collections are current. <a href='collections:open'>Try again</a>"
E5 partial —
"Official collections — checked <DATE>: <NAME1> and <NAME2> matched. <NAME3> did not answer, so its state is unknown — it was not compared. <a href='collections:open'>Check again</a>"
E6 installed outside the checker —
"Official collections — <NAME> is on your shelf but was installed outside this checker, so there is no fingerprint to compare and the tool cannot tell you whether it is current. Installing it once through Official collections gives it a fingerprint. <a href='collections:open'>Open Official collections</a>"
E7 none installed —
"Official collections — none of the three are on your shelf yet. <a href='collections:open'>Download them from asianlegacylibrary.org</a>"
E8 clock moved —
"Official collections — the stored check time (<SHORT>) is in the future, so this machine's clock has changed. Treat every date here as unverified and check again. <a href='collections:open'>Check now</a>"

=== §F  STATUS BAR + TOOLTIP ===
F0 status bar (only when an unseen, undismissed difference is on record) —
"<NAME>: a new release is on the website (seen <SHORT>) — File ▸ Official collections"
F0a its context menu — "Dismiss this notice"
F1 ribbon tooltip —
"Official collections — download the Kangyur, Tengyur, and Sungbum from asianlegacylibrary.org and see when they were last checked. Nothing is transferred until you ask."

=== §G  THE APP'S OWN UPDATE DIALOGS ===
G1 Help ▸ Check for Updates… —
"This is the Diamond Cutter Translation Tool <version>.

The application itself is distributed as a new disk image by the ALL team. There is no auto-update for the app, and the app does not contact any server about itself. Ask Adam for the current DMG, or check the team's shared folder.

The Kangyur, Tengyur, and Sungbum are a separate matter: they come from asianlegacylibrary.org, and this tool checks them only when you ask — unless you have turned on a scheduled check under File ▸ Official collections ▸ When to check, which is off by default. Last answered by that website: <DATE> / never."
Buttons: [Check Collections Now…] [OK]
G2 View ▸ Check for Updates… first clause —
"This checks for a new version of the APPLICATION — not the Kangyur, Tengyur, or Sungbum, which live under File ▸ Official collections. Updates are distributed by the ALL team as DMGs in a shared folder (e.g. the team Dropbox).

Choose that folder now? It will be remembered."

## Test pins

- collections/open-offline: with the network unavailable, openCollectionsWindow() builds 3 rows from QSettings alone; a QNetworkAccessManager call-counter probe reads 0 for both the open and a File-submenu aboutToShow rebuild (the menu must never HEAD)
- collections/dateless-status-inexpressible: collstat::rowLine(seen, asOf) has no default for asOf; rowLine(seen with null .at, QDateTime()) returns a string containing "never" and containing neither "current" nor "matched"; every non-never return value contains a rendered date
- collections/tick-licence: over all 8 combinations of (live HEAD ok, folder present, installed etag non-empty, etags equal), the substring "✓" appears in exactly one — live && present && fingerprinted && equal — and that string also contains a clock time
- collections/failed-check-preserves-truth: after a check where every HEAD errors, collections/lastCheck and collections/lastResult are byte-identical to their pre-check values, lastCheckAttempt advanced, lastCheckOutcome=="unreachable", and the rendered header contains "Nothing was compared" and does not contain "current"
- collections/never-vs-silent: the never-checked string and the did-not-answer string are distinct and neither is a substring of the other; both are reachable from collstat and both name the reason
- collections/default-is-quiet: collections/checkPolicy defaults to "ask"; autoCheckCollections() under "ask" opens 0 sockets, under "weekly" with lastAutoCheckAt set to now opens 0, and under g_sweepActive or g_harnessRun opens 0 in every policy
- collections/no-nested-loop-in-background: the headless branch of runCollectionCheck contains no call to await( and constructs no QEventLoop — pinned by a source-text assertion over the function body extracted at build time, since a blocked event loop cannot be observed from inside itself
- collections/local-zip-drops-the-fingerprint: installZipPath(kangyur.zip, false) erases collections/etag/kangyur and sets seen/kangyur/state=="unfingerprinted"; installZipPath(tmp, true) leaves both intact, and the resulting row text contains "no fingerprint to compare"
- collections/download-is-guarded: downloadAndInstall() returns before its QMessageBox when g_sweepActive || g_harnessRun; a full --sweep Library run and a 20,000-step gauntlet leave the temp staging path absent and collections/installedAt/* unwritten
- collections/two-update-doors-name-their-object: Help ▸ Check for Updates and View ▸ Check for Updates each mention both "application" and "asianlegacylibrary.org"; the phrase "never phones home" appears nowhere unqualified, and the Help text names the current checkPolicy

## Full spec

DECISION: base = Proposal 3 (menu-first). Grafted: the open/check SPLIT + sweep guards + Help-menu amendment + installZipPath fingerprint invalidation from Proposal 2; the Shelf Line, the callback-only background path, and the "remote fingerprint never conflated with installed fingerprint" rule from Proposal 1. REJECTED: the icon badge dot (a dot is a dateless claim, and repainting the Library surface icon touches the mirror/pin machinery for no wording gain — the status-bar sentence carries the same signal in words); the daily-on-by-default quiet check (it contradicts the app's own Help promise and pre-empts the input-centre policy question — it survives as an opt-in weekly policy); a top-level Collections menu (the menu bar between Edit and View is generated from tab groups at main.cpp:31893, so a ninth top-level menu would claim to be a pane); a download leaf in any menu.

CORRECTION TO THE BRIEF, which all three proposals inherited: gap #1 is only half open. `collections/lastCheck` and `collections/lastResult` ALREADY EXIST — written at app/main.cpp:21195-21201 (only when at least one row was reachable) and rendered at app/main.cpp:21165-21186 as "Previously checked 6 August 2026 at 4:02 pm (16 days ago) — all 3 current". The memory exists; it is trapped inside a dialog you cannot open without spending the network. Do NOT invent `lastCheckAt`/`lastCheckSummary` parallel keys. Keep these two, keep their exact meaning (lastCheck = last time the site actually ANSWERED), and add an attempt key beside them.

=========================================================
STEP 1 — file-scope status vocabulary (insert after main.cpp:2433, beside g_importRelease)
=========================================================
1.1  Add globals:
       static QString g_libRoot;                              // set by LibraryPane ctor
       static std::function<void(QString)> g_openCollections;  // focus name, may be empty
       static std::function<void()> g_collectionsAutoCheck;    // policy-gated, headless
1.2  Add `namespace collstat { … }` — the ONLY place a collection status string is
     manufactured. Nothing anywhere else in main.cpp may concatenate a status.
     struct Seen { QString name, etag, lastModified, state; qint64 bytes = 0;
                   QDateTime at; };                 // state ∈ absent|unfingerprinted|match|differs|unanswered|never
     QList<Seen> all();          // union of {kangyur,tengyur,sungbum} and the child
                                 // groups of collections/seen/ — so a fourth collection
                                 // found by a pressed check joins the menu permanently.
                                 // installed-ness = QDir(g_libRoot+"/"+name) non-empty;
                                 // fingerprinted = !collections/etag/<name>.isEmpty()
     QString rowLine(const Seen& s, const QDateTime& asOf);   // asOf NOT defaultable
     QString stampLine();        // "Last asked the site: …"
     QString submenuTitle();
     QString shelfSentence();    // rich text, one sentence + one <a href>
     QString statusBarNotice();  // empty unless an unseen, undismissed differs exists
     int unseenUpdates();
     Hard rule, enforced by pin 2: rowLine takes the timestamp as a non-defaultable
     argument and, when `s.at.isNull()`, is only permitted to return a "never" string.
     A dateless status must not be expressible.
1.3  Date formats, fixed: full stamp `d MMMM yyyy 'at' h:mm ap` (matches main.cpp:21174);
     compact row `d MMM yyyy`; age suffix " — N days ago" appended by rowLine whenever
     asOf.daysTo(now) >= 30.
1.4  CLOCK GUARD: if any stored timestamp is later than now by more than 1 day,
     collstat returns the CLOCK MOVED sentence (see wording) instead of any age phrase.

=========================================================
STEP 2 — the Shelf Line (LibraryPane, main.cpp:19764-19790)
=========================================================
2.1  In the ctor, first line after `QDir().mkpath(...)`: `g_libRoot = libRoot_;`.
2.2  After `layout->addWidget(libBanner)` (main.cpp:19787) insert `shelfLine_`,
     a QLabel: setWordWrap(true), setMinimumWidth(1) (same reason as libBanner,
     main.cpp:19782-19786), setTextFormat(Qt::RichText), setOpenExternalLinks(false),
     styled `color: ux::kMuted; font-size: ux::kFsMeta`.
2.3  connect(shelfLine_, &QLabel::linkActivated, …): "collections:open" →
     openCollectionsWindow({}); "collections:dismiss/<name>" → write
     collections/dismissedEtag/<name> = seen etag, then refreshShelfLine().
2.4  `void refreshShelfLine() { shelfLine_->setText(collstat::shelfSentence()); }`
     Called: end of ctor, end of runCollectionCheck (both modes), end of
     downloadAndInstall, end of installZipPath. It reads QSettings ONLY — constructing
     the Library pane must never open a socket.

=========================================================
STEP 3 — split the one function in two (main.cpp:21041)
=========================================================
3.1  Rename `checkCollectionUpdates()` → `openCollectionsWindow(const QString& focus = {})`.
     Delete the g_sweepActive early-return at main.cpp:21042-21046 from THIS function
     (the window itself is offline and therefore safe; the reaper rejects it in headless
     modes, and rejecting it does nothing destructive).
3.2  Update the three call sites: main.cpp:19907 (Maintenance — deleted, see step 11),
     main.cpp:20130 (getBtn), and add `g_openCollections = [this](QString f){
     openCollectionsWindow(f); };` next to `g_importRelease = …` (main.cpp:19911).
3.3  openCollectionsWindow builds the SAME dialog (title "Official collections —
     asianlegacylibrary.org", 680x320, WA_DeleteOnClose, non-modal, main.cpp:21138-21141)
     entirely from QSettings via collstat::all(). Zero network. Row order is
     officialCollectionUrls("") order first (the three, always, main.cpp:21030-21039),
     then any extra remembered names. If `focus` names a row, that row's QLabel gets
     a 2px ux::kGold border and dlg is scrolled to it.
3.4  New member `void runCollectionCheck(QDialog* dlg, bool headless)` holding the
     network work formerly at main.cpp:21050-21134:
       - FIRST LINE: `if (g_sweepActive || g_harnessRun) { … return; }` — under sweep,
         set the dialog header to the SWEEP string (wording §A5) and return; headless
         mode just returns.
       - dlg mode: page GET (main.cpp:21064-21073) + HEAD per URL, keeping the existing
         `await()` helper (main.cpp:21050) — this path is user-initiated and the button
         is disabled while it runs.
       - headless mode: NO page GET, NO await, NO nested QEventLoop. HEAD only the three
         URLs from officialCollectionUrls(""), each with
         QNetworkRequest::TransferTimeoutAttribute = 8000, results collected in
         `finished` lambdas, a shared completion counter firing the settings write +
         refreshShelfLine() + g_collectionsUiRefresh(). A reviewer must reject any
         `await(` inside this branch (pin 7).
3.5  Signature of the row struct (main.cpp:21075-21082) gains `QString prevEtag;
     bool live = false;` — `live` is true only for a row whose HEAD succeeded during
     THIS dialog session. It is the sole licence for the tick (step 5.3).

=========================================================
STEP 4 — what a check writes (replaces main.cpp:21159-21207)
=========================================================
Every check, success or failure, writes:
  collections/lastCheckAttempt = now (ISO)
  collections/lastCheckOutcome = "ok" | "partial" | "unreachable"
  collections/lastCheckError   = QNetworkReply::errorString() of the first failure, or ""
Per row, always (including the failure case):
  collections/seen/<name>/at    = now (ISO)
  collections/seen/<name>/state = absent|unfingerprinted|match|differs|unanswered
and, only when that row's HEAD succeeded:
  collections/seen/<name>/etag, /lastModified (yyyy-MM-dd), /bytes
Only when reachable > 0 (PRESERVE the existing condition at main.cpp:21192-21194):
  collections/lastCheck, collections/lastResult   — unchanged keys, unchanged meaning
INVARIANT: collections/seen/<name>/etag is what the SITE served.
collections/etag/<name> (main.cpp:21103, written main.cpp:21301) is what THIS CHECKER
INSTALLED. No code path may copy one into the other except downloadAndInstall.

=========================================================
STEP 5 — the dialog, rebuilt (main.cpp:21138-21245)
=========================================================
5.1  HEADER, above the existing note (main.cpp:21143): a word-wrapped QLabel carrying
     wording §A1/§A2/§A3/§A4/§A5 chosen from lastCheck / lastCheckAttempt /
     lastCheckOutcome. Then the existing `note` label, with its "checked live just now"
     clause deleted (it is a lie the moment the window opens from cache).
5.2  A primary QPushButton "Check the website now" + a grey sub-label (§A6). While
     running it disables and reads "Asking the website… (2 of 3)". On completion the
     header, every row, the shelf line and the menu title all re-render.
5.3  ROW LAYOUT unchanged (main.cpp:21209-21243): rich-text QLabel + selectable URL +
     "Download && install…" on EVERY row whose state is not `match`, plus every row whose
     HEAD did not answer — the refused-HEAD-must-not-dead-end rule (main.cpp:21125-21131)
     survives verbatim. Status text comes from collstat::rowLine (§B). THE TICK: a row
     may render "current as of this check, 2:03 pm ✓" if and ONLY IF
     `row.live && !installedEtag.isEmpty() && installedEtag == row.etag`. Cached rows,
     unanswered rows, and unfingerprinted rows may never carry a tick.
5.4  Add a second line to any row with collections/installedAt/<name>: §B9.

=========================================================
STEP 6 — downloadAndInstall (main.cpp:21247-21311)
=========================================================
6.1  New first line: `if (g_sweepActive || g_harnessRun) return;` — it currently has NO
     guard and is safe only by being unreachable; the window now opens offline, so its
     button is in front of the clicker. The QMessageBox default of No (main.cpp:21266)
     is the second line of defence, not the first.
6.2  Confirmation text → §C1. QMessageBox::No stays the default button.
6.3  `installZipPath(tmp)` → `installZipPath(tmp, /*fromChecker=*/true)`.
6.4  Beside the etag write (main.cpp:21301) also write collections/installedAt/<name> =
     now (ISO), collections/installedFrom/<name> = url, and collections/seen/<name>/
     {etag,lastModified,bytes,state="match",at=now}. Clear collections/dismissedEtag/<name>.
6.5  Post-install box → §C2 (the current text at main.cpp:21302-21309 makes a
     forward-looking promise about a future check; it is replaced).
6.6  Call refreshShelfLine().

=========================================================
STEP 7 — a local ZIP must destroy the fingerprint (main.cpp:20967)
=========================================================
7.1  `bool installZipPath(const QString& zip, bool fromChecker = false)`.
7.2  After the name resolution loop (main.cpp:20970-20972) and on `ok`, when
     `!fromChecker` and name ∈ {kangyur,tengyur,sungbum,varanasi}: remove
     collections/etag/<name>, set collections/seen/<name>/state = "unfingerprinted",
     collections/seen/<name>/at = now, and write collections/installedAt/<name> = now,
     collections/installedFrom/<name> = "a ZIP on this Mac".
     Rationale: after a manual install the row's stored match describes a folder that no
     longer exists, and the dialog would show a matched claim that is not true. Falling
     back to "no fingerprint to compare" is the honest state.
7.3  Call refreshShelfLine() at the end.

=========================================================
STEP 8 — the optional background check
=========================================================
8.1  `collections/checkPolicy` default "ask". Values: "ask" | "weekly" | "launch".
8.2  In the LibraryPane ctor: `g_collectionsAutoCheck = [this]{ autoCheckCollections(); };`
8.3  `void autoCheckCollections()` refuses, in this order, before any socket:
       g_sweepActive || g_harnessRun  →  return;
       policy == "ask"                →  return;
       policy == "weekly" && collections/lastAutoCheckAt within 7 days  →  return;
     then writes lastAutoCheckAt = now and calls runCollectionCheck(nullptr, true).
     It never opens a dialog, never raises a pane, never plays a sound, and can never
     download. Its entire cost is three HEAD request/response header pairs (~4 KB).
8.4  Arm it at main.cpp:34925 (immediately after `win.show()`, before the W1 welcome
     block): `QTimer::singleShot(90'000, &win, []{ if (g_collectionsAutoCheck)
     g_collectionsAutoCheck(); });`. Ninety seconds because the first minute of a
     session belongs to the translator; the check has no deadline.

=========================================================
STEP 9 — File ▸ Official collections (main.cpp, inside the File block 31494-31796)
=========================================================
9.1  Insert immediately ABOVE the "Import Data Release…" action (main.cpp:31793) —
     both are acts of getting bulk material in, which is File's stated grammar
     (comment at main.cpp:31491-31493).
       QMenu* coll = fileM->addMenu("Official collections");
9.2  Rebuild `coll` on QMenu::aboutToShow, exactly the Open Recent pattern
     (main.cpp:31572-31580). It reads QSettings ONLY, forever. A future "if we have no
     cached etag, just do a quick HEAD" here would turn hovering over File into a
     15-second freeze; pin 1 forbids it.
     Contents:
       <one action per collstat::all() row, text = collstat::rowLine(...)>
         · bold via the QFont idiom already in the file (QFont f = a->font();
           f.setBold(true); a->setFont(f)) when state == differs and undismissed
         · NEVER setCheckable/setChecked — a checkmark means "this mode is on"
         · triggered → if (g_openCollections) g_openCollections(<name>)
       ──────────
       <collstat::stampLine()>                      addAction(txt)->setEnabled(false)
       "Check the site for new releases…"           → g_openCollections({}) then the
                                                      dialog's own check button is
                                                      pressed programmatically
       "When to check"  ▸  QActionGroup radio submenu, §D3
       <cost footnote, disabled>                    §D4
       ──────────
       "Install a collection ZIP from this Mac…"    → the Library pane's installAny()
9.3  File's own aboutToShow recomputes `coll->setTitle(collstat::submenuTitle())` — the
     proactive signal, at zero network cost.
9.4  NO keyboard shortcut anywhere in this submenu. 258 MB must never be one mistyped
     chord away. NO download leaf: menu leaves are reached by hover during traversal.

=========================================================
STEP 10 — the status bar (main.cpp:34896-34920)
=========================================================
10.1 Inside the existing addPermanentWidget block, add
     `g_collectionsNotice = new QLabel;` styled ux::kWarn, `setVisible(false)`.
10.2 A file-scope `std::function<void()> g_collectionsUiRefresh` sets its text from
     collstat::statusBarNotice() and its visibility from `!text.isEmpty()`. Called by
     runCollectionCheck (both modes) and after any install.
10.3 Context menu on the label: "Dismiss this notice" → writes
     collections/dismissedEtag/<name>. The dismissal suppresses the BAR only; the menu
     row and the shelf line keep reporting the difference (wording §E3).

=========================================================
STEP 11 — the doors, reconciled
=========================================================
11.1 KEEP the ribbon button `getBtn` "Collections…" (main.cpp:19863). It exists because
     of a reported failure (comment main.cpp:19856-19862) and is pinned by the Library
     selftest at main.cpp:20164-20176, which asserts its text startsWith("Collections").
     Do not lengthen it — the ribbon refuses to squeeze labels and scrolls instead.
11.2 Tooltip → §F1.
11.3 DELETE `maintMenu->addAction("Check for collection updates…", …)` at
     main.cpp:19905-19907. Three doors to one window inside one pane is clutter; the
     ribbon button plus File ▸ Official collections is coverage, and the auto-mirror at
     main.cpp:31893-31905 still generates Read ▸ Library ▸ Collections… from the button.
11.4 Naming ruling, applied in all four places: the noun is "Official collections"
     (File submenu, dialog title, shelf line, status bar). The ribbon keeps the short
     "Collections…" for width, and its tooltip opens with the full noun.

=========================================================
STEP 12 — the app's own update dialogs stop lying by omission
=========================================================
12.1 Help ▸ Check for Updates… (main.cpp:32324-32336) → §G1, with buttons
     [Check Collections Now…] [OK]; the first calls g_openCollections({}) and is HIDDEN,
     not disabled, if the hook is null.
12.2 View ▸ Check for Updates… (main.cpp:32227, the DMG-folder scanner) — its first
     question box (main.cpp:32233-32240) gains the first clause of §G2 so it names its
     object. Two "Check for Updates" entries pointing at different objects is exactly
     the confusion the honesty rules exist to prevent.
12.3 Neither may ever be shortened to a bare "Check for Updates".

=========================================================
STEP 13 — selftest (LibraryPane::selfTest, main.cpp:20151)
=========================================================
Add the pins listed in testPins. The three existing collection pins
(main.cpp:20164-20195) stay untouched.

=========================================================
ORDERING / COMMIT DISCIPLINE
=========================================================
Steps 6.1 and 7 must land in the SAME commit as step 3 (the split). Opening the window
offline puts a 258 MB Download button in front of the sweep's clicker, and puts a local
ZIP in front of a stale "matched" claim. Step 12 must land in the same commit as step 8;
shipping a background check while another menu says "the app never phones home" is the
tool lying in one dropdown about what it does in another.
