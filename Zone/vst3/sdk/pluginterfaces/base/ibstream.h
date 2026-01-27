//------------------------------------------------------------------------
// Project     : VST SDK
//
// Category    : Interfaces
// Filename    : ibstream.h
// Created by  : Steinberg, 01/2004
// Description : Stream Attributes
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

//------------------------------------------------------------------------
namespace Steinberg {

//------------------------------------------------------------------------
/** Seek mode for streams. */
enum FSeekMode
{
	kSeekSet = 0,	///< from beginning of stream
	kSeekCur,		///< from current position in stream
	kSeekEnd		///< from end of stream
};

//------------------------------------------------------------------------
/** Stream interface.
\ingroup pluginBase */
//------------------------------------------------------------------------
class IBStream : public FUnknown
{
public:
//------------------------------------------------------------------------
	/** Read from stream. */
	virtual tresult PLUGIN_API read (void* buffer, int32 numBytes, int32* numBytesRead = nullptr) = 0;

	/** Write to stream. */
	virtual tresult PLUGIN_API write (void* buffer, int32 numBytes, int32* numBytesWritten = nullptr) = 0;

	/** Seek in stream. */
	virtual tresult PLUGIN_API seek (int64 pos, int32 mode, int64* result = nullptr) = 0;

	/** Get current stream position. */
	virtual tresult PLUGIN_API tell (int64* pos) = 0;

//------------------------------------------------------------------------
	static const FUID iid;
};

DECLARE_CLASS_IID (IBStream, 0xC3BF6EA2, 0x30994762, 0x8D39ED62, 0x298418A3)

//------------------------------------------------------------------------
} // namespace Steinberg
