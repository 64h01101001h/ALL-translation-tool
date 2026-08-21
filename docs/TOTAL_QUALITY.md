# TOTAL QUALITY ENGINE — LEDGER
Per docs/TOTAL_QUALITY_PROMPT.md. Severity: P0 data-loss/dishonesty/crash ·
P1 blocks-a-journey · P2 friction/debt · P3 polish.

## T1 — WARNINGS → ZERO (closed 2026-08-20)
Starting state: 34 warnings (default flags) + 27 more once -Wall stood.
- **T1-01 · P1 · FIXED.** The 84000 glossary entity-link regex was a
  plain C string: `\(`, `\s`, `\)` collapsed, turning literal parens
  into CAPTURE GROUPS — captured(1) returned "Original glossary entry:
  https…" (prefix text, not the URL) and `[^)s]` truncated URLs at the
  first 's'. The clickable [84000] entity links were broken since the
  glossary refresh. Raw string literal restores the real pattern.
- T1-02 · P2 · FIXED. Deprecated implicit this-capture under `[=]`.
- T1-03 · P2 · RULED+FIXED. 29 fixture-file opens ignored [[nodiscard]]:
  test fixtures get explicit (void) (their pins fail loudly anyway);
  the two APP-code sites (UTFC read/normalize write) join the
  warnWriteFail/warnOpenFail voices. One regex over-match reverted by
  the compiler itself (a checked ternary open).
- T1-04 · P3 · FIXED. Vendored diff_match_patch bitap precedence made
  explicit (upstream semantics preserved, parens added).
- T1-05 · P2 · FIXED. Dead layouts (two orphaned QHBoxLayout news —
  leftovers of band conversions), unused loop counter, ctor-init-order
  mismatch, 9 unused lambda captures. ux:: tokens spoken only in HTML
  literals carry [[maybe_unused]] + the M2 ruling comment.
- **The wall**: -Wall is now a standing compile option in the root
  CMakeLists; the count is zero and stays zero. (-Wextra: evaluated
  next time the build is otherwise idle — not adopted blind.)
Proof: clean rebuild 0 warnings · 59/59 suites green.
