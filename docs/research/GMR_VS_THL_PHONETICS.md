# GMR vs THL phonetics — the convention differences, documented

*2026-08-15 · THL-sweep follow-up #1, generated from the app's own
two PROVEN engines (pron_engine port, battery-locked; thlPhonetics,
proven against the THL standard's own examples + exception list).
40 representative terms run through both; every difference below is
reproducible from the table.*

## Authority note

GMR's convention is the **binding** one for this project's outputs
(rule 1 analog: his system, his spellings; ruled exceptions like
*kamdir* overlay the engine). THL Simplified Phonetics ships as a
labeled **display mode** — the scholarly-web convention a reader may
know from THL/84000 publications. Neither is "corrected" toward the
other; the fence in pron_engine.cpp keeps external oracles away from
the GMR engine entirely.

## The systematic differences

| dimension | GMR | THL | example (GMR / THL) |
|---|---|---|---|
| umlauted vowels | plain romanization for natural English reading | ö / ü / é diacritics | *sunam* / *sönam*; *trul-ku* / *trülku* |
| final -é marking | unmarked | acute é | *dorje*→*doje* / *dorjé*; *le* / *lé* |
| final stops | voiced spelling -b | unvoiced -p (and -k kept) | *jangchub* / *jangchup*; *sherab* / *sherap* |
| aspiration after velars | dropped | kept (kh-) | *kenpo* / *khenpo*; *kyabjuk* / *khyanjuk* |
| lh- clusters | h-first (*hl-*) | l-first (*lh-*) | *hlasa* / *lhasa*; *hlaktong* / *lhaktong* |
| zh / sh | folded to *sh* | distinguished (*zh*) | *shi* / *zhi* (bzhi); *shine* / *zhiné* |
| o-umlaut syllables (chos, bod) | *-u* spelling | *-ö* | *chu* / *chö*; *bu* / *bö* |
| prenasal collapse | labial (b→m per GMR's ruled *kamdir*; engine baseline *kabdir*) | dental n | *kabdir* (ruled: *kamdir*) / *kandir* |
| nasal clusters (rnam 'grel) | nasal retained in cluster (*namndrel*) | simplified (*nandrel*) | — |
| dbu- | *u-* | *wu-* | *uma* / *wuma* |
| word joining | occasional splits/hyphens (*chenre sik*, *trul-ku*) | solid words | */ chenrezik, trülku* |

## The full 40-term table

| wylie | GMR | THL |
|---|---|---|
| bsod nams | sunam | sönam |
| chos | chu | chö |
| sangs rgyas | sangye | sanggyé |
| byang chub | jangchub | jangchup |
| sems dpa' | sempa | sempa |
| tshad ma | tsema | tsema |
| rnam 'grel | namndrel | nandrel |
| sgom | gom | gom |
| bka' 'gyur | kangyur | kangyur |
| dbu ma | uma | wuma |
| shes rab | sherab | sherap |
| phar phyin | parchin | parchin |
| sku | ku | ku |
| thugs rje | tukje | tukjé |
| rdo rje | doje | dorjé |
| dge lugs | geluk | geluk |
| bla ma | lama | lama |
| mkhan po | kenpo | khenpo |
| dge bshes | geshe | geshé |
| sprul sku | trul-ku | trülku |
| lha sa | hlasa | lhasa |
| bod | bu | bö |
| gzhan stong | shentong | zhentong |
| rang stong | rangtong | rangtong |
| skabs 'dir | kabdir (ruled: kamdir) | kandir |
| dbang phyug | wangchuk | wangchuk |
| brgyad | gye | gyé |
| bzhi | shi | zhi |
| drug | druk | druk |
| lnga | nga | nga |
| khyab 'jug | kyabjuk | khyanjuk |
| 'jam dpal | jampel | jampel |
| spyan ras gzigs | chenre sik | chenrezik |
| phyag rgya | chakgya | chakgya |
| zhi gnas | shine | zhiné |
| lhag mthong | hlaktong | lhaktong |
| rten 'brel | tendrel | temdrel |
| las | le | lé |
| nyon mongs | nyunmong | nyönmong |
| grub mtha' | drupta | drupta |

*Regeneration: the probe pattern is in the memory log
(pron_compare.cpp — both engines from liballcore).*
