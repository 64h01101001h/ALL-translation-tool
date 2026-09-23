import re
R='/Users/adamderickandrade/ALL-translation-tool/'
with open(R+'build/botok_tok_corpus.tsv',encoding='utf-8') as f:
    for i,line in enumerate(f):
        if i<3: continue
        key,_,rest=line.rstrip('\n').partition('\t')
        toks=re.findall(r'text=(.*?)type=(\w+)start=(\d+)len=(\d+)',rest)
        print(repr(key[:80])); print(toks[:6]); print(repr(rest[:300])); break
