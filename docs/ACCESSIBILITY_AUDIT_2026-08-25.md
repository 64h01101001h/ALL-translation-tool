# Accessibility audit — 2026-08-25

*Diamond Cutter Translation Tool · WCAG 2.2 AA as the reference
standard. Written for the P2 gap "Accessibility conformance is
unproven": the project has an accessibility skill and a design wing,
and no recorded audit result. Every number below was computed or
counted, not estimated.*

---

## Summary

| | |
|---|---|
| Colour contrast, reading surface | **11 of 11 tokens pass AA**, and every ratio the source claims is accurate |
| Colour contrast, chrome (both appearances) | **4 of 4 pass AA** |
| Screen-reader names | **0** `setAccessibleName`, **0** `setAccessibleDescription`, against **157** tooltips |
| Explicit focus/tab order | **0** `setFocusPolicy`, **0** `setTabOrder` |
| Tier labels | rendered at **11px**, the smallest type in the interface |

The colour work is genuinely good and was *documented honestly* — that
is the pleasant surprise. The gap is non-visual access, and it is
total rather than partial.

---

## 1. Contrast — verified, not trusted

`app/ux_tokens.h` annotates several colours with a measured ratio
(`AA 4.97`, `AA 4.66`, `AA 4.51`). I recomputed all of them from the
WCAG 2.x relative-luminance formula against the reading surface
`#FAF6EE`:

| token | colour | measured | claimed |
|---|---|---|---|
| kInk | `#2B2118` | 14.61 | — |
| kGold | `#82672A` | 4.97 | — |
| kMuted | `#6E675D` | 5.18 | — |
| kFaint | `#6F6F6F` | **4.66** | 4.66 ✓ |
| kSoft | `#78706A` | **4.51** | 4.51 ✓ |
| kAct | `#1E6B4E` | 5.96 | — |
| kWarn | `#935800` | **5.36** | 5.36 ✓ |
| kMachine | `#B4540A` | 4.62 | — |
| kError | `#8C2F2B` | 7.62 | — |
| kDoc | `#2E629E` | 5.80 | — |
| kPeople | `#6E3E8E` | 7.10 | — |

**Every claim in the file is correct.** Not one is rounded in its own
favour. Chrome inks likewise, on both grounds:

| | on `#2D2D2D` | on `#ECECEC` |
|---|---|---|
| chromeMuted | 5.44 | 6.03 |
| chromeGold | 5.92 | **4.53** |

`chromeGold` on light chrome (4.53) and `kMachine` on paper (4.62) are
the two with least headroom. Neither fails; both are close enough that
a future "slightly warmer gold" would.

**Recommendation:** pin the ratios. The tokens are now in
`app/ux_tokens.h` with a test already asserting the reserved greens; a
contrast assertion belongs beside it, so the arithmetic is checked by
the battery instead of by an auditor once a year.

---

## 2. Screen readers — the real gap

**Zero accessible names. Zero accessible descriptions. 157 tooltips.**

Qt derives an accessible name from a widget's text where it can, so
buttons with visible labels are partly covered. What is not covered:

- **Icon-only controls.** `ⓘ`, `⇄` and similar carry meaning in a
  glyph with no text. A screen reader announces nothing useful.
- **The 157 tooltips.** These carry a lot of this app's explanation —
  and a tooltip is not an accessible description. The information
  exists and is unreachable non-visually.
- **The tier badges.** PROVISIONAL, MACHINE and the reference labels
  are the load-bearing honesty signals of this product. A user who
  cannot see them has no non-visual way to learn that a gloss is
  machine-derived.

That last one is not a WCAG box — it is house rule 1. **A blind
translator using this tool today cannot tell binding English from
machine output.** Every audit this week has found a version of "a lower
tier dressed as a higher one"; this is the same defect for a user who
cannot see the dressing at all.

**Recommendation, in order:** set `setAccessibleName` on every
icon-only control; promote tooltip text to `setAccessibleDescription`
where it explains rather than repeats; and give the tier badges an
accessible description that states the tier in words.

---

## 3. Keyboard

`setShortcut` × 12, `setFocusPolicy` × 0, `setTabOrder` × 0.

Qt's default tab order follows construction order, which is usually
sane and never audited. With no explicit focus policy, custom widgets
that should be focusable may not be, and there is no test that a pane
is operable without a mouse.

This is **unverified rather than known-broken** — I did not drive the
UI by keyboard for this audit, and I am not going to claim a result I
did not measure. WCAG 2.2 §2.1.1 (Keyboard) is therefore *untested*.

**Recommendation:** a harness pass that tabs through each pane and
asserts every actionable control is reachable. The sweep machinery
already clicks every control; reaching them by keyboard is the same
walk with a different input.

---

## 4. Type size

The tier badges render at **11px** (`kFsMeta`), the smallest type in
the interface.

WCAG sets no minimum size, so this is not a conformance failure. It is
a judgement worth recording: the labels that carry *whose words these
are* are the least legible text on the card, and this application is
used for long reading sessions by people who may be older than its
author. Nothing here says they must be larger — only that they were
made smallest, and that the decision should be deliberate.

---

## 5. What this audit did not cover

Named because an audit that only lists what it checked implies it
checked everything:

- **No screen-reader session.** Nothing was tested with VoiceOver. The
  findings above are from source, not from use.
- **No keyboard walk.** §2.1.1 is untested, as stated.
- **Tibetan script rendering at size** — whether stacked glyphs remain
  distinguishable at `kFsTibetanInline` (16px) — needs eyes, not
  arithmetic.
- **Reduced-motion, zoom to 200%, and OS contrast settings** were not
  exercised.
- **Colour is not the only channel** was not verified per surface. The
  tier system uses colour *and* a text label, which is the right shape;
  I did not confirm it holds everywhere.

---

*Next audit should start by re-running section 1, which is arithmetic
and takes seconds, and then do the screen-reader session that this one
did not.*
