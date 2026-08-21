# Scan viewer — BUDA reference idiom (Adam's screenshot, 2026-08-13)

Adam supplied a full screenshot of the BUDA (BDRC) image viewer
(rnam bshad snying po'i rgyan, I1KG9480007.tif) as **the basis for
our scan viewer**. Banked here for the build-out; his words: "maybe
save this for later, but here is the exact picture/screenshot of the
BDRC image/scan view that I'd like to use as a basis for ours."

## The idiom, element by element

**Layout**
- Left rail: vertical **thumbnail strip**, one thumbnail per image,
  labeled `img.N` beneath each; the current image carries a small
  red marker at its left edge; the rail is collapsible via a slim
  ◀ handle on its right border (red accent stripe divides rail from
  canvas).
- Main canvas: **dark (near-black) background**, the page displayed
  on a white sheet centered in the canvas — the pecha leaf floats on
  the sheet. Dark surround makes the scan pop.
- Top bar (white): archive identity left; center: collection
  browse control + work title (`rnam bshad snying po'i rgyan/ pod…`);
  right: current image filename + close ✕.

**Bottom toolbar (dark), left → right**
1. **Download Images** with a circled ⬇ icon.
2. **Go to img. ▾** dropdown + a number entry box (type an image
   number, jump straight there).
3. **◀ (prev) · "img. 7" · ▶ (next)** — circled arrow buttons with
   the current position label between them.
4. **⊖ zoom out** · **pan pad** (a circled four-direction arrow
   cluster) · **⊕ zoom in** · **⤢ fit/expand** · **image-adjustment
   icon** (brightness/contrast) · **ⓘ info**.

## Mapping to our tool (build plan when Adam calls it up)

Target surface: the standalone woodblock window (today's
"Locate selection on the woodblock…" dialog) grown into THE scan
viewer, and eventually the Input pane's viewer column.

- Thumbnail rail ← we already cache folio images (offline store
  `library/scans/<work>/`); thumbnails labeled by **folio** (`94a`)
  rather than raw img.N — our folio identity is stronger than
  BUDA's; keep an img.N subtitle for parity with BDRC citations.
  Red current-marker + collapsible rail as in the reference.
- Dark canvas + white sheet: straightforward QScrollArea styling.
- Go-to box ← accepts folio ("94a") AND plain image number.
- Prev/next ← existing stepFolio.
- Zoom cluster ← the Input pane's new zoom system (−/+/percent
  box/fit, ⌘-wheel anchored) reused; add the pan pad.
- Download Images ← existing "Download all folio scans" lane.
- Image adjustment (brightness/contrast/invert) — NEW, valuable for
  faint woodblocks; simple QImage lookup-table pass, non-destructive.
- ⓘ info ← folio, work id, license line (public domain note), BDRC
  attribution — we already hold all of it.
- Our additions beyond BUDA: the OCR locate-selection highlight and
  the folio/line band, which BUDA does not have.

## Status
BUILT (task #24): archive-grade viewer window, thumbnail rail, dark
canvas, go-to, zoom, pan, brightness/contrast/invert, whole-work
overview, downloads. This doc remains the spec.

**But Adam could not find it (2026-08-15), and he was right to say
so — two defects, tracked as task #41:**

1. It is reachable ONLY from the Overlay pane, via the "Scan viewer
   (window)…" button. The Input workflow — where scans are actually
   loaded — has no entry point at all, which is where he looked.
2. `openWoodblockViewer()` early-returns whenever `folioOrder_` is
   empty, showing "Link the scans first (Follow along in scans /
   Find scans on BDRC)". To anyone who has not read this doc, that
   dialog reads as "not implemented". It should open on locally
   loaded images when no BDRC folio order exists (the Input pane
   already holds `pages_`), or at minimum offer the two linking
   actions as buttons rather than as prose.

LESSON FOR THIS DOC AND THE TASK BOARD: "built" is not the same as
"findable". A status line that records only the former is how a
finished feature gets reported as missing.
