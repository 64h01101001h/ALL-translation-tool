# The in-house proposal & approval channel

*Adam's design, 2026-08-08. Translators propose; the authority
(Geshe Michael, or Adam as second admin) approves — without taking
liberties with the corpus.*

## What it is

A way for the translation team to offer Geshe Michael a reviewable
list of proposed honorific and high-honorific words and verbs,
abbreviation candidates, pronunciation exceptions, and possible
renderings for words, phrases, and clauses — and for him (or Adam) to
approve or decline each one from an authority-only tab, with every
decision stamped and preserved.

## Deliberately not a login server

It is **identity, not authentication**. Each installation records a
name and a role (translator / authority) once; that name rides along
on every proposal as provenance ("proposed by Tenzin, 2026-08-14").
Within a trusted in-house team this is the honest model — it gives
attribution and an audit trail without accounts, passwords, or a
server to run or secure, and it keeps the app's offline-first design
intact.

## The store is a folder of files — point it at Dropbox

Every proposal is one row in `<proposals folder>/proposals.tsv`. Set
that folder to a path inside the shared **Asian Classics Dropbox** and
the whole team's queue synchronizes with zero infrastructure: it works
offline, syncs when online, and leaves a complete paper trail. The
store is `allcore::ProposalStore` (37th test suite,
`proposals_smoke`), with tab/newline-safe escaping so a pasted passage
survives as evidence.

## The boundary that protects the corpus

The app's founding rule is that it imports data releases and never
owns the data. The channel honors it by routing two kinds of proposal
differently:

- **Register categories** — honorific, high-honorific, pronunciation,
  abbreviation — are app-native layers. On approval they apply **in
  the app immediately**, appended to the register file with the
  approved tier and a provenance comment (`# approved by <name>
  <date>`).
- **Dictionary / corpus categories** — word, phrase, and clause
  renderings, and notes — are approved in the app but never written
  into the corpus. The authority's **"Export approved dictionary
  candidates"** produces a signed TSV for the data project to ingest
  into the next release. Same one-click approval for Geshe Michael;
  the master data keeps its single pipeline and provenance chain.

## Using it

- **Propose tab** (everyone): a one-time identity setup (name, role,
  proposals folder), then a form — pick the kind, enter the Tibetan,
  the proposed value, optional secondary field (ordinary form, domain,
  level), and evidence — and file it.
- **Approval tab** (authority only; appears when the role is set):
  the pending queue, each item showing its evidence and proposer, with
  **Approve · Decline · Defer** and an optional note. Register
  approvals apply on the spot; dictionary approvals collect for export.

## Roadmap (v2 and beyond)

1. ~~Propose from context~~ — DONE 2026-08-08: the Overlay entry
   card carries a "propose to the authority…" link beside its
   glossary action. One click asks the kind (honorific / high
   honorific / pronunciation / abbreviation / word rendering / note),
   the proposed value, and files it — with the term pre-filled and
   the **surrounding source line auto-captured as evidence** (plus the
   file name). Honorific proposals correctly route the ordinary
   counterpart into the register's secondary column. Still to extend
   to the Lookup card (same anchor pattern).
2. **Approved-register regeneration** — fold approved rows into the
   canonical register TSVs at release time rather than appending live,
   so the derivation tools and the approved additions stay one file.
3. **Notification counts** — a pending-count badge on the Approval tab
   so the authority sees at a glance when the queue has grown.
4. **Signed decisions archive** — the ruling history is already
   preserved in the store; a read-only "Rulings" view would make that
   record browsable as the scholarly artifact it becomes over time.
