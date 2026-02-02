//------------------------------------------------------------------------
// Copyright(c) 2026 AIV.
//------------------------------------------------------------------------

#pragma once

#include <algorithm>
#include <cmath>

namespace AIV {
namespace DSP {

//------------------------------------------------------------------------
// AutoLevel - Automatic gain riding for consistent vocal levels
//------------------------------------------------------------------------
class AutoLevel {
public:
  void reset(double sampleRate) {
    mSampleRate = sampleRate;
    mEnvelope = 0.0;
    mCurrentGain = 1.0;
  }

  void setParameters(float target, float speed) {
    // target: 0-1 maps to -24dB to 0dB target level
    mTargetLevel = std::pow(10.0, (target * 24.0 - 24.0) / 20.0);

    // speed: 0-1 maps to slow (500ms) to fast (50ms) response
    double responseMs = 500.0 - speed * 450.0;
    mAttackCoeff = std::exp(-1.0 / (mSampleRate * responseMs / 1000.0));
    mReleaseCoeff = std::exp(
        -1.0 / (mSampleRate * responseMs * 2.0 / 1000.0)); // Release 2x slower
  }

  void process(float *left, float *right, int numSamples) {
    for (int i = 0; i < numSamples; ++i) {
      // Peak detection
      double input = std::max(std::fabs(left[i]), std::fabs(right[i]));

      // RMS-like envelope (slower response)
      if (input > mEnvelope)
        mEnvelope = mAttackCoeff * mEnvelope + (1.0 - mAttackCoeff) * input;
      else
        mEnvelope = mReleaseCoeff * mEnvelope + (1.0 - mReleaseCoeff) * input;

      // Calculate desired gain
      double targetGain = 1.0;
      if (mEnvelope > 1e-6) {
        targetGain = mTargetLevel / mEnvelope;
        // Limit gain range to avoid extreme values
        targetGain = std::clamp(targetGain, 0.1, 10.0);
      }

      // Smooth gain changes
      mCurrentGain = 0.9999 * mCurrentGain + 0.0001 * targetGain;

      // Apply gain
      float gain = static_cast<float>(mCurrentGain);
      left[i] *= gain;
      right[i] *= gain;
    }
  }

private:
  double mSampleRate = 44100.0;
  double mTargetLevel = 0.5;
  double mAttackCoeff = 0.99;
  double mReleaseCoeff = 0.999;
  double mEnvelope = 0.0;
  double mCurrentGain = 1.0;
};

//------------------------------------------------------------------------
} // namespace DSP
} // namespace AIV
