#!/usr/bin/env python3
"""Regression tests for source-justified display shortening (no generated files)."""
import unittest

from build_dictionary_view import trim_glued
from test_view_matches_layer import CEILING, classify


BODY = 'There is no place to go, no one to help, nothing to protect you.'
HEADING = (' 73 The Asian Classics Institute Course V: How Karma Works'
           ' Reading Five: How Karma is Carried')


def case(eng, source=BODY + HEADING, refs=None, source_depth=1, band='1'):
    refs = ['C05:206'] if refs is None else refs
    full = {'links': [{'tib': 'log lta', 'eng': source, 'course': 'C05',
                       'seg': 206, 'd': source_depth}]}
    row = ['log lta', '', [[eng, refs]]]
    view = {'depths': {} if band == 'phon' else {band: [row]},
            'phonetics': [row] if band == 'phon' else []}
    return classify(view, full, {'pairs': {}})[0]


class HeadingTrimmingTests(unittest.TestCase):
    def test_numbered_course_heading_removed_from_sentence_boundary(self):
        self.assertEqual(trim_glued(BODY + HEADING), (BODY, True))

    def test_ordinary_mid_sentence_title_case_preserved(self):
        for source in ['We learn from As Master Shantideva explains the path.',
                       'We examined 73 The Asian Classics Institute Course V: How Karma Works references.']:
            with self.subTest(source=source):
                self.assertEqual(trim_glued(source), (source, False))

    def test_numbered_course_without_reading_witness_preserves_legacy_text(self):
        for suffix in ['', ' Readings Five and Six: Meditation',
                       ' Reading 5A: Omniscience', ' Reading 1B: Introduction']:
            source = BODY + ' 73 The Asian Classics Institute Course V: How Karma Works' + suffix
            with self.subTest(suffix=suffix):
                self.assertEqual(trim_glued(source), (source, False))

    def test_heading_only_is_not_emptied(self):
        source = '73 The Asian Classics Institute Course V: How Karma Works'
        self.assertEqual(trim_glued(source), (source, False))

    def test_existing_reading_trim_still_permitted(self):
        source = BODY + ' Reading Five: A heading'
        # Preserve the existing Reading-rule boundary, including its punctuation.
        self.assertEqual(trim_glued(source), (BODY[:-1], True))
        self.assertEqual(case(BODY[:-1], source)['trimmed (trim_glued)'], 1)

    def test_exact_numbered_trim_accepted_with_source_provenance(self):
        self.assertEqual(case(BODY)['trimmed (trim_glued)'], 1)

    def test_arbitrary_empty_shorter_extended_and_invented_outputs_rejected(self):
        for eng in ['', 'There', BODY[:-1], BODY + ' 73', BODY + ' invented']:
            with self.subTest(eng=eng):
                self.assertEqual(case(eng)['INVENTED-BY-THE-VIEW'], 1)

    def test_trim_requires_every_citation_and_matching_depth(self):
        for refs in [[], ['C05:999'], ['C05:206', 'C05:999']]:
            with self.subTest(refs=refs):
                self.assertEqual(case(BODY, refs=refs)['INVENTED-BY-THE-VIEW'], 1)
        self.assertEqual(case(BODY, band='2')['INVENTED-BY-THE-VIEW'], 1)

    def test_phonetic_trim_is_exact_and_held_in_its_band(self):
        tib = "'gro la phan phyir sangs rgyas 'grub par shog,"
        short = 'drola penchir sangye druppar shok.'
        source = short + ' Refuge and The Wish I go for refuge To the Buddha, Dharma, and Sangha Until I achieve enlightenment.'
        full = {'links': [{'tib': tib, 'eng': source, 'course': 'C01', 'seg': 11, 'd': 1}]}
        def counts(eng, band):
            row = [tib, '', [[eng, ['C01:11']]]]
            view = {'depths': {} if band == 'phon' else {band: [row]},
                    'phonetics': [row] if band == 'phon' else []}
            return classify(view, full, {'pairs': {}})[0]
        self.assertEqual(counts(short, 'phon')['trimmed (trim_glued)'], 1)
        self.assertEqual(counts('drola', 'phon')['INVENTED-BY-THE-VIEW'], 1)
        self.assertEqual(counts(short, '1')['INVENTED-BY-THE-VIEW'], 1)

    def test_growth_accepts_only_verified_trims(self):
        links, rows = [], []
        for seq in range(41):
            links.append({'tib': str(seq), 'eng': BODY + HEADING,
                          'course': 'C05', 'seg': seq, 'd': 1})
            rows.append([str(seq), '', [[BODY, ['C05:%d' % seq]]]])
        p = {'depths': {'1': rows}, 'phonetics': []}
        counts = classify(p, {'links': links}, {'pairs': {}})[0]
        self.assertEqual(counts['trimmed (trim_glued)'], 41)
        self.assertFalse(any(counts[k] > cap for k, cap in CEILING.items()))
        rows[-1][2][0][0] = 'There'
        counts = classify(p, {'links': links}, {'pairs': {}})[0]
        self.assertTrue(any(counts[k] > cap for k, cap in CEILING.items()))


if __name__ == '__main__':
    unittest.main()
