C05:250–252 scoped evidence-only fix round1 — Codex (gpt-6-astra).
Addressed Q251-APOSTROPHE and Q252-APOSTROPHE at exactly all six reviewer-recorded current prose locations.
251 report lines 5/6 and first suspicion ground b now preserve the initial apostrophe in "'jug".
252 report line 4 and third suspicion grounds b/d now preserve the initial apostrophe in "'du shes".
The six matching literals in 250/finalize_reconciliation.py were corrected; AST verification proves only those six literal values changed.
Every pre-fix reconciliation file, all freeze manifests, every helper and all prior failed/superseded files were copied byte-for-byte before edits; exact mapping is in pre-fix-manifest.json.
Specs, canonical notes, bodies, original inputs, all candidate proofs and saved generator/replay bytes remain identical. No generator was rerun.
Semantic-review files remain unchanged. No source/master/repository file was edited.
The scoped textual diff is scoped.diff; per-file original/new SHA-256 and bytes are in changes.json; unchanged-candidate evidence is identity-proof.json.
Separate scoped semantic re-review follows; this fix claims no human acceptance or hgm_gloss promotion.
251/report.md: 4302877604b8673fdeb29b2c5821d5d927e81eb654ac429ddc96788baa02b242 (2771 bytes) -> 4eb7c2b8607bf3ee0699f0e16b8b5679aac62a3c5cc2d5b8761a7fa7df2e9848 (2773 bytes).
251/errata-five-ground-audit.json: 27b860020bb3c6472ede44037b79a07c4702cb2af774c61a431b29dd0c03f448 (7630 bytes) -> 9a0b704eae145bf6fd54be7d13385d99debb43d8c28ab2dd2bf1e9e07781a911 (7631 bytes).
252/report.md: 962ff9b03ec835e3306a640ff7a246b0b47da7dcfe0b60f400532875d6524b7d (2839 bytes) -> 446915de2eda514bad9ec06be5a7a4613864b0cdb466edaa0c80f59fb24c855a (2840 bytes).
252/errata-five-ground-audit.json: 45da9ef72bd9c977bb7c6782cdaf8617ecb8e6ffaf391eccb63a70ba13c89055 (14133 bytes) -> cdd91a10a0693d9584a62e34bed509bfcc53efdbb96c88fcf13e2429907a1cc6 (14135 bytes).
250/finalize_reconciliation.py: f97f0ad90af2f5c8ff3ff6323da2f1da67bfaf590dc97422a0ace597af980956 (21153 bytes) -> 793094b1f3bff274f7fac272d89cafd08d86614bbe31bd0034badbe473b7cfb4 (21162 bytes).
