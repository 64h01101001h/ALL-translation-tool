# Heading-trim repair: implementation and proof

The code repair is ready for root regeneration and fresh independent review. No builder was run, no source/bank/spec/generated output was edited, and no commit was made by this subtask. Four allowed repository files changed (the root explicitly expanded scope for the disclosure sentence): `tools/build_dictionary_view.py`, `tools/test_view_matches_layer.py`, new `tools/test_dictionary_view_trimming.py`, and one disclosure sentence in `tools/dictionary_view_shell.html`. CMake was not changed.

## Diagnosis and inventory

The original gate failed with `trimmed (trim_glued): 41 > ceiling 40` (exit 1, `before-gate.txt`). Its implementation accepted **any** prefix of a banked value, despite documenting canonical `trim_glued` output. A numeric allowance could thus reject legitimate growth and permit unjustified truncation below the limit.

All prior 40 shortened rows were inventoried with complete banked-prefix sources and actual old canonical output (`baseline-inventory.json`). They comprise 22 heading trims and 18 separate phonetic-line extractions. All 22 nonphonetic values equal the old `trim_glued` output. None of the 18 phonetic values equals that function's output; they come from the pronunciation/punctuation extraction inside the builder.

Ten of the 22 heading trims retained a numbered ACI course heading, because the old trimmer found the later `Reading` heading. C05:206 adds the eleventh instance. The full source is preserved in the bank; the repair changes derived English only.

## Design

A new conservative pattern requires sentence-ending punctuation, whitespace, a numeric page, the exact `The Asian Classics Institute Course` phrase, a Roman numeral and colon, a course title, then one of the numbered `Reading` forms already recognized by the existing rule. The cut preserves the source sentence through its punctuation. The earliest recognized boundary wins, and the existing 12-character minimum remains.

A first broader pattern recognized the full course heading without the Reading witness. The exhaustive comparison revealed five additional formerly exact rows it would change (C03:151; C04:27,62,86,93). This was narrowed before delivery. The rejected broad proof is preserved as `broad-pattern-*.json`; the narrowing RED and GREEN logs are preserved. These five legacy heading variants remain unchanged. Existing unnumbered-heading behavior and its punctuation handling remain unchanged; this is not a general editorial cleanup.

The gate replaces the arbitrary-prefix/count permission with exact per-source validation. Each shortened row needs nonempty references, and **every** reference must resolve to a full-bank link for that Tibetan. A translation trim must have the same depth and equal `trim_glued(source)` exactly. A phonetic extraction must be classified as phonetic, reside in the phonetics band, and equal `trim_phonetic(source)` exactly. Empty, arbitrary shorter, extended, invented, wrongly cited and wrongly banded/depth outputs are rejected at the existing zero ceiling. The global exact-banked-pair path and phonetics/depth-5 pairwise disjointness invariant are unchanged.

The old pronunciation/punctuation extraction was moved without behavior changes into `trim_phonetic`, shared by the builder and validator. This avoids a broad legacy prefix exception. Independent tests with literal expected values guard the shared code against correlated acceptance of a broken implementation. The existing CTest `view_matches_layer` entry runs those tests before the real view gate, so no CMake registration/census changes were needed.

The shell disclosure now accurately says that following section headings or translations appended to phonetic lines are cut back, that full source text remains in the layer, and that nothing is composed, added or reworded. Its schema, counts, markup and layout are unchanged. Nearby builder comments now distinguish both shortening paths.

## Exhaustive migration proof

`migration.json` contains each full source link (including course, segment, id, depth and Tibetan), old/new English, exact zero-based half-open Unicode cut ranges, removed display suffix, match range and reason. Exactly eleven link outputs and eleven display rows change:

- C03:131, C03:199
- C04:37, C04:49, C04:117, C04:133, C04:168
- C05:96, C05:118, C05:145, C05:206

No other link output changes across the entire current full bank. All 947 phonetic link outputs are byte-identical to the original extraction (`phonetic-all-links-proof.json`), including all 18 prior shortened phonetic display rows (`baseline-inventory-after.json`). The prior 12 other heading-trim rows retain their old exact display output.

An in-memory copy of the existing view with only these eleven row replacements passes the stricter classifier: **21,406 exact + 41 verified shortened = 21,447 published rows; zero invented; zero phonetics/depth-5 overlap**. Citations and every other field of the copied payload are retained. This comparison does not write generated artifacts or substitute for the root's actual regeneration.

## Test results

All commands used `PYTHONDONTWRITEBYTECODE=1`.

- Original real view gate: exit 1, expected `41 > 40`; preserved `before-gate.txt`.
- Focused test-first suite: failed on the old implementation; initial RED log retained. Two assertions initially assumed different legacy punctuation/heading-only behavior; corrected to characterize existing behavior before implementation, with a separate corrected RED log retained.
- Final focused suite: **10 tests, zero failures, exit 0** (`green-narrowing.txt`). Tests cover numbered heading removal, mid-sentence capitals/page-number prose, heading-only source, unchanged unrecognized legacy variants, existing Reading trim, exact source provenance, rejected arbitrary/empty/shorter/extended/invented values, every citation and depth, phonetic exactness and band separation, and growth to 41 verified trims with a subsequent invalid row rejected.
- Exhaustive proof script: **exit 0**, empty stderr (`prove.py`, `proof.stdout`, `proof.stderr`, `proof-summary.json`).
- Six mutation checks: all rejected (`mutations.json`): real-payload arbitrary prefix, retained `73` extension, invented English, wrong citation; implementation removal of numbered-header recognition; implementation arbitrary first-word phonetic extraction.
- `git diff --check` restricted to the four changed files: **exit 0** (`diff-check.txt`).
- Existing on-disk view with fixed code but before root regeneration: **exit 1, exactly eleven noncanonical shortened values rejected** (`gate-after-code-before-root-regeneration.txt`). The focused ten tests pass in that invocation. This is expected evidence that old partial trims are no longer accepted.

## Scope and remaining integration

Root must regenerate the dictionary with the fixed original 4aa4 CSV seed, verify export migration only for the eleven enumerated sentence-heading repairs, rerun integration, and obtain fresh independent code review. This subtask makes no claim that generated artifacts already pass after regeneration.

The source-bank SHA-256 fingerprints at proof time are recorded in `proof-summary.json`. No SQLite connection was needed. No native/build/browser/device/paid tooling, delegates, master edits, source edits or generated-file writes were used.

The gate still proves mechanical display fidelity, not semantic correctness or universal editorial recognition of headings. Exact globally banked pairs retain the gate's existing behavior; new source/depth/citation checks apply specifically to shortening. The limited heading recognizer intentionally does not newly process the five other legacy forms discovered by the exhaustive sweep.
