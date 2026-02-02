//------------------------------------------------------------------------
// Copyright(c) 2026 AIV.
//------------------------------------------------------------------------

#pragma once

#include <algorithm>
#include <cmath>

namespace AIV {
namespace DSP {

//------------------------------------------------------------------------
// Gate - Noise gate with smooth envelope
//------------------------------------------------------------------------
class Gate {
public:
  void reset(double sampleRate) {
    mSampleRate = sampleRate;
    mEnvelope = 0.0;
    mGain = 1.0;
    mHoldCounter = 0;
  }

  void setParameters(float threshold, float attack, float hold, float release,
                     float range) {
    // threshold: 0-1 maps to -60dB to 0dB
    mThresholdLin = std::pow(10.0, (threshold * 60.0 - 60.0) / 20.0);

    // attack: 0-1 maps to 0.1ms to 100ms
    double attackMs = 0.1 + attack * 99.9;
    mAttackCoeff = std::exp(-1.0 / (mSampleRate * attackMs / 1000.0));

    // hold: 0-1 maps to 0 to 500ms
    mHoldSamples = static_cast<int>(hold * 500.0 * mSampleRate / 1000.0);

    // release: 0-1 maps to 10ms to 1000ms
    double releaseMs = 10.0 + release * 990.0;
    mReleaseCoeff = std::exp(-1.0 / (mSampleRate * releaseMs / 1000.0));

    // range: 0-1 maps to -80dB to 0dB attenuation
    mRangeLin = std::pow(10.0, (range * -80.0) / 20.0);
  }

  void process(float *left, float *right, int numSamples) {
    for (int i = 0; i < numSamples; ++i) {
      // Detect level (peak)
      double input = std::max(std::fabs(left[i]), std::fabs(right[i]));

      // Envelope follower
      if (input > mEnvelope)
        mEnvelope = mAttackCoeff * mEnvelope + (1.0 - mAttackCoeff) * input;
      else
        mEnvelope = mReleaseCoeff * mEnvelope;

      // Gate logic
      if (mEnvelope >= mThresholdLin) {
        mHoldCounter = mHoldSamples;
        mTargetGain = 1.0;
      } else if (mHoldCounter > 0) {
        mHoldCounter--;
        mTargetGain = 1.0;
      } else {
        mTargetGain = mRangeLin;
      }

      // Smooth gain transition
      mGain = 0.999 * mGain + 0.001 * mTargetGain;

      left[i] *= static_cast<float>(mGain);
      right[i] *= static_cast<float>(mGain);
    }
  }

private:
  double mSampleRate = 44100.0;
  double mThresholdLin = 0.01;
  double mAttackCoeff = 0.9;
  double mReleaseCoeff = 0.9999;
  double mRangeLin = 0.0;
  int mHoldSamples = 0;
  int mHoldCounter = 0;
  double mEnvelope = 0.0;
  double mGain = 1.0;
  double mTargetGain = 1.0;
};

//------------------------------------------------------------------------
} // namespace DSP
} // namespace AIV
