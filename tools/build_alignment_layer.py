#!/usr/bin/env python3
"""build_alignment_layer.py — the ACI alignment evidence layer.

Converts the delivered full-depth alignment pages (span markup, the
source of truth for pass three) into a versioned evidence sidecar the
app can load: word-level (depth-5) Tibetan<->English pairs, each
citing the corpus segments that attest it.

GUARDRAILS (inviolable rule 1): every English string in this layer is
Geshe Michael's own corpus English, machine-MATCHED never composed.
The layer is evidence, tier TENTATIVE; it never writes hgm_gloss and
the card renders it amber, toggleable, unmistakably labeled.

Battery (refuses to write on any failure — rule 3):
  * every pair's Tibetan must appear letter-exact in the wylie of
    every segment it cites, straight from the spine;
  * every English fragment must appear letter-exact in that segment's
    English;
  * the pair count must not fall below the floor (a thin layer is a
    broken parse, not a small corpus).

Stage-then-swap. Usage: build_alignment_layer.py [--pages DIR]
"""
import hashlib
import html
import io
import json
import os
import re
import sqlite3
import sys
from datetime import date

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
sys.path.insert(0, os.path.join(ROOT, "engines"))
from hgm_tools import acip_to_ewts          # noqa: E402  (canonical)

SPINE = os.path.join(ROOT, "build", "hgm_spine_v27_2.db")
OUTDIR = os.path.join(ROOT, "data", "alignment")
OUT = os.path.join(OUTDIR, "alignment_evidence_v1.json")
OUT_FULL = os.path.join(OUTDIR, "alignment_full_v1.json")
FLOOR = 150   # distinct word-level pairs across all courses

# course-aware (Adam's standing policy: every new reading scans at
# full depth and flows into the evidence layer the same way)
COURSES = {
    "C02": {
        # complete=True means EVERY segment of the course must be
        # registered on some page. Adam 2026-08-28: C01 was declared
        # complete in prose while 25 segments had no page at all, and
        # nothing could catch it because the gates only ever checked
        # the segments a page CITES. Completeness is now a claim the
        # builder proves or refuses.
        "complete": True,    # LOCKED 2026-08-28: gate-proved 68/68
        "dir": os.path.join(OUTDIR, "pages"),
        "pages": {
            "c2L1": [1, 2, 3], "c2L2": [4, 5, 6], "c2L3": [7, 8, 9], "c2L4": [10, 11, 12], "c2L5": [13, 14, 15], "c2L6": [16, 17, 18], "c2L7": [19, 20, 21, 22], "p01": [23], "p02": [24, 25], "p03": [26, 27, 28, 29, 30],
            "p04": [31, 32, 33, 34], "p05": [35, 36, 37],
            "p06": [38, 39, 40], "p07": [41, 42, 43, 44, 45],
            "p08": [46], "p09": [47, 48, 49], "p10": [50, 51, 52],
            "p11": [53, 54, 55, 56], "p12": [57, 58, 59],
            "p13": [60, 61], "p14": [62, 63, 64],
            "p15": [65, 66, 67, 68],
        },
    },
    "C01": {
        "complete": True,    # LOCKED 2026-08-28: gate-proved 496/496
        "dir": os.path.join(OUTDIR, "pages_c01"),
        "pages": {
            "c1L1": [1, 2, 3], "c1L2": [4, 5, 6], "c1L3": [7, 8, 9], "c1L4": [10, 11, 12], "c1L5": [13, 14, 15], "c1L6": [16, 17, 18], "c1L7": [19, 20, 21, 22], "c1L8": [39, 43, 143], "r1": [23, 24, 25, 26], "r2": [27, 28, 29],
            "r3": [30, 31], "r4": [32, 33, 34, 35, 36],
            "r5": [37, 38],
            "c1p01": [40, 41, 42],
            "c1p02": [44, 45],
            "c1p03": [46, 47],
            "c1p04": [48, 49, 50],
            "c1p05": [51, 52, 53],
            "c1p06": [54, 55, 56],
            "c1p07": [57, 58, 59],
            "c1p08": [60, 61, 62],
            "c1p09": [63, 64, 65],
            "c1p10": [66, 67, 68],
            "c1p11": [69, 70, 71],
            "c1p12": [72, 73, 74],
            "c1p13": [75, 76, 77],
            "c1p14": [78, 79, 80],
            "c1p15": [81, 82, 83],
            "c1p16": [84, 85, 86],
            "c1p17": [87, 88, 89],
            "c1p18": [90, 91, 92],
            "c1p19": [93, 94, 95],
            "c1p20": [96, 97, 98],
            "c1p21": [99, 100, 101],
            "c1p22": [102, 103, 104],
            "c1p23": [105, 106, 107],
            "c1p24": [108, 109, 110],
            "c1p25": [111, 112, 113],
            "c1p26": [114, 115, 116],
            "c1p27": [117, 118, 119],
            "c1p28": [120, 121, 122],
            "c1p29": [123, 124, 125],
            "c1p30": [126, 127, 128],
            "c1p31": [129, 130, 131],
            "c1p32": [132, 133, 134],
            "c1p33": [135, 136, 137],
            "c1p34": [138, 139, 140],
            "c1p35": [141, 142],
            "c1p36": [144, 145],
            "c1p37": [146, 147, 148],
            "c1p38": [149, 150, 151],
            "c1p39": [152, 153, 154],
            "c1p40": [155, 156, 157],
            "c1p41": [158, 159, 160],
            "c1p42": [161, 162, 163],
            "c1p43": [164, 165, 166],
            "c1p44": [167, 168, 169],
            "c1p45": [170, 171, 172],
            "c1p46": [173, 174, 175],
            "c1p47": [176, 177, 178],
            "c1p48": [179, 180, 181],
            "c1p49": [182, 183, 184],
            "c1p50": [185, 186, 187],
            "c1p51": [188, 189, 190],
            "c1p52": [191, 192, 193], "c1p53": [194, 195, 196], "c1p54": [197, 198, 199], "c1p55": [200, 201, 202], "c1p56": [203, 204, 205], "c1p57": [206, 207, 208], "c1p58": [209, 210, 211], "c1p59": [212, 213, 214], "c1p60": [215, 216, 217], "c1p61": [218, 219, 220], "c1p62": [221, 222, 223], "c1p63": [224, 225, 226], "c1p64": [227, 228, 229], "c1p65": [230, 231, 232], "c1p66": [233, 234, 235], "c1p67": [236, 237, 238], "c1p68": [239, 240, 241], "c1p69": [242, 243, 244], "c1p70": [245, 246, 247], "c1p71": [248, 249, 250], "c1p72": [251, 252, 253], "c1p73": [254, 255, 256], "c1p74": [257, 258, 259], "c1p75": [260, 261, 262], "c1p76": [263, 264, 265], "c1p77": [266, 267, 268], "c1p78": [269, 270, 271], "c1p79": [272, 273, 274], "c1p80": [275, 276, 277], "c1p81": [278, 279, 280], "c1p82": [281, 282, 283], "c1p83": [284, 285, 286], "c1p84": [287, 288, 289], "c1p85": [290, 291, 292], "c1p86": [293, 294, 295], "c1p87": [296, 297, 298], "c1p88": [299, 300, 301], "c1p89": [302, 303, 304], "c1p90": [305, 306, 307], "c1p91": [308, 309, 310], "c1p92": [311, 312, 313], "c1p93": [314, 315, 316], "c1p94": [317, 318, 319], "c1p95": [320, 321, 322], "c1p96": [323, 324, 325], "c1p97": [326, 327, 328], "c1p98": [329, 330, 331], "c1p99": [332, 333, 334], "c1p100": [335, 336, 337], "c1p101": [338, 339, 340], "c1p102": [341, 342, 343], "c1p103": [344, 345, 346], "c1p104": [347, 348, 349], "c1p105": [350, 351, 352], "c1p106": [353, 354, 355], "c1p107": [356, 357, 358], "c1p108": [359, 360, 361], "c1p109": [362, 363, 364], "c1p110": [365, 366, 367], "c1p111": [368, 369, 370], "c1p112": [371, 372, 373], "c1p113": [374, 375, 376], "c1p114": [377, 378, 379], "c1p115": [380, 381, 382], "c1p116": [383, 384, 385], "c1p117": [386, 387, 388], "c1p118": [389, 390, 391], "c1p119": [392, 393, 394], "c1p120": [395, 396, 397], "c1p121": [398, 399, 400], "c1p122": [401, 402, 403], "c1p123": [404, 405, 406], "c1p124": [407, 408, 409], "c1p125": [410, 411, 412], "c1p126": [413, 414, 415], "c1p127": [416, 417, 418], "c1p128": [419, 420, 421], "c1p129": [422, 423, 424], "c1p130": [425, 426, 427], "c1p131": [428, 429, 430], "c1p132": [431, 432, 433], "c1p133": [434, 435, 436], "c1p134": [437, 438, 439], "c1p135": [440, 441, 442], "c1p136": [443, 444, 445], "c1p137": [446, 447, 448], "c1p138": [449, 450, 451], "c1p139": [452, 453, 454], "c1p140": [455, 456, 457], "c1p141": [458, 459, 460], "c1p142": [461, 462, 463], "c1p143": [464, 465, 466], "c1p144": [467, 468, 469], "c1p145": [470, 471, 472], "c1p146": [473, 474, 475], "c1p147": [476, 477, 478], "c1p148": [479, 480, 481], "c1p149": [482, 483, 484], "c1p150": [485, 486, 487], "c1p151": [488, 489, 490], "c1p152": [491, 492, 493], "c1p153": [494, 495, 496],
        },
    },
    "C03": {
        "complete": True,    # LOCKED 2026-09-03: gate-proved 620/620 (200 pages; markers 22/294/303/430/476/513/558)
        "dir": os.path.join(OUTDIR, "pages_c03"),
        "pages": {
            "c3p1": [1, 2, 3], "c3p2": [4, 5, 6], "c3p3": [7, 8, 9], "c3p4": [10, 11, 12], "c3p5": [13, 14, 15], "c3p6": [16, 17, 18], "c3p7": [19, 20, 21], "c3p8": [22, 23, 24], "c3p9": [25, 26, 27], "c3p10": [28, 29, 30], "c3p11": [31, 32, 33], "c3p12": [34, 35, 36], "c3p13": [37, 38, 39], "c3p14": [40, 41, 42], "c3p15": [43, 44, 45], "c3p16": [46, 47, 48], "c3p17": [49, 50, 51], "c3p18": [52, 53, 54], "c3p19": [55, 56, 57], "c3p20": [58, 59, 60], "c3p21": [61, 62, 63], "c3p22": [64, 65, 66], "c3p23": [67, 68, 69], "c3p24": [70, 71, 72], "c3p25": [73, 74, 75], "c3p26": [76, 77, 78], "c3p27": [79, 80, 81], "c3p28": [82, 83, 84], "c3p29": [85, 86, 87], "c3p30": [88, 89, 90], "c3p31": [91, 92, 93], "c3p32": [94, 95, 96], "c3p33": [97, 98, 99], "c3p34": [100, 101, 102], "c3p35": [103, 104, 105], "c3p36": [106, 107, 108], "c3p37": [109, 110, 111], "c3p38": [112, 113, 114], "c3p39": [115, 116, 117], "c3p40": [118, 119, 120], "c3p41": [121, 122, 123], "c3p42": [124, 125, 126], "c3p43": [127, 128, 129], "c3p44": [130, 131, 132], "c3p45": [133, 134, 135], "c3p46": [136, 137, 138], "c3p47": [139, 140, 141], "c3p48": [142, 143, 144], "c3p49": [145, 146, 147], "c3p50": [148, 149, 150], "c3p51": [151, 152, 153], "c3p52": [154, 155, 156], "c3p53": [157, 158, 159],
                   "c3p54": [160, 161, 162],
                   "c3p55": [163, 164, 165],
                   "c3p56": [166, 167, 168],
                   "c3p57": [169, 170, 171],
                   "c3p58": [172, 173, 174],
                   "c3p59": [175, 176, 177],
                   "c3p60": [178, 179, 180],
                   "c3p61": [181, 182, 183],
                   "c3p62": [184, 185, 186],
                   "c3p63": [187, 188, 189],
                   "c3p64": [190, 191, 192],
                   "c3p65": [193, 194, 195],
                   "c3p66": [196, 197, 198],
                   "c3p67": [199, 200, 201],
                   "c3p68": [202, 203],
                   "c3p69": [204, 205],
                   "c3p70": [206, 207],
                   "c3p71": [208, 209],
                   "c3p72": [210, 211],
                   "c3p73": [212],
                   "c3p74": [213, 214, 215],
                   "c3p75": [216, 217],
                   "c3p76": [218, 219, 220, 221, 222],
                   "c3p77": [223, 224, 225, 226, 227],
                   "c3p78": [228, 229, 230, 231, 232],
                   "c3p79": [233, 234, 235, 236, 237],
                   "c3p80": [238, 239, 240, 241, 242],
                   "c3p81": [243, 244, 245, 246, 247],
                   "c3p82": [248, 249, 250, 251, 252],
                   "c3p83": [253, 254, 255, 256, 257],
                   "c3p84": [258, 259, 260, 261, 262],
                   "c3p85": [263, 264, 265],
                   "c3p86": [266, 267, 268, 269, 270],
                   "c3p87": [271, 272, 273, 274, 275],
                   "c3p88": [276, 277, 278, 279, 280],
                   "c3p89": [281, 282, 283, 284, 285],
                   "c3p90": [286, 287, 288, 289, 290],
                   "c3p91": [291, 292, 293, 294, 295],
                   "c3p92": [296, 297, 298],
                   "c3p93": [299, 300, 301],
                   "c3p94": [302, 303, 304],
                   "c3p95": [305, 306, 307],
                   "c3p96": [308, 309, 310],
                   "c3p97": [311, 312, 313],
                   "c3p98": [314, 315, 316],
                   "c3p99": [317, 318, 319],
                   "c3p100": [320, 321, 322],
                   "c3p101": [323, 324, 325],
                   "c3p102": [326, 327, 328],
                   "c3p103": [329, 330, 331],
                   "c3p104": [332, 333, 334],
                   "c3p105": [335, 336, 337],
                   "c3p106": [338, 339, 340],
                   "c3p107": [341, 342, 343],
                   "c3p108": [344, 345, 346],
                   "c3p109": [347, 348, 349],
                   "c3p110": [350, 351, 352],
                   "c3p111": [353, 354, 355],
                   "c3p112": [356, 357, 358],
                   "c3p113": [359, 360, 361],
                   "c3p114": [362, 363, 364],
                   "c3p115": [365, 366, 367],
                   "c3p116": [368, 369, 370],
                   "c3p117": [371, 372, 373],
                   "c3p118": [374, 375, 376],
                   "c3p119": [377, 378, 379],
                   "c3p120": [380, 381, 382],
                   "c3p121": [383, 384, 385],
                   "c3p122": [386, 387, 388],
                   "c3p123": [389, 390],
                   "c3p124": [391, 392, 393],
                   "c3p125": [394, 395, 396],
                   "c3p126": [397, 398, 399],
                   "c3p127": [400, 401, 402],
                   "c3p128": [403, 404, 405],
                   "c3p129": [406, 407, 408],
                   "c3p130": [409, 410, 411],
                   "c3p131": [412, 413, 414],
                   # c3p131 took THREE attempts. Refused first for a d=5
                   # listed after the d=3 containing it, then for depth-7
                   # members hung off a depth-3 clause with no depth-5 parent.
                   # The second refusal was the brief's omission, not the
                   # analyst's - the rules had never said d=7 needs a d=5
                   # parent, because until then nobody had tried it. Landed
                   # flat: d=5 words and d=6 particles at top level, d=7
                   # members inside their compounds, no clause layer at all.
                   "c3p132": [415, 416, 417],
                   "c3p133": [418, 419, 420],
                   "c3p134": [421, 422, 423],
                   "c3p135": [424, 425, 426],
                   "c3p136": [427, 428, 429],
                   "c3p137": [430, 431, 432],
                   "c3p138": [433, 434, 435],
                   "c3p139": [436, 437, 438],
                   "c3p140": [439, 440, 441],
                   "c3p141": [442, 443, 444],
                   "c3p142": [445, 446, 447],
                   "c3p143": [448, 449, 450],
                   "c3p144": [451, 452, 453],
                   "c3p145": [454, 455, 456],
                   "c3p146": [457, 458, 459],
                   "c3p147": [460, 461, 462],
                   "c3p148": [463, 464, 465],
                   "c3p149": [466, 467, 468],
                   "c3p150": [469, 470, 471],
                   "c3p151": [472, 473, 474],
                   "c3p152": [475, 476, 477],
                   "c3p153": [478, 479, 480],
                   "c3p154": [481, 482, 483],
                   "c3p155": [484, 485, 486],
                   "c3p156": [487, 488, 489],
                   "c3p157": [490, 491, 492],
                   "c3p158": [493, 494, 495],
                   "c3p159": [496, 497, 498],
                   "c3p160": [499, 500, 501],
                   "c3p161": [502, 503, 504],
                   "c3p162": [505, 506, 507],
                   "c3p163": [508, 509, 510],
                   "c3p164": [511, 512, 513],
                   "c3p165": [514, 515, 516],
                   "c3p166": [517, 518, 519],
                   "c3p167": [520, 521, 522],
                   "c3p168": [523, 524, 525],
                   "c3p169": [526, 527, 528],
                   "c3p170": [529, 530, 531],
                   "c3p171": [532, 533, 534],
                   "c3p172": [535, 536, 537],
                   "c3p173": [538, 539, 540],
                   "c3p174": [541, 542, 543],
                   "c3p175": [544, 545, 546],
                   "c3p176": [547, 548, 549],
                   "c3p177": [550, 551, 552],
                   "c3p178": [553, 554, 555],
                   "c3p179": [556, 557, 558],
                   "c3p180": [559, 560, 561],
                   "c3p181": [562, 563, 564],
                   "c3p182": [565, 566, 567],
                   "c3p183": [568, 569, 570],
                   "c3p184": [571, 572, 573],
                   "c3p185": [574, 575, 576],
                   "c3p186": [577, 578, 579],
                   "c3p187": [580, 581, 582],
                   "c3p188": [583, 584, 585],
                   "c3p189": [586, 587, 588],
                   "c3p190": [589, 590, 591],
                   "c3p191": [592, 593, 594],
                   "c3p192": [595, 596, 597],
                   "c3p193": [598, 599, 600],
                   "c3p194": [601, 602, 603],
                   "c3p195": [604, 605, 606],
                   "c3p196": [607, 608, 609],
                   "c3p197": [610, 611, 612],
                   "c3p198": [613, 614, 615],
                   "c3p199": [616, 617, 618],
                   "c3p200": [619, 620],
        },
    },
    "C04": {
        # OPENED 2026-09-04 after the C03-stratified audit and the
        # no_supplied_span_head gate (Phase 3 gate §5b precondition).
        # 198 segments. 1-21 are byte-identical to C03:1-21 (the
        # liturgical opening): c4p1-c4p7 are the landed c3p1-c3p7 copied
        # verbatim with course labels changed (2026-09-04); every sentence
        # was checked against the C04 rows at copy time and the gates
        # below re-derive every span against them on each run.
        "complete": True,
        "dir": os.path.join(OUTDIR, "pages_c04"),
        "pages": {
            "c4p1": [1, 2, 3], "c4p2": [4, 5, 6], "c4p3": [7, 8, 9], "c4p4": [10, 11, 12], "c4p5": [13, 14, 15], "c4p6": [16, 17, 18], "c4p7": [19, 20, 21],
            "c4p8": [22, 23, 24], "c4p9": [25, 26, 27], "c4p10": [28, 29, 30], "c4p11": [31, 32, 33], "c4p12": [34, 35, 36], "c4p13": [37, 38, 39], "c4p14": [40, 41, 42], "c4p15": [43, 44, 45], "c4p16": [46, 47, 48], "c4p17": [49, 50, 51], "c4p18": [52, 53, 54], "c4p19": [55, 56, 57], "c4p20": [58, 59, 60], "c4p21": [61, 62, 63], "c4p22": [64, 65, 66], "c4p23": [67, 68, 69], "c4p24": [70, 71, 72], "c4p25": [73, 74, 75], "c4p26": [76, 77, 78], "c4p27": [79, 80, 81], "c4p28": [82, 83, 84], "c4p29": [85, 86, 87], "c4p30": [88, 89, 90], "c4p31": [91, 92, 93], "c4p32": [94, 95, 96], "c4p33": [97, 98, 99], "c4p34": [100, 101, 102], "c4p35": [103, 104, 105], "c4p36": [106, 107, 108], "c4p37": [109, 110, 111], "c4p38": [112, 113, 114], "c4p39": [115, 116, 117], "c4p40": [118, 119, 120], "c4p41": [121, 122, 123], "c4p42": [124, 125, 126], "c4p43": [127, 128, 129], "c4p44": [130, 131, 132], "c4p45": [133, 134, 135], "c4p46": [136, 137, 138], "c4p47": [139, 140, 141], "c4p48": [142, 143, 144], "c4p49": [145, 146, 147], "c4p50": [148, 149, 150], "c4p51": [151, 152, 153], "c4p52": [154, 155, 156], "c4p53": [157, 158, 159], "c4p54": [160, 161, 162], "c4p55": [163, 164, 165], "c4p56": [166, 167, 168], "c4p57": [169, 170, 171], "c4p58": [172, 173, 174], "c4p59": [175, 176, 177], "c4p60": [178, 179, 180], "c4p61": [181, 182, 183], "c4p62": [184, 185, 186], "c4p63": [187, 188, 189], "c4p64": [190, 191, 192], "c4p65": [193, 194, 195], "c4p66": [196, 197, 198],
        },
    },
    "C05": {
        # OPENED 2026-09-05 (Adam: scan C05 then C06). 511 segments.
        # 1-21 byte-identical to C03:1-21: c5p1-c5p7 are c3p1-c3p7 copied
        # verbatim with course labels changed, checked against the C05
        # rows at copy time (21/21); the gates re-derive them each run.
        "complete": False,
        "dir": os.path.join(OUTDIR, "pages_c05"),
        "pages": {
            "c5p1": [1, 2, 3], "c5p2": [4, 5, 6], "c5p3": [7, 8, 9], "c5p4": [10, 11, 12], "c5p5": [13, 14, 15], "c5p6": [16, 17, 18], "c5p7": [19, 20, 21],
            "c5p8": [22, 23, 24], "c5p9": [25, 26, 27], "c5p10": [28, 29, 30], "c5p11": [31, 32, 33], "c5p12": [34, 35, 36], "c5p13": [37, 38, 39], "c5p14": [40, 41, 42], "c5p15": [43, 44, 45], "c5p16": [46, 47, 48], "c5p17": [49, 50, 51], "c5p18": [52, 53, 54], "c5p19": [55, 56, 57], "c5p20": [58, 59, 60], "c5p21": [61, 62, 63], "c5p22": [64, 65, 66], "c5p23": [67, 68, 69], "c5p24": [70, 71, 72], "c5p25": [73, 74, 75], "c5p26": [76, 77, 78], "c5p27": [79, 80, 81], "c5p28": [82, 83, 84], "c5p29": [85, 86, 87], "c5p30": [88, 89, 90], "c5p31": [91, 92, 93], "c5p32": [94, 95, 96], "c5p33": [97, 98, 99], "c5p34": [100, 101, 102], "c5p35": [103, 104, 105], "c5p36": [106, 107, 108],
        },
    },
}

# The class attribute is "u" plus whatever grammar label the analyst supplied
# ("u noun", "u verb-nominal", "u compound member, noun"). This pattern
# required class="u" EXACTLY, so every labelled span was invisible to the
# builder and its analysis was discarded without a word. Measured when found
# on 2026-08-28: 1,260 spans across 141 of 320 pages - 2.7% of everything
# analysed, and 77-80% of the three newest C03 pages, whose analysts happened
# to label heavily. The page rendered the labels correctly the whole time,
# which is why nothing looked wrong.
SPAN = re.compile(
    r'<span class="u(?:[^"]*)" data-d="(\d)" data-l="([A-Za-z0-9_ ]+?)">')
# The data-l character class excludes "-" and that exclusion is LOAD-BEARING,
# so it is stated rather than left to look like an oversight. A hyphenated
# label - s434-typo, s396app-lab, s453-jaws - marks an APPARATUS span: a typo
# queue entry, a supplied-sentence flag, an editorial note. Those carry a
# <span class="nul"> body, are not alignment pairs, and must never enter the
# dictionary. 13 of them exist across 11 C01 pages. test_builder_sees_every_span
# counts them separately and prints the number, so the exclusion stays visible
# instead of being an accident of a character class.
APPARATUS = re.compile(
    r'<span class="u[^"]*" data-d="\d" data-l="[A-Za-z0-9_ ]*-[^"]*"\s*>')
NOTE = re.compile(r'<div class="note">(.*?)</div>', re.S)
TREE = re.compile(r'<div class="tree">(.*?)</div>', re.S)


def letters(s):
    return re.sub(r"[^a-z']", "", s.lower())


_ACIP_CACHE = {}


def norm_w(s):
    return re.sub(r"\s+", " ", s or "").strip()


def acip_span(acip, wylie, span):
    """Recover the ACIP substring whose conversion is `span`.

    ACIP is the source of record (Adam, 2026-08-28); the wylie column
    is a derived conversion, proven equal to acip_to_ewts(acip) on
    8,961/8,961 segments of C01-C18. acip_to_ewts is monotonic and
    order-preserving but not length-preserving (TS->tsh) and spans may
    begin mid-token (1a/gser), so map by converted-prefix offsets
    rather than by tokens. Returns None rather than guessing.
    """
    if not acip:
        return None
    want = norm_w(span)
    if not want:
        return None
    cached = _ACIP_CACHE.get(acip)
    if cached is None:
        conv = acip_to_ewts(acip)
        lens = [len(acip_to_ewts(acip[:k]))
                for k in range(len(acip) + 1)]
        cached = _ACIP_CACHE[acip] = (conv, lens)
    conv, lens = cached

    # Candidate offsets, token-boundaried first: a bare span like
    # "shog" also occurs INSIDE "tshogs", and the naive first hit
    # would recover the wrong ACIP. Boundary hits are tried before
    # interior ones, and every candidate must still round-trip.
    hits, k = [], conv.find(want)
    while k >= 0:
        hits.append(k)
        k = conv.find(want, k + 1)
    if not hits:
        return None

    def bounded(x):
        before = (x == 0) or conv[x - 1] in " ,"
        y = x + len(want)
        after = (y == len(conv)) or conv[y] in " ,"
        return before and after

    for i in sorted(hits, key=lambda x: (not bounded(x), x)):
        j = i + len(want)
        start = max((k for k in range(len(lens)) if lens[k] <= i),
                    default=None)
        end = next((k for k in range(len(lens)) if lens[k] >= j), None)
        if start is None or end is None or end <= start:
            continue
        got = acip[start:end].strip().strip(",").strip()
        # the proof: a recovery is accepted only if it round-trips
        if got and norm_w(acip_to_ewts(got)) == want:
            return got
    return None


def strip_tags(s):
    # Tags become NOTHING, not a space. Both page sides are generated from
    # the spine text verbatim, so deleting the markup reconstructs exactly
    # that text -- literal spaces between sibling spans are already in the
    # source. Substituting a space instead was a hack that held only while
    # every nested span happened to sit on a word boundary; the moment a
    # depth-7 member split `dbu ma'i` into `dbu ma` + `'i`, it banked the
    # headword as "dbu ma 'i", a Tibetan string that does not exist.
    # Found 2026-08-28 via the ACIP round-trip, which could not convert it.
    return html.unescape(re.sub(r"<[^>]+>", "", s))


def spans_of(doc, side):
    """Yield (depth, id, text) for every span inside the given side
    ('tib' or 'eng') of every .pair block, text tag-stripped."""
    out = []
    for chunk in doc.split('<div class="pair">')[1:]:
        for stop in ('<div class="note"', '<h2'):
            i = chunk.find(stop)
            if i >= 0:
                chunk = chunk[:i]
        j = chunk.find('<div class="eng">')
        if j < 0:
            continue
        block = chunk[:j] if side == "tib" else chunk[j:]
        # the machine's own notation is not evidence: drop nul-marker
        # annotations and bracketed editorial text before walking
        block = re.sub(r'<span class="nul">.*?</span>', "", block,
                       flags=re.S)
        block = re.sub(r"<i[^>]*>\[NO ENGLISH.*?</i>", "", block,
                       flags=re.S)
        # walk spans with a manual stack so nesting resolves
        pos, stack, hits = 0, [], []
        for tk in re.finditer(r'<span\b[^>]*>|</span>', block):
            if tk.group(0).startswith("</"):
                if stack:
                    d, l, start = stack.pop()
                    hits.append((d, l, block[start:tk.start()]))
            else:
                m2 = SPAN.match(tk.group(0))
                if m2:
                    toks = m2.group(2).split(" ")
                    stack.append((int(m2.group(1)),
                                  toks[0] +
                                  ("|case" if "case" in toks else ""),
                                  tk.end()))
                else:
                    stack.append((0, "", tk.end()))
        for d, l, raw in hits:
            if d and l:
                out.append((d, l, " ".join(strip_tags(raw).split())))
    return out


def main():
    con = sqlite3.connect(SPINE)
    pairs = {}   # tib_norm -> {eng_display -> set("COURSE:seq")}
    acip_forms = {}   # tib_norm -> set(ACIP forms recovered)
    acip_miss = []    # spans whose ACIP could not be proven
    full = {"links": [], "notes": {}, "trees": {}}
    sha = hashlib.sha256()
    for course, cfg in sorted(COURSES.items()):
        segs = {r[0]: (r[1], r[2], r[3]) for r in con.execute(
            "SELECT seq, wylie, english, acip FROM corpus_segments "
            "WHERE course=?", (course,))}
        # COVERAGE GATE (Adam 2026-08-28). A course that claims
        # completeness must have a page for every segment; a course
        # that does not must still say out loud what is missing, so a
        # gap is never silent.
        registered = set()
        for _pg, _sl in cfg["pages"].items():
            registered.update(_sl)
        gap = sorted(set(segs) - registered)
        if gap:
            rngs, s, prev = [], gap[0], gap[0]
            for q in gap[1:]:
                if q == prev + 1:
                    prev = q
                else:
                    rngs.append((s, prev)); s = prev = q
            rngs.append((s, prev))
            desc = ", ".join("%d" % a if a == b else "%d-%d" % (a, b)
                             for a, b in rngs)
            if cfg.get("complete"):
                sys.exit("REFUSED: %s claims complete but %d segment(s) "
                         "have no page: %s" % (course, len(gap), desc))
            print("COVERAGE %s: %d/%d segments scanned, %d open (%s)"
                  % (course, len(registered), len(segs),
                     len(gap), desc if len(desc) < 90
                     else desc[:87] + "..."))
        elif not cfg.get("complete"):
            print("COVERAGE %s: %d/%d scanned — no gaps; set "
                  "complete=True to lock it" % (course, len(registered),
                                                len(segs)))

        for pg, _seglist in sorted(cfg["pages"].items()):
            path = os.path.join(cfg["dir"], pg + ".html")
            # A page registered but never written means a generator run
            # died between the sed-register and the page write. Say so
            # plainly instead of dying on a FileNotFoundError traceback.
            if not os.path.exists(path):
                sys.exit("REFUSED: %s/%s is registered in COURSES but the "
                         "page file does not exist (%s). Either the "
                         "generator failed after registration, or the "
                         "registration is stale — write the page or "
                         "remove the entry." % (course, pg, path))
            doc = io.open(path, encoding="utf-8").read()
            sha.update(doc.encode())
            tib, eng = {}, {}
            tibAll, engAll = {}, {}
            for d, l, txt in spans_of(doc, "tib"):
                lid, case = l.split("|")[0], l.endswith("|case")
                tibAll.setdefault((d, lid), {"t": txt, "case": case})
                if d == 5 and txt:
                    tib.setdefault(lid, txt)
            for d, l, txt in spans_of(doc, "eng"):
                lid = l.split("|")[0]
                engAll.setdefault((d, lid), []).append(txt)
                if d == 5 and txt:
                    eng.setdefault(lid, []).append(txt)
            # THE BANK: every span at every depth, both-sided or not.
            for (d, lid), rec in sorted(tibAll.items()):
                m0 = re.match(r"s(\d+)", lid)
                _sg = int(m0.group(1)) if m0 else None
                _ac = (acip_span(segs[_sg][2], segs[_sg][0], rec["t"])
                       if _sg in segs else None)
                full["links"].append({
                    "course": course, "page": pg,
                    "seg": _sg,
                    "id": lid, "d": d, "tib": rec["t"],
                    "tib_acip": _ac,
                    "eng": " … ".join(engAll.get((d, lid), [])) or None,
                    "case": rec["case"],
                })
            key = course + "/" + pg
            full["notes"][key] = [
                " ".join(strip_tags(n).split())
                for n in NOTE.findall(doc)]
            tr = TREE.findall(doc)
            if tr:
                full["trees"][key] = [
                    " ".join(strip_tags(t).split()) for t in tr]
            for l, t in tib.items():
                if l not in eng:
                    continue   # no-exponent word: not evidence
                m = re.match(r"s(\d+)", l)
                if not (m and int(m.group(1)) in segs):
                    continue
                seg = int(m.group(1))
                e = " … ".join(eng[l])
                # NO .lower(). In Wylie a capital is a DIFFERENT LETTER -
                # N is the retroflex Na, D the retroflex Da, Sh the retroflex
                # sha, and A/I/U the long vowels. Lowercasing produced 16
                # headwords that are not Tibetan words at all: paN chen ->
                # "pan chen", nA ro pa -> "na ro pa", shA ri'i bu -> "sha ri'i
                # bu", maNDla -> "mandla", paNDi ta -> "pandi ta". Those are
                # the Sanskrit loanwords and proper names a reader is most
                # likely to look up, and the case is the only thing carrying
                # the consonant. Measured before changing: preserving case
                # splits ZERO existing headword keys, so this is a pure
                # correction and not a re-grouping.
                tn = " ".join(t.split())
                # BATTERY: letter-exact against the spine, per course
                if letters(t) not in letters(segs[seg][0]):
                    sys.exit("REFUSED: tib %r not in %s:%d wylie"
                             % (t, course, seg))
                for frag in eng[l]:
                    if letters(frag) and \
                            letters(frag) not in letters(segs[seg][1]):
                        sys.exit("REFUSED: eng %r not in %s:%d english"
                                 % (frag, course, seg))
                # ACIP is the join key (Adam, 2026-08-28). Recovery
                # is self-proving: acip_span returns None unless the
                # candidate round-trips back to this very span.
                ac = acip_span(segs[seg][2], segs[seg][0], t)
                if ac:
                    acip_forms.setdefault(tn, set()).add(ac)
                else:
                    acip_miss.append("%s:%d %r" % (course, seg, t))
                pairs.setdefault(tn, {}).setdefault(e, set()).add(
                    "%s:%d" % (course, seg))
    con.close()

    # BATTERY (Adam 2026-08-28): ACIP is the source of record, so a
    # headword we cannot prove an ACIP form for is not evidence. Every
    # recovery already round-tripped inside acip_span; here we check
    # coverage and uniqueness.
    ambiguous = {k: sorted(v) for k, v in acip_forms.items()
                 if len(v) > 1}
    if ambiguous:
        for k, v in sorted(ambiguous.items())[:10]:
            print("AMBIGUOUS %r -> %r" % (k, v))
        sys.exit("REFUSED: %d headwords recovered more than one ACIP "
                 "form; the wylie->ACIP map must be 1:1"
                 % len(ambiguous))
    n_head = len(pairs)
    n_acip = len(acip_forms)
    if n_head and n_acip * 100 < n_head * 95:
        for m in acip_miss[:10]:
            print("NO-ACIP " + m)
        sys.exit("REFUSED: ACIP recovered for only %d/%d headwords "
                 "(%.1f%%); the recovery is broken, not the corpus odd"
                 % (n_acip, n_head, 100.0 * n_acip / max(n_head, 1)))

    n_pairs = sum(len(v) for v in pairs.values())
    if n_pairs < FLOOR:
        sys.exit("REFUSED: only %d pairs (< floor %d) — the parse is "
                 "broken, not the corpus small" % (n_pairs, FLOOR))

    def refkey(r):
        c, n = r.split(":")
        return (c, int(n))

    doc = {
        "meta": {
            "layer": "alignment-evidence",
            "tier": "TENTATIVE (machine-matched, unreviewed)",
            "generator": "Claude (AI), full-depth alignment",
            "date": str(date.today()),
            "source_pages": "banked pages, %d course(s) (sha256 %s)"
                            % (len(COURSES), sha.hexdigest()[:16]),
            "source_corpus": "hgm_spine_v27_2 corpus_segments "
                             + "+".join(sorted(COURSES)),
            "rule": "English is HGM's corpus text verbatim, machine-"
                    "MATCHED never composed; never enters hgm_gloss",
            "transliteration": "ACIP is the source of record and the "
                               "join key (acip / acip_index); wylie is "
                               "the derived display form, proven equal "
                               "to acip_to_ewts(acip) on 8961/8961 "
                               "segments of C01-C18 (Adam 2026-08-28)",
            "limit": "these gates prove the text is VERBATIM, never "
                     "that the correspondence is CORRECT; a wrong "
                     "pairing passes every one of them (risk R10)",
        },
        "pairs": {
            tn: [{"eng": e, "refs": sorted(ss, key=refkey),
                  "n": len(ss),
                  "acip": (sorted(acip_forms[tn])[0]
                           if tn in acip_forms else None)}
                 for e, ss in sorted(evs.items())]
            for tn, evs in sorted(pairs.items())
        },
        # ACIP -> wylie headword. ACIP is the source of record and the
        # join key for ACIP-native data; wylie stays the display form.
        "acip_index": {sorted(v)[0]: k
                       for k, v in sorted(acip_forms.items())},
    }
    os.makedirs(OUTDIR, exist_ok=True)
    tmpf = OUT_FULL + ".staging"
    with io.open(tmpf, "w", encoding="utf-8") as f:
        json.dump({"meta": {"what": "the COMPLETE seven-layer bank, "
                            "all courses: every span at every depth "
                            "incl. one-sided (no-exponent) records, "
                            "Wilson case flags, AI notes, tree blocks",
                            "consumer": "the Overlay grammar view; "
                            "the review workflow"},
                   "links": full["links"], "notes": full["notes"],
                   "trees": full["trees"]},
                  f, ensure_ascii=False, indent=None,
                  separators=(",", ":"))
    os.replace(tmpf, OUT_FULL)
    print("full bank: %d links (%d with no English exponent), "
          "%d note blocks, %d tree blocks"
          % (len(full["links"]),
             sum(1 for L in full["links"] if not L["eng"]),
             sum(len(v) for v in full["notes"].values()),
             sum(len(v) for v in full["trees"].values())))
    print("ACIP: %d/%d headwords carry a proven ACIP form (%.1f%%)"
          % (n_acip, n_head, 100.0 * n_acip / max(n_head, 1)))
    if acip_miss:
        print("      %d span(s) without a provable ACIP form"
              % len(acip_miss))
    tmp = OUT + ".staging"
    with io.open(tmp, "w", encoding="utf-8") as f:
        json.dump(doc, f, ensure_ascii=False, indent=1)
    os.replace(tmp, OUT)
    print("alignment layer: %d headwords, %d pairs -> %s"
          % (len(pairs), n_pairs, os.path.relpath(OUT, ROOT)))


if __name__ == "__main__":
    main()
