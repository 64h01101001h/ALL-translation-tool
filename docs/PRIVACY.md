# What this application stores, and what leaves your machine

*Diamond Cutter Translation Tool · written 2026-08-24 for the P1 gap
"No privacy/data-handling statement". Every claim below was checked
against the source, and the file:line is given so you can check it too.*

The short version, because it is the part people actually need:

- **There is no telemetry.** No analytics, no crash reporting, no usage
  beacons, no phoning home on launch. Verified by search: no reference
  to any analytics or telemetry SDK exists anywhere in `app/`, `core/`
  or `ocr/`.
- **One feature sends your text to a third party**, it is off unless
  you turn it on, and it is described in full below.
- Everything else stays on your machine or in the folder your team
  already shares.

---

## 1. The one thing that leaves your machine

**The Analysis / AI back-check feature** sends the passage you are
working on and **your own draft translation** to Anthropic's API
(`api.anthropic.com/v1/messages`, `app/main.cpp:3433` and `:20046`),
along with the corpus and library citations the engine found for that
passage.

**It cannot run unless you configure a key.** `anthropicKey()`
(`app/main.cpp:2578`) looks in exactly two places — the
`ANTHROPIC_API_KEY` environment variable, and a file at
`~/Library/Application Support/Diamond Cutter Translation Tool/anthropic_api_key`.
With neither present the feature is inert and the app tells you so
rather than failing quietly. There is no bundled key and no default
endpoint credential.

If you use it, your draft is subject to Anthropic's terms, not ours.
If you do not want your work sent anywhere, **do not set a key** — the
rest of the application is fully functional without it, because the
deterministic core is the product and the AI layer is optional by
design.

*(The same function returns an empty key during test and sweep runs, so
an automated run can never spend money or transmit anything.)*

---

## 2. What is stored about a person

This is an in-house tool for a named team, so it records **who did
what**. That is the point of it, not a side effect.

| What | Contains | Where |
|---|---|---|
| **Catalog roster** | member name, and a salted hash per member | `library/`, shared |
| **Comments** | author name, timestamp, the text you wrote, the file and line | shared team folder |
| **Proposals and rulings** | who proposed, who ruled, when, and the reason given | shared team folder |
| **Dossiers** | your working desks: title, source path, line, timestamps | local only |
| **Recently-viewed scans** | file paths you opened, newest ten | local only |
| **Preferences** | pane settings, window state | local `QSettings` |

Two honest notes about the roster. Its hashes exist so a member can
identify themselves to the catalog, **not to protect a secret of
value** — the threat model states plainly that the roster is a record
of who did what and *not* a security boundary
([docs/THREAT_MODEL.md](THREAT_MODEL.md) §4, S6). And the team stores
live in a **shared Dropbox folder** (`core/include/allcore/comments.h`),
which means everyone on the team can read everyone else's comments and
rulings by design.

---

## 3. Network requests, all of them

Every outbound host the application can contact, and none is contacted
on launch — each is a lookup you initiate:

| Host | Why | Sends |
|---|---|---|
| `api.anthropic.com` | Analysis / AI back-check (§1) | your passage and draft |
| `purl.bdrc.io`, `library.bdrc.io`, `iiif*.bdrc.io` | scan lookups, IIIF page images | a work id or a title query |
| `treasuryoflives.org` | biography links | a person id |
| `84000.co`, `lotsawahouse.org`, `adarshah.org`, `thlib.org`, `sanskrit-lexicon`, `kmaps.virginia.edu` | reference link-outs | the term or work you clicked |
| `huggingface.co`, `buda-base.github.io` | OCR model download | nothing about you |
| `asianlegacylibrary.org`, `all-library-docs.s3.*` | update check, docs | nothing about you |

The rule the project works to: **the deterministic core stays offline.**
Dictionary lookup, conversion, segmentation, phonetics, search and OCR
all run locally. Network use is a convenience layer, user-initiated,
and labelled as such in the interface.

---

## 4. Where your data physically is

- **Your work** — texts, glossaries, dossiers, drafts — in the folders
  you chose, plus `~/Library/Application Support/Diamond Cutter
  Translation Tool/backups` (dated copies at each launch, most recent
  fourteen kept).
- **Team judgements** — in the shared folder your team configured.
- **Nothing is uploaded** to the Asian Legacy Library. There is no
  server that receives your work.

---

## 5. What we have not done

Stated because a privacy document that only lists reassurances is not
worth reading:

- **No retention policy for the shared team stores.** Comments and
  rulings accumulate indefinitely, by design — they are a permanent
  record of an authority's judgements. Nobody has decided how long that
  should be true, or who may delete an entry.
- **No deletion path.** There is no "remove my data" action. On a
  small in-house team this has not mattered; at an input centre with
  turnover it will.
- **No answer for a shared or multi-user machine.** The app assumes the
  person at the keyboard is the person named in the roster.
- **The DMG is not notarised** (SQA BUILD-3), so a recipient cannot
  cryptographically verify the build came from us.

---

*Questions or corrections: contact the Asian Legacy Library. If
something here is wrong, it is a defect and will be fixed in the next
release.*
