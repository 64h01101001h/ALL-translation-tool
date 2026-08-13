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
Reference banked; task #24 tracks the build. Not started — Adam
said "save this for later."
