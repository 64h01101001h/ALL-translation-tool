# Preserve every rendering — Adam's ruling, 2026-08-28

> "We don't want to destroy any of the renderings. We want to preserve all
> renderings of the English equivalents. The Tibetan should have and will
> have multiple equivalents, multiple renderings for the same Tibetan term
> or phrase or clause. We want to preserve all of the glosses, for posterity
> but also to give the translator a more robust way and more context for
> different ways to treat any individual Tibetan phrase, term, clause or
> word."

This is now the governing principle for the scan layer, and it settles the
attestation-versus-definition question raised in
`TWO_DICTIONARIES_INTEGRATION.md`: the layer is an attestation record and
multiplicity is the point, not noise to be resolved.

## What the data already does — verified

| | |
|---|---|
| headwords | 4,095 |
| with a single rendering | 2,857 |
| **with multiple renderings** | **1,238 (30.2%)** |
| **total distinct renderings held** | **7,558** |
| headwords with >4 renderings | 286 |
| deepest | `chos` — **36 renderings** |

`chos` alone spans "spiritual practice" (12×), "practice" (11×), "dharma"
(6×), "religion" (3×), "the teaching" (3×), "Dharma" (2×), "objects" (2×)
and more. Every one is Geshe Michael's own English, each carrying the
segments that attest it. Nothing in `build_alignment_layer.py` dedupes or
collapses: renderings accumulate per headword and are written out in full.

**The data obeys the ruling already.** The problem is downstream.

## Where renderings are currently lost to the reader

**1. The entry card truncates at four.** `app/main.cpp:1378` —
`if (shown >= 4) break;` — then prints "…and N more attested pairings".
That affects **286 headwords** and puts **1,047 renderings** out of reach.
For `chos` the translator sees four and is told there are 32 more, with no
way to see them. The line is dead text, not a link.

**2. The Review pane does it right, but narrowly.** The register-spread
block (`app/main.cpp:26288`) iterates every rendering for a term **with no
cap**, showing each with its count and flagging which the draft used. That
is exactly the behaviour the ruling asks for. But it only fires for terms
that appear in the draft being reviewed, only for terms with ≥2 renderings,
and it stops after 6 terms per report.

So a translator working in the Review pane sees full spreads for a handful
of terms; a translator reading an entry card sees at most four.

## What this needs

The card cap is defensible as a *summary* — a card swamped by 36 lines helps
nobody. What is not defensible is that the summary is a dead end. The
recommendation is that "…and N more attested pairings" becomes a way in to
the full list, so nothing is unreachable, rather than raising the cap and
hoping 36 lines fit.

**This is GUI work, so it goes through the project's own governance**
(CLAUDE.md: GUI-touching work opens with brainstorming + design-critique and
ships through accessibility-audit + verification-before-completion). It is
not a cap I should quietly raise. Flagged for Adam to open.

## Standing consequence for the scan

Multiplicity is now explicitly a feature. When one Tibetan term is rendered
several ways across the corpus, every rendering is recorded with its
citations, and no batch may prefer one and drop the others. Recent examples
worth keeping in view: `chog shes pa` stated then unpacked inside a single
segment (C03:123); `rnam rtog` → "the stupid thoughts" (C03:126) beside its
ordinary sense; `ri rab` → "Meru" and "The Great Mountain"; `tshogs` →
"conditions" and "collections"; `lhag mthong` → "special sight" and
"(vipashyana, or insight)".
