//------------------------------------------------------------------------
// Project     : VST SDK
//
// Category    : Interfaces
// Filename    : ivstparameters.h
// Created by  : Steinberg, 04/2005
// Description : VST Parameter Interfaces
//
//-----------------------------------------------------------------------------
// This file is part of a Steinberg SDK. It is subject to the license terms
// in the LICENSE file found in the top-level directory of this distribution
// and at www.steinberg.net/sdklicenses. 
// No part of the SDK, including this file, may be copied, modified, propagated,
// or distributed except according to the terms contained in the LICENSE file.
//-----------------------------------------------------------------------------

#pragma once

#include "pluginterfaces/vst/vsttypes.h"
#include "pluginterfaces/base/funknown.h"

//------------------------------------------------------------------------
namespace Steinberg {
namespace Vst {

//------------------------------------------------------------------------
/** Parameter Info
\ingroup vst3struct
*/
struct ParameterInfo
{
	ParamID id;					///< parameter identifier
	String128 title;			///< parameter title
	String128 shortTitle;		///< parameter short title
	String128 units;			///< parameter units
	int32 stepCount;			///< number of discrete steps (0 for continuous)
	ParamValue defaultNormalizedValue; ///< default value in range [0, 1]
	UnitID unitId;				///< associated unit
	int32 flags;				///< flags
};

//------------------------------------------------------------------------
/** Parameter flags. */
enum ParameterFlags
{
	kCanAutomate = 1 << 0,		///< parameter can be automated
	kIsReadOnly = 1 << 1,		///< parameter is read-only
	kIsWrapAround = 1 << 2,		///< parameter wraps around (e.g. rotary controllers)
	kIsList = 1 << 3,			///< parameter is a list of discrete values
	kIsProgramChange = 1 << 4,	///< parameter is a program change
	kIsBypass = 1 << 5,			///< parameter is a bypass
};

//------------------------------------------------------------------------
/** Unit Info
\ingroup vst3struct
*/
struct UnitInfo
{
	UnitID id;					///< unit identifier
	UnitID parentUnitId;		///< parent unit identifier
	String128 name;				///< unit name
	ProgramListID programListId;///< program list identifier
};

//------------------------------------------------------------------------
/** Program List Info
\ingroup vst3struct
*/
struct ProgramListInfo
{
	ProgramListID id;			///< program list identifier
	String128 name;				///< program list name
	int32 programCount;			///< number of programs
};

//------------------------------------------------------------------------
} // namespace Vst
} // namespace Steinberg
