//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//
// This is a part of the Litestep Shell source code.
//
// Copyright (C) 1997-2012  LiteStep Development Team
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

#include <assert.h>

#if defined (_MSC_VER)
#  include <stddef.h>
#else
#  include <stdint.h>
#endif

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
#  define VERIFY(f)     ASSERT(f)
#  define VERIFY_HR(f)  ASSERT(SUCCEEDED(f))
#else
#  define VERIFY(f)     ((void)(f))
#  define VERIFY_HR(f)  ((void)(f))
#endif


//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//
// TRACE
//
// Sends a formatted (printf-style) message to the debug output window.
// Automatically inserts \n at the end of the string.
//
void DbgTraceMessage(const char* pszFormat, ...);

#if defined(TRACE_ENABLED)
#  define TRACE  DbgTraceMessage
#else
#  define TRACE  1 ? (void)0 : DbgTraceMessage
#endif

// DbgTraceWindowMessage
#if defined (TRACE_ENABLED)
void    DbgTraceWindowMessage(const char* pszPrefix, unsigned int uMsg,
                              uintptr_t wParam, uintptr_t lParam);
#else
#define DbgTraceWindowMessage(pszPrefix, uMsg, wParam, lParam)
#endif


//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//
// DbgSetCurrentThreadName
//
#if defined(MSVC_DEBUG)
void DbgSetCurrentThreadName(const char* pszThreadName);
#else
#define DbgSetCurrentThreadName(Name)  (void)0
#endif


#endif // DEBUG_HPP
