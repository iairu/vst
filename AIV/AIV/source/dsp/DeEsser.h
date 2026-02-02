//------------------------------------------------------------------------
// Copyright(c) 2026 AIV.
//------------------------------------------------------------------------

#pragma once

#include <algorithm>
#include <cmath>

namespace AIV {
namespace DSP {

//------------------------------------------------------------------------
// DeEsser - Frequency-selective compressor for sibilance
//------------------------------------------------------------------------
class DeEsser {
public:
  void reset(double sampleRate) {
    mSampleRate = sampleRate;
    mBandEnvelope = 0.0;
    mGain = 1.0;
    // Reset filter states
    mBandpassState[0] = mBandpassState[1] = mBandpassState[2] =
        mBandpassState[3] = 0.0;
  }

  void setParameters(float freq, float threshold, float range) {
    // freq: 0-1 maps to 2kHz to 12kHz
    mCenterFreq = 2000.0 + freq * 10000.0;
    updateFilterCoeffs();

    // threshold: 0-1 maps to -40dB to 0dB
    mThresholdLin = std::pow(10.0, (threshold * 40.0 - 40.0) / 20.0);

    // range: 0-1 maps to 0dB to -24dB
    mMaxReduction = range;
  }

  void process(float *left, float *right, int numSamples) {
    for (int i = 0; i < numSamples; ++i) {
      // Mono sum for detection
      double mono = (left[i] + right[i]) * 0.5;

      // Bandpass filter for sibilance detection
      double band = applyBandpass(mono);
      double bandLevel = std::fabs(band);

      // Envelope follower
      double attackCoeff = 0.001;
      double releaseCoeff = 0.0001;
      if (bandLevel > mBandEnvelope)
        mBandEnvelope =
            attackCoeff * mBandEnvelope + (1.0 - attackCoeff) * bandLevel;
      else
        mBandEnvelope =
            releaseCoeff * mBandEnvelope + (1.0 - releaseCoeff) * bandLevel;

      // Compute gain reduction
      double targetGain = 1.0;
      if (mBandEnvelope > mThresholdLin) {
        double overDb = 20.0 * std::log10(mBandEnvelope / mThresholdLin);
        double reductionDb = std::min(overDb * 0.8, mMaxReduction * 24.0);
        targetGain = std::pow(10.0, -reductionDb / 20.0);
      }

      // Smooth gain
      mGain = 0.99 * mGain + 0.01 * targetGain;

      // Apply wideband gain reduction
      left[i] *= static_cast<float>(mGain);
      right[i] *= static_cast<float>(mGain);
    }
  }

private:
  void updateFilterCoeffs() {
    // 2nd order bandpass filter coefficients
    double omega = 2.0 * M_PI * mCenterFreq / mSampleRate;
    double Q = 2.0; // Bandwidth
    double alpha = std::sin(omega) / (2.0 * Q);

    double a0 = 1.0 + alpha;
    mB[0] = alpha / a0;
    mB[1] = 0.0;
    mB[2] = -alpha / a0;
    mA[1] = -2.0 * std::cos(omega) / a0;
    mA[2] = (1.0 - alpha) / a0;
  }

  double applyBandpass(double input) {
    double output = mB[0] * input + mB[1] * mBandpassState[0] +
                    mB[2] * mBandpassState[1] - mA[1] * mBandpassState[2] -
                    mA[2] * mBandpassState[3];

    mBandpassState[1] = mBandpassState[0];
    mBandpassState[0] = input;
    mBandpassState[3] = mBandpassState[2];
    mBandpassState[2] = output;

    return output;
  }

  double mSampleRate = 44100.0;
  double mCenterFreq = 6000.0;
  double mThresholdLin = 0.1;
  double mMaxReduction = 0.5;
  double mBandEnvelope = 0.0;
  double mGain = 1.0;

  double mB[3] = {0.0, 0.0, 0.0};
  double mA[3] = {1.0, 0.0, 0.0};
  double mBandpassState[4] = {0.0, 0.0, 0.0, 0.0};
};

//------------------------------------------------------------------------
} // namespace DSP
} // namespace AIV
