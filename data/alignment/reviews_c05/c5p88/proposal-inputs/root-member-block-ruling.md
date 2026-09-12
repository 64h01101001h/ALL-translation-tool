2026-09-12T20:11:19.637783+00:00

# C05:262–264 reconciliation correction 1 — consecutive members

Ruling: ALIGNMENT_LAYER_SPEC.md:78–85 says a d7 span immediately follows its d5 parent. Interpret this as a contiguous member block after that parent, with no intervening flat span; it does not limit each compound to one member. The unchanged canonical tools/gen_alignment_page.py:113–151 explicitly gathers multiple children under the same parent and reorders them within the English parent. Its resolve routine retains that parent across consecutive d7 spans. The original English263 proposal already passed this canonical behavior for both members.

Finding: Reconciled263 omitted original English w32 khrag→blood solely because rnag→pus intervenes between it and the rnag khrag→blood and pus parent. This structural explanation is false. Original exact ranges: parent Tibetan313:323/English470:483; rnag Tibetan313:317/English480:483; khrag Tibetan318:323/English470:475. Both members are tighter, unique constituents in their respective parent ranges. Their positive glossary HTG2016 records remain separate semantic evidence, not an automatic approval.

Fix round1: preserve all prior frozen reconciliation bytes and hashes, then correct this omission and every associated note/disposition/count/report/proof. Restore khrag→blood if confirmed by current source judgment, and run the actual unchanged canonical generator. Keep every original proposal and failed or superseded attempt intact. Do not alter source, master, accepted pages, engine, gate, or governing spec. A different non-author will independently review the complete corrected candidate and this ruling before acceptance.

Cost if wrong: the prose might have intended a stricter one-member rule than implemented; however, the explicit multi-child algorithm and crossing-member commentary are affirmative evidence of intended multiple-member support. This ruling preserves strict parent containment and uniqueness, and does not weaken acceptance checks.
