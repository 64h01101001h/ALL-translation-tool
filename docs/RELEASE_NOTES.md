# Release notes

Written for the person installing the application, not for the people who
build it. The engineering record is `docs/`; this is what changes about your
day.

---

## First run

### One pause, once, and only if you use 84000 material

The application ships with the 84000 translation-memory **source** (27 MB) but
not its **search cache** (279 MB). The cache is built on your machine the first
time a Lookup touches 84000 material, and never again.

- **What you will see:** a wait cursor for about **6 seconds** — 5.65 s
  measured on an M-series Mac, building 400,745 rows.
- **When:** the first 84000 lookup, not at launch. If you never use 84000
  material, it is never built.
- **Afterwards:** nothing. The cache lives beside the other data and is reused.

**Why it is not simply shipped.** The cache is *derived* — every byte of it can
be rebuilt from the source file that ships with it. Putting it in the download
would add 279 MB to every copy, for every user, to save each of them six
seconds once. It is also a file that would go stale against its own source; a
cache that ships is a cache that can disagree with the thing it caches.

**If the six seconds is unwelcome**, do one 84000 lookup after installing and
it is done.

### The application runs fully offline

No account, no network call at startup, no telemetry. `docs/PRIVACY.md` states
what that means precisely, including what the diagnostic report does and does
not contain.

---

## Reporting a problem

**Help ▸ Report a Problem…** opens a message in your own mail application with
the build already filled in. Nothing is sent by the application — you see the
message and you send it.

**Help ▸ Save Diagnostic Report…** writes a text file you can attach. It
carries the build, the Qt version, the operating system and the *file types*
you have opened. It deliberately does **not** carry file titles, file contents
or your user name: on this project a text's title can itself be the sensitive
part.

---

## Your work

An update never touches what you made. `docs/DATA_MIGRATION.md` sets out what
that covers — glossaries, saved searches, versions, snippets, the roster and
your study record — and, just as importantly, the four things it does not
promise.
