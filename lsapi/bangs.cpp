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
#include "../utility/common.h"
#include "bangs.h"
#include "lsapi.h"
#include "../utility/safestr.h" // Always include last in cpp file

extern char szAppPath[MAX_PATH + 1];
extern char szRcPath[MAX_PATH + 1];

void setupBangs()
{
	AddBangCommand("!ABOUT", BangAbout);
	AddBangCommand("!ALERT", BangAlert);
	AddBangCommand("!CASCADEWINDOWS", BangCascadeWindows);
	AddBangCommand("!CONFIRM", BangConfirm);
	AddBangCommand("!EXECUTE", BangExecute);
	AddBangCommand("!GATHER", BangGather);
	AddBangCommand("!LOGOFF", BangLogoff);
	AddBangCommand("!MINIMIZEWINDOWS", BangMinimizeWindows);
	AddBangCommand("!QUIT", BangQuit);
	AddBangCommand("!POPUP", BangPopup);
	AddBangCommand("!RECYCLE", BangRecycle);
	AddBangCommand("!REFRESH", BangRefresh);
	AddBangCommand("!RELOAD", BangReload);
	AddBangCommand("!RELOADMODULE", BangReloadModule);
	AddBangCommand("!RESTOREWINDOWS", BangRestoreWindows);
	AddBangCommand("!RUN", BangRun);
	AddBangCommand("!SHUTDOWN", BangShutdown);
	AddBangCommand("!SWITCHUSER", BangSwitchUser);
	AddBangCommand("!TILEWINDOWSH", BangTileWindowsH);
	AddBangCommand("!TILEWINDOWSV", BangTileWindowsV);
	AddBangCommand("!TOGGLEWHARF", BangToggleWharf);
	AddBangCommand("!UNLOADMODULE", BangUnloadModule);
	AddBangCommand("!HIDEMODULES", BangHideModules);
	AddBangCommand("!SHOWMODULES", BangShowModules);
}


ULONG WINAPI AboutBoxThread(void *);


//
// BangAbout(HWND hCaller, LPCSTR pszArgs)
//
void BangAbout(HWND /* hCaller */, LPCSTR /* pszArgs */)
{
	/*DWORD dwThread;

	HANDLE hThread = CreateThread(NULL, 0, AboutBoxThread, NULL, 0, &dwThread);

	if (hThread)
	{
		CloseHandle(hThread);
	}*/
}


//
// BangAlert(HWND hCaller, LPCSTR pszArgs)
//
void BangAlert(HWND hCaller, LPCSTR pszArgs)
{
	SAFE_CHAR(szMessage, MAX_PATH);
	SAFE_CHAR(szTitle, MAX_PATH);
	LPSTR aszTokens[] = { szMessage, szTitle };

	int nTokenCount = LCTokenize(pszArgs, aszTokens, 2, 0);

	if (nTokenCount >= 1)
	{
		if (nTokenCount == 1)
		{
			StringCchCopy(szTitle, MAX_PATH, "Litestep");
		}

		MessageBox(hCaller, szMessage, szTitle, MB_OK | MB_SETFOREGROUND);
	}
}


//
// BangCascadeWindows(HWND hCaller, LPCSTR pszArgs)
//
void BangCascadeWindows(HWND /* hCaller */, LPCSTR /* pszArgs */)
{
	CascadeWindows(NULL, MDITILE_SKIPDISABLED, NULL, 0, NULL);
}


//
// BangConfirm(HWND hCaller, LPCSTR pszArgs)
//
void BangConfirm(HWND hCaller, LPCSTR pszArgs)
{
	if (IsValidStringPtr(pszArgs))
	{
		SAFE_CHAR(szFirst, MAX_PATH);
		SAFE_CHAR(szSecond, MAX_PATH);
		SAFE_CHAR(szThird, MAX_PATH);
		SAFE_CHAR(szFourth, MAX_PATH);
		LPSTR aszTokens[] = { szFirst, szSecond, szThird, szFourth };

		int nTokenCount = CommandTokenize(pszArgs, aszTokens, 4, 0);

		if (nTokenCount >= 3)
		{
			if (nTokenCount == 3)
			{
				StringCchCopy(szFourth, MAX_PATH, "Litestep");
			}

			if (MessageBox(hCaller, szFirst, (nTokenCount == 3) ? szFourth : szSecond, MB_YESNO | MB_SETFOREGROUND) == IDYES)
			{
				LSExecute(hCaller, (nTokenCount == 3) ? szSecond : szThird, SW_SHOWNORMAL);
			}
			else
			{
				LSExecute(hCaller, (nTokenCount == 3) ? szThird : szFourth, SW_SHOWNORMAL);
			}
		}
	}
}


//
// BangExecute(HWND hCaller, LPCSTR pszArgs)
//
void BangExecute(HWND hCaller, LPCSTR pszArgs)
{
	if (IsValidStringPtr(pszArgs))
	{
		LPCSTR pszNextToken = pszArgs;
		SAFE_CHAR(szCommand, MAX_LINE_LENGTH);

		while (GetToken(pszNextToken, szCommand, &pszNextToken, true))
		{
			LSExecute(hCaller, szCommand, SW_SHOWDEFAULT);
		}
	}
}


//
// BangGather(HWND hCaller, LPCSTR pszArgs)
//
void BangGather(HWND /* hCaller */, LPCSTR /* pszArgs */)
{
	HWND hLiteStep = GetLitestepWnd();

	if (hLiteStep)
	{
		SendMessage(hLiteStep, LM_BRINGTOFRONT, 1, 0);
	}
}


//
// BangLogoff(HWND hCaller, LPCSTR pszArgs)
//
void BangLogoff(HWND /* hCaller */, LPCSTR /* pszArgs */)
{
	HWND hLiteStep = GetLitestepWnd();

	if (hLiteStep)
	{
		SendMessage(hLiteStep, LM_RECYCLE, LR_LOGOFF, 0);
	}
}


//
// CALLBACK WindowsEnumProc(HWND hWnd, LPARAM lParam)
//
BOOL CALLBACK WindowsEnumProc(HWND hWnd, LPARAM lParam)
{
	/* Based off of Jugg's task.dll */
	if (IsWindow(hWnd))
	{
		if ((GetWindowLong(hWnd, GWL_USERDATA) != magicDWord) &&
		        (!(GetWindowLong(hWnd, GWL_STYLE) & WS_CHILD) &&
		         (GetWindowLong(hWnd, GWL_STYLE) & WS_VISIBLE)) &&
		        (!(GetWindowLong(hWnd, GWL_EXSTYLE) & WS_EX_TOOLWINDOW)))
		{
			ShowWindow(hWnd, (UINT)lParam);
		}
	}
	return TRUE;
}


//
// BangMinimizeWindows(HWND hCaller, LPCSTR pszArgs)
//
void BangMinimizeWindows(HWND /* hCaller */, LPCSTR /* pszArgs */)
{
	EnumWindows(WindowsEnumProc, (LPARAM)SW_MINIMIZE);
}


//
// BangPopup(HWND hCaller, LPCSTR pszArgs)
//
void BangPopup(HWND /* hCaller */, LPCSTR pszArgs)
{
	HWND hLiteStep = GetLitestepWnd();
	POINT p;
	int nCountTokens = 0;
	SAFE_CHAR(szFirst, MAX_LINE_LENGTH);
	SAFE_CHAR(szSecond, MAX_LINE_LENGTH);
	LPSTR aszTokens[] = { szFirst, szSecond };

	if (IsValidStringPtr(pszArgs))
	{
		nCountTokens = LCTokenize(pszArgs, aszTokens, 2, NULL);
	}
	if (hLiteStep)
	{
		if (GetCursorPos((LPPOINT) & p) == 0)
		{
			p.x = p.y = 0;
		}
		if (nCountTokens > 1)
		{
			for (int i = 0; i <= 1; i++)
			{
				if (strnicmp(aszTokens[i], "X=", 2) == 0)
				{
					p.x = atol(aszTokens[i] + 2);
				}
				else if (strnicmp(aszTokens[i], "Y=", 2) == 0)
				{
					p.y = atol(aszTokens[i] + 2);
				}
			}
		}
		SendMessage(hLiteStep, LM_HIDEPOPUP, 0, 0);
		SendMessage(hLiteStep, LM_POPUP, 0, MAKELPARAM(p.x, p.y));
	}
}


//
// BangQuit(HWND hCaller, LPCSTR pszArgs)
//
void BangQuit(HWND /* hCaller */, LPCSTR /* pszArgs */)
{
	HWND hLiteStep = GetLitestepWnd();

	if (hLiteStep)
	{
		SendMessage(hLiteStep, LM_RECYCLE, LR_QUIT, 0);
	}
}


//
// BangRecycle(HWND hCaller, LPCSTR pszArgs)
//
void BangRecycle(HWND /* hCaller */, LPCSTR /* pszArgs */)
{
	HWND hLiteStep = GetLitestepWnd();

	if (hLiteStep)
	{
		SendMessage(hLiteStep, LM_RECYCLE, LR_RECYCLE, 0);
	}
}


//
// BangRefresh(HWND hCaller, LPCSTR pszArgs)
//
void BangRefresh(HWND hCaller, LPCSTR pszArgs)
{
	HWND hLiteStep = GetLitestepWnd();

	BangReload(hCaller, pszArgs);

	if (hLiteStep)
	{
		SendMessage(hLiteStep, LM_REFRESH, 0, 0);
	}
}


void BangReload(HWND hCaller, LPCSTR pszArgs)
{
	//CloseRC();
	//setupVars(szAppPath);
	//SetupRC(szRcPath);
}


//
// BangReloadModule(HWND hCaller, LPCSTR pszArgs)
//
void BangReloadModule(HWND /* hCaller */, LPCSTR pszArgs)
{
	if (IsValidStringPtr(pszArgs))
	{
		HWND hLiteStep = GetLitestepWnd();

		if (hLiteStep)
		{
			LPCSTR pszNextToken = pszArgs;
			SAFE_CHAR(szPath, MAX_PATH);

			while (GetToken(pszNextToken, szPath, &pszNextToken, false))
			{
				SendMessage(hLiteStep, LM_RELOADMODULE, (WPARAM)szPath, 0);
			}
		}
	}
}


//
// BangRestoreWindows(HWND hCaller, LPCSTR pszArgs)
//
void BangRestoreWindows(HWND /* hCaller */, LPCSTR /* pszArgs */)
{
	EnumWindows(WindowsEnumProc, (LPARAM)SW_RESTORE);
}


//
// BangRun(HWND hCaller, LPCSTR pszArgs)
//
void BangRun(HWND /* hCaller */, LPCSTR /* pszArgs */)
{
	FARPROC (__stdcall * MSRun)(HWND, int, int, LPSTR, LPSTR, int);

	MSRun = (FARPROC (__stdcall *) (HWND, int, int, LPSTR, LPSTR, int))GetProcAddress(GetModuleHandle("SHELL32.DLL"), (LPSTR)((long)0x3D));
	MSRun(NULL, 0, 0, NULL, NULL, 0);
}


//
// BangShutdown(HWND hCaller, LPCSTR pszArgs)
//
void BangShutdown(HWND /* hCaller */, LPCSTR /* pszArgs */)
{
	HWND hLiteStep = GetLitestepWnd();

	if (hLiteStep)
	{
		SendMessage(hLiteStep, LM_RECYCLE, LR_MSSHUTDOWN, 0);
	}
}


//
// BangSwitchUser(HWND hCaller, LPCSTR pszArgs)
//
void BangSwitchUser(HWND /* hCaller */, LPCSTR /* pszArgs */)
{
	FARPROC (__stdcall * LockWorkStation)() = NULL;
	LockWorkStation = (FARPROC (__stdcall *)())GetProcAddress(GetModuleHandle("USER32.DLL"), "LockWorkStation");

	if (LockWorkStation)
	{
		LockWorkStation();
	}
}


//
// BangTileWindowsH(HWND hCaller, LPCSTR pszArgs)
//
void BangTileWindowsH(HWND /* hCaller */, LPCSTR /* pszArgs */)
{
	TileWindows(NULL, MDITILE_HORIZONTAL, NULL, 0, NULL);
}


//
// BangTileWindowsV(HWND hCaller, LPCSTR pszArgs)
//
void BangTileWindowsV(HWND /* hCaller */, LPCSTR /* pszArgs */)
{
	TileWindows(NULL, MDITILE_VERTICAL, NULL, 0, NULL);
}


//
// BangToggleWharf(HWND hCaller, LPCSTR pszArgs)
//
void BangToggleWharf(HWND /* hCaller */, LPCSTR /* pszArgs */)
{
	HWND hLiteStep = GetLitestepWnd();

	if (hLiteStep)
	{
		SendMessage(hLiteStep, LM_SHADETOGGLE, 0, 0);
	}
}


//
// BangUnloadModule(HWND hCaller, LPCSTR pszArgs)
//
void BangUnloadModule(HWND /* hCaller */, LPCSTR pszArgs)
{
	HWND hLiteStep = GetLitestepWnd();

	if (hLiteStep)
	{
		LPCSTR pszNextToken = pszArgs;
		SAFE_CHAR(szPath, MAX_PATH);

		while (GetToken(pszNextToken, szPath, &pszNextToken, false))
		{
			SendMessage(hLiteStep, LM_UNLOADMODULE, (WPARAM)szPath, 0);
		}
	}
}


//
// CALLBACK EnumHideWindowsProc(HWND hWnd, LPARAM lParam)
//
BOOL CALLBACK EnumHideWindowsProc(HWND hWnd, LPARAM lParam)
{
	if (IsWindow(hWnd) &&
	        IsWindowVisible(hWnd) &&
	        (GetWindowLong(hWnd, GWL_USERDATA) == magicDWord))
	{
		SetWindowLong(hWnd, GWL_USERDATA, HIDEmagicDWord);
		ShowWindow(hWnd, SW_HIDE);
	}

	return TRUE;
}


//
// BangHideModules(HWND hCaller, LPCSTR pszArgs)
//
void BangHideModules(HWND /* hCaller */, LPCSTR /* pszArgs */)
{
	EnumWindows(EnumHideWindowsProc, NULL);
}


//
// CALLBACK EnumShowWindowsProc(HWND hWnd, LPARAM lParam)
//
BOOL CALLBACK EnumShowWindowsProc(HWND hWnd, LPARAM lParam)
{
	if (IsWindow(hWnd) &&
	        (GetWindowLong(hWnd, GWL_USERDATA) == HIDEmagicDWord))
	{
		SetWindowLong(hWnd, GWL_USERDATA, magicDWord);
		ShowWindow(hWnd, SW_SHOW);
	}

	return TRUE;
}


//
// BangShowModules(HWND hCaller, LPCSTR pszArgs)
//
void BangShowModules(HWND /* hCaller */, LPCSTR /* pszArgs */)
{
	EnumWindows(EnumShowWindowsProc, NULL);
}
