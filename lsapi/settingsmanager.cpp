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
#include "SettingsManager.h"
#include "SettingsFileParser.h"
#include "../utility/shellhlp.h"
#include "../utility/core.hpp"

#if !defined(CSIDL_COMMON_ADMINTOOLS)
#  define CSIDL_COMMON_ADMINTOOLS 0x002F
#  define CSIDL_ADMINTOOLS 0x0030
#endif

SettingsManager::SettingsManager(LPCSTR pszLiteStepPath)
{
    _SetupVars(pszLiteStepPath);
}


SettingsManager::~SettingsManager()
{
    m_SettingsMap.clear();
}


BOOL SettingsManager::_SetShellFolderVariable(LPCSTR pszVariable, int nFolder)
{
    char szPath[MAX_PATH];

    BOOL bReturn = GetShellFolderPath(nFolder, szPath, MAX_PATH);
    
    if (bReturn)
    {
        PathQuoteSpaces(szPath);
        SetVariable(pszVariable, szPath);
    }

    return bReturn;    
}


void SettingsManager::_SetupVars(LPCSTR pszLiteStepPath)
{
	char szTemp[MAX_PATH];
	DWORD dwLength = MAX_PATH;
	OSVERSIONINFO OsVersionInfo;

	StringCchCopy(szTemp, MAX_PATH, pszLiteStepPath);
    PathQuoteSpaces(szTemp);
    SetVariable("litestepdir", szTemp);
	
    if (GetWindowsDirectory(szTemp, MAX_PATH))
    {
        PathAddBackslash(szTemp);
        SetVariable("windir", szTemp);
    }

	if (GetUserName(szTemp, &dwLength))
    {
        PathQuoteSpaces(szTemp);
        SetVariable("username", szTemp);
    }

    if (GetShellFolderPath(CSIDL_APPDATA, szTemp, MAX_PATH))
    {
        StringCchCat(szTemp, MAX_PATH,
            "Microsoft\\Internet Explorer\\Quick Launch\\");
        
        PathQuoteSpaces(szTemp);
        SetVariable("quicklaunch", szTemp);
    }

    SetVariable("bitbucket", "::{645FF040-5081-101B-9F08-00AA002F954E}");
	SetVariable("documents", "::{450D8FBA-AD25-11D0-98A8-0800361B1103}");
	SetVariable("drives", "::{20D04FE0-3AEA-1069-A2D8-08002B30309D}");
	SetVariable("network", "::{208D2C60-3AEA-1069-A2D7-08002B30309D}");
	SetVariable("controls", "::{20D04FE0-3AEA-1069-A2D8-08002B30309D}\\::{21EC2020-3AEA-1069-A2DD-08002B30309D}");
	SetVariable("dialup", "::{20D04FE0-3AEA-1069-A2D8-08002B30309D}\\::{992CFFA0-F557-101A-88EC-00DD010CCC48}");
	SetVariable("networkanddialup", "::{20D04FE0-3AEA-1069-A2D8-08002B30309D}\\::{7007ACC7-3202-11D1-AAD2-00805FC1270E}");
	SetVariable("printers", "::{20D04FE0-3AEA-1069-A2D8-08002B30309D}\\::{2227A280-3AEA-1069-A2DE-08002B30309D}");
	SetVariable("scheduled", "::{20D04FE0-3AEA-1069-A2D8-08002B30309D}\\::{D6277990-4C6A-11CF-8D87-00AA0060F5BF}");
	SetVariable("admintools", "::{20D04FE0-3AEA-1069-A2D8-08002B30309D}\\::{21EC2020-3AEA-1069-A2DD-08002B30309D}\\::{D20EA4E1-3957-11d2-A40B-0C5020524153}");

    _SetShellFolderVariable("commondesktopdir", CSIDL_COMMON_DESKTOPDIRECTORY);
    _SetShellFolderVariable("commonfavorites", CSIDL_COMMON_FAVORITES);
    _SetShellFolderVariable("commonprograms", CSIDL_COMMON_PROGRAMS);
    _SetShellFolderVariable("commonstartmenu", CSIDL_COMMON_STARTMENU);
    _SetShellFolderVariable("commonstartup", CSIDL_COMMON_STARTUP);
    _SetShellFolderVariable("cookies", CSIDL_COOKIES);
    _SetShellFolderVariable("desktop", CSIDL_DESKTOP);
    _SetShellFolderVariable("desktopdir", CSIDL_DESKTOPDIRECTORY);
    _SetShellFolderVariable("favorites", CSIDL_FAVORITES);
    _SetShellFolderVariable("fonts", CSIDL_FONTS);
    _SetShellFolderVariable("history", CSIDL_HISTORY);
    _SetShellFolderVariable("internet", CSIDL_INTERNET);
    _SetShellFolderVariable("internetcache", CSIDL_INTERNET_CACHE);
    _SetShellFolderVariable("nethood", CSIDL_NETHOOD);
    _SetShellFolderVariable("documentsdir", CSIDL_PERSONAL);
    _SetShellFolderVariable("printhood", CSIDL_PRINTHOOD);
    _SetShellFolderVariable("programs", CSIDL_PROGRAMS);
    _SetShellFolderVariable("recent", CSIDL_RECENT);
    _SetShellFolderVariable("sendto", CSIDL_SENDTO);
    _SetShellFolderVariable("startmenu", CSIDL_STARTMENU);
    _SetShellFolderVariable("startup", CSIDL_STARTUP);
    _SetShellFolderVariable("templates", CSIDL_TEMPLATES);
    _SetShellFolderVariable("commonadmintoolsdir", CSIDL_COMMON_ADMINTOOLS);
    _SetShellFolderVariable("admintoolsdir", CSIDL_ADMINTOOLS);

	OsVersionInfo.dwOSVersionInfoSize = sizeof(OsVersionInfo);
	GetVersionEx(&OsVersionInfo);

	if (OsVersionInfo.dwPlatformId == VER_PLATFORM_WIN32_WINDOWS)
	{
        // Any Win9x-series OS
        SetVariable("Win9x", "true");

        if (OsVersionInfo.dwMinorVersion >= 90) // Windows ME (4.90)
			SetVariable("WinME", "true");
		else if (OsVersionInfo.dwMinorVersion >= 10) // Windows 98 (4.10)
			SetVariable("Win98", "true");
		else // Windows 95 (4.00)
			SetVariable("Win95", "true");
	}
	else if (OsVersionInfo.dwPlatformId == VER_PLATFORM_WIN32_NT)
	{
        // Any WinNT-series OS
        SetVariable("WinNT", "true");

        if (OsVersionInfo.dwMajorVersion == 5)
        {
            if (OsVersionInfo.dwMinorVersion >= 1)
            {
                SetVariable("WinXP", "true");       // Windows XP (5.1)
            }
            else
            {
                SetVariable("Win2000", "true");     // Windows 2000 (5.0)
            }
        }
		else if (OsVersionInfo.dwMajorVersion >= 4) // Windows NT 4.0
        {
            SetVariable("WinNT4", "true");
        }
	}

	// screen resolution
	StringCchPrintf(szTemp, MAX_PATH, "%d", GetSystemMetrics(SM_CXSCREEN));
	SetVariable("ResolutionX", szTemp);

	StringCchPrintf(szTemp, MAX_PATH, "%d", GetSystemMetrics(SM_CYSCREEN));
	SetVariable("ResolutionY", szTemp);

    StringCchPrintf(szTemp, MAX_PATH, "\"%s\"", __DATE__);
    SetVariable("CompileDate", szTemp);
}

void SettingsManager::ParseFile(LPCSTR pszFileName)
{
	FileParser fpParser(&m_SettingsMap);
	fpParser.ParseFile(pszFileName);
}

BOOL SettingsManager::_FindLine(LPCSTR pszName, SettingsMap::iterator &it)
{
	BOOL bReturn = FALSE;
	
    // first appearance of a setting takes effect
    it = m_SettingsMap.lower_bound(pszName);
    
    if (it != m_SettingsMap.end() && stricmp(pszName, it->first.c_str()) == 0)
	{
		bReturn = TRUE;
	}

	return bReturn;
}


BOOL SettingsManager::GetRCString(LPCSTR pszKeyName, LPSTR pszValue, LPCSTR pszDefStr, int nMaxLen)
{
	SettingsMap::iterator it;
	BOOL bReturn = FALSE;

	pszValue[0] = '\0';
	if (_FindLine(pszKeyName, it))
	{
		CHAR szToken[MAX_LINE_LENGTH + 1];
		GetToken(it->second.c_str(), szToken, NULL, FALSE);
		VarExpansionEx(pszValue, szToken, nMaxLen);
		bReturn = TRUE;
	}
	else if (pszDefStr)
	{
		StringCchCopy(pszValue, nMaxLen, pszDefStr);
		// for compatibility reasons GetRCString and GetRCLine return FALSE
        // if the default value is returned.
        //bReturn = TRUE;
	}

	return bReturn;
}


BOOL SettingsManager::GetRCLine(LPCSTR pszKeyName, LPSTR pszValue, int nMaxLen, LPCSTR pszDefStr)
{
	SettingsMap::iterator it;
	BOOL bReturn = FALSE;

	pszValue[0] = '\0';
	if (_FindLine(pszKeyName, it))
	{
		// for compatibility reasons GetRCLine expands $evars$
        VarExpansionEx(pszValue, it->second.c_str(), nMaxLen);
        //StringCchCopy(pszValue, nMaxLen, it->second.c_str());
		bReturn = TRUE;
	}
	else if (pszDefStr)
	{
		StringCchCopy(pszValue, nMaxLen, pszDefStr);

        // for compatibility reasons GetRCString and GetRCLine return FALSE
        // if the default value is returned.
        //bReturn = TRUE;
	}
	return bReturn;
}


BOOL SettingsManager::GetRCBool(LPCSTR pszKeyName, BOOL bIfFound)
{
	SettingsMap::iterator it;
	CHAR szToken[MAX_LINE_LENGTH + 1];
	CHAR szExpanded[MAX_LINE_LENGTH + 1];

	if (_FindLine(pszKeyName, it))
	{
		VarExpansionEx(szExpanded, it->second.c_str(), MAX_LINE_LENGTH);
		if (GetToken(szExpanded, szToken, NULL, false))
		{
			if (stricmp(szToken, "off") && stricmp(szToken, "false") && stricmp(szToken, "no"))
			{
				return bIfFound;
			}
		}
		else
		{
			return bIfFound;
		}
	}

	return !bIfFound;
}


BOOL SettingsManager::GetRCBoolDef(LPCSTR pszKeyName, BOOL bDefault)
{
	SettingsMap::iterator it;
	CHAR szToken[MAX_LINE_LENGTH];
	CHAR szExpanded[MAX_LINE_LENGTH + 1];

	if (_FindLine(pszKeyName, it))
	{
		VarExpansionEx(szExpanded, it->second.c_str(), MAX_LINE_LENGTH);
		if (GetToken(szExpanded, szToken, NULL, false))
		{
			if ((stricmp(szToken, "off") == 0) ||
			        (stricmp(szToken, "false") == 0) ||
			        (stricmp(szToken, "no") == 0))
			{
				return FALSE;
			}
		}
		return TRUE;
	}

	return bDefault;
}


int SettingsManager::GetRCInt(LPCSTR pszKeyName, int nDefault)
{
	SettingsMap::iterator it;
	CHAR szToken[MAX_LINE_LENGTH + 1];
	CHAR szExpanded[MAX_LINE_LENGTH + 1];
	int nValue = nDefault;

	if (_FindLine(pszKeyName, it))
	{
		VarExpansionEx(szExpanded, it->second.c_str(), MAX_LINE_LENGTH);
		if (GetToken(szExpanded, szToken, NULL, false))
		{
			nValue = strtol(szToken, NULL, 0);
		}
	}

	return nValue;
}


COLORREF SettingsManager::GetRCColor(LPCSTR pszKeyName, COLORREF crDefault)
{
	COLORREF crReturn = crDefault;
	SettingsMap::iterator it;

	if (_FindLine(pszKeyName, it))
	{
		char szBuffer[MAX_LINE_LENGTH];
        char szFirst[MAX_LINE_LENGTH];
		char szSecond[MAX_LINE_LENGTH];
		char szThird[MAX_LINE_LENGTH];

		LPSTR lpszTokens[3] = { szFirst, szSecond, szThird	};

        VarExpansionEx(szBuffer, it->second.c_str(), MAX_LINE_LENGTH);
        int nCount = LCTokenize(szBuffer, lpszTokens, 3, NULL);

		if (nCount >= 3)
		{
			int nRed, nGreen, nBlue;

			nRed = strtol(szFirst, NULL, 10);
			nGreen = strtol(szSecond, NULL, 10);
			nBlue = strtol(szThird, NULL, 10);

			crReturn = RGB(nRed, nGreen, nBlue);
		}
		else if (nCount >= 1)
		{
			crReturn = strtol(szFirst, NULL, 16);
            // convert from BGR to RGB
            crReturn = RGB(GetBValue(crReturn), GetGValue(crReturn),
                           GetRValue(crReturn));
		}
	}

	return crReturn;
}

BOOL SettingsManager::GetVariable(LPCSTR pszKeyName, LPSTR pszValue, DWORD dwLength)
{
	// using GetRCString instead of GetRCLine here, again for compatibility reasons.
    // as a side effect this strips any "" quotes around the variable's value.
    return GetRCString(pszKeyName, pszValue, NULL, dwLength);
}


void SettingsManager::SetVariable(LPCSTR pszKeyName, LPCSTR pszValue)
{
    if ((pszKeyName) && (pszValue))
    {
        // in order for LSSetVariable to work evars must be redefinable

        // m_SettingsMap.insert(SettingsMap::value_type(pszKeyName, pszValue));
        SettingsMap::iterator it = m_SettingsMap.lower_bound(pszKeyName);
        
        if (it != m_SettingsMap.end() &&
            stricmp(pszKeyName, it->first.c_str()) == 0)
        {
            it->second.assign(pszValue);
        }
        else
        {
            m_SettingsMap.insert(SettingsMap::value_type(pszKeyName, pszValue));
        }
    }
}


void SettingsManager::VarExpansionEx(LPSTR pszExpandedString, LPCSTR pszTemplate, DWORD dwLength)
{
	LPCSTR pszVariable;
	CHAR szVariable[MAX_LINE_LENGTH + 1];
    char szTempExpandedString[MAX_LINE_LENGTH] = { 0 };
	LPSTR pszTempExpandedString = szTempExpandedString;
	DWORD dwWorkLength = dwLength;

	if ((pszTemplate != NULL) && (pszExpandedString != NULL) && (dwWorkLength > 0))
	{
		//szTempExpandedString[0] = '\0';

		while ((*pszTemplate != '\0') && (dwWorkLength > 0))
		{
			if (*pszTemplate != '$')
			{
				*pszTempExpandedString = *pszTemplate;
				pszTemplate++;
				pszTempExpandedString++;
				dwWorkLength--;
			}
			else
			{
				//
				// This is a variable so we need to find the end of it:
				//
                pszTemplate++;
                pszVariable = pszTemplate;
                while ((*pszTemplate != '$') && (*pszTemplate != '\0'))
                {
                    pszTemplate++;
                }

				bool bSucceeded = false;

				if (*pszTemplate == '\0')
				{
					strncpy(pszTempExpandedString, pszVariable, pszTemplate - pszVariable);
					bSucceeded = true;
				}
				else
				{
					//
					// We've found the end of the variable so copy it
					// someplace usefull:
					//
					strncpy(szVariable, pszVariable, pszTemplate - pszVariable);
                    szVariable[pszTemplate - pszVariable] = '\0';
                    if (szVariable[0] != '\0')
                    {
                        //
                        // Get the value, if we can.
                        //
                        SettingsMap::iterator it;
                        if (_FindLine(szVariable, it))
                        {
                            GetToken(it->second.c_str(), pszTempExpandedString, NULL, FALSE);
                            bSucceeded = true;
                        }
                        else if (GetEnvironmentVariable(szVariable, pszTempExpandedString, dwLength))
                        {
                            bSucceeded = true;
                        }
                        else
                        {
#ifdef LS_COMPAT_MATH
                            int nValue = static_cast<int>(_MathEvaluate(szVariable));
                            
                            if (nValue != INT_MAX)
                            {
                                StringCchPrintf(pszTempExpandedString, dwLength,
                                    "%d", nValue);
                                
                                bSucceeded = true;
                            }
                            else
                            {
                                pszTempExpandedString[0] = '\0';
                            }
#else
                            pszTempExpandedString[0] = '\0';
#endif // LS_COMPAT_MATH
                        }
					}
				}
				//
				// If we succeeded, adjust our output buffers
				// accordingly:
				//
				if (bSucceeded)
				{
					dwWorkLength -= strlen(pszTempExpandedString);
					pszTempExpandedString += strlen(pszTempExpandedString);
				}

				//
				// Move to the next character if we didn't run out of space:
				//
				if (*pszTemplate != '\0')
				{
					pszTemplate++;
				}
			}
		}
		*pszTempExpandedString = '\0';

		if (strchr(szTempExpandedString, '$'))
		{
			VarExpansionEx(pszExpandedString, szTempExpandedString, dwLength);
		}
		else
		{
			StringCchCopy(pszExpandedString, dwLength, szTempExpandedString);
		}
	}
}


//
// FILE* LCOpen(LPCSTR pszPath)
//
FILE* SettingsManager::LCOpen(LPCSTR pszPath)
{
	FILE * pFile = NULL;

	if (pszPath == NULL)
	{
		SettingsIterator * psiNew = new SettingsIterator(&m_SettingsMap, "\0");
		if (psiNew)
		{
			m_Iterators.insert(psiNew);
			pFile = (FILE*)psiNew;
		}
	}
	else
	{
		char szPath[MAX_PATH];
		VarExpansionEx(szPath, pszPath, MAX_PATH);
		FileMap::iterator it = m_FileMap.find(szPath);
		if (it == m_FileMap.end())
		{
			FileInfo * pFileInfo = new FileInfo;
			if (pFileInfo)
			{
                pFileInfo->m_pSettingsMap = new SettingsMap;
				pFileInfo->m_Count = 1;

                FileParser fpParser(pFileInfo->m_pSettingsMap);
				fpParser.ParseFile(szPath);

				m_FileMap.insert(FileMap::value_type(szPath, pFileInfo));
				it = m_FileMap.find(szPath);
			}
		}
		if (it != m_FileMap.end())
		{
			SettingsIterator * psiNew = new SettingsIterator(it->second->m_pSettingsMap, szPath);
			if (psiNew)
			{
				it->second->m_Count++;
				m_Iterators.insert(psiNew);
				pFile = (FILE*)psiNew;
			}
		}
	}

	return pFile;
}


//
// BOOL LCClose(FILE* pFile)
//
BOOL SettingsManager::LCClose(FILE* pFile)
{
	BOOL bReturn = FALSE;

	if (pFile != NULL)
	{
		IteratorSet::iterator it = m_Iterators.find((SettingsIterator*)pFile);
		if (it != m_Iterators.end())
		{
			m_Iterators.erase(it);
			FileMap::iterator fmIt = m_FileMap.find(((SettingsIterator*)pFile)->get_Path());
			if (fmIt != m_FileMap.end())
			{
				if (fmIt->second->m_Count == 1)
				{
                    delete fmIt->second->m_pSettingsMap;
					delete fmIt->second;
				}
				else
				{
					fmIt->second->m_Count--;
				}
			}
			delete pFile;
		}
		bReturn = TRUE;
	}

	return bReturn;
}


//
// BOOL LCReadNextConfig(FILE *pFile, LPCSTR pszConfig, LPSTR pszValue, size_t cchValue)
//
BOOL SettingsManager::LCReadNextConfig(FILE *pFile, LPCSTR pszConfig, LPSTR pszValue, size_t cchValue)
{
	BOOL bReturn = FALSE;
	char szTempValue[MAX_LINE_LENGTH];

	if ((pFile != NULL) && IsValidStringPtr(pszConfig) && IsValidStringPtr(pszValue, cchValue))
	{
		IteratorSet::iterator it = m_Iterators.find((SettingsIterator*)pFile);
		if (it != m_Iterators.end())
		{
			bReturn = (*it)->ReadNextConfig(pszConfig, szTempValue, MAX_LINE_LENGTH);
			if (bReturn)
			{
				VarExpansionEx(pszValue, szTempValue, cchValue);
			}
		}
	}

	return bReturn;
}


//
// BOOL LCReadNextLineOrCommand(FILE *pFile, LPSTR pszValue, size_t cchValue)
//
BOOL SettingsManager::LCReadNextLineOrCommand(FILE *pFile, LPSTR pszValue, size_t cchValue)
{
	BOOL bReturn = FALSE;
	char szTempValue[MAX_LINE_LENGTH];

	if ((pFile != NULL) && IsValidStringPtr(pszValue, cchValue))
	{
		IteratorSet::iterator it = m_Iterators.find((SettingsIterator*)pFile);
		if (it != m_Iterators.end())
		{
			bReturn = (*it)->ReadNextLine(szTempValue, MAX_LINE_LENGTH);
			if (bReturn)
			{
				VarExpansionEx(pszValue, szTempValue, cchValue);
			}
		}
	}

	return bReturn;
}

#ifdef LS_COMPAT_MATH
//
// double _MathEvaluate(LPCTSTR ptzInput)
//
// we're here to be compatible, not to be efficient or pretty.
// leave it alone as long as it works exactly like the old function.
double SettingsManager::_MathEvaluate(LPTSTR ptzInput)
{
    int nCount, nBlock = 0, nStart;
    
    PathRemoveBlanks(ptzInput);
    int nLen = _tcslen(ptzInput);

    for (nCount = 0; nCount <= nLen; nCount++)
    {
        if ((ptzInput[nCount] == _T('(')) && (++nBlock == 1))
        {
            nStart = nCount;
        }
        else if ((ptzInput[nCount] == _T(')')) && !--nBlock && !nStart &&
                 (nCount == (nLen - 1)))
        {
            // one pair of parentheses surrounding the whole expression		
            ptzInput[nCount] = 0;
            return _MathEvaluate(ptzInput + 1);
        }
    }

    char rcSign[4] = { _T('+'), _T('-'), _T('*'), _T('/') };

	for(int nSign = 0; nSign < 4; nSign++)
    {
        for(nCount = nLen; nCount >= 0; nCount--)
        {
            if(ptzInput[nCount] == _T('('))
            {
                ++nBlock;
            }
            else if(ptzInput[nCount] == _T(')'))
            {
                --nBlock;
            }
            else if((ptzInput[nCount] == rcSign[nSign]) && !nBlock)
            {
                ptzInput[nCount] = 0;
                
                switch(nSign)
                {
                    case 0: return _MathEvaluate(ptzInput) +
                                _MathEvaluate(ptzInput + nCount + 1);

                    case 1: return _MathEvaluate(ptzInput) -
                                _MathEvaluate(ptzInput + nCount + 1);
                        
                    case 2: return _MathEvaluate(ptzInput) *
                                _MathEvaluate(ptzInput + nCount + 1);
                        
                    case 3: return _MathEvaluate(ptzInput) /
                                _MathEvaluate(ptzInput + nCount + 1);
                }
            }
        }
    }
    
    TCHAR tzValue[MAX_LINE_LENGTH];
    if(GetVariable(ptzInput, tzValue, MAX_LINE_LENGTH))
    {
        double nTemp = _tcstod(tzValue, NULL);
        return nTemp;
    }
    else
    {
        return _tcstod(ptzInput, NULL);
    }
}
#endif // LS_COMPAT_MATH