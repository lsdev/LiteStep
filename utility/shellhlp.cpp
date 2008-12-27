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
#include "shellhlp.h"
#include "core.hpp"
#include <MMSystem.h>

//
// GetShellFolderPath
//
// Given a CLSID, returns the given shell folder.
// cchPath must be atleast equal to MAX_PATH.
// Does NOT quote spaces, because often times the first thing that is done with
// the returned path is to append another string to it, which doesn't work with
// quotes.
//
bool GetShellFolderPath(int nFolder, LPTSTR ptzPath, size_t cchPath)
{
    ASSERT(cchPath >= MAX_PATH);
    ASSERT(NULL != ptzPath);
    UNREFERENCED_PARAMETER(cchPath);

    IMalloc* pMalloc;
    bool bReturn = false;
    
    // SHGetSpecialFolderPath is not available on Win95
    // use SHGetSpecialFolderLocation and SHGetPathFromIDList instead
    if (SUCCEEDED(SHGetMalloc(&pMalloc)))
    {
        PIDLIST_ABSOLUTE pidl;
        
        if (SUCCEEDED(SHGetSpecialFolderLocation(NULL, nFolder, &pidl)))
        {
            bReturn = SHGetPathFromIDList(pidl, ptzPath) ? true : false;
            pMalloc->Free(pidl);
        }
        
        pMalloc->Release();
    }
    
    return bReturn;
}


//
// PathAddBackslashEx
//
// Checked version of PathAddBackslash which also handles quoted paths
//
// Return values:  S_OK          - backslash appended
//                 S_FALSE       - path already ended with a backslash
//                 E_OUTOFMEMORY - buffer too small
//                 E_FAIL        - other failure (invalid input string)
//
HRESULT PathAddBackslashEx(LPTSTR ptzPath, size_t cchPath)
{
    ASSERT(cchPath <= STRSAFE_MAX_CCH);
    ASSERT(NULL != ptzPath); ASSERT(0 != cchPath);
    
    HRESULT hr = E_FAIL;
    size_t cchCurrentLength = 0;
    
    if (SUCCEEDED(StringCchLength(ptzPath, cchPath, &cchCurrentLength)))
    {
        bool bHasQuote = false;
        LPTSTR ptzEnd = ptzPath + cchCurrentLength;
        
        if ((ptzEnd > ptzPath) && (*(ptzEnd-1) == _T('\"')))
        {
            --ptzEnd;
            bHasQuote = true;
        }
        
        if (ptzEnd > ptzPath)
        {
            if (*(ptzEnd-1) != _T('\\'))
            {
                if (cchPath - cchCurrentLength > 1)
                {
                    if (bHasQuote)
                    {
                        *(ptzEnd+1) = *ptzEnd;
                    }
                    
                    *ptzEnd = _T('\\');
                    
                    if (bHasQuote)
                    {
                        ++ptzEnd;
                    }
                    
                    ASSERT((size_t)(ptzEnd - ptzPath) < cchPath);
                    *(ptzEnd+1) = _T('\0');
                    
                    hr = S_OK;
                }
                else
                {
                    hr = E_OUTOFMEMORY;
                }
            }
            else
            {
                hr = S_FALSE;
            }
        }
    }
    
    return hr;
}


//
// GetSystemString
//
bool GetSystemString(DWORD dwCode, LPTSTR ptzBuffer, DWORD cchBuffer)
{
    ASSERT(NULL != ptzBuffer); ASSERT(0 != cchBuffer);
    
    return (0 != FormatMessage(
        FORMAT_MESSAGE_FROM_SYSTEM |
        FORMAT_MESSAGE_IGNORE_INSERTS,
        NULL,
        dwCode,
        0,
        ptzBuffer,
        cchBuffer,
        NULL
    ));
}


//
// CLSIDToString
// Mostly for debugging purposes (TRACE et al)
//
HRESULT CLSIDToString(REFCLSID rclsid, LPTSTR ptzBuffer, size_t cchBuffer)
{
    ASSERT(NULL != ptzBuffer); ASSERT(0 != cchBuffer);
    
    LPOLESTR pOleString = NULL;
    
    HRESULT hr = StringFromCLSID(rclsid, &pOleString);
    
    if (SUCCEEDED(hr) && pOleString)
    {
#ifdef UNICODE
        hr = StringCchCopy(ptzBuffer, cchBuffer, pOleString);
#else // UNICODE
        int nReturn = WideCharToMultiByte(CP_ACP, 0, pOleString,
            (int)wcslen(pOleString), ptzBuffer, (int)cchBuffer, NULL, NULL);

        if (nReturn == 0)
        {
            hr = HrGetLastError();
        }
#endif
    }

    CoTaskMemFree(pOleString);
    
    return hr;
}


//
// LSGetModuleFileName
//
// Wrapper around GetModuleFileName that takes care of truncated buffers. If
// people are interested in the number of bytes written we could add another
// parameter (DWORD* pcchWritten)
//
bool LSGetModuleFileName(HINSTANCE hInst, LPTSTR pszBuffer, DWORD cchBuffer)
{
    bool bSuccess = false;
    
    DWORD cchCopied = GetModuleFileName(hInst, pszBuffer, cchBuffer);
    
    if (cchCopied == cchBuffer)
    {
        ASSERT(GetLastError() == ERROR_INSUFFICIENT_BUFFER);
        
        // GetModuleFileName doesn't null-terminate the buffer if it is too
        // small. Make sure that even in this error case the buffer is properly
        // terminated - some people don't check return values.
        pszBuffer[cchBuffer-1] = '\0';
    }
    else if (cchCopied > 0 && cchCopied < cchBuffer)
    {
        bSuccess = true;
    }
    
    return bSuccess;
}


//
// TryAllowSetForegroundWindow
// Calls AllowSetForegroundWindow on platforms that support it
//
HRESULT TryAllowSetForegroundWindow(HWND hWnd)
{
    ASSERT(hWnd != NULL);
    HRESULT hr = E_FAIL;

    typedef BOOL (WINAPI* ASFWPROC)(DWORD);

    ASFWPROC pAllowSetForegroundWindow = (ASFWPROC)GetProcAddress(
        GetModuleHandle(_T("user32.dll")), "AllowSetForegroundWindow");

    if (pAllowSetForegroundWindow)
    {
        DWORD dwProcessId = 0;
        GetWindowThreadProcessId(hWnd, &dwProcessId);

        if (pAllowSetForegroundWindow(dwProcessId))
        {
            hr = S_OK;
        }
        else
        {
            hr = HrGetLastError();
        }
    }
    else
    {
        // this platform doesn't have ASFW (Win95, NT4), so the
        // target process is allowed to set the foreground window anyway
        hr = S_FALSE;
    }

    return hr;
}


//
// IsVistaOrAbove
//
bool IsVistaOrAbove()
{
    bool bVistaOrAbove = false;

    OSVERSIONINFO ovi = { 0 };
    ovi.dwOSVersionInfoSize = sizeof(ovi);

    VERIFY(GetVersionEx(&ovi));

    if (ovi.dwPlatformId == VER_PLATFORM_WIN32_NT &&
        ovi.dwMajorVersion >= 6)
    {
        bVistaOrAbove = true;
    }

    return bVistaOrAbove;
}


//
// LSShutdownDialog
//
void LSShutdownDialog(HWND hWnd)
{
    FARPROC fnProc = GetProcAddress(
        GetModuleHandle("SHELL32.DLL"), (LPCSTR)((long)0x003C));

    if (fnProc)
    {
        if (IsVistaOrAbove())
        {
            typedef void (WINAPI* ExitWindowsDialogProc)(HWND, DWORD);

            ExitWindowsDialogProc fnExitWindowsDialog =
                (ExitWindowsDialogProc)fnProc;

            // Meaning of second parameter unknown
            fnExitWindowsDialog(hWnd, NULL);
        }
        else
        {
            typedef void (WINAPI* ExitWindowsDialogProc)(HWND);

            ExitWindowsDialogProc fnExitWindowsDialog =
                (ExitWindowsDialogProc)fnProc;

            fnExitWindowsDialog(hWnd);
        }
    }
}


//
// LSPlaySystemSound
// Wrapper around PlaySound to play system event sounds
//
BOOL LSPlaySystemSound(LPCTSTR pszSoundAlias)
{
    BOOL bResult = FALSE;

    // We want to avoid linking to winmm.dll as long as it's used just here
    HMODULE hWinMM = LoadLibrary(_T("winmm.dll"));

    if (hWinMM)
    {
        typedef BOOL (WINAPI* PlaySoundProc)(LPCTSTR, HMODULE, DWORD);

#ifdef UNICODE
        PlaySoundProc fnPlaySound = (PlaySoundProc)
            GetProcAddress(hWinMM, "PlaySoundW");
#else
        PlaySoundProc fnPlaySound = (PlaySoundProc)
            GetProcAddress(hWinMM, "PlaySoundA");
#endif

        if (fnPlaySound)
        {
            DWORD dwFlags = SND_ALIAS;

            if (IsVistaOrAbove())
            {
                // Vista has a special volume slider for system sounds, ie. all
                // those beeps etc. can be adjusted independently of all other
                // sounds. SND_SYSTEM makes PlaySound use that slider.
                dwFlags |= SND_SYSTEM;
            }

            bResult = fnPlaySound(pszSoundAlias, NULL, dwFlags);
        }

        VERIFY(FreeLibrary(hWinMM));
    }

    return bResult;
}


//
// LS_THREAD_DATA
// for use in LSCreateThread/LSThreadThunk
//
struct LS_THREAD_DATA
{
    HANDLE hEvent;
    LPCSTR pszName;
    LPTHREAD_START_ROUTINE fnOrigFunc;
    LPVOID pOrigParam;
};


//
// LSThreadThunk
// for use LSCreateThread
//
DWORD WINAPI LSThreadThunk(LPVOID pParam)
{
    LS_THREAD_DATA* pData = (LS_THREAD_DATA*)pParam;
    ASSERT(pData != NULL);

    // create local copy
    LS_THREAD_DATA data = *pData;

    if (data.pszName)
    {
        DbgSetCurrentThreadName(data.pszName);
    }

    SetEvent(data.hEvent);
    return data.fnOrigFunc(data.pOrigParam);
}


//
// LSCreateThread
// The name param is intentionally CHAR as the debugger doesn't handle WCHAR
//
HANDLE LSCreateThread(LPCSTR pszName, LPTHREAD_START_ROUTINE fnStartAddres,
                      LPVOID lpParameter, LPDWORD pdwThreadId)
{
    DWORD dwDummy = 0;

    if (!pdwThreadId)
    {
        // On Win9x pdwThreadId must be valid
        pdwThreadId = &dwDummy;
    }

#ifdef MSVC_DEBUG
    LS_THREAD_DATA data = { 0 };
    data.hEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
    data.pszName = pszName;
    data.fnOrigFunc = fnStartAddres;
    data.pOrigParam = lpParameter;

    // Should be fine to assert this since this is debug-mode only code
    ASSERT(data.hEvent != NULL);

    HANDLE hThread = CreateThread(
        NULL, 0, LSThreadThunk, (LPVOID)&data, 0, pdwThreadId);

    if (hThread != NULL && data.hEvent)
    {
        WaitForSingleObject(data.hEvent, INFINITE);
    }

    return hThread;
#else
    return CreateThread(NULL, 0, fnStartAddres, lpParameter, 0, pdwThreadId);
    UNREFERENCED_PARAMETER(pszName);
#endif
}
