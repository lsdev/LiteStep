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
#include "CommandLine.h"

void installLitestep(LPCSTR szPath, HINSTANCE hLiteStep)
{

	OSVERSIONINFO osInfo;
	char szLs[MAX_PATH];

	RESOURCE_TITLE(hLiteStep, IDS_LITESTEP_TITLE_INSTALL, "LiteStep Installation")

	ZeroMemory(&osInfo, sizeof(osInfo));
	osInfo.dwOSVersionInfoSize = sizeof(osInfo);
	GetVersionEx(&osInfo);

	RESOURCE_STR(hLiteStep, IDS_LITESTEP_INSTALL1,
	             "Do you want to install Litestep as your default shell?");
	if (RESOURCE_YESNO_MSGBOX == IDYES)
	{

		if (osInfo.dwPlatformId == VER_PLATFORM_WIN32_NT)
		{
			RESOURCE_STR(hLiteStep, IDS_LITESTEP_INSTALL2,
			             "Would you like to update your Registry with the Shell path?");
			if (RESOURCE_YESNO_MSGBOX == IDYES)
			{

				HKEY key;
				DWORD result;

				if (szLs[strlen(szLs) - 1] == '\\')
				{
					wsprintf(szLs, "%sLitestep.exe\0\0", szPath);
				}
				else
				{
					wsprintf(szLs, "%s\\Litestep.exe\0\0", szPath);
				}


				RESOURCE_STREX(hLiteStep, IDS_LITESTEP_INSTALL3, resourceTextBuffer, MAX_LINE_LENGTH,
				               "Install %s for All Users? No will install for Current User (yourself only)", szLs);
				if (RESOURCE_YESNO_MSGBOX == IDYES)
				{
					// setup reg key for all users
					if (RegCreateKeyEx(HKEY_LOCAL_MACHINE, "Software\\Microsoft\\Windows NT\\CurrentVersion\\Winlogon\0", 0,
					                   NULL, REG_OPTION_NON_VOLATILE, KEY_WRITE, NULL, &key, &result) == ERROR_SUCCESS)
					{
						if (RegSetValueEx(key, "Shell\0", 0, REG_SZ, (LPBYTE)szLs, lstrlen(szLs) + 1) == ERROR_SUCCESS)
						{
							RESOURCE_MSGBOX(hLiteStep, IDS_LITESTEP_INSTALL4,
							                "Installation successful!\nInstallation complete!", resourceTitleBuffer)
						}
						else
						{
							RESOURCE_MSGBOX(hLiteStep, IDS_LITESTEP_INSTALL5,
							                "Installation unsuccessful!\nUnable to set registry key.", resourceTitleBuffer)
							RegCloseKey(key);
						}
					}
					else
					{
						RESOURCE_MSGBOX(hLiteStep, IDS_LITESTEP_INSTALL5,
						                "Installation unsuccessful!\nUnable to set registry key.", resourceTitleBuffer)
					}
				}
				else
				{
					// setup reg key for current user only
					if (RegCreateKeyEx(HKEY_CURRENT_USER, "Software\\Microsoft\\Windows NT\\CurrentVersion\\Winlogon\0", 0,
					                   NULL, REG_OPTION_NON_VOLATILE, KEY_WRITE, NULL, &key, &result) == ERROR_SUCCESS)
					{
						if (RegSetValueEx(key, "Shell\0", 0, REG_SZ, (LPBYTE)szLs, lstrlen(szLs) + 1) == ERROR_SUCCESS)
						{
							RESOURCE_MSGBOX(hLiteStep, IDS_LITESTEP_INSTALL4,
							                "Installation successful!\nInstallation complete!", resourceTitleBuffer)
						}
						else
						{
							RESOURCE_MSGBOX(hLiteStep, IDS_LITESTEP_INSTALL5,
							                "Installation unsuccessful!\nUnable to set registry key.", resourceTitleBuffer)
							RegCloseKey(key);
						}
					}
					else
					{
						RESOURCE_MSGBOX(hLiteStep, IDS_LITESTEP_INSTALL5,
						                "Installation unsuccessful!\nUnable to set registry key.", resourceTitleBuffer)
					}
				}
			}
		}
		else
		{ // Win9x
			char szWinDir[MAX_PATH];

			GetWindowsDirectory(szWinDir, sizeof(szWinDir));
			strcat(szWinDir, "\\");
			strcat(szWinDir, "SYSTEM.INI");
			if (szPath[strlen(szPath) - 1] == '\\')
				wsprintf(szLs, "%slitestep.exe", szPath);
			else
				wsprintf(szLs, "%s\\litestep.exe", szPath);

			//wsprintf(szInf, "Would you like to update %s with the following\r\n\r\nShell=%s", szWinDir, szLs);
			RESOURCE_STREX(hLiteStep, IDS_LITESTEP_INSTALL6, resourceTextBuffer, MAX_LINE_LENGTH,
			               "Would you like to update %s with the following\r\n\r\nShell=%s",
			               szWinDir, szLs);
			if (RESOURCE_YESNO_MSGBOX == IDYES)
			{
				if (WritePrivateProfileString("Boot", "Shell", szLs, szWinDir) != 0)
				{
					RESOURCE_MSGBOX(hLiteStep, IDS_LITESTEP_INSTALL4,
					                "Installation successful!\nInstallation complete!", resourceTitleBuffer)
				}
				else
				{
					//wsprintf(szInf, "Unable to update %s \n You will need to manually add %s to %s", szWinDir, szLs, szWinDir);
					RESOURCE_STREX(hLiteStep, IDS_LITESTEP_INSTALL7, resourceTextBuffer, MAX_LINE_LENGTH,
					               "Installation unsuccessful!\nUnable to update %s \n You will need to manually add %s to %s",
					               szWinDir, szLs, szWinDir);
					RESOURCE_DISPLAY_MSGBOX
				}
			}
			else
			{
				//wsprintf(szInf, "You will need to manually add %s to %s", szLs, szWinDir);
				RESOURCE_STREX(hLiteStep, IDS_LITESTEP_INSTALL8, resourceTextBuffer, MAX_LINE_LENGTH,
				               "Installation unsuccessful!\nYou will need to manually add %s to %s",
				               szLs, szWinDir);
				RESOURCE_DISPLAY_MSGBOX
			}
		}
	}
}

void uninstallLitestep()
{

	OSVERSIONINFO osInfo;
	// bool setupSucceed; // UNUSED

	ZeroMemory(&osInfo, sizeof(osInfo));
	osInfo.dwOSVersionInfoSize = sizeof(osInfo);
	GetVersionEx(&osInfo);

	if (MessageBox(NULL, "Do you want to uninstall Litestep?", "LiteStep Installation", MB_YESNO | MB_TOPMOST) == IDYES)
	{
		if (osInfo.dwPlatformId == VER_PLATFORM_WIN32_NT)
		{
			char szLs[MAX_PATH], szInf[1024];
			HKEY key;
			DWORD result;

			GetWindowsDirectory(szInf, MAX_PATH);

			wsprintf(szLs, "%s\\explorer.exe\0\0", szInf);

			wsprintf(szInf, "Install %s for All Users? No will install for Current User (yourself only)", szLs);
			if (MessageBox(NULL, szInf, "LiteStep Uninstallation", MB_YESNO | MB_TOPMOST) == IDYES)
			{
				// setup reg key for all users
				if (RegCreateKeyEx(HKEY_LOCAL_MACHINE, "Software\\Microsoft\\Windows NT\\CurrentVersion\\Winlogon\0", 0,
				                   NULL, REG_OPTION_NON_VOLATILE, KEY_WRITE, NULL, &key, &result) == ERROR_SUCCESS)
				{
					if (RegSetValueEx(key, "Shell\0", 0, REG_SZ, (LPBYTE)szLs, lstrlen(szLs) + 1) == ERROR_SUCCESS)
						MessageBox(NULL, "Uninstallation successful, you may now safely delete the Litestep Directory.", "LiteStep Uninstallation Complete", MB_OK | MB_TOPMOST);
					else
						MessageBox(NULL, "Unable to set registry key!", "LiteStep Uninstallation Did Not Complete", MB_OK | MB_TOPMOST);
					RegCloseKey(key);
				}
				else
					MessageBox(NULL, "Unable to open registry key!", "LiteStep Uninstallation Did Not Complete", MB_OK);
			}
			else
			{
				// setup reg key for current user only
				if (RegCreateKeyEx(HKEY_CURRENT_USER, "Software\\Microsoft\\Windows NT\\CurrentVersion\\Winlogon\0", 0,
				                   NULL, REG_OPTION_NON_VOLATILE, KEY_WRITE, NULL, &key, &result) == ERROR_SUCCESS)
				{
					if (RegSetValueEx(key, "Shell\0", 0, REG_SZ, (LPBYTE)szLs, lstrlen(szLs) + 1) == ERROR_SUCCESS)
						MessageBox(NULL, "Uninstallation successful, ensure no other users are using Litestep before deleting the Litestep directory.", "LiteStep Uinstallation Complete", MB_OK | MB_TOPMOST);
					else
						MessageBox(NULL, "Unable to set registry key!", "LiteStep Uninstallation Did Not Complete", MB_OK | MB_TOPMOST);
					RegCloseKey(key);
				}
				else
					MessageBox(NULL, "Unable to open registry key!", "LiteStep Uninstallation Did Not Complete", MB_OK | MB_TOPMOST);
			}
		}
		else
		{ // Win9x
			char szWinDir[MAX_PATH], szInf[1024], szLs[MAX_PATH];

			GetWindowsDirectory(szWinDir, sizeof(szWinDir));
			strcat(szWinDir, "\\");
			wsprintf(szLs, "%sexplorer.exe", szWinDir);
			strcat(szWinDir, "SYSTEM.INI");


			wsprintf(szInf, "Would you like to update %s with the following?\n\nShell=%s", szWinDir, szLs);
			if (MessageBox(NULL, szInf, "LiteStep Uninstallation", MB_YESNO | MB_TOPMOST) == IDYES)
			{
				if (WritePrivateProfileString("Boot", "Shell", szLs, szWinDir) != 0)
					MessageBox(NULL, "Uninstallation successful, you may now safely delete the Litestep Directory", "LiteStep Uninstallation Complete", MB_OK | MB_TOPMOST);
				else
				{
					wsprintf(szInf, "Unable to update %s \n You will need to manually add %s to %s", szWinDir, szLs, szWinDir);
					MessageBox(NULL, szInf, "LiteStep Uninstallation Did Not Complete", MB_OK | MB_TOPMOST);
				}
			}
			else
			{
				wsprintf(szInf, "You will need to manually add %s to %s", szLs, szWinDir);
				MessageBox(NULL, szInf, "LiteStep Uninstallation Did Not Complete", MB_OK | MB_TOPMOST);
			}
		}
	}
}