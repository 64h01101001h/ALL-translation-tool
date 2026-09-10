# Three gaps in `acip_to_ewts`, measured 2026-09-09

Found while generating the iOS drill pack. **Not yet fixed** — this is a
canonical engine, and Rule 2 says a change to it ships only with its validation
battery behind it, not at the end of a long evening.

## What fails

The C++ port and the Python oracle agree exactly, so the port is faithful and
the gap is in the engine itself. Measured over a 4,000-drill pack:

| Class | Example | ACIP → EWTS gives | EWTS should be | Occurrences |
|---|---|---|---|---|
| The `G-Y` disambiguator | `G-YON`, `G-YAS`, `G-YO` | `g-yon` | `g.yon` | ~250 |
| Wazur `V` | `KVA`, `DVAGS`, `ZHVA`, `RVA` | `kva`, `dvags` | `kwa`, `dwags` | ~100 |
| Visarga `:` | `A'A:`, `HO:`, `DZA:` | `a'a:`, `ho:` | `aH`, `hoH` | ~100 |

`wylieToUnicode` then correctly refuses each one and flags it `⟨g-yon⟩`, so
nothing is fabricated — but the reader sees a flag where Tibetan belongs.

## Why it matters beyond the phone

This is not a drill-pack problem. The same call runs in the **Overlay**, so the
desktop has been showing these flags too:

```
DE LTAR G-YON PA'I, ,LAG PA KVA DVAGS
  →  དེ་ལྟར་⟨g-yon⟩་པའི། །ལག་པ་⟨kva⟩་⟨dvags⟩
```

## Corroborating evidence that the `V` gap is already known

`iastToTibetan` patches it at the call site rather than in the engine:

```cpp
const std::string w = acipToEwts(a);
… .replace('v', 'w')   // "Sanskrit va renders with wazur"
```

A call site patching an engine's output is the usual sign the engine is wrong.

## What a fix has to clear before it ships

1. The `ewts_unicode` battery: 98.88% variant-aware exact match over the
   master's 26,318 (wylie, tibetan) ground-truth pairs. The number must not
   fall.
2. The full `ctest` suite, including the port-parity diff over 79,452 Lokesh
   Chandra terms — Python and C++ must still agree exactly.
3. `GA-YAS` and friends are **not** covered by this note. `G-Y` is documented;
   `GA-` is not understood yet and must not be guessed at.

## Measured effect of NOT fixing it

In the shipped drill pack: 1,020 of 4,000 drills (26%) carry at least one
flagged syllable. Every flag is honest — the engine truly cannot read the form
— but a quarter of drills showing a flag is a poor reading experience, and
fixing this is the single largest remaining win for both the phone and the
Overlay.
