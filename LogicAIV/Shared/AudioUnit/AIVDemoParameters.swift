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
        case compThresh = 13
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
    }

    // Parameters
    var gainParam: AUParameter!
    var bypassParam: AUParameter!
    
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
    var compThreshParam: AUParameter!
    var compRatioParam: AUParameter!
    var compAttackParam: AUParameter!
    var compReleaseParam: AUParameter!
    var compMakeupParam: AUParameter!
    
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

    let parameterTree: AUParameterTree

    init(kernelAdapter: AIVDSPKernelAdapter) {

        // 1. Create Parameters
        
        // Global
        gainParam = AUParameterTree.createParameter(withIdentifier: "gain", name: "Gain", address: AIVParam.gain.rawValue, min: 0.0, max: 1.0, unit: .linearGain, unitName: nil, flags: [.flag_IsReadable, .flag_IsWritable], valueStrings: nil, dependentParameters: nil)
        gainParam.value = 0.5
        
        bypassParam = AUParameterTree.createParameter(withIdentifier: "bypass", name: "Bypass", address: AIVParam.bypass.rawValue, min: 0.0, max: 1.0, unit: .boolean, unitName: nil, flags: [.flag_IsReadable, .flag_IsWritable], valueStrings: nil, dependentParameters: nil)
        bypassParam.value = 0.0
        
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
        compThreshParam = AUParameterTree.createParameter(withIdentifier: "compThresh", name: "Threshold", address: AIVParam.compThresh.rawValue, min: -60.0, max: 0.0, unit: .decibels, unitName: nil, flags: [.flag_IsReadable, .flag_IsWritable], valueStrings: nil, dependentParameters: nil)
        compThreshParam.value = 0.0
        
        compRatioParam = AUParameterTree.createParameter(withIdentifier: "compRatio", name: "Ratio", address: AIVParam.compRatio.rawValue, min: 1.0, max: 20.0, unit: .ratio, unitName: nil, flags: [.flag_IsReadable, .flag_IsWritable], valueStrings: nil, dependentParameters: nil)
        compRatioParam.value = 1.0
        
        compAttackParam = AUParameterTree.createParameter(withIdentifier: "compAttack", name: "Attack", address: AIVParam.compAttack.rawValue, min: 0.1, max: 100.0, unit: .milliseconds, unitName: nil, flags: [.flag_IsReadable, .flag_IsWritable], valueStrings: nil, dependentParameters: nil)
        compAttackParam.value = 10.0
        
        compReleaseParam = AUParameterTree.createParameter(withIdentifier: "compRelease", name: "Release", address: AIVParam.compRelease.rawValue, min: 10.0, max: 1000.0, unit: .milliseconds, unitName: nil, flags: [.flag_IsReadable, .flag_IsWritable], valueStrings: nil, dependentParameters: nil)
        compReleaseParam.value = 100.0
        
        compMakeupParam = AUParameterTree.createParameter(withIdentifier: "compMakeup", name: "Makeup", address: AIVParam.compMakeup.rawValue, min: 0.0, max: 24.0, unit: .decibels, unitName: nil, flags: [.flag_IsReadable, .flag_IsWritable], valueStrings: nil, dependentParameters: nil)
        
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


        // 2. Create Parameter Tree
        parameterTree = AUParameterTree.createTree(withChildren: [
            gainParam, bypassParam,
            pitchAmountParam, pitchSpeedParam,
            eqBand1FreqParam, eqBand1GainParam, eqBand1QParam,
            eqBand2FreqParam, eqBand2GainParam, eqBand2QParam,
            eqBand3FreqParam, eqBand3GainParam, eqBand3QParam,
            compThreshParam, compRatioParam, compAttackParam, compReleaseParam, compMakeupParam,
            satDriveParam, satTypeParam,
            delayTimeParam, delayFeedbackParam, delayMixParam,
            reverbSizeParam, reverbDampParam, reverbMixParam
        ])

        // 3. Connect to Kernel
        parameterTree.implementorValueObserver = { param, value in
            kernelAdapter.setParameter(param, value: value)
        }

        parameterTree.implementorValueProvider = { param in
            return kernelAdapter.value(for: param)
        }

        parameterTree.implementorStringFromValueCallback = { param, value in
            let v = value ?? param.value
            return String(format: "%.2f", v)
        }
    }
}
