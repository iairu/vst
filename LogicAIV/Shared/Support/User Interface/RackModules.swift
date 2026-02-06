import SwiftUI
import AIVFramework

// --- 1. Input Panel ---
struct InputPanel: View {
    @ObservedObject var viewModel: AudioUnitViewModel
    
    var body: some View {
        RackPanel(title: "Input Stage", isEnabled: $viewModel.satEnable) {
            HStack(spacing: 20) {
                ArcKnob(value: $viewModel.inputGain, range: -24...24, title: "Input", unit: "dB")
                
                VStack(spacing: 5) {
                    Text("Phase")
                        .font(.caption)
                        .foregroundColor(.gray)
                    
                    SwiftUI.Button(action: { viewModel.phaseInvert = viewModel.phaseInvert > 0.5 ? 0 : 1 }) {
                        ZStack {
                            Circle()
                                .fill(viewModel.phaseInvert > 0.5 ? Color.orange.opacity(0.8) : Color.white.opacity(0.1))
                                .frame(width: 25, height: 25)
                            
                            Image(systemName: "phase.invert")
                                .font(.system(size: 12, weight: .bold))
                                .foregroundColor(viewModel.phaseInvert > 0.5 ? .white : .gray)
                        }
                    }
                    .buttonStyle(PlainButtonStyle())
                }
                
                ArcKnob(value: $viewModel.saturation, range: 0...100, title: "Drive", unit: "%")
                
                VStack {
                    Text("Mode")
                        .font(.caption)
                        .foregroundColor(.gray)
                    Picker("Type", selection: Binding(get: { Int(viewModel.satType) }, set: { viewModel.satType = Double($0) })) {
                        Text("Tape").tag(0)
                        Text("Tube").tag(1)
                    }
                    .pickerStyle(SegmentedPickerStyle())
                    .frame(width: 80)
                }
            }
        }
    }
}

// --- 2. Gate Panel ---
struct GatePanel: View {
    @ObservedObject var viewModel: AudioUnitViewModel
    
    var body: some View {
        RackPanel(title: "Noise Gate", isEnabled: $viewModel.gateEnable) {
            HStack(spacing: 15) {
                ArcKnob(value: $viewModel.gateThresh, range: -80...0, title: "Thresh", unit: "dB")
                ArcKnob(value: $viewModel.gateRange, range: -80...0, title: "Range", unit: "dB")
                ArcKnob(value: $viewModel.gateAttack, range: 1...100, title: "Attack", unit: "ms")
                ArcKnob(value: $viewModel.gateHold, range: 0...500, title: "Hold", unit: "ms")
                ArcKnob(value: $viewModel.gateRelease, range: 10...1000, title: "Release", unit: "ms")
            }
        }
    }
}

// --- 3. Pitch & Deesser Panel ---
struct PitchDeesserPanel: View {
    @ObservedObject var viewModel: AudioUnitViewModel
    
    var body: some View {
        HStack(alignment: .top, spacing: 10) {
            // Pitch
            RackPanel(title: "Pitch", isEnabled: $viewModel.pitchEnable) {
                HStack(spacing: 15) {
                    ArcKnob(value: $viewModel.pitchAmount, range: 0...100, title: "Amount", unit: "%")
                    ArcKnob(value: $viewModel.pitchSpeed, range: 0...100, title: "Speed", unit: "%")
                }
            }
            
            // De-Esser
            RackPanel(title: "De-Esser", isEnabled: $viewModel.deesserEnable) {
                HStack(spacing: 15) {
                    ArcKnob(value: $viewModel.deesserThresh, range: -60...0, title: "Thresh", unit: "dB")
                    ArcKnob(value: $viewModel.deesserFreq, range: 2000...10000, title: "Freq", unit: "Hz")
                    ArcKnob(value: $viewModel.deesserRange, range: -24...0, title: "Range", unit: "dB")
                }
            }
        }
    }
}

// --- 4. EQ Panel ---
struct EQPanel: View {
    @ObservedObject var viewModel: AudioUnitViewModel
    
    var body: some View {
        RackPanel(title: "Equalizer", isEnabled: $viewModel.eqEnable) {
            HStack(spacing: 0) {
                // Low Band
                VStack(spacing: 5) {
                    Text("LOW").font(.system(size: 9, weight: .bold)).foregroundColor(.blue)
                    HStack(spacing: 5) {
                        ArcKnob(value: $viewModel.eq1Freq, range: 20...500, title: "Freq", unit: "Hz", size: 50, trackWidth: 3)
                        ArcKnob(value: $viewModel.eq1Gain, range: -12...12, title: "Gain", unit: "dB", size: 50, trackWidth: 3)
                    }
                }
                .padding(.horizontal, 5)
                
                Divider().background(Color.white.opacity(0.1))
                
                // Mid Band
                VStack(spacing: 5) {
                    Text("MID").font(.system(size: 9, weight: .bold)).foregroundColor(.green)
                    HStack(spacing: 5) {
                        ArcKnob(value: $viewModel.eq2Freq, range: 200...5000, title: "Freq", unit: "Hz", size: 50, trackWidth: 3)
                        ArcKnob(value: $viewModel.eq2Gain, range: -12...12, title: "Gain", unit: "dB", size: 50, trackWidth: 3)
                        ArcKnob(value: $viewModel.eq2Q, range: 0.1...10.0, title: "Q", unit: "", size: 50, trackWidth: 3)
                    }
                }
                .padding(.horizontal, 5)

                Divider().background(Color.white.opacity(0.1))
                
                // High Band
                VStack(spacing: 5) {
                    Text("HIGH").font(.system(size: 9, weight: .bold)).foregroundColor(.orange)
                    HStack(spacing: 5) {
                        ArcKnob(value: $viewModel.eq3Freq, range: 2000...20000, title: "Freq", unit: "Hz", size: 50, trackWidth: 3)
                        ArcKnob(value: $viewModel.eq3Gain, range: -12...12, title: "Gain", unit: "dB", size: 50, trackWidth: 3)
                    }
                }
                .padding(.horizontal, 5)
            }
        }
    }
}

// --- 5. Dynamics Panel ---
struct DynamicsPanel: View {
    @ObservedObject var viewModel: AudioUnitViewModel
    
    var body: some View {
        HStack(alignment: .top, spacing: 10) {
            RackPanel(title: "Compressor (FET)", isEnabled: $viewModel.compEnable) {
                VStack {
                    HStack(spacing: 15) {
                        ArcKnob(value: $viewModel.compInput, range: -48...12, title: "Input", unit: "dB")
                        ArcKnob(value: $viewModel.compRatio, range: 1...20, title: "Ratio", unit: ":1")
                        ArcKnob(value: $viewModel.compMakeup, range: 0...24, title: "Makeup", unit: "dB")
                    }
                    HStack(spacing: 15) {
                        ArcKnob(value: $viewModel.compAttack, range: 0.1...100.0, title: "Attack", unit: "ms")
                        ArcKnob(value: $viewModel.compRelease, range: 50...1000, title: "Release", unit: "ms")
                        
                        Toggle("Auto Mkup", isOn: $viewModel.compAutoMakeup)
                            .toggleStyle(SwitchToggleStyle(tint: .cyan))
                            .labelsHidden()
                            .frame(width: 50)
                    }
                }
            }
            
            RackPanel(title: "Limiter", isEnabled: $viewModel.limiterEnable) {
                HStack(spacing: 15) {
                    ArcKnob(value: $viewModel.limiterCeiling, range: -6...0, title: "Ceiling", unit: "dB")
                    ArcKnob(value: $viewModel.limiterLookahead, range: 0...5, title: "Lookahead", unit: "ms")
                }
            }
        }
    }
}

// --- 6. Spatial Panel ---
struct SpatialPanel: View {
    @ObservedObject var viewModel: AudioUnitViewModel
    
    var body: some View {
        HStack(alignment: .top, spacing: 10) {
            // Delay
            RackPanel(title: "Delay", isEnabled: $viewModel.delayEnable) {
                HStack(spacing: 10) {
                    ArcKnob(value: $viewModel.delayTime, range: 0...2.0, title: "Time", unit: "s")
                    ArcKnob(value: $viewModel.delayFeedback, range: 0...100, title: "Fdbk", unit: "%")
                    ArcKnob(value: $viewModel.delayMix, range: 0...100, title: "Mix", unit: "%")
                }
            }
            
            // Reverb
            RackPanel(title: "Reverb", isEnabled: $viewModel.reverbEnable) {
                HStack(spacing: 10) {
                    ArcKnob(value: $viewModel.reverbSize, range: 0...100, title: "Size", unit: "%")
                    ArcKnob(value: $viewModel.reverbDamp, range: 0...100, title: "Damp", unit: "%")
                    ArcKnob(value: $viewModel.reverbMix, range: 0...100, title: "Mix", unit: "%")
                }
            }
        }
    }
}

// --- 7. Output Panel ---
struct OutputPanel: View {
    @ObservedObject var viewModel: AudioUnitViewModel
    
    var body: some View {
         RackPanel(title: "Master") {
                HStack(spacing: 30) {
                    ArcKnob(value: $viewModel.gain, range: 0...1, title: "Output", unit: "", size: 80, trackWidth: 6)
                    
                    VStack {
                        Text("BYPASS")
                            .font(.system(size: 10, weight: .bold))
                            .foregroundColor(.gray)
                        Toggle("", isOn: Binding(get: { viewModel.bypass > 0.5 }, set: { viewModel.bypass = $0 ? 1 : 0 }))
                            .toggleStyle(SwitchToggleStyle(tint: .red))
                            .labelsHidden()
                    }
                    
                    ArcKnob(value: $viewModel.resonance, range: -20...20, title: "Resonance", unit: "dB", size: 60)
                }
         }
    }
}
