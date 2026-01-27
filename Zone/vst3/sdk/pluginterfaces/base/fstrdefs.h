//------------------------------------------------------------------------
// Project     : VST SDK
//
// Category    : Interfaces
// Filename    : fstrdefs.h
// Created by  : Steinberg, 01/2004
// Description : Basic string definitions
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

#if defined(__GNUC__) && (__GNUC__ >= 4)
#define STR16(x) u##x
#define STR16_DEFINED 1
#endif

#ifndef STR16
#if DEPRECATED_PLATFORM_STRINGS
#if MAC
#define STR16(x) L##x
#define STR16_DEFINED 1
#elif WINDOWS
#define STR16(x) L##x
#define STR16_DEFINED 1
#endif
#endif
#endif

#ifndef STR16
#define STR16(x) u##x
#define STR16_DEFINED 1
#endif

#if SMTG_OS_WINDOWS
#undef WINDOWS
#define WINDOWS 1
#endif

#ifdef __cplusplus
//--------------------------------------------------------------------
/** \defgroup StringMacros String Macros */
/*@{*/
//--------------------------------------------------------------------
/** Puts a string literal into a "const Steinberg::char8*". */
#define S8(s) s
/** Puts a string literal into a "const Steinberg::char16*". */
#define S16(s) STR16(s)
/** Puts a string literal into a "const Steinberg::tchar*". */
#define TSTR(s) PLATFORM_STR(s)

/*@}*/
#endif // __cplusplus
