#!/bin/bash
# transcribe_archive.sh — task #65: local transcription of Adam's GMR
# lecture archive. whisper.cpp (Metal), fully offline; audio never
# leaves this machine. Resume-safe: finished files are skipped.
#
# Usage:  bash tools/transcribe_archive.sh <archive_folder> [count]
#         count = limit for a pilot run (e.g. 5); omit for full batch
set -euo pipefail
SRC="${1:?usage: transcribe_archive.sh <folder> [count]}"
LIMIT="${2:-0}"
ROOT="$(cd "$(dirname "$0")/.." && pwd)"
MODEL="$ROOT/build/whisper_models/ggml-large-v3-turbo.bin"
OUT="$ROOT/data/teaching/archive_transcripts"
mkdir -p "$OUT"
N=0
find "$SRC" -type f \( -iname "*.mp3" -o -iname "*.m4a" -o \
     -iname "*.wav" -o -iname "*.mp4" \) | sort | while read -r f; do
  base=$(echo "$f" | shasum | cut -c1-12)-$(basename "${f%.*}" |
         tr -cd 'A-Za-z0-9._-' | cut -c1-60)
  [ -e "$OUT/$base.vtt" ] && continue
  N=$((N+1))
  if [ "$LIMIT" -gt 0 ] && [ "$N" -gt "$LIMIT" ]; then break; fi
  echo "[$N] $f"
  # provenance sidecar: transcript -> source file
  echo "$f" > "$OUT/$base.src"
  whisper-cli -m "$MODEL" -ovtt -of "$OUT/$base" -l en \
      -t 8 "$f" > /dev/null 2>&1 || echo "  FAILED: $f"
done
echo "DONE — transcripts in $OUT (machine-derived; label wherever quoted)"
