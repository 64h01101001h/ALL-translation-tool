#!/usr/bin/env bash
# test_shortwrite.sh — the first test in this repository that can catch
# a write which failed AFTER the file opened successfully.
#
# Why it needs a RAM disk. The 2026-08-22 SQA assessment measured
# ProposalStore::save() returning TRUE having written 16,384 of 123,576
# bytes, and found that no test could see it. The obvious test — save
# into a chmod'd directory — does NOT work: an unwritable directory
# fails at open(), and the `if (!f) return false;` guard at the top of
# save() has always handled that correctly. Such a test passes whether
# or not the bug is present. (Written, mutation-tested, mutant survived,
# discarded. That is the same defect the assessment found in three
# shipped pins.)
#
# A short write needs a volume that runs out of room mid-stream. macOS
# gives us one without sudo: hdiutil attach ram://... plus
# diskutil eraseVolume. 2 MB, filled to leave a small hole, then a store
# far larger than the hole is saved into it.
#
# SKIPS loudly (exit 0) when the RAM disk cannot be created — a machine
# without hdiutil should not fail the suite, but it must not silently
# report a pass either, or we are back to a test that cannot fail.
set -uo pipefail

PROBE="${1:?usage: test_shortwrite.sh <path-to-shortwrite_probe>}"
VOL="SQASHORTWRITE"
MNT="/Volumes/$VOL"
DEV=""

cleanup() {
    [[ -d "$MNT" ]] && diskutil unmount force "$MNT" >/dev/null 2>&1
    [[ -n "$DEV" ]] && hdiutil detach "$DEV" -force >/dev/null 2>&1
    return 0
}
trap cleanup EXIT

skip() { echo "  [SKIP] $1"; echo "shortwrite: SKIPPED"; exit 0; }

command -v hdiutil  >/dev/null 2>&1 || skip "no hdiutil (not macOS)"
command -v diskutil >/dev/null 2>&1 || skip "no diskutil"
[[ -x "$PROBE" ]] || skip "probe not built at $PROBE"

# 4096 sectors x 512 B = 2 MB
# Review finding: a leftover mount from a killed run makes macOS mount
# the NEW volume as "SQASHORTWRITE 1" while $MNT still resolves to the
# stale one — the test would then measure the wrong disk and unmount it.
[[ -d "$MNT" ]] && skip "$MNT already exists (leftover from a previous run); unmount it first"
DEV=$(hdiutil attach -nomount "ram://4096" 2>/dev/null | tr -d '[:space:]')
[[ -n "$DEV" ]] || skip "could not attach a RAM device"
diskutil eraseVolume HFS+ "$VOL" "$DEV" >/dev/null 2>&1 || skip "could not format the RAM disk"
[[ -d "$MNT" ]] || skip "RAM disk did not mount at $MNT"

fails=0
check() { if [[ "$1" == "ok" ]]; then echo "  [PASS] $2"; else echo "  [FAIL] $2"; fails=$((fails+1)); fi; }

# --- control: a healthy volume must report success honestly ---
out=$("$PROBE" "$MNT" 40)
echo "  healthy volume -> $out"
[[ "$out" == CLAIMED=TRUE* ]] && check ok "save() reports TRUE when the bytes really landed" \
                              || check no "save() reports TRUE when the bytes really landed"

# The seeded queue STAYS. It is the thing the next step must not damage:
# a real team's proposals.tsv already on disk when a save runs out of room.
seed_sum=$(shasum -a 256 "$MNT/proposals.tsv" | awk '{print $1}')
seed_bytes=$(wc -c < "$MNT/proposals.tsv" | tr -d ' ')
echo "  seeded queue: ${seed_bytes} bytes, sha ${seed_sum:0:12}"

# --- the real case: fill the volume, leave a hole far too small ---
dd if=/dev/zero of="$MNT/filler" bs=1024 count=1750 >/dev/null 2>&1
free_k=$(df -k "$MNT" | tail -1 | awk '{print $4}')
echo "  volume filled; ${free_k}K free, writing a store far larger than that"

out=$("$PROBE" "$MNT" 400)
echo "  full volume    -> $out"

claimed=$(echo "$out" | sed -E 's/.*CLAIMED=([A-Z]+).*/\1/')
bytes=$(echo   "$out" | sed -E 's/.*BYTES=(-?[0-9]+).*/\1/')

# Review finding 2026-08-23: asserting only CLAIMED=FALSE let an OPEN
# failure satisfy this test identically to the short write it exists to
# catch. The evidence demanded then was "bytes actually landed in
# proposals.tsv" — which only held because save() truncated the real queue
# and wrote into it. It no longer does: it writes a temporary beside the
# queue and renames, so a cut-short write leaves proposals.tsv untouched
# and BYTES is now -1 on the very run that used to prove the point.
#
# So the evidence moves, it is not dropped. A hole was deliberately left on
# the volume, so if free space remains the open CANNOT have failed for want
# of room — which makes this a genuine short write, exactly as before. What
# is asserted on top is the property that actually protects the team: the
# queue that was already on disk is byte-for-byte what it was.
if [[ "$claimed" != "FALSE" ]]; then
    check no "save() CLAIMED SUCCESS after a short write — only $bytes bytes landed. This is SQA FAIL-2 back again."
elif [[ "$free_k" -le 0 ]]; then
    check no "the volume was filled completely (${free_k}K free) — this run tested the open guard, not a short write. Retune the dd filler so a hole remains."
else
    check ok "save() reports FALSE when the write was cut short (${free_k}K was free, so the open succeeded)"
fi

now_sum=$(shasum -a 256 "$MNT/proposals.tsv" 2>/dev/null | awk '{print $1}')
if [[ "$now_sum" == "$seed_sum" ]]; then
    check ok "and the queue already on disk is byte-for-byte unchanged — a save that runs out of room does not eat the team's proposals"
else
    check no "the team's queue was DAMAGED by a failed save (was ${seed_sum:0:12}, now ${now_sum:-absent}) — a partial TSV reloads as a silently SHORTER queue"
fi

leftover=$(ls "$MNT"/proposals.tsv.writing 2>/dev/null | wc -l | tr -d ' ')
[[ "$leftover" == "0" ]] && check ok "no half-written temporary is left behind"                          || check no "a proposals.tsv.writing temporary was left on the volume"

echo "shortwrite: $([[ $fails -eq 0 ]] && echo 'ALL PASS' || echo 'FAIL') ($fails failures)"
exit $fails
