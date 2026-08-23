# STEWARDSHIP ENGINE — LEDGER
Per docs/STEWARDSHIP_PROMPT.md. The data hierarchy governs: human
judgments outrank computed data.

## S1 — THE MANY-HANDS SURFACE (closed 2026-08-20)
Hunted the proposals store as the shared-Dropbox reality it lives in.
- **S1-01 · P0 · FIXED.** save() truncated and rewrote proposals.tsv —
  last writer wins. A proposal filed on one machine while the authority
  ruled on another could vanish into a "conflicted copy" nobody reads.
  save() now reload-merges first: rows with ids we have never seen are
  preserved; our own ids stay authoritative (single-authority model
  rules same-id edits). A never-loaded store can no longer wipe the
  queue (the smoke test relied on exactly that wipe and was corrected
  to an explicit file reset).
- **S1-02 · P1 · FIXED.** load() was blind to Dropbox "conflicted copy"
  siblings — forked queue history was invisible. Now: siblings are
  detected, their UNSEEN rows join the queue (absorbed), same-id rows
  that differ are COUNTED but never guess-merged (main file wins; the
  copy stays on disk for a human). The Approval pane banners the event:
  file names, rows absorbed, rows diverging.
- **S1-03 · P1 · FIXED.** Proposal ids were kind-date-count — two
  machines proposing the same day collided, and a ruling addresses rows
  BY id. The proposer's initials now join the scheme.
- **Proof**: a battery drill fabricates a conflicted copy with one new
  and one diverged row and pins detection/absorption/divergence/
  main-wins; battery run twice back-to-back to prove the drill cleans
  up after itself (first version didn't — caught and fixed).

## Q:T2 — SANITIZED BATTERY (closed 2026-08-20)
Full ASan+UBSan build, all 59 suites: ONE failure — a heap-use-after-
free in abbr_smoke itself (byWylie pointers held across a second
load(); reallocation dangled them). Test re-fetches; the lifetime
contract is now documented in abbr.h. App call sites verified: loads
complete before queries. Sanitized battery green.

### Q:T2 has machinery now (MEM-2, 2026-08-23)
That run was a manual one-off, and it stayed one: `grep -rn fsanitize`
across every CMake/sh/py file returned **0**, so nothing in the tree
could be sanitized without re-deriving the flags by hand. It can be
now, and by name:

    tools/sanitized_battery.sh          # configure + build + full ctest
    cmake -S . -B cmake-build-asan -DALL_SANITIZE=ON     # by hand

`ALL_SANITIZE` (root `CMakeLists.txt`, OFF by default) puts
`-fsanitize=address,undefined -fno-sanitize-recover=undefined` on
allcore, allocr, the app and every suite. It is deliberately NOT on
`tools/verify.sh`: the everyday gate stays fast, and this is a
release-time and memory-work gate. Leak detection stays off — macOS
arm64 has no usable LSan.

Recorded run, 2026-08-23, at 73 suites (the closed run above covered
59): configure + full sanitized build clean, **73/73 green** on two
full runs (100.7 s and 71.0 s of test wall time), zero ASan reports and
zero UBSan diagnostics. The machinery was then proved to bite:
reinstating bounty #10's fixed 64-slot depth table in
`OverlayPane::saBcadShapeOf` gave

    ==99558==ERROR: AddressSanitizer: heap-buffer-overflow
    WRITE of size 4 ... in OverlayPane::saBcadShapeOf(...)
    0 bytes after 256-byte region        EXIT=134

so a green sanitized battery here means something. (MEM-1 is the
reason it must: a pin that can only fire under a sanitizer is dead
code while nothing runs one.)
