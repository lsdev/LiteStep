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
#include "litestep.h"
#include "../utility/macros.h"
#include "../utility/core.hpp"


// How long to wait for Explorer to start up as shell, in milliseconds
// Shouldn't use INFINITE since we may block forever in safe mode
#define EXPLORER_WAIT_TIMEOUT   20000


//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//
// IsOtherInstanceRunning
// Checks if another LiteStep instance is running already
//
bool IsOtherInstanceRunning(LPHANDLE phMutex)
{
    ASSERT(phMutex);
    bool bIsOther = false;
    
    *phMutex = CreateMutex(NULL, FALSE, _T("LiteStep"));
    
    if (*phMutex && (GetLastError() == ERROR_ALREADY_EXISTS))
    {
        bIsOther = true;
    }
    
    return bIsOther;
}


//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//
// ParseCommandLine
// Converts command line parameters to start flags
//
WORD ParseCommandLine(LPCTSTR pszCommandLine, LPTSTR pszFile, DWORD cchFile)
{
    ASSERT(pszCommandLine);
    
    // By default, run LiteStep and startup apps
    WORD wStartFlags = LSF_RUN_LITESTEP | LSF_RUN_STARTUPAPPS;
    
    TCHAR szToken[MAX_LINE_LENGTH] = { 0 };
    LPCTSTR pszNextToken = pszCommandLine;
    
    while (GetTokenW(pszNextToken, szToken, &pszNextToken, FALSE))
    {
        if (szToken[0] == '-')
        {
            if (!_tcsicmp(szToken, _T("-nostartup")))
            {
                wStartFlags &= ~LSF_RUN_STARTUPAPPS;
            }
            else if (!_tcsicmp(szToken, _T("-startup")))
            {
                wStartFlags |= LSF_FORCE_STARTUPAPPS;
            }
            else if (!_tcsicmp(szToken, _T("-explorer")))
            {
                wStartFlags &= ~LSF_RUN_LITESTEP;
                wStartFlags |= LSF_RUN_EXPLORER;
            }
        }
        else
        {
            ASSERT(szToken[0] != '!');
            DWORD dwCopied = GetFullPathName(szToken, cchFile, pszFile, NULL);
            
            if (dwCopied == 0 || dwCopied > cchFile)
            {
                pszFile[0] = _T('\0');
            }
            
            wStartFlags |= LSF_ALTERNATE_CONFIG;
        }
    }
    
    return wStartFlags;
}


//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//
// SendCommandLineBang
// Sends given !bang to a running litestep.exe instance
//
bool SendCommandLineBang(LPCTSTR pszCommand, LPCTSTR pszArgs)
{
    ASSERT(pszCommand);
    bool bSuccess = false;
    
    HWND hWnd = FindWindow(szMainWindowClass, szMainWindowTitle);
    
    if (IsWindow(hWnd))
    {
        LMBANGCOMMAND bangCommand;
        bangCommand.cbSize = sizeof(LMBANGCOMMAND);
        bangCommand.hWnd = NULL;
        
        HRESULT hr = StringCchCopy(
            bangCommand.wzCommand, MAX_BANGCOMMAND, pszCommand);
        
        if (SUCCEEDED(hr))
        {
            if (pszArgs)
            {
                hr = StringCchCopy(bangCommand.wzArgs, MAX_BANGARGS, pszArgs);
            }
            else
            {
                bangCommand.wzArgs[0] = '\0';
            }
        }
        
        if (SUCCEEDED(hr))
        {
            // Since we're a new, different litestep.exe process here, give the
            // other, "real" instance the right to set the foreground window
            TryAllowSetForegroundWindow(hWnd);
            
            COPYDATASTRUCT cds = { 0 };
            
            cds.cbData = sizeof(LMBANGCOMMAND);
            cds.dwData = LM_BANGCOMMANDW;
            cds.lpData = &bangCommand;
            
            if (SendMessage(hWnd, WM_COPYDATA, 0, (LPARAM)&cds))
            {
                bSuccess = true;
            }
        }
    }
    
    return bSuccess;
}


//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//
// HandleCommandLineBang
// Grabs !bang command and any arguments from a command line and
// sends them to a running litestep.exe instance
//
int HandleCommandLineBang(LPCTSTR pszCommandLine)
{
    ASSERT(pszCommandLine);
    int nReturn = -1;
    
    // Can't just use MAX_BANGCOMMAND + MAX_BANGARGS since
    // there may be lots of whitespace on the command line
    TCHAR szBuffer[MAX_LINE_LENGTH] = { 0 };
    
    if (SUCCEEDED(StringCchCopy(szBuffer, COUNTOF(szBuffer), pszCommandLine)))
    {
        LPCTSTR pszArgs = NULL;
        LPTSTR pszBangEnd = _tcschr(szBuffer, _T(' '));
        
        if (pszBangEnd)
        {
            pszArgs = pszBangEnd + _tcsspn(pszBangEnd, _T(" "));
            
            // Cut off !bang arguments in szBuffer
            *pszBangEnd = _T('\0');
        }
        
        if (SendCommandLineBang(szBuffer, pszArgs))
        {
            nReturn = 0;
        }
    }
    
    return nReturn;
}


//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//
// StartExplorerShell
// Try to start Explorer in shell mode
//
bool StartExplorerShell(DWORD dwWaitTimeout)
{
    bool bStarted = false;
    TCHAR szOldShell[MAX_PATH] = { 0 };
    
    DWORD dwCopied = GetPrivateProfileString(_T("boot"), _T("shell"), NULL,
        szOldShell, COUNTOF(szOldShell), _T("system.ini"));
    
    // If this user account has limited access rights and
    // the shell is in HKLM, GetPrivateProfileString returns 0
    if (dwCopied > 0 && dwCopied < (COUNTOF(szOldShell)-1))
    {
        if (WritePrivateProfileString(
            _T("boot"), _T("shell"), _T("explorer.exe"), _T("system.ini")))
        {
            // We have successfully set Explorer as shell, now launch it...
            SHELLEXECUTEINFO sei = { 0 };
            sei.cbSize = sizeof(sei);
            sei.fMask = SEE_MASK_DOENVSUBST | SEE_MASK_NOCLOSEPROCESS;
            sei.lpVerb = _T("open");
            sei.lpFile = _T("%windir%\\explorer.exe");
            
            if (LSShellExecuteEx(&sei))
            {
                // If we don't wait here, there'll be a race condition:
                // We may reset the 'shell' setting before Explorer reads it.
                if (WaitForInputIdle(sei.hProcess, dwWaitTimeout) == 0)
                {
                    bStarted = true;
                }
                
                CloseHandle(sei.hProcess);
            }
            
            WritePrivateProfileString(
                _T("boot"), _T("shell"), szOldShell, _T("system.ini"));
        }
    }
    
    return bStarted;
}


//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//
// _tWinMain
// Main entry point. Chooses appropriate startup mode.
//
int WINAPI _tWinMain(HINSTANCE hInst, HINSTANCE, LPTSTR lpCmdLine, int)
{
    int nReturn = 0;
    
    // This is safe since lpCmdLine has no leading spaces and is never NULL
    if (lpCmdLine[0] == _T('!'))
    {
        //
        // Mode 1: Command line !bang handling
        //
        nReturn = HandleCommandLineBang(lpCmdLine);
    }
    else
    {
        TCHAR szAltConfigFile[MAX_PATH] = { 0 };
        
        WORD wStartFlags = ParseCommandLine(
            lpCmdLine, szAltConfigFile, COUNTOF(szAltConfigFile));
        
        if (GetSystemMetrics(SM_CLEANBOOT))
        {
            // We're in safe mode. We really want Explorer to run now, in case
            // LiteStep is the reason we're here.
            // But if we can't, make sure we at least ignore startup apps
            // (see docs for GetSystemMetrics(SM_CLEANBOOT))
            wStartFlags |= LSF_RUN_EXPLORER;
            wStartFlags &= ~LSF_RUN_STARTUPAPPS;
        }

        do
        {
            if (wStartFlags & LSF_RUN_EXPLORER)
            {
                //
                // Mode 2: (Try to) start Explorer
                //
                if (StartExplorerShell(EXPLORER_WAIT_TIMEOUT))
                {
                    // Explorer started as shell, no need try LiteStep as well
                    wStartFlags &= ~LSF_RUN_LITESTEP;
                }
                else
                {
                    wStartFlags &= ~LSF_RUN_EXPLORER;
                }
            }
        
            if (wStartFlags & LSF_RUN_LITESTEP)
            {
                HANDLE hMutex = NULL;
            
                if (IsOtherInstanceRunning(&hMutex))
                {
                    //
                    // Mode 3a: Other LiteStep instance already running
                    //
                    RESOURCE_STR(hInst, IDS_LITESTEP_ERROR1,
                        L"A previous instance of LiteStep was detected.\n"
                        L"Are you sure you want to continue?");
                
                    // Can show a MessageBox here since the other instance
                    // should have closed the welcome screen already
                    INT idConfirm =  RESOURCE_MSGBOX_F(
                        L"LiteStep", MB_ICONINFORMATION | MB_YESNO | MB_DEFBUTTON2);
                
                    if (idConfirm == IDNO)
                    {
                        wStartFlags &= ~LSF_RUN_LITESTEP;
                    }
                }
            
                if (wStartFlags & LSF_RUN_LITESTEP)
                {
                    //
                    // Mode 3b: Start the shell!
                    //
                    nReturn = StartLitestep(hInst, wStartFlags, szAltConfigFile);
                }
            
                if (hMutex)
                {
                    CloseHandle(hMutex);
                }

                if (nReturn == LRV_EXPLORER_START)
                {
                    // User wants Explorer as shell anyway
                    wStartFlags |= LSF_RUN_EXPLORER;
                }
            }

        } while (nReturn == LRV_EXPLORER_START && (wStartFlags & LSF_RUN_LITESTEP));
    }
    
    return nReturn;
}
