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
#include "SettingsFileParser.h"
#include "lsapi.h"
#define _UNICODE
#include <tchar.h>
#undef _UNICODE
#include "../utility/safestr.h" // Always include last in cpp file

BOOL FileParser::_ReadLineFromFile(LPSTR pszName, LPSTR pszValue)
{
	WCHAR wzBuffer[MAX_LINE_LENGTH + 1];
	BOOL bReturn = FALSE;

	if ((m_phFile) && IsValidStringPtr(pszName))
	{
		while (!feof(m_phFile))
		{
			if (!fgetws(wzBuffer, MAX_LINE_LENGTH, m_phFile))
			{
				break;
			}

			if (wzBuffer[0] != L';')
			{
				LPWSTR pwzCurrent = wzBuffer;
				pwzCurrent += wcsspn(pwzCurrent, WHITESPACEW);

				if (pwzCurrent[0] && pwzCurrent[0] != L';')
				{
					WCHAR wzName[MAX_RCCOMMAND + 1];
					size_t nEndConfig = wcscspn(pwzCurrent, WHITESPACEW);

					wcsncpy(wzName, pwzCurrent, nEndConfig);
					wzName[nEndConfig] = '\0';
					CharLowerBuffW(wzName, MAX_RCCOMMAND);

					WideCharToMultiByte(CP_ACP, 0, wzName, -1, pszName, MAX_RCCOMMAND, NULL, NULL);

					if (IsValidStringPtr(pszValue))
					{
						LPWSTR pwzValueStart;

						if (nEndConfig < wcslen(pwzCurrent))
						{
							pwzValueStart = pwzCurrent + nEndConfig + 1;
						}
						else
						{
							pwzValueStart = pwzCurrent + nEndConfig;
						}

						_StripString(pwzValueStart);

						WideCharToMultiByte(CP_ACP, 0, pwzValueStart, -1, pszValue, MAX_LINE_LENGTH, NULL, NULL);
					}

					bReturn = TRUE;
					break;
				}
			}
		}
	}

	return bReturn;
}

FileParser::FileParser(SettingsMap* pSettingsMap)
{
	m_pSettingsMap = NULL;
	m_phFile = NULL;
	m_pEvalParser = NULL;

	if (IsValidReadPtr(pSettingsMap))
	{
		m_pSettingsMap = pSettingsMap;
	}
}

FileParser::~FileParser()
{
	if (m_pEvalParser != NULL)
	{
		delete m_pEvalParser;
	}
}

void FileParser::ParseFile(LPCSTR pszFileName)
{
	CHAR szExpandedPath[MAX_PATH_LENGTH + 1];
	CHAR szFullPath[MAX_PATH_LENGTH + 1];
	CHAR szKey[MAX_RCCOMMAND + 1];
	CHAR szValue[MAX_LINE_LENGTH + 1];

	VarExpansionEx(szExpandedPath, pszFileName, MAX_PATH_LENGTH);
	/*
	if (OsVersionInfo.dwPlatformId == VER_PLATFORM_WIN32_NT)
	{
		GetFullPathNameW(wzExpandedPath, MAX_PATH_LENGTH, wzFullPath, NULL);
	}
	else
	{
		char szPath[MAX_PATH_LENGTH];
		char szFullPath[MAX_PATH_LENGTH];
		size_t nLength = wcslen(wzExpandedPath) + 1;
		WideCharToMultiByte(CP_ACP, 0, wzExpandedPath, nLength, szPath, nLength, NULL, NULL);
		*/
	GetFullPathName(szExpandedPath, MAX_PATH_LENGTH, szFullPath, NULL);
	/*
	MultiByteToWideChar(CP_ACP, 0, szFullPath, nLength, wzFullPath, nLength);
	}*/

	m_phFile = fopen(szFullPath, "r");

	if (m_phFile)
	{
		fseek(m_phFile, 0, SEEK_SET);

		while (_ReadLineFromFile(szKey, szValue))
		{
			_ProcessLine(szKey, szValue);
		}

		fclose(m_phFile);
	}
}


void FileParser::_StripString(LPWSTR pwzString)
{
	LPWSTR pwzCurrent = pwzString;
	LPWSTR pwzStart = NULL;
	LPWSTR pwzLast = NULL;
	int nQuoteLevel = 0;
	WCHAR wLastQuote = 0;

	while (*pwzCurrent != L'\0')
	{
		if (wcschr(WHITESPACEW, *pwzCurrent) == NULL)
		{
			if (pwzStart == NULL)
			{
				pwzStart = pwzCurrent;
			}
			pwzLast = NULL;
		}
		else
		{
			if (pwzLast == NULL)
			{
				pwzLast = pwzCurrent;
			}
		}
		if (pwzStart != NULL)
		{
			if (*pwzCurrent == L'[')
			{
				++nQuoteLevel;
			}
			else if (*pwzCurrent == L']')
			{
				nQuoteLevel--;
			}
			else if ((*pwzCurrent == L'"') || (*pwzCurrent == L'\''))
			{
				if (wLastQuote == *pwzCurrent)
				{
					nQuoteLevel--;
					wLastQuote = 0;
				}
				else if (!wLastQuote)
				{
					nQuoteLevel++;
					wLastQuote = *pwzCurrent;
				}
			}
			else if (*pwzCurrent == L';')
			{
				if (!nQuoteLevel)
				{
					pwzLast = pwzCurrent;
					break;
				}
			}
		}
		++pwzCurrent;
	}
	if (pwzLast != NULL)
	{
		*pwzLast = '\0';
	}
	if ((pwzCurrent != pwzString) && *pwzCurrent)
	{
		StringCchCopyW(pwzString, wcslen(pwzString) + 1, pwzStart);
	}
}


void FileParser::_ProcessLine(LPCSTR pszName, LPCSTR pszValue)
{
	if (strcmp(pszName, "if") == 0)
	{
		_ProcessIf(pszValue);
	}
	else if (strcmp(pszName, "include") == 0)
	{
		CHAR szPath[MAX_PATH_LENGTH];
		GetToken(pszValue, szPath, NULL, false);
		//ParseFile(szPath);
		FileParser fpParser(m_pSettingsMap);
		fpParser.ParseFile(szPath);
	}
	else
	{
		m_pSettingsMap->insert(SettingsMap::value_type(pszName, pszValue));
	}
}


void FileParser::_ProcessIf(LPCSTR pszExpression)
{
	CHAR szName[MAX_RCCOMMAND + 1];
	CHAR szValue[MAX_LINE_LENGTH + 1];
	BOOL bResult;

	if (m_pEvalParser == NULL)
	{
		m_pEvalParser = new EvalParser();
	}

	if (m_pEvalParser)
	{

		if (!m_pEvalParser->evaluate(pszExpression, &bResult))
		{
			CHAR szError[MAX_LINE_LENGTH];

			StringCchPrintf(szError, MAX_LINE_LENGTH,
			                "Syntax error in If expression:\n%s",
			                pszExpression);

			MessageBox(NULL, szError, "LiteStep", MB_SETFOREGROUND);
		}
		else
		{
			if (bResult)
			{
				// when the If condition evaluates true we read and process
				// all lines until we reach ElseIf, Else, or EndIf
				while (_ReadLineFromFile(szName, szValue))
				{
					if ((strcmp(szName, "else") == 0) || (strcmp(szName, "elseif") == 0))
					{
						// if we find an ElseIf or Else then we read and ignore
						// all lines until we find EndIf
						_SkipIf();
						break;
					}
					else if (strcmp(szName, "endif") == 0)
					{
						// we're done
						break;
					}
					else
					{
						// just a normal line, process it
						_ProcessLine(szName, szValue);
					}
				}
			}
			else
			{
				// when the If expression evaluates false we read and ignore
				// all lines until we find an ElseIf, Else, or EndIf
				while (_ReadLineFromFile(szName, szValue))
				{
					if (strcmp(szName, "if") == 0)
					{
						// nested Ifs are a special case
						_SkipIf();
					}
					else if (strcmp(szName, "elseif") == 0)
					{
						// we handle ElseIfs by recursively calling ProcessIf
						_ProcessIf(szValue);
						break;
					}
					else if (strcmp(szName, "else") == 0)
					{
						// since the If expression was false, when we see Else
						// we start process lines until EndIf
						while (_ReadLineFromFile(szName, szValue))
						{
							// we should probably check that there are no ElseIfs
							// after the Else, but for now we silently ignore
							// such an error

							// break on EndIf
							if (strcmp(szName, "endif") == 0)
								break;

							// otherwise process the line
							_ProcessLine(szName, szValue);
						}

						// we're done
						break;
					}
					else if (strcmp(szName, "endif") == 0)
					{
						// we're done
						break;
					}
				}
			}
		}
	}
}

void FileParser::_SkipIf()
{
	CHAR szName[MAX_RCCOMMAND];

	while (_ReadLineFromFile(szName, NULL))
	{
		if (strcmp(szName, "if") == 0)
		{
			_SkipIf();
		}
		else if (strcmp(szName, "endif") == 0)
		{
			break;
		}
	}
}
