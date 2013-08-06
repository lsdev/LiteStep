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
#if !defined(SHELLHLP_H)
#define SHELLHLP_H

#include "core.hpp"
#include <ObjBase.h>


//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//
// Shell Helpers
//
// These are mostly small wrappers that work around inconveniences or even bugs
// in some Windows APIs
//
#define LS_CSIDL_QUICKLAUNCH   0x00FF
bool GetShellFolderPath(int nFolder, LPTSTR ptzPath, size_t cchPath);

bool GetSystemString(DWORD dwCode, LPTSTR ptzBuffer, DWORD cchBuffer);
HRESULT PathAddBackslashEx(LPTSTR ptzPath, size_t cchPath);
HRESULT PathAddBackslashExA(LPSTR pszPath, size_t cchPath);
HRESULT CLSIDToString(REFCLSID rclsid, LPTSTR ptzBuffer, size_t cchBuffer);
bool LSGetModuleFileName(HINSTANCE hInst, LPTSTR pszBuffer, DWORD cchBuffer);
DWORD LSGetModuleFileNameEx(HANDLE hProcess, HMODULE hModule,
    LPTSTR pszBuffer, DWORD cchBuffer);
DWORD LSGetProcessImageFileName(HANDLE hProcess, LPTSTR pszBuffer, DWORD cchBuffer);
HRESULT TryAllowSetForegroundWindow(HWND hWnd);
bool IsVistaOrAbove();
void LSShutdownDialog(HWND hWnd);
BOOL LSPlaySystemSound(LPCWSTR pwzSoundAlias);
HANDLE LSCreateThread(LPCSTR pszName, LPTHREAD_START_ROUTINE fnStartAddres,
                      LPVOID lpParameter, LPDWORD pdwThreadId);
BOOL LSGUIDFromString(LPCTSTR ptzGUIDString, LPGUID pGUID);


//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//
// WOW64 Wrappers
//
BOOL LSDisableWow64FsRedirection(PVOID* ppvOldValue);
BOOL LSRevertWow64FsRedirection(PVOID pvOldValue);

BOOL LSShellExecuteEx(LPSHELLEXECUTEINFOW lpExecInfo);
HINSTANCE LSShellExecute(HWND hwnd, LPCWSTR lpOperation, LPCWSTR lpFile,
                         LPCWSTR lpParameters, LPCWSTR lpDirectory, INT nShow);


//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//
// Actication contexts
//
HANDLE LSActivateActCtxForDll(LPCTSTR pszDll, PULONG_PTR pulCookie);
HANDLE LSActivateActCtxForClsid(REFCLSID rclsid, PULONG_PTR pulCookie);
void   LSDeactivateActCtx(HANDLE hActCtx, ULONG_PTR* pulCookie);


//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//
// LiteStep Return Values (LRV)
//
#define LRV_NO_APP_PATH        -1
#define LRV_OK                  0 // is also S_OK from WinError
#define LRV_SHUTDOWN_DLG        1
#define LRV_NO_STEP             2
#define LRV_LSAPI_FAIL          3
#define LRV_EXPLORER_START      4


//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//
// Windows Versions
//
UINT GetWindowsVersion();

#define WINVER_UNKNOWN  (UINT)-1

// Win9x
#define WINVER_WIN95        1
#define WINVER_WIN98        2
#define WINVER_WINME        3

// Windows NT Workstation
#define WINVER_WINNT4       4
#define WINVER_WIN2000      5
#define WINVER_WINXP        6
#define WINVER_VISTA        7
#define WINVER_WIN7         8
#define WINVER_WIN8         9
#define WINVER_WIN81        10

// Windows NT Server
#define WINVER_WIN2003      11
#define WINVER_WHS          12
#define WINVER_WIN2008      13
#define WINVER_WIN2008R2    14
#define WINVER_WIN2012      15
#define WINVER_WIN2012R2    16


//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//
// HrGetLastError
// Convert GetLastError to HRESULT without an ugly macro
//
inline HRESULT HrGetLastError()
{
    return HRESULT_FROM_WIN32(GetLastError());
}

HRESULT DescriptionFromHR(HRESULT hr, LPWSTR buf, size_t cchBuf);

#endif // SHELLHLP_H
