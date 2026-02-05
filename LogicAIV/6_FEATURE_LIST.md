# AIV Vocal Chain - Comprehensive Feature List & Roadmap

This document outlines the development roadmap and detailed technical specifications for the AIV Vocal Chain, derived from the advanced physics research in `5_PHYSICS_RESEARCH.md`.

## Development Roadmap

### Alpha Version (Core Stability & Basic Physics)
*   **Objective**: Establish the signal flow and implement fundamental "Correct" DSP algorithms.
*   **Features**:
    *   **Architecture**: Linear signal path establishment (Preamp -> Gate -> EQ -> Comp -> Output).
    *   **Preamp**: Tanh() soft saturation (odd harmonics) with Input Gain.
    *   **Subtractive EQ**: 3-Band ZDF (Zero Delay Feedback) implementation to prevent high-frequency cramping.
    *   **FET Compressor**: Basic feedback topology with fixed threshold and Input drive logic. Soft knee modeling.
    *   **Basic Gate**: Standard lookahead gate (without hysteresis initially).
    *   **UI**: Basic slider/knob layout for parameter control.

### Beta Version (Advanced Physics & Psychoacoustics)
*   **Objective**: Implement the advanced psychoacoustic models and time-variant behaviors.
*   **Features**:
    *   **Oversampling**: 4x/8x oversampling in Preamp/Saturation to eliminate aliasing.
    *   **Gate Upgrade**: Implementation of Hysteresis (Dual Thresholds) and Decoupled RMS/Peak detection.
    *   **De-Esser**: Spectral Centroid detection implementation for intelligent sibilance recognition.
    *   **Opto Compressor**: Multi-stage release ("Memory Effect") and wavelength-dependent sidechain.
    *   **Spatial Effects**: 4th-order Lagrange interpolated Delay and Hadamard Matrix FDN Reverb.
    *   **Additive EQ**: Proportional Q filters for "Air" and "Presence".

### Release Version (Safety, Optimization & Polish)
*   **Objective**: Compliance, safety mechanisms, and final user experience.
*   **Features**:
    *   **True Peak Limiter**: Sinc-interpolated lookahead limiting to prevent inter-sample peaks.
    *   **Auto-Makeup Gain**: Dynamic compensation based on Threshold/Ratio inverse curve.
    *   **Optimization**: CPU load reduction for convolution/oversampling.
    *   **Presets**: Factory presets (Prominent, Bright, Warm) calibrated to "Sweet Spot" physics ranges.
    *   **Visuals**: Real-time gain reduction metering and frequency analysis.

---

## Detailed Feature Specifications (Physics, Thresholds, Limits)

### 1. Preamplifier & Saturation
*   **Physics**: Non-linear transfer function using Hyperbolic Tangent ($\tanh$) for "transistor" odd-order harmonics, or Asymmetrical shaping for "tube" even-order harmonics.
*   **Thresholds/Ranges**:
    *   `Input Gain`: -$\infty$ to +24 dB (Drives $k$ coefficient).
    *   `Headroom`: 0 dBFS internal clipping point (soft knee).
*   **Limits/Problems**:
    *   *Aliasing*: Non-linearities expand bandwidth. Requires **Oversampling** (min 4x) + Steep LPF to prevent foldback distortion.
    *   *DC Offset*: Asymmetrical saturation introduces DC bias. Requires a High Pass Filter (DC Blocker) at ~10Hz post-saturation.

### 2. Intelligent Noise Gate
*   **Physics**: State-machine logic with **Hysteresis** (separate Open/Close thresholds) and Decoupled RMS/Peak detection using variable release coefficients.
*   **Thresholds/Ranges**:
    *   `Threshold`: -80 dB to 0 dB.
    *   `Hysteresis`: 0 dB to 12 dB (Safety margin).
    *   `Attack`: 0.01 ms (ultra-fast for plosives) to 100 ms.
*   **Limits/Problems**:
    *   *Chatter*: Rapid open/close cycling on sustaining notes. Solved by Hysteresis and `Hold` time (>50ms).
    *   *Truncation*: Cutting off breath starts. Solved by **Lookahead** buffer (2ms).

### 3. Subtractive EQ (ZDF)
*   **Physics**: **Zero Delay Feedback (TPT)** topology. Solves the implicit delay in standard Direct Form I biquads.
*   **Thresholds/Ranges**:
    *   `HPF`: 20 Hz to 500 Hz (12/18 dB/oct).
    *   `Cut`: -12 dB max (Subtractive philosophy).
*   **Limits/Problems**:
    *   *Frequency Cramping*: Standard BT filters warp near Nyquist. ZDF preserves analog shape up to $f_s/2$.
    *   *Pre-ringing*: Linear phase filters smear transients. **Minimum Phase** is chosen for tighter low-end transient response.

### 4. Intelligent De-Esser
*   **Physics**: Split-band processing with **Spectral Centroid** detection. Triggers based on the rate of change of the spectral center of gravity, not just absolute amplitude.
*   **Thresholds/Ranges**:
    *   `Frequency`: 2 kHz to 12 kHz (Sidechain crossover).
    *   `Centroid Jump`: >2000 Hz delta implies sibilance.
*   **Limits/Problems**:
    *   *Lisping*: Over-compression of sibilance. Solved by `Range` clamp (max -6 to -12 dB reduction).
    *   *False Positives*: Snare hits or cymbals (if applied to mix). Vocal-specific tuning required.

### 5. FET Compressor (AIV 76)
*   **Physics**: Feedback topology with **Fixed Threshold** and variable Input Drive. Resistive FET modeling ($R_{DS} \approx 1/k(V_{GS}-V_P)$).
*   **Thresholds/Ranges**:
    *   `Attack`: 20 $\mu$s to 800 $\mu$s (Microseconds! Not Milliseconds).
    *   `Release`: 50 ms to 1100 ms.
    *   `Ratio`: 4, 8, 12, 20, All (Bias shift).
*   **Limits/Problems**:
    *   *Distortion*: Fast release times on low frequencies (<100 Hz) cause waveform distortion ("pumping").
    *   *Mapping*: Hardware knobs are often reversed (Right = Fast). AIV standardizes this (Right = Fast? Or Standard DAW Left-to-Right?). *Note: Research says AIV uses standard Left=Slow, Right=Fast.*

### 6. Opto Compressor (AIV 2A)
*   **Physics**: **Memory Effect** (Multi-stage release). T4B Photo-cell emulation where $\tau_{rel}$ depends on gain reduction history. Frequency-dependent sidechain (less sensitive to lows).
*   **Thresholds/Ranges**:
    *   `Peak Reduction`: 0-100 (arbitrary scale driving EL panel).
    *   `Release`: Complex variable (60ms initial -> 0.5s-5s tail).
*   **Limits/Problems**:
    *   *Sluggishness*: Too slow for rap/staccato vocals. Intended for leveling (Macro-dynamics), not peak catching. Used in series *after* FET.

### 7. Additive EQ ("Air")
*   **Physics**: **Proportional Q**. Bandwidth narrows as gain increases, preventing phase smear in lower octaves when boosting highs.
*   **Thresholds/Ranges**:
    *   `Air Band`: 8 kHz to 20 kHz (High Shelf).
    *   `Gain`: 0 to +12 dB.
*   **Limits/Problems**:
    *   *Harshness*: Boosting digital highs can amplify aliasing from previous stages. Input to this stage must be clean (hence Oversampling in Preamp).

### 8. Spatial Effects (Delay & Reverb)
*   **Physics**:
    *   *Delay*: 4th-Order **Lagrange Interpolation** for subsample accuracy (pitch-shifting echoes).
    *   *Reverb*: **Feedback Delay Network (FDN)** with Unitary **Hadamard Matrix** ($8 \times 8$) for maximum diffusion.
*   **Thresholds/Ranges**:
    *   `Feedback`: 0-100% (Energy loop).
    *   `Matrix Gain`: Must be $\le 1.0$ (Unitary) to prevent explosion.
*   **Limits/Problems**:
    *   *Metallic Sound*: Short delay lines in FDN cause metallic ringing. Modulating delay lines (LFO) breaks up eigenmodes.
    *   *Zipper Noise*: Modulating delay time without interpolation causes clicking. Lagrange prevents this.

### 9. Output Limiter
*   **Physics**: **True Peak** detection via Sinc Interpolation (4x resampling of sidechain).
*   **Thresholds/Ranges**:
    *   `Ceiling`: -1.0 to 0.0 dBFS.
    *   `Lookahead`: 2.0 ms (ensures perfect catch).
*   **Limits/Problems**:
    *   *Latency*: Lookahead adds latency. Must report latency to host (Logic Pro) for PDC (Plugin Delay Compensation).
    *   *ISP*: Inter-sample peaks can clip DACs even if digital meter shows 0dB. True Peak detection is mandatory for streaming compliance.
