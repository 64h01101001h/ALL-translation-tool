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

struct PackMeta: Codable { let built_by: String; let source: String; let tier: String }
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
///   Translate & compare — needs the whole 105,634-entry dictionary run
///                         against a draft as you type it. Not bakeable.
enum DrillKind: String, CaseIterable, Identifiable {
    case cloze = "Fill the blank"
    case order = "Chunk order"
    case particle = "Particle choice"
    case script = "Script — his own cards"
    case debate = "Debate — what does the reply attack?"
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

// MARK: - Trainer

struct TrainerView: View {
    let passage: Passage
    @ObservedObject var deck: Deck
    let next: () -> Void
    @Environment(\.colorScheme) private var scheme
    @State private var shown: Set<Int> = []

    private let layers = ["1 · chunks", "2 · particle roles", "3 · reading order",
                          "4 · the verb", "5 · answer key", "6 · full parse"]

    var body: some View {
        let c = Ink.of(scheme)
        ScrollView {
            VStack(alignment: .leading, spacing: 16) {
                Text("Read it yourself first, then reveal one layer at a time.")
                    .font(.system(size: 15)).foregroundColor(c.muted)

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
                if shown.contains(2) {
                    Layer("Reading order", ink: c) {
                        ForEach(passage.plan.sorted { $0.order < $1.order }, id: \.chunk) { st in
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
        for ch in text {
            if ch == "[" { flush(false); depth += 1; buf.append(ch) }
            else if ch == "]" { buf.append(ch); flush(true); depth = max(0, depth - 1) }
            else { buf.append(ch) }
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
                        }
                    } else { Spacer() }
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
            .navigationTitle("Tibetan Translation Trainer")
            .navigationBarTitleDisplayMode(.inline)
            .toolbar {
                // In the bottom bar this floated OVER the action row and
                // crowded Skip. It belongs beside the title, where it is
                // always visible and never on top of a control.
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
