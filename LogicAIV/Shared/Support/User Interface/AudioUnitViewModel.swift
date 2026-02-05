import SwiftUI
import CoreAudioKit
import AIVFramework

class AudioUnitViewModel: ObservableObject {
    // Global
    @Published var gain: Double = 0.5 { didSet { setParam(gainParam, gain) } }
    @Published var bypass: Double = 0.0 { didSet { setParam(bypassParam, bypass) } }
    
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
    @Published var deesserRange: Double = -6 { didSet { setParam(deesserRangeParam, deesserRange) } }
    
    // Gate
    @Published var gateThresh: Double = -40 { didSet { setParam(gateThreshParam, gateThresh) } }
    @Published var gateRange: Double = -20 { didSet { setParam(gateRangeParam, gateRange) } }
    @Published var gateAttack: Double = 1 { didSet { setParam(gateAttackParam, gateAttack) } }
    @Published var gateHold: Double = 150 { didSet { setParam(gateHoldParam, gateHold) } }
    @Published var gateRelease: Double = 300 { didSet { setParam(gateReleaseParam, gateRelease) } }
    @Published var gateHysteresis: Double = 6 { didSet { setParam(gateHysteresisParam, gateHysteresis) } }
    
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
    @Published var compInput: Double = -18 { didSet { setParam(compInputParam, compInput) } } // Was compThresh
    @Published var compRatio: Double = 4 { didSet { setParam(compRatioParam, compRatio) } }
    @Published var compAttack: Double = 0.4 { didSet { setParam(compAttackParam, compAttack) } }
    @Published var compRelease: Double = 100 { didSet { setParam(compReleaseParam, compRelease) } }
    @Published var compMakeup: Double = 0 { didSet { setParam(compMakeupParam, compMakeup) } }
    @Published var compAutoMakeup: Bool = false { didSet { setParam(compAutoMakeupParam, compAutoMakeup ? 1.0 : 0.0) } }

    // Limiter
    @Published var limiterCeiling: Double = -0.1 { didSet { setParam(limiterCeilingParam, limiterCeiling) } }
    @Published var limiterLookahead: Double = 2.0 { didSet { setParam(limiterLookaheadParam, limiterLookahead) } }
    
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
    
    // Filter
    @Published var cutoff: Double = 20000 { didSet { setParam(cutoffParam, cutoff) } }
    @Published var resonance: Double = 0 { didSet { setParam(resonanceParam, resonance) } }

    // Preamp (Missing from previous view model dump but likely needed)
    @Published var inputGain: Double = 0.0 { didSet { setParam(inputGainParam, inputGain) } }
    @Published var saturation: Double = 0.0 { didSet { setParam(saturationParam, saturation) } }
    @Published var phaseInvert: Double = 0.0 { didSet { setParam(phaseInvertParam, phaseInvert) } }

    private var gainParam: AUParameter?
    private var bypassParam: AUParameter?
    private var cutoffParam: AUParameter?
    private var resonanceParam: AUParameter?
    private var autoLevelTargetParam: AUParameter?
    private var autoLevelRangeParam: AUParameter?
    private var autoLevelSpeedParam: AUParameter?
    private var pitchAmountParam: AUParameter?
    private var pitchSpeedParam: AUParameter?
    private var deesserThreshParam: AUParameter?
    private var deesserFreqParam: AUParameter?
    private var deesserRatioParam: AUParameter?
    private var deesserRangeParam: AUParameter?

    private var gateThreshParam: AUParameter?
    private var gateRangeParam: AUParameter?
    private var gateAttackParam: AUParameter?
    private var gateHoldParam: AUParameter?
    private var gateReleaseParam: AUParameter?
    private var gateHysteresisParam: AUParameter?
    private var eq1FreqParam: AUParameter?
    private var eq1GainParam: AUParameter?
    private var eq1QParam: AUParameter?
    private var eq2FreqParam: AUParameter?
    private var eq2GainParam: AUParameter?
    private var eq2QParam: AUParameter?
    private var eq3FreqParam: AUParameter?
    private var eq3GainParam: AUParameter?
    private var eq3QParam: AUParameter?
    private var compInputParam: AUParameter? 
    private var compRatioParam: AUParameter?
    private var compAttackParam: AUParameter?
    private var compReleaseParam: AUParameter?
    private var compMakeupParam: AUParameter?
    private var compAutoMakeupParam: AUParameter?
    private var limiterCeilingParam: AUParameter?
    private var limiterLookaheadParam: AUParameter?
    private var satDriveParam: AUParameter?
    private var satTypeParam: AUParameter?
    private var delayTimeParam: AUParameter?
    private var delayFeedbackParam: AUParameter?
    private var delayMixParam: AUParameter?
    private var reverbSizeParam: AUParameter?
    private var reverbDampParam: AUParameter?
    private var reverbMixParam: AUParameter?
    private var inputGainParam: AUParameter?
    private var saturationParam: AUParameter?
    private var phaseInvertParam: AUParameter?
    
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
        cutoffParam = bind("cutoff"); cutoff = Double(cutoffParam?.value ?? 20000)
        resonanceParam = bind("resonance"); resonance = Double(resonanceParam?.value ?? 0)
        
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
        deesserRangeParam = bind("deesserRange"); deesserRange = Double(deesserRangeParam?.value ?? -6)

        gateThreshParam = bind("gateThresh"); gateThresh = Double(gateThreshParam?.value ?? -40)
        gateRangeParam = bind("gateRange"); gateRange = Double(gateRangeParam?.value ?? -20)
        gateAttackParam = bind("gateAttack"); gateAttack = Double(gateAttackParam?.value ?? 1)
        gateHoldParam = bind("gateHold"); gateHold = Double(gateHoldParam?.value ?? 150)
        gateReleaseParam = bind("gateRelease"); gateRelease = Double(gateReleaseParam?.value ?? 300)
        gateHysteresisParam = bind("gateHysteresis"); gateHysteresis = Double(gateHysteresisParam?.value ?? 6)
        
        eq1FreqParam = bind("eq1Freq"); eq1Freq = Double(eq1FreqParam?.value ?? 100)
        eq1GainParam = bind("eq1Gain"); eq1Gain = Double(eq1GainParam?.value ?? 0)
        eq1QParam = bind("eq1Q"); eq1Q = Double(eq1QParam?.value ?? 0.7)
        
        eq2FreqParam = bind("eq2Freq"); eq2Freq = Double(eq2FreqParam?.value ?? 1000)
        eq2GainParam = bind("eq2Gain"); eq2Gain = Double(eq2GainParam?.value ?? 0)
        eq2QParam = bind("eq2Q"); eq2Q = Double(eq2QParam?.value ?? 0.7)
        
        eq3FreqParam = bind("eq3Freq"); eq3Freq = Double(eq3FreqParam?.value ?? 5000)
        eq3GainParam = bind("eq3Gain"); eq3Gain = Double(eq3GainParam?.value ?? 0)
        eq3QParam = bind("eq3Q"); eq3Q = Double(eq3QParam?.value ?? 0.7)
        
        compInputParam = bind("compInput"); compInput = Double(compInputParam?.value ?? -18)
        compRatioParam = bind("compRatio"); compRatio = Double(compRatioParam?.value ?? 4)
        compAttackParam = bind("compAttack"); compAttack = Double(compAttackParam?.value ?? 0.4)
        compReleaseParam = bind("compRelease"); compRelease = Double(compReleaseParam?.value ?? 100)
        compMakeupParam = bind("compMakeup"); compMakeup = Double(compMakeupParam?.value ?? 0)
        compAutoMakeupParam = bind("compAutoMakeup"); compAutoMakeup = (Double(compAutoMakeupParam?.value ?? 0) > 0.5)
        
        limiterCeilingParam = bind("limiterCeiling"); limiterCeiling = Double(limiterCeilingParam?.value ?? -0.1)
        limiterLookaheadParam = bind("limiterLookahead"); limiterLookahead = Double(limiterLookaheadParam?.value ?? 2.0)
        
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
        else if address == cutoffParam?.address { cutoff = Double(value) }
        else if address == resonanceParam?.address { resonance = Double(value) }
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
        else if address == deesserRangeParam?.address { deesserRange = Double(value) }
        // Gate
        else if address == gateThreshParam?.address { gateThresh = Double(value) }
        else if address == gateRangeParam?.address { gateRange = Double(value) }
        else if address == gateAttackParam?.address { gateAttack = Double(value) }
        else if address == gateHoldParam?.address { gateHold = Double(value) }
        else if address == gateReleaseParam?.address { gateRelease = Double(value) }
        else if address == gateHysteresisParam?.address { gateHysteresis = Double(value) }
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
        else if address == compInputParam?.address { compInput = Double(value) }
        else if address == compRatioParam?.address { compRatio = Double(value) }
        else if address == compAttackParam?.address { compAttack = Double(value) }
        else if address == compReleaseParam?.address { compRelease = Double(value) }
        else if address == compMakeupParam?.address { compMakeup = Double(value) }
        else if address == compAutoMakeupParam?.address { compAutoMakeup = (value > 0.5) }
        else if address == limiterCeilingParam?.address { limiterCeiling = Double(value) }
        else if address == limiterLookaheadParam?.address { limiterLookahead = Double(value) }
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
