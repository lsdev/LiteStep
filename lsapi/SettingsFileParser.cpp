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
#include "../utility/macros.h"
#include <algorithm>
#include <vector>


//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//
// FileParser constructor
//
FileParser::FileParser(SettingsMap* pSettingsMap) :
    m_pSettingsMap(pSettingsMap), m_phFile(NULL), m_trail(m_baseTrail)
{
    ASSERT(NULL != m_pSettingsMap);
}


//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//
// FileParser constructor
//
FileParser::FileParser(SettingsMap* pSettingsMap, std::list<TrailItem> &trail) :
    m_pSettingsMap(pSettingsMap), m_phFile(NULL), m_trail(trail)
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
    ASSERT(nullptr == m_phFile);
    ASSERT(nullptr != ptzFileName);
    
    TCHAR tzExpandedPath[MAX_PATH_LENGTH];
    
    VarExpansionExW(tzExpandedPath, ptzFileName, MAX_PATH_LENGTH);
    PathUnquoteSpaces(tzExpandedPath);
    
    DWORD dwLen = GetFullPathName(
        tzExpandedPath, MAX_PATH_LENGTH, m_tzFullPath, nullptr);
    
    if (0 == dwLen || dwLen > MAX_PATH_LENGTH)
    {
        TRACE("Error: Can not get full path for \"%ls\"", tzExpandedPath);
        return;
    }

    std::list<TrailItem>::iterator check = std::find(m_trail.begin(), m_trail.end(), TrailItem(0, m_tzFullPath));
    if (check != m_trail.end())
    {
        TCHAR trail[MAX_LINE_LENGTH];
        TCHAR line[MAX_LINE_LENGTH];

        *trail = _T('\0');
        *line = _T('\0');
        for (; check != m_trail.end(); ++check)
        {
            StringCchCat(trail, _countof(trail), _T("\""));
            StringCchCat(trail, _countof(trail), check->ptzPath);
            StringCchCat(trail, _countof(trail), _T("\""));
            StringCchCat(trail, _countof(trail), line);
            StringCchPrintf(line, _countof(line), _T(" on line %d"), check->uLine);
            StringCchCat(trail, _countof(trail), _T("\nIncludes "));
        }
        StringCchCat(trail, _countof(trail), _T("\""));
        StringCchCat(trail, _countof(trail), m_tzFullPath);
        StringCchCat(trail, _countof(trail), _T("\""));
        StringCchCat(trail, _countof(trail), line);

        RESOURCE_STREX(
            GetModuleHandle(NULL), IDS_RECURSIVEINCLUDE,
            resourceTextBuffer, MAX_LINE_LENGTH,
            L"Error: Reursive include detected!\n %ls.",
            trail);
                            
        RESOURCE_MSGBOX_F(L"LiteStep", MB_ICONERROR);

        return;
    }
    
    _tfopen_s(&m_phFile, m_tzFullPath, _T("r"));
    
    if (nullptr == m_phFile)
    {
        TRACE("Error: Can not open file \"%ls\" (Defined as \"%ls\").",
            m_tzFullPath, ptzFileName);
        return;
    }
    
    TRACE("Parsing \"%ls\"", m_tzFullPath);
    m_trail.push_back(TrailItem(0, m_tzFullPath));
    
    fseek(m_phFile, 0, SEEK_SET);
    
    TCHAR tzKey[MAX_RCCOMMAND] = { 0 };
    TCHAR tzValue[MAX_LINE_LENGTH] = { 0 };
    
    m_uLineNumber = 0;
    
    _ReadNextLine(m_tzReadAhead);
    while (_ReadLineFromFile(tzKey, tzValue))
    {
        _ProcessLine(tzKey, tzValue);
    }
    
    fclose(m_phFile);
    m_phFile = nullptr;
    m_trail.pop_back();
    
    TRACE("Finished Parsing \"%ls\"", m_tzFullPath);
}


//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//
// _ReadLineFromFile
//
// ptzBuffer must be MAX_LINE_LENGTH size
//
bool FileParser::_ReadNextLine(LPTSTR ptzBuffer)
{
    ASSERT(nullptr != m_phFile);
    ASSERT(nullptr != ptzBuffer);

    TCHAR tzBuffer[MAX_LINE_LENGTH];
    bool bReturn = false;

    ptzBuffer[0] = _T('\0');

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
        if (ptzCurrent[0] != '\0' && ptzCurrent[0] != _T(';'))
        {
            // End on first reserved character or whitespace
            size_t stEndConfig = _tcscspn(ptzCurrent, WHITESPACE RESERVEDCHARS);
            
            // If the character is not whitespace or a comment
            // then the line has an invalid format.  Ignore it.
            if (_tcschr(WHITESPACE _T(";"), ptzCurrent[stEndConfig]) == NULL)
            {
                TRACE("Syntax Error (%ls, %d): Invalid line format",
                    m_tzFullPath, m_uLineNumber);
                continue;
            }

            StringCchCopy(ptzBuffer, MAX_LINE_LENGTH, ptzCurrent);
            bReturn = true;
        }
    }

    return bReturn;
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
    
    bool bReturn = false;

    if (m_tzReadAhead[0] == '}')
    {
        if (m_stPrefixes.empty())
        {
            TRACE("Syntax Error (%ls, %d): Unexpected }",
                m_tzFullPath, m_uLineNumber);
        }
        else
        {
            m_stPrefixes.pop_front();
        }
        
        // Skip this line
        _ReadNextLine(m_tzReadAhead);
        bReturn = _ReadLineFromFile(ptzName, ptzValue);
    }
    else if (m_tzReadAhead[0] != _T('\0'))
    {
        LPTSTR ptzCurrent = m_tzReadAhead;

        // End on first reserved character or whitespace
        size_t stEndConfig = _tcscspn(ptzCurrent, WHITESPACE RESERVEDCHARS);
        
        if (stEndConfig != 0)
        {
            ptzName[0] = _T('\0');

            // Apply any prefix, as necesary
            if (!m_stPrefixes.empty())
            {
                // If the key starts with a *, put that * at the begining
                if (*ptzCurrent == _T('*'))
                {
                    StringCchCat(ptzName, MAX_RCCOMMAND, _T("*"));
                    ++ptzCurrent;
                    --stEndConfig;
                }

                // Don't apply prefixes to special keywords
                if (!( _tcsnicmp(ptzCurrent, _T("if"), stEndConfig) == 0
                    || _tcsnicmp(ptzCurrent, _T("else"), stEndConfig) == 0
                    || _tcsnicmp(ptzCurrent, _T("elseif"), stEndConfig) == 0
                    || _tcsnicmp(ptzCurrent, _T("endif"), stEndConfig) == 0
                    ))
                {
                    StringCchCat(ptzName, MAX_RCCOMMAND, m_stPrefixes.front().tzString);
                }

                // If the keyname is simply -, ignore it.
                if (_tcsnicmp(ptzCurrent, _T("-"), stEndConfig) == 0)
                {
                    ++ptzCurrent;
                    stEndConfig = 0;
                }
            }

            // Copy directive name to ptzName.
            if (SUCCEEDED(StringCchCatN(ptzName, MAX_RCCOMMAND, ptzCurrent, stEndConfig)))
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

                    DWORD cchRemaining = MAX_LINE_LENGTH;

                    // If we have prefixes, check if the string contains any @'s
                    if (!m_stPrefixes.empty())
                    {
                        LPTSTR ptzAtSearch;
                        while ((ptzAtSearch = _tcschr(ptzValueStart, _T('@'))) != nullptr)
                        {
                            // Copy this part of the value over.
                            DWORD nSize = (DWORD)(ptzAtSearch - ptzValueStart);
                            StringCchCopyN(ptzValue, cchRemaining, ptzValueStart, nSize);
                            cchRemaining -= nSize;
                            ptzValue += nSize;

                            // Figure out how many levels up to go
                            auto prefix = m_stPrefixes.begin();
                            for (; *(ptzAtSearch + 1) == _T('@'); ++ptzAtSearch)
                            {
                                if (prefix != m_stPrefixes.end())
                                {
                                    ++prefix;
                                }
                            }
                            
                            // Copy over the prefix.
                            if (prefix != m_stPrefixes.end())
                            {
                                StringCchCopy(ptzValue, cchRemaining, prefix->tzString);
                                nSize = (DWORD)_tcslen(prefix->tzString);
                                ptzValue += nSize;
                                cchRemaining -= nSize;
                            }

                            // Move our pointer past the @'s
                            ptzValueStart = ++ptzAtSearch;
                        }
                    }
                    StringCchCopy(ptzValue, cchRemaining, ptzValueStart);
                }
                
                bReturn = true;

                // Reads the next line
                if (_ReadNextLine(m_tzReadAhead))
                {
                    if (m_tzReadAhead[0] == '{')
                    {
                        m_stPrefixes.push_front(TCStack(ptzName));

                        // Skip these 2 lines.
                        _ReadNextLine(m_tzReadAhead);
                        bReturn = _ReadLineFromFile(ptzName, ptzValue);
                    }
                }
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
        if (wcschr(WHITESPACE, *ptzCurrent) == NULL)
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
        while (ptzLast > ptzString && wcschr(WHITESPACE, *(ptzLast-1)))
        {
            --ptzLast;
        }
        
        *ptzLast = '\0';
    }
    
    if (ptzStart != NULL && ptzStart != ptzString)
    {
        StringCchCopy(ptzString, wcslen(ptzString) + 1, ptzStart);
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
    
    if (_wcsicmp(ptzName, _T("if")) == 0)
    {
        _ProcessIf(ptzValue);
    }
#if defined(_DEBUG)
    // In a release build ignore dangling elseif/else/endif.
    // Too much overhead just for error handling
    else if (
           (_wcsicmp(ptzName, L"else") == 0)
        || (_wcsicmp(ptzName, L"elseif") == 0)
        || (_wcsicmp(ptzName, L"endif") == 0)
    )
    {
        TRACE("Error: Dangling pre-processor directive (%ls, line %d): \"%ls\"",
            m_tzFullPath, m_uLineNumber, ptzName);
    }
#endif
    else if (_wcsicmp(ptzName, L"include") == 0)
    {
        TCHAR tzPath[MAX_PATH_LENGTH] = { 0 };
        
        if (!GetTokenW(ptzValue, tzPath, NULL, FALSE))
        {
            TRACE("Syntax Error (%ls, %d): Empty \"Include\" directive",
                m_tzFullPath, m_uLineNumber);
            return;
        }
        
        TRACE("Include (%ls, line %d): \"%ls\"",
            m_tzFullPath, m_uLineNumber, tzPath);
        
        m_trail.back().uLine = m_uLineNumber;
        FileParser fpParser(m_pSettingsMap, m_trail);
        fpParser.ParseFile(tzPath);
    }
#if defined(LS_CUSTOM_INCLUDEFOLDER)
    else if (_wcsicmp(ptzName, _T("includefolder")) == 0)
    {
        TCHAR tzPath[MAX_PATH_LENGTH]; // path+pattern
        TCHAR tzFilter[MAX_PATH_LENGTH]; // path only
          
        // expands string in ptzValue to tzPath
        // buffer size defined by MAX_PATH_LENGTH
        VarExpansionExW(tzPath, ptzValue, MAX_PATH_LENGTH);
        
        PathUnquoteSpaces(tzPath); // strips quotation marks from string
        
        TRACE("Searching IncludeFolder (%ls, line %d): \"%ls\"",
            m_tzFullPath, m_uLineNumber, tzPath);
        
        // Hard-coded filter for *.rc files to limit search operation.
        //
        // Create tzFilter as tzPath appended with *.rc
        //  - the API takes care of trailing slash handling thankfully.
        PathCombine(tzFilter, tzPath, _T("*.rc"));
        
        WIN32_FIND_DATA findData; // defining variable for filename
        
        // Looking in tzFilter for data :)
        HANDLE hSearch = FindFirstFile(tzFilter, &findData);

        // List of found files
        std::vector<std::wstring> foundFiles;

        //
        auto fileComparer = [] (const std::wstring s1, const std::wstring s2) -> bool {
            return (_wcsicmp(s1.c_str(), s2.c_str()) > 0);
        };
        
        if (INVALID_HANDLE_VALUE != hSearch)
        {
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
                    foundFiles.push_back(findData.cFileName);
                    std::push_heap(foundFiles.begin(), foundFiles.end(), fileComparer);
                }
            } while (FindNextFile(hSearch, &findData) != FALSE);
            
            FindClose(hSearch);
        }

        while (!foundFiles.empty())
        {
            // Processing the valid cFileName data now.
            TCHAR tzFile[MAX_PATH_LENGTH];
                    
            // adding (like above) filename to tzPath to set tzFile
            // for opening.
            m_trail.back().uLine = m_uLineNumber;
            if (tzFile == PathCombine(tzFile, tzPath, foundFiles.begin()->c_str()))
            {
                TRACE("Found and including: \"%ls\"", tzFile);
                
                FileParser fpParser(m_pSettingsMap, m_trail);
                fpParser.ParseFile(tzFile);
            }

            std::pop_heap(foundFiles.begin(), foundFiles.end(), fileComparer);
            foundFiles.pop_back();
        }
        
        TRACE("Done searching IncludeFolder (%ls, line %d): \"%ls\"",
            m_tzFullPath, m_uLineNumber, tzPath);
    }
#endif // LS_CUSTOM_INCLUDEFOLDER
    else
    {
        m_pSettingsMap->insert(SettingsMap::value_type(ptzName, SettingValue(ptzValue, false)));
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
        TRACE("Error parsing expression \"%ls\" (%ls, line %d)",
            ptzExpression, m_tzFullPath, m_uLineNumber);
        
        // Invalid syntax, so quit processing entire conditional block
        _SkipIf();
        return;
    }
    
    TRACE("Expression (%ls, line %d): \"%ls\" evaluated to %ls",
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
            if ((_tcsicmp(tzName, _T("else")) == 0) ||
                (_tcsicmp(tzName, _T("elseif")) == 0))
            {
                // After an ElseIf or Else, skip all lines until EndIf
                _SkipIf();
                break;
            }
            else if (_tcsicmp(tzName, _T("endif")) == 0)
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
            if (_tcsicmp(tzName, _T("if")) == 0)
            {
                // Nested Ifs are a special case
                _SkipIf();
            }
            else if (_tcsicmp(tzName, _T("elseif")) == 0)
            {
                // Handle ElseIfs by recursively calling ProcessIf
                _ProcessIf(tzValue);
                break;
            }
            else if (_tcsicmp(tzName, _T("else")) == 0)
            {
                // Since the If expression was false, when we see Else we
                // start processing lines until EndIf
                while (_ReadLineFromFile(tzName, tzValue))
                {
                    if (_tcsicmp(tzName, _T("elseif")) == 0)
                    {
                        // Error: ElseIf after Else
                        TRACE("Syntax Error (%ls, %d): "
                              "\"ElseIf\" directive after \"Else\"",
                            m_tzFullPath, m_uLineNumber);
                        
                        // Invalid syntax, so quit processing conditional block
                        _SkipIf();
                        break;
                    }
                    else if (_tcsicmp(tzName, _T("endif")) == 0)
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
            else if (_tcsicmp(tzName, _T("endif")) == 0)
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
        if (_tcsicmp(tzName, _T("if")) == 0)
        {
            _SkipIf();
        }
        else if (_tcsicmp(tzName, _T("endif")) == 0)
        {
            break;
        }
    }
}
