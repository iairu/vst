//------------------------------------------------------------------------
// Project     : VST SDK
//
// Category    : Interfaces
// Filename    : fbasedefs.h
// Created by  : Steinberg, 01/2004
// Description : Basic definitions
//
//-----------------------------------------------------------------------------
// This file is part of a Steinberg SDK. It is subject to the license terms
// in the LICENSE file found in the top-level directory of this distribution
// and at www.steinberg.net/sdklicenses. 
// No part of the SDK, including this file, may be copied, modified, propagated,
// or distributed except according to the terms contained in the LICENSE file.
//-----------------------------------------------------------------------------

#pragma once

//----------------------------------------------------------------
// Compiler specific settings
//----------------------------------------------------------------
#if defined(_MSC_VER)
	#define COM_COMPATIBLE 1
	#if _MSC_VER >= 1900 // VS 2015
		#define SMTG_CPP11_STD_FEATURES 1
	#endif
#elif defined(__GNUC__)
	#if __GNUC__ >= 4
		#if __cplusplus >= 201103L
			#define SMTG_CPP11_STD_FEATURES 1
		#endif
	#endif
#endif

//----------------------------------------------------------------
// Platform specific settings
//----------------------------------------------------------------
#if defined(__APPLE__)
	#include <TargetConditionals.h>
	#if TARGET_OS_IPHONE || TARGET_IPHONE_SIMULATOR
		#define SMTG_OS_IOS 1
	#else
		#define SMTG_OS_MAC 1
	#endif
	#define SMTG_PLATFORM_NAME "Mac"
#elif defined(_WIN32)
	#define SMTG_OS_WINDOWS 1
	#define SMTG_PLATFORM_NAME "Windows"
#elif defined(ANDROID)
	#define SMTG_OS_LINUX 1
	#define SMTG_OS_ANDROID 1
	#define SMTG_PLATFORM_NAME "Android"
#elif defined(__linux__)
	#define SMTG_OS_LINUX 1
	#define SMTG_PLATFORM_NAME "Linux"
#endif

#if !SMTG_OS_WINDOWS
	#define SMTG_RENAME_SYMBOL(name) __asm__(#name)
#else
	#define SMTG_RENAME_SYMBOL(name)
#endif

#ifndef CDECL
#define CDECL
#endif

//----------------------------------------------------------------
// Types
//----------------------------------------------------------------
namespace Steinberg
{
	typedef char int8;
	typedef unsigned char uint8;

	typedef short int16;
	typedef unsigned short uint16;

	typedef int int32;
	typedef unsigned int uint32;

#if SMTG_OS_WINDOWS
	typedef __int64 int64;
	typedef unsigned __int64 uint64;
#else
	typedef long long int64;
	typedef unsigned long long uint64;
#endif

	typedef float float32;
	typedef double float64;

	typedef unsigned char uchar;
	typedef unsigned short ushort;
	typedef unsigned int uint;

	typedef char8_t char8;
	typedef char16_t char16;

	typedef int32 tresult;

	typedef int32 ParamID;
	typedef double ParamValue;

	typedef bool TBool;
}
