# THL (thlib.org) — Collections & Catalogs Research

Researched 2026-08-12 for the Diamond Cutter Translation Tool (Library pane / BDRC links / citation layers).
Method: live WebFetch of thlib.org and its satellite sites; WebSearch where pages were dead or JS-only.
**Licensing rule applied throughout: unknown means unknown — nothing is assumed ingestable.**

## Site-structure reality check (important before anything else)

- `https://www.thlib.org/` is the **current** homepage (nine sections: Collections, Audio-Video, Images, Subjects, Places, Tibetan Terms, Modern Texts, Bibliographies, Classical Literature). It calls itself "an open online library" where "all content is open access and built collaboratively" — a marketing statement, **not** a license.
- Most deep links on the legacy path 301-redirect to **`https://old.thlib.org/...`** — the old PHP site is still alive there and is where the canons catalogs and JIATS actually live.
- `https://www.thlib.org/collections/` itself returns **404**; the working legacy index is `https://old.thlib.org/collections/`.
- The old catalog/essay pages are hash-fragment JS apps, but they honor the Google AJAX crawling scheme: replace `#foo` with `?_escaped_fragment_=foo` to get server-rendered content. Example that works:
  `https://old.thlib.org/encyclopedias/literary/canons/index.php?_escaped_fragment_=essay%3D/stanley/tibcanons/s/b23`
- The media collections migrated: SHANTI/UVa "Mandala" platform (`av.mandala.library.virginia.edu`, `images.mandala.library.virginia.edu`, `texts.mandala.library.virginia.edu`). These are JS SPAs that serve an **empty shell to non-browser fetchers** — they are alive (search engines index them) but could not be read programmatically in this pass.
- The Places/Subjects backends are the SHANTI **Knowledge Maps (kmaps)** Rails apps at `places.kmaps.virginia.edu` and `subjects.kmaps.virginia.edu` — these DO have a working, unauthenticated JSON API (verified below).

---

## 1. Tibetan text collections and catalogs

### 1.1 The Tibetan Canons catalogs (Kangyur/Tengyur)

Sources:
- https://old.thlib.org/encyclopedias/literary/ (Literary Encyclopedia hub)
- https://old.thlib.org/encyclopedias/literary/canons/
- https://old.thlib.org/encyclopedias/literary/canons/kt/catalog.php (the catalog app)
- Canon-history essay (David Stanley), served via `?_escaped_fragment_=essay%3D/stanley/tibcanons/...`

**Editions cataloged** (per the canons index page):
- Kangyur: **Master, Degé, Choné, Nartang, Peking, Lhasa, Urga**
- Tengyur: **Master, Degé, Choné, Nartang, Peking**
- The project's stated goal (Stanley essay / project description found via search): a master catalog covering "the seven printed Kangyurs of Peking, Cone, Derge, Urga, Narthang, Lhasa, and Mongolian translation; the five printed Tengyurs of Peking, Cone, Derge, Narthang, and Golden; and one manuscript Kangyur of Shekar in London", with "direct links to the actual manuscript images for a single edition of each text". The Tengyur cataloging effort identified "4,093 Tengyur texts with 162,000 pages" across Peking, Cone, Derge, Narthang.
- The essay uses Western scholarly edition sigla in prose: Peking (Q), Golden (G), Narthang (N), Derge (D), Cone (C).

**ID / sigla format (verbatim from the canons help text):** IDs are "edition sigla followed by the text number separated by a period", e.g. **`Tb.5`**, **`dg.0043`**, **`Tk.93`** — capitalization and leading zeros optional. URL-side, catalogs are addressed as `#cat=<siglum>/<k|t>` (e.g. `#cat=d/k` = Degé Kangyur); individual records as `#cat=d/k/d0001`-style paths. So the internal Degé Kangyur numbering looks like `d0001, d0002, ...`.

**"Master" edition concept:** both the KT catalog and the NGB catalog maintain a synthetic **Master catalog** that concords the per-edition catalogs — i.e., THL's own edition-comparison layer. This is the interesting data structure: one master record → per-edition text numbers.

**Search**: by People's Names, Titles, Chapter Titles, Categories, Colophons, IDs; input in English, Extended Wylie, or Unicode Tibetan; `"exact phrase"`, `*` and `?` wildcards. Display togglable: Tibetan script, Wylie, Sanskrit, English translations, page numbers, line numbers (the page/line options imply at least partial **eText** availability behind the catalog, searchable as "Texts" vs "Catalog Records").

**Downloads/data access: none advertised.** No export, dump, or API is mentioned anywhere on the catalog pages. The catalog is view-only through the JS interface. (Whether the underlying TEI/XML is fetchable was not established — the `_escaped_fragment_` render only returns the search shell for catalog listings, unlike essays.)

**Numbering vs. Tohoku / other systems** (cross-checked via Rigpa Wiki "Toh", 84000 Kangyur catalog, Esukhia derge-kangyur GitHub): the Tohoku catalogue (1934) numbers texts in the order of the **Degé** Kangyur/Tengyur, and is the de-facto shared key used by 84000, Adarsha, BDRC outlines, and (project-side knowledge) ACIP's Degé-based KL/TD numbers. THL's Degé catalog is likewise ordered by the Degé edition with sequential numbers (`d0001`...). **It is therefore highly likely `d.NNNN` ≈ Toh NNNN, but THL nowhere states this equivalence explicitly, and I could not verify it record-by-record in this pass** (individual records are JS-only). Treat "THL d-number = Toh number" as PROBABLE-UNVERIFIED; spot-check ~20 records across sections before wiring any automatic mapping.

**Relevance to ALL Tool:**
- The master-catalog concordance (one work → Degé/Choné/Nartang/Peking/Lhasa/Urga numbers) is exactly the edition-mapping layer our Library pane's citation system wants. Since no data download exists, the lane is **link-out**: deep-link `https://old.thlib.org/encyclopedias/literary/canons/kt/catalog.php#cat=d/k/dNNNN` from our Degé/Toh-keyed records once the d↔Toh equivalence is verified.
- If d = Toh holds, cross-linking is trivial from anything Toh-keyed: ACIP catalog numbers (KL/TD, Degé-based), 84000 (Toh), BDRC Degé outlines. That gives us THL as a fourth spoke on the same Toh hub — no new join table needed.
- Longevity risk: the catalog lives on `old.thlib.org` and depends on a 2010s JS app. Don't build anything load-bearing on these URLs; treat as supplementary external links.

### 1.2 Collected Tantras of the Ancients (Nyingma Gyübum, NGB)

Source: https://old.thlib.org/encyclopedias/literary/canons/ngb/ (interface at `.../ngb/index.php`)

- Editions: **Master Edition** (THL-synthesized reference edition, produced under David Germano), **Collected Tantras of Vairocana** (`#cat=bg`), **Degé** (`#cat=dg`), **Tsamdrak** (`#cat=tb`), **Tingkyé** (`#cat=tk`).
- IDs use the same sigla.number convention: `Tb.5` (Tsamdrak), `Tk.93` (Tingkyé), `dg.0043` (Degé NGB).
- Related scholarly catalogs: Cantwell/Mayer/Fischer (2002) Rig 'dzin Tshe dbang nor bu edition catalogue; Cantwell, Mayer, Kowalewski & Achard (2006).
- No download/data-export options mentioned. **Lane: link-out.**
- Relevance: only matters to us if/when the Library pane carries NGB editions; BDRC holds NGB scans, so THL's NGB master catalog would again be a concordance link-out target.

### 1.3 Sungbum (collected works) and other catalogs

Source: https://old.thlib.org/encyclopedias/literary/

- **Dilgo Khyentsé collected-works catalog**: `https://old.thlib.org/encyclopedias/literary/shechen/dkcw/catalog.php`
- **Longchenpa collected-works catalog** (listed on the Literary hub)
- **Kama Catalog** (Nyingma Kama)
- **Literary Genres** knowledge map + "Tibetan Literature: Studies in Genre" overview: `https://old.thlib.org/encyclopedias/literary/genres/`
- Monastery documentation projects: Drepung, Sera, Meru Nyingpa; Antiquities of Zhangzhung.
- No downloads advertised anywhere. **Lane: link-out.**
- Relevance: our Library pane's Sungbum shelf (Longchenpa, Dilgo Khyentsé authors) could deep-link these catalogs from the author/work info panel — same pattern as our existing BDRC links.

### 1.4 eTexts / text archives

- **`https://texts.thlib.org/` is DEAD in practice** — it serves a stock Apache Tomcat 9.0.29 welcome page ("If you're seeing this, you've successfully installed Tomcat."). The old THL eText reader that lived there is gone.
- The KT catalog's "Texts" search scope + page/line display options imply THL still serves some canonical eTexts through the catalog app, but only interactively; extent unknown.
- **Modern Texts** moved to Mandala (`texts.mandala.library.virginia.edu`) — JS-only, unreadable in this pass; contents/licensing UNKNOWN.
- `https://old.thlib.org/collections/texts/` returns **403 Forbidden**.
- **Lane: unknown → treat as link-out at most; nothing ingestable identified.**

---

## 2. Audio-Video collections

Sources:
- https://www.thlib.org/ (homepage claims "Over 8,000 recordings documenting life in Tibet and the Himalayas")
- https://old.thlib.org/collections/ → points to https://audio-video.shanti.virginia.edu/collection/tibetan-and-himalayan-library which 301s to **https://av.mandala.library.virginia.edu/collection/tibetan-and-himalayan-library**
- Search-index evidence: the THL collection paginates to at least "Page 75" (≈750+ items in that named collection alone); subcollections exist (e.g. "Amdo Collection"); items are browsable by kmaps place (e.g. `/kmaps/places/433` = Sera Monastery) and by subject.
- `old.thlib.org/avarch/` 301s to `mediabase.shanti.virginia.edu` which **no longer resolves (DNS dead)**.

Content (per THL project description): original field recordings documenting "oral and embodied forms of knowledge in Tibet and the Himalayas" — spoken-dialect documentation, folk songs, monastery life, teachings, plus films by others that THL disseminates. Items are "dynamically linked to geographical maps as well as to knowledge maps of cultural subjects."

**Access/licensing: UNKNOWN.** The Mandala AV site serves an empty shell to programmatic fetchers; no site-wide license statement was found via search; per-item licensing (if displayed) could not be read. No download option confirmed.
**Lane: link-out only** (stream-in-browser links, e.g. by kmaps place ID). Do not ingest media. Revisit with a real browser session if AV ever matters to us — note that spoken-Tibetan recordings are outside the ALL tool's current scope anyway.

---

## 3. Image collections

Sources:
- https://old.thlib.org/collections/ → https://images.shanti.virginia.edu/collection/central-tibet-collections which 301s to **https://images.mandala.library.virginia.edu/collection/central-tibet-collections**
- Homepage: "Photographs indexed by subject, location, or descriptive terms."

Known collections: "Central Tibet Collections" (murals, monastery photography, historical photos — THL's photo holdings include the Sera/Drepung/Meru Nyingpa documentation projects). Details unverifiable: **images.mandala is a JS SPA returning empty content to fetchers**; `/collections/all` unreadable.

**Access/licensing: UNKNOWN. Lane: link-out only.** No ingestion; no downloads confirmed.

---

## 4. Places, maps, and subjects (kmaps)

### 4.1 THL Place Dictionary / gazetteer — **the one confirmed machine-readable THL dataset**

Sources:
- https://old.thlib.org/places/ (hub; points to http://places.thlib.org/ and https://mandala.shanti.virginia.edu/places)
- **https://places.kmaps.virginia.edu/** — the live gazetteer app ("SHANTI Place Dictionary", maintained by Andrés Montano, SHANTI/UVa)
- **JSON API verified live 2026-08-12**:
  - `https://places.kmaps.virginia.edu/features/433.json` → full record for Sera Monastery: fields `id`, `db_id`, `header`, `names[]` (each with `language`, `writing_system`, `view` — Tibetan script, Wylie, Chinese, romanizations), `feature_types[]` (+ ancestor hierarchy, e.g. Religious > Monastery > "Large Monastery"), `parents[]`/`children[]`, `perspectives[]` (admin hierarchy Earth → China → Tibet → Lhasa; also Cultural Regions, Religious Relationships, Historical Polity Administrative Units, Environmental Relationships), `associated_resources` (counts of descriptions/pictures/related), `interactive_map_url`, **`kmz_url`** (per-feature KMZ download), `has_shapes`, `has_altitudes`, `citations`, `time_units`.
  - `https://places.kmaps.virginia.edu/features.json` → root feature ("Earth") with `children[]`; the tree is walkable from the root, so the full gazetteer is enumerable via the API.
- Scale: the subjects API reports **64,429 places** associated with the Geographical Features taxonomy — that's the order of magnitude of the gazetteer.
- ID scheme: plain numeric feature IDs (`features/433` = Sera; `1` = China, `5226` = Tibet, `637` = Lhasa, `15349` = Central Tibet, `1179` = Mangra, `1086` = Drango). Mandala AV/Images reuse these IDs (`av.mandala.../kmaps/places/433`), so the kmaps ID is the join key across all THL media.
- Multi-language names: English, Tibetan, Dzongkha, Chinese, with six romanization/script options.

**Licensing: NOT STATED anywhere found** (no license text on places.kmaps or subjects.kmaps; only a UVa/SHANTI copyright notice). The API is open and unauthenticated, but **absence of a license is not permission** — lane: **link-out + live API queries at most; no bulk ingestion until license clarified with SHANTI/UVa.** (Historical note: THL's own created content is nominally under the THL Community License — see §6 — but the kmaps sites do not carry that footer.)

**Relevance to ALL Tool:** highest-value THL asset for us. Our Library/People layers could resolve monastery/place names (Tibetan script or Wylie) → kmaps feature ID at lookup time via the JSON API, and deep-link the Place Dictionary page. BDRC also has place records (G-prefixed); kmaps↔BDRC place mapping would have to be done by name-matching — no shared key found.

### 4.2 Maps collections

Source: https://old.thlib.org/places/maps/collections/ — states verbatim: "Unfortunately, the THL Map Collections are not currently available. We are working on updating them, but do not know when they will be available. (April 2020)". Scanned paper maps, custom digital maps: **offline since 2020**. Interactive map advertised at `/places/maps/interactive/` (not tested). Nepal census data at `/places/culturalgeography/nepal/census/`.
**Lane: n/a (offline).**

### 4.3 Subjects taxonomies

Source: **https://subjects.kmaps.virginia.edu/** — 40+ hierarchical subject taxonomies (Architecture, Ritual, Religious Systems, Cultural Landscapes, Geographical Features, ...), same numeric-ID scheme (`features/20` = Geographical Features, `features/5550` = Administration), same JSON API (verified: `features/20.json` returns hierarchy + `associated_resources`). Same licensing situation: **unstated**. Includes a **Literary Genres** taxonomy (via the Literary Encyclopedia) that could inform our Library's genre facets — but license-unknown, so reference/link-out only.

---

## 5. JIATS — Journal of the International Association of Tibetan Studies

Sources:
- https://old.thlib.org/collections/texts/jiats/ (www.thlib.org path 301s here)
- Downloads listing (rendered via escaped fragment): `https://old.thlib.org/collections/texts/jiats/index.php?_escaped_fragment_=jiats=/downloads/`
- Wikipedia (THL article): JIATS is "a freely available online, peer-reviewed English language academic journal focusing on Tibetan studies."

Findings:
- **Open access in practice**: freely readable online; **Issues 1–7** downloadable — complete issues, bundled sections (Articles, Reviews, Miscellanea...), or individual articles, as **PDFs packaged in .zip archives**. Content types: peer-reviewed articles, book reviews, critical editions, brief communications, review essays, field notes.
- Dedicated pages exist: Copyrights (`about/wiki/jiats copyrights.html`), How to Cite (bibliographic format for citing JIATS articles), Contact.
- **The Copyrights page body could not be retrieved** (two fetch attempts returned only navigation chrome — likely wiki-content loaded client-side). The only license signal captured is the site-wide footer: "THL Community License © 2026 www.thlib.org". **Actual reuse terms for JIATS articles: UNKNOWN** (author-retained copyright is common for such journals but is NOT confirmed — do not assume).
- **Lane: link-out** (link articles/issues from our citation layer). Redistribution/bundling of the PDFs inside the app: **not until the Copyrights page is read in a real browser** and terms confirmed.

---

## 6. Licensing: the THL Community License

Sources:
- https://old.thlib.org/tools/thl-license.php ("THL Open Community License Version 1.0")
- https://old.thlib.org/tools/opl-license.php (the underlying license text)

Structure: the THL Community License is a two-part wrapper — thl-license.php (THL-specific attribution notice) + opl-license.php ("the general licensing policy"), which is the **Open Public License (OPL) v1.0**, a Mozilla-MPL-style open license. Key captured terms:
- Grant: "a worldwide, royalty-free, non-exclusive license" to "use, reproduce, modify, display, perform, sublicense and distribute the Original Code."
- Copyleft on source: "The Source Code version of Covered Code may be distributed only under the terms of this License"; must include the license with every copy; modifications must be documented with dates and carry "a prominent statement that the Modification is derived...from Original Code provided by the Initial Developer."
- Attribution: "The Initial Developer of this software is the Tibetan and Himalayan Library (THL). Portions created by the THL are Copyright 2001-2010 THL."
- Commercial use permitted subject to the above; no warranty.

**Caveats for us:**
1. The OPL is written in software terms ("Source Code", "Executable"). Its application to catalog *data*, essays, images, or AV is legally mushy; THL's footer slaps it site-wide, but per-collection/per-item rights (especially third-party films, photographs, and JIATS author copyrights) plainly are not all THL's to license.
2. Therefore: **the site-wide license is NOT a green light to ingest any specific collection.** Per-resource lane decisions below stand; anything marked unknown stays unknown until confirmed (ideally by asking THL/SHANTI directly — contact via `old.thlib.org/contact.php`).

---

## Licensing lanes — summary table

| Resource | Downloadable? | License found | Lane |
|---|---|---|---|
| Kangyur/Tengyur catalogs (kt) | No (view-only JS app) | Site-wide THL Community License (OPL 1.0) only | **Link-out** (deep links keyed by sigla.number) |
| NGB catalogs | No | Same | **Link-out** |
| Sungbum catalogs (Dilgo Khyentsé, Longchenpa), Kama, Genres | No | Same | **Link-out** |
| Canonical eTexts behind kt catalog | Interactive only; extent unknown | Unknown | **Link-out / unknown** |
| texts.thlib.org eText reader | — | — | **Dead** |
| Modern Texts (texts.mandala) | Unknown (JS-only) | Unknown | **Link-out at most; revisit** |
| Audio-Video (av.mandala) | Unknown; no downloads confirmed | Unknown (per-item terms unread) | **Link-out only** |
| Images (images.mandala) | Unknown | Unknown | **Link-out only** |
| Places gazetteer (places.kmaps JSON API, per-feature KMZ) | Yes technically (open API, KMZ) | **None stated** | **Live API query + link-out; NO bulk ingest until license clarified** |
| Subjects taxonomies (subjects.kmaps JSON API) | Yes technically | **None stated** | Same as Places |
| Maps collections | Offline since April 2020 | — | **n/a** |
| JIATS issues 1–7 (PDF zips) | Yes | Unknown (Copyrights page unreadable) | **Link-out; no redistribution until terms read** |
| THL-authored site software/content generally | — | THL Community License = OPL v1.0 (open, attribution, source-copyleft) | Case-by-case |

## Cross-linking opportunities (Relevance to ALL Tool, consolidated)

1. **Toh-number hub (likely, verify first).** THL Degé catalog IDs (`d0001`-style) almost certainly track Tohoku numbers, the same key behind ACIP's Degé-based catalog numbers, 84000, and BDRC Degé outlines. One verification pass (spot-check ~20 THL records against Toh across Kangyur sections) unlocks a fourth external link per Degé work in our Library info panel: `old.thlib.org/...catalog.php#cat=d/k/dNNNN`.
2. **Edition concordance.** THL's Master catalogs are the only free multi-edition concordance (Degé/Choné/Nartang/Peking/Lhasa/Urga) found; view-only, but a citation-layer "other editions" link can point at the master record.
3. **Places.** kmaps JSON API is live and unauthenticated — resolve monastery/place names (Tibetan/Wylie/Chinese available in `names[]`) to kmaps IDs for our People/Library place references; deep-link Place Dictionary pages; per-feature KMZ for maps. No shared key with BDRC place IDs — name-matching required.
4. **Subjects/genres.** kmaps Literary Genres taxonomy could inform Library genre facets (reference only).
5. **JIATS.** Natural link-out target for the citation layer (peer-reviewed Tibetan-studies articles, stable URLs, How-to-Cite format provided).

## DEAD / UNREACHABLE list

| URL | Status |
|---|---|
| https://www.thlib.org/collections/ | 404 (use https://old.thlib.org/collections/) |
| https://texts.thlib.org/ | Serves stock Apache Tomcat 9.0.29 default page — eText reader gone |
| https://old.thlib.org/collections/texts/ | 403 Forbidden |
| https://old.thlib.org/encyclopedias/literary/canons/ (www host) | 404 on www.thlib.org; works on old.thlib.org |
| http://mediabase.shanti.virginia.edu/ (target of old.thlib.org/avarch/ 301) | DNS does not resolve |
| https://collab.its.virginia.edu/wiki/tibetanstudies/... (Kangyur & Tengyur Text Finding Aids) | DNS does not resolve (UVa Collab retired) |
| https://av.mandala.library.virginia.edu/* | Alive but JS-only; empty to programmatic fetch |
| https://images.mandala.library.virginia.edu/* | Alive but JS-only; empty to programmatic fetch |
| https://texts.mandala.library.virginia.edu/ | Alive but JS-only; empty to programmatic fetch |
| JIATS Copyrights page (old.thlib.org/collections/texts/jiats/about/wiki/jiats copyrights.html) | Loads, but body content not retrievable (client-side wiki render) — read in a real browser |
| web.archive.org | Not fetchable from this environment (tool restriction), so no archive fallback was used |
| THL Maps collections (/places/maps/collections/) | Page alive; collections offline since April 2020 per THL's own notice |

## Open follow-ups

1. Spot-verify THL `d.NNNN` ↔ Toh NNNN before wiring links (JS app — needs a browser session).
2. Read the JIATS Copyrights page and one Mandala AV + one Images item page in a real browser to pin per-item licensing.
3. If Places ingestion ever becomes desirable (offline gazetteer in the app), email THL/SHANTI (contact: old.thlib.org/contact.php; kmaps maintainer named on-site: Andrés Montano, SHANTI/UVa) for an explicit data license.
