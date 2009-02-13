//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//
// This is a part of the Litestep Shell source code.
//
// Copyright (C) 1997-2009  LiteStep Development Team
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
#include "debug.hpp"
#include "core.hpp"


//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//
// DbgTraceMessage
//
void DbgTraceMessage(const char* pszFormat, ...)
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


//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//
// DbgTraceWindowMessage
//
#if defined(TRACE_ENABLED)
void DbgTraceWindowMessage(const char* pszPrefix, unsigned int uMsg,
                           uintptr_t wParam, uintptr_t lParam)
{
    if (uMsg < WM_USER)
    {
        TRACE("[%s] WM_%.4X(%.8X, %.8X)",
            pszPrefix, uMsg, wParam, lParam);
    }
    else if (uMsg >= WM_USER && uMsg <= (WM_APP-1))
    {
        TRACE("[%s] WM_USER+%u(%.8X, %.8X)",
            pszPrefix, uMsg-WM_USER, wParam, lParam);
    }
    else if (uMsg >= WM_APP && uMsg <= (MAXINTATOM-1))
    {
        TRACE("[%s] WM_APP+%u(%.8X, %.8X)",
            pszPrefix, uMsg-WM_APP, wParam, lParam);
    }
    else if (uMsg >= MAXINTATOM)
    {
        TCHAR szMsgName[MAX_PATH] = { 0 };
        
        // GetClipboardFormatName retrieves the name of
        // registered window messages too!
        if (GetClipboardFormatName(
            uMsg, szMsgName, COUNTOF(szMsgName)) > 0)
        {
            TRACE("[%s] WM_'%s'(%.8X, %.8X)",
                pszPrefix, szMsgName, wParam, lParam);
        }
        else
        {
            TRACE("[%s] WM_%.8X(%.8X, %.8X)",
                pszPrefix, uMsg, wParam, lParam);
        }
    }
}
#endif // #TRACE_ENABLED


#if defined(MSVC_DEBUG)

#define MS_VC_EXCEPTION 0x406D1388


//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//
// THREADNAME_INFO
// Helper struct for SetCurrentThreadName
//
#pragma pack(push,8)

typedef struct tagTHREADNAME_INFO
{
    DWORD dwType; // Must be 0x1000.
    LPCSTR szName; // Pointer to name (in user addr space).
    DWORD dwThreadID; // Thread ID (-1=caller thread).
    DWORD dwFlags; // Reserved for future use, must be zero.
} THREADNAME_INFO;

#pragma pack(pop)


//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//
// DbgSetCurrentThreadName
//
void DbgSetCurrentThreadName(const char* pszThreadName)
{
    //
    // While you can put any thread's ID in the info struct, the thread
    // apparently needs to be up and running for this to work, ie. there's a
    // race condition. Thus we only support naming the current thread.
    //
    THREADNAME_INFO info = { 0 };
    info.dwType = 0x1000;
    info.szName = pszThreadName;
    info.dwThreadID = (DWORD)-1;
    info.dwFlags = 0;
    
    __try
    {
        RaiseException(MS_VC_EXCEPTION, 0,
            sizeof(info) / sizeof(ULONG_PTR), (ULONG_PTR*)&info);
    }
    __except(EXCEPTION_EXECUTE_HANDLER)
    {
        // do nothing
    }
}

#endif // MSVC_DEBUG
