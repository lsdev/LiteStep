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
#include "StartupRunner.h"
#include "../utility/shellhlp.h"
#include <regstr.h>
#include "../utility/core.hpp"

#ifdef __GNUC__ // mingw
enum RESTRICTIONS
{
    REST_NOLOCALMACHINERUN     = 0x40000046,
    REST_NOCURRENTUSERRUN      = 0x40000047,
    REST_NOLOCALMACHINERUNONCE = 0x40000048,
    REST_NOCURRENTUSERRUNONCE  = 0x40000049
};
#endif

#define ERK_NONE                0x0000
#define ERK_RUNSUBKEYS          0x0001 // runs key and its subkeys
#define ERK_DELETE              0x0002
#define ERK_WAITFOR_QUIT        0x0004 // wait until process exits
#define ERK_WAITFOR_IDLE        0x0008 // wait until process waits for input

#ifndef REGSTR_PATH_RUN_POLICY
#  define REGSTR_PATH_RUN_POLICY  REGSTR_PATH_POLICIES _T("\\Explorer\\Run")
#endif

StartupRunner::StartupRunner()
{}

StartupRunner::~StartupRunner()
{}


void StartupRunner::Run(BOOL bForce)
{
    CloseHandle(LSCreateThread("StartupRunner",
        StartupRunner::_ThreadProc, (LPVOID)(INT_PTR)bForce, NULL));
}


DWORD WINAPI StartupRunner::_ThreadProc(LPVOID lpData)
{
    bool bRunStartup = IsFirstRunThisSession(_T("StartupHasBeenRun"));
    BOOL bForceStartup = (lpData != 0);

    if (IsVistaOrAbove())
    {
        // On Vista there's this additional subkey.
        // Its meaning is currently unknown. We create but ignore it.
        IsFirstRunThisSession(_T("RunStuffHasBeenRun"));
    }

    // by keeping the call to _IsFirstRunThisSession() above we make sure the
    // regkey is created even if we're in "force startup" mode
    if (bRunStartup || bForceStartup)
    {
        // Need to call CoInitializeEx for ShellExecuteEx
        VERIFY_HR(CoInitializeEx(
            NULL, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE));

        bool bHKLMRun = true;
        bool bHKCURun = true;
        bool bHKLMRunOnce = true;
        bool bHKCURunOnce = true;
        
        //
        // SHRestricted is not available on Windows 95
        //
        typedef DWORD (WINAPI* SHREST_PROC)(RESTRICTIONS);
        
        SHREST_PROC pSHRestricted = (SHREST_PROC)GetProcAddress(
            GetModuleHandle(_T("shell32.dll")), (LPCSTR)((long)0x0064));
        
        if (pSHRestricted)
        {
            bHKLMRun = !pSHRestricted(REST_NOLOCALMACHINERUN);
            bHKCURun = !pSHRestricted(REST_NOCURRENTUSERRUN);
            bHKLMRunOnce = !pSHRestricted(REST_NOLOCALMACHINERUNONCE);
            bHKCURunOnce = !pSHRestricted(REST_NOCURRENTUSERRUNONCE);
        }
        
        if (bHKLMRunOnce)
        {
            _RunRegKeys(HKEY_LOCAL_MACHINE, REGSTR_PATH_RUNONCE,
                (ERK_RUNSUBKEYS | ERK_DELETE | ERK_WAITFOR_QUIT));
        }
        
        _RunRunOnceEx();
        
        if (bHKLMRun)
        {
            _RunRegKeys(HKEY_LOCAL_MACHINE, REGSTR_PATH_RUN, ERK_NONE);
        }
        
        _RunRegKeys(HKEY_LOCAL_MACHINE, REGSTR_PATH_RUN_POLICY, ERK_NONE);
        _RunRegKeys(HKEY_CURRENT_USER, REGSTR_PATH_RUN_POLICY, ERK_NONE);
        
        if (bHKCURun)
        {
            _RunRegKeys(HKEY_CURRENT_USER, REGSTR_PATH_RUN, ERK_NONE);
        }
        
        _RunStartupMenu();
        
        if (bHKCURunOnce)
        {
            _RunRegKeys(HKEY_CURRENT_USER, REGSTR_PATH_RUNONCE,
                (ERK_RUNSUBKEYS | ERK_DELETE));
        }

        CoUninitialize();
    }

    return bRunStartup;
}

void StartupRunner::_RunRunOnceEx()
{
    TCHAR szArgs[MAX_PATH] = { 0 };
    UINT uChars = GetSystemDirectory(szArgs, MAX_PATH);
    
    if (uChars > 0 && uChars < MAX_PATH)
    {
        if (SUCCEEDED(StringCchCat(szArgs, MAX_PATH, _T("\\iernonce.dll"))))
        {
            // The file doesn't exist on NT4
            if (PathFileExists(szArgs) && SUCCEEDED(StringCchCat(szArgs,
                MAX_PATH, _T(",RunOnceExProcess"))))
            {
                LSShellExecute(NULL,
                    _T("open"), _T("rundll32.exe"), szArgs, NULL, SW_NORMAL);
            }
        }
    }
}

void StartupRunner::_RunStartupMenu()
{
    // Starting with Vista, the "ALT" CSIDLs are deprecated
    // and are mapped to the regular non-ALT versions.
    _RunShellFolderContents(CSIDL_COMMON_STARTUP);

    if (!IsVistaOrAbove())
    {
        _RunShellFolderContents(CSIDL_COMMON_ALTSTARTUP);
    }

    _RunShellFolderContents(CSIDL_STARTUP);

    if (!IsVistaOrAbove())
    {
        _RunShellFolderContents(CSIDL_ALTSTARTUP);
    }
}

void StartupRunner::_RunShellFolderContents(int nFolder)
{
    TCHAR tzPath[MAX_PATH] = { 0 };

    if (GetShellFolderPath(nFolder, tzPath, COUNTOF(tzPath)))
    {
        if (tzPath[0])
        {
            TCHAR tzSearchPath[MAX_PATH] = { 0 };
            PathCombine(tzSearchPath, tzPath, _T("*.*"));

            WIN32_FIND_DATA findData = { 0 };
            HANDLE hSearch = FindFirstFile(tzSearchPath, &findData);

            while (hSearch != INVALID_HANDLE_VALUE)
            {
                if (!PathIsDirectory(findData.cFileName) &&
                    !(findData.dwFileAttributes & FILE_ATTRIBUTE_SYSTEM) &&
                    !(findData.dwFileAttributes & FILE_ATTRIBUTE_HIDDEN))
                {
                    SHELLEXECUTEINFO seiCommand = { 0 };

                    seiCommand.cbSize = sizeof(SHELLEXECUTEINFO);
                    seiCommand.lpFile = findData.cFileName;
                    seiCommand.lpDirectory = tzPath;
                    seiCommand.nShow = SW_SHOWNORMAL;
                    seiCommand.fMask =
                        SEE_MASK_DOENVSUBST | SEE_MASK_FLAG_NO_UI;

                    VERIFY(LSShellExecuteEx(&seiCommand));
                }

                if (!FindNextFile(hSearch, &findData))
                {
                    FindClose(hSearch);
                    hSearch = INVALID_HANDLE_VALUE;
                }
            }
        }
    }
    else
    {
        TRACE("Failed to get full path to Startup folder %d", nFolder);
    }
}


//
// _CreateSessionInfoKey
//
// Note that unlike _IsFirstRunThisSession this function can be called
// multiple times without side-effects.
//
HKEY StartupRunner::_CreateSessionInfoKey()
{
    HKEY hkSessionInfo = NULL;
    HANDLE hToken = NULL;

    if (OpenProcessToken(GetCurrentProcess(), TOKEN_QUERY, &hToken))
    {
        HRESULT hr = E_FAIL;

        TCHAR tzSessionInfo[128] = { 0 };
        DWORD dwOutSize = 0;

        if (IsVistaOrAbove())
        {
            DWORD dwSessionId = 0;

            // On Vista the subkey's name is the Session ID
            if (GetTokenInformation(hToken, TokenSessionId,
                &dwSessionId, sizeof(dwSessionId), &dwOutSize))
            {
                hr = StringCchPrintf(tzSessionInfo, COUNTOF(tzSessionInfo),
                    REGSTR_PATH_EXPLORER _T("\\SessionInfo\\%u"), dwSessionId);
            }
        }
        else
        {
            TOKEN_STATISTICS tsStats = { 0 };

            // Prior to Vista the subkey's name is the AuthenticationId
            if (GetTokenInformation(hToken, TokenStatistics,
                &tsStats, sizeof(tsStats), &dwOutSize))
            {
                hr = StringCchPrintf(tzSessionInfo, COUNTOF(tzSessionInfo),
                    REGSTR_PATH_EXPLORER _T("\\SessionInfo\\%08x%08x"),
                    tsStats.AuthenticationId.HighPart,
                    tsStats.AuthenticationId.LowPart);
            }
        }

        if (SUCCEEDED(hr))
        {
            // Finally open the SessionInfo key
            LONG lResult = RegCreateKeyEx(
                HKEY_CURRENT_USER, tzSessionInfo, 0, NULL,
                REG_OPTION_VOLATILE, KEY_WRITE, NULL, &hkSessionInfo, NULL);

            if (lResult != ERROR_SUCCESS)
            {
                hkSessionInfo = NULL;
            }
        }

        CloseHandle(hToken);
    }

    return hkSessionInfo;
}


//
// IsFirstRunThisSession()
//
bool StartupRunner::IsFirstRunThisSession(LPCTSTR pszSubkey)
{
    bool bReturn = false;

    OSVERSIONINFO OsVersionInfo = { 0 };
    OsVersionInfo.dwOSVersionInfoSize = sizeof(OsVersionInfo);
    VERIFY(GetVersionEx(&OsVersionInfo));

    if (OsVersionInfo.dwPlatformId == VER_PLATFORM_WIN32_NT)
    {
        HKEY hkSessionInfo = _CreateSessionInfoKey();

        if (hkSessionInfo != NULL)
        {
            DWORD dwDisposition;
            HKEY hkStartup;

            LONG lResult = RegCreateKeyEx(
                hkSessionInfo, pszSubkey, 0, NULL, REG_OPTION_VOLATILE,
                KEY_WRITE, NULL, &hkStartup, &dwDisposition);

            RegCloseKey(hkStartup);

            if (lResult == ERROR_SUCCESS &&
                dwDisposition == REG_CREATED_NEW_KEY)
            {
                bReturn = true;
            }
        }

        RegCloseKey(hkSessionInfo);
    }
    else
    {
        bReturn = true;
    }

    return bReturn;
}


//
// RunRegKeys(HKEY hkParent, LPCSTR pszSubKey, DWORD dwFlags)
//
void StartupRunner::_RunRegKeys(HKEY hkParent, LPCTSTR ptzSubKey, DWORD dwFlags)
{
    HKEY hkSubKey;
    
    LONG lResult = RegOpenKeyEx(hkParent, ptzSubKey, 0, MAXIMUM_ALLOWED,
        &hkSubKey);
    
    if (lResult == ERROR_SUCCESS)
    {
        //
        // Run the key itself
        //
        for (DWORD dwLoop = 0; ; ++dwLoop)
        {
            TCHAR tzNameBuffer[MAX_PATH] = { 0 };
            TCHAR tzValueBuffer[MAX_LINE_LENGTH] = { 0 };
            
            DWORD dwNameSize = MAX_PATH;
            DWORD dwValueSize = MAX_LINE_LENGTH;
            DWORD dwType;
            
            lResult = RegEnumValue(hkSubKey, dwLoop, tzNameBuffer, &dwNameSize,
                NULL, &dwType, (LPBYTE)tzValueBuffer, &dwValueSize);
            
            if (lResult == ERROR_MORE_DATA)
            {
                // tzNameBuffer too small?
                continue;
            }
            else if (lResult == ERROR_SUCCESS)
            {
                if ((dwType == REG_SZ) || (dwType == REG_EXPAND_SZ))
                {
                    if (tzValueBuffer[0])
                    {
                        _SpawnProcess(tzValueBuffer, dwFlags);
                    }
                    
                    if ((dwFlags & ERK_DELETE) && (tzNameBuffer[0] != _T('!')))
                    {
                        if (RegDeleteValue(hkSubKey, tzNameBuffer) ==
                            ERROR_SUCCESS)
                        {
                            --dwLoop;
                        }
                    }
                }
            }
            else
            {
                break;
            }
        }
        
        //
        // Run subkeys as well?
        //
        if (dwFlags & ERK_RUNSUBKEYS)
        {
            dwFlags &= ~(ERK_RUNSUBKEYS);
            
            for (DWORD dwLoop = 0; ; ++dwLoop)
            {
                TCHAR tzNameBuffer[MAX_PATH] = { 0 };
                lResult = RegEnumKey(hkSubKey, dwLoop, tzNameBuffer, MAX_PATH);
                
                if (lResult == ERROR_MORE_DATA)
                {
                    // tzNameBuffer too small?
                    continue;
                }
                else if (lResult == ERROR_SUCCESS)
                {
                    _RunRegKeys(hkSubKey, tzNameBuffer, dwFlags);
                    
                    if (dwFlags & ERK_DELETE)
                    {
                        if (RegDeleteKey(hkSubKey, tzNameBuffer) ==
                            ERROR_SUCCESS)
                        {
                            --dwLoop;
                        }
                    }
                }
                else
                {
                    break;
                }
            }
        }
    }
    
    RegCloseKey(hkSubKey);
}


void StartupRunner::_SpawnProcess(LPTSTR ptzCommandLine, DWORD dwFlags)
{
    ASSERT(!(dwFlags & ERK_WAITFOR_QUIT && dwFlags & ERK_WAITFOR_IDLE));
    
    //
    // The following cases need to be supported:
    //
    // 1. "C:\Program Files\App\App.exe" -params
    // 2. C:\Program Files\App\App.exe -params
    // 3. App.exe -params   (app.exe is in %path% or HKLM->REGSTR_PATH_APPPATHS)
    // and all the above cases without arguments.
    //
    // CreateProcess handles 1 and 2, ShellExecuteEx handles 1 and 3.
    // So if the first token doesn't contain path characters (':' or '\')
    // ShellExecuteEx is used. That's really ugly but it *should* work.
    //
    
    TCHAR tzToken[MAX_LINE_LENGTH] = { 0 };
    LPCTSTR ptzArgs = NULL;
    
    GetToken(ptzCommandLine, tzToken, &ptzArgs, FALSE);
    
    HANDLE hProcess = NULL;
    
    if (strchr(tzToken, _T('\\')) || strchr(tzToken, _T(':')))
    {
        hProcess = _CreateProcess(ptzCommandLine);
    }
    else
    {
        hProcess = _ShellExecuteEx(tzToken, ptzArgs);
    }
    
    if (hProcess != NULL)
    {
        if (dwFlags & ERK_WAITFOR_QUIT)
        {
            WaitForSingleObject(hProcess, INFINITE);
        }
        else if (dwFlags & ERK_WAITFOR_IDLE)
        {
            WaitForInputIdle(hProcess, INFINITE);
        }
    
        CloseHandle(hProcess);
    }
}


HANDLE StartupRunner::_CreateProcess(LPTSTR ptzCommandLine)
{
    HANDLE hReturn = NULL;
    
    STARTUPINFO suInfo = { 0 };
    PROCESS_INFORMATION procInfo = { 0 };
    
    suInfo.cb = sizeof(suInfo);
    
    if (CreateProcess(NULL, ptzCommandLine, NULL, NULL, FALSE,
        CREATE_DEFAULT_ERROR_MODE | NORMAL_PRIORITY_CLASS,
        NULL, NULL, &suInfo, &procInfo))
    {
        CloseHandle(procInfo.hThread);
        hReturn = procInfo.hProcess;
    }
    
    return hReturn;
}


HANDLE StartupRunner::_ShellExecuteEx(LPCTSTR ptzExecutable, LPCTSTR ptzArgs)
{
    HANDLE hReturn = NULL;
    
    SHELLEXECUTEINFO sei = { 0 };
    sei.cbSize = sizeof(sei);
    sei.lpFile = ptzExecutable;
    sei.lpParameters = ptzArgs;
    sei.nShow = SW_SHOWNORMAL;
    sei.fMask =
        SEE_MASK_DOENVSUBST | SEE_MASK_FLAG_NO_UI | SEE_MASK_NOCLOSEPROCESS;

    if (LSShellExecuteEx(&sei))
    {
        hReturn = sei.hProcess;
    }

    return hReturn;
}
