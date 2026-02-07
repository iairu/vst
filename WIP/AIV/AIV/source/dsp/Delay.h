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
// Delay - Stereo delay with feedback and filtering
//------------------------------------------------------------------------
class Delay {
public:
  void reset(double sampleRate) {
    mSampleRate = sampleRate;
    int maxDelaySamples = static_cast<int>(sampleRate * 2.0); // 2 seconds max
    mBufferL.resize(static_cast<size_t>(maxDelaySamples));
    mBufferR.resize(static_cast<size_t>(maxDelaySamples));
    std::fill(mBufferL.begin(), mBufferL.end(), 0.0f);
    std::fill(mBufferR.begin(), mBufferR.end(), 0.0f);
    mWritePos = 0;
    mFilterStateL = mFilterStateR = 0.0;
  }

  void setParameters(float timeL, float timeR, float feedback, float mix,
                     float /*sync*/, float highpass, float lowpass) {
    // time: 0-1 maps to 0ms to 1000ms
    mDelaySamplesL = static_cast<int>(timeL * mSampleRate);
    mDelaySamplesR = static_cast<int>(timeR * mSampleRate);

    // feedback: 0-1 maps to 0% to 95%
    mFeedback = feedback * 0.95;

    // mix: wet/dry
    mMix = mix;

    // Filter coefficients (simple 1-pole)
    double hpFreq = 20.0 + highpass * 980.0; // 20Hz to 1kHz
    mHighpassCoeff = std::exp(-2.0 * M_PI * hpFreq / mSampleRate);

    double lpFreq = 1000.0 + (1.0 - lowpass) * 19000.0; // 1kHz to 20kHz
    mLowpassCoeff = std::exp(-2.0 * M_PI * lpFreq / mSampleRate);
  }

  void process(float *left, float *right, int numSamples) {
    int bufferSize = static_cast<int>(mBufferL.size());

    for (int i = 0; i < numSamples; ++i) {
      // Read from delay buffer
      int readPosL = (mWritePos - mDelaySamplesL + bufferSize) % bufferSize;
      int readPosR = (mWritePos - mDelaySamplesR + bufferSize) % bufferSize;

      double delayedL = mBufferL[static_cast<size_t>(readPosL)];
      double delayedR = mBufferR[static_cast<size_t>(readPosR)];

      // Simple lowpass on delayed signal
      mFilterStateL =
          mLowpassCoeff * mFilterStateL + (1.0 - mLowpassCoeff) * delayedL;
      mFilterStateR =
          mLowpassCoeff * mFilterStateR + (1.0 - mLowpassCoeff) * delayedR;

      // Write to buffer with feedback
      mBufferL[static_cast<size_t>(mWritePos)] =
          static_cast<float>(left[i] + mFilterStateL * mFeedback);
      mBufferR[static_cast<size_t>(mWritePos)] =
          static_cast<float>(right[i] + mFilterStateR * mFeedback);

      // Mix output
      left[i] = static_cast<float>(left[i] * (1.0 - mMix) + delayedL * mMix);
      right[i] = static_cast<float>(right[i] * (1.0 - mMix) + delayedR * mMix);

      mWritePos = (mWritePos + 1) % bufferSize;
    }
  }

private:
  double mSampleRate = 44100.0;
  std::vector<float> mBufferL;
  std::vector<float> mBufferR;
  int mWritePos = 0;
  int mDelaySamplesL = 0;
  int mDelaySamplesR = 0;
  double mFeedback = 0.3;
  double mMix = 0.3;
  double mHighpassCoeff = 0.99;
  double mLowpassCoeff = 0.1;
  double mFilterStateL = 0.0;
  double mFilterStateR = 0.0;
};

//------------------------------------------------------------------------
} // namespace DSP
} // namespace AIV
