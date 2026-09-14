import json,sys
from pathlib import Path
assert __debug__ and sys.dont_write_bytecode
P=Path(__file__).resolve().parent
for seq in map(int,sys.argv[2:]):
 d=P/'reviewed-final'/str(seq)
 if sys.argv[1]=='spans':
  for r in json.loads((d/'final-decisions.json').read_text()):print(str(seq),r['final_span']['id'],r['rationale'])
 elif sys.argv[1]=='omissions':
  for r in json.loads((d/'omissions.json').read_text())['material']:print(str(seq),r['side'],[r['start'],r['end']],repr(r['text']),r['reason'])
 elif sys.argv[1]=='recasts':
  for r in json.loads((d/'original-dispositions.json').read_text()):
   if r.get('disposition')!='retained':print(json.dumps(r,ensure_ascii=False))
