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
#include "../litestep/buildoptions.h"

// _WINDOWS_ is used by MSVC, _WINDOWS_H is the MinGW variant
#if defined (_WINDOWS_) || defined(_WINDOWS_H)
#error Do not include windows.h before this
#endif

#define _WIN32_IE 0x600
#define _WIN32_WINNT 0x0600

#define WIN32_LEAN_AND_MEAN
#define STRICT
#define NOCRYPT
#include <windows.h>

#include "hook.h"

#if defined(__GNUC__)

static HHOOK g_hHookMessage __attribute__((section("SHAREDATA"), shared)) = 0;
static HHOOK g_hHookCallWnd __attribute__((section("SHAREDATA"), shared)) = 0;
static HWND g_hwndNotify __attribute__((section("SHAREDATA"), shared)) = NULL;

#else

#pragma comment(linker, "/SECTION:SHAREDATA,RWS")
#pragma data_seg("SHAREDATA")
static HHOOK g_hHookMessage = 0;
static HHOOK g_hHookCallWnd = 0;
static HWND g_hwndNotify = NULL;
#pragma data_seg()

#endif // defined(__GNUC__)

static HINSTANCE g_hinstDll;
static bool g_bFilter;


static void processGetMsg(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	if (IsWindow(g_hwndNotify))
	{
		MSG msgd;
		COPYDATASTRUCT cds = {HOOK_CDS_MSG, sizeof(MSG), &msgd};

		msgd.hwnd = hwnd;
		msgd.message = msg;
		msgd.lParam = lParam;
		msgd.wParam = wParam;

		SendMessage(g_hwndNotify, WM_COPYDATA, (WPARAM)msg, (LPARAM)&cds);
	}
}


static LRESULT CALLBACK getMsgProc(int nCode, WPARAM wParam, LPARAM lParam)
{
	if (nCode >= 0 && !g_bFilter)
	{
		PMSG pmsg = (PMSG)lParam;

		if (pmsg && pmsg->hwnd)
		{
			UINT msg = pmsg->message;

			if (msg < WM_USER && msg > WM_NULL)
			{
				switch (msg)
				{
				case WM_TIMER:
					break;
				case WM_PAINT:
					break;
				default:
					processGetMsg(pmsg->hwnd, pmsg->message, pmsg->wParam, pmsg->lParam);
					break;
				}
			}
		}
	}

	return (CallNextHookEx(g_hHookMessage, nCode, wParam, lParam));
}


static LRESULT CALLBACK callWndProc(int nCode, WPARAM wParam, LPARAM lParam)
{
	if (nCode >= 0 && !g_bFilter)
	{
		PCWPSTRUCT pmsg = (PCWPSTRUCT)lParam;

		if (pmsg && pmsg->hwnd)
		{
			UINT msg = pmsg->message;

			if (msg < WM_USER && msg > WM_NULL)
			{
				switch (msg)
				{
				case WM_ERASEBKGND:
					break;
				case WM_NCPAINT:
					break;
				case WM_COPYDATA:
					break;
				default:
					processGetMsg(pmsg->hwnd, pmsg->message, pmsg->wParam, pmsg->lParam);
					break;
				}
			}
		}
	}

	return (CallNextHookEx(g_hHookCallWnd, nCode, wParam, lParam));
}


extern "C" __declspec(dllexport) BOOL __cdecl SetHooks(HWND hwndNotify)
{
	if (NULL != hwndNotify)
	{
		g_hwndNotify = hwndNotify;

		if (NULL == g_hHookCallWnd)
		{
			g_hHookCallWnd = SetWindowsHookEx(WH_CALLWNDPROC, callWndProc, g_hinstDll, 0);
		}

		if (NULL == g_hHookMessage)
		{
			g_hHookMessage = SetWindowsHookEx(WH_GETMESSAGE, getMsgProc, g_hinstDll, 0);
		}

		if (NULL == g_hHookCallWnd || NULL == g_hHookMessage)
		{
			SetHooks(NULL);
		}
	}
	else
	{
		if (NULL != g_hHookMessage)
		{
			UnhookWindowsHookEx(g_hHookMessage);
			g_hHookMessage = NULL;
		}

		if (NULL != g_hHookCallWnd)
		{
			UnhookWindowsHookEx(g_hHookCallWnd);
			g_hHookCallWnd = NULL;
		}

		g_hwndNotify = NULL;
	}

	return NULL != g_hwndNotify;
}


extern "C" BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved)
{
	UNREFERENCED_PARAMETER(lpvReserved);

	if (fdwReason == DLL_PROCESS_ATTACH)
	{
		// We don't need thread notifications for what we're doing.  Thus, get
		// rid of them, thereby eliminating some of the overhead of this DLL
		DisableThreadLibraryCalls(hinstDLL);

		g_hinstDll = hinstDLL;

		HKEY hKey;

		// If you are debugging this project, be sure to include your
		// debugger process (eg. MSDEV.EXE) in the exclusion list!
		// Otherwise, you'll be asking for a system wide lockup.
		if (RegOpenKeyEx(HKEY_CURRENT_USER, "Software\\LiteStep\\HookExclude",
			0, KEY_READ, &hKey) == ERROR_SUCCESS)
		{
			char szExeName[MAX_PATH+1] = { 0 };
			char *szFileName = szExeName;
			char *tcp;

			GetModuleFileName(NULL, szExeName, sizeof(szExeName)-1);

			for (tcp = szExeName; *tcp; tcp++)
			{
				if (*tcp == '/' || *tcp == '\\')
					szFileName = tcp + 1;
			}

			if (RegQueryValueEx(hKey, szFileName, NULL, NULL, NULL, NULL) == ERROR_SUCCESS)
			{
				g_bFilter = true;
			}
			else
			{
				g_bFilter = false;
			}

			RegCloseKey(hKey);
		}
	}

	return TRUE;
}


extern "C" BOOL WINAPI _DllMainCRTStartup(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved)
{
	return DllMain(hinstDLL, fdwReason, lpvReserved);
}
