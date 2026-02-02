//------------------------------------------------------------------------
// Copyright(c) 2026 AIV.
//------------------------------------------------------------------------

#pragma once

#include "pluginterfaces/vst/vsttypes.h"

namespace AIV {

//------------------------------------------------------------------------
// Parameter IDs - organized by module
//------------------------------------------------------------------------
enum ParameterIDs : Steinberg::Vst::ParamID
{
    // Global Parameters (0-9)
    kParamInputGain = 0,
    kParamOutputGain = 1,
    kParamDryWet = 2,

    // Gate Module (10-19)
    kParamGateEnable = 10,
    kParamGateThreshold = 11,
    kParamGateAttack = 12,
    kParamGateHold = 13,
    kParamGateRelease = 14,
    kParamGateRange = 15,

    // Compressor Module (20-29)
    kParamCompEnable = 20,
    kParamCompThreshold = 21,
    kParamCompRatio = 22,
    kParamCompAttack = 23,
    kParamCompRelease = 24,
    kParamCompMakeup = 25,
    kParamCompKnee = 26,

    // De-esser Module (30-39)
    kParamDeEsserEnable = 30,
    kParamDeEsserFreq = 31,
    kParamDeEsserThreshold = 32,
    kParamDeEsserRange = 33,

    // EQ Module (40-59)
    kParamEQEnable = 40,
    // Band 1 - Low Shelf
    kParamEQBand1Gain = 41,
    kParamEQBand1Freq = 42,
    kParamEQBand1Q = 43,
    // Band 2 - Parametric
    kParamEQBand2Gain = 44,
    kParamEQBand2Freq = 45,
    kParamEQBand2Q = 46,
    // Band 3 - Parametric
    kParamEQBand3Gain = 47,
    kParamEQBand3Freq = 48,
    kParamEQBand3Q = 49,
    // Band 4 - High Shelf
    kParamEQBand4Gain = 50,
    kParamEQBand4Freq = 51,
    kParamEQBand4Q = 52,

    // Saturation Module (60-69)
    kParamSatEnable = 60,
    kParamSatDrive = 61,
    kParamSatMix = 62,
    kParamSatWarmth = 63,

    // Pitch Module (70-79)
    kParamPitchEnable = 70,
    kParamPitchSpeed = 71,
    kParamPitchAmount = 72,

    // Delay Module (80-89)
    kParamDelayEnable = 80,
    kParamDelayTimeL = 81,
    kParamDelayTimeR = 82,
    kParamDelayFeedback = 83,
    kParamDelayMix = 84,
    kParamDelaySync = 85,
    kParamDelayHighpass = 86,
    kParamDelayLowpass = 87,

    // Reverb Module (90-99)
    kParamReverbEnable = 90,
    kParamReverbSize = 91,
    kParamReverbDecay = 92,
    kParamReverbPredelay = 93,
    kParamReverbMix = 94,
    kParamReverbDamping = 95,

    // Stereo Width Module (100-109)
    kParamStereoEnable = 100,
    kParamStereoWidth = 101,
    kParamStereoMonoFreq = 102,

    // Auto Level Module (110-119)
    kParamAutoLevelEnable = 110,
    kParamAutoLevelTarget = 111,
    kParamAutoLevelSpeed = 112,

    // Breath Control Module (120-129)
    kParamBreathEnable = 120,
    kParamBreathSensitivity = 121,
    kParamBreathReduction = 122,

    kNumParameters
};

//------------------------------------------------------------------------
// Parameter ranges and defaults
//------------------------------------------------------------------------
namespace Defaults {
    // Global
    constexpr float InputGain = 0.5f;      // 0dB (range: -24 to +24)
    constexpr float OutputGain = 0.5f;     // 0dB
    constexpr float DryWet = 1.0f;         // 100% wet

    // Gate
    constexpr float GateThreshold = 0.25f; // -36dB
    constexpr float GateAttack = 0.01f;    // 1ms
    constexpr float GateHold = 0.1f;       // 10ms
    constexpr float GateRelease = 0.2f;    // 100ms
    constexpr float GateRange = 0.5f;      // -40dB range

    // Compressor
    constexpr float CompThreshold = 0.5f;  // -12dB
    constexpr float CompRatio = 0.2f;      // 4:1
    constexpr float CompAttack = 0.1f;     // 10ms
    constexpr float CompRelease = 0.3f;    // 100ms
    constexpr float CompMakeup = 0.5f;     // 0dB
    constexpr float CompKnee = 0.3f;       // 3dB

    // De-esser
    constexpr float DeEsserFreq = 0.6f;    // 6kHz
    constexpr float DeEsserThreshold = 0.5f;
    constexpr float DeEsserRange = 0.5f;

    // EQ (all gains default to 0.5 = 0dB)
    constexpr float EQGain = 0.5f;
    constexpr float EQBand1Freq = 0.1f;    // 100Hz
    constexpr float EQBand2Freq = 0.3f;    // 800Hz
    constexpr float EQBand3Freq = 0.5f;    // 3kHz
    constexpr float EQBand4Freq = 0.8f;    // 10kHz
    constexpr float EQQ = 0.5f;            // Q = 1.0

    // Saturation
    constexpr float SatDrive = 0.3f;
    constexpr float SatMix = 0.5f;
    constexpr float SatWarmth = 0.5f;

    // Pitch
    constexpr float PitchSpeed = 0.5f;
    constexpr float PitchAmount = 0.5f;

    // Delay
    constexpr float DelayTimeL = 0.25f;    // 250ms
    constexpr float DelayTimeR = 0.25f;
    constexpr float DelayFeedback = 0.3f;
    constexpr float DelayMix = 0.3f;

    // Reverb
    constexpr float ReverbSize = 0.5f;
    constexpr float ReverbDecay = 0.5f;
    constexpr float ReverbPredelay = 0.1f;
    constexpr float ReverbMix = 0.3f;
    constexpr float ReverbDamping = 0.5f;

    // Stereo
    constexpr float StereoWidth = 0.5f;    // 100% (no change)
    constexpr float StereoMonoFreq = 0.1f; // 100Hz

    // Auto Level
    constexpr float AutoLevelTarget = 0.5f; // -12dB
    constexpr float AutoLevelSpeed = 0.5f;

    // Breath
    constexpr float BreathSensitivity = 0.5f;
    constexpr float BreathReduction = 0.5f;
}

//------------------------------------------------------------------------
} // namespace AIV
