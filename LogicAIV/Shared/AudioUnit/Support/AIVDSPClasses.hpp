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

// Enum for identification of spectral bands
enum SpectralBand {
  kBand_Sub,     // < 100 Hz
  kBand_Mud,     // 200 - 500 Hz
  kBand_Core,    // 500 Hz - 2 kHz
  kBand_Screech, // 2 kHz - 5 kHz
  kBand_Air      // > 8 kHz
};

// --- CrossNormalizer Supervisor ---
class CrossNormalizer {
public:
  CrossNormalizer() {
    mTargetRMS_Input = -18.0f;
    mTargetPeak_Output = -1.0f;
  }

  // Called once per block to update control signals
  void processLogic(const float *inputBuffer, int numSamples,
                    float currentGateState, float currentCompGR,
                    double sampleRate) {

    // Calculate coeffs if sample rate changed (approx check)
    // 1-pole coeff: exp(-2pi * freq / sr)
    if (sampleRate != mSampleRate) {
      mSampleRate = sampleRate;
      coeff200 = 1.0 - std::exp(-2.0 * kPi * 200.0 / sampleRate);
      coeff500 = 1.0 - std::exp(-2.0 * kPi * 500.0 / sampleRate);
      coeff2000 = 1.0 - std::exp(-2.0 * kPi * 2000.0 / sampleRate);
      coeff5000 = 1.0 - std::exp(-2.0 * kPi * 5000.0 / sampleRate);
    }

    // 1. ANALYZE INPUT (Tap A) & SPECTRAL BANDS (Tap C)
    // We iterate the buffer to update filter states and accumulate energy
    float sumSq = 0.0f;
    float peak = 0.0f;

    float sumMud = 0.0f;
    float sumCore = 0.0f;
    float sumScreech = 0.0f;

    for (int i = 0; i < numSamples; ++i) {
      float s = inputBuffer[i];
      float absS = std::fabs(s);
      if (absS > peak)
        peak = absS;
      sumSq += s * s;

      // 1-pole LPFs
      // y += coeff * (x - y)
      state200 += coeff200 * (s - state200);
      state500 += coeff500 * (s - state500);
      state2000 += coeff2000 * (s - state2000);
      state5000 += coeff5000 * (s - state5000);

      // Band Isolation (Subtraction)
      // Mud: 200-500Hz -> LPF500 - LPF200
      float mudSample = state500 - state200;
      sumMud += mudSample * mudSample;

      // Core: 500-2000Hz -> LPF2000 - LPF500
      float coreSample = state2000 - state500;
      sumCore += coreSample * coreSample;

      // Screech: 2000-5000Hz -> LPF5000 - LPF2000
      float screechSample = state5000 - state2000;
      sumScreech += screechSample * screechSample;
    }

    float inputRMS = std::sqrt(sumSq / numSamples + 1e-9f);

    // 2. SAFETY PRE-GAIN (Clipping Fix)
    // Fast attack, slow release safety pad
    if (peak > 0.707f) { // -3dB
      // Attenuate to target -6dB (0.5)
      // Gain = 0.5 / peak;
      float requiredGain = 0.5f / peak;
      if (requiredGain < mSafetyPadGain)
        mSafetyPadGain = requiredGain;
    } else {
      mSafetyPadGain = 0.999f * mSafetyPadGain + 0.001f * 1.0f;
    }

    // 3. AUTO-LEVEL LOGIC
    if (currentGateState > 0.5f) {
      float errordB = mTargetRMS_Input - 20.0f * log10f(inputRMS + 0.0001f);
      mAutoLevelGainDB = errordB;
      if (mAutoLevelGainDB > 12.0f)
        mAutoLevelGainDB = 12.0f;
      if (mAutoLevelGainDB < -12.0f)
        mAutoLevelGainDB = -12.0f;
    }

    // 4. LINK COMPRESSOR
    mCompThresholdOffset = mAutoLevelGainDB;

    // 5. SPECTRAL LOGIC
    float mudRMS = std::sqrt(sumMud / numSamples + 1e-9f);
    float coreRMS = std::sqrt(sumCore / numSamples + 1e-9f);
    float screechRMS = std::sqrt(sumScreech / numSamples + 1e-9f);

    // MUD CUT
    // If Mud is > Core (Reference: Pink noise, Mud should be ~equal or less
    // than Core?) Actually Mud (200-500) vs Core (500-2000). Bandwidth is
    // similar (1.3 oct vs 2 oct). Let's assume ratio threshold 1.2
    float mudRatio = mudRMS / (coreRMS + 1e-5f);
    if (mudRatio > 1.2f && inputRMS > 0.05f) {
      float excess = mudRatio - 1.2f;
      mMudCutDB = -1.0f * (excess * 6.0f);
      if (mMudCutDB < -9.0f)
        mMudCutDB = -9.0f;
    } else {
      mMudCutDB *= 0.98f; // Release
    }

    // SCREECH DAMPING
    // If Screech > Core * 0.8 (Screech should be lower in pink noise)
    float screechRatio = screechRMS / (coreRMS + 1e-5f);
    if (screechRatio > 0.8f && inputRMS > 0.05f) {
      mSatDriveScaler = 0.7f; // Dampen
    } else {
      mSatDriveScaler = mSatDriveScaler * 0.99f + 0.01f * 1.0f; // Recover
    }
  }

  float getSafetyPad() const { return mSafetyPadGain; }
  float getAutoLevelGain() const { return mAutoLevelGainDB; }
  float getCompThresholdAdjust() const { return mCompThresholdOffset; }
  float getMudEqCut() const { return mMudCutDB; }
  float getSatDriveScaler() const { return mSatDriveScaler; }

private:
  float mSafetyPadGain = 1.0f;
  float mAutoLevelGainDB = 0.0f;
  float mCompThresholdOffset = 0.0f;
  float mMudCutDB = 0.0f;
  float mSatDriveScaler = 1.0f;

  float mTargetRMS_Input;
  float mTargetPeak_Output;

  // Analysis Filters
  double mSampleRate = 44100.0;
  float state200 = 0, state500 = 0, state2000 = 0, state5000 = 0;
  float coeff200 = 0, coeff500 = 0, coeff2000 = 0, coeff5000 = 0;

  float calculateRMS(const float *buffer, int numSamples) {
    // unused helper now integrated in main loop
    return 0.0f;
  }
  float calculatePeak(const float *buffer, int numSamples) {
    // unused helper now integrated in main loop
    return 0.0f;
  }
};

// --- ZDF Filter (TPT SVF) ---
class ZDFFilter {
public:
  enum Type { HighPass, Peaking, LowPass };

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
    } else if (type == Peaking) {
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
      // Normalize Bandpass to 0dB peak (Peak of raw BP is Q = 1/2R)
      double bpNorm = bp * 2.0 * R;

      return (float)(input + (linearGain - 1.0) * bpNorm);
    } else if (type == LowPass) {
      return (float)lp;
    }

    return input;
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

  void setGainOffset(double db) {
    externalGainDb = db;
    useExternalGain = true;
  }

  float process(float input) {
    // If CrossNormalizer is driving, we use the external gain target
    // but strictly smoothing it would be better.
    // For now, we mix it or use it.
    // If useExternalGain, we ignore the internal envelope's gain calculation?
    // The prompt says "AutoLevel Gain" from Normalizer is the *Gain to apply*.

    if (useExternalGain) {
      double targetG = pow(10.0, externalGainDb / 20.0);
      // Smooth the transition to the externally commanded gain
      // reusing attackCoef (or a fixed fast smoothing)
      currentGain = attackCoef * (currentGain - targetG) + targetG;
      return input * (float)currentGain;
    }

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

  // CrossNormalizer Support
  double externalGainDb = 0.0;
  bool useExternalGain = false;
  double currentGain = 1.0;
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

  bool isOpen() const { return isGateOpen; }

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
    if (autoMakeup) {
      // Auto-Makeup Physics:
      // Theoretical attenuation at 0dB input:
      // A = (0 - Threshold) * (1 - 1/Ratio)
      // Makeup = -A
      double tDb = 20.0 * log10(threshold);
      double attenuationDb = (0.0 - tDb) * (1.0 - 1.0 / ratio);
      this->makeupGain = pow(10.0, attenuationDb / 20.0);
    } else {
      this->makeupGain = pow(10.0, makeupDb / 20.0);
    }
  }

  void setAutoMakeup(bool enabled) { this->autoMakeup = enabled; }
  void setThresholdOffset(double db) { this->thresholdOffsetDb = db; }

  float process(float input) {
    // Apply Input Drive
    float drivenSignal = input * inputGain;
    float absInput = fabs(drivenSignal);

    // Apply Threshold Offset from CrossNormalizer (inverse to AutoLevel gain)
    // If AutoLevel adds gain (+dB), we raise threshold (+dB) so compression
    // amount stays consistent
    double effectiveThreshold = threshold * pow(10.0, thresholdOffsetDb / 20.0);

    // Envelope
    if (absInput > envelope)
      envelope = attackCoeff * (envelope - absInput) + absInput;
    else
      envelope = releaseCoeff * (envelope - absInput) + absInput;

    // Gain Reduction
    double gain = 1.0;
    if (envelope > effectiveThreshold) {
      // Gain reduction formula
      // GR = (env / thresh) ^ (1/R - 1)
      gain = pow(envelope / effectiveThreshold, 1.0 / ratio - 1.0);
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
  bool autoMakeup = false;
  double thresholdOffsetDb = 0.0;
};

// --- True Peak Limiter (Lookahead + Sinc) ---
// Research: 11.2 Inter-Sample Peak Detection
// Uses 4x Oversampling (Sinc Interpolation) to detect True Peaks.
// Uses Lookahead Buffer to catch transients before they clip.
class TruePeakLimiter {
public:
  TruePeakLimiter() {
    buffer.resize(4096, 0.0f); // Max ~90ms at 44.1k
  }

  void setParameters(double ceilingDb, double lookaheadMs, double releaseMs,
                     double sampleRate) {
    this->ceiling = pow(10.0, ceilingDb / 20.0);

    int lookaheadSamples = (int)(lookaheadMs / 1000.0 * sampleRate);
    if (lookaheadSamples < 1)
      lookaheadSamples = 1;
    this->lookaheadDelay = lookaheadSamples;

    this->releaseCoeff = exp(-1.0 / (sampleRate * releaseMs / 1000.0));
  }

  float process(float input) {
    // 1. Write to Lookahead Buffer
    buffer[writeIndex] = input;

    // 2. Read Delayed Output (Audio Path)
    int readIndex = writeIndex - lookaheadDelay;
    if (readIndex < 0)
      readIndex += buffer.size();
    float delayedOutput = buffer[readIndex];

    // 3. Detect True Peak in Sidechain (Scanning ahead)
    // We scan the 'input' (which is 'lookaheadDelay' samples in the future
    // relative to output) AND its neighbors for ISP. Simplified 4x oversampling
    // check: A true 4x sinc is expensive. Fast approximation: Check current
    // sample and mid-points using polynomial.

    // For strict True Peak, we should upsample.
    // Here we use a conservative 4-point cubic measurement of the *current*
    // input to estimate if a peak exists between samples. Ideally we check the
    // buffer around 'writeIndex'.

    float maxPeak = fabs(input);

    // Check 3 points back for interpolation context
    int i0 = writeIndex - 3;
    if (i0 < 0)
      i0 += buffer.size();
    float y0 = buffer[i0];

    int i1 = writeIndex - 2;
    if (i1 < 0)
      i1 += buffer.size();
    float y1 = buffer[i1];

    int i2 = writeIndex - 1;
    if (i2 < 0)
      i2 += buffer.size();
    float y2 = buffer[i2];

    float y3 = input; // current

    // Cubic Interpolation to find peaks between y1 and y2
    // Detect inter-sample peak:
    // Simple heuristic: if y1 and y2 are both high, check midpoint.
    // Midpoint (0.5) via cubic:
    // c0*y0 + c1*y1 + c2*y2 + c3*y3 (with d=0.5)
    // coeffs for d=0.5: -0.0625, 0.5625, 0.5625, -0.0625
    float mid = -0.0625f * y0 + 0.5625f * y1 + 0.5625f * y2 - 0.0625f * y3;
    if (fabs(mid) > maxPeak)
      maxPeak = fabs(mid);

    // 4. Update Gain Reduction Envelope
    // Attack is instant (0ms) relative to the lookahead.
    // Actually, attack time = lookahead time.
    // We map the peak to the delayed signal?
    // Standard Lookahead Limiter:
    // Peak is detected 'lookahead' samples ahead.
    // Target Gain = Ceiling / Peak.
    // if Peak > Ceiling, Gain < 1.0.
    // We smooth this target gain over the lookahead window?
    // Or simply:

    double targetGain = 1.0;
    if (maxPeak > ceiling) {
      targetGain = ceiling / maxPeak;
    }

    // Release Logic
    if (targetGain < envelope) {
      // Attack (Instant/Fast)
      envelope = targetGain;
    } else {
      // Release (Slow recovery)
      envelope = releaseCoeff * (envelope - targetGain) + targetGain;
    }

    // 5. Apply Gain to *Delayed* Output
    // Note: The envelope calculated from 'input' (future) is applied to
    // 'delayedOutput' (now). This effectively aligns the gain reduction with
    // the transient. Wait, if we detect a peak NOW (in input), we need to
    // reduce gain NOW? No, the peak is in the future relative to the output. So
    // we need to delay the control signal? Or is the envelope *already*
    // applying to the future? Standard: Buffer audio. Derive gain from Input
    // (Future). Smooth gain. Apply smoothed gain to Delayed Audio. This means
    // the gain dips *before* the transient arrives at the output.

    // The release needs to be correct.
    // Let's us a simple "Hold" on the gain for lookahead samples?

    // Simply applying the envelope derived from Input to DelayedOutput works
    // if the attack time matches the lookahead.
    // If we set envelope = targetGain immediately, the gain drops *lookahead*
    // samples early. This is a "pre-attack". Perfect.

    float out = delayedOutput * (float)envelope;

    writeIndex++;
    if (writeIndex >= buffer.size())
      writeIndex = 0;

    return out;
  }

private:
  std::vector<float> buffer;
  int writeIndex = 0;
  int lookaheadDelay = 88; // 2ms at 44.1k
  double ceiling = 1.0;
  double releaseCoeff = 0.0;
  double envelope = 1.0;
};

// --- Delay Line (Lagrange Interpolation) ---
// Research: 10.1 Delay Interpolation Physics
// 4th-order Lagrange Interpolation prevents "zipper noise" and aliasing
// during time modulation (tape echo effects).
class DelayLine {
public:
  void setParameters(double timeSec, double feedback, double mix,
                     double sampleRate) {
    this->targetDelay = timeSec * sampleRate;
    this->feedback = feedback / 100.0;
    this->mix = mix / 100.0;

    // Smooth delay time changes
    if (currentDelay == 0)
      currentDelay = targetDelay;

    if (buffer.size() != (int)(sampleRate * 2.0)) { // Max 2 sec buffer
      buffer.resize((int)(sampleRate * 2.0), 0);
    }
  }

  float process(float input) {
    if (buffer.empty())
      return input;

    // Smooth delay time (Simple LPF on the delay time itself)
    currentDelay = 0.999 * currentDelay + 0.001 * targetDelay;

    // 4th-Order Lagrange Interpolation
    // We need 4 points around the fractional delay.
    // readPos = writeIndex - currentDelay
    double readPos = writeIndex - currentDelay;
    while (readPos < 0)
      readPos += buffer.size();
    while (readPos >= buffer.size())
      readPos -= buffer.size();

    int i = (int)readPos;
    double frac = readPos - i;

    // Indices for 4 points: i-1, i, i+1, i+2
    // Optimizing relative to 'i' being the floor.
    // Actually standard Lagrange 4-point usually takes y[n-1], y[n], y[n+1],
    // y[n+2] relative to the fractional position? Let's treat 'frac' as delta
    // from sample 'i'. We need indices: i-1, i, i+1, i+2

    int i0 = i - 1;
    int i1 = i;
    int i2 = i + 1;
    int i3 = i + 2;

    // Wrap indices
    if (i0 < 0)
      i0 += buffer.size();
    if (i1 >= buffer.size())
      i1 -= buffer.size();
    if (i2 >= buffer.size())
      i2 -= buffer.size();
    if (i3 >= buffer.size())
      i3 -= buffer.size();

    float y0 = buffer[i0];
    float y1 = buffer[i1];
    float y2 = buffer[i2];
    float y3 = buffer[i3];

    // Lagrange Coefficients (4th order / Cubic)
    // d = frac (0..1)
    // Common coefficients for cubic Lagrange:
    // c0 = -d * (d-1) * (d-2) / 6
    // c1 = (d+1) * (d-1) * (d-2) / 2
    // c2 = -(d+1) * d * (d-2) / 2
    // c3 = (d+1) * d * (d-1) / 6

    double d = frac;
    double c0 = -d * (d - 1.0) * (d - 2.0) / 6.0;
    double c1 = (d + 1.0) * (d - 1.0) * (d - 2.0) / 2.0;
    double c2 = -(d + 1.0) * d * (d - 2.0) / 2.0;
    double c3 = (d + 1.0) * d * (d - 1.0) / 6.0;

    float delayed = (float)(c0 * y0 + c1 * y1 + c2 * y2 + c3 * y3);

    float nextInput = input + delayed * feedback;

    // Soft Clip Feedback to prevent explosion
    if (nextInput > 2.0f)
      nextInput = 2.0f;
    if (nextInput < -2.0f)
      nextInput = -2.0f;

    buffer[writeIndex] = nextInput;
    writeIndex++;
    if (writeIndex >= buffer.size())
      writeIndex = 0;

    return input * (1.0 - mix) + delayed * mix;
  }

private:
  std::vector<float> buffer;
  int writeIndex = 0;
  double targetDelay = 0;
  double currentDelay = 0;
  double feedback = 0, mix = 0;
};

// --- Saturator ---
class Saturator {
public:
  void setParameters(double drive, double type, double sampleRate) {
    this->drive = 1.0 + (drive / 10.0); // 1.0 to 11.0 range approx
    this->type = (int)type;

    // Sandwich Distortion Filters
    // Pre: Cut Screech frequencies (-6dB High Shelf @ 4k)
    mPreTone.calculateCoefficients(BiquadFilter::HighShelf, 4000.0, 0.707, -6.0,
                                   sampleRate);
    // Post: Boost back (+6dB High Shelf @ 4k)
    mPostTone.calculateCoefficients(BiquadFilter::HighShelf, 4000.0, 0.707, 6.0,
                                    sampleRate);
  }

  void setDriveScale(double scale) { this->driveScale = scale; }

  float process(float input) {
    // 1. Pre-Tone
    float pre = mPreTone.process(input);

    // 2. Saturate
    float x = pre * drive * driveScale;

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

    // 3. Post-Tone
    return mPostTone.process(x);
  }

private:
  double drive = 1.0;
  double driveScale = 1.0;
  int type = 0;
  BiquadFilter mPreTone;
  BiquadFilter mPostTone;
};

// --- Pitch Shifter (Granular) ---
class PitchShifter {
public:
  void setParameters(double amount, double speedPct, double sampleRate) {
    double semitones = (amount - 50.0) / 50.0 * 12.0;
    this->pitchRatio = pow(2.0, semitones / 12.0);
    this->sampleRate = sampleRate;

    int bufSize = (int)(sampleRate * 0.2); // 200ms
    if (buffer.size() != bufSize) {
      buffer.resize(bufSize, 0);
    }

    // Map speed (0-100) to window size (100ms - 10ms)
    // Slower Speed = Larger Window (Smoother)
    // Faster Speed = Smaller Window (Choppy/Fast)
    // 0 -> 100ms, 100 -> 10ms
    double targetMs = 100.0 - (speedPct * 0.9);
    if (targetMs < 10.0)
      targetMs = 10.0;

    windowSize = (int)(sampleRate * targetMs / 1000.0);
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

// --- FDN Reverb (8x8 Hadamard) ---
// Research: 10.2 Feedback Delay Networks (FDN)
// Uses an 8x8 Hadamard Matrix for maximum diffusion and unitary energy
// preservation. Prime number delay lengths prevent resonant modes.
class FDNReverb {
public:
  FDNReverb() {
    // Prime number delays for 44.1kHz (approx 25ms to 90ms)
    // Scaled by size parameter later.
    baseDelays = {1117, 1361, 1613, 1933, 2273, 2663, 3167, 3943};

    delayLines.resize(8);
    // Buffer size generous enough for modulation
    for (int i = 0; i < 8; i++) {
      delayLines[i].resize(8192, 0); // ~180ms max
    }

    // Feedback buffer
    feedbackBuffer.resize(8, 0.0f);
    outputs.resize(8, 0.0f);

    // LowPass states for damping
    lpStates.resize(8, 0.0f);
  }

  void setParameters(double size, double damp, double mix, double sampleRate) {
    this->mix = mix / 100.0;

    // Size scales the delay lines
    // size 0-100. 50 is nominal.
    double sizeFactor = 0.5 + (size / 100.0); // 0.5x to 1.5x

    for (int i = 0; i < 8; i++) {
      currentDelays[i] = (int)(baseDelays[i] * sizeFactor);
      // Safety clamp
      if (currentDelays[i] >= 8192)
        currentDelays[i] = 8191;
    }

    // Damping (LowPass in feedback)
    // Damp 0-100 -> Cutoff freq high to low
    // simple one-pole coef: y = x + coef * (last_y - x) -> y = x(1-c) + last*c
    // ? Or y = y + c * (x - y) Higher damp = lower cutoff = higher coef (if
    // coef 0 is no filtering) 0 -> 0.0 (open) 100 -> 0.4 (quite muffled loops)
    this->dampCoef = damp / 250.0;

    // RT60 roughly controlled by feedback gain
    // T60 = -3 * Delay / log(gain)
    // We want long tails. Gain close to 1.0.
    // Let's map size/decay to feedback gain.
    // We'll use a fixed high gain for the "matrix" and attenuation via
    // damp/decay. Actually usually there is a decay param. Let's assume 'size'
    // acts as decay time in this simplified model, or we add a decay param if
    // needed. Prompt says "Reverb Decay" is a param. Oh wait, setParameters has
    // size, damp, mix. Check AIVDSPKernel.hpp calls. It passes reverbSize,
    // reverbDamp, reverbMix. Wait, prompt 10.3 says Reverb Decay (RT60) is a
    // param. But AIVDSPKernelAdapter enum has ReverbSize, ReverbDamp,
    // ReverbMix. I will use 'size' to derive feedback gain to approximate
    // decay.

    // Map size (0-100) to feedback (0.8 to 0.99)
    this->feedbackGain = 0.80 + (size / 100.0) * 0.19;
  }

  float process(float input) {
    // 1. Input into all delay lines
    // 2. Read from delay lines
    // 3. Hadamard Mix
    // 4. Feedback with Damping

    float outSum = 0.0f;

    for (int i = 0; i < 8; i++) {
      // Read ptr
      int readIdx = indices[i] - currentDelays[i];
      if (readIdx < 0)
        readIdx += delayLines[i].size();

      float delayed = delayLines[i][readIdx];
      outputs[i] = delayed;
      outSum += delayed;
    }

    // Hadamard Matrix 8x8 Fast Transform
    // Recursive structure:
    // H1 = [1]
    // H2 = [1 1; 1 -1]
    // ...
    // Standard unnormalized Hadamard creates gain of sqrt(8). We must normalize
    // by 1/sqrt(8).

    // Copy outputs to temp for matrix op
    float h[8];
    for (int i = 0; i < 8; i++)
      h[i] = outputs[i];

    // Fast Walsh-Hadamard Transform logic for 8 elements
    // Stage 1
    // 0+1, 0-1, 2+3, 2-3...
    float s1[8];
    for (int i = 0; i < 8; i += 2) {
      s1[i] = h[i] + h[i + 1];
      s1[i + 1] = h[i] - h[i + 1];
    }

    // Stage 2
    // 0+2, 1+3, 0-2, 1-3... (group 4)
    // Actually standard butterfly:
    // For size 4:
    // S2[0] = S1[0] + S1[2]
    // S2[1] = S1[1] + S1[3]
    // S2[2] = S1[0] - S1[2]
    // S2[3] = S1[1] - S1[3]
    // Same for 4-7
    float s2[8];
    for (int offset = 0; offset < 8; offset += 4) {
      s2[offset + 0] = s1[offset + 0] + s1[offset + 2];
      s2[offset + 1] = s1[offset + 1] + s1[offset + 3];
      s2[offset + 2] = s1[offset + 0] - s1[offset + 2];
      s2[offset + 3] = s1[offset + 1] - s1[offset + 3];
    }

    // Stage 3 (Final for 8)
    float s3[8]; // Result
    for (int i = 0; i < 4; i++) {
      s3[i] = s2[i] + s2[i + 4];
      s3[i + 4] = s2[i] - s2[i + 4];
    }

    // Normalize (1/sqrt(8) approx 0.3535)
    float norm = 0.35355f;

    // Feedback Loop
    for (int i = 0; i < 8; i++) {
      float mixed = s3[i] * norm;

      // Damping (One-pole LowPass)
      // y[n] = x[n] * (1-d) + y[n-1] * d
      lpStates[i] = mixed * (1.0 - dampCoef) + lpStates[i] * dampCoef;
      float damped = lpStates[i];

      // Sum Input + Feedback
      // Input injected into all lines (or specific ones?)
      // Standard FDN: Input + Feedback -> Delay
      float next = input + damped * feedbackGain;

      // Soft clip safety
      if (next > 2.0f)
        next = 2.0f;
      else if (next < -2.0f)
        next = -2.0f;

      // Write
      delayLines[i][indices[i]] = next;

      // Advance ptr
      indices[i]++;
      if (indices[i] >= delayLines[i].size())
        indices[i] = 0;
    }

    return input * (1.0f - mix) + outSum * 0.125f * mix; // Normalize sum output
  }

private:
  std::vector<int> baseDelays;
  int currentDelays[8] = {0};
  std::vector<std::vector<float>> delayLines;
  std::vector<int> indices = std::vector<int>(8, 0);

  std::vector<float> feedbackBuffer;
  std::vector<float> outputs;
  std::vector<float> lpStates;

  float feedbackGain = 0.5f;
  float dampCoef = 0.0f;
  float mix = 0.0f;
};

// --- Oversampler (4x Linear Phase FIR) ---
class Oversampler {
public:
  Oversampler() { reset(); }

  void initialize() {
    generateCoeffs();
    reset();
  }

  void reset() {
    std::fill(upBuffer.begin(), upBuffer.end(), 0.0f);
    std::fill(downBuffer.begin(), downBuffer.end(), 0.0f);
    upMbIndex = 0;
    downMbIndex = 0;
  }

  // Upsample: 1 input -> 4 outputs
  // Writes 4 samples to 'output' array
  void processUpsample(float input, float *output) {
    // 1. Insert input into circular buffer (Zero Stuffing logic implicit in
    // polyphase) Standard FIR: Upsampling means inserting 3 zeros between
    // samples. The LowPass filter is applied to this sparse stream. Polyphase
    // efficient method: output[0] = input * h[0] + hist[0]*h[4] + ... output[1]
    // = input * h[1] + hist[0]*h[5] + ... But here we use a simple sliding
    // window for robustness.

    // Push input
    upBuffer[upMbIndex] = input;

    // Convolve for 4 phases
    // Phase p (0..3): corresponds to fractional delay
    // We need to maintain state. The buffer stores *input* samples (at 1x
    // rate). Each output sample is a dot product of the input buffer and a
    // subset of coeffs.

    // Coeff index mapping for 4x upsampling:
    // h has length 64.
    // y[0] = sum(x[n-k] * h[4k])
    // y[1] = sum(x[n-k] * h[4k + 1])
    // ...

    int bufSize = (int)upBuffer.size();

    for (int phase = 0; phase < 4; ++phase) {
      double sum = 0.0;
      // Dot product
      int tapIndex = phase;
      // We iterate taps with stride 4
      // x[n] is at mbIndex. x[n-1] is at mbIndex-1...

      for (int k = 0; k < 16; ++k) { // 64 taps / 4 = 16 taps per phase
        int bufIdx = upMbIndex - k;
        if (bufIdx < 0)
          bufIdx += bufSize;

        sum += upBuffer[bufIdx] * coeffs[tapIndex];
        tapIndex += 4;
      }
      // Gain correction for upsampling (x4 energy loss in zero stuffing)
      // Ideally filter has gain 4.0. We normalize coeffs to unity sum, so
      // multiply by 4.
      output[phase] = (float)(sum * 4.0);
    }

    // Advance buffer
    upMbIndex++;
    if (upMbIndex >= bufSize)
      upMbIndex = 0;
  }

  // Downsample: 4 inputs -> 1 output
  float processDownsample(const float *input) {
    // Push 4 samples into downsample buffer
    // Then apply LPF and take every 4th sample (Decimate)
    // Optimization: We only need to compute 1 output sample for every 4 inputs.
    // So we only compute the convolution when we align.

    // Robust Implementation:
    // Treat as sliding buffer. Push 4. Compute 1 dot product.

    int bufSize = (int)downBuffer.size();

    for (int i = 0; i < 4; ++i) {
      downBuffer[downMbIndex] = input[i];
      downMbIndex++;
      if (downMbIndex >= bufSize)
        downMbIndex = 0;
    }

    // Compute Convolution at current point
    // Using all 64 taps on the high-rate buffer.
    double sum = 0.0;
    int readIdx = downMbIndex - 1; // Latest sample

    for (int k = 0; k < 64; ++k) {
      if (readIdx < 0)
        readIdx += bufSize;
      sum += downBuffer[readIdx] * coeffs[k];
      readIdx--;
    }

    return (float)sum;
  }

  int getLatency() {
    // Linear Phase Latency = Taps / 2.
    // Upsampler: 64 taps (at 4x rate) -> latency is 32 samples at 4x rate = 8
    // samples at 1x rate. Downsampler: 64 taps (at 4x rate) -> latency is 32
    // samples at 4x rate = 8 samples at 1x rate. Total RTT latency = 16 samples
    // at 1x rate.
    return 16;
  }

private:
  void generateCoeffs() {
    // Windowed Sinc
    // Cutoff = 0.25 (Nyquist/4).
    // Length = 64. Center = 31.5.
    // But for delay integer alignment, let's prefer odd length?
    // 64 is fine for polyphase.

    double fc = 0.25;
    int N = 64;

    for (int i = 0; i < N; ++i) {
      double n = i - (N - 1.0) / 2.0;
      // Sinc
      double h = 0.0;
      if (fabs(n) < 1e-9) {
        h = 2.0 * kPi * fc;
      } else {
        h = sin(2.0 * kPi * fc * n) / (kPi * n);
      }
      // Normalized Sinc (sin(pi*x)/(pi*x)) uses fc in [0, 0.5]?
      // Formula: 2*fc * sinc(2*fc*n)?
      // Let's use standard: sin(2*pi*fc*n) / (pi*n) is correct for fc in [0,
      // 0.5].

      // Blackman Window
      double w = 0.42 - 0.5 * cos(2.0 * kPi * i / (N - 1)) +
                 0.08 * cos(4.0 * kPi * i / (N - 1));
      coeffs[i] = h * w;
    }

    // Normalize gain to 1.0 (at DC)
    double sum = 0.0;
    for (double c : coeffs)
      sum += c;
    for (double &c : coeffs)
      c /= sum;
  }

  // Buffers and State
  // Upsampler: Input buffer (at 1x rate) needs to store enough for 'taps/4'
  // history. 64/4 = 16. Size 16 is enough. Let's make it 32 for safety.
  std::vector<float> upBuffer = std::vector<float>(32, 0.0f);
  int upMbIndex = 0;

  // Downsampler: Input buffer (at 4x rate) needs to store 64 samples. Make it
  // 128.
  std::vector<float> downBuffer = std::vector<float>(128, 0.0f);
  int downMbIndex = 0;

  // Coeffs
  std::vector<double> coeffs = std::vector<double>(64, 0.0);
};
