//------------------------------------------------------------------------
// Project     : VST SDK
//
// Category    : Helpers
// Filename    : public.sdk/source/vst/vstaudioeffect.h
// Created by  : Steinberg, 04/2005
// Description : VST Audio Effect Implementation
//
//-----------------------------------------------------------------------------
// This file is part of a Steinberg SDK. It is subject to the license terms
// in the LICENSE file found in the top-level directory of this distribution
// and at www.steinberg.net/sdklicenses. 
// No part of the SDK, including this file, may be copied, modified, propagated,
// or distributed except according to the terms contained in the LICENSE file.
//-----------------------------------------------------------------------------

#pragma once

#include "pluginterfaces/vst/ivstaudioprocessor.h"
#include "pluginterfaces/vst/ivstcomponent.h"
#include "pluginterfaces/vst/ivsteditcontroller.h"
#include "public.sdk/source/vst/vstcomponentbase.h"

namespace Steinberg {
namespace Vst {

//------------------------------------------------------------------------
/** Default implementation of an audio effect.
\ingroup vstClasses
*/
class AudioEffect : public ComponentBase, public IAudioProcessor
{
public:
//------------------------------------------------------------------------
	AudioEffect ();

	//---from IAudioProcessor----------------------------------
	tresult PLUGIN_API setBusArrangements (SpeakerArrangement* inputs, int32 numIns,
	                                         SpeakerArrangement* outputs, int32 numOuts) override;
	tresult PLUGIN_API getBusArrangement (BusDirection dir, int32 index,
	                                         SpeakerArrangement& arr) override;
	tresult PLUGIN_API canProcessSampleSize (int32 symbolicSampleSize) override;
	uint32 PLUGIN_API getLatencySamples () override;
	uint32 PLUGIN_API getTailSamples () override;
	tresult PLUGIN_API setupProcessing (ProcessSetup& setup) override;
	tresult PLUGIN_API setProcessing (TBool state) override;
	tresult PLUGIN_API process (ProcessData& data) override;
	tresult PLUGIN_API setSampleRate (ParamValue sampleRate) override;

	//---from ComponentBase-------------------------------------
	tresult PLUGIN_API activateBus (MediaType type, BusDirection dir, int32 index,
	                                  TBool state) override;
//------------------------------------------------------------------------
	OBJ_METHODS (AudioEffect, ComponentBase)
	DEFINE_INTERFACES
		DEF_INTERFACE (IAudioProcessor)
	END_DEFINE_INTERFACES (ComponentBase)

protected:
	ProcessSetup processSetup;
	bool bypass;
};

//------------------------------------------------------------------------
} // namespace Vst
} // namespace Steinberg
