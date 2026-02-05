import SwiftUI
import CoreAudioKit
import AIVFramework

class AudioUnitViewModel: ObservableObject {
    // Global
    @Published var gain: Double = 0.5 { didSet { setParam(gainParam, gain) } }
    @Published var bypass: Double = 0.0 { didSet { setParam(bypassParam, bypass) } }
    
    // Preamp
    @Published var inputGain: Double = 0.0 { didSet { setParam(inputGainParam, inputGain) } }
    @Published var saturation: Double = 0.0 { didSet { setParam(saturationParam, saturation) } }
    @Published var phaseInvert: Double = 0.0 { didSet { setParam(phaseInvertParam, phaseInvert) } }
    
    // Auto Level
    @Published var autoLevelTarget: Double = -10 { didSet { setParam(autoLevelTargetParam, autoLevelTarget) } }
    @Published var autoLevelRange: Double = 12 { didSet { setParam(autoLevelRangeParam, autoLevelRange) } }
    @Published var autoLevelSpeed: Double = 50 { didSet { setParam(autoLevelSpeedParam, autoLevelSpeed) } }
    
    // Pitch
    @Published var pitchAmount: Double = 50 { didSet { setParam(pitchAmountParam, pitchAmount) } }
    @Published var pitchSpeed: Double = 20 { didSet { setParam(pitchSpeedParam, pitchSpeed) } }
    
    // Deesser
    @Published var deesserThresh: Double = -20 { didSet { setParam(deesserThreshParam, deesserThresh) } }
    @Published var deesserFreq: Double = 5000 { didSet { setParam(deesserFreqParam, deesserFreq) } }
    @Published var deesserRatio: Double = 5 { didSet { setParam(deesserRatioParam, deesserRatio) } }
    
    // EQ
    @Published var eq1Freq: Double = 100 { didSet { setParam(eq1FreqParam, eq1Freq) } }
    @Published var eq1Gain: Double = 0 { didSet { setParam(eq1GainParam, eq1Gain) } }
    @Published var eq1Q: Double = 0.7 { didSet { setParam(eq1QParam, eq1Q) } }
    
    @Published var eq2Freq: Double = 1000 { didSet { setParam(eq2FreqParam, eq2Freq) } }
    @Published var eq2Gain: Double = 0 { didSet { setParam(eq2GainParam, eq2Gain) } }
    @Published var eq2Q: Double = 0.7 { didSet { setParam(eq2QParam, eq2Q) } }
    
    @Published var eq3Freq: Double = 5000 { didSet { setParam(eq3FreqParam, eq3Freq) } }
    @Published var eq3Gain: Double = 0 { didSet { setParam(eq3GainParam, eq3Gain) } }
    @Published var eq3Q: Double = 0.7 { didSet { setParam(eq3QParam, eq3Q) } }
    
    // Compressor
    @Published var compThresh: Double = 0 { didSet { setParam(compThreshParam, compThresh) } }
    @Published var compRatio: Double = 1 { didSet { setParam(compRatioParam, compRatio) } }
    @Published var compAttack: Double = 10 { didSet { setParam(compAttackParam, compAttack) } }
    @Published var compRelease: Double = 100 { didSet { setParam(compReleaseParam, compRelease) } }
    @Published var compMakeup: Double = 0 { didSet { setParam(compMakeupParam, compMakeup) } }
    
    // Saturation
    @Published var satDrive: Double = 0 { didSet { setParam(satDriveParam, satDrive) } }
    @Published var satType: Double = 0 { didSet { setParam(satTypeParam, satType) } }
    
    // Delay
    @Published var delayTime: Double = 0.5 { didSet { setParam(delayTimeParam, delayTime) } }
    @Published var delayFeedback: Double = 0 { didSet { setParam(delayFeedbackParam, delayFeedback) } }
    @Published var delayMix: Double = 0 { didSet { setParam(delayMixParam, delayMix) } }
    
    // Reverb
    @Published var reverbSize: Double = 50 { didSet { setParam(reverbSizeParam, reverbSize) } }
    @Published var reverbDamp: Double = 50 { didSet { setParam(reverbDampParam, reverbDamp) } }
    @Published var reverbMix: Double = 0 { didSet { setParam(reverbMixParam, reverbMix) } }
    
    private var gainParam: AUParameter?
    private var bypassParam: AUParameter?
    private var inputGainParam: AUParameter?
    private var saturationParam: AUParameter?
    private var phaseInvertParam: AUParameter?
    private var autoLevelTargetParam: AUParameter?
    private var autoLevelRangeParam: AUParameter?
    private var autoLevelSpeedParam: AUParameter?
    private var pitchAmountParam: AUParameter?
    private var pitchSpeedParam: AUParameter?
    private var deesserThreshParam: AUParameter?
    private var deesserFreqParam: AUParameter?
    private var deesserRatioParam: AUParameter?
    private var eq1FreqParam: AUParameter?
    private var eq1GainParam: AUParameter?
    private var eq1QParam: AUParameter?
    private var eq2FreqParam: AUParameter?
    private var eq2GainParam: AUParameter?
    private var eq2QParam: AUParameter?
    private var eq3FreqParam: AUParameter?
    private var eq3GainParam: AUParameter?
    private var eq3QParam: AUParameter?
    private var compThreshParam: AUParameter?
    private var compRatioParam: AUParameter?
    private var compAttackParam: AUParameter?
    private var compReleaseParam: AUParameter?
    private var compMakeupParam: AUParameter?
    private var satDriveParam: AUParameter?
    private var satTypeParam: AUParameter?
    private var delayTimeParam: AUParameter?
    private var delayFeedbackParam: AUParameter?
    private var delayMixParam: AUParameter?
    private var reverbSizeParam: AUParameter?
    private var reverbDampParam: AUParameter?
    private var reverbMixParam: AUParameter?
    
    private var observerToken: AUParameterObserverToken?
    private var paramTree: AUParameterTree?
    
    func connect(audioUnit: AIVDemo) {
        guard let tree = audioUnit.parameterTree else { return }
        self.paramTree = tree
        
        // Helper to get param and set initial value
        func bind(_ key: String) -> AUParameter? {
            guard let param = tree.value(forKey: key) as? AUParameter else { return nil }
            return param
        }
        
        gainParam = bind("gain"); gain = Double(gainParam?.value ?? 0.5)
        bypassParam = bind("bypass"); bypass = Double(bypassParam?.value ?? 0)
        
        inputGainParam = bind("inputGain"); inputGain = Double(inputGainParam?.value ?? 0.0)
        saturationParam = bind("saturation"); saturation = Double(saturationParam?.value ?? 0.0)
        phaseInvertParam = bind("phaseInvert"); phaseInvert = Double(phaseInvertParam?.value ?? 0.0)
        
        autoLevelTargetParam = bind("autoLevelTarget"); autoLevelTarget = Double(autoLevelTargetParam?.value ?? -10)
        autoLevelRangeParam = bind("autoLevelRange"); autoLevelRange = Double(autoLevelRangeParam?.value ?? 12)
        autoLevelSpeedParam = bind("autoLevelSpeed"); autoLevelSpeed = Double(autoLevelSpeedParam?.value ?? 50)
        
        pitchAmountParam = bind("pitchAmount"); pitchAmount = Double(pitchAmountParam?.value ?? 50)
        pitchSpeedParam = bind("pitchSpeed"); pitchSpeed = Double(pitchSpeedParam?.value ?? 20)
        
        deesserThreshParam = bind("deesserThresh"); deesserThresh = Double(deesserThreshParam?.value ?? -20)
        deesserFreqParam = bind("deesserFreq"); deesserFreq = Double(deesserFreqParam?.value ?? 5000)
        deesserRatioParam = bind("deesserRatio"); deesserRatio = Double(deesserRatioParam?.value ?? 5)
        
        eq1FreqParam = bind("eq1Freq"); eq1Freq = Double(eq1FreqParam?.value ?? 100)
        eq1GainParam = bind("eq1Gain"); eq1Gain = Double(eq1GainParam?.value ?? 0)
        eq1QParam = bind("eq1Q"); eq1Q = Double(eq1QParam?.value ?? 0.7)
        
        eq2FreqParam = bind("eq2Freq"); eq2Freq = Double(eq2FreqParam?.value ?? 1000)
        eq2GainParam = bind("eq2Gain"); eq2Gain = Double(eq2GainParam?.value ?? 0)
        eq2QParam = bind("eq2Q"); eq2Q = Double(eq2QParam?.value ?? 0.7)
        
        eq3FreqParam = bind("eq3Freq"); eq3Freq = Double(eq3FreqParam?.value ?? 5000)
        eq3GainParam = bind("eq3Gain"); eq3Gain = Double(eq3GainParam?.value ?? 0)
        eq3QParam = bind("eq3Q"); eq3Q = Double(eq3QParam?.value ?? 0.7)
        
        compThreshParam = bind("compThresh"); compThresh = Double(compThreshParam?.value ?? 0)
        compRatioParam = bind("compRatio"); compRatio = Double(compRatioParam?.value ?? 1)
        compAttackParam = bind("compAttack"); compAttack = Double(compAttackParam?.value ?? 10)
        compReleaseParam = bind("compRelease"); compRelease = Double(compReleaseParam?.value ?? 100)
        compMakeupParam = bind("compMakeup"); compMakeup = Double(compMakeupParam?.value ?? 0)
        
        satDriveParam = bind("satDrive"); satDrive = Double(satDriveParam?.value ?? 0)
        satTypeParam = bind("satType"); satType = Double(satTypeParam?.value ?? 0)
        
        delayTimeParam = bind("delayTime"); delayTime = Double(delayTimeParam?.value ?? 0.5)
        delayFeedbackParam = bind("delayFeedback"); delayFeedback = Double(delayFeedbackParam?.value ?? 0)
        delayMixParam = bind("delayMix"); delayMix = Double(delayMixParam?.value ?? 0)
        
        reverbSizeParam = bind("reverbSize"); reverbSize = Double(reverbSizeParam?.value ?? 50)
        reverbDampParam = bind("reverbDamp"); reverbDamp = Double(reverbDampParam?.value ?? 50)
        reverbMixParam = bind("reverbMix"); reverbMix = Double(reverbMixParam?.value ?? 0)
        
        observerToken = tree.token(byAddingParameterObserver: { [weak self] address, value in
            DispatchQueue.main.async {
                self?.updateFromObserver(address: address, value: value)
            }
        })
    }
    
    private func setParam(_ param: AUParameter?, _ value: Double) {
        // Prevent feedback loop if value matches
        guard let p = param, abs(Double(p.value) - value) > 0.001 else { return }
        p.value = AUValue(value)
    }
    
    private func updateFromObserver(address: AUParameterAddress, value: AUValue) {
        // Map address back to property. This is tedious, so we check address.
        if address == gainParam?.address { gain = Double(value) }
        else if address == bypassParam?.address { bypass = Double(value) }
        // Preamp
        else if address == inputGainParam?.address { inputGain = Double(value) }
        else if address == saturationParam?.address { saturation = Double(value) }
        else if address == phaseInvertParam?.address { phaseInvert = Double(value) }
        // AutoLevel
        else if address == autoLevelTargetParam?.address { autoLevelTarget = Double(value) }
        else if address == autoLevelRangeParam?.address { autoLevelRange = Double(value) }
        else if address == autoLevelSpeedParam?.address { autoLevelSpeed = Double(value) }
        // Pitch
        else if address == pitchAmountParam?.address { pitchAmount = Double(value) }
        else if address == pitchSpeedParam?.address { pitchSpeed = Double(value) }
        // Deesser
        else if address == deesserThreshParam?.address { deesserThresh = Double(value) }
        else if address == deesserFreqParam?.address { deesserFreq = Double(value) }
        else if address == deesserRatioParam?.address { deesserRatio = Double(value) }
        // EQ
        else if address == eq1FreqParam?.address { eq1Freq = Double(value) }
        else if address == eq1GainParam?.address { eq1Gain = Double(value) }
        else if address == eq1QParam?.address { eq1Q = Double(value) }
        else if address == eq2FreqParam?.address { eq2Freq = Double(value) }
        else if address == eq2GainParam?.address { eq2Gain = Double(value) }
        else if address == eq2QParam?.address { eq2Q = Double(value) }
        else if address == eq3FreqParam?.address { eq3Freq = Double(value) }
        else if address == eq3GainParam?.address { eq3Gain = Double(value) }
        else if address == eq3QParam?.address { eq3Q = Double(value) }
        // Comp
        else if address == compThreshParam?.address { compThresh = Double(value) }
        else if address == compRatioParam?.address { compRatio = Double(value) }
        else if address == compAttackParam?.address { compAttack = Double(value) }
        else if address == compReleaseParam?.address { compRelease = Double(value) }
        else if address == compMakeupParam?.address { compMakeup = Double(value) }
        // Sat
        else if address == satDriveParam?.address { satDrive = Double(value) }
        else if address == satTypeParam?.address { satType = Double(value) }
        // Delay
        else if address == delayTimeParam?.address { delayTime = Double(value) }
        else if address == delayFeedbackParam?.address { delayFeedback = Double(value) }
        else if address == delayMixParam?.address { delayMix = Double(value) }
        // Reverb
        else if address == reverbSizeParam?.address { reverbSize = Double(value) }
        else if address == reverbDampParam?.address { reverbDamp = Double(value) }
        else if address == reverbMixParam?.address { reverbMix = Double(value) }
    }
}

struct AIVMainView: View {
    @StateObject var viewModel = AudioUnitViewModel()
    var audioUnit: AIVDemo?
    
    var body: some View {
        ZStack {
            Color(red: 0.12, green: 0.12, blue: 0.14).edgesIgnoringSafeArea(.all)
            
            VStack(spacing: 0) {
                // Header
                HStack {
                    Text("AIV VOCAL CHAIN")
                        .font(.system(size: 18, weight: .bold, design: .serif))
                        .foregroundColor(.white)
                        .tracking(2)
                    
                    Spacer()
                    
                    Toggle("BYPASS", isOn: Binding(get: { viewModel.bypass > 0.5 }, set: { viewModel.bypass = $0 ? 1 : 0 }))
                        .toggleStyle(SwitchToggleStyle(tint: .cyan))
                }
                .padding()
                .background(Color(red: 0.15, green: 0.15, blue: 0.17))
                
                ScrollView {
                    VStack(spacing: 24) {
                        
                        // Row 0: Preamp
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
                        
                        // Row 1: Auto Level & Pitch
                        HStack(alignment: .top, spacing: 20) {
                            EffectGroup(title: "AUTO LEVEL") {
                                VStack {
                                    ArcKnob(value: $viewModel.autoLevelTarget, range: -60...0, title: "TARGET", unit: "dB")
                                    HStack {
                                        ArcKnob(value: $viewModel.autoLevelRange, range: 0...40, title: "RANGE", unit: "dB")
                                        ArcKnob(value: $viewModel.autoLevelSpeed, range: 0...100, title: "SPEED", unit: "%")
                                    }
                                }
                            }
                            
                            EffectGroup(title: "PITCH") {
                                HStack {
                                    ArcKnob(value: $viewModel.pitchAmount, range: 0...100, title: "AMOUNT", unit: "%")
                                    ArcKnob(value: $viewModel.pitchSpeed, range: 0...100, title: "SPEED", unit: "%")
                                }
                            }
                        }
                        
                        // Row 2: Deesser & Compressor
                        HStack(alignment: .top, spacing: 20) {
                            EffectGroup(title: "DE-ESSER") {
                                VStack {
                                    ArcKnob(value: $viewModel.deesserThresh, range: -60...0, title: "THRESH", unit: "dB")
                                    HStack {
                                        ArcKnob(value: $viewModel.deesserFreq, range: 2000...10000, title: "FREQ", unit: "Hz")
                                        ArcKnob(value: $viewModel.deesserRatio, range: 1...20, title: "RATIO")
                                    }
                                }
                            }
                            
                            EffectGroup(title: "COMPRESSOR") {
                                VStack {
                                    HStack {
                                        ArcKnob(value: $viewModel.compThresh, range: -60...(-0.1), title: "THRESH", unit: "dB")
                                        ArcKnob(value: $viewModel.compRatio, range: 1...20, title: "RATIO")
                                    }
                                    HStack {
                                        ArcKnob(value: $viewModel.compAttack, range: 0.1...100, title: "ATTACK", unit: "ms")
                                        ArcKnob(value: $viewModel.compRelease, range: 10...1000, title: "RELEASE", unit: "ms")
                                        ArcKnob(value: $viewModel.compMakeup, range: 0...24, title: "MAKEUP", unit: "dB")
                                    }
                                }
                            }
                        }
                        
                        // Row 3: EQ
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
                        
                        // Row 4: Saturation, Delay, Reverb
                        HStack(alignment: .top, spacing: 20) {
                            EffectGroup(title: "SATURATION") {
                                VStack {
                                    ArcKnob(value: $viewModel.satDrive, range: 0...100, title: "DRIVE", unit: "%")
                                    // Make satType a picker or toggle if only 2 types
                                    // For now knob 0..1
                                    ArcKnob(value: $viewModel.satType, range: 0...1, title: "TYPE")
                                }
                            }
                            
                            EffectGroup(title: "DELAY") {
                                VStack {
                                    ArcKnob(value: $viewModel.delayMix, range: 0...100, title: "MIX", unit: "%")
                                    HStack {
                                        ArcKnob(value: $viewModel.delayTime, range: 0...2, title: "TIME", unit: "s")
                                        ArcKnob(value: $viewModel.delayFeedback, range: 0...100, title: "FDBK", unit: "%")
                                    }
                                }
                            }
                            
                            EffectGroup(title: "REVERB") {
                                VStack {
                                    ArcKnob(value: $viewModel.reverbMix, range: 0...100, title: "MIX", unit: "%")
                                    HStack {
                                        ArcKnob(value: $viewModel.reverbSize, range: 0...100, title: "SIZE", unit: "%")
                                        ArcKnob(value: $viewModel.reverbDamp, range: 0...100, title: "DAMP", unit: "%")
                                    }
                                }
                            }
                        }
                        
                        // Divider
                        Rectangle().fill(Color.gray.opacity(0.3)).frame(height: 1)
                        
                        // Global Output
                        HStack {
                            Text("OUTPUT").font(.caption).foregroundColor(.gray)
                            ModernSlider(value: $viewModel.gain, range: 0...1, title: "MASTER GAIN")
                        }
                        .padding(.horizontal)
                        .padding(.bottom, 20)
                    }
                    .padding()
                }
            }
        }
        .onAppear {
            if let au = audioUnit {
                viewModel.connect(audioUnit: au)
            }
        }
    }
}

struct EffectGroup<Content: View>: View {
    var title: String
    var content: Content
    
    init(title: String, @ViewBuilder content: () -> Content) {
        self.title = title
        self.content = content()
    }
    
    var body: some View {
        VStack(alignment: .leading, spacing: 10) {
            Text(title)
                .font(.system(size: 10, weight: .bold))
                .foregroundColor(.cyan)
                .tracking(1)
            
            VStack {
                content
            }
            .padding()
            .background(Color.black.opacity(0.3))
            .cornerRadius(8)
            .overlay(
                RoundedRectangle(cornerRadius: 8)
                    .stroke(Color.white.opacity(0.1), lineWidth: 1)
            )
        }
    }
}
