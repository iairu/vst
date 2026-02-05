import SwiftUI
import AIVFramework

struct InputModuleView: View {
    @ObservedObject var viewModel: AudioUnitViewModel
    
    var body: some View {
        VStack(spacing: 24) {
            // Preamp
            EffectGroup(title: "PREAMP") {
                HStack(spacing: 30) {
                    ArcKnob(value: $viewModel.inputGain, range: -100...24, title: "INPUT", unit: "dB")
                    ArcKnob(value: $viewModel.saturation, range: 0...100, title: "SATURATE", unit: "%")
                    VStack {
                        Text("PHASE").font(.caption).foregroundColor(.gray)
                        Toggle("", isOn: Binding(get: { viewModel.phaseInvert > 0.5 }, set: { viewModel.phaseInvert = $0 ? 1 : 0 }))
                            .toggleStyle(SwitchToggleStyle(tint: .red))
                            .labelsHidden()
                        Text(viewModel.phaseInvert > 0.5 ? "INV" : "NARM")
                            .font(.system(size: 10, weight: .bold, design: .monospaced))
                            .foregroundColor(viewModel.phaseInvert > 0.5 ? .red : .gray)
                    }
                }
            }
            
            // Pitch
            EffectGroup(title: "PITCH") {
                HStack(spacing: 30) {
                    ArcKnob(value: $viewModel.pitchAmount, range: 0...100, title: "AMOUNT", unit: "%")
                    ArcKnob(value: $viewModel.pitchSpeed, range: 0...100, title: "SPEED", unit: "%")
                }
            }
            
            // Filter
            EffectGroup(title: "FILTER") {
                HStack(spacing: 30) {
                    ArcKnob(value: $viewModel.cutoff, range: 20...20000, title: "CUTOFF", unit: "Hz")
                    ArcKnob(value: $viewModel.resonance, range: -20...20, title: "RES", unit: "dB")
                }
            }
            
            // Saturation (Dedicated)
            EffectGroup(title: "SATURATION") {
                HStack(spacing: 30) {
                    ArcKnob(value: $viewModel.satDrive, range: 0...100, title: "DRIVE", unit: "%")
                    ArcKnob(value: $viewModel.satType, range: 0...1, title: "TYPE")
                }
            }
        }
        .padding()
    }
}
