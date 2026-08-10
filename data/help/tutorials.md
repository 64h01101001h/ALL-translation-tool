# ALL Translation Tool — Help & Tutorials

## Getting Started

**What this app is.** One window, sixteen working areas ("panes"), one
shared dictionary: 105,634 entries anchored to Geshe Michael Roach's
English, plus his aligned corpus of 42,000+ passages. Everything runs
offline except two clearly-marked features (the Analysis pane's AI
report, and the woodblock images from BDRC).

**The three rules the app lives by.** (1) Geshe Michael's English is
the binding layer — everything else is labeled reference. (2) The
tool never guesses — failures are flagged, provisional material LOOKS
provisional (amber-tinted shading). (3) Nothing enters the official
data without approval — proposals wait in the Approval queue.

**Finding anything.** Every pane's functions are also in the MENU BAR
(one dropdown per pane). On a Mac, open Help and TYPE INTO THE SEARCH
BOX — the system finds any menu item and points an arrow at it. This
Help window's own search (below) additionally finds every button and
toggle with a how-to.

**The typical first session.** Library tab → double-click a text →
it opens in the Overlay, shaded → click any colored word → read its
card → switch "text as" to Tibetan script to see it in native script.

## Overlay

**What it's for.** Reading a Tibetan text with the dictionary
underneath — the flagship pane.

**Loading text — two ways.**
1. *From the Library* (easiest): Library tab → double-click any text.
2. *Paste*: click the SMALL editable box at top-left (under
   "Document (ACIP)"), paste ACIP or wylie — the app auto-detects
   which — then press "Load into overlay". The BIG right-hand area is
   the read-only display; you cannot paste there.

**Reading the display.** Every word or phrase the dictionary knows is
shaded. Multi-syllable phrases carry solid color bands (mint, blue,
peach, rose — adjacent phrases differ by color); single-word matches
are a faint lavender; provisional (machine-aligned) glosses show an
amber-leaning tint — that tint is a warning, not a decoration. Red
wavy underline = spelling fails the classical syllable rules. Dotted
amber = a particle disagreeing with the preceding suffix (a review
hint, never auto-corrected). A full volume opens in about two
seconds; shading finishes silently if the text is huge.

**The card.** Click any shaded word: the panel below the display
shows its dictionary entry — the quality tier (curated / glossary /
provisional), Geshe Michael's actual corpus renderings course by
course, reference definitions clearly marked as references, Sanskrit
equivalences (Mahavyutpatti), honorific badge with the ordinary
counterpart, community pronunciations ("also heard"), published
footnotes that deal with this term, your own per-text glossary
(editable in place), and links out to 84000/BDRC/Lotsawa House.
"Propose to the authority…" on the card files a proposal with the
passage auto-attached as evidence.

**Display panel (left).** "text as": ACIP / Tibetan script / Wylie —
same shading, three scripts. Typeface picker for script mode. Each
reference layer toggles on/off; your choices are remembered.

**Scans.** "Find scans on BDRC…" opens the original woodblock images
under the text; the pages turn following your cursor. "locate word
(OCR)" draws a box around the very word under your cursor on the
woodblock (recognition is used only to locate, never as text).

**Publishing utilities.** "Export print Tibetan (Unicode)…" produces
print-ready script. "Prepare for translation (Mixed Nuts)…" runs the
demonstrated translation-preparation format.

## Library

**What it's for.** The preserved canon on your disk — 2,700+ texts of
ACIP Release 6 plus your own folders.

**Use.** Browse the tree or the table (catalog identities decoded
from the filenames: collection, number, verification level).
Double-click a text → it opens in the Overlay. The tree remembers
which folders you had open, across restarts. The table sorts by any
column.

## Analysis

**What it's for.** The full eighteen-section grammatical analysis of
a passage. The deterministic engine extracts everything verifiable
first; an AI model writes the report; the machine re-checks every
quotation. Needs a connection; costs cents per passage; output is
banner-labeled AI, always.

**Use.** Paste a passage → run → read the report; save to the
analyses folder.

## Trainer

**What it's for.** Learning to read, layer by layer — the pedagogy
pane.

**Use.** Paste a passage into the input, press Load. Attempt the
passage yourself, then reveal help one layer at a time with the
checkboxes: (1) clause and phrase boundaries; (2) each particle's
grammatical role; (3) suggested reading order — find the verb first
(from the dictionary's own tense evidence, or honestly marked
unverified); (4) vocabulary with quality tiers; (5) THE ANSWER KEY —
Geshe Michael's own English wherever the passage exists in his
corpus (the machine never invents answers); (6) the full Wilson
parse — every unit designated, case numbers assigned by verb class,
every syllable dot named per the Science of the Dots.

## Drills

**What it's for.** Exercises that write themselves from the corpus —
every answer is Geshe Michael's own text.

**Use.** Pick a drill type from the dropdown: restore a scrambled
clause; fill the blank given his English; choose the correct particle
variant (explained from the classical tables); parallel reading
(reveal his English only when needed); review. "New drill" generates
the next one. Progress is tracked per word and adapts.

## Draft

**What it's for.** The working translator's bench.

**Use.** Paste the source → "Load source". Click any clause in the
middle list → its ANCHORS appear on the right: the clause in Tibetan
script with each established term banded in a color, and below, each
term's entry chipped in the SAME color — which English goes with
which Tibetan reads by color. Click a term for its corpus
concordance. Select Tibetan and press "Phrase memory" to see every
prior rendering (his corpus, binding; plus Align-pane harvests,
labeled PENDING). Other tools: outline (sa bcad), structural units
(bam po), verse meter, VERSE READING ORDER (stanza-by-stanza with the
verb named), quotation detection (exact corpus matches), "Check
terminology" against your English draft, footnote proposals (pending
GMR), bibliography composer, RTF export.

## Review

**What it's for.** A senior translator checking someone else's draft.

**Use.** Paste the Tibetan source on the left, the finished English
on the right, press Review. The report flags — advisory only, never
verdicts: register-sensitive terms (confirm the rendering fits this
text's register); renderings resting on provisional glosses;
established terms that don't appear in the draft; one English word
serving two Tibetan terms; terms with 3+ senses; honorific terms in
the source ("check the English carries the register of respect");
and published footnotes that apply — reuse, don't rewrite.

## Align

**What it's for.** Building tomorrow's dictionary — align Tibetan
with English, harvest translator-authored pairs.

**Use.** Load the Tibetan and English sides. Select a Tibetan span
and its English counterpart, link them; links save per text. Import
.hyp files from the original Hypercontext era. Export candidate
pairs — always labeled PENDING until approved.

## Input

**What it's for.** The input-center workstation — ACE reborn.

**Use.** "Open scan folder…" for a folio-per-file workflow, or open a
single scan. Type in the editor; the scan follows your typing
(proportional, with detected line bands). "Pre-fill from OCR" drafts
the page for you — ONLY onto an empty page, never over your typing —
banner-marked as review material. "Compare with partner's file…"
runs the double-keying diff: orange = only in yours, green seam =
partner has extra; correct until they match exactly. Save exports
the block.

## Search (Gofer)

**What it's for.** Geshe Michael's daily search tool, reborn — same
algorithms, original layout.

**Use.** Type terms into any of the eight "go for" boxes. Choose the
combiner: OR (any term), AND (all terms in the same file), or NEAR
with a proximity (all terms within N lines). Check the targets to
search: the HGM ALIGNED CORPUS (his translations beside the Tibetan),
the PUBLISHED APPARATUS (all 2,234 footnotes + 705 bibliography
entries), and any folders — Add / Remove / Duplicate manage the
folder list exactly as in the original Gofer, and it persists. Press
Find; results appear in the Search Results tab with hits per target.
Save stores the whole search by name in the Saved Search tab
(double-click to reload).

## Convert

**What it's for.** Every writing system, and time itself.

**Use.** Type or paste in the input — ACIP, wylie, or IAST Sanskrit,
auto-detected — and every equivalent renders: the other
transliteration, Tibetan script, and for Sanskrit the full cascade.
The colophon-year calculator converts Tibetan calendar years.
"Sanskrit OCR…" reads a Devanagari image (needs the optional
tesseract install; instructions appear if missing).

## Lookup

**What it's for.** The stacked multi-dictionary.

**Use.** Type a headword (wylie, Tibetan, or ACIP) and press Return.
Results stack: HGM's entry FIRST (binding), then reference
dictionaries, each labeled. Honorific badges, community
pronunciations, published-footnote flags. Affix-stripping fallback
(po'i finds po). "Browse the dictionary (Tibetan order)" lists all
headwords in traditional collation. "propose to the authority…" at
the bottom files a proposal for the searched term.

## Sanskrit

**What it's for.** The whole Sanskrit stack in one workbench.

**Use.** Enter IAST (pramāṇa), Devanagari, or ACIP input-code
(prama#n%a) — auto-detected. Press Analyze: the full cascade renders
(input-code, next-letter, Tibetanized ACIP, Tibetan script,
Devanagari, IPA), then Whitney's root analyses (with past-participle
reverse lookup — gata finds gam), the Mahavyutpatti bridge to
classical Tibetan, and the Monier-Williams link-out. "Sanskrit OCR…"
recognizes a Devanagari image into the same pipeline.

## Scan

**What it's for.** Tibetan text recognition on woodblock images —
embedded, proven, honest.

**Use.** Open a page image → lines are detected and recognized (BDRC
Woodblock model). Output is ALWAYS review material — banner-marked,
first-pass checked against the syllable-legality rules, never
trusted as text. Batch mode processes a volume folder.

## Propose

**What it's for.** The team's channel to the authority.

**Use.** One-time setup at the top: your name, whether you are an
authority (Geshe Michael / Adam only), and the shared proposals
folder (point it at the team Dropbox). Then propose: pick the kind —
honorific, HIGH honorific, humilific, double honorific,
pronunciation exception, abbreviation, word/phrase rendering, note —
enter the Tibetan, the proposed value, and evidence, and file it.
Proposals also file from any entry card ("propose to the
authority…") with the passage auto-attached. "My proposals — what
happened?" shows each of yours with its status and the authority's
comment.

## Approval

**What it's for.** The authority's queue (visible only with the
authority role). Every proposal, ruled one by one — nothing enters
the registers without a ruling.

**Use.** The list shows each pending item with its evidence, source
citation, and proposer. Approve / Decline / Defer — one click, plus
an optional note. Approving a register item (honorific,
pronunciation, abbreviation) applies it in the app immediately,
stamped with your name and date; declining a machine-derived
pronunciation removes it. Dictionary items export instead ("Export
approved dictionary candidates…") for the data project — the app
never edits the corpus. Filter by kind to take one category per
sitting (e.g. just the 2 HIGH honorifics); the list keeps your
scroll position between rulings. "Rulings archive" is the permanent
record of every decision.

## Concepts

**Tiers.** curated (hand-verified) > glossary (from his glossaries) >
provisional (machine-aligned; amber-tinted everywhere it appears).

**Registers.** The honorific register has four levels (per Tournadre):
general honorific (H), HIGH honorific (for high lamas and dignitaries
— rare), humilific (one's own actions — the honorific is never used
of oneself), and double honorific. Pronunciations carry classes:
community, hgm-attested, prenasal-derived (pending ruling), approved.

**Provenance.** Everything machine-derived says so; every approved
item carries who approved it and when; every mined fact carries its
source and page.

**Night mode.** View menu → Night mode: dark chrome, cream reading
pages. Remembered.

**Self-test.** If anything misbehaves: run the app from Terminal with
`--selftest` — 66 checks print PASS/FAIL and localize the problem.
