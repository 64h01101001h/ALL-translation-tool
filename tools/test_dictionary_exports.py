#!/usr/bin/env python3
"""Course CSV rows must retain the depth of their own contributing refs.

Regression: adding C05's depth-6 rendering relabelled C04's unchanged
depth-5 rendering, and multiple depths in one course were collapsed into
one arbitrarily labelled count. Exercise the real exporter and CSV reader
with small inputs; no bank, spine, or persistent exports are needed.
"""
import contextlib
import csv
import io
from pathlib import Path
import tempfile
import unittest
from unittest.mock import patch

import build_dictionary_exports as exports


META = {"tier": "provisional", "rule": "fixture corpus match",
        "source_corpus": "fixture corpus", "source_pages": "fixture pages",
        "generator": "fixture generator", "date": "2026-09-11"}
COURSE_FILE = "geshe_michael_roach_dictionary_by_course.csv"


def write_csvs(depths, phonetics=()):
    with tempfile.TemporaryDirectory() as tmp:
        # Redirect only the destination. Row assembly, conversions, grouping,
        # counting, sorting, and CSV serialization all run as in production.
        with patch.object(exports, "OUTDIR", tmp), \
                contextlib.redirect_stderr(io.StringIO()):
            exports.write_exports(depths, phonetics, META,
                                  {"YIN PA'I": "yin pa'i", "SEMS": "sems"})
        return {path.name: path.read_bytes() for path in Path(tmp).glob("*.csv")}


def read_csv(raw):
    return list(csv.DictReader(io.StringIO(raw.decode("utf-8-sig"), newline="")))


def course_records(raw):
    return [(r["course"], r["wylie"], r["english"], r["depth"],
             r["occurrences_in_course"]) for r in read_csv(raw)]


class CourseExports(unittest.TestCase):
    def test_another_courses_depth_cannot_relabel_an_earlier_course(self):
        depths = {
            5: [["yin pa'i", "", [["is", ["C04:1", "C04:2"]]]]],
            6: [["yin pa'i", "", [["is", ["C05:178"]]]]],
        }
        self.assertEqual(course_records(write_csvs(depths)[COURSE_FILE]), [
            ("C04", "yin pa'i", "is", "5", "2"),
            ("C05", "yin pa'i", "is", "6", "1"),
        ])

    def test_each_depth_in_one_course_keeps_its_own_occurrence_count(self):
        depths = {
            5: [["yin pa'i", "", [["is", ["C04:1", "C04:2"]]]]],
            6: [["yin pa'i", "", [["is", ["C04:2"]]]]],
        }
        # C04:2 contributes once to each depth; neither row can claim the
        # combined count of three as evidence for just one depth.
        self.assertEqual(course_records(write_csvs(depths)[COURSE_FILE]), [
            ("C04", "yin pa'i", "is", "5", "2"),
            ("C04", "yin pa'i", "is", "6", "1"),
        ])

    def test_depth_metadata_is_independent_of_input_depth_order(self):
        depths = {
            5: [["yin pa'i", "", [["is", ["C04:1", "C05:1"]]]]],
            6: [["yin pa'i", "", [["is", ["C04:2"]]]]],
        }
        forward = read_csv(write_csvs(depths)[COURSE_FILE])
        backward = read_csv(write_csvs(dict(reversed(list(depths.items()))))[COURSE_FILE])
        self.assertCountEqual(forward, backward)
        self.assertEqual(len(forward), 3)

    def test_ranking_uses_each_course_depth_count_and_excludes_phonetics(self):
        depths = {
            5: [["sems", "", [["mind", ["C04:1", "C04:2",
                                          "C05:1", "C05:2", "C05:3"]]]],
                ["yin pa'i", "", [["is", ["C04:1", "C04:2", "C04:3",
                                            "C05:1"]]]]],
            6: [["sems", "", [["mind", ["C04:3", "C04:4"]]]]],
        }
        phonetics = [["sems", "", [["sem", ["C04:1", "C04:2", "C04:3", "C04:4"]]]]]
        csvs = write_csvs(depths, phonetics)
        records = course_records(csvs[COURSE_FILE])
        self.assertEqual(records[0], ("C04", "yin pa'i", "is", "5", "3"))
        self.assertCountEqual(records[1:3], [
            ("C04", "sems", "mind", "5", "2"),
            ("C04", "sems", "mind", "6", "2"),
        ])
        self.assertEqual(records[3:], [
            ("C05", "sems", "mind", "5", "3"),
            ("C05", "yin pa'i", "is", "5", "1"),
        ])
        main_rows = read_csv(csvs["geshe_michael_roach_dictionary.csv"])
        self.assertEqual([(r["english"], r["kind"]) for r in main_rows
                          if r["english"] == "sem"],
                         [("sem", "phonetic transcription")])

    def test_course_csv_preserves_schema_text_and_excel_encoding(self):
        depths = {5: [["sems", "", [['mind, "awareness"', ["C04:1"]]]]]}
        raw = write_csvs(depths)[COURSE_FILE]
        self.assertTrue(raw.startswith(b"\xef\xbb\xbf"))
        self.assertEqual(raw.count(b"\r\n"), 2)
        self.assertNotIn(b"\n", raw.replace(b"\r\n", b""))
        rows = read_csv(raw)
        self.assertEqual(list(rows[0]), [
            "course", "wylie", "acip", "tibetan_generated",
            "pronunciation_generated", "english", "occurrences_in_course",
            "depth", "tier",
        ])
        self.assertEqual(rows[0]["english"], 'mind, "awareness"')
        self.assertEqual(rows[0]["acip"], "SEMS")
        self.assertEqual(rows[0]["tier"], "provisional")


if __name__ == "__main__":
    unittest.main()
