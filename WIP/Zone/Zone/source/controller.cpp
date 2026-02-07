//------------------------------------------------------------------------
// Copyright(c) 2026 .
//------------------------------------------------------------------------

#include "controller.h"
#include "base/source/fstreamer.h"
#include "cids.h"
#include "vstgui/plugin-bindings/vst3editor.h"

using namespace Steinberg;

namespace MyCompanyName {

//------------------------------------------------------------------------
// ZoneController Implementation
//------------------------------------------------------------------------
tresult PLUGIN_API ZoneController::initialize(FUnknown *context) {
  // Here the Plug-in will be instantiated

  //---do not forget to call parent ------
  tresult result = EditControllerEx1::initialize(context);
  if (result != kResultOk) {
    return result;
  }

  // Register parameters
  // Shimmer Amount (0-100%)
  parameters.addParameter(STR16("Shimmer"), STR16("%"), 0,
                          0.3, // default value
                          Vst::ParameterInfo::kCanAutomate,
                          kParamShimmerAmount);

  // Chorus Depth (0-100%)
  parameters.addParameter(STR16("Chorus Depth"), STR16("%"), 0,
                          0.5, // default value
                          Vst::ParameterInfo::kCanAutomate, kParamChorusDepth);

  // Chorus Rate (0.1-3.0 Hz)
  parameters.addParameter(STR16("Chorus Rate"), STR16("Hz"), 0,
                          0.5, // default value (maps to ~1.5 Hz)
                          Vst::ParameterInfo::kCanAutomate, kParamChorusRate);

  // Saturation (0-100%)
  parameters.addParameter(STR16("Saturation"), STR16("%"), 0,
                          0.4, // default value
                          Vst::ParameterInfo::kCanAutomate, kParamSaturation);

  // Reverb Mix (0-100%)
  parameters.addParameter(STR16("Reverb Mix"), STR16("%"), 0,
                          0.4, // default value
                          Vst::ParameterInfo::kCanAutomate, kParamReverbMix);

  // Reverb Decay (0-100%)
  parameters.addParameter(STR16("Reverb Decay"), STR16("%"), 0,
                          0.6, // default value
                          Vst::ParameterInfo::kCanAutomate, kParamReverbDecay);

  // Stereo Width (0-200%, 100% = normal)
  parameters.addParameter(STR16("Stereo Width"), STR16("%"), 0,
                          0.5, // default value (100%)
                          Vst::ParameterInfo::kCanAutomate, kParamStereoWidth);

  // Master Mix (0-100%)
  parameters.addParameter(STR16("Master Mix"), STR16("%"), 0,
                          0.5, // default value
                          Vst::ParameterInfo::kCanAutomate, kParamMasterMix);

  return result;
}

//------------------------------------------------------------------------
tresult PLUGIN_API ZoneController::terminate() {
  // Here the Plug-in will be de-instantiated, last possibility to remove some
  // memory!

  //---do not forget to call parent ------
  return EditControllerEx1::terminate();
}

//------------------------------------------------------------------------
tresult PLUGIN_API ZoneController::setComponentState(IBStream *state) {
  // Here you get the state of the component (Processor part)
  if (!state)
    return kResultFalse;

  IBStreamer streamer(state, kLittleEndian);

  float fval;
  if (streamer.readFloat(fval))
    setParamNormalized(kParamShimmerAmount, fval);
  if (streamer.readFloat(fval))
    setParamNormalized(kParamChorusDepth, fval);
  if (streamer.readFloat(fval))
    setParamNormalized(kParamChorusRate, fval);
  if (streamer.readFloat(fval))
    setParamNormalized(kParamSaturation, fval);
  if (streamer.readFloat(fval))
    setParamNormalized(kParamReverbMix, fval);
  if (streamer.readFloat(fval))
    setParamNormalized(kParamReverbDecay, fval);
  if (streamer.readFloat(fval))
    setParamNormalized(kParamStereoWidth, fval);
  if (streamer.readFloat(fval))
    setParamNormalized(kParamMasterMix, fval);

  return kResultOk;
}

//------------------------------------------------------------------------
tresult PLUGIN_API ZoneController::setState(IBStream *state) {
  // Here you get the state of the controller

  return kResultTrue;
}

//------------------------------------------------------------------------
tresult PLUGIN_API ZoneController::getState(IBStream *state) {
  // Here you are asked to deliver the state of the controller (if needed)
  // Note: the real state of your plug-in is saved in the processor

  return kResultTrue;
}

//------------------------------------------------------------------------
IPlugView *PLUGIN_API ZoneController::createView(FIDString name) {
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
