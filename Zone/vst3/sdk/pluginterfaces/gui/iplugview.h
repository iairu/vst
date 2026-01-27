//------------------------------------------------------------------------
// Project     : VST SDK
//
// Category    : Interfaces
// Filename    : iplugview.h
// Created by  : Steinberg, 01/2004
// Description : Plug-in view interface
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
#include "pluginterfaces/base/fstrdefs.h"

//------------------------------------------------------------------------
namespace Steinberg {
namespace Vst {

//------------------------------------------------------------------------
/** View rectangle */
struct ViewRect
{
	int32 left;
	int32 top;
	int32 right;
	int32 bottom;

	int32 getWidth () const { return right - left; }
	int32 getHeight () const { return bottom - top; }
};

//------------------------------------------------------------------------
/** Plug-in view interface.
\ingroup vstGUI */
//------------------------------------------------------------------------
class IPlugView : public FUnknown
{
public:
//------------------------------------------------------------------------
	/** Is the view a platform window? */
	virtual tresult PLUGIN_API isPlatformTypeSupported (FIDString type) = 0;

	/** The parent window has been created. */
	virtual tresult PLUGIN_API attached (void* parent, FIDString type) = 0;

	/** The parent window is about to be destroyed. */
	virtual tresult PLUGIN_API removed () = 0;

	/** The view is requested to be resized. */
	virtual tresult PLUGIN_API onSize (ViewRect* newSize) = 0;

	/** The parent window's focus has changed. */
	virtual tresult PLUGIN_API onFocus (TBool state) = 0;

	/** The view gets the event. */
	virtual tresult PLUGIN_API setFrame (IPlugFrame* frame) = 0;

	/** Is view resizable? */
	virtual tresult PLUGIN_API canResize () = 0;

	/** On live resize. */
	virtual tresult PLUGIN_API checkSizeConstraint (ViewRect* rect) = 0;

//------------------------------------------------------------------------
	static const FUID iid;
};

DECLARE_CLASS_IID (IPlugView, 0x02A0C179, 0x4170443B, 0x8545233E, 0x64364028)

//------------------------------------------------------------------------
/** Plug-in frame interface.
\ingroup vstGUI */
//------------------------------------------------------------------------
class IPlugFrame : public FUnknown
{
public:
//------------------------------------------------------------------------
	/** Resizes the view. */
	virtual tresult PLUGIN_API resizeView (IPlugView* view, ViewRect* newSize) = 0;

//------------------------------------------------------------------------
	static const FUID iid;
};

DECLARE_CLASS_IID (IPlugFrame, 0x367FAF01, 0xA5B7446C, 0x96011210, 0x4426815E)

//------------------------------------------------------------------------
} // namespace Vst
} // namespace Steinberg
