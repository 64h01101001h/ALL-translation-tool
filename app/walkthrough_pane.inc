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
//     segment -> clauses       16      69%       100%
//     clause  -> phrases       57      40%        60%
//     phrase  -> sub-phrases   14      57%        36%
//     sub     -> words        120      50%        31%
//
// The engine is WORSE THAN DOING NOTHING at the top two levels and clearly
// better at the bottom two. Clause order is essentially never inverted -- a
// second, independent measurement over 972 clause pairs puts it at 5%
// backwards -- so reordering clauses can only break what was already right,
// and that is precisely what the engine does 31% of the time.
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
inline constexpr int kOrderSpans = 207;   // spans where GMR's order was recoverable
inline constexpr int kOrderAgree = 102;   // ... and the engine matched it
inline constexpr int kOrderPlain = 92;    // ... and plain left-to-right matched it
// The level this pane's step 4 works at: clause -> phrases.
inline constexpr int kClauseSpans = 57;
inline constexpr int kClauseEngine = 40;  // per cent
inline constexpr int kClausePlain = 60;   // per cent
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
            // written order is the better bet (60% against 40%). A later
            // edit that "simplifies" step 4 down to the engine's ruling
            // alone would be presenting the weaker of two measured options
            // as the answer, and this fails if it happens.
            src_->setPlainText("sangs rgyas kyis chos bstan");
            walk();
            const QString t = out_->toPlainText();
            check(t.contains("the engine says") && t.contains("as written"),
                  "step 4 shows BOTH orders — the engine's and the written one");
            check(t.contains("the better bet"),
                  "step 4 says which of the two the measurement favours");
            check(t.contains(QString::number(kEngineHere) + "%") &&
                      t.contains(QString::number(kPlainHere) + "%"),
                  "step 4 quotes the measured figures for its own level");
            check(t.contains("do not shuffle them"),
                  "the reader is told to leave the clause order alone");
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
             "%</b>. At this level the engine is the weaker of the two, so step "
             "4 shows you both and you should lean on the written order unless "
             "the engine gives you a reason you can check. Deeper down — inside "
             "a phrase, among the words — the ruling wins handily (50% against "
             "31%), which is where its verb-first and genitive-rightward rules "
             "were earned. Fixing the middle level is on the punchlist; "
             "reporting it as better than it is, is not an option.</div>";
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
                e += " <span style='color:#B4540A'>UNVERIFIED</span> — the last "
                     "dictionary word in the final chunk. Nothing in the "
                     "glossary says it is a verb; it is a candidate, not a finding.";
            if (verb.cls)
                e += "<br>Wilson class: <b>" +
                     QString::fromUtf8(verb.cls->label).toHtmlEscaped() +
                     "</b> — " + QString::fromUtf8(verb.cls->expects).toHtmlEscaped();
            h += body(e);
        }

        // ---- step 3 ----
        h += step(3, "The chunks, and what each one is doing");
        h += "<table cellspacing='0' cellpadding='4' style='margin:2px 0'>";
        for (size_t i = 0; i < chunks.size(); ++i) {
            h += "<tr><td style='color:#888'>[" + QString::number(i + 1) +
                 "]</td><td style='font-family:monospace'>" +
                 walk::slice(doc, chunks[i].beg, chunks[i].end) + "</td><td>" +
                 (chunks[i].marker.empty()
                      ? QString("<span style='color:#888'>no particle</span>")
                      : "<b>" + walk::esc(chunks[i].marker) + "</b>" +
                            (chunks[i].marker_fused ? " <span style='color:#888'>"
                                                      "(fused)</span>"
                                                    : QString())) +
                 "</td><td>" + QString(chunks[i].role).toHtmlEscaped() +
                 "</td></tr>";
        }
        h += "</table>";

        // ---- step 4 ----
        h += step(4, "What order to read them in");
        h += "<div style='color:#B4540A;font-size:11px;padding:0 0 4px 0'>"
             "the engine's ruling. At this level it matched Geshe Michael " +
             QString::number(walk::kClauseEngine) + "% of the time and the "
             "written order matched him " + QString::number(walk::kClausePlain) +
             "% — so the column on the right is the better bet until you have a "
             "reason to prefer the left.</div>";
        // Both orders, side by side. Showing only the engine's would be
        // presenting the weaker of two measured options as the answer.
        h += "<table cellspacing='0' cellpadding='4' style='margin:2px 0'>"
             "<tr><td style='color:#888;font-size:11px'><b>the engine says</b>"
             "</td><td style='color:#888;font-size:11px'><b>as written "
             "(the better bet here)</b></td></tr><tr><td valign='top'>";
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
            auto at = doc.spansAt(t);
            int best = -1, bestEnd = t + 1;
            for (int si : at) {
                const auto& s = doc.spans[si];
                if (s.beg == t && s.end <= c.end && s.end > bestEnd - 1 &&
                    s.entry_ix >= 0) {
                    if (s.end > bestEnd || best < 0) { best = si; bestEnd = s.end; }
                }
            }
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
            if (g.isEmpty())
                g = "<span style='color:#B4540A'>no Geshe Michael equivalent "
                    "banked for this entry</span>";
            // G2: the binding gloss renders TIERED, always. Labelling only
            // the provisional ones leaves curated and glossary material
            // indistinguishable from each other and from an unlabelled
            // default, and the reader cannot see how much weight a line
            // carries. Every row says which tier it is.
            QString tier = e.provisional()
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
            t = bestEnd; ++rows;
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
                auto at = doc.spansAt(t);
                int best = -1, bestEnd = t + 1;
                for (int si : at) {
                    const auto& s = doc.spans[si];
                    if (s.beg == t && s.end <= chunks[ix].end && s.entry_ix >= 0 &&
                        (s.end > bestEnd || best < 0)) { best = si; bestEnd = s.end; }
                }
                if (best >= 0 && !doc.entries[doc.spans[best].entry_ix].hgm_gloss.empty()) {
                    // G2 again, and this is the site that mattered: step 6
                    // assembled provisional glosses into a running line with
                    // nothing to distinguish them, so a line built entirely
                    // out of auto-aligned material read exactly like one
                    // built from curated material. A provisional word is
                    // marked where it is used, not only where it is listed.
                    const auto& ent = doc.entries[doc.spans[best].entry_ix];
                    const QString g = walk::esc(ent.hgm_gloss[0]);
                    piece += (piece.isEmpty() ? "" : " ") +
                             (ent.provisional()
                                  ? "<span style='color:#B4540A'>" + g +
                                        "<sup>prov</sup></span>"
                                  : g);
                    if (ent.provisional()) ++provisional_;
                    t = bestEnd;
                } else {
                    piece += (piece.isEmpty() ? "" : " ") +
                             QString("&#10216;") + walk::slice(doc, t, t + 1) +
                             "&#10217;";
                    ++t;
                }
            }
            line += (line.isEmpty() ? "" : " <span style='color:#888'>|</span> ") + piece;
        }
        QString note =
            "<div style='color:#666;font-size:11px;padding-top:4px'>"
            "Geshe Michael's glossary words in the engine's order — not a "
            "translation, and not English. It is the raw material a "
            "translation gets made out of. Anything in &#10216;angle "
            "brackets&#10217; is a word the glossary does not have.";
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
