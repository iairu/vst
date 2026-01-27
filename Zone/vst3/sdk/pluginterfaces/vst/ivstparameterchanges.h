//------------------------------------------------------------------------
// Project     : VST SDK
//
// Category    : Interfaces
// Filename    : ivstparameterchanges.h
// Created by  : Steinberg, 04/2005
// Description : VST Parameter Changes Interfaces
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

//------------------------------------------------------------------------
namespace Steinberg {
namespace Vst {

//------------------------------------------------------------------------
/** Point in a parameter change ramp.
\ingroup vstIPlug
*/
struct ParameterValueQueue
{
	ParamID paramID;
	int32 numPoints;
	IParamValueQueue* points;
};

//------------------------------------------------------------------------
/** Queue of parameter values.
\ingroup vstIPlug
*/
class IParamValueQueue : public FUnknown
{
public:
	/** Returns the parameter ID. */
	virtual ParamID PLUGIN_API getParameterId () = 0;

	/** Returns the number of points. */
	virtual int32 PLUGIN_API getPointCount () = 0;

	/** Returns a point. */
	virtual tresult PLUGIN_API getPoint (int32 index, int32& sampleOffset, ParamValue& value) = 0;

	/** Adds a point. */
	virtual tresult PLUGIN_API addPoint (int32 sampleOffset, ParamValue value, int32& index) = 0;

	static const FUID iid;
};

DECLARE_CLASS_IID (IParamValueQueue, 0x01263548, 0x78574092, 0x89D91A12, 0x4B58726F)

//------------------------------------------------------------------------
/** Changes of a parameter.
\ingroup vstIPlug
*/
class IParameterChanges : public FUnknown
{
public:
	/** Returns the number of parameters. */
	virtual int32 PLUGIN_API getParameterCount () = 0;

	/** Returns the queue for a given index. */
	virtual IParamValueQueue* PLUGIN_API getParameterData (int32 index) = 0;

	/** Adds a new parameter queue. */
	virtual IParamValueQueue* PLUGIN_API addParameterData (const ParamID* paramID, int32& index) = 0;

	static const FUID iid;
};

DECLARE_CLASS_IID (IParameterChanges, 0xBB947352, 0x11794C5B, 0x93C4362A, 0x7E12B852)

//------------------------------------------------------------------------
} // namespace Vst
} // namespace Steinberg
