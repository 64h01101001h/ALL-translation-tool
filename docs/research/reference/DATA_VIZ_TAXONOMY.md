# Choose the right chart — the taxonomy, and where it lands here

*Supplied by Adam 2026-08-22. Source: Taras Bakusevych, "20 ideas for
better data visualization",
https://uxdesign.cc/20-ideas-for-better-data-visualization-73f7e3c2782d
Backlog #42.*

## The taxonomy

Four intents, each split snapshot vs over-time:

| Intent | Snapshot | Over time |
|---|---|---|
| **Relationship** | scatter · bubble · network diagram | — |
| **Comparison** | column · bar · circular area | line · column · column-overlap |
| **Composition** | pie/donut · tree map · heat map · sunburst | stacked column · stacked area · waterfall |
| **Distribution** | scatter · histogram · bell curve | — |

## What actually applies to this tool

Being honest about fit matters more than adopting the whole vocabulary.
A canon has no time axis, so every "over time" column is empty here, and
a pie chart of anything in this data would be decoration.

**Distribution — the strongest fit, and already measured.**
- The meter census: 333,802 verse lines, 7-syllable 61.1%, 5-syl 15.4%,
  9-syl 8.5%, 6-syl mgur 6.2%. Currently a sentence in a heading. It is
  a histogram, and a reader deciding whether a deviant line is a keying
  slip or poetic licence needs the shape, not the percentage.
- Glosses per entry: median 1, mean 2.3, tail to 169. This explains to a
  reader WHY a card is crowded, which is more respectful than silently
  reflowing it.
- Syllable counts within one text — where the verse is, at a glance.

**Composition — the most useful thing a translator does not currently
have.** Before starting a text: how much of it is HGM-glossed, how much
is reference-only, how much is unattested. One bar per text would answer
"how hard is this going to be" before the first line. The library's
subject coverage (33.7%) is the same shape at collection scale.

**Comparison — the register evidence #31 needs.** One term's renderings
across the 75 courses: bsod nams gives merit 323, goodness 50, good
karma 40, virtue 36. That distribution IS the register evidence, and it
is currently invisible. Showing it as comparison lets a translator judge
without the tool composing a rule it has no right to compose.

**Relationship — real but expensive.** citation_web.json, the 1,768
person-text links, term co-occurrence. A network diagram is the honest
shape for a citation web; it is also the hardest thing on this page to
draw well in Qt, and the easiest to make pretty and useless.

## Constraints any of this must respect

- Qt rich text has NO charting. Anything drawn is QPainter into a
  QPixmap, or composed from table cells, background widths and unicode
  block characters. Say which, per chart.
- Every chart carries its N and its source, exactly like every other
  surface. A bar with no denominator is the visual form of a cap
  presented as completeness.
- Tier colour is already load-bearing: green is binding authority,
  #b00 provisional, #B4540A machine, #2E629E reference. A chart may not
  spend those hues on unrelated categories.
- Absence must stay visible. A zero bar and a missing bar are different
  facts and must not look alike.
