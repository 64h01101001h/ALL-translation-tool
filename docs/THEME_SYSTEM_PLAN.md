# Theme System Plan — Papers

Status: proposal, awaiting Adam's rulings (§11). Written 2026-09-09 against the
current tree. Every number quoted here was measured by the scripts banked in
`tools/theme_tune.py` (to be committed with step 1) and re-measured against the
live source; the Qt behaviour claims in §5 were verified by compiling and
rendering, not read from documentation.

---

## 1. What a theme is, and what it is not

A theme is **a paper**: one page colour, one ink, one accent, one chrome scheme,
from which every other ground in the app is computed by a solver that has to
pass twenty-two measured gates before the theme can ship.

A theme is **not** a colour table anyone can hand-edit, and it is **not** a way
to change what a colour means: the seven epistemic hues (HGM binding, evidence,
reference, machine, AI, advisory, people), the five badge words, and the five
badge shapes are frozen in identity across every paper, and only their lightness
and chroma are re-derived so they stay legible on the paper they are standing on.

A theme therefore answers one question, "what surface am I reading on?", and is
structurally unable to answer the other one, "whose English is this?"

---

## 2. The token model

### 2.1 Where the code goes

| File | Holds |
|---|---|
| `app/ux_tokens.h` (existing) | the frozen constants (unchanged), the colour arithmetic, the `Palette` type, and the accessors every pane calls |
| `app/ux_theme.h` (new) | the eight theme seeds and `solve()` — the only file that knows what a paper is |
| `tools/theme_tune.py` (new) | the Python oracle. Same arithmetic, diffed against the C++ by the battery, the way the engine ports are diffed against `engines/` |
| `tools/theme_golden.tsv` (new) | the pinned solved palettes. A **data** file, not source, so no colour table ever enters a file the constitution scans |

The palettes are **computed at startup from the seeds** (about 2 ms for all
eight) and pinned by the golden file to a tolerance of 1/255 per channel. That
choice is not cosmetic: it is what keeps §7 honest.

### 2.2 Additions to `app/ux_tokens.h`

`srgbLin` already exists. The `const char*` versions of `luminance` and
`contrastRatio` stay and delegate, so there is one arithmetic in the codebase.

```cpp
// ---- WCAG helpers, extended to QColor -----------------------------------
inline double luminance(const QColor& c) {
    return 0.2126 * srgbLin(c.red()) + 0.7152 * srgbLin(c.green())
         + 0.0722 * srgbLin(c.blue());
}
inline double contrastRatio(const QColor& a, const QColor& b) {
    const double la = luminance(a), lb = luminance(b);
    const double hi = la > lb ? la : lb, lo = la > lb ? lb : la;
    return (hi + 0.05) / (lo + 0.05);
}

// ---- OKLab / OKLCH ------------------------------------------------------
struct OkLch { double L, C, h; };            // h in degrees
double  lin2srgb(double v);                  // inverse of srgbLin, 0..255
OkLch   toOkLch(const QColor& c);
bool    okRaw(double L, double C, double h, double* rgb);   // in-gamut test
QColor  fromOkLch(double L, double C, double h);  // clamps CHROMA, never L or h
double  maxChroma(double L, double h);            // bisection, 20 steps
double  deltaE2000(const QColor& a, const QColor& b);   // battery only

// ---- the solved palette -------------------------------------------------
struct Palette {
    QString id, name, forWhom;
    Qt::ColorScheme chrome = Qt::ColorScheme::Light;
    bool   achromatic = false;          // Xylograph pair: no hue channel at all

    QColor paper, ink, muted, faint, soft;
    QColor plaque, chip, rule, focus, selection;   // chip = the badge ground

    QColor wash [5][3];   // slots 0..3 cycle; slot 4 is the singles neutral
    QColor hatch[5][3];   // the provisional stripe colour for each ground
    double zebra = 0.020; // okL lift applied to odd-parity phrases

    QColor band[4], bandHatch[4], bandInk, kbd;   // the X-ray highlight
    QColor tier[7];       // Binding Evidence Reference Machine Ai Warn People
    QColor mark[4], bandMark[4];      // machine grammar unattested spelling
    QColor diff[7];       // change insert delete minor moved filler mark
    QColor dna[5], icon[6];

    double inkFloor = 0.0, hatchFloor = 0.0;   // measured; printed in Preferences
};

const Palette& T();                       // solved once, re-solved on paper change
bool  setPaper(const QString& id);        // writes the setting, re-solves, re-paints

// ---- the two seams every pane uses --------------------------------------
QString paint(QString html);              // "@muted" -> "#6E675D", one regex pass
void    setDoc(QTextEdit* v, const QString& html);   // paint + scaleCardPx +
                                                     // re-paint on paper change
void    skin(QWidget* w, std::function<QString()> mk);  // themedStyle, token-aware

// ---- the reading view ---------------------------------------------------
QColor  washColor  (int slot, int depth, int phraseIx);
QBrush  groundBrush(int slot, int depth, int phraseIx, bool provisional, qreal dpr);
int     brushId    (int slot, int depth, int phraseIx, bool provisional); // run-merge key
```

`fromOkLch` gives up chroma and never lightness or hue. A clipped channel
silently shifts the hue, which is exactly what destroys a hue wheel, and a hue
wheel is the identity channel in §4.

### 2.3 How a pane file uses it, in one line

```cpp
// any HTML view
ux::setDoc(card_, entryHtml(e));

// any widget
ux::skin(banner_, []{ return ux::paint(
    "QLabel{color:@muted;background:@plaque;border-top:1px solid @rule;}"); });
```

Those two calls are the whole API. `setDoc` also fixes a live bug in passing:
`scaleCardPx` (main.cpp:10729) already rewrites card HTML at render time, but it
is wired to exactly one caller, `OverlayPane::setCardHtml` (10753). Every other
view calls `setHtml` directly and never gets the card scaling. Moving the seam
gives all of them both behaviours at once.

### 2.4 How the hardcoded sites migrate

Counted in the current tree:

| Form | `app/main.cpp` | `app/*.inc` | Total |
|---|---:|---:|---:|
| `#rrggbb` | 474 | 42 | 516 |
| `#rgb` | 184 | 1 | 185 |
| `QColor(0x..,0x..,0x..)` / `qRgb` | 55 | 21 | 76 |
| **colour literals** | **713** | **64** | **777** |
| `setHtml(` call sites | 171 | 2 | 173 |

A builder writes `style='color:@muted'` instead of `style='color:#777'`, and
`ux::paint` resolves it against the live palette. An unknown `@name` is a hard
failure in debug and renders `#FF00FF` in release, so a typo cannot ship quietly.

The mapping is **re-based, never transliterated**. `#777` appears 119 times, 38
of them inside `<small>` and 18 at an explicit 10–11px, and it measures **4.15:1
on cream**: it fails AA at the size it is used. `#888` (7 sites) is 3.29 and
`#c80` (2 sites) is 2.75. These map onto audited palette values, not onto their
own failing numbers. The parallel untokenised family collapses at the same time:
`#9A7A33` ×28 (a second gold, 3.74:1), `#555` ×30 and `#666` ×7 and `#888` ×7
(three more spellings of `@muted`), `#B00` ×7 (a second error red), `#6B5E4E` ×8
(a fourth muted), `#8A6D1F` ×9 and `#7A5A00` ×9 (two more golds). Counts are from
the current tree, not from the census, which has drifted.

Two idioms the codebase already invented are generalised rather than joined by a
third: `compare_pane.inc:39-45`'s seven accessor functions become `ux::T().diff[]`,
and `TierSkin` / `skinFor` becomes the shape of `Palette::tier`.

### 2.5 The five surface classes, all declared

The code claims two surfaces. There are five, and only two obey the current rule.

| | Surface | Follows the paper? |
|---|---|---|
| S1 | Chrome: ribbon, tabs, QAT, menus, dialog widgets | Yes, via `Qt::ColorScheme` |
| S2 | Page: every `QTextEdit`/`QTextBrowser`/`QPlainTextEdit` | Yes. This is what changes |
| S3 | Manuscript dialogs (Preferences, Help, Hunt): hardcoded parchment `#F1EBDD` / `#C9B992` / maroon heads | Yes. It joins the theme; it has no defence for being hardcoded, and its maroon head is `kError` doing decorative duty, which the palette replaces with `@dialogHead` |
| S4 | Light table, the folio/scan viewer, hardcoded `#17181C` | **No, by declaration.** It is a light table and the scan is the subject. `Palette` carries a separate light-table block so it still feels like the same app, and its BUDA accent `#C22A2A` at **2.31:1** on `#2B3038` is a live failure that gets re-derived on the way through |
| S5 | Rasters: Text-DNA strip, scan overlays, thumbnails, icon families | Yes, but no stylesheet reach. Each gets an explicit `ux::onChromeChange(w, rebuild)` and reads `ux::T()` at paint time, never at construction |

---

## 3. The shipped papers

Eight records, seven distinct page palettes. Every hex below is solver output,
not a hand pick.

### Manuscript Day — the default
Paper `#FAF6EE` · ink `#2B2118` · accent `#82672A` · light chrome
*For everyone who has the app now, and anyone reading in a lit room.*
Washes d1 `#D4EDDC` `#F6DEEF` `#D3E9FD` `#F7E2CE`, singles `#DEDDE2`
d2 `#C3DBCA` `#E3CCDD` `#C1D7EA` `#E5D0BC`, singles `#CCCBD0`
d3 `#B1C9B8` `#D1BACB` `#B0C5D8` `#D3BEAB`, singles `#BBB9BE`
Band `#FADB9C` `#F8CA63` `#ECBA45` `#DBAC40` · chip `#FEFDFA` · plaque `#EFECE7` · rule `#DAD8D5`
Tier inks: **all five badge inks are the shipped values, unchanged** —
`#82672A` `#1E6B4E` `#2E629E` `#B4540A` `#8C2F2B`.
Ink floor 8.10 (AAA). Distance from today's washes: 1.77–5.01 ΔE00, so the page
moves a little and does not change character. The families survive: green, blue,
peach and rose still cycle; the singles ground goes from lavender to a warm-grey.

### Manuscript Night — the default night paper
Paper `#FAF6EE` · ink `#2B2118` · accent `#C9A55C` · **dark chrome**
*For working late with a lit page. Cream page under dark chrome, which is exactly
what `app/appearance = night` renders today.*
Shares Manuscript Day's solved page palette object: same paper, same solve, one
thing to maintain and one thing to prove. This is why nobody's screen changes on
upgrade (§9).

### Butter Lamp — a true night
Paper `#1E1B16` · ink `#EFE7D8` · accent `#C9A55C` · dark chrome
*For long evening sessions, for photophobia and migraine, and for anyone whose
eyes hurt at a 14:1 bright page after two hours. Warm, not blue-grey: Tibetan
script should read as script, not as terminal output.*
d1 `#2A270D` `#23233A` `#062C2C` `#381D20`, singles `#342417`
d2 `#37351B` `#313148` `#163A3A` `#472B2D`, singles `#423224`
d3 `#464329` `#3F3F58` `#254948` `#56393B`, singles `#514032`
Band `#AE8730` `#BE9436` `#CFA13B` `#E0AF41` with band ink `#1E1B16`. On a dark
page a lit phrase is a bright plate carrying its own dark ink; that is the only
construction that keeps the deepest rung above 4.5:1.
Tier inks `#A28033` `#379570` `#4485D1` `#D56200` `#D75F57`. Ink floor 8.03 (AAA).

### Slate — cool dark
Paper `#1B2028` · ink `#DDE4ED` · accent `#7FB2E8` · dark chrome
*For cataloguers and metadata editors who live in tables and want the neutral
dark their other tools use, with nothing warm competing with the data.*
d1 `#29253D` `#2A2B11` `#3C2020` `#073032`, singles `#322D2E`
d2 `#37334C` `#38391E` `#4B2E2E` `#183E40`, singles `#403B3C`
d3 `#45425B` `#47482C` `#5B3C3C` `#274D4F`, singles `#4F494B`
Band `#308ADE` `#3D98EF` `#5DA7F1` `#79B5F4`, band ink `#1B2028`.
Tier inks `#A58336` `#3A9873` `#4A8CD8` `#DE6700` `#DA625A`.
Ink floor 6.87. Slate and Palm Leaf are the two papers that do not quite reach
AAA on their deepest ground; Preferences prints 6.9:1 rather than rounding up.

### Catalogue — cool light
Paper `#EEF1F5` · ink `#1A2028` · accent `#26707C` · light chrome
*For long days in lists, collation and reports, where warm parchment fights the
data and every tint reads as a status.*
d1 `#D2E6FC` `#F4E0C9` `#F6DAEB` `#CFECDB`, singles `#E8EBDF`
d2 `#C0D4EA` `#E2CEB8` `#E4C9D9` `#BDDAC9`, singles `#D6D9CD`
d3 `#AEC2D8` `#D0BCA6` `#D1B7C7` `#ACC8B8`, singles `#C4C7BC`
Band `#9EECFA` `#65E3F8` `#4DD4EA` `#46C5D9`. Tier inks are the shipped values
except MACHINE, which darkens to `#B15100` to hold 4.5:1 on this chip.
Ink floor 8.83 (AAA). The accent is deliberately **not** `kDoc #2E629E`: the
first solve used it, and G6 caught the band sitting 1.36 ΔE00 from the documents
ink. A theme accent may not be an epistemic ink (§5, §7).

### Palm Leaf — warm sepia
Paper `#F4ECD8` · ink `#33291B` · accent `#7A5E20` · light chrome
*For scan and collation work. When a folio scan is open beside the text, a white
page next to a beige scan makes the eye re-adapt on every glance; a page near the
scan's own luminance stops that. Also the best colour-vision margins in the set.*
d1 `#DEE3C2` `#E0DAFB` `#BEE9ED` `#FCD5D1`, singles `#D9D9D9`
d2 `#CDD1B0` `#CEC8E9` `#ACD7DB` `#EAC3BF`, singles `#C7C7C7`
d3 `#BBC09F` `#BDB7D6` `#9BC5C9` `#D7B1AE`, singles `#B5B5B5`
Band `#F9D387` `#F7C149` `#E6B343` `#D5A63D`. Tier inks are the shipped values
except MACHINE, which darkens to `#AD4E00` to hold 4.5:1 on this chip.
Ink floor 6.95. Cycle separation under protanopia 8.45 and deuteranopia 4.77,
the widest margins in the set, so this is also the paper to recommend to a
colour-blind translator who wants a light page.

### Xylograph — high contrast, no hue at all
Paper `#FFFFFF` · ink `#000000` · light chrome, forced high contrast
*For low vision, for every kind of colour-vision deficiency including tritanopia
and achromatopsia (which no coloured theme can serve), for monochrome printing
and photocopying, for e-ink, and for bright sunlight. Named for the par ma, the
woodblock print: black ink on paper, no colour, and it has carried this
literature for six hundred years.*
Two parity ladders instead of four hues.
even `#D9D9D9` `#C1C1C1` `#A9A9A9` · odd `#E9E9E9` `#D1D1D1` `#B9B9B9`
Band `#555555` `#404040` `#2B2B2B` `#181818` with white band ink (reversed out).
Rule `#989898` (3:1 non-text, SC 1.4.11). Ink floor 8.94.
Tier inks darken to hold 7:1: `#6F5200` `#006345` `#1258A0` `#8F4000` `#8C2F2B`.
Every phrase additionally carries a 2px ink bar at its start and a 1px rule at
its end. This theme is not a special case bolted on; it is the redundant channel
every paper already ships, with the colour turned off. If Xylograph reads, every
paper reads, which is why it cannot rot.

### Xylograph Night — high contrast, inverted
Paper `#000000` · ink `#FFFFFF` · dark chrome, forced high contrast
*Many low-vision readers specifically need light on dark. Forcing a choice
between high contrast and a dark page is a real accessibility failure.*
even `#222222` `#343434` `#484848` · odd `#161616` `#282828` `#3A3A3A`
Band `#717171` `#898989` `#A1A1A1` `#BABABA`, band ink `#000000`. Ink floor 9.15.
Preferences says in one line that pure white on pure black causes halation for
some readers, and points at Butter Lamp and Slate, which are AAA without it.
Offering the maximum and naming its cost is more honest than quietly capping it.

### The gate card for all eight

| Paper | L0 | C | rot | ink floor | on hatch | depth | hue id | edge | vs paper | zebra ΔL\* | tier pairs | tier ink | G6 |
|---|---:|---:|---:|---:|---:|---:|---:|---:|---:|---:|---:|---:|---:|
| Manuscript Day / Night | .050 | .0353 | 70 | 8.10 | 6.66 | 4.01 | 16.50 | 9.63 | 8.27 | 2.17 | 18.34 | 4.62 | 25.7 |
| Butter Lamp | .045 | .0414 | 20 | 8.03 | 5.82 | 4.34 | 15.05 | 11.15 | 10.10 | 2.14 | 18.24 | 4.51 | 14.0 |
| Slate | .040 | .0432 | 40 | 6.87 | 5.25 | 4.38 | 15.23 | 11.10 | 10.75 | 2.15 | 18.76 | 4.58 | 14.2 |
| Catalogue | .040 | .0374 | 40 | 8.83 | 6.80 | 3.99 | 17.87 | 9.51 | 8.60 | 2.17 | 18.20 | 4.58 | 30.1 |
| Palm Leaf | .040 | .0447 | 30 | 6.95 | 5.62 | 4.14 | 16.01 | 13.38 | 8.15 | 2.17 | 18.25 | 4.55 | 24.2 |
| Xylograph | .090 | 0 | — | 8.94 | 6.75 | 8.63 | n/a | 5.65 ΔL\* | 7.65 ΔL\* | 5.65 | 15.52 | 7.17 | 20.9 |
| Xylograph Night | .225 | 0 | — | 9.15 | 6.58 | 8.48 | n/a | 5.59 ΔL\* | 7.25 ΔL\* | 5.59 | 18.09 | 7.02 | 20.6 |

Two physical facts the solver discovers and a hand-tuned table would get wrong:
pure white needs a first rung of 0.090 okL against cream's 0.050, because near
okL 1.0 the sRGB gamut caps chroma and the washes must step further from the
paper before they can be told apart; and pure black needs 0.225, because okL near
zero buys almost no luminance. The paper decides the depth.

---

## 4. The shading rule

### 4.1 Four meanings, four channels

The shipped design puts identity, depth and tier on one channel and they collide.
Measured on the current build: peach and rose sit **11.19 ΔE00** apart where 15
is needed to be identifiable, whisper and rose **7.86** where 8 is needed to sit
side by side, a provisional wash of one hue lands **2.18 ΔE00** from a confirmed
wash of another (a machine gloss reading as a settled phrase, which is a house
rule 2 defect on cream today with no theme system involved), and the lightness
difference across a phrase boundary is **0.40 ΔL\***, which is nothing.

| Meaning | Channel |
|---|---|
| which phrase | OKLCH **hue**, four hues at quadrature, plus a fifth non-cycling neutral for single-syllable matches |
| how deep | OKLCH **lightness**, always away from the paper |
| settled or machine-matched | **texture**, a 45° hatch |
| where a phrase ends | **parity zebra**, ±okL by phrase index |

### 4.2 The formula

Inputs: paper `P`, ink `I`, accent `A`, max depth 3.

```
1.  Lp = okL(P);  s = (Lp >= 0.5) ? -1 : +1      // washes move AWAY from paper
2.  R = 7.0 (AAA target), retry 4.5 if the ramp will not fit
      Ylim  = (s<0) ? R*(Y_I+0.05) - 0.05 : (Y_I+0.05)/R - 0.05
      budget = |okL(grey(Ylim)) - Lp| - 0.030    // 0.030 = cliff guard
3.  Ld = 0.055 okL per rung, fixed.  L0 solved upward from 0.040 in 0.005 steps.
      L(d) = Lp + s*(L0 + (d-1)*Ld)
4.  hues = h_accent + rot + {0, 180, 90, 270}, IN CYCLE ORDER, rot searched 15..85
      C = 0.90 * min over (d,k) of maxChroma(L(d), h_k)
      accept when: hue identity >= 15, edge >= 8, vs-paper >= 8,
                   protan and deutan cycle separation >= 3,
                   and no ground within 12 dE00 of a frozen ink (G6)
      score = 0.5*min(identity/18,1) + 0.5*min(min(protan,deutan)/6,1)
5.  singles neutral: hue, chroma and rung offset solved jointly so it clears
      8 dE00 from bare paper AND 8 from every chromatic wash
6.  band: accent hue at 0.85 * maxChroma, four rungs 0.045 okL apart, placement
      searched until it clears 10 dE00 from every wash, carries at least 2x the
      wash chroma, and holds 4.5:1 with either the page ink or the paper
7.  hatch line = same hue, same chroma, okL shifted by s * 0.075
8.  zebra = 0.020 okL on odd-parity phrases, constrained to <= 0.5 * Ld
```

`QColor::darker(108)/darker(118)` is retired. It is a uniform multiply of the
sRGB encoding: on cream it happens to give a constant 0.054 okL step, and on a
dark page it collapses. The ramp is expressed in okL, and `Ld = 0.055`
reproduces today's step to within 0.5 ΔE00.

**The hatch step is 0.075 and not 0.055 for a measured reason.** At 0.055 the
stripe colour is *exactly* the next depth rung, and the collision measures
ΔE00 0.00: a provisional phrase at depth 1 would be striped in depth 2's colour.
The gate `|hatchStep − Ld| ≥ 0.015` exists because that was hit.

### 4.3 The call sites

```cpp
static QColor washColor(int slot, int depth, int phraseIx) {
    if (depth <= 0) return QColor(0, 0, 0, 0);
    const QColor b = ux::T().wash[((slot % 5) + 5) % 5][qMin(depth, 3) - 1];
    if (!(phraseIx & 1)) return b;
    const ux::OkLch c = ux::toOkLch(b);
    return ux::fromOkLch(std::clamp(c.L + ux::T().zebra, 0.0, 1.0), c.C, c.h);
}
```

Two small changes make it work. The phrase counter already exists in the
`tokHue` loop (main.cpp:10392-10403) and is thrown away; it also stores
`tokParity[t] = hue & 1`. And `TokFmt` (10481) gains an `int brush` beside its
`QRgb bg`, so the run merge at 10556 keeps comparing integers rather than
QBrushes and the progressive-apply machinery is untouched.

Provisional selects a texture brush, not a colour: an 8×8 `QPixmap` at
`devicePixelRatio`, filled with the ground, one 1px 45° line in the hatch
colour, 25% coverage. Fifteen tiles per paper, cached, rebuilt on paper change.

Because the tier rides texture, `ΔE00(provisional A, confirmed B)` equals
`ΔE00(confirmed A, confirmed B)` **by construction**. The 2.18 collision cannot
come back without the edge gate failing first.

**Verified, not assumed** (probe run 2026-09-09 against the bundled Qt 6; it
becomes W17 and is banked as `tools/theme_qt_probe.cpp` in step 1):
a texture `QBrush` set as a `QTextCharFormat` background renders its wash *and*
its stripe pixels at devicePixelRatio 1 and 2. The stock `Qt::BDiagPattern`
brush does not: the page shows through its gaps, confirmed by rendering a run
over a magenta page and finding magenta inside the run. Use a tile.

### 4.4 Worked example: ink on wash at each depth

The lightest paper, **Xylograph** (`#FFFFFF`, ink `#000000`). Two parity ladders,
no hue:

| | depth 1 | depth 2 | depth 3 |
|---|---|---|---|
| even phrase | `#D9D9D9` **14.88:1** | `#C1C1C1` **11.67:1** | `#A9A9A9` **8.94:1** |
| odd phrase | `#E9E9E9` **17.30:1** | `#D1D1D1` **13.75:1** | `#B9B9B9` **10.70:1** |
| hatch line (provisional) | `#C1C1C1` 11.67:1 | `#A9A9A9` 8.94:1 | `#929292` 6.75:1 |

Floor 8.94:1 on a ground, 6.75:1 on a stripe. Depth step 8.63 ΔL\*, parity step
5.65 ΔL\*, shallowest ground 7.65 ΔL\* from the paper.

The darkest paper, **Xylograph Night** (`#000000`, ink `#FFFFFF`):

| | depth 1 | depth 2 | depth 3 |
|---|---|---|---|
| even phrase | `#222222` **15.91:1** | `#343434` **12.45:1** | `#484848` **9.15:1** |
| odd phrase | `#161616` **18.10:1** | `#282828` **14.74:1** | `#3A3A3A` **11.37:1** |
| hatch line | `#343434` 12.45:1 | `#484848` 9.15:1 | `#5D5D5D` 6.58:1 |

Floor 9.15:1 on a ground, 6.58:1 on a stripe. Note the sign flip: the washes go
*up* from a black paper, because you cannot darken a tint toward black and keep
anything.

The chromatic pair, for the case that exercises hue. **Manuscript Day**
(`#FAF6EE` / `#2B2118`), ink contrast by slot and depth:

| slot | d1 | d2 | d3 |
|---|---|---|---|
| 0 green `#D4EDDC` → | 12.71 | 10.73 | **8.94** |
| 1 rose `#F6DEEF` → | 12.46 | 10.46 | 8.70 |
| 2 blue `#D3E9FD` → | 12.63 | 10.63 | 8.87 |
| 3 peach `#F7E2CE` → | 12.54 | 10.56 | 8.79 |
| 4 singles `#DEDDE2` → | 11.66 | 9.76 | 8.10 |

**Butter Lamp** (`#1E1B16` / `#EFE7D8`): 12.26 / 10.13 / 8.16 on slot 0, floor
8.03 across all five slots. Both are AAA at every depth, and the measured floor
is what Preferences prints.

---

## 5. The epistemic rule

### 5.1 The measurement that decides this

On the inks that ship today, the two tiers that must never be confused are the
two closest under dichromacy:

| Pair | normal | protan | deutan |
|---|---:|---:|---:|
| HGM `#82672A` vs MACHINE `#B4540A` (paper) | 18.34 | **3.86** | 6.30 |
| chromeGold `#C9A55C` vs chromeMachine `#E8874A` (dark chrome) | 17.55 | 6.20 | **3.13** |

At 3.1–3.9 ΔE00 a protanope or deuteranope cannot tell Geshe Michael's own
English from a machine's by colour. Roughly 8% of men are affected. This is a
live defect today, before any theme exists, and it is not fixable by re-tinting:
a constrained re-solve holding the frozen hues tops out at 7.98 / 7.68, and an
unconstrained solve reaches 9.75 only by turning REFERENCE into `#010716`, which
is black. **Colour cannot separate five epistemic tiers under dichromacy**, and
the design stops pretending it can.

### 5.2 Three channels, and colour is the third

A tier declares itself three ways at once:

1. **The word.** HGM · EVIDENCE · REFERENCE · MACHINE · AI. Always present,
   never abbreviated to a dot, never replaced by an icon, always in the
   accessible name. This is the channel that works for every reader and every
   assistive technology. It is already there; the plan's contribution is to make
   it un-removable.
2. **The shape.** Binding = solid fill. Evidence = solid rule. Machine = dashed
   rule. AI = dotted rule. Reference = a 4px left rule.
3. **The colour.** The frozen hue anchor (Binding 85.0°, Evidence 163.8°,
   Reference 253.8°, Machine 49.4°, AI 26.2°, Warn 66.3°, People 309.9° in
   OKLCH) with only L and C re-derived per paper.

**The shape channel has to be rebuilt, because it does not currently exist.**
Verified by rendering with the same probe: `border:1px solid` on an
inline `<span>` produces **pixel-identical output to no border at all**. So
`sourceBadge` (ux_tokens.h:124-132) emits four "outlined" chips that render as
bare coloured text, and only HGM's `background` renders. Inline `border-left`
does not render either, and neither does a border on a `<div>`. What does render:
a border on a **table cell**, `background` on an inline span, and
`text-decoration`. Also verified: `border-style: double` renders identically to
`solid`, so `double` is not available as a fifth shape; solid, dashed, dotted and
a thick left rule are, and all five treatments above are pairwise distinct as
pixels.

So each badge becomes a one-cell table. That is heavier HTML for a small chip,
and it is the price of the channel actually existing.

### 5.3 Where a tier ink is allowed to stand

The census found the epistemic inks fall to **2.83:1** (kMachine) and 3.04
(kGold) on a depth-3 wash. Latent today because badges live in cards and washes
live in the reading view, and real the moment a badge is drawn inside a shaded
run. The fix is structural rather than chromatic:

- a badge always sits on its own opaque **chip** ground (`Palette::chip`), and
  the gate is `contrast(tier ink, chip) ≥ 4.5` and `contrast(tier ink, paper) ≥ 4.5`;
- a static rule (R7, §7) fails the build if `sourceBadge(` appears in the overlay
  document-format path, so a badge cannot reach a wash;
- **marks** (the dashed provisional underline, the grammar, unattested and
  spelling underlines) do sit on washes and on the band, so they get their own
  derived inks gated at 3:1 (SC 1.4.11, non-text) against every ground they can
  land on. Two sets per paper, one for the page and one for the band, because the
  band is opaque and much darker or lighter than the page.

That last one closes a defect nobody had measured: today's nested-provisional
dashed underline is `kMachine #B4540A` on the deepest X-ray tone `#C48A26`, which
is **1.66:1**. It is invisible.

### 5.4 What the app refuses to let a user do

- Recolour, rename or reshape any provenance mark: the seven tier hues, the
  five words, the five shapes, the `PROVISIONAL (auto-aligned)` label, `⟪ruled⟫`,
  `[generated script]`, `HGM · TENTATIVE`.
- Install a colour table or a third-party scheme.
- Turn off the provisional hatch, or replace it with a tint.
- Turn off the parity zebra, or Xylograph's phrase bars.
- Override a failing gate on a paper they seeded themselves.

The reason, in the words the UI uses: *a colour here is a claim about who wrote
the English. Gold means Geshe Michael's own words; amber means a machine matched
it and you should check. If you could change those, the page would stop meaning
what it says. You choose the paper; the claims stay put.*

### 5.5 The one wording change, and it needs a ruling

Preferences page 9 currently promises (main.cpp:37512):

> "Both keep the honesty colours — provisional tier, generated data, HGM binding
> — identical, so a label never changes meaning with the theme. Installable
> third-party schemes are not offered: nothing here may recolour a provenance
> label."

This plan keeps the second sentence verbatim and needs the first one restated,
because a badge's *value* already changes today: `sourceBadge` swaps `kGold` for
`chromeGold` on dark chrome, ungated. The proposed replacement:

> "A paper may make a provenance colour lighter or darker so it stays legible on
> that paper. It may never change which colour it is, what it says, or what shape
> it wears. Installable third-party schemes are not offered: nothing here may
> recolour a provenance label."

That is a stronger promise than the current one, because it brings existing
behaviour under a rule and a test. It is still a rewrite of a standing promise
about honesty, and it is Adam's to make. If he rules against it, dark papers must
carry the light-paper tier inks, which measure 1.7–2.8:1 on a dark page: the real
choice is "rule on the wording" or "no dark page at all", and that should be said
plainly rather than slid past.

---

## 6. What the user may customise

### 6.1 The controls

| Control | Where | Default |
|---|---|---|
| Day paper | Preferences ▸ Appearance ▸ Papers | Manuscript Day |
| Night paper | same gallery, second column | Manuscript Night |
| Day / Night / Match system | Appearance, top of the page (the existing three-state control, unchanged) | Match system's current value |
| Type scale, 100–200% | Appearance ▸ Reading | 100% |
| Phrase shading: colour + texture / texture and bars only / off | Appearance ▸ Reading | colour + texture |
| Shading intensity | Appearance ▸ Reading | solved L0 |
| Belt and braces (amplified zebra + phrase bars in every paper) | Appearance ▸ Accessibility | off |
| Monochrome provenance (tier hues become greys; word and shape still carry it) | Appearance ▸ Accessibility | off |
| Reduce motion · Stronger focus outline | Appearance ▸ Accessibility | off / off |
| Seed your own paper (step 9, §10) | Appearance ▸ Advanced, collapsed | — |

Shading intensity moves `L0` **within the solved budget** and re-runs the gates
live. Positions that fail are visibly unavailable, with the failing gate named on
hover and in the accessible description. It cannot be dragged into an illegible
page, because there is nowhere illegible to drag it to.

### 6.2 What the page looks like

Preferences page 9 keeps its position, its icon and its keywords, and is renamed
from "Colours" to "Appearance". Four groups down one scrollable column, keyboard
reachable in order.

**1. Appearance.** The existing Day / Night / Match system radio group, moved to
the top and left exactly as it is (main.cpp:37513-37521). One line beneath it:
"Match system follows the Mac's own Light/Dark setting, so night arrives on its
own at sunset. It chooses which of your two papers is live; it never chooses a
colour."

**2. Papers.** A gallery of eight tiles in two columns, headed "Day paper" and
"Night paper". Each tile is a **live specimen rendered by the real code path**,
not a picture: three lines of Tibetan with two nested phrase washes, one hatched
provisional run, one X-ray band, and the five badges in a row, on that paper's
actual page and chrome. If a paper were broken, the tile would show it broken.
Under each tile: the name, the one-line "who this is for", and the measured
floor, read from `Palette::inkFloor` rather than typed. For example:
"Butter Lamp — warm dark page, for long evening sessions. Text on the deepest
shading: 8.0:1 (AAA)."
Selecting a tile applies immediately to the whole app. There is no Apply button
for this control.
Below the gallery, one link: **"Show me"**, which opens the Overlay on a short
prepared passage under the selected paper. A specimen inside Preferences is a
promise; the real pane is the proof, and the distance between them is where a
theme bug hides.

**3. Reading and Accessibility.** The controls in §6.1, each with the same
three-line specimen beside it so the trade is visible before it is taken. The
shading-intensity slider shows "ink on the deepest shading: 8.1:1" live as it
moves.

**4. Provenance colours.** A read-only specimen block, not a control. The five
badges in the selected paper, at their real size, each with its word, its shape,
its hex and its contrast on that paper's chip. Beneath them, the paragraph from
§5.5 plus one sentence of the reason:
"HGM's binding gold and the machine amber are far apart in shape and word as
well as colour, because for a reader with red-green colour blindness they measure
only 3.9 apart in colour alone."

Microcopy discipline: no "optimise", no "enhance". Every claim on this page is a
number the app measured, and every number is one the battery re-measures on
every build.

---

## 7. Living with the constitution

### 7.1 Why G1 stays quiet

G1 (`tools/constitution_check.py:161-185`) fails the build on any `#rrggbb` in
`app/main.cpp` with `g > r && g >= b && g < 0xC8` outside
`{1E6B4E, 2E7D32, 3B7A3B}`. The census established that this blocks a green
phrase wash on any dark or sepia paper while the blue, peach and rose siblings
pass untouched: G1 is asymmetric across the hue wheel.

Measured: the eight papers' **computed** palettes contain **31 colours that trip
G1's predicate** — for instance Butter Lamp's `#062C2C` and `#254948`, Slate's
`#2A2B11`. **None of them is a source literal.** They are the arithmetic
consequence of a paper and an accent, computed at startup from four seeds, and
G1 scans source text. The only literals this work adds are 32 seeds (eight papers
× four), and running G1's own predicate over all 32 gives **zero hits**.

The gate is not evaded; it is respected. G1 means "a new saturated green *ink* is
a new claim of binding authority." None of these 31 is an ink or a claim, and no
reader can learn a meaning from a number that only exists at runtime.

The corollary is a rule, not a habit: **palettes are computed, never tabulated.**
The golden table lives in `tools/theme_golden.tsv`, a data file, precisely so
that nobody is ever tempted to paste a colour table into a header the
constitution cannot read. That is C2's disarmed-gate lesson applied before it
costs anything.

### 7.2 The widening, and the reviewed baseline bump

G1 has three measured blind spots. Closing them is the price of this work, and it
is worth paying on its own.

Widen G1's file list to `app/main.cpp` + `app/ux_tokens.h` + `app/ux_theme.h` +
`app/*.inc`, and its pattern to `QColor(0x..,0x..,0x..)` and `qRgb`. Run today,
that widened gate finds **six greens**, three of which the census did not have:

| Site | Value | What it is | Disposition |
|---|---|---|---|
| `app/idioms.inc:269` | `#2E5D34` | "ruled by" line, an approval | **Re-base to `kAct`.** Same meaning, existing vocabulary |
| `app/idioms.inc:43` | `#2E5D34` (QColor) | same | **Re-base to `kAct`** |
| `app/compare_pane.inc:55` | `#1C3E24` (QColor) | the dark-chrome sibling of `colInsert()`, a diff **ground** | **Allowlist as a ground**, or better, becomes `T().diff[Insert]` in step 6 and stops being a literal |
| `app/main.cpp:5542` | `#1E7A4E` (QColor) | the icon family's green | **Ruling needed.** Either re-base onto `chromeAct()` or name it |
| `app/main.cpp:32108` | `#62A862` (QColor) | the OCR line-box pen | **Ruling needed.** Same |
| `app/ux_tokens.h:93` | `#5FBF8E` | `chromeAct()`, the dark-chrome sibling of `kAct`, already shipping and already asserted | **Declare the derived-sibling clause** |

So the reviewed baseline bump is: the named vocabulary goes from three entries to
**three entries plus one clause**. The clause: *a colour that shares a frozen
ink's OKLCH hue to within 2° and differs only in lightness and chroma is that
ink, not a new one.* `#5FBF8E` is `kAct` on dark chrome and always was; the plan
makes that sayable. Two greens (`#1E7A4E`, `#62A862`) need Adam's ruling or a
re-base before the widening lands, and one (`#1C3E24`) is a ground that the token
sweep removes anyway.

Net after the widening: the "three named greens" stop being six in practice.

### 7.3 New rules this work adds

| Rule | Says | Why |
|---|---|---|
| **G4** | `app/ux_theme.h` may contain at most 4 hex literals per theme entry plus the 7 frozen hue anchors. Anything more fails | Forbids the *shape* of the cheat (a palette smuggled in as a table) rather than enumerating instances |
| **G5** | No `setHtml(` in `app/` outside `ux::setDoc`; no `#RRGGBB`, `QColor(0x..)`, `qRgb` or `rgba()` inside a string that reaches it | Stops the 173-site problem regrowing. Lands in WARN mode with a ratchet, flips to FAIL at zero |
| **G6** | No ground in any paper (wash, hatch, zebra, band, chip, plaque) may sit within **12.0 ΔE00** of any frozen epistemic ink | A wash may be green; it may never be *the* green. Strictly stronger than anything policing grounds today. Measured floors 14.0 (Butter Lamp) to 30.1 (Catalogue), and it already earned its place by rejecting Catalogue's first accent at 1.36 |
| **G7** | The battery must register at least 20 gates across at least 8 papers, and the golden file must exist | A theme system whose battery has quietly emptied is worse than none, because it still reads green. G3's own liveness discipline |
| **R6** | A committed count of colour literals in `app/main.cpp` + `app/*.inc` that may only fall. Baseline **777** | Turns a 777-site sweep from an unreviewable heroic commit into a monotone process |
| **R7** | `sourceBadge(` may not appear in the overlay document-format path | A badge can never reach a wash, so §5.3's guarantee is structural |

G3 is untouched and stays true by construction: the tier signal is texture, which
has no hue of its own, so it cannot be red.

---

## 8. The battery

`ctest -R theme_`, over `ux_tokens.h` + `ux_theme.h`, needing no Qt widget beyond
`QColor`, `QImage` and an offscreen `QGuiApplication`. Every assertion below runs
over all eight papers and fails with the paper name, the measured value and the
threshold. Joins the press ritual.

**W1 — ink floor.** `min contrastRatio(ink, ground) ≥ 4.5` over every wash × depth
× parity × hatch line, with the design target 7.0 reported per paper. `kFsTibetan`
is 22px, below the 24px large-text relaxation, and Tibetan subjoined strokes are
hairlines, so 4.5 applies with no exemption. *Measured floors 6.87–9.15 on
grounds, 5.25–6.80 on stripes.* Fails loudly with: "Slate ink floor 6.87 on
`#274D4F`, needs 4.5, target 7.0."

**W2 — depth step.** ΔE00 between consecutive rungs of one slot ≥ 3.0.
*Measured 3.99–4.38 chromatic, 8.48–8.63 ΔL\* achromatic.*

**W3 — hue identity.** ΔE00 between the four cycling hues at the same depth ≥ 15.0.
*Measured 15.05–17.87.* Explicitly **skipped with a printed reason** on the
achromatic pair, never silently passed. Today's palette scores 9.75 and fails.

**W4 — edge separation.** ΔE00 between any two grounds that can share an edge as
different phrases, including the singles neutral, ≥ 8.0. *Measured 9.51–13.38.*
Achromatic substitution: ΔL\* ≥ 4.5, *measured 5.59–5.65*, and the substitution
and its reason are in the test name so nobody later mistakes it for a weaker
version of the same gate.

**W5 — a shaded phrase looks shaded.** Every depth-1 ground ≥ 8.0 ΔE00 from bare
paper (achromatic: ≥ 7.0 ΔL\*). *Measured 8.15–10.75; achromatic 7.25 and 7.65.*
Today: 6.53, fails.

**W6 — the hatch.** (a) line vs its own ground ≥ 3.0 ΔE00, *measured 5.57–6.08*;
(b) ≥ 1.5 ΔL\* so the tier survives greyscale, photocopying and every kind of
colour vision, *measured 8.36–8.74*; (c) `|hatchStep − Ld| ≥ 0.015 okL`, so a
provisional stripe is never exactly the next depth rung.

**W7 — the tier does not collide.** `ΔE00(provisional A, confirmed B) ≥ 4.5` for
A ≠ B. Under the texture design this **equals W4 by construction**, and the test
asserts that identity, so a future "improvement" that reintroduces a colour lean
is caught. Today: 2.18, fails.

**W8 — provisional looks provisional in its own hue.** ΔE00 ≥ 4.0 and ΔL\* ≥ 1.5
between a provisional run and its settled sibling.

**W9 — ink on the stripe.** `contrastRatio(ink, hatch line) ≥ 4.5`, measured on
the **line** and not on the average, because a glyph stem can land entirely on
one. *Measured 5.25–6.80.*

**W10 — a boundary without colour.** ΔL\* across a phrase boundary with colour
removed ≥ 1.0, and ≤ 0.5 × the depth step so it cannot be read as nesting.
*Measured 2.14–2.17 chromatic, 5.59–5.65 achromatic.* Today: 0.40, a live WCAG
2.2 SC 1.4.1 failure.

**W11 — the tiers stay apart.** Pairwise ΔE00 between the five badge inks on their
chip ≥ 12.0. *Measured 15.52–18.76.*

**W12 — a tier ink is legible where it stands.** `contrast(tier ink, chip) ≥ 4.5`
and `contrast(tier ink, paper) ≥ 4.5`; every mark ink ≥ 3.0 against every wash,
zebra, hatch and band rung it can land on. *Measured 4.51–7.17 for badges,
3.00–3.06 for marks.*

**W13 — deeper is always further from the paper**, including after the zebra step.
Strict, on every paper.

**W14 — dichromacy, with a shape escape.** For protan and deutan, every badge pair
with ΔE00 < 8 **must differ in shape**. Written this way because no honest palette
can meet a pure colour threshold: today's set fails at 3.86 protan and 3.13
deutan and passes only because the shapes differ, which is the point. Cycle
separation under protan and deutan is gated separately at ≥ 3.0, *measured
3.93–13.85*.

**W15 — tritan and greyscale: recorded, not gated**, with the reason printed.
Four hues at constant lightness cannot pass tritanopia in any theme (*measured
0.65–9.34, and 1.16 for today's mint and powder*), so W10's zebra is the gate
that honestly covers the same failure mode. Recording a number you cannot gate,
and saying why, is the difference between a measurement and a lie.

**W16 — the shape channel is live, in pixels.** Render all five `sourceBadge`
outputs to `QImage`, convert to `Grayscale8`, assert pairwise pixel difference
above a floor. This is the only test in the plan that checks a rendered pixel,
and it exists because the current inline-border outline renders as nothing at
all. A refactor that collapses two shapes fails the build even when the colours
still differ.

**W17 — the hatch actually paints.** Offscreen render of a `QPlainTextEdit` run
whose `QTextCharFormat` background is a texture brush; assert the tile's wash
pixels and stripe pixels both appear at devicePixelRatio 1 and 2. If it ever
fails on a platform, the paper falls back to a doubled zebra for provisional runs
and the test asserts the fallback's ΔL\* ≥ 2.0, so the honesty signal degrades to
a weaker channel rather than to none.

**W18 — no hardcoded chrome ground in `app/*.inc`.** Static scan: no `#rrggbb`,
`#rgb`, `QColor(0x..,..)`, `qRgb` or `rgba()` in any `app/*.inc` outside a
declared exemption list. Baseline today **64 occurrences** (42 six-digit, 1
three-digit, 21 QColor triples), ratcheted downward, zero at the end of step 6.
Wired into `constitution_check.py` as G5. Fails as: "compare_pane.inc:55 hardcodes
`QColor(0x1C,0x3E,0x24)`; use `ux::T().diff[Insert]`."

**W19 — ground never impersonates ink (G6).** Every ground of every paper ≥ 12.0
ΔE00 from all nine frozen inks. *Measured floors 14.0–30.1.*

**W20 — artifacts carry no paper.** Save a Manuscript document under Xylograph
Night, reopen under Manuscript Day, assert the rendered ink is Day's and that the
stored HTML carries no colour outside the frozen epistemic set. Same for the
three `QPrinter` paths. Print always uses the print palette: white paper, black
ink, achromatic parity grounds plus phrase bars, hatch, shapes and words intact.
A document is a document; it does not belong to the reader's eyesight.

**W21 — the oracle diff.** C++ `toOkLch` / `fromOkLch` / `maxChroma` /
`deltaE2000` / `contrastRatio` against `tools/theme_tune.py` over the full
eight-paper palette and a 500-seed sweep, to ≤ 1/255 per channel and ≤ 0.01 ΔE00.
Same discipline as the engine ports: the Python is the oracle, the port proves
itself against it, and a port that fails its battery does not ship.

**W22 — the 500-seed sweep.** 500 pseudo-random `(paper, ink, accent)` triples
from a fixed seed. Every one must either pass all gates or **refuse** with the
failing gate, the measured value and the threshold. The **refusal rate is
recorded**, so a solver regression that starts refusing everything is visible
rather than reading as a green suite.

**T-CONTROL — the mutation guard.** A synthetic paper whose grounds are all set
to its own paper must **fail** W1 through W5; one with two tier inks swapped must
**fail** W11 and W14; one with the zebra set to zero must **fail** W10. The
control's failures are **printed**, so a reader can watch the battery
discriminate. Without this the whole suite could be passing vacuously, which is
exactly how the 2026-08-25 audit found the `kAct` pin missing.

**T-PRESERVED.** `manuscript-day.paper == "#FAF6EE"`, `.ink == "#2B2118"`,
light chrome; `manuscript-night` identical except the chrome scheme; both
reference the **same** solved palette object. This pins the migration promise by
object identity rather than by two tables somebody has to keep equal.

**T-PERF.** Format-pass timing on a 40k-token document across all eight papers.
The run merge (main.cpp:10545-10570) compares `bg`, `ul`, `ulc` as integers; the
zebra doubles the distinct backgrounds and the hatch adds an `int brush` key, so
the comparison stays integer-cheap. Budget: no more than 1.3× the current pass
time, measured, with the number printed.

---

## 9. Migration

### 9.1 The settings, as they actually are

`app/nightMode` is no longer the owner. As of 2026-09-09 the live key is
`app/appearance` with three states (main.cpp:5813-5838); `nightMode` is still
written, but only "in step for older readers". Any plan written against the
boolean is written against last month's tree.

```
app/appearance   "day" | "night" | "system"     unchanged, still the owner
app/paperDay     paper id, default "manuscript-day"        NEW
app/paperNight   paper id, default "manuscript-night"      NEW
app/washIntensity        double, default 0 (the solved L0) NEW
app/reduceColour, app/beltAndBraces, app/monoProvenance    NEW, all default off
app/nightMode    still written by setAppearanceMode, untouched by this work
```

`appearanceMode()` keeps its exact semantics. `applyAppearance()` gains one
change: in `"system"` mode it no longer calls `unsetColorScheme()` and walks
away. It watches `QStyleHints::colorSchemeChanged`, and when the OS scheme flips
it selects `paperDay` or `paperNight` and sets the scheme **from the chosen
paper**. The OS chooses which of your two papers is live; it never chooses a
colour. That is the strict replacement for "night chrome, paper page", and it
makes the 2026-08-10 bug (a system-dark Mac rendering dark pages under
cream-tuned washes) structurally impossible instead of avoided by force.

### 9.2 Nobody's screen changes

| Existing state | Lands on | Renders |
|---|---|---|
| `app/appearance = day` | `paperDay = manuscript-day` | identical page, identical chrome |
| `app/appearance = night` | `paperNight = manuscript-night` | identical page, identical chrome |
| `app/appearance = system` | the pair above, chosen by the OS | identical, and now actually obeys the preference |
| `app/appearance` absent, `nightMode` present | the existing one-time boolean migration runs first, then the above | identical |

`manuscript-night` is not a compatibility shim invented to dodge the question. It
is a real paper with a real audience (a bright page in a dark room), and its
being pixel-identical to today's render is what makes it the correct zero-risk
landing spot.

### 9.3 The one honest surprise

The phrase washes change. They have to: today's set fails four gates (identity
9.75 against 15, edge 7.86 against 8, ground-vs-paper 6.53 against 8, tier
collision 2.18 against 4.5). The change is small and it is not a redesign. Slot
by slot, Manuscript Day's new washes sit **1.77, 2.31, 2.02 and 4.18 ΔE00** from
today's mint, blue, peach and rose; the singles ground moves 5.01, from lavender
to a warm grey. The families survive and the cycle order changes to quadrature,
which is what fixes the 11.19 peach-rose collision.

Three other visible changes, all of them repairs:

- Machine-matched phrases become hatched instead of amber-leaning, so they read
  as provisional in greyscale, in print and to a colour-blind reader.
- The X-ray band's provisional highlight becomes visible. Today it is
  `#F2E3BC` against `#F2DFAE`, **1.034:1**, which is to say it does not exist,
  and the promise that provisional looks provisional is not kept there.
- A faint alternating step marks phrase boundaries for a reader who cannot see
  the hues.

A one-time dismissible card on first launch says so in plain words, with a link
to the gate readout. Reproducing a measured defect for the sake of familiarity
would make the whole battery decorative.

---

## 10. Build order

Nine steps. The app is green at every one, and each of the first three is worth
shipping on its own.

| # | Step | Hours |
|---:|---|---:|
| **1** | **The arithmetic and the battery, over today's palette.** OKLab/OKLCH/ΔE2000/CVD added to `ux_tokens.h`; `tools/theme_tune.py` banked as the oracle; W1–W22 and T-CONTROL wired as `ctest -R theme_` with **today's shipped colours as paper zero**. The four failing gates land as expected-fail with their measured numbers. No visual change; the census findings become a standing machine-checked record instead of a memo. | 18 |
| **2** | **The four live defects, fixed on cream, no theme system involved.** The X-ray provisional band (1.034:1), the cross-hue tier collision (2.18), the colour-only phrase boundary (0.40 ΔL\*), and the nested-provisional underline on the deepest band tone (1.66:1). Plus the badge shape channel rebuilt as one-cell tables, because the current outline renders as nothing. Ships alone and is worth shipping alone. | 22 |
| **3** | **`ux_theme.h`, the solver, the golden file, the two default papers.** Manuscript Day and Manuscript Night solved rather than hardcoded; `washColor` and the band read the palette; `tokParity` stored; the hatch tiles cached; W21's oracle diff green. Still two papers, still nobody's screen changed. | 20 |
| **4** | **`ux::paint` / `ux::setDoc` / `ux::skin`, and G5 in WARN mode** with the 777-literal baseline recorded. The Overlay pane migrated first, because that is where every wash property is visible and a mistake shows immediately. | 16 |
| **5** | **The epistemic layer.** `sourceBadge`, the tier labels, the chip grounds, the card blocks in `entryHtml`. Highest risk, smallest diff, most review. R7 lands here. | 14 |
| **6** | **The long tail.** The eight `.inc` panes; `compare_pane.inc`'s accessors become `T().diff[]`; the legacy greys re-based onto audited tokens; S3's parchment dialogs join the theme; S5's rasters get their re-render hook; the light table's declared sub-palette and its failing 2.31:1 BUDA accent. G5 and R6 ratcheted to zero, W18 flips to FAIL. | 26 |
| **7** | **The other six papers**, plus the constitution work: G1 widened, the six blind-spot greens dispositioned, G4/G6/G7 added, the baseline bump reviewed. | 14 |
| **8** | **Preferences, the specimen tiles, "Show me", and the migration card.** Plus the screenshot sweep: `run-diamond-cutter` drives every pane under every paper into a contact sheet. | 18 |
| **9** | **Seed your own paper.** The refusal type, the live gate readout, import/export of three hexes, and the 500-seed sweep green before it ships at all. | 14 |
| | **Total** | **162** |

Steps 1 and 2 together are 40 hours and fix four measured honesty and
accessibility defects with no theme system in sight. If the rest of this plan is
never built, those two should be.

The cream forcing at main.cpp:39213 comes out in **step 3**, not before, and only
once `ctest -R theme_` is green in CI. What replaces "night chrome, paper page"
is not a looser rule but a stricter one: the page may follow the paper only for a
palette that has passed the battery.

---

## 11. Open questions for Adam

1. **The reference website.** You mentioned a site you wanted this to take
   influence from, and its URL did not reach us. Nothing in this plan is drawn
   from it. Send the link and we will read it against §3 and §6 before step 8;
   if it changes the paper set or the Preferences layout, those are the two
   cheapest things to change and step 8 is where they land.

2. **The Preferences promise (§5.5).** Does the sentence about honesty colours
   get restated as written there? This is the one piece of shipped copy the plan
   changes. Ruling needed before step 5. If the answer is no, dark papers cannot
   ship and the plan stops at Manuscript Day, Manuscript Night, Xylograph and
   Catalogue.

3. **Two greens need a ruling or a re-base** before G1 widens (§7.2):
   `#1E7A4E` at main.cpp:5542 (the icon family's green) and `#62A862` at
   main.cpp:32108 (the OCR line-box pen). Either is fine; both need your name on
   them. The derived-sibling clause covers `chromeAct #5FBF8E`.

4. **Four live defects, found by this work and independent of it.** The X-ray
   band's provisional highlight at 1.034:1; the nested-provisional dashed
   underline at 1.66:1 on the deepest band tone; the cross-hue tier collision at
   2.18 ΔE00; and phrase boundaries carried by colour alone at 0.40 ΔL\*
   (WCAG 2.2 SC 1.4.1, Level A). Step 2 fixes all four. Confirm that step 2
   ships whether or not the rest does.

5. **The badge outline does not exist.** Verified by rendering: an inline
   `<span>` border is a no-op in Qt's rich text, so four of the five source
   badges have never had the outline the code and the comments describe. The fill
   versus outline distinction, which house rule 1 leans on, is currently carried
   by the fill alone. Step 2 rebuilds the badges as one-cell tables. Worth
   knowing that this has been true since the badges shipped.

6. **Eight papers or fewer?** Each paper costs one seed row, one golden row and
   one column of the screenshot sweep, so the marginal cost is small; the real
   cost is that every pane has to look right eight ways. If you want a shorter
   list, Catalogue and Slate are the two with the narrowest audience.

7. **Palm Leaf's accent.** It is `#7A5E20`, an ochre in the app's own gold
   family, rather than the rubric red the tradition would suggest. Rubric red
   sits in `kError`'s hue and would put the refusal colour on decorative duty,
   which is the same collision the parchment dialogs already have. Confirm the
   ochre, or rule that a rubric accent is worth the collision.

8. **Wash intensity as a user control.** It is in §6.1 and it moves the page's
   loudness within the solved budget. It cannot break a gate. It can still make
   the app look different in two translators' screenshots of the same passage.
   Keep it, or drop it and ship one solved intensity per paper?

9. **Colour management.** Every ratio in this document assumes sRGB. On a P3
   display Qt may render wider than the arithmetic assumes, so a measured 4.51:1
   could land differently in the eye. Either the app pins its colour space or
   Preferences states the assumption next to the number. A contrast figure that
   is only true on some displays invites exactly the trust it does not deserve.
