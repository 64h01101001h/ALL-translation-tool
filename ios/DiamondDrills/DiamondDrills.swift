// Diamond Cutter Drills — the Learn tab's drills, standing alone on iPhone.
//
// Path A (Adam, 2026-09-09): the phone generates nothing. Every drill in
// drills.json was built on the Mac by the same allcore the desktop uses, so
// this app inherits the desktop's fixes and its refusals — the whole segment
// shown against the English rather than a quarter of it, the title catalogue
// kept out of the draw, and editorial markup never passed off as Tibetan.
//
// The honesty rules travel with it. A smaller screen is not a more forgiving
// one: every English line here is Geshe Michael Roach's own text, and anything
// the engine could not read arrives already flagged in ⟨ ⟩.
import SwiftUI

// MARK: - the pack

struct Drill: Codable, Identifiable {
    let before: String
    let after: String
    let english: String
    let role: String
    let course: String
    let seq: Int
    let title: Bool
    let answer: Int
    let options: [String]
    var id: String { "\(course):\(seq):\(answer):\(options.first ?? "")" }
}

struct PackMeta: Codable {
    let built_by: String
    let source: String
    let tier: String
}

struct Pack: Codable {
    let meta: PackMeta
    let cloze: [Drill]
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

// MARK: - the manuscript palette, carried over

enum Ink {
    static let paper = Color(red: 0.980, green: 0.965, blue: 0.933)
    static let ink = Color(red: 0.169, green: 0.130, blue: 0.094)
    static let gold = Color(red: 0.510, green: 0.404, blue: 0.165)
    static let muted = Color(red: 0.431, green: 0.404, blue: 0.365)
    static let act = Color(red: 0.118, green: 0.420, blue: 0.306)
    static let machine = Color(red: 0.663, green: 0.302, blue: 0.043)
    static let error = Color(red: 0.549, green: 0.184, blue: 0.169)
}

// MARK: - progress, local to the phone

final class Deck: ObservableObject {
    @Published var done = 0
    @Published var right = 0
    private let d = UserDefaults.standard
    init() {
        done = d.integer(forKey: "drills.done")
        right = d.integer(forKey: "drills.right")
    }
    func record(correct: Bool) {
        done += 1
        if correct { right += 1 }
        d.set(done, forKey: "drills.done")
        d.set(right, forKey: "drills.right")
    }
}

// MARK: - the drill

struct DrillView: View {
    let drill: Drill
    @ObservedObject var deck: Deck
    let next: () -> Void

    @State private var picked: Int? = nil
    @State private var checked = false

    private var isRight: Bool { picked == drill.answer }

    var body: some View {
        ScrollView {
            VStack(alignment: .leading, spacing: 14) {
                if drill.title {
                    HStack(spacing: 6) {
                        Text("TITLE LINE")
                            .font(.system(size: 10, weight: .semibold))
                            .tracking(1)
                            .foregroundColor(Ink.gold)
                            .padding(.horizontal, 6).padding(.vertical, 2)
                            .background(Ink.gold.opacity(0.12))
                        Text("a work's title, not running prose")
                            .font(.system(size: 11)).foregroundColor(Ink.muted)
                    }
                }

                Text("Which chunk fills the blank?")
                    .font(.system(size: 13)).foregroundColor(Ink.muted)

                Text(drill.english)
                    .font(.system(size: 15, design: .serif))
                    .italic()
                    .foregroundColor(Ink.ink)
                    .padding(10)
                    .frame(maxWidth: .infinity, alignment: .leading)
                    .background(Ink.act.opacity(0.07))

                // the WHOLE segment, with the answer blanked where it stands
                (Text(drill.before)
                    + Text("  [ … ]  ").foregroundColor(Ink.machine).bold()
                    + Text(drill.after))
                    .font(.system(size: 21))
                    .foregroundColor(Ink.ink)
                    .frame(maxWidth: .infinity, alignment: .leading)

                Divider()

                ForEach(Array(drill.options.enumerated()), id: \.offset) { i, opt in
                    Button {
                        if !checked { picked = i }
                    } label: {
                        HStack(alignment: .top, spacing: 10) {
                            Image(systemName: picked == i ? "largecircle.fill.circle" : "circle")
                                .foregroundColor(picked == i ? Ink.act : Ink.muted)
                            Text(opt)
                                .font(.system(size: 19))
                                .foregroundColor(optionColor(i))
                                .multilineTextAlignment(.leading)
                            Spacer()
                        }
                        .padding(.vertical, 5)
                    }
                    .buttonStyle(.plain)
                }

                if checked {
                    VStack(alignment: .leading, spacing: 5) {
                        Text(isRight ? "Correct." : "Not yet — the answer is highlighted.")
                            .font(.system(size: 15, weight: .semibold))
                            .foregroundColor(isRight ? Ink.act : Ink.machine)
                        if !drill.role.isEmpty {
                            Text("role of the blanked chunk: \(drill.role)")
                                .font(.system(size: 12)).foregroundColor(Ink.muted)
                        }
                        Text("[\(drill.course):\(drill.seq)]")
                            .font(.system(size: 11)).foregroundColor(Ink.muted)
                    }
                    .padding(.top, 4)
                }

                HStack(spacing: 12) {
                    Button(checked ? "New drill" : "Check") {
                        if checked { next(); picked = nil; checked = false }
                        else if picked != nil { checked = true; deck.record(correct: isRight) }
                    }
                    .font(.system(size: 16, weight: .semibold))
                    .foregroundColor(.white)
                    .padding(.horizontal, 18).padding(.vertical, 10)
                    .background(picked == nil && !checked ? Ink.muted : Ink.act)
                    .cornerRadius(8)
                    .disabled(picked == nil && !checked)

                    if !checked {
                        Button("Skip") { next(); picked = nil; checked = false }
                            .font(.system(size: 15)).foregroundColor(Ink.muted)
                    }
                    Spacer()
                }
                .padding(.top, 8)
            }
            .padding(18)
        }
        .background(Ink.paper.ignoresSafeArea())
    }

    private func optionColor(_ i: Int) -> Color {
        guard checked else { return Ink.ink }
        if i == drill.answer { return Ink.act }
        if i == picked { return Ink.error }
        return Ink.muted
    }
}

// MARK: - the app

struct RootView: View {
    @StateObject private var deck = Deck()
    @State private var pack: Pack? = PackLoader.load()
    @State private var order: [Int] = []
    @State private var at = 0

    var body: some View {
        NavigationStack {
            Group {
                if let pack, !pack.cloze.isEmpty, at < order.count {
                    DrillView(drill: pack.cloze[order[at]], deck: deck) {
                        at = (at + 1) % max(order.count, 1)
                    }
                } else if pack == nil {
                    VStack(spacing: 10) {
                        Text("The drill pack is missing from this build.")
                            .font(.system(size: 16, weight: .semibold))
                        Text("drills.json was not found in the bundle. Nothing is invented to fill the gap.")
                            .font(.system(size: 13)).foregroundColor(Ink.muted)
                            .multilineTextAlignment(.center)
                    }
                    .padding(30)
                } else {
                    ProgressView()
                }
            }
            .navigationTitle("Drills")
            .navigationBarTitleDisplayMode(.inline)
            .toolbar {
                ToolbarItem(placement: .bottomBar) {
                    Text("\(deck.done) done · \(deck.right) right" +
                         (pack.map { " · \($0.cloze.count) in the pack" } ?? ""))
                        .font(.system(size: 12)).foregroundColor(Ink.muted)
                }
            }
        }
        .onAppear {
            if order.isEmpty, let p = pack {
                order = Array(0..<p.cloze.count).shuffled()
            }
        }
    }
}

@main
struct DiamondDrillsApp: App {
    var body: some Scene {
        WindowGroup { RootView() }
    }
}
