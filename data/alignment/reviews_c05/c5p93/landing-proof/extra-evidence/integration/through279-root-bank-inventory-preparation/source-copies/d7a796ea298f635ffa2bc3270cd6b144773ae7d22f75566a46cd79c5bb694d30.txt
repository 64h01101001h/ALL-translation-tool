#!/usr/bin/env python3
"""Wrap a gen_alignment_page.py body into a finished campaign page.

Promoted out of the session scratchpad 2026-08-28. It had been living in a
temp directory as three near-identical copies (wrap_c01/c02/c03.py) that
differed only in a course number, which meant the page-production pipeline
could not be reproduced from a clean checkout at all - the generator was
tracked, its wrapper was not.

usage: gen_alignment_page.py < spec.json | wrap_alignment_page.py \
           <out.html> <title> <subtitle> <prev|-> <next|-> <course-number>

Proven byte-identical to the scratchpad wrappers on the same body before
promotion; the course number is the only parameterised difference.
"""
import sys, io
CSS = '  :root { --paper:#FAF6EE; --ink:#2B2118; --gold:#82672A; --act:#1E6B4E;\n          --machine:#B4540A; --soft:#78706A; --doc:#2E629E; --ppl:#6E3E8E; }\n  body { background:var(--paper); color:var(--ink);\n         font-family:"Iowan Old Style",Georgia,serif; margin:0;\n         padding:26px 34px; line-height:1.7; }\n  h1 { font-size:21px; color:#7C2D26; margin:0 0 2px; }\n  .sub { font-size:13px; color:var(--soft); }\n  .banner { background:#FDF3E7; border:1px solid var(--machine);\n            color:var(--machine); padding:10px 14px; font-size:13px;\n            margin:14px 0 16px; border-radius:3px; }\n  h2 { font-size:14px; color:var(--gold); letter-spacing:1.5px;\n       text-transform:uppercase; margin:26px 0 8px;\n       border-bottom:1px solid #E4DCC9; padding-bottom:3px; }\n  .controls { font-size:14px; margin:0 0 6px; }\n  .controls input { vertical-align:middle; accent-color:var(--gold); width:260px; }\n  #depthname { color:var(--gold); font-weight:600; }\n  .pair { display:grid; grid-template-columns:1fr 1fr; gap:26px; margin:10px 0 8px; }\n  .lab { font-size:10px; letter-spacing:2px; color:var(--soft); margin-bottom:4px; }\n  .tib { font-size:16.5px; } .eng { font-size:15px; }\n  .u { border-radius:2px; }\n  .active { cursor:pointer; border-bottom:1px dotted #C9B98F; }\n  .hl { background:#F3E4B3 !important; border-bottom-color:var(--gold) !important; }\n  .pin { background:#EAD98F !important; outline:1px solid var(--gold); }\n  .case { color:var(--act); font-weight:600; }\n  .nul { color:var(--soft); font-size:10.5px; vertical-align:super; }\n  .note { font-size:12px; color:var(--soft); font-style:italic; margin:4px 0 0; }\n  .note b { color:var(--machine); font-style:normal; font-size:10px; letter-spacing:1px; }\n  .tree { font-family:"SF Mono",Menlo,monospace; font-size:12.5px;\n          background:#F6F1E4; border:1px solid #E4DCC9; border-radius:3px;\n          padding:14px 18px; margin:12px 0; overflow-x:auto; line-height:1.85; }\n  .tree .k { color:var(--doc); font-size:10.5px; letter-spacing:.5px; }\n  .tree .w { color:var(--ink); font-weight:600; }\n  .tree .e { color:var(--act); }\n  .tree .m { color:var(--machine); }\n  .tree i  { color:var(--soft); }\n  .legend { font-size:12px; color:var(--soft); border-top:1px solid #E4DCC9;\n            margin-top:22px; padding-top:10px; }\n  .wil { color:var(--ppl); }\n  .nav { font-size:13px; margin:6px 0 0; }\n  .nav a { color:var(--doc); text-decoration:none; margin-right:18px; }\n  .nav a:hover { text-decoration:underline; }\n'
JS = '<script>\nconst names={1:\'1 \\u00b7 Sentence\',2:\'2 \\u00b7 Clause\',3:\'3 \\u00b7 Phrase\',4:\'4 \\u00b7 Embedded phrase\',5:\'5 \\u00b7 Word / compound\',6:\'6 \\u00b7 Morpheme / particle\'};\nconst slider=document.getElementById(\'depth\'), nm=document.getElementById(\'depthname\');\nfunction apply(){\n  const d=slider.value; nm.textContent=names[d];\n  document.querySelectorAll(\'.u\').forEach(u=>{\n    u.classList.toggle(\'active\', u.dataset.d===d);\n    u.classList.remove(\'hl\',\'pin\');\n  });\n}\nslider.addEventListener(\'input\',apply); apply();\nconst peers=el=>document.querySelectorAll(`[data-l="${el.dataset.l.split(\' \')[0]}"]`);\ndocument.addEventListener(\'mouseover\',e=>{const t=e.target.closest(\'.active\');if(t)peers(t).forEach(p=>p.classList.add(\'hl\'));});\ndocument.addEventListener(\'mouseout\',e=>{const t=e.target.closest(\'.active\');if(t)peers(t).forEach(p=>p.classList.remove(\'hl\'));});\ndocument.addEventListener(\'click\',e=>{\n  const t=e.target.closest(\'.active\');\n  document.querySelectorAll(\'.pin\').forEach(p=>p.classList.remove(\'pin\'));\n  if(t)peers(t).forEach(p=>p.classList.add(\'pin\'));\n});\n</script>'
BANNER = """<div class="banner"><b>MACHINE ALIGNMENT — AI (Claude), PROVISIONAL. PASS THREE: THE FULL-DEPTH RUN.</b>
Not HGM dictionary material. Geshe Michael's English is verbatim and untouched; the
machine matches spans of it and never composes. Grammar labels are the machine's
analysis under Wilson's taxonomy (SOAS POS as cross-reference) and are themselves
provisional. &#8709; marks a Tibetan morpheme with no English exponent. Input variants
are flagged, never silently corrected.</div>"""
CONTROLS = """<div class="controls">
  Depth: <input type="range" id="depth" min="1" max="7" value="2">
  <span id="depthname">2 &middot; Clause</span>
  <span style="color:var(--soft);font-size:12px">&mdash; slide deeper; hover either language; click pins</span><br>
  <span style="font-size:11.5px;color:var(--soft)">1 Sentence &middot; 2 Clause &middot; 3 Phrase &middot; 4 Embedded phrase &middot; 5 Word/compound &middot; 6 Morpheme &amp; particle &middot; 7 Syllable/compound member</span>
</div>"""
LEGEND_T = """<div class="legend"><b>Depth key (Wilson-grounded, seven layers):</b> as the C02 set; verse pages add: line-crossing links honor GMR's published verse order. &middot; Source: hgm_spine_v27_2 corpus C{course2}. English: Geshe Michael Roach, ACI Course {course}, verbatim. Analysis &amp; links: Claude (AI), PROVISIONAL &mdash; evaluation only, not dictionary material.</div>"""
def main():
    out, title, sub, prev, nxt = sys.argv[1:6]
    course = sys.argv[6] if len(sys.argv) > 6 else "3"
    LEGEND = (LEGEND_T.replace("{course}", course)
                       .replace("{course2}", "0" + course))
    body = sys.stdin.read()
    nav = ["<a href='index.html'>&#8962; Index</a>"]
    if prev != "-": nav.append(f"<a href='{prev}'>&larr; Previous</a>")
    if nxt != "-": nav.append(f"<a href='{nxt}'>Next &rarr;</a>")
    navh = "<div class='nav'>" + "".join(nav) + "</div>"
    html = f"""<!DOCTYPE html>
<html lang="en">
<head>
<meta charset="utf-8">
<title>{title}</title>
<style>
{CSS}</style>
</head>
<body data-depth="2">
<h1>{title}</h1>
<div class="sub">{sub}</div>
{navh}
{BANNER}
{CONTROLS}
{body}
{LEGEND}
{navh}
{JS}
</body>
</html>
"""
    io.open(out, "w", encoding="utf-8").write(html)
    print(f"wrote {out} ({len(html)} bytes)")
main()
