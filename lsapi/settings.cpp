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
#include "settingsmanager.h"
#include "../utility/core.hpp"

extern SettingsManager* gSettingsManager;

FILE* LCOpen(LPCSTR pszPath)
{
	FILE * pFile = NULL;

	if (gSettingsManager)
	{
		if (pszPath)
		{
			if (pszPath[0] != '\0')
			{
				pFile = gSettingsManager->LCOpen(pszPath);
			}
		}
		if (pFile == NULL)
		{
			pFile = gSettingsManager->LCOpen(NULL);
		}
	}

	return pFile;
}


BOOL LCClose(FILE *pFile)
{
	BOOL bReturn = FALSE;

	if ((pFile != NULL) && (gSettingsManager))
	{
		bReturn = gSettingsManager->LCClose(pFile);
	}

	return bReturn;
}


BOOL LCReadNextCommand(FILE *pFile, LPSTR pszValue, size_t cchValue)
{
	BOOL bReturn = FALSE;

	if ((pFile != NULL) && IsValidStringPtr(pszValue, cchValue)
	        && (gSettingsManager))
	{
		bReturn = gSettingsManager->LCReadNextLineOrCommand(pFile, pszValue, cchValue);
	}

	return bReturn;
}


BOOL LCReadNextConfig(FILE *pFile, LPCSTR pszConfig, LPSTR pszValue, size_t cchValue)
{
	BOOL bReturn = FALSE;

	if ((pFile != NULL) && IsValidStringPtr(pszConfig) &&
	        IsValidStringPtr(pszValue, cchValue) && (gSettingsManager))
	{
		bReturn = gSettingsManager->LCReadNextConfig(pFile, pszConfig, pszValue, cchValue);
	}

	return bReturn;
}


BOOL LCReadNextLine(FILE *pFile, LPSTR pszValue, size_t cchValue)
{
	BOOL bReturn = FALSE;

	if ((pFile != NULL) && IsValidStringPtr(pszValue, cchValue)
	        && (gSettingsManager))
	{
		bReturn = gSettingsManager->LCReadNextLineOrCommand(pFile, pszValue, cchValue);
	}

	return bReturn;
}

int GetRCInt(LPCSTR szKeyName, int nDefault)
{
	if (gSettingsManager)
	{
		return gSettingsManager->GetRCInt(szKeyName, nDefault);
	}

	return nDefault;
}


BOOL GetRCBool(LPCSTR szKeyName, BOOL ifFound)
{
	if (gSettingsManager)
	{
		return gSettingsManager->GetRCBool(szKeyName, ifFound);
	}

	return !ifFound;
}


BOOL GetRCBoolDef(LPCSTR szKeyName, BOOL bDefault)
{
	if (gSettingsManager)
	{
		return gSettingsManager->GetRCBoolDef(szKeyName, bDefault);
	}

	return bDefault;
}


BOOL GetRCString(LPCSTR szKeyName, LPSTR szValue, LPCSTR defStr, int maxLen)
{
	if (gSettingsManager)
	{
		return gSettingsManager->GetRCString(szKeyName, szValue, defStr, maxLen);
	}
	else if (szValue && defStr)
	{
		strncpy(szValue, defStr, maxLen);
	}
	return FALSE;
}


COLORREF GetRCColor(LPCSTR szKeyName, COLORREF colDef)
{
	if (gSettingsManager)
	{
		return gSettingsManager->GetRCColor(szKeyName, colDef);
	}

	return colDef;
}


BOOL GetRCLine(LPCSTR szKeyName, LPSTR szBuffer, UINT nBufLen, LPCSTR szDefault)
{
	if (gSettingsManager)
	{
		return gSettingsManager->GetRCLine(szKeyName, szBuffer, nBufLen, szDefault);
	}
	else if(szBuffer && szDefault)
	{
		strncpy(szBuffer, szDefault, nBufLen);
	}
	return FALSE;
}

BOOL LSGetVariableEx(LPCSTR pszKeyName, LPSTR pszValue, DWORD dwLength)
{
	if (gSettingsManager)
	{
		return gSettingsManager->GetVariable(pszKeyName, pszValue, dwLength);
	}

	return FALSE;
}

BOOL LSGetVariable(LPCSTR pszKeyName, LPSTR pszValue)
{
	BOOL bReturn = FALSE;
	char szTempValue[MAX_LINE_LENGTH];

	if (pszValue)
	{
		pszValue[0] = '\0';
	}

	if (gSettingsManager)
	{
		bReturn = gSettingsManager->GetVariable(pszKeyName, szTempValue, MAX_LINE_LENGTH);

		if (bReturn && pszValue)
		{
			StringCchCopy(pszValue, strlen(szTempValue) + 1, szTempValue);
		}
	}

	return bReturn;
}


void LSSetVariable(LPCSTR pszKeyName, LPCSTR pszValue)
{
	if (gSettingsManager)
	{
		gSettingsManager->SetVariable(pszKeyName, pszValue);
	}
}
