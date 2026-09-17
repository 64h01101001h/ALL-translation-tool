export const meta = {
  name: 'c05-align',
  description: 'C05 full-depth alignment: two angles per segment, reconcile, generator must exit 0',
  phases: [
    { title: 'Propose', detail: 'two independent analysts per segment', model: 'opus' },
    { title: 'Reconcile', detail: 'merge into one spec the generator accepts', model: 'opus' },
  ],
}

// args: { seqs: [172,173,...], note?: "page-specific known facts" }
const SEQS = (args && args.seqs) || []
const EXTRA = (args && args.note) || ''
if (!SEQS.length) throw new Error('pass args.seqs, e.g. {"seqs":[172,173,174]}')

const ROOT = '/Users/adamderickandrade/ALL-translation-tool'
// args.scratch, because the first copy of this script hardcoded ONE session's
// scratch directory and so could only ever run in the session that wrote it.
// Pass the session scratchpad; the fallback is repo-local and gitignored.
const SCRATCH = (args && args.scratch) || (ROOT + '/build/align_scratch/c05')

const COMMON = [
  'You are working on the ACI Course 5 full-depth alignment campaign in ' + ROOT + '.',
  'Course 5 is "How Karma Works". Segments 24-511 are Vasubandhu\'s Treasure House of',
  'Knowledge (Abhidharmakosha) chapter IV on karma, with the First Dalai Lama Gendun',
  'Drup\'s commentary. The campaign resumed 2026-09-17 at its paused point.',
  '',
  'READ THESE FIRST, they govern everything:',
  '  ' + ROOT + '/docs/alignment_briefs/PROPOSE_BRIEF.md   (all 117 lines)',
  '  ' + ROOT + '/data/alignment/C05_CAMPAIGN.md           ("Course shape" section)',
  '',
  'GET THE SOURCE TEXT YOURSELF, never from this prompt:',
  '  sqlite3 ' + ROOT + '/build/hgm_spine_v27_2.db \\',
  '    "select wylie, english, acip from corpus_segments where course=\'C05\' and seq=<SEQ>"',
  '',
  'THE TEN RULES ARE INVIOLABLE AND THE GENERATOR ENFORCES THEM. The brief is the',
  'authority; in summary:',
  ' 1. VERBATIM - every "tib" a contiguous substring of the wylie, every "eng" a',
  '    contiguous substring of the English, byte-for-byte INCLUDING ERRORS.',
  ' 2. TIBETAN ORDER - a forward-only cursor over every span of depth 1-6.',
  ' 3. DEPTH - d=5 WORD or COMPOUND (the dictionary layer, where the value is),',
  '    d=6 STANDALONE particle that STARTS A SYLLABLE, d=7 BOUND morpheme or',
  '    COMPOUND MEMBER which MUST be immediately preceded by the d=5 it lives in.',
  '    PREFER THE FLAT SHAPE: d=5 and d=6 at top level in wylie order, d=7 inside',
  '    its compound, no clause layer unless the clause is genuinely what is aligned.',
  ' 4. A member must be UNIQUE inside its parent, TIGHTER than it, and its English',
  '    must lie INSIDE the parent\'s English.',
  ' 5. NO INVENTED ENGLISH - a morpheme with no exponent gets "eng": null.',
  ' 6. AMBIGUOUS ENGLISH - equal counts resolve, unequal do not. Supply "eng_order"',
  '    (span ids in ENGLISH order) whenever the English crosses.',
  ' 7. NO OVER-CAPTURE - the dominant measured defect. Leftover English stays',
  '    UNWRAPPED. An article or hinge Geshe Michael supplied has no Tibetan under it.',
  ' 8. NO BROKEN WORDS, except the four negative affixes (n\'t, un, im, less) with',
  '    "subword": true.',
  ' 9. AGENTIVE -S IS NOT SEPARABLE - span "gis", "kyis", "pas", "\'dis" whole.',
  '10. NEVER BANK A FALSEHOOD, EVEN A VERBATIM ONE. Rule 10 outranks coverage.',
  '    Losing a mapping is cheap; banking one you cannot point at is not. A span',
  '    that passes every mechanical check can still be false.',
  '',
  'Prose of this length takes 25-60 spans. Cover the content words; null or omit',
  'particles freely - an omitted particle costs nothing, a mis-depthed one costs the page.',
  '',
  'Geshe Michael\'s English stays VERBATIM everywhere - quote it as it stands, never',
  'corrected. Where it is wrong, that is an erratum to report, never a licence to',
  'distort a span.',
  EXTRA,
].join('\n')

const SPEC_SCHEMA = {
  type: 'object',
  properties: {
    seq: { type: 'number' }, angle: { type: 'string' }, title: { type: 'string' },
    spans: { type: 'array', items: { type: 'object', properties: {
      id: { type: 'string' }, d: { type: 'number' }, tib: { type: 'string' },
      eng: { type: ['string', 'null'] }, subword: { type: 'boolean' },
      cls: { type: 'string' }, nul: { type: 'string' } },
      required: ['id', 'd', 'tib'] } },
    eng_order: { type: 'array', items: { type: 'string' } },
    note: { type: 'string' },
    errata: { type: 'array', items: { type: 'string' } },
    reasoning: { type: 'string' },
  },
  required: ['seq', 'angle', 'title', 'spans', 'reasoning'],
}

const ANGLES = [
  { key: 'tibetan-first', how:
    'YOUR ANGLE IS TIBETAN-FIRST. Walk the WYLIE from its first syllable to its last.\n' +
    'For each Tibetan word or particle decide what, if anything, in the English renders\n' +
    'it. Let the Tibetan set the inventory: if a Tibetan word has no English exponent\n' +
    'that is a null, not a reason to stretch a nearby phrase. You will naturally produce\n' +
    'good d=5/d=7 structure and may under-read the English.' },
  { key: 'english-first', how:
    'YOUR ANGLE IS ENGLISH-FIRST. Walk Geshe Michael\'s ENGLISH from its first word to\n' +
    'its last. For each content word or phrase decide which Tibetan it renders. Where he\n' +
    'has supplied a hinge, an article or a connective with no Tibetan under it, LEAVE IT\n' +
    'UNWRAPPED - that is rule 7 and it is the dominant measured defect. You will catch\n' +
    'English the Tibetan-first analyst misses, and may over-reach; do not.' },
]

phase('Propose')
log(SEQS.length + ' segments, ' + (SEQS.length * 3) + ' agents')

const proposals = await parallel(
  SEQS.flatMap(seq => ANGLES.map(a => () =>
    agent(COMMON + '\n\nYOUR TASK: propose the full-depth alignment for C05 segment ' + seq + '.\n\n' +
      a.how + '\n\nFetch the segment yourself. Read the brief in full before you start.\n' +
      'Work in ' + SCRATCH + '/' + seq + '-' + a.key + '/ if you need scratch files;\n' +
      'touch nothing else in the repository.\n\n' +
      'Return the spec as structured output. In "reasoning" say what you were UNSURE\n' +
      'about and where the other angle is likely to disagree with you - that honesty is\n' +
      'what the reconciler uses.',
      { label: 'propose:' + seq + ':' + a.key, phase: 'Propose', schema: SPEC_SCHEMA,
        model: 'opus', effort: 'high' })
      .then(r => ({ seq, angle: a.key, spec: r }))
  ))
)

const bySeq = {}
for (const p of proposals.filter(Boolean)) (bySeq[p.seq] = bySeq[p.seq] || []).push(p)

phase('Reconcile')

const reconciled = await parallel(SEQS.map(seq => () => {
  const pair = bySeq[seq] || []
  if (pair.length < 2) return Promise.resolve({ seq, failed: 'fewer than two proposals' })
  return agent(COMMON +
    '\n\nYOUR TASK: reconcile the two independent proposals for C05 segment ' + seq +
    ' into ONE spec, and PROVE IT by making the generator accept it.\n\n' +
    'PROPOSAL A (' + pair[0].angle + '):\n' + JSON.stringify(pair[0].spec, null, 1) + '\n\n' +
    'PROPOSAL B (' + pair[1].angle + '):\n' + JSON.stringify(pair[1].spec, null, 1) + '\n\n' +
    'Read ' + ROOT + '/docs/alignment_briefs/RECONCILE_BRIEF.md in full first.\n\n' +
    'YOUR DELIVERABLE IS A SPEC FILE THAT EXITED 0. Write your merged spec to\n' +
    SCRATCH + '/' + seq + '-reconciled.json in the format gen_alignment_page.py reads:\n\n' +
    '  { "course": "C05", "segments": [ { "seq": ' + seq + ', "title": "...",\n' +
    '      "spans": [...], "eng_order": [...], "note": "..." } ] }\n\n' +
    'then RUN IT and keep fixing until it exits 0:\n\n' +
    '  cd ' + ROOT + ' && python3 tools/gen_alignment_page.py \\\n' +
    '      < ' + SCRATCH + '/' + seq + '-reconciled.json > ' + SCRATCH + '/' + seq + '-body.html\n' +
    '  echo "exit=$?"\n\n' +
    'The generator REFUSES on any rule violation and names the span. A refusal is\n' +
    'information, not an obstacle - fix the span, do not delete the rule. If a span\n' +
    'cannot be made to pass honestly, DROP IT: rule 10 outranks coverage.\n\n' +
    'SETTLE DISAGREEMENTS AGAINST THE LANDED CORPUS, not by taste: count the existing\n' +
    'links for a contested Tibetan string in ' + ROOT + '/data/alignment/alignment_full_v1.json\n' +
    'and let precedent decide. Say what you counted. Do not trust any total
quoted to you - including by this prompt - count the file yourself; the bank
grows every page and a stale total is how a wrong precedent gets justified.\n\n' +
    'ALSO FLAG, for the lander: any d=5 or d=7 span whose English begins with\n' +
    'the/a/an/and/or/his/our/your/I/you - it will trip the no_supplied_span_head gate\n' +
    'and needs a licensor in data/alignment/span_head_allow.json naming the Tibetan\n' +
    'word that owns the function word.\n\n' +
    'Return: seq, angle "reconciled", final title, final spans, eng_order, note,\n' +
    'errata, and in "reasoning" - the exit code you achieved, the span count, what the\n' +
    'analysts disagreed about and how you settled it, what you DROPPED rather than\n' +
    'bank, and any span-head licensor the lander must add.',
    { label: 'reconcile:' + seq, phase: 'Reconcile', schema: SPEC_SCHEMA,
      model: 'opus', effort: 'high' })
    .then(r => ({ seq, spec: r }))
}))

return {
  scratch: SCRATCH,
  proposals: proposals.filter(Boolean).map(p => ({ seq: p.seq, angle: p.angle, spans: (p.spec && p.spec.spans || []).length })),
  reconciled: reconciled.filter(Boolean).map(r => ({
    seq: r.seq, spans: (r.spec && r.spec.spans || []).length,
    title: r.spec && r.spec.title, errata: (r.spec && r.spec.errata) || [],
    reasoning: r.spec && r.spec.reasoning })),
}
