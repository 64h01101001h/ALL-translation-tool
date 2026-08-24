# Author reference metadata on the PERSON card — design

*Adam, 2026-08-23: "it would be nice to have total reference of
metadata associated with each author, like in the screenshot" — a
Treasury of Lives sidebar for Gyaltsab Je showing offices held,
teachers, students, historical period, institutions, name variants and
Wylie. Supersedes nothing; absorbs the author-portrait request from the
same day, because portraits arrive in the same fetch.*

Status: DESIGN, not yet approved for implementation.

---

## 1. What already exists

This is smaller than it looks. The surface is built.

| Piece | Where |
|---|---|
| Roster of 462 people, keyed by **BDRC person id** | `data/extracted/author_index.json` |
| PERSON card renderer | `app/main.cpp:22115` (`g_personCardByName`) |
| Card is already injected into lookup results | `app/main.cpp:2290` |
| Links out to BDRC and Treasury of Lives already | same lambda, with `tolLinkForPid` |
| Honest "(matched via Wikidata)" label already | same lambda |
| Author link schemes (`author:`, `authorname:`, `authortable:`) | `app/main.cpp:22417` |
| Tiered name search, 5 tiers, homonyms never merged | `core/include/allcore/authorsearch.h` |

The card shows a name, dates, and two outbound links. Everything in
Adam's screenshot beyond that is missing.

## 2. Source decision, and why

**Structured metadata comes from Wikidata (CC0). Prose stays at
Treasury of Lives, reached by a link.**

Treasury of Lives pages and biographies are copyrighted (Shelley &
Donald Rubin Foundation). Harvesting them into a shipped DMG is the
BUILD-5 shape — payload whose terms were asserted rather than
established — which this project closed on the same day this was
requested. Wikidata is CC0, carries the same relations, and its
per-file terms are machine-readable.

Measured 2026-08-23 for ToL 9095 (Gyaltsab Je), Wikidata returned 55
statements covering the screenshot almost line for line:

| Screenshot section | Wikidata property |
|---|---|
| TEACHERS | `student of` (P1066) |
| STUDENTS | `student` (P802) |
| OFFICES HELD | `position held` (P39) |
| HISTORICAL PERIOD | `date of birth`/`date of death` (P569/P570) |
| Wylie / native name | `name in native language` (P1559) — returned `རྒྱལ་ཚབ་རྗེ་` |
| (portrait) | `image` (P18) |

Plus 20 library authority ids (VIAF, LoC, GND, ISNI, Open Library).

**INSTITUTION is the one section Wikidata does not answer well.**
`educated at` / `member of` exist but are sparse in this corpus. The
design does not fake it: absent means absent.

## 3. The join

Join on **BDRC person id via `P2477`**, which is the key the roster is
already built on. Fall back to Treasury of Lives id via `P4138` for
anyone `P2477` misses.

Coverage measured 2026-08-23:

| | ToL-linked (193 of 462) | ToL-less (269 of 462) |
|---|---|---|
| Resolve on Wikidata | 60/60 (100%) | 72/80 (90%) via `P2477` |
| Have teachers | 57/60 | 63/80 |
| Have students | 51/60 | 62/80 |
| Have a portrait | 20/60 | 19/80 |

Projected: **~435 of 462 reachable (94%)**. Portraits are a minority
feature at **~28%** — an earlier 75% estimate came from a sample of 12
and was wrong; this figure is over 140.

## 4. Provenance and tier — the load-bearing part

Every fact harvested here is **REFERENCE tier**. It is never the ALL's
own assertion, never rendered with binding authority, and never uses
the reserved greens (`#1E6B4E`, `#2E7D32`).

Three rules, all inherited rather than invented:

1. **Label the source on the card.** The card already says
   "(matched via Wikidata)" for inferred ToL links; relations get the
   same treatment. A reader must be able to tell an ALL fact from a
   Wikidata fact without clicking anything.
2. **Conflicts are surfaced, never resolved.** Where Wikidata's dates
   disagree with the roster's, show both and mark the disagreement.
   House rule 3: never guess. (Gyaltsab Je agrees — 1364–1432 both
   sides — but disagreements will exist across 435 people.)
3. **Absence is stated, not hidden.** A person with no recorded
   teachers renders no TEACHERS section, and a person Wikidata does not
   know renders the card exactly as it does today. No section is ever
   padded to look complete.

Note for the record: a good deal of Wikidata's Tibetan biographical
data is itself derived from Treasury of Lives. That makes it a
secondary source twice over, which is precisely why it is REFERENCE.

## 5. Data shape

Harvest at build time; ship a sidecar. No runtime network on this path
— the deterministic core stays offline, and input centres cannot rely
on connectivity.

`data/extracted/author_meta.json`:

```
{
  "_meta": {
    "source": "Wikidata (CC0)",
    "join": "BDRC person id via P2477, ToL id via P4138 as fallback",
    "retrieved": "YYYY-MM-DD",
    "query": "<the exact SPARQL, so the harvest is reproducible>",
    "people": <n>, "with_teachers": <n>, "with_students": <n>
  },
  "people": {
    "P65": {
      "qid": "Q…",
      "native_name": "རྒྱལ་ཚབ་རྗེ་",
      "dates": {"born": "1364", "died": "1432",
                "conflicts_roster": false},
      "teachers": [{"qid": "Q…", "label": "Tsongkhapa",
                    "roster_pid": "P64"}],
      "students":  [{"qid": "Q…", "label": "Khedrubje Gelek Pelzang",
                     "roster_pid": "P66"}],
      "positions": ["Ganden Tripa"],
      "authority_ids": {"viaf": "…", "loc": "…"},
      "portrait": {"file": "portraits/P65.jpg",
                   "licence": "Public domain",
                   "source_url": "https://commons.wikimedia.org/…"}
    }
  }
}
```

`roster_pid` is the whole point: a relation that names somebody already
in the roster becomes a **click-through to their works**. That turns a
fact sheet into a navigable lineage.

Every shipped file gets its `PAYLOAD_MANIFEST.txt` row — `CC0` for the
JSON, per-image licence for portraits — or the press fails. That gate
exists as of `a5f2e09`.

## 6. The harvest tool

`tools/build_author_meta.py`, following `tools/build_author_index.py`'s
conventions.

- Reads the roster, batches SPARQL by `VALUES`, writes the sidecar.
- Records the query and retrieval date in `_meta` so a future run is
  comparable and the provenance is auditable.
- **Never partially overwrites.** Stage-then-swap, like the data
  release importer. A half-written sidecar must not become the shipped
  one.
- Fails loudly on an unreachable endpoint rather than writing a thin
  file that looks like "these people have no teachers."
- Portraits downscaled to ~400px. At ~28% coverage that is roughly
  130 images; full size would add ~29 MB to a DMG from which 406 MB
  was just removed.

## 7. Rendering

Extend `g_personCardByName` (`app/main.cpp:22115`). No new pane.

Sections in the screenshot's order, each omitted when empty: offices
held · teachers · students · period · name variants · native name.
Relations render as links where `roster_pid` resolves and as plain text
where it does not — the difference is honest and visible.

## 8. What gets pinned

Written before the code, mutation-verified, per the day's standing
lesson that a pin which cannot fail is worse than no pin.

1. A person with harvested relations renders them, with the Wikidata
   label present. Mutant: drop the label → red.
2. A person with **no** Wikidata match renders exactly today's card.
   Mutant: render an empty TEACHERS heading → red.
3. A date conflict renders both values and the flag. Mutant: silently
   prefer one → red.
4. A relation naming a roster member renders as a link; one that does
   not renders as text. Mutant: linkify everything → red.
5. The harvest tool refuses to write a partial sidecar on a failed
   fetch. Mutant: write what arrived → red.

## 9. Out of scope

- Treasury of Lives prose. Link only.
- INSTITUTION, until a CC0 source for it is found.
- Runtime lookup for people the harvest missed. Offline-first; revisit
  only if a real user asks.
- Any promotion of this material toward binding tier. It is reference,
  permanently.

## 10. Open question for Adam

The screenshot's TEACHERS list mixes registers — `rin chen rgyal
mtshan` (wylie) beside `Rendawa Zhonnu Lodro` (phonetic English).
Wikidata labels are English-ish and inconsistent the same way. Do you
want the card to show Wikidata's label as-is (faithful to source,
visibly uneven), or to run names through the project's own
converters for a consistent register (tidier, but a transformation of
somebody else's data)? I lean **as-is with the source labelled**, on
the same principle that keeps reference layers unmerged.
