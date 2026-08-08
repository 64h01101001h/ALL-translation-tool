#!/usr/bin/env python3
"""Extract the banked Mahāvyutpatti TEI (DILA/DDBC, CC BY-SA 3.0 — see
data/mahavyutpatti/README.md) into the app's reference table.

    python3 tools/extract_mahavyutpatti.py

Reads  data/mahavyutpatti/mahavyutpatti.dila.tei.p5.xml.zip
Writes data/extracted/mahavyutpatti.tsv with one row per entry:
    key <TAB> chapter <TAB> iast <TAB> devanagari <TAB> wylie|... <TAB>
    tibetan|... <TAB> chinese|...
Multiple renderings are |-joined in source order. DDBC's own editorial
additions (<add resp="ddbc">) carry a {ddbc} marker — kept distinct from
the historical glossary, never silently merged. Fields the source lacks
are left empty (never invented). Rows are written for every entry, even
Sanskrit-only ones.
"""
import io
import sys
import zipfile
import xml.etree.ElementTree as ET

NS = {"tei": "http://www.tei-c.org/ns/1.0"}
REPO = __file__.rsplit("/", 2)[0]


def text_of(el) -> str:
    """Flatten an element's text, marking <add resp="ddbc"> content."""
    parts = []
    def walk(e):
        tag = e.tag.split("}")[-1]
        is_ddbc_add = tag == "add" and e.get("resp") == "ddbc"
        if e.text and e.text.strip():
            parts.append(e.text.strip() + ("{ddbc}" if is_ddbc_add else ""))
        for c in e:
            walk(c)
            if c.tail and c.tail.strip():
                parts.append(c.tail.strip())
    walk(el)
    return " ".join(parts)


def main() -> int:
    src = f"{REPO}/data/mahavyutpatti/mahavyutpatti.dila.tei.p5.xml.zip"
    out_path = f"{REPO}/data/extracted/mahavyutpatti.tsv"
    with zipfile.ZipFile(src) as z:
        name = [n for n in z.namelist()
                if n.endswith(".xml") and not n.startswith("__MACOSX")][0]
        tree = ET.parse(io.TextIOWrapper(z.open(name), encoding="utf-8"))
    root = tree.getroot()

    n = 0
    skipped = 0
    with open(out_path, "w", encoding="utf-8") as f:
        for div in root.iter("{http://www.tei-c.org/ns/1.0}div"):
            head_el = div.find("tei:head", NS)
            chapter = text_of(head_el) if head_el is not None else ""
            for entry in div.findall("tei:entry", NS):
                key = entry.get("key", "")
                iast = deva = ""
                for orth in entry.findall("tei:form/tei:orth", NS):
                    lang = orth.get("{http://www.w3.org/XML/1998/namespace}lang", "")
                    if lang == "san-Latn":
                        iast = text_of(orth)
                    elif lang == "san-Deva":
                        deva = text_of(orth)
                wylie, tib, chi = [], [], []
                for cit in entry.findall("tei:cit", NS):
                    lang = cit.get("{http://www.w3.org/XML/1998/namespace}lang", "")
                    q = cit.find("tei:quote", NS)
                    if q is None:
                        continue
                    t = text_of(q)
                    if not t:
                        continue
                    if lang == "bod-Latn":
                        wylie.append(t)
                    elif lang == "bod-Tibt":
                        tib.append(t)
                    elif lang == "zho-Hant":
                        chi.append(t)
                row = [key, chapter, iast, deva, "|".join(wylie),
                       "|".join(tib), "|".join(chi)]
                if any("\t" in c or "\n" in c for c in row):
                    skipped += 1
                    continue
                f.write("\t".join(row) + "\n")
                n += 1
    print(f"wrote {n} entries to {out_path} ({skipped} skipped)")
    return 0


if __name__ == "__main__":
    sys.exit(main())
