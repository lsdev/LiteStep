//
//  This is a part of the LiteStep Shell source code.
//
//  Copyright (C) 1997-2003 The LiteStep Development Team.
//
//
//  This program is free software; you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation; either version 2 of the License, or
//  (at your option) any later version.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with this program; if not, write to the Free Software
//  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
//
#include "SettingsFileParser.h"
#include "../utility/core.hpp"


//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
//
// FileParser constructor
//
FileParser::FileParser(SettingsMap* pSettingsMap)
: m_pSettingsMap(pSettingsMap), m_phFile(NULL)
{
    ASSERT_ISWRITEPTR(m_pSettingsMap);
}


//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
//
// FileParser destructor
//
FileParser::~FileParser()
{
}


//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
//
// ParseFile
//
void FileParser::ParseFile(LPCTSTR ptzFileName)
{
    ASSERT_ISSTRING(ptzFileName);
    ASSERT(m_phFile == NULL);
    
    TCHAR tzExpandedPath[MAX_PATH_LENGTH];
    TCHAR tzFullPath[MAX_PATH_LENGTH];
    
    VarExpansionEx(tzExpandedPath, ptzFileName, MAX_PATH_LENGTH);

    PathUnquoteSpaces(tzExpandedPath);
    GetFullPathName(tzExpandedPath, MAX_PATH_LENGTH, tzFullPath, NULL);
    
    TRACE("Parsing \"%s\"", tzFullPath);

    m_phFile = _tfopen(tzFullPath, _T("r"));
    
    if (m_phFile)
    {
        fseek(m_phFile, 0, SEEK_SET);
        
        TCHAR tzKey[MAX_RCCOMMAND] = { 0 };
        TCHAR tzValue[MAX_LINE_LENGTH] = { 0 };

        while (_ReadLineFromFile(tzKey, tzValue))
        {
            _ProcessLine(tzKey, tzValue);
        }
        
        fclose(m_phFile);
    }
    else
    {
        ErrorEx(LOCALIZE_THIS,
            _T("Error opening \"%s\" for parsing.\n")
            _T("Received as: %s"), tzFullPath, ptzFileName);
    }
}


//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
//
// _ReadLineFromFile
//
// ptzName must be MAX_RCOCOMMAND size
// ptzValue must be MAX_LINE_LENGTH size (or NULL)
//
bool FileParser::_ReadLineFromFile(LPTSTR ptzName, LPTSTR ptzValue)
{
    ASSERT_ISWRITEDATA(ptzName, MAX_RCCOMMAND);
    ASSERT((ptzValue == NULL) || !IsBadWritePtr(ptzValue, MAX_LINE_LENGTH));
    ASSERT_ISREADPTR(m_phFile);
    
    TCHAR tzBuffer[MAX_LINE_LENGTH] = { 0 };
    bool bReturn = false;
    
    while (!feof(m_phFile))
    {
        if (!_fgetts(tzBuffer, MAX_LINE_LENGTH, m_phFile))
        {
            break;
        }
        
        LPTSTR ptzCurrent = tzBuffer;
        ptzCurrent += StrSpn(ptzCurrent, WHITESPACE);

        if (ptzCurrent[0] && ptzCurrent[0] != _T(';'))
        {
            size_t stEndConfig = _tcscspn(ptzCurrent, WHITESPACE);
            
            if (SUCCEEDED(StringCchCopyNEx(ptzName, MAX_RCCOMMAND, ptzCurrent,
                stEndConfig, NULL, NULL, STRSAFE_NULL_ON_FAILURE)))
            {
                if (ptzValue != NULL)
                {
                    LPTSTR ptzValueStart = ptzCurrent + stEndConfig;
                    
                    if (stEndConfig < (size_t)lstrlen(ptzCurrent))
                    {
                        ++ptzValueStart;
                    }
                    
                    _StripString(ptzValueStart);
                    
                    StringCchCopyEx(ptzValue, MAX_LINE_LENGTH, ptzValueStart,
                        NULL, NULL, STRSAFE_NULL_ON_FAILURE);
                }
                
                bReturn = true;
            }
            
            break;
        }
    }

    return bReturn;
}


//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
//
// _StripString
//
void FileParser::_StripString(LPTSTR ptzString)
{
    ASSERT_ISWRITEPTR(ptzString);

    LPTSTR ptzCurrent = ptzString;
	LPTSTR ptzStart = NULL;
	LPTSTR ptzLast = NULL;
	size_t stQuoteLevel = 0;
	TCHAR tLastQuote = '\0';

    while (*ptzCurrent != _T('\0'))
	{
		if (StrChr(WHITESPACE, *ptzCurrent) == NULL)
		{
			if (ptzStart == NULL)
			{
				ptzStart = ptzCurrent;
			}
			
            ptzLast = NULL;
		}
		else if (ptzLast == NULL)
        {
            ptzLast = ptzCurrent;
        }

		if (ptzStart != NULL)
		{
			switch (*ptzCurrent)
            {
                case _T('['):
                {
                    ++stQuoteLevel;
                }
                break;
                
                case _T(']'):
                {
                    if (stQuoteLevel > 0)
                    {
                        --stQuoteLevel;
                    }
                }
                break;
                
                case _T('"'):
                case _T('\''):
                {
                    if (*ptzCurrent == tLastQuote)
                    {
                        --stQuoteLevel;
                        tLastQuote = '\0';
                    }
                    else if (tLastQuote == '\0')
                    {
                        ++stQuoteLevel;
                        tLastQuote = *ptzCurrent;
                    }
                }
                break;

                case _T(';'):
                {
                    if (!stQuoteLevel)
                    {
                        ptzLast = ptzCurrent;
                    }
                }
                break;
                
                default:
                break;
            }
        }

        ++ptzCurrent;
    }

	if (ptzLast != NULL)
	{
		*ptzLast = '\0';
	}
	if ((ptzCurrent != ptzString) && ptzStart)
	{
		StringCchCopy(ptzString, lstrlen(ptzString) + 1, ptzStart);
	}
}


//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
//
// _ProcessLine
//
void FileParser::_ProcessLine(LPCTSTR ptzName, LPCTSTR ptzValue)
{
    ASSERT_ISSTRING(ptzName); ASSERT_ISSTRING(ptzValue);

    if (lstrcmpi(ptzName, _T("if")) == 0)
	{
		_ProcessIf(ptzValue);
	}
	else if (lstrcmpi(ptzName, _T("include")) == 0)
	{
		TCHAR tzPath[MAX_PATH_LENGTH];

		GetToken(ptzValue, tzPath, NULL, FALSE);
		
		FileParser fpParser(m_pSettingsMap);
        fpParser.ParseFile(tzPath);
    }
    else
	{
		m_pSettingsMap->insert(SettingsMap::value_type(ptzName, ptzValue));
	}
}


//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
//
// _ProcessIf
//
void FileParser::_ProcessIf(LPCTSTR ptzExpression)
{
	ASSERT_ISSTRING(ptzExpression);

    TCHAR tzName[MAX_RCCOMMAND] = { 0 };
	TCHAR tzValue[MAX_LINE_LENGTH] = { 0 };

	if (!m_pEvalParser.get())
	{
		m_pEvalParser.reset(new EvalParser());
	}

	if (m_pEvalParser.get())
	{
        BOOL bResult;

        if (!m_pEvalParser->evaluate(ptzExpression, &bResult))
		{
            ErrorEx(LOCALIZE_THIS,
                _T("Syntax error in If expression:\n%s"), ptzExpression);
		}
		else
		{
			if (bResult)
			{
				// when the If condition evaluates true we read and process
				// all lines until we reach ElseIf, Else, or EndIf
				while (_ReadLineFromFile(tzName, tzValue))
				{
                    if ((lstrcmpi(tzName, _T("else")) == 0) ||
                        (lstrcmpi(tzName, _T("elseif")) == 0))
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
							// we should probably check that there are no
							// ElseIfs after the Else, but for now we silently
							// ignore such an error

							// break on EndIf
							if (lstrcmpi(tzName, _T("endif")) == 0)
                            {
                                break;
                            }

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


//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
//
// _SkipIf
//
void FileParser::_SkipIf()
{
    TCHAR tzName[MAX_RCCOMMAND] = { 0 };

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
