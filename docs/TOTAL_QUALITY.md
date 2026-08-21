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

## T6 — DEAD WEIGHT (swept 2026-08-20)
- TODO/FIXME in shipping code: exactly TWO, both references to
  externally-gated items already on the master TODO (GMR's systematic
  honorific ruling; the Uploads naming convention). RULED: they stay —
  they mark honest waiting, not neglect.
- Settings keys: zero written-never-read; 70 read-with-default-only
  (normal). CLEAN.
- Dead allocations: two orphaned QHBoxLayouts found via T1 and removed.

## T5 — THE MONOLITH, MAPPED (2026-08-20; regenerate with the
one-liner in git log for this entry)
app/main.cpp = 32,896 lines. Anchor table (class · start · ~end · size):
ux 255-347 · OcrModelInfo 377-549 · G84000 561-653 · TeachingMoment
654-820 · ApparatusNote 830-1496 · ReleasePkg 1497-2074 · LookupPopup
2075-2267 · sess 2268-2706 (split by the W2 helpers) · AnalysisPane
2707-3180 · RibbonProxy/Group/Bar 3181-3416 · ScanCanvasLabel 3417-3477 ·
**OverlayPane 3478-12384 (8,906)** · ApparatusPane 12385-12653 ·
ScansPane 12654-12739 · ExportPane 12740-12860 · GoferPane 12861-13633 ·
SanskritPane 13634-14517 · TrainerPane 14518-14902 · DrillsPane
14903-15545 · DraftPane 15546-17546 (2,000) · FilesPane 17560-19255
(1,695) · LibraryPane 19256-20824 (1,568) · ReviewPane 20825-21125 ·
AlignPane 21126-21716 · InputPane 21717-23222 (1,505) · ScanPane
23223-24033 · ProposePane 24034-24285 · ApprovalPane 24286-24777 ·
ManuscriptPane 24778-25235 · CatalogMember/Tree 25236-25479 ·
CatalogPane 25480-27756 (2,276) · SettingsDialog 27757-27880 ·
HelpWindow 27881-28211 · HuntPalette+main+selftest 28212-32896 (4,684).
RULING (per T5): no extraction this wave — nothing here is being
touched for another reason. The map exists so every future block edit
anchors against known boundaries.
