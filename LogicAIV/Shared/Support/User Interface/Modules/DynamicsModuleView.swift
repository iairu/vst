import SwiftUI
import AIVFramework

struct DynamicsModuleView: View {
    @ObservedObject var viewModel: AudioUnitViewModel
    
    var body: some View {
        VStack(spacing: 24) {
            
            // Noise Gate (New)
            EffectGroup(title: "INTELLIGENT GATE") {
                HStack(spacing: 20) {
                    ArcKnob(value: $viewModel.gateThresh, range: -80...0, title: "THRESH", suffix: "dB")
                    ArcKnob(value: $viewModel.gateRange, range: -80...0, title: "RANGE", suffix: "dB")
                    ArcKnob(value: $viewModel.gateHysteresis, range: 0...12, title: "HYST", suffix: "dB")
                    
                    VStack(spacing: 5) {
                        ModernSlider(value: $viewModel.gateAttack, range: 0.01...100, title: "ATTACK")
                        ModernSlider(value: $viewModel.gateHold, range: 0...1000, title: "HOLD")
                        ModernSlider(value: $viewModel.gateRelease, range: 10...2000, title: "RELEASE")
                    }
                    .frame(width: 120)
                }
            }
            
            // Auto Level
            EffectGroup(title: "AUTO LEVEL") {
                VStack {
                    ArcKnob(value: $viewModel.autoLevelTarget, range: -60...0, title: "TARGET", unit: "dB")
                    HStack {
                        ArcKnob(value: $viewModel.autoLevelRange, range: 0...40, title: "RANGE", unit: "dB")
                        ArcKnob(value: $viewModel.autoLevelSpeed, range: 0...100, title: "SPEED", unit: "%")
                    }
                }
            }
            
            // De-Esser
            EffectGroup(title: "DE-ESSER") {
                HStack(spacing: 20) {
                    ArcKnob(value: $viewModel.deesserThresh, range: -60...0, title: "THRESH", suffix: "dB")
                    ArcKnob(value: $viewModel.deesserFreq, range: 2000...12000, title: "FREQ", suffix: "Hz")
                    ArcKnob(value: $viewModel.deesserRange, range: -24...0, title: "RANGE", suffix: "dB")
                    ArcKnob(value: $viewModel.deesserRatio, range: 1...20, title: "RATIO")
                }
            }
            
            // Compressor
            EffectGroup(title: "COMPRESSOR (FET)") {
                VStack {
                    HStack {
                        ArcKnob(value: $viewModel.compInput, range: -48...12, title: "INPUT", unit: "dB")
                        ArcKnob(value: $viewModel.compRatio, range: 1...20, title: "RATIO")
                    }
                    HStack {
                        ArcKnob(value: $viewModel.compAttack, range: 0.02...0.8, title: "ATTACK", unit: "ms")
                        ArcKnob(value: $viewModel.compRelease, range: 50...1100, title: "RELEASE", unit: "ms")
                        ArcKnob(value: $viewModel.compMakeup, range: 0...24, title: "MAKEUP", unit: "dB")
                    }
                }
            }
        }
        .padding()
    }
}
