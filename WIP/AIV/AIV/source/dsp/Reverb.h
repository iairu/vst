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
// Reverb - Freeverb-inspired algorithm
//------------------------------------------------------------------------
class Reverb {
public:
  void reset(double sampleRate) {
    mSampleRate = sampleRate;

    // Comb filter delay lengths (tuned for ~44.1kHz, scaled)
    double scale = sampleRate / 44100.0;
    int combLengths[8] = {1116, 1188, 1277, 1356, 1422, 1491, 1557, 1617};
    int allpassLengths[4] = {556, 441, 341, 225};

    for (int i = 0; i < 8; ++i) {
      mCombBuffers[i].resize(static_cast<size_t>(combLengths[i] * scale + 0.5));
      std::fill(mCombBuffers[i].begin(), mCombBuffers[i].end(), 0.0f);
      mCombPos[i] = 0;
      mCombFilterStore[i] = 0.0;
    }

    for (int i = 0; i < 4; ++i) {
      mAllpassBuffers[i].resize(
          static_cast<size_t>(allpassLengths[i] * scale + 0.5));
      std::fill(mAllpassBuffers[i].begin(), mAllpassBuffers[i].end(), 0.0f);
      mAllpassPos[i] = 0;
    }

    // Predelay buffer (up to 200ms)
    mPredelayBuffer.resize(static_cast<size_t>(sampleRate * 0.2));
    std::fill(mPredelayBuffer.begin(), mPredelayBuffer.end(), 0.0f);
    mPredelayPos = 0;
  }

  void setParameters(float size, float decay, float predelay, float mix,
                     float damping) {
    // size: scales comb delay times (0.5 to 1.5x)
    mRoomSize = 0.5 + size;

    // decay: 0-1 maps to feedback amount
    mFeedback = 0.5 + decay * 0.45; // 0.5 to 0.95

    // predelay: 0-1 maps to 0-200ms
    mPredelaySamples = static_cast<int>(predelay * mSampleRate * 0.2);

    // mix: wet/dry
    mMix = mix;

    // damping: high frequency damping in feedback loop
    mDamping = damping * 0.4; // 0 to 0.4
  }

  void process(float *left, float *right, int numSamples) {
    for (int i = 0; i < numSamples; ++i) {
      // Input (mono sum)
      double input = (left[i] + right[i]) * 0.5;

      // Predelay
      int predelayReadPos = (mPredelayPos - mPredelaySamples +
                             static_cast<int>(mPredelayBuffer.size())) %
                            static_cast<int>(mPredelayBuffer.size());
      double predelayed = mPredelayBuffer[static_cast<size_t>(predelayReadPos)];
      mPredelayBuffer[static_cast<size_t>(mPredelayPos)] =
          static_cast<float>(input);
      mPredelayPos =
          (mPredelayPos + 1) % static_cast<int>(mPredelayBuffer.size());

      // Parallel comb filters
      double combOut = 0.0;
      for (int c = 0; c < 8; ++c) {
        int bufSize = static_cast<int>(mCombBuffers[c].size());
        double output = mCombBuffers[c][static_cast<size_t>(mCombPos[c])];

        // Lowpass filter in feedback path (damping)
        mCombFilterStore[c] =
            output * (1.0 - mDamping) + mCombFilterStore[c] * mDamping;

        mCombBuffers[c][static_cast<size_t>(mCombPos[c])] =
            static_cast<float>(predelayed + mCombFilterStore[c] * mFeedback);

        mCombPos[c] = (mCombPos[c] + 1) % bufSize;
        combOut += output;
      }
      combOut *= 0.125; // Average

      // Series allpass filters
      double allpassOut = combOut;
      for (int a = 0; a < 4; ++a) {
        int bufSize = static_cast<int>(mAllpassBuffers[a].size());
        double bufOut = mAllpassBuffers[a][static_cast<size_t>(mAllpassPos[a])];
        double newVal = allpassOut + bufOut * 0.5;

        mAllpassBuffers[a][static_cast<size_t>(mAllpassPos[a])] =
            static_cast<float>(newVal);
        allpassOut = bufOut - allpassOut * 0.5;

        mAllpassPos[a] = (mAllpassPos[a] + 1) % bufSize;
      }

      // Mix output (slight stereo spread)
      double wet = allpassOut;
      left[i] = static_cast<float>(left[i] * (1.0 - mMix) + wet * mMix);
      right[i] = static_cast<float>(right[i] * (1.0 - mMix) + wet * mMix);
    }
  }

private:
  double mSampleRate = 44100.0;

  std::vector<float> mCombBuffers[8];
  int mCombPos[8] = {0};
  double mCombFilterStore[8] = {0.0};

  std::vector<float> mAllpassBuffers[4];
  int mAllpassPos[4] = {0};

  std::vector<float> mPredelayBuffer;
  int mPredelayPos = 0;
  int mPredelaySamples = 0;

  double mRoomSize = 1.0;
  double mFeedback = 0.84;
  double mMix = 0.3;
  double mDamping = 0.2;
};

//------------------------------------------------------------------------
} // namespace DSP
} // namespace AIV
