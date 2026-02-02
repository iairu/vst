//------------------------------------------------------------------------
// Copyright(c) 2026 AIV.
//------------------------------------------------------------------------

#pragma once

#include <algorithm>
#include <cmath>

namespace AIV {
namespace DSP {

//------------------------------------------------------------------------
// Compressor - Soft-knee compressor with makeup gain
//------------------------------------------------------------------------
class Compressor {
public:
  void reset(double sampleRate) {
    mSampleRate = sampleRate;
    mEnvelope = 0.0;
  }

  void setParameters(float threshold, float ratio, float attack, float release,
                     float makeup, float knee) {
    // threshold: 0-1 maps to -48dB to 0dB
    mThresholdDb = threshold * 48.0 - 48.0;
    mThresholdLin = std::pow(10.0, mThresholdDb / 20.0);

    // ratio: 0-1 maps to 1:1 to 20:1
    mRatio = 1.0 + ratio * 19.0;

    // attack: 0-1 maps to 0.1ms to 100ms
    double attackMs = 0.1 + attack * 99.9;
    mAttackCoeff = std::exp(-1.0 / (mSampleRate * attackMs / 1000.0));

    // release: 0-1 maps to 10ms to 1000ms
    double releaseMs = 10.0 + release * 990.0;
    mReleaseCoeff = std::exp(-1.0 / (mSampleRate * releaseMs / 1000.0));

    // makeup: 0-1 maps to -12dB to +24dB
    double makeupDb = makeup * 36.0 - 12.0;
    mMakeupLin = std::pow(10.0, makeupDb / 20.0);

    // knee: 0-1 maps to 0dB to 12dB
    mKneeDb = knee * 12.0;
  }

  void process(float *left, float *right, int numSamples) {
    for (int i = 0; i < numSamples; ++i) {
      // Detect level (peak to dB)
      double input = std::max(std::fabs(left[i]), std::fabs(right[i]));
      double inputDb = (input > 1e-6) ? 20.0 * std::log10(input) : -120.0;

      // Compute gain reduction with soft knee
      double gainReductionDb = computeGainReduction(inputDb);

      // Envelope follower for gain
      double targetEnv = std::pow(10.0, gainReductionDb / 20.0);
      if (targetEnv < mEnvelope)
        mEnvelope = mAttackCoeff * mEnvelope + (1.0 - mAttackCoeff) * targetEnv;
      else
        mEnvelope =
            mReleaseCoeff * mEnvelope + (1.0 - mReleaseCoeff) * targetEnv;

      // Apply gain
      float gain = static_cast<float>(mEnvelope * mMakeupLin);
      left[i] *= gain;
      right[i] *= gain;
    }
  }

  double getGainReduction() const {
    return 20.0 * std::log10(mEnvelope + 1e-6);
  }

private:
  double computeGainReduction(double inputDb) {
    double overshoot = inputDb - mThresholdDb;

    if (mKneeDb > 0.0 && overshoot > -mKneeDb / 2.0 &&
        overshoot < mKneeDb / 2.0) {
      // Soft knee region
      double x = overshoot + mKneeDb / 2.0;
      double compressionDb = (1.0 / mRatio - 1.0) * x * x / (2.0 * mKneeDb);
      return compressionDb;
    } else if (overshoot > 0.0) {
      // Above threshold
      return overshoot * (1.0 / mRatio - 1.0);
    }

    return 0.0; // Below threshold
  }

  double mSampleRate = 44100.0;
  double mThresholdDb = -12.0;
  double mThresholdLin = 0.25;
  double mRatio = 4.0;
  double mAttackCoeff = 0.9;
  double mReleaseCoeff = 0.9999;
  double mMakeupLin = 1.0;
  double mKneeDb = 3.0;
  double mEnvelope = 1.0;
};

//------------------------------------------------------------------------
} // namespace DSP
} // namespace AIV
