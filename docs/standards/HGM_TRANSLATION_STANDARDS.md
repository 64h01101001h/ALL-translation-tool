# HGM Translation Standards — compilation document
*(scaffold created 2026-08-06. This document RECEIVES standards as they are
gathered from Geshe Michael Roach himself and from the translators of the
Mixed Nuts translation group. Nothing here is invented: every entry must be
sourced — who said it, when, where (recording / document / message) — and
until material arrives, sections stand empty by design. Rule one applies to
standards exactly as to glosses: we compile his guidance; we never compose
it in his name.)*

## How to add a standard
Each entry follows this form:

> **STD-###** · *category* · source: (GMR | translator name), date, medium
> The standard, stated as given.
> *App integration:* which feature enforces or assists it, and status.

## Categories (pre-mapped to the app's integration points)

### 1. Terminology
*(binding equivalents, preferred renderings, terms never to translate)*

> **STD-011** · terminology · source: *The Other Side of Emptiness* (tr.
> Chang w/ GMR, ©2026), commentary discussion of mental seeds
> Prefer "mental seed" over the loanword "karma": "this poor word
> 'karma' is so abused in our modern world that perhaps we should just
> let him take a rest, and use the expression 'mental seed'
> instead—since that's all we are ever really talking about, when we
> use the word 'karma.'"
> *App integration:* style advisory for the Draft workspace (the
> dictionary layer remains the binding authority; this records the
> published preference in the translators' own words).

**Integration hooks ready:** the Draft pane's terminology checker (flags
non-HGM renderings, mixed renderings, collapsed distinctions); the tier
system (curated > glossary > provisional); the corpus concordance.

### 2. Register
*(prayer vs. philosophy vs. narrative; when the same term renders
differently — the bsod nams principle)*
— nothing compiled yet —
**Integration hooks ready:** register warnings on single-register terms
(never auto-resolved); the verse-meter tool's verse/prose flag.

### 3. Voice & style
*(sentence rhythm, plain-English principles, how literal is too literal,
handling of Tibetan syntax in English)*

> **STD-005** · voice & style · source: *Sixty Verses* (tr. Lashaw w/ GMR,
> ©2023), endnote 60
> Epithets referring to a specific person are capitalized. In the
> volume's own words: "We are using 'realized one' as a translation of
> the Sanskrit word arya … he is often referred to simply as 'The
> Arya,' which is why we will be capitalizing 'Realized One' when it
> refers to him."
> *App integration:* style note for the Draft workspace; no enforcement
> (style stays human).
**Integration hooks ready:** the Draft workspace (side-by-side anchoring);
AI back-check instructed to diff, never to restyle.

### 4. Names, mantras, and Sanskrit
*(when to Sanskritize vs. translate vs. transliterate; pronunciation
spellings for readers)*

> **STD-001** · names · source: *A Song of My Spiritual Life* (tr. Chang
> w/ GMR, ©2025), endnote 1
> Tibetan names in running text are written in English PRONUNCIATION
> spelling — "Je Tsongkapa" — explicitly: "This is … the correct way to
> write our author's name in English pronunciation. Other ways of doing
> it are confusing transcription with pronunciation."
> *App integration:* ALREADY ALIGNED — the pronunciation engine (GMR card
> conventions) produces exactly these spellings.

> **STD-002** · names/apparatus · source: SSL preface & endnotes; *Sixty
> Verses* endnotes (recurring form)
> The technical transliteration is supplied alongside, in the form
> "(technical spelling nyams-mgur)" — running text stays in pronunciation
> spelling; the Wylie appears as labeled apparatus (nyams-mgur, Lam-rim
> bsdus-don, dgag-bya, Blo-bzang grags-pa).
> *App integration:* SHIPPED 2026-08-06 — every term in the Draft pane's
> anchors panel carries a "[+ technical spelling]" link that inserts
> "pronunciation (technical spelling: hyphen-paired-wylie)" at the draft
> cursor (allcore::hgmTechnicalSpelling, tested against the volumes' own
> examples).

> **STD-003** · Sanskrit · source: *Sixty Verses* (tr. Lashaw w/ GMR,
> ©2023), endnote 62
> Sanskrit passages are presented twice: "the full technical
> transcription of the line with traditional diacritical marks; and then
> … the closest normal English pronunciation, broken out into separate
> words as far as possible," for readers who are not Sanskrit scholars.
> *App integration:* ALREADY ALIGNED — the Convert pane's IAST +
> simplified-pronunciation pair (per the ACIP standard) is exactly this
> dual presentation.
**Integration hooks ready:** the Sanskrit converter (IAST/ACIP styles,
simplified pronunciation per the ACIP standard); the pronunciation engine
(GMR card conventions).

### 5. Formatting & presentation
*(title pages, root text vs. commentary layout, verse lineation, folio
references, footnote conventions)*
— partially known from the preliminary-formatting video (banked at
docs/standards/MIXED_NUTS_PRELIMINARY_FORMATTING.md): flow → re-paragraph
at double shads → verses one line each → brackets to folio-tagged notes —

> **STD-004** · formatting · source: *A Song of My Spiritual Life* (tr.
> Chang w/ GMR, ©2025), endnote 5
> When a lama's name is tucked into the syllables of a verse (the
> classical device), the name-parts are ITALICIZED in the English — "in
> Tibetan, a small 'x' mark would typically be carved under them."
> *App integration:* SHIPPED 2026-08-06 — the Draft pane's "Export
> draft (RTF, *italics*)…": text between *asterisks* becomes true
> italics in an RTF the publishing workflow opens directly (verified by
> textutil round-trip: italics render, Tibetan Unicode survives
> exactly); unclosed asterisks are warned about, never guessed shut.
**Integration hooks ready:** the Mixed Nuts prep formatter; the print
export; the sa bcad outline extractor.

### 6. Apparatus & honesty
*(how to mark uncertainty, variants, damaged readings, translator
insertions)*

> **STD-006** · apparatus · source: *Sixty Verses* (tr. Lashaw w/ GMR,
> ©2023), endnote 62
> Where the Sanskrit source is incomplete, inserted fragments are
> declared as insertions and their source edition is cited (bibliography
> entry named). Translator supplements are never silent.
> *App integration:* matches the project's own ⟨…⟩/flag discipline;
> FUTURE — a Draft convention for marking translator insertions.

> **STD-009** · sources & editions · source: *The Sixty Verses of
> Nagarjuna* (tr. Lashaw w/ GMR, ©2023), colophon notes to the combined
> root text and endnote 243
> Declare the edition of every source used. The volume takes the root
> text from the Derge Tengyur ("we have used the straight Derge
> Tengyur version in the combined root text"), RETAINS the commentary's
> own differing embedded root verses because they may descend from a
> different early translation ("we have retained his version of the
> root text, which appears to be a different translation from the
> Sanskrit"), and where editions differ, states which reading was
> adopted and why (n.243: "we have used the version found in the Derge
> edition of the Tengyur, which seems to make more sense"). The
> Sanskrit editions consulted are named in full in the bibliography.
> *App integration:* aligned with the project's provenance rule; the
> bibliography composer carries edition statements verbatim; FUTURE —
> a Draft note convention for variant readings ("Derge reads X; the
> commentary embeds Y; we follow …").

> **STD-010** · apparatus/honesty · source: *A Song of My Spiritual
> Life* (tr. Chang w/ GMR, ©2025), endnotes 28 and 48
> Homonym and spelling variants are DECLARED, never silently
> normalized: when the root text spells a word with a homonym (bsten-pa
> "rely upon" vs. brtan-pa "firm", n.28; so-sor rtog-pa vs. so-sor
> rtogs-pa, n.48), the note gives both readings, the evidence (the
> Sanskrit original where available — pratyavekṣa, n.48 — and standard
> usage), and the meaning adopted. Further attested examples: SSL n.33
> ("spelled rin-cen, which is a rare but acceptable variant for the
> nearly ubiquitous rin-chen") and *Sixty Verses* n.134 (school name
> given with its Tibetan spelling, rNam-rig-pa).
> *App integration:* ALREADY ALIGNED — rule 3 (never guess) and the
> spellcheck layer's review flags embody this; the notes bank makes
> the published examples reusable as models.
**Integration hooks ready:** bracket/note conventions in the prep
formatter; ⟨…⟩ failure marks; provisional labeling throughout.

### 7. Bibliography
*(entry format, section organization, citation of ACIP numbers)*

> **STD-007** · bibliography · source: *A Song of My Spiritual Life* (tr.
> Chang w/ GMR, ©2025; bibliography edited by AN per the working file
> name), bibliography section, entries B1–…
> House entry format for Tibetan works, as published:
> `B<n> · (Epithets/alternate names) Author in technical spelling
> (dates). English Title: Subtitle ("Quoted source work" if any)
> (Tibetan title in technical spelling, ACIP S#####), ff. <folio range>.`
> Example (B3): (Co-ne bla-ma) Grags-pa bshad-sgrub, 1675-1748. A Brief
> Clarification of Heart: A Word-by-Word Commentary to "An Abbreviated
> Presentation of the Steps to the Path" (Lam-rim bsdus-don gyi
> tsig-'grel snying-po mdor-bsdus gsal-ba, ACIP S00184), ff. 1a-11a.
> Sections: works originally in Sanskrit · in Tibetan · in English ·
> the Diamond Cutter Classics series.
> *App integration:* PARTIALLY SHIPPED 2026-08-06 — allcore
> composeBibliographyEntry assembles the house format exactly (battery:
> reproduces published B3 verbatim); Draft pane "Compose bibliography
> entry…" dialog with live preview, optional STD-002 pair-hyphenation
> helper for author names (explicit button, result reviewed — lexical
> hyphenation in titles is NOT derivable, so fields print as typed),
> BDRC scan link derived from the ACIP number (five-digit catalog refs
> now normalize to BUDA's four digits), insertion labeled "NEW ENTRY …
> pending publication" (STD-008 tier discipline). FUTURE: auto-fill
> author/titles/folios from the catalog/title-pair data once Adam
> uploads the Master Catalog (docs/TITLES_INGEST_DESIGN.md).

### 8. Footnote reuse (GMR directive)

> **STD-008** · apparatus/workflow · source: Geshe Michael Roach, as
> reported by Adam Andrade, 2026-08-06 (this session) — *to be confirmed
> in GMR's own words when gathered*
> Any footnote created in any RELEASED Mixed Nuts translation can and
> should be REUSED by translators as the same topics come up — work
> already performed is never performed twice.
> **Refinement (Adam, 2026-08-06):** apparatus enters the OFFICIAL
> documents only once PUBLISHED by our publisher and 100% approved by
> Geshe Michael. Translator-created candidates live in a pending queue,
> clearly labeled, never mixed into the official tier. See
> docs/APPARATUS_DESIGN.md.
> *App integration:* SHIPPED 2026-08-06 — the shared notes bank:
> tools/extract_mixed_nuts_notes.py extracts every endnote from the
> released volumes (344 notes: SSL 53 · OSE 28 · SVN 263) into
> data/extracted/mixed_nuts_notes.json; the Draft pane's notes-bank
> search finds notes by lemma or content and inserts them at the draft
> cursor WITH their source citation ("reused from …, n.N") — reuse stays
> attributed. Also shipped same day: the bibliography bank (138
> published entries, 118 with ACIP catalog numbers, via
> tools/extract_mixed_nuts_bibliography.py); the centralized MAIN
> documents (docs/apparatus/MAIN_FOOTNOTES.md + MAIN_BIBLIOGRAPHY.md,
> generated by tools/build_main_apparatus.py from released volumes
> ONLY); and the pending queue ("Propose footnote" in the Draft pane).
> Regenerate the banks + MAIN documents whenever a new volume is
> released.

## Pending sources (recorded, not yet mined)

- **Diamond Way Translator Course, 2026-08-15** — GMR gives
  translation points on the record, explicitly including
  **removing semicolons** and **adding spaces**. Awaiting the
  recording/transcript from Adam. On arrival: transcribe the
  rulings verbatim, number them as STD- entries with the session
  as citation, and implement the punctuation rules as Draft-pane
  style-check lints (never silently auto-apply -- flag, per the
  house rule that the translator decides).
- **First: ask Ven. Utpala** whether the semicolon/spacing rules
  are already on the house style sheet. If they are, the sheet is
  the authority and the lint cites it; if they are not, this is a
  new ruling from the session and the sheet itself may need
  updating. Resolve which before building the lint, so the app
  never carries a rule the style sheet contradicts.

## Gathering plan (Adam)
- [ ] Ask Geshe Michael directly for standing translation principles
      (recordings or writing).
- [ ] Collect from Mixed Nuts translators (Gibson Chang, Nicholas Lashaw,
      AN, and the team's editor) — including the style sheets already
      promised from the editor (see TODO).
- [x] Mine the three ingested volumes' front matter / translator notes —
      first six sourced entries compiled 2026-08-06 (STD-001…006).
- [ ] Each acquisition → entries here → integration ticket in TODO.
