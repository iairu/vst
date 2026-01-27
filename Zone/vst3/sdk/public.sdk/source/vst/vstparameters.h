//------------------------------------------------------------------------
// Project     : VST SDK
//
// Category    : Helpers
// Filename    : public.sdk/source/vst/vstparameters.h
// Created by  : Steinberg, 04/2005
// Description : VST Parameter Implementation
//
//-----------------------------------------------------------------------------
// This file is part of a Steinberg SDK. It is subject to the license terms
// in the LICENSE file found in the top-level directory of this distribution
// and at www.steinberg.net/sdklicenses. 
// No part of the SDK, including this file, may be copied, modified, propagated,
// or distributed except according to the terms contained in the LICENSE file.
//-----------------------------------------------------------------------------

#pragma once

#include "pluginterfaces/vst/ivstparameters.h"
#include <vector>

namespace Steinberg {
namespace Vst {

//------------------------------------------------------------------------
/** Implementation of a parameter.
\ingroup vstClasses
*/
class Parameter
{
public:
	Parameter (const ParameterInfo& info);
	virtual ~Parameter ();

	ParameterInfo& getInfo ();
	void setNormalized (ParamValue v);
	ParamValue getNormalized () const;

	virtual void toString (ParamValue valueNormalized, String128 string) const;
	virtual bool fromString (const String128 string, ParamValue& valueNormalized) const;

protected:
	ParameterInfo info;
	ParamValue value;
};

//------------------------------------------------------------------------
/** Container for parameters.
\ingroup vstClasses
*/
class ParameterContainer
{
public:
	ParameterContainer ();
	virtual ~ParameterContainer ();

	void addParameter (Parameter* p);
	Parameter* getParameter (ParamID id);

	int32 getParameterCount ();
	Parameter* getParameterByIndex (int32 index);

protected:
	std::vector<Parameter*> parameters;
};

//------------------------------------------------------------------------
} // namespace Vst
} // namespace Steinberg
