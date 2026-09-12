#!/usr/bin/env python3
"""Prove the page gates inspect C05 and later courses, and refuse empty input.

Regression: four gates hardcoded directories through C04 and reported clean
results while all 55 C05 pages went unread. These fixtures put real malformed
spans in both C05 and a future course. Restoring the old directory lists makes
the corresponding rejection checks fail; this is behavior, not a source census.
"""
import contextlib
import importlib.util
import io
from pathlib import Path
import tempfile
import unittest


TOOLS = Path(__file__).resolve().parent
GATES = {
    "no_broken_words": (
        '<div class="eng"><span class="u" data-d="5" data-l="s1w1">rip</span>ening</div>',
        "BROKEN WORDS",
    ),
    "no_split_syllables": (
        '<div class="tib"><span class="u" data-d="7" data-l="s1w1">se</span>ms</div>',
        "SPLIT SYLLABLES",
    ),
    "no_degenerate_members": (
        '<span class="u" data-d="5" data-l="s1w1"><span class="u" data-d="7" data-l="s1m1">word</span></span>',
        "DEGENERATE MEMBERS",
    ),
    "builder_sees_every_span": (
        '<span class="u" data-d="5" data-l="unseen:1">word</span>',
        "FAIL the builder does not see everything",
    ),
}
VALID = ('<div class="tib"><span class="u" data-d="5" data-l="s1w1">sems</span></div>'
         '<div class="eng"><span class="u" data-d="5" data-l="s1w1">mind</span></div>')


def run_gate(name, pages):
    spec = importlib.util.spec_from_file_location(name, TOOLS / ("test_" + name + ".py"))
    gate = importlib.util.module_from_spec(spec)
    spec.loader.exec_module(gate)
    with tempfile.TemporaryDirectory() as tmp:
        gate.ROOT = tmp
        (Path(tmp) / "data/alignment").mkdir(parents=True)
        for course_dir, body in pages.items():
            folder = Path(tmp) / "data/alignment" / course_dir
            folder.mkdir()
            (folder / "fixture.html").write_text(body, encoding="utf-8")
        output = io.StringIO()
        with contextlib.redirect_stdout(output):
            try:
                code = gate.main()
            except SystemExit as exc:
                code = exc.code
        return code or 0, output.getvalue()


class GateCoverage(unittest.TestCase):
    def test_malformed_spans_in_c05_and_future_courses_are_rejected(self):
        for name, (malformed, diagnostic) in GATES.items():
            for course in ("pages_c05", "pages_c06"):
                with self.subTest(gate=name, course=course):
                    code, output = run_gate(name, {"pages_c01": VALID, course: malformed})
                    self.assertNotEqual(code, 0, output)
                    self.assertIn(diagnostic, output)

    def test_valid_c05_and_future_pages_are_actually_counted(self):
        for name in GATES:
            with self.subTest(gate=name):
                code, output = run_gate(name, {"pages_c05": VALID, "pages_c06": VALID})
                self.assertEqual(code, 0, output)
                self.assertIn("2 pages", output)

    def test_absent_pages_cannot_report_a_clean_scan(self):
        for name in GATES:
            with self.subTest(gate=name):
                code, output = run_gate(name, {})
                self.assertNotEqual(code, 0, output)
                self.assertIn("no alignment pages", output.lower())


if __name__ == "__main__":
    unittest.main()
