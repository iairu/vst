//------------------------------------------------------------------------
// Copyright(c) 2026 AIV.
//------------------------------------------------------------------------

#pragma once

#include "dsp/AutoLevel.h"
#include "dsp/BreathControl.h"
#include "dsp/Compressor.h"
#include "dsp/DeEsser.h"
#include "dsp/Delay.h"
#include "dsp/EQ.h"
#include "dsp/Gate.h"
#include "dsp/Pitch.h"
#include "dsp/Reverb.h"
#include "dsp/Saturation.h"
#include "dsp/StereoWidth.h"
#include "params.h"
#include "public.sdk/source/vst/vstaudioeffect.h"

namespace MyCompanyName {

//------------------------------------------------------------------------
//  AIVProcessor
//------------------------------------------------------------------------
class AIVProcessor : public Steinberg::Vst::AudioEffect {
public:
  AIVProcessor();
  ~AIVProcessor() SMTG_OVERRIDE;

  // Create function
  static Steinberg::FUnknown *createInstance(void * /*context*/) {
    return (Steinberg::Vst::IAudioProcessor *)new AIVProcessor;
  }

  //--- ---------------------------------------------------------------------
  // AudioEffect overrides:
  //--- ---------------------------------------------------------------------
  /** Called at first after constructor */
  Steinberg::tresult PLUGIN_API initialize(Steinberg::FUnknown *context)
      SMTG_OVERRIDE;

  /** Called at the end before destructor */
  Steinberg::tresult PLUGIN_API terminate() SMTG_OVERRIDE;

  /** Switch the Plug-in on/off */
  Steinberg::tresult PLUGIN_API setActive(Steinberg::TBool state) SMTG_OVERRIDE;

  /** Will be called before any process call */
  Steinberg::tresult PLUGIN_API
  setupProcessing(Steinberg::Vst::ProcessSetup &newSetup) SMTG_OVERRIDE;

  /** Asks if a given sample size is supported see SymbolicSampleSizes. */
  Steinberg::tresult PLUGIN_API
  canProcessSampleSize(Steinberg::int32 symbolicSampleSize) SMTG_OVERRIDE;

  /** Here we go...the process call */
  Steinberg::tresult PLUGIN_API process(Steinberg::Vst::ProcessData &data)
      SMTG_OVERRIDE;

  /** For persistence */
  Steinberg::tresult PLUGIN_API setState(Steinberg::IBStream *state)
      SMTG_OVERRIDE;
  Steinberg::tresult PLUGIN_API getState(Steinberg::IBStream *state)
      SMTG_OVERRIDE;

  //------------------------------------------------------------------------
protected:
  // Sample rate
  double mSampleRate = 44100.0;

  // DSP Modules
  AIV::DSP::Gate mGate;
  AIV::DSP::Compressor mCompressor;
  AIV::DSP::DeEsser mDeEsser;
  AIV::DSP::EQ mEQ;
  AIV::DSP::Saturation mSaturation;
  AIV::DSP::Pitch mPitch;
  AIV::DSP::Delay mDelay;
  AIV::DSP::Reverb mReverb;
  AIV::DSP::StereoWidth mStereoWidthDSP;
  AIV::DSP::AutoLevel mAutoLevel;
  AIV::DSP::BreathControl mBreathControl;

  // Parameter values - Global
  float mInputGain = AIV::Defaults::InputGain;
  float mOutputGain = AIV::Defaults::OutputGain;
  float mDryWet = AIV::Defaults::DryWet;

  // Gate parameters
  bool mGateEnabled = false;
  float mGateThreshold = AIV::Defaults::GateThreshold;
  float mGateAttack = AIV::Defaults::GateAttack;
  float mGateHold = AIV::Defaults::GateHold;
  float mGateRelease = AIV::Defaults::GateRelease;
  float mGateRange = AIV::Defaults::GateRange;

  // Compressor parameters
  bool mCompEnabled = false;
  float mCompThreshold = AIV::Defaults::CompThreshold;
  float mCompRatio = AIV::Defaults::CompRatio;
  float mCompAttack = AIV::Defaults::CompAttack;
  float mCompRelease = AIV::Defaults::CompRelease;
  float mCompMakeup = AIV::Defaults::CompMakeup;
  float mCompKnee = AIV::Defaults::CompKnee;

  // De-esser parameters
  bool mDeEsserEnabled = false;
  float mDeEsserFreq = AIV::Defaults::DeEsserFreq;
  float mDeEsserThreshold = AIV::Defaults::DeEsserThreshold;
  float mDeEsserRange = AIV::Defaults::DeEsserRange;

  // EQ parameters
  bool mEQEnabled = false;
  float mEQBand1Gain = AIV::Defaults::EQGain;
  float mEQBand1Freq = AIV::Defaults::EQBand1Freq;
  float mEQBand1Q = AIV::Defaults::EQQ;
  float mEQBand2Gain = AIV::Defaults::EQGain;
  float mEQBand2Freq = AIV::Defaults::EQBand2Freq;
  float mEQBand2Q = AIV::Defaults::EQQ;
  float mEQBand3Gain = AIV::Defaults::EQGain;
  float mEQBand3Freq = AIV::Defaults::EQBand3Freq;
  float mEQBand3Q = AIV::Defaults::EQQ;
  float mEQBand4Gain = AIV::Defaults::EQGain;
  float mEQBand4Freq = AIV::Defaults::EQBand4Freq;
  float mEQBand4Q = AIV::Defaults::EQQ;

  // Saturation parameters
  bool mSatEnabled = false;
  float mSatDrive = AIV::Defaults::SatDrive;
  float mSatMix = AIV::Defaults::SatMix;
  float mSatWarmth = AIV::Defaults::SatWarmth;

  // Pitch parameters
  bool mPitchEnabled = false;
  float mPitchSpeed = AIV::Defaults::PitchSpeed;
  float mPitchAmount = AIV::Defaults::PitchAmount;

  // Delay parameters
  bool mDelayEnabled = false;
  float mDelayTimeL = AIV::Defaults::DelayTimeL;
  float mDelayTimeR = AIV::Defaults::DelayTimeR;
  float mDelayFeedback = AIV::Defaults::DelayFeedback;
  float mDelayMix = AIV::Defaults::DelayMix;
  float mDelaySync = 0.0f;
  float mDelayHighpass = 0.0f;
  float mDelayLowpass = 1.0f;

  // Reverb parameters
  bool mReverbEnabled = false;
  float mReverbSize = AIV::Defaults::ReverbSize;
  float mReverbDecay = AIV::Defaults::ReverbDecay;
  float mReverbPredelay = AIV::Defaults::ReverbPredelay;
  float mReverbMix = AIV::Defaults::ReverbMix;
  float mReverbDamping = AIV::Defaults::ReverbDamping;

  // Stereo parameters
  bool mStereoEnabled = false;
  float mStereoWidthParam = AIV::Defaults::StereoWidth;
  float mStereoMonoFreq = AIV::Defaults::StereoMonoFreq;

  // Auto Level parameters
  bool mAutoLevelEnabled = false;
  float mAutoLevelTarget = AIV::Defaults::AutoLevelTarget;
  float mAutoLevelSpeed = AIV::Defaults::AutoLevelSpeed;

  // Breath Control parameters
  bool mBreathEnabled = false;
  float mBreathSensitivity = AIV::Defaults::BreathSensitivity;
  float mBreathReduction = AIV::Defaults::BreathReduction;

  // Helper functions
  void updateDSPParameters();
};

//------------------------------------------------------------------------
} // namespace MyCompanyName
