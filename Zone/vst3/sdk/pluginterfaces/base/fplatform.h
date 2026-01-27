//------------------------------------------------------------------------
// Project     : VST SDK
//
// Category    : Interfaces
// Filename    : fplatform.h
// Created by  : Steinberg, 01/2004
// Description : Platform specific code
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

#if SMTG_OS_WINDOWS
#include <windows.h>
#endif

#if SMTG_OS_MAC
#include <CoreFoundation/CoreFoundation.h>
#endif

// -----------------------------------------------------------------
// SMTG_OVERRIDE
// -----------------------------------------------------------------
#ifndef SMTG_OVERRIDE
#if SMTG_CPP11_STD_FEATURES
#define SMTG_OVERRIDE override
#else
#define SMTG_OVERRIDE
#endif
#endif

#define SMTG_DEPRECATED_ATTRIBUTE

// -----------------------------------------------------------------
// FObject
// -----------------------------------------------------------------
#define OBJ_METHODS(T, P)
#define DEFINE_INTERFACES
#define DEF_INTERFACE(i)
#define END_DEFINE_INTERFACES(p)
#define REFCOUNT_METHODS(p)

namespace Steinberg {
class FObject {
public:
    virtual ~FObject() {}
    virtual tresult PLUGIN_API queryInterface(const TUID _iid, void** obj) { return kNoInterface; }
    virtual uint32 PLUGIN_API addRef() { return 1; }
    virtual uint32 PLUGIN_API release() { return 1; }
};
}

#define kNoInterface 0
#define kResultOk 1
