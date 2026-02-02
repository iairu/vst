//------------------------------------------------------------------------
// Copyright(c) 2026 AIV.
//------------------------------------------------------------------------

#pragma once

#include <algorithm>
#include <cmath>

namespace AIV {
namespace DSP {

//------------------------------------------------------------------------
// Biquad filter implementation
//------------------------------------------------------------------------
class BiquadFilter {
public:
  enum Type { LowShelf, HighShelf, Peaking, LowPass, HighPass };

  void reset() {
    mX1L = mX2L = mY1L = mY2L = 0.0;
    mX1R = mX2R = mY1R = mY2R = 0.0;
  }

  void setCoeffs(Type type, double sampleRate, double freq, double gainDb,
                 double Q) {
    double A = std::pow(10.0, gainDb / 40.0);
    double omega = 2.0 * M_PI * freq / sampleRate;
    double sinOmega = std::sin(omega);
    double cosOmega = std::cos(omega);
    double alpha = sinOmega / (2.0 * Q);

    double a0, a1, a2, b0, b1, b2;

    switch (type) {
    case LowShelf: {
      double sqrtA = std::sqrt(A);
      b0 = A * ((A + 1.0) - (A - 1.0) * cosOmega + 2.0 * sqrtA * alpha);
      b1 = 2.0 * A * ((A - 1.0) - (A + 1.0) * cosOmega);
      b2 = A * ((A + 1.0) - (A - 1.0) * cosOmega - 2.0 * sqrtA * alpha);
      a0 = (A + 1.0) + (A - 1.0) * cosOmega + 2.0 * sqrtA * alpha;
      a1 = -2.0 * ((A - 1.0) + (A + 1.0) * cosOmega);
      a2 = (A + 1.0) + (A - 1.0) * cosOmega - 2.0 * sqrtA * alpha;
      break;
    }
    case HighShelf: {
      double sqrtA = std::sqrt(A);
      b0 = A * ((A + 1.0) + (A - 1.0) * cosOmega + 2.0 * sqrtA * alpha);
      b1 = -2.0 * A * ((A - 1.0) + (A + 1.0) * cosOmega);
      b2 = A * ((A + 1.0) + (A - 1.0) * cosOmega - 2.0 * sqrtA * alpha);
      a0 = (A + 1.0) - (A - 1.0) * cosOmega + 2.0 * sqrtA * alpha;
      a1 = 2.0 * ((A - 1.0) - (A + 1.0) * cosOmega);
      a2 = (A + 1.0) - (A - 1.0) * cosOmega - 2.0 * sqrtA * alpha;
      break;
    }
    case Peaking:
    default: {
      b0 = 1.0 + alpha * A;
      b1 = -2.0 * cosOmega;
      b2 = 1.0 - alpha * A;
      a0 = 1.0 + alpha / A;
      a1 = -2.0 * cosOmega;
      a2 = 1.0 - alpha / A;
      break;
    }
    case LowPass: {
      b0 = (1.0 - cosOmega) / 2.0;
      b1 = 1.0 - cosOmega;
      b2 = (1.0 - cosOmega) / 2.0;
      a0 = 1.0 + alpha;
      a1 = -2.0 * cosOmega;
      a2 = 1.0 - alpha;
      break;
    }
    case HighPass: {
      b0 = (1.0 + cosOmega) / 2.0;
      b1 = -(1.0 + cosOmega);
      b2 = (1.0 + cosOmega) / 2.0;
      a0 = 1.0 + alpha;
      a1 = -2.0 * cosOmega;
      a2 = 1.0 - alpha;
      break;
    }
    }

    // Normalize coefficients
    mB0 = b0 / a0;
    mB1 = b1 / a0;
    mB2 = b2 / a0;
    mA1 = a1 / a0;
    mA2 = a2 / a0;
  }

  void processStereo(float *left, float *right, int numSamples) {
    for (int i = 0; i < numSamples; ++i) {
      // Left channel
      double inL = left[i];
      double outL =
          mB0 * inL + mB1 * mX1L + mB2 * mX2L - mA1 * mY1L - mA2 * mY2L;
      mX2L = mX1L;
      mX1L = inL;
      mY2L = mY1L;
      mY1L = outL;
      left[i] = static_cast<float>(outL);

      // Right channel
      double inR = right[i];
      double outR =
          mB0 * inR + mB1 * mX1R + mB2 * mX2R - mA1 * mY1R - mA2 * mY2R;
      mX2R = mX1R;
      mX1R = inR;
      mY2R = mY1R;
      mY1R = outR;
      right[i] = static_cast<float>(outR);
    }
  }

private:
  double mB0 = 1.0, mB1 = 0.0, mB2 = 0.0;
  double mA1 = 0.0, mA2 = 0.0;
  double mX1L = 0.0, mX2L = 0.0, mY1L = 0.0, mY2L = 0.0;
  double mX1R = 0.0, mX2R = 0.0, mY1R = 0.0, mY2R = 0.0;
};

//------------------------------------------------------------------------
// EQ - 4-band parametric equalizer
//------------------------------------------------------------------------
class EQ {
public:
  void reset(double sampleRate) {
    mSampleRate = sampleRate;
    for (int i = 0; i < 4; ++i)
      mBands[i].reset();
  }

  void setBand(int band, float gain, float freq, float q) {
    if (band < 0 || band > 3)
      return;

    // gain: 0-1 maps to -18dB to +18dB
    double gainDb = gain * 36.0 - 18.0;

    // freq: 0-1 maps logarithmically 20Hz to 20kHz
    double frequency = 20.0 * std::pow(1000.0, static_cast<double>(freq));
    frequency = std::clamp(frequency, 20.0, 20000.0);

    // q: 0-1 maps to 0.1 to 10
    double Q = 0.1 + q * 9.9;

    BiquadFilter::Type type = BiquadFilter::Peaking;
    if (band == 0)
      type = BiquadFilter::LowShelf;
    else if (band == 3)
      type = BiquadFilter::HighShelf;

    mBands[band].setCoeffs(type, mSampleRate, frequency, gainDb, Q);
  }

  void process(float *left, float *right, int numSamples) {
    for (int i = 0; i < 4; ++i) {
      mBands[i].processStereo(left, right, numSamples);
    }
  }

private:
  double mSampleRate = 44100.0;
  BiquadFilter mBands[4];
};

//------------------------------------------------------------------------
} // namespace DSP
} // namespace AIV
