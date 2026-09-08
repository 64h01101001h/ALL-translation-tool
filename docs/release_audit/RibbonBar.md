# Release audit — `RibbonBar` / `RibbonGroup` / `RibbonProxy`

Source: `app/main.cpp` (single-file Qt 6 app). Audited against the working tree at the time of writing; no source file was modified.

## 1. What it is

The ribbon is the app's primary chrome: a Word-style, full-application-width command band that sits directly under the single tab row of the main window and changes contents to follow the active pane. It is built from three cooperating classes — `RibbonProxy` (`main.cpp:4246`), a `QToolButton` that mirrors a pane's real, hidden `QAbstractButton`; `RibbonGroup` (`main.cpp:4313`), one captioned cluster of controls; and `RibbonBar` (`main.cpp:4358`), the horizontal strip of groups that a pane builds in its constructor and then registers with `attachTo(this)` (`main.cpp:4394`). The user does not "reach" the ribbon — it is always visible. Panes never host their own ribbon in the layout; `main()` collects every registered ribbon into one shared band (`main.cpp:~36470–36540`), places each in a horizontal `QScrollArea`, and swaps the visible one on tab change. The band also carries, at its left, a synthetic `RibbonGroup` of pane-selector tool buttons for the current tab group. The only user-facing ribbon *setting* is **View ▸ Ribbon: show labels** (`main.cpp:37187`).

Design note recorded in the code itself (`main.cpp:4396–4413`): there is deliberately **no overflow menu**. Two attempts were abandoned on 2026-08-23 because the bar lives inside a `QScrollArea` and `finish()` ends with `addStretch()`, so an appended overflow button lands clipped at the far right. The accepted mitigation is the band's horizontal scrollbar.

## 2. User-visible functions

| Control/action | What it does (one line) | Code anchor (main.cpp) | Automated coverage | Notes/risks |
|---|---|---|---|---|
| Ribbon button (proxy) — click | Records `usage::tick("action", <label>)` then forwards `click()` to the pane's hidden source button | `RibbonProxy::RibbonProxy` lambda, 4281–4285 | Indirect only: the `--gauntlet` monkey stepper clicks random enabled, menu-less, non-denied ribbon proxies (41277–41290). No `selfTest()` `check(...)` names the ribbon anywhere (grep for `check(` + ribbon returns nothing) | Coverage is random, not deterministic; a proxy whose source is broken still "clicks" successfully. `usage::tick` writes usage telemetry keyed on the button's visible text — label changes silently break usage history |
| Ribbon button with a menu — click | If the source is a `QPushButton`/`QToolButton` carrying a `QMenu`, the proxy adopts that menu and opens it immediately (`InstantPopup`) | 4273–4280 | NONE — the gauntlet explicitly **excludes** buttons with menus (`!t->menu()`, 41281) | Every menu-bearing ribbon button in the app is outside all automated clicking. This is the single largest coverage hole in the chrome |
| Ribbon toggle button (checkable) | Two-way mirrors checked state with the hidden source button | 4286–4297 | NONE | Mirror is guarded against loops but there is no initial-state resync if the source changes checked-ness before the proxy exists |
| Ribbon button enable/disable | Proxy follows the source's enabled state via an event filter on `EnabledChange` | `RibbonProxy::eventFilter`, 4303–4307 | NONE | Only `EnabledChange` is mirrored — a source that is *hidden* or has its text/tooltip changed later does not update the proxy |
| Ribbon button tooltip (hover) | Shows the source's tooltip, or its text when the tooltip is empty | 4265–4266 | NONE | Tooltip is captured once at construction; later `setToolTip` on the source is not reflected |
| Ribbon button label | Source button text with the ellipsis character `…` stripped | 4262–4264 | NONE | Stripping `…` removes the standard "this opens a dialog" affordance — a discoverability regression the owner may want to reverse |
| **View ▸ Ribbon: show labels** (checkable menu action) | Toggles icon+label vs icon-only for every proxy; persists to `QSettings` key `ui/ribbonLabels` and calls `applyRibbonLabelStyle()` | action 37187, handler 37190–37194; helpers `ribbonLabelsOn` 4234, `applyRibbonLabelStyle` 4239 | NONE (menu actions are exercised by the gauntlet's random menu walk at 41339 only if not denied) | Off state leaves an icon-only bar; the hand-drawn `miniIcon()` glyph set (3993) is small and abstract, so icon-only mode is close to unusable for unfamiliar users. Setting is global and immediate |
| Band pane-selector buttons (left group of the band) | One checkable tool button per pane in the current tab group; click switches `QTabWidget` index; checked state re-synced on `currentChanged` | built 36481–36500, `syncBand` 36531–36540 | Gauntlet switches tabs directly (41263–41276), not via these buttons — so the buttons themselves are NONE | Labels are truncated at the first `" ("` (36484), so a pane titled `Foo (beta)` silently loses its qualifier in the ribbon but not in the tab |
| Band horizontal scroll | Each pane's ribbon is wrapped in a `QScrollArea` with `ScrollBarAsNeeded` horizontally, always-off vertically | 36517–36528 | NONE | This is the *only* way to reach clipped actions; the code comment (4396) states the Catalog ribbon wants ~2,200 px and that QC/HANDOFF clip at normal widths. Actions reachable only by scrolling are effectively hidden |
| Ribbon overflow | **Not implemented, deliberately** | comment 4396–4413 | N/A | Documented dead end; the owner should decide whether to fix properly (host the button in the band, outside the scroller) or accept scrolling |
| Ribbon group caption | Small gold uppercase caption under each group, themed via `ux::themedStyle` | `RibbonGroup::RibbonGroup`, 4322–4331 | NONE | Hard-codes `font-size:9px` — below typical accessibility minimums and not affected by the app's text-size actions |
| Checked-state visual | Gold border + tint so a checked tool looks checked (noted as fix "M4") | stylesheet 4368–4375 (bar) and 36470–36476 (band) | NONE | Colour `#9A7A33` is hard-coded in both stylesheets rather than tokenised through `ux::chromeGold()` — night mode does not adjust it |

### Ribbon groups actually shipped (per pane)

Every group caption in the build, with the line where it is created. This is the full list of ribbon surfaces the user can see:

| Pane (approx. class start) | Groups | Lines |
|---|---|---|
| OverlayPane (4569) | DOCUMENT, REVIEW, SPELLING, WOODBLOCK | 6753–6756 |
| ExportPane (15872) | PUBLISH | 15891 |
| DraftPane (19335) | WORKBENCH, STRUCTURE, EVIDENCE, PUBLISH | 19503–19506 |
| FilesPane (21460) | VIEW, GO, TRANSFER, TOOLS | 21467–21470 |
| LibraryPane (23202) | SHELVE, STUDY, CARE, FIND | 23304–23307 |
| ReviewPane (26272) | REVIEW | 26283 |
| AlignPane (26686) | TEXTS, LINKING, HARVEST | 26700–26702 |
| InputPane (27310) | PAGES, VIEW, TYPING, OCR, PARTNER, EXPORT | 27326–27331 |
| ScanPane / OCR (28893) | PAGE, VIEW, VOLUME, MODELS | 28970–28973 |
| ApprovalPane (30136) | QUEUE, RULINGS, RELEASE | 30150–30152 |
| ManuscriptPane (30835) | MANUSCRIPT, TYPE, HOUSE STYLES, VIEW | 30884–30887 |
| CatalogPane (31614) | ACCESS, SUGGEST, INVENTORY, QC, HANDOFF | 31637–31641 |

Panes with **no** registered ribbon (no `new RibbonBar` in their constructor) fall through to the band's empty placeholder widget (36508–36511): AnalysisPane, ApparatusPane, ScansPane, GoferPane, SanskritPane, TrainerPane, DrillsPane, ProposePane. The owner should confirm each of those is intentional rather than an unfinished pane.

## 3. Internal functions

| Name | Line | Purpose | Called from |
|---|---|---|---|
| `ribbonHost()` | 4221 | Static `QHash<QWidget*,QWidget*>` mapping a ribbon widget to the scroller hosting it in the band | `main()` band construction 36528, `syncBand` 36535 |
| `paneRibbons()` | 4226 | Static hash mapping a pane widget to its registered `RibbonBar` | `RibbonBar::attachTo` 4394; `main()` 36514, 36534 |
| `ribbonProxies()` | 4230 | Static list of every proxy/tool button created, for bulk restyling | `RibbonProxy` ctor 4301, `applyRibbonLabelStyle` 4241, band buttons 36498, gauntlet 41279 |
| `ribbonLabelsOn()` | 4234 | Reads `QSettings("ALL","TranslationTool")` key `ui/ribbonLabels`, default `true` | 4240, 4268, 36493, 37189 |
| `applyRibbonLabelStyle()` | 4239 | Sets `ToolButtonTextUnderIcon` or `ToolButtonIconOnly` on every registered proxy | View menu toggle handler 37193 |
| `miniIcon(kind)` | 3993 | Hand-drawn `QIcon` for a named glyph kind (`page`, `book`, `image`, `folder`, `clock`, `ocr`, `diff`, `save`, `out`, `pecha`, `gear`, `search`…) | `RibbonGroup::addBig` 4349, plus direct `setIcon` calls at 6758, 6761, 6925, 7000, 23352, 26704, 27333, 27336, 27344, 27454, 27463, 27548, 27553, 36488, 36590 |
| `RibbonProxy::minimumSizeHint()` | 4257 | Returns `sizeHint()` so buttons refuse to shrink and Qt cannot elide labels | Qt layout |
| `RibbonProxy::RibbonProxy(src, icon)` | 4259 | Builds the mirroring proxy: icon, de-ellipsised label, tooltip, menu adoption or click forwarding, checkable mirroring; hides the source and installs an event filter | `RibbonGroup::addBig` 4349 |
| `RibbonProxy::eventFilter` | 4303 | Mirrors the source's enabled state | Qt event delivery |
| `RibbonGroup::RibbonGroup(caption)` | 4315 | Lays out a row of controls above a small gold caption | `RibbonBar::group` 4390; also `main()` 36480 for the synthetic pane-selector group |
| `RibbonGroup::add(QWidget*)` | 4334 | Adds a raw widget (checkbox, spinbox, label, line edit) into the group row unchanged | Panes; `main()` 36497 |
| `RibbonGroup::addColumn({...})` | 4337 | Stacks two or three compact widgets in one narrow column | Panes |
| `RibbonGroup::addBig(src, iconKind)` | 4348 | Wraps a real button in a `RibbonProxy` and adds it to the row | Panes (all `addBig` call sites) |
| `RibbonBar::RibbonBar` | 4360 | Creates the strip, sets `#ribbonBar` object name and the checked/hover stylesheet | Pane constructors: 6752, 15890, 19502, 21466, 23303, 26282, 26699, 27325, 28969, 30149, 30883, 31636 |
| `RibbonBar::group(caption)` | 4383 | Appends a vertical separator (except before the first group) and a new `RibbonGroup` | Pane constructors |
| `RibbonBar::finish()` | 4393 | Adds a trailing stretch so groups pack left | Pane constructors |
| `RibbonBar::attachTo(pane)` | 4394 | Registers this bar as the pane's ribbon for the shared band | Pane constructors: 6997, 15924, 19762, 21562, 23574, 26292, 26734, 27556, 29035, 30195, 30992, 31765 |
| `syncBand` (lambda) | 36531 | On tab change, swaps the band's tool stack to the current pane's ribbon and re-checks the matching selector button | `QTabWidget::currentChanged` 36541, and once at construction 36543 |

## 4. Dependencies

- **Data folders/files:** none. The ribbon classes read no data files; all icons are drawn in code by `miniIcon()` (`main.cpp:3993`), so there are no image resources to ship.
- **Network hosts:** none. `RibbonBar`/`RibbonGroup`/`RibbonProxy` make no network calls. (Nearby free functions in the same region of the file — `bdrcScanUrlChecked` at 4415 and `iiifThumbUrl` at 4497 — do produce `library.bdrc.io` URLs, but they belong to the scan viewer, not the ribbon.)
- **QSettings:** organisation `ALL`, application `TranslationTool`.
  - `ui/ribbonLabels` — read in `ribbonLabelsOn()` (4234, default `true`), written by the View menu toggle (37191).
  - Adjacent but not owned by the ribbon: `qat/pins` (37175) drives the Quick Access pinning menu that sits alongside the ribbon chrome.
- **Telemetry:** `usage::tick("action", <button text>)` on every proxy click (4283). The owner should confirm where `usage::` persists and whether it ships enabled.
- **Disk writes:** none directly, beyond the `QSettings` write above and whatever `usage::tick` does.

## 5. Release questions

- **Overflow.** Ship with the documented no-overflow compromise, or do the band-hosted overflow surgery? The code states Catalog's ribbon needs ~2,200 px and that QC and HANDOFF clip — meaning real actions are reachable only by horizontal scrolling on smaller displays. Verify manually at 1280 px and at the smallest supported window size.
- **Menu-bearing ribbon buttons have zero automated coverage** (the gauntlet skips them by design). Decide whether to add deterministic `selfTest()` pins for at least the menu-carrying ribbon buttons before release.
- **No `selfTest()` covers the ribbon at all.** Every row above is "NONE" or "gauntlet-random". Is that acceptable for the app's primary chrome?
- **Icon-only mode.** With `ui/ribbonLabels` off, the whole command surface is 20×20 hand-drawn abstract glyphs with tooltips. Confirm this mode is genuinely usable, or consider removing the toggle for the public release.
- **Ellipsis stripping** (4263) removes the "opens a dialog" convention from every ribbon label. Intentional?
- **Panes with no ribbon** (AnalysisPane, ApparatusPane, ScansPane, GoferPane, SanskritPane, TrainerPane, DrillsPane, ProposePane) show an empty band. Confirm each is deliberate; an empty command band next to populated ones reads as broken.
- **Hard-coded gold `#9A7A33`** appears in two stylesheets (4370, 36473) instead of `ux::chromeGold()`. Verify checked/hover states are legible in night mode.
- **Caption font-size 9px** (4326) is fixed and not affected by the app's text-size actions — check against the accessibility audit already in `docs/ACCESSIBILITY_AUDIT_2026-08-25.md`.
- **`usage::tick` telemetry** fires on every ribbon click, keyed by visible button text. Confirm this is local-only and appropriate for a public build.
