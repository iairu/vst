//
//  AIVDSPClasses.hpp
//  AIVExtension
//
//  Created by AIV on 02/02/2026.
//

#pragma once

#import <algorithm>
#import <cmath>
#import <vector>

// Constants
const double kPi = 3.14159265358979323846;

// --- ZDF Filter (TPT SVF) ---
class ZDFFilter {
public:
  enum Type { HighPass, Peaking };

  void setParameters(Type type, double freq, double Q, double gainDb,
                     double sampleRate) {
    this->type = type;

    // Pre-warp frequency
    double w = kPi * freq / sampleRate;
    g = std::tan(w);

    // Q limiting
    if (Q < 0.1)
      Q = 0.1;
    this->Q = Q;

    // Gain (for Peaking)
    // For peaking, we often adjust Q or K based on gain, but standard TPT:
    // K = A.
    // Let's use standard constant-Q peaking logic
    A = std::pow(
        10.0,
        gainDb / 40.0); // A is linear gain^0.5 usually, or just linear logic
    // Actually standard typically uses A = 10^(G/40).

    // Optimization: Pre-calculate denominator
    // SVF Feedback scaling: R = 1 / (2*Q) ? Or just use 1/Q damping
    // Using standard topology:
    // iceq_d = 1 / (1 + g * (g + 1/Q)); for LP/HP/BP
    // For Peaking, it varies. Let's use the generalized one.
  }

  float process(float input) {
    // 2-pole TPT SVF
    // Ref: Zavalishin "The Art of VA Filter Design"

    // Damping factor R. Q = 1/(2R) -> R = 1/(2Q)
    double R = 1.0 / (2.0 * Q);

    // High Pass (12dB/oct)
    // hp = (vin - (2*R + g)*s1 - s2) / (1 + 2*R*g + g*g)
    // bp = g * hp + s1
    // lp = g * bp + s2
    // s1 = g * hp + bp -> 2*bp - s1 (optimized)

    // Denominator
    double den = 1.0 + 2.0 * R * g + g * g;
    double hp = (input - (2.0 * R + g) * s1 - s2) / den;
    double bp = g * hp + s1;
    double lp = g * bp + s2;

    // Update states
    s1 = 2.0 * bp - s1;
    s2 = 2.0 * lp - s2;

    if (type == HighPass) {
      return (float)hp;
    } else {
      // Peaking: H = 1 + (A^2 - 1) * BP_normalized?
      // Or H = x + c * BP?
      // Standard constant-Q peaking TPT:
      // out = lp + hp + (A*A) * bp / Q ? No.

      // Let's use the normalized bandpass form for peaking:
      // H_peak = 1 + (A*A - 1) * H_bp
      // where H_bp = (g/Q * s) / ...

      // Simpler implementation for Peaking in SVF:
      // mix of Input and Bandpass.
      // K = A*A - 1.0 (Boost/Cut amount)
      // With correct gain scaling.
      // Actually, simpler formula:
      // y = input + (A*A - 1.0) * bp_normalized
      // bp_normalized = bp * 2 * R ? (to have 0dB gain at center)
      // SVF BP transfer at w=wc is 1/(2R). So multiply by 2R.

      double K =
          std::pow(10.0, std::log10(A * A)) -
          1.0; // A*A is linear power gain? A is sqrt. A*A is linear gain.
      // Just use raw linear gain
      double linearGain = A * A;

      // Bandpass normalized to 0dB peak
      double bpNorm = bp * 2.0 * R; // Peak is 1

      return (float)(input + (linearGain - 1.0) * bpNorm);
    }
  }

  void reset() {
    s1 = 0;
    s2 = 0;
  }

private:
  Type type = HighPass;
  double g = 0.0;
  double Q = 0.707;
  double A = 1.0;
  double s1 = 0.0, s2 = 0.0;
};
class BiquadFilter {
public:
  enum Type { LowShelf, HighShelf, Peaking, LowPass, HighPass };

  void calculateCoefficients(Type type, double freq, double Q, double dbGain,
                             double sampleRate) {
    if (Q < 0.1)
      Q = 0.1;

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
    case LowPass: {
      b0 = (1 - cosw0) / 2;
      b1 = 1 - cosw0;
      b2 = (1 - cosw0) / 2;
      a0 = 1 + alpha;
      a1 = -2 * cosw0;
      a2 = 1 - alpha;
      break;
    }
    case HighPass: {
      b0 = (1 + cosw0) / 2;
      b1 = -(1 + cosw0);
      b2 = (1 + cosw0) / 2;
      a0 = 1 + alpha;
      a1 = -2 * cosw0;
      a2 = 1 - alpha;
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
  double b0 = 0, b1 = 0, b2 = 0, a0 = 1.0, a1 = 0, a2 = 0;
  double x1 = 0, x2 = 0, y1 = 0, y2 = 0;
};

// --- Auto Level (AGC) ---
class AutoLevel {
public:
  void setParameters(double targetDb, double rangeDb, double speed,
                     double sampleRate) {
    targetLevel = pow(10.0, targetDb / 20.0);
    maxGain = pow(10.0, rangeDb / 20.0);
    // Speed 0-100% maps to attack/release times
    double attackMs = 1000.0 - (speed * 9.0); // 100ms to 1000ms
    double releaseMs = attackMs * 2.0;

    attackCoef = exp(-1.0 / (sampleRate * attackMs / 1000.0));
    releaseCoef = exp(-1.0 / (sampleRate * releaseMs / 1000.0));
  }

  float process(float input) {
    double absInput = fabs(input);

    // Envelope follower
    if (absInput > envelope)
      envelope = attackCoef * (envelope - absInput) + absInput;
    else
      envelope = releaseCoef * (envelope - absInput) + absInput;

    if (envelope < 0.0001)
      return input;

    // Calculate required gain to hit target
    double currentDb = 20.0 * log10(envelope);
    double targetDbVal = 20.0 * log10(targetLevel);
    double gainDb = targetDbVal - currentDb;

    // Clamp gain
    if (gainDb > 20.0 * log10(maxGain))
      gainDb = 20.0 * log10(maxGain);
    if (gainDb < -20.0 * log10(maxGain))
      gainDb = -20.0 * log10(maxGain);

    double gain = pow(10.0, gainDb / 20.0);

    return input * gain;
  }

private:
  double targetLevel = 0.5;
  double maxGain = 2.0;
  double attackCoef = 0.0;
  double releaseCoef = 0.0;
  double envelope = 0.0;
};

// --- Noise Gate (Intelligent w/ Hysteresis) ---
class NoiseGate {
public:
  void setParameters(double thresholdDb, double rangeDb, double attackMs,
                     double holdMs, double releaseMs, double hysteresisDb,
                     double sampleRate) {
    this->openThreshold = pow(10.0, thresholdDb / 20.0);
    this->closeThreshold = pow(10.0, (thresholdDb - hysteresisDb) / 20.0);

    // Range: -80dB to 0dB.
    // If range is -20dB, gain factor is 0.1.
    this->rangeFactor = pow(10.0, rangeDb / 20.0);

    // Time constants
    this->attackCoeff = exp(-1.0 / (sampleRate * attackMs / 1000.0));
    this->releaseCoeff = exp(-1.0 / (sampleRate * releaseMs / 1000.0));

    this->holdSamples = (int)(holdMs / 1000.0 * sampleRate);
  }

  float process(float input) {
    float absInput = fabs(input);

    // Envelope Follower (Decoupled)
    if (absInput > envelope)
      envelope = attackCoeff * (envelope - absInput) + absInput;
    else
      envelope = releaseCoeff * (envelope - absInput) + absInput;

    // State Machine
    if (isGateOpen) {
      if (envelope < closeThreshold) {
        // Should close, check hold
        if (holdCounter >= holdSamples) {
          isGateOpen = false;
        } else {
          holdCounter++;
        }
      } else {
        // Signal is strong, reset hold
        holdCounter = 0;
      }
    } else {
      // Gate Closed
      if (envelope > openThreshold) {
        isGateOpen = true;
        holdCounter = 0;
      }
    }

    // Apply Gain
    // Smooth transition?
    // Logic: if Open, gain = 1.0. If Closed, gain = rangeFactor.
    // Needs smoothing to avoid clicks.
    // We use a separate 'gainEnvelope' for the actual VCA

    double targetGain = isGateOpen ? 1.0 : rangeFactor;

    // Attack/Release for the gain element itself (smoothing)
    // Use fixed fast smoothing or reuse attack/release params?
    // Prompt says "Release: The speed at which the signal fades to the Range
    // level" So we smooth the gain change.
    if (targetGain > currentGain)
      currentGain =
          attackCoeff * (currentGain - targetGain) + targetGain; // Opening
    else
      currentGain =
          releaseCoeff * (currentGain - targetGain) + targetGain; // Closing

    return input * (float)currentGain;
  }

private:
  double openThreshold = 0.0;
  double closeThreshold = 0.0;
  double rangeFactor = 0.0;
  double attackCoeff = 0.0;
  double releaseCoeff = 0.0;

  double envelope = 0.0;
  double currentGain = 0.0;

  bool isGateOpen = false;
  int holdSamples = 0;
  int holdCounter = 0;
};

// --- Deesser ---
class Deesser {
public:
  void setParameters(double thresholdDb, double frequency, double rangeDb,
                     double ratio, double sampleRate) {
    this->threshold = pow(10.0, thresholdDb / 20.0);
    this->ratio = ratio;
    this->maxAttenuation = pow(10.0, rangeDb / 20.0); // e.g. -6dB = 0.5

    // Split Band Architecture:
    // Use LowPass for crossover.
    // LowBand = LPF(Input)
    // HighBand = Input - LowBand
    crossoverFilter.calculateCoefficients(BiquadFilter::LowPass, frequency,
                                          0.707, 0.0, sampleRate);

    // Timing
    attack = exp(-1.0 / (sampleRate * 0.5 / 1000.0));   // 0.5ms fast attack
    release = exp(-1.0 / (sampleRate * 50.0 / 1000.0)); // 50ms
  }

  float process(float input) {
    // Split
    float lowBand = crossoverFilter.process(input);
    float highBand = input - lowBand;

    // Detect on High Band
    double absHigh = fabs(highBand);

    // Envelope
    if (absHigh > envelope)
      envelope = attack * (envelope - absHigh) + absHigh;
    else
      envelope = release * (envelope - absHigh) + absHigh;

    // Gain reduction
    double gain = 1.0;
    if (envelope > threshold) {
      gain = pow(envelope / threshold, 1.0 / ratio - 1.0);
    }

    // Range Check
    if (gain < maxAttenuation)
      gain = maxAttenuation;

    // Apply GR to HighBand only
    float processedHigh = highBand * gain;

    // Sum back
    return lowBand + processedHigh;
  }

private:
  BiquadFilter crossoverFilter;
  double threshold = 0.5;
  double ratio = 5.0;
  double maxAttenuation = 0.5;
  double envelope = 0.0;
  double attack = 0.0;
  double release = 0.0;
};

// --- FET Compressor (AIV 76) ---
class FETCompressor {
public:
  void setParameters(double inputDb, double ratio, double attackMs,
                     double releaseMs, double makeupDb, double sampleRate) {
    // 1. Input Drive (Gain before detector)
    // Range is -48 to +12 usually.
    this->inputGain = pow(10.0, inputDb / 20.0);

    // 2. Ratio
    this->ratio = ratio;

    // 3. Time Constants (Microseconds for Attack)
    // Attack: 20us to 800us.
    // Physics: tau = -1 / (fs * ln(1 - 0.632))
    // Formula from prompt: exp(-1 / (time * sampleRate))
    // Note: time in seconds.
    this->attackCoeff = exp(-1.0 / (sampleRate * attackMs / 1000.0));

    // Release: 50ms to 1100ms
    this->releaseCoeff = exp(-1.0 / (sampleRate * releaseMs / 1000.0));

    // 4. Makeup
    this->makeupGain = pow(10.0, makeupDb / 20.0);

    // Fixed Threshold (-20 dBFS internally)
    this->threshold = pow(10.0, -20.0 / 20.0); // 0.1
  }

  float process(float input) {
    // Apply Input Drive
    float drivenSignal = input * inputGain;
    float absInput = fabs(drivenSignal);

    // Peak Detector (Feedback topology simulation? No, prompt says Fixed
    // Threshold, input drives into it.) 1176 is feedback, but for digital
    // simplified model, feedforward with fixed threshold behaves similarly if
    // detector is after input gain.

    // Envelope
    if (absInput > envelope)
      envelope = attackCoeff * (envelope - absInput) + absInput;
    else
      envelope = releaseCoeff * (envelope - absInput) + absInput;

    // Gain Reduction
    double gain = 1.0;
    if (envelope > threshold) {
      // Gain reduction formula
      // GR = (env / thresh) ^ (1/R - 1)
      gain = pow(envelope / threshold, 1.0 / ratio - 1.0);
    }

    // Apply GR to the driven signal (or original? Standard 1176: Input gain IS
    // the volume knob) So output is drivenSignal * gain * makeup.
    return (float)(drivenSignal * gain * makeupGain);
  }

private:
  double inputGain = 1.0;
  double threshold = 0.1; // -20dB
  double ratio = 4.0;
  double attackCoeff = 0.0;
  double releaseCoeff = 0.0;
  double makeupGain = 1.0;
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
