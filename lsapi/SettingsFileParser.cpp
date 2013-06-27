//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//
// This is a part of the Litestep Shell source code.
//
// Copyright (C) 1997-2013  LiteStep Development Team
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
#include "SettingsFileParser.h"
#include "MathEvaluate.h"
#include "../utility/core.hpp"


//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//
// FileParser constructor
//
FileParser::FileParser(SettingsMap* pSettingsMap) :
    m_pSettingsMap(pSettingsMap), m_phFile(NULL)
{
    ASSERT(NULL != m_pSettingsMap);
}


//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//
// FileParser destructor
//
FileParser::~FileParser()
{
    // do nothing
}


//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
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
    
    DWORD dwLen = GetFullPathName(
        tzExpandedPath, MAX_PATH_LENGTH, m_tzFullPath, NULL);
    
    if (0 == dwLen || dwLen > MAX_PATH_LENGTH)
    {
        TRACE("Error: Can not get full path for \"%s\"", tzExpandedPath);
        return;
    }
    
    m_phFile = _tfopen(m_tzFullPath, _T("r"));
    
    if (NULL == m_phFile)
    {
        TRACE("Error: Can not open file \"%s\" (Defined as \"%s\").",
            m_tzFullPath, ptzFileName);
        return;
    }
    
    TRACE("Parsing \"%s\"", m_tzFullPath);
    
    fseek(m_phFile, 0, SEEK_SET);
    
    TCHAR tzKey[MAX_RCCOMMAND] = { 0 };
    TCHAR tzValue[MAX_LINE_LENGTH] = { 0 };
    
    m_uLineNumber = 0;
    
    while (_ReadLineFromFile(tzKey, tzValue))
    {
        _ProcessLine(tzKey, tzValue);
    }
    
    fclose(m_phFile);
    m_phFile = NULL;
    
    TRACE("Finished Parsing \"%s\"", m_tzFullPath);
}


//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
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
        
        ++m_uLineNumber;
        
        LPTSTR ptzCurrent = tzBuffer;
        
        // Jump over any initial whitespace
        ptzCurrent += _tcsspn(ptzCurrent, WHITESPACE);
        
        // Ignore empty lines, and comments
        if (ptzCurrent[0] && ptzCurrent[0] != _T(';'))
        {
            // End on first reserved character or whitespace
            size_t stEndConfig = _tcscspn(ptzCurrent, WHITESPACE RESERVEDCHARS);
            
            // If the character is not whitespace or a comment
            // then the line has an invalid format.  Ignore it.
            if (_tcschr(WHITESPACE _T(";"), ptzCurrent[stEndConfig]) == NULL)
            {
                TRACE("Syntax Error (%s, %d): Invalid line format",
                    m_tzFullPath, m_uLineNumber);
                continue;
            }
            
            // Copy directive name to ptzName.
            if (stEndConfig && SUCCEEDED(StringCchCopyN(
                ptzName, MAX_RCCOMMAND, ptzCurrent, stEndConfig)))
            {
                // If ptzValue is NULL, then the caller doesn't want the value,
                // however, we still will return TRUE.  If the caller does want
                // the value, then we need to ensure we put something into the
                // buffer, even if its is zero length.
                if (ptzValue != NULL)
                {
                    LPTSTR ptzValueStart = ptzCurrent + stEndConfig;
                    
                    // Avoid expensive in-place copy from _StripString
                    // Simply increment passed any whitespace, here.
                    ptzValueStart += _tcsspn(ptzValueStart, WHITESPACE);
                    
                    // Removing trailing whitespace and comments
                    _StripString(ptzValueStart);
                    
                    StringCchCopy(ptzValue, MAX_LINE_LENGTH, ptzValueStart);
                }
                
                bReturn = true;
            }
        }
    }
    
    return bReturn;
}


//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
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
        if (strchr(WHITESPACE, *ptzCurrent) == NULL)
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
        while (ptzLast > ptzString && strchr(WHITESPACE, *(ptzLast-1)))
        {
            --ptzLast;
        }
        
        *ptzLast = '\0';
    }
    
    if (ptzStart != NULL && ptzStart != ptzString)
    {
        StringCchCopy(ptzString, strlen(ptzString) + 1, ptzStart);
    }
}


//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//
// _ProcessLine
//
void FileParser::_ProcessLine(LPCTSTR ptzName, LPCTSTR ptzValue)
{
    ASSERT(NULL != m_pSettingsMap);
    ASSERT(NULL != ptzName); ASSERT(NULL != ptzValue);
    
    if (_stricmp(ptzName, _T("if")) == 0)
    {
        _ProcessIf(ptzValue);
    }
#if defined(_DEBUG)
    // In a release build ignore dangling elseif/else/endif.
    // Too much overhead just for error handling
    else if (
           (_stricmp(ptzName, "else") == 0)
        || (_stricmp(ptzName, "elseif") == 0)
        || (_stricmp(ptzName, "endif") == 0)
    )
    {
        TRACE("Error: Dangling pre-processor directive (%s, line %d): \"%s\"",
            m_tzFullPath, m_uLineNumber, ptzName);
    }
#endif
    else if (_stricmp(ptzName, "include") == 0)
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
#if defined(LS_CUSTOM_INCLUDEFOLDER)
    else if (_stricmp(ptzName, _T("includefolder")) == 0)
    {
        TCHAR tzPath[MAX_PATH_LENGTH]; // path+pattern
        TCHAR tzFilter[MAX_PATH_LENGTH]; // path only
          
        // expands string in ptzValue to tzPath
        // buffer size defined by MAX_PATH_LENGTH
        VarExpansionEx(tzPath, ptzValue, MAX_PATH_LENGTH);
        
        PathUnquoteSpaces(tzPath); // strips quotation marks from string
        
        TRACE("Searching IncludeFolder (%s, line %d): \"%s\"",
            m_tzFullPath, m_uLineNumber, tzPath);
        
        // Hard-coded filter for *.rc files to limit search operation.
        //
        // Create tzFilter as tzPath appended with *.rc
        //  - the API takes care of trailing slash handling thankfully.
        PathCombine(tzFilter, tzPath, _T("*.rc"));
        
        WIN32_FIND_DATA findData; // defining variable for filename
        
        // Looking in tzFilter for data :)
        HANDLE hSearch = FindFirstFile(tzFilter, &findData);
        
        if (INVALID_HANDLE_VALUE != hSearch)
        {
            BOOL FoundNextFile;
            
            do
            {
                // stripping out directories, system and hidden files as
                // we're not interested in them and MS throws these kind of
                // files around from time to time....
                const DWORD dwAttrib = (FILE_ATTRIBUTE_DIRECTORY |
                                        FILE_ATTRIBUTE_HIDDEN |
                                        FILE_ATTRIBUTE_SYSTEM);
                
                if (0 == (dwAttrib & findData.dwFileAttributes))
                {
                    // Processing the valid cFileName data now.
                    TCHAR tzFile[MAX_PATH_LENGTH];
                    
                    // adding (like above) filename to tzPath to set tzFile
                    // for opening.
                    if (tzFile == PathCombine(tzFile, tzPath, findData.cFileName))
                    {
                        TRACE("Found and including: \"%s\"", tzFile);
                        
                        FileParser fpParser(m_pSettingsMap);
                        fpParser.ParseFile(tzFile);
                    }
                }
                
                FoundNextFile = FindNextFile(hSearch, &findData);
            } while (FoundNextFile);
            
            FindClose(hSearch);
        }
        
        TRACE("Done searching IncludeFolder (%s, line %d): \"%s\"",
            m_tzFullPath, m_uLineNumber, tzPath);
    }
#endif // LS_CUSTOM_INCLUDEFOLDER
    else
    {
        m_pSettingsMap->insert(SettingsMap::value_type(ptzName, ptzValue));
    }
}


//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//
// _ProcessIf
//
void FileParser::_ProcessIf(LPCTSTR ptzExpression)
{
    ASSERT(NULL != m_pSettingsMap);
    ASSERT(NULL != ptzExpression);
    
    bool result = false;
    
    if (!MathEvaluateBool(*m_pSettingsMap, ptzExpression, result))
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
            if ((_stricmp(tzName, _T("else")) == 0) ||
                (_stricmp(tzName, _T("elseif")) == 0))
            {
                // After an ElseIf or Else, skip all lines until EndIf
                _SkipIf();
                break;
            }
            else if (_stricmp(tzName, _T("endif")) == 0)
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
            if (_stricmp(tzName, _T("if")) == 0)
            {
                // Nested Ifs are a special case
                _SkipIf();
            }
            else if (_stricmp(tzName, _T("elseif")) == 0)
            {
                // Handle ElseIfs by recursively calling ProcessIf
                _ProcessIf(tzValue);
                break;
            }
            else if (_stricmp(tzName, _T("else")) == 0)
            {
                // Since the If expression was false, when we see Else we
                // start processing lines until EndIf
                while (_ReadLineFromFile(tzName, tzValue))
                {
                    if (_stricmp(tzName, "elseif") == 0)
                    {
                        // Error: ElseIf after Else
                        TRACE("Syntax Error (%s, %d): "
                              "\"ElseIf\" directive after \"Else\"",
                            m_tzFullPath, m_uLineNumber);
                        
                        // Invalid syntax, so quit processing conditional block
                        _SkipIf();
                        break;
                    }
                    else if (_stricmp(tzName, _T("endif")) == 0)
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
            else if (_stricmp(tzName, _T("endif")) == 0)
            {
                // We're done
                break;
            }
        }
    }
}


//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//
// _SkipIf
//
void FileParser::_SkipIf()
{
    TCHAR tzName[MAX_RCCOMMAND];
    
    while (_ReadLineFromFile(tzName, NULL))
    {
        if (_stricmp(tzName, _T("if")) == 0)
        {
            _SkipIf();
        }
        else if (_stricmp(tzName, _T("endif")) == 0)
        {
            break;
        }
    }
}
