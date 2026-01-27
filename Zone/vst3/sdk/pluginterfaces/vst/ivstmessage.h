//------------------------------------------------------------------------
// Project     : VST SDK
//
// Category    : Interfaces
// Filename    : ivstmessage.h
// Created by  : Steinberg, 04/2005
// Description : VST Message Interfaces
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

namespace Steinberg {
namespace Vst {

//------------------------------------------------------------------------
/** Message interface.
\ingroup vstIPlug
*/
class IMessage : public FUnknown
{
public:
	/** Returns the message ID. */
	virtual const char* PLUGIN_API getMessageID () = 0;

	/** Returns the attributes. */
	virtual IAttributeList* PLUGIN_API getAttributes () = 0;

	static const FUID iid;
};

DECLARE_CLASS_IID (IMessage, 0x07A44498, 0x81734563, 0x82828C09, 0xAAD378A6)

//------------------------------------------------------------------------
/** Connection point interface.
\ingroup vstIPlug
*/
class IConnectionPoint : public FUnknown
{
public:
	/** Connects this connection point to another one. */
	virtual tresult PLUGIN_API connect (IConnectionPoint* other) = 0;

	/** Disconnects this connection point from another one. */
	virtual tresult PLUGIN_API disconnect (IConnectionPoint* other) = 0;

	/** Receives a message. */
	virtual tresult PLUGIN_API notify (IMessage* message) = 0;

	static const FUID iid;
};

DECLARE_CLASS_IID (IConnectionPoint, 0x70A11346, 0x49814104, 0x82828C09, 0xAAD378A6)

//------------------------------------------------------------------------
/** Attribute list interface.
\ingroup vstIPlug
*/
class IAttributeList : public FUnknown
{
public:
	/** Sets an integer attribute. */
	virtual tresult PLUGIN_API setInt (const char* id, int64 value) = 0;

	/** Gets an integer attribute. */
	virtual tresult PLUGIN_API getInt (const char* id, int64& value) = 0;

	/** Sets a float attribute. */
	virtual tresult PLUGIN_API setFloat (const char* id, double value) = 0;

	/** Gets a float attribute. */
	virtual tresult PLUGIN_API getFloat (const char* id, double& value) = 0;

	/** Sets a string attribute. */
	virtual tresult PLUGIN_API setString (const char* id, const TChar* string) = 0;

	/** Gets a string attribute. */
	virtual tresult PLUGIN_API getString (const char* id, TChar* string, uint32 size) = 0;

	/** Sets a binary data attribute. */
	virtual tresult PLUGIN_API setBinary (const char* id, const void* data, uint32 size) = 0;

	/** Gets a binary data attribute. */
	virtual tresult PLUGIN_API getBinary (const char* id, const void*& data, uint32& size) = 0;

	static const FUID iid;
};

DECLARE_CLASS_IID (IAttributeList, 0x099F84AB, 0x7E38425E, 0x82828C09, 0xAAD378A6)

//------------------------------------------------------------------------
} // namespace Vst
} // namespace Steinberg
