//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//
// This is a part of the Litestep Shell source code.
//
// Copyright (C) 1997-2013  LiteStep Development Team
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


//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//
// Constants for older PSDKs
//
#if !defined(VER_SUITE_WH_SERVER)
#define VER_SUITE_WH_SERVER 0x00008000
#endif


//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//
// LSGetKnownFolderIDList
// Wrapper around SHGetKnownFolderIDList. Helper for GetShellFolderPath.
//
HRESULT LSGetKnownFolderIDList(REFKNOWNFOLDERID rfid, PIDLIST_ABSOLUTE* ppidl)
{
    HRESULT hr = E_FAIL;
    
    HMODULE hShell32 = GetModuleHandle(_T("SHELL32.DLL"));
    
    typedef HRESULT (WINAPI* SHGetKnownFolderIDListProc)(
        REFKNOWNFOLDERID, DWORD, HANDLE, PIDLIST_ABSOLUTE*);
    
    SHGetKnownFolderIDListProc fnSHGetKnownFolderIDList =
        (SHGetKnownFolderIDListProc)GetProcAddress(
        hShell32, "SHGetKnownFolderIDList");
    
    if (fnSHGetKnownFolderIDList)
    {
        hr = fnSHGetKnownFolderIDList(rfid, 0, NULL, ppidl);
    }
    else
    {
        hr = E_NOTIMPL;
    }
    
    return hr;
}


//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//
// GetShellFolderPath
//
// Wrapper around SHGetSpecialFolderLocation to get around the lack of
// SHGetSpecialFolderPath on Win95. Also fetches the QuickLaunch folder.
//
bool GetShellFolderPath(int nFolder, LPTSTR ptzPath, size_t cchPath)
{
    ASSERT(cchPath >= MAX_PATH);
    ASSERT(NULL != ptzPath);
    
    HRESULT hr = E_FAIL;
    
    PIDLIST_ABSOLUTE pidl = NULL;
    
    if (nFolder == LS_CSIDL_QUICKLAUNCH)
    {
        if (IsVistaOrAbove())
        {
            //
            // Vista turned the QuickLaunch folder into a "Known Folder"
            // whose location can be customized. But it didn't retroactively
            // get a CSIDL constant so we have to use the known folder API.
            //
            hr = LSGetKnownFolderIDList(FOLDERID_QuickLaunch, &pidl);
        }
        else
        {
            //
            // Prior to Vista there is no documented way of getting the path.
            // We try to hardcode as little as possible.
            //
            hr = GetShellFolderPath(CSIDL_APPDATA, ptzPath, cchPath);
            
            if (SUCCEEDED(hr))
            {
                PathAppend(ptzPath,
                    _T("Microsoft\\Internet Explorer\\Quick Launch"));
                
                //
                // SHGetSpecialFolderLocation only returns directories that
                // exist, so we do the same for our custom QuickLaunch case.
                //
                if (!PathFileExists(ptzPath))
                {
                    hr = HRESULT_FROM_WIN32(ERROR_FILE_NOT_FOUND);
                }
            }
        }
    }
    else
    {
        // Starting with Vista, the "ALT" folders don't exist any more.
        // SHGetSpecialFolderLocation maps them to the non-"ALT" versions.
        if (IsVistaOrAbove() && 
           (nFolder == CSIDL_ALTSTARTUP || nFolder == CSIDL_COMMON_ALTSTARTUP))
        {
            hr = E_FAIL;
        }
        else
        {
            hr = SHGetSpecialFolderLocation(NULL, nFolder, &pidl);
        }
    }
    
    //
    // Convert PIDL to path
    //
    if (pidl != NULL)
    {
        if (SUCCEEDED(hr) && !SHGetPathFromIDList(pidl, ptzPath))
        {
            hr = E_FAIL;
        }
        
        CoTaskMemFree(pidl);
    }
    
    if (FAILED(hr))
    {
        ptzPath[0] = '\0';
    }
    
    return (SUCCEEDED(hr));
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
// PathAddBackslashEx
//
// Checked version of PathAddBackslash which also handles quoted paths
//
// Return values:  S_OK          - backslash appended
//                 S_FALSE       - path already ended with a backslash
//                 E_OUTOFMEMORY - buffer too small
//                 E_FAIL        - other failure (invalid input string)
//
HRESULT PathAddBackslashExA(LPSTR pszPath, size_t cchPath)
{
    ASSERT(cchPath <= STRSAFE_MAX_CCH);
    ASSERT(nullptr != pszPath); ASSERT(0 != cchPath);
    
    HRESULT hr = E_FAIL;
    size_t cchCurrentLength = 0;
    
    if (SUCCEEDED(StringCchLengthA(pszPath, cchPath, &cchCurrentLength)))
    {
        bool bHasQuote = false;
        LPSTR ptzEnd = pszPath + cchCurrentLength;
        
        if ((ptzEnd > pszPath) && (*(ptzEnd-1) == _T('\"')))
        {
            --ptzEnd;
            bHasQuote = true;
        }
        
        if (ptzEnd > pszPath)
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
                    
                    ASSERT((size_t)(ptzEnd - pszPath) < cchPath);
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
    
    HRESULT hr = ProgIDFromCLSID(rclsid, &pOleString);
    
    if (FAILED(hr))
    {
        hr = StringFromCLSID(rclsid, &pOleString);
    }
    
    if (SUCCEEDED(hr) && pOleString)
    {
#if defined(UNICODE)
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
        pszBuffer[cchBuffer-1] = L'\0';
    }
    else if (cchCopied > 0 && cchCopied < cchBuffer)
    {
        bSuccess = true;
    }
    
    return bSuccess;
}


//
// LSGetModuleFileNameEx
//
// Wrapper around GetModuleFileNameEx, dealing with that it moved from
// psapi to Kernel32 with Windows 7.
//
DWORD LSGetModuleFileNameEx(HANDLE hProcess, HMODULE hModule, LPTSTR pszBuffer, DWORD cchBuffer)
{
    typedef DWORD(WINAPI * GetModuleProc)(HANDLE hProcess, HMODULE hModule, LPTSTR pszBuffer, DWORD cchBuffer);
    static GetModuleProc proc = nullptr;

    if (proc == nullptr)
    {
        proc = (GetModuleProc)GetProcAddress(GetModuleHandle(_T("Kernel32.dll")), "K32GetModuleFileNameExW");
        if (proc == nullptr)
        {
            // Try pre Windows 7
            proc = (GetModuleProc)GetProcAddress(LoadLibrary(_T("Psapi.dll")), "GetModuleFileNameExW");
        }
    }

    if (proc)
    {
        return proc(hProcess, hModule, pszBuffer, cchBuffer);
    }

    return 0;
}


//
// LSGetProcessImageFileName
//
// Wrapper around GetProcessImageFileName, dealing with that it moved from
// psapi to Kernel32 with Windows 7.
//
DWORD LSGetProcessImageFileName(HANDLE hProcess, LPTSTR pszBuffer, DWORD cchBuffer)
{
    typedef DWORD(WINAPI * GetImageNameProc)(HANDLE hProcess,  LPTSTR pszBuffer, DWORD cchBuffer);
    static GetImageNameProc proc = nullptr;

    if (proc == nullptr)
    {
        proc = (GetImageNameProc)GetProcAddress(GetModuleHandle(_T("Kernel32.dll")), "K32GetProcessImageFileNameW");
        if (proc == nullptr)
        {
            // Try pre Windows 7
            proc = (GetImageNameProc)GetProcAddress(LoadLibrary(_T("Psapi.dll")), "GetProcessImageFileNameW");
        }
    }

    if (proc)
    {
        return proc(hProcess, pszBuffer, cchBuffer);
    }

    return 0;
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
    OSVERSIONINFOEX ovi = { 0 };
    ovi.dwOSVersionInfoSize = sizeof(ovi);
    ovi.dwMajorVersion = 6;
    ovi.dwPlatformId = VER_PLATFORM_WIN32_NT;

    ULONGLONG uConditionMask = 0;
    uConditionMask = VerSetConditionMask(uConditionMask, VER_MAJORVERSION, VER_GREATER_EQUAL);
    uConditionMask = VerSetConditionMask(uConditionMask, VER_PLATFORMID, VER_EQUAL);
    return VerifyVersionInfo(&ovi, VER_MAJORVERSION | VER_PLATFORMID, uConditionMask) != FALSE;
}


//
// LSShutdownDialog
//
void LSShutdownDialog(HWND hWnd)
{
    FARPROC fnProc = GetProcAddress(
        GetModuleHandleW(L"SHELL32.DLL"), (LPCSTR)((long)0x003C));
    
    if (fnProc)
    {
        if (IsVistaOrAbove())
        {
            typedef VOID (WINAPI* ExitWindowsDialogProc)(HWND, DWORD);
            
            ExitWindowsDialogProc fnExitWindowsDialog = \
                (ExitWindowsDialogProc)fnProc;
            
            // Meaning of second parameter unknown
            fnExitWindowsDialog(hWnd, NULL);
        }
        else
        {
            typedef VOID (WINAPI* ExitWindowsDialogProc)(HWND);
            
            ExitWindowsDialogProc fnExitWindowsDialog = \
                (ExitWindowsDialogProc)fnProc;
            
            fnExitWindowsDialog(hWnd);
        }
        
        // provide same mechansim for exiting the shell as explorer
        if ((GetAsyncKeyState(VK_SHIFT) & 0x8000) &&
            (GetAsyncKeyState(VK_CONTROL) & 0x8000) &&
            (GetAsyncKeyState(VK_MENU) & 0x8000))
        {
            PostQuitMessage(LRV_SHUTDOWN_DLG);
        }
    }
}


//
// LSPlaySystemSound
// Wrapper around PlaySound to play system event sounds
//
BOOL LSPlaySystemSound(LPCWSTR pwzSoundAlias)
{
    BOOL bResult = FALSE;
    
    // We want to avoid linking to winmm.dll as long as it's used just here
    HMODULE hWinMM = LoadLibrary(_T("winmm.dll"));
    
    if (hWinMM)
    {
        typedef BOOL (WINAPI* PlaySoundProc)(LPCWSTR, HMODULE, DWORD);
        
        PlaySoundProc fnPlaySound = (PlaySoundProc)
            GetProcAddress(hWinMM, "PlaySoundW");
        
        if (fnPlaySound)
        {
            // Use SND_NODEFAULT to make sure that not even a default sound is
            // played if the sound alias has no corresponding sound set
            // (if e.g. the user has disabled this particular sound)
            DWORD dwFlags = SND_ALIAS | SND_NODEFAULT;
            
            if (IsVistaOrAbove())
            {
                // Vista has a special volume slider for system sounds, ie. all
                // those beeps etc. can be adjusted independently of all other
                // sounds. SND_SYSTEM makes PlaySound use that slider.
                dwFlags |= SND_SYSTEM;
            }
            
            bResult = fnPlaySound(pwzSoundAlias, NULL, dwFlags);
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
    
#if defined(MSVC_DEBUG)
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


//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//
// GetVersionMetric
// Helper for GetWindowsVersion
//
enum class WINVER_METRIC
{
    ANY,
    SERVER,
    WORKSTATION,
    HOMESERVER
};


BOOL GetVersionMetric(WINVER_METRIC metric)
{
    // NB: OSVERSIONINFOEX is not supported on very early platforms but
    //     those don't require such a "metric"
    OSVERSIONINFOEXW ovi = { 0 };
    ovi.dwOSVersionInfoSize = sizeof(ovi);

    ULONGLONG dwlConditionMask = 0;

    switch (metric)
    {
        case WINVER_METRIC::ANY:
            return TRUE;
            
        case WINVER_METRIC::SERVER:
            ovi.wProductType = VER_NT_WORKSTATION;
            dwlConditionMask = VerSetConditionMask(dwlConditionMask, VER_PRODUCT_TYPE, VER_EQUAL);
            return VerifyVersionInfoW(&ovi, VER_PRODUCT_TYPE, dwlConditionMask) == FALSE;
            
        case WINVER_METRIC::WORKSTATION:
            ovi.wProductType = VER_NT_WORKSTATION;
            dwlConditionMask = VerSetConditionMask(dwlConditionMask, VER_PRODUCT_TYPE, VER_EQUAL);
            return VerifyVersionInfoW(&ovi, VER_PRODUCT_TYPE, dwlConditionMask) != FALSE;
            
        case WINVER_METRIC::HOMESERVER:
            ovi.wSuiteMask = VER_SUITE_WH_SERVER;
            dwlConditionMask = VerSetConditionMask(dwlConditionMask, VER_SUITENAME, VER_EQUAL);
            return VerifyVersionInfoW(&ovi, VER_SUITENAME, dwlConditionMask) != FALSE;
            
        default:
            ASSERT(false);
            break;
    }
    
    return FALSE;
}


//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//
// IsWindowsVersionOrGreater
//
// Taken from VersionHelpers.h
//
bool IsWindowsVersionOrGreater(WORD wMajorVersion, WORD wMinorVersion, WORD wServicePackMajor)
{
    OSVERSIONINFOEXW osvi = { sizeof(osvi), 0, 0, 0, 0, {0}, 0, 0 };
    DWORDLONG        const dwlConditionMask = VerSetConditionMask(
        VerSetConditionMask(
        VerSetConditionMask(
            0, VER_MAJORVERSION, VER_GREATER_EQUAL),
               VER_MINORVERSION, VER_GREATER_EQUAL),
               VER_SERVICEPACKMAJOR, VER_GREATER_EQUAL);

    osvi.dwMajorVersion = wMajorVersion;
    osvi.dwMinorVersion = wMinorVersion;
    osvi.wServicePackMajor = wServicePackMajor;

    return VerifyVersionInfoW(&osvi, VER_MAJORVERSION | VER_MINORVERSION | VER_SERVICEPACKMAJOR, dwlConditionMask) != FALSE;
}


//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//
// GetWindowsVersion
//
UINT GetWindowsVersion()
{
    UINT uVersion = WINVER_UNKNOWN;

    struct VerStringTable
    {
        WORD dwMajor;
        WORD dwMinor;
        WINVER_METRIC metric;
        UINT uVersion;
    }
    versions[] = \
    {
        { 6,  3, WINVER_METRIC::SERVER,      WINVER_WIN2012R2 },
        { 6,  3, WINVER_METRIC::WORKSTATION, WINVER_WIN81     },

        { 6,  2, WINVER_METRIC::SERVER,      WINVER_WIN2012   },
        { 6,  2, WINVER_METRIC::WORKSTATION, WINVER_WIN8      },
            
        { 6,  1, WINVER_METRIC::SERVER,      WINVER_WIN2008R2 },
        { 6,  1, WINVER_METRIC::WORKSTATION, WINVER_WIN7      },

        { 6,  0, WINVER_METRIC::SERVER,      WINVER_WIN2008   },
        { 6,  0, WINVER_METRIC::WORKSTATION, WINVER_VISTA     },
            
        // WVM_HOMESERVER should also match WVM_SERVER, so list it first
        { 5,  2, WINVER_METRIC::HOMESERVER,  WINVER_WHS       },
        { 5,  2, WINVER_METRIC::SERVER,      WINVER_WIN2003   },
        { 5,  2, WINVER_METRIC::WORKSTATION, WINVER_WINXP     }, // 64-Bit

        { 5,  1, WINVER_METRIC::ANY,         WINVER_WINXP     }, // 32-Bit
        { 5,  0, WINVER_METRIC::ANY,         WINVER_WIN2000   },
        { 4,  0, WINVER_METRIC::ANY,         WINVER_WINNT4    }
    };
        
    for (VerStringTable &version : versions)
    {
        if (IsWindowsVersionOrGreater(version.dwMajor, version.dwMinor, 0) &&
            GetVersionMetric(version.metric))
        {
            uVersion = version.uVersion;
            break;
        }
    }
    
    return uVersion;
}


//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//
// LSDisableWow64FsRedirection
//
BOOL LSDisableWow64FsRedirection(PVOID* ppvOldValue)
{
#ifndef _WIN64
    typedef BOOL (WINAPI* Wow64DisableWow64FsRedirectionProc)(PVOID*);
    
    HMODULE hKernel32 = GetModuleHandle(_T("kernel32.dll"));
    
    Wow64DisableWow64FsRedirectionProc fnWow64DisableWow64FsRedirection = \
        (Wow64DisableWow64FsRedirectionProc)GetProcAddress(
        hKernel32, "Wow64DisableWow64FsRedirection");
    
    BOOL bResult = TRUE;
    
    if (fnWow64DisableWow64FsRedirection)
    {
        bResult = fnWow64DisableWow64FsRedirection(ppvOldValue);
    }
    
    return bResult;
#else
    UNREFERENCED_PARAMETER(ppvOldValue);
    return TRUE;
#endif
}


//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//
// LSRevertWow64FsRedirection
//
BOOL LSRevertWow64FsRedirection(PVOID pvOldValue)
{
#ifndef _WIN64
    typedef BOOL (WINAPI* Wow64RevertWow64FsRedirectionProc)(PVOID);
    
    HMODULE hKernel32 = GetModuleHandle(_T("kernel32.dll"));
    
    Wow64RevertWow64FsRedirectionProc fnWow64RevertWow64FsRedirection =
        (Wow64RevertWow64FsRedirectionProc)GetProcAddress(
        hKernel32, "Wow64RevertWow64FsRedirection");
    
    BOOL bResult = TRUE;
    
    if (fnWow64RevertWow64FsRedirection)
    {
        bResult = fnWow64RevertWow64FsRedirection(pvOldValue);
    }
    
    return bResult;
#else
    UNREFERENCED_PARAMETER(pvOldValue);
    return TRUE;
#endif
}


//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//
// LSShellExecuteEx
//
BOOL LSShellExecuteEx(LPSHELLEXECUTEINFOW lpExecInfo)
{
    PVOID pvOldValue = nullptr;
    LSDisableWow64FsRedirection(&pvOldValue);
    
    BOOL bReturn = ShellExecuteExW(lpExecInfo);
    
    LSRevertWow64FsRedirection(pvOldValue);
    return bReturn;
}


//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//
// LSShellExecuteA
//
HINSTANCE LSShellExecute(HWND hwnd, LPCWSTR lpOperation, LPCWSTR lpFile,
                          LPCWSTR lpParameters, LPCWSTR lpDirectory, INT nShow)
{
    PVOID pvOldValue = nullptr;
    LSDisableWow64FsRedirection(&pvOldValue);

    HINSTANCE hinstResult = ShellExecuteW(
        hwnd, lpOperation, lpFile, lpParameters, lpDirectory, nShow);
    
    LSRevertWow64FsRedirection(pvOldValue);
    return hinstResult;
}


//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//
// LSActivateActCtxForDll
// Activates the custom activation context for the specified DLL
//
HANDLE LSActivateActCtxForDll(LPCTSTR pszDll, PULONG_PTR pulCookie)
{
    HANDLE hContext = INVALID_HANDLE_VALUE;
    
    typedef HANDLE (WINAPI* CreateActCtx_t)(PACTCTX pCtx);
    typedef BOOL (WINAPI* ActivateActCtx_t)(HANDLE hCtx, ULONG_PTR* pCookie);
    
#if defined(UNICODE)
    CreateActCtx_t fnCreateActCtx = (CreateActCtx_t)
        GetProcAddress(GetModuleHandle(_T("KERNEL32")), "CreateActCtxW");
#else
    CreateActCtx_t fnCreateActCtx = (CreateActCtx_t)
        GetProcAddress(GetModuleHandle(_T("KERNEL32")), "CreateActCtxA");
#endif
    
    ActivateActCtx_t fnActivateActCtx = (ActivateActCtx_t)
        GetProcAddress(GetModuleHandle(_T("KERNEL32")), "ActivateActCtx");
    
    if (fnCreateActCtx != NULL && fnActivateActCtx != NULL)
    {
        ACTCTX act = { 0 };
        act.cbSize = sizeof(act);
        act.dwFlags = ACTCTX_FLAG_RESOURCE_NAME_VALID;
        act.lpSource = pszDll;
        act.lpResourceName = MAKEINTRESOURCE(123);
        
        hContext = fnCreateActCtx(&act);
        
        if (hContext != INVALID_HANDLE_VALUE)
        {
            if (!fnActivateActCtx(hContext, pulCookie))
            {
                LSDeactivateActCtx(hContext, NULL);
                hContext = INVALID_HANDLE_VALUE;
            }
        }
    }
    
    return hContext;
}


//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//
// LSActivateActCtxForClsid
// Activates the custom activation context for the specified CLSID
//
HANDLE LSActivateActCtxForClsid(REFCLSID rclsid, PULONG_PTR pulCookie)
{
    HANDLE hContext = INVALID_HANDLE_VALUE;
    TCHAR szCLSID[39] = { 0 };
    
    //
    // Get the DLL that implements the COM object in question
    //
    if (SUCCEEDED(CLSIDToString(rclsid, szCLSID, COUNTOF(szCLSID))))
    {
        TCHAR szSubkey[MAX_PATH] = { 0 };
        
        HRESULT hr = StringCchPrintf(szSubkey, COUNTOF(szSubkey),
            _T("CLSID\\%ls\\InProcServer32"), szCLSID);
        
        if (SUCCEEDED(hr))
        {
            TCHAR szDll[MAX_PATH] = { 0 };
            DWORD cbDll = sizeof(szDll);
            
            LONG lres = SHGetValue(
                HKEY_CLASSES_ROOT, szSubkey, NULL, NULL, szDll, &cbDll);
            
            if (lres == ERROR_SUCCESS)
            {
                //
                // Activate the custom manifest (if any) of that DLL
                //
                hContext = LSActivateActCtxForDll(szDll, pulCookie);
            }
        }
    }
    
    return hContext;
}


//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//
// LSDeactivateActCtx
// Removes an activation context from the activation context stack
//
void LSDeactivateActCtx(HANDLE hActCtx, ULONG_PTR* pulCookie)
{
    typedef BOOL (WINAPI* DeactivateActCtx_t)(DWORD dwFlags, ULONG_PTR ulc);
    typedef void (WINAPI* ReleaseActCtx_t)(HANDLE hActCtx);
    
    DeactivateActCtx_t fnDeactivateActCtx = (DeactivateActCtx_t)
        GetProcAddress(GetModuleHandle(_T("KERNEL32")), "DeactivateActCtx");
    
    ReleaseActCtx_t fnReleaseActCtx = (ReleaseActCtx_t)
        GetProcAddress(GetModuleHandle(_T("KERNEL32")), "ReleaseActCtx");
    
    if (fnDeactivateActCtx != NULL && fnReleaseActCtx != NULL)
    {
        if (hActCtx != INVALID_HANDLE_VALUE)
        {
            if (pulCookie != NULL)
            {
                fnDeactivateActCtx(0, *pulCookie);
            }
            
            fnReleaseActCtx(hActCtx);
        }
    }
}


//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//
// DescriptionFromHR
// Retrives a description of a HRESULT error code.
//
HRESULT DescriptionFromHR(HRESULT hr, LPWSTR buf, size_t cchBuf) {
    if (FACILITY_WINDOWS == HRESULT_FACILITY(hr))
    {
         hr = HRESULT_CODE(hr);
    }

    if (FormatMessageW(FORMAT_MESSAGE_FROM_SYSTEM, nullptr, hr,
        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), buf, DWORD(cchBuf), nullptr) == 0)
    {
        return StringCchPrintfW(buf, cchBuf, L"Unknown error, 0x%.8X", hr);
    }

    return S_OK;
}
