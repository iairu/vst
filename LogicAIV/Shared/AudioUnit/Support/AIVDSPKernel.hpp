//
//  AIVDSPKernel.hpp
//  AIVExtension
//
//  Created by Ondrej Špánik on 02/02/2026.
//

#pragma once

#import <AudioToolbox/AudioToolbox.h>
#import <algorithm>
#import <vector>

#import <algorithm>
#import <cmath>
#import <vector>

#import "AIVDSPClasses.hpp"
#import "AIVDSPKernelAdapter.h"

/*
 AIVDSPKernel
 As a non-ObjC class, this is safe to use from render thread.
 */
class AIVDSPKernel {
public:
  void initialize(int inputChannelCount, int outputChannelCount,
                  double inSampleRate) {
    mSampleRate = inSampleRate;
    mChannelCount = inputChannelCount;

    // Resize DSP modules for each channel
    mAutoLevel.resize(mChannelCount);
    mGate.resize(mChannelCount);
    mPitch.resize(mChannelCount);
    mDeesser.resize(mChannelCount);
    mSafetyHPF.resize(mChannelCount);
    mHPF.resize(mChannelCount);
    mLowMidCut.resize(mChannelCount);
    // mEQBand3 remains Biquad HighShelf
    mEQBand3.resize(mChannelCount);
    mLPF.resize(mChannelCount);
    mCompressor.resize(mChannelCount);
    mSaturator.resize(mChannelCount);
    mDelay.resize(mChannelCount);
    mReverb.resize(mChannelCount);
    mOversampler.resize(mChannelCount);
    mLimiter.resize(mChannelCount);

    for (auto &os : mOversampler)
      os.initialize();

    updatePreamp();
    updateAutoLevel();
    updateGate();
    updatePitch();
    updateDeesser();
    updateEQ();
    updateFilter();
    updateComp();
    updateDelay();
    updateReverb();
    updateLimiter();

    // Resize scratch buffer for Interleaved handling (Stereo)
    // Max frames typically 1024, but allow for host resizing
    mScratchBuffer.resize(mMaxFramesToRender * 2);
  }

  float *getScratchPointer(int channel) {
    if (channel < 0 || channel > 1)
      return nullptr;
    return mScratchBuffer.data() + (channel * mMaxFramesToRender);
  }

  void deInitialize() {}

  // MARK: - Bypass
  bool isBypassed() { return mBypassed; }

  void setBypass(bool shouldBypass) { mBypassed = shouldBypass; }

  // MARK: - Parameter Getter / Setter
  void setParameter(AUParameterAddress address, AUValue value) {
    switch (address) {
    case AIVParameterAddressGain:
      mGain = value;
      break;
    case AIVParameterAddressBypass:
      mBypassed = (value > 0.5f);
      break;

    // Preamp
    case AIVParameterAddressInputGain:
      mInputGainDb = value;
      updatePreamp();
      break;
    case AIVParameterAddressSaturation:
      mSaturation = value;
      break;
    case AIVParameterAddressPhaseInvert:
      mPhaseInvert = (value > 0.5f);
      break;

    // Gate
    case AIVParameterAddressGateThresh:
      mGateThresh = value;
      updateGate();
      break;
    case AIVParameterAddressGateRange:
      mGateRange = value;
      updateGate();
      break;
    case AIVParameterAddressGateAttack:
      mGateAttack = value;
      updateGate();
      break;
    case AIVParameterAddressGateHold:
      mGateHold = value;
      updateGate();
      break;
    case AIVParameterAddressGateRelease:
      mGateRelease = value;
      updateGate();
      break;
    case AIVParameterAddressGateHysteresis:
      mGateHysteresis = value;
      updateGate();
      break;

    // Auto Level
    case AIVParameterAddressAutoLevelTarget:
      mAutoLevelTarget = value;
      updateAutoLevel();
      break;
    case AIVParameterAddressAutoLevelRange:
      mAutoLevelRange = value;
      updateAutoLevel();
      break;
    case AIVParameterAddressAutoLevelSpeed:
      mAutoLevelSpeed = value;
      updateAutoLevel();
      break;

    // Pitch
    case AIVParameterAddressPitchAmount:
      mPitchAmount = value;
      updatePitch();
      break;
    case AIVParameterAddressPitchSpeed:
      mPitchSpeed = value;
      updatePitch();
      break;

    // Deesser
    case AIVParameterAddressDeesserThresh:
      mDeesserThresh = value;
      updateDeesser();
      break;
    case AIVParameterAddressDeesserFreq:
      mDeesserFreq = value;
      updateDeesser();
      break;
    case AIVParameterAddressDeesserRatio:
      mDeesserRatio = value;
      updateDeesser();
      break;
    case AIVParameterAddressDeesserRange:
      mDeesserRange = value;
      updateDeesser();
      break;

    // EQ
    case AIVParameterAddressEQBand1Freq:
      mEQ1Freq = value;
      updateEQ();
      break;
    case AIVParameterAddressEQBand1Gain:
      mEQ1Gain = value;
      updateEQ();
      break;
    case AIVParameterAddressEQBand1Q:
      mEQ1Q = value;
      updateEQ();
      break;
    case AIVParameterAddressEQBand2Freq:
      mEQ2Freq = value;
      updateEQ();
      break;
    case AIVParameterAddressEQBand2Gain:
      mEQ2Gain = value;
      updateEQ();
      break;
    case AIVParameterAddressEQBand2Q:
      mEQ2Q = value;
      updateEQ();
      break;
    case AIVParameterAddressEQBand3Freq:
      mEQ3Freq = value;
      updateEQ();
      break;
    case AIVParameterAddressEQBand3Gain:
      mEQ3Gain = value;
      updateEQ();
      break;
    case AIVParameterAddressEQBand3Q:
      mEQ3Q = value;
      updateEQ();
      break;

    // Filter (LPF)
    case AIVParameterAddressCutoff:
      mCutoff = value;
      updateFilter();
      break;
    case AIVParameterAddressResonance:
      mResonance = value;
      updateFilter();
      break;

    // Compressor
    case AIVParameterAddressCompThresh:
      mCompThresh = value;
      updateComp();
      break;
    case AIVParameterAddressCompRatio:
      mCompRatio = value;
      updateComp();
      break;
    case AIVParameterAddressCompAttack:
      mCompAttack = value;
      updateComp();
      break;
    case AIVParameterAddressCompRelease:
      mCompRelease = value;
      updateComp();
      break;
    case AIVParameterAddressCompMakeup:
      mCompMakeup = value;
      updateComp();
      break;
    case AIVParameterAddressCompAutoMakeup:
      mCompAutoMakeup = (value > 0.5f);
      updateComp();
      break;

    // Saturation
    case AIVParameterAddressSatDrive:
      mSatDrive = value;
      for (auto &s : mSaturator)
        s.setParameters(mSatDrive, mSatType);
      break;
    case AIVParameterAddressSatType:
      mSatType = value;
      for (auto &s : mSaturator)
        s.setParameters(mSatDrive, mSatType);
      break;

    // Delay
    case AIVParameterAddressDelayTime:
      mDelayTime = value;
      updateDelay();
      break;
    case AIVParameterAddressDelayFeedback:
      mDelayFeedback = value;
      updateDelay();
      break;
    case AIVParameterAddressDelayMix:
      mDelayMix = value;
      updateDelay();
      break;

    // Reverb
    case AIVParameterAddressReverbSize:
      mReverbSize = value;
      updateReverb();
      break;
    case AIVParameterAddressReverbDamp:
      mReverbDamp = value;
      updateReverb();
      break;
    case AIVParameterAddressReverbMix:
      mReverbMix = value;
      updateReverb();
      break;

    // Limiter
    case AIVParameterAddressLimiterCeiling:
      mLimiterCeiling = value;
      updateLimiter();
      break;
    case AIVParameterAddressLimiterLookahead:
      mLimiterLookahead = value;
      updateLimiter();
      break;
    }
  }

  AUValue getParameter(AUParameterAddress address) {
    switch (address) {
    case AIVParameterAddressGain:
      return (AUValue)mGain;
    case AIVParameterAddressBypass:
      return (AUValue)(mBypassed ? 1.0f : 0.0f);

    case AIVParameterAddressInputGain:
      return mInputGainDb;
    case AIVParameterAddressSaturation:
      return mSaturation;
    case AIVParameterAddressPhaseInvert:
      return (AUValue)(mPhaseInvert ? 1.0f : 0.0f);

    case AIVParameterAddressAutoLevelTarget:
      return mAutoLevelTarget;
    case AIVParameterAddressAutoLevelRange:
      return mAutoLevelRange;
    case AIVParameterAddressAutoLevelSpeed:
      return mAutoLevelSpeed;

    case AIVParameterAddressPitchAmount:
      return mPitchAmount;
    case AIVParameterAddressPitchSpeed:
      return mPitchSpeed;

    case AIVParameterAddressDeesserThresh:
      return mDeesserThresh;
    case AIVParameterAddressDeesserFreq:
      return mDeesserFreq;
    case AIVParameterAddressDeesserRatio:
      return mDeesserRatio;
    case AIVParameterAddressDeesserRange:
      return mDeesserRange;

    case AIVParameterAddressEQBand1Freq:
      return mEQ1Freq;
    case AIVParameterAddressEQBand1Gain:
      return mEQ1Gain;
    case AIVParameterAddressEQBand1Q:
      return mEQ1Q;
    case AIVParameterAddressEQBand2Freq:
      return mEQ2Freq;
    case AIVParameterAddressEQBand2Gain:
      return mEQ2Gain;
    case AIVParameterAddressEQBand2Q:
      return mEQ2Q;
    case AIVParameterAddressEQBand3Freq:
      return mEQ3Freq;
    case AIVParameterAddressEQBand3Gain:
      return mEQ3Gain;
    case AIVParameterAddressEQBand3Q:
      return mEQ3Q;

    case AIVParameterAddressCompThresh:
      return mCompThresh;
    case AIVParameterAddressCompRatio:
      return mCompRatio;
    case AIVParameterAddressCompAttack:
      return mCompAttack;
    case AIVParameterAddressCompRelease:
      return mCompRelease;
    case AIVParameterAddressCompMakeup:
      return mCompMakeup;
    case AIVParameterAddressCompAutoMakeup:
      return (AUValue)(mCompAutoMakeup ? 1.0f : 0.0f);

    case AIVParameterAddressGateThresh:
      return mGateThresh;
    case AIVParameterAddressGateRange:
      return mGateRange;
    case AIVParameterAddressGateAttack:
      return mGateAttack;
    case AIVParameterAddressGateHold:
      return mGateHold;
    case AIVParameterAddressGateRelease:
      return mGateRelease;
    case AIVParameterAddressGateHysteresis:
      return mGateHysteresis;

    case AIVParameterAddressLimiterCeiling:
      return mLimiterCeiling;
    case AIVParameterAddressLimiterLookahead:
      return mLimiterLookahead;

    case AIVParameterAddressSatDrive:
      return mSatDrive;
    case AIVParameterAddressSatType:
      return mSatType;

    case AIVParameterAddressDelayTime:
      return mDelayTime;
    case AIVParameterAddressDelayFeedback:
      return mDelayFeedback;
    case AIVParameterAddressDelayMix:
      return mDelayMix;

    case AIVParameterAddressReverbSize:
      return mReverbSize;
    case AIVParameterAddressReverbDamp:
      return mReverbDamp;
    case AIVParameterAddressReverbMix:
      return mReverbMix;

    case AIVParameterAddressCutoff:
      return mCutoff;
    case AIVParameterAddressResonance:
      return mResonance;

    default:
      return 0.f;
    }
  }

  // MARK: - Max Frames
  AUAudioFrameCount maximumFramesToRender() const { return mMaxFramesToRender; }

  void setMaximumFramesToRender(const AUAudioFrameCount &maxFrames) {
    mMaxFramesToRender = maxFrames;
    mScratchBuffer.resize(mMaxFramesToRender * 2);
  }

  // MARK: - Musical Context
  void setMusicalContextBlock(AUHostMusicalContextBlock contextBlock) {
    mMusicalContextBlock = contextBlock;
  }

  /**
   MARK: - Internal Process
   */
  void process(float **inputBuffers, float **outputBuffers,
               AUEventSampleTime bufferStartTime, AUAudioFrameCount frameCount,
               int channelCount) {

    if (mBypassed) {
      for (int channel = 0; channel < channelCount; ++channel) {
        if (inputBuffers[channel] && outputBuffers[channel]) {
          std::copy_n(inputBuffers[channel], frameCount,
                      outputBuffers[channel]);
        }
      }
      return;
    }

    for (int channel = 0; channel < channelCount; ++channel) {
      // Safety check
      if (channel >= mPitch.size())
        break;
      if (!inputBuffers[channel] || !outputBuffers[channel])
        continue;

      float *in = inputBuffers[channel];
      float *out = outputBuffers[channel];

      for (UInt32 frameIndex = 0; frameIndex < frameCount; ++frameIndex) {
        float sample = in[frameIndex];

        // --- UPSAMPLE (1 -> 4) ---
        float osBlock[4];
        mOversampler[channel].processUpsample(sample, osBlock);

        float processedBlock[4];

        // --- CORE PROCESS LOOP (4x) ---
        for (int k = 0; k < 4; ++k) {
          float s = osBlock[k];

          // 0. Preamp & Saturation (Step 1.1) - Now running at 4x
          /*
           Physics: y = tanh(k * x) / tanh(k)
           Ideally Preamp saturation benefits most from OS.
           */
          float inputSample = s;
          float linearSample = inputSample * mInputGainLin;

          float k_val = (mInputGainLin < 0.01f) ? 0.01f : mInputGainLin;
          float wetSample = std::tanh(k_val * inputSample) / std::tanh(k_val);

          float drySample = linearSample;
          float mix = mSaturation / 100.0f;
          s = (1.0f - mix) * drySample + mix * wetSample;

          // Phase Invert
          if (mPhaseInvert) {
            s = -s;
          }

          // 0b. Noise Gate
          s = mGate[channel].process(s);

          // 1. Auto Level
          s = mAutoLevel[channel].process(s);

          // 2. Pitch
          s = mPitch[channel].process(s);

          // 2. Deesser
          s = mDeesser[channel].process(s);

          // 3. EQ
          s = mSafetyHPF[channel].process(s);
          s = mHPF[channel].process(s);
          s = mLowMidCut[channel].process(s);
          s = mLowMidCut[channel].process(s);
          s = mEQBand3[channel].process(s);
          s = mLPF[channel].process(s);

          // 3. Compressor (FET / AIV 76)
          s = mCompressor[channel].process(s);

          // 4. Saturator (Module)
          s = mSaturator[channel].process(s);

          processedBlock[k] = s;
        }

        // --- DOWNSAMPLE (4 -> 1) ---
        sample = mOversampler[channel].processDownsample(processedBlock);

        // --- POST PROCESS (1x) ---

        // 5. Delay
        sample = mDelay[channel].process(sample);

        // 6. Reverb
        sample = mReverb[channel].process(sample);

        // 7. Limiter (TruePeak - 1x is fine as it implements its own
        // lookahead/ISP check logic if robust, or just relies on previous OS
        // being clean)
        sample = mLimiter[channel].process(sample);

        // 8. Global Gain
        out[frameIndex] = sample * mGain;
      }
    }
  }

  void handleOneEvent(AUEventSampleTime now, AURenderEvent const *event) {
    switch (event->head.eventType) {
    case AURenderEventParameter: {
      handleParameterEvent(now, event->parameter);
      break;
    }
    default:
      break;
    }
  }

  void handleParameterEvent(AUEventSampleTime now,
                            AUParameterEvent const &parameterEvent) {
    setParameter(parameterEvent.parameterAddress, parameterEvent.value);
  }

  // Latency Report (4x Oversampling + Limiter Lookahead)
  double getLatency() {
    // Oversampler Latency (16 samples at 1x)
    double osLatency = 0.0;
    if (!mOversampler.empty())
      osLatency = mOversampler[0].getLatency();

    // Limiter Lookahead (Seconds converted to samples)
    // Actually limiter has fixed delay buffer?
    // check TruePeakLimiter implementation: uses lookaheadDelay samples.
    // But lookahead is a parameter.
    // So we report max possible? Or current?
    // Host latency property usually static or changes trigger restart.
    // Let's report current.
    double limLatency = 0.0; // Handled by limiter class? No getter yet.
    // We know mLimiterLookahead is ms.
    // latency = ms * fs / 1000.
    double limSamples = mLimiterLookahead / 1000.0 * mSampleRate;

    return osLatency + limSamples;
  }

private:
  void updateAutoLevel() {
    for (auto &al : mAutoLevel)
      al.setParameters(mAutoLevelTarget, mAutoLevelRange, mAutoLevelSpeed,
                       mSampleRate * 4.0);
  }

  void updatePitch() {
    for (auto &p : mPitch)
      p.setParameters(mPitchAmount, mPitchSpeed, mSampleRate * 4.0);
  }

  void updateGate() {
    for (auto &g : mGate)
      g.setParameters(mGateThresh, mGateRange, mGateAttack, mGateHold,
                      mGateRelease, mGateHysteresis, mSampleRate * 4.0);
  }

  void updateDeesser() {
    for (auto &ds : mDeesser)
      ds.setParameters(mDeesserThresh, mDeesserFreq, mDeesserRange,
                       mDeesserRatio, mSampleRate * 4.0);
  }

  void updateEQ() {
    // Safety HPF: 20Hz, Q=0.707
    for (auto &eq : mSafetyHPF)
      eq.setParameters(ZDFFilter::HighPass, 20.0, 0.707, 0.0,
                       mSampleRate * 4.0);

    // Band 1: Main HPF (User controls Freq)
    for (auto &eq : mHPF)
      eq.setParameters(ZDFFilter::HighPass, mEQ1Freq, 0.707, 0.0,
                       mSampleRate * 4.0);

    // Band 2: Low Mid Cut (Peaking)
    for (auto &eq : mLowMidCut)
      eq.setParameters(ZDFFilter::Peaking, mEQ2Freq, mEQ2Q, mEQ2Gain,
                       mSampleRate * 4.0);

    // Band 3: High Shelf (Standard Biquad)
    for (auto &eq : mEQBand3)
      eq.calculateCoefficients(BiquadFilter::HighShelf, mEQ3Freq, mEQ3Q,
                               mEQ3Gain, mSampleRate * 4.0);
  }

  void updateFilter() {
    // Main LPF
    // Map Resonance (-20 to 20dB) to Q
    // Q = 0.707 * 10^(db/20)
    double q = 0.707 * pow(10.0, mResonance / 20.0);

    for (auto &f : mLPF)
      f.setParameters(ZDFFilter::LowPass, mCutoff, q, 0.0, mSampleRate * 4.0);
    // Using HighPass for LPF module? Wait. ZDFFilter::HighPass is enum 0.
    // ZDFFilter has HighPass and Peaking.
    // Does it support LowPass?
    // Let's check ZDFFilter class.
  }

  void updateComp() {
    for (auto &c : mCompressor) {
      c.setAutoMakeup(mCompAutoMakeup);
      c.setParameters(mCompThresh, mCompRatio, mCompAttack, mCompRelease,
                      mCompMakeup, mSampleRate * 4.0);
    }
  }

  void updateDelay() {
    for (auto &d : mDelay)
      d.setParameters(mDelayTime, mDelayFeedback, mDelayMix, mSampleRate);
  }

  void updateReverb() {
    for (auto &r : mReverb)
      r.setParameters(mReverbSize, mReverbDamp, mReverbMix, mSampleRate);
  }

  void updateLimiter() {
    for (auto &l : mLimiter)
      l.setParameters(mLimiterCeiling, mLimiterLookahead, 100.0,
                      mSampleRate); // Fixed 100ms release
  }

  void updatePreamp() { mInputGainLin = std::pow(10.0f, mInputGainDb / 20.0f); }

  // MARK: Member Variables
  AUHostMusicalContextBlock mMusicalContextBlock;

  double mSampleRate = 44100.0;
  double mGain = 0.5;
  bool mBypassed = false;

  // Preamp State
  float mInputGainDb = 0.0f;
  float mInputGainLin = 1.0f;
  float mSaturation = 0.0f;
  bool mPhaseInvert = false;

  AUAudioFrameCount mMaxFramesToRender = 1024;
  int mChannelCount = 2;

  // DSP Modules (Vector for multi-channel)
  std::vector<PitchShifter> mPitch;
  std::vector<AutoLevel> mAutoLevel;
  std::vector<NoiseGate> mGate;
  std::vector<Deesser> mDeesser;
  std::vector<ZDFFilter> mSafetyHPF;
  std::vector<ZDFFilter> mHPF;
  std::vector<ZDFFilter> mLowMidCut;
  std::vector<BiquadFilter> mEQBand3;
  std::vector<ZDFFilter> mLPF;
  std::vector<FETCompressor> mCompressor;
  std::vector<Saturator> mSaturator;
  std::vector<DelayLine> mDelay;
  std::vector<FDNReverb> mReverb;
  std::vector<Oversampler> mOversampler;
  std::vector<TruePeakLimiter> mLimiter;

  // Parameter State Cache
  float mPitchAmount = 0;
  float mPitchSpeed = 20;

  float mAutoLevelTarget = -10, mAutoLevelRange = 12, mAutoLevelSpeed = 50;

  float mGateThresh = -40, mGateRange = -20, mGateAttack = 1.0, mGateHold = 150,
        mGateRelease = 300, mGateHysteresis = 6.0;

  float mDeesserThresh = -20, mDeesserFreq = 5000, mDeesserRatio = 5,
        mDeesserRange = -6.0;

  float mEQ1Freq = 100, mEQ1Gain = 0, mEQ1Q = 0.7;
  float mEQ2Freq = 1000, mEQ2Gain = 0, mEQ2Q = 0.7;
  float mEQ3Freq = 5000, mEQ3Gain = 0, mEQ3Q = 0.7;

  float mCompThresh = -20, mCompRatio = 2.0, mCompAttack = 10,
        mCompRelease = 100, mCompMakeup = 0;
  bool mCompAutoMakeup = false;

  float mSatDrive = 0, mSatType = 0;

  float mDelayTime = 0.5, mDelayFeedback = 20, mDelayMix = 0;

  float mReverbSize = 0.5f, mReverbDamp = 0.5f, mReverbMix = 0.0f;
  float mLimiterCeiling = -0.1f, mLimiterLookahead = 2.0f;

  float mCutoff = 20000.0f;
  float mResonance = 0.0f;

  std::vector<float> mScratchBuffer;
};
