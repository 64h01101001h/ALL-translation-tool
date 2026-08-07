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
— nothing compiled yet beyond the dictionary itself —
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
> Epithets referring to a specific person are capitalized: "Realized One"
> is capitalized when it refers to Arya Nagarjuna.
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
> *App integration:* print-export / Draft formatting note; FUTURE —
> italics support in the print pipeline.
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
**Integration hooks ready:** bracket/note conventions in the prep
formatter; ⟨…⟩ failure marks; provisional labeling throughout.

## Gathering plan (Adam)
- [ ] Ask Geshe Michael directly for standing translation principles
      (recordings or writing).
- [ ] Collect from Mixed Nuts translators (Gibson Chang, Nicholas Lashaw,
      AN, and the team's editor) — including the style sheets already
      promised from the editor (see TODO).
- [x] Mine the three ingested volumes' front matter / translator notes —
      first six sourced entries compiled 2026-08-06 (STD-001…006).
- [ ] Each acquisition → entries here → integration ticket in TODO.
