#!/bin/bash
# test_press_relaunch_trap.sh — the press must never end with the app down.
#
# package_macos.sh quits the translator's running app at step 0 and replaces
# /Applications at step 6c. Between that point and the step 8 relaunch the
# machine has NO app running, and several exits live in that window: the two
# DMG verify outcomes (9 corrupt, 10 busy), the ENOSPC refusal, and `set -e`
# on anything in between. 2026-09-08 is the recorded case — exit 9 on a
# HEALTHY image, after the install, before the relaunch, leaving the user
# with nothing open and a message saying their image was corrupt.
#
# The fix is an EXIT trap armed at 6c. This proves the trap, without running
# a press: the same trap structure is extracted here and driven through the
# three cases that matter. `open` is stubbed, so nothing is launched.
set -uo pipefail
fails=0
check() { if [[ "$1" == ok ]]; then echo "  [PASS] $2"; else echo "  [FAIL] $2"; fails=$((fails+1)); fi; }

TMP="$(mktemp -d)"; trap 'rm -rf "$TMP"' EXIT

# The trap exactly as package_macos.sh defines it, with `open` and `pgrep`
# replaced by stubs so the test observes intent rather than launching an app.
make_harness() {  # $1 = armed, $2 = app already running, $3 = exit code
  cat > "$TMP/h.sh" <<EOF
set -uo pipefail
INSTALL="$TMP"; APPNAME="stub"
open() { echo "OPENED" >> "$TMP/opened"; }
pgrep() { [[ "$2" == running ]] && return 0 || return 1; }
RELAUNCH_ARMED=$1
relaunch_guard() {
  local rc=\$?
  [[ \${RELAUNCH_ARMED:-0} == 1 ]] || exit \$rc
  if ! pgrep -x $2 >/dev/null 2>&1; then
    open "\$INSTALL/\$APPNAME.app" 2>/dev/null || true
  fi
  exit \$rc
}
trap relaunch_guard EXIT
exit $3
EOF
}

run() { : > "$TMP/opened"; bash "$TMP/h.sh"; echo $?; }

# 1. the case the trap exists for: abnormal exit, app not running
make_harness 1 down 9
rc="$(run)"
[[ -s "$TMP/opened" ]] && check ok "an abnormal exit with no app running relaunches it" \
                       || check no "an abnormal exit with no app running relaunches it"
[[ "$rc" == 9 ]] && check ok "and the press's own exit code survives the trap (9)" \
                 || check no "and the press's own exit code survives the trap (got $rc)"

# 2. idempotent with step 8: if the app is already up, do not open a second
make_harness 1 running 0
run >/dev/null
[[ -s "$TMP/opened" ]] && check no "an already-running app is not relaunched again" \
                       || check ok "an already-running app is not relaunched again"

# 3. before 6c the trap must do nothing: /Applications is still the OLD app
#    and the press may legitimately refuse (disk, load, gates) with the
#    translator's existing app still running.
make_harness 0 down 3
rc="$(run)"
[[ -s "$TMP/opened" ]] && check no "a refusal BEFORE the install does not touch the app" \
                       || check ok "a refusal BEFORE the install does not touch the app"
[[ "$rc" == 3 ]] && check ok "and that exit code survives too (3)" \
                 || check no "and that exit code survives too (got $rc)"

# 4. the real script must actually arm it, and after the install
SRC="$(dirname "$0")/package_macos.sh"
grep -q "trap relaunch_guard EXIT" "$SRC" \
  && check ok "package_macos.sh installs the trap" \
  || check no "package_macos.sh installs the trap"
arm=$(grep -n "RELAUNCH_ARMED=1" "$SRC" | head -1 | cut -d: -f1)
inst=$(grep -n 'installed: \$INSTALL' "$SRC" | head -1 | cut -d: -f1)
if [[ -n "$arm" && -n "$inst" && "$arm" -gt "$inst" ]]; then
  check ok "and arms it AFTER /Applications has been replaced, not before"
else
  check no "and arms it AFTER /Applications has been replaced (arm=$arm install=$inst)"
fi

echo "press relaunch trap: $([[ $fails -eq 0 ]] && echo 'ALL PASS' || echo FAIL) ($fails failures)"
exit $fails
