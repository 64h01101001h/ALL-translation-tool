# File Browser Programme — Path Finder × ForkLift 4 × Commander One
*2026-08-14 · Adam's commission: "plan on integrating all of the
features found in both Path Finder … and the ForkLift 4," extended
same day: "let's also look at Commander One file manager and do
some research to see what people say is best about its
functionality and use/integrate them where necessary."*

## Legal ground first
The GitHub org Adam linked (Path-Finder-Mac) was verified to be an
**unofficial promo repository** — one README, no source, no
license, one star. Adam suspected as much and he was right. Path
Finder (Cocoatech) and ForkLift 4 (BinaryNights) are both
**proprietary**. This programme therefore builds our OWN browser
in Qt, inspired by their feature *sets* — zero code from either,
no assets, no trademarks in our UI.

## Combined feature matrix (both apps, deduplicated)

### Core browsing
| Feature | Source | Verdict |
|---|---|---|
| Dual-pane browsing | both | **BUILD (P1)** — the spine of the design |
| Tabs per pane | both | BUILD (P1) |
| Breadcrumb path navigation | PF (namesake) | BUILD (P1) |
| Folders-above-files, smart sorting | PF | BUILD (P1) |
| Hidden-files toggle | both | BUILD (P1) |
| Full keyboard navigation + custom keys | both | BUILD (P1 core keys; P2 custom map) |
| Sync browsing (panes navigate together) | FL | BUILD (P2) |
| Workspaces (saved layouts) | FL | BUILD (P2, QSettings) |
| Spatial/orthodox modes | PF | ADAPT — orthodox = our dual-pane; spatial declined (niche) |

### Shelf & selection
| Drop Stack (persistent shelf) | PF | **BUILD (P1)** — perfect for gathering texts for a project |
| Quick Select (by name/ext/tag) | FL | BUILD (P2) |
| Quick Open palette | FL | ADAPT — extend our ⌘K Hunt with file targets (P2) |

### File operations
| Batch rename (patterns) | both | BUILD (P2) — scan pages/input files want this |
| Folder compare | both | ADAPT (P2) — our dmp edition-diff for text; size/date tree for the rest |
| Folder synchronization (1/2-way) | both | BUILD (P2) — double-keying partners exchange folders |
| Transparent archive browsing | both | BUILD (P2) — release ZIPs; via system `unzip`/`ditto` list+extract (no new deps) |
| Transfer queue (reorder, conflict rules, bandwidth) | FL | BUILD (P3, with connections) |
| Activity + Log views | FL | BUILD (P3) |
| App Deleter | FL | **DECLINE** — outside a translation tool's trust boundary |
| Root access / full ACL GUI | PF | **DECLINE** — safety; we never escalate privileges |

### Preview & info
| Preview panel (image/PDF/audio/text) | both | ADAPT — Library preview exists; unify + extend (P1/P2) |
| Hex viewer | PF | BUILD (P3, read-only viewer) |
| File compare hand-off | FL | ADAPT — route text pairs to our edition diff (P2) |
| Get-info / metadata module | PF | ADAPT — our catalog-aware info panel IS this, richer (P1 reuse) |

### Remote (ForkLift's crown)
| SFTP/FTP/WebDAV | FL | BUILD (P3) — input centers exchange with servers; SFTP via vendored libssh2 or `sftp` batch-mode wrapper (decide at P3) |
| S3/B2/Drive/OneDrive/Dropbox/SMB/AFP/NFS | FL | PARTIAL (P4) — S3 first (our collections live there); OAuth clouds declined v1 (secret-handling burden) |
| Remote editing w/ auto-upload | FL | BUILD (P3, atop connections) |
| Multi-server + favorites + iCloud favorite sync | FL | favorites BUILD (P2, local); iCloud sync declined (platform lock) |

### Utilities
| Terminal (open-in / embedded) | both | P2 open-in-Terminal.app; embedded emulator P4 (big) |
| Process viewer | PF | DECLINE — not our domain |
| Git support | FL | DEFER — until team workflows demand it |
| Tags (read/write macOS tags) | FL | BUILD (P2, xattr) |
| Themes | FL | ADAPT — we already have a design system; no theme engine |
| Share sheet | FL | BUILD (P2, NSSharingService via Qt) |
| Set-default-viewer | FL | DECLINE — the OS owns that choice |
| iOS-device browsing | PF10 | DECLINE v1 |

## Commander One (Eltima) — the deltas and the verdicts
Research 2026-08-14: official site + support docs + six review
sources (MacUpdate, App Store, AlternativeTo, Android Police,
mqdir, tokie). What users praise MOST: (1) one hub for
local + cloud + FTP + devices; (2) the strict Total-Commander
F-key model (F2 rename · F3 view · F5 copy · F6 move · F7 new
folder · F8 delete) — "muscle memory transfers cleanly" for
Total Commander veterans; (3) Android/MTP mounting; (4) archive
handling incl. RAR browse. What users criticize: paywalled
drag-and-drop, non-Mac shortcuts forced on everyone, weak
FTP-favorites/sync vs ForkLift, slow big trees, weak batch
rename.

| Commander One feature | Verdict |
|---|---|
| **TC F-key command set** (F2/F3/F5/F6/F7/F8) | **BUILD (P2) as an OPTIONAL "Commander keys" toggle** — their top-loved pattern; their top criticism is forcing it, so ours is opt-in and never displaces ⌘-conventions |
| Tabs persist across sessions | BUILD (P2, QSettings) — cheap, loved |
| F3 quick view (multi-format incl. hex) | ADAPT — our preview + P3 hex viewer already planned |
| Regex search incl. inside archives | BUILD (P2) — fold into Quick Select + ⌘K file targets |
| .zip/.jar/.ipa browsable as folders | ADAPT — same P2 archive-browsing lane |
| RAR browse/extract | DEFER — needs unrar dep; revisit if a partner ships RARs |
| Cloud mounts as local drives, client-side cloud encryption | DECLINE v1 — P3/P4 connection lanes stay as planned; encryption is out of scope |
| iOS/Android/MTP device mounting | **DECLINE** — device drivers are not a translation tool's business |
| Process viewer / quit process / restart-as-root | DECLINE (already declined for Path Finder — same reasoning) |
| Free-tier-with-paywall model | n/a — everything in our tool ships to everyone |

**Lessons taken from its criticisms** (as binding as the
features): native Mac conventions stay default everywhere;
drag-and-drop must simply work; batch rename (P2) must be
genuinely good, not a checkbox.

## What makes OURS different (the translation superpowers)
Every row above lands in a browser that already knows Tibetan:
catalog identity on every ACIP filename, English titles, author
links, scan routing, open-in-Overlay/Input, the citation locator,
and the survey. Path Finder and ForkLift browse files; ours will
browse *a canon*.

## Phases
- **P1 (now): the skeleton** — new **Files** pane: dual-pane
  QFileSystemModel browsers, per-pane path breadcrumbs, tabs,
  hidden toggle, folders-first sort, core keys, Drop Stack shelf,
  catalog-aware single-click info reuse, open-in-Overlay/Input.
- **P2: power ops** — batch rename, folder compare+sync, archive
  browsing, tags, Quick Select, favorites, workspaces, sync
  browsing, ⌘K file targets, open-in-Terminal, share, optional
  Commander-keys mode, persistent tabs, regex search.
  *Shipped 2026-08-14 (first P2 tranche):* batch rename
  (preview + collision-revert, selftested), pane compare,
  quick select (wildcard/regex), favorites, sync browsing,
  Commander-keys opt-in (F3–F8 + Trash), persistent tabs,
  ZIP browsing (zipinfo/unzip, never-overwrite extract),
  open-in-Terminal, multi-select + recursive folder copy/move,
  Move-to-Trash. *Remaining in P2:* tags, workspaces, folder
  synchronization, ⌘K file targets, share sheet, archive
  formats beyond ZIP.
- **P3: remote** — SFTP/FTP/WebDAV connections, transfer queue,
  activity/log, remote editing.
- **P4: long tail** — S3, embedded terminal, hex viewer.
Declines stay declined unless Adam overrules with reasons logged.
