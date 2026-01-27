//------------------------------------------------------------------------
// Project     : VST SDK
//
// Category    : Helpers
// Filename    : public.sdk/source/vst/vstcomponentbase.h
// Created by  : Steinberg, 04/2005
// Description : VST Component Base Implementation
//
//-----------------------------------------------------------------------------
// This file is part of a Steinberg SDK. It is subject to the license terms
// in the LICENSE file found in the top-level directory of this distribution
// and at www.steinberg.net/sdklicenses. 
// No part of the SDK, including this file, may be copied, modified, propagated,
// or distributed except according to the terms contained in the LICENSE file.
//-----------------------------------------------------------------------------

#pragma once

#include "pluginterfaces/vst/ivstcomponent.h"
#include "pluginterfaces/base/fplatform.h"
#include "pluginterfaces/base/ustring.h"

namespace Steinberg {
namespace Vst {

//------------------------------------------------------------------------
/** Base class for VST component implementations.
\ingroup vstClasses
*/
class ComponentBase : public IComponent
{
public:
//------------------------------------------------------------------------
	ComponentBase ();

	//---from IComponent------------------------------------------
	tresult PLUGIN_API initialize (FUnknown* context) override;
	tresult PLUGIN_API terminate () override;
	tresult PLUGIN_API getControllerClassId (TUID classId) override;
	tresult PLUGIN_API setIoMode (IoMode mode) override;
	int32 PLUGIN_API getBusCount (MediaType type, BusDirection dir) override;
	tresult PLUGIN_API getBusInfo (MediaType type, BusDirection dir, int32 busIndex, BusInfo& bus) override;
	tresult PLUGIN_API getRoutingInfo (RoutingInfo& inInfo, RoutingInfo& outInfo) override;
	tresult PLUGIN_API activateBus (MediaType type, BusDirection dir, int32 index, TBool state) override;
	tresult PLUGIN_API setActive (TBool state) override;
	tresult PLUGIN_API setState (IBStream* state) override;
	tresult PLUGIN_API getState (IBStream* state) override;

//------------------------------------------------------------------------
	OBJ_METHODS (ComponentBase, FObject)
	DEFINE_INTERFACES
		DEF_INTERFACE (IComponent)
	END_DEFINE_INTERFACES (FObject)
	REFCOUNT_METHODS(FObject)

protected:
	//--------------------------------------------------------------------
	HostApplication* hostApplication;
	TUID controllerClassId;
};

//------------------------------------------------------------------------
} // namespace Vst
} // namespace Steinberg
