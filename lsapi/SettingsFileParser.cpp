//
//  This is a part of the LiteStep Shell source code.
//
//  Copyright (C) 1997-2006 The LiteStep Development Team.
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
FileParser::FileParser(SettingsMap* pSettingsMap) :
    m_pSettingsMap(pSettingsMap), m_phFile(NULL), m_pEvalParser(NULL)
{
    ASSERT(NULL != m_pSettingsMap);
}


//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
//
// FileParser destructor
//
FileParser::~FileParser()
{
    if(NULL != m_pEvalParser)
    {
        delete m_pEvalParser;
    }
}


//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
//
// ParseFile
//
void FileParser::ParseFile(LPCTSTR ptzFileName)
{
    ASSERT(NULL == m_phFile);
    ASSERT(NULL != ptzFileName);
    
    TCHAR tzExpandedPath[MAX_PATH_LENGTH];
    
    VarExpansionEx(tzExpandedPath, ptzFileName, MAX_PATH_LENGTH);
    PathUnquoteSpaces(tzExpandedPath);
    
    DWORD dwLen;
    dwLen = GetFullPathName(tzExpandedPath, MAX_PATH_LENGTH, m_tzFullPath, NULL);
    
    if(0 == dwLen || dwLen > MAX_PATH_LENGTH)
    {
        TRACE("Error: Can not get full path for \"%s\"", tzExpandedPath);
        return;
    }
    
    m_phFile = _tfopen(m_tzFullPath, _T("r"));
    
    if(NULL == m_phFile)
    {
        TRACE("Error: Can not open file \"%s\" (Defined as \"%s\").", m_tzFullPath, ptzFileName);
        return;
    }
    
    TRACE("Parsing \"%s\"", m_tzFullPath);
    
    fseek(m_phFile, 0, SEEK_SET);
    
    TCHAR tzKey[MAX_RCCOMMAND] = { 0 };
    TCHAR tzValue[MAX_LINE_LENGTH] = { 0 };
    
    m_uLineNumber = 0;
    
    while(_ReadLineFromFile(tzKey, tzValue))
    {
        _ProcessLine(tzKey, tzValue);
    }
    
    fclose(m_phFile);
    m_phFile = NULL;
    
    TRACE("Finished Parsing \"%s\"", m_tzFullPath);
}


//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
//
// _ReadLineFromFile
//
// ptzName must be MAX_RCCOMMAND size
// ptzValue must be MAX_LINE_LENGTH size (or NULL)
//
bool FileParser::_ReadLineFromFile(LPTSTR ptzName, LPTSTR ptzValue)
{
    ASSERT(NULL != m_phFile);
    ASSERT(NULL != ptzName);
    
    TCHAR tzBuffer[MAX_LINE_LENGTH];
    bool bReturn = false;
    
    while (!feof(m_phFile) && !bReturn)
    {
        if (!_fgetts(tzBuffer, MAX_LINE_LENGTH, m_phFile))
        {
            // End Of File or an Error occured. We don't care which.
            break;
        }
        
        m_uLineNumber++;
        
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
    ASSERT(NULL != ptzString);
    
    LPTSTR ptzCurrent = ptzString;
    LPTSTR ptzStart = NULL;
    LPTSTR ptzLast = NULL;
    size_t stQuoteLevel = 0;
    TCHAR tLastQuote = _T('\0');
    
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
            if (*ptzCurrent == '[')
            {
                ++stQuoteLevel;
            }
            else if (*ptzCurrent == ']')
            {
                if (stQuoteLevel > 0)
                {
                    --stQuoteLevel;
                }
            }
            else if ((*ptzCurrent == '"') || (*ptzCurrent == '\''))
            {
                if (tLastQuote == *ptzCurrent)
                {
                    ASSERT(stQuoteLevel > 0);
                    --stQuoteLevel;
                    tLastQuote = 0;
                }
                else if (!tLastQuote)
                {
                    ++stQuoteLevel;
                    tLastQuote = *ptzCurrent;
                }
            }
            else if (*ptzCurrent == ';')
            {
                if (!stQuoteLevel)
                {
                    ptzLast = ptzCurrent;
                    break;
                }
            }
        }
        
        ++ptzCurrent;
    }
    
    if (ptzLast != NULL)
    {
        while (ptzLast > ptzString && StrChr(WHITESPACE, *(ptzLast-1)))
        {
            --ptzLast;
        }
        
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
    ASSERT(NULL != m_pSettingsMap);
    ASSERT(NULL != ptzName); ASSERT(NULL != ptzValue);
    
    if (lstrcmpi(ptzName, _T("if")) == 0)
    {
        _ProcessIf(ptzValue);
    }
#ifdef _DEBUG
    // In a release build ignore dangling elseif/else/endif.
    // Too much overhead just for error handling
    else if (
           (lstrcmpi(ptzName, "else") == 0)
        || (lstrcmpi(ptzName, "elseif") == 0)
        || (lstrcmpi(ptzName, "endif") == 0)
    )
    {
        TRACE("Error: Dangling pre-processor directive (%s, line %d): \"%s\"",
            m_tzFullPath, m_uLineNumber, ptzName);
    }
#endif
    else if (lstrcmpi(ptzName, "include") == 0)
    {
        TCHAR tzPath[MAX_PATH_LENGTH] = { 0 };
        
        if (!GetToken(ptzValue, tzPath, NULL, FALSE))
        {
            TRACE("Syntax Error (%s, %d): Empty \"Include\" directive",
                m_tzFullPath, m_uLineNumber);
            return;
        }
        
        TRACE("Include (%s, line %d): \"%s\"",
            m_tzFullPath, m_uLineNumber, tzPath);
        
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
    ASSERT(NULL != m_pSettingsMap);
    ASSERT(NULL != ptzExpression);
    
    BOOL result = FALSE;
    
    if (NULL == m_pEvalParser)
    {
        // Don't create it until the first time conditionals are used.  No reason
        // to penalize people who don't use them.
        m_pEvalParser = new EvalParser();
    }
    
    if (NULL == m_pEvalParser || !m_pEvalParser->evaluate(ptzExpression, &result))
    {
        TRACE("Error parsing expression \"%s\" (%s, line %d)",
            ptzExpression, m_tzFullPath, m_uLineNumber);
        
        // Invalid syntax, so quit processing entire conditional block
        _SkipIf();
        return;
    }
    
    TRACE("Expression (%s, line %d): \"%s\" evaluated to %s",
        m_tzFullPath, m_uLineNumber,
        ptzExpression, result ? "TRUE" : "FALSE");
    
    TCHAR tzName[MAX_RCCOMMAND] = { 0 };
    TCHAR tzValue[MAX_LINE_LENGTH] = { 0 };
    
    if (result)
    {
        // When the If expression evaluates true, process lines until we find
        // an ElseIf. Else, or EndIf
        while (_ReadLineFromFile(tzName, tzValue))
        {
            if ((lstrcmpi(tzName, _T("else")) == 0) ||
                (lstrcmpi(tzName, _T("elseif")) == 0))
            {
                // After an ElseIf or Else, skip all lines until EndIf
                _SkipIf();
                break;
            }
            else if (lstrcmpi(tzName, _T("endif")) == 0)
            {
                // We're done
                break;
            }
            else
            {
                // Just a line, so process it
                _ProcessLine(tzName, tzValue);
            }
        }
    }
    else
    {
        // When the If expression evaluates false, skip lines until we find an
        // ElseIf, Else, or EndIf
        while (_ReadLineFromFile(tzName, tzValue))
        {
            if (lstrcmpi(tzName, _T("if")) == 0)
            {
                // Nested Ifs are a special case
                _SkipIf();
            }
            else if (lstrcmpi(tzName, _T("elseif")) == 0)
            {
                // Handle ElseIfs by recursively calling ProcessIf
                _ProcessIf(tzValue);
                break;
            }
            else if (lstrcmpi(tzName, _T("else")) == 0)
            {
                // Since the If expression was false, when we see Else we
                // start processing lines until EndIf
                while (_ReadLineFromFile(tzName, tzValue))
                {
                    if (lstrcmpi(tzName, "elseif") == 0)
                    {
                        // Error: ElseIf after Else
                        TRACE("Syntax Error (%s, %d): \"ElseIf\" directive after \"Else\"",
                            m_tzFullPath, m_uLineNumber);
                        
                        // Invalid syntax, so quit processing conditional block
                        _SkipIf();
                        break;
                    }
                    else if (lstrcmpi(tzName, _T("endif")) == 0)
                    {
                        // We're done
                        break;
                    }
                    else
                    {
                        // Just a line, so process it
                        _ProcessLine(tzName, tzValue);
                    }
                }
                // We're done
                break;
            }
            else if (lstrcmpi(tzName, _T("endif")) == 0)
            {
                // We're done
                break;
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
