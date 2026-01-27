//------------------------------------------------------------------------
// Project     : VST SDK
//
// Category    : Helpers
// Filename    : public.sdk/source/vst/vsteditcontroller.h
// Created by  : Steinberg, 04/2005
// Description : VST Edit Controller Implementation
//
//-----------------------------------------------------------------------------
// This file is part of a Steinberg SDK. It is subject to the license terms
// in the LICENSE file found in the top-level directory of this distribution
// and at www.steinberg.net/sdklicenses. 
// No part of the SDK, including this file, may be copied, modified, propagated,
// or distributed except according to the terms contained in the LICENSE file.
//-----------------------------------------------------------------------------

#pragma once

#include "pluginterfaces/vst/ivsteditcontroller.h"
#include "pluginterfaces/vst/ivstcomponent.h"
#include "public.sdk/source/vst/vstbus.h"
#include "public.sdk/source/vst/vstparameters.h"
#include "pluginterfaces/base/fplatform.h"
#include "pluginterfaces/base/ustring.h"

namespace Steinberg {
namespace Vst {

//------------------------------------------------------------------------
/** Default implementation of an edit controller.
\ingroup vstClasses
*/
class EditController : public IEditController, public IConnectionPoint
{
public:
//------------------------------------------------------------------------
	EditController ();
	virtual ~EditController ();

	//---from IEditController---------------------------------
	tresult PLUGIN_API setComponentState (IBStream* state) override;
	tresult PLUGIN_API setState (IBStream* state) override;
	tresult PLUGIN_API getState (IBStream* state) override;
	int32 PLUGIN_API getParameterCount () override;
	tresult PLUGIN_API getParameterInfo (int32 paramIndex, ParameterInfo& info) override;
	tresult PLUGIN_API getParamStringByValue (ParamID id, ParamValue valueNormalized, String128 string) override;
	tresult PLUGIN_API getParamValueByString (ParamID id, String128 string, ParamValue& valueNormalized) override;
	ParamValue PLUGIN_API normalizedParamToPlain (ParamID id, ParamValue valueNormalized) override;
	ParamValue PLUGIN_API plainParamToNormalized (ParamID id, ParamValue plainValue) override;
	ParamValue PLUGIN_API getParamNormalized (ParamID id) override;
	tresult PLUGIN_API setParamNormalized (ParamID id, ParamValue value) override;
	tresult PLUGIN_API setComponentHandler (IComponentHandler* handler) override;
	IPlugView* PLUGIN_API createView (FIDString name) override;

	//---from IConnectionPoint---------------------------------
	tresult PLUGIN_API connect (IConnectionPoint* other) override;
	tresult PLUGIN_API disconnect (IConnectionPoint* other) override;
	tresult PLUGIN_API notify (IMessage* message) override;

protected:
	ParameterContainer parameters;
	IComponentHandler* componentHandler;
};

//------------------------------------------------------------------------
} // namespace Vst
} // namespace Steinberg
