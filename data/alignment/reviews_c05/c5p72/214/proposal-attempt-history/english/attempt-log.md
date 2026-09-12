Producer: Codex, English-first.
The first candidate-glossary SQL call failed before querying because literal `\n` escapes reached SQLite; stderr was `Error: in prepare, unrecognized token: "\\"`; the query was rerun successfully as `glossary-evidence-original.json`.
Generator attempt 01 was refused because the spec quoted lemma `'byung ba` where the source has `'byung bar`; exact stdout/stderr survive in `generator-attempt-01.*`.
The shared first harness then failed to record that exit because zsh reserves `status`; its shell stderr was `zsh:2: read-only variable: status`.
Generator attempt 02 was refused because the second object form was quoted as `gzung ba` rather than source `gzung bar`; exact exit/stdout/stderr survive in `generator-attempt-02.*`.
Generator attempt 03 used the unchanged canonical read-only runner and is the final successful output copied byte-for-byte to `body.html` and `generator.stdout`.
