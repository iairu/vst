//------------------------------------------------------------------------
// Copyright(c) 2026 .
//------------------------------------------------------------------------

#include "processor.h"
#include "cids.h"

#include "base/source/fstreamer.h"
#include "pluginterfaces/vst/ivstparameterchanges.h"

#include <algorithm>
#include <cmath>

using namespace Steinberg;

namespace MyCompanyName {

// Define the static constexpr member
constexpr int ZoneProcessor::kReverbDelayTimes[kReverbDelayLines];

//------------------------------------------------------------------------
// ZoneProcessor
//------------------------------------------------------------------------
ZoneProcessor::ZoneProcessor() {
  //--- set the wanted controller for our processor
  setControllerClass(kZoneControllerUID);
}

//------------------------------------------------------------------------
ZoneProcessor::~ZoneProcessor() {}

//------------------------------------------------------------------------
void ZoneProcessor::clearDelayBuffers() {
  // Clear chorus delays
  std::fill(chorusDelayL.begin(), chorusDelayL.end(), 0.0f);
  std::fill(chorusDelayR.begin(), chorusDelayR.end(), 0.0f);
  chorusWritePos = 0;
  chorusLfoPhase = 0.0f;

  // Clear shimmer delays
  std::fill(shimmerDelayL.begin(), shimmerDelayL.end(), 0.0f);
  std::fill(shimmerDelayR.begin(), shimmerDelayR.end(), 0.0f);
  shimmerWritePos = 0;
  shimmerFilterL = 0.0f;
  shimmerFilterR = 0.0f;

  // Clear reverb delays
  for (int i = 0; i < kReverbDelayLines; i++) {
    std::fill(reverbDelayL[i].begin(), reverbDelayL[i].end(), 0.0f);
    std::fill(reverbDelayR[i].begin(), reverbDelayR[i].end(), 0.0f);
    reverbWritePos[i] = 0;
  }
  reverbFilterL = 0.0f;
  reverbFilterR = 0.0f;
}

//------------------------------------------------------------------------
float ZoneProcessor::softClip(float x, float amount) {
  // Soft saturation using tanh-like waveshaping
  float drive = 1.0f + amount * 5.0f;
  x *= drive;
  // Fast tanh approximation
  float x2 = x * x;
  return x * (27.0f + x2) / (27.0f + 9.0f * x2);
}

//------------------------------------------------------------------------
tresult PLUGIN_API ZoneProcessor::initialize(FUnknown *context) {
  // Here the Plug-in will be instantiated

  //---always initialize the parent-------
  tresult result = AudioEffect::initialize(context);
  // if everything Ok, continue
  if (result != kResultOk) {
    return result;
  }

  //--- create Audio IO ------
  addAudioInput(STR16("Stereo In"), Steinberg::Vst::SpeakerArr::kStereo);
  addAudioOutput(STR16("Stereo Out"), Steinberg::Vst::SpeakerArr::kStereo);

  /* If you don't need an event bus, you can remove the next line */
  addEventInput(STR16("Event In"), 1);

  return kResultOk;
}

//------------------------------------------------------------------------
tresult PLUGIN_API ZoneProcessor::terminate() {
  // Here the Plug-in will be de-instantiated, last possibility to remove some
  // memory!

  //---do not forget to call parent ------
  return AudioEffect::terminate();
}

//------------------------------------------------------------------------
tresult PLUGIN_API ZoneProcessor::setActive(TBool state) {
  //--- called when the Plug-in is enable/disable (On/Off) -----
  if (state) {
    // Allocate delay buffers
    chorusDelayL.resize(kChorusDelayLength, 0.0f);
    chorusDelayR.resize(kChorusDelayLength, 0.0f);

    shimmerDelayL.resize(kMaxDelayLength, 0.0f);
    shimmerDelayR.resize(kMaxDelayLength, 0.0f);

    for (int i = 0; i < kReverbDelayLines; i++) {
      reverbDelayL[i].resize(kReverbDelayTimes[i], 0.0f);
      reverbDelayR[i].resize(kReverbDelayTimes[i], 0.0f);
    }

    clearDelayBuffers();
  } else {
    // Free delay buffers
    chorusDelayL.clear();
    chorusDelayR.clear();
    shimmerDelayL.clear();
    shimmerDelayR.clear();
    for (int i = 0; i < kReverbDelayLines; i++) {
      reverbDelayL[i].clear();
      reverbDelayR[i].clear();
    }
  }

  return AudioEffect::setActive(state);
}

//------------------------------------------------------------------------
tresult PLUGIN_API ZoneProcessor::process(Vst::ProcessData &data) {
  //--- First : Read inputs parameter changes-----------
  if (data.inputParameterChanges) {
    int32 numParamsChanged = data.inputParameterChanges->getParameterCount();
    for (int32 index = 0; index < numParamsChanged; index++) {
      if (auto *paramQueue =
              data.inputParameterChanges->getParameterData(index)) {
        Vst::ParamValue value;
        int32 sampleOffset;
        int32 numPoints = paramQueue->getPointCount();

        // Get the last point (most recent value)
        if (paramQueue->getPoint(numPoints - 1, sampleOffset, value) ==
            kResultOk) {
          switch (paramQueue->getParameterId()) {
          case kParamShimmerAmount:
            fShimmerAmount = static_cast<float>(value);
            break;
          case kParamChorusDepth:
            fChorusDepth = static_cast<float>(value);
            break;
          case kParamChorusRate:
            fChorusRate = static_cast<float>(value);
            break;
          case kParamSaturation:
            fSaturation = static_cast<float>(value);
            break;
          case kParamReverbMix:
            fReverbMix = static_cast<float>(value);
            break;
          case kParamReverbDecay:
            fReverbDecay = static_cast<float>(value);
            break;
          case kParamStereoWidth:
            fStereoWidth = static_cast<float>(value);
            break;
          case kParamMasterMix:
            fMasterMix = static_cast<float>(value);
            break;
          }
        }
      }
    }
  }

  //--- Process audio
  if (data.numSamples > 0 && data.numInputs > 0 && data.numOutputs > 0) {
    // Get input/output buffers with mono fallback
    int32 numInChannels = data.inputs[0].numChannels;
    int32 numOutChannels = data.outputs[0].numChannels;

    float *inL = data.inputs[0].channelBuffers32[0];
    float *inR = (numInChannels > 1) ? data.inputs[0].channelBuffers32[1] : inL;
    float *outL = data.outputs[0].channelBuffers32[0];
    float *outR =
        (numOutChannels > 1) ? data.outputs[0].channelBuffers32[1] : outL;

    // Constants for processing
    const float pi = 3.14159265358979323846f;
    const float chorusLfoInc =
        (0.1f + fChorusRate * 2.9f) / static_cast<float>(sampleRate);
    const float chorusMaxDelay =
        fChorusDepth * 0.003f * static_cast<float>(sampleRate); // Max 3ms
    const float reverbFeedback = 0.6f + fReverbDecay * 0.35f;

    for (int32 sample = 0; sample < data.numSamples; sample++) {
      float dryL = inL[sample];
      float dryR = inR[sample];
      float wetL = dryL;
      float wetR = dryR;

      // === CHORUS / DETUNE ===
      if (fChorusDepth > 0.001f) {
        // Write to chorus delay
        chorusDelayL[chorusWritePos] = wetL;
        chorusDelayR[chorusWritePos] = wetR;

        // LFO modulation
        float lfo1 = std::sin(chorusLfoPhase * 2.0f * pi);
        float lfo2 = std::sin((chorusLfoPhase + 0.5f) * 2.0f * pi);

        // Calculate delay times
        float delay1 = 20.0f + (1.0f + lfo1) * chorusMaxDelay;
        float delay2 = 20.0f + (1.0f + lfo2) * chorusMaxDelay;

        // Read from chorus delay with interpolation
        float readPos1 = chorusWritePos - delay1;
        float readPos2 = chorusWritePos - delay2;
        if (readPos1 < 0)
          readPos1 += kChorusDelayLength;
        if (readPos2 < 0)
          readPos2 += kChorusDelayLength;

        int idx1 = static_cast<int>(readPos1);
        int idx2 = (idx1 + 1) % kChorusDelayLength;
        float frac1 = readPos1 - idx1;

        int idx3 = static_cast<int>(readPos2);
        int idx4 = (idx3 + 1) % kChorusDelayLength;
        float frac2 = readPos2 - idx3;

        float chorus1L =
            chorusDelayL[idx1] * (1.0f - frac1) + chorusDelayL[idx2] * frac1;
        float chorus2L =
            chorusDelayL[idx3] * (1.0f - frac2) + chorusDelayL[idx4] * frac2;
        float chorus1R =
            chorusDelayR[idx1] * (1.0f - frac1) + chorusDelayR[idx2] * frac1;
        float chorus2R =
            chorusDelayR[idx3] * (1.0f - frac2) + chorusDelayR[idx4] * frac2;

        // Mix chorus voices
        float chorusMix = fChorusDepth * 0.5f;
        wetL = wetL * (1.0f - chorusMix) +
               (chorus1L + chorus2R) * 0.5f * chorusMix;
        wetR = wetR * (1.0f - chorusMix) +
               (chorus2L + chorus1R) * 0.5f * chorusMix;

        // Advance write position and LFO
        chorusWritePos = (chorusWritePos + 1) % kChorusDelayLength;
        chorusLfoPhase += chorusLfoInc;
        if (chorusLfoPhase >= 1.0f)
          chorusLfoPhase -= 1.0f;
      }

      // === SHIMMER (high-frequency content enhancement) ===
      if (fShimmerAmount > 0.001f) {
        // High-pass filter to extract high frequencies
        float shimmerCoeff = 0.95f;
        float highPassL = wetL - shimmerFilterL;
        float highPassR = wetR - shimmerFilterR;
        shimmerFilterL =
            shimmerFilterL * shimmerCoeff + wetL * (1.0f - shimmerCoeff);
        shimmerFilterR =
            shimmerFilterR * shimmerCoeff + wetR * (1.0f - shimmerCoeff);

        // Add the shimmer delay feedback
        int shimmerReadPos =
            (shimmerWritePos + kMaxDelayLength - 2000) % kMaxDelayLength;
        float shimmerL = shimmerDelayL[shimmerReadPos] * 0.5f * fShimmerAmount;
        float shimmerR = shimmerDelayR[shimmerReadPos] * 0.5f * fShimmerAmount;

        // Write to shimmer delay (high frequencies + feedback)
        shimmerDelayL[shimmerWritePos] = highPassL * 0.3f + shimmerL * 0.6f;
        shimmerDelayR[shimmerWritePos] = highPassR * 0.3f + shimmerR * 0.6f;
        shimmerWritePos = (shimmerWritePos + 1) % kMaxDelayLength;

        // Add shimmer to wet signal
        wetL += shimmerL;
        wetR += shimmerR;
      }

      // === SATURATION ===
      if (fSaturation > 0.001f) {
        wetL = softClip(wetL, fSaturation);
        wetR = softClip(wetR, fSaturation);
      }

      // === REVERB ===
      if (fReverbMix > 0.001f) {
        float reverbL = 0.0f;
        float reverbR = 0.0f;

        // Read from all delay lines
        for (int i = 0; i < kReverbDelayLines; i++) {
          int readPos = (reverbWritePos[i] + 1) % kReverbDelayTimes[i];
          reverbL += reverbDelayL[i][readPos];
          reverbR += reverbDelayR[i][readPos];
        }
        reverbL *= 0.25f;
        reverbR *= 0.25f;

        // Low-pass filter reverb tail
        float lpCoeff = 0.3f;
        reverbFilterL = reverbFilterL * (1.0f - lpCoeff) + reverbL * lpCoeff;
        reverbFilterR = reverbFilterR * (1.0f - lpCoeff) + reverbR * lpCoeff;

        // Write to delay lines with feedback (Hadamard-like mixing)
        float inputL = wetL * 0.5f + reverbFilterL * reverbFeedback;
        float inputR = wetR * 0.5f + reverbFilterR * reverbFeedback;

        for (int i = 0; i < kReverbDelayLines; i++) {
          // Different mixing for each delay line
          float sign = (i % 2 == 0) ? 1.0f : -1.0f;
          reverbDelayL[i][reverbWritePos[i]] =
              inputL * sign + inputR * (1.0f - sign);
          reverbDelayR[i][reverbWritePos[i]] =
              inputR * sign + inputL * (1.0f - sign);
          reverbWritePos[i] = (reverbWritePos[i] + 1) % kReverbDelayTimes[i];
        }

        // Mix reverb
        wetL = wetL * (1.0f - fReverbMix) + reverbFilterL * fReverbMix;
        wetR = wetR * (1.0f - fReverbMix) + reverbFilterR * fReverbMix;
      }

      // === STEREO WIDTH ===
      if (std::abs(fStereoWidth - 0.5f) > 0.001f) {
        // Mid-side processing
        float mid = (wetL + wetR) * 0.5f;
        float side = (wetL - wetR) * 0.5f;

        // Adjust width (0=mono, 0.5=normal, 1=wide)
        float widthMult = fStereoWidth * 2.0f; // 0 to 2
        side *= widthMult;

        wetL = mid + side;
        wetR = mid - side;
      }

      // === MASTER MIX ===
      outL[sample] = dryL * (1.0f - fMasterMix) + wetL * fMasterMix;
      outR[sample] = dryR * (1.0f - fMasterMix) + wetR * fMasterMix;
    }

    data.outputs[0].silenceFlags = 0;
  } else if (data.numSamples > 0 && data.numOutputs > 0) {
    // No input, clear output
    for (int32 c = 0; c < data.outputs[0].numChannels; c++) {
      memset(data.outputs[0].channelBuffers32[c], 0,
             data.numSamples * sizeof(float));
    }
    data.outputs[0].silenceFlags =
        ((uint64)1 << data.outputs[0].numChannels) - 1;
  }

  return kResultOk;
}

//------------------------------------------------------------------------
tresult PLUGIN_API ZoneProcessor::setupProcessing(Vst::ProcessSetup &newSetup) {
  //--- called before any processing ----
  sampleRate = newSetup.sampleRate;
  return AudioEffect::setupProcessing(newSetup);
}

//------------------------------------------------------------------------
tresult PLUGIN_API
ZoneProcessor::canProcessSampleSize(int32 symbolicSampleSize) {
  // by default kSample32 is supported
  if (symbolicSampleSize == Vst::kSample32)
    return kResultTrue;

  return kResultFalse;
}

//------------------------------------------------------------------------
tresult PLUGIN_API ZoneProcessor::setState(IBStream *state) {
  // called when we load a preset, the model has to be reloaded
  IBStreamer streamer(state, kLittleEndian);

  // Read all parameters
  if (!streamer.readFloat(fShimmerAmount))
    return kResultFalse;
  if (!streamer.readFloat(fChorusDepth))
    return kResultFalse;
  if (!streamer.readFloat(fChorusRate))
    return kResultFalse;
  if (!streamer.readFloat(fSaturation))
    return kResultFalse;
  if (!streamer.readFloat(fReverbMix))
    return kResultFalse;
  if (!streamer.readFloat(fReverbDecay))
    return kResultFalse;
  if (!streamer.readFloat(fStereoWidth))
    return kResultFalse;
  if (!streamer.readFloat(fMasterMix))
    return kResultFalse;

  return kResultOk;
}

//------------------------------------------------------------------------
tresult PLUGIN_API ZoneProcessor::getState(IBStream *state) {
  // here we need to save the model
  IBStreamer streamer(state, kLittleEndian);

  // Write all parameters
  streamer.writeFloat(fShimmerAmount);
  streamer.writeFloat(fChorusDepth);
  streamer.writeFloat(fChorusRate);
  streamer.writeFloat(fSaturation);
  streamer.writeFloat(fReverbMix);
  streamer.writeFloat(fReverbDecay);
  streamer.writeFloat(fStereoWidth);
  streamer.writeFloat(fMasterMix);

  return kResultOk;
}

//------------------------------------------------------------------------
} // namespace MyCompanyName
