//------------------------------------------------------------------------
// Copyright(c) 2026 .
//------------------------------------------------------------------------

#pragma once

#include "pluginterfaces/base/funknown.h"
#include "pluginterfaces/vst/vsttypes.h"

namespace MyCompanyName {
//------------------------------------------------------------------------
static const Steinberg::FUID kZoneProcessorUID (0x1A2C4883, 0xC8995771, 0xA90ADD83, 0x9967D30E);
static const Steinberg::FUID kZoneControllerUID (0x2372F749, 0x309B5D20, 0x90065396, 0xDD9776AA);

#define ZoneVST3Category "Fx|Reverb|Modulation"

//------------------------------------------------------------------------
// Parameter IDs
//------------------------------------------------------------------------
enum ZoneParams : Steinberg::Vst::ParamID
{
    kParamShimmerAmount = 0,    // High-frequency shimmer intensity
    kParamChorusDepth = 1,      // Chorus/detune depth
    kParamChorusRate = 2,       // Chorus modulation speed (Hz)
    kParamSaturation = 3,       // Saturation/overdrive amount
    kParamReverbMix = 4,        // Reverb wet/dry mix
    kParamReverbDecay = 5,      // Reverb tail length
    kParamStereoWidth = 6,      // Stereo widening amount
    kParamMasterMix = 7,        // Overall dry/wet mix
    kNumParams = 8
};

//------------------------------------------------------------------------
} // namespace MyCompanyName

