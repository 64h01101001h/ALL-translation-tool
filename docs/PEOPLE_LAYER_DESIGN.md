# People Layer — BDRC Persons + Treasury of Lives
*Design banked 2026-08-10 (Adam's request: "a way of working with all
of the famous people and authors and gurus and teachers in both BDRC
and The Treasury of Lives"). Task #59; queued behind #58 (GMR
highlighting directive).*

## Survey findings (verified live, 2026-08-10)

1. **BDRC person records are rich and open.** `purl.bdrc.io/resource/
   P<id>.json` returns full JSON-LD: prefLabel in EWTS + other
   languages, personName nodes (many name forms), birth/death and
   office events with dates, roles (R-ids), notes with sources.
   Proven on P64 (Tsongkhapa).
2. **BDRC carries the Treasury of Lives cross-reference.** P64 →
   `owl:sameAs http://api.treasuryoflives.org/resource/TOLP8986`.
   One pipeline serves both sources.
3. **Treasury of Lives licensing is favorable.** Metadata is CC0
   (fully reusable); biography ESSAYS are licensed for non-commercial
   reading/linking — so we bank metadata and LINK OUT to essays,
   never copy them. (treasuryoflives.org/terms)
4. **ToL API is Cloudflare-gated for scripts** (browser fine) — so
   the app links users to ToL pages; harvesting goes through BDRC.
5. **Persons_all is capped at 500 rows** (verified 2026-08-11:
   numResults=500 regardless of pageSize) — NOT a full-dump route.
   Harvest therefore goes per-person: resolve our catalog's author
   names / work records to P-ids, then fetch each person's JSON
   (proven rich). Original note:
5b. **Bulk access:** `ldspdi.bdrc.io/queries` hosts registered SPARQL
   templates; a `Persons_all` template exists (HTTP 200). Fallback:
   per-person JSON fetch for the bounded author set (see below).
   BDRC API code is Apache-2.0; record exact DATA license terms in
   data/licenses/ at harvest time.

## Design

**Scope rule: start from OUR library, not from all of BDRC.** The
catalog already joins texts to BDRC works for the scan feature; a
BDRC work (MW/W) names its creators as P-ids. Harvest = the closure
of: persons who authored works in our catalog + all ToL-linked
("famous") persons from Persons_all. This keeps the register
relevant and small.

**Harvest tool (build-time, network; NOT app runtime):**
`tools/fetch_persons.py` →
`data/extracted/persons_bdrc.json`, one row per person:
`pid, names[] (ewts + tibetan unicode via our own converter), dates
(birth/death/floruit), roles[], tol_id, works[] (ACIP numbers where
they map into our library), source: "BDRC", license note,
fetched_date`. Provenance rule 4 applies: every row says where it
came from and when.

**allcore:** a People register (load JSON once; name index keyed by
affix-normalized EWTS so PA'I-type forms still hit). API:
`peopleByName(wylie) -> matches`, `personById(pid)`.

**UI (increment order):**
1. **Lookup**: a PERSON section when the query matches a person name
   — dates, roles, name forms, links out to the ToL biography and
   the BDRC library page, plus "their works in your Library" (click
   → opens in Overlay).
2. **Library**: author names in the catalog table become clickable →
   the same person card.
3. **Overlay card**: when a clicked span matches a person name, the
   card carries a PERSON badge with the same content — a reader
   meets Tsongkhapa in a colophon and is one click from his life.
4. (Later) a People browser under Research: filter by role/century,
   the lineage at a glance.

**Honesty rules applied:** person data is REFERENCE (never HGM
material); essays are link-outs, never embedded; name matches are
exact-or-affix-normalized only — no fuzzy guessing between similarly
named lamas (homonymous teachers are legion; when several match, ALL
are shown with dates so the human disambiguates).

## Increments
1. fetch_persons.py + banked persons_bdrc.json + license record.
2. allcore People register + battery (name index round-trip, known
   P64 lookup, homonym listing).
3. Lookup person card + Library author links + selftest checks.
4. Overlay card badge; People browser later if wanted.
