#!/usr/bin/env python3
"""English-first author declarations, BEFORE canonical generation. Not a generator."""
import json, re
from pathlib import Path
HERE=Path(__file__).resolve().parent
E=HERE.parent
rows={r['seq']:r for r in json.loads((HERE/'queries/target-rows.json').read_text())['rows']}
# tib, English, depth, Tibetan whole-word occurrence (1-based), English occurrence,
# precise ownership reason. All strings are proposed matches to original GMR words.
A={
283:[
('kho na re','someone responds',3,1,1,'The pronoun and quotative jointly identify the reply; no claim that kho alone has a binding glossary gloss someone. C05:259/293 retain the same formula.'),
('skyon','problem',5,1,1,'Explicit fault noun; HGM 860.'),
('med','no',5,1,1,'Negative existence licenses the lowercase no attached to problem. Initial No and the existential carrier remain unwrapped; HGM 14511.'),
('yan lag','protuberances',5,1,1,'HGM 17346 exact; preceding C05:282 identifies four limbs plus head.'),
('lnga','five',5,1,1,'First English five counts the explicitly named limbs; the later anaphoric five is not captured; HGM 3997.'),
('skyes bu','person',5,1,1,'First explicit person, without supplied one/article; HGM 804.'),
('cha shas','parts',5,1,1,'Established compound; excludes supplied all and surrounding genitive carrier; HGM 4384.'),
('yin','are',5,1,1,'Copular verb at are all parts, not the later progressive auxiliary are.'),
('bya ba','actions',5,1,1,'First action noun is the limbs action; HGM 12506; excludes some.'),
('byas','performing',5,2,1,'Second Tibetan byas: after the first bya ba. Opening byas pa is anaphoric narration, not this action. HGM 12728.'),
('skyes bu','person',5,2,2,'Second person is the whole actor in the consequent.'),
('bya ba','actions',5,2,2,'Second action noun belongs to that whole person.'),
('byas','performing',5,3,2,'Third Tibetan byas and second English performing; omits the English is auxiliary.'),
("'jog",'say',5,1,1,'Positing/concluding lexical verb; HGM 5312. Does not capture we, that or have to.'),
('dgos','have to',5,1,1,'Obligation predicate exactly attested HGM 2434; not the supplied participant we.')],
284:[
('snga ma','above',5,1,1,'Earlier/former reference, HGM 4017. Supplied case stays unwrapped.'),
('mtshungs','same',5,1,1,'Equality adjective, HGM 15769; excludes the and emphatic exactly.'),
('rnag','pus',5,1,1,'Pus noun, crossing khrag/blood in English; HGM 10448 and physical source context.'),
('khrag','blood',5,1,1,'Blood noun; HGM 1368. Does not inherit reversed noisy alternatives from the glossary.'),
('sogs','other',5,1,1,'Only the closing other gesture. The remaining count two is supplied from the three-item discussion and is deliberately outside the span.'),
('dngos po','things',5,1,1,'Explicit functional-object noun; HGM 3672. Does not claim mentioned.'),
('gsum','three',5,1,1,'First explicit numeral counts the things.'),
('brlan','wet',5,1,1,'First wet, within the parts of the glass statement; HGM 19192.'),
('zhing','and',6,1,3,'Conjunctive zhing between wet and flowing; not noun zhing/paradise. Third whole-word and in the English.'),
('gsher ba','flowing',5,1,1,'First flowing, HGM 20149 exact.'),
('phor pa','glass',5,1,1,'First glass, HGM 24066; excludes article.'),
('gang po','full',5,1,1,'First full, HGM 22118; excludes of and supplied something.'),
('cha shas','parts',5,1,1,'Parts of the glass, HGM 4384.'),
('yin','are',5,1,1,'The copula in are all parts; not the later passive are.'),
('gsum','three',5,2,2,'Second numeral in When the three appear.'),
('snang ba','appear',5,1,1,'Nominalized verb in temporal subordinate clause, verbal register attested by the exact source and parallel, not an invented noun gloss.'),
('na','When',6,1,1,'Temporal conditional closes the immediately preceding appearing clause; capitalized original When.'),
('brlan','wet',5,2,2,'Second wet, within the later glass-as-basis clause.'),
('zhing','and',6,2,4,'Second wet-and-flowing conjunction, fourth whole-word and.'),
('gsher ba','flowing',5,2,2,'Second flowing.'),
('phor pa','glass',5,2,2,'Second glass.'),
('gang po','full',5,2,2,'Second full.'),
('snang tshul','ways of appearing',5,1,1,'Established mode-of-appearance compound, HGM 11049 exact; does not absorb different.'),
("mi 'dra ba",'different',5,1,1,'Established negative adjective, HGM 14183. No invented independent exponent for mi inside different.'),
('gsum','three',5,3,3,'Third numeral counts the modes of appearing.'),
('snang','displayed',5,3,1,'Third snang occurrence follows the third gsum; appearance predicate rendered displayed. English are being and Tibetan bar remain unwrapped.'),
("'jog",'say',5,1,1,'Positing verb; we can and that are unwrapped.')],
285:[
('yang','yet another',6,1,1,'Renewed-argument discourse marker; HGM 17092. Does not capture argument or also claim else.'),
('kha cig','Someone',5,1,1,'Indefinite speaker, HGM 1003; leaves else and expanded speaking formula unwrapped.'),
('rnag','pus',5,1,1,'Pus noun; literal order here differs from 284.'),
('khrag','blood',5,1,1,'Blood noun.'),
('sogs','other',5,1,1,'Closing other only; excludes supplied two, article and conjunction.'),
('dngos po','things',5,1,1,'First object noun appears as things.'),
('gsum','three',5,1,1,'First numeral counts those things, not the later beings.'),
('phan tshun','each other',5,1,1,'Reciprocal compound, HGM 11678. Later each one is explanatory and unwrapped.'),
('go sa','space',5,1,1,'Spatial-place compound, HGM 1865. Excludes that each one occupies, a distributed explanation of blocking occupancy.'),
("'gog pa",'block',5,1,1,'Blocking verb, HGM 2580. Excludes would and from entering.'),
('dngos po','objects',5,2,1,'Second Tibetan object noun corresponds to first English objects, within the blocking challenge.'),
('ma','not',6,1,1,'Explicit negation of the copula; not null.'),
('yin','are',5,1,1,'Copular are in these are not types of objects; no supplied types captured.'),
('las','karma',5,1,1,'Karmic register locally licensed by las can, HGM 19364 and C05:262/270/312; curated deeds is not substituted into GMR English.'),
('can','with',6,1,1,'Possessive/qualifying formative starts its own syllable and licenses with; excludes their own and each.'),
("'gro ba",'beings',5,1,1,'Living beings, HGM 2677; excludes supplied types of.'),
('gsum','three',5,2,2,'Second numeral counts the beings.'),
('chu','water',5,1,1,'Explicit water; HGM 4459.'),
('phor pa','glass',5,1,1,'Container noun; HGM 24066.'),
('gang','full',5,1,1,'Filled/full adjective, HGM 1648 in the container construction.'),
('la','at',6,2,1,'The la following chu phor pa gang marks the object of look at. First la belongs to la sogs and is not this occurrence.'),
('cig car','together',5,1,1,'Simultaneous looking, HGM 4168 and C05:270 all together. Does not license sit down.'),
('bltas pa','look',5,1,1,'Looking verb, HGM 7117; la owns at separately.'),
('dngos po','objects',5,3,2,'Third Tibetan object noun is the later possible-three-objects claim.'),
('tshad','valid perception',5,1,1,'Abbreviated valid-perception term, HGM 15282. C05:270 expands tshad mas grub pa with the same English construction; excludes by a.'),
('grub pa','confirmed',5,1,1,'Established/confirmed predicate, HGM 2046, crossing tshad on the English side.'),
('gsum','three',5,3,3,'Third numeral counts the final objects; different is supplied here, unlike the explicit mi dra ba in 284.'),
('srid','possible',5,1,1,'Possibility predicate, HGM 20964; excludes the English existential continuation.')]
}
NOTES={
283:'Provisional machine alignment by Codex, independent English-first proposal; unreviewed. Repeated actions and performing belong separately to the limbs and the person. The anaphoric second five and distributed causal/quotation wording remain unwrapped.',
284:'Provisional machine alignment by Codex, independent English-first proposal; unreviewed. Each wet-and-flowing occurrence is resolved separately. Other excludes the supplied count two. The locative explanation rendered both acting as a basis and upon it remains unwrapped.',
285:'Provisional machine alignment by Codex, independent English-first proposal; unreviewed. Object and numeral occurrences are distinguished. Karma/with and valid perception/confirmed are split at their lexical owners. Distributed speaking, occupancy and admission wording remains unwrapped.'}
TITLES={283:'the person and the parts',284:'three appearances on one basis',285:'objects and occupied space'}
def occurrences(text,piece):
    return [[m.start(),m.end()] for m in re.finditer(re.escape(piece),text) if (not piece[0].isalnum() or m.start()==0 or not text[m.start()-1].isalnum()) and (not piece[-1].isalnum() or m.end()==len(text) or not text[m.end()].isalnum())]
for seq, declarations in A.items():
    dest=E/str(seq);spans=[];intended=[]
    for i,(tib,eng,d,tn,en,reason) in enumerate(declarations,1):
        ident='w'+str(i)
        tr=occurrences(rows[seq]['wylie'],tib)[tn-1]
        er=occurrences(rows[seq]['english'],eng)[en-1]
        spans.append({'id':ident,'d':d,'tib':tib,'eng':eng})
        intended.append({'id':ident,'tib_range':tr,'eng_range':er,'tib_occurrence':tn,'eng_occurrence':en,'tib':tib,'eng':eng,'d':d,'reason':reason})
    assert all(intended[i]['tib_range'][1]<=intended[i+1]['tib_range'][0] for i in range(len(intended)-1))
    eng_sorted=sorted(intended,key=lambda x:x['eng_range'])
    assert all(eng_sorted[i]['eng_range'][1]<=eng_sorted[i+1]['eng_range'][0] for i in range(len(eng_sorted)-1))
    spec={'course':'C05','segments':[{'seq':seq,'title':TITLES[seq],'spans':spans,'eng_order':[x['id'] for x in eng_sorted],'note':NOTES[seq]}]}
    for name,data in [('spec.json',spec),('intended-spans.json',intended),('errata.json',[])]:
        path=dest/name;assert not path.exists(),path
        path.write_text(json.dumps(data,ensure_ascii=False,indent=2)+'\n')
    print(seq,len(spans),{str(d):sum(s['d']==d for s in spans) for d in [3,5,6,7]})
