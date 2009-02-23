/*
This is a part of the LiteStep Shell Source code.

Copyright (C) 1997-2005 The LiteStep Development Team

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
#include "HookManager.h"
#include "../utility/criticalsection.h"
#include "../utility/macros.h"
#include "resource.h"

#include <map>
#include <set>

typedef std::set<HOOKCALLBACKPROC> HookCallbackSet;
typedef std::map<UINT, HookCallbackSet*> Msg2HookMap; // Maps a message to a set of hook callbacks

static Msg2HookMap g_m2hmap;
static CriticalSection * g_pm2hmapCS;

static HANDLE g_hHookMgrThread;
static HWND g_hwndHookMgr;
static HINSTANCE g_hInstance;
static HMODULE g_hmodHook;

static SETHOOKSPROC SetHooks;

static bool loadHookModule();
static bool createHookThread();

static DWORD WINAPI hookMgrMain(LPVOID lpv);
static LRESULT CALLBACK hookMgrWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);


bool StartHookManager(HINSTANCE hInstance)
{
	ASSERT(hInstance);

	if (NULL == g_hInstance)
	{
		g_pm2hmapCS = new CriticalSection();

		g_hInstance = hInstance;

		if(!loadHookModule())
		{
			RESOURCE_MSGBOX(g_hInstance, IDS_HOOKMGR_LOADHOOK_ERROR,
				"Could not load hook.dll", HOOKMGRWINDOWNAME);

			g_hInstance = NULL;

			delete g_pm2hmapCS;
			g_pm2hmapCS = NULL;

			return false;
		}

		if (!createHookThread())
		{
			RESOURCE_MSGBOX(g_hInstance, IDS_HOOKMGR_CREATETHREAD_ERROR,
				"Could not create HookManager Thread", HOOKMGRWINDOWNAME);

			FreeLibrary(g_hmodHook);
			g_hmodHook = NULL;

			g_hInstance = NULL;

			delete g_pm2hmapCS;
			g_pm2hmapCS = NULL;

			return false;
		}
	}

	return g_hInstance == hInstance;
}


void StopHookManager()
{
	if (NULL != g_hInstance)
	{
		SendMessage(g_hwndHookMgr, WM_CLOSE, 0, 0);

		WaitForSingleObject(g_hHookMgrThread, INFINITE);
		CloseHandle(g_hHookMgrThread);
		g_hHookMgrThread = NULL;

		UnregisterClass(HOOKMGRWINDOWCLASS, g_hInstance);

		FreeLibrary(g_hmodHook);
		g_hmodHook = NULL;

		g_hInstance = NULL;

		delete g_pm2hmapCS;
		g_pm2hmapCS = NULL;
	}
}


static bool createHookThread()
{
	//
	// Register a class for the hook stuff to forward its messages to.
	//
	WNDCLASS wc;
	wc.hCursor = NULL;  // this window never shown, so no
	wc.hIcon = NULL;    // cursor or icon are necessary
	wc.lpszMenuName = NULL;
	wc.lpszClassName = HOOKMGRWINDOWCLASS;
	wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wc.hInstance = g_hInstance;
	wc.style = 0;
	wc.lpfnWndProc = hookMgrWndProc;
	wc.cbWndExtra = sizeof(HWND) + sizeof(HWND);
	wc.cbClsExtra = 0;

	if (!RegisterClass(&wc))
	{
		return false;
	}

	//
	// Now create a thread to handle the new queue
	//
	HANDLE hReadyEvent = CreateEvent(NULL, TRUE, FALSE, NULL);

	DWORD dwID;
	g_hHookMgrThread = CreateThread(NULL, 0, hookMgrMain, hReadyEvent, 0, &dwID);

	if (!g_hHookMgrThread)
	{
		UnregisterClass(HOOKMGRWINDOWCLASS, g_hInstance);
		return false;
	}

	HANDLE hWait[2] = { g_hHookMgrThread, hReadyEvent };

	DWORD dwEvent = WaitForMultipleObjects(2, hWait, FALSE, INFINITE);

	switch (dwEvent)
	{
	case WAIT_OBJECT_0 + 0: // Premature ending of thread
		{
			CloseHandle(g_hHookMgrThread);
			g_hHookMgrThread = NULL;
			return false;
		}
		break;
	default: // should not happen, nothing sensible to do if so
		ASSERT(FALSE);
		// FALL THROUGH
	case WAIT_OBJECT_0 + 1: // Ready Event signaled
		{
			CloseHandle(hReadyEvent);
		}
		break;
	}

	return true;
}

static bool loadHookModule()
{
	g_hmodHook = LoadLibrary("hook");

	if (NULL != g_hmodHook)
	{
		SetHooks = (SETHOOKSPROC)GetProcAddress(g_hmodHook, "SetHooks");

		if(NULL == SetHooks)
		{
			FreeLibrary(g_hmodHook);
			g_hmodHook = NULL;
		}
	}

	return NULL != g_hmodHook;
}


static DWORD WINAPI hookMgrMain(LPVOID lpParameter)
{
	g_hwndHookMgr = CreateWindow(
		HOOKMGRWINDOWCLASS, HOOKMGRWINDOWNAME,
		WS_OVERLAPPEDWINDOW,
		0, 0, 0, 0,
		NULL, NULL,
		g_hInstance, NULL);

	if (!g_hwndHookMgr)
	{

		RESOURCE_MSGBOX(g_hInstance, IDS_LITESTEP_CREATEWINDOW_ERROR,
			"Unable to create window.", HOOKMGRWINDOWNAME);
		return 1;
	}

	SetEvent((HANDLE)lpParameter);

	MSG msg;
	while (GetMessage(&msg, g_hwndHookMgr, 0, 0) > 0)
	{
		DispatchMessage(&msg);
	}

	return 0;
}


UINT RegisterHookMessage(UINT uMsg, HOOKCALLBACKPROC pfnCallback)
{
	if (NULL != g_hInstance && WM_NULL < uMsg && NULL != pfnCallback)
	{
		Lock lock(*g_pm2hmapCS);

		if (g_m2hmap.empty())
		{
			SetHooks(g_hwndHookMgr);
		}

		HookCallbackSet * phcs;
		Msg2HookMap::iterator m2hit = g_m2hmap.find(uMsg);

		if (m2hit == g_m2hmap.end())
		{
			phcs = new HookCallbackSet;
			g_m2hmap.insert(Msg2HookMap::value_type(uMsg, phcs));
		}
		else
		{
			phcs = m2hit->second;
		}

		phcs->insert(pfnCallback);

		return TRUE;
	}

	return FALSE;
}


UINT UnregisterHookMessage(UINT uMsg, HOOKCALLBACKPROC pfnCallback)
{
	Lock lock(*g_pm2hmapCS);

	if (NULL != g_hInstance && WM_NULL < uMsg && NULL != pfnCallback)
	{
		Msg2HookMap::iterator m2hit = g_m2hmap.find(uMsg);

		if (m2hit != g_m2hmap.end())
		{
			HookCallbackSet * phcs = m2hit->second;
			HookCallbackSet::iterator hcit = phcs->find(pfnCallback);

			if (hcit != phcs->end())
			{
				phcs->erase(hcit);
			}

			if (phcs->empty())
			{
				delete phcs;
				g_m2hmap.erase(m2hit);
			}
		}

		if (g_m2hmap.empty())
		{
			SetHooks(NULL);
		}
	}

	return g_m2hmap.size();
}


static LRESULT CALLBACK hookMgrWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
	case WM_COPYDATA:
		{
			PCOPYDATASTRUCT pcds = (PCOPYDATASTRUCT)lParam;

			if (pcds->dwData != HOOK_CDS_MSG || pcds->cbData != sizeof(MSG))
			{
				break;
			}

			Lock lock(*g_pm2hmapCS);

			PMSG pmsg = (PMSG)(pcds->lpData);
			Msg2HookMap::iterator m2hit = g_m2hmap.find(pmsg->message);

			if (m2hit != g_m2hmap.end())
			{
				HookCallbackSet * phcs = m2hit->second;

				for (HookCallbackSet::reverse_iterator hcrit = phcs->rbegin(); hcrit != phcs->rend(); hcrit++)
				{
					(*hcrit)(pmsg->hwnd, pmsg->message, pmsg->wParam, pmsg->lParam);
				}
			}
			return TRUE;
		}
		break;

	case WM_DESTROY:
		{
			Lock lock(*g_pm2hmapCS);

			for (Msg2HookMap::iterator m2hit = g_m2hmap.begin(); m2hit != g_m2hmap.end(); m2hit++)
			{
				delete m2hit->second;
			}
			g_m2hmap.clear();
			SetHooks(NULL);

			g_hwndHookMgr = NULL;

			PostQuitMessage(0);
		}
		break;

	default:
		{
			return DefWindowProc(hwnd, msg, wParam, lParam);
		}
		break;
	}

	return 0;
}
