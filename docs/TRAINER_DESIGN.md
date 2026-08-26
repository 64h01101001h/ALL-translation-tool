# Translation Trainer — design (banked 2026-08-06)

Adam's goal, in his words: after learning vocabulary, the main hurdle is
learning how to **break a section down into phrases and clauses**, then **in
which order to read them**, then **in which direction to read the words within
each phrase/clause**. The trainer should take a pasted section of Tibetan text
and, as it is highlighted, reveal the grammar and linguistics associated with
it, using Wilson's system to show *how to read it*.

## Pedagogy: progressive reveal, not answer dumping

The trainer is a *reading tutor*, not a translator. The learner works; the app
reveals structure one layer at a time, on demand:

1. **Chunk boundaries** — the passage split into clauses (at shad and
   clause-connective particles) and, within clauses, into phrases (at case
   particles). No meanings shown yet: just the skeleton.
2. **Particle functions** — each boundary particle labeled with its Wilson
   role: agentive "by", genitive "of", la don "to/at/in", ablative "from",
   semi-final "…and/thus", quotative, etc. (already in `allcore/particles`).
3. **Reading order** — the chunks numbered in suggested English reading
   order per Wilson's method: locate the clause-final verb first, then the
   agentive-marked agent, then objects/complements, qualifiers last. Within a
   phrase, direction arrows: genitive chains read right-to-left
   ("X-gi Y" = "the Y of X"), everything else left-to-right.
4. **Vocabulary** — per-word tier-labeled HGM glosses from the spine (the
   Overlay lattice, already built).
5. **The check** — if the passage (or a clause of it) exists in the corpus,
   reveal HGM's actual English as the answer key; tier-labeled, register
   warnings preserved. If not in the corpus, this layer offers the AI
   analysis pipeline (clearly labeled AI, as in the Analysis pane).

A session flows: paste → try to read → reveal 1 → try again → reveal 2 → …
→ check. Layers are per-selection, so the learner can peel one clause fully
while leaving the rest untouched.

## What the engine must provide (deterministic, allcore)

- **Clause splitter** — segment at shad/barriers plus clause-level particles
  (ste/te/de semi-final, na conditional, kyang concessive, nas/las when
  clause-connective, cing/zhing/shing, quotatives ces/zhes/shes, final 'o).
  Builds on `tokenizeDocument` barriers + `classifyParticle`. NOTE: cing/
  zhing/shing and ces/zhes/shes SHIPPED into the particle tables (core/src/particles.cpp; allomorph licensing in reader.cpp — 2026-08-14 audit-verified). Original ask kept for the record: add them
  (with their suffix-agreement rows: cing after g/d/b, zhing after
  ng/n/m/'/r/l/vowel, shing after s; same pattern for the quotatives).
- **Phrase chunker** — within a clause, cut after each case-particle
  (freestanding or fused, both already detected by the lattice) so every chunk
  carries its role marker; chunks with no marker are candidate
  objects/predicates.
- **Verb spotter** — Wilson's method starts at the clause-final verb. Start
  with: last chunk of the clause = predicate; recognize the copulas/auxiliaries
  (yin/min/yod/med/'gyur/gyur/byed/byas/bya/mdzad/…) and verb-final
  morphology (final 'o, pa/ba nominalizer + case). A curated verb table (from
  Wilson's verb classes, as rule data — never his prose) deepens this later;
  when the spotter is unsure it says so (rule 3: flag, never guess).
- **Reading-order planner** — deterministic ordering over the chunk roles:
  verb → agent (agentive) → object (unmarked) → destination/purpose (la don)
  → source (ablative) → topic (ni) fronted as English demands; genitive
  chunks attach to their right neighbor. Output = numbered chunks + per-chunk
  internal direction. Where the planner's rules don't cover the shape, it
  labels the chunk "unplanned" honestly.
- All of it pure `allcore` (offline); the AI layer stays optional exactly as
  in the Analysis pane.

## What exists already

Lattice segmentation + nested spans (Overlay), fused-particle splits,
freestanding-particle classification + suffix agreement, tier-labeled spine
glosses, 42,199-segment aligned corpus (the answer key), Tibetan-script
rendering via the ported engines, spellcheck. The trainer is mostly a new
*clause/role layer* over existing machinery plus a dedicated pane with the
progressive-reveal interaction.

## Honesty rules (inherited, non-negotiable)

- HGM corpus lines are the only "answers"; anything machine-suggested
  (reading order, verb identification) is labeled as engine guidance, and
  AI output is labeled AI.
- Wilson's framework is implemented as rule tables; his text is never
  shipped or quoted in the app.
- Provisional glosses look provisional; register warnings never auto-resolve.

## Jskad/THDL assessment (2026-08-06)

Reviewed https://thdltools.sourceforge.net/pubapi/org/thdl/tib/input/Jskad.html
plus the org.thdl.tib.input and org.thdl.tib.text package docs. Verdict:
**nothing to extract for this project.** Jskad is a Java Swing editor whose
value is live romanized→Tibetan *input* rendered in the pre-Unicode
TibetanMachineWeb fonts (DuffPane/DuffCode/TibetanMachineWeb classes are all
TMW-glyph plumbing). We are Unicode-native via Qt/HarfBuzz, our converters are
already ported and battery-proven, and Qt handles Tibetan line wrapping.
Two ideas worth remembering, no code needed:
- a live type-romanized-see-Tibetan input pane (our Convert pane already does
  this read-only; a composing editor could serve the input-center formatter,
  roadmap item A);
- their keyboard abstraction (multiple input schemes) — relevant only if input
  centers use non-Wylie keystroke schemes; revisit when formatter samples
  arrive.
