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
#include "StartupRunner.h"
#include "../lsapi/lsapi.h"
#include "../utility/shellhlp.h"
#include <regstr.h>
#include "../utility/safestr.h"

#define ERK_NONE				0x0000
#define ERK_RUNSUBKEYS			0x0001
#define ERK_DELETE				0x0002
#define REGSTR_PATH_RUN_POLICY	REGSTR_PATH_POLICIES _T("\\Explorer\\Run")

StartupRunner::StartupRunner()
{}

StartupRunner::~StartupRunner()
{}

DWORD StartupRunner::Run(void* pvVoid)
{
	BOOL bRunStartup = _IsFirstRunThisSession();

	if (bRunStartup)
	{
		// When MS releases the SHRestricted API, we will use it
		BOOL bHKLMRun = !SHRestricted(REST_NOLOCALMACHINERUN);
		BOOL bHKCURun = !SHRestricted(REST_NOCURRENTUSERRUN);
		BOOL bHKLMRunOnce = !SHRestricted(REST_NOLOCALMACHINERUNONCE);
		BOOL bHKCURunOnce = !SHRestricted(REST_NOCURRENTUSERRUNONCE);

		// Ini Sections

		// HKLM Keys
		_RunRegKeys(HKEY_LOCAL_MACHINE, REGSTR_PATH_RUN_POLICY, ERK_NONE);
		if (bHKLMRun)
		{
			_RunRegKeys(HKEY_LOCAL_MACHINE, REGSTR_PATH_RUN, ERK_NONE);
		}

		// HKCU Keys
		_RunRegKeys(HKEY_CURRENT_USER, REGSTR_PATH_RUN_POLICY, ERK_NONE);
		if (bHKCURun)
		{
			_RunRegKeys(HKEY_CURRENT_USER, REGSTR_PATH_RUN, ERK_NONE);
		}

		_RunStartupMenu();

		if (bHKLMRunOnce)
		{
			_RunRegKeys(HKEY_LOCAL_MACHINE, REGSTR_PATH_RUNONCE, (ERK_RUNSUBKEYS | ERK_DELETE));
		}

		if (bHKCURunOnce)
		{
			_RunRegKeys(HKEY_CURRENT_USER, REGSTR_PATH_RUNONCE, (ERK_RUNSUBKEYS | ERK_DELETE));
		}

		_RunRegKeys(HKEY_LOCAL_MACHINE, REGSTR_PATH_RUN_POLICY, ERK_RUNSUBKEYS);
		if (bHKLMRun)
		{
			_RunRegKeys(HKEY_LOCAL_MACHINE, REGSTR_PATH_RUN, ERK_RUNSUBKEYS);
		}

		_RunRegKeys(HKEY_CURRENT_USER, REGSTR_PATH_RUN_POLICY, ERK_RUNSUBKEYS);
		if (bHKCURun)
		{
			_RunRegKeys(HKEY_CURRENT_USER, REGSTR_PATH_RUN, ERK_RUNSUBKEYS);
		}
	}

	return bRunStartup;
}

void StartupRunner::_RunStartupMenu()
{
	const UINT STARTUPMENU_TABLE[] =
	    {
	        CSIDL_COMMON_STARTUP,
	        CSIDL_COMMON_ALTSTARTUP,
	        CSIDL_STARTUP,
	        CSIDL_ALTSTARTUP
	    };
	const ULONG STARTUPMENU_SIZE = sizeof(STARTUPMENU_TABLE) / sizeof(UINT);

	TCHAR tzPath[MAX_PATH];

	for (int i = 0; i < STARTUPMENU_SIZE; ++i)
	{
		if (GetShellFolderPath(STARTUPMENU_TABLE[i], tzPath, MAX_PATH))
		{
			PathUnquoteSpaces(tzPath);
			_RunFolderContents(tzPath);
		}
	}
}

void StartupRunner::_RunFolderContents(LPCTSTR ptzPath)
{
	TCHAR tzSearchPath[MAX_PATH];
	WIN32_FIND_DATA findData;
	HANDLE hSearch;

	if (IsValidStringPtr(ptzPath) && (ptzPath[0] != _T('\0')))
	{
		StringCchCopy(tzSearchPath, MAX_PATH, ptzPath);
		PathAppend(tzSearchPath, _T("*.*"));

		hSearch = FindFirstFile(tzSearchPath, &findData);
		while (hSearch != INVALID_HANDLE_VALUE)
		{
			if (!PathIsDirectory(findData.cFileName) &&
			        (!(findData.dwFileAttributes & FILE_ATTRIBUTE_SYSTEM) && !(findData.dwFileAttributes & FILE_ATTRIBUTE_HIDDEN)))
			{
				SHELLEXECUTEINFO seiCommand;

				memset(&seiCommand, 0, sizeof(seiCommand));
				seiCommand.cbSize = sizeof(SHELLEXECUTEINFO);

				seiCommand.lpFile = findData.cFileName;
				seiCommand.lpDirectory = ptzPath;
				seiCommand.nShow = SW_SHOWNORMAL;
				seiCommand.fMask = SEE_MASK_DOENVSUBST | SEE_MASK_FLAG_NO_UI;

				ShellExecuteEx(&seiCommand);
			}


			if (!FindNextFile(hSearch, &findData))
			{
				FindClose(hSearch);
				hSearch = INVALID_HANDLE_VALUE;
			}
		}
	}
}


//
// IsFirstRunThisSession()
//
BOOL StartupRunner::_IsFirstRunThisSession()
{
	BOOL bReturn = FALSE;
	HKEY hkExplorer;
	TCHAR tzSessionInfo[30];
	LONG lResult;

	OSVERSIONINFO OsVersionInfo;
	OsVersionInfo.dwOSVersionInfoSize = sizeof(OsVersionInfo);
	GetVersionEx(&OsVersionInfo);

	// On NT systems, the SessionInfo subkey will be the AuthenticationID
	if (OsVersionInfo.dwPlatformId == VER_PLATFORM_WIN32_NT)
	{
		HANDLE hToken;
		if (OpenProcessToken(GetCurrentProcess(), TOKEN_QUERY, &hToken) != 0)
		{
			TOKEN_STATISTICS tsStats;
			DWORD dwOutSize;

			if (GetTokenInformation(hToken, TokenStatistics, &tsStats, sizeof(tsStats), &dwOutSize) != 0)
			{
				CloseHandle(hToken);

				StringCchPrintf(tzSessionInfo, 30, _T("SessionInfo\\%08x%08x"), tsStats.AuthenticationId.HighPart, tsStats.AuthenticationId.LowPart);

				// Create the SessionInfo and StartUpHasBeenRun keys
				lResult = RegCreateKeyEx(HKEY_CURRENT_USER, REGSTR_PATH_EXPLORER, 0, NULL, REG_OPTION_NON_VOLATILE,
				                         MAXIMUM_ALLOWED, NULL, &hkExplorer, NULL);
				if (lResult == ERROR_SUCCESS)
				{
					HKEY hkSessionInfo;

					lResult = RegCreateKeyEx(hkExplorer, tzSessionInfo, 0, NULL, REG_OPTION_VOLATILE,
					                         KEY_WRITE, NULL, &hkSessionInfo, NULL);
					RegCloseKey(hkExplorer);
					if (lResult == ERROR_SUCCESS)
					{
						DWORD dwDisposition;
						HKEY hkStartup;
						lResult = RegCreateKeyEx(hkSessionInfo, _T("StartupHasBeenRun"), 0, NULL, REG_OPTION_VOLATILE,
						                         KEY_WRITE, NULL, &hkStartup, &dwDisposition);
						RegCloseKey(hkSessionInfo);
						
                        if (lResult == ERROR_SUCCESS)
                        {
                            RegCloseKey(hkStartup);
                        }

                        if (dwDisposition == REG_CREATED_NEW_KEY)
						{
							bReturn = TRUE;
						}
					}
				}
			}
		}
	}
	else
	{
		bReturn = TRUE;
	}

	return bReturn;
}


//
// RunRegKeys(HKEY hkParent, LPCSTR pszSubKey, DWORD dwFlags)
//
void StartupRunner::_RunRegKeys(HKEY hkParent, LPCTSTR ptzSubKey, DWORD dwFlags)
{
	HKEY hkSubKey;
	LONG lResult;

	lResult = RegOpenKeyEx(hkParent, ptzSubKey, 0, MAXIMUM_ALLOWED, &hkSubKey);
	if (lResult == ERROR_SUCCESS)
	{
		if (dwFlags & ERK_RUNSUBKEYS)
		{
			DWORD dwNewFlags = dwFlags & ~(ERK_RUNSUBKEYS);

			TCHAR tzNameBuffer[255];
			DWORD dwLoop, dwNameSize;

			for (dwLoop = 0; ; ++dwLoop)
			{
				dwNameSize = 255;
				lResult = RegEnumKey(hkSubKey, dwLoop, tzNameBuffer, dwNameSize);
				if (lResult == ERROR_MORE_DATA)
				{
					continue;
				}
				else if (lResult == ERROR_SUCCESS)
				{
					_RunRegKeys(hkSubKey, tzNameBuffer, dwNewFlags);

					if (dwFlags & ERK_DELETE)
					{
						if (RegDeleteValue(hkSubKey, tzNameBuffer) == ERROR_SUCCESS)
						{
							dwLoop--;
						}
					}
				}
				else
				{
					break;
				}
			}
		}
		else
		{
			TCHAR tzNameBuffer[255], tzValueBuffer[MAX_LINE_LENGTH];
			DWORD dwLoop, dwNameSize, dwValueSize, dwType;

			for (dwLoop = 0; ; ++dwLoop)
			{
				dwNameSize = 255;
				dwValueSize = sizeof(tzValueBuffer);
				lResult = RegEnumValue(hkSubKey, dwLoop, tzNameBuffer, &dwNameSize, NULL, &dwType,
				                       (LPBYTE)tzValueBuffer, &dwValueSize);
				if (lResult == ERROR_MORE_DATA)
				{
					continue;
				}
				else if (lResult == ERROR_SUCCESS)
				{
					if ((dwType == REG_SZ) || (dwType == REG_EXPAND_SZ))
					{
						SHELLEXECUTEINFO seiCommand;
						LPSTR pszArgs;

						memset(&seiCommand, 0, sizeof(seiCommand));
						seiCommand.cbSize = sizeof(SHELLEXECUTEINFO);

						// We are going to let ShellExecuteEx expand environment strings
						// for us
						/*if (dwType == REG_EXPAND_SZ)
						{
							DWORD dwResult;
							char szTempBuffer[MAX_LINE_LENGTH];
							StringCchCopy(szTempBuffer, MAX_LINE_LENGTH, szValueBuffer);

							dwResult = ExpandEnvironmentStrings(szTempBuffer, szValueBuffer, MAX_LINE_LENGTH);
							if ((dwResult > MAX_LINE_LENGTH) || (dwResult == 0))
							{
								continue;
							}
						}*/

						if ((dwFlags & ERK_DELETE) && (tzNameBuffer[0] != _T('!')))
						{
							if (RegDeleteValue(hkSubKey, tzNameBuffer) == ERROR_SUCCESS)
							{
								dwLoop--;
							}
						}

						pszArgs = PathGetArgs(tzValueBuffer);
						if (*pszArgs)
						{
							tzValueBuffer[pszArgs - tzValueBuffer - 1] = _T('\0');
						}

						PathUnquoteSpaces(tzValueBuffer);

						seiCommand.lpFile = tzValueBuffer;
						seiCommand.lpParameters = pszArgs;
						seiCommand.nShow = SW_SHOWNORMAL;
						seiCommand.fMask = SEE_MASK_DOENVSUBST | SEE_MASK_FLAG_NO_UI;

						ShellExecuteEx(&seiCommand);

						if ((dwFlags & ERK_DELETE) && (tzNameBuffer[0] == _T('!')))
						{
							if (RegDeleteValue(hkSubKey, tzNameBuffer) == ERROR_SUCCESS)
							{
								dwLoop--;
							}
						}
					}
				}
				else
				{
					break;
				}
			}
		}
		RegCloseKey(hkSubKey);
	}
}
