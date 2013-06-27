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
HRESULT CLSIDToString(REFCLSID rclsid, LPTSTR ptzBuffer, size_t cchBuffer);
bool LSGetModuleFileName(HINSTANCE hInst, LPTSTR pszBuffer, DWORD cchBuffer);
HRESULT TryAllowSetForegroundWindow(HWND hWnd);
bool IsVistaOrAbove();
void LSShutdownDialog(HWND hWnd);
BOOL LSPlaySystemSound(LPCTSTR pszSoundAlias);
HANDLE LSCreateThread(LPCSTR pszName, LPTHREAD_START_ROUTINE fnStartAddres,
                      LPVOID lpParameter, LPDWORD pdwThreadId);


//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//
// WOW64 Wrappers
//
BOOL LSDisableWow64FsRedirection(PVOID* ppvOldValue);
BOOL LSRevertWow64FsRedirection(PVOID pvOldValue);

BOOL LSShellExecuteEx(LPSHELLEXECUTEINFO lpExecInfo);
HINSTANCE LSShellExecute(HWND hwnd, LPCTSTR lpOperation, LPCTSTR lpFile,
                         LPCTSTR lpParameters, LPCTSTR lpDirectory, INT nShow);


//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//
// Actication contexts
//
HANDLE LSActivateActCtxForDll(LPCTSTR pszDll, PULONG_PTR pulCookie);
HANDLE LSActivateActCtxForClsid(REFCLSID rclsid, PULONG_PTR pulCookie);
void   LSDeactivateActCtx(HANDLE hActCtx, ULONG_PTR* pulCookie);


//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//
// Windows Versions
//
UINT GetWindowsVersion();

#define WINVER_UNKNOWN  (UINT)-1

// Win9x
#define WINVER_WIN95    1
#define WINVER_WIN98    2
#define WINVER_WINME    3

// Windows NT Workstation
#define WINVER_WINNT4   4
#define WINVER_WIN2000  5
#define WINVER_WINXP    6
#define WINVER_VISTA    7
#define WINVER_WIN7     8
#define WINVER_WIN8     9

// Windows NT Server
#define WINVER_WIN2003 10
#define WINVER_WHS     11
#define WINVER_WIN2008 12
#define WINVER_WIN2012 13


//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//
// HrGetLastError
// Convert GetLastError to HRESULT without an ugly macro
//
inline HRESULT HrGetLastError()
{
    return HRESULT_FROM_WIN32(GetLastError());
}

#endif // SHELLHLP_H
