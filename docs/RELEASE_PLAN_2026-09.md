# Release plan — public 1.0 in two weeks (Adam, 2026-09-08)

*Adam's directive, 2026-09-08: pause the alignment-dictionary campaign
(C05 stands at 165/511, ledger records the pause), go back to the TODO
list, walk every function of every pane one by one, and ship a public
release in about two weeks. After that, dictionary data is updated
weekly and pushed to the copies on GitHub; users must be told on their
desktop when a new dictionary is available and be able to install it.*

Everything below is checked against the repo on 2026-09-08. Where a
number is quoted it was measured that day.

## 0. Where we stand

| Fact | Value |
|---|---|
| Version string | `1.0.0-rc.1` (`VERSION`) |
| Panes in the ribbon | 22 tabs in 7 groups (Read, Translate, Research, Learn, Input, Catalog, Community — ⌘1…7) |
| Battery | 101 ctest suites; app selftest inside suite `app_selftest` |
| Press | `tools/package_macos.sh` — gated DMG (release gate → Release build → battery → macdeployqt fixup → ad-hoc sign → arch/floor check → data manifest → SBOM → launch test → DMG). Checklist: `docs/RELEASE_CHECKLIST.md` |
| Distribution docs | `docs/distribution/` (white paper, technical paper, complete feature guide, install quick start, data & privacy, authority admin guide, open-source notices) |
| Source hosting | NO GitHub remote. Only `backup` (a mirror on the 8 TB drive). `gh` is logged in. |
| Update channel | App: View → "Check for Updates…" scans a *team folder* for newer DMGs. Data: Library → Maintenance → "Import data release…" rebuilds the spine from a data package via the bundled builder. Neither touches the network for *our* releases; nothing notifies. |
| Signing | Ad-hoc. First launch is right-click → Open. Notarisation needs an ALL Apple Developer ID. |
| Licence | NONE for our own code (P1 in TODO). Blocks a public release lawfully. |

## 1. The function audit (week 1)

Goal: every user-reachable function in every pane has been looked at by
a person, with a disposition: **SHIP** (works, tested, honest label) ·
**HIDE** (remove or disable for 1.0, keep code) · **FIX** (named
defect, fix before press) · **LABEL** (works but the UI overclaims —
add PROVISIONAL / experimental wording).

Method:
1. Machine inventory first. `docs/release_audit/<Pane>.md` — one file
   per pane, generated 2026-09-08 by reading the code: every button,
   action, shortcut, link scheme and member function, with its code
   anchor, its automated coverage (selftest check / ctest suite / NONE)
   and its risks. This is the checklist we step through.
2. Walk it with Adam, pane by pane, in the running app. Order = ribbon
   order, largest risk first inside each group:
   1. Read: Overlay → Library → Files → Scans → Export
   2. Translate: Draft → Manuscript → Apparatus → Review → Align
   3. Research: Search → Lookup → Sanskrit → Convert → Analysis
   4. Learn: Trainer → Drills
   5. Input: Input → OCR
   6. Catalog: Catalog (SIDELINED per TODO — candidate HIDE)
   7. Community: Propose → Approval
   8. Main window: menus, Settings, Help, Hunt palette, About, diagnostics
3. Each disposition is written back into the pane's audit file the same
   day (a row gets a `Disposition` column). FIX items become TODO
   entries under a `## RELEASE 1.0 — FIX LIST` heading; HIDE items are
   implemented behind one `kReleaseHidden` list so they can return
   without archaeology.
4. Nothing ships on the strength of its label: a function with NONE
   coverage either gets a selftest check or is exercised by hand and
   the result recorded in the audit file.

Budget: Overlay alone is ~11k lines / ~117 functions; the whole app is
~430 member functions across 27 classes. At two panes a day the walk
takes the working week; Overlay gets a day of its own.

## 2. The P1 items the audit does not cover (week 1, in parallel)

From TODO.md "SOFTWARE ENGINEERING DISCIPLINE AUDIT — P1":

- [ ] **LICENSE file** — an ALL decision. Recommend Adam asks ALL this
      week; the app cannot be *publicly* released without it. Candidate
      defaults if ALL has no policy: Apache-2.0 for code (patent grant,
      institution-friendly); data layers keep their own terms as the
      SBOM already records (24 layers TERMS OPEN — those must be
      resolved or the layers pulled from the public DMG).
- [ ] **Defect intake** — an email alias (or GitHub Issues once the repo
      exists) printed in About and in Help → Save Diagnostic Report.
- [ ] **Update channel** — §3 below.
- [ ] **Crash handler** — none exists (no `std::set_terminate`, no
      signal handler). Minimum: write the lifecycle log line and the
      diagnostic report on abort. One day of work; decide if it is in
      1.0.
- [ ] **Data-migration statement** — what happens to a user's
      glossaries, saved searches, roster and progress.db when the
      dictionary updates weekly. Must be written before §3 ships,
      because §3 is exactly the event that could clobber them.
- [ ] **User manual** — `docs/distribution/COMPLETE_FEATURE_GUIDE.md`
      exists; the audit will tell us which sections describe HIDDEN
      features and must be cut.

## 3. GitHub hosting, weekly dictionary releases, desktop notification

### 3a. Hosting (decision needed: public or private repo?)

Two repositories, because the data and the code move at different speeds
and have different licences:

- `ALL-translation-tool` — the app source. GitHub Releases carry the
  DMG (`Diamond-Cutter-Translation-Tool-<ver>.dmg`) plus
  `BUILD_MANIFEST.json` and the release notes.
- `ALL-dictionary-releases` — data only. Each weekly release is one
  GitHub Release tagged `data-v<X.Y.Z>` carrying the release package
  the app already knows how to import (the same format
  `tools/validate_release.py` gates and "Import data release…" reads)
  plus a `sha256`.

One tiny **manifest** at a stable URL is what makes both automatic:

```
https://github.com/<org>/ALL-dictionary-releases/releases/latest/download/manifest.json
{
  "data":  {"version": "27.3.0", "url": ".../data-v27.3.0/hgm_release_v27_3.zip",
            "sha256": "…", "bytes": 123456789, "notes_url": "…", "published": "2026-09-15"},
  "app":   {"version": "1.0.1", "url": ".../v1.0.1/Diamond-Cutter-Translation-Tool-1.0.1.dmg",
            "sha256": "…", "min_data_version": "27.2.0"}
}
```

`releases/latest/download/<asset>` is a stable GitHub URL that always
resolves to the newest release's asset, so the app never needs the
GitHub API. Publishing a week's dictionary = run the release gate,
`gh release create data-vX.Y.Z pkg.zip manifest.json`. That is a
one-command script (`tools/publish_data_release.sh`, to write).

### 3b. In-app: check, notify, install

New, small, and built on what exists:

1. **Check** — on launch (after the window shows, 10 s delay) and once
   every 24 h while running, GET the manifest with `TimedNam` (already
   in main.cpp). Compare `data.version` against the installed data
   version (the spine's own provenance / `spine_current.txt`) and
   `app.version` against `ALL_APP_VERSION` with the existing
   `versionLess()`. Failures are silent to the user and one line in the
   lifecycle log; the app never blocks on the network.
2. **Notify** — a macOS desktop notification via
   `QSystemTrayIcon::showMessage` (Qt routes it to Notification Center
   on macOS; no tray icon has to stay visible) *and* a persistent
   non-modal banner in the main window: "Dictionary 27.3.0 is available
   (you have 27.2.0) — Install… / Release notes / Later". Later hides it
   until the next version. A Settings toggle "Check for dictionary and
   app updates" (default ON) — and `docs/PRIVACY.md` gains the new host
   (github.com / objects.githubusercontent.com) and exactly what is
   sent (nothing but the GET).
3. **Install** — "Install…" downloads the package to a temp dir with a
   progress dialog, verifies `sha256`, then hands it to the *existing*
   Import-data-release pipeline (validate → copy → run the bundled
   builder → switch `spine_current.txt` only on success). Nothing new is
   trusted: a bad checksum or a failed build leaves the current
   dictionary untouched, exactly as today. App updates: notify and open
   the DMG URL in the browser (no self-replacing binary while we are
   ad-hoc signed; Sparkle is deferred until there is a Developer ID).
4. **Fallback** — the team-folder "Check for Updates…" stays as the
   offline path (input centres without internet).

Tests: a fixture manifest served from a local file URL drives a selftest
(newer → banner shown; equal → no banner; malformed → silent, logged;
bad sha256 → refused, dictionary untouched). Mutation-verified like the
other gates.

Estimate: 2–3 days including tests and the privacy/manual updates.

### 3c. What "weekly dictionary update" must NOT do

- Overwrite user-owned data: per-text glossaries (`*.tsv` beside the
  document), saved searches, roster, `progress.db`, proposals. These
  live outside the spine and are untouched by an import — verify and
  state it in the migration note (§2).
- Change the honesty labels. Whatever the campaign banks later stays
  PROVISIONAL in the UI; a weekly push does not promote anything.

## 4. Week 2 — fix, press, prove

1. Work the FIX list (target: closed by day 10).
2. Implement HIDE list; regenerate the feature guide without hidden
   features.
3. `VERSION` → `1.0.0`; release notes from git history.
4. Press: `bash tools/package_macos.sh` — full gate.
5. **Clean-Mac test** — the DMG on a machine that never had Homebrew or
   the dev tools (docs/RELEASE_READINESS.md step 5). This is the one
   test of the experience the public will have; it is not optional.
6. Create the GitHub repos, push, `gh release create v1.0.0 <dmg>`,
   publish the first data release + manifest, confirm the installed app
   sees "up to date".
7. Tag, back up, write `docs/distribution/RELEASE_NOTES_1.0.0.md`.

## 5. Decisions Adam owns (ask early — they gate the date)

1. Licence for our code (from ALL).
2. Public or private GitHub repos; org name.
3. Apple Developer ID for notarisation — or accept right-click → Open
   for 1.0 and say so in the install guide.
4. Which panes are HIDDEN for 1.0 (Catalog is the obvious candidate;
   Propose/Approval need a shared folder to mean anything).
5. Defect-intake address.
6. Whether the 24 data layers with TERMS OPEN ship in a public DMG.
