//
//  This is a part of the LiteStep Shell source code.
//
//  Copyright (C) 2003,2005 The LiteStep Development Team.
//
//
//  This program is free software; you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation; either version 2 of the License, or
//  (at your option) any later version.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with this program; if not, write to the Free Software
//  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
//
#ifndef LS_DEBUG_HPP_INCLUDED
#define LS_DEBUG_HPP_INCLUDED

#if	defined(_MFC_VER)
#  error This file is not meant to be used with MFC
#endif // defined(_MFC_VER)

#include <assert.h>
#if defined(_DEBUG)
#  include <limits.h>
#  include <stdarg.h>
#  include <stdio.h>
#  define STRSAFE_NO_DEPRECATE
#  include <strsafe.h>
#  undef STRSAFE_NO_DEPRECATE
#  include <windows.h>
#endif // defined(_DEBUG)


//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
//
// helper macros
//
#define TOSTRING_HELPER(str) #str
#define TOSTRING(str) TOSTRING_HELPER(str) 
#define LINE_LOCATION __FILE__ "("TOSTRING(__LINE__)") : "


//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
//
// compile-time messages (MSVC specific)
//
// use like this:
// #pragma COMPILE_TODO("need to localize these strings")
//
#define COMPILE_ERROR(msg) message(LINE_LOCATION "ERROR: " ## msg)
#define COMPILE_WARN(msg)  message(LINE_LOCATION "WARNING: " ## msg)
#define COMPILE_TODO(msg)  message(LINE_LOCATION "TODO: " ## msg)
#define COMPILE_MSG(msg)   message(LINE_LOCATION msg)


//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
//
// ASSERTs
//
#define ASSERT assert

#define ASSERT_ISVALIDBUF(p,c)   ASSERT((NULL != (p) && 0 != (c)))
#define ASSERT_ISNOTNULL(p)      ASSERT((NULL != (p)))
#define ASSERT_ISNULL(p)         ASSERT((NULL == (p)))


//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
//
// Tracing helpers (ignore)
//
#if defined(_DEBUG)
namespace debug
{
    template <class Tracer> inline void Trace(const char* pszFormat, ...)
    {
        ASSERT_ISNOTNULL(pszFormat);
        
        va_list args;
        va_start(args, pszFormat);
        
        char szBuffer[512];
        StringCchVPrintfExA(szBuffer, 512, NULL, NULL, STRSAFE_NULL_ON_FAILURE,
            pszFormat, args);
        
        Tracer::Output(szBuffer);
        va_end(args);
    }

    
    template <class Tracer> class PrefixedOutput
    {
        unsigned int m_uLine;
        const char* m_pszFile;
        
    public:
        PrefixedOutput(unsigned int uLine, const char* pszFile) :
          m_uLine(uLine), m_pszFile(pszFile) {}
          
        void operator()(const char* pszFormat, ...)
        {
            ASSERT_ISNOTNULL(pszFormat);
            char szFormatBuffer[512] = { 0 };
            
            StringCchPrintfExA(szFormatBuffer, 512, NULL, NULL,
                STRSAFE_NULL_ON_FAILURE, "%s (%u) : %s", m_pszFile, m_uLine,
                pszFormat);
            
            va_list args;
            va_start(args, pszFormat);
            char szBuffer[4096] = { 0 };
            
            StringCchVPrintfExA(szBuffer, 4096, NULL, NULL,
                STRSAFE_NULL_ON_FAILURE, szFormatBuffer, args);
            
            Tracer::Output(szBuffer);
            va_end(args);              
        }
    };
}
#endif // defined(_DEBUG)


//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
//
// Tracing
//
// TRACE  - Sends a formatted (printf-style) message to the debug output window.
//          Automatically inserts \n at the end of the string.
// INFO   - Like TRACE but the message is prefixed with __FILE__ and __LINE__.
//
#if defined(_DEBUG)

struct DebugOutputTracer
{
    static void Output(const char* pszText)
    {
        OutputDebugStringA(pszText);
        OutputDebugStringA("\n");
    }
};
    
#define TRACE_ENABLED
#define TRACE  debug::Trace<DebugOutputTracer>
#define INFO   debug::PrefixedOutput<DebugOutputTracer>(__LINE__, __FILE__)

#else	// !defined(_DEBUG)

namespace debug
{
    // dummy function, just needs the correct signature
    inline void TraceDummy(const char*, ...) {}
}

#define TRACE  1 ? (void)0 : debug::TraceDummy
#define INFO   TRACE

#endif	// !defined(_DEBUG)


//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
//
// Extended tracing
//
// TRACE_FN      - Tracks function entry and exit
// TRACE_LASTERR - Logs the string associated with GetLastError()
//
#if defined(_DEBUG)

namespace debug
{
    inline void TraceLastError(const char* pszDescription)
    {
        ASSERT_ISNOTNULL(pszDescription);
        void* pvBuffer;
        
        FormatMessage(
            FORMAT_MESSAGE_ALLOCATE_BUFFER |
            FORMAT_MESSAGE_FROM_SYSTEM |
            FORMAT_MESSAGE_IGNORE_INSERTS,
            NULL,
            GetLastError(),
            0,
            (LPSTR)&pvBuffer,
            0,
            NULL
            );
        
        TRACE("Last Error (%s) = %s", pszDescription, (const char*)pvBuffer);
        LocalFree(pvBuffer);
    }
    
    class TraceFunction
    {
        const char* m_pszName;
    public:
        TraceFunction(const char* pszName) : m_pszName(pszName)
        { TRACE("%s - entered", m_pszName); }
        ~TraceFunction()
        { TRACE("%s - left", m_pszName); }
    };
}

#define TRACE_FN(name)       debug::TraceFunction \
                             trace_fn_7acc0ecb_09ab_4dec_86a2_0d075c666120(name)
#define TRACE_LASTERR(desc)  debug::TraceLastError(desc)

#else	// !defined(_DEBUG)

#define TRACE_FN(name)       (void)0
#define TRACE_LASTERR(desc)  (void)0

#endif	// !defined(_DEBUG)


//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
//
// miscellaneous
//
// debug mode:
// VERIFY - Equivalent to ASSERT.
// BREAK  - Breaks into the debugger.
// DBG    - Executes the argument.
//
// release mode:
// VERIFY     - The argument is executed.
// the others - do nothing
//
#if defined(_DEBUG)

#define	VERIFY(f)  ASSERT(f)
#define	BREAK()	   DebugBreak()
#define	DBG(f)     (f)

#else	// !defined(_DEBUG)

#define	VERIFY(f)  ((void)(f))
#define	BREAK()    (void)0
#define	DBG(f)     (void)0

#endif	// !defined(_DEBUG)


#endif // !defined(LS_DEBUG_HPP_INCLUDED)
