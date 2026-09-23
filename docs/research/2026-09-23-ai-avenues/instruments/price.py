# Profile A (measured, per segment, 3 agents): tokens
R=5_045_521   # cache reads  (5,398,830 - 248,612 - 104,580 - 117)
W=248_612     # cache writes (5-min)
I=117         # uncached input
O=104_580     # output
N=4073        # unique segments in mandate
# (input, cache_read, cache_write, output) $/MTok ; write=None -> billed at input rate
P = {
 'Claude Fable 5.1':      (10, 0.25, 12.5, 50),
 'Claude Opus 5 (incumbent)': (5, 0.50, 6.25, 25),
 'Claude Opus 5.5':       (4, 0.20, 5.0, 20),
 'Claude Sonnet 5':       (2, 0.20, 2.5, 10),
 'Claude Haiku 4.5':      (1, 0.10, 1.25, 5),
 'GPT-6 Astra':           (10, 1.00, None, 50),
 'GPT-5.6 Sol (promo)':   (4, 0.40, None, 20),
 'GPT-6 Sol':             (2, 0.20, None, 10),
 'GPT-6 Sol Flex':        (1, 0.10, None, 5),
 'GPT-6 Luna':            (0.10, 0.01, None, 0.50),
 'Gemini 3.1 Pro Preview':(2, 0.20, None, 12),
 'Gemini 3.1 Pro Flex':   (1, 0.10, None, 6),
 'Gemini 3.8 Flash (intro to 2026-12-31)': (0.75, 0.075, None, 3.75),
 'Gemini 3.8 Flash (from 2027-01-01)':     (1.50, 0.15, None, 7.50),
 'Grok 4.7':              (2, 0.50, None, 6),
 'DeepSeek V4-Pro off-peak': (0.66, 0.022, None, 1.98),
 'DeepSeek V4-Pro peak':  (1.32, 0.044, None, 3.96),
 'DeepSeek V4.1-Flash off-peak': (0.15, 0.003, None, 0.60),
 'Kimi K3':               (3, 0.30, 3.0, 15),
 'GLM-5.3':               (1.4, 0.26, None, 4.4),
 'Qwen3.8-Max (cache@20% assumed)': (2, 0.40, None, 6),
 'Mistral Medium 3.5':    (1.5, 0.15, None, 7.5),
 'Muse Spark 1.3 std':    (1.25, 0.15, None, 4.25),
 'MiMo-V2.6-Pro (3rd-party listing)': (0.435, 0.0036, None, 0.87),
}
def cost(p, R=R,W=W,I=I,O=O):
    i,r,w,o = p
    w = i if w is None else w
    parts = (R*r/1e6, W*w/1e6, I*i/1e6, O*o/1e6)
    return sum(parts), parts
print(f"{'model':40s} {'$/seg':>7s} {'reads':>6s} {'writes':>6s} {'out':>6s} {'x4073':>9s} {'vs Opus5':>8s}")
base = cost(P['Claude Opus 5 (incumbent)'])[0]
for k,p in P.items():
    c,parts = cost(p)
    print(f"{k:40s} {c:7.3f} {parts[0]:6.3f} {parts[1]:6.3f} {parts[3]:6.3f} {c*N:9,.0f} {c/base:8.2%}")
print()
# mixed configs: propose = 2/3 of profile, reconcile = 1/3 (ASSUMPTION)
def mix(prop, rec):
    return (2/3)*cost(P[prop])[0] + (1/3)*cost(P[rec])[0]
cfgs = [
 ('Claude Opus 5 (incumbent)','Claude Opus 5 (incumbent)'),
 ('Claude Opus 5 (incumbent)','Claude Fable 5.1'),
 ('Claude Opus 5.5','Claude Opus 5.5'),
 ('Claude Sonnet 5','Claude Opus 5.5'),
 ('Claude Sonnet 5','Claude Fable 5.1'),
 ('Claude Haiku 4.5','Claude Opus 5.5'),
 ('Gemini 3.1 Pro Preview','Claude Opus 5.5'),
 ('GPT-6 Sol','Claude Opus 5.5'),
 ('DeepSeek V4-Pro off-peak','Claude Opus 5.5'),
 ('GLM-5.3','Claude Opus 5.5'),
 ('Kimi K3','Claude Opus 5.5'),
]
for a,b in cfgs:
    c=mix(a,b); print(f"PROPOSE {a:28s} RECONCILE {b:26s} ${c:6.3f}/seg  ${c*N:9,.0f}  {c/base:6.1%}")
print()
# Profile B (ILLUSTRATIVE, post span-ID compiler, single-shot + 0.5 repair per agent, 3 agents)
RB, WB, IB, OB = 247_500, 3_000, 45_000, 54_000
for k in ['Claude Opus 5 (incumbent)','Claude Opus 5.5','Claude Sonnet 5','Claude Fable 5.1','GPT-6 Sol','Gemini 3.1 Pro Preview','DeepSeek V4-Pro off-peak','Gemini 3.8 Flash (intro to 2026-12-31)']:
    c,parts = cost(P[k],RB,WB,IB,OB)
    print(f"B {k:40s} ${c:6.3f}/seg  batch/flex~${c/2:6.3f}  x4073=${c*N:8,.0f}  batch ${c*N/2:8,.0f}")
