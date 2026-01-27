//------------------------------------------------------------------------
// Project     : VST SDK
//
// Category    : Helpers
// Filename    : base/source/fstreamer.h
// Created by  : Steinberg, 01/2004
// Description : Streamer helper
//
//-----------------------------------------------------------------------------
// This file is part of a Steinberg SDK. It is subject to the license terms
// in the LICENSE file found in the top-level directory of this distribution
// and at www.steinberg.net/sdklicenses. 
// No part of the SDK, including this file, may be copied, modified, propagated,
// or distributed except according to the terms contained in the LICENSE file.
//-----------------------------------------------------------------------------

#pragma once

#include "pluginterfaces/base/ibstream.h"

namespace Steinberg {

//------------------------------------------------------------------------
/** Streamer helper class.
\ingroup pluginBase
*/
class BStreamer
{
public:
	BStreamer (IBStream* stream, bool byteOrder);
	~BStreamer ();

	bool ok () const { return stream != 0; }

	bool readInt8 (int8&);
	bool readInt16 (int16&);
	bool readInt32 (int32&);
	bool readInt64 (int64&);

	bool writeInt8 (const int8);
	bool writeInt16 (const int16);
	bool writeInt32 (const int32);
	bool writeInt64 (const int64);

protected:
	IBStream* stream;
	bool byteOrder;
};

//------------------------------------------------------------------------
} // namespace Steinberg
