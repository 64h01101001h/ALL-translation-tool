#!/usr/bin/env python3
"""Harvest the Kawachen Tibetan Reader's audio into data/audio/kawachen/.

The reader at itibet.org is a state machine, not a folder of files: it serves
recordings indexed by SOUND, and its filenames mean nothing until you have
driven the site to learn what each index says.  That decoding is recorded in
docs/KAWACHEN_READER_FORMAT.md and lives as data in data/kawachen_labels.json;
this script applies it, so every file lands next to a manifest row saying which
Tibetan syllable it actually pronounces.

Files are stored under their descriptive names — `49_2_3--klud-glud-…mp3` —
but are always FETCHED and RESUMED by the site's own key, so a rename never
causes a re-download.  See tools/kawachen_labels.py for the naming rule.

LICENSING.  Adam holds Kawachen's permission to use these recordings IN HOUSE.
That is not permission to redistribute.  The audio is gitignored and must stay
that way, and it must not be bundled into anything that leaves this machine —
including the TestFlight build — without going back to Kawachen first.

Usage:
    python3 tools/kawachen_harvest.py            # harvest, resuming
    python3 tools/kawachen_harvest.py --verify   # report on disk; fetch nothing
"""
import argparse
import csv
import hashlib
import http.client
import io
import json
import os
import sys
import time

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
from kawachen_labels import (VOWEL, SUFFIX, TERMS, spellings_for,   # noqa: E402
                             descriptive_name)

HOST = "www.itibet.org"
PATH = "/tibetan_reader/sound_file"
UA = ("Mozilla/5.0 (Macintosh; Intel Mac OS X 10_15_7) "
      "AppleWebKit/605.1.15 (KHTML, like Gecko) Version/17.0 Safari/605.1.15")
DELAY = 0.20
ROOT_DIR = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
OUT = os.path.join(ROOT_DIR, "data", "audio", "kawachen")

# An MP3 frame or an ID3 tag.  The site answers a miss with a 302 to an HTML
# page rather than a 404, so the magic bytes decide, never the status code.
MPEG = (b"\xff\xfb", b"\xff\xfa", b"\xff\xf3", b"\xff\xf2", b"\xff\xe3")

_conn = None


def get(sub, site_key):
    """Fetch one recording over a kept-alive connection, or None if absent."""
    global _conn
    data = b""
    for attempt in (1, 2):
        if _conn is None:
            _conn = http.client.HTTPSConnection(HOST, timeout=25)
        try:
            _conn.request("GET", "%s/%s/%s.mp3" % (PATH, sub, site_key),
                          headers={"User-Agent": UA, "Accept": "*/*",
                                   "Referer": "https://%s%s" % (HOST, PATH)})
            data = _conn.getresponse().read()
            break
        except (http.client.HTTPException, OSError):
            try:
                _conn.close()
            except OSError:
                pass
            _conn = None
            if attempt == 2:
                return None
            time.sleep(1.0)
    time.sleep(DELAY)
    if data[:3] == b"ID3" or data[:2] in MPEG:
        return data
    return None


def on_disk(sub, site_key, wylie):
    """Bytes already held for a recording, under either naming generation.

    Resolution is always by the site's key, so renaming the store descriptively
    never makes the harvester think a file is missing and re-fetch it.
    """
    for name in (descriptive_name(site_key, wylie), site_key + ".mp3"):
        p = os.path.join(OUT, sub, name)
        if os.path.exists(p) and os.path.getsize(p):
            with open(p, "rb") as f:
                return f.read()
    return None


def save(sub, site_key, wylie, data):
    d = os.path.join(OUT, sub)
    os.makedirs(d, exist_ok=True)
    with open(os.path.join(d, descriptive_name(site_key, wylie)), "wb") as f:
        f.write(data)


def write_manifest(rows, force=False):
    """Write the label record atomically, and never silently shrink it.

    The manifest is the only place that says what each recording says. An
    earlier version of this function truncated it on every run, including
    under --verify, which meant one interrupted migration could have destroyed
    the record with no committed copy to restore from.
    """
    path = os.path.join(OUT, "manifest.csv")
    if os.path.exists(path) and not force:
        have = sum(1 for _ in csv.DictReader(io.open(path, encoding="utf-8")))
        if len(rows) < have:
            print("REFUSING to shrink the manifest: %d rows on disk, %d to "
                  "write.\nIf that is genuinely intended, re-run with --force."
                  % (have, len(rows)), file=sys.stderr)
            return False
    cols = ['file', 'dir', 'kind', 'consonant_index', 'vowel_index',
            'suffix_index', 'tibetan', 'wylie', 'identified', 'bytes', 'sha256']
    os.makedirs(OUT, exist_ok=True)
    tmp = path + ".tmp"
    with io.open(tmp, "w", encoding="utf-8", newline="") as f:
        w = csv.DictWriter(f, fieldnames=cols)
        w.writeheader()
        w.writerows(rows)
    os.replace(tmp, path)
    jtmp = os.path.join(OUT, "manifest.json.tmp")
    io.open(jtmp, "w", encoding="utf-8").write(
        json.dumps(rows, ensure_ascii=False, indent=1))
    os.replace(jtmp, os.path.join(OUT, "manifest.json"))
    return True


def syllable_row(site_key, data):
    c, v, s = (int(x) for x in site_key.split('_'))
    sp, stem = spellings_for(c)
    wylie = '/'.join(st + VOWEL[v][1] + SUFFIX[s][1]
                     for st in stem.split('/')) if (stem or c == 30) else ''
    return {
        'file': descriptive_name(site_key, wylie), 'dir': 'syllable_mp3',
        'kind': 'syllable', 'consonant_index': c, 'vowel_index': v,
        'suffix_index': s,
        'tibetan': ' / '.join(x + VOWEL[v][0] + SUFFIX[s][0] for x in sp),
        'wylie': wylie, 'identified': bool(sp), 'bytes': len(data),
        'sha256': hashlib.sha256(data).hexdigest(),
    }


def harvest(verify_only=False, force=False):
    rows = []
    keys = ['%d_%d_%d' % (c, v, s)
            for c in range(1, 55) for v in range(5) for s in range(11)]
    print('syllable_mp3: %d candidate recordings' % len(keys), flush=True)
    for i, k in enumerate(keys):
        c, v, s = (int(x) for x in k.split('_'))
        _, stem = spellings_for(c)
        wylie = '/'.join(st + VOWEL[v][1] + SUFFIX[s][1]
                         for st in stem.split('/')) if (stem or c == 30) else ''
        data = on_disk('syllable_mp3', k, wylie)
        if data is None and not verify_only:
            data = get('syllable_mp3', k)
            if data:
                save('syllable_mp3', k, wylie, data)
        if data:
            rows.append(syllable_row(k, data))
        if i and i % 250 == 0:
            print('  %d/%d  kept %d' % (i, len(keys), len(rows)), flush=True)

    print('word_mp3', flush=True)
    wkeys = (['ta_%d' % i for i in range(1, 41)] +
             ['vowel_%d' % i for i in range(1, 9)] +
             ['add_%d' % i for i in range(1, 9)] +
             ['prefix_%d' % i for i in range(1, 9)] +
             ['zk_%d' % i for i in range(1, 9)])
    for k in wkeys:
        kind, tib, wy = TERMS.get(k + '.mp3', ('unidentified term', '', ''))
        data = on_disk('word_mp3', k, wy)
        if data is None and not verify_only:
            data = get('word_mp3', k)
            if data:
                save('word_mp3', k, wy, data)
        if data:
            rows.append({
                'file': descriptive_name(k, wy), 'dir': 'word_mp3',
                'kind': kind, 'consonant_index': '', 'vowel_index': '',
                'suffix_index': '', 'tibetan': tib, 'wylie': wy,
                'identified': bool(tib), 'bytes': len(data),
                'sha256': hashlib.sha256(data).hexdigest(),
            })

    ok = write_manifest(rows, force=force)
    uniq = len({r['sha256'] for r in rows})
    print()
    print('  files            %d' % len(rows))
    print('  distinct audio   %d' % uniq)
    print('  total size       %.1f MB' % (sum(r['bytes'] for r in rows) / 1e6))
    print('  unlabelled       %d' % sum(1 for r in rows if not r['identified']))
    print('  manifest         %s' % ('written' if ok else 'NOT written'))
    return 0 if ok else 1


def main():
    ap = argparse.ArgumentParser(description=__doc__,
                                 formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument('--verify', action='store_true',
                    help='report on what is already on disk; fetch nothing')
    ap.add_argument('--force', action='store_true',
                    help='allow the manifest to shrink')
    a = ap.parse_args()
    return harvest(verify_only=a.verify, force=a.force)


if __name__ == '__main__':
    sys.exit(main())
