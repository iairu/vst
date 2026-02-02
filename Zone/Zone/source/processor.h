//------------------------------------------------------------------------
// Copyright(c) 2026 .
//------------------------------------------------------------------------

#pragma once

#include "public.sdk/source/vst/vstaudioeffect.h"
#include <cmath>
#include <vector>

namespace MyCompanyName {

//------------------------------------------------------------------------
// Constants for audio processing
//------------------------------------------------------------------------
static const int kMaxDelayLength = 88200;   // 2 seconds at 44.1kHz
static const int kChorusDelayLength = 4410; // ~100ms max chorus delay
static const int kReverbDelayLines = 4;

//------------------------------------------------------------------------
//  ZoneProcessor
//------------------------------------------------------------------------
class ZoneProcessor : public Steinberg::Vst::AudioEffect {
public:
  ZoneProcessor();
  ~ZoneProcessor() SMTG_OVERRIDE;

  // Create function
  static Steinberg::FUnknown *createInstance(void * /*context*/) {
    return (Steinberg::Vst::IAudioProcessor *)new ZoneProcessor;
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
  // Parameter values (0.0 to 1.0)
  float fShimmerAmount = 0.3f;
  float fChorusDepth = 0.5f;
  float fChorusRate = 0.5f;
  float fSaturation = 0.4f;
  float fReverbMix = 0.4f;
  float fReverbDecay = 0.6f;
  float fStereoWidth = 0.5f;
  float fMasterMix = 0.5f;

  // Sample rate
  double sampleRate = 44100.0;

  // Chorus state
  std::vector<float> chorusDelayL;
  std::vector<float> chorusDelayR;
  int chorusWritePos = 0;
  float chorusLfoPhase = 0.0f;

  // Shimmer state (pitch-shifted feedback with filtering)
  std::vector<float> shimmerDelayL;
  std::vector<float> shimmerDelayR;
  int shimmerWritePos = 0;
  float shimmerFilterL = 0.0f;
  float shimmerFilterR = 0.0f;

  // Reverb state (simple feedback delay network)
  std::vector<float> reverbDelayL[kReverbDelayLines];
  std::vector<float> reverbDelayR[kReverbDelayLines];
  int reverbWritePos[kReverbDelayLines] = {0, 0, 0, 0};
  float reverbFilterL = 0.0f;
  float reverbFilterR = 0.0f;

  // Reverb delay times (prime numbers for diffusion)
  static constexpr int kReverbDelayTimes[kReverbDelayLines] = {1559, 1847, 2203,
                                                               2647};

  // Helper functions
  void clearDelayBuffers();
  float softClip(float x, float amount);
  float processSample(float inL, float inR, float &outL, float &outR);
};

//------------------------------------------------------------------------
} // namespace MyCompanyName
