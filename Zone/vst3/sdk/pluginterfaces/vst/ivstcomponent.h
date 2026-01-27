//------------------------------------------------------------------------
// Project     : VST SDK
//
// Category    : Interfaces
// Filename    : ivstcomponent.h
// Created by  : Steinberg, 04/2005
// Description : VST Component Interfaces
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
#include "pluginterfaces/vst/vstbus.h"
#include "pluginterfaces/base/ibstream.h"

//------------------------------------------------------------------------
namespace Steinberg {
namespace Vst {

//------------------------------------------------------------------------
/** Component interface.
\ingroup vstIPlug
*/
class IComponent : public FUnknown
{
public:
//------------------------------------------------------------------------
	/** Called before initializing the component to get information about the context. */
	virtual tresult PLUGIN_API initialize (FUnknown* context) = 0;

	/** Called after the plug-in is initialized. */
	virtual tresult PLUGIN_API terminate () = 0;

	/** Called before the plug-in is disposed. */
	virtual tresult PLUGIN_API getControllerClassId (TUID classId) = 0;

	/** Called to set the component's state. */
	virtual tresult PLUGIN_API setIoMode (IoMode mode) = 0;

	/** Called to get the current number of buses. */
	virtual int32 PLUGIN_API getBusCount (MediaType type, BusDirection dir) = 0;

	/** Called to get the bus info. */
	virtual tresult PLUGIN_API getBusInfo (MediaType type, BusDirection dir, int32 busIndex, BusInfo& bus) = 0;

	/** Called to get the routing info. */
	virtual tresult PLUGIN_API getRoutingInfo (RoutingInfo& inInfo, RoutingInfo& outInfo) = 0;

	/** Called to activate a bus. */
	virtual tresult PLUGIN_API activateBus (MediaType type, BusDirection dir, int32 index, TBool state) = 0;

	/** Called when the plug-in is activated. */
	virtual tresult PLUGIN_API setActive (TBool state) = 0;

	/** Sets the state of the plug-in. */
	virtual tresult PLUGIN_API setState (IBStream* state) = 0;

	/** Gets the state of the plug-in. */
	virtual tresult PLUGIN_API getState (IBStream* state) = 0;

//------------------------------------------------------------------------
	static const FUID iid;
};

DECLARE_CLASS_IID (IComponent, 0xE89F4828, 0x65774696, 0x8A7E56B2, 0x376495E3)

//------------------------------------------------------------------------
} // namespace Vst
} // namespace Steinberg
