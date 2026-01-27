//------------------------------------------------------------------------
// Project     : VST SDK
//
// Category    : Interfaces
// Filename    : ivstevents.h
// Created by  : Steinberg, 04/2005
// Description : VST Events Interfaces
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
/** Event types. */
enum EventTypes
{
	kNoteOnEvent = 0,		///< Note-on event
	kNoteOffEvent,			///< Note-off event
	kDataEvent,				///< Data event
	kPolyPressureEvent,		///< Polyphonic aftertouch
	kNoteExpressionValueEvent,///< Note expression value event
	kNoteExpressionTextEvent, ///< Note expression text event
	kChordEvent,			///< Chord event
	kScaleEvent,			///< Scale event
	kNumEventTypes
};

//------------------------------------------------------------------------
/** Event structure.
\ingroup vst3struct
*/
struct Event
{
	int32 busIndex;				///< bus index
	int32 sampleOffset;			///< sample offset
	TQuarterNotes ppqPosition;	///< project time position
	uint16 flags;				///< flags
	uint16 type;				///< event type (see EventTypes)

	union
	{
		struct
		{
			int16 channel;			///< channel
			int16 pitch;			///< pitch
			float velocity;			///< velocity
			int32 length;			///< length in samples
			int32 noteId;			///< note identifier
		} noteOn;
		struct
		{
			int16 channel;			///< channel
			int16 pitch;			///< pitch
			float velocity;			///< velocity
			int32 noteId;			///< note identifier
		} noteOff;
	};
};

//------------------------------------------------------------------------
/** Event list interface.
\ingroup vstIPlug
*/
class IEventList : public FUnknown
{
public:
	/** Returns the number of events. */
	virtual int32 PLUGIN_API getEventCount () = 0;

	/** Returns an event. */
	virtual tresult PLUGIN_API getEvent (int32 index, Event& e) = 0;

	/** Adds an event. */
	virtual tresult PLUGIN_API addEvent (Event& e) = 0;

	static const FUID iid;
};

DECLARE_CLASS_IID (IEventList, 0x33A06493, 0x83244112, 0x9559A127, 0x4A58726F)

//------------------------------------------------------------------------
} // namespace Vst
} // namespace Steinberg
