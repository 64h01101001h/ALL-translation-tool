#!/bin/sh
# Build the py-tiblegenc legacy-font→Unicode converter (buda-base,
# Apache-2.0) into a machine-local venv. 28+ pre-Unicode encodings
# (TibetanMachine/-Web, Youtsoweb, Drutsa, Khamdris…). The app
# offers it as an OPTIONAL external engine in the Library pane's
# legacy rescue when this venv exists — no Python at app runtime,
# only a QProcess call into this venv (same pattern as UTFC).
set -e
HERE="$(cd "$(dirname "$0")/.." && pwd)"
VENV="$HERE/build/tiblegenc_venv"
python3 -m venv "$VENV"
"$VENV/bin/pip" install --quiet --upgrade pip
"$VENV/bin/pip" install --quiet "git+https://github.com/buda-base/py-tiblegenc"
"$VENV/bin/python" -c "import pytiblegenc; print('py-tiblegenc ready:', pytiblegenc.__file__)"
