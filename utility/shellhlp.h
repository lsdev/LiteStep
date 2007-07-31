/*
This is a part of the LiteStep Shell Source code.

Copyright (C) 1997-2003 The LiteStep Development Team

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
*/ 
/****************************************************************************
****************************************************************************/
#ifndef __INCLUDE_SHELLHLP_H
#define __INCLUDE_SHELLHLP_H

#include "common.h"
#include <shlobj.h>

bool GetShellFolderPath(int nFolder, LPTSTR ptzPath, size_t cchPath);
bool GetSystemString(DWORD dwCode, LPTSTR ptzBuffer, size_t cchBuffer);
HRESULT PathAddBackslashEx(LPTSTR ptzPath, size_t cchPath);
HRESULT CLSIDToString(REFCLSID rclsid, LPTSTR ptzBuffer, size_t cchBuffer);
bool LSGetModuleFileName(HINSTANCE hInst, LPTSTR pszBuffer, DWORD cchBuffer);
HRESULT TryAllowSetForegroundWindow(HWND hWnd);

#endif // __INCLUDE_SHELLHLP_H
