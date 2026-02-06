# Audio Cross-Normalization Research

This document aggregates the physics code (DSP algorithms) from the AIV Vocal Chain to support research into "Cross-Normalization"—a strategy to ensure consistent audio levels, prevent clipping, and eliminate unwanted artifacts (mudiness/screechiness) across the entire signal chain.

## 1. DSP Kernel Routing (Signal Chain)
The core processing loop from `AIVDSPKernel.hpp`:

```cpp
// --- CORE PROCESS LOOP (4x Oversampled) ---
for (int k = 0; k < 4; ++k) {
  float s = osBlock[k];

  // 1. Preamp & Saturation (tanh)
  float inputSample = s;
  float linearSample = inputSample * mInputGainLin;
  float k_val = (mInputGainLin < 0.01f) ? 0.01f : mInputGainLin;
  float wetSample = std::tanh(k_val * inputSample) / std::tanh(k_val);
  float drySample = linearSample;
  float mix = mSaturation / 100.0f;
  s = (1.0f - mix) * drySample + mix * wetSample;

  // Phase Invert
  if (mPhaseInvert) s = -s;

  // 2. Noise Gate
  if (mGateEnable) s = mGate[channel].process(s);

  // 3. Auto Level (AGC)
  s = mAutoLevel[channel].process(s);

  // 4. Pitch
  if (mPitchEnable) s = mPitch[channel].process(s);

  // 5. Deesser
  if (mDeesserEnable) s = mDeesser[channel].process(s);

  // 6. EQ (Safety HPF -> HPF -> LowMidCut -> HighShelf -> LPF)
  if (mEQEnable) {
    s = mSafetyHPF[channel].process(s);
    s = mHPF[channel].process(s);
    s = mLowMidCut[channel].process(s); // Peaking Cut
    s = mEQBand3[channel].process(s);   // High Shelf
    s = mLPF[channel].process(s);
  }

  // 7. Compressor (FET)
  if (mCompEnable) s = mCompressor[channel].process(s);

  // 8. Saturator (Module)
  if (mSatEnable) s = mSaturator[channel].process(s);

  processedBlock[k] = s;
}

// --- DOWNSAMPLE (4 -> 1) ---
sample = mOversampler[channel].processDownsample(processedBlock);

// --- POST PROCESS (1x) ---

// 9. Delay
if (mDelayEnable) sample = mDelay[channel].process(sample);

// 10. Reverb
if (mReverbEnable) sample = mReverb[channel].process(sample);

// 11. Limiter (TruePeak)
if (mLimiterEnable) sample = mLimiter[channel].process(sample);

// 12. Global Gain
out[frameIndex] = sample * mGain;
```

## 2. Module Physics (DSP Algorithms)

### Auto Level (AGC)
```cpp
float process(float input) {
  double absInput = fabs(input);
  // Envelope follower
  if (absInput > envelope)
    envelope = attackCoef * (envelope - absInput) + absInput;
  else
    envelope = releaseCoef * (envelope - absInput) + absInput;

  if (envelope < 0.0001) return input;

  // Calculate required gain
  double currentDb = 20.0 * log10(envelope);
  double targetDbVal = 20.0 * log10(targetLevel);
  double gainDb = targetDbVal - currentDb;

  // Clamp gain
  if (gainDb > 20.0 * log10(maxGain)) gainDb = 20.0 * log10(maxGain);
  if (gainDb < -20.0 * log10(maxGain)) gainDb = -20.0 * log10(maxGain);

  double gain = pow(10.0, gainDb / 20.0);
  return input * gain;
}
```

### Noise Gate
```cpp
float process(float input) {
  float absInput = fabs(input);
  // Envelope Follower
  if (absInput > envelope)
    envelope = attackCoeff * (envelope - absInput) + absInput;
  else
    envelope = releaseCoeff * (envelope - absInput) + absInput;

  // State Machine (Simplified)
  // ... Open/Close logic ...

  double targetGain = isGateOpen ? 1.0 : rangeFactor;
  // Smooth Gain
  if (targetGain > currentGain)
    currentGain = attackCoeff * (currentGain - targetGain) + targetGain;
  else
    currentGain = releaseCoeff * (currentGain - targetGain) + targetGain;

  return input * (float)currentGain;
}
```

### Deesser
```cpp
float process(float input) {
  float lowBand = crossoverFilter.process(input);
  float highBand = input - lowBand;
  double absHigh = fabs(highBand);

  // Envelope
  if (absHigh > envelope)
    envelope = attack * (envelope - absHigh) + absHigh;
  else
    envelope = release * (envelope - absHigh) + absHigh;

  // Gain Reduction
  double gain = 1.0;
  if (envelope > threshold) {
    gain = pow(envelope / threshold, 1.0 / ratio - 1.0);
  }
  if (gain < maxAttenuation) gain = maxAttenuation;

  return lowBand + (highBand * gain);
}
```

### FET Compressor
```cpp
float process(float input) {
  float drivenSignal = input * inputGain;
  float absInput = fabs(drivenSignal);

  // Peak Detector
  if (absInput > envelope)
    envelope = attackCoeff * (envelope - absInput) + absInput;
  else
    envelope = releaseCoeff * (envelope - absInput) + absInput;

  // Gain Reduction
  double gain = 1.0;
  if (envelope > threshold) {
    gain = pow(envelope / threshold, 1.0 / ratio - 1.0);
  }

  return (float)(drivenSignal * gain * makeupGain);
}
```

### Saturator
```cpp
float process(float input) {
  float x = input * drive;
  if (type == 0) { // Soft Clip
    if (x > 1.0f) x = 1.0f;
    else if (x < -1.0f) x = -1.0f;
    else x = x - (x * x * x) / 3.0f;
  } else { // Hard Clip (Tube/Fuzz)
    x = std::tanh(x);
  }
  return x;
}
```

### True Peak Limiter
```cpp
float process(float input) {
  // 1. Write to Lookahead Buffer
  buffer[writeIndex] = input;

  // 2. Read Delayed Output
  int readIndex = writeIndex - lookaheadDelay;
  if (readIndex < 0) readIndex += buffer.size();
  float delayedOutput = buffer[readIndex];

  // 3. Detect True Peak (4x Sinc Approx / Cubic)
  // ... interpolation logic ...
  float maxPeak = ...; 

  // 4. Update Gain Reduction Envelope
  double targetGain = 1.0;
  if (maxPeak > ceiling) {
    targetGain = ceiling / maxPeak;
  }

  // Release Logic (Instant Attack)
  if (targetGain < envelope) envelope = targetGain;
  else envelope = releaseCoeff * (envelope - targetGain) + targetGain;

  // 5. Apply Gain to Delayed Output
  float out = delayedOutput * (float)envelope;
  writeIndex++;
  return out;
}
```

### Pitch Shifter (Granular)
```cpp
float process(float input) {
  buffer[writeIndex] = input;
  // Dual tap read with crossfade
  // ...
  float output = out1 * env1 + out2 * (1.0f - env1);
  // Phase increment
  phase += (1.0 - pitchRatio);
  // ...
  return output;
}
```

### Delay & Reverb
(Standard implementations: Delay uses Lagrange interpolation; Reverb uses FDN with Hadamard matrix).

---

## 3. PROMPT for Audio Cross-Normalization Research

**Context:**
I am building a vocal processing chain (AIV) with the following modules in order: **Input/Preamp -> Gate -> AutoLevel -> Pitch -> Deesser -> EQ -> Compressor -> Saturator -> Delay -> Reverb -> Limiter**. Currently, user controls can easily push gain stages into clipping or cause spectral imbalances ("screechiness" in highs, "mudiness" in lows) between modules.

**Objective:**
I need a "Cross-Normalization" strategy that intelligently manages gain and spectral balance dynamically across the entire chain to prevent clipping and ensure a polished sound, regardless of user settings.

**Request:**
Using the physics code provided above, please generate a research report and implementation strategy that addresses:

1.  **Clipping Fix (Priority)**:
    - Analyze the gain structure. Where are the high-risk points?
    - Propose a "Safety/Cross-Normal" algorithm that can be inserted between modules (or control module parameters) to guarantee headroom.
    - Example: Should the Auto-Level communicate with the Compressor? Should the EQ have auto-gain?

2.  **Spectral Balance (Fixing Mud/Screech)**:
    - How can we prevent "Mudiness" (200-500Hz buildup from Saturation/Comp)?
    - How can we prevent "Screechiness" (2-5kHz buildup from Distortion/Eq)?
    - Propose a dynamic spectral control (e.g., dynamic EQ or sidechain logic) that "normalizes" the tone curve to a reference "Ideal Vocal" curve.

3.  **Implementation Plan**:
    - Provide C++ psuedocode for a `CrossNormalizer` class that monitors signal levels at multiple tap points (Pre-EQ, Post-EQ, Post-Comp) and applies corrective gain/EQ coefficients in real-time.

**Goal**: "Never Clips. Never Screeches. Never Muddy."
