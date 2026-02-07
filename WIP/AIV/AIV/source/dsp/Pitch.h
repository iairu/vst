//------------------------------------------------------------------------
// Copyright(c) 2026 AIV.
//------------------------------------------------------------------------

#pragma once

#include <algorithm>
#include <cmath>
#include <vector>

namespace AIV {
namespace DSP {

//------------------------------------------------------------------------
// Pitch - Simple pitch correction (chromatic scale)
// Note: Full pitch correction is complex; this provides basic functionality
//------------------------------------------------------------------------
class Pitch {
public:
  void reset(double sampleRate) {
    mSampleRate = sampleRate;
    mBuffer.resize(static_cast<size_t>(sampleRate * 0.1)); // 100ms buffer
    std::fill(mBuffer.begin(), mBuffer.end(), 0.0f);
    mWritePos = 0;
    mPhaseL = mPhaseR = 0.0;
  }

  void setParameters(float speed, float amount) {
    // speed: 0-1 maps to very fast (0) to very slow (1) correction
    mCorrectionSpeed = 0.01 + (1.0 - speed) * 0.99;

    // amount: 0-1 how much correction to apply
    mAmount = amount;
  }

  void process(float *left, float *right, int numSamples) {
    // Simplified pitch processing - subtle pitch smoothing
    // Full pitch correction would require FFT-based analysis

    double smoothing = 0.995 - mCorrectionSpeed * 0.01;

    for (int i = 0; i < numSamples; ++i) {
      // Simple smoothing-based "pitch stability" (not true pitch correction)
      double dry = (left[i] + right[i]) * 0.5;

      // Low-pass the pitch variations
      mSmoothed = smoothing * mSmoothed + (1.0 - smoothing) * dry;

      // Mix processed with original based on amount
      double delta = (mSmoothed - dry) * mAmount * 0.5;
      left[i] += static_cast<float>(delta);
      right[i] += static_cast<float>(delta);
    }
  }

private:
  double mSampleRate = 44100.0;
  std::vector<float> mBuffer;
  size_t mWritePos = 0;
  double mPhaseL = 0.0;
  double mPhaseR = 0.0;
  double mCorrectionSpeed = 0.5;
  double mAmount = 0.5;
  double mSmoothed = 0.0;
};

//------------------------------------------------------------------------
} // namespace DSP
} // namespace AIV
