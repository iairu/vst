//------------------------------------------------------------------------
// Project     : VST SDK
//
// Category    : Interfaces
// Filename    : ivsteditcontroller.h
// Created by  : Steinberg, 04/2005
// Description : VST Edit Controller Interfaces
//
//-----------------------------------------------------------------------------
// This file is part of a Steinberg SDK. It is subject to the license terms
// in the LICENSE file found in the top-level directory of this distribution
// and at www.steinberg.net/sdklicenses. 
// No part of the SDK, including this file, may be copied, modified, propagated,
// or distributed except according to the terms contained in the LICENSE file.
//-----------------------------------------------------------------------------

#pragma once

#include "pluginterfaces/base/funknown.h"
#include "pluginterfaces/vst/vsttypes.h"
#include "pluginterfaces/gui/iplugview.h"

//------------------------------------------------------------------------
namespace Steinberg {
namespace Vst {

//------------------------------------------------------------------------
/** Edit controller interface.
\ingroup vstIPlug
*/
class IEditController : public FUnknown
{
public:
//------------------------------------------------------------------------
	/** Receives the component state. */
	virtual tresult PLUGIN_API setComponentState (IBStream* state) = 0;

	/** Sets the controller state. */
	virtual tresult PLUGIN_API setState (IBStream* state) = 0;

	/** Gets the controller state. */
	virtual tresult PLUGIN_API getState (IBStream* state) = 0;

	/** Returns the number of parameters. */
	virtual int32 PLUGIN_API getParameterCount () = 0;

	/** Gets parameter information. */
	virtual tresult PLUGIN_API getParameterInfo (int32 paramIndex, ParameterInfo& info) = 0;

	/** Gets parameter display text. */
	virtual tresult PLUGIN_API getParamStringByValue (ParamID id, ParamValue valueNormalized, String128 string) = 0;

	/** Gets parameter value from display text. */
	virtual tresult PLUGIN_API getParamValueByString (ParamID id, String128 string, ParamValue& valueNormalized) = 0;

	/** Normalizes a parameter value. */
	virtual ParamValue PLUGIN_API normalizedParamToPlain (ParamID id, ParamValue valueNormalized) = 0;

	/** Denormalizes a parameter value. */
	virtual ParamValue PLUGIN_API plainParamToNormalized (ParamID id, ParamValue plainValue) = 0;

	/** Gets the normalized value of a parameter. */
	virtual ParamValue PLUGIN_API getParamNormalized (ParamID id) = 0;

	/** Sets the normalized value of a parameter. */
	virtual tresult PLUGIN_API setParamNormalized (ParamID id, ParamValue value) = 0;

	/** This is the MIDI mapping interface. */
	virtual tresult PLUGIN_API setComponentHandler (IComponentHandler* handler) = 0;

	/** Creates the editor view. */
	virtual IPlugView* PLUGIN_API createView (FIDString name) = 0;

//------------------------------------------------------------------------
	static const FUID iid;
};

DECLARE_CLASS_IID (IEditController, 0x1A4F4711, 0x8C444B62, 0x9599A127, 0x4A58726F)

//------------------------------------------------------------------------
/** Component handler interface.
\ingroup vstIPlug
*/
class IComponentHandler : public FUnknown
{
public:
//------------------------------------------------------------------------
	/** Begins a parameter editing gesture. */
	virtual tresult PLUGIN_API beginEdit (ParamID id) = 0;

	/** Performs a parameter edit. */
	virtual tresult PLUGIN_API performEdit (ParamID id, ParamValue valueNormalized) = 0;

	/** Ends a parameter editing gesture. */
	virtual tresult PLUGIN_API endEdit (ParamID id) = 0;

	/** Restarts the component. */
	virtual tresult PLUGIN_API restartComponent (int32 flags) = 0;

//------------------------------------------------------------------------
	static const FUID iid;
};

DECLARE_CLASS_IID (IComponentHandler, 0x93A06493, 0x83244112, 0x9559A127, 0x4A58726F)

//------------------------------------------------------------------------
} // namespace Vst
} // namespace Steinberg
