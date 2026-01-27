//------------------------------------------------------------------------
// Project     : VST SDK
//
// Category    : Interfaces
// Filename    : ustring.h
// Created by  : Steinberg, 01/2004
// Description : Unicode string classes
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

namespace Steinberg {

//------------------------------------------------------------------------
/** Basic Unicode string.
\ingroup pluginBase */
//------------------------------------------------------------------------
class UString
{
public:
//------------------------------------------------------------------------
	UString (const char16* wStr = nullptr, bool adopt = false);
	UString (const UString& wStr);
	~UString ();

	UString& operator= (const UString& wStr);
	UString& operator= (const char16* wStr);

	bool operator== (const UString& str) const;
	bool operator!= (const UString& str) const;

	void assign (const char16* wStr, bool adopt = false);
	void assign (const UString& wStr);

	const char16* getChar16 () const { return str; }

	uint32 length () const;

	void setChar16 (char16 c, uint32 index);
	char16 getChar16 (uint32 index) const;

protected:
//------------------------------------------------------------------------
	char16* str;
	bool wide;
};

//------------------------------------------------------------------------
/** Unicode string with a fixed size buffer.
\ingroup pluginBase */
//------------------------------------------------------------------------
template <int32 size>
class UStringBuffer : public UString
{
public:
	UStringBuffer (const char16* wStr = 0);
	UStringBuffer (const UString& wStr);

	UStringBuffer<size>& operator= (const UString& wStr);
	UStringBuffer<size>& operator= (const char16* wStr);

protected:
	char16 buffer[size];
};

//------------------------------------------------------------------------
} // namespace Steinberg
