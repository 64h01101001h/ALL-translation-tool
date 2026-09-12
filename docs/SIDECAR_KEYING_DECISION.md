# The properties sidecar's key — examined, and deliberately not changed

**2026-09-11. Decision: do NOT re-key now. The reasons are below, with the
design a later attempt would need.**

## The defect

Two keying schemes exist for a document's saved state, and they disagree:

| | key |
|---|---|
| `docprops::sidecarPath` — document properties | the **bare base name**, always |
| `docprops::versionsDir` — version history | `docKeyFor`: the stem inside the data root, `stem~<sha1 of path>` outside it |

So two drafts both called `draft.txt` in different folders **share one
properties record** — one revision counter, one accumulated editing time, one
"last saved by". Save As offers a bare `draft.txt`, which lands outside the
data root, so this is reachable in ordinary use rather than contrived.

The obvious fix is to re-key the sidecar on `docKeyFor` so the two schemes
agree. It was examined and refused for now.

## Why not now

Four findings, each verified in the code:

**1. Adoption cannot live where it would naturally go.** Five independent
places read the sidecar — the properties dialog, `noteSave` via `load`, the
Overlay's bookmark load and save, and `noteVersion`. But `noteSave(parent,
sidecar, seconds)` and `store(parent, sidecar, o)` receive a **finished path**
and have no `dataRoot`/`docPath` to reconstruct the legacy name from. Wire
adoption into the save path only, and the Overlay's `loadBookmarks` reads the
new empty key, every existing document loses its gutter bookmarks, and
`saveBookmarks` then writes that empty array over the new key — destroying the
legacy record on the next toggle.

**2. Adoption inside `sidecarPath` makes a pure path helper mutating**, and at
least one gate asserts a sidecar's **absence** by calling that helper for the
path it is about to check. A helper that renames a legacy file into place as a
side effect can create the very file the gate is testing is not there.

**3. `carryDocHistory` would not carry legacy records.** It tests
`QFile::exists(os)` at the **new**-key path. A document whose properties still
sit at the legacy bare name yields a new-key path that is absent, so nothing is
carried — and at the destination, adoption would look for the legacy name of a
**different** document. A rename or move performed before the user's first
post-upgrade save would lose the record permanently.

**4. It would have introduced a regression.** A move currently preserves the
properties record for free, because a bare-name key cannot change on a move.
Re-keying makes a move outside the data root change the key, so properties
would begin orphaning on every move — which is only safe once every pane
carries history. That was the finding worth having: **the Manuscript did not**,
and it was fixed on the strength of this check rather than the re-key.

## What a later attempt needs

- A `(dataRoot, docPath)` wrapper that **all five** read paths go through, so
  adoption happens once and early — at document open, not at save.
- Adoption that runs **eagerly**, before any rename or move can be attempted,
  so `carryDocHistory` never sees a half-migrated document.
- A rule for the ambiguous case: two different files can both claim the same
  legacy bare-name record, and exactly one may have it. Adopt for the first
  document opened and leave the record in place, or refuse both and say so —
  but never copy it to both, which would fabricate a history.
- Gates for the migration itself, not only the new keying: a legacy record is
  adopted, a legacy record is adopted **once**, and an adopted record keeps its
  revision count rather than restarting at 1.

## What was done instead

Nothing to the key. The check's genuinely urgent finding — the Manuscript
carrying no history on move and orphaning it on rename — was fixed the same
day. The sidecar collision remains, and is recorded here and in `TODO.md`
rather than left as an unexplained gap in the audit.
