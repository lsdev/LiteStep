/*
This is a part of the LiteStep Shell Source code.

Copyright (C) 1997-2002 The LiteStep Development Team

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
#ifndef __BANGS_H
#define __BANGS_H

#include "../utility/common.h"


void BangAbout(HWND hCaller, LPCSTR pszArgs);
void BangAlert(HWND hCaller, LPCSTR pszArgs);
void BangCascadeWindows(HWND hCaller, LPCSTR pszArgs);
void BangConfirm(HWND hCaller, LPCSTR pszArgs);
void BangExecute(HWND hCaller, LPCSTR pszArgs);
void BangLogoff(HWND hCaller, LPCSTR pszArgs);
void BangMinimizeWindows(HWND hCaller, LPCSTR pszArgs);
void BangQuit(HWND caller, LPCSTR param);
void BangRecycle (HWND hCaller, LPCSTR pszArgs);
void BangRefresh(HWND hCaller, LPCSTR pszArgs);
void BangReload(HWND hCaller, LPCSTR pszArgs);
void BangReloadModule (HWND hCaller, LPCSTR pszArgs);
void BangRestoreWindows(HWND hCaller, LPCSTR pszArgs);
void BangRun (HWND hCaller, LPCSTR pszArgs);
void BangShutdown(HWND hCaller, LPCSTR pszArgs);
void BangSwitchUser(HWND hCaller, LPCSTR pszArgs);
void BangTileWindowsH(HWND hCaller, LPCSTR pszArgs);
void BangTileWindowsV(HWND hCaller, LPCSTR pszArgs);
void BangUnloadModule (HWND hCaller, LPCSTR pszArgs);
void BangHideModules (HWND hCaller, LPCSTR pszArgs);
void BangShowModules (HWND hCaller, LPCSTR pszArgs);
void BangToggleModules (HWND hCaller, LPCSTR pszArgs);

#endif // __BANGS_H

