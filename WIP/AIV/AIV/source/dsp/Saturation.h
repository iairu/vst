//------------------------------------------------------------------------
// Copyright(c) 2026 AIV.
//------------------------------------------------------------------------

#pragma once

#include <algorithm>
#include <cmath>

namespace AIV {
namespace DSP {

//------------------------------------------------------------------------
// Saturation - Tube/tape-style harmonic saturation
//------------------------------------------------------------------------
class Saturation {
public:
  void reset(double /*sampleRate*/) {
    // Stateless processing
  }

  void setParameters(float drive, float mix, float warmth) {
    // drive: 0-1 maps to 1x to 10x pregain
    mDrive = 1.0 + drive * 9.0;

    // mix: 0-1 as wet/dry
    mMix = mix;

    // warmth: 0-1 adds subtle low-end boost and high roll-off
    mWarmth = warmth;
  }

  void process(float *left, float *right, int numSamples) {
    for (int i = 0; i < numSamples; ++i) {
      double dryL = left[i];
      double dryR = right[i];

      // Apply drive
      double wetL = dryL * mDrive;
      double wetR = dryR * mDrive;

      // Soft clipping waveshaper (tanh-based tube simulation)
      wetL = std::tanh(wetL);
      wetR = std::tanh(wetR);

      // Add warmth (subtle even harmonics)
      if (mWarmth > 0.0) {
        wetL = warmthProcess(wetL);
        wetR = warmthProcess(wetR);
      }

      // Compensate for volume increase from saturation
      double compensation = 1.0 / (0.5 + 0.5 * mDrive / 10.0);
      wetL *= compensation;
      wetR *= compensation;

      // Mix
      left[i] = static_cast<float>(dryL * (1.0 - mMix) + wetL * mMix);
      right[i] = static_cast<float>(dryR * (1.0 - mMix) + wetR * mMix);
    }
  }

private:
  double warmthProcess(double x) {
    // Asymmetric waveshaping for even harmonics
    double asymmetry = 0.1 * mWarmth;
    if (x > 0)
      return x * (1.0 + asymmetry * x);
    else
      return x * (1.0 - asymmetry * x);
  }

  double mDrive = 1.0;
  double mMix = 0.5;
  double mWarmth = 0.5;
};

//------------------------------------------------------------------------
} // namespace DSP
} // namespace AIV
