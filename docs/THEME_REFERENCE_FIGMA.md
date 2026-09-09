# Colour-combination reference (Figma resource library)

*Source: https://www.figma.com/resource-library/color-combinations/ — read 2026-09-09 at Adam's direction, as the reference for the theme system. This page records what the source actually says, so the theme plan can cite it rather than paraphrase from memory. Where the source gives no figure, this page says so rather than supplying one.*

## The six harmony schemes it names

| Scheme | How it is built | What it is for |
|---|---|---|
| Complementary | Two colours at opposite ends of the wheel | Striking contrast when paired |
| Monochromatic | Lighter and darker versions of one base colour | A consistent, subtle palette |
| Analogous | Hues sitting next to each other on the wheel | A natural scheme |
| Triadic | Three colours equally spaced on the wheel, one primary and the others accents | Vivid, eye-catching |
| Tetradic (square) | Four colours forming a square on the wheel | Vivid contrast, but "tricky to balance" |
| Split complementary | A base colour plus the two either side of its complement | Vibrant yet balanced |

## What it says about applying these to an interface

- Lighter shades for backgrounds and subtle accents; darker hues for primary buttons and for text.
- In a monochromatic scheme, lighter shades keep it fresh and darker shades add depth.
- Blue-grey combinations suit dark mode, to reduce eye strain in low light.
- Colour harmony "establishes visual hierarchy, guiding users' attention to key elements", and must adhere to "colour contrast guidelines, making content readable for all users".
- Hue, value, saturation and temperature are the four properties to work with; adding white to a hue gives a pastel with a calmer feeling.

## What it does NOT give

- **No hex values.** The page presents around a hundred palettes by evocative name only — "Stormy morning", "Mossy hollow", "Blue eclipse", "Fresh peach", "Cherry blossom" — with descriptive colour words rather than codes.
- **No proportion rule.** The 60-30-10 convention is not stated on the page; we should not attribute it to this source.
- **No contrast figures.** It requires adherence to contrast guidelines without naming a ratio, so our own floors come from WCAG 2.2, not from here.
- **No guidance on when to avoid a scheme**, beyond tetradic being hard to balance.

## What we take from it, and what we must add

Take: the harmony vocabulary, so each shipped theme is built on a **named relationship** rather than assembled by eye. That is what makes a set of themes read as a family and is the direct answer to "colour schemes that all work together cohesively". Take also the light-ground / dark-ink guidance, which matches the tool's existing manuscript-paper model.

Add, because the source does not cover it and this application depends on it:

- **Colour here carries meaning, not mood.** The tier inks say whether a rendering is Geshe Michael's, the corpus's, a reference work's, or a machine's. A harmony scheme may re-ground them; it may never let two tiers converge, and it may never let a machine gloss wear the binding colour.
- **The phrase washes are a working surface, not decoration.** Five hues must stay mutually distinguishable and must carry a legible depth ramp for nesting, with the provisional lean still reading as provisional. That is a measurable requirement, and the numbers are ours to set.
- **Every pairing is checked, not judged by eye.** `ux::contrastRatio()` already exists; a theme ships only when the battery has measured its ink on every ground it can appear on.
