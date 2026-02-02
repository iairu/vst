//------------------------------------------------------------------------
// Copyright(c) 2026 AIV.
//------------------------------------------------------------------------

#pragma once

#include <algorithm>
#include <cmath>

namespace AIV {
namespace DSP {

//------------------------------------------------------------------------
// BreathControl - Detect and attenuate breath sounds
//------------------------------------------------------------------------
class BreathControl {
public:
  void reset(double sampleRate) {
    mSampleRate = sampleRate;
    mLowEnvelope = 0.0;
    mHighEnvelope = 0.0;
    mGain = 1.0;
    mLowpassState = 0.0;
    mHighpassState = 0.0;
  }

  void setParameters(float sensitivity, float reduction) {
    // sensitivity: 0-1, how easily breaths are detected
    mSensitivity = 0.1 + sensitivity * 0.9;

    // reduction: 0-1 maps to 0dB to -24dB
    mReductionLin = std::pow(10.0, (reduction * -24.0) / 20.0);
  }

  void process(float *left, float *right, int numSamples) {
    // Breath detection: breaths have high frequency energy, low low-frequency
    // energy and relatively steady amplitude (not transient)

    double lpCoeff = std::exp(-2.0 * M_PI * 500.0 / mSampleRate);
    double hpCoeff = std::exp(-2.0 * M_PI * 2000.0 / mSampleRate);

    for (int i = 0; i < numSamples; ++i) {
      double mono = (left[i] + right[i]) * 0.5;
      double absMono = std::fabs(mono);

      // Estimate low frequency content
      mLowpassState = lpCoeff * mLowpassState + (1.0 - lpCoeff) * mono;
      double lowFreq = std::fabs(mLowpassState);

      // Estimate high frequency content
      mHighpassState = hpCoeff * mHighpassState + (1.0 - hpCoeff) * mono;
      double highFreq = std::fabs(mono - mHighpassState);

      // Envelope followers
      double envCoeff = 0.999;
      mLowEnvelope = envCoeff * mLowEnvelope + (1.0 - envCoeff) * lowFreq;
      mHighEnvelope = envCoeff * mHighEnvelope + (1.0 - envCoeff) * highFreq;

      // Breath detection: high ratio of high-to-low frequency content
      double ratio = (mHighEnvelope + 1e-6) / (mLowEnvelope + 1e-6);

      // If ratio is high (breath-like) and level is moderate, apply reduction
      double threshold = 3.0 / mSensitivity;
      double targetGain = 1.0;

      if (ratio > threshold && absMono > 0.01 && absMono < 0.3) {
        // Blend towards reduction based on how breath-like it is
        double breathAmount = std::min((ratio - threshold) / threshold, 1.0);
        targetGain = 1.0 - (1.0 - mReductionLin) * breathAmount;
      }

      // Smooth gain
      mGain = 0.999 * mGain + 0.001 * targetGain;

      left[i] *= static_cast<float>(mGain);
      right[i] *= static_cast<float>(mGain);
    }
  }

private:
  double mSampleRate = 44100.0;
  double mSensitivity = 0.5;
  double mReductionLin = 0.5;
  double mLowEnvelope = 0.0;
  double mHighEnvelope = 0.0;
  double mGain = 1.0;
  double mLowpassState = 0.0;
  double mHighpassState = 0.0;
};

//------------------------------------------------------------------------
} // namespace DSP
} // namespace AIV
