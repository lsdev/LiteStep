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
#ifndef __HOOK_H
#define __HOOK_H

#ifdef _DEBUG
#define _LSDEBUG
#endif

#include "../utility/common.h"

#define HOOKMGRWINDOWNAME  "HookMgrWindow"
#define HOOKMGRWINDOWCLASS "HookMgrClass"

#ifdef HOOK_DLL
	#define HOOK_EXPORT __declspec(dllexport)
#else
	#define HOOK_EXPORT __declspec(dllimport)
#endif

#ifdef __cplusplus
extern "C"
{
#endif

	HOOK_EXPORT void setMsgHook(HHOOK);
	HOOK_EXPORT HHOOK getMsgHook();
	HOOK_EXPORT void setCallWndHook(HHOOK);
	HOOK_EXPORT HHOOK getCallWndHook();

#ifdef __cplusplus
}
#endif

LRESULT CALLBACK GetMsgProc (int nCode, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK CallWndProc (int nCode, WPARAM wParam, LPARAM lParam);

typedef void (HookCallback)(HWND, UINT, WPARAM, LPARAM);

#endif // __HOOK_H
