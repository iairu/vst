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
    mPitch.resize(mChannelCount);
    mDeesser.resize(mChannelCount);
    mSafetyHPF.resize(mChannelCount);
    mHPF.resize(mChannelCount);
    mLowMidCut.resize(mChannelCount);
    // mEQBand3 remains Biquad HighShelf
    mEQBand3.resize(mChannelCount);
    mCompressor.resize(mChannelCount);
    mSaturator.resize(mChannelCount);
    mDelay.resize(mChannelCount);
    mReverb.resize(mChannelCount);

    updateAutoLevel();
    updateDeesser();
    updateEQ();
    updateComp();
    updateDelay();
    updateReverb();
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
      for (auto &p : mPitch)
        p.setParameters(mPitchAmount, mSampleRate);
      break;
    case AIVParameterAddressPitchSpeed:
      mPitchSpeed = value;
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

    default:
      return 0.f;
    }
  }

  // MARK: - Max Frames
  AUAudioFrameCount maximumFramesToRender() const { return mMaxFramesToRender; }

  void setMaximumFramesToRender(const AUAudioFrameCount &maxFrames) {
    mMaxFramesToRender = maxFrames;
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

        // 0. Preamp & Saturation (Step 1.1)
        /*
         Research Note (Section 2.1):
         To model the "warmth" associated with transformer-based preamps, we
         utilize a continuous, non-linear transfer function. A common mistake is
         using hard clipping.

         Correct Formula: Hyperbolic Tangent (Soft Saturation)
         f(x) = tanh(k * x) / tanh(k)
         Where k is the drive coefficient derived from the Input Gain. As x
         increases, predominantly odd-order harmonics (3rd, 5th, 7th) are
         generated.
         */
        float inputSample = sample;

        // Research Note (Section 1.1):
        // Correct Decibel Formula: 20 * log10(V / Vref)
        // We convert the dB parameter to linear voltage ratio here.
        float linearSample = inputSample * mInputGainLin;

        // Physics: y = tanh(k * x) / tanh(k)
        // k is derived from input gain.
        // We use linearSample as 'x' logic for Drive, but use formula for
        // Shape. Actually, if k=gain, then tanh(k * x_raw) / tanh(k) is the
        // formula. Let's use the robust blend interpretation:

        // Wet Path (Saturated/Limited)
        // Protect against k=0 (though min gain is usually handled)
        float k = (mInputGainLin < 0.01f) ? 0.01f : mInputGainLin;
        float wetSample = std::tanh(k * inputSample) / std::tanh(k);

        // Dry Path (Amplified Linear)
        float drySample = linearSample;

        // Mix: Saturation 0% = Dry (Amplified), 100% = Wet (Limited/Saturated)
        float mix = mSaturation / 100.0f;
        sample = (1.0f - mix) * drySample + mix * wetSample;

        // Phase Invert
        if (mPhaseInvert) {
          sample = -sample;
        }

        // 1. Auto Level
        sample = mAutoLevel[channel].process(sample);

        // 2. Pitch
        sample = mPitch[channel].process(sample);

        // 2. Deesser
        sample = mDeesser[channel].process(sample);

        // 3. EQ (ZDF + Biquad)
        /*
         Research Note (Section 4.1):
         Standard Biquads suffer from amplitude and phase distortion (cramping)
         near Nyquist. We use Zero Delay Feedback (TPT) topology with
         Trapezoidal Integration. g = tan(pi * fc / fs) This preserves the
         analog amplitude and phase response perfectly.
         */
        // Safety HPF (Fixed 20Hz)
        sample = mSafetyHPF[channel].process(sample);
        // Band 1: HPF (ZDF)
        sample = mHPF[channel].process(sample);
        // Band 2: Low Mid Cut (ZDF Peaking)
        sample = mLowMidCut[channel].process(sample);
        // Band 3: High Shelf (Biquad)
        sample = mEQBand3[channel].process(sample);

        // 3. Compressor
        sample = mCompressor[channel].process(sample);

        // 4. Saturation
        sample = mSaturator[channel].process(sample);

        // 5. Delay
        sample = mDelay[channel].process(sample);

        // 6. Reverb
        sample = mReverb[channel].process(sample);

        // 7. Global Gain
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

private:
  void updateAutoLevel() {
    for (auto &al : mAutoLevel)
      al.setParameters(mAutoLevelTarget, mAutoLevelRange, mAutoLevelSpeed,
                       mSampleRate);
  }

  void updateDeesser() {
    for (auto &ds : mDeesser)
      ds.setParameters(mDeesserThresh, mDeesserFreq, mDeesserRatio,
                       mSampleRate);
  }

  void updateEQ() {
    // Safety HPF: 20Hz, Q=0.707
    for (auto &eq : mSafetyHPF)
      eq.setParameters(ZDFFilter::HighPass, 20.0, 0.707, 0.0, mSampleRate);

    // Band 1: Main HPF (User controls Freq)
    // Q fixed at 0.707 or user? Prompt says 12dB/oct, implied Q=0.707
    // (Butterworth)
    for (auto &eq : mHPF)
      eq.setParameters(ZDFFilter::HighPass, mEQ1Freq, 0.707, 0.0,
                       mSampleRate); // Gain irrelevant for HPF

    // Band 2: Low Mid Cut (Peaking)
    // "Low-Mid Gain ... -12 dB to 0 dB"
    for (auto &eq : mLowMidCut)
      eq.setParameters(ZDFFilter::Peaking, mEQ2Freq, mEQ2Q, mEQ2Gain,
                       mSampleRate);

    // Band 3: High Shelf (Standard Biquad)
    for (auto &eq : mEQBand3)
      eq.calculateCoefficients(BiquadFilter::HighShelf, mEQ3Freq, mEQ3Q,
                               mEQ3Gain, mSampleRate);
  }

  void updateComp() {
    for (auto &c : mCompressor)
      c.setParameters(mCompThresh, mCompRatio, mCompAttack, mCompRelease,
                      mCompMakeup, mSampleRate);
  }

  void updateDelay() {
    for (auto &d : mDelay)
      d.setParameters(mDelayTime, mDelayFeedback, mDelayMix, mSampleRate);
  }

  void updateReverb() {
    for (auto &r : mReverb)
      r.setParameters(mReverbSize, mReverbDamp, mReverbMix, mSampleRate);
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
  std::vector<Deesser> mDeesser;
  std::vector<ZDFFilter> mSafetyHPF;
  std::vector<ZDFFilter> mHPF;
  std::vector<ZDFFilter> mLowMidCut;
  std::vector<BiquadFilter> mEQBand3;
  std::vector<SimpleCompressor> mCompressor;
  std::vector<Saturator> mSaturator;
  std::vector<DelayLine> mDelay;
  std::vector<SchroederReverb> mReverb;

  // Parameter State Cache
  float mPitchAmount = 0;
  float mPitchSpeed = 20;

  float mAutoLevelTarget = -10, mAutoLevelRange = 12, mAutoLevelSpeed = 50;
  float mDeesserThresh = -20, mDeesserFreq = 5000, mDeesserRatio = 5;

  float mEQ1Freq = 100, mEQ1Gain = 0, mEQ1Q = 0.7;
  float mEQ2Freq = 1000, mEQ2Gain = 0, mEQ2Q = 0.7;
  float mEQ3Freq = 5000, mEQ3Gain = 0, mEQ3Q = 0.7;

  float mCompThresh = -20, mCompRatio = 2.0, mCompAttack = 10,
        mCompRelease = 100, mCompMakeup = 0;

  float mSatDrive = 0, mSatType = 0;

  float mDelayTime = 0.5, mDelayFeedback = 20, mDelayMix = 0;

  float mReverbSize = 0.5f, mReverbDamp = 0.5f, mReverbMix = 0.0f;
};
