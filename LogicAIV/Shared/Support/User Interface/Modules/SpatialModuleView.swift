import SwiftUI
import AIVFramework

struct SpatialModuleView: View {
    @ObservedObject var viewModel: AudioUnitViewModel
    
    var body: some View {
        VStack(spacing: 24) {
            
            // Delay
            EffectGroup(title: "DELAY") {
                VStack {
                    ArcKnob(value: $viewModel.delayMix, range: 0...100, title: "MIX", unit: "%")
                    HStack {
                        ArcKnob(value: $viewModel.delayTime, range: 0...2, title: "TIME", unit: "s")
                        ArcKnob(value: $viewModel.delayFeedback, range: 0...100, title: "FDBK", unit: "%")
                    }
                }
            }
            
            // Reverb
            EffectGroup(title: "REVERB") {
                VStack {
                    ArcKnob(value: $viewModel.reverbMix, range: 0...100, title: "MIX", unit: "%")
                    HStack {
                        ArcKnob(value: $viewModel.reverbSize, range: 0...100, title: "SIZE", unit: "%")
                        ArcKnob(value: $viewModel.reverbDamp, range: 0...100, title: "DAMP", unit: "%")
                    }
                }
            }
            
            // Saturation Type? (The original view had Saturation Drive and Type together. I put Saturation Drive in Input.
            // Let's verify where Saturation Type is.
            // In AIVMainView: EffectGroup(title: "SATURATION") { Drive, Type }
            // In InputModuleView I included Saturation Drive but maybe missed Type?
            // Let's add Saturation Type to InputModuleView in next step if missed, or here.
            // Preamp fits Input.
        }
        .padding()
    }
}
