import json, collections, re, statistics as st
C=json.load(open('acalls.json'))

def cls_tool(n, key, rl):
    k=' '.join((key or '').split())
    kl=k.lower()
    if n in ('Write','Edit'):
        if 'spec' in kl or '.json' in kl or 'body.html' in kl or 'report' in kl or 'errata' in kl:
            return 'A-scaffold-write'
        return 'A-scaffold-write'
    if n=='Read':
        if 'gen_alignment_page' in kl: return 'C-read-generator-source'
        if 'brief' in kl: return 'C-read-brief'
        if 'campaign' in kl: return 'C-read-campaign'
        if 'alignment_full_v1' in kl or 'span_head_allow' in kl or 'alignment_evidence' in kl: return 'B-precedent-bank'
        if '/scratchpad/' in kl or '/tmp/' in kl: return 'A-reread-own-output'
        return 'C-read-other'
    if n=='StructuredOutput': return 'F-final-spec'
    # Bash
    if re.match(r'^(mkdir|cd\s+\S+\s*&&\s*mkdir|ls\b|pwd|which|echo\s+(ok|done|hi))', k): return 'A-scaffold-fs'
    if 'mkdir -p' in k and len(k)<200: return 'A-scaffold-fs'
    if 'gen_alignment_page.py <' in k or ('gen_alignment_page.py' in k and ('<' in k or 'stdin' in k)): return 'A-run-validator'
    if 'sqlite3' in k and 'corpus_segments' in k:
        if re.search(r"english like|wylie like|acip like", k, re.I): return 'C-parallel-witness-search'
        return 'C-fetch-segment'
    if 'sqlite3' in k: return 'C-db-other'
    if 'alignment_full_v1.json' in k or 'alignment_evidence_v1.json' in k: return 'B-precedent-bank'
    if 'span_head_allow.json' in k: return 'B-precedent-bank'
    if re.search(r"(sed -n|head|tail|cat|grep|wc).{0,80}gen_alignment_page\.py", k): return 'C-read-generator-source'
    if re.search(r"(sed -n|head|tail|cat|grep|awk|wc).{0,120}_CAMPAIGN\.md", k): return 'C-read-campaign'
    if re.search(r"(sed -n|head|tail|cat|grep|wc).{0,120}(PROPOSE_BRIEF|RECONCILE_BRIEF|ALIGNMENT_LAYER_SPEC|alignment_briefs)", k): return 'C-read-brief'
    if re.search(r"cat >|cat <<|heredoc|tee ", k) and ('spec' in kl or '.json' in kl): return 'A-scaffold-write'
    if re.search(r"python3?\s+-\s*<<|python3? -c", k):
        # substring / count / verify style
        if re.search(r"\.find\(|\.count\(|in wylie|in english|substring|index\(|verif|assert", k, re.I): return 'B-substring-verify'
        if 'json.load' in k or 'json.dump' in k: return 'A-json-plumbing'
        return 'B-adhoc-compute'
    if '/scratchpad/' in k or '/tmp/claude' in k: return 'A-reread-own-output'
    if re.search(r'^(grep|rg|find)\b', k): return 'C-read-other'
    if re.search(r'^(sed -n|head|tail|cat|wc)\b', k): return 'C-read-other'
    return 'Z-other'

# per-call: cost and the classes it touched
rows=[]
for c in C:
    tools=c['tools']
    classes=[cls_tool(n,k,rl) for n,k,rl in tools] or (['F-reasoning-only'])
    bill=c['i']+c['cc']+c['o']
    rows.append(dict(c, classes=classes, bill=bill))

# ATTRIBUTION 1: simple -- split a call's billable equally across the classes of tools it called
share=collections.Counter(); ncalls=collections.Counter(); crshare=collections.Counter(); outshare=collections.Counter()
for r in rows:
    n=len(r['classes'])
    for cl in r['classes']:
        share[cl]+=r['bill']/n; crshare[cl]+=r['cr']/n; outshare[cl]+=r['o']/n; ncalls[cl]+=1/n
tot=sum(share.values()); totcr=sum(crshare.values())
print('=== ATTRIBUTION 1: call cost -> class of tool that call invoked ===')
print('%-28s %9s %8s %10s %8s' % ('class','calls','%bill','bill','%cacheread'))
for cl,v in share.most_common():
    print('%-28s %9.0f %7.1f%% %10.0f %7.1f%%' % (cl, ncalls[cl], 100*v/tot, v, 100*crshare[cl]/totcr))
print('TOTAL billable %.0f  cache_read %.0f  calls %.0f' % (tot, totcr, sum(ncalls.values())))
json.dump({k:[ncalls[k], share[k], crshare[k], outshare[k]] for k in share}, open('attr1.json','w'))

# tool result byte volume by class -- the causal driver of cache_creation
vol=collections.Counter(); cnt=collections.Counter()
for c in C:
    for n,k,rl in c['tools']:
        cl=cls_tool(n,k,rl); vol[cl]+=rl; cnt[cl]+=1
print()
print('=== TOOL-RESULT BYTES PULLED INTO CONTEXT, by class (the cache_creation driver) ===')
tv=sum(vol.values())
print('%-28s %7s %12s %7s  %s' % ('class','n','bytes','%','~tokens'))
for cl,v in vol.most_common():
    print('%-28s %7d %12d %6.1f%%  %9.0f' % (cl, cnt[cl], v, 100*v/tv, v/3.6))
print('TOTAL tool-result bytes %d  (~%.0f tokens) over %d tool calls' % (tv, tv/3.6, sum(cnt.values())))
