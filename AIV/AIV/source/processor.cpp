//------------------------------------------------------------------------
// Copyright(c) 2026 AIV.
//------------------------------------------------------------------------

#include "processor.h"
#include "cids.h"
#include "params.h"

#include "base/source/fstreamer.h"
#include "pluginterfaces/vst/ivstparameterchanges.h"

#include <cmath>
#include <vector>

using namespace Steinberg;

namespace MyCompanyName {
//------------------------------------------------------------------------
// AIVProcessor
//------------------------------------------------------------------------
AIVProcessor::AIVProcessor() {
  //--- set the wanted controller for our processor
  setControllerClass(kAIVControllerUID);
}

//------------------------------------------------------------------------
AIVProcessor::~AIVProcessor() {}

//------------------------------------------------------------------------
tresult PLUGIN_API AIVProcessor::initialize(FUnknown *context) {
  // Here the Plug-in will be instantiated

  //---always initialize the parent-------
  tresult result = AudioEffect::initialize(context);
  // if everything Ok, continue
  if (result != kResultOk) {
    return result;
  }

  //--- create Audio IO ------
  addAudioInput(STR16("Stereo In"), Steinberg::Vst::SpeakerArr::kStereo);
  addAudioOutput(STR16("Stereo Out"), Steinberg::Vst::SpeakerArr::kStereo);

  /* If you don't need an event bus, you can remove the next line */
  addEventInput(STR16("Event In"), 1);

  return kResultOk;
}

//------------------------------------------------------------------------
tresult PLUGIN_API AIVProcessor::terminate() {
  // Here the Plug-in will be de-instantiated, last possibility to remove some
  // memory!

  //---do not forget to call parent ------
  return AudioEffect::terminate();
}

//------------------------------------------------------------------------
tresult PLUGIN_API AIVProcessor::setActive(TBool state) {
  if (state) {
    // Reset all DSP modules when becoming active
    mGate.reset(mSampleRate);
    mCompressor.reset(mSampleRate);
    mDeEsser.reset(mSampleRate);
    mEQ.reset(mSampleRate);
    mSaturation.reset(mSampleRate);
    mPitch.reset(mSampleRate);
    mDelay.reset(mSampleRate);
    mReverb.reset(mSampleRate);
    mStereoWidthDSP.reset(mSampleRate);
    mAutoLevel.reset(mSampleRate);
    mBreathControl.reset(mSampleRate);

    // Update DSP with current parameters
    updateDSPParameters();
  }

  //--- called when the Plug-in is enable/disable (On/Off) -----
  return AudioEffect::setActive(state);
}

//------------------------------------------------------------------------
void AIVProcessor::updateDSPParameters() {
  mGate.setParameters(mGateThreshold, mGateAttack, mGateHold, mGateRelease,
                      mGateRange);
  mCompressor.setParameters(mCompThreshold, mCompRatio, mCompAttack,
                            mCompRelease, mCompMakeup, mCompKnee);
  mDeEsser.setParameters(mDeEsserFreq, mDeEsserThreshold, mDeEsserRange);

  mEQ.setBand(0, mEQBand1Gain, mEQBand1Freq, mEQBand1Q);
  mEQ.setBand(1, mEQBand2Gain, mEQBand2Freq, mEQBand2Q);
  mEQ.setBand(2, mEQBand3Gain, mEQBand3Freq, mEQBand3Q);
  mEQ.setBand(3, mEQBand4Gain, mEQBand4Freq, mEQBand4Q);

  mSaturation.setParameters(mSatDrive, mSatMix, mSatWarmth);
  mPitch.setParameters(mPitchSpeed, mPitchAmount);
  mDelay.setParameters(mDelayTimeL, mDelayTimeR, mDelayFeedback, mDelayMix,
                       mDelaySync, mDelayHighpass, mDelayLowpass);
  mReverb.setParameters(mReverbSize, mReverbDecay, mReverbPredelay, mReverbMix,
                        mReverbDamping);
  mStereoWidthDSP.setParameters(mStereoWidthParam, mStereoMonoFreq);
  mAutoLevel.setParameters(mAutoLevelTarget, mAutoLevelSpeed);
  mBreathControl.setParameters(mBreathSensitivity, mBreathReduction);
}

//------------------------------------------------------------------------
tresult PLUGIN_API AIVProcessor::process(Vst::ProcessData &data) {
  //--- First : Read inputs parameter changes-----------
  if (data.inputParameterChanges) {
    int32 numParamsChanged = data.inputParameterChanges->getParameterCount();
    for (int32 index = 0; index < numParamsChanged; index++) {
      if (auto *paramQueue =
              data.inputParameterChanges->getParameterData(index)) {
        Vst::ParamValue value;
        int32 sampleOffset;
        int32 numPoints = paramQueue->getPointCount();
        if (paramQueue->getPoint(numPoints - 1, sampleOffset, value) ==
            kResultTrue) {
          switch (paramQueue->getParameterId()) {
          // Global
          case AIV::kParamInputGain:
            mInputGain = static_cast<float>(value);
            break;
          case AIV::kParamOutputGain:
            mOutputGain = static_cast<float>(value);
            break;
          case AIV::kParamDryWet:
            mDryWet = static_cast<float>(value);
            break;

          // Gate
          case AIV::kParamGateEnable:
            mGateEnabled = value > 0.5;
            break;
          case AIV::kParamGateThreshold:
            mGateThreshold = static_cast<float>(value);
            break;
          case AIV::kParamGateAttack:
            mGateAttack = static_cast<float>(value);
            break;
          case AIV::kParamGateHold:
            mGateHold = static_cast<float>(value);
            break;
          case AIV::kParamGateRelease:
            mGateRelease = static_cast<float>(value);
            break;
          case AIV::kParamGateRange:
            mGateRange = static_cast<float>(value);
            break;

          // Compressor
          case AIV::kParamCompEnable:
            mCompEnabled = value > 0.5;
            break;
          case AIV::kParamCompThreshold:
            mCompThreshold = static_cast<float>(value);
            break;
          case AIV::kParamCompRatio:
            mCompRatio = static_cast<float>(value);
            break;
          case AIV::kParamCompAttack:
            mCompAttack = static_cast<float>(value);
            break;
          case AIV::kParamCompRelease:
            mCompRelease = static_cast<float>(value);
            break;
          case AIV::kParamCompMakeup:
            mCompMakeup = static_cast<float>(value);
            break;
          case AIV::kParamCompKnee:
            mCompKnee = static_cast<float>(value);
            break;

          // De-esser
          case AIV::kParamDeEsserEnable:
            mDeEsserEnabled = value > 0.5;
            break;
          case AIV::kParamDeEsserFreq:
            mDeEsserFreq = static_cast<float>(value);
            break;
          case AIV::kParamDeEsserThreshold:
            mDeEsserThreshold = static_cast<float>(value);
            break;
          case AIV::kParamDeEsserRange:
            mDeEsserRange = static_cast<float>(value);
            break;

          // EQ
          case AIV::kParamEQEnable:
            mEQEnabled = value > 0.5;
            break;
          case AIV::kParamEQBand1Gain:
            mEQBand1Gain = static_cast<float>(value);
            break;
          case AIV::kParamEQBand1Freq:
            mEQBand1Freq = static_cast<float>(value);
            break;
          case AIV::kParamEQBand1Q:
            mEQBand1Q = static_cast<float>(value);
            break;
          case AIV::kParamEQBand2Gain:
            mEQBand2Gain = static_cast<float>(value);
            break;
          case AIV::kParamEQBand2Freq:
            mEQBand2Freq = static_cast<float>(value);
            break;
          case AIV::kParamEQBand2Q:
            mEQBand2Q = static_cast<float>(value);
            break;
          case AIV::kParamEQBand3Gain:
            mEQBand3Gain = static_cast<float>(value);
            break;
          case AIV::kParamEQBand3Freq:
            mEQBand3Freq = static_cast<float>(value);
            break;
          case AIV::kParamEQBand3Q:
            mEQBand3Q = static_cast<float>(value);
            break;
          case AIV::kParamEQBand4Gain:
            mEQBand4Gain = static_cast<float>(value);
            break;
          case AIV::kParamEQBand4Freq:
            mEQBand4Freq = static_cast<float>(value);
            break;
          case AIV::kParamEQBand4Q:
            mEQBand4Q = static_cast<float>(value);
            break;

          // Saturation
          case AIV::kParamSatEnable:
            mSatEnabled = value > 0.5;
            break;
          case AIV::kParamSatDrive:
            mSatDrive = static_cast<float>(value);
            break;
          case AIV::kParamSatMix:
            mSatMix = static_cast<float>(value);
            break;
          case AIV::kParamSatWarmth:
            mSatWarmth = static_cast<float>(value);
            break;

          // Pitch
          case AIV::kParamPitchEnable:
            mPitchEnabled = value > 0.5;
            break;
          case AIV::kParamPitchSpeed:
            mPitchSpeed = static_cast<float>(value);
            break;
          case AIV::kParamPitchAmount:
            mPitchAmount = static_cast<float>(value);
            break;

          // Delay
          case AIV::kParamDelayEnable:
            mDelayEnabled = value > 0.5;
            break;
          case AIV::kParamDelayTimeL:
            mDelayTimeL = static_cast<float>(value);
            break;
          case AIV::kParamDelayTimeR:
            mDelayTimeR = static_cast<float>(value);
            break;
          case AIV::kParamDelayFeedback:
            mDelayFeedback = static_cast<float>(value);
            break;
          case AIV::kParamDelayMix:
            mDelayMix = static_cast<float>(value);
            break;
          case AIV::kParamDelaySync:
            mDelaySync = static_cast<float>(value);
            break;
          case AIV::kParamDelayHighpass:
            mDelayHighpass = static_cast<float>(value);
            break;
          case AIV::kParamDelayLowpass:
            mDelayLowpass = static_cast<float>(value);
            break;

          // Reverb
          case AIV::kParamReverbEnable:
            mReverbEnabled = value > 0.5;
            break;
          case AIV::kParamReverbSize:
            mReverbSize = static_cast<float>(value);
            break;
          case AIV::kParamReverbDecay:
            mReverbDecay = static_cast<float>(value);
            break;
          case AIV::kParamReverbPredelay:
            mReverbPredelay = static_cast<float>(value);
            break;
          case AIV::kParamReverbMix:
            mReverbMix = static_cast<float>(value);
            break;
          case AIV::kParamReverbDamping:
            mReverbDamping = static_cast<float>(value);
            break;

          // Stereo
          case AIV::kParamStereoEnable:
            mStereoEnabled = value > 0.5;
            break;
          case AIV::kParamStereoWidth:
            mStereoWidthParam = static_cast<float>(value);
            break;
          case AIV::kParamStereoMonoFreq:
            mStereoMonoFreq = static_cast<float>(value);
            break;

          // Auto Level
          case AIV::kParamAutoLevelEnable:
            mAutoLevelEnabled = value > 0.5;
            break;
          case AIV::kParamAutoLevelTarget:
            mAutoLevelTarget = static_cast<float>(value);
            break;
          case AIV::kParamAutoLevelSpeed:
            mAutoLevelSpeed = static_cast<float>(value);
            break;

          // Breath Control
          case AIV::kParamBreathEnable:
            mBreathEnabled = value > 0.5;
            break;
          case AIV::kParamBreathSensitivity:
            mBreathSensitivity = static_cast<float>(value);
            break;
          case AIV::kParamBreathReduction:
            mBreathReduction = static_cast<float>(value);
            break;
          }
        }
      }
    }

    // Update DSP parameters after reading changes
    updateDSPParameters();
  }

  //--- Here we go...the processing
  if (data.numSamples > 0 && data.numInputs > 0 && data.numOutputs > 0) {
    float *inL = data.inputs[0].channelBuffers32[0];
    float *inR = data.inputs[0].channelBuffers32[1];
    float *outL = data.outputs[0].channelBuffers32[0];
    float *outR = data.outputs[0].channelBuffers32[1];

    int32 numSamples = data.numSamples;

    // Copy input to output if not in-place
    if (outL != inL)
      memcpy(outL, inL, static_cast<size_t>(numSamples) * sizeof(float));
    if (outR != inR)
      memcpy(outR, inR, static_cast<size_t>(numSamples) * sizeof(float));

    // Store dry signal for wet/dry mix
    std::vector<float> dryL(static_cast<size_t>(numSamples));
    std::vector<float> dryR(static_cast<size_t>(numSamples));
    memcpy(dryL.data(), outL, static_cast<size_t>(numSamples) * sizeof(float));
    memcpy(dryR.data(), outR, static_cast<size_t>(numSamples) * sizeof(float));

    // Apply input gain
    float inputGainLin = std::pow(10.0f, (mInputGain * 48.0f - 24.0f) / 20.0f);
    for (int32 i = 0; i < numSamples; ++i) {
      outL[i] *= inputGainLin;
      outR[i] *= inputGainLin;
    }

    // Process through DSP chain
    // Order: Gate -> Comp -> De-Ess -> EQ -> Sat -> Pitch -> Delay -> Reverb ->
    // Stereo -> AutoLevel -> Breath

    if (mGateEnabled)
      mGate.process(outL, outR, numSamples);

    if (mCompEnabled)
      mCompressor.process(outL, outR, numSamples);

    if (mDeEsserEnabled)
      mDeEsser.process(outL, outR, numSamples);

    if (mEQEnabled)
      mEQ.process(outL, outR, numSamples);

    if (mSatEnabled)
      mSaturation.process(outL, outR, numSamples);

    if (mPitchEnabled)
      mPitch.process(outL, outR, numSamples);

    if (mDelayEnabled)
      mDelay.process(outL, outR, numSamples);

    if (mReverbEnabled)
      mReverb.process(outL, outR, numSamples);

    if (mStereoEnabled)
      mStereoWidthDSP.process(outL, outR, numSamples);

    if (mAutoLevelEnabled)
      mAutoLevel.process(outL, outR, numSamples);

    if (mBreathEnabled)
      mBreathControl.process(outL, outR, numSamples);

    // Apply output gain and wet/dry mix
    float outputGainLin =
        std::pow(10.0f, (mOutputGain * 48.0f - 24.0f) / 20.0f);
    for (int32 i = 0; i < numSamples; ++i) {
      outL[i] =
          dryL[static_cast<size_t>(i)] * (1.0f - mDryWet) + outL[i] * mDryWet;
      outR[i] =
          dryR[static_cast<size_t>(i)] * (1.0f - mDryWet) + outR[i] * mDryWet;
      outL[i] *= outputGainLin;
      outR[i] *= outputGainLin;
    }

    data.outputs[0].silenceFlags = 0;
  }

  return kResultOk;
}

//------------------------------------------------------------------------
tresult PLUGIN_API AIVProcessor::setupProcessing(Vst::ProcessSetup &newSetup) {
  mSampleRate = newSetup.sampleRate;

  //--- called before any processing ----
  return AudioEffect::setupProcessing(newSetup);
}

//------------------------------------------------------------------------
tresult PLUGIN_API
AIVProcessor::canProcessSampleSize(int32 symbolicSampleSize) {
  // by default kSample32 is supported
  if (symbolicSampleSize == Vst::kSample32)
    return kResultTrue;

  return kResultFalse;
}

//------------------------------------------------------------------------
tresult PLUGIN_API AIVProcessor::setState(IBStream *state) {
  // called when we load a preset, the model has to be reloaded
  IBStreamer streamer(state, kLittleEndian);

  // Read all parameters
  streamer.readFloat(mInputGain);
  streamer.readFloat(mOutputGain);
  streamer.readFloat(mDryWet);

  int32 enabled;
  streamer.readInt32(enabled);
  mGateEnabled = enabled != 0;
  streamer.readFloat(mGateThreshold);
  streamer.readFloat(mGateAttack);
  streamer.readFloat(mGateHold);
  streamer.readFloat(mGateRelease);
  streamer.readFloat(mGateRange);

  streamer.readInt32(enabled);
  mCompEnabled = enabled != 0;
  streamer.readFloat(mCompThreshold);
  streamer.readFloat(mCompRatio);
  streamer.readFloat(mCompAttack);
  streamer.readFloat(mCompRelease);
  streamer.readFloat(mCompMakeup);
  streamer.readFloat(mCompKnee);

  streamer.readInt32(enabled);
  mDeEsserEnabled = enabled != 0;
  streamer.readFloat(mDeEsserFreq);
  streamer.readFloat(mDeEsserThreshold);
  streamer.readFloat(mDeEsserRange);

  streamer.readInt32(enabled);
  mEQEnabled = enabled != 0;
  streamer.readFloat(mEQBand1Gain);
  streamer.readFloat(mEQBand1Freq);
  streamer.readFloat(mEQBand1Q);
  streamer.readFloat(mEQBand2Gain);
  streamer.readFloat(mEQBand2Freq);
  streamer.readFloat(mEQBand2Q);
  streamer.readFloat(mEQBand3Gain);
  streamer.readFloat(mEQBand3Freq);
  streamer.readFloat(mEQBand3Q);
  streamer.readFloat(mEQBand4Gain);
  streamer.readFloat(mEQBand4Freq);
  streamer.readFloat(mEQBand4Q);

  streamer.readInt32(enabled);
  mSatEnabled = enabled != 0;
  streamer.readFloat(mSatDrive);
  streamer.readFloat(mSatMix);
  streamer.readFloat(mSatWarmth);

  streamer.readInt32(enabled);
  mPitchEnabled = enabled != 0;
  streamer.readFloat(mPitchSpeed);
  streamer.readFloat(mPitchAmount);

  streamer.readInt32(enabled);
  mDelayEnabled = enabled != 0;
  streamer.readFloat(mDelayTimeL);
  streamer.readFloat(mDelayTimeR);
  streamer.readFloat(mDelayFeedback);
  streamer.readFloat(mDelayMix);
  streamer.readFloat(mDelaySync);
  streamer.readFloat(mDelayHighpass);
  streamer.readFloat(mDelayLowpass);

  streamer.readInt32(enabled);
  mReverbEnabled = enabled != 0;
  streamer.readFloat(mReverbSize);
  streamer.readFloat(mReverbDecay);
  streamer.readFloat(mReverbPredelay);
  streamer.readFloat(mReverbMix);
  streamer.readFloat(mReverbDamping);

  streamer.readInt32(enabled);
  mStereoEnabled = enabled != 0;
  streamer.readFloat(mStereoWidthParam);
  streamer.readFloat(mStereoMonoFreq);

  streamer.readInt32(enabled);
  mAutoLevelEnabled = enabled != 0;
  streamer.readFloat(mAutoLevelTarget);
  streamer.readFloat(mAutoLevelSpeed);

  streamer.readInt32(enabled);
  mBreathEnabled = enabled != 0;
  streamer.readFloat(mBreathSensitivity);
  streamer.readFloat(mBreathReduction);

  return kResultOk;
}

//------------------------------------------------------------------------
tresult PLUGIN_API AIVProcessor::getState(IBStream *state) {
  // here we need to save the model
  IBStreamer streamer(state, kLittleEndian);

  // Write all parameters
  streamer.writeFloat(mInputGain);
  streamer.writeFloat(mOutputGain);
  streamer.writeFloat(mDryWet);

  streamer.writeInt32(mGateEnabled ? 1 : 0);
  streamer.writeFloat(mGateThreshold);
  streamer.writeFloat(mGateAttack);
  streamer.writeFloat(mGateHold);
  streamer.writeFloat(mGateRelease);
  streamer.writeFloat(mGateRange);

  streamer.writeInt32(mCompEnabled ? 1 : 0);
  streamer.writeFloat(mCompThreshold);
  streamer.writeFloat(mCompRatio);
  streamer.writeFloat(mCompAttack);
  streamer.writeFloat(mCompRelease);
  streamer.writeFloat(mCompMakeup);
  streamer.writeFloat(mCompKnee);

  streamer.writeInt32(mDeEsserEnabled ? 1 : 0);
  streamer.writeFloat(mDeEsserFreq);
  streamer.writeFloat(mDeEsserThreshold);
  streamer.writeFloat(mDeEsserRange);

  streamer.writeInt32(mEQEnabled ? 1 : 0);
  streamer.writeFloat(mEQBand1Gain);
  streamer.writeFloat(mEQBand1Freq);
  streamer.writeFloat(mEQBand1Q);
  streamer.writeFloat(mEQBand2Gain);
  streamer.writeFloat(mEQBand2Freq);
  streamer.writeFloat(mEQBand2Q);
  streamer.writeFloat(mEQBand3Gain);
  streamer.writeFloat(mEQBand3Freq);
  streamer.writeFloat(mEQBand3Q);
  streamer.writeFloat(mEQBand4Gain);
  streamer.writeFloat(mEQBand4Freq);
  streamer.writeFloat(mEQBand4Q);

  streamer.writeInt32(mSatEnabled ? 1 : 0);
  streamer.writeFloat(mSatDrive);
  streamer.writeFloat(mSatMix);
  streamer.writeFloat(mSatWarmth);

  streamer.writeInt32(mPitchEnabled ? 1 : 0);
  streamer.writeFloat(mPitchSpeed);
  streamer.writeFloat(mPitchAmount);

  streamer.writeInt32(mDelayEnabled ? 1 : 0);
  streamer.writeFloat(mDelayTimeL);
  streamer.writeFloat(mDelayTimeR);
  streamer.writeFloat(mDelayFeedback);
  streamer.writeFloat(mDelayMix);
  streamer.writeFloat(mDelaySync);
  streamer.writeFloat(mDelayHighpass);
  streamer.writeFloat(mDelayLowpass);

  streamer.writeInt32(mReverbEnabled ? 1 : 0);
  streamer.writeFloat(mReverbSize);
  streamer.writeFloat(mReverbDecay);
  streamer.writeFloat(mReverbPredelay);
  streamer.writeFloat(mReverbMix);
  streamer.writeFloat(mReverbDamping);

  streamer.writeInt32(mStereoEnabled ? 1 : 0);
  streamer.writeFloat(mStereoWidthParam);
  streamer.writeFloat(mStereoMonoFreq);

  streamer.writeInt32(mAutoLevelEnabled ? 1 : 0);
  streamer.writeFloat(mAutoLevelTarget);
  streamer.writeFloat(mAutoLevelSpeed);

  streamer.writeInt32(mBreathEnabled ? 1 : 0);
  streamer.writeFloat(mBreathSensitivity);
  streamer.writeFloat(mBreathReduction);

  return kResultOk;
}

//------------------------------------------------------------------------
} // namespace MyCompanyName
