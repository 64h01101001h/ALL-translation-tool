// thl_wylie_runner.js — stdin/stdout wrapper around THL's wylie.js
// (Roger Espel Llima's EWTS converter, the JS port of THL Wylie.java —
// the reference EWTS implementation this whole ecosystem descends
// from). Reads one JSON-encoded wylie string per line, prints one JSON
// {u: unicode, w: [warnings]} per line. VALIDATION HARNESS ONLY — this
// code never runs in the app; our canonical engines stay authoritative.
//
//   node thl_wylie_runner.js /path/to/tibetan/wylie.js
const path = process.argv[2];
if (!path) {
    console.error("usage: node thl_wylie_runner.js <wylie.js>");
    process.exit(2);
}
const thl = require(require("path").resolve(path));
const readline = require("readline");
const rl = readline.createInterface({ input: process.stdin, terminal: false });
rl.on("line", (line) => {
    let wylie;
    try {
        wylie = JSON.parse(line);
    } catch {
        process.stdout.write("null\n");
        return;
    }
    const warns = [];
    let u = "";
    try {
        u = thl.fromWylie(wylie, warns);
    } catch (e) {
        process.stdout.write(JSON.stringify({ err: String(e) }) + "\n");
        return;
    }
    process.stdout.write(JSON.stringify({ u: u, w: warns }) + "\n");
});
