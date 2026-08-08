#!/bin/sh
# Build the Universal Tibetan Font Converter (UTFC — Tashi Tsering /
# Trace Foundation, GPL v3) as an EXTERNAL tool for the legacy-font
# rescue lane (docs/ACIP_DEV_FOLDER_SURVEY.md Tier 1). GPL isolation:
# separate binary, separate process; never linked into the app.
#
#   sh tools/setup_utfc.sh [path-to-UTFC-master]
#
# Default source: ~/ACIP Software Developmeent/UTFC-master (Adam's copy).
# Produces build/utfc/utfc; the app runs it with cwd = the UTFC dir so
# the 39 .tbl conversion tables are found in place.
set -e
SRC="${1:-$HOME/ACIP Software Developmeent/UTFC-master}"
if [ ! -f "$SRC/Converter.c" ]; then
  echo "UTFC source not found at: $SRC" >&2
  exit 1
fi
mkdir -p build/utfc
# macOS has no malloc.h; the shim is the only change
sed 's/#include <malloc.h>/#include <stdlib.h>/' "$SRC/Converter.c" \
  > build/utfc/utfc_macos.c
cc -O2 -o build/utfc/utfc build/utfc/utfc_macos.c
printf '%s\n' "$SRC" > build/utfc/utfc_dir.txt
echo "built build/utfc/utfc (tables at: $SRC)"
echo "usage: utfc <src> <dst> <srcEnc> <dstEnc> <txt|rtf|UnicodeTXT|webpage> <stylesheet|none>"
echo "encodings: Unicode TMW TM Fz Hg ACIP Wylie LTibetan OldSambhota NewSambhota THDLWylie ..."
