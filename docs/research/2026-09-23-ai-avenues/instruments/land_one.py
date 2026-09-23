import sys, os
sys.path.insert(0, '/Users/adamderickandrade/ALL-translation-tool/tools')
import transfer_duplicate_segments as T
plan = T.measure()
keep = [i for i in plan["transferable_now"]
        if i["target"] == ["C07", 85]]
assert keep, "C07:85 not in the transferable set"
plan["transferable_now"] = keep
print("\nLANDING ONLY:", keep)
T.write_pages(plan)
