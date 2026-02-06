/*
See LICENSE folder for this sample’s licensing information.

Abstract:
The object managing the plugin parameters.
*/

import Foundation
import AIVFramework

/// Manages the AIV object's parameters.
class AIVDemoParameters {

    private enum AIVParam: AUParameterAddress {
        case gain = 0
        case bypass = 1
        case inputGain = 34
        case saturation = 35
        case phaseInvert = 36
        case pitchAmount = 2
        case pitchSpeed = 3
        case eqBand1Freq = 4
        case eqBand1Gain = 5
        case eqBand1Q = 6
        case eqBand2Freq = 7
        case eqBand2Gain = 8
        case eqBand2Q = 9
        case eqBand3Freq = 10
        case eqBand3Gain = 11
        case eqBand3Q = 12

        case compInput = 13 // Was compThresh
        case compRatio = 14
        case compAttack = 15
        case compRelease = 16
        case compMakeup = 17
        case satDrive = 18
        case satType = 19
        case delayTime = 20
        case delayFeedback = 21
        case delayMix = 22
        case reverbSize = 23
        case reverbDamp = 24
        case reverbMix = 25
        case autoLevelTarget = 26
        case autoLevelRange = 27
        case autoLevelSpeed = 28
        case deesserThresh = 29
        case deesserFreq = 30
        case deesserRatio = 31
        case deesserRange = 32
        
        case gateThresh = 40
        case gateRange = 41
        case gateAttack = 42
        case gateHold = 43
        case gateRelease = 44
        case gateHysteresis = 45

        case cutoff = 50
        case resonance = 51
        
        case limiterCeiling = 60
        case limiterLookahead = 61
        case compAutoMakeup = 62
        
        // Enables
        case gateEnable = 70
        case deesserEnable = 71
        case eqEnable = 72
        case compEnable = 73
        case satEnable = 74
        case delayEnable = 75
        case reverbEnable = 76
        case pitchEnable = 77
        case limiterEnable = 78
    }

    // Parameters
    var gainParam: AUParameter!
    var bypassParam: AUParameter!
    
    // Preamp
    var inputGainParam: AUParameter!
    var saturationParam: AUParameter!
    var phaseInvertParam: AUParameter!
    
    var pitchAmountParam: AUParameter!
    var pitchSpeedParam: AUParameter!
    
    // EQ
    var eqBand1FreqParam: AUParameter!
    var eqBand1GainParam: AUParameter!
    var eqBand1QParam: AUParameter!
    
    var eqBand2FreqParam: AUParameter!
    var eqBand2GainParam: AUParameter!
    var eqBand2QParam: AUParameter!
    
    var eqBand3FreqParam: AUParameter!
    var eqBand3GainParam: AUParameter!
    var eqBand3QParam: AUParameter!
    
    // Compressor
    var compInputParam: AUParameter! // Was compThreshParam
    var compRatioParam: AUParameter!
    var compAttackParam: AUParameter!
    var compReleaseParam: AUParameter!
    var compMakeupParam: AUParameter!
    var compAutoMakeupParam: AUParameter!
    
    // Limiter
    var limiterCeilingParam: AUParameter!
    var limiterLookaheadParam: AUParameter!
    

    
    // Saturation
    var satDriveParam: AUParameter!
    var satTypeParam: AUParameter!
    
    // Delay
    var delayTimeParam: AUParameter!
    var delayFeedbackParam: AUParameter!
    var delayMixParam: AUParameter!
    
    // Reverb
    var reverbSizeParam: AUParameter!
    var reverbDampParam: AUParameter!
    var reverbMixParam: AUParameter!

    // Auto Level
    var autoLevelTargetParam: AUParameter!
    var autoLevelRangeParam: AUParameter!
    var autoLevelSpeedParam: AUParameter!

    // Deesser
    var deesserThreshParam: AUParameter!
    var deesserFreqParam: AUParameter!
    var deesserRatioParam: AUParameter!
    var deesserRangeParam: AUParameter!
    
    // Gate
    var gateThreshParam: AUParameter!
    var gateRangeParam: AUParameter!
    var gateAttackParam: AUParameter!
    var gateHoldParam: AUParameter!
    var gateReleaseParam: AUParameter!
    var gateHysteresisParam: AUParameter!

    // Filter (Legacy)
    var cutoffParam: AUParameter!
    var resonanceParam: AUParameter!
    
    // Enables
    var gateEnableParam: AUParameter!
    var deesserEnableParam: AUParameter!
    var eqEnableParam: AUParameter!
    var compEnableParam: AUParameter!
    var satEnableParam: AUParameter!
    var delayEnableParam: AUParameter!
    var reverbEnableParam: AUParameter!
    var pitchEnableParam: AUParameter!
    var limiterEnableParam: AUParameter!

    let parameterTree: AUParameterTree
    let kernelAdapter: AIVDSPKernelAdapter

    init(kernelAdapter: AIVDSPKernelAdapter) {
        self.kernelAdapter = kernelAdapter

        // 1. Create Parameters
        
        // Global
        gainParam = AUParameterTree.createParameter(withIdentifier: "gain", name: "Gain", address: AIVParam.gain.rawValue, min: 0.0, max: 1.0, unit: .linearGain, unitName: nil, flags: [.flag_IsReadable, .flag_IsWritable], valueStrings: nil, dependentParameters: nil)
        gainParam.value = 1.0 // Unity Gain by default
        
        bypassParam = AUParameterTree.createParameter(withIdentifier: "bypass", name: "Bypass", address: AIVParam.bypass.rawValue, min: 0.0, max: 1.0, unit: .boolean, unitName: nil, flags: [.flag_IsReadable, .flag_IsWritable], valueStrings: nil, dependentParameters: nil)
        bypassParam.value = 0.0
        
        // Preamp
        inputGainParam = AUParameterTree.createParameter(withIdentifier: "inputGain", name: "Input Gain", address: AIVParam.inputGain.rawValue, min: -100.0, max: 24.0, unit: .decibels, unitName: nil, flags: [.flag_IsReadable, .flag_IsWritable], valueStrings: nil, dependentParameters: nil)
        inputGainParam.value = 0.0
        
        saturationParam = AUParameterTree.createParameter(withIdentifier: "saturation", name: "Saturation", address: AIVParam.saturation.rawValue, min: 0.0, max: 100.0, unit: .percent, unitName: nil, flags: [.flag_IsReadable, .flag_IsWritable], valueStrings: nil, dependentParameters: nil)
        saturationParam.value = 0.0
        
        phaseInvertParam = AUParameterTree.createParameter(withIdentifier: "phaseInvert", name: "Phase Invert", address: AIVParam.phaseInvert.rawValue, min: 0.0, max: 1.0, unit: .boolean, unitName: nil, flags: [.flag_IsReadable, .flag_IsWritable], valueStrings: nil, dependentParameters: nil)
        phaseInvertParam.value = 0.0
        
        // Pitch
        pitchAmountParam = AUParameterTree.createParameter(withIdentifier: "pitchAmount", name: "Pitch Amount", address: AIVParam.pitchAmount.rawValue, min: 0.0, max: 100.0, unit: .percent, unitName: nil, flags: [.flag_IsReadable, .flag_IsWritable], valueStrings: nil, dependentParameters: nil)
        pitchAmountParam.value = 50.0 // No shift
        
        pitchSpeedParam = AUParameterTree.createParameter(withIdentifier: "pitchSpeed", name: "Pitch Speed", address: AIVParam.pitchSpeed.rawValue, min: 0.0, max: 100.0, unit: .percent, unitName: nil, flags: [.flag_IsReadable, .flag_IsWritable], valueStrings: nil, dependentParameters: nil)
        
        // EQ
        eqBand1FreqParam = AUParameterTree.createParameter(withIdentifier: "eq1Freq", name: "Low Freq", address: AIVParam.eqBand1Freq.rawValue, min: 20.0, max: 20000.0, unit: .hertz, unitName: nil, flags: [.flag_IsReadable, .flag_IsWritable], valueStrings: nil, dependentParameters: nil)
        eqBand1FreqParam.value = 100.0
        
        eqBand1GainParam = AUParameterTree.createParameter(withIdentifier: "eq1Gain", name: "Low Gain", address: AIVParam.eqBand1Gain.rawValue, min: -20.0, max: 20.0, unit: .decibels, unitName: nil, flags: [.flag_IsReadable, .flag_IsWritable], valueStrings: nil, dependentParameters: nil)
        
        eqBand1QParam = AUParameterTree.createParameter(withIdentifier: "eq1Q", name: "Low Q", address: AIVParam.eqBand1Q.rawValue, min: 0.1, max: 10.0, unit: .generic, unitName: nil, flags: [.flag_IsReadable, .flag_IsWritable], valueStrings: nil, dependentParameters: nil)
        eqBand1QParam.value = 0.707
        
        eqBand2FreqParam = AUParameterTree.createParameter(withIdentifier: "eq2Freq", name: "Mid Freq", address: AIVParam.eqBand2Freq.rawValue, min: 20.0, max: 20000.0, unit: .hertz, unitName: nil, flags: [.flag_IsReadable, .flag_IsWritable], valueStrings: nil, dependentParameters: nil)
        eqBand2FreqParam.value = 1000.0
        
        eqBand2GainParam = AUParameterTree.createParameter(withIdentifier: "eq2Gain", name: "Mid Gain", address: AIVParam.eqBand2Gain.rawValue, min: -20.0, max: 20.0, unit: .decibels, unitName: nil, flags: [.flag_IsReadable, .flag_IsWritable], valueStrings: nil, dependentParameters: nil)
        
        eqBand2QParam = AUParameterTree.createParameter(withIdentifier: "eq2Q", name: "Mid Q", address: AIVParam.eqBand2Q.rawValue, min: 0.1, max: 10.0, unit: .generic, unitName: nil, flags: [.flag_IsReadable, .flag_IsWritable], valueStrings: nil, dependentParameters: nil)
        eqBand2QParam.value = 0.707
        
        eqBand3FreqParam = AUParameterTree.createParameter(withIdentifier: "eq3Freq", name: "High Freq", address: AIVParam.eqBand3Freq.rawValue, min: 20.0, max: 20000.0, unit: .hertz, unitName: nil, flags: [.flag_IsReadable, .flag_IsWritable], valueStrings: nil, dependentParameters: nil)
        eqBand3FreqParam.value = 5000.0
        
        eqBand3GainParam = AUParameterTree.createParameter(withIdentifier: "eq3Gain", name: "High Gain", address: AIVParam.eqBand3Gain.rawValue, min: -20.0, max: 20.0, unit: .decibels, unitName: nil, flags: [.flag_IsReadable, .flag_IsWritable], valueStrings: nil, dependentParameters: nil)
        
        eqBand3QParam = AUParameterTree.createParameter(withIdentifier: "eq3Q", name: "High Q", address: AIVParam.eqBand3Q.rawValue, min: 0.1, max: 10.0, unit: .generic, unitName: nil, flags: [.flag_IsReadable, .flag_IsWritable], valueStrings: nil, dependentParameters: nil)
        eqBand3QParam.value = 0.707
        
        // Compressor
        // Input Drive (FET Style): -48 to +12 dB
        compInputParam = AUParameterTree.createParameter(withIdentifier: "compInput", name: "Input", address: AIVParam.compInput.rawValue, min: -48.0, max: 12.0, unit: .decibels, unitName: nil, flags: [.flag_IsReadable, .flag_IsWritable], valueStrings: nil, dependentParameters: nil)
        compInputParam.value = 0.0 // Default Unity (was -18.0 which attenuated signal)
        
        compRatioParam = AUParameterTree.createParameter(withIdentifier: "compRatio", name: "Ratio", address: AIVParam.compRatio.rawValue, min: 1.0, max: 20.0, unit: .ratio, unitName: nil, flags: [.flag_IsReadable, .flag_IsWritable], valueStrings: nil, dependentParameters: nil)
        compRatioParam.value = 4.0 // Default 4:1
        
        // Attack: 20us to 800us (0.02ms to 0.8ms)
        compAttackParam = AUParameterTree.createParameter(withIdentifier: "compAttack", name: "Attack", address: AIVParam.compAttack.rawValue, min: 0.1, max: 100.0, unit: .milliseconds, unitName: nil, flags: [.flag_IsReadable, .flag_IsWritable], valueStrings: nil, dependentParameters: nil)
        compAttackParam.value = 10.0
        
        // Release: 50ms to 1100ms
        compReleaseParam = AUParameterTree.createParameter(withIdentifier: "compRelease", name: "Release", address: AIVParam.compRelease.rawValue, min: 50.0, max: 1100.0, unit: .milliseconds, unitName: nil, flags: [.flag_IsReadable, .flag_IsWritable], valueStrings: nil, dependentParameters: nil)
        compReleaseParam.value = 100.0
        
        compMakeupParam = AUParameterTree.createParameter(withIdentifier: "compMakeup", name: "Makeup", address: AIVParam.compMakeup.rawValue, min: 0.0, max: 24.0, unit: .decibels, unitName: nil, flags: [.flag_IsReadable, .flag_IsWritable], valueStrings: nil, dependentParameters: nil)
        
        compAutoMakeupParam = AUParameterTree.createParameter(withIdentifier: "compAutoMakeup", name: "Auto Makeup", address: AIVParam.compAutoMakeup.rawValue, min: 0.0, max: 1.0, unit: .boolean, unitName: nil, flags: [.flag_IsReadable, .flag_IsWritable], valueStrings: nil, dependentParameters: nil)
        
        // Limiter
        limiterCeilingParam = AUParameterTree.createParameter(withIdentifier: "limiterCeiling", name: "Ceiling", address: AIVParam.limiterCeiling.rawValue, min: -6.0, max: 0.0, unit: .decibels, unitName: nil, flags: [.flag_IsReadable, .flag_IsWritable], valueStrings: nil, dependentParameters: nil)
        limiterCeilingParam.value = -0.1
        
        limiterLookaheadParam = AUParameterTree.createParameter(withIdentifier: "limiterLookahead", name: "Lookahead", address: AIVParam.limiterLookahead.rawValue, min: 0.1, max: 5.0, unit: .milliseconds, unitName: nil, flags: [.flag_IsReadable, .flag_IsWritable], valueStrings: nil, dependentParameters: nil)
        limiterLookaheadParam.value = 2.0
        

        
        // Saturation
        satDriveParam = AUParameterTree.createParameter(withIdentifier: "satDrive", name: "Drive", address: AIVParam.satDrive.rawValue, min: 0.0, max: 100.0, unit: .percent, unitName: nil, flags: [.flag_IsReadable, .flag_IsWritable], valueStrings: nil, dependentParameters: nil)
        
        satTypeParam = AUParameterTree.createParameter(withIdentifier: "satType", name: "Type", address: AIVParam.satType.rawValue, min: 0.0, max: 1.0, unit: .indexed, unitName: nil, flags: [.flag_IsReadable, .flag_IsWritable], valueStrings: nil, dependentParameters: nil)
        
        // Delay
        delayTimeParam = AUParameterTree.createParameter(withIdentifier: "delayTime", name: "Time", address: AIVParam.delayTime.rawValue, min: 0.0, max: 2.0, unit: .seconds, unitName: nil, flags: [.flag_IsReadable, .flag_IsWritable], valueStrings: nil, dependentParameters: nil)
        
        delayFeedbackParam = AUParameterTree.createParameter(withIdentifier: "delayFeedback", name: "Feedback", address: AIVParam.delayFeedback.rawValue, min: 0.0, max: 100.0, unit: .percent, unitName: nil, flags: [.flag_IsReadable, .flag_IsWritable], valueStrings: nil, dependentParameters: nil)
        
        delayMixParam = AUParameterTree.createParameter(withIdentifier: "delayMix", name: "Mix", address: AIVParam.delayMix.rawValue, min: 0.0, max: 100.0, unit: .percent, unitName: nil, flags: [.flag_IsReadable, .flag_IsWritable], valueStrings: nil, dependentParameters: nil)
        
        // Reverb
        reverbSizeParam = AUParameterTree.createParameter(withIdentifier: "reverbSize", name: "Size", address: AIVParam.reverbSize.rawValue, min: 0.0, max: 100.0, unit: .percent, unitName: nil, flags: [.flag_IsReadable, .flag_IsWritable], valueStrings: nil, dependentParameters: nil)
        reverbSizeParam.value = 50.0
        
        reverbDampParam = AUParameterTree.createParameter(withIdentifier: "reverbDamp", name: "Damp", address: AIVParam.reverbDamp.rawValue, min: 0.0, max: 100.0, unit: .percent, unitName: nil, flags: [.flag_IsReadable, .flag_IsWritable], valueStrings: nil, dependentParameters: nil)
        reverbDampParam.value = 50.0
        
        reverbMixParam = AUParameterTree.createParameter(withIdentifier: "reverbMix", name: "Mix", address: AIVParam.reverbMix.rawValue, min: 0.0, max: 100.0, unit: .percent, unitName: nil, flags: [.flag_IsReadable, .flag_IsWritable], valueStrings: nil, dependentParameters: nil)
        reverbMixParam.value = 40.0
    
        // Auto Level
        autoLevelTargetParam = AUParameterTree.createParameter(withIdentifier: "autoLevelTarget", name: "Target Level", address: AIVParam.autoLevelTarget.rawValue, min: -60.0, max: 0.0, unit: .decibels, unitName: nil, flags: [.flag_IsReadable, .flag_IsWritable], valueStrings: nil, dependentParameters: nil)
        autoLevelTargetParam.value = -10.0

        autoLevelRangeParam = AUParameterTree.createParameter(withIdentifier: "autoLevelRange", name: "Level Range", address: AIVParam.autoLevelRange.rawValue, min: 0.0, max: 40.0, unit: .decibels, unitName: nil, flags: [.flag_IsReadable, .flag_IsWritable], valueStrings: nil, dependentParameters: nil)
        autoLevelRangeParam.value = 12.0

        autoLevelSpeedParam = AUParameterTree.createParameter(withIdentifier: "autoLevelSpeed", name: "Level Speed", address: AIVParam.autoLevelSpeed.rawValue, min: 0.0, max: 100.0, unit: .percent, unitName: nil, flags: [.flag_IsReadable, .flag_IsWritable], valueStrings: nil, dependentParameters: nil)
        autoLevelSpeedParam.value = 50.0

        // Deesser
        deesserThreshParam = AUParameterTree.createParameter(withIdentifier: "deesserThresh", name: "Deess Thresh", address: AIVParam.deesserThresh.rawValue, min: -60.0, max: 0.0, unit: .decibels, unitName: nil, flags: [.flag_IsReadable, .flag_IsWritable], valueStrings: nil, dependentParameters: nil)
        deesserThreshParam.value = -20.0

        deesserFreqParam = AUParameterTree.createParameter(withIdentifier: "deesserFreq", name: "Deess Freq", address: AIVParam.deesserFreq.rawValue, min: 2000.0, max: 10000.0, unit: .hertz, unitName: nil, flags: [.flag_IsReadable, .flag_IsWritable], valueStrings: nil, dependentParameters: nil)
        deesserFreqParam.value = 5000.0

        deesserRatioParam = AUParameterTree.createParameter(withIdentifier: "deesserRatio", name: "Deess Ratio", address: AIVParam.deesserRatio.rawValue, min: 1.0, max: 20.0, unit: .ratio, unitName: nil, flags: [.flag_IsReadable, .flag_IsWritable], valueStrings: nil, dependentParameters: nil)
        deesserRatioParam.value = 5.0
        
        deesserRangeParam = AUParameterTree.createParameter(withIdentifier: "deesserRange", name: "Deess Range", address: AIVParam.deesserRange.rawValue, min: -24.0, max: 0.0, unit: .decibels, unitName: nil, flags: [.flag_IsReadable, .flag_IsWritable], valueStrings: nil, dependentParameters: nil)
        deesserRangeParam.value = -6.0

        // Gate
        gateThreshParam = AUParameterTree.createParameter(withIdentifier: "gateThresh", name: "Gate Thresh", address: AIVParam.gateThresh.rawValue, min: -80.0, max: 0.0, unit: .decibels, unitName: nil, flags: [.flag_IsReadable, .flag_IsWritable], valueStrings: nil, dependentParameters: nil)
        gateThreshParam.value = -40.0
        
        gateRangeParam = AUParameterTree.createParameter(withIdentifier: "gateRange", name: "Gate Range", address: AIVParam.gateRange.rawValue, min: -80.0, max: 0.0, unit: .decibels, unitName: nil, flags: [.flag_IsReadable, .flag_IsWritable], valueStrings: nil, dependentParameters: nil)
        gateRangeParam.value = -20.0
        
        gateAttackParam = AUParameterTree.createParameter(withIdentifier: "gateAttack", name: "Gate Attack", address: AIVParam.gateAttack.rawValue, min: 0.01, max: 100.0, unit: .milliseconds, unitName: nil, flags: [.flag_IsReadable, .flag_IsWritable], valueStrings: nil, dependentParameters: nil)
        gateAttackParam.value = 1.0
        
        gateHoldParam = AUParameterTree.createParameter(withIdentifier: "gateHold", name: "Gate Hold", address: AIVParam.gateHold.rawValue, min: 0.0, max: 1000.0, unit: .milliseconds, unitName: nil, flags: [.flag_IsReadable, .flag_IsWritable], valueStrings: nil, dependentParameters: nil)
        gateHoldParam.value = 150.0
        
        gateReleaseParam = AUParameterTree.createParameter(withIdentifier: "gateRelease", name: "Gate Release", address: AIVParam.gateRelease.rawValue, min: 10.0, max: 2000.0, unit: .milliseconds, unitName: nil, flags: [.flag_IsReadable, .flag_IsWritable], valueStrings: nil, dependentParameters: nil)
        gateReleaseParam.value = 300.0
        
        gateHysteresisParam = AUParameterTree.createParameter(withIdentifier: "gateHysteresis", name: "Gate Hysteresis", address: AIVParam.gateHysteresis.rawValue, min: 0.0, max: 12.0, unit: .decibels, unitName: nil, flags: [.flag_IsReadable, .flag_IsWritable], valueStrings: nil, dependentParameters: nil)
        gateHysteresisParam.value = 6.0

        // Filter
        cutoffParam = AUParameterTree.createParameter(withIdentifier: "cutoff", name: "Cutoff", address: AIVParam.cutoff.rawValue, min: 20.0, max: 20000.0, unit: .hertz, unitName: nil, flags: [.flag_IsReadable, .flag_IsWritable], valueStrings: nil, dependentParameters: nil)
        cutoffParam.value = 20000.0

        resonanceParam = AUParameterTree.createParameter(withIdentifier: "resonance", name: "Resonance", address: AIVParam.resonance.rawValue, min: -20.0, max: 20.0, unit: .decibels, unitName: nil, flags: [.flag_IsReadable, .flag_IsWritable], valueStrings: nil, dependentParameters: nil)
    resonanceParam.value = 0.0
        
        // Enables (Default OFF)
        gateEnableParam = AUParameterTree.createParameter(withIdentifier: "gateEnable", name: "Gate Enable", address: AIVParam.gateEnable.rawValue, min: 0, max: 1, unit: .boolean, unitName: nil, flags: [.flag_IsReadable, .flag_IsWritable], valueStrings: nil, dependentParameters: nil)
        gateEnableParam.value = 0.0
        
        deesserEnableParam = AUParameterTree.createParameter(withIdentifier: "deesserEnable", name: "Deesser Enable", address: AIVParam.deesserEnable.rawValue, min: 0, max: 1, unit: .boolean, unitName: nil, flags: [.flag_IsReadable, .flag_IsWritable], valueStrings: nil, dependentParameters: nil)
        deesserEnableParam.value = 0.0
        
        eqEnableParam = AUParameterTree.createParameter(withIdentifier: "eqEnable", name: "EQ Enable", address: AIVParam.eqEnable.rawValue, min: 0, max: 1, unit: .boolean, unitName: nil, flags: [.flag_IsReadable, .flag_IsWritable], valueStrings: nil, dependentParameters: nil)
        eqEnableParam.value = 0.0
        
        compEnableParam = AUParameterTree.createParameter(withIdentifier: "compEnable", name: "Comp Enable", address: AIVParam.compEnable.rawValue, min: 0, max: 1, unit: .boolean, unitName: nil, flags: [.flag_IsReadable, .flag_IsWritable], valueStrings: nil, dependentParameters: nil)
        compEnableParam.value = 0.0

        satEnableParam = AUParameterTree.createParameter(withIdentifier: "satEnable", name: "Sat Enable", address: AIVParam.satEnable.rawValue, min: 0, max: 1, unit: .boolean, unitName: nil, flags: [.flag_IsReadable, .flag_IsWritable], valueStrings: nil, dependentParameters: nil)
        satEnableParam.value = 0.0

        delayEnableParam = AUParameterTree.createParameter(withIdentifier: "delayEnable", name: "Delay Enable", address: AIVParam.delayEnable.rawValue, min: 0, max: 1, unit: .boolean, unitName: nil, flags: [.flag_IsReadable, .flag_IsWritable], valueStrings: nil, dependentParameters: nil)
        delayEnableParam.value = 0.0

        reverbEnableParam = AUParameterTree.createParameter(withIdentifier: "reverbEnable", name: "Reverb Enable", address: AIVParam.reverbEnable.rawValue, min: 0, max: 1, unit: .boolean, unitName: nil, flags: [.flag_IsReadable, .flag_IsWritable], valueStrings: nil, dependentParameters: nil)
        reverbEnableParam.value = 0.0

        pitchEnableParam = AUParameterTree.createParameter(withIdentifier: "pitchEnable", name: "Pitch Enable", address: AIVParam.pitchEnable.rawValue, min: 0, max: 1, unit: .boolean, unitName: nil, flags: [.flag_IsReadable, .flag_IsWritable], valueStrings: nil, dependentParameters: nil)
        pitchEnableParam.value = 0.0

        limiterEnableParam = AUParameterTree.createParameter(withIdentifier: "limiterEnable", name: "Limiter Enable", address: AIVParam.limiterEnable.rawValue, min: 0, max: 1, unit: .boolean, unitName: nil, flags: [.flag_IsReadable, .flag_IsWritable], valueStrings: nil, dependentParameters: nil)
        limiterEnableParam.value = 0.0


        // 2. Create Parameter Tree
        parameterTree = AUParameterTree.createTree(withChildren: [
            gainParam, bypassParam,
            inputGainParam, saturationParam, phaseInvertParam,
            pitchAmountParam, pitchSpeedParam,
            eqBand1FreqParam, eqBand1GainParam, eqBand1QParam,
            eqBand2FreqParam, eqBand2GainParam, eqBand2QParam,
            eqBand3FreqParam, eqBand3GainParam, eqBand3QParam,
            compInputParam, compRatioParam, compAttackParam, compReleaseParam, compMakeupParam, compAutoMakeupParam,
            limiterCeilingParam, limiterLookaheadParam,

            satDriveParam, satTypeParam,
            delayTimeParam, delayFeedbackParam, delayMixParam,
            reverbSizeParam, reverbDampParam, reverbMixParam,
            autoLevelTargetParam, autoLevelRangeParam, autoLevelSpeedParam,
            deesserThreshParam, deesserFreqParam, deesserRatioParam, deesserRangeParam,
            gateThreshParam, gateRangeParam, gateAttackParam, gateHoldParam, gateReleaseParam, gateHysteresisParam,
            cutoffParam, resonanceParam,
            
            gateEnableParam, deesserEnableParam, eqEnableParam, compEnableParam, satEnableParam, delayEnableParam, reverbEnableParam, pitchEnableParam, limiterEnableParam
        ])

        // 3. Connect to Kernel
        parameterTree.implementorValueObserver = { param, value in
            kernelAdapter.setParameter(param, value: value)
        }

        parameterTree.implementorValueProvider = { param in
            return kernelAdapter.value(for: param)
        }

        parameterTree.implementorStringFromValueCallback = { param, value in
            let v = value?.pointee ?? param.value
            return String(format: "%.2f", v)
        }
        
        syncParametersToKernel()
    }
    
    private func syncParametersToKernel() {
        for param in parameterTree.allParameters {
            kernelAdapter.setParameter(param, value: param.value)
        }
    }
}
