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
#ifndef __HookManager_H
#define __HookManager_H

#include "../utility/common.h"
#include "../hook/hook.h"

#include <map> // STL
#include <set> // STL


typedef std::set<HookCallback*> sMsgHookList;
typedef std::map<UINT, sMsgHookList*> msg2hwnd; // Maps a message to a list of HWNDs

BOOL startHookManager(HINSTANCE dllInst);
void stopHookManager();
UINT RegisterHookMessage(HWND hwnd, UINT msg, HookCallback* pCallback);
UINT UnregisterHookMessage(HWND hwnd, UINT msg, HookCallback* pCallback);
bool createHookThread();
DWORD HookMgrMain(LPVOID lpv);
bool InstallMsgFilter(bool install);
LRESULT CALLBACK HookMgrWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

#endif // __HookManager_H
