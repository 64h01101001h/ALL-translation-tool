import collections,datetime,hashlib,json,sys
from pathlib import Path
assert __debug__ and sys.dont_write_bytecode
P=Path(__file__).resolve().parent;B=P.parent;F=P/'reviewed-final'
def dump(name,v):
 p=P/name;assert not p.exists(),p;p.write_text(json.dumps(v,ensure_ascii=False,indent=2)+'\n')
def pin(p):
 b=p.read_bytes();return {'path':str(p),'bytes':len(b),'sha256':hashlib.sha256(b).hexdigest()}
span_reasons={283:'''The full objector-response formula supplies this complete short speech phrase; d3 avoids inventing a dictionary compound.
The defect/problem noun occurs within the denied problem, independently of the following negation.
The selected lowercase no expresses med's denial; the opening capital No is a separate discourse realization.
The anatomical compound names the protruding parts discussed in the preceding analogy; protuberances is the attested local noun.
This is the explicit five modifying protuberances, not the anaphorically supplied later five.
The first person is the whole whose parts are being identified; the surrounding possessive words remain outside the noun.
The established compound names those parts, without absorbing all or the genitive relation.
This are is the parts-equation copula, not the progressive auxiliary in the following action.
The causal pas attached to being parts supports the consequence so; the later action-premise pas has a different syntactic attachment.
The first actions belongs to the parts' action premise and has its own source noun.
This performing belongs to the parts, at the middle byas occurrence rather than the opening discourse byas.
The second person is the actor to whom the parts' action is attributed.
The final actions is the action attributed to that whole person, distinct from the earlier action noun occurrence.
The final performing is predicated of the whole person; the preceding is remains an English auxiliary.
The attribution verb can be rendered say here, where the argument establishes what one must say about the person's action.
Necessity expressed by have to belongs to dgos; the inclusive speaker we remains outside it.'''.splitlines(),284:'''The earlier-case reference supports above as a narrow lexical realization of snga ma.
The complete equality idiom exactly the same is positively attested for mtshungs and expresses this case's equivalence to the preceding analogy; it does not add another referent.
Pus-and-blood is one established substance compound in the three-kind argument; its English components occur in reversed order.
Pus is the rnag member, uniquely inside the parent despite its later English position.
Blood is the khrag member, uniquely inside the parent at the earlier English position.
Sogs carries the continuing-list gesture and the other, including its connective. The separately inferred remainder two is excluded.
The noun things refers to the three listed appearances, excluding mentioned.
The first three counts the things in the opening enumeration.
The first wet describes the glass contents in the first description.
This and is the first adjective coordination licensed by zhing, not a substance/list conjunction.
The complete gsher ba adjective has the locally attested flowing realization.
The first glass is the container whose contents are described in the first clause.
Gang po is an attested complete fullness adjective; full does not absorb the following content phrase.
The compound parts is the predicate noun identifying the relation of the three appearances to the shared glass.
The selected are is the explicit yin copula of the parts claim.
The middle three counts the appearances inside the following conditional clause.
Snang ba is a finite appearance predicate in this clause; appear is supported by that syntax as well as the lexical family.
Na introduces the appearance condition, represented by When; it does not independently own the later then.
The later wet belongs to the repeated description of the glass, not the first description.
The later zhing coordinates the second wet/flowing pair.
The second flowing is the second complete gsher ba adjective occurrence.
The second glass belongs to the subsequent locus-of-appearance clause.
The second gang po is the full adjective attached to that second glass description.
The complete locative steng du directly supplies upon in the display relation. Acting as a basis elaborates the same relation elsewhere; banking upon once is defensible.
The established manner-of-appearance compound supplies ways of appearing, distinct from the final display predicate.
Mi 'dra ba is a complete lexical different unit; extracting a negative English affix here would be false.
The final three counts ways, distinct from the earlier two numeral occurrences.
The final appearance verb supplies displayed; passive/progressive auxiliaries are excluded.
The attribution verb is the sole say in the clause, with speaker and modal framing left outside.'''.splitlines(),285:'''The initial discourse renewal yang supports yet another modifying the argument; else is not counted as a second mapping.
The indefinite objector expression supplies Someone while the expansive speaking frame remains separate.
Pus and blood is the established paired substance compound, with English components now in the same order as Tibetan.
The first compound constituent rnag is the unique pus member.
The second compound constituent khrag is the unique blood member.
The continuing-list expression sogs licenses and the other; the inferred count two remains outside it.
The initial dngos po names the three things in the topic, not either later objects occurrence.
The first numeral counts those initial three things.
The reciprocal unit supplies each other as the object of block; the later each one is explanatory restatement.
The occupied-space noun supports space. The longer relative clause explains that location without creating a second source noun.
The complete blocking predicate supplies block inside the questioned object classification, without asserting the entire proposition positively.
The second source object noun supplies the first English objects, inside the negative classification.
The overt ma negates that classification; the selected not is neither an inferred denial nor a question particle.
The lower-case subordinate are is the source copula paired with ma; initial capital Are introduces the surrounding question.
The karma noun remains a useful independent head. The broader attested las can phrase is also valid evidence, but does not force individualized pronouns into this smaller noun.
The possessive particle can supplies with. Its relation to las is preserved by adjacent flat noun/particle spans, without requiring a d7 child lacking a parent.
The living-being noun denotes the three perceivers; beings is a valid local reduction of living beings.
The middle numeral counts the perceiving beings, not objects.
The actual water noun supplies water here; this does not import water into the less specific wet/flowing wording of284.
The container noun supplies this sole glass occurrence.
The actual fullness form is gang without po in this segment; full is its local adjectival sense.
This la marks the object of looking and supplies at, excluding both the enumeration la and the final clause-linking la.
The established simultaneous/together adverb includes du. Together captures the shared looking without assigning an independent sitting action to it; wider glossary variants remain acknowledged.
The complete look form bltas pa supplies look, while the separately overt la supplies at.
The last source object noun supplies the final English objects, described as validly established.
The established tshad grub pa compound has a complete attested confirmed by a valid perception exponent. This preserves the epistemic relation lost by the clipped auto-aligned alternative.
The abbreviated valid-perception member tshad is independently attested and uniquely occupies the later English part of the compound.
The complete grub pa member supplies confirmed at the beginning of the English compound, crossing its sibling while remaining contained.
The final numeral counts the final objects, excluding the contextually supplied different.
Possibility, rather than a rebirth sense, is required by this accepted-premise clause and supports possible.
The admitted/asserted stance can be nominalized as position in this sentence. Selecting it once is supported by both lexical variants and source argument structure; the repeated report said remains outside.'''.splitlines()}
resolved=json.loads((P/'resolved-spans.json').read_bytes());span_reviews=[]
for seq in [283,284,285]:
 tuples=[r for r in resolved if r['seq']==seq];assert len(tuples)==len(span_reasons[seq])
 for t,reason in zip(tuples,span_reasons[seq]):span_reviews.append({'span':t,'verdict':'SOUND','reviewer_reason':reason,'evidence':{'full_source':str(P/'source-reopened.json'),'complete_lexical_and_corpus_replays':str(P/'query-replay-index.json'),'raw_archive_comparisons':str(P/'archive-reopened-checks.json')}})
dump('span-review.json',span_reviews)
omission_reasons={283:'''The lead-in refers to the preceding argument rather than a new performing action; omission avoids reusing a later lexical exponent.
This connective's explanatory function is carried by the full reply structure; no extra isolated English word is required.
The completed numeral plus demonstrative cannot supply a second lexical five without treating anaphora as another numeral.
The demonstrative-genitive relation is expressed across the reordered noun phrase; leaving it unbanked avoids a forced standalone bound suffix.
The agentive anaphor identifies the already mentioned parts; five is a referential expansion, not its lexical meaning.
The action-premise connective differs from the causal link after yin; withholding a separate mapping avoids duplicating so.
The person-agent relation is expressed by English subject position, not by the progressive auxiliary.
This complement linkage is distributed through English predicate structure, with no necessary isolated particle exponent.
The reason and hypothetical quotation close the whole argument; no second responds or so is available.
The complete preceding-argument frame may remain unbanked; it is not another dictionary action unit.
Opening denial and existential support surround the selected lower-case no; not duplicating med is sound.
Such is idiomatic specification of the denied problem; it is not part of the selected problem noun.
The localizing adverb and article do not enlarge either adjacent lexical head.
The totalizing presentation is not a separate component of cha shas.
The whole-person/genitive relation is wider than the person noun, and one is not an additional source gcig.
The inclusive English speaker has no separate Tibetan participant word here.
The embedded premise and resumed parts are expanded grammatically; no extra lnga or copula is asserted.
This indefinite quantity has no separate Tibetan quantifier.
The article belongs to English noun presentation rather than skyes bu's lexical exponent.
The progressive auxiliary is not a repeated source yin.
The second indefinite quantity likewise has no independent quantifier.'''.splitlines(),284:'''The resumed comparison target is represented by the equivalence sentence as a whole; it does not require inflating mtshungs.
The explanatory connective survives in clause structure without a separately necessary lexical span.
The list construction has its continuing-list exponent in sogs; an extra la-to-word mapping would duplicate that construction.
The bound continuation participates in nominal syntax rather than a standalone English dictionary unit.
The resumed numeral reference does not introduce another count.
The genitive relates parts to the earlier glass phrase; it need not become an independent bound-suffix mapping without a parent.
The reason and resumed referent are reorganized across sentences; the selected When already has na as its direct source.
The glass reference is resumed by it later; this does not warrant a second glass mapping.
The complement ending supports the English attribution construction without adding a separate content head.
The closing reason has no explicit because to bank and need not be falsely nulled.
Well and case introduce the comparison around the lexical above.
This English copular support is not another overt source yin.
Punctuation and article presentation are properly excluded from three.
The resumed-list apparatus does not enlarge things or blood-and-pus.
The inferred remainder of three kinds is defensible translation but not a separate source two.
Totalizing all is outside the parts compound.
The genitive/article phrase is outside parts and glass lexical heads.
The content introducer does not enlarge wet, flowing or full.
The article does not belong inside the numeral's exponent.
Conditional consequence and speaker/modality are distributed outside the directly selected When and say.
The clause introducer and article do not enlarge say or glass.
The relative content description supplies English structure around directly mapped adjectives.
The attested basis paraphrase repeats the locative relation also expressed by upon; selecting one realization avoids duplicated ownership.
Passive/progressive support is outside displayed.
The final pronoun resumes the glass rather than introducing a new dictionary noun.'''.splitlines(),285:'''The reporting formula is expanded around the selected indefinite subject and discourse particle; no contiguous narrow speaking phrase is forced.
This la belongs to the list construction and is not the at of looking.
The bound enumeration continuation contributes nominal structure without a separate English head.
The demonstrative resumes the three objects, not a second numeral or object noun.
The bound attributive relation is carried by English clause syntax; no orphan particle span is created.
The consequence question surrounds the negative proposition; withholding its expanded frame preserves the distinct overt ma/yin pair.
The possessing modifier is realized around with their own; assigning with once to can keeps the relations distinct.
The grammatical relation of the beings to looking is expressed by English subject syntax.
The temporal/situation frame is blended with reporting language; leaving it unbanked is a defensible conservative choice.
Complement structure connects the possibility statement to admission without a new isolated English lexical item.
The nominalized premise continues into286; treating this connective as an isolated null would overstate its independence.
The extra new-speaker narrative does not need a second yang mapping.
The topic and reporting frame are broader than the selected words, and can remain unbanked.
The article and punctuation are outside the substance compound.
The explanatory remainder and consequence question are not extra source content nouns or copulas.
The classification language is outside the object noun's lexical exponent.
The relative/modal support does not enlarge the blocking predicate.
Entering spells out the obstruction relation without a separately occurring Tibetan motion verb.
The occupancy elaboration and recalled situation are distributed around the retained spatial and premise words.
The classificatory phrase is outside the beings noun.
The individualizing each has no standalone source quantifier here.
The broader las can equivalent with their own karma is acknowledged; the narrower selected noun/particle decomposition is also semantically sound.
The wider seated-staging glossary variant remains valid evidence; this analysis banks the narrower shared-time adverb without an invented sitting verb.
This conjunction links English staged actions and is not a separately occurring source dang.
The indefinite article stays outside glass.
The contents relation stays outside water/full lexical heads.
The reported-admission frame repeats the assertion conveyed by position; the source is deliberately banked once.
English tense and complement support do not supply another source copula.
The existential complement structure surrounds possibility and objects without another explicit existence word.
Context distinguishes the three objects, but no extra mi 'dra ba occurs here.
The locating and individualizing explanation does not introduce extra source numerals or pronouns.'''.splitlines()}
omission_reviews=[]
for seq in [283,284,285]:
 rows=json.loads((F/str(seq)/'omissions.json').read_bytes())['material'];assert len(rows)==len(omission_reasons[seq]),(seq,len(rows),len(omission_reasons[seq]))
 for row,reason in zip(rows,omission_reasons[seq]):omission_reviews.append({'seq':seq,'side':row['side'],'start':row['start'],'end':row['end'],'text':row['text'],'verdict':'OMISSION_DEFENSIBLE','reviewer_reason':reason,'null_claim':False,'full_reconciler_record':str(F/str(seq)/'omissions.json')})
dump('omission-review.json',omission_reviews)
dispositions=[]
for seq in [283,284,285]:
 records=json.loads((F/str(seq)/'original-dispositions.json').read_bytes())
 for r in records:
  ident=r['original_identity'];original=json.loads((P/'original-angles'/ident['angle']/str(seq)/'spec.json').read_bytes())['segments'][0]['spans'];assert next(s for s in original if s['id']==ident['id'])==r['original_span']
  assert r['final_ids'] and all(any(s['seq']==seq and s['id']==i for s in resolved) for i in r['final_ids'])
  dispositions.append({'original_identity':ident,'reconciler_disposition':r['disposition'],'final_ids':r['final_ids'],'reviewer_verdict':'SOUND_FINAL_DISPOSITION','reviewer_evidence':'See independent span-review.json for each final id; the old and new boundaries/depths were compared with their actual sources.'})
dc=collections.Counter(r['reconciler_disposition'] for r in dispositions);assert dc=={'retained':129,'recast':16}
dump('original-disposition-review.json',dispositions)
head=[{'key':'c5p95/s284w6','seq':284,'id':'w6','licensor':'sogs','verdict':'LICENSED','reason':'The enumeration-continuing lexical expression sogs licenses and the other as one local phrase. Its initial and is a list connective supplied by this expression; the article is internal to the reference to remaining examples. The explanatory numeral two remains outside.'},{'key':'c5p95/s285w5','seq':285,'id':'w5','licensor':'sogs','verdict':'LICENSED','reason':'The same continuing-list expression licenses and the other in the new objection. This does not license adding two or other translator-supplied noun material.'}]
assert [x['key'] for x in head]==[x['key'] for x in json.loads((P/'actual-head-hits.json').read_bytes())]
dump('head-licensor-review.json',head)
results=json.loads((P/'canonical-results.json').read_bytes());ref=json.loads((P/'errata-refutations.json').read_bytes());assert len(ref)==4 and all(r['refuted'] for r in ref)
review={'verdict':'APPROVE','open_findings':[],'producer':{'role':'fresh independent Codex semantic reviewer','known_model_identity':'GPT-6 family per inherited developer identity; exact runtime model unavailable','metadata_exposure':'Before final release, originals-complete.json exposed original-author counts and path/hash pins only; no author/reconciler decisions were opened before release. Disclosed to and acknowledged by root.'},'reviewed_at':datetime.datetime.now(datetime.timezone.utc).isoformat(),'page':'c5p95','course':'C05','sequences':[283,284,285],'baseline':'fb9182dc6fca2230be3bc5169ba1288a22430280','segments':results,'totals':{'spans':len(resolved),'nulls':0,'d5_nonnull':sum(r['d5_nonnull'] for r in results),'material_omissions':len(omission_reviews),'original_dispositions':len(dispositions),'recasts':dc['recast'],'errata_screenings_refuted':len(ref),'bank_ready_errata':0},'resolved_spans':pin(P/'resolved-spans.json'),'head_licensors':head,'evidence':{'input_verification':str(P/'input-verification.json'),'canonical_proof':str(P/'audit-proof.json'),'source_preparation':str(B/'semantic-source-preparation/freeze.json'),'source_reopens':str(P/'source-reopened.json'),'full_query_replays':str(P/'query-replay-index.json'),'original_archives':str(P/'archive-reopened-checks.json'),'physical_reopens':str(P/'physical-reopen.json'),'original_canonical_replays':str(P/'original-canonical-summary.json'),'errata_refutations':str(P/'errata-refutations.json'),'full_reconciled_inputs':str(P/'reviewed-final'),'original_to_copy_paths':str(P/'original-copy-manifest.json'),'additional_original_paths':str(P/'additional-original-copy-manifest.json')},'limits':['Machine semantic approval is not human acceptance, publication approval, or promotion into hgm_gloss.','Digital parallels do not prove independent publication or ingestion lineage.','This review does not assert that the separate pending root mechanical/config verification has passed. Landing requires root to obtain that independent result as well.','Omitted distributed or explanatory wording remains unbanked with reasons. Zero nulls were present; no uncertainty was encoded as null.']}
dump('review.json',review)
print(json.dumps({'verdict':review['verdict'],'spans':len(span_reviews),'omissions':len(omission_reviews),'original_dispositions':dict(dc),'errata_refuted':len(ref),'open_findings':[]},indent=2))
