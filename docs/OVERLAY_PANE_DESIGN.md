# Overlay Pane Design — document context on the HGM spine
### v1 · 2026-08-06 · roadmap item 6 · informed by full source study of the ACIP Hypercontext Tool

## What the Hypercontext Tool actually does (source findings)

Ven. Phil Baker's tool (ObjC, 2012–14, ~17.6k lines) is an ACIP text viewer with two lookup
sources queried together on hover:

1. **A static dictionary** (`TibetanDictionary.dic`/`.ndx`, binary, prefix-indexed by a 6-byte
   per-syllable collation key) — hovering a syllable pulls every dictionary phrase *beginning
   with* that syllable.
2. **A per-text glossary** mined from the translator's own markup: `<NNN …>` items give the same
   numeric ID to a Tibetan span and its English rendering elsewhere in the same document
   (`TblHC` merges the pair). Hovering a marked span lights up **both** languages at once, and
   glossary hits display *above* the general dictionary under "FROM THIS GLOSSARY:" — **the
   text's own usage wins over the general dictionary.** That precedence is the "hypercontext."

**Phrase matching** (`findTextPhrases`/`buildPhraseMatchList`): from the hovered syllable, scan
forward no further than the longest candidate; keep *every* prefix length that exactly matches a
candidate; present the **longest match first**; **double-click cycles to shorter nested
matches**, re-selecting the exact span each time so the user sees which reading is active.

**The three-shade nested highlighting Adam wants was designed but never finished** in the
original: three level-colors (word / phrase / larger unit) exist in preferences, six
`CurrHilite` slots and the embedding-aware queries (`embeddedIn`, `bHasEmbedded`,
`TblDT getItem:`) are all in the code — but the shipped renderer only ever painted flat level 1.
We are completing their design, not inventing a new one.

**Weaknesses to avoid repeating:**
- Per-hover linear scans of a rebuilt phrase table (O(candidates × phrase-length) per mouse-move).
- Character-by-character attribute restore on un-hover (destructive, loses styling).
- Main view shows ACIP romanization; Tibetan script appears only in the lower panes.
- No scroll sync; lower panes reset to top on every hover.
- Parse-error UI stubbed out (errors silently discarded).

## What the spine changes

The original needed hand-authored `<NNN>` markup to pair Tibetan with English. **We get the
pairing from data**: 105,634 headwords (96.7% multi-syllable, up to 48 syllables) for
segmentation, and the 42,199-segment aligned corpus for "how did HGM actually translate this
passage" — the corpus is the per-text glossary, generalized to the whole curriculum. Hand markup
becomes optional, not required.

## Design

### 1. Document model (`allcore::OverlayDocument`)
- **Input formats:** ACIP text (incl. `<t>/<e>` language markup — parse it, don't choke),
  Tibetan unicode, EWTS/wylie. Detect per run; normalize internally to a **syllable token
  stream**: each token = {source range, wylie form, unicode form}. Conversion via the C++ ports
  of `hgm_tools.acip_to_ewts` / `ewts_unicode` (roadmap item 7 — the overlay is a consumer;
  until ported, a bundled Python sidecar can pre-tokenize during development).
- **Failed conversions follow the flag conventions:** un-parseable syllables render as
  `⟨wylie⟩` markers, never guessed, and are listed in a real errors panel (fixing the
  original's discarded-errors stub).
- Display: **real Tibetan script by default** (Qt shapes it via HarfBuzz), with a
  wylie/ACIP toggle. This fixes the original's biggest UX gap.

### 2. Segmentation lattice (`allcore::OverlayLattice`) — precomputed, not per-hover
- Build once per document load (and incrementally per edit range later):
  - In-memory **first-syllable index** over headwords+variants (wylie-normalized), built once
    from the spine at startup (~106k entries; hash map syllable → sorted candidate list, with
    per-bucket max-length watermark — same watermark trick as the original, applied globally).
  - For every token position, longest-match scan bounded by the bucket watermark; record
    **every** matching span (not just the longest): `Span {begTok, endTok, entryId, tier}`.
- Store spans in an **interval structure** ordered by (start, −length). Nesting **depth** of a
  span = number of strictly-containing spans. Punctuation (shad) and sentence boundaries cap
  spans exactly as the original's hard barriers did.
- Corpus pass: match runs of the document against `corpus_segments` (FTS5 seed + verify) so
  whole passages that HGM translated can be surfaced with his English — the "FROM THIS
  GLOSSARY" precedence, powered by the corpus instead of markup.

### 3. Nested shading (the ask)
- **Color encodes nesting depth, tier encodes trust:**
  - Depth 1 (innermost word): strongest shade. Depth 2 (phrase): mid shade. Depth 3+
    (sentence-length/definitional entries): lightest wash. Per-depth colors user-configurable
    (three shade slots, honoring the original's unfinished 3-level scheme).
  - Provisional-tier spans (auto-aligned) get a **distinct visual treatment** (e.g. dashed
    underline) at any depth — tier honesty is non-negotiable (CLAUDE.md rule).
- Qt rendering: `QTextDocument` character formats for the static depth wash (computed once from
  the lattice — cheap, no per-hover work), and `QTextEdit::setExtraSelections` for the *active*
  hover/selection highlight — extra selections overlay and vanish without touching stored
  formats, eliminating the original's destructive un-highlight.

### 4. Interaction
- **Hover** = light up the innermost span under the cursor + its nesting chain (all containing
  spans get a border tint) — the multi-level display the original never shipped.
- **Click** = pin the innermost span and fill the context panel.
  **Repeated click (or wheel over a pinned span)** = cycle outward through containing spans,
  longest-first wrap-around — preserving the original's signature double-click length cycling,
  but driven by the precomputed lattice instead of a rescan.
- Selection follows the active span exactly (like `setDefSelection`), so the user always sees
  which reading is active.
- End-of-line guard: ignore hover past the last glyph of a line (the original's clever
  `endLineX` check — keep it).

### 5. Context panel (stacked, tier-ranked)
For the active span, stacked in this order:
1. **Corpus concordance hits for the containing passage** — HGM's own English for this text,
   when the document overlaps the corpus (the hypercontext payoff, from data).
2. **HGM equivalents** for the entry: curated > glossary > auto-aligned, each labeled;
   auto-aligned rendered as `PROVISIONAL (auto-aligned)`. Register-split glosses shown verbatim
   (e.g. *bsod nams* "goodness (prayer register)") — never auto-resolved.
3. **Pronunciation** (⟪card⟫ marker when card-attested), ACIP, variants.
4. **Reference layers** (Hopkins / {LC} Sanskrit), visually subordinate, labeled "reference".
5. **Concordance browser**: `corpus_occurrences` count → click through to FTS-backed segment
   list with course filter.
- Panel updates in place with a stable scroll position per span (fixing the reset-to-top
  annoyance).

### 6. Legacy interop (later, cheap)
- Read Ven. Phil's `<NNN>`-marked ACIP files: parse markup into explicit paired spans that
  merge into the lattice as a "translator layer" ranked above corpus hits. His `.hcprj`
  archives are NSKeyedArchiver plists — read-only import is feasible but low priority.
- `TibetanDictionary.dic` as an extra reference layer stays roadmap item 3 (licensing gate);
  the overlay does not depend on it.

## Build order
1. `allcore`: first-syllable index + lattice builder + interval queries, unit-tested against
   hand-checked segmentations of corpus passages (ground truth: corpus wylie segments).
2. Qt: document view with depth-wash rendering + hover/click/cycle on the lattice.
3. Context panel wired to `Spine` (lookup + englishSearch + corpusSearch already exist).
4. Corpus-passage matching layer ("FROM THE CORPUS" hits).
5. Errors panel, preferences (shade colors), wylie/ACIP display toggle.
