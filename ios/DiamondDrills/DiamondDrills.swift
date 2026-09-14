// Tibetan Translation Trainer — the Learn tab, standing alone on iPhone.
//
// Two modes, as on the desktop: Trainer (read a passage yourself, then reveal
// six layers one at a time) and Drills (fill the blank from four options).
//
// Path A: the phone generates nothing. Every passage and every drill was built
// on the Mac by the same allcore the desktop uses, so this app inherits the
// desktop's fixes and, more to the point, its refusals — the title catalogue
// and English-in-the-Tibetan-field are never drawn, English wildly out of
// proportion is never drawn, editorial markup is stripped so it cannot leak
// into the script or tell the learner which option is the answer, and anything
// the engine could not read arrives already flagged in ⟨ ⟩.
//
// The honesty rules travel. Every English line is Geshe Michael Roach's own
// text; engine guidance is labelled guidance; nothing here is binding.
import SwiftUI

// MARK: - the pack

struct Drill: Codable {
    let before: String
    let after: String
    let english: String
    let role: String
    /// The skill this drill exercises, in the same vocabulary the
    /// desktop files misses under. Optional so an older pack decodes.
    let skill: String?
    let course: String
    let seq: Int
    let title: Bool
    let answer: Int
    let options: [String]
    /// What the blanked word means, MATCHED from his own English for this
    /// segment — never composed. Optional so an older pack still decodes.
    let means: [TermMeans]?
}

/// One glossed headword inside the blanked chunk.
struct TermMeans: Codable {
    let wylie: String
    let tier: String
    let provisional: Bool
    /// His recorded equivalents for this headword, capped at eight.
    let glosses: [String]
    /// How many more were recorded beyond those eight.
    let more: Int
    /// Which of them actually occur in his English for THIS segment.
    let used: [String]
}

struct PlanStep: Codable {
    let chunk: Int
    let order: Int
    let how: String
}

struct Passage: Codable {
    let tibetan: String
    let english: String
    let course: String
    let seq: Int
    let chunks: [String]
    let roles: [String]
    let plan: [PlanStep]
    let verb: String
    let verb_evidence: String
    let verb_confident: Bool
    /// How much of `tibetan` the chunks/roles/plan actually describe.
    /// The passage is the whole segment; the layers come from ONE
    /// clause of it, a median of about a third. Optional so a pack
    /// built before this field still decodes.
    let clause_share: Int?
}

/// The pack's own account of itself. `built_from` is the commit and moment the
/// Mac built it: WITHOUT it a stale pack is indistinguishable from a fresh one,
/// which is the whole reason the builder stamps it. It was stamped and then
/// never decoded here, so the phone could not have told you either way.
/// Optional, so a pack built before the stamp still decodes.
struct PackMeta: Codable {
    let built_by: String
    let source: String
    let tier: String
    let built_from: String?
}
/// One chunk-order drill, pre-built on the Mac.
struct OrderDrill: Codable {
    let english: String
    let course: String
    let seq: Int
    let chunks: [String]
    let presented: [Int]
    let verb: VerbGuess
}
struct VerbGuess: Codable {
    let wylie: String
    let evidence: String
    let confident: Bool
}
/// One particle-choice drill.
struct ParticleDrill: Codable {
    let english: String
    let course: String
    let seq: Int
    let explanation: String
    let correct: Int
    let options: [String]
    let tokens: [String]
}
/// One of his own Language Study Guide cards.
struct ScriptCard: Codable {
    let tibetan: String
    let wylie: String
    let pron: String
    let tier: String
    let provisional: Bool
    let gloss: [String]
}
/// One formal debate statement, already split into its elements.
/// One case particle in a real aligned segment, and whether he rendered it.
struct SilentSpan: Codable {
    let tib: String
    let wylie: String
    let course: String
    let rendered: Bool
}
/// One headword and every rendering he is recorded using for it.
struct SecondThought: Codable {
    let tib: String
    let wylie: String
    let renderings: [Rendering]
}
struct Rendering: Codable { let eng: String; let n: Int }


/// One span of his, cut into the layer just below it, with the order HIS
/// English takes those pieces. The key is ATTESTED: it is where each piece's
/// English actually sits inside the parent's English in the alignment bank —
/// his own rendering of this very span — and never the engine's ruling about
/// what order the pieces ought to be read in.
struct ReadOrderItem: Codable {
    let ref: String
    let parent: String
    let parentEng: String
    /// In the order they are WRITTEN.
    let kids: [String]
    let kidsEng: [String]
    /// Indices into `kids`, in the order his English takes them.
    let answer: [Int]
}

/// One segment with its punctuation stripped: where do the clauses end?
struct BoundaryItem: Codable {
    let tokens: [String]
    let ends: [Int]
    /// Parallel to `ends`. True when the scribe marked that boundary himself
    /// and the key is attested; false when the splitter ruled from the
    /// particle and the key is ENGINE GUIDANCE, which the reveal says.
    let attested: [Bool]
    /// Positions where an ambiguous `na` split was dropped for want of verb
    /// evidence. Absence of evidence is not evidence of a noun, so a mark
    /// here is not counted against the learner.
    let unscored: [Int]
    let functions: [String]
    let course: String
    let seq: Int
    /// The source carried no punctuation at all — the real skill.
    let hard: Bool
    let english: String
}

/// A word with TWO attestations: the segment it was met in, and a different
/// one. "You know it here" and "you know it anywhere" are different claims.
struct VocabItem: Codable {
    let tib: String
    let wylie: String
    /// The alignment layer's most-attested English for the word — machine
    /// matched, TENTATIVE. NOT his dictionary gloss, and never shown as one.
    let aligned_eng: String
    let here: VocabContext
    let anywhere: VocabContext
}

struct VocabContext: Codable {
    let tib: String
    let course: String
    let seq: Int
    let english: String
}

/// A span and the pieces it contains, from the alignment layer's own nesting.
struct PeelSpan: Codable {
    let parent: String
    let course: String
    let seq: Int
    let pieces: [String]
}

struct DebateStatement: Codable {
    let preamble: String
    let subject: String
    let consequence: String
    let reason: String
    let english: String
    let course: String
    let seq: Int
}

struct Pack: Codable {
    let meta: PackMeta
    let cloze: [Drill]
    let trainer: [Passage]
    // Optional so a pack built before 2026-09-11 still decodes rather than
    // leaving the app with nothing at all.
    let order: [OrderDrill]?
    let particle: [ParticleDrill]?
    let script: [ScriptCard]?
    let debate: [DebateStatement]?
    let silent: [SilentSpan]?
    let second: [SecondThought]?
    let peel: [PeelSpan]?
    let boundary: [BoundaryItem]?
    let vocab: [VocabItem]?
    let readorder: [ReadOrderItem]?
}

enum PackLoader {
    static func load() -> Pack? {
        guard let url = Bundle.main.url(forResource: "drills", withExtension: "json"),
              let data = try? Data(contentsOf: url),
              let pack = try? JSONDecoder().decode(Pack.self, from: data)
        else { return nil }
        return pack
    }
}

// MARK: - the manuscript palette, light and dark

struct Ink {
    let paper, ink, gold, muted, act, machine, error, plaque: Color
    static func of(_ scheme: ColorScheme) -> Ink {
        scheme == .dark
            ? Ink(paper: Color(red: 0.106, green: 0.098, blue: 0.090),
                  ink: Color(red: 0.898, green: 0.878, blue: 0.839),
                  gold: Color(red: 0.788, green: 0.647, blue: 0.361),
                  muted: Color(red: 0.659, green: 0.635, blue: 0.604),
                  act: Color(red: 0.373, green: 0.749, blue: 0.557),
                  machine: Color(red: 0.910, green: 0.529, blue: 0.290),
                  error: Color(red: 0.910, green: 0.537, blue: 0.494),
                  plaque: Color(red: 0.200, green: 0.188, blue: 0.169))
            : Ink(paper: Color(red: 0.980, green: 0.965, blue: 0.933),
                  ink: Color(red: 0.169, green: 0.130, blue: 0.094),
                  gold: Color(red: 0.510, green: 0.404, blue: 0.165),
                  muted: Color(red: 0.431, green: 0.404, blue: 0.365),
                  act: Color(red: 0.118, green: 0.420, blue: 0.306),
                  machine: Color(red: 0.663, green: 0.302, blue: 0.043),
                  error: Color(red: 0.549, green: 0.184, blue: 0.169),
                  plaque: Color(red: 0.957, green: 0.937, blue: 0.894))
    }
}

// MARK: - progress

final class Deck: ObservableObject {
    @Published var done = 0
    @Published var right = 0
    @Published var read = 0
    private let d = UserDefaults.standard
    init() {
        done = d.integer(forKey: "drills.done")
        right = d.integer(forKey: "drills.right")
        read = d.integer(forKey: "trainer.read")
    }
    func record(correct: Bool) {
        done += 1; if correct { right += 1 }
        d.set(done, forKey: "drills.done"); d.set(right, forKey: "drills.right")
    }

    /// Per-skill attempts and hits, so the phone can name a weak spot rather
    /// than only count total wrongs. Stored as two small dictionaries in
    /// UserDefaults — the phone has no progress database, and this is the
    /// same taxonomy the desktop uses so the two mean the same thing.
    func record(skill: String?, correct: Bool) {
        guard let k = skill, !k.isEmpty else { return }
        var att = d.dictionary(forKey: "skill.attempts") as? [String: Int] ?? [:]
        var hit = d.dictionary(forKey: "skill.right") as? [String: Int] ?? [:]
        att[k, default: 0] += 1
        if correct { hit[k, default: 0] += 1 }
        d.set(att, forKey: "skill.attempts")
        d.set(hit, forKey: "skill.right")
        objectWillChange.send()
    }

    /// (skill, attempts, right), weakest first. `enough` marks whether there
    /// are enough attempts to call a trend — below the floor the surface must
    /// say so rather than draw one, exactly as the desktop does.
    func weakSpots(floor: Int = 8) -> [(String, Int, Int, Bool)] {
        let att = d.dictionary(forKey: "skill.attempts") as? [String: Int] ?? [:]
        let hit = d.dictionary(forKey: "skill.right") as? [String: Int] ?? [:]
        return att.map { ($0.key, $0.value, hit[$0.key] ?? 0, $0.value >= floor) }
            .filter { $0.1 > $0.2 }                     // has at least one miss
            .sorted { Double($0.2) / Double($0.1) < Double($1.2) / Double($1.1) }
    }
    func recordRead() { read += 1; d.set(read, forKey: "trainer.read") }
}


/// The weak-spots sheet: the phone's half of "Train this".
///
/// It names each skill with its accuracy, and offers to aim the draw at it.
/// Below a floor of attempts it says the count is too small rather than
/// drawing a trend — the same rule the desktop enforces, because a learner
/// reading "40% right" off four attempts is being misled by arithmetic.
///
/// Aiming FILTERS the pre-tagged pack rather than generating, since the pack
/// is built on the Mac and the phone generates nothing.
struct WeakSpotsSheet: View {
    @ObservedObject var deck: Deck
    let train: (String) -> Void
    @Environment(\.colorScheme) private var scheme
    @Environment(\.dismiss) private var dismiss


    var body: some View {
        let c = Ink.of(scheme)
        NavigationStack {
            ScrollView {
                VStack(alignment: .leading, spacing: 14) {
                    Text("Every wrong answer is filed under the skill it reveals. Nothing leaves this phone.")
                        .font(.system(size: 13)).foregroundColor(c.muted)
                    ForEach(deck.weakSpots(), id: \.0) { spot in
                        VStack(alignment: .leading, spacing: 4) {
                            Text(spot.0).font(.system(size: 16, weight: .semibold))
                                .foregroundColor(c.ink)
                            Text(Self.score(spot))
                                .font(.system(size: 13)).foregroundColor(c.muted)
                            Button("train this") { train(spot.0) }
                                .font(.system(size: 14)).foregroundColor(c.act)
                        }
                        Divider()
                    }
                }.padding(20)
            }
            .background(c.paper.ignoresSafeArea())
            .navigationTitle("My weak spots")
            .navigationBarTitleDisplayMode(.inline)
            .toolbar {
                ToolbarItem(placement: .confirmationAction) {
                    Button("Done") { dismiss() }
                }
            }
        }
    }

    /// Kept out of the body: Swift's type-checker will not solve a string
    /// this shape inline.
    static func score(_ s: (String, Int, Int, Bool)) -> String {
        if !s.3 { return "\(s.1) attempt(s) — too few to call a trend" }
        return "right \(s.2) of \(s.1)"
    }
}


// MARK: - the modes the phone can offer

/// What the phone can ask. Three desktop modes are deliberately absent and
/// each has a reason the user can be told:
///   Parallel reading    — the Trainer tab already is corpus reading.
///   Vocabulary (SRS)    — the deck is built from what you click on the
///                         desktop, and the two cannot sync; a second
///                         unconnected deck here would compete with the real
///                         one.
///   the chunk-order drill's `plan` and `markers` — the core's OrderDrill
///                         carries both and the pack bakes neither, which
///                         until 2026-09-14 was an undeclared omission rather
///                         than a decision. Declared now, with reasons.
///                         `plan` is planReading's reading-order guidance, and
///                         the alignment bank says it is NOT established at
///                         this level: 51% against 59% over 112 spans, with
///                         only 21 separating them. Worse, it answers a
///                         DIFFERENT question than this drill asks — this one
///                         asks the order he WROTE the chunks in, and the
///                         reading order is the Reading order drill. Putting
///                         unestablished guidance about question B inside the
///                         reveal for question A is how the two get conflated,
///                         which is the confusion this drill's own warning
///                         exists to prevent.
///                         `markers` feeds the desktop's miss taxonomy — which
///                         role you got wrong — and the phone has no miss
///                         taxonomy for this drill to feed. It ships when that
///                         does.
///   Translate & compare — needs the whole 105,634-entry dictionary run
///                         against a draft as you type it. Not bakeable.
// The names here are the picker's whole vocabulary, so they are kept to a
// couple of words. They used to carry the drill's question too — "Boundary
// hunt — where do the clauses end?" — and the menu style renders the selected
// name in full: at an accessibility text size one name ran to five lines and
// left the drill beneath it a two-line strip. Every view states its own
// question on entry, so nothing is lost by keeping the label bare.
enum DrillKind: String, CaseIterable, Identifiable {
    case cloze = "Fill the blank"
    case order = "Chunk order"
    case particle = "Particle choice"
    case script = "Script cards"
    case debate = "Debate"
    case silent = "Silent particle"
    case second = "His second thought"
    case peel = "Peel"
    case boundary = "Boundary hunt"
    case vocab = "Known here / known anywhere"
    // Short on purpose. The desktop names this drill in full because a combo
    // box has room; the phone renders the whole label in the picker, and at an
    // accessibility text size the full sentence ran to five lines and left the
    // drill itself a two-line strip that would not scroll. The question is put
    // in the instruction line directly beneath it instead.
    case readorder = "Reading order"
    case mixed = "Mixed set"
    var id: String { rawValue }
}

/// Restore a clause to the order it was written in.
struct OrderView: View {
    let drill: OrderDrill
    let next: () -> Void
    @Environment(\.colorScheme) private var scheme
    @State private var revealed = false

    var body: some View {
        let c = Ink.of(scheme)
        ScrollView {
            VStack(alignment: .leading, spacing: 14) {
                Text("These chunks are out of order. Read them, decide the order they were written in, then reveal.")
                    .font(.system(size: 15)).foregroundColor(c.muted)
                // The desktop carries this warning and gates it; the phone
                // shipped the English hint without it. The two orders are
                // different questions -- this drill asks for the TIBETAN one --
                // and a learner shown his English first will reach for the
                // English order, which is what the Reading order drill is for.
                Text("The answer is the order he WROTE them in, not the order "
                     + "his English takes them. Those are different questions; "
                     + "the second one is the Reading order drill.")
                    .font(.system(size: 12)).foregroundColor(c.machine)
                    .fixedSize(horizontal: false, vertical: true)
                Text("ATTESTED · the key is his Tibetan for [\(drill.course):\(drill.seq)]")
                    .font(.system(size: 11)).foregroundColor(c.act)
                Text("his English for the whole segment, for sense — not the key:")
                    .font(.system(size: 11)).foregroundColor(c.muted)
                EnglishHint(text: drill.english, ink: c)
                ForEach(Array(drill.presented.enumerated()), id: \.offset) { i, ix in
                    HStack(alignment: .top, spacing: 10) {
                        Text(String(UnicodeScalar(65 + i)!))
                            .font(.system(size: 15, weight: .semibold))
                            .foregroundColor(c.gold)
                        Text(drill.chunks[ix]).font(.system(size: 22))
                            .foregroundColor(c.ink)
                        Spacer()
                    }
                }
                if revealed {
                    Divider()
                    Text("As written:").font(.system(size: 14)).foregroundColor(c.muted)
                    ForEach(Array(drill.chunks.enumerated()), id: \.offset) { i, ch in
                        Text("\(i + 1).  \(ch)").font(.system(size: 20))
                            .foregroundColor(c.act)
                    }
                    if drill.verb.confident && !drill.verb.wylie.isEmpty {
                        Text("verb: \(drill.verb.wylie) — \(drill.verb.evidence)")
                            .font(.system(size: 13)).foregroundColor(c.act)
                    } else {
                        Text("No verb the dictionary can confirm in this clause.")
                            .font(.system(size: 13)).foregroundColor(c.machine)
                    }
                    Text("[\(drill.course):\(drill.seq)]")
                        .font(.system(size: 12)).foregroundColor(c.muted)
                }
            }.padding(20)
        }
        VStack(spacing: 0) {
            Divider()
            HStack(spacing: 14) {
                Button(revealed ? "New drill" : "Reveal") {
                    if revealed { next(); revealed = false } else { revealed = true }
                }
                .font(.system(size: 17, weight: .semibold)).foregroundColor(.white)
                .lineLimit(1).fixedSize(horizontal: true, vertical: false)
                .padding(.horizontal, 20).padding(.vertical, 11)
                .background(c.act).cornerRadius(9)
                Spacer()
            }.padding(.horizontal, 20).padding(.top, 10).padding(.bottom, 26)
        }.background(c.paper)
    }
}

/// Which particle of the family belongs in the blank.
struct ParticleView: View {
    let drill: ParticleDrill
    @ObservedObject var deck: Deck
    let next: () -> Void
    @Environment(\.colorScheme) private var scheme
    @State private var picked: Int? = nil
    @State private var checked = false
    private var isRight: Bool { picked == drill.correct }

    var body: some View {
        let c = Ink.of(scheme)
        ScrollView {
            VStack(alignment: .leading, spacing: 14) {
                Text("Which particle belongs in the blank?")
                    .font(.system(size: 15)).foregroundColor(c.muted)
                EnglishHint(text: drill.english, ink: c)
                Text(drill.tokens.joined(separator: " "))
                    .font(.system(size: 22)).foregroundColor(c.ink)
                Divider()
                ForEach(Array(drill.options.enumerated()), id: \.offset) { i, o in
                    Button { if !checked { picked = i } } label: {
                        HStack(spacing: 11) {
                            Image(systemName: picked == i ? "largecircle.fill.circle" : "circle")
                                .foregroundColor(picked == i ? c.act : c.muted)
                            Text(o).font(.system(size: 20))
                                .foregroundColor(checked && i == drill.correct ? c.act : c.ink)
                            Spacer()
                        }.padding(.vertical, 6)
                    }.buttonStyle(.plain)
                }
                if checked {
                    Text(isRight ? "Correct." : "Not yet — the text has \(drill.options[drill.correct]).")
                        .font(.system(size: 17, weight: .semibold))
                        .foregroundColor(isRight ? c.act : c.machine)
                    if !drill.explanation.isEmpty {
                        Text(drill.explanation).font(.system(size: 14))
                            .foregroundColor(c.muted)
                    }
                    Text("[\(drill.course):\(drill.seq)]")
                        .font(.system(size: 12)).foregroundColor(c.muted)
                }
            }.padding(20)
        }
        VStack(spacing: 0) {
            Divider()
            HStack(spacing: 14) {
                Button(checked ? "New drill" : "Check") {
                    if checked { next(); picked = nil; checked = false }
                    else if picked != nil { checked = true; deck.record(correct: isRight) }
                }
                .font(.system(size: 17, weight: .semibold)).foregroundColor(.white)
                .lineLimit(1).fixedSize(horizontal: true, vertical: false)
                .padding(.horizontal, 20).padding(.vertical, 11)
                .background(picked == nil && !checked ? c.muted : c.act).cornerRadius(9)
                .disabled(picked == nil && !checked)
                if !checked {
                    Button("Skip") { next(); picked = nil; checked = false }
                        .font(.system(size: 16)).foregroundColor(c.muted)
                }
                Spacer()
            }.padding(.horizontal, 20).padding(.top, 10).padding(.bottom, 26)
        }.background(c.paper)
    }
}

/// His own card: the Tibetan large, and how he reads it.
struct ScriptView: View {
    let card: ScriptCard
    let next: () -> Void
    @Environment(\.colorScheme) private var scheme
    @State private var shown = false

    var body: some View {
        let c = Ink.of(scheme)
        ScrollView {
            VStack(alignment: .leading, spacing: 16) {
                Text("How is this read aloud? Say it, then reveal.")
                    .font(.system(size: 15)).foregroundColor(c.muted)
                Text(card.tibetan).font(.system(size: 56)).foregroundColor(c.ink)
                if shown {
                    Text(card.pron).font(.system(size: 28, weight: .semibold))
                        .foregroundColor(c.act)
                    Text(card.wylie).font(.system(size: 15)).foregroundColor(c.muted)
                    if !card.gloss.isEmpty {
                        Text(card.provisional
                             ? "auto-aligned [PROVISIONAL] — not his own English: "
                               + card.gloss.joined(separator: " · ")
                             : "Geshe Michael Roach has: " + card.gloss.joined(separator: " · "))
                            .font(.system(size: 14))
                            .foregroundColor(card.provisional ? c.machine : c.muted)
                    }
                }
            }.padding(20)
        }
        VStack(spacing: 0) {
            Divider()
            HStack(spacing: 14) {
                Button(shown ? "New card" : "Reveal") {
                    if shown { next(); shown = false } else { shown = true }
                }
                .font(.system(size: 17, weight: .semibold)).foregroundColor(.white)
                .lineLimit(1).fixedSize(horizontal: true, vertical: false)
                .padding(.horizontal, 20).padding(.vertical, 11)
                .background(c.act).cornerRadius(9)
                Spacer()
            }.padding(.horizontal, 20).padding(.top, 10).padding(.bottom, 26)
        }.background(c.paper)
    }
}

/// A formal statement in three parts, and which element a reply attacks.
struct DebateView: View {
    let st: DebateStatement
    @ObservedObject var deck: Deck
    let next: () -> Void
    @Environment(\.colorScheme) private var scheme
    @State private var picked: Int? = nil
    @State private var checked = false

    private static let replies = ["MA GRUB NA", "RTAGS MA GRUB",
                                  "MA KHYAB NA", "RTZA BAR 'DOD NA"]
    private var replyIx: Int { abs(st.seq) % 4 }
    private var answer: Int { replyIx }
    private static let targets = [
        "the subject — it is not established",
        "the reason — it is not established",
        "the entailment — the reason does not force the consequence",
        "nothing — it accepts the consequence, and the root claim with it"]

    var body: some View {
        let c = Ink.of(scheme)
        ScrollView {
            VStack(alignment: .leading, spacing: 12) {
                Text("If your opponent answers \(Self.replies[replyIx]), which element are they attacking?")
                    .font(.system(size: 15)).foregroundColor(c.muted)
                if !st.preamble.isEmpty { part("PREAMBLE", st.preamble, c) }
                part("SUBJECT · CHOS CAN", st.subject, c)
                part("CONSEQUENCE · THAL", st.consequence, c)
                part("REASON · PHYIR", st.reason, c)
                Divider()
                ForEach(Array(Self.targets.enumerated()), id: \.offset) { i, t in
                    Button { if !checked { picked = i } } label: {
                        HStack(alignment: .top, spacing: 11) {
                            Image(systemName: picked == i ? "largecircle.fill.circle" : "circle")
                                .foregroundColor(picked == i ? c.act : c.muted)
                            Text(t).font(.system(size: 16))
                                .foregroundColor(checked && i == answer ? c.act : c.ink)
                                .multilineTextAlignment(.leading)
                            Spacer()
                        }.padding(.vertical, 6)
                    }.buttonStyle(.plain)
                }
                if checked {
                    Text(picked == answer ? "Correct." : "Not that one.")
                        .font(.system(size: 17, weight: .semibold))
                        .foregroundColor(picked == answer ? c.act : c.machine)
                    Text("Word-level drills are refused in this mode: his English for a debate segment is an expansion with the opponent's turns supplied, not a word-for-word rendering.")
                        .font(.system(size: 12)).foregroundColor(c.muted)
                    Text("[\(st.course):\(st.seq)]")
                        .font(.system(size: 12)).foregroundColor(c.muted)
                }
            }.padding(20)
        }
        VStack(spacing: 0) {
            Divider()
            HStack(spacing: 14) {
                Button(checked ? "New statement" : "Check") {
                    if checked { next(); picked = nil; checked = false }
                    else if picked != nil { checked = true; deck.record(correct: picked == answer) }
                }
                .font(.system(size: 17, weight: .semibold)).foregroundColor(.white)
                .lineLimit(1).fixedSize(horizontal: true, vertical: false)
                .padding(.horizontal, 20).padding(.vertical, 11)
                .background(picked == nil && !checked ? c.muted : c.act).cornerRadius(9)
                .disabled(picked == nil && !checked)
                Spacer()
            }.padding(.horizontal, 20).padding(.top, 10).padding(.bottom, 26)
        }.background(c.paper)
    }

    @ViewBuilder private func part(_ label: String, _ t: String, _ c: Ink) -> some View {
        VStack(alignment: .leading, spacing: 2) {
            Text(label).font(.system(size: 11)).tracking(1).foregroundColor(c.muted)
            Text(t).font(.system(size: 19)).foregroundColor(c.ink)
        }
    }
}


/// Did he render this particle in English here? The base rate is the lesson:
/// 1,595 of the 1,698 case markers in the layer have no English exponent, and
/// no reader predicts that at first.
struct SilentView: View {
    let span: SilentSpan
    let next: () -> Void
    @Environment(\.colorScheme) private var scheme
    @State private var picked: Int? = nil
    @State private var checked = false
    @AppStorage("silent.right") private var right = 0
    @AppStorage("silent.wrong") private var wrong = 0
    @AppStorage("silent.abstain") private var abstain = 0

    var body: some View {
        let c = Ink.of(scheme)
        ScrollView {
            VStack(alignment: .leading, spacing: 14) {
                Text("Tibetan marks this relation with a particle. Did Geshe Michael Roach render it with a word of English here?")
                    .font(.system(size: 15)).foregroundColor(c.muted)
                Text(span.tib).font(.system(size: 44)).foregroundColor(c.ink)
                Text("\(span.wylie) · \(span.course) · the alignment layer is TENTATIVE")
                    .font(.system(size: 12)).foregroundColor(c.muted)
                Divider()
                ForEach(Array(["He rendered it in English",
                               "He did not render it",
                               "I cannot tell from this alone"].enumerated()),
                        id: \.offset) { i, t in
                    Button { if !checked { picked = i } } label: {
                        HStack(spacing: 11) {
                            Image(systemName: picked == i ? "largecircle.fill.circle" : "circle")
                                .foregroundColor(picked == i ? c.act : c.muted)
                            Text(t).font(.system(size: 16)).foregroundColor(c.ink)
                            Spacer()
                        }.padding(.vertical, 6)
                    }.buttonStyle(.plain)
                }
                if checked {
                    Text(verdict).font(.system(size: 17, weight: .semibold))
                        .foregroundColor(picked == 2 ? c.muted
                                         : (said == span.rendered ? c.act : c.machine))
                    Text(span.rendered
                         ? "He rendered it with a word of English."
                         : "He rendered it with no English word at all — the relation is carried by the shape of his sentence.")
                        .font(.system(size: 14)).foregroundColor(c.ink)
                    Text("In the aligned layer he leaves case particles unrendered 93.9% of the time. Your run: \(right) right · \(wrong) wrong · \(abstain) abstained.")
                        .font(.system(size: 12)).foregroundColor(c.muted)
                }
            }.padding(20)
        }
        VStack(spacing: 0) {
            Divider()
            HStack(spacing: 14) {
                Button(checked ? "Next" : "Check") {
                    if checked { next(); picked = nil; checked = false }
                    else if let p = picked {
                        checked = true
                        // Abstaining is neither right nor wrong. Scoring it
                        // correct would make it the only sensible answer,
                        // since the particle varies 91.3% of the time;
                        // scoring it wrong would punish honesty.
                        if p == 2 { abstain += 1 }
                        else if (p == 0) == span.rendered { right += 1 }
                        else { wrong += 1 }
                    }
                }
                .font(.system(size: 17, weight: .semibold)).foregroundColor(.white)
                .lineLimit(1).fixedSize(horizontal: true, vertical: false)
                .padding(.horizontal, 20).padding(.vertical, 11)
                .background(picked == nil && !checked ? c.muted : c.act).cornerRadius(9)
                .disabled(picked == nil && !checked)
                Spacer()
            }.padding(.horizontal, 20).padding(.top, 10).padding(.bottom, 26)
        }.background(c.paper)
    }
    private var said: Bool { picked == 0 }
    private var verdict: String {
        if picked == 2 { return "Abstained — neither right nor wrong." }
        return said == span.rendered ? "Correct." : "Not here."
    }
}

/// One term, every rendering he is recorded using. The ambiguity is the
/// lesson and is deliberately not resolved.
struct SecondThoughtView: View {
    let item: SecondThought
    let next: () -> Void
    @Environment(\.colorScheme) private var scheme
    @State private var shown = false

    var body: some View {
        let c = Ink.of(scheme)
        ScrollView {
            VStack(alignment: .leading, spacing: 14) {
                Text("He renders this term more than one way. How many can you name before you look?")
                    .font(.system(size: 15)).foregroundColor(c.muted)
                Text(item.tib).font(.system(size: 44)).foregroundColor(c.ink)
                Text(item.wylie).font(.system(size: 14)).foregroundColor(c.muted)
                if shown {
                    Divider()
                    Text("Everything he has been recorded writing for it, most used first:")
                        .font(.system(size: 13)).foregroundColor(c.muted)
                    ForEach(Array(item.renderings.enumerated()), id: \.offset) { _, r in
                        HStack {
                            Text(r.eng).font(.system(size: 18)).foregroundColor(c.ink)
                            Text("×\(r.n)").font(.system(size: 12)).foregroundColor(c.muted)
                            Spacer()
                        }
                    }
                    Text("None of these is the right translation of the term. They are all his, and which one fits depends on the passage — that is the lesson, and this drill does not resolve it.")
                        .font(.system(size: 12)).foregroundColor(c.muted)
                    Text("TENTATIVE — machine-aligned from his courses, awaiting his ruling.")
                        .font(.system(size: 11)).foregroundColor(c.machine)
                }
            }.padding(20)
        }
        VStack(spacing: 0) {
            Divider()
            HStack(spacing: 14) {
                Button(shown ? "Next term" : "Reveal") {
                    if shown { next(); shown = false } else { shown = true }
                }
                .font(.system(size: 17, weight: .semibold)).foregroundColor(.white)
                .lineLimit(1).fixedSize(horizontal: true, vertical: false)
                .padding(.horizontal, 20).padding(.vertical, 11)
                .background(c.act).cornerRadius(9)
                Spacer()
            }.padding(.horizontal, 20).padding(.top, 10).padding(.bottom, 26)
        }.background(c.paper)
    }
}


/// How many pieces does this span split into? The SPLIT is scored, not the
/// translation — which is what makes it a reading skill rather than a
/// vocabulary test.
struct PeelView: View {
    let span: PeelSpan
    @ObservedObject var deck: Deck
    let next: () -> Void
    @Environment(\.colorScheme) private var scheme
    @State private var picked: Int? = nil
    @State private var checked = false
    private var options: [Int] { Array(max(2, span.pieces.count - 1)...(max(2, span.pieces.count - 1) + 3)) }
    private var answer: Int { options.firstIndex(of: span.pieces.count) ?? 0 }

    var body: some View {
        let c = Ink.of(scheme)
        ScrollView {
            VStack(alignment: .leading, spacing: 14) {
                Text("Read this span. How many pieces does it split into at the next level down? Decide before you check — the split is what is scored, not the translation.")
                    .font(.system(size: 15)).foregroundColor(c.muted)
                Text(span.parent).font(.system(size: 26)).foregroundColor(c.ink)
                Text("[\(span.course):\(span.seq)]")
                    .font(.system(size: 12)).foregroundColor(c.muted)
                Divider()
                ForEach(Array(options.enumerated()), id: \.offset) { i, n in
                    Button { if !checked { picked = i } } label: {
                        HStack(spacing: 11) {
                            Image(systemName: picked == i ? "largecircle.fill.circle" : "circle")
                                .foregroundColor(picked == i ? c.act : c.muted)
                            Text("\(n) pieces").font(.system(size: 17))
                                .foregroundColor(checked && i == answer ? c.act : c.ink)
                            Spacer()
                        }.padding(.vertical, 6)
                    }.buttonStyle(.plain)
                }
                if checked {
                    Text(picked == answer ? "Correct."
                         : "Not quite — it splits into \(span.pieces.count).")
                        .font(.system(size: 17, weight: .semibold))
                        .foregroundColor(picked == answer ? c.act : c.machine)
                    ForEach(Array(span.pieces.enumerated()), id: \.offset) { i, p in
                        Text("\(i + 1)  \(p)").font(.system(size: 20))
                            .foregroundColor(c.ink)
                    }
                    Text("The nesting is the alignment layer's own, TENTATIVE throughout — machine-matched from his courses and awaiting his ruling.")
                        .font(.system(size: 11)).foregroundColor(c.machine)
                }
            }.padding(20)
        }
        VStack(spacing: 0) {
            Divider()
            HStack(spacing: 14) {
                Button(checked ? "Next span" : "Check") {
                    if checked { next(); picked = nil; checked = false }
                    else if picked != nil { checked = true; deck.record(correct: picked == answer) }
                }
                .font(.system(size: 17, weight: .semibold)).foregroundColor(.white)
                .lineLimit(1).fixedSize(horizontal: true, vertical: false)
                .padding(.horizontal, 20).padding(.vertical, 11)
                .background(picked == nil && !checked ? c.muted : c.act).cornerRadius(9)
                .disabled(picked == nil && !checked)
                Spacer()
            }.padding(.horizontal, 20).padding(.top, 10).padding(.bottom, 26)
        }.background(c.paper)
    }
}


/// Boundary hunt. Tap the word each clause ends ON. The punctuation is gone
/// from what is shown, so in the hard pool nothing on the page marks the
/// boundary — only the particle does.
struct BoundaryView: View {
    let item: BoundaryItem
    @ObservedObject var deck: Deck
    let next: () -> Void
    @Environment(\.colorScheme) private var scheme
    @State private var marks: Set<Int> = []
    @State private var checked = false

    private var key: Set<Int> { Set(item.ends) }
    private var waived: Set<Int> { Set(item.unscored) }
    private var perfect: Bool {
        key.isSubset(of: marks) &&
        marks.subtracting(key).subtracting(waived).isEmpty
    }

    private func colour(_ i: Int, _ c: Ink) -> Color {
        guard checked else { return marks.contains(i) ? c.act : c.ink }
        if key.contains(i) { return marks.contains(i) ? c.act : c.machine }
        if marks.contains(i) { return waived.contains(i) ? c.gold : c.machine }
        return c.ink
    }

    var body: some View {
        let c = Ink.of(scheme)
        ScrollView {
            VStack(alignment: .leading, spacing: 13) {
                Text("The punctuation has been stripped. Tap the word each clause ends ON.")
                    .font(.system(size: 15)).foregroundColor(c.muted)
                Text(item.hard
                     ? "Hard pool: this segment carried no punctuation at all. Nothing on the page marks the boundary — only the particle does."
                     : "Warm-up pool: the scribe marked these boundaries himself, so the key is attested.")
                    .font(.system(size: 12))
                    .foregroundColor(item.hard ? c.machine : c.muted)
                Text("[\(item.course):\(item.seq)] — there is a key: his own English for this segment.")
                    .font(.system(size: 11)).foregroundColor(c.muted)
                Divider()
                FlowTokens(tokens: item.tokens, ink: c,
                           colour: { colour($0, c) },
                           marked: { checked ? key.contains($0) : marks.contains($0) }) { i in
                    if !checked {
                        if marks.contains(i) { marks.remove(i) } else { marks.insert(i) }
                    }
                }
                if checked {
                    let found = key.filter { marks.contains($0) }.count
                    let spurious = marks.subtracting(key).subtracting(waived).count
                    Text(perfect ? "Every boundary found."
                         : "\(found) of \(key.count) found\(spurious > 0 ? ", \(spurious) where no clause ends" : "").")
                        .font(.system(size: 17, weight: .semibold))
                        .foregroundColor(perfect ? c.act : c.machine)
                    ForEach(Array(item.ends.enumerated()), id: \.offset) { i, e in
                        Text("after word \(e + 1) — \(item.functions[i])  (\(item.attested[i] ? "the scribe marked this one" : "engine ruling, not his own mark"))")
                            .font(.system(size: 12)).foregroundColor(c.muted)
                    }
                    let hit = marks.intersection(waived).count
                    if hit > 0 {
                        Text("\(hit) of your marks sat on an ambiguous na. The splitter dropped that split because the word before it carries no verb evidence — which is not the same as knowing it is a noun. Those are not scored against you.")
                            .font(.system(size: 12)).foregroundColor(c.gold)
                    }
                    EnglishHint(text: item.english, ink: c)
                }
            }.padding(20)
        }
        VStack(spacing: 0) {
            Divider()
            HStack(spacing: 14) {
                Button(checked ? "Next segment" : "Check") {
                    if checked { next(); marks = []; checked = false }
                    else { checked = true; deck.record(correct: perfect) }
                }
                .font(.system(size: 17, weight: .semibold)).foregroundColor(.white)
                .lineLimit(1).fixedSize(horizontal: true, vertical: false)
                .padding(.horizontal, 20).padding(.vertical, 11)
                .background(marks.isEmpty && !checked ? c.muted : c.act)
                .cornerRadius(9)
                .disabled(marks.isEmpty && !checked)
                Spacer()
            }.padding(.horizontal, 20).padding(.top, 10).padding(.bottom, 26)
        }.background(c.paper)
    }
}

/// Reading order. The chunks are shown in the order they are WRITTEN; tap them
/// in the order his English takes them. This is the drill for the difficulty
/// Adam named before any of the others: not what the words mean, but what
/// order to read them in, and which way round.
///
/// The parent's English is withheld until the reveal for a reason — it
/// contains every child's English, in his order. Showing it would not be a
/// hint, it would be the answer.
struct ReadOrderView: View {
    let item: ReadOrderItem
    @ObservedObject var deck: Deck
    let next: () -> Void
    @Environment(\.colorScheme) private var scheme
    @State private var picked: [Int] = []
    @State private var checked = false

    private var correct: Bool { picked == item.answer }
    private var placed: Bool { picked.count == item.kids.count }
    private func mine(_ i: Int) -> Int? {
        guard let at = picked.firstIndex(of: i) else { return nil }
        return at + 1
    }
    private func his(_ i: Int) -> Int { (item.answer.firstIndex(of: i) ?? 0) + 1 }

    var body: some View {
        let c = Ink.of(scheme)
        ScrollView {
            VStack(alignment: .leading, spacing: 12) {
                Text("These chunks are shown in the order they are written. Tap them in the order his English takes them.")
                    .font(.system(size: 15)).foregroundColor(c.muted)
                Text("ATTESTED — the key is his own English for this very span, not the engine's ruling.")
                    .font(.system(size: 12)).foregroundColor(c.act)
                Text("[\(item.ref)]")
                    .font(.system(size: 11)).foregroundColor(c.muted)
                Divider()
                ForEach(Array(item.kids.enumerated()), id: \.offset) { i, k in
                    Button {
                        guard !checked else { return }
                        if let at = picked.firstIndex(of: i) { picked.remove(at: at) }
                        else { picked.append(i) }
                    } label: {
                        HStack(alignment: .top, spacing: 12) {
                            Text("\(i + 1)")
                                .font(.system(size: 13, weight: .medium))
                                .foregroundColor(c.muted)
                                .frame(width: 18, alignment: .trailing)
                                .padding(.top, 8)
                            VStack(alignment: .leading, spacing: 3) {
                                Text(k).font(.system(size: 26))
                                    .foregroundColor(c.ink)
                                    .fixedSize(horizontal: false, vertical: true)
                                if checked {
                                    Text(item.kidsEng[i])
                                        .font(.system(size: 13, design: .serif)).italic()
                                        .foregroundColor(c.muted)
                                        .fixedSize(horizontal: false, vertical: true)
                                }
                            }
                            Spacer(minLength: 8)
                            if checked {
                                Text("his \(his(i))")
                                    .font(.system(size: 12, weight: .semibold))
                                    .foregroundColor(mine(i) == his(i) ? c.act : c.machine)
                                    .padding(.top, 8)
                            } else if let m = mine(i) {
                                Text("\(m)")
                                    .font(.system(size: 15, weight: .bold))
                                    .foregroundColor(c.act)
                                    .frame(width: 27, height: 27)
                                    .background(Circle().fill(c.act.opacity(0.15)))
                                    .padding(.top, 4)
                            }
                        }
                        .padding(.vertical, 5)
                        .frame(maxWidth: .infinity, alignment: .leading)
                        .contentShape(Rectangle())
                    }
                    .buttonStyle(.plain)
                    Divider().opacity(0.35)
                }
                if checked {
                    Text(correct
                         ? "That is his order."
                         : "His order is \(item.answer.map { String($0 + 1) }.joined(separator: " ")) — yours was \(picked.map { String($0 + 1) }.joined(separator: " ")).")
                        .font(.system(size: 17, weight: .semibold))
                        .foregroundColor(correct ? c.act : c.machine)
                    EnglishHint(text: item.parentEng, ink: c)
                    Text("Clause order is almost never inverted — 5% of 972 measured pairs. Inside a clause it is a different story, which is what this drills.")
                        .font(.system(size: 11)).foregroundColor(c.muted)
                } else {
                    Text(picked.isEmpty
                         ? "Tap the chunk his English takes first."
                         : "So far: \(picked.map { String($0 + 1) }.joined(separator: " "))")
                        .font(.system(size: 13))
                        .foregroundColor(picked.isEmpty ? c.muted : c.ink)
                }
            }.padding(20)
        }
        VStack(spacing: 0) {
            Divider()
            HStack(spacing: 14) {
                Button(checked ? "Next span" : "Check") {
                    if checked {
                        next(); picked = []; checked = false
                    } else {
                        checked = true; deck.record(correct: correct)
                    }
                }
                .font(.system(size: 17, weight: .semibold)).foregroundColor(.white)
                .lineLimit(1).fixedSize(horizontal: true, vertical: false)
                .padding(.horizontal, 20).padding(.vertical, 11)
                .background(!placed && !checked ? c.muted : c.act)
                .cornerRadius(9)
                .disabled(!placed && !checked)
                if !checked && !picked.isEmpty {
                    Button("Start over") { picked = [] }
                        .font(.system(size: 15)).foregroundColor(c.muted)
                }
                Spacer()
            }.padding(.horizontal, 20).padding(.top, 10).padding(.bottom, 26)
        }.background(c.paper)
    }
}

/// Where this pack came from. The pack has always carried its own provenance --
/// who built it, from which corpus, on what terms, and from which commit -- and
/// the phone decoded it and showed none of it. Provenance you cannot see is not
/// provenance, and the build stamp in particular is the only way to tell a
/// stale pack from a fresh one.
struct ProvenanceSheet: View {
    let meta: PackMeta?
    let ink: Ink
    @Environment(\.dismiss) private var dismiss

    var body: some View {
        NavigationStack {
            ScrollView {
                VStack(alignment: .leading, spacing: 14) {
                    if let m = meta {
                        row("Built by", m.built_by)
                        row("From", m.source)
                        row("Built from", m.built_from ?? "unstamped — this pack cannot say which build it came from")
                        VStack(alignment: .leading, spacing: 4) {
                            Text("Terms").font(.system(size: 12, weight: .semibold))
                                .foregroundColor(ink.muted)
                            Text(m.tier)
                                .font(.system(size: 15, design: .serif))
                                .foregroundColor(ink.ink)
                                .fixedSize(horizontal: false, vertical: true)
                        }
                    } else {
                        Text("No pack is loaded, so there is nothing to account for.")
                            .font(.system(size: 15)).foregroundColor(ink.muted)
                    }
                    Text("The phone generates nothing. Every exercise here was "
                         + "built on the Mac from his corpus, and anything the "
                         + "engine ruled rather than he wrote says so on the card.")
                        .font(.system(size: 12)).foregroundColor(ink.muted)
                        .fixedSize(horizontal: false, vertical: true)
                }.padding(20)
            }
            .background(ink.paper.ignoresSafeArea())
            .navigationTitle("This pack")
            .navigationBarTitleDisplayMode(.inline)
            .toolbar {
                ToolbarItem(placement: .topBarTrailing) {
                    Button("Done") { dismiss() }
                }
            }
        }
    }

    private func row(_ k: String, _ v: String) -> some View {
        VStack(alignment: .leading, spacing: 3) {
            Text(k).font(.system(size: 12, weight: .semibold))
                .foregroundColor(ink.muted)
            Text(v).font(.system(size: 15, design: .monospaced))
                .foregroundColor(ink.ink)
                .fixedSize(horizontal: false, vertical: true)
        }
    }
}

/// Tibetan laid out as tappable words that wrap. Each carries its 1-based
/// number, because the reveal refers to words by number.
struct FlowTokens: View {
    let tokens: [String]
    let ink: Ink
    let colour: (Int) -> Color
    let marked: (Int) -> Bool
    let tap: (Int) -> Void

    var body: some View {
        FlowLayout(spacing: 6) {
            ForEach(Array(tokens.enumerated()), id: \.offset) { i, t in
                Button { tap(i) } label: {
                    HStack(spacing: 2) {
                        Text("\(i + 1)").font(.system(size: 10))
                            .foregroundColor(ink.muted)
                        Text(t).font(.system(size: 21))
                            .foregroundColor(colour(i))
                        if marked(i) {
                            Text("|").font(.system(size: 19, weight: .bold))
                                .foregroundColor(colour(i))
                        }
                    }
                    .padding(.horizontal, 4).padding(.vertical, 3)
                }.buttonStyle(.plain)
            }
        }
    }
}

/// Wraps its children onto as many rows as they need. Tibetan words differ
/// too much in width for a fixed column count — a grid either clips the long
/// ones or wastes half the row on the short ones, and this drill is read as
/// running text.
struct FlowLayout: Layout {
    var spacing: CGFloat = 6

    func sizeThatFits(proposal: ProposedViewSize,
                      subviews: Subviews, cache: inout ()) -> CGSize {
        let maxW = proposal.width ?? .infinity
        var x: CGFloat = 0, y: CGFloat = 0, rowH: CGFloat = 0
        for v in subviews {
            let sz = v.sizeThatFits(.unspecified)
            if x > 0 && x + sz.width > maxW { x = 0; y += rowH + spacing; rowH = 0 }
            x += sz.width + spacing
            rowH = max(rowH, sz.height)
        }
        return CGSize(width: maxW == .infinity ? x : maxW, height: y + rowH)
    }

    func placeSubviews(in bounds: CGRect, proposal: ProposedViewSize,
                       subviews: Subviews, cache: inout ()) {
        var x = bounds.minX, y = bounds.minY, rowH: CGFloat = 0
        for v in subviews {
            let sz = v.sizeThatFits(.unspecified)
            if x > bounds.minX && x + sz.width > bounds.maxX {
                x = bounds.minX; y += rowH + spacing; rowH = 0
            }
            v.place(at: CGPoint(x: x, y: y), proposal: ProposedViewSize(sz))
            x += sz.width + spacing
            rowH = max(rowH, sz.height)
        }
    }
}

/// Known here / known anywhere. Stage 0 presents the word inside the segment
/// it was met in; stage 1 presents it somewhere else entirely. The two are
/// tracked apart because they are different claims — and a deck that merged
/// them would report "known" for a word recognised only by its neighbours.
struct VocabView: View {
    let item: VocabItem
    @ObservedObject var deck: Deck
    let next: () -> Void
    @Environment(\.colorScheme) private var scheme
    @State private var stage = 0
    @State private var revealed = false

    private var ctx: VocabContext { stage == 0 ? item.here : item.anywhere }

    var body: some View {
        let c = Ink.of(scheme)
        ScrollView {
            VStack(alignment: .leading, spacing: 14) {
                Text(stage == 0 ? "KNOWN HERE" : "KNOWN ANYWHERE")
                    .font(.system(size: 11, weight: .semibold)).tracking(2)
                    .foregroundColor(c.gold)
                Text(stage == 0
                     ? "The segment you met it in. Do you know this word?"
                     : "A different segment entirely. Same word — do you still know it?")
                    .font(.system(size: 15)).foregroundColor(c.muted)
                Text(item.tib).font(.system(size: 30)).foregroundColor(c.ink)
                Divider()
                Text(ctx.tib).font(.system(size: 22)).foregroundColor(c.ink)
                Text("[\(ctx.course):\(ctx.seq)]")
                    .font(.system(size: 11)).foregroundColor(c.muted)
                if revealed {
                    EnglishHint(text: ctx.english, ink: c)
                    Text("aligned evidence, TENTATIVE — \(item.aligned_eng)")
                        .font(.system(size: 12)).foregroundColor(c.machine)
                    Text("Machine-matched from his courses, not his dictionary gloss. His English above is the attested part.")
                        .font(.system(size: 11)).foregroundColor(c.muted)
                }
            }.padding(20)
        }
        VStack(spacing: 0) {
            Divider()
            HStack(spacing: 12) {
                if !revealed {
                    Button("Reveal") { revealed = true }
                        .font(.system(size: 17, weight: .semibold))
                        .foregroundColor(.white).lineLimit(1)
                        .fixedSize(horizontal: true, vertical: false)
                        .padding(.horizontal, 20).padding(.vertical, 11)
                        .background(c.act).cornerRadius(9)
                } else {
                    Button("I knew it") { advance(true) }
                        .font(.system(size: 16, weight: .semibold))
                        .foregroundColor(.white).lineLimit(1)
                        .fixedSize(horizontal: true, vertical: false)
                        .padding(.horizontal, 16).padding(.vertical, 11)
                        .background(c.act).cornerRadius(9)
                    Button("I did not") { advance(false) }
                        .font(.system(size: 16)).foregroundColor(c.ink)
                        .lineLimit(1)
                        .fixedSize(horizontal: true, vertical: false)
                        .padding(.horizontal, 16).padding(.vertical, 11)
                        .overlay(RoundedRectangle(cornerRadius: 9)
                            .stroke(c.muted, lineWidth: 1))
                }
                Spacer()
            }.padding(.horizontal, 20).padding(.top, 10).padding(.bottom, 26)
        }.background(c.paper)
    }

    /// Knowing it HERE promotes to the transfer question; knowing it ANYWHERE
    /// finishes the word. Not knowing it at either stage sends it back to the
    /// segment it was met in — the context is the scaffold, so the repair
    /// belongs where the scaffold is.
    private func advance(_ knew: Bool) {
        deck.record(correct: knew)
        revealed = false
        if knew && stage == 0 { stage = 1 } else { stage = 0; next() }
    }
}

// MARK: - Trainer

struct TrainerView: View {
    let passage: Passage
    @ObservedObject var deck: Deck
    let next: () -> Void
    @Environment(\.colorScheme) private var scheme
    @State private var shown: Set<Int> = []
    /// The ungraded reading-order gate. Default OFF — it is an extra step,
    /// and a learner who does not want it should never meet it.
    @AppStorage("trainer.gate") private var gateOn = false
    @State private var gateAnswer: Int? = nil

    private let layers = ["1 · chunks", "2 · particle roles", "3 · reading order",
                          "4 · the verb", "5 · answer key", "6 · full parse"]

    var body: some View {
        let c = Ink.of(scheme)
        ScrollView {
            VStack(alignment: .leading, spacing: 16) {
                Text("Read it yourself first, then reveal one layer at a time.")
                    .font(.system(size: 15)).foregroundColor(c.muted)

                // The key / no-key badge. Every pack passage is drawn from
                // the corpus, so on the phone this state is always "there is
                // a key" — the phone has no out-of-corpus surface to reach
                // the other state from. It is still said rather than assumed:
                // knowing the guidance is checked against his own English is
                // the thing a working translator most wants to know, and a
                // learner should never have to infer it from silence.
                HStack(spacing: 6) {
                    Image(systemName: "checkmark.seal.fill")
                        .font(.system(size: 12)).foregroundColor(c.act)
                    Text("THERE IS A KEY — his own English for this segment · [\(passage.course):\(passage.seq)]")
                        .font(.system(size: 10, weight: .semibold)).tracking(0.6)
                        .foregroundColor(c.act)
                }
                .padding(.horizontal, 9).padding(.vertical, 5)
                .background(c.act.opacity(0.10)).cornerRadius(6)

                Text(passage.tibetan)
                    .font(.system(size: 26))
                    .foregroundColor(c.ink)
                    .frame(maxWidth: .infinity, alignment: .leading)
                    .padding(.vertical, 6)

                // The layers below describe ONE clause of the passage above —
                // a median of about a third of it. Without saying so they read
                // as an account of everything on screen. The desktop already
                // discloses the same mismatch on its chunk-order drills
                // (app/main.cpp:22683); this is that disclosure, carried over.
                if let share = passage.clause_share, share < 90 {
                    Text("The layers below cover one clause of this passage — about \(share)% of it.")
                        .font(.system(size: 12))
                        .foregroundColor(c.machine)
                        .padding(.top, 2)
                }

                FlowChips(labels: layers, on: shown, ink: c) { i in
                    if shown.contains(i) { shown.remove(i) } else { shown.insert(i) }
                }

                if passage.verb_confident {
                    Toggle(isOn: $gateOn) {
                        Text("Ask me which chunk I read first, before layer 3 opens")
                            .font(.system(size: 12)).foregroundColor(c.muted)
                    }
                    .toggleStyle(.switch)
                    .tint(c.act)
                }

                if shown.contains(0) {
                    Layer("Chunks", ink: c) {
                        ForEach(Array(passage.chunks.enumerated()), id: \.offset) { i, ch in
                            Text("\(i + 1).  \(ch)").font(.system(size: 21)).foregroundColor(c.ink)
                        }
                    }
                }
                if shown.contains(1) {
                    Layer("Particle roles", ink: c) {
                        ForEach(Array(passage.roles.enumerated()), id: \.offset) { i, r in
                            if !r.isEmpty {
                                Text("\(i + 1).  \(r)").font(.system(size: 17)).foregroundColor(c.gold)
                            }
                        }
                    }
                }
                // One ungraded gate before the reading-order layer opens:
                // which chunk did you read first? UNGRADED and default off.
                // The attempt is recorded with NO correct bit — the
                // retrieval-practice gain comes from attempting and then
                // seeing, not from being scored, so the ungraded form keeps
                // the documented benefit without scoring a learner against
                // engine guidance. Offered only where the verb is attested,
                // because the plan is only worth predicting when the engine
                // itself is confident.
                if shown.contains(2), passage.verb_confident,
                   gateOn, gateAnswer == nil, passage.chunks.count >= 2 {
                    Layer("Before it opens — which chunk did you read first?",
                          ink: c) {
                        Text("Ungraded. Nothing is scored and no counter moves.")
                            .font(.system(size: 12)).foregroundColor(c.muted)
                        ForEach(Array(passage.chunks.enumerated()), id: \.offset) { i, ch in
                            Button { gateAnswer = i } label: {
                                HStack {
                                    Text("\(i + 1).  \(ch)")
                                        .font(.system(size: 19))
                                        .foregroundColor(c.ink)
                                    Spacer()
                                }.padding(.vertical, 4)
                            }.buttonStyle(.plain)
                        }
                    }
                }
                if shown.contains(2), let g = gateAnswer, passage.verb_confident, gateOn {
                    Text("You read chunk \(g + 1) first. The engine's plan is below — guidance, not a mark.")
                        .font(.system(size: 13)).foregroundColor(c.gold)
                }
                if shown.contains(2), !(passage.verb_confident && gateOn && gateAnswer == nil) {
                    Layer("Reading order", ink: c) {
                        // NOT re-sorted. The Mac computes this plan and hands
                        // it over in CHUNK order, with each step carrying the
                        // place it is read at; the desktop renders it that way,
                        // so a reader sees the chunks as they lie on the page
                        // and the numbers tell them where to jump. Sorting by
                        // `order` here silently turned it into a different
                        // reading — same data, different lesson — and the two
                        // apps disagreed about what the plan said.
                        ForEach(passage.plan, id: \.chunk) { st in
                            Text(st.order == 0
                                 ? "•  reads with the next chunk — \(st.how)"
                                 : "\(st.order).  \(st.how)")
                                .font(.system(size: 16)).foregroundColor(c.ink)
                        }
                        Text("Engine guidance, labelled guidance — not Geshe Michael Roach's word.")
                            .font(.system(size: 12)).foregroundColor(c.muted).padding(.top, 4)
                    }
                }
                if shown.contains(3) {
                    Layer("The verb — it frames the clause", ink: c) {
                        // The desktop WITHHOLDS a verb it cannot attest rather
                        // than printing the guess with a caveat under it
                        // (app/main.cpp:21498). A guess in the answer position is
                        // still a guess, and this is the surface with the least
                        // room for a caveat. 253 of the 1,000 packed passages are
                        // in this state, so the difference is not academic.
                        if passage.verb.isEmpty || !passage.verb_confident {
                            Text("No verb the dictionary can confirm in this clause.")
                                .font(.system(size: 16)).foregroundColor(c.muted)
                            Text("A candidate was found but is unverified, so it is not shown.")
                                .font(.system(size: 12)).foregroundColor(c.machine).padding(.top, 2)
                        } else {
                            Text(passage.verb).font(.system(size: 21)).foregroundColor(c.ink)
                            if !passage.verb_evidence.isEmpty {
                                Text(passage.verb_evidence).font(.system(size: 14)).foregroundColor(c.muted)
                            }
                        }
                    }
                }
                if shown.contains(4) {
                    Layer("Geshe Michael Roach's English", ink: c) {
                        Text(passage.english)
                            .font(.system(size: 18, design: .serif)).italic()
                            .foregroundColor(c.ink)
                    }
                }
                if shown.contains(5) {
                    Layer("Full parse", ink: c) {
                        ForEach(Array(passage.chunks.enumerated()), id: \.offset) { i, ch in
                            let role = i < passage.roles.count ? passage.roles[i] : ""
                            let ord = passage.plan.first { $0.chunk == i }?.order
                            Text("\(ch)   \(role.isEmpty ? "" : "· \(role)")\(ord.map { $0 == 0 ? "  · reads with the next" : "  · read \($0)" } ?? "")")
                                .font(.system(size: 18)).foregroundColor(c.ink)
                        }
                    }
                }

                HStack {
                    Button("New passage") { deck.recordRead(); shown = []; next() }
                        .font(.system(size: 17, weight: .semibold)).foregroundColor(.white)
                        .padding(.horizontal, 18).padding(.vertical, 11)
                        .background(c.act).cornerRadius(9)
                    Spacer()
                    Text("[\(passage.course):\(passage.seq)]")
                        .font(.system(size: 12)).foregroundColor(c.muted)
                }
                .padding(.top, 10)
            }
            .padding(20)
            // the read/drills counter floats in a bottomBar over this
            // scroll view, so the last line needs room to clear it
            .padding(.bottom, 72)
        }
        .background(c.paper.ignoresSafeArea())
    }
}

struct Layer<Content: View>: View {
    let title: String
    let ink: Ink
    @ViewBuilder let content: Content
    init(_ title: String, ink: Ink, @ViewBuilder content: () -> Content) {
        self.title = title; self.ink = ink; self.content = content()
    }
    var body: some View {
        VStack(alignment: .leading, spacing: 6) {
            Text(title.uppercased())
                .font(.system(size: 11, weight: .semibold)).tracking(1.2)
                .foregroundColor(ink.gold)
            content
        }
        .frame(maxWidth: .infinity, alignment: .leading)
        .padding(12)
        .background(ink.plaque)
        .cornerRadius(8)
    }
}

struct FlowChips: View {
    let labels: [String]
    let on: Set<Int>
    let ink: Ink
    let tap: (Int) -> Void
    var body: some View {
        VStack(alignment: .leading, spacing: 8) {
            ForEach(0..<((labels.count + 1) / 2), id: \.self) { row in
                HStack(spacing: 8) {
                    ForEach(0..<2, id: \.self) { col in
                        let i = row * 2 + col
                        if i < labels.count {
                            Button { tap(i) } label: {
                                Text(labels[i])
                                    .font(.system(size: 14, weight: on.contains(i) ? .semibold : .regular))
                                    .foregroundColor(on.contains(i) ? .white : ink.ink)
                                    .padding(.horizontal, 12).padding(.vertical, 8)
                                    .frame(maxWidth: .infinity)
                                    .background(on.contains(i) ? ink.act : ink.plaque)
                                    .cornerRadius(7)
                            }.buttonStyle(.plain)
                        }
                    }
                }
            }
        }
    }
}

// MARK: - Drills

struct DrillView: View {
    let drill: Drill
    @ObservedObject var deck: Deck
    /// The skill the draw is aimed at, if any. The phone cannot GENERATE
    /// a targeted drill — the pack is built on the Mac — so it filters the
    /// pre-tagged pack instead, reaching the same place by the other road.
    @Binding var aim: String?
    let next: () -> Void
    @Environment(\.colorScheme) private var scheme
    @State private var picked: Int? = nil
    @State private var checked = false
    /// One option struck out by the hint, if it was used on this drill.
    @State private var ruledOut: Int? = nil
    /// The skill the draw is aimed at, if any. The phone cannot GENERATE a
    /// targeted drill — the pack is built on the Mac — so it filters the
    /// pre-tagged pack instead, which reaches the same place by the other
    /// road. When no drill in the pack carries the skill, it says so.
    @State private var showWeak = false
    /// Whether the learner asked for the full recorded gloss list.
    @State private var expanded = false
    private var isRight: Bool { picked == drill.answer }

    var body: some View {
        let c = Ink.of(scheme)
        ScrollView {
            VStack(alignment: .leading, spacing: 15) {
                if drill.title {
                    Text("TITLE LINE — a work's title, not running prose")
                        .font(.system(size: 11, weight: .semibold)).tracking(1)
                        .foregroundColor(c.gold)
                }
                Text("Which chunk fills the blank?")
                    .font(.system(size: 15)).foregroundColor(c.muted)

                EnglishHint(text: drill.english, ink: c)

                (Text(drill.before)
                    + Text("  [ … ]  ").foregroundColor(c.machine).bold()
                    + Text(drill.after))
                    .font(.system(size: 24))
                    .foregroundColor(c.ink)
                    .frame(maxWidth: .infinity, alignment: .leading)

                Divider()

                ForEach(Array(drill.options.enumerated()), id: \.offset) { i, opt in
                    Button { if !checked { picked = i } } label: {
                        HStack(alignment: .top, spacing: 11) {
                            Image(systemName: picked == i ? "largecircle.fill.circle" : "circle")
                                .foregroundColor(picked == i ? c.act : c.muted)
                            Text(opt).font(.system(size: 22))
                                .foregroundColor(ruledOut == i ? c.muted : colorFor(i, c))
                                .strikethrough(ruledOut == i)
                                .multilineTextAlignment(.leading)
                            Spacer()
                        }.padding(.vertical, 6)
                    }.buttonStyle(.plain)
                }

                if checked {
                    VStack(alignment: .leading, spacing: 5) {
                        // "the text", not "his text": the Tibetan source is the
                            // classical work, not Geshe Michael Roach's writing.
                            // The English below IS his, and stays attributed.
                            Text(isRight ? "Correct." : "Not yet — the text has \(drill.options[drill.answer]).")
                            .font(.system(size: 17, weight: .semibold))
                            .foregroundColor(isRight ? c.act : c.machine)
                        // Shown the SAME whether the answer was right or
                        // wrong. Being right is exactly when Adam reported
                        // learning nothing, so withholding it from a correct
                        // answer would miss the point of the request.
                        if let ms = drill.means, !ms.isEmpty {
                            ForEach(Array(ms.enumerated()), id: \.offset) { _, m in
                                VStack(alignment: .leading, spacing: 3) {
                                    Text(m.wylie).font(.system(size: 15, weight: .semibold))
                                        .foregroundColor(c.ink)
                                    Text("Geshe Michael Roach has: "
                                         + (expanded ? m.glosses
                                            : Array(m.glosses.prefix(8)))
                                           .joined(separator: " · ")
                                         + (expanded && m.more > 0
                                            ? " (+\(m.more) more not carried)" : ""))
                                        .font(.system(size: 14)).foregroundColor(c.muted)
                                    if !m.used.isEmpty {
                                        Text("\(m.used.joined(separator: ", ")) — his word here, above.")
                                            .font(.system(size: 13)).foregroundColor(c.act)
                                    } else {
                                        Text("None of these appears verbatim in his English above.")
                                            .font(.system(size: 13)).foregroundColor(c.muted)
                                    }
                                    if m.provisional {
                                        Text("\(m.tier) [PROVISIONAL] — not his own English")
                                            .font(.system(size: 12)).foregroundColor(c.machine)
                                    }
                                    // "Look up" is reachable only here, after
                                    // answering. Before answering it would
                                    // replace the retrieval effort that makes a
                                    // drill work at all. It expands what this
                                    // pack holds; the phone ships no dictionary
                                    // and does not pretend to one.
                                    if m.glosses.count > 8 && !expanded {
                                        Button("Look up — \(m.glosses.count - 8) more of his equivalents") {
                                            expanded = true
                                        }
                                        .font(.system(size: 13)).foregroundColor(c.act)
                                    }
                                }
                                .padding(.vertical, 4)
                            }
                        }
                        if !drill.role.isEmpty {
                            Text("role of the blanked chunk: \(drill.role)")
                                .font(.system(size: 14)).foregroundColor(c.muted)
                        }
                        Text("[\(drill.course):\(drill.seq)]")
                            .font(.system(size: 12)).foregroundColor(c.muted)
                    }
                }

                HStack(spacing: 14) {
                    // The hint deliberately reveals almost nothing. Showing the
                    // grammatical role instead would leave exactly one option
                    // standing in roughly half of all drills, which is not a
                    // hint but the answer. Removing one wrong option leaves
                    // three, and the learner still has to read.
                    if let a = aim {
                        Button("training: \(a)  ×") { aim = nil }
                            .font(.system(size: 13)).foregroundColor(c.machine)
                    } else if !deck.weakSpots().isEmpty {
                        Button("My weak spots") { showWeak = true }
                            .font(.system(size: 13)).foregroundColor(c.muted)
                    }
                    if !checked && ruledOut == nil {
                        Button("Rule out one") {
                            let wrong = (0..<drill.options.count).filter {
                                $0 != drill.answer && $0 != picked
                            }
                            ruledOut = wrong.randomElement()
                        }
                        .font(.system(size: 15)).foregroundColor(c.muted)
                            .lineLimit(1)
                    }
                    Spacer()
                }.padding(.top, 6)
                // A second row: four controls on one line squeezed the primary
                // button until its label broke mid-word.
                HStack(spacing: 14) {
                    Button(checked ? "New drill" : "Check") {
                        if checked { next(); picked = nil; checked = false; ruledOut = nil; expanded = false }
                        else if picked != nil {
                            checked = true
                            deck.record(correct: isRight)
                            deck.record(skill: drill.skill, correct: isRight)
                        }
                    }
                    .font(.system(size: 17, weight: .semibold)).foregroundColor(.white)
                    .lineLimit(1)
                    .fixedSize(horizontal: true, vertical: false)
                    .padding(.horizontal, 20).padding(.vertical, 11)
                    .background(picked == nil && !checked ? c.muted : c.act)
                    .cornerRadius(9)
                    .disabled(picked == nil && !checked)
                    if !checked {
                        Button("Skip") { next(); picked = nil; checked = false }
                            .font(.system(size: 16)).foregroundColor(c.muted)
                    }
                    Spacer()
                }.padding(.top, 8)
            }
            .padding(20)
            // the read/drills counter floats in a bottomBar over this
            // scroll view, so the last line needs room to clear it
            .padding(.bottom, 72)
        }
        .sheet(isPresented: $showWeak) {
            WeakSpotsSheet(deck: deck) { skill in
                aim = skill
                showWeak = false
            }
        }
        .background(c.paper.ignoresSafeArea())
    }

    private func colorFor(_ i: Int, _ c: Ink) -> Color {
        guard checked else { return c.ink }
        if i == drill.answer { return c.act }
        if i == picked { return c.error }
        return c.muted
    }
}

// Material in [square brackets] is supplied by the translator — a debate
// restatement, a gloss, a page number — and has no Tibetan in this segment.
// It is greyed rather than passed off as the parallel.
struct EnglishHint: View {
    let text: String
    let ink: Ink
    var body: some View {
        VStack(alignment: .leading, spacing: 5) {
            build().font(.system(size: 17, design: .serif)).italic()
            if text.contains("[") {
                Text("Greyed text in brackets is supplied by the translator — it has no Tibetan here.")
                    .font(.system(size: 11)).foregroundColor(ink.muted)
            }
        }
        .padding(11)
        .frame(maxWidth: .infinity, alignment: .leading)
        .background(ink.act.opacity(0.08))
    }
    private func build() -> Text {
        var out = Text("")
        var buf = ""
        var depth = 0
        func flush(_ supplied: Bool) {
            if buf.isEmpty { return }
            out = out + Text(buf).foregroundColor(supplied ? ink.muted : ink.ink)
            buf = ""
        }
        // The flush at an OPENING bracket must carry the depth it is
        // leaving, not `false`. With `false`, a nested bracket — "[a [b] c]",
        // which is how a debate restatement inside a gloss is written —
        // flushed "[a " as ordinary ink, and ordinary ink here means "this is
        // Geshe Michael Roach's own English". Supplied text was being passed
        // off as his. Latent rather than live when it was found: no segment
        // in the shipped pack nests, which is exactly why it survived.
        // The closing bracket only flushes when it returns to depth 0, so an
        // inner "]" no longer ends the greying early. This now matches
        // englishWithSupplied() on the desktop (app/main.cpp) character for
        // character.
        for ch in text {
            if ch == "[" {
                flush(depth > 0)
                depth += 1
                buf.append(ch)
            } else if ch == "]" {
                buf.append(ch)
                depth = max(0, depth - 1)
                if depth == 0 { flush(true) }
            } else {
                buf.append(ch)
            }
        }
        flush(depth > 0)
        return out
    }
}

// MARK: - the app

struct RootView: View {
    @StateObject private var deck = Deck()
    @State private var pack: Pack? = PackLoader.load()
    @State private var mode = 0
    @State private var dOrder: [Int] = []
    /// The skill the drill draw is aimed at, if any.
    @State private var aim: String? = nil
    /// Set when an aim found nothing, so the reason can be shown.
    @State private var aimEmpty: String? = nil
    @State private var kind: DrillKind = .cloze
    @State private var oAt = 0
    @State private var pAt = 0
    @State private var sAt = 0
    @State private var bAt = 0
    @State private var siAt = 0
    @State private var seAt = 0
    @State private var plAt = 0
    @State private var bdAt = 0
    @State private var roAt = 0
    @State private var vcAt = 0
    /// The mixed set's queue of kinds, and where we are in it. The picker
    /// keeps saying "Mixed set" — naming the kind is exactly the leak the
    /// mode exists to close — so only this queue knows what is being asked.
    @State private var mixQueue: [DrillKind] = []
    @State private var mixAt = 0

    /// The grammar kinds interleave; the vocabulary is blocked at the end.
    /// Interleaving is what makes the learner decide WHICH rule applies
    /// before applying it, which is the whole point — so no kind may be
    /// asked twice in a row. A shuffle with a repair pass does not guarantee
    /// that (the desktop's own gate caught it), so the queue is BUILT with
    /// the property: at each step take the kind with the most remaining,
    /// never the one just asked.
    static func buildMix(_ pack: Pack) -> [DrillKind] {
        var pool: [DrillKind: Int] = [:]
        let grammar: [(DrillKind, Bool)] = [
            (.cloze, !pack.cloze.isEmpty),
            (.order, !(pack.order ?? []).isEmpty),
            (.particle, !(pack.particle ?? []).isEmpty),
            (.boundary, !(pack.boundary ?? []).isEmpty),
            (.readorder, !(pack.readorder ?? []).isEmpty),
        ]
        for (k, have) in grammar where have { pool[k] = 3 }
        guard pool.count >= 2 else { return pool.keys.map { $0 } }
        var out: [DrillKind] = []
        var prev: DrillKind? = nil
        while true {
            let avail = pool.filter { $0.value > 0 && $0.key != prev }
            guard let top = avail.values.max() else { break }
            let tied = avail.filter { $0.value == top }.keys.sorted {
                $0.rawValue < $1.rawValue
            }
            let pick = tied[Int.random(in: 0..<tied.count)]
            out.append(pick)
            pool[pick]! -= 1
            prev = pick
        }
        // vocabulary blocked at the end: spacing helps rules, massing helps
        // word forms, and the two want opposite treatment
        if !(pack.vocab ?? []).isEmpty { out += [.vocab, .vocab, .vocab] }
        return out
    }

    @ViewBuilder private func mixedBody(_ pack: Pack, _ c: Ink) -> some View {
        let q = mixQueue
        if q.isEmpty {
            missing("drills to mix", c)
        } else {
            let k = q[min(mixAt, q.count - 1)]
            VStack(spacing: 0) {
                Text("MIXED SET — the category is not given away")
                    .font(.system(size: 10, weight: .semibold)).tracking(1.5)
                    .foregroundColor(c.gold)
                    .frame(maxWidth: .infinity, alignment: .leading)
                    .padding(.horizontal, 20).padding(.top, 6)
                mixedItem(k, pack, c) {
                    mixAt += 1
                    if mixAt >= q.count {
                        mixQueue = RootView.buildMix(pack)
                        mixAt = 0
                    }
                }
            }
        }
    }

    @ViewBuilder private func mixedItem(_ k: DrillKind, _ pack: Pack,
                                        _ c: Ink,
                                        _ advance: @escaping () -> Void)
        -> some View {
        switch k {
        case .order:
            if let a = pack.order, !a.isEmpty {
                OrderView(drill: a[oAt % a.count]) { oAt = (oAt + 1) % a.count; advance() }
            }
        case .particle:
            if let a = pack.particle, !a.isEmpty {
                ParticleView(drill: a[pAt % a.count], deck: deck) { pAt = (pAt + 1) % a.count; advance() }
            }
        case .boundary:
            if let a = pack.boundary, !a.isEmpty {
                BoundaryView(item: a[bdAt % a.count], deck: deck) { bdAt = (bdAt + 1) % a.count; advance() }
            }
        case .vocab:
            if let a = pack.vocab, !a.isEmpty {
                VocabView(item: a[vcAt % a.count], deck: deck) { vcAt = (vcAt + 1) % a.count; advance() }
            }
        case .readorder:
            if let a = pack.readorder, !a.isEmpty {
                ReadOrderView(item: a[roAt % a.count], deck: deck) { roAt = (roAt + 1) % a.count; advance() }
            }
        default:
            if !pack.cloze.isEmpty, dAt < dOrder.count {
                DrillView(drill: pack.cloze[dOrder[dAt]], deck: deck, aim: $aim) {
                    dAt = (dAt + 1) % max(dOrder.count, 1); advance()
                }
            }
        }
    }

    /// Shown when a pack predates a drill kind. Never a blank screen and
    /// never an invented drill — it says which pack is in the app.
    @ViewBuilder private func missing(_ what: String, _ c: Ink) -> some View {
        VStack(spacing: 8) {
            Text("This pack has no \(what).")
                .font(.system(size: 16, weight: .semibold))
            Text("The pack is built on the Mac; a newer one will carry them. Nothing is invented to fill the gap.")
                .font(.system(size: 13)).foregroundColor(c.muted)
                .multilineTextAlignment(.center)
        }.padding(30)
        Spacer()
    }

    /// Kept out of the view body: a concatenation this long inside an
    /// alert defeats Swift's type-checker.
    static func aimEmptyMessage(_ skill: String?) -> String {
        let s = skill ?? "that skill"
        return "This pack holds no drill exercising \(s), so the draw is "
            + "an ordinary one and is NOT counted as training it. The pack "
            + "is rebuilt on the Mac; a later one may hold some."
    }
    @State private var showProvenance = false
    @State private var tOrder: [Int] = []
    @State private var dAt = 0
    @State private var tAt = 0
    @Environment(\.colorScheme) private var scheme

    var body: some View {
        let c = Ink.of(scheme)
        NavigationStack {
            VStack(spacing: 0) {
                Picker("", selection: $mode) {
                    Text("Trainer").tag(0)
                    Text("Drills").tag(1)
                }
                .pickerStyle(.segmented)
                .padding(.horizontal, 16).padding(.vertical, 8)

                if let pack {
                    if mode == 0, !pack.trainer.isEmpty, tAt < tOrder.count {
                        TrainerView(passage: pack.trainer[tOrder[tAt]], deck: deck) {
                            tAt = (tAt + 1) % max(tOrder.count, 1)
                        }
                        // A new passage is a new question: carrying the last
                        // answer over would show "you read chunk 3 first"
                        // above a passage that was never asked about.
                        .id(tAt)
                    } else if mode == 1 {
                        // The kind picker. Until 2026-09-11 the phone offered
                        // only the cloze, while the desktop had eleven modes —
                        // Adam caught it. The phone generates nothing, so each
                        // kind here is one the Mac bakes into the pack.
                        Picker("", selection: $kind) {
                            ForEach(DrillKind.allCases) { k in
                                Text(k.rawValue).tag(k)
                            }
                        }
                        .pickerStyle(.menu)
                        // Every other piece of type in this app is set at a
                        // fixed point size and so ignores the system text
                        // setting. This picker is the exception: its collapsed
                        // label uses the scalable body font, so at a large
                        // accessibility size it alone balloons. With the old
                        // names — "Boundary hunt — where do the clauses end?"
                        // — the label ran to five lines and left the drill
                        // beneath it a two-line strip.
                        //
                        // Shortening the names is what fixed it; the worst
                        // case is now two lines. Both .font() and .lineLimit()
                        // were tried here first and NEITHER has any effect on
                        // a .menu picker's collapsed label, so they are not
                        // left sitting here looking like they work. Making the
                        // picker itself scale properly means replacing the
                        // menu style; that is on the backlog, with the rest of
                        // the app's fixed-size type.
                        .padding(.horizontal, 20)
                        switch kind {
                        case .cloze:
                            if !pack.cloze.isEmpty, dAt < dOrder.count {
                                DrillView(drill: pack.cloze[dOrder[dAt]],
                                          deck: deck, aim: $aim) {
                                    dAt = (dAt + 1) % max(dOrder.count, 1)
                                }
                            } else { missing("fill-the-blank drills", c) }
                        case .order:
                            if let a = pack.order, !a.isEmpty {
                                OrderView(drill: a[oAt % a.count]) {
                                    oAt = (oAt + 1) % a.count
                                }
                            } else { missing("chunk-order drills", c) }
                        case .particle:
                            if let a = pack.particle, !a.isEmpty {
                                ParticleView(drill: a[pAt % a.count], deck: deck) {
                                    pAt = (pAt + 1) % a.count
                                }
                            } else { missing("particle drills", c) }
                        case .script:
                            if let a = pack.script, !a.isEmpty {
                                ScriptView(card: a[sAt % a.count]) {
                                    sAt = (sAt + 1) % a.count
                                }
                            } else { missing("his own cards", c) }
                        case .debate:
                            if let a = pack.debate, !a.isEmpty {
                                DebateView(st: a[bAt % a.count], deck: deck) {
                                    bAt = (bAt + 1) % a.count
                                }
                            } else { missing("debate statements", c) }
                        case .silent:
                            if let a = pack.silent, !a.isEmpty {
                                SilentView(span: a[siAt % a.count]) {
                                    siAt = (siAt + 1) % a.count
                                }
                            } else { missing("aligned particles", c) }
                        case .second:
                            if let a = pack.second, !a.isEmpty {
                                SecondThoughtView(item: a[seAt % a.count]) {
                                    seAt = (seAt + 1) % a.count
                                }
                            } else { missing("multi-rendering terms", c) }
                        case .peel:
                            if let a = pack.peel, !a.isEmpty {
                                PeelView(span: a[plAt % a.count], deck: deck) {
                                    plAt = (plAt + 1) % a.count
                                }
                            } else { missing("nested spans", c) }
                        case .boundary:
                            if let a = pack.boundary, !a.isEmpty {
                                BoundaryView(item: a[bdAt % a.count], deck: deck) {
                                    bdAt = (bdAt + 1) % a.count
                                }
                            } else { missing("boundary-hunt segments", c) }
                        case .vocab:
                            if let a = pack.vocab, !a.isEmpty {
                                VocabView(item: a[vcAt % a.count], deck: deck) {
                                    vcAt = (vcAt + 1) % a.count
                                }
                            } else { missing("twice-attested words", c) }
                        case .readorder:
                            if let a = pack.readorder, !a.isEmpty {
                                ReadOrderView(item: a[roAt % a.count], deck: deck) {
                                    roAt = (roAt + 1) % a.count
                                }
                            } else { missing("aligned spans with his reading order", c) }
                        case .mixed:
                            mixedBody(pack, c)
                        }
                    } else {
                        // The Trainer is the tab the app opens on. With no
                        // trainer passages this fell through to a bare Spacer
                        // -- a blank screen, no message, nothing to act on,
                        // and no way to tell a broken build from an empty one.
                        // Every other empty pool in this app says what is
                        // missing; this one said nothing.
                        missing("reading passages", c)
                        Spacer()
                    }
                } else {
                    VStack(spacing: 10) {
                        Text("The pack is missing from this build.")
                            .font(.system(size: 17, weight: .semibold))
                        Text("Nothing is invented to fill the gap.")
                            .font(.system(size: 14)).foregroundColor(c.muted)
                    }.padding(30)
                    Spacer()
                }
            }
            .background(c.paper.ignoresSafeArea())
            .sheet(isPresented: $showProvenance) {
                ProvenanceSheet(meta: pack?.meta, ink: c)
            }
            .navigationTitle("Tibetan Translation Trainer")
            .navigationBarTitleDisplayMode(.inline)
            .toolbar {
                // In the bottom bar this floated OVER the action row and
                // crowded Skip. It belongs beside the title, where it is
                // always visible and never on top of a control.
                ToolbarItem(placement: .topBarLeading) {
                    Button { showProvenance = true } label: {
                        Image(systemName: "info.circle")
                    }
                    .accessibilityLabel("Where this pack came from")
                }
                ToolbarItem(placement: .topBarTrailing) {
                    Text("\(deck.done)/\(deck.right)")
                        .font(.system(size: 12)).foregroundColor(c.muted)
                        .accessibilityLabel(
                            "\(deck.done) drills done, \(deck.right) right, "
                            + "\(deck.read) passages read")
                }
            }
        }
        .onAppear {
            guard let p = pack else { return }
            if dOrder.isEmpty { dOrder = Array(0..<p.cloze.count).shuffled() }
            if tOrder.isEmpty { tOrder = Array(0..<p.trainer.count).shuffled() }
            // The mixed queue belongs here with the others. It used to be
            // built inside the view body whenever it was empty, and buildMix
            // is random, so EVERY re-render dealt a different queue: the kind
            // on screen changed under the learner's fingers, and pressing
            // Check could score an answer against a drill that had just been
            // swapped out. Rendering must not roll dice.
            if mixQueue.isEmpty { mixQueue = RootView.buildMix(p) }
        }
        // Aiming filters the pre-tagged pack rather than generating, because
        // the phone generates nothing. If the pack holds no drill for that
        // skill, the aim is dropped and said out loud — never silently
        // replaced by an ordinary draw that then counts as training it.
        .onChange(of: aim) { _, want in
            guard let p = pack else { return }
            guard let want, !want.isEmpty else {
                dOrder = Array(0..<p.cloze.count).shuffled(); dAt = 0; return
            }
            let hits = (0..<p.cloze.count).filter { p.cloze[$0].skill == want }
            if hits.isEmpty {
                aimEmpty = want
                aim = nil
            } else {
                dOrder = hits.shuffled(); dAt = 0
            }
        }
        .alert("No drill for that skill in this pack",
               isPresented: Binding(get: { aimEmpty != nil },
                                    set: { if !$0 { aimEmpty = nil } })) {
            Button("OK", role: .cancel) { aimEmpty = nil }
        } message: {
            Text(Self.aimEmptyMessage(aimEmpty))
        }
    }
}

@main
struct TibetanTranslationTrainerApp: App {
    var body: some Scene { WindowGroup { RootView() } }
}
