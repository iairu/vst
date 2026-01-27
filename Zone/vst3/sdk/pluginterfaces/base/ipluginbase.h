//------------------------------------------------------------------------
// Project     : VST SDK
//
// Category    : Interfaces
// Filename    : ipluginbase.h
// Created by  : Steinberg, 01/2004
// Description : Plug-in interface
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
/** Basic Plug-in interface.
\ingroup pluginBase */
//------------------------------------------------------------------------
class IPluginBase : public FUnknown
{
public:
	/** Called after the plug-in is created. */
	virtual tresult PLUGIN_API initialize (FUnknown* context) = 0;

	/** Called before the plug-in is destroyed. */
	virtual tresult PLUGIN_API terminate () = 0;

	static const FUID iid;
};

DECLARE_CLASS_IID (IPluginBase, 0x00000000, 0x00000000, 0x00000000, 0x00000000)

//------------------------------------------------------------------------
/** Plug-in factory interface.
\ingroup pluginBase */
//------------------------------------------------------------------------
class IPluginFactory : public FUnknown
{
public:
	/** Returns information about the factory. */
	virtual tresult PLUGIN_API getFactoryInfo (PFactoryInfo* info) = 0;

	/** Returns the number of classes this factory can create. */
	virtual int32 PLUGIN_API countClasses () = 0;

	/** Returns information about a class. */
	virtual tresult PLUGIN_API getClassInfo (int32 index, PClassInfo* info) = 0;

	/** Creates a component instance. */
	virtual tresult PLUGIN_API createInstance (FIDString cid, FIDString iid, void** obj) = 0;

	static const FUID iid;
};

DECLARE_CLASS_IID (IPluginFactory, 0x7A4D811C, 0x52D94248, 0x98622B49, 0x346495E3)

struct PFactoryInfo
{
	char company[64];
	char web[128];
	char email[128];
	int32 flags;
};

struct PClassInfo
{
	TUID cid;
	int32 cardinality;
	char category[32];
	char name[64];
};

struct PClassInfo2 : PClassInfo
{
	char vendor[64];
	char version[64];
	char sdkVersion[64];
};

struct PClassInfoW : PClassInfo2
{
	wchar_t subCategories[128];
};


//------------------------------------------------------------------------
} // namespace Steinberg

#define INLINE_UID_FROM_FUID(fuid) fuid
#define SMTG_EXPORT_SYMBOL __attribute__((visibility("default")))
#define stringCompanyName "MyCompany"
#define stringCompanyWeb "www.mycompany.com"
#define stringCompanyEmail "info@mycompany.com"
#define PClassInfo Steinberg::PClassInfo
#define PFactoryInfo Steinberg::PFactoryInfo
#define PClassInfo2 Steinberg::PClassInfo2
#define PClassInfoW Steinberg::PClassInfoW
#define IPluginFactory Steinberg::IPluginFactory
#define CPluginFactory Steinberg::CPluginFactory
#define IPluginFactory3 Steinberg::IPluginFactory3
#define CPlugin Steinberg::CPlugin
#define DEF_CLASS2(cid1, cid2, className, category, name)
#define BEGIN_FACTORY_DEF(a,b,c) \
	namespace Steinberg { \
	IPluginFactory* gPluginFactory = nullptr; \
	}
#define END_FACTORY
namespace Steinberg { namespace Vst { enum PlugType { kInstrument }; } }
