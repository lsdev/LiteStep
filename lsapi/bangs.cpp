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
#include "bangs.h"
#include "../utility/core.hpp"


void SetupBangs()
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
	AddBangCommand("!UNLOADMODULE", BangUnloadModule);
	AddBangCommand("!HIDEMODULES", BangHideModules);
	AddBangCommand("!SHOWMODULES", BangShowModules);
    AddBangCommand("!TOGGLEMODULES", BangToggleModules);
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

    MessageBox(NULL,
        "LiteStep 0.24.7 Beta 4\nBeta release.\n\n"
        "Build Date: "__DATE__"\n"
        "Website TBA\n\n"
        "Copyright © 1997-2003\nThe LiteStep Development Team.",
        "About LiteStep...",
        MB_OK | MB_TOPMOST | MB_SETFOREGROUND | MB_ICONINFORMATION);
}


//
// BangAlert(HWND hCaller, LPCSTR pszArgs)
//
void BangAlert(HWND hCaller, LPCSTR pszArgs)
{
	char szMessage[MAX_PATH];
	char szTitle[MAX_PATH];
	LPSTR aszTokens[] = { szMessage, szTitle };

	int nTokenCount = LCTokenize(pszArgs, aszTokens, 2, 0);

	if (nTokenCount >= 1)
	{
		if (nTokenCount == 1)
		{
			StringCchCopy(szTitle, MAX_PATH, "Litestep");
		}

		MessageBox(hCaller, szMessage, szTitle, MB_OK | MB_TOPMOST | MB_SETFOREGROUND);
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
		char szFirst[MAX_PATH];
		char szSecond[MAX_PATH];
		char szThird[MAX_PATH];
		char szFourth[MAX_PATH];
		LPSTR aszTokens[] = { szFirst, szSecond, szThird, szFourth };

		int nTokenCount = CommandTokenize(pszArgs, aszTokens, 4, 0);

		if (nTokenCount >= 3)
		{
			if (nTokenCount == 3)
			{
				StringCchCopy(szFourth, MAX_PATH, "Litestep");
			}

			if (MessageBox(hCaller, szFirst, (nTokenCount == 3) ? szFourth : szSecond, MB_YESNO | MB_TOPMOST | MB_SETFOREGROUND) == IDYES)
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
		char szCommand[MAX_LINE_LENGTH];

		while (GetToken(pszNextToken, szCommand, &pszNextToken, TRUE))
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
		PostMessage(hLiteStep, LM_RECYCLE, LR_LOGOFF, 0);
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
// BangQuit(HWND hCaller, LPCSTR pszArgs)
//
void BangQuit(HWND /* hCaller */, LPCSTR /* pszArgs */)
{
	HWND hLiteStep = GetLitestepWnd();

	if (hLiteStep)
	{
		PostMessage(hLiteStep, LM_RECYCLE, LR_QUIT, 0);
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
		PostMessage(hLiteStep, LM_RECYCLE, LR_RECYCLE, 0);
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


void BangReload(HWND /* hCaller */, LPCSTR /* pszArgs */)
{
    DeleteSettingsManager();
    SetupSettingsManager(NULL, NULL);
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
			char szModuleString[MAX_PATH];

			while (GetToken(pszNextToken, szModuleString, &pszNextToken, true))
			{
				SendMessage(hLiteStep, LM_RELOADMODULE, (WPARAM)szModuleString, 0);
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
		PostMessage(hLiteStep, LM_RECYCLE, LR_MSSHUTDOWN, 0);
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
// BangUnloadModule(HWND hCaller, LPCSTR pszArgs)
//
void BangUnloadModule(HWND /* hCaller */, LPCSTR pszArgs)
{
	HWND hLiteStep = GetLitestepWnd();

	if (hLiteStep)
	{
		LPCSTR pszNextToken = pszArgs;
		char szPath[MAX_PATH];

		while (GetToken(pszNextToken, szPath, &pszNextToken, false))
		{
			SendMessage(hLiteStep, LM_UNLOADMODULE, (WPARAM)szPath, 0);
		}
	}
}


#define EMP_HIDE   0
#define EMP_SHOW   1
#define EMP_TOGGLE 2

//
// CALLBACK EnumModulesProc(HWND hWnd, LPARAM lParam)
//
BOOL CALLBACK EnumModulesProc(HWND hWnd, LPARAM lParam)
{
    if (IsWindow(hWnd))
    {
        long lUserData = GetWindowLong(hWnd, GWL_USERDATA);

        if ((lUserData == magicDWord) && IsWindowVisible(hWnd) &&
            (lParam == EMP_HIDE || lParam == EMP_TOGGLE))
        {
            SetWindowLong(hWnd, GWL_USERDATA, HIDEmagicDWord);
            ShowWindow(hWnd, SW_HIDE);
        }
        else if ((lUserData == HIDEmagicDWord) &&
            (lParam == EMP_SHOW || lParam == EMP_TOGGLE))
        {
            SetWindowLong(hWnd, GWL_USERDATA, magicDWord);
            ShowWindow(hWnd, SW_SHOW);
        }
    }

    return TRUE;
}


//
// BangHideModules(HWND hCaller, LPCSTR pszArgs)
//
void BangHideModules(HWND /* hCaller */, LPCSTR /* pszArgs */)
{
	EnumWindows(EnumModulesProc, EMP_HIDE);
}


//
// BangShowModules(HWND hCaller, LPCSTR pszArgs)
//
void BangShowModules(HWND /* hCaller */, LPCSTR /* pszArgs */)
{
	EnumWindows(EnumModulesProc, EMP_SHOW);
}


//
// BangToggleModules(HWND hCaller, LPCSTR pszArgs)
//
void BangToggleModules(HWND /* hCaller */, LPCSTR /* pszArgs */)
{
    EnumWindows(EnumModulesProc, EMP_TOGGLE);
}
