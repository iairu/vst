//------------------------------------------------------------------------
// Copyright(c) 2026 AIV.
//------------------------------------------------------------------------

#pragma once

#include <algorithm>
#include <cmath>

namespace AIV {
namespace DSP {

//------------------------------------------------------------------------
// StereoWidth - Mid-side based stereo widening
//------------------------------------------------------------------------
class StereoWidth {
public:
  void reset(double sampleRate) {
    mSampleRate = sampleRate;
    mLowpassState = 0.0;
  }

  void setParameters(float width, float monoFreq) {
    // width: 0-1 maps to 0% (mono) to 200% (extra wide)
    mWidth = width * 2.0;

    // monoFreq: frequency below which signal is summed to mono
    // 0-1 maps to 0Hz to 400Hz
    double freq = monoFreq * 400.0;
    mLowpassCoeff = std::exp(-2.0 * M_PI * freq / mSampleRate);
  }

  void process(float *left, float *right, int numSamples) {
    for (int i = 0; i < numSamples; ++i) {
      double l = left[i];
      double r = right[i];

      // Convert to mid-side
      double mid = (l + r) * 0.5;
      double side = (l - r) * 0.5;

      // Apply width to side signal
      side *= mWidth;

      // Optionally mono bass
      if (mLowpassCoeff > 0.0) {
        // Extract low frequencies
        mLowpassState =
            mLowpassCoeff * mLowpassState + (1.0 - mLowpassCoeff) * mid;
        double highMid = mid - mLowpassState;
        double highSide =
            side; // Side doesn't need filtering if we're just mono-ing bass

        // Zero out low frequency side component
        double lowSide = 0.0;

        // Reconstruct
        mid = mLowpassState + highMid;
        side = lowSide + highSide * (1.0 - mLowpassCoeff * 0.5);
      }

      // Convert back to L-R
      left[i] = static_cast<float>(mid + side);
      right[i] = static_cast<float>(mid - side);
    }
  }

private:
  double mSampleRate = 44100.0;
  double mWidth = 1.0;
  double mLowpassCoeff = 0.0;
  double mLowpassState = 0.0;
};

//------------------------------------------------------------------------
} // namespace DSP
} // namespace AIV
