//------------------------------------------------------------------------
// Project     : VST SDK
//
// Category    : Interfaces
// Filename    : vstbus.h
// Created by  : Steinberg, 04/2005
// Description : VST Bus Interfaces
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

//------------------------------------------------------------------------
namespace Steinberg {
namespace Vst {

//------------------------------------------------------------------------
/** Bus media types. */
enum MediaType
{
	kAudio = 0,		///< audio bus
	kEvent,			///< event bus
	kNumMediaTypes
};

//------------------------------------------------------------------------
/** Bus directions. */
enum BusDirection
{
	kInput = 0,		///< input bus
	kOutput,		///< output bus
};

//------------------------------------------------------------------------
/** Bus types. */
enum BusType
{
	kMain,			///< main bus
	kAux,			///< auxiliary bus
};

//------------------------------------------------------------------------
/** Bus information.
\ingroup vst3struct
*/
struct BusInfo
{
	MediaType mediaType;		///< media type
	BusDirection direction;		///< input or output
	int32 channelCount;			///< number of channels
	String128 name;				///< bus name
	BusType busType;			///< main or aux
	uint32 flags;				///< flags
};

//------------------------------------------------------------------------
/** Routing information.
\ingroup vst3struct
*/
struct RoutingInfo
{
	MediaType mediaType;		///< media type
	int32 busIndex;				///< bus index
	int32 channel;				///< channel index
};

//------------------------------------------------------------------------
/** I/O mode. */
enum IoMode
{
	kSimple,
	kAdvanced,
	kOffline
};

//------------------------------------------------------------------------
} // namespace Vst
} // namespace Steinberg
