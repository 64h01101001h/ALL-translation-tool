# Menu parity with Word and Sublime Text — what earns its place (Adam, 2026-09-08)

Second reference added by Adam the same evening: **Sublime Text** — its
search / search-and-replace, document window (line numbers, minimap,
line/column status, tab size, syntax label), themes and colour schemes,
and its settings menu. Screenshots are the reference; the steelman still
decides.

Adam asked for the File, Edit, View, Insert, Format, Tools and Window menus
to carry every function Word's do, plus a Word-style Preferences panel,
"where it will suit our translation tool", and for anything added to be
usable and implemented. This file applies the audit's steelman to each
item: BUILD (serves a Tibetan translator on our surfaces), ADAPT (Word's
idea, our meaning), or SKIP (no honest use here — listed so the decision
is visible). Our surfaces: the Overlay Document box (plain ACIP/Wylie),
the Draft (plain English), the Manuscript (rich text), the Input pane.

## File — DONE 2026-09-08
New Document ⌘N · New from Template ⇧⌘P · Open ⌘O · Open Recent · Close ⌘W
(Close Window → ⇧⌘W) · Save ⌘S · Save As ⇧⌘S · Save as Template · Move ·
Rename · Properties ⌥⌘P (General · Summary · Statistics · Content ·
Custom; Tibetan statistics: folios, syllables, shads; sidecar metadata).

## File additions from Sublime — batch 1b
| Sublime | Ours | Verdict |
|---|---|---|
| Set File Encoding to ▸ / Reload with Encoding ▸ / Reopen with Encoding ▸ | encodings for legacy ACIP files: UTF-8 · UTF-8 with BOM · UTF-16 LE/BE · Windows-1252 · MacRoman · Latin-1; reopen the Document box with a chosen decoder; Save with Encoding ▸ | ADAPT (input-centre reality) |
| Save All | save every dirty document across Overlay / Draft / Manuscript | BUILD |
| Print… ⌘P | print the front pane's document (plain text) | BUILD |
| Revert File | reload the Document box from disk, discarding edits (confirm) | BUILD |
| Close File ⌘W · Close All Files ⌃⌥⌘W | Close (done) · close the document in all three panes | BUILD |
| New Window · Split View | one-window app; the Overlay already splits | SKIP |

## Sublime document window — batch 1c
| Sublime | Ours | Verdict |
|---|---|---|
| line-number gutter | toggle (View › Line Numbers) on the Document box and the Draft; persisted | BUILD |
| status bar "Line N, Column M" · Tab Size · syntax label | our status bar: line/column of the focused editor, and the detected script (ACIP / Wylie / Unicode) | ADAPT |
| minimap | skip for now (Focus mode covers distraction-free) | SKIP |
| Select Color Scheme… / Customize · Select Theme… / Customize · Font ▸ | colour schemes for the reading surfaces (several built in, plus "install from file" — a JSON of the ux tokens); theme = chrome (night mode exists); Font ▸ larger/smaller/reset (exists) | ADAPT |
| Settings ⌘, · Settings – Syntax Specific · Distraction Free · Key Bindings | the Preferences panel (batch 7); key bindings = the Pin bar | covered |

## Selection menu (Sublime) — batch 1d
Qt's text widgets hold ONE selection, so Sublime's multi-caret items are
not honestly buildable here; the expand family is, and two are ours.
| Sublime | Ours | Verdict |
|---|---|---|
| Expand Selection to Line ⌘L / Line Upward ⌥⌘L / Word / Paragraph / Whitespace / Indentation / Brackets / Quotes | QTextCursor operations on the focused editor (Word without ⌘D — that stays Look Up) | BUILD |
| Expand Selection to Block | **to Folio**: the text between @NNNA/B markers | ADAPT |
| Expand Selection to Scope ⇧⌘Space | **to Phrase**: the Overlay's nesting chain, one rung outward | ADAPT |
| Expand Selection Until… ⌃⇧S | extend the selection to a typed string | BUILD |
| Reverse Current Selection ⌃⇧R | swap anchor and caret | BUILD |
| Select All | exists | — |
| Split into Lines, Single Selection, Add Previous/Next Line, Add next caret position, Invert Selection, Swap Selections, Tab Selection ▸, MultiEditUtils ▸, Copy To New Files… | multi-caret / plugin items with no single-selection meaning | SKIP |

## Edit — batch 1
| Word | Ours | Verdict |
|---|---|---|
| Undo / Redo | on the focused text widget | BUILD |
| Cut / Copy / Paste | focused widget | BUILD |
| Paste and Match Formatting ⌥⇧⌘V | plain-text paste into the Manuscript | BUILD |
| Paste Special… | **Paste as ACIP / as Wylie / as Tibetan script** — clipboard text run through the converters | ADAPT (genuinely ours) |
| Clear ▸ | delete selection | BUILD |
| Select All | focused widget | BUILD |
| Find ▸ Find… ⌘F · Find Next ⌘G · Find Previous ⇧⌘G · Replace… ⌥⌘F | an in-document find bar for the focused text (Hunt ⌘K stays the cross-corpus search) | BUILD |
| Links…, Select Data…, Toggle Drawing | no OLE, no drawing | SKIP |
| Dictation, Writing Tools, AutoFill, Emoji & Symbols | macOS system features; the Tibetan symbol palette goes under Insert | SKIP (Insert covers symbols) |

## View — batch 2
| Word | Ours | Verdict |
|---|---|---|
| Print / Web / Outline / Draft layouts | **Text as: Tibetan script · ACIP · Wylie · Phonetics** (radio) + Outline (sa bcad) | ADAPT |
| Focus ⌃⇧⌘F | hide the control column and ribbon; reading pane and card only | BUILD |
| Ribbon ⌥⌘R | show/hide the ribbon (labels toggle already exists) | BUILD |
| Sidebar ▸ | Card pane · Spelling doubts panel · Document summary | ADAPT |
| Markup | the Document marks: grammar marks · Botok segmentation · unattested hints | ADAPT |
| Zoom ▸ | Larger · Smaller · Actual size (exists) → submenu | BUILD |
| Enter Full Screen ⌃⌘F | QMainWindow full screen | BUILD |
| Ruler, Header and Footer, Footnotes, Master Document, Reveal Formatting, Gridlines, Message Bar | page-layout notions with no meaning on a text file | SKIP |

## Insert — batch 3
| Word | Ours | Verdict |
|---|---|---|
| Break ▸ | **Folio marker** (next @NNNA/B for the Document box) · page break (Manuscript) | ADAPT (best item in the menu) |
| Advanced Symbol… | **Tibetan & ACIP symbols palette**: tsheg, shad, nyis shad, gter tsheg, yig mgo, ACIP codes for Sanskrit stacks, common apparatus marks | ADAPT |
| Footnote… | Draft: insert from the published footnote bank / propose one | ADAPT (wires existing) |
| Comment | Team comments on this text (exists) | ADAPT (wires existing) |
| Date and Time… | insert, with Tibetan-calendar year via the Convert engine as an option | BUILD |
| File… | insert a text file's contents at the cursor | BUILD |
| Hyperlink… · Table… · Pictures ▸ | Manuscript only (rich text) | BUILD (Manuscript) |
| Bookmark…, Cross-reference…, Caption…, Index and Tables…, Text Box, AutoText, Object…, Field…, Equation, Shape, SmartArt, Chart, Audio, Movie, Icons, 3D Models, Watermark, Page Numbers | no honest use on a Tibetan text file or an English draft | SKIP |

## Format — batch 4 (mostly Manuscript)
| Word | Ours | Verdict |
|---|---|---|
| Font… | font dialog for the Manuscript selection; Tibetan face picker for the Overlay | BUILD |
| Paragraph… | alignment · spacing · indent (Manuscript) | BUILD |
| Bullets and Numbering… | Manuscript lists | BUILD |
| Change Case… | English: Sentence · Title · UPPER · lower. **Tibetan: To ACIP · To Wylie · To Tibetan script** (transliteration, not case) | ADAPT |
| Style… | the Manuscript HOUSE STYLES gallery (exists) | ADAPT (wires existing) |
| Document… | page setup for RTF/PDF output | BUILD |
| Text Effects, Borders and Shading, Columns, Tabs, Drop Cap, Text Direction, Asian Layout, Font Substitution, Equation Options, Selection Pane | print typography that pecha export already owns, or no meaning here | SKIP |

## Tools — batch 5
| Word | Ours | Verdict |
|---|---|---|
| Spelling and Grammar ▸ | Overlay spelling doubts (show / check all / file for ruling) · Draft house-style check | ADAPT (wires existing) |
| Thesaurus… | **Phrase memory**: every corpus rendering of the selected Tibetan (exists in Draft) | ADAPT |
| Translate ▸ | Send selection to Draft · to Manuscript | ADAPT |
| Word Count… | Properties › Statistics, opened on that tab | BUILD |
| Protect Document… | toggle the file's read-only attribute (shown in Properties › General) | BUILD |
| Templates and Add-ins… | open the templates folder | BUILD |
| Track Changes ▸ | not for 1.0; the Review pane is the oversight surface. Noted for later | DEFER |
| Language…, Hyphenation, AutoCorrect, Check Accessibility, Combine Documents, Block Authors, Envelopes, Labels, Macro, Add-ins, Customize Keyboard | no honest use, or the Pin bar already covers it | SKIP |

## Window — batch 6
| Word | Ours | Verdict |
|---|---|---|
| Minimize ⌘M · Zoom · Bring All to Front | standard | BUILD |
| open-window list | our floating windows: Scan viewer, Lookup popup, reports, Properties | BUILD |
| New Window, Arrange All, Split, Fill, Center, Move & Resize, Tile, Move to iPad | one-window app; macOS handles tiling itself | SKIP |

## Preferences panel — batch 7 (replaces the four-group Settings dialog)
Icon grid in Word's three bands, back/forward, search box; each page is a
form bound to the settings store; every existing setting moves in.
- **Authoring and Proofing:** General (night mode, startup pane, recents) ·
  View (ribbon labels, text as, shading, card layers preset) · Ribbon &
  Toolbar (pin bar, ribbon sections) · Edit (autosave interval, paste
  default) · Spelling & Grammar (spelling doubts on/off, Monlam hints) ·
  Accessibility (font size floor, high contrast) · **AI** (where the key
  lives, consent to send text, model label) · **Tibetan & Sanskrit** (faces,
  pronunciation convention, Sanskrit style).
- **Output and Sharing:** Save (defaults, sidecar location) · Print / Pecha
  (layout preset, cover sheet) · Updates (check for dictionary/app updates,
  team folder).
- **Personal Settings:** User Information (name, role) · Security (admin,
  roster) · File Locations (data root, library, templates, glossaries,
  properties) · Privacy (every outbound host, on/off).
- SKIP: Copilot (ours is "AI"), Add-ins, Show Your Pride, Track Changes.

## Sequencing and the honest caveat
Batches 1–7 are new features arriving inside the two-week release window.
Each is small and self-tested, but together they are several working
days that the audit walk does not get. Adam's call on order; default is
the order above, one batch per commit, gates green at every step.
