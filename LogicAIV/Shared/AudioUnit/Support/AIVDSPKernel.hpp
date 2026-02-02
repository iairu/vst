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
    mPitch.resize(mChannelCount);
    mEQBand1.resize(mChannelCount);
    mEQBand2.resize(mChannelCount);
    mEQBand3.resize(mChannelCount);
    mCompressor.resize(mChannelCount);
    mSaturator.resize(mChannelCount);
    mDelay.resize(mChannelCount);
    mReverb.resize(mChannelCount);

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

    // Pitch
    case AIVParameterAddressPitchAmount:
      mPitchAmount = value;
      for (auto &p : mPitch)
        p.setParameters(mPitchAmount, mSampleRate);
      break;
    case AIVParameterAddressPitchSpeed:
      mPitchSpeed = value;
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

        // 1. Pitch
        sample = mPitch[channel].process(sample);

        // 2. EQ
        sample = mEQBand1[channel].process(sample);
        sample = mEQBand2[channel].process(sample);
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
  void updateEQ() {
    for (auto &eq : mEQBand1)
      eq.calculateCoefficients(BiquadFilter::LowPass, mEQ1Freq, mEQ1Q, mEQ1Gain,
                               mSampleRate);
    for (auto &eq : mEQBand2)
      eq.calculateCoefficients(BiquadFilter::Peaking, mEQ2Freq, mEQ2Q, mEQ2Gain,
                               mSampleRate);
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

  // MARK: Member Variables
  AUHostMusicalContextBlock mMusicalContextBlock;

  double mSampleRate = 44100.0;
  double mGain = 0.5;
  bool mBypassed = false;
  AUAudioFrameCount mMaxFramesToRender = 1024;
  int mChannelCount = 2;

  // DSP Modules (Vector for multi-channel)
  std::vector<PitchShifter> mPitch;
  std::vector<BiquadFilter> mEQBand1, mEQBand2, mEQBand3;
  std::vector<SimpleCompressor> mCompressor;
  std::vector<Saturator> mSaturator;
  std::vector<DelayLine> mDelay;
  std::vector<SchroederReverb> mReverb;

  // Parameter State Cache
  float mPitchAmount = 0;
  float mPitchSpeed = 20;

  float mEQ1Freq = 100, mEQ1Gain = 0, mEQ1Q = 0.7;
  float mEQ2Freq = 1000, mEQ2Gain = 0, mEQ2Q = 0.7;
  float mEQ3Freq = 5000, mEQ3Gain = 0, mEQ3Q = 0.7;

  float mCompThresh = -20, mCompRatio = 2.0, mCompAttack = 10,
        mCompRelease = 100, mCompMakeup = 0;

  float mSatDrive = 0, mSatType = 0;

  float mDelayTime = 0.5, mDelayFeedback = 20, mDelayMix = 0;

  float mReverbSize = 0.5f, mReverbDamp = 0.5f, mReverbMix = 0.0f;
};
