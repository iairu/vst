//------------------------------------------------------------------------
// Project     : VST SDK
//
// Category    : Helpers
// Filename    : public.sdk/source/main/pluginfactory.h
// Created by  : Steinberg, 01/2004
// Description : VST 3 Plug-in Factory
//
//-----------------------------------------------------------------------------
// This file is part of a Steinberg SDK. It is subject to the license terms
// in the LICENSE file found in the top-level directory of this distribution
// and at www.steinberg.net/sdklicenses. 
// No part of the SDK, including this file, may be copied, modified, propagated,
// or distributed except according to the terms contained in the LICENSE file.
//-----------------------------------------------------------------------------

#pragma once

#include "pluginterfaces/base/ipluginbase.h"
#include "pluginterfaces/base/funknown.h"

//------------------------------------------------------------------------
#define BEGIN_FACTORY_DEF( company, url, email ) \
	extern "C" { \
	SMTG_EXPORT_SYMBOL IPluginFactory* PLUGIN_API GetPluginFactory () \
	{ \
		static Steinberg::IPluginFactory* gPluginFactory = new Steinberg::CPluginFactory (company, url, email); \
		return gPluginFactory; \
	} \
	} \
	namespace Steinberg { \
	IPluginFactory* CPluginFactory::instance = 0; \
	CPluginFactory::CPluginFactory (const char* company, const char* url, const char* email) \
	{ \
		if (instance) return; \
		instance = this; \
		registerPlugin (

#define DEF_CLASS2(cid1, cid2, className, category, name) \
		new CPlugin<className> (cid1, cid2, name, category),

#define END_FACTORY \
		0); \
	}

//------------------------------------------------------------------------
namespace Steinberg {

//------------------------------------------------------------------------
class IPluginFactory3;
//------------------------------------------------------------------------
class CPluginFactory : public IPluginFactory3
{
public:
	CPluginFactory (const char* company, const char* url, const char* email);
	static IPluginFactory* instance;

	//---from IPluginFactory-----------------------
	tresult PLUGIN_API getFactoryInfo (PFactoryInfo* info) override;
	int32 PLUGIN_API countClasses () override;
	tresult PLUGIN_API getClassInfo (int32 index, PClassInfo* info) override;
	tresult PLUGIN_API createInstance (FIDString cid, FIDString iid, void** obj) override;

	//---from IPluginFactory2----------------------
	tresult PLUGIN_API getClassInfo2 (int32 index, PClassInfo2* info) override;

	//---from IPluginFactory3----------------------
	tresult PLUGIN_API getClassInfoUnicode (int32 index, PClassInfoW* info) override;
	tresult PLUGIN_API setHostContext (FUnknown* context) override;

	void registerPlugin (void*);

	OBJ_METHODS (CPluginFactory, FObject)
	DEFINE_INTERFACES
		DEF_INTERFACE (IPluginFactory)
		DEF_INTERFACE (IPluginFactory2)
		DEF_INTERFACE (IPluginFactory3)
	END_DEFINE_INTERFACES (FObject)
	REFCOUNT_METHODS(FObject)

private:
	PFactoryInfo factoryInfo;
	std::vector<void*> classEntries;
};

//------------------------------------------------------------------------
} // namespace Steinberg
