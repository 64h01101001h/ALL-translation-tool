# What an update does to your work

**Status: the promise this file makes is enforced by a versioned study record
and gated tests. Written 2026-09-11.**

The Diamond Cutter Translation Tool ships in two halves that move at different
speeds. The **dictionary and corpus** are a data release, re-issued as the
Geshe Michael Roach dictionary project publishes new work. The **application**
is a separate download. Either can be updated without the other.

Your own work is a third thing, and it belongs to you.

## The promise

**A data release never touches anything you made.** Importing one writes into
the `data/` folder — the dictionary master, the parallel corpus, the reverse
index — and rebuilds the search database from them. It does not read, move or
rewrite:

| your work | where it lives |
|---|---|
| glossaries you built | `library/glossaries/` |
| saved searches | application settings |
| document versions and their history | `library/versions/` |
| document properties and sidecars | `library/properties/` |
| snippets and templates | `library/snippets/`, `library/templates/` |
| the translator roster and proposals | `data/proposals/` |
| **your study record** — words met, drills, reading | `progress.db` |
| every preference, including House Style | application settings |

The import is also **staged, not overwritten in place**: each file is copied
beside the old one and swapped in only once the copy succeeded. A failed or
interrupted import leaves the previous release working, and says so.

## Your study record is versioned

`progress.db` is the one file in the product holding something you cannot get
back. A dictionary can be downloaded again; a year of study cannot. So it
carries a **format number**, and the application handles it explicitly:

- Opening a record written by an **older** version upgrades it in place, adds
  whatever is new, and **leaves every existing row untouched**. Your ease
  factors, intervals, counts and history survive the upgrade unchanged.
- Opening a record written by a **newer** version is **refused**, with a
  message saying so. An older build cannot know what a newer one's columns
  mean, and writing anyway could destroy work the newer build can still read.
  Updating the app is the fix; there is never a silent downgrade.
- Changes to the format are numbered steps that run once. A shipped step is
  never renumbered or edited, and no step drops or rewrites a column holding
  your work.

Eight tests in `core/tests/progress_smoke.cpp` hold this: a record written in
the original format upgrades, keeps its rows, and records its new format; a
record from the future is refused with a message a person can act on; and
opening the same record twice does not fail. That last one is not theoretical
— an unguarded migration crashed the application on the second launch of any
study record, and it reached a press build on 2026-09-11 because every test
database until then happened to be new.

## What this file does NOT promise

Said plainly, because a guarantee with an unstated edge is worse than none:

- **The dictionary's contents will change between releases.** Equivalents are
  added, corrected and re-tiered as the project's own review proceeds. A
  glossary you built that quotes an entry keeps what you saved; it does not
  silently follow the entry.
- **Nothing here backs your work up.** The guarantee is that an update will
  not harm it, not that a disk failure cannot. Copy `library/` and
  `progress.db` somewhere safe if the work matters.
- **The application's own settings can be reset** by the usual macOS means,
  and that is outside anything an update controls.
- **Downgrading the application is not supported** once a newer format has
  been written. Keeping the older build available does not help; it will
  refuse the record rather than damage it.

## Where the files are

    ~/Library/Application Support/ALL/Translation Tool/
        data/         the dictionary release — replaced by an import
        library/      your documents, glossaries, versions, snippets
        progress.db   your study record
        build/        the search database, rebuilt from data/

Everything under `data/` and `build/` can be rebuilt from a release package.
Everything else is yours and is never regenerated.
