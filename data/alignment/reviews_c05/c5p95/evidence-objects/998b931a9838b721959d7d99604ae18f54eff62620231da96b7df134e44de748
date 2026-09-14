from pathlib import Path
import json,sys,importlib.util,hashlib,copy,os
D=Path(__file__).resolve().parent;R=D.parent;B=R.parent;W=B.parents[1]/'campaign-worktree'
assert sys.dont_write_bytecode

def load(p):return json.loads(p.read_bytes())
def save(p,o):
 p.parent.mkdir(parents=True,exist_ok=True)
 with p.open('x') as f:json.dump(o,f,indent=2,ensure_ascii=False);f.write('\n')
def pin(b):return dict(bytes=len(b),sha256=hashlib.sha256(b).hexdigest())
s=importlib.util.spec_from_file_location('canonical_resolver',W/'tools/gen_alignment_page.py');g=importlib.util.module_from_spec(s);s.loader.exec_module(g)
rows={x['seq']:x for x in load(D/'source-rows-full.json')};originals=load(B/'proposal-verification/root-resolved-spans.json')
# Every original identity and optional field is checked against the full source spec/declaration.
for angle in ['tibetan','english']:
 for seq in rows:
  seg=load(B/angle/str(seq)/'spec.json')['segments'][0];decl=load(B/angle/str(seq)/'intended-spans.json');these=[o for o in originals if o['angle']==angle and o['seq']==seq];assert len(these)==len(seg['spans'])==len(decl)
  rr=g.resolve(rows[seq]['wylie'],seg['spans'],'tib',seq);by={s['id']:s for s in seg['spans']};ee=g.resolve(rows[seq]['english'],g.with_members(seg['spans'],[by[k] for k in seg['eng_order']],rows[seq]['english']),'eng',seq)
  for o,sp,de in zip(these,seg['spans'],decl):
   assert o['original_span']==sp and o['original_field_presence']==list(sp) and o['original_occurrence_declaration']==de
   assert [*rr[sp['id']]]==o['tib_range'] and [*ee[sp['id']]]==o['eng_range']
# These are current contextual judgments; entries/corpus are supporting evidence, not automatic translations.
why={283:{
'w1':"kho na re is the objector's speaking formula: someone responds. The complete relational recast belongs at d3, not a dictionary compound. Initial byas pa la frames the preceding reasoning and terminal zer na closes the hypothetical objection; neither supplies another independently owned responds.",
'w2':"skyon names the problem denied in the response. Select problem inside no such problem; do not include supplied such or here. HGM860 supports problem and whole skyon med869 also supports the negative expression; this flat analysis preserves the separate negative word.",
'w3':"med is the lexical absence predicate, d5, rendered by no inside there's no such problem. Opening No is the discourse reply and is not this selected occurrence. The negation is overt, so a null or omission would lose useful ownership.",
'w4':"yan lag is an established anatomical/part compound, here the protuberances of the person. HGM17346 and the preceding five-parts argument support this local sense; not limbs alone or the following person.",
'w5':"lnga supplies the first five, modifying protuberances. The later English five restates the Tibetan anaphor des and is not a second source lnga. No numeral defect is inferred from the earlier four limbs plus head.",
'w6':"The first skyes bu names the whole person whose parts the protuberances are. Select the first person after one, leaving of the one outside this lexical unit; the source demonstrative is de, not gcig.",
'w7':"cha shas is the established parts compound, predicating the five protuberances as the person's parts. English all and the possessive relation remain outside its lexical exponent.",
'w8':"yin is the copula are in are all parts. It does not own either progressive are before performing. Preserve the distinct predicate rather than borrowing an auxiliary occurrence.",
'p9':"The standalone causal pas immediately after yin links being parts of the person to attributing the parts' actions to that person. English so expresses that very consequence at the sentence hinge; d6 is appropriate for this particle. Later byas pas lies within the when-action premise, and the closing pa'i phyir supports the whole explanatory claim; neither is additionally mapped to so.",
'w10':"The first bya ba is actions performed by the parts, the first English actions after some. This established action noun remains distinct from the later action attributed to the whole person.",
'w11':"The byas after the first bya ba is the parts' performing predicate, not initial byas pa introducing the reasoning. HGM12728/12732 support the performing sense; progressive morphology is English grammatical presentation and are/some stay outside.",
'w12':"The later skyes bu is the person to whom the parts' action is attributed: the second English person, the subject of the second performing. Do not reuse the first person occurrence.",
'w13':"The second bya ba denotes the actions attributed to the whole person, matching the last actions. Its occurrence and argument role distinguish it from the parts' first actions.",
'w14':"The later byas is performing predicated of the person. Select the second performing, with the preceding progressive is unwrapped; initial reasoning byas pa and the parts' performing are separate occurrences.",
'w15':"'jog is the conventional attribution verb: say that the person's action has occurred. It is a lexical verb (HGM5312), not a null and not the earlier responds.",
'w16':"dgos supplies the necessity expressed by have to before say. This modal lexical item is d5 (HGM2434); we is the translator's inclusive framing and is not part of its exponent."
},284:{
'w1':"snga ma is the earlier case, locally expressed by above. The nominal head case and the initial Well are not included in this narrow lexical exponent.",
'w2':"mtshungs asserts equivalence of this case with the preceding analogy. HGM15769 positively includes same and exactly the same, and the emphatic English equivalence is the local predicate. Select exactly the same as the complete attested idiomatic equivalent, retaining the article internal to the phrase; no separate Tibetan intensifier is invented. Both originals' same is a defensible narrower constituent, recast here to the complete equivalent.",
'w3':"rnag khrag is the attested combined substance (HGM23908), blood and pus, rather than three separate listed things. The compound's English constituents reverse Tibetan order. The internal and coordinates these components; it is not the following list-closure sogs. Two consecutive tighter d7 members preserve each constituent without making overlapping flat words.",
'm4':"rnag is pus inside rnag khrag, the later English constituent of blood and pus. HGM10448 is glossary HTG2016, not corpus-mined; reciprocal noisy variants do not override the current compound meaning. The member is unique and strictly contained on both sides.",
'm5':"khrag is blood inside rnag khrag, the earlier English constituent of blood and pus. HGM1368 is glossary HTG2016. Canonical member grouping permits this crossed English order and both siblings belong to w3.",
'w6':"sogs closes the list with remaining examples: and the other. HGM20919 positively attests that full expression, and the local la sogs pa'i structure supplies list continuation, including the connective and. Two is an explanatory remainder of the three and has no separately occurring gnyis here; it stays unwrapped. The actual and-headed predicate hit needs a named licensor allowance.",
'w7':"The first dngos po denotes the three things being compared, so things is its lexical exponent. mentioned is explanatory framing, not part of the noun.",
'w8':"The first gsum enumerates the things: the first three before things. It is not the later three appearing nor the last three ways.",
'w9':"The first brlan describes the glass's contents as wet. Choose the first wet; the second descriptor occurs later in the restated glass description. HGM19192 and the actual source adjective support this sense.",
'p10':"The first zhing coordinates wet and flowing. Its standalone d6 exponent is the and between the first wet and flowing, not either and around blood/pus/list closure.",
'w11':"The first gsher ba is flowing in the first contents description. It is a lexical predicate/adjective (HGM20149), not the connective zhing.",
'w12':"The first phor pa names the glass of wet and flowing contents. Select the first glass, independently of the repeated glass later in the sentence.",
'w13':"The first gang po is the complete adjective full (HGM22118), with po part of the selected lexical form. Tibetan original gang is a defensible stem; English's gang po preserves the full local unit. of something remains outside.",
'w14':"cha shas states that the three things are parts of the glass of wet and flowing contents. parts is the established compound's exponent; all is outside.",
'w15':"yin supplies are in are all parts. It does not own later is acting or are being displayed; those belong to the English paraphrase of the later source relation/predicate.",
'w16':"The second gsum enumerates the things when they appear, corresponding to three immediately after When the. It is distinct from the first list count and the last number of ways.",
'w17':"snang ba is the appearance predicate here, appear in When the three appear. The curated nominal HGM11018 alone cannot prove an inflected verb; the source conditional clause and finite English predicate license this local use.",
'p18':"na is the standalone conditional/temporal particle after snang ba, expressed by When introducing that appearance clause. then belongs to the ensuing English recast and is not duplicated onto the same particle.",
'w19':"The second brlan owns the second wet, in the later glass description. Do not attach it to the earlier descriptor despite identical spelling.",
'p20':"The second zhing owns the and between the second wet and flowing. It is d6 and distinct from the first wet/flowing connective and later and that.",
'w21':"The second gsher ba owns the second flowing, in the later description of the glass; no imported participant or relative clause is included.",
'w22':"The second phor pa names the glass now serving as the locus of appearing. Its exponent is the second glass; the later it is a referential restatement, not another phor pa.",
'w23':"The second gang po is full modifying the second glass. As in the first occurrence, select the full adjective (HGM22118), excluding of a thing which is.",
'w24':"steng du is the complete spatial postposition: upon in displayed upon it. Both steng6935/upon and steng du6941/upon are positive, and the local source explicitly locates the appearances on the glass. Acting as a basis is a wider explanatory realization of the same relation; retain its minimal spatial exponent once, leaving that wider phrase unwrapped and avoiding duplicate ownership. English omission of the entire relation is unnecessarily lossy.",
'w25':"snang tshul is the established ways-of-appearing compound (HGM11049), not two unrelated flat nouns. The local phrase concerns modes of appearance of the same locus; the displayed predicate later belongs to the second snang.",
'w26':"mi 'dra ba is the lexical unequal/different unit modifying those modes. different is not a freestanding English negative affix, so do not create false null or affix mappings for its internal negative.",
'w27':"The third gsum counts ways of appearing: the last three before different. Its exact occurrence must not be borrowed from either earlier three.",
'w28':"The final snang is the display/appearance predicate, rendered displayed. The preceding snang tshul names the ways, and the earlier snang ba is appear in the conditional clause. Passive auxiliaries are being remain unwrapped.",
'w29':"'jog expresses the conventional attribution, say in we can say. The surrounding we can is English modal/speaker framing rather than part of the verb's dictionary exponent."
},285:{
'p0':"Initial yang marks the renewed objection, rendered yet another before argument. This is a standalone discourse particle at d6, not a d5 multiword noun. HGM17092 positively attests yet another; the current new-objector frame confirms it. else remains outside this single exponent and is not a second use of yang.",
'w1':"kha cig introduces an indefinite objector, Someone. It is the established indefinite expression, not the entire speaking frame; may come now and make and argument remain outside this narrow nominal exponent.",
'w2':"rnag khrag is the closed pus-and-blood substance compound (HGM23908), now in matching constituent order. Two unique d7 members identify pus and blood inside the parent; two is not a count of these two lexical components in the three-object argument.",
'm3':"rnag is pus, the first constituent inside pus and blood. Its HTG2016 glossary origin and this source substance support the mapping; the tighter member stays within w2 on both sides.",
'm4':"khrag is blood, the second constituent inside pus and blood, unique and strictly contained in w2. This occurrence differs in order from284, so no copied English ordering is assumed.",
'w5':"sogs licenses the continued list and the other (HGM20919). The connective and belongs to this list-closing lexical expression; two is the explanatory remainder of gsum's three examples, not a separate Tibetan numeral. The actual d5 and-headed hit is explicitly proposed for c5p95.",
'w6':"The first dngos po names the initial three things, before the negative question. Select things, not either later objects occurrence.",
'w7':"The first gsum is three modifying things in the initial topic. It is separate from the number of beings and the final number of validly established objects.",
'w8':"phan tshun is the reciprocal each other in block each other. Later each one occupies elaborates the reciprocal spatial relation, but the same source occurrence is not cloned across both locations.",
'w9':"go sa is the space/position occupied, expressed by space. From entering and that each one occupies explain the mutual-blocking relation and remain outside this noun's minimal exponent. Exact longer go sa 'gog pa having no dictionary hit does not negate these constituents.",
'w10':"'gog pa supplies block in the mutual obstruction predicate. Keep the full lexical form at d5, excluding would and which; it is inside the proposition whose object type is negated, not an asserted positive conclusion.",
'w11':"The second dngos po supplies objects in not types of objects. It is the first English objects, distinct from the later validly established objects. types of is the explanatory nominal classification around this noun.",
'p12':"ma is the explicit standalone negative particle, not in these are not types of objects. Its d6 scope negates yin and the described object classification; it is not an English question word or uncertain content null.",
'w13':"yin is the subordinate are following these, paired with explicit ma/not. The initial capital Are belongs to the interrogative consequence frame and is not this copula. The full sentence remains a challenge asking what follows, not an affirmative claim.",
'w14':"las is karma, the possessed property of the beings. Full las can19364 positively attests with their own karma, but the current flat lexical/particle analysis assigns karma to las and with to can; their own is an individualized English expansion of the possessive relation, not a separate source possessive word. This narrower analysis is a contextual choice, not a claim the wider glossary equivalent is absent or false.",
'p15':"can is the standalone possessing particle, expressed by with in each with their own karma. HGM4128 positively attests with. Retain it at d6 after las; do not turn it into a d7 member without selecting a las can parent, and do not independently assign their own.",
'w16':"'gro ba names the beings looking at the water. The curated living-being head supports the referent; the actual local English exponent is beings, with types of unwrapped.",
'w17':"The second gsum counts those beings: three before types of beings. It is not the initial three things or final three objects.",
'w18':"chu is water in the shared glass. of is a noun-relation realization outside the lexical exponent; no water is imported into284's less specific something wet and flowing.",
'w19':"phor pa names the single glass being looked at, the only glass here. The source chu modifies its contents and gang its fullness; no extra source glass is inferred from later there.",
'w20':"gang is the full/filled sense modifying glass, not interrogative which. Here there is no following po, unlike284; select the actual source form gang and exact English full.",
'p21':"la immediately after gang marks the object toward which looking is directed, at before a glass. It is the second la in the Tibetan sentence, not list-initial la sogs or final pa la.",
'w22':"cig car du is the complete simultaneous/together adverb (HGM4169). together expresses this shared looking; even though sit down together is a positive wider glossary variant, sitting posture is not separately encoded in this local source form. Retain the adverb with du and leave sit down outside, acknowledging rather than denying the wider attestation.",
'w23':"bltas pa is the complete look/looked lexical form (HGM7117) governing the glass. look is its English exponent; at is already assigned to overt la. Tibetan's shorter bltas has positive HGM7116 support but the full pa form preserves the current lexical boundary without absorbing posture or temporal framing.",
'w24':"The third dngos po names the validly established objects, the second English objects near the sentence end. Keep the three objects distinct from the earlier classification of non-obstructing objects.",
'w25':"tshad grub pa is the attested epistemic compound, confirmed by a valid perception (HGM24799, glossary HTG2016). Full contextual C05:270/281/313 witnesses support this relation, unlike the clipped auto-aligned tshad grub variant. The complete predicate owns the internal by a relation; tighter lexical members preserve its constituents, not two overlapping flat spans.",
'm26':"tshad is valid perception inside the epistemic compound. HGM15282 explicitly supports valid perception; it is not invented from the longer parent alone. It occupies the later English constituent, unique within w25.",
'm27':"grub pa is confirmed, the complete lexical member (HGM2046) inside tshad grub pa. Its English precedes valid perception despite Tibetan order. This tightens within the parent while keeping the pa ending of the lexical form; Tibetan's shorter grub is recast to this supported full member, not duplicated as a flat word.",
'w28':"The third gsum counts the final objects: three immediately before different objects. different elaborates their plurality/context and remains outside the numeral.",
'w29':"srid expresses possible in the assertion being recalled. It is not the earlier beings sense of another entry; the local existential possibility clause selects the modal adjective (HGM20964).","w30":"khas blangs is the accepted/asserted position recalled at the end, locally rendered by position. Both bare1137 and fuller1139 positively attest this nominal English recast, and the current source attaches the possibility proposition to that admission. Select position once, keeping the Tibetan original's narrow established unit; said is a distributed reporting realization of the admission and is not mapped again. The English omission is conservative but would discard a defensible dictionary correspondence. This is a contextual lexical nominalization, not a claim that admission and all of said ... position form a contiguous English span."}}
titles={283:'the actions of parts and person',284:'three appearances on one basis',285:'the challenge about mutually obstructing objects'}
for seq,row in rows.items():
 out=R/str(seq);old=load(B/'tibetan'/str(seq)/'spec.json');seg=copy.deepcopy(old['segments'][0]);seg['title']=titles[seq]
 if seq==284:
  for sp in seg['spans']:
   if sp['id']=='w2':sp['eng']='exactly the same'
   if sp['id'] in ['w13','w23']:sp['tib']='gang po'
   if sp['id']=='w24':sp['tib']='steng du'
 if seq==285:
  seg['spans'].insert(0,{'id':'p0','d':6,'tib':'yang','eng':'yet another'})
  for sp in seg['spans']:
   if sp['id']=='w23':sp['tib']='bltas pa'
   if sp['id']=='m27':sp['tib']='grub pa'
 seg['note']='PROVISIONAL — Codex model reconciliation, using independently frozen Tibetan-first and English-first originals. GMR’s English and source Wylie are preserved verbatim. Lexical units, grammatical exponents and repeated occurrences are distinguished; distributed wording stays unwrapped and no uncertainty is represented as a null. Evidence and all original dispositions are retained with this segment. This is machine analysis, not human certification or publication approval.'
 # Exact declared target ranges supply English ordering; changed expressions remain anchored to their original context.
 intended={o['id']:o for o in originals if o['angle']=='tibetan' and o['seq']==seq}
 if seq==285:intended['p0']=next(o for o in originals if o['angle']=='english' and o['seq']==seq and o['tib']=='yang')
 engpositions={sp['id']:(row['english'].index('exactly the same') if seq==284 and sp['id']=='w2' else intended[sp['id']]['eng_range'][0]) for sp in seg['spans']}
 seg['eng_order']=[sp['id'] for sp in sorted([sp for sp in seg['spans'] if sp['d']!=7],key=lambda sp:engpositions[sp['id']])]
 assert set(why[seq])=={sp['id'] for sp in seg['spans']}
 by={s['id']:s for s in seg['spans']};tr=g.resolve(row['wylie'],seg['spans'],'tib',seq);er=g.resolve(row['english'],g.with_members(seg['spans'],[by[x] for x in seg['eng_order']],row['english']),'eng',seq)
 tuples=[];parent=None
 for sp in seg['spans']:
  if sp['d']!=7:parent=sp['id']
  tuples.append({'id':sp['id'],'d':sp['d'],'tib':sp['tib'],'eng':sp['eng'],'tib_range':list(tr[sp['id']]),'eng_range':list(er[sp['id']]),'parent':parent if sp['d']==7 else None})
  assert er[sp['id']][0]==engpositions[sp['id']]
 save(out/'spec.json',{'course':'C05','segments':[seg]});save(out/'final-tuples.json',tuples)
 decisions=[{'final_span':sp,'final_tuple':t,'rationale':why[seq][sp['id']]+f" Exact source ranges: Tibetan {t['tib_range']}; English {t['eng_range']}."} for sp,t in zip(seg['spans'],tuples)];save(out/'final-decisions.json',decisions)
 dispositions=[]
 for o in [x for x in originals if x['seq']==seq]:
  exact=[t for t in tuples if all(o[k]==t[k] for k in ['d','tib','eng','tib_range','eng_range'])]
  if exact:chosen=exact;disposition='retained'
  else:
   chosen=[t for t in tuples if t['tib_range'][0]==o['tib_range'][0] and (t['tib']==o['tib'] or t['tib'].startswith(o['tib'])) and (t['eng']==o['eng'] or o['eng'] in t['eng'])]
   # rnag/blood constituent should select its tighter member rather than its compound.
   chosen=sorted(chosen,key=lambda t:abs(len(t['tib'])-len(o['tib']))+abs(len(t['eng'])-len(o['eng'])))[:1]
   assert chosen,(seq,o)
   disposition='recast'
  assert len(chosen)==1
  t=chosen[0];reason=('Same depth, wording and exact occurrences. ' if disposition=='retained' else 'Recast the original boundary/depth to the final established unit or strictly contained compound member. ')+why[seq][t['id']]
  dispositions.append({'original_identity':{k:o[k] for k in ['angle','seq','id','d','tib','eng','tib_range','eng_range']},'original_span':o['original_span'],'original_key_order':o['original_field_presence'],'optional_presence':{k:dict(present=k in o['original_span'],**({'value':o['original_span'][k]} if k in o['original_span'] else {})) for k in ['cls','nul','subword']},'original_parent':o['parent_id'],'original_declaration':o['original_occurrence_declaration'],'disposition':disposition,'final_ids':[t['id']],'reason':reason})
 save(out/'original-dispositions.json',dispositions)
 gaps={}
 for side,field in [('tib','wylie'),('eng','english')]:
  ranges=sorted([t[side+'_range'] for t in tuples if t['d']!=7]);cur=0;items=[]
  for lo,hi in ranges:
   if cur<lo:items.append({'start':cur,'end':lo,'text':row[field][cur:lo]})
   cur=hi
  if cur<len(row[field]):items.append({'start':cur,'end':len(row[field]),'text':row[field][cur:]})
  gaps[side]=items
 save(out/'unwrapped-complements-draft.json',gaps)
 print(seq,len(tuples),'D5',sum(t['d']==5 for t in tuples),'originals',len(dispositions),'recast',sum(x['disposition']=='recast' for x in dispositions))
 for side,items in gaps.items():print(side,[(x['start'],x['end'],x['text']) for x in items])
