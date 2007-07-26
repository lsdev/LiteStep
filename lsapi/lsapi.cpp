//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//
// This is a part of the Litestep Shell source code.
//
// Copyright (C) 1997-2007  Litestep Development Team
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
//
//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
#include "lsapi.h"
#include "lsapiinit.h"
#include "BangCommand.h"
#include "../utility/shellhlp.h"
#include "../utility/core.hpp"

static int _Tokenize(LPCSTR pszString, LPSTR* lpszBuffers, DWORD dwNumBuffers, LPSTR pszExtraParameters, BOOL bUseBrackets);

extern const char rcsRevision[];
const char rcsRevision[] = "$Revision: 1.26 $"; // Our Version
const char rcsId[] = "$Id: lsapi.cpp,v 1.26 2007/07/26 21:53:13 ilmcuts Exp $"; // The Full RCS ID.


BOOL LSAPIInitialize(LPCSTR pszLitestepPath, LPCSTR pszRcPath)
{
    try
    {
        g_LSAPIManager.Initialize(pszLitestepPath, pszRcPath);
    }
    catch(LSAPIException& lse)
    {
        lse.Type();
    }

    return g_LSAPIManager.IsInitialized() ? TRUE:FALSE;
}

void LSAPIReloadBangs(VOID)
{
    try
    {
        g_LSAPIManager.ReloadBangs();
    }
    catch(LSAPIException& lse)
    {
        lse.Type();
    }
}

void LSAPIReloadSettings(VOID)
{
    try
    {
        g_LSAPIManager.ReloadSettings();
    }
    catch(LSAPIException& lse)
    {
        lse.Type();
    }
}

void LSAPISetLitestepWindow(HWND hLitestepWnd)
{
    g_LSAPIManager.SetLitestepWindow(hLitestepWnd);
}


template<typename T>
BOOL AddBangCommandWorker(LPCSTR pszCommand, T pfnBangCommand)
{
	BOOL bReturn = false;

	if (IsValidStringPtr(pszCommand) && IsValidCodePtr((FARPROC)pfnBangCommand))
	{
		DWORD dwCurrentThreadID = GetCurrentThreadId();

		Bang* pBang = new Bang(dwCurrentThreadID, pfnBangCommand, pszCommand);

		if (IsValidReadPtr(pBang))
		{
			//bBang->AddRef();
			g_LSAPIManager.GetBangManager()->AddBangCommand(pszCommand, pBang);
			pBang->Release();
			bReturn = TRUE;
		}
	}

	return bReturn;
}


//
// AddBangCommand(LPCSTR pszCommand, BangCommand pfnBangCommand)
//
BOOL AddBangCommand(LPCSTR pszCommand, BangCommand pfnBangCommand)
{
	return AddBangCommandWorker(pszCommand, pfnBangCommand);
}


//
// AddBangCommandEx(LPCSTR pszCommand, BangCommand pfnBangCommand)
//
BOOL AddBangCommandEx(LPCSTR pszCommand, BangCommandEx pfnBangCommand)
{
	return AddBangCommandWorker(pszCommand, pfnBangCommand);
}


//
// RemoveBangCommand(LPCSTR pszCommand)
//
BOOL RemoveBangCommand(LPCSTR pszCommand)
{
	return g_LSAPIManager.GetBangManager()->RemoveBangCommand(pszCommand);
}


//
// InternalExecuteBangCommand
// Just like ParseBangCommand but without the variable expansion
//
BOOL InternalExecuteBangCommand(HWND hCaller, LPCSTR pszCommand, LPCSTR pszArgs)
{
	return g_LSAPIManager.GetBangManager()->
		ExecuteBangCommand(pszCommand, hCaller, pszArgs);
}


//
// ParseBangCommand(HWND hCaller, LPCSTR pszCommand, LPCSTR pszArgs)
//
BOOL ParseBangCommand(HWND hCaller, LPCSTR pszCommand, LPCSTR pszArgs)
{
	char szExpandedArgs[MAX_LINE_LENGTH] = { 0 };
	BOOL bReturn = FALSE;

	if (IsValidStringPtr(pszCommand))
	{
		if (IsValidStringPtr(pszArgs))
		{
			VarExpansionEx(szExpandedArgs, pszArgs, MAX_LINE_LENGTH);
		}

		bReturn = \
			InternalExecuteBangCommand(hCaller, pszCommand, szExpandedArgs);
	}

	return bReturn;
}


//
// CommandParse(LPCSTR pszCommand, LPSTR pszOutCommand, LPSTR pszOutArgs, size_t cchOutCommand, size_t cchOutArgs)
//
void CommandParse(LPCSTR pszCommand, LPSTR pszOutCommand, LPSTR pszOutArgs, size_t cchOutCommand, size_t cchOutArgs)
{
	char szCommand[MAX_LINE_LENGTH];
	char szTempCommand[MAX_LINE_LENGTH];
	LPCSTR pszTempArgs = NULL;

	if (IsValidStringPtr(pszCommand))
	{
		if (IsValidStringPtr(pszOutCommand, cchOutCommand))
		{
			VarExpansionEx(szCommand, pszCommand, MAX_LINE_LENGTH);

			GetToken(szCommand, szTempCommand, &pszTempArgs, true);

			StringCchCopy(pszOutCommand, cchOutCommand, szTempCommand);
		}
		if (IsValidStringPtr(pszOutArgs, cchOutArgs))
		{
			StringCchCopy(pszOutArgs, cchOutArgs, pszTempArgs);
		}
	}
}


//
// LSExecuteEx(HWND hOwner, LPCSTR pszOperation, LPCSTR pszCommand, LPCSTR pszArgs, LPCSTR pszDirectory, int nShowCmd)
//
HINSTANCE LSExecuteEx(HWND hOwner, LPCSTR pszOperation, LPCSTR pszCommand, LPCSTR pszArgs, LPCSTR pszDirectory, int nShowCmd)
{
	HINSTANCE hReturn = HINSTANCE(32);

	if (IsValidStringPtr(pszCommand))
	{
		if (pszCommand[0] == '!')
		{
			hReturn = ParseBangCommand(hOwner, pszCommand, pszArgs) ? HINSTANCE(33) : HINSTANCE(32);
		}
		else
		{
			DWORD dwType = GetFileAttributes(pszCommand);
			if ((dwType & FILE_ATTRIBUTE_DIRECTORY) && (dwType != 0xFFFFFFFF))
			{
				hReturn = ShellExecute(hOwner, pszOperation, pszCommand, pszArgs, NULL, nShowCmd ? nShowCmd : SW_SHOWNORMAL);
			}
			else
			{
				SHELLEXECUTEINFO seiCommand = { 0 };
				seiCommand.cbSize = sizeof(SHELLEXECUTEINFO);
				seiCommand.hwnd = hOwner;
				seiCommand.lpVerb = pszOperation;
				seiCommand.lpFile = pszCommand;
				seiCommand.lpParameters = pszArgs;
				seiCommand.lpDirectory = pszDirectory;
				seiCommand.nShow = nShowCmd;
				seiCommand.fMask = SEE_MASK_DOENVSUBST | SEE_MASK_FLAG_NO_UI;

				ShellExecuteEx(&seiCommand);

				hReturn = seiCommand.hInstApp;
			}
		}
	}

	return hReturn;
}


//
// LSExecute(HWND hOwner, LPCSTR pszCommand, int nShowCmd)
//
HINSTANCE LSExecute(HWND hOwner, LPCSTR pszCommand, int nShowCmd)
{
	char szCommand[MAX_LINE_LENGTH];
	char szExpandedCommand[MAX_LINE_LENGTH];
	LPCSTR pszArgs;
	HINSTANCE hResult = HINSTANCE(32);

	if (IsValidStringPtr(pszCommand))
	{
		VarExpansionEx(szExpandedCommand, pszCommand, MAX_LINE_LENGTH);

		if (GetToken(szExpandedCommand, szCommand, &pszArgs, true))
		{
			if (pszArgs > (szExpandedCommand + strlen(szExpandedCommand)))
			{
				pszArgs = NULL;
			}

			if (szCommand[0] == '!')
			{
				hResult = LSExecuteEx(hOwner, NULL, szCommand, pszArgs, NULL, 0);
			}
			else
			{
				char szDir[_MAX_DIR];
				char szFullDir[_MAX_DIR + _MAX_DRIVE];

				_splitpath(szCommand, szFullDir, szDir, NULL, NULL);
				StringCchCat(szFullDir, _MAX_DIR + _MAX_DRIVE, szDir);

				hResult = LSExecuteEx(hOwner, NULL, szCommand, pszArgs,
					szFullDir, nShowCmd ? nShowCmd : SW_SHOWNORMAL);
			}
		}
	}

	return hResult;
}

//
// SetDesktopArea(int left, int top, int right, int bottom)
//
void SetDesktopArea(int left, int top, int right, int bottom)
{
	RECT r = { left, top, right, bottom };

	SystemParametersInfo(SPI_SETWORKAREA, 0, (PVOID) & r, SPIF_SENDCHANGE);
	SystemParametersInfo(SPI_GETWORKAREA, 0, (PVOID) & r, SPIF_SENDCHANGE);
}


//
//	GetLitestepWnd()
//
HWND GetLitestepWnd()
{
	return g_LSAPIManager.GetLitestepWnd();
}


//
// void GetResStr(HINSTANCE hInstance, UINT uIDText, LPSTR pszText, size_t cchText, LPCSTR pszDefText)
//
void GetResStr(HINSTANCE hInstance, UINT uIDText, LPSTR pszText, size_t cchText, LPCSTR pszDefText)
{
	if (IsValidStringPtr(pszText, cchText))
	{
		if (LoadString(hInstance, uIDText, pszText, cchText) == 0)
		{
			StringCchCopy(pszText, cchText, pszDefText);
		}
	}
}


//
// GetResStrEx(HINSTANCE hInstance, UINT uIDText, LPSTR pszText, size_t cchText, LPCSTR pszDefText, ...)
//
void GetResStrEx(HINSTANCE hInstance, UINT uIDText, LPSTR pszText, size_t cchText, LPCSTR pszDefText, ...)
{
	char szFormat[MAX_LINE_LENGTH];
	va_list vargs;

	if (IsValidStringPtr(pszText, cchText))
	{
		GetResStr(hInstance, uIDText, szFormat, MAX_LINE_LENGTH, pszDefText);

		va_start(vargs, pszDefText);
		StringCchVPrintf(pszText, cchText, szFormat, vargs);
		va_end(vargs);
	}
}


//
// LSGetLitestepPath(LPSTR pszPath, size_t cchPath)
//
BOOL WINAPI LSGetLitestepPath(LPSTR pszPath, size_t cchPath)
{
    BOOL bReturn = FALSE; 

    if (IsValidStringPtr(pszPath, cchPath)) 
    { 
        // Default to user defined variable 
        if (GetRCString("litestepdir", pszPath, NULL, cchPath)) 
        { 
            bReturn = TRUE; 
        } 
    } 

    return bReturn; 
}


//
// LSGetImagePath(LPSTR pszPath, size_t cchPath)
//
BOOL WINAPI LSGetImagePath(LPSTR pszPath, size_t cchPath)
{
	BOOL bReturn = FALSE;

	if (IsValidStringPtr(pszPath, cchPath))
	{
		if (GetRCString("LSImageFolder", pszPath, NULL, cchPath))
		{
			bReturn = SUCCEEDED(PathAddBackslashEx(pszPath, cchPath));
		}
		else
		{
			if (LSGetLitestepPath(pszPath, cchPath))
			{
				StringCchCat(pszPath, cchPath, "images\\");
				bReturn = TRUE;
			}
		}
	}

	return bReturn;
}


static int _Tokenize(LPCSTR pszString, LPSTR* lpszBuffers, DWORD dwNumBuffers, LPSTR pszExtraParameters, BOOL bUseBrackets)
{
	char szBuffer[MAX_LINE_LENGTH];
	LPCSTR pszNextToken;
	DWORD dwTokens = 0;

	if (IsValidStringPtr(pszString))
	{
		pszNextToken = pszString;

		if ((lpszBuffers != NULL) && (dwNumBuffers > 0))
		{
			for (dwTokens = 0; pszNextToken && (dwTokens < dwNumBuffers); dwTokens++)
			{
				GetToken(pszNextToken, szBuffer, &pszNextToken, bUseBrackets);

				if (IsValidStringPtr(lpszBuffers[dwTokens]))
				{
					StringCchCopy(lpszBuffers[dwTokens], strlen(szBuffer) + 1, szBuffer);
				}
			}

			for (DWORD dwClearTokens = dwTokens; dwClearTokens < dwNumBuffers; dwClearTokens++)
			{
				if (IsValidStringPtr(lpszBuffers[dwClearTokens]))
				{
					lpszBuffers[dwClearTokens][0] = '\0';
				}
			}

			if (IsValidStringPtr(pszExtraParameters))
			{
				if (pszNextToken)
				{
					StringCchCopy(pszExtraParameters, strlen(pszNextToken) + 1, pszNextToken);
				}
				else
				{
					pszExtraParameters[0] = '\0';
				}
			}
		}
		else
		{
			while (GetToken(pszNextToken, NULL, &pszNextToken, bUseBrackets))
			{
				++dwTokens;
			}
		}
	}

	return dwTokens;
}

int LCTokenize(LPCSTR szString, LPSTR *lpszBuffers, DWORD dwNumBuffers, LPSTR szExtraParameters)
{
	return _Tokenize(szString, lpszBuffers, dwNumBuffers, szExtraParameters, FALSE);
}

BOOL GetToken(LPCSTR pszString, LPSTR pszToken, LPCSTR* pszNextToken, BOOL bUseBrackets)
{
	LPCSTR pszCurrent = pszString;
	LPCSTR pszStartMarker = NULL;
	int iBracketLevel = 0;
	CHAR cQuote = '\0';
	bool bIsToken = false;
	bool bAppendNextToken = false;

	if (pszString)
	{
		if (pszToken)
		{
			pszToken[0] = '\0';
		}

		if (pszNextToken)
		{
			*pszNextToken = NULL;
		}

		pszCurrent += strspn(pszCurrent, WHITESPACE);

		for (; *pszCurrent; pszCurrent++)
		{
			if (isspace((unsigned char)*pszCurrent) && !cQuote)
			{
				break;
			}

			if (bUseBrackets && strchr("[]", *pszCurrent) &&
				(!strchr("\'\"", cQuote) || !cQuote))
			{
				if (*pszCurrent == '[')
				{
					if (bIsToken && !cQuote)
					{
						break;
					}

					iBracketLevel++;
					cQuote = '[';
					
					if (iBracketLevel == 1)
					{
						continue;
					}
				}
				else
				{
					iBracketLevel--;
					
					if (iBracketLevel <= 0)
					{
						break;
					}
				}
			}

			if (strchr("\'\"", *pszCurrent) && (cQuote != '['))
			{
				if (!cQuote)
				{
					if (bIsToken)
					{
						bAppendNextToken = true;
						break;
					}

					cQuote = *pszCurrent;
					continue;
				}
				else if (*pszCurrent == cQuote)
				{
					break;
				}
			}

			if (!bIsToken)
			{
				bIsToken = true;
				pszStartMarker = pszCurrent;
			}
		}

		if (pszStartMarker && pszToken)
		{
			strncpy(pszToken, pszStartMarker, pszCurrent - pszStartMarker);
			pszToken[pszCurrent - pszStartMarker] = '\0';
		}


		if (!bAppendNextToken && *pszCurrent)
		{
			pszCurrent++;
		}

		pszCurrent += strspn(pszCurrent, WHITESPACE);

		if (*pszCurrent && pszNextToken)
		{
			*pszNextToken = pszCurrent;
		}

		if (bAppendNextToken && *pszCurrent)
		{
			LPSTR pszNewToken = pszToken;

			if (pszNewToken)
			{
				pszNewToken += strlen(pszToken);
			}

			GetToken(pszCurrent, pszNewToken, pszNextToken, bUseBrackets);
		}

		return pszStartMarker != NULL;
	}

	return FALSE;
}

int CommandTokenize(LPCSTR szString, LPSTR *lpszBuffers, DWORD dwNumBuffers, LPSTR szExtraParameters)
{
	return _Tokenize(szString, lpszBuffers, dwNumBuffers, szExtraParameters, TRUE);
}


//
// VarExpansion(LPSTR pszExpandedString, LPCSTR pszTemplate)
//
void VarExpansion(LPSTR pszExpandedString, LPCSTR pszTemplate)
{
	if (IsValidStringPtr(pszExpandedString) && IsValidStringPtr(pszTemplate))
	{
		char szTempBuffer[MAX_LINE_LENGTH];

		VarExpansionEx(szTempBuffer, pszTemplate, MAX_LINE_LENGTH);

		StringCchCopy(pszExpandedString, strlen(szTempBuffer) + 1, szTempBuffer);
	}
}


//
// VarExpansionEx(LPSTR pszExpandedString, LPCSTR pszTemplate, size_t cchLength)
//
// Taken in part from Raptor's rlib\rutil.cpp\UtilExpandString
//
void VarExpansionEx(LPSTR pszExpandedString, LPCSTR pszTemplate, size_t cchExpandedString)
{
	if (IsValidStringPtr(pszExpandedString, cchExpandedString) &&
	    IsValidStringPtr(pszTemplate))
	{
        if(g_LSAPIManager.IsInitialized()) 
        {
            g_LSAPIManager.GetSettingsManager()->VarExpansionEx(
                pszExpandedString, pszTemplate, cchExpandedString); 
		}
		else
		{
			StringCchCopy(pszExpandedString, cchExpandedString, pszTemplate);
		}
	}
}

//
// EnumLSData
//
// Return values:
//   E_INVALIDARG - Invalid value for uInfo
//   E_POINTER    - Invalid callback
//   E_FAIL       - Unspecified error
//   E_UNEXPECTED - Callback crashed or other catastrophic failure
//   S_OK         - Enumeration successful, callback always returned TRUE
//   S_FALSE      - Enumeration successful, but cancelled by callback
//
HRESULT EnumLSData(UINT uInfo, FARPROC pfnCallback, LPARAM lParam)
{
	HRESULT hr = E_INVALIDARG;

	if (NULL != pfnCallback)
	{
		switch (uInfo)
		{
			case ELD_BANGS:
			{
				hr = g_LSAPIManager.GetBangManager()->
					EnumBangs((LSENUMBANGSPROC)pfnCallback, lParam);
			}
			break;

			case ELD_REVIDS:
			{
				hr = (HRESULT)SendMessage(GetLitestepWnd(), LM_ENUMREVIDS,
					(WPARAM)pfnCallback, lParam);
			}
			break;

			case ELD_MODULES:
			{
				hr = (HRESULT)SendMessage(GetLitestepWnd(), LM_ENUMMODULES,
					(WPARAM)pfnCallback, lParam);
			}
			break;

			default:
			break;
		}
	}
	else
	{
		hr = E_POINTER;
	}

	return hr;
}
