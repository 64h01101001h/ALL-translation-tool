#!/usr/bin/env python3
"""Build tools/digest_reference.docx — the Word styles every digest uses.

Starts from pandoc's default reference document and sets readable
spacing (Adam, 2026-09-08: "hypercritical of readability"): body text
11 pt with 1.15 line spacing and 8 pt after each paragraph; list items
4 pt apart; headings with air before and after; figures and captions
with 14 pt above and below so images never touch the text.
"""
import re, subprocess, sys, zipfile, shutil, tempfile, os

OUT = os.path.join(os.path.dirname(__file__), "digest_reference.docx")

def spacing(before=None, after=None, line=None):
    attrs = []
    if before is not None: attrs.append(f'w:before="{before}"')
    if after is not None: attrs.append(f'w:after="{after}"')
    if line is not None: attrs.append(f'w:line="{line}" w:lineRule="auto"')
    return f'<w:spacing {" ".join(attrs)}/>'

# style id → (spacing xml, keepNext)
RULES = {
    "Normal":          (spacing(after=160, line=276), False),
    "BodyText":        (spacing(after=160, line=276), False),
    "FirstParagraph":  (spacing(after=160, line=276), False),
    "Compact":         (spacing(after=80, line=264), False),
    "Heading1":        (spacing(before=480, after=200), True),
    "Heading2":        (spacing(before=420, after=160), True),
    "Heading3":        (spacing(before=320, after=120), True),
    "Title":           (spacing(after=240), False),
    "Figure":          (spacing(before=280, after=80), True),
    "CaptionedFigure": (spacing(before=280, after=80), True),
    "ImageCaption":    (spacing(before=60, after=320), False),
    "Caption":         (spacing(before=60, after=320), False),
    "TableCaption":    (spacing(before=200, after=120), False),
}

def patch_styles(xml):
    for sid, (sp, keep) in RULES.items():
        # find the style block
        m = re.search(r'(<w:style\b[^>]*w:styleId="%s"[^>]*>)(.*?)(</w:style>)' % re.escape(sid), xml, re.S)
        if not m:
            continue
        head, body, tail = m.groups()
        # paragraph properties: create or amend
        if "<w:pPr>" in body or "<w:pPr " in body:
            body = re.sub(r'<w:spacing\b[^>]*/>', '', body, count=1)
            body = re.sub(r'(<w:pPr[^>]*>)', r'\1' + sp + ('<w:keepNext/>' if keep else ''), body, count=1)
        else:
            body = body.replace(head[len(head):], '', 0)
            body = '<w:pPr>' + sp + ('<w:keepNext/>' if keep else '') + '</w:pPr>' + body
        xml = xml[:m.start()] + head + body + tail + xml[m.end():]
    # base font size 11 pt for Normal (22 half-points)
    xml = re.sub(r'(<w:style\b[^>]*w:styleId="Normal"[^>]*>.*?<w:rPr>)(.*?)(</w:rPr>)',
                 lambda m: m.group(1) + re.sub(r'<w:sz\b[^>]*/>', '', m.group(2)) + '<w:sz w:val="22"/><w:szCs w:val="22"/>' + m.group(3),
                 xml, count=1, flags=re.S)
    return xml

def main():
    tmp = tempfile.mkdtemp()
    ref = os.path.join(tmp, "ref.docx")
    with open(ref, "wb") as f:
        f.write(subprocess.check_output(["pandoc", "--print-default-data-file", "reference.docx"]))
    zin = zipfile.ZipFile(ref)
    zout = zipfile.ZipFile(OUT, "w", zipfile.ZIP_DEFLATED)
    for item in zin.infolist():
        data = zin.read(item.filename)
        if item.filename == "word/styles.xml":
            data = patch_styles(data.decode("utf-8")).encode("utf-8")
        zout.writestr(item, data)
    zout.close(); zin.close(); shutil.rmtree(tmp)
    print(OUT)

if __name__ == "__main__":
    main()
