# Text ↔ Woodblock Sync — design ruling and architecture
*2026-08-13 · answering Adam's question: "format a new version of the
ACIP file, or OCR the scan and solve it from the scan side, or both,
or neither?"*

## The ruling: mostly neither — then both, in a strict order

The instinct that this is a hard alignment problem is right in
general, but **our data already contains the alignment**. The input
centers keyed the library the way they did precisely so this would be
possible one day:

1. **Every ACIP file carries `@folio` markers** (`@001A`, `@012B`) at
   the exact points where the woodblock sides turn.
2. **The input centers preserved the woodblock's own line breaks** —
   one typed line per carved line. So counting newlines after the
   marker gives the exact carved line number. No inference.
3. **BDRC's IIIF manifests label every scan image with its folio
   side** ("94a", "94b") — folio → image is *their* catalog metadata,
   not our guess.

That means text-position → (folio, line) → scan image is **pure
deterministic arithmetic end to end**. No new file format, no
alignment algorithm, no machine learning. Reformatting the ACIP would
re-derive information it already carries; solving from the scan side
(OCR the whole volume and align text to it) would replace exact
markers with probabilistic output — strictly worse on both counts.

## Where each side's tool IS used

- **Text side (the folio-faithful export)** — kept, as an *export*,
  not a reformat: "Export folio-faithful text files…" splits the
  document at its markers into one file per woodblock side
  (`001a.txt`, `001b.txt`…), named to sort beside the downloaded scan
  files (`001a.jpg`). That is the OCR-ready, side-by-side keying
  target Adam described — derived from the markers, so it can never
  disagree with them.
- **Scan side (OCR)** — used only for the **last inch**: locating the
  selected words *horizontally within the line*. Line detection
  (BDRC PhotiLines) + recognition (BDRC Woodblock) produce word
  boxes; we search the recognized text for the selection's wylie and
  highlight the covering boxes on the carving. The recognized text is
  **never surfaced as text** (the project's review-material rule);
  it is a locator. When OCR misses, the approximate line band (uniform
  arithmetic, labeled approximate) still shows — the feature degrades
  honestly, never silently.

## The vertical stack (all shipped 2026-08-13)

| Layer | Mechanism | Authority |
|---|---|---|
| position → folio | last `@NNNa/b` marker before the cursor/selection | the file's own markers |
| position → line | newline count since the marker | input centers' preserved lineation |
| folio → image | BDRC IIIF manifest labels | BDRC catalog |
| image → local file | offline folio store `library/scans/<work>/094a.jpg` | our download, folio-named |
| line → pixel band | uniform division of page height (labeled approximate) | arithmetic |
| word → pixel box | OCR word boxes, deskew 0° (labeled deviation) | locator only |

## Surfaces

- **Overlay, right-click in the document**: header `folio 94a · line
  3/7`, then *Show this folio in the scan panel* · *Open this folio in
  the Input workflow* (downloads the one image if needed, hands the
  local file to the Input pane and raises it) · *Locate selection on
  the woodblock…* (new zoomable window, OCR word-box highlights +
  line band) · *Download all N folio scans (offline + Input)…* ·
  *Export folio-faithful text files…* · *Copy location*.
- **Follow-along caption** now advertises the right-click after
  mapping ("the suggestion" — the volume download is proposed the
  moment scans are linked).
- **Input pane, right-click in the editor**: the nearest typed
  `@folio` marker resolves, and if the open scan folder contains a
  filename carrying that folio token (the downloaded store always
  does), *Jump to this folio's scan (page N)*.

## The download suggestion (Adam's "highly suggest" idea)

"Download all folios" writes folio-named files into
`library/scans/<work>/`. From then on: the Overlay's follow-along
prefers the local store (works offline), "Open scan folder…" in the
Input workflow opens the whole volume in folio order, and folio jumps
on both sides are instant. One folder is the shared ground between
reading and keying.

## Honesty rules carried through

- Line band: labeled "approximate" (uniform arithmetic).
- OCR: locator only; recognized text never shown as text; deskew-0
  labeled as a deviation from the BDRC pipeline.
- Unmatched words: named in the caption ("Not found on this side:
  …"), never silently dropped.
- Scan volume identity: deterministic catalog mapping where we have
  it, else the user-confirmed title-search link — never asserted.
