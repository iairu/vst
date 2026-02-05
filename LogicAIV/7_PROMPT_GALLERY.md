# AIV Vocal Chain - Implementation Prompt Gallery

This gallery contains a series of **Actionable Prompts** designed to guide an AI Agent through the systematic implementation of the AIV Vocal Chain.

Each prompt is derived from the physics research (`5_PHYSICS_RESEARCH.md`) and the feature specifications (`6_FEATURE_LIST.md`). They are organized by development phase (Alpha, Beta, Release) and tailored to the current project structure.

> **Usage:** Copy and paste the text within the code blocks one by one to your AI assistant.

---

## Phase 1: Alpha (Core Physics & Architecture)

### Prompt 1.1: Preamp & Saturation Logic
```markdown
I need to implement the Preamplifier and Saturation stage for the AIV Vocal Chain based on the physics specification in "6_FEATURE_LIST.md" (Section 1).

**Target Files:**
- `Shared/AudioUnit/Support/AIVDSPKernel.hpp` (Processing logic)
- `Shared/AudioUnit/AIVDemoParameters.swift` (Parameter definitions)

**Instructions:**
1.  **Define Parameters**: Ensure `inputGain` (-inf to +24dB), `saturation` (0-100%), and `phaseInvert` (Bool) are defined in `AIVDemoParameters.swift` and mapped in `AIVDSPKernel.hpp`.
2.  **Implement Physics**: In the `process()` loop of `AIVDSPKernel.hpp`:
    - Apply `inputGain`.
    - Implement the **Hyperbolic Tangent** transfer function: `y = tanh(k * x) / tanh(k)`.
    - `k` should be derived from `inputGain`.
    - If `phaseInvert` is true, multiply by -1.0.
3.  **Mix**: Implement a Dry/Wet blend based on the `saturation` parameter.

**Verification:**
After applying changes, run the following command to verify the build. If there are compilation errors, fix them iteratively until the build succeeds.
`xcodebuild -scheme "AIV macOS" -destination "platform=macOS,arch=arm64" clean build`
```

### Prompt 1.2: Zero Delay Feedback (ZDF) EQ
```markdown
I need to implement the Subtractive EQ module using Zero Delay Feedback (TPT) topology to prevent high-frequency cramping, as detailed in "6_FEATURE_LIST.md" (Section 3).

**Target Files:**
- `Shared/AudioUnit/Support/AIVDSPClasses.hpp` (Create new ZDF Filter class here)
- `Shared/AudioUnit/Support/AIVDSPKernel.hpp` (Integration)

**Instructions:**
1.  **Create ZDF Class**: In `AIVDSPClasses.hpp`, define a `ZDFFilter` class (or struct) that implements the Trapezoidal Integration method:
    - `g = tan(PI * frequency / sampleRate)`
    - `v = (x - s) / (1 + g)`
    - `y = v * g + s`
    - `s = v * 2 * g + s`
2.  **Filter Types**: Support `HighPass` (12dB/oct) and `Peaking` (Bell) types within this class.
3.  **Integration**: In `AIVDSPKernel.hpp`:
    - Instantiate 3 ZDF filters: `HPF`, `LowMidCut`, `HighPass` (Safety).
    - Map parameters from `AIVDemoParameters`: `eq1Freq`, `eq1Gain`, `eq1Q`, etc.
4.  **Process**: Insert this EQ block immediately after the Preamp stage.

**Verification:**
After applying changes, run the following command to verify the build. If there are compilation errors, fix them iteratively until the build succeeds.
`xcodebuild -scheme "AIV macOS" -destination "platform=macOS,arch=arm64" clean build`
```

### Prompt 1.3: FET Compressor (AIV 76)
```markdown
I need to implement the 'AIV 76' FET Compressor core using the fixed-threshold feedback topology described in "6_FEATURE_LIST.md" (Section 5).

**Target Files:**
- `Shared/AudioUnit/Support/AIVDSPKernel.hpp`

**Instructions:**
1.  **Logic**: Implement a compressor where the Threshold is **Fixed** (e.g., -20dB internally).
2.  **Input Drive**: The `Input` parameter should apply gain *before* the detector loop effectively pushing the signal into the threshold.
3.  **Attack/Release**:
    - Attack: 20 microseconds to 800 microseconds (Use `exp(-1 / (time * sampleRate))`).
    - Release: 50ms to 1100ms.
4.  **Gain Reduction**: Calculate GR based on the detector signal.
5.  **Output**: Apply `Output` (Makeup) gain after compression.
6.  **Ratio**: Implement 4:1, 8:1, 12:1, 20:1 logic.

**Verification:**
After applying changes, run the following command to verify the build. If there are compilation errors, fix them iteratively until the build succeeds.
`xcodebuild -scheme "AIV macOS" -destination "platform=macOS,arch=arm64" clean build`
```

---

## Phase 2: Beta (Psychoacoustics & Advanced DSP)

### Prompt 2.1: Intelligent Gate with Hysteresis
```markdown
I need to upgrade the basic noise gate to include Hysteresis (Dual Thresholds) as specified in "6_FEATURE_LIST.md" (Section 2).

**Target Files:**
- `Shared/AudioUnit/Support/AIVDSPKernel.hpp`

**Instructions:**
1.  **State Machine**: Implement a state variable `isGateOpen`.
2.  **Thresholds**:
    - `openThreshold` = User Parameter.
    - `closeThreshold` = User Parameter - `Hysteresis` (e.g., 6dB lower).
3.  **Logic**:
    - If `isGateOpen` is true: switch to false ONLY if `signal < closeThreshold`.
    - If `isGateOpen` is false: switch to true ONLY if `signal > openThreshold`.
4.  **Envelope**: Use a decoupled peak detector (Fast Attack, user-defined Release) for the sidechain signal.

**Verification:**
After applying changes, run the following command to verify the build. If there are compilation errors, fix them iteratively until the build succeeds.
`xcodebuild -scheme "AIV macOS" -destination "platform=macOS,arch=arm64" clean build`
```

### Prompt 2.2: Spectral De-Esser
```markdown
I need to implement the Spectral Centroid De-Esser logic as defined in "6_FEATURE_LIST.md" (Section 4).

**Target Files:**
- `Shared/AudioUnit/Support/AIVDSPKernel.hpp`

**Instructions:**
1.  **Spectral Centroid**: Implement the simplified time-domain approximation or a zero-crossing rate estimator to detect high-frequency dominance.
    - Alternatively, implement a split-band architecture: `HighBand = Signal - LowPass(Signal)`.
2.  **Detection**: If `HighBand` energy > `Threshold`, apply gain reduction to the `HighBand` ONLY, then sum back with `LowBand`.
3.  **Range**: Ensure the gain reduction never exceeds the `Range` parameter (e.g., max -6dB reduction).

**Verification:**
After applying changes, run the following command to verify the build. If there are compilation errors, fix them iteratively until the build succeeds.
`xcodebuild -scheme "AIV macOS" -destination "platform=macOS,arch=arm64" clean build`
```

### Prompt 2.3: FDN Reverb & Lagrange Delay
```markdown
I need to implement the Spatial Effects module using Feedback Delay Networks (FDN) and Lagrange Interpolation for the Delay, per "6_FEATURE_LIST.md" (Section 8).

**Target Files:**
- `Shared/AudioUnit/Support/AIVDSPClasses.hpp` (Add DelayLine class with Lagrange interpolation)
- `Shared/AudioUnit/Support/AIVDSPKernel.hpp`

**Instructions:**
1.  **Weighted Interpolation**: Implement a 4th-order Lagrange interpolator for the Delay read pointer to allow smooth time modulation.
2.  **FDN Reverb**:
    - Create 8 parallel delay lines of prime number lengths.
    - Connect them via an **8x8 Hadamard Matrix** (Unitary feedback matrix).
    - Apply LowPass filtering in the feedback loop equivalent to the `Damp` parameter.
3.  **Mix**: Implement Wet/Dry mixing for both Delay and Reverb stages.

**Verification:**
After applying changes, run the following command to verify the build. If there are compilation errors, fix them iteratively until the build succeeds.
`xcodebuild -scheme "AIV macOS" -destination "platform=macOS,arch=arm64" clean build`
```

---

## Phase 3: Release (Safety & Polish)

### Prompt 3.1: True Peak Limiting & Auto-Makeup
```markdown
I need to implement the final Output Stage with True Peak Limiting and Auto-Makeup Gain as described in "6_FEATURE_LIST.md" (Sections 9 & 11).

**Target Files:**
- `Shared/AudioUnit/Support/AIVDSPKernel.hpp`

**Instructions:**
1.  **Auto-Makeup**:
    - In the FET and Opto compressors, calculate the theoretical attenuation `A` at 0dB input.
    - Apply `Makeup = 10^(A/20)` automatically if the Auto-Makeup flag is true.
2.  **True Peak Limiter**:
    - Implement a Lookahead buffer (2ms).
    - Implement a Sinc interpolator (or 4x OS simulation) to check for inter-sample peaks in the sidechain.
    - Apply gain reduction to strictly ceil the signal at -0.1 dB (or user parameter).

**Verification:**
After applying changes, run the following command to verify the build. If there are compilation errors, fix them iteratively until the build succeeds.
`xcodebuild -scheme "AIV macOS" -destination "platform=macOS,arch=arm64" clean build`
```

### Prompt 3.2: Oversampling Architecture
```markdown
I need to implement 4x Oversampling for the Saturation and Compressor stages to reduce aliasing, as specified in the "Safety" section of "6_FEATURE_LIST.md".

**Target Files:**
- `Shared/AudioUnit/Support/AIVDSPKernel.hpp`

**Instructions:**
1.  **Upsampling**: Implement a polyphase FIR interpolator or zero-stuffing + steep LPF to increase sample rate to 4x.
2.  **Processing**: Run the Preamp/Sat and FET Compressor code within the 4x loop.
3.  **Downsampling**: Implement a steep Anti-Aliasing filter and decimate back to the host sample rate.
4.  **Latency**: Report the added latency to the host via `kAudioUnitProperty_Latency`.

**Verification:**
After applying changes, run the following command to verify the build. If there are compilation errors, fix them iteratively until the build succeeds.
`xcodebuild -scheme "AIV macOS" -destination "platform=macOS,arch=arm64" clean build`
```
