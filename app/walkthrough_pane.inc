// walkthrough_pane.inc — "Walk me through it".
//
// Adam, 2026-09-12: "my main problem in reading/translating Tibetan is
// understanding in what order to read the chunks and which direction to read
// the chunks", and then: "we need a tool that will take input, say a paragraph
// or so of text, and our tool should break down how to read/translate the
// paragraph step by step."
//
// Every engine this needs already existed — splitClauses, refineClauses,
// chunkClause, spotVerb, planReading, the lattice, the corpus. What did not
// exist was the thing that puts them in a row and walks a paragraph down
// them. planReading in particular was reachable only as an after-the-fact
// reveal inside a drill that asks a DIFFERENT question (restore the TIBETAN
// order of scrambled chunks), which never teaches the Tibetan-to-English
// mapping at all.
//
// The honesty problem, measured before this pane was written. Steps 1-3 and 5
// are checkable: they come from the dictionary and the particle tables and
// they cite their evidence. Step 4 -- the reading order -- is a RULING, and
// the ruling is not uniformly good. Checked against Geshe Michael's own
// translations by way of the alignment bank (docs/READING_ORDER_MEASUREMENT.md),
// against the only baseline that matters -- just reading left to right:
//
//     level                 spans   engine   left-to-right
//     segment -> clauses      425      83%        90%
//     clause  -> phrases      112      51%        59%
//     phrase  -> sub-phrases   18      61%        44%
//     sub     -> words        169      36%        23%
//     ALL                     724      67%        68%
//
// The engine is WORSE THAN DOING NOTHING at the top two levels and clearly
// better at the bottom two, and over everything it is a tie. Clause order is
// essentially never inverted -- a second, independent measurement over 972
// clause pairs puts it at 5% backwards -- so reordering clauses can only break
// what was already right.
//
// This pane's step 4 operates at clause -> phrases: the level where the
// engine scores 40% and doing nothing scores 60%. Shipping that ruling as
// advice, unqualified, would be banking a falsehood (Rule 10). So step 4
// shows BOTH orders -- the engine's and the written one -- names the
// measurement for the level, and says outright which one the evidence
// favours. When the pane's own engine is the weaker of the two, the pane
// says so. Fixing planReading is on the punchlist; misreporting it while it
// is unfixed is not an option.
//
// Nothing here composes English. Step 6 assembles HGM's own glosses in the
// engine's order and says so; step 7 shows his actual translation when the
// passage is in the corpus. Rule 1 holds: match, never compose.
namespace walk {

// Recovered from the alignment bank; see docs/READING_ORDER_MEASUREMENT.md.
// Named constants so the pane, the doc and the gate all quote one number and
// a change to the measurement cannot leave stale text behind.
// Re-measured 2026-09-12 after the probe was fixed to walk EVERY clause
// instead of only the first. The first pass left 523 of 730 spans unmappable
// -- any child sitting in a second clause had no chunk to map onto -- so its
// figures were an artefact of my harness, not of the data. 724 of 730 map now.
// The shape of the finding survived the correction; the magnitudes did not.
inline constexpr int kOrderSpans = 724;   // spans where GMR's order was recoverable
inline constexpr int kOrderAgree = 482;   // ... and the engine matched it
inline constexpr int kOrderPlain = 495;   // ... and plain left-to-right matched it
// The level this pane's step 4 works at: clause -> phrases.
inline constexpr int kClauseSpans = 112;
inline constexpr int kClauseEngine = 51;  // per cent
inline constexpr int kClausePlain = 59;   // per cent
// The bottom level, where the ruling earns its keep.
inline constexpr int kWordEngine = 36;    // per cent
inline constexpr int kWordPlain = 23;     // per cent
// McNemar on the paired comparison -- only spans where the two orders DISAGREE
// carry information, and at this level there are 21 of them. The 51-vs-59 gap
// is a trend, not a finding, and the pane must not spend it as one.
inline constexpr int kClauseDiscordant = 21;   // 6 engine-only, 15 written-only
// Clause order itself, from the independent pair measurement.
inline constexpr int kClausePairs = 972;
inline constexpr int kClauseBackPct = 5;

inline QString pct() {
    return QString::number(100.0 * kOrderAgree / kOrderSpans, 'f', 0) + "%";
}

// ACIP, EWTS wylie, or Tibetan script — all three arrive by paste, and a
// reader who has just copied a line out of a PDF should not have to know
// which one they have.
struct Detected {
    std::string acip;
    QString how;
    bool ok = true;
};

inline Detected detect(const QString& raw) {
    Detected d;
    bool tib = false;
    for (QChar c : raw)
        if (c.unicode() >= 0x0F00 && c.unicode() <= 0x0FFF) { tib = true; break; }
    if (tib) {
        const std::string w = allcore::unicodeToWylie(raw.toStdString()).wylie;
        if (w.empty()) { d.ok = false; d.how = "Tibetan script (could not be read back to wylie)"; return d; }
        d.acip = allcore::ewtsToAcip(w);
        d.how = "Tibetan script";
        return d;
    }
    int upper = 0, letters = 0;
    for (QChar c : raw)
        if (c.isLetter()) { ++letters; if (c.isUpper()) ++upper; }
    if (letters == 0) { d.ok = false; d.how = "no letters"; return d; }
    if (upper * 10 > letters * 6) {
        d.acip = raw.toStdString();
        d.how = "ACIP";
    } else {
        d.acip = allcore::ewtsToAcip(raw.toStdString());
        d.how = "EWTS wylie";
    }
    return d;
}

inline QString esc(const std::string& s) {
    return QString::fromStdString(s).toHtmlEscaped();
}

// The tokens of a span, joined — used for every chunk and clause display.
inline QString slice(const allcore::OverlayDoc& doc, int beg, int end) {
    QString out;
    for (int t = beg; t < end && t < (int)doc.tokens.size(); ++t)
        out += (out.isEmpty() ? "" : " ") + QString::fromStdString(doc.tokens[t]);
    return out.toHtmlEscaped();
}

// The longest span starting at `t` that actually carries a binding gloss.
//
// This began as "the longest span starting at t", which is wrong in a way the
// structural gates could not see: the lattice matches SANGS RGYAS KYIS as a
// span, that span has an entry, and the entry has no hgm_gloss — so the
// picker took it, reported "no Geshe Michael equivalent banked" for a term
// that is curated as "Buddha", and step 6 fell through to single syllables
// and emitted <CHOS> mentioned | <SANGS> grow with. Twenty gates passed on
// that output, because every one of them checked the SHAPE of the page and
// none checked whether a word it knows came back known. Length is the
// tiebreak; carrying a gloss is the requirement.
//
// Returns -1 when nothing in [t, limit) is glossed, and the caller flags the
// syllable rather than approximating it (rule 3).
inline int bestGlossSpan(const allcore::OverlayDoc& doc, int t, int limit) {
    int best = -1, bestLen = 0;
    for (int si : doc.spansAt(t)) {
        const auto& sp = doc.spans[si];
        if (sp.beg != t || sp.end > limit || sp.entry_ix < 0) continue;
        if (doc.entries[sp.entry_ix].hgm_gloss.empty()) continue;
        const int len = sp.end - sp.beg;
        if (len > bestLen) { bestLen = len; best = si; }
    }
    return best;
}

}  // namespace walk

class WalkthroughPane : public QWidget {
public:
    explicit WalkthroughPane(allcore::Spine& spine) : spine_(spine) {
        auto* outer = new QVBoxLayout(this);
        auto* banner = new QLabel(
            "<b>Walk me through it</b> — paste a paragraph and this lays out how to "
            "read it: where it breaks, what the verb is, what the chunks are, "
            "<i>what order to read them in</i>, what each word means, and — when the "
            "passage is in the corpus — Geshe Michael's own translation of it.");
        banner->setWordWrap(true);
        outer->addWidget(banner);

        auto* split = new QSplitter(Qt::Horizontal);
        auto* left = new QWidget;
        auto* ll = new QVBoxLayout(left);
        ll->addWidget(new QLabel("<b>Paragraph</b> — ACIP, wylie, or Tibetan script"));
        src_ = new QPlainTextEdit;
        src_->setPlaceholderText(
            "Paste a paragraph here.\n\nACIP (SANGS RGYAS KYIS CHOS BSTAN), EWTS wylie "
            "(sangs rgyas kyis chos bstan), or Tibetan script — it works out which.");
        sess::remember(src_, "walk/source");
        ll->addWidget(src_, 1);
        auto* row = new QHBoxLayout;
        go_ = new QPushButton("Walk me through it");
        go_->setDefault(true);
        row->addWidget(go_);
        detect_ = new QLabel;
        detect_->setWordWrap(true);
        row->addWidget(detect_, 1);
        ll->addLayout(row);
        split->addWidget(left);

        out_ = new QTextBrowser;
        out_->setOpenExternalLinks(false);
        split->addWidget(out_);
        split->setStretchFactor(0, 2);
        split->setStretchFactor(1, 5);
        outer->addWidget(split, 1);

        connect(go_, &QPushButton::clicked, this, [this] { walk(); });
        idle();
    }

    // --selftest: drive the pane the way a reader does and check what it says
    int selfTest(QStringList& log) {
        int fails = 0;
        auto check = [&](bool ok, const char* what) {
            log << QString("  [%1] Walkthrough: %2")
                       .arg(ok ? "PASS" : "FAIL").arg(what);
            if (!ok) ++fails;
        };
        const QString keep = src_->toPlainText();

        {   // an empty box restores the instruction. The Draft pane shipped
            // for weeks with a middle column that replaced its own
            // instruction with a bare "no clauses" and never put it back;
            // this pane is not going to repeat it.
            src_->setPlainText("");
            walk();
            const QString t = out_->toPlainText();
            check(t.contains("Paste a paragraph on the left"),
                  "an empty box shows the instruction, not a blank page");
        }
        {   // input with no Tibetan in it says so rather than walking nothing
            src_->setPlainText("12345 !!!");
            walk();
            check(out_->toPlainText().contains("Nothing to walk through"),
                  "input with no Tibetan says so instead of rendering an "
                  "empty walkthrough");
        }
        {   // a real passage: every step present, one section per clause
            src_->setPlainText("sangs rgyas kyis chos bstan te, "
                               "'phags pa'i bden pa bzhi la slob");
            walk();
            const QString t = out_->toPlainText();
            for (int i = 1; i <= 7; ++i)
                check(t.contains(QString("Step %1 ").arg(i)),
                      i == 1 ? "a real passage walks step 1"
                             : i == 4 ? "a real passage walks step 4 (the order)"
                                      : "a real passage walks every step");
            check(clauses_ >= 2, "the passage was cut into its clauses");
            check(t.count("Clause ") >= clauses_,
                  "one section per clause, not one for the paragraph");
        }
        {   // THE gate. Step 4 must show the written order beside the
            // engine's, because at this level the measurement says the
            // written order came out ahead, though not decisively. A later
            // edit that "simplifies" step 4 down to the engine's ruling
            // alone would be presenting the weaker of two measured options
            // as the answer, and this fails if it happens.
            src_->setPlainText("sangs rgyas kyis chos bstan");
            walk();
            const QString t = out_->toPlainText();
            check(t.contains("the engine says") && t.contains("as written"),
                  "step 4 shows BOTH orders — the engine's and the written one");
            check(t.contains("the simpler bet"),
                  "step 4 names which column to try first");
            // Rule 10, applied to my own hedging as well as my own claims.
            // The pane said the written order was "the better bet" on a
            // 51-vs-59 split. McNemar on the 21 spans that actually separate
            // the two gives p=0.08 — a trend, not a finding. Overstating in
            // the direction of humility is still overstating, so the pane now
            // says the difference does not hold up and this gate keeps it
            // saying so.
            check(t.contains("does not hold up"),
                  "step 4 admits the middle-level gap is not statistically "
                  "established rather than spending a trend as a finding");
            check(t.contains("p&lt;0.0001") || t.contains("p<0.0001"),
                  "and it cites what IS established \u2014 the clause-level "
                  "result, which is decisive");
            check(t.contains(QString::number(kEngineHere) + "%") &&
                      t.contains(QString::number(kPlainHere) + "%"),
                  "step 4 quotes the measured figures for its own level");
            check(t.contains("do not shuffle them"),
                  "the reader is told to leave the clause order alone");
        }
        {   // Preston's eight elements. Checked against HIS OWN example
            // sentence, xvii: "Buddha taught the doctrine in India" — agent in
            // the 3rd, qualifier of place in the la don, object in the 1st,
            // transitive verb ending the clause.
            src_->setPlainText("sangs rgyas kyis rgya gar la chos bstan");
            walk();
            const QString t = out_->toPlainText();
            check(t.contains("third case") && t.contains("AGENT"),
                  "cases: kyis is named as the third case marking the agent");
            check(t.contains("first case") &&
                      t.contains("object of the transitive verb"),
                  "cases: the bare object is named as the FIRST case, not as "
                  "a missing particle");
            // The la-family gate. Preston reads this one as 2nd from the
            // relationship; the particle alone cannot, and the pane must not
            // pretend it can.
            check(t.contains("subject readings are out because the verb is "
                             "transitive"),
                  "cases: the pane says WHY a reading was excluded, not just "
                  "a shorter list");
            {   // The two panes must not contradict each other. Wilson
                // Parse resolves a la don from the verb class; this pane used
                // to print "2/4/7 — the particle cannot tell you which" about
                // the very chunk that pane called "2nd (destination)".
                src_->setPlainText("rgya gar du 'gro");
                walk();
                const QString n = out_->toPlainText();
                check(n.contains("narrowed by the VERB, not by the particle"),
                      "la don: a verb that CAN narrow the case does, and the "
                      "cell says the verb is what did it");
                check(!n.contains("2/4/7"),
                      "la don: and it no longer offers all three where the "
                      "verb has already ruled");
            }
            check(t.contains("2/4/7") && t.contains("cannot tell you which"),
                  "cases: a la don particle reports 2nd/4th/7th and says the "
                  "particle cannot choose between them");
            check(t.contains("transitive verb, ending the clause"),
                  "cases: the verb is split out of the predicate chunk, as "
                  "Preston separates it from its object");
            // Belongs to THIS fixture — bstan is in Wilson's tables as class
            // V. I split this block to add the unknown-class case below and
            // left this assertion behind in the new one, where the verb has
            // no class and the line it looks for correctly never appears.
            check(t.contains("an agent, not a subject"),
                  "cases: a transitive verb is said to take an agent, in "
                  "Preston's exclusive sense of the word");
        }
        {   // A verb spotted from its banked tenses but ABSENT from Wilson's
            // class tables. The pane used to print "the intransitive verb,
            // ending the clause" here — a positive false claim on no
            // evidence, and false in fact: Preston's p.217 table lists bskyed
            // as trans. v. (ag-nom), produce/generate/create. It printed that
            // directly beneath a row reading "third case — marks the AGENT of
            // a transitive verb", so the pane contradicted itself inside one
            // table.
            //
            // "Attested" and "classified" are separate facts about a verb and
            // the display may not fuse them. A two-branch ternary standing in
            // for a three-state fact is what did.
            src_->setPlainText("bdag gis sems bskyed");
            walk();
            const QString t = out_->toPlainText();
            check(!t.contains("the intransitive verb, ending the clause"),
                  "unknown class: the pane never calls a verb intransitive "
                  "merely because Wilson's tables do not have it");
            check(t.contains("transitivity is unknown"),
                  "unknown class: it says the transitivity is unknown, which "
                  "is the fact");
            check(!t.contains("complement / subject material"),
                  "unknown class: and the row above does not quietly drop the "
                  "object reading either");
        }
        {   // An UNVERIFIED candidate must not read as "a verb is here".
            // spotVerb always returns the last dictionary word, so a verbless
            // clause gets a named candidate exactly like a real verb the
            // glossary happens not to mark. Preston's vol.2 index carries
            // sixteen page references for implied verbs; they are not an edge
            // case.
            //
            // UNCONDITIONAL, deliberately. My first version wrapped these in
            // `if (t.contains("UNVERIFIED"))` and hung them on the bskyed
            // fixture — which is ATTESTED, so the condition never held and
            // both gates silently skipped while reporting nothing. That is
            // the same shape as the clauses gate earlier in this file: a
            // check that can decline to run is a check that agrees with the
            // bug. This asserts the fixture produces the state first.
            src_->setPlainText("chos thams cad mi rtag");
            walk();
            const QString u = out_->toPlainText();
            check(u.contains("UNVERIFIED"),
                  "implied verb: the fixture really does produce an "
                  "unverified candidate \u2014 without this the two gates "
                  "below could pass by never running");
            check(u.contains("may have NO verb at all"),
                  "implied verb: an unverified candidate says the clause may "
                  "have no verb at all, rather than naming a word and leaving "
                  "the reader to assume one is present");
            check(u.contains("last resort"),
                  "implied verb: and calls the candidate what it is");
            // and the caution, on a transitive clause with the agent removed
            src_->setPlainText("chos bstan");
            walk();
            {   // dang under a conjunctive verb: not a case, but our own
                // verbclass table says it completes the verb. The column used
                // to print an em dash, throwing away a fact we held.
                src_->setPlainText("chos dang 'brel");
                walk();
                const QString d = out_->toPlainText();
                check(d.contains("qualifier of the verb") &&
                          d.contains("syntactic particle, not by a case"),
                      "cases: dang under a conjunctive verb is named as the "
                      "qualifier, marked by a syntactic particle");
                src_->setPlainText("chos ni bstan");
                walk();
                check(out_->toPlainText().contains("not a case particle"),
                      "cases: and a particle no Wilson class claims says "
                      "\"not a case particle\", not a bare dash that reads as "
                      "nothing known");
            }
            {   // Preston vol.2 states on four pages that a normally
                // transitive verb can be used with a nom-loc syntax. The pane
                // announced "agent, not subject" as settled by the class.
                src_->setPlainText("dang po gnyis la don smra ba zer");
                walk();
                const QString a = out_->toPlainText();
                check(a.contains("ATTRIBUTIVE SYNTAX is open"),
                      "attributive: a transitive verb with no agent and a la "
                      "don chunk offers the attributive reading");
                check(a.contains("both are live"),
                      "attributive: and says both readings are live rather "
                      "than picking one");
                // With an agent present it must NOT be offered.
                src_->setPlainText("sangs rgyas kyis rgya gar la chos bstan");
                walk();
                check(!out_->toPlainText().contains("ATTRIBUTIVE SYNTAX is open"),
                      "attributive: not offered when the agent is right there "
                      "in the third case");
                // Put back the fixture the NEXT check needs. Twice today an
                // inserted block has left a different passage loaded and
                // stranded the assertion after it — the first time the gate
                // failed honestly, which is the only reason I found it. A
                // block that changes the fixture restores it.
                src_->setPlainText("chos bstan");
                walk();
            }
            check(out_->toPlainText().contains("agent is UNSTATED"),
                  "cases: a transitive verb with no third-case chunk warns "
                  "that the agent is unstated — absence of an agent is not "
                  "evidence of an intransitive construction");
        }
        {   // Rule 3: a word the glossary does not have is flagged, never
            // guessed; and step 6 never calls itself a translation.
            src_->setPlainText("sangs rgyas kyis chos bstan");
            walk();
            const QString t = out_->toPlainText();
            check(t.contains("not a translation"),
                  "step 6 refuses the word \"translation\" for its own output "
                  "(match, never compose)");
            // Rule 3 proper: a syllable the glossary has never seen must
            // come back in angle brackets, not silently dropped and not
            // approximated. "zzzz" is not a Tibetan word and nothing in the
            // spine can gloss it.
            src_->setPlainText("sangs rgyas zzzz bstan");
            walk();
            const QString u = out_->toPlainText();
            check(u.contains("\u27E8") && u.contains("\u27E9"),
                  "a syllable the glossary does not have comes back in angle "
                  "brackets \u2014 flagged, never guessed (rule 3)");
            check(u.contains("not in the glossary"),
                  "and step 5 says in words that it does not know it");
        }
        {   // G2 — the gate the constitution fired on. Step 6 was assembling
            // provisional glosses into a running line with nothing to
            // distinguish them, so a line made entirely of auto-aligned
            // material read exactly like one made of curated material. Every
            // gloss the pane renders now carries its tier, at the point of
            // use, not only in the list above it.
            src_->setPlainText("sangs rgyas kyis chos bstan");
            walk();
            const QString t = out_->toPlainText();
            check(t.contains("[GMR ") || t.contains("[PROVISIONAL") ||
                      t.contains("[no tier recorded]"),
                  "every gloss in step 5 renders with its tier, not just the "
                  "provisional ones");
            check(!t.contains("[GMR auto-aligned]"),
                  "an auto-aligned gloss is never dressed as a GMR tier \u2014 "
                  "it reads PROVISIONAL");
        }
        {   // CONTENT, not shape. The first twenty gates on this pane all
            // passed while step 5 reported "no Geshe Michael equivalent
            // banked" for sangs rgyas — curated as "Buddha" — and step 6
            // emitted <CHOS> mentioned | <SANGS> grow with. The picker was
            // taking the longest span starting at the token whether or not it
            // carried a gloss, and SANGS RGYAS KYIS is such a span. Every
            // gate checked the shape of the page; none asked whether a word
            // the glossary knows came back known. These do.
            src_->setPlainText("sangs rgyas kyis chos bstan");
            walk();
            const QString t = out_->toPlainText();
            check(t.contains("Buddha"),
                  "sangs rgyas comes back as Buddha \u2014 a curated term is "
                  "not reported as unglossed because a longer gloss-less span "
                  "starts at the same syllable");
            check(t.contains("Dharma"), "chos comes back as Dharma");
            check(t.contains("[GMR curated]"),
                  "and the curated tier is named, so the reader can see the "
                  "weight the line carries");
            // Every syllable of this clause is in the glossary, so step 6 must
            // not be flagging any of them. An angle bracket here means the
            // picker has fallen back to syllables again.
            const int s6 = t.indexOf("Step 6");
            const int s7 = t.indexOf("Step 7", s6 < 0 ? 0 : s6);
            const QString six = (s6 >= 0 && s7 > s6) ? t.mid(s6, s7 - s6) : QString();
            // Scope the bracket check to the ASSEMBLED LINE only. The
            // disclaimer under it explains what angle brackets mean and so
            // contains a pair of its own — my first version of this gate read
            // the whole block and failed on the explanation, which is the
            // gate being wrong rather than the pane.
            const QStringList lines = six.split('\n', Qt::SkipEmptyParts);
            const QString assembledLine = lines.size() > 1 ? lines.at(1) : QString();
            check(!assembledLine.isEmpty() &&
                      !assembledLine.contains(QChar(0x27E8)),
                  "step 6 flags nothing in a clause whose every word is "
                  "glossed \u2014 no silent fallback to single syllables");
            check(six.contains("FIRST one"),
                  "step 6 admits it took the FIRST of several equivalents, not "
                  "the best one");
        }
        src_->setPlainText(keep);
        return fails;
    }

    // the constants the gate quotes, kept next to the gate that quotes them
    static constexpr int kEngineHere = walk::kClauseEngine;
    static constexpr int kPlainHere = walk::kClausePlain;

    void setSourceForTest(const QString& s) { src_->setPlainText(s); }
    void walkForTest() { walk(); }
    QString outHtml() const { return out_->toHtml(); }
    QString outText() const { return out_->toPlainText(); }

private:
    void idle() {
        out_->setHtml(
            "<div style='padding:10px;color:#666'>"
            "<p><b>Paste a paragraph on the left and press the button.</b></p>"
            "<p>You will get one section per clause, and inside each:</p>"
            "<ol>"
            "<li>where the sentence breaks, and on what</li>"
            "<li>which word is the verb, and the evidence for saying so</li>"
            "<li>the chunks and the role each one plays</li>"
            "<li><b>what order to read them in</b></li>"
            "<li>what each word means, from Geshe Michael's glossary</li>"
            "<li>those meanings put together in that order</li>"
            "<li>his own translation, when the passage is in the corpus</li>"
            "</ol>"
            "<p style='padding-top:6px'>Steps 1&ndash;3 and 5 cite their evidence. "
            "Step 4 is the engine's ruling and is labelled as one wherever it "
            "appears — the pane will tell you what it is worth.</p></div>");
    }

    void walk() {
        const QString raw = src_->toPlainText().trimmed();
        if (raw.isEmpty()) { detect_->clear(); idle(); return; }
        const walk::Detected det = walk::detect(raw);
        detect_->setText(det.ok ? "read as <b>" + det.how + "</b>"
                                : "<span style='color:#B4540A'>could not read this: " +
                                      det.how + "</span>");
        if (!det.ok) {
            out_->setHtml("<div style='padding:10px'><b>Nothing to walk through.</b>"
                          "<div style='padding-top:6px;color:#666'>The box has no "
                          "Tibetan in it that this could read — " +
                          det.how.toHtmlEscaped() +
                          ". Paste ACIP, EWTS wylie, or Tibetan script.</div></div>");
            return;
        }

        auto doc = allcore::buildOverlay(spine_, det.acip);
        auto clauses = allcore::splitClauses(doc.tokens, doc.barrier_after);
        std::vector<int> merged_na;
        clauses = allcore::refineClauses(doc, clauses, &merged_na);
        clauses_ = (int)clauses.size();

        QString h;
        h += header(det, (int)doc.tokens.size(), clauses, merged_na);
        for (size_t i = 0; i < clauses.size(); ++i)
            h += oneClause(doc, clauses[i], (int)i + 1, (int)clauses.size());
        h += "<hr><div style='color:#666;font-size:11px;padding:6px 0'>"
             "Nothing on this page was composed. Every English word above is "
             "Geshe Michael's, taken from his glossary or his translation; the "
             "engine supplied only the order, and said where it did.</div>";
        out_->setHtml(h);
    }

    QString header(const walk::Detected& det, int tokens,
                   const std::vector<allcore::Clause>& cls,
                   const std::vector<int>& merged_na) {
        QString h;
        h += "<div style='padding:4px 0'><b>" + QString::number(tokens) +
             "</b> syllables · <b>" + QString::number(cls.size()) +
             "</b> clause(s) · read as " + det.how.toHtmlEscaped() + "</div>";
        // Two things the reader has to know before step 4 means anything:
        // the clause order above it is trustworthy, and step 4 itself is not
        // yet as good as leaving the phrases alone.
        h += "<div style='border-left:3px solid #3B7A3B;padding:6px 10px;"
             "margin:8px 0;background:rgba(59,122,59,0.06)'>"
             "<b>The clauses above are already in reading order.</b> Tibetan "
             "clause order survives into English almost intact — across " +
             QString::number(walk::kClausePairs) +
             " measured clause pairs in Geshe Michael's own translations, only " +
             QString::number(walk::kClauseBackPct) +
             "% come out backwards. So take the clauses in the order they are "
             "written and do not shuffle them. The work is inside each one.</div>";
        h += "<div style='border-left:3px solid #B4540A;padding:6px 10px;"
             "margin:8px 0;background:rgba(180,84,10,0.06)'>"
             "<b>How much to trust step 4 — less than you would like.</b> The "
             "reading order inside a clause is the engine's ruling from the "
             "particle tables, not a fact from the dictionary. Measured against "
             "Geshe Michael's own translations at exactly this level (" +
             QString::number(walk::kClauseSpans) + " spans), the engine matched "
             "his order <b>" + QString::number(walk::kClauseEngine) +
             "%</b> of the time and simply reading the phrases left to right "
             "matched it <b>" + QString::number(walk::kClausePlain) +
             "%</b> — but only " + QString::number(walk::kClauseDiscordant) +
             " spans separate them, and at that count the difference does not "
             "hold up (McNemar p=0.08). So step 4 shows you both and neither "
             "is established here; the honest posture is to try the written "
             "order first because it is simpler, not because it is proven. "
             "What IS established sits either side of this level: at the "
             "clause level the written order wins decisively (p&lt;0.0001, and "
             "reordering clauses is the one thing you should not do), and "
             "deeper down — inside "
             "a phrase, among the words — the ruling wins decisively, " +
             QString::number(walk::kWordEngine) + "% against " +
             QString::number(walk::kWordPlain) +
             "% (p=0.003). Fixing the middle level is on the punchlist, and "
             "so is measuring it properly; reporting either as better than it "
             "is, is not an option.</div>";
        if (!merged_na.empty())
            h += "<div style='color:#B4540A;padding:4px 0'>" +
                 QString::number(merged_na.size()) +
                 " clause break(s) at <i>na</i> were dropped because the word "
                 "before carried no verb evidence. That is a ruling made on "
                 "ABSENT evidence, not on evidence that the word is a noun — so "
                 "the split may belong there after all.</div>";
        return h;
    }

    QString oneClause(const allcore::OverlayDoc& doc, const allcore::Clause& c,
                      int n, int of) {
        auto chunks = allcore::chunkClause(doc, c);
        auto verb = allcore::spotVerb(doc, chunks);
        auto plan = allcore::planReading(chunks, verb);

        QString h;
        h += "<hr><div style='padding:6px 0'><b style='font-size:14px'>Clause " +
             QString::number(n) + " of " + QString::number(of) + "</b></div>";
        h += "<div style='font-family:monospace;padding:2px 0 8px 0'>" +
             walk::slice(doc, c.beg, c.end) + "</div>";

        // ---- step 1 ----
        h += step(1, "Where it ends");
        if (c.boundary.empty())
            h += body("the end of the text — nothing closed it");
        else
            h += body("closed by <b>" + walk::esc(c.boundary) + "</b>" +
                      (c.boundary_function[0]
                           ? QString(" — ") + QString(c.boundary_function).toHtmlEscaped()
                           : QString()));

        // ---- step 2 ----
        h += step(2, "The verb");
        if (verb.chunk < 0) {
            h += body("<span style='color:#B4540A'>no verb found.</span> The "
                      "clause may be a noun phrase, or the verb may not be in "
                      "the glossary.");
        } else {
            QString e = "<b>" + walk::esc(verb.wylie) + "</b>";
            if (verb.confident)
                e += " <span style='color:#3B7A3B'>ATTESTED</span> — " +
                     walk::esc(verb.evidence);
            else
                // "A candidate, not a finding" was honest about the WORD
                // and silent about the clause. spotVerb always returns
                // something — the last dictionary word in the final chunk —
                // so a clause with no verb in it at all still gets a named
                // candidate, and the reader has no way to tell that case
                // apart from a real verb the glossary happens not to mark.
                //
                // Tibetan omits verbs. Preston's vol. 2 index carries sixteen
                // page references for implied verbs — twelve for the implied
                // linking verb, four for the implied verb of existence and
                // possession — and his p.60 diagram shows an implied yod
                // carrying a class (nom-loc) it is not present to declare.
                // So "no verb here, one is implied" is a live reading of an
                // unverified candidate, and a frequent one.
                // (Preston vol. 2 p.60 and the Index of Diagrammed Syntactic
                // Elements, 2026-09-13.)
                e += " <span style='color:#B4540A'>UNVERIFIED</span> — the last "
                     "dictionary word in the final chunk. Nothing in the "
                     "glossary says it is a verb; it is a candidate, not a "
                     "finding.<br><span style='color:#B4540A'>And the clause "
                     "may have NO verb at all.</span> Tibetan omits them "
                     "freely, most often a verb of existence or a linking "
                     "verb, and an implied verb leaves nothing on the page for "
                     "this step to find. A named candidate here is the "
                     "engine's last resort, not evidence that a verb is "
                     "present.";
            if (verb.cls) {
                e += "<br>Wilson class: <b>" +
                     QString::fromUtf8(verb.cls->label).toHtmlEscaped() +
                     "</b> — " + QString::fromUtf8(verb.cls->expects).toHtmlEscaped();
                // Preston uses "subject" only of intransitive constructions
                // and "agent" only of transitive ones, against ordinary
                // English usage, because it makes the transitivity visible in
                // the label itself. Wilson's class names already carry the
                // split (agentive- against nominative-), so this reads it off
                // the class rather than deciding anything.
                using T = allcore::Transitivity;
                const T tr = allcore::transitivityOf(verb.cls->cls);
                if (tr == T::Specialized) {
                    e += QString("<br>This is <b>%1</b>.")
                             .arg(QString(allcore::specializedNote(
                                              verb.cls->cls))
                                      .toHtmlEscaped());
                } else {
                    e += QString("<br>This is a <b>%1</b> verb, so the clause "
                                 "has an <b>%2</b>, not a %3. (Preston keeps "
                                 "the two words apart on purpose: %2 belongs "
                                 "to %1 constructions only.)")
                             .arg(tr == T::Transitive ? "transitive"
                                                      : "intransitive")
                             .arg(allcore::actorWord(verb.cls->cls))
                             .arg(tr == T::Transitive ? "subject" : "agent");
                }
            }
            h += body(e);
        }

        // ---- step 3 ----
        h += step(3, "The chunks, and what each one is doing");
        // Preston's eight basic syntactic elements (How to Read Classical
        // Tibetan I, p. xv and the grammar review p. 59, in the Hopkins /
        // Napper / Wilson line). The pane used to print the particle and a
        // role phrase; his students are drilled to say which of eight
        // FUNCTIONS the chunk fills and which CASE it is declined into, and
        // that is a strictly more useful thing to be told.
        h += "<table cellspacing='0' cellpadding='4' style='margin:2px 0'>";
        h += "<tr><td></td><td style='color:#888;font-size:11px'>chunk</td>"
             "<td style='color:#888;font-size:11px'>particle</td>"
             "<td style='color:#888;font-size:11px'>case</td>"
             "<td style='color:#888;font-size:11px'>particle note</td>"
             "<td style='color:#888;font-size:11px'>what it could be doing"
             "</td></tr>";
        bool sawThird = false, sawLaGroup = false;
        for (size_t i = 0; i < chunks.size(); ++i) {
            const auto cr = allcore::caseOf(chunks[i].marker);
            for (int c : cr.cases) if (c == 3) sawThird = true;
            if (cr.cases.size() == 3 && cr.cases[0] == 2) sawLaGroup = true;
            QString caseCell;
            if (!cr.known()) {
                caseCell = "<span style='color:#888'>—</span>";
            } else if (cr.ambiguous()) {
                // The particle cannot choose, but the VERB often can, and
                // wilsonParse has been resolving exactly this for far longer
                // than this pane has existed. Without asking, the Wilson Parse
                // pane says "7th (locative)" about the same chunk this one
                // calls "2/4/7 — the particle cannot tell you which": the tool
                // contradicting itself in front of one reader.
                //
                // The honesty is unchanged. The particle still cannot choose,
                // and the cell says so — it names the VERB as what narrowed
                // it. table_conformance holds this to wilsonParse's answer so
                // the two panes cannot drift apart again.
                const auto nar = allcore::narrowLaDon(chunks[i].marker, verb.cls);
                if (nar.narrowed()) {
                    caseCell = "<b>" + QString::number(nar.cases[0]) +
                               "</b> <span style='color:#666'>" +
                               QString(cr.family).toHtmlEscaped() +
                               " — narrowed by the VERB, not by the particle: " +
                               QString(nar.because).toHtmlEscaped() + "</span>";
                    for (int c : nar.cases) if (c == 3) sawThird = true;
                } else {
                    QString nums;
                    for (size_t z = 0; z < cr.cases.size(); ++z)
                        nums += (z ? "/" : "") + QString::number(cr.cases[z]);
                    // Nothing narrowed it. Printing one of the three would be
                    // a guess (rule 3); printing all three with the reason is
                    // what the grammar supports.
                    caseCell = "<b>" + nums + "</b> <span style='color:#B4540A'>"
                               "(the <i>" + QString(cr.family) + "</i> particles "
                               "decline into any of these three — the particle "
                               "cannot tell you which, the relationship does)"
                               "</span>";
                }
            } else {
                caseCell = "<b>" + QString::number(cr.cases.front()) +
                           "</b> <span style='color:#666'>" +
                           QString(cr.gloss).toHtmlEscaped() + "</span>";
                if (cr.caveat[0])
                    caseCell += " <span style='color:#B4540A;font-size:11px'>" +
                                QString(cr.caveat).toHtmlEscaped() + "</span>";
            }
            // Preston separates the object of a transitive verb from the
            // verb itself; our chunker does not, because it closes a chunk at
            // each case particle and the last chunk therefore swallows both.
            // On his own example sentence that produced one row reading
            // "CHOS BSTAN — predicate slot", where his analysis has two: the
            // object chos in the first case, and the transitive verb bstan.
            //
            // spotVerb already knows where the verb starts, so the row can be
            // split for display without touching chunkClause. Nothing in the
            // engine changes — the split is a rendering, and the deferred
            // engine correction is recorded in
            // docs/READING_ORDER_MEASUREMENT.md rather than attempted here.
            const bool isPredicate =
                chunks[i].marker.empty() && (int)i == (int)chunks.size() - 1;
            const bool splittable =
                isPredicate && verb.chunk == (int)i && verb.tok > chunks[i].beg &&
                verb.tok < chunks[i].end;
            const QString row1Text =
                splittable ? walk::slice(doc, chunks[i].beg, verb.tok)
                           : walk::slice(doc, chunks[i].beg, chunks[i].end);
            // Three states, not two. A two-branch ternary here stood in for
            // a THREE-state fact — transitive, intransitive, and unknown —
            // and collapsed the third into the second.
            const bool clsKnown = verb.cls != nullptr;
            const bool clsTrans = clsKnown && allcore::isTransitive(verb.cls->cls);
            const QString row1Role =
                splittable
                    ? (!clsKnown
                           ? QString("the material before the verb \u2014 what "
                                     "it is depends on the verb's transitivity, "
                                     "which is not known")
                           : clsTrans ? QString("object of the transitive verb")
                                      : QString("complement / subject material"))
                    : QString(chunks[i].role).toHtmlEscaped();
            h += "<tr><td style='color:#888'>[" + QString::number(i + 1) +
                 "]</td><td style='font-family:monospace'>" + row1Text +
                 "</td><td>" +
                 (chunks[i].marker.empty()
                      ? QString("<span style='color:#888'>no particle</span>")
                      : "<b>" + walk::esc(chunks[i].marker) + "</b>" +
                            (chunks[i].marker_fused ? " <span style='color:#888'>"
                                                      "(fused)</span>"
                                                    : QString())) +
                 "</td><td>" + caseCell + "</td><td>" + row1Role +
                 "</td><td>" + functionCell(cr, verb, chunks[i].marker) +
                 "</td></tr>";
            if (splittable)
                h += "<tr><td style='color:#888'>[" + QString::number(i + 1) +
                     "b]</td><td style='font-family:monospace'>" +
                     walk::slice(doc, verb.tok, chunks[i].end) +
                     "</td><td><span style='color:#888'>—</span></td>"
                     "<td><span style='color:#888'>—</span></td>"
                     "<td><span style='color:#888'>—</span></td><td>" +
                     // This asserted INTRANSITIVITY whenever Wilson's tables
                     // did not have the verb — a positive false claim, not an
                     // omission. On "bdag gis sems bskyed" the pane printed
                     // "the intransitive verb" directly beneath a row reading
                     // "third case — marks the AGENT of a transitive verb",
                     // and bskyed is listed in Preston's own p.217 table as
                     // trans. v. (ag-nom), produce/generate/create. The pane
                     // contradicted itself in one table and was wrong in the
                     // bargain.
                     //
                     // spotVerb can be CONFIDENT about a verb from its banked
                     // tenses while classifyVerb has no class for it, so
                     // "attested" and "classified" are separate facts and the
                     // display must not fuse them.
                     // (Adversarial pass over Preston, 2026-09-13.)
                     QString(!clsKnown
                                 ? "the verb, ending the clause \u2014 Wilson's "
                                   "tables do not have this one, so its "
                                   "transitivity is unknown"
                             : clsTrans
                                 ? "the transitive verb, ending the clause"
                                 : "the intransitive verb, ending the clause") +
                     "</td></tr>";
        }
        h += "</table>";
        // Preston's warning, and the one a machine most needs to hear: an
        // absent agent is not evidence of an intransitive construction.
        if (verb.cls) {
            const char* caution =
                allcore::unstatedAgentCaution(verb.cls->cls, sawThird);
            if (caution[0])
                h += "<div style='color:#B4540A;padding:4px 0 0 12px'>" +
                     QString(caution).toHtmlEscaped() + ".</div>";
            // The counterexample to this pane's central assumption, and the
            // reason step 2 must not announce "agent, not subject" as settled.
            const char* attr = allcore::attributiveSyntaxOpen(
                verb.cls, sawThird, sawLaGroup);
            if (attr[0])
                h += "<div style='color:#B4540A;padding:4px 0 0 12px'>" +
                     QString(attr).toHtmlEscaped() +
                     ". <span style='font-size:11px'>(Preston vol. 2 pp.60, "
                     "70, 163, 200)</span></div>";
        }

        // ---- step 4 ----
        h += step(4, "What order to read them in");
        h += "<div style='color:#B4540A;font-size:11px;padding:0 0 4px 0'>"
             "the engine's ruling. At this level it matched Geshe Michael " +
             QString::number(walk::kClauseEngine) + "% of the time and the "
             "written order matched him " + QString::number(walk::kClausePlain) +
             "% — a gap of " + QString::number(walk::kClauseDiscordant) +
             " spans that does not hold up statistically (p=0.08). Neither "
             "column is established at this level; try the simpler one "
             "first.</div>";
        // Both orders, side by side. Showing only the engine's would be
        // presenting the weaker of two measured options as the answer.
        h += "<table cellspacing='0' cellpadding='4' style='margin:2px 0'>"
             "<tr><td style='color:#888;font-size:11px'><b>the engine says</b>"
             "</td><td style='color:#888;font-size:11px'><b>as written "
             "(the simpler bet)</b></td></tr><tr><td valign='top'>";
        std::vector<std::pair<int, int>> ordered;   // (order, chunk)
        for (size_t i = 0; i < plan.size(); ++i)
            if (plan[i].order > 0) ordered.push_back({plan[i].order, (int)i});
        std::sort(ordered.begin(), ordered.end());
        h += "<table cellspacing='0' cellpadding='4'>";
        for (const auto& [ord, ix] : ordered)
            h += "<tr><td><b>" + QString::number(ord) +
                 ".</b></td><td style='font-family:monospace'>" +
                 walk::slice(doc, chunks[ix].beg, chunks[ix].end) +
                 "</td><td style='color:#666'>" +
                 QString(plan[ix].how).toHtmlEscaped() + "</td></tr>";
        for (size_t i = 0; i < plan.size(); ++i)
            if (plan[i].order == 0)
                h += "<tr><td style='color:#888'>&mdash;</td><td "
                     "style='font-family:monospace'>" +
                     walk::slice(doc, chunks[i].beg, chunks[i].end) +
                     "</td><td style='color:#666'>" +
                     QString(plan[i].how).toHtmlEscaped() + "</td></tr>";
        h += "</table></td><td valign='top'>";
        h += "<table cellspacing='0' cellpadding='4'>";
        for (size_t i = 0; i < chunks.size(); ++i)
            h += "<tr><td><b>" + QString::number(i + 1) +
                 ".</b></td><td style='font-family:monospace'>" +
                 walk::slice(doc, chunks[i].beg, chunks[i].end) + "</td></tr>";
        h += "</table></td></tr></table>";

        // ---- step 5 ----
        h += step(5, "What each word means");
        h += glossTable(doc, c);

        // ---- step 6 ----
        h += step(6, "Put together in that order");
        h += assembled(doc, chunks, ordered);

        // ---- step 7 ----
        h += step(7, "Geshe Michael's own translation");
        h += corpusHit(doc, c);
        return h;
    }

    // What this chunk could be DOING, from Preston's p.xv chart inverted and
    // narrowed by the verb. A set, never a single answer, unless the grammar
    // really does give one — which happens only for the fifth case.
    static QString functionCell(const allcore::CaseReading& cr,
                                const allcore::VerbGuess& verb,
                                const std::string& marker) {
        if (!cr.known()) {
            // Not a case particle. That is NOT the same as "nothing is known"
            // — Preston's chart lists s.p. among the qualifier markers, and
            // Wilson's nom-s.p. classes say which particle each one uses. An
            // em dash here threw away a fact our own verbclass table already
            // held: for a conjunctive or disjunctive verb, the dang chunk
            // completes the verb.
            if (verb.cls &&
                allcore::syntacticParticleMarksQualifier(marker, verb.cls->cls))
                return "<b style='color:#3B7A3B'>qualifier of the verb</b> "
                       "<span style='color:#666;font-size:11px'>(marked by a "
                       "syntactic particle, not by a case \u2014 this verb's "
                       "class is what says so)</span>";
            return "<span style='color:#888'>not a case particle \u2014 see "
                   "the note</span>";
        }
        // Collect over every case the particle admits: a la don chunk is
        // 2nd/4th/7th, so its function set is the union of all three.
        std::vector<allcore::Function> fs;
        const bool haveVerb = verb.cls != nullptr;
        const auto t = haveVerb ? allcore::transitivityOf(verb.cls->cls)
                                : allcore::Transitivity::Specialized;
        for (int c : cr.cases)
            for (auto f : allcore::functionsFor(c, t)) {
                bool seen = false;
                for (auto g : fs) if (g == f) seen = true;
                if (!seen) fs.push_back(f);
            }
        if (fs.empty())
            return "<span style='color:#888'>fills no slot of its own — it "
                   "joins this noun to the next</span>";
        QString out;
        for (size_t i = 0; i < fs.size(); ++i)
            out += (i ? " · " : "") +
                   QString(allcore::functionName(fs[i])).toHtmlEscaped();
        if (fs.size() == 1)
            return "<b style='color:#3B7A3B'>" + out + "</b>";
        // More than one, and the grammar cannot choose between them. Say so:
        // naming the first would be the overreach this column exists to end.
        // Say WHY the list is as short as it is. A reader shown four
        // readings on one row and two on the next, with no reason given, has
        // to guess whether the engine knew something or simply had less to
        // say.
        QString why;
        if (!haveVerb)
            why = "no verb identified, so nothing narrows this";
        else if (t == allcore::Transitivity::Transitive)
            why = "the case cannot choose between these; subject readings are "
                  "out because the verb is transitive";
        else if (t == allcore::Transitivity::Intransitive)
            why = "the case cannot choose between these; agent and object "
                  "readings are out because the verb is intransitive";
        else
            why = "the case cannot choose between these, and a specialized "
                  "verb rules nothing out \u2014 it has a subject AND an object";
        return out + " <span style='color:#B4540A;font-size:11px'>(" + why +
               ")</span>";
    }

    static QString step(int n, const char* title) {
        return "<div style='padding:10px 0 2px 0'><b>Step " + QString::number(n) +
               " · " + QString(title).toHtmlEscaped() + "</b></div>";
    }
    static QString body(const QString& s) {
        return "<div style='padding:2px 0 2px 12px'>" + s + "</div>";
    }

    // The innermost span covering each token, so a multi-syllable term is
    // glossed once as a term rather than syllable by syllable.
    QString glossTable(const allcore::OverlayDoc& doc, const allcore::Clause& c) {
        QString h = "<table cellspacing='0' cellpadding='4'>";
        int t = c.beg;
        int rows = 0;
        while (t < c.end) {
            const int best = walk::bestGlossSpan(doc, t, c.end);
            if (best < 0) {
                h += "<tr><td style='font-family:monospace'>" +
                     walk::slice(doc, t, t + 1) +
                     "</td><td style='color:#B4540A'>&#10216;" +
                     walk::slice(doc, t, t + 1) +
                     "&#10217; not in the glossary</td></tr>";
                ++t; ++rows; continue;
            }
            const auto& e = doc.entries[doc.spans[best].entry_ix];
            QString g;
            for (size_t i = 0; i < e.hgm_gloss.size() && i < 3; ++i)
                g += (i ? " · " : "") + walk::esc(e.hgm_gloss[i]);
            // G2: the binding gloss renders TIERED, always. Labelling only
            // the provisional ones leaves curated and glossary material
            // indistinguishable, and the reader cannot see how much weight
            // a line carries.
            const QString tier = e.provisional()
                ? " <span style='color:#B4540A;font-size:11px'>[PROVISIONAL "
                  "\u2014 auto-aligned]</span>"
                : e.tier.empty()
                      ? QString(" <span style='color:#888;font-size:11px'>"
                                "[no tier recorded]</span>")
                      : " <span style='color:#3B7A3B;font-size:11px'>[GMR " +
                            walk::esc(e.tier) + "]</span>";
            h += "<tr><td style='font-family:monospace'>" +
                 walk::slice(doc, doc.spans[best].beg, doc.spans[best].end) +
                 "</td><td>" + g + tier + "</td></tr>";
            t = doc.spans[best].end; ++rows;
        }
        h += "</table>";
        if (rows == 0) h = body("nothing to gloss");
        return h;
    }

    // HGM's own glosses, in the engine's order. Never a sentence — a sentence
    // would be composition, and the machine may match but never compose.
    QString assembled(const allcore::OverlayDoc& doc,
                      const std::vector<allcore::Chunk>& chunks,
                      const std::vector<std::pair<int, int>>& ordered) {
        provisional_ = 0;
        if (ordered.empty())
            return body("<span style='color:#888'>the engine gave no chunk its "
                        "own number, so there is no order to put together.</span>");
        QString line;
        for (const auto& [ord, ix] : ordered) {
            QString piece;
            int t = chunks[ix].beg;
            while (t < chunks[ix].end) {
                const int best = walk::bestGlossSpan(doc, t, chunks[ix].end);
                if (best < 0) {
                    piece += (piece.isEmpty() ? "" : " ") +
                             QString("&#10216;") + walk::slice(doc, t, t + 1) +
                             "&#10217;";
                    ++t;
                    continue;
                }
                // G2, and this is the site that mattered: step 6 assembled
                // provisional glosses into a running line with nothing to
                // distinguish them, so a line built entirely from
                // auto-aligned material read exactly like one built from
                // curated material. A provisional word is marked where it is
                // USED, not only where it is listed.
                const auto& ent = doc.entries[doc.spans[best].entry_ix];
                // bestGlossSpan's contract is that the span it returns carries
                // a gloss, so this index is safe — but a contract is not a
                // guard. When I mutation-tested the picker by dropping its
                // gloss requirement, this line indexed an empty vector and the
                // binary died before printing anything, and my harness counted
                // zero [FAIL] lines and called that a pass. A crash is a bad
                // way to learn a gate works. Falling through to the flag keeps
                // the failure legible, so the CONTENT gate is what catches a
                // broken picker rather than a segfault.
                if (ent.hgm_gloss.empty()) {
                    piece += (piece.isEmpty() ? "" : " ") +
                             QString("&#10216;") + walk::slice(doc, t, t + 1) +
                             "&#10217;";
                    ++t;
                    continue;
                }
                const QString g = walk::esc(ent.hgm_gloss[0]);
                piece += (piece.isEmpty() ? "" : " ") +
                         (ent.provisional()
                              ? "<span style='color:#B4540A'>" + g +
                                    "<sup>prov</sup></span>"
                              : g);
                if (ent.provisional()) ++provisional_;
                t = doc.spans[best].end;
            }
            line += (line.isEmpty() ? "" : " <span style='color:#888'>|</span> ") + piece;
        }
        QString note =
            "<div style='color:#666;font-size:11px;padding-top:4px'>"
            "Geshe Michael's glossary words in the engine's order — not a "
            "translation, and not English. It is the raw material a "
            "translation gets made out of. Anything in &#10216;angle "
            "brackets&#10217; is a word the glossary does not have. Where he "
            "banked several equivalents this takes the FIRST one, which is not "
            "always the best one — <i>bstan</i> comes out \"mentioned\" here "
            "when \"teach\" is the sense. Step 5 lists the alternatives; "
            "choosing between them is your job, because choosing would be "
            "composing and the machine may only match.";
        if (provisional_ > 0)
            note += " <span style='color:#B4540A'>" +
                    QString::number(provisional_) +
                    " word(s) marked <sup>prov</sup> are PROVISIONAL "
                    "auto-aligned glosses, not Geshe Michael's own ruling — "
                    "the line is only as good as they are.</span>";
        note += "</div>";
        return body(line + note);
    }

    QString corpusHit(const allcore::OverlayDoc& doc, const allcore::Clause& c) {
        // Search the corpus for the clause's own wylie. A hit means this exact
        // material is in a course he translated.
        QString q;
        for (int t = c.beg; t < c.end && t < (int)doc.tokens.size(); ++t)
            q += (q.isEmpty() ? "" : " ") + QString::fromStdString(doc.tokens[t]);
        if (q.trimmed().isEmpty()) return body("nothing to look up");
        std::string fts = "\"" + q.toStdString() + "\"";
        std::vector<allcore::CorpusSegment> hits;
        try {
            hits = spine_.corpusSearch(fts, "", 3);
        } catch (...) {
            return body("<span style='color:#888'>the corpus could not be "
                        "searched for this clause.</span>");
        }
        if (hits.empty())
            return body(
                "<span style='color:#888'>this clause is not in the corpus.</span>"
                "<div style='color:#666;font-size:11px;padding-top:4px'>Which "
                "means step 4 has nothing to be checked against here. That is "
                "the ordinary case for new material, and the reason step 4 is "
                "labelled the way it is.</div>");
        QString h;
        for (const auto& s : hits) {
            h += "<div style='padding:4px 0'>"
                 "<span style='color:#3B7A3B'>ATTESTED</span> · " +
                 QString::fromStdString(s.course).toHtmlEscaped() + " §" +
                 QString::number(s.seq) + "<br><i>" +
                 QString::fromStdString(s.english).toHtmlEscaped() + "</i></div>";
        }
        return "<div style='padding:2px 0 2px 12px'>" + h +
               "<div style='color:#666;font-size:11px'>His English. Compare its "
               "order with step 4 — where they differ, one of them is telling "
               "you something.</div></div>";
    }

    allcore::Spine& spine_;
    QPlainTextEdit* src_ = nullptr;
    QTextBrowser* out_ = nullptr;
    QPushButton* go_ = nullptr;
    QLabel* detect_ = nullptr;
public:
    int clauses_ = 0;       // last walk's clause count, for the selftest
    int provisional_ = 0;   // provisional glosses used in the last step 6
};
