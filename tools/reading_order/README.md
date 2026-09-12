# Re-measuring the reading order

Two files, one command each. Run these when the alignment bank has grown, then
update the constants in `namespace walk` (app/walkthrough_pane.inc) and re-run
the app selftest — the pane's gates quote the constants, so stale UI text fails
rather than shipping.

    c++ -std=c++20 -I core/include -o /tmp/orderprobe \
        tools/reading_order/orderprobe.cpp -L build/core -lallcore -lsqlite3
    python3 tools/reading_order/measure.py

`orderprobe` exposes the **shipping** engine — `buildOverlay`, `splitClauses`,
`refineClauses`, `chunkClause`, `spotVerb`, `planReading`. It reimplements
nothing. Wylie reaches it through the proven `ewtsToAcip`. `measure.py`
recovers what Geshe Michael actually did from `data/alignment/alignment_full_v1.json`
and scores the engine against the only baseline that matters: read the chunks
left to right and change nothing.

**The probe walks every clause.** Version 1 walked only `cls[0]`, which left
523 of 730 spans unmappable — a child sitting in a second clause had no chunk
to map onto — and the figures that came out of it were an artefact of the
harness. If you change the probe, check `unmappable` in the output before
believing anything else it says.
