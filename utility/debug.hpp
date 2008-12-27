//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//
// This is a part of the Litestep Shell source code.
//
// Copyright (C) 1997-2007  Litestep Development Team
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
//
//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
#if !defined(DEBUG_HPP)
#define DEBUG_HPP

#include "common.h"
#include <assert.h>

#if defined(_DEBUG)
   // THIS IS BOGUS, WE NEED TO FIX OUR STRING FUNCTION USAGE!!!
#  define STRSAFE_NO_DEPRECATE
#  include <strsafe.h>
#  undef STRSAFE_NO_DEPRECATE
#endif // defined(_DEBUG)


// MSVC debug
#if defined(_DEBUG) && defined (_MSC_VER)
#  define MSVC_DEBUG
#endif


#if defined(_DEBUG)
#  define TRACE_ENABLED
#endif


//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//
// ASSERT and VERIFY
//
// VERIFY in...
//     debug mode:   Equivalent to ASSERT
//     release mode: Argument is still executed
//
#define ASSERT assert

#if defined(_DEBUG)
#  define VERIFY(f)  ASSERT(f)
#else
#  define VERIFY(f)  ((void)(f))
#endif


//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//
// TRACE helper
//
#if defined(TRACE_ENABLED)
namespace debug
{
    inline void Trace(const char* pszFormat, ...)
    {
        ASSERT(NULL != pszFormat);
        
        va_list args;
        va_start(args, pszFormat);
        
        char szBuffer[512];
        StringCchVPrintfExA(szBuffer, 512,
            NULL, NULL, STRSAFE_NULL_ON_FAILURE,
            pszFormat, args);

        va_end(args);

        OutputDebugStringA(szBuffer);
        
    #if !defined(__GNUC__)
        // This just outputs a blank line in gdb
        OutputDebugStringA("\n");
    #endif
    }
}
#endif // defined(TRACE_ENABLED)


//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//
// TRACE
//
// Sends a formatted (printf-style) message to the debug output window.
// Automatically inserts \n at the end of the string.
//
#if defined(TRACE_ENABLED)

#define TRACE  debug::Trace

#else

// dummy function, just needs the correct signature
inline void TraceDummy(const char*, ...) {}

#define TRACE  1 ? (void)0 : TraceDummy

#endif // !defined(TRACE_ENABLED)


//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//
// DbgSetCurrentThreadName
//
#ifdef MSVC_DEBUG
void DbgSetCurrentThreadName(LPCSTR pszThreadName);
#else
#define DbgSetCurrentThreadName(Name)  (void)0
#endif


#endif // DEBUG_HPP
