//------------------------------------------------------------------------
// Project     : VST SDK
//
// Category    : Interfaces
// Filename    : vsttypes.h
// Created by  : Steinberg, 04/2005
// Description : Basic VST types
//
//-----------------------------------------------------------------------------
// This file is part of a Steinberg SDK. It is subject to the license terms
// in the LICENSE file found in the top-level directory of this distribution
// and at www.steinberg.net/sdklicenses. 
// No part of the SDK, including this file, may be copied, modified, propagated,
// or distributed except according to the terms contained in the LICENSE file.
//-----------------------------------------------------------------------------

#pragma once

#include "pluginterfaces/base/fstrdefs.h"

//------------------------------------------------------------------------
namespace Steinberg {
namespace Vst {
//------------------------------------------------------------------------
/** \defgroup vst3typedef VST 3 Data Types */
/*@{*/

//------------------------------------------------------------------------
/** Sample value */
typedef float Sample32;				///< 32-bit float sample
typedef double Sample64;			///< 64-bit float sample

//------------------------------------------------------------------------
/** Audio sample processing precision */
enum SamplePrecision
{
	kSample32,			///< 32-bit float samples
	kSample64			///< 64-bit float samples
};

//------------------------------------------------------------------------
/** Speaker arrangement */
typedef uint64 SpeakerArrangement;

//------------------------------------------------------------------------
/** Musical time in samples.
\see \ref musicalTime */
typedef int64 TQuarterNotes;

//------------------------------------------------------------------------
/** Unit ID */
typedef int32 UnitID;

//------------------------------------------------------------------------
/** Parameter ID */
typedef uint32 ParamID;

//------------------------------------------------------------------------
/** Program / Preset list ID */
typedef int32 ProgramListID;

//------------------------------------------------------------------------
/** Note Expression Type ID */
typedef uint32 NoteExpressionTypeID;

//------------------------------------------------------------------------
/** Automation value */
typedef double ParamValue;

//------------------------------------------------------------------------
/** Bypassed status */
typedef bool TBool;

//------------------------------------------------------------------------
/** Keyswitch */
typedef int32 KeySwitch;

//-------------------------------------------------
/** A four character code (32-bit integer) to identify a class of objects. */
typedef char TUID[16];

//------------------------------------------------------------------------
/** Note-on velocity value (0.0 = no velocity, 1.0 = full velocity) */
typedef double Velocity;

//------------------------------------------------------------------------
/** Tuning value in semitones (0. = default tuning, 1. = one semitone up, -1. = one semitone down) */
typedef double Tuning;

//------------------------------------------------------------------------
/** Generic string, used for names, labels, etc. */
typedef char16 String128[128];

//------------------------------------------------------------------------
/** Path string */
typedef char16 String256[256];

//------------------------------------------------------------------------
/** Version of a plug-in (e.g. 1.0.0.1)
 *  The version is a dotted quad of the form "Major.Minor.Sub.Build".
 */
typedef uint32 Version;

//------------------------------------------------------------------------
/** Unique identifier for a component/unit category */
typedef const char* CString;

//------------------------------------------------------------------------
/** View rectangle */
struct ViewRect
{
	int32 left;
	int32 top;
	int32 right;
	int32 bottom;
};

//------------------------------------------------------------------------
/** MIDI mapping */
struct MidiMapping
{
	int32 a;
	int32 b;
	int32 c;
	int32 d;
};
/*@}*/

//------------------------------------------------------------------------
} // namespace Vst
} // namespace Steinberg
