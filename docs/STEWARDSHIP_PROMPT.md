# MASTER PROMPT 4 — THE STEWARDSHIP ENGINE
*(successor to Prompt 1: surface honesty, Prompt 2: lived experience,
Prompt 3: total quality. Each audited a ring; this is the outermost ring —
the product as a LIVING THING: its releases, its irreplaceable data, its
many hands, its security, its future maintainers. Prompt 3's unfinished
waves [T2 sanitizer verdicts, T3 fuzz, T7 test gaps, T4 perf floors,
T8 menu reorg, T9 docs] are ABSORBED here as track Q and run to their own
exit criteria — nothing is orphaned.)*

## PART I — STANCE

**§1. Stewardship over craftsmanship.** The first three prompts made the
tool honest, livable, and sound. This one makes it SURVIVABLE: a tool that
protects the work entrusted to it (the authority's rulings, translators'
alignments, catalogers' stagings), survives its own updates, tells the
truth about its provenance to future maintainers, and fails safely in the
hands of people who will never read this repo.

**§2. The data hierarchy.** Rule 4 (provenance is sacred) extends
outward: the most irreplaceable bytes in this system are not the
dictionary (regenerable from releases) but the HUMAN JUDGMENTS — the
rulings store, staged catalog actions, glossaries, alignment links, notes.
Anything a human typed into this tool outranks anything the tool computed.
Every stewardship decision weighs that hierarchy.

**§3. Method inherited.** HUNT → FIX → PROVE → PRESS, with sabotage
tests, §10 adversarial review for behavior changes, the §11 visual gate,
the 59+-suite battery green at every commit, announced presses, ledger
docs/STEWARDSHIP.md, memory banked every wave.

## PART II — THE TRACKS

**S1. The many-hands problem (highest priority).** The proposals store,
catalog ledger, and roster live in a Dropbox-synced folder used by
MULTIPLE people. Hunt the whole conflict surface:
- Dropbox "conflicted copy" files: does the store's loader see them,
  ignore them, or silently fork history? Simulate one and prove it.
- Two approvers ruling at once; a cataloger staging while the authority
  approves; append-vs-rewrite races on the TSVs.
- Verdict + fix: conflict-safe formats (append-only where possible),
  conflicted-copy detection with an honest banner naming the file, and a
  documented merge path — never silent loss, never a guess.

**S2. Backup and recovery.** The human-judgment stores get a safety net:
- A rolling local backup (dated, bounded count) of the rulings store,
  roster, glossaries, links, and notes — automatic, quiet, and proven by
  a restore drill in the battery.
- Corruption handling: a truncated/garbled TSV must degrade to "N rows
  readable, file quarantined" — never crash, never rewrite the damaged
  original.

**S3. Release engineering.** The press is solid; the RELEASE around it
is informal:
- VERSION discipline: a release checklist that bumps VERSION, tags git,
  and writes a CHANGELOG entry assembled from the ledgers (the material
  already exists — the checklist makes it a habit).
- DMG integrity: verify the staged app runs from a clean mount;
  document the unsigned/unnotarized status honestly and what Gatekeeper
  will say on a fresh Mac (the input centers will hit this) — with the
  right-click-open instruction in the README and the Help.
- The blessed-shots baseline versioned alongside releases so a rollback
  can re-bless.

**S4. Security posture, honestly scoped.** The model is access
discipline, not cryptography (the roster's own words). Verify the
boundaries hold: no secrets in the repo or logs; the API key path
documented and excluded from backups/exports; FTP/SFTP passwords never
persisted (the tooltip's promise — prove it); the roster hash salted
per member (verify); harness runs never touch live credentials. Fix
what fails; document the trust model in one place for future eyes.

**S5. The maintainer's handbook.** The institutional knowledge of these
sessions lives in CLAUDE.md, ledgers, and memory files. Distill the
OPERATIONAL core into docs/MAINTAINERS.md: how to build, test, press,
bless, release; the inviolable rules and where they bite; the monolith
map; the edit disciplines (exact anchors, size asserts, the failed-assert
hazard); where every data store lives and its provenance; what is
externally gated and on whom. Written for a competent stranger.

**S6. Field feedback, closed-loop.** The Troubleshooting menu files
findings; the campaign doc collects them. Close the loop: a single
FINDINGS inbox file the app can append to, the ledger review cadence,
and the promise that every finding gets a ledger disposition. Adam's
outstanding campaign verdicts get a standing section so they never
silently expire.

**Q. The quality-engine remainder (absorbed from Prompt 3).**
T2 sanitizer verdicts → fixes; T3 engine fuzz with banked corpus;
T7 chrome test gaps (every P0-capable path pinned); T4 perf floors
pinned; T8 the Word-style menu reorganization (9l part 2) under the
§63 design governance; T9 docs-vs-UI truth pass. Same exit criteria as
written in Prompt 3 §11.

## PART III — ORDER AND EXIT

**§10. Order:** S1 (conflict surface — the live data-loss risk) → Q:T2
verdicts (already running) → S2 backup → Q:T3 fuzz → S4 security →
Q:T7 pins → S3 release → Q:T4 perf → T8 menus → S5 handbook → S6 loop →
Q:T9 docs. P0s jump the queue everywhere.

**§11. Exit.** Every track closed or explicitly ruled; the handbook
exists and a stranger could press a release with it; the backup restore
drill is a battery case; the conflict surface has no silent-loss path;
Prompt 3's exit criteria met. Close with a stewardship letter in the
ledger, memory banked, and the engine idling — re-armed at Adam's word.
