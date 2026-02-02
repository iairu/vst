//
//  AIVDSPClasses.hpp
//  AIVExtension
//
//  Created by AIV on 02/02/2026.
//

#pragma once

#import <algorithm>
#import <cmath>
#import <iostream>
#import <span>
#import <vector>

// Constants
const double kPi = 3.14159265358979323846;

// --- Biquad Filter (EQ) ---
class BiquadFilter {
public:
  enum Type { LowShelf, HighShelf, Peaking };

  void calculateCoefficients(Type type, double freq, double Q, double dbGain,
                             double sampleRate) {
    double A = std::pow(10.0, dbGain / 40.0);
    double w0 = 2.0 * kPi * freq / sampleRate;
    double alpha = std::sin(w0) / (2.0 * Q);
    double cosw0 = std::cos(w0);

    switch (type) {
    case LowShelf: {
      double sqrtA = std::sqrt(A);
      b0 = A * ((A + 1) - (A - 1) * cosw0 + 2 * sqrtA * alpha);
      b1 = 2 * A * ((A - 1) - (A + 1) * cosw0);
      b2 = A * ((A + 1) - (A - 1) * cosw0 - 2 * sqrtA * alpha);
      a0 = (A + 1) + (A - 1) * cosw0 + 2 * sqrtA * alpha;
      a1 = -2 * ((A - 1) + (A + 1) * cosw0);
      a2 = (A + 1) + (A - 1) * cosw0 - 2 * sqrtA * alpha;
      break;
    }
    case HighShelf: {
      double sqrtA = std::sqrt(A);
      b0 = A * ((A + 1) + (A - 1) * cosw0 + 2 * sqrtA * alpha);
      b1 = -2 * A * ((A - 1) + (A + 1) * cosw0);
      b2 = A * ((A + 1) + (A - 1) * cosw0 - 2 * sqrtA * alpha);
      a0 = (A + 1) - (A - 1) * cosw0 + 2 * sqrtA * alpha;
      a1 = 2 * ((A - 1) - (A + 1) * cosw0);
      a2 = (A + 1) - (A - 1) * cosw0 - 2 * sqrtA * alpha;
      break;
    }
    case Peaking: {
      b0 = 1 + alpha * A;
      b1 = -2 * cosw0;
      b2 = 1 - alpha * A;
      a0 = 1 + alpha / A;
      a1 = -2 * cosw0;
      a2 = 1 - alpha / A;
      break;
    }
    }

    // Normalize
    b0 /= a0;
    b1 /= a0;
    b2 /= a0;
    a1 /= a0;
    a2 /= a0;
  }

  float process(float input) {
    double output = b0 * input + b1 * x1 + b2 * x2 - a1 * y1 - a2 * y2;
    x2 = x1;
    x1 = input;
    y2 = y1;
    y1 = output;
    return (float)output;
  }

  void reset() { x1 = x2 = y1 = y2 = 0; }

private:
  double b0 = 0, b1 = 0, b2 = 0, a0 = 0, a1 = 0, a2 = 0;
  double x1 = 0, x2 = 0, y1 = 0, y2 = 0;
};

// --- Simple Compressor ---
class SimpleCompressor {
public:
  void setParameters(double threshDb, double ratio, double attackMs,
                     double releaseMs, double makeupDb, double sampleRate) {
    threshold = pow(10.0, threshDb / 20.0);
    this->ratio = ratio;
    attack = exp(-1.0 / (sampleRate * attackMs / 1000.0));
    release = exp(-1.0 / (sampleRate * releaseMs / 1000.0));
    makeup = pow(10.0, makeupDb / 20.0);
  }

  float process(float input) {
    double absInput = fabs(input);

    // Envelope follower
    if (absInput > envelope)
      envelope = attack * (envelope - absInput) + absInput;
    else
      envelope = release * (envelope - absInput) + absInput;

    // Gain reduction
    double gain = 1.0;
    if (envelope > threshold) {
      if (ratio > 0.0)
        gain = pow(envelope / threshold, 1.0 / ratio - 1.0);
    }

    return (float)(input * gain * makeup);
  }

private:
  double threshold = 1.0, ratio = 1.0, attack = 0.0, release = 0.0,
         makeup = 1.0;
  double envelope = 0.0;
};

// --- Delay Line ---
class DelayLine {
public:
  void setParameters(double timeSec, double feedback, double mix,
                     double sampleRate) {
    delaySamples = (int)(timeSec * sampleRate);
    this->feedback = feedback / 100.0;
    this->mix = mix / 100.0;
    if (buffer.size() != (int)(sampleRate * 2.0)) { // Max 2 sec buffer
      buffer.resize((int)(sampleRate * 2.0), 0);
    }
  }

  float process(float input) {
    if (buffer.empty())
      return input;

    int readIndex = writeIndex - delaySamples;
    if (readIndex < 0)
      readIndex += buffer.size();

    float delayed = buffer[readIndex];
    float nextInput = input + delayed * feedback;

    buffer[writeIndex] = nextInput;
    writeIndex++;
    if (writeIndex >= buffer.size())
      writeIndex = 0;

    return input * (1.0 - mix) + delayed * mix;
  }

private:
  std::vector<float> buffer;
  int writeIndex = 0;
  int delaySamples = 0;
  double feedback = 0, mix = 0;
};

// --- Saturator ---
class Saturator {
public:
  void setParameters(double drive, double type) {
    this->drive = 1.0 + (drive / 10.0); // 1.0 to 11.0 range approx
    this->type = (int)type;
  }

  float process(float input) {
    float x = input * drive;

    if (type == 0) { // Soft Clip (Tape-ish)
      if (x > 1.0f)
        x = 1.0f;
      else if (x < -1.0f)
        x = -1.0f;
      else
        x = x - (x * x * x) / 3.0f;
    } else { // Hard Clip (Tube-ish / Fuzz)
      x = std::tanh(x);
    }

    return x;
  }

private:
  double drive = 1.0;
  int type = 0;
};

// --- Pitch Shifter (Granular) ---
class PitchShifter {
public:
  void setParameters(double amount, double sampleRate) {
    double semitones = (amount - 50.0) / 50.0 * 12.0;
    this->pitchRatio = pow(2.0, semitones / 12.0);
    this->sampleRate = sampleRate;

    int bufSize = (int)(sampleRate * 0.2); // 200ms
    if (buffer.size() != bufSize) {
      buffer.resize(bufSize, 0);
    }
    windowSize = (int)(sampleRate * 0.05); // 50ms
    if (windowSize > bufSize)
      windowSize = bufSize;
  }

  float process(float input) {
    if (buffer.empty())
      return input;

    buffer[writeIndex] = input;

    double delay1 = phase;
    double delay2 = phase + (windowSize / 2.0);
    if (delay2 >= windowSize)
      delay2 -= windowSize;

    float out1 = readBuffer(delay1);
    float out2 = readBuffer(delay2);

    float env1 = 1.0f - fabs(2.0f * (float)phase / windowSize - 1.0f);

    float output = out1 * env1 + out2 * (1.0f - env1);

    phase += (1.0 - pitchRatio);
    if (phase >= windowSize)
      phase -= windowSize;
    if (phase < 0)
      phase += windowSize;

    writeIndex++;
    if (writeIndex >= buffer.size())
      writeIndex = 0;

    return output;
  }

private:
  float readBuffer(double delaySamples) {
    double readPos = writeIndex - delaySamples;
    while (readPos < 0)
      readPos += buffer.size();
    while (readPos >= buffer.size())
      readPos -= buffer.size();

    int i = (int)readPos;
    double f = readPos - i;
    int i2 = i + 1;
    if (i2 >= buffer.size())
      i2 = 0;

    return buffer[i] * (1.0 - f) + buffer[i2] * f;
  }

  std::vector<float> buffer;
  int writeIndex = 0;
  double phase = 0;
  int windowSize = 0;
  double pitchRatio = 1.0;
  double sampleRate = 44100;
};

// --- Schroeder Reverb ---
class SchroederReverb {
public:
  SchroederReverb() {
    // Tunings for 44.1kHz
    int cD[] = {1557, 1617, 1491, 1422};
    combDelays.assign(cD, cD + 4);

    int aD[] = {225, 556};
    allpassDelays.assign(aD, aD + 2);

    combs.resize(4);
    for (int i = 0; i < 4; i++)
      combs[i].resize(2000, 0);

    allpasses.resize(2);
    for (int i = 0; i < 2; i++)
      allpasses[i].resize(1000, 0);
  }

  void setParameters(double size, double damp, double mix, double sampleRate) {
    this->feedback = 0.7 + (damp * 0.28);
    this->mix = mix / 100.0;
  }

  float process(float input) {
    float out = 0.0f;

    // Parallel Combs
    for (int i = 0; i < 4; i++) {
      int idx = combIndices[i];
      float delayed = combs[i][idx];
      float val = delayed * feedback;
      float combOut = delayed;
      combs[i][idx] = input + val;

      combIndices[i]++;
      if (combIndices[i] >= combDelays[i])
        combIndices[i] = 0;

      out += combOut;
    }

    // Series Allpasses
    for (int i = 0; i < 2; i++) {
      int idx = allpassIndices[i];
      float delayed = allpasses[i][idx];
      float g = 0.5f;
      float buf = out + g * delayed;
      float apOut = delayed - g * buf;
      allpasses[i][idx] = buf;
      allpassIndices[i]++;
      if (allpassIndices[i] >= allpassDelays[i])
        allpassIndices[i] = 0;

      out = apOut;
    }

    return input * (1.0f - mix) + out * mix;
  }

private:
  std::vector<std::vector<float>> combs;
  std::vector<int> combDelays;
  int combIndices[4] = {0};

  std::vector<std::vector<float>> allpasses;
  std::vector<int> allpassDelays;
  int allpassIndices[2] = {0};

  float feedback = 0.8f;
  float mix = 0.5f;
};
