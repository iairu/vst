import SwiftUI
import AIVFramework

struct EQModuleView: View {
    @ObservedObject var viewModel: AudioUnitViewModel
    
    var body: some View {
        VStack(spacing: 24) {
            
            EffectGroup(title: "EQUALIZER") {
                HStack(spacing: 30) {
                    VStack {
                        Text("HPF").font(.caption).foregroundColor(.gray)
                        ArcKnob(value: $viewModel.eq1Freq, range: 20...500, title: "FREQ", unit: "Hz")
                        // Gain removed for HPF
                    }
                    VStack {
                        Text("MID").font(.caption).foregroundColor(.gray)
                        ArcKnob(value: $viewModel.eq2Freq, range: 200...5000, title: "FREQ", unit: "Hz")
                        ArcKnob(value: $viewModel.eq2Gain, range: -20...20, title: "GAIN", unit: "dB")
                        ArcKnob(value: $viewModel.eq2Q, range: 0.1...10, title: "Q")
                    }
                    VStack {
                        Text("HIGH").font(.caption).foregroundColor(.gray)
                        ArcKnob(value: $viewModel.eq3Freq, range: 2000...20000, title: "FREQ", unit: "Hz")
                        ArcKnob(value: $viewModel.eq3Gain, range: -20...20, title: "GAIN", unit: "dB")
                    }
                }
            }
            
            // Saturation (Moved here or kept in Input? Prompt History structure implies its own thing or bundled. Let's put Saturation in Input or create a generic output stage.
            // Original monolithic view had Saturation near Delay/Reverb.
            // Let's stick it in Input based on Prompt 1.1 "Preamp & Saturation", but wait, prompt 3.2 mentions Saturation oversampling.
            // The user request says "utilize extension tabs for different modules".
            // I'll leave Saturation in Input (Preamp) as implemented in InputModuleView, but also checking the original view it was separate.
            // Actually, `InputModuleView` already has Preamp Input & Saturation. so we are good.
        }
        .padding()
    }
}
