# Author Reference Metadata Implementation Plan

> **For agentic workers:** REQUIRED SUB-SKILL: Use superpowers:subagent-driven-development (recommended) or superpowers:executing-plans to implement this plan task-by-task. Steps use checkbox (`- [ ]`) syntax for tracking.

**Goal:** Fill out the existing PERSON card with teachers, students,
offices, native-script name and (where available) a portrait, harvested
from Wikidata under CC0.

**Architecture:** A build-time Python harvester queries the Wikidata
SPARQL endpoint, joins to the roster on BDRC person id, and writes a
sidecar JSON. The Qt app loads that sidecar beside the roster it
already loads, and the existing `g_personCardByName` lambda renders
additional sections from it. No new pane, no runtime network.

**Tech Stack:** Python 3 (stdlib only — `urllib`, `json`), C++20/Qt 6
Widgets, CMake/CTest.

**Spec:** `docs/superpowers/specs/2026-08-23-author-metadata-design.md`

## Global Constraints

- Every harvested fact is **REFERENCE tier**. Never binding, never
  promoted, never rendered in the reserved greens `#1E6B4E` or
  `#2E7D32`.
- The source is **named on the card**. A reader must distinguish an ALL
  fact from a Wikidata fact without clicking.
- **Conflicts are surfaced, never resolved** (house rule 3).
- **Absence is stated, never padded.** No empty section headings.
- **No runtime network** on this path. The deterministic core stays
  offline; input centres cannot rely on connectivity.
- **Constitution R3:** no unguarded dialogs. Anything added to the card
  path must survive `--selftest` and `--sweep` headlessly.
- Every shipped file needs a `docs/distribution/PAYLOAD_MANIFEST.txt`
  row or the press fails. Format: `mode | status | path | licence |
  anchor | provenance`, separated by ` | `, no field may contain `|`.
- Wikidata labels render **as-is with the source labelled**. Do not run
  them through the project's converters.
- Every pin is **mutation-verified**: plant the bug, watch it go red,
  restore, watch it go green. A pin that cannot fail does not count.
- Gate after every task: `bash tools/verify.sh` — currently 79 suites.
  Capture the exit code **directly**, never through a pipe.

---

### Task 1: The harvest tool

**Files:**
- Create: `tools/build_author_meta.py`
- Modify: `core/CMakeLists.txt` (register the selftest)

**Interfaces:**
- Consumes: `data/extracted/author_index.json` (`{"_meta":…,
  "people": {"<bdrc_pid>": {"display","aliases","works","tol",…}}}`)
- Produces: `data/extracted/author_meta.json`, shape given in Step 3.
  Task 2 reads it. Function `build(roster: dict, rows: list) -> dict`
  is pure and is what the selftest drives — no network in tests.

- [ ] **Step 1: Write the failing selftest**

Append to `tools/build_author_meta.py` (created in this step, tool body
in Step 3):

```python
def cmd_selftest(root):
    """Drive build() with canned SPARQL rows. No network."""
    bad = []
    roster = {"people": {
        "P65": {"display": "RGYAL TSAB DAR MA RIN CHEN",
                "dates": "1364-1432", "tol": "9095"},
        "P64": {"display": "RJE TZONG KHA PA", "dates": "1357-1419"},
        "P999": {"display": "NOBODY", "dates": ""}}}
    rows = [
        {"bdrc": "P65", "qid": "Q1", "nativeName": "རྒ\u0fb�",
         "born": "1364", "died": "1432",
         "teachers": [{"qid": "Q2", "label": "Tsongkhapa"}],
         "students": [{"qid": "Q3", "label": "Khedrubje"}],
         "positions": ["Ganden Tripa"]},
        {"bdrc": "P64", "qid": "Q2", "born": "1357", "died": "1500",
         "teachers": [], "students": [], "positions": []},
    ]
    out = build(roster, rows)
    p = out["people"]

    if "P999" in p:
        bad.append("a person Wikidata does not know must not appear")
    if p.get("P65", {}).get("teachers", [{}])[0].get("label") \
            != "Tsongkhapa":
        bad.append("teacher label not carried through")
    # the join back to the roster is the whole point
    if p.get("P65", {}).get("teachers", [{}])[0].get("roster_pid") \
            != "P64":
        bad.append("teacher matching a roster member must carry "
                   "roster_pid so the card can link it")
    if p.get("P65", {}).get("dates", {}).get("conflicts_roster"):
        bad.append("1364-1432 agrees with the roster; must not be "
                   "flagged as a conflict")
    if not p.get("P64", {}).get("dates", {}).get("conflicts_roster"):
        bad.append("1357-1500 disagrees with the roster's 1357-1419 "
                   "and MUST be flagged, not silently preferred")
    if p.get("P64", {}).get("teachers") != []:
        bad.append("empty relations stay empty; never padded")

    for b in bad:
        print("  [FAIL] " + b)
    print("author_meta selftest: %d failure(s)" % len(bad))
    return 1 if bad else 0
```

- [ ] **Step 2: Run it and verify it fails**

Run: `python3 tools/build_author_meta.py --selftest .`
Expected: FAIL — `NameError: name 'build' is not defined`. That is the
correct failure: the function does not exist yet.

- [ ] **Step 3: Write the minimal tool**

Prepend to `tools/build_author_meta.py`:

```python
#!/usr/bin/env python3
"""Harvest author reference metadata from Wikidata (CC0).

Why Wikidata and not Treasury of Lives: ToL pages and prose are
copyrighted (Rubin Foundation). Wikidata is CC0 and models the same
relations. We link to ToL; we do not copy it. See SQA BUILD-5 for what
happens when payload terms are asserted rather than established.

Join on BDRC person id (P2477) - the key the roster already uses -
with Treasury of Lives id (P4138) as a fallback.

Usage:  python3 tools/build_author_meta.py [--selftest] <repo_root>
Writes: data/extracted/author_meta.json
"""
import json
import os
import sys
import time
import urllib.parse
import urllib.request

UA = "DiamondCutterTranslationTool/1.0 (ALL; author metadata harvest)"
ENDPOINT = "https://query.wikidata.org/sparql"

QUERY = """
SELECT ?bdrc ?item ?nativeName ?born ?died
       (GROUP_CONCAT(DISTINCT CONCAT(STR(?teacher), "=",
                    ?teacherLabel); separator="||") AS ?teachers)
       (GROUP_CONCAT(DISTINCT CONCAT(STR(?student), "=",
                    ?studentLabel); separator="||") AS ?students)
       (GROUP_CONCAT(DISTINCT ?posLabel; separator="||") AS ?positions)
       (SAMPLE(?img) AS ?image)
WHERE {
  VALUES ?bdrc { %s }
  ?item wdt:P2477 ?bdrc .
  OPTIONAL { ?item wdt:P1559 ?nativeName }
  OPTIONAL { ?item wdt:P569 ?born }
  OPTIONAL { ?item wdt:P570 ?died }
  OPTIONAL { ?item wdt:P1066 ?teacher .
             ?teacher rdfs:label ?teacherLabel .
             FILTER(LANG(?teacherLabel) = "en") }
  OPTIONAL { ?item wdt:P802 ?student .
             ?student rdfs:label ?studentLabel .
             FILTER(LANG(?studentLabel) = "en") }
  OPTIONAL { ?item wdt:P39 ?pos .
             ?pos rdfs:label ?posLabel .
             FILTER(LANG(?posLabel) = "en") }
  OPTIONAL { ?item wdt:P18 ?img }
}
GROUP BY ?bdrc ?item ?nativeName ?born ?died
"""


def _year(iso):
    """1364-01-01T00:00:00Z -> '1364'. '' stays ''."""
    return iso[:4] if iso and len(iso) >= 4 else ""


def build(roster, rows):
    """Pure: roster dict + SPARQL rows -> the sidecar document.

    Kept free of network and filesystem so the selftest can drive it.
    """
    people = roster["people"]
    # label -> roster pid, so a relation naming somebody we already
    # have becomes a click-through to their works
    by_display = {}
    for pid, v in people.items():
        for name in [v.get("display", "")] + list(v.get("aliases", [])):
            if name:
                by_display[name.upper().replace(" ", "")] = pid

    def link(label):
        return by_display.get(label.upper().replace(" ", ""))

    out = {}
    for r in rows:
        pid = r["bdrc"]
        if pid not in people:
            continue
        rec = {"qid": r.get("qid", "")}
        if r.get("nativeName"):
            rec["native_name"] = r["nativeName"]

        born, died = _year(r.get("born", "")), _year(r.get("died", ""))
        roster_dates = (people[pid].get("dates") or "").strip()
        conflict = False
        if roster_dates and (born or died):
            conflict = roster_dates != "%s-%s" % (born, died)
        rec["dates"] = {"born": born, "died": died,
                        "roster": roster_dates,
                        "conflicts_roster": conflict}

        for key in ("teachers", "students"):
            rec[key] = [{"qid": t.get("qid", ""),
                         "label": t.get("label", ""),
                         "roster_pid": link(t.get("label", ""))}
                        for t in r.get(key, []) if t.get("label")]
        rec["positions"] = list(r.get("positions", []))
        if r.get("image"):
            rec["image_url"] = r["image"]
        out[pid] = rec
    return {"_meta": {}, "people": out}
```

- [ ] **Step 4: Run the selftest and verify it passes**

Run: `python3 tools/build_author_meta.py --selftest .`
Expected: `author_meta selftest: 0 failure(s)`, exit 0.

- [ ] **Step 5: Mutation-verify every assertion**

For each, edit, re-run, confirm exactly the matching `[FAIL]` appears,
then restore and confirm green:

| Mutation | Must turn red |
|---|---|
| `conflict = False` always | the 1357-1500 conflict assertion |
| drop `"roster_pid": link(...)` | the roster_pid assertion |
| `if pid not in people: pass` | the P999 assertion |

Restore each with `cp` **and `touch`** — `verify.sh`'s header records
that a copied file's older mtime makes `make` skip the rebuild.

- [ ] **Step 6: Add the network fetch and stage-then-swap**

Append to `tools/build_author_meta.py`:

```python
def fetch(pids, batch=120):
    """Query Wikidata in batches. Raises on any failed batch."""
    rows = []
    for i in range(0, len(pids), batch):
        vals = " ".join('"%s"' % p for p in pids[i:i + batch])
        q = QUERY % vals
        url = ENDPOINT + "?format=json&query=" + urllib.parse.quote(q)
        req = urllib.request.Request(url, headers={"User-Agent": UA})
        with urllib.request.urlopen(req, timeout=90) as fh:
            d = json.loads(fh.read().decode("utf-8"))
        for b in d["results"]["bindings"]:
            def g(k):
                return b.get(k, {}).get("value", "")

            def pairs(k):
                raw = g(k)
                out = []
                for chunk in raw.split("||") if raw else []:
                    if "=" in chunk:
                        uri, lab = chunk.split("=", 1)
                        out.append({"qid": uri.rsplit("/", 1)[-1],
                                    "label": lab})
                return out

            rows.append({
                "bdrc": g("bdrc"),
                "qid": g("item").rsplit("/", 1)[-1],
                "nativeName": g("nativeName"),
                "born": g("born"), "died": g("died"),
                "teachers": pairs("teachers"),
                "students": pairs("students"),
                "positions": [p for p in g("positions").split("||") if p],
                "image": g("image")})
        time.sleep(1)   # be a good citizen of a free endpoint
    return rows


def main(root):
    ex = os.path.join(root, "data", "extracted")
    roster = json.load(open(os.path.join(ex, "author_index.json")))
    pids = sorted(roster["people"].keys())
    print("querying Wikidata for %d people..." % len(pids))
    rows = fetch(pids)          # raises rather than returning thin
    doc = build(roster, rows)
    got = doc["people"]
    doc["_meta"] = {
        "source": "Wikidata (CC0)",
        "join": "BDRC person id via P2477",
        "retrieved": time.strftime("%Y-%m-%d"),
        "endpoint": ENDPOINT,
        "query": QUERY,
        "roster_people": len(pids),
        "resolved": len(got),
        "with_teachers": sum(1 for v in got.values() if v["teachers"]),
        "with_students": sum(1 for v in got.values() if v["students"]),
        "tier": "REFERENCE - never binding, never promoted",
    }
    if len(got) < len(pids) // 2:
        raise SystemExit(
            "refusing to write: only %d of %d people resolved. A thin "
            "file is indistinguishable from 'these people have no "
            "teachers'." % (len(got), len(pids)))
    out = os.path.join(ex, "author_meta.json")
    tmp = out + ".tmp"
    with open(tmp, "w") as f:
        json.dump(doc, f, indent=1, ensure_ascii=False)
    os.replace(tmp, out)        # stage-then-swap: never half a file
    print("wrote %s (%d people)" % (out, len(got)))


if __name__ == "__main__":
    args = [a for a in sys.argv[1:]]
    if "--selftest" in args:
        sys.exit(cmd_selftest(args[-1]))
    sys.exit(main(args[-1] if args else "."))
```

- [ ] **Step 7: Register the selftest in ctest**

In `core/CMakeLists.txt`, after the `payload_manifest` test block:

```cmake
if(Python3_EXECUTABLE)
  add_test(NAME author_meta
           COMMAND ${Python3_EXECUTABLE}
                   ${CMAKE_SOURCE_DIR}/tools/build_author_meta.py
                   --selftest ${CMAKE_SOURCE_DIR})
endif()
```

- [ ] **Step 8: Run the gate**

Run: `bash tools/verify.sh > /tmp/g.txt 2>&1; echo "EXIT=$?"`
Expected: `EXIT=0`, `100% tests passed out of 80`.

- [ ] **Step 9: Commit**

```bash
git add tools/build_author_meta.py core/CMakeLists.txt
git commit -m "Author metadata: the Wikidata harvest, and its drill"
```

---

### Task 2: Run the harvest and manifest its output

**Files:**
- Create: `data/extracted/author_meta.json` (generated)
- Modify: `docs/distribution/PAYLOAD_MANIFEST.txt`
- Modify: `docs/distribution/OPEN_SOURCE_NOTICES.md`

**Interfaces:**
- Consumes: `tools/build_author_meta.py` from Task 1.
- Produces: the sidecar on disk. Task 3 reads it from the app.

- [ ] **Step 1: Run the harvest**

Run: `python3 tools/build_author_meta.py .`
Expected: `wrote …/author_meta.json (N people)` with N ≈ 400+.
If it raises instead, that is the tool working — read the message.

- [ ] **Step 2: Sanity-check the output against the spec's measurements**

```bash
python3 -c "
import json;d=json.load(open('data/extracted/author_meta.json'))
m=d['_meta'];p=d['people']
print('resolved',m['resolved'],'of',m['roster_people'])
print('teachers',m['with_teachers'],'students',m['with_students'])
print('P65 teachers',[t['label'] for t in p['P65']['teachers']])
"
```
Expected: resolved ≈ 400+ of 462; `P65` teachers include `Tsongkhapa`.

- [ ] **Step 3: Add the manifest row**

Append to `docs/distribution/PAYLOAD_MANIFEST.txt`, in the
`data/extracted/` itemised block:

```
both | ok | Diamond Cutter Tool Data/data/extracted/author_meta.json | CC0-1.0 | Author reference metadata (Wikidata) | harvested by tools/build_author_meta.py; Wikidata is CC0; REFERENCE tier, never binding
```

- [ ] **Step 4: Add the notices row**

Append to the reference-data-layers table in
`docs/distribution/OPEN_SOURCE_NOTICES.md`:

```
| Author reference metadata (Wikidata) | CC0 1.0 — public domain dedication; harvest query and retrieval date recorded in the file's `_meta` | teachers, students, offices and native-script names on the PERSON card, labelled as Wikidata and never merged into ALL's own data |
```

- [ ] **Step 5: Verify the press gate accepts it**

Run: `python3 tools/manifest_check.py --selftest .` then
`bash tools/verify.sh > /tmp/g.txt 2>&1; echo "EXIT=$?"`
Expected: `EXIT=0`. An unmanifested file fails the press by design —
if it fails here, the row above is wrong, not the gate.

- [ ] **Step 6: Commit**

```bash
git add data/extracted/author_meta.json \
        docs/distribution/PAYLOAD_MANIFEST.txt \
        docs/distribution/OPEN_SOURCE_NOTICES.md
git commit -m "Author metadata: harvested sidecar, manifested and credited"
```

---

### Task 3: Render the sections on the PERSON card

**Files:**
- Modify: `app/main.cpp` — the `g_personCardByName` lambda at ~`:22115`,
  inserting before its closing `works in your Library` line
- Modify: `app/main.cpp` — the Library pane selftest that already
  drives `g_personCardByName` at ~`:36364`

**Interfaces:**
- Consumes: `data/extracted/author_meta.json` from Task 2.
- Produces: card HTML containing `AUTHORITIES (Wikidata)` sections.

- [ ] **Step 1: Write the failing pins**

At `app/main.cpp:36364`, where `g_personCardByName` is already driven,
add:

```cpp
// Spec pins 1, 2 and 4 (2026-08-23). REFERENCE tier: the source is
// named on the card, absent sections are absent rather than empty,
// and a relation is a link ONLY when it names somebody in the roster.
const QString gj = g_personCardByName
    ? g_personCardByName("RGYAL TSAB DAR MA RIN CHEN") : QString();
check(gj.contains("Tsongkhapa"),
      "PERSON card lists a harvested teacher");
check(gj.contains("Wikidata"),
      "PERSON card names Wikidata as the source (REFERENCE tier)");
check(!gj.contains("#1E6B4E") && !gj.contains("#2E7D32"),
      "PERSON card never uses the reserved binding greens for "
      "reference material");
check(gj.contains("author:P64"),
      "a teacher who IS in the roster renders as a click-through");
const QString unknown = g_personCardByName
    ? g_personCardByName("KLONG RDOL BLA MA NGAG DBANG BLO BZANG")
    : QString();
check(!unknown.contains("TEACHERS"),
      "a person with no harvested relations renders no empty "
      "TEACHERS heading (absence is stated, not padded)");
```

- [ ] **Step 2: Run and verify they fail**

Run: `cmake --build cmake-build-release -j8 && (cd cmake-build-release && ctest -R app_selftest --output-on-failure)`
Expected: FAIL on "lists a harvested teacher" and "names Wikidata" —
the sections do not exist yet. The reserved-greens and empty-heading
pins may pass trivially at this point; that is expected and is why
Step 5 mutation-tests them.

- [ ] **Step 3: Load the sidecar beside the roster**

In the same class as `loadPersons()`, add:

```cpp
    QJsonObject authorMeta_;          // bdrc pid -> harvested record
    bool authorMetaLoaded_ = false;

    void loadAuthorMeta() {
        if (authorMetaLoaded_) return;
        authorMetaLoaded_ = true;
        QFile f(dataRoot_ + "/extracted/author_meta.json");
        if (!f.open(QIODevice::ReadOnly)) return;   // absent is fine
        const auto d = QJsonDocument::fromJson(f.readAll());
        authorMeta_ = d.object().value("people").toObject();
    }
```

- [ ] **Step 4: Render the sections**

In `g_personCardByName`, immediately before the existing
`h += "<br><small style='color:#777'>works in your Library:` line:

```cpp
                // Harvested reference metadata (spec 2026-08-23).
                // REFERENCE tier: the source is named, nothing is
                // padded, and the reserved greens are not used here.
                loadAuthorMeta();
                const QJsonObject am =
                    authorMeta_.value(pidForCard).toObject();
                if (!am.isEmpty()) {
                    auto section =
                        [&](const char* title, const QString& key) {
                        const auto arr = am.value(key).toArray();
                        if (arr.isEmpty()) return;   // never an empty heading
                        h += QString("<div style='margin-top:6px'>"
                                     "<small style='color:#777'>%1"
                                     "</small><br>").arg(title);
                        QStringList bits;
                        for (const auto& v : arr) {
                            const auto o = v.toObject();
                            const QString lab =
                                o.value("label").toString().toHtmlEscaped();
                            const QString rp =
                                o.value("roster_pid").toString();
                            bits << (rp.isEmpty()
                                     ? lab
                                     : "<a href='author:" + rp + "'>" +
                                           lab + "</a>");
                        }
                        h += bits.join(" · ") + "</div>";
                    };
                    section("TEACHERS", "teachers");
                    section("STUDENTS", "students");
                    const auto pos = am.value("positions").toArray();
                    if (!pos.isEmpty()) {
                        QStringList ps;
                        for (const auto& v : pos)
                            ps << v.toString().toHtmlEscaped();
                        h += "<div style='margin-top:6px'><small "
                             "style='color:#777'>OFFICES HELD</small>"
                             "<br>" + ps.join(" · ") + "</div>";
                    }
                    h += "<div style='margin-top:6px'><small "
                         "style='color:#777'>the three sections above "
                         "are reference from Wikidata (CC0) — not "
                         "the Library's own record</small></div>";
                }
```

`pidForCard` is the `pid` already read from the candidates loop; hoist
it to a local declared before the loop so it is in scope here.

- [ ] **Step 5: Run, verify green, then mutation-verify each pin**

Run the gate. Then, one at a time:

| Mutation | Must turn red |
|---|---|
| delete the "reference from Wikidata" line | "names Wikidata as the source" |
| `if (arr.isEmpty()) {}` instead of `return` | "renders no empty TEACHERS heading" |
| always emit plain `lab`, never the `<a>` | "renders as a click-through" |
| set a section colour to `#1E6B4E` | "never uses the reserved binding greens" |

Restore each with `cp` **and `touch`** before rebuilding.

- [ ] **Step 6: Commit**

```bash
git add app/main.cpp
git commit -m "Author metadata: teachers, students and offices on the PERSON card"
```

---

### Task 4: Surface date conflicts

**Files:**
- Modify: `app/main.cpp` — same lambda as Task 3
- Modify: `app/main.cpp` — same selftest block as Task 3

**Interfaces:**
- Consumes: `dates.conflicts_roster` from the Task 1 sidecar.

- [ ] **Step 1: Write the failing pin**

```cpp
// Spec pin 3. House rule 3: a disagreement between the Library's
// dates and Wikidata's is SHOWN, never silently resolved.
const QString conf = renderPersonDates(
    /*rosterDates=*/"1357-1419", /*wdBorn=*/"1357",
    /*wdDied=*/"1500", /*conflict=*/true);
check(conf.contains("1357-1419") && conf.contains("1500"),
      "a date conflict shows BOTH values, not a preferred one");
check(conf.contains("disagree"),
      "a date conflict says so in words a reader can act on");
```

- [ ] **Step 2: Run and verify it fails**

Expected: build error — `renderPersonDates` is not declared. That is
the correct failure.

- [ ] **Step 3: Write the minimal helper**

Above the lambda, as a free function so the selftest can drive it
without a card:

```cpp
// Pure so the drill can reach it. Reference dates that disagree with
// the Library's own are shown side by side and named as a
// disagreement - never merged, never silently preferred.
static QString renderPersonDates(const QString& rosterDates,
                                 const QString& wdBorn,
                                 const QString& wdDied, bool conflict) {
    if (!conflict) return rosterDates.toHtmlEscaped();
    return rosterDates.toHtmlEscaped() +
           " <small style='color:#a33'>(Wikidata says " +
           wdBorn.toHtmlEscaped() + "–" + wdDied.toHtmlEscaped() +
           " — these disagree)</small>";
}
```

Call it where the card currently prints `dates`.

- [ ] **Step 4: Run and verify green**

- [ ] **Step 5: Mutation-verify**

| Mutation | Must turn red |
|---|---|
| `if (conflict) return rosterDates;` | "shows BOTH values" |
| drop the word `disagree` | "says so in words" |

- [ ] **Step 6: Commit**

```bash
git add app/main.cpp
git commit -m "Author metadata: date disagreements are shown, not resolved"
```

---

### Task 5: Portraits

Deferred by design — **do not start until Tasks 1–4 are committed and
green.** Coverage is ~28%, so this is a minority feature and the card
is useful without it.

**Files:**
- Modify: `tools/build_author_meta.py` (download + downscale)
- Create: `data/extracted/portraits/<pid>.jpg`
- Modify: `docs/distribution/PAYLOAD_MANIFEST.txt` (per-image rows)
- Modify: `app/main.cpp` (render)

**Interfaces:**
- Consumes: `image_url` already emitted by Task 1's `build()`.

- [ ] **Step 1: Write the failing pin — licence before pixels**

```cpp
check(!portraitLicence("P65").isEmpty(),
      "no portrait renders without a recorded licence (BUILD-5)");
```

- [ ] **Step 2: Run and verify it fails**

- [ ] **Step 3: Fetch each image's Commons licence, then the image**

Query `https://commons.wikimedia.org/w/api.php?action=query&prop=
imageinfo&iiprop=extmetadata` per file; store
`LicenseShortName`, `Artist`, `UsageTerms` and the source URL beside
the file. **Skip any image whose licence cannot be read** — a portrait
without established terms is exactly BUILD-5 and must not ship.

- [ ] **Step 4: Downscale to ~400px longest edge**

Full size would add ~29 MB to a DMG from which 406 MB was just
removed. Keep the Commons URL in the sidecar for the original.

- [ ] **Step 5: One manifest row per image, then gate, then commit**

---

## Self-Review

**Spec coverage:** §2 source decision → Tasks 1–2. §3 join → Task 1
Step 3. §4 provenance/tier → Global Constraints + Task 3 Step 4 +
Task 4. §5 data shape → Task 1 Step 3. §6 harvest tool → Task 1. §7
rendering → Task 3. §8 pins 1,2,4 → Task 3 Step 1; pin 3 → Task 4;
pin 5 → Task 1 Step 6 (`raise SystemExit` on a thin result, asserted by
the `refusing to write` guard). §9 out of scope → nothing added. §10
open question → resolved in Global Constraints (as-is, labelled).

**Placeholder scan:** none — every code step carries real code.

**Type consistency:** `build(roster, rows)` in Task 1 Steps 1/3/6;
`roster_pid` used identically in Task 1 Step 3 and Task 3 Step 4;
`conflicts_roster` in Task 1 Step 3 and Task 4 Step 1;
`renderPersonDates` signature matches between Task 4 Steps 1 and 3.

**Known soft spot:** Task 3 Step 4 assumes `pidForCard` and
`dataRoot_` are reachable in that scope. Both exist in the surrounding
class, but the implementer should confirm the exact member name for
the data root before writing — `loadPersons()` in the same class shows
the established way to reach it.
