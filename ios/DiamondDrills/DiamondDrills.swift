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
    let course: String
    let seq: Int
    let title: Bool
    let answer: Int
    let options: [String]
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
struct Pack: Codable { let meta: PackMeta; let cloze: [Drill]; let trainer: [Passage] }

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
    func recordRead() { read += 1; d.set(read, forKey: "trainer.read") }
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
    let next: () -> Void
    @Environment(\.colorScheme) private var scheme
    @State private var picked: Int? = nil
    @State private var checked = false
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
                                .foregroundColor(colorFor(i, c))
                                .multilineTextAlignment(.leading)
                            Spacer()
                        }.padding(.vertical, 6)
                    }.buttonStyle(.plain)
                }

                if checked {
                    VStack(alignment: .leading, spacing: 5) {
                        Text(isRight ? "Correct." : "Not yet — the answer is in green.")
                            .font(.system(size: 17, weight: .semibold))
                            .foregroundColor(isRight ? c.act : c.machine)
                        if !drill.role.isEmpty {
                            Text("role of the blanked chunk: \(drill.role)")
                                .font(.system(size: 14)).foregroundColor(c.muted)
                        }
                        Text("[\(drill.course):\(drill.seq)]")
                            .font(.system(size: 12)).foregroundColor(c.muted)
                    }
                }

                HStack(spacing: 14) {
                    Button(checked ? "New drill" : "Check") {
                        if checked { next(); picked = nil; checked = false }
                        else if picked != nil { checked = true; deck.record(correct: isRight) }
                    }
                    .font(.system(size: 17, weight: .semibold)).foregroundColor(.white)
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
                    } else if mode == 1, !pack.cloze.isEmpty, dAt < dOrder.count {
                        DrillView(drill: pack.cloze[dOrder[dAt]], deck: deck) {
                            dAt = (dAt + 1) % max(dOrder.count, 1)
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
                ToolbarItem(placement: .bottomBar) {
                    Text("\(deck.read) read · \(deck.done) drills, \(deck.right) right")
                        .font(.system(size: 12)).foregroundColor(c.muted)
                }
            }
        }
        .onAppear {
            guard let p = pack else { return }
            if dOrder.isEmpty { dOrder = Array(0..<p.cloze.count).shuffled() }
            if tOrder.isEmpty { tOrder = Array(0..<p.trainer.count).shuffled() }
        }
    }
}

@main
struct TibetanTranslationTrainerApp: App {
    var body: some Scene { WindowGroup { RootView() } }
}
