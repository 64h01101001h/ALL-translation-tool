# THL Knowledge Domains & Technical Infrastructure

Research date: 2026-08-12. Scope: thlib.org's encyclopedias, scholarship, educational resources, technical infrastructure, live-vs-dead site map, and partner network. Companion to `collections_catalogs.md`.

Method note: everything below was verified by direct fetch (live site, live APIs) or Wayback Machine snapshot on the research date. Gaps are flagged as gaps, not filled from general knowledge.

---

## 0. Headline findings

1. **THL is NOT simply a legacy site.** There are two parallel THLs today:
   - **New thlib.org** — a 2020s rebuild: WordPress + a React "Mandala" plugin (theme `mandala-kadence`), serving Audio-Video, Images, Places, Subjects, Terms (dictionary + converters + translation tool), Classical Literature catalogs (Kangyur, Tengyur, Nyingma Gyübum, Dilgo Khyentsé, Longchenpa), Modern Texts, Bibliographies, Thematic Collections. It even publishes an uptime page: https://stats.uptimerobot.com/QbeCcnGpiO
   - **old.thlib.org** — the full legacy site, still served (encyclopedias, toolbox wiki, converters, JIATS, genres book reader).
2. **Publicly queryable Solr API** backs the new site: `https://mandala-index.internal.lib.virginia.edu/solr/` (despite the "internal" hostname it answers from the public internet). Cores: `kmterms` (4,488,953 docs — the Terms dictionary + kmaps), `kmassets`, `mandala-av`. Standard Solr `select?q=...&wt=json` works. This is effectively a free machine-readable API to THL's entire terms/subjects/places graph.
3. **The complete THL Tibetan Literary Genres taxonomy was captured** (§2 below) — ~200 nodes, 4 levels, extracted in one Solr query and cross-verified against Wayback snapshots of the old knowledge-map app.
4. **All THL software is open source** on GitHub org `shanti-uva` (actively maintained — commits through Aug 2026) plus the old SourceForge `thdltools` project (Jskad, Translation Tool, TiblEdit, Tibetan Machine fonts).
5. **The online Tibetan Translation Tool is alive** at https://ttt.thlib.org/ — the same lineage as the `DictionarySearchStandalone.jar` we already hold locally (`~/Tibetan Translation Tool`). Meta description: "This Java tool takes Tibetan language passages and divides the passages up into their component phrases and words, and displays corresponding dictionary definitions." Dictionaries named on the page: Jim Valby, Rangjung Yeshe, Jeffrey Hopkins, Tsig Mdzod Chen Mo.

---

## 1. Encyclopedias

### 1.1 Tibetan Literary Encyclopedia — ALIVE (legacy host)
- URL: http://old.thlib.org/encyclopedias/literary/ (200 OK)
- The only encyclopedia that made it onto thlib.org proper. Portal to:
  - Catalogs: Tibetan Buddhist Canon (Kangyur/Tengyur), Collected Tantras of the Ancients (Nyingma Gyübum), Kama Catalog, Dilgo Khyentsé Catalog, Longchenpa Catalog. Canons page http://old.thlib.org/encyclopedias/literary/canons/ is 200 OK. These same catalogs are re-served on the NEW site under https://thlib.org/classical-literature/.
  - Literary Genres Overview: http://old.thlib.org/encyclopedias/literary/genres/ (page loads; the interactive tree loads dynamically from the dead `subjects.thlib.org`, so the embedded tree is broken there — but see §2, the data survives elsewhere).
  - *Studies in Genre* book reader: http://old.thlib.org/encyclopedias/literary/genres/genres-book.php (200 OK) — digitization of *Tibetan Literature: Studies in Genre* (ed. Cabezón & Jackson), which THL positioned as the "initial core of rich essays on specific genres." (Chapter list not captured — the reader is a JS app; the Mandala texts mirror at texts.mandala.library.virginia.edu returns an empty page. GAP: enumerate chapters by driving the reader in a browser.)
  - Wiki-served encyclopedia articles, e.g. https://www.thlib.org/encyclopedias/literary/wiki/'bri%20khung%20thel.html (Google-indexed; wiki HTML pages under /encyclopedias/literary/wiki/ still resolve).
- Historical tab structure (from search snippets of the encyclopedia help pages): Home, **Canons, Authors, Sects, Periods, Genres, Orals**. Only Canons and Genres have substantial surviving content that we located. GAP: Authors/Sects/Periods/Orals content not found live; likely never fully built out.

### 1.2 Tibetan Medicine Encyclopedia — DEAD (was a project, mostly aspirational)
- A "Tibetan Medicine Encyclopedia Status Report" wiki page existed on www.thlib.org until at least mid-2025 (Wayback snapshot 2025-07-14: http://web.archive.org/web/20250714172607/https://www.thlib.org/about/wiki/Tibetan%20Medicine%20Encyclopedia%20Status%20Report.html — snapshot exists but is behind archive.org's bot check; live URL now 404). Search snippets date the report to 2008 and describe the Medicine Encyclopedia as a thematic collection on the old THDL site. No live medicine encyclopedia exists on either host today.

### 1.3 Bön — no separate encyclopedia found
- No standalone Bön encyclopedia located on thlib.org or old.thlib.org. Bön material surfaces inside other structures: the genre taxonomy includes Bönpo canonical collections ("The Bönpo Translations of the Divine Precepts", "The Bönpo Translations of the Commentarial Treatises" — see §2 under Multi-author Anthology), and THL's reference-resources page points to Adarshah for searchable Yungdrung Bön texts. GAP: if a THDL-era Bön encyclopedia existed, it would be in Wayback under thdl.org; not enumerated in this pass.

### 1.4 Other "encyclopedic" layers on the new site — ALIVE
- **Subjects** (knowledge maps): https://thlib.org/subjects/ — hierarchical subject trees with linked multimedia.
- **Places** (geographic dictionary): https://thlib.org/places/ — plus monastery microsites on legacy (Sera, Drepung, Meru Nyingpa: old.thlib.org/places/monasteries/...).
- **Terms** (historical dictionary): https://thlib.org/terms/ — with Advanced Search, phonetics converter, transliteration converter, translation tool, and a "Tibetan Dictionary Resources" guide.

---

## 2. The THL Tibetan Literary Genres taxonomy (CAPTURED IN FULL)

Provenance: extracted 2026-08-12 from the live public Solr core `kmassets` (query: `asset_type:subjects AND ancestors_txt:"Tibetan Literary Genres"`, 201 docs), root = kmaps subject **subjects-119** ("Tibetan Literary Genres", child of subjects-5809 "Literary Genres"). Cross-checked against 2015 Wayback snapshots of the old `subjects.thlib.org/categories/119/children/*` pages (IDs match). Browse live: https://subjects.kmaps.virginia.edu/features/119.json (JSON API, rate-limited) or https://thlib.org/subjects/.

THL's own description: "The Knowledge Map of Literary Genres provides an expandable-collapsible hierarchy of genres found within Tibetan literature," intended to link each category to summaries, essays, excerpts, translations, and oral commentaries.

**Eleven top-level branches** (kmaps IDs in brackets):

- **Academic (Modern)** [121]
  - Academic Journal [371], Article [368], Book [369]
- **Administrative & Legal Literature** [122]
  - Census Document [372], Government Edict [374], Government Regulations and Law Codes [377], Military Document [375], Monastic Regulation [379], Official Letter [367], Parking Ticket [376], Pillar Inscription [378], Tax Record [373]
- **Biography** [123]
  - Autobiography [380], Diary [384], Expression of Realization [382], Hagiography [381], Past Life Narrative [383], Record of (Teachings) Received [386], Return from Death Narrative [385]
- **Compilations & Anthologies** [124]
  - Miscellaneous Works [391], Selected Works [390], Single Author Collected Works [388], Treasure Cycle [387]
  - Multi-author Anthology [389]
    - Collected Spells [482], Collected Tantras of the Ancients [477], The Bönpo Translations of the Divine Precepts [480], The Bönpo Translations of the Commentarial Treatises [481], Translation of the Buddhas' Own Precepts [478], Translation of the Commentarial Treatises [479]
- **Guidebooks & Reference Works** [129]
  - Catalog [392] → Bibliography [419], Notice of Contents [420]
  - Cookbook [393], Dictionary [396], Encyclopedia [398], Grammar [399]
  - Geographical Work [394] → Guide to Paintings [5804], Pilgrimage Guide [424], Place Guide [421], Sacred Site Guide [423], Travelog [422]
- **Historical & Legendary Literature** [125]
  - Annals [134], Chronology of Teachings [142], Clan Record [138], Family Lineage [137], History [132], History of Teachings [140], History of Treasures [141], Lineage Note [139], Monastic Chronicle [135], Ordered Outline [143], Religious History [133], Royal Genealogy [136]
- **Literary Arts** [126]
  - Epic [144] → Gesar [154]
  - Graffiti [152], Note [153], Pamphlet [150], Poster [151]
  - Journalism [149] → Magazine/Popular Periodical [175], Newsletter [176], Newspaper [174]
  - Letters [146] → Epistolary Manual [7163], Letter [156], Official Letter [155]
  - Poetics [147] → Metrics [9077], Poetic Example [164], Synonymics [165]
  - Poetry [157] → Adamantine Song [161], Alphabet Verse [163], Aphoristic Verse [158], Experiential Melody [162], Poetical Song [159], Song [160]
  - Story and Drama [148] → Comic Book [170], Folk Story [168], Humorous Story [167], Novel [169], Proverb [171], Short Story [166], Testament [173], Verse of Praise [172]
- **Music** [127]
  - Music Notation [400], Songbook [401]
- **Non-Philosophical Religious Literature** [130]
  - Canonical [409] → Sūtra [426], Tantra [425]
  - Compilations and Cycles [410] → Seminal Essence [427]
  - Consecratory Labels and Formulae [411] → Inscription on Statue [431], Mantra Roll Inserted in a Stūpa [429], Spell [430], Tangka Labels & Inscriptions [428]
  - Contemplative Guidebooks and Instructional Manuals [414] → Esoteric Precept [463], Exegetical Guidance [468], Experience-driven Guidance [471], Experiential Guidance [470], Guidance Text [464], Instructional Precept [462], Ostensive Guidance [469], Preliminary Practices [467], Purification of the Mind [466], Stages of the Path [465]
  - Ritual and Liturgical Texts [413] → (Petition for) a Speedy Rebirth [457], Cleansing-smoke Offering [448], Confession [461], Consecration Ritual [455], Dedication [445], Evocation Ritual [436], Explanation of Maṇḍala [440], Fire Offering [442], Guru Worship [456], Guru Yoga [441], Initiation in the Maṇḍala Ritual [434], Longevity Ritual [438], Method of Invoking [460], Permission to Invoke [450], Prayer [453], Prayer for Long Life [435], Printing Colophon Prayer [458], Prosperity Riddle Ritual [437], Regular Propitiation [459], Ritual Action [451], Ritual of Fasting and Silence [454], Ritual of the Circle of the Group [452], Supplication [443], Supplication of Previous Lives [446], Supplication of the Lineage Lamas [444], Torma-offering Ritual [447], Transference of Consciousness [449], Vase Consecration [439]
  - Sermons and Discourses [412] → Lecture [432], Public Pronouncement [433]
- **Philosophical & Doctrinal Literature** [128]
  - Answers to Questions [9338], Doxographical Literature [406], Miscellaneous [408], Polemical Literature [402]
  - Doctrinal Commentarial Text [407] → Annotation Commentary [496], Commentary [493], Commentary on Difficult Points [497], Content Commentary [499], Detailed Explanation [10269], Explanation of Difficult Points [9339], Extensive Commentary [503], Extensive Explanation [9305], Note [505], Outline [502], Overview [501], Simplified Commentary [500], Syllable-by-syllable Commentary [495], Synoptic Commentary [504], Synthetic Commentary [498], Word-by-word Commentary [494]
  - Monastic Textbook [403] → Doxography [488], Introductory Logic Textbook [486], Introductory Philosophical Textbook [485], Introductory Psychology Textbook [487], Stages and Paths [483], Stages and Paths of Mantra [484], Stages of the Vehicles [489], Textbook [490] → Definitive Analysis [6765], Generic Overview [6764]
  - Path Systematics [405] → Stages of the Path [491], Stages of the Teachings [492]
- **Scientific and Technical Literature** [131]
  - Art [416] → Guide to Painting [418], Illustration [417]
  - Astrology [472], Calendar [415], Calligraphy and Graphology (also under reference) [473], Medicine [475], Predictions of when Great Buddhist Teachers will Arise [474], Textbook [476]

Raw Solr extract saved during research (scratchpad `genre_tree_solr.json`); rerunnable in one query — see §4.2.

**Relevance to ALL Tool:** this is a ready-made, ID-stable genre facet for classifying texts in our Library pane and for tagging corpus sources. The commentary sub-typology (word-by-word vs annotation vs synoptic etc.) maps directly onto the kinds of texts GMR translates, and Tibetan-language equivalents for each node are retrievable per-ID from the kmaps JSON API (each feature carries multilingual `names`).

---

## 3. Essays and scholarship

- **Studies in Genre book** (see §1.1) — the central scholarly anchor of the genres project; reader live on legacy host.
- **JIATS** — *Journal of the International Association of Tibetan Studies*, THL-hosted e-journal: http://old.thlib.org/collections/texts/jiats/ (200 OK; first probe returned 503 — the legacy host is flaky under load).
- **Wiki essay layer** — hundreds of TEI-derived HTML essays under `/…/wiki/…` paths on both www and old hosts (encyclopedia articles, help docs, project reports). No index page found; discoverable via search engines and internal links.
- **Canon textual history** — the canons catalog pages (§1.1) carry edition sigla and structural documentation of Kangyur/Tengyur editions; detailed capture belongs to the collections/catalogs research track (`collections_catalogs.md`).
- GAP: colophon-studies and manuscript-culture essays were not specifically located in this pass; likeliest locations are the Toolbox's "Tibetan Texts" cataloging docs (§4.4) and JIATS articles.

---

## 4. Technical infrastructure

### 4.1 The new-site stack (observed directly)
- WordPress (`wp-json` REST alive at https://thlib.org/wp-json/) + **Mandala WP plugin** (React 18 app in `wp-content/plugins/mandala/`) + `mandala-kadence` child theme.
- The React app's bundle reveals its backends: Solr (`mandala-index.internal.lib.virginia.edu`), IIIF image server (`https://iiif.lib.virginia.edu`, `images.mandala.library.virginia.edu`, `cicada.shanti.virginia.edu/images`), Kaltura for video (`cfvod.kaltura.com`), AV site `av.mandala.library.virginia.edu`, legacy text service `texts.thdl.org/django/`, docs at `confluence.its.virginia.edu/display/KB/Mandala`.

### 4.2 Public Solr API (the big one)
- Base: `https://mandala-index.internal.lib.virginia.edu/solr/` — answers publicly, no auth, standard Solr query syntax.
- Cores confirmed: `kmterms` (4,488,953 docs), `kmassets`, `mandala-av`; plus a dev replica `mandala-solr-replica-dev.internal.lib.virginia.edu`.
- Example (used for §2): `…/solr/kmassets/select?q=asset_type:subjects AND ancestors_txt:"Tibetan Literary Genres"&rows=1000&fl=uid,title,ancestors_txt&wt=json`
- `kmterms` contains term records with Tibetan headers in Wylie (e.g. `header: "sde srid sangs rgyas rgya mtsho/"`), relation blocks (`is headed by`, phoneme analyses), and cross-links to subjects/places. This is THL's Terms dictionary in machine-readable form.
- Caveat: an "internal" hostname exposed publicly may be unintentional; treat as read-only research access, cache what we need, do not build a product dependency on it without asking THL.

### 4.3 Kmaps JSON APIs — ALIVE (rate-limited)
- `https://subjects.kmaps.virginia.edu/features/{id}.json` (also `.xml`), same for `places.kmaps.virginia.edu`, `terms.kmaps.virginia.edu`. Root feature list at `/features.json`. Aggressive 429 rate limiting (single-digit requests per minute tolerated).
- Top-level live subject trees: Administration (5550), Architecture (6669), Ritual (5805), Contemplation (5806), Religious Systems (5810), Historical Periods (5807), Geographical Features (20), Cultural Regions (305), Religious Sects (302), Language Tree (301), Scripts (192), Material Objects (2693), Politics (7174), Profession (6670), Tibet and Himalayas (6403), Literary Genres (5809 → 119).

### 4.4 THL Toolbox — ALIVE (legacy host)
- Home: https://old.thlib.org/tools/wiki/home.html (200 OK). The staff technical manual: Audio-Video documentation (their transcript/annotation workflow), Photography & Immersive Technologies, Scanning & OCR, Tibetan Scripts & Fonts, Tibetan Texts (creation + cataloging), Transliteration standards, Places & Geography, Reference-resource creation, Developers' Zone, "THL Technologies & Open Standards", workflow docs (Sakai, Subversion, TrackPlus), fonts/input for Tibetan, Chinese, Dzongkha, Nepali, diacritics.
- Per search snippets, the Toolbox includes an **XML Markup Manual** ("exhaustive documentation of the various elements and attributes used in THL's TEI-based XML markup scheme for essays"), an XML Editors manual (XMLmind customization with a Tibetan Text template incl. metadata section). The canonical Toolbox wiki previously also lived at `collab.its.virginia.edu` (UVA Sakai) — that host no longer resolves in DNS; the old.thlib.org rendering is the surviving copy.

### 4.5 Open-source software
- **GitHub org `shanti-uva`** (active, commits through 2026-08): the kmaps Rails stack (`kmaps_engine`, `subjects`, `places`, `terms`, `terms_engine`, `*_integration` gems), Solr tooling (`flare`, `solr-shanti-configsets`, `kmaps-solr-proxy`, `kmsolrq`), **`tibetan-collation-solr`** (Solr configset for Tibetan sort order — directly reusable idea for our FTS spine), `dictionary_to_terms` (their dictionary→terms migration engine), `wp-kmaps`, `mandala-wp-proxy` (CORS proxy plugin), `mandala-kadence` theme, `mms_image_import`, typeahead/relations-tree jQuery plugins.
- **SourceForge `thdltools`** (https://thdltools.sourceforge.net/, alive): **Jskad** (Tibetan input/editing, Java), **Translation Tool** (the ttt lineage), **TiblEdit** (TIBBIBL bibliography/catalog editor for their TEI text cataloging), **Tibetan Machine / Tibetan Machine Web fonts**. Licensing not stated on the index page (THDL historically used its own "THL Community License" — legacy page `old.thlib.org/tools/thl-license.php`; verify before reusing code).
- **ttt.thlib.org** — live online translation-tool deployment (see §0.5).

### 4.6 Converters — ALIVE (legacy host)
- Wylie converter: https://old.thlib.org/reference/transliteration/wyconverter.php (200)
- Phonetics converter: https://old.thlib.org/reference/transliteration/phconverter.php (200)
- Translation tool page: https://old.thlib.org/reference/dictionaries/tibetan-dictionary/translate.php (200)
- New-site equivalents: https://thlib.org/terms/#/terms/phonetics, #/terms/transliterate, #/translate/tibetan, and the THL Extended Wylie spec at https://thlib.org/terms/#/texts/67579.

**Relevance to ALL Tool:** (a) THL's phonetics converter is the other major Tibetan-phonetics convention — worth diff-testing against our GMR-convention `pron_engine` port to document divergences; (b) `tibetan-collation-solr` solves Tibetan sort order for Solr — the same collation logic is worth studying for our SQLite spine ordering; (c) TiblEdit + the TEI markup manual document a mature Tibetan text-cataloging schema (sigla, editions, colophons) to compare against our catalog design; (d) the kmterms Solr core is a huge free terms dataset for cross-referencing (reference-only per our Rule 1 — never promoted into HGM glosses).

---

## 5. Educational resources — DEAD (never migrated)

- The `/education/` section (Courses, Programs, Learning Languages: English/Nepali/Tibetan) exists in old.thlib.org's navigation but **every education URL returns 404** — and Wayback shows thlib.org/education/ has been 404 since at least 2009. The section never migrated from the predecessor THDL site.
- The real content lives only in Wayback under **thdl.org** (2005–2008): `www.thdl.org/education/` — courses ("Buddhist culture" e-class with syllabus/movies, "Renaissance" course), Nepali learning pages, `tiblang/` (Tibetan language), and English-for-Tibetans PDFs (ABC text, English-Tibetan Folktale Reader, EnglishForTibetans-Intermediate). Example snapshot: http://web.archive.org/web/20060114040224/http://www.thdl.org:80/education/index.html
- The new site has no education section.
- **Relevance to ALL Tool:** low; nothing here beats resources we already hold. Not worth further excavation unless the input-center training materials task needs an English-for-Tibetans reader.

---

## 6. ALIVE vs DEAD map (all verified 2026-08-12)

### ALIVE
| What | URL | Status |
|---|---|---|
| New THL main site (WordPress+React) | https://thlib.org/ | 200 |
| New: Terms dictionary, converters, translation tool | https://thlib.org/terms/ | 200 |
| New: Classical Literature catalogs | https://thlib.org/classical-literature/ | 200 |
| New: Audio-Video, Images, Places, Subjects, Modern Texts, Bibliographies, Thematic Collections | https://thlib.org/{section}/ | 200 |
| New: reference-resources guide (partner map) | https://thlib.org/terms/reference-resources/ | 200 |
| WordPress REST | https://thlib.org/wp-json/ | 200 |
| **Public Solr (kmterms/kmassets/mandala-av)** | https://mandala-index.internal.lib.virginia.edu/solr/ | 200, no auth |
| Kmaps JSON APIs (heavy rate limiting) | https://{subjects,places,terms}.kmaps.virginia.edu/features/{id}.json | 200/429 |
| Legacy full site | https://old.thlib.org/ | 200 |
| Legacy Literary Encyclopedia + canons | https://old.thlib.org/encyclopedias/literary/ | 200 |
| Legacy Studies-in-Genre book reader | https://old.thlib.org/encyclopedias/literary/genres/genres-book.php | 200 |
| Legacy THL Toolbox wiki (TEI/XML manuals etc.) | https://old.thlib.org/tools/wiki/home.html | 200 |
| Legacy converters (Wylie, phonetics, translate) | https://old.thlib.org/reference/transliteration/… | 200 |
| JIATS journal | https://old.thlib.org/collections/texts/jiats/ | 200 (flaky, intermittent 503) |
| Online Translation Tool | https://ttt.thlib.org/ | 200 |
| Legacy texts reader host | https://texts.thlib.org/ | 200 (flaky, intermittent 503) |
| Media Management System | https://mms.thlib.org/ | 200 |
| SourceForge THDL Tools | https://thdltools.sourceforge.net/ | 200 |
| GitHub | https://github.com/shanti-uva | active |
| Uptime dashboard | https://stats.uptimerobot.com/QbeCcnGpiO | (linked from footer) |

### DEAD / UNREACHABLE
| What | URL | Symptom |
|---|---|---|
| Old knowledge-maps app (genre tree host) | http://subjects.thlib.org/ | DNS/cert retired; superseded by subjects.kmaps.virginia.edu |
| Old place dictionary host | http://places.thlib.org/ | connection fails (000) |
| Old knowledge maps host | http://tmb.thlib.org/ | connection fails (000) |
| THL Connections (social) | http://connections.thlib.org/ | connection fails (000) |
| UVA Sakai collab (original Toolbox wiki) | https://collab.its.virginia.edu/ | DNS not found |
| Education section (both hosts) | https://old.thlib.org/education/ (+courses, programs, learning-languages) | 404 (and 404 in Wayback since 2009; real content only on archived thdl.org) |
| Medicine Encyclopedia status page | https://www.thlib.org/about/wiki/Tibetan%20Medicine%20Encyclopedia%20Status%20Report.html | 404 live (Wayback snapshot to 2025-07-14 exists) |
| Mandala platform (Drupal-era SHANTI) | https://mandala.shanti.virginia.edu/, https://audio-video.shanti.virginia.edu/, https://images.shanti.virginia.edu/ | HTTP 202 with empty body — placeholder/challenge, effectively dead |
| Mandala texts mirror | https://texts.mandala.library.virginia.edu/ | 202/empty (the Studies-in-Genre book page renders blank) |
| www.thlib.org legacy paths | https://www.thlib.org/encyclopedias/, /tools/, /education/, /reference/transliteration/wyconverter.php | 404 — the new WordPress site does not serve legacy paths; use old.thlib.org |
| Archived genre subpages for branches 121/129/130 | subjects.thlib.org/categories/119/children/{121,129,130} | Wayback holds only HTTP 500 captures (data recovered instead via live Solr, §2) |

Practical rules: (1) any legacy URL on www.thlib.org → retry on **old.thlib.org**; (2) legacy hosts are flaky — retry on 503; (3) kmaps APIs rate-limit hard — space requests ≥15 s.

---

## 7. Community / partner network (from https://thlib.org/terms/reference-resources/, live)

Partners/resources THL itself points its users to — a ready outreach map:
- **BDRC** (Buddhist Digital Resource Center) — "over 17 million pages" digitized Buddhist literature.
- **ACIP** (Asian Classics Input Project) — listed by THL as downloadable searchable canonical collections. (Us.)
- **Adarshah** — searchable Kangyur/Tengyur, collected works, **Yungdrung Bön** texts.
- **Nitartha Digital Library** — Dege Kangyur & Tengyur, 650,000 page images.
- **84000** — Kangyur translation project (glossaries + bibliographies).
- **Lotsawa House** — 5,000+ texts and translations.
- **UMA-Tibet** — annotated Geluk/Nyingma textbook translations (Hopkins school).
- Dictionaries THL recommends: Rangjung Yeshe/Dharma Dictionary, Monlam, Rigpa Shedra, Christian Steinert's multi-dictionary search, plus Sanskrit (Monier-Williams, Apte, Edgerton BHS) and Newar dictionaries.
- Also: Himalayan Art Resources, Treasury of Lives (biographical encyclopedia), Tibetan Newspapers archive.
- Institutional home: University of Virginia (SHANTI / UVA Library run the Mandala/kmaps infrastructure; docs on UVA Confluence). Legacy microsite partners: Sera, Drepung, Meru Nyingpa monastery documentation projects; Antiquities of Zhangzhung; JIATS (IATS journal).
- GAP: the legacy "THL Partners" wiki page (old.thlib.org/about/wiki/thl partners.html) renders only navigation — the detailed partner list did not survive rendering; the reference-resources page above is the better live source.

---

## 8. Follow-ups worth doing (ranked)

1. Diff-test THL's phonetics converter output against our `pron_engine` battery words to document GMR-vs-THL convention differences (useful for the Phonetics pane's documentation).
2. Pull the THL Extended Wylie spec from https://thlib.org/terms/#/texts/67579 and archive it next to our EWTS engine docs.
3. Drive the genres-book reader in a browser to enumerate the *Studies in Genre* chapter list (or check the texts app JSON behind it).
4. Study `shanti-uva/tibetan-collation-solr` for Tibetan collation rules applicable to our SQLite spine ORDER BY.
5. If we ever want THL terms data at scale, email THL first about the Solr endpoint rather than scraping the "internal" host.
