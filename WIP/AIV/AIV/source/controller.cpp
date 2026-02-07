//------------------------------------------------------------------------
// Copyright(c) 2026 AIV.
//------------------------------------------------------------------------

#include "controller.h"
#include "cids.h"
#include "params.h"
#include "vstgui/plugin-bindings/vst3editor.h"

using namespace Steinberg;

namespace MyCompanyName {

//------------------------------------------------------------------------
// AIVController Implementation
//------------------------------------------------------------------------
tresult PLUGIN_API AIVController::initialize(FUnknown *context) {
  // Here the Plug-in will be instantiated

  //---do not forget to call parent ------
  tresult result = EditControllerEx1::initialize(context);
  if (result != kResultOk) {
    return result;
  }

  // Register parameters
  using namespace AIV;

  //--- Global Parameters ---
  parameters.addParameter(STR16("Input Gain"), STR16("dB"), 0,
                          Defaults::InputGain, Vst::ParameterInfo::kCanAutomate,
                          kParamInputGain);
  parameters.addParameter(STR16("Output Gain"), STR16("dB"), 0,
                          Defaults::OutputGain,
                          Vst::ParameterInfo::kCanAutomate, kParamOutputGain);
  parameters.addParameter(STR16("Dry/Wet"), STR16("%"), 0, Defaults::DryWet,
                          Vst::ParameterInfo::kCanAutomate, kParamDryWet);

  //--- Gate Parameters ---
  parameters.addParameter(STR16("Gate Enable"), nullptr, 1, 0,
                          Vst::ParameterInfo::kCanAutomate |
                              Vst::ParameterInfo::kIsBypass,
                          kParamGateEnable);
  parameters.addParameter(
      STR16("Gate Threshold"), STR16("dB"), 0, Defaults::GateThreshold,
      Vst::ParameterInfo::kCanAutomate, kParamGateThreshold);
  parameters.addParameter(STR16("Gate Attack"), STR16("ms"), 0,
                          Defaults::GateAttack,
                          Vst::ParameterInfo::kCanAutomate, kParamGateAttack);
  parameters.addParameter(STR16("Gate Hold"), STR16("ms"), 0,
                          Defaults::GateHold, Vst::ParameterInfo::kCanAutomate,
                          kParamGateHold);
  parameters.addParameter(STR16("Gate Release"), STR16("ms"), 0,
                          Defaults::GateRelease,
                          Vst::ParameterInfo::kCanAutomate, kParamGateRelease);
  parameters.addParameter(STR16("Gate Range"), STR16("dB"), 0,
                          Defaults::GateRange, Vst::ParameterInfo::kCanAutomate,
                          kParamGateRange);

  //--- Compressor Parameters ---
  parameters.addParameter(STR16("Comp Enable"), nullptr, 1, 0,
                          Vst::ParameterInfo::kCanAutomate, kParamCompEnable);
  parameters.addParameter(
      STR16("Comp Threshold"), STR16("dB"), 0, Defaults::CompThreshold,
      Vst::ParameterInfo::kCanAutomate, kParamCompThreshold);
  parameters.addParameter(STR16("Comp Ratio"), STR16(":1"), 0,
                          Defaults::CompRatio, Vst::ParameterInfo::kCanAutomate,
                          kParamCompRatio);
  parameters.addParameter(STR16("Comp Attack"), STR16("ms"), 0,
                          Defaults::CompAttack,
                          Vst::ParameterInfo::kCanAutomate, kParamCompAttack);
  parameters.addParameter(STR16("Comp Release"), STR16("ms"), 0,
                          Defaults::CompRelease,
                          Vst::ParameterInfo::kCanAutomate, kParamCompRelease);
  parameters.addParameter(STR16("Comp Makeup"), STR16("dB"), 0,
                          Defaults::CompMakeup,
                          Vst::ParameterInfo::kCanAutomate, kParamCompMakeup);
  parameters.addParameter(STR16("Comp Knee"), STR16("dB"), 0,
                          Defaults::CompKnee, Vst::ParameterInfo::kCanAutomate,
                          kParamCompKnee);

  //--- De-esser Parameters ---
  parameters.addParameter(STR16("DeEss Enable"), nullptr, 1, 0,
                          Vst::ParameterInfo::kCanAutomate,
                          kParamDeEsserEnable);
  parameters.addParameter(STR16("DeEss Frequency"), STR16("Hz"), 0,
                          Defaults::DeEsserFreq,
                          Vst::ParameterInfo::kCanAutomate, kParamDeEsserFreq);
  parameters.addParameter(
      STR16("DeEss Threshold"), STR16("dB"), 0, Defaults::DeEsserThreshold,
      Vst::ParameterInfo::kCanAutomate, kParamDeEsserThreshold);
  parameters.addParameter(STR16("DeEss Range"), STR16("dB"), 0,
                          Defaults::DeEsserRange,
                          Vst::ParameterInfo::kCanAutomate, kParamDeEsserRange);

  //--- EQ Parameters ---
  parameters.addParameter(STR16("EQ Enable"), nullptr, 1, 0,
                          Vst::ParameterInfo::kCanAutomate, kParamEQEnable);
  // Band 1 - Low Shelf
  parameters.addParameter(STR16("EQ Low Gain"), STR16("dB"), 0,
                          Defaults::EQGain, Vst::ParameterInfo::kCanAutomate,
                          kParamEQBand1Gain);
  parameters.addParameter(STR16("EQ Low Freq"), STR16("Hz"), 0,
                          Defaults::EQBand1Freq,
                          Vst::ParameterInfo::kCanAutomate, kParamEQBand1Freq);
  parameters.addParameter(STR16("EQ Low Q"), nullptr, 0, Defaults::EQQ,
                          Vst::ParameterInfo::kCanAutomate, kParamEQBand1Q);
  // Band 2 - Low Mid
  parameters.addParameter(STR16("EQ LMid Gain"), STR16("dB"), 0,
                          Defaults::EQGain, Vst::ParameterInfo::kCanAutomate,
                          kParamEQBand2Gain);
  parameters.addParameter(STR16("EQ LMid Freq"), STR16("Hz"), 0,
                          Defaults::EQBand2Freq,
                          Vst::ParameterInfo::kCanAutomate, kParamEQBand2Freq);
  parameters.addParameter(STR16("EQ LMid Q"), nullptr, 0, Defaults::EQQ,
                          Vst::ParameterInfo::kCanAutomate, kParamEQBand2Q);
  // Band 3 - High Mid
  parameters.addParameter(STR16("EQ HMid Gain"), STR16("dB"), 0,
                          Defaults::EQGain, Vst::ParameterInfo::kCanAutomate,
                          kParamEQBand3Gain);
  parameters.addParameter(STR16("EQ HMid Freq"), STR16("Hz"), 0,
                          Defaults::EQBand3Freq,
                          Vst::ParameterInfo::kCanAutomate, kParamEQBand3Freq);
  parameters.addParameter(STR16("EQ HMid Q"), nullptr, 0, Defaults::EQQ,
                          Vst::ParameterInfo::kCanAutomate, kParamEQBand3Q);
  // Band 4 - High Shelf
  parameters.addParameter(STR16("EQ High Gain"), STR16("dB"), 0,
                          Defaults::EQGain, Vst::ParameterInfo::kCanAutomate,
                          kParamEQBand4Gain);
  parameters.addParameter(STR16("EQ High Freq"), STR16("Hz"), 0,
                          Defaults::EQBand4Freq,
                          Vst::ParameterInfo::kCanAutomate, kParamEQBand4Freq);
  parameters.addParameter(STR16("EQ High Q"), nullptr, 0, Defaults::EQQ,
                          Vst::ParameterInfo::kCanAutomate, kParamEQBand4Q);

  //--- Saturation Parameters ---
  parameters.addParameter(STR16("Sat Enable"), nullptr, 1, 0,
                          Vst::ParameterInfo::kCanAutomate, kParamSatEnable);
  parameters.addParameter(STR16("Sat Drive"), nullptr, 0, Defaults::SatDrive,
                          Vst::ParameterInfo::kCanAutomate, kParamSatDrive);
  parameters.addParameter(STR16("Sat Mix"), STR16("%"), 0, Defaults::SatMix,
                          Vst::ParameterInfo::kCanAutomate, kParamSatMix);
  parameters.addParameter(STR16("Sat Warmth"), nullptr, 0, Defaults::SatWarmth,
                          Vst::ParameterInfo::kCanAutomate, kParamSatWarmth);

  //--- Pitch Parameters ---
  parameters.addParameter(STR16("Pitch Enable"), nullptr, 1, 0,
                          Vst::ParameterInfo::kCanAutomate, kParamPitchEnable);
  parameters.addParameter(STR16("Pitch Speed"), nullptr, 0,
                          Defaults::PitchSpeed,
                          Vst::ParameterInfo::kCanAutomate, kParamPitchSpeed);
  parameters.addParameter(STR16("Pitch Amount"), nullptr, 0,
                          Defaults::PitchAmount,
                          Vst::ParameterInfo::kCanAutomate, kParamPitchAmount);

  //--- Delay Parameters ---
  parameters.addParameter(STR16("Delay Enable"), nullptr, 1, 0,
                          Vst::ParameterInfo::kCanAutomate, kParamDelayEnable);
  parameters.addParameter(STR16("Delay Time L"), STR16("ms"), 0,
                          Defaults::DelayTimeL,
                          Vst::ParameterInfo::kCanAutomate, kParamDelayTimeL);
  parameters.addParameter(STR16("Delay Time R"), STR16("ms"), 0,
                          Defaults::DelayTimeR,
                          Vst::ParameterInfo::kCanAutomate, kParamDelayTimeR);
  parameters.addParameter(
      STR16("Delay Feedback"), STR16("%"), 0, Defaults::DelayFeedback,
      Vst::ParameterInfo::kCanAutomate, kParamDelayFeedback);
  parameters.addParameter(STR16("Delay Mix"), STR16("%"), 0, Defaults::DelayMix,
                          Vst::ParameterInfo::kCanAutomate, kParamDelayMix);
  parameters.addParameter(STR16("Delay Sync"), nullptr, 1, 0,
                          Vst::ParameterInfo::kCanAutomate, kParamDelaySync);
  parameters.addParameter(STR16("Delay HP"), STR16("Hz"), 0, 0,
                          Vst::ParameterInfo::kCanAutomate,
                          kParamDelayHighpass);
  parameters.addParameter(STR16("Delay LP"), STR16("Hz"), 0, 1.0,
                          Vst::ParameterInfo::kCanAutomate, kParamDelayLowpass);

  //--- Reverb Parameters ---
  parameters.addParameter(STR16("Reverb Enable"), nullptr, 1, 0,
                          Vst::ParameterInfo::kCanAutomate, kParamReverbEnable);
  parameters.addParameter(STR16("Reverb Size"), nullptr, 0,
                          Defaults::ReverbSize,
                          Vst::ParameterInfo::kCanAutomate, kParamReverbSize);
  parameters.addParameter(STR16("Reverb Decay"), nullptr, 0,
                          Defaults::ReverbDecay,
                          Vst::ParameterInfo::kCanAutomate, kParamReverbDecay);
  parameters.addParameter(
      STR16("Reverb Predelay"), STR16("ms"), 0, Defaults::ReverbPredelay,
      Vst::ParameterInfo::kCanAutomate, kParamReverbPredelay);
  parameters.addParameter(STR16("Reverb Mix"), STR16("%"), 0,
                          Defaults::ReverbMix, Vst::ParameterInfo::kCanAutomate,
                          kParamReverbMix);
  parameters.addParameter(
      STR16("Reverb Damping"), nullptr, 0, Defaults::ReverbDamping,
      Vst::ParameterInfo::kCanAutomate, kParamReverbDamping);

  //--- Stereo Parameters ---
  parameters.addParameter(STR16("Stereo Enable"), nullptr, 1, 0,
                          Vst::ParameterInfo::kCanAutomate, kParamStereoEnable);
  parameters.addParameter(STR16("Stereo Width"), STR16("%"), 0,
                          Defaults::StereoWidth,
                          Vst::ParameterInfo::kCanAutomate, kParamStereoWidth);
  parameters.addParameter(
      STR16("Stereo Mono Freq"), STR16("Hz"), 0, Defaults::StereoMonoFreq,
      Vst::ParameterInfo::kCanAutomate, kParamStereoMonoFreq);

  //--- Auto Level Parameters ---
  parameters.addParameter(STR16("AutoLevel Enable"), nullptr, 1, 0,
                          Vst::ParameterInfo::kCanAutomate,
                          kParamAutoLevelEnable);
  parameters.addParameter(
      STR16("AutoLevel Target"), STR16("dB"), 0, Defaults::AutoLevelTarget,
      Vst::ParameterInfo::kCanAutomate, kParamAutoLevelTarget);
  parameters.addParameter(
      STR16("AutoLevel Speed"), nullptr, 0, Defaults::AutoLevelSpeed,
      Vst::ParameterInfo::kCanAutomate, kParamAutoLevelSpeed);

  //--- Breath Control Parameters ---
  parameters.addParameter(STR16("Breath Enable"), nullptr, 1, 0,
                          Vst::ParameterInfo::kCanAutomate, kParamBreathEnable);
  parameters.addParameter(
      STR16("Breath Sensitivity"), nullptr, 0, Defaults::BreathSensitivity,
      Vst::ParameterInfo::kCanAutomate, kParamBreathSensitivity);
  parameters.addParameter(
      STR16("Breath Reduction"), STR16("dB"), 0, Defaults::BreathReduction,
      Vst::ParameterInfo::kCanAutomate, kParamBreathReduction);

  return result;
}

//------------------------------------------------------------------------
tresult PLUGIN_API AIVController::terminate() {
  // Here the Plug-in will be de-instantiated, last possibility to remove some
  // memory!

  //---do not forget to call parent ------
  return EditControllerEx1::terminate();
}

//------------------------------------------------------------------------
tresult PLUGIN_API AIVController::setComponentState(IBStream *state) {
  // Here you get the state of the component (Processor part)
  if (!state)
    return kResultFalse;

  return kResultOk;
}

//------------------------------------------------------------------------
tresult PLUGIN_API AIVController::setState(IBStream *state) {
  // Here you get the state of the controller

  return kResultTrue;
}

//------------------------------------------------------------------------
tresult PLUGIN_API AIVController::getState(IBStream *state) {
  // Here you are asked to deliver the state of the controller (if needed)
  // Note: the real state of your plug-in is saved in the processor

  return kResultTrue;
}

//------------------------------------------------------------------------
IPlugView *PLUGIN_API AIVController::createView(FIDString name) {
  // Here the Host wants to open your editor (if you have one)
  if (FIDStringsEqual(name, Vst::ViewType::kEditor)) {
    // create your editor here and return a IPlugView ptr of it
    auto *view = new VSTGUI::VST3Editor(this, "view", "editor.uidesc");
    return view;
  }
  return nullptr;
}

//------------------------------------------------------------------------
} // namespace MyCompanyName
