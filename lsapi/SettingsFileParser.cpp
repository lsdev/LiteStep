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
#include "SettingsFileParser.h"
#include "lsapi.h"
#include "../utility/safestr.h" // Always include last in cpp file


//
// BOOL _ReadLineFromFile(LPTSTR ptzName, LPTSTR ptzValue)
//
// ptzName must be MAX_RCOCOMMAND size
// ptzValue must be MAX_LINE_LENGTH size
//
BOOL FileParser::_ReadLineFromFile(LPTSTR ptzName, LPTSTR ptzValue)
{
	WCHAR wzBuffer[MAX_LINE_LENGTH];
	BOOL bReturn = FALSE;

	if ((m_phFile) && IsValidStringPtr(ptzName, MAX_RCCOMMAND))
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
				pwzCurrent += StrSpnW(pwzCurrent, WHITESPACEW);

				if (pwzCurrent[0] && pwzCurrent[0] != L';')
				{
					int nEndConfig = wcscspn(pwzCurrent, WHITESPACEW);

#ifdef _UNICODE
					StringCchCopyNExW(ptzName, MAX_RCCOMMAND, pwzCurrent, nEndConfig, NULL, NULL, STRSAFE_NULL_ON_FAILURE);
#else // _UNICODE
					WCHAR wzName[MAX_RCCOMMAND];
					StringCchCopyNExW(wzName, MAX_RCCOMMAND, pwzCurrent, nEndConfig, NULL, NULL, STRSAFE_NULL_ON_FAILURE);

					WideCharToMultiByte(CP_ACP, 0, wzName, -1, ptzName, MAX_RCCOMMAND, NULL, NULL);
#endif // _UNICODE

					if (IsValidStringPtr(ptzValue))
					{
						LPWSTR pwzValueStart;

						if (nEndConfig < lstrlenW(pwzCurrent))
						{
							pwzValueStart = pwzCurrent + nEndConfig + 1;
						}
						else
						{
							pwzValueStart = pwzCurrent + nEndConfig;
						}

                        _StripString(pwzValueStart);

#ifdef _UNICODE
						StringCchCopyExW(ptzValue, MAX_LINE_LENGTH, pwzValueStart, NULL, NULL, STRSAFE_NULL_ON_FAILURE);
#else // _UNICODE
						WideCharToMultiByte(CP_ACP, 0, pwzValueStart, -1, ptzValue, MAX_LINE_LENGTH, NULL, NULL);
#endif // _UNICODE
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

void FileParser::ParseFile(LPCTSTR ptzFileName)
{
	TCHAR tzExpandedPath[MAX_PATH_LENGTH];
	TCHAR tzFullPath[MAX_PATH_LENGTH];
	TCHAR tzKey[MAX_RCCOMMAND];
	TCHAR tzValue[MAX_LINE_LENGTH];

	VarExpansionEx(tzExpandedPath, ptzFileName, MAX_PATH_LENGTH);

    PathUnquoteSpaces(tzExpandedPath);
	GetFullPathName(tzExpandedPath, MAX_PATH_LENGTH, tzFullPath, NULL);

    m_phFile = fopen(tzFullPath, _T("r"));

	if (m_phFile)
	{
		fseek(m_phFile, 0, SEEK_SET);

		while (_ReadLineFromFile(tzKey, tzValue))
		{
			_ProcessLine(tzKey, tzValue);
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
		if (StrChrW(WHITESPACEW, *pwzCurrent) == NULL)
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
					++nQuoteLevel;
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
	if ((pwzCurrent != pwzString) && pwzStart)
	{
		StringCchCopyW(pwzString, lstrlenW(pwzString) + 1, pwzStart);
	}
}


void FileParser::_ProcessLine(LPCTSTR ptzName, LPCTSTR ptzValue)
{
    if (lstrcmpi(ptzName, _T("if")) == 0)
	{
		_ProcessIf(ptzValue);
	}
	else if (lstrcmpi(ptzName, _T("include")) == 0)
	{
		TCHAR tzPath[MAX_PATH_LENGTH];

		GetToken(ptzValue, tzPath, NULL, false);
		
		FileParser fpParser(m_pSettingsMap);
        fpParser.ParseFile(tzPath);
    }
    else
	{
		m_pSettingsMap->insert(SettingsMap::value_type(ptzName, ptzValue));
	}
}


void FileParser::_ProcessIf(LPCTSTR ptzExpression)
{
	TCHAR tzName[MAX_RCCOMMAND];
	TCHAR tzValue[MAX_LINE_LENGTH];
	BOOL bResult;

	if (m_pEvalParser == NULL)
	{
		m_pEvalParser = new EvalParser();
	}

	if (m_pEvalParser)
	{

		if (!m_pEvalParser->evaluate(ptzExpression, &bResult))
		{
			TCHAR tzError[MAX_LINE_LENGTH];

			StringCchPrintf(tzError, MAX_LINE_LENGTH,
			                _T("Syntax error in If expression:\n%s"),
			                ptzExpression);

			MessageBox(NULL, tzError, _T("LiteStep"), MB_SETFOREGROUND);
		}
		else
		{
			if (bResult)
			{
				// when the If condition evaluates true we read and process
				// all lines until we reach ElseIf, Else, or EndIf
				while (_ReadLineFromFile(tzName, tzValue))
				{
                    if ((lstrcmpi(tzName, _T("else")) == 0) || (lstrcmpi(tzName, _T("elseif")) == 0))
					{
						// if we find an ElseIf or Else then we read and ignore
						// all lines until we find EndIf
						_SkipIf();
						break;
					}
					else if (lstrcmpi(tzName, _T("endif")) == 0)
					{
						// we're done
						break;
					}
					else
					{
						// just a normal line, process it
						_ProcessLine(tzName, tzValue);
					}
				}
			}
			else
			{
				// when the If expression evaluates false we read and ignore
				// all lines until we find an ElseIf, Else, or EndIf
				while (_ReadLineFromFile(tzName, tzValue))
				{
					if (lstrcmpi(tzName, _T("if")) == 0)
					{
						// nested Ifs are a special case
						_SkipIf();
					}
					else if (lstrcmpi(tzName, _T("elseif")) == 0)
					{
						// we handle ElseIfs by recursively calling ProcessIf
						_ProcessIf(tzValue);
						break;
					}
					else if (lstrcmpi(tzName, _T("else")) == 0)
					{
						// since the If expression was false, when we see Else
						// we start process lines until EndIf
						while (_ReadLineFromFile(tzName, tzValue))
						{
							// we should probably check that there are no ElseIfs
							// after the Else, but for now we silently ignore
							// such an error

							// break on EndIf
							if (lstrcmpi(tzName, _T("endif")) == 0)
								break;

							// otherwise process the line
							_ProcessLine(tzName, tzValue);
						}

						// we're done
						break;
					}
					else if (lstrcmpi(tzName, _T("endif")) == 0)
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
	TCHAR tzName[MAX_RCCOMMAND];

	while (_ReadLineFromFile(tzName, NULL))
	{
		if (lstrcmpi(tzName, _T("if")) == 0)
		{
			_SkipIf();
		}
		else if (lstrcmpi(tzName, _T("endif")) == 0)
		{
			break;
		}
	}
}
