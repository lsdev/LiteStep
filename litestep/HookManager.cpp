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
/****************************************************************************
****************************************************************************/
#include "HookManager.h"
#include "../utility/macros.h"
#include "resource.h"

msg2hwnd m2hmap;
int numMessages;

HHOOK hMsgHook;
HHOOK hCallWndHook;

HANDLE hHookMgrThread;
HWND hwndHookMgr;
HINSTANCE hInst;
bool processHooks;
HINSTANCE hmodHook;

MSG msgd;
COPYDATASTRUCT gcds = { 0, sizeof(MSG), &msgd };

BOOL startHookManager(HINSTANCE dllInst)
{
	hMsgHook = NULL;
	hHookMgrThread = NULL;
	hwndHookMgr = NULL;
	hInst = dllInst;

	hmodHook = LoadLibrary("hook");
	if (hmodHook)
	{
		processHooks = true;
		if (!createHookThread())
		{
			RESOURCE_MSGBOX(hInst, IDS_HOOKMGR_CREATETHREAD_ERROR,
			                "Could not create Hook Manager Thread", HOOKMGRWINDOWNAME)

			processHooks = false;
		}
	}
	else
	{
		RESOURCE_MSGBOX(hInst, IDS_HOOKMGR_LOADHOOK_ERROR,
		                "Could not load hook.dll", HOOKMGRWINDOWNAME)

		processHooks = false;
	}

	return processHooks;
}


void stopHookManager()
{
	SendMessage(hwndHookMgr, WM_CLOSE, 0, 0);
	WaitForSingleObject(hHookMgrThread, INFINITE);
	processHooks = false;
	UnregisterClass(HOOKMGRWINDOWCLASS, hInst);
	FreeLibrary(hmodHook);
}


bool createHookThread()
{
	WNDCLASS wc;
	DWORD Id;

	//
	// Register a class for the hook stuff to forward its messages to.
	//
	wc.hCursor = NULL;    // this window never shown, so no
	wc.hIcon = NULL;    // cursor or icon are necessary
	wc.lpszMenuName = NULL;
	wc.lpszClassName = HOOKMGRWINDOWCLASS;
	wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wc.hInstance = hInst;
	wc.style = 0;
	wc.lpfnWndProc = HookMgrWndProc;
	wc.cbWndExtra = sizeof(HWND) + sizeof(HWND);
	wc.cbClsExtra = 0;

	if (!RegisterClass(&wc))
	{
		return FALSE;
	}
	//
	// Now create another thread to handle the new queue
	//
	if (!(hHookMgrThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)HookMgrMain,
	                                    0L, STANDARD_RIGHTS_REQUIRED, &Id)))
	{
		return FALSE;
	}
	return TRUE;

}


DWORD HookMgrMain(LPVOID lpv)
{
	MSG msg;

	hwndHookMgr = CreateWindow(HOOKMGRWINDOWCLASS, HOOKMGRWINDOWNAME,
	                           WS_OVERLAPPEDWINDOW,
	                           0, 0, 0, 0,
	                           NULL, NULL,
	                           hInst, NULL);

	if (!hwndHookMgr)
	{

		RESOURCE_MSGBOX(hInst, IDS_LITESTEP_CREATEWINDOW_ERROR,
		                "Unable to create window.", HOOKMGRWINDOWNAME)

		ExitThread(0);
	}

	while (IsWindow(hwndHookMgr) && GetMessage(&msg, hwndHookMgr, 0, 0))
	{
		if (processHooks)
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	hwndHookMgr = NULL;

	return 0;

}


bool InstallMsgFilter(bool install)
{
	TRACE("Installing Hooks...");
	if (install && !hMsgHook && !hCallWndHook)
	{

		setCallWndHook(SetWindowsHookEx(WH_CALLWNDPROC,
		                                (HOOKPROC)GetProcAddress(hmodHook, "CallWndProc"), hmodHook, 0));
		TRACE_LASTERR("hook CallWndProc");
		hCallWndHook = getCallWndHook();

		setMsgHook(SetWindowsHookEx(WH_GETMESSAGE,
		                            (HOOKPROC)GetProcAddress(hmodHook, "GetMsgProc"), hmodHook, 0));
		TRACE_LASTERR("hook GetMsgProc");
		hMsgHook = getMsgHook();

		if (hMsgHook && hCallWndHook)
		{
			TRACE("Hooks Installed!");
			return true;
		}
	}
	else if (!install && hMsgHook && hCallWndHook)
	{
		setMsgHook(NULL);
		UnhookWindowsHookEx(hMsgHook);
		hMsgHook = NULL;

		setCallWndHook(NULL);
		UnhookWindowsHookEx(hCallWndHook);
		hCallWndHook = NULL;
	}
	TRACE("Hooks Not Installed!");
	return false;
}


UINT RegisterHookMessage(HWND hwnd, UINT msg, HookCallback* pCallback)
{
	sMsgHookList * psHwnds;
	sMsgHookList::reverse_iterator lrit;
	msg2hwnd::iterator m2hit;

	if ((hwnd != NULL) && (msg > 0) && (pCallback != NULL))
	{
		if (numMessages == 0)
		{
			if (InstallMsgFilter(true))
			{
				psHwnds = new sMsgHookList;
				psHwnds->insert(pCallback);
				m2hmap.insert(msg2hwnd::value_type(msg, psHwnds));
				numMessages++;
			}
			else
			{
				return FALSE;
			}
		}
		else
		{
			m2hit = m2hmap.find(msg);
			if (m2hit == m2hmap.end())
			{
				psHwnds = new sMsgHookList;
				psHwnds->insert(pCallback);
				m2hmap.insert(msg2hwnd::value_type(msg, psHwnds));
			}
			else
			{
				psHwnds = (*m2hit).second;
				psHwnds->insert(pCallback);
			}
			numMessages++;
		}

		return TRUE;
	}

	return FALSE;
}


UINT UnregisterHookMessage(HWND hwnd, UINT msg, HookCallback* pCallback)
{
	sMsgHookList * psHwnds;
	sMsgHookList::reverse_iterator lrit;
	msg2hwnd::iterator m2hit;

	if ((hwnd != NULL) && (msg > 0) && (pCallback != NULL))
	{
		if (numMessages)
		{
			m2hit = m2hmap.find(msg);
			if (m2hit != m2hmap.end())
			{
				sMsgHookList::iterator lit;
				psHwnds = (*m2hit).second;
				lit = psHwnds->find(pCallback);
				if (lit != psHwnds->end())
				{
					psHwnds->erase(lit);
					if (psHwnds->empty())
					{
						delete (*m2hit).second;
						m2hmap.erase(m2hit);
					}
					numMessages--;
					if (!numMessages)
					{
						InstallMsgFilter(false);
					}
				}
			}
		}
	}

	return numMessages;
}


LRESULT CALLBACK HookMgrWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	sMsgHookList * psHwnds;
	sMsgHookList::reverse_iterator lrit;
	msg2hwnd::iterator m2hit;

	switch (msg)
	{
		case WM_COPYDATA:
		{
			if (numMessages == 0)
				break;
			if (((PCOPYDATASTRUCT)lParam)->cbData != sizeof(MSG))
				break;
			msgd.message = ((PMSG)((PCOPYDATASTRUCT)lParam)->lpData)->message;
			msgd.hwnd = ((PMSG)((PCOPYDATASTRUCT)lParam)->lpData)->hwnd;
			msgd.wParam = ((PMSG)((PCOPYDATASTRUCT)lParam)->lpData)->wParam;
			msgd.lParam = ((PMSG)((PCOPYDATASTRUCT)lParam)->lpData)->lParam;
			m2hit = m2hmap.find(msgd.message);
			if (m2hit != m2hmap.end())
			{
				TRACE("WM_COPYDATA message found = %u", msgd.message);
				psHwnds = (*m2hit).second;
				for (lrit = psHwnds->rbegin(); lrit != psHwnds->rend(); lrit++)
				{
					(*lrit)(msgd.hwnd, msgd.message, msgd.wParam, msgd.lParam);
				}
			}
		}
		return TRUE;

		case WM_DESTROY:
		{
			InstallMsgFilter(false);
			PostQuitMessage(0);
		}
		return 0;

		case WM_NCDESTROY:
		{
			processHooks = FALSE;
		}
		break;
	}
	return DefWindowProc(hwnd, msg, wParam, lParam);
}
