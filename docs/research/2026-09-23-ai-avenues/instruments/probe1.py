import io
p='tools/build_dictionary_view.py'
s=io.open(p,encoding='utf-8').read()
old='    return sum(1 for r in refs if not is_xfer(r))'
assert s.count(old)==1
io.open(p,'w',encoding='utf-8').write(s.replace(old,'    return len(refs)',1))
print("HOLE PROBE 1 APPLIED: n_read() in build_dictionary_view.py redefined to len(refs) -- the marks stay, the ranking reverts to the pre-fix total")
