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
#include "RecoveryMenu.h"

struct
{
	int nStringID;
	int nCommandID;
	LPSTR pszDefText;
}
rgMenuCommands[] =
    {
        IDS_LITESTEP_RECYCLELS, ID_RECYCLE, "&Recycle LiteStep",
        IDS_LITESTEP_QUITLS, ID_QUIT, "&Quit LiteStep",
        IDS_LITESTEP_TERMINATELS, ID_TERMINATE, "Forcibly &Terminate LiteStep",
        IDS_LITESTEP_UNINSTALLLS, ID_UNINSTALL, "U&ninstall LiteStep",
        0, -1, "",
        IDS_LITESTEP_RUN, ID_RUN, "R&un...",
        0, -1, "",
        IDS_LITESTEP_SHUTDOWNWIN, ID_SHUTDOWN, "&Shutdown Windows..."
    };

const int cMenuCommands = sizeof(rgMenuCommands) / sizeof(rgMenuCommands[0]);

extern HINSTANCE hLSInstance;

// this thread provides a recovery menu which can be accessed in
// case errors render Litestep unusable
DWORD WINAPI RecoveryThreadProc(LPVOID)
{
	WNDCLASSEX wc;
	ZeroMemory(&wc, sizeof(WNDCLASSEX));

	wc.cbSize = sizeof(WNDCLASSEX);
	wc.lpfnWndProc = RecoveryMenuWndProc;
	wc.hInstance = hLSInstance;
	wc.lpszClassName = szRecoveryMenuWndClass;

	RegisterClassEx(&wc);

	HWND hRecoveryWnd =
            CreateWindowEx(0,
	               szRecoveryMenuWndClass,
	               NULL,
	               WS_POPUP,
	               0, 0, 0, 0,
	               NULL,
	               NULL,
	               hLSInstance,
	               NULL);

	MSG msg;

	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	DestroyWindow(hRecoveryWnd);
    UnregisterClass(szRecoveryMenuWndClass, hLSInstance);

	return 0;
}

LRESULT WINAPI RecoveryMenuWndProc(HWND hWnd, UINT nMessage, WPARAM wParam, LPARAM lParam)
{
	switch (nMessage)
	{
		case WM_HOTKEY:
		{
			if (wParam != ID_HOTKEY)
				break;

			// our hotkey was pressed, create the menu
            HMENU hMenu = CreatePopupMenu();

			// populate the menu
            for (int i = 0; i < cMenuCommands; i++)
			{
				if (rgMenuCommands[i].nStringID)
				{
					char szBuffer[MAX_PATH];
					//LoadString( (HINSTANCE) GetWindowLong( hWnd, GWL_HINSTANCE ), rgMenuCommands[i].nStringID, szBuffer, MAX_PATH );
					GetResStr(hLSInstance,
					          rgMenuCommands[i].nStringID, szBuffer, MAX_PATH,
					          rgMenuCommands[i].pszDefText);
					AppendMenu(hMenu, MF_STRING, rgMenuCommands[i].nCommandID, szBuffer);
				}
				else
				{
					AppendMenu(hMenu, MF_SEPARATOR, 0, NULL);
				}
			}

			// get the current position of the mouse
            POINT pt;
			GetCursorPos(&pt);

			SetForegroundWindow(hWnd);

            int nCommand = (int) TrackPopupMenu(hMenu,
			                                    TPM_LEFTBUTTON | TPM_RETURNCMD | TPM_NONOTIFY,
			                                    pt.x, pt.y,
			                                    0, hWnd, NULL);

            DestroyMenu(hMenu);
            
            switch (nCommand)
			{
				case ID_RECYCLE:
				{
					SendMessage(GetLitestepWnd(), LM_RECYCLE, 0, 0);
				}
				break;

				case ID_QUIT:
				{
					SendMessage(GetLitestepWnd(), LM_RECYCLE, 2, 0);
				}
				break;

				case ID_TERMINATE:
				{
					TerminateProcess(GetCurrentProcess(), 1);
				}
				break;

				case ID_RUN:
				{
					void (WINAPI * RunDlg)(HWND, HICON, LPCSTR, LPCSTR, LPCSTR, UINT) = (void (WINAPI *)(HWND, HICON, LPCSTR, LPCSTR, LPCSTR, UINT)) GetProcAddress(
					            GetModuleHandle("SHELL32.DLL"), (LPCSTR) 61);

					RunDlg(NULL, NULL, NULL, NULL, NULL, 0);
				}
				break;

				case ID_SHUTDOWN:
				{
					void (WINAPI * ShutdownDlg)(HWND) = (void (WINAPI *)(HWND)) GetProcAddress(
					                                        GetModuleHandle("SHELL32.DLL"), (LPCSTR) 60);

					ShutdownDlg(NULL);
				}
				break;

				case ID_UNINSTALL:
				{
					HMODULE hInstall = LoadLibrary("install.dll");
					if (hInstall)
					{
						FARPROC (__stdcall * UninstallShell)(VOID) = NULL;
						UninstallShell = (FARPROC (__stdcall *)(VOID))GetProcAddress(hInstall, "UninstallShell");

						if (UninstallShell)
						{
							UninstallShell();
						}

						UninstallShell = NULL;
						FreeLibrary(hInstall);
						hInstall = NULL;

						exit(0);
					}
				}
				break;
			}

			return 0;
		}

		case WM_CREATE:
		{
			RegisterHotKey(hWnd, ID_HOTKEY, MOD_CONTROL | MOD_ALT, VK_F1);
			return 0;
		}

		case WM_DESTROY:
		{
			UnregisterHotKey(hWnd, ID_HOTKEY);
			return 0;
		}

        case WM_CLOSE:
        {
            return 0;
        }
	}

	return DefWindowProc(hWnd, nMessage, wParam, lParam);
}
