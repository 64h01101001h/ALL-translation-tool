# Per-segment token profile re-derived from 21 completed c05-align runs (189 segments, 567 agents, 10,931 API calls)
N=57.836; R=5045521; W=248612; O=104580; I=116; THINK=64576
SYS=32908; ATT=17429           # measured: shared tools+system prefix; per-agent first-message attachments
AGENTS=3
P={'opus5':dict(i=5,w5=6.25,w1=10,r=0.5,o=25),'opus55':dict(i=4,w5=5,w1=8,r=0.2,o=20),
   'fable51':dict(i=10,w5=12.5,w1=20,r=0.25,o=50),'sonnet5':dict(i=2,w5=2.5,w1=4,r=0.2,o=10)}
def cost(p,R=R,W=W,O=O,I=I,wkey='w5'):
    return (R*p['r']+W*p[wkey]+O*p['o']+I*p['i'])/1e6
for k,p in P.items(): print('S0/S1 profile at %-8s $%.3f'%(k,cost(p)))
b=cost(P['opus5'])
print('shares opus5: reads %.1f%% writes %.1f%% output %.1f%% (thinking %.1f%%)'%(R*.5/1e6/b*100,W*6.25/1e6/b*100,O*25/1e6/b*100,THINK*25/1e6/b*100))
# 1h TTL on all subagent writes, minus measured expiry rewrites (7,993 tok/seg would become reads)
EXP=7993
for k in ['opus5','opus55']:
    p=P[k]; d=W*(p['w1']-p['w5'])/1e6 - EXP*(p['w5']-p['r'])/1e6
    print('S4 1h TTL delta at %s: +$%.3f/segment (%.1f%%)'%(k,d,d/cost(p)*100))
# Lean agentType: fixed prefix SYS+ATT -> T (tools+minimal system); brief moves into shared system prompt (B tokens)
def lean(p,T,B=4800,skills_kept=0):
    sys_saved=N*(SYS-T-skills_kept)            # read on every call
    att_reads_saved=(N-AGENTS)*(ATT)           # attachments were read on calls 2..n
    att_writes_saved=AGENTS*ATT
    brief=AGENTS*B*(p['w5']-p['r'])            # brief write->read (post-repair baseline)
    s=(sys_saved+att_reads_saved)*p['r']/1e6 + att_writes_saved*p['w5']/1e6 + brief/1e6
    return s
for k in ['opus5','opus55']:
    p=P[k]
    for T in (3000,6000,10000):
        for sk in (0,7500):
            s=lean(p,T,skills_kept=sk)
            print('S2 lean %s T=%5d skills_kept=%4d: saves $%.3f/seg = %.1f%%'%(k,T,sk,s,s/cost(p)*100))
# value of removing one round trip (avg context per call 91.5K measured; avg output/call)
ctx=91537; outpc=O/N
for k in ['opus5','opus55']:
    p=P[k]; print('one removed round trip at %s ~ $%.4f (ctx read) + $%.4f (its output)'%(k,ctx*p['r']/1e6,outpc*p['o']/1e6))
# Batch, single-shot + deterministic repair rounds (no harness). assumptions stated
def batch_ss(p,r,disc=0.5,prop_in=9000,prop_out=29500,rep_in_p=15500,rep_out=13000,rec_in=20000,rec_out=20500,rep_in_r=26500):
    i=p['i']*disc; o=p['o']*disc
    prop=2*(prop_in*i+prop_out*o)/1e6
    prep=2*r*(rep_in_p*i+rep_out*o)/1e6
    rec=(rec_in*i+rec_out*o)/1e6
    rrep=r*(rep_in_r*i+rep_out*o)/1e6
    return prop+prep+rec+rrep
for k in ['opus5','opus55']:
    for r in (0.5,1,2,3.7):
        c=batch_ss(P[k],r); cs=batch_ss(P[k],r,disc=1.0)
        print('S5 %s repairs/agent=%.1f: batch $%.2f/seg ($%.0f for 4,073)  sync $%.2f/seg ($%.0f)'%(k,r,c,c*4073,cs,cs*4073))
# Batch lockstep agentic (keep loop; each round a batch; 1h cache at batch price), hit rate h
for k in ['opus5','opus55']:
    p=P[k]
    for h in (0.3,0.7,0.88,0.95,0.98):
        c=(R*(h*p['r']*0.5+(1-h)*p['w1']*0.5)+W*p['w1']*0.5+O*p['o']*0.5)/1e6
        print('S6 %s lockstep batch h=%.2f: $%.2f/seg (vs sync $%.2f)'%(k,h,c,cost(p)))
# Scope
for k in ['opus5','opus55']:
    print('scope 4,073 x profile at %s = $%.0f'%(k,4073*cost(P[k])))
# per-link
links_per_seg=50370/1748
print('links/segment %.1f ; $/banked link at opus5 profile %.3f'%(links_per_seg,cost(P['opus5'])/links_per_seg))
