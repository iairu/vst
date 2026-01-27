//------------------------------------------------------------------------
// Project     : VST SDK
//
// Category    : Interfaces
// Filename    : funknown.h
// Created by  : Steinberg, 01/2004
// Description : Basic COM-like interface
//
//-----------------------------------------------------------------------------
// This file is part of a Steinberg SDK. It is subject to the license terms
// in the LICENSE file found in the top-level directory of this distribution
// and at www.steinberg.net/sdklicenses. 
// No part of the SDK, including this file, may be copied, modified, propagated,
// or distributed except according to the terms contained in the LICENSE file.
//-----------------------------------------------------------------------------

#pragma once

#include "pluginterfaces/base/fbasedefs.h"

//--------------------------------------------------------------------
// FUID definition
//--------------------------------------------------------------------
namespace Steinberg
{
	struct FUID
	{
		uint32 data1;
		uint32 data2;
		uint32 data3;
		uint32 data4;

		bool operator== (const FUID& other) const
		{
			return (data1 == other.data1 && data2 == other.data2 &&
					data3 == other.data3 && data4 == other.data4);
		}
	};

	#define DECLARE_UID(a,b,c,d) {a,b,c,d}

	//--------------------------------------------------------------------
	/** Base class for all interfaces.
	\ingroup pluginBase */
	//--------------------------------------------------------------------
	class FUnknown
	{
	public:
		virtual tresult PLUGIN_API queryInterface (const TUID _iid, void** obj) = 0;
		virtual uint32 PLUGIN_API addRef () = 0;
		virtual uint32 PLUGIN_API release () = 0;
	};

#define DECLARE_CLASS_IID(ClassName,a,b,c,d) \
	static const FUID iid = DECLARE_UID(a,b,c,d);

} // namespace Steinberg

//--------------------------------------------------------------------
#define PLUGIN_API

#define BEGIN_QUERY_INTERFACE(ClassName) \
	tresult PLUGIN_API queryInterface(const Steinberg::TUID _iid, void** obj) override \
	{ \
		if (Steinberg::FUnknown::iid == _iid) \
		{ \
			*obj = static_cast<Steinberg::FUnknown*>(this); \
			addRef(); \
			return Steinberg::kResultOk; \
		}

#define QUERY_INTERFACE(InterfaceName) \
		else if (InterfaceName::iid == _iid) \
		{ \
			*obj = static_cast<InterfaceName*>(this); \
			addRef(); \
			return Steinberg::kResultOk; \
		}

#define END_QUERY_INTERFACE \
		*obj = nullptr; \
		return Steinberg::kNoInterface; \
	}
