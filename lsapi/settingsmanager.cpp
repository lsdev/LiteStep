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
#include "SettingsManager.h"
#include "SettingsFileParser.h"
#include "MathEvaluate.h"
#include "../utility/macros.h"
#include "../utility/core.hpp"


SettingsManager::SettingsManager()
{
    // do nothing
}


SettingsManager::~SettingsManager()
{
    Lock lock(m_CritSection);
    
    // check if nasty modules forgot to call LCClose
    for (IteratorSet::iterator itSet = m_Iterators.begin();
         itSet != m_Iterators.end(); ++itSet)
    {
        delete *itSet;
    }
    
    for (FileMap::iterator itFiles = m_FileMap.begin();
         itFiles != m_FileMap.end(); ++itFiles)
    {
        if (itFiles->second->m_Count == 1)
        {
            delete itFiles->second->m_pSettingsMap;
            delete itFiles->second;
        }
        else
        {
            --itFiles->second->m_Count;
        }
    }
}


void SettingsManager::ParseFile(LPCWSTR pwzFileName)
{
    TRACE("Loading config file \"%ls\"", pwzFileName);
    
    FileParser fpParser(&m_SettingsMap);
    fpParser.ParseFile(pwzFileName);
}


BOOL SettingsManager::_FindLine(LPCWSTR pwzName, SettingsMap::iterator &it)
{
    ASSERT(NULL != pwzName);
    BOOL bReturn = FALSE;
    
    // first appearance of a setting takes effect
    it = m_SettingsMap.lower_bound(pwzName);
    
    if (it != m_SettingsMap.end() && _wcsicmp(pwzName, it->first.c_str()) == 0)
    {
        bReturn = TRUE;
    }
    
    return bReturn;
}


BOOL SettingsManager::GetRCString(LPCWSTR pwzKeyName, LPWSTR pwzValue, LPCWSTR pwzDefStr, int nMaxLen)
{
    SettingsMap::iterator it;
    BOOL bReturn = FALSE;
    
    if (pwzValue)
    {
        pwzValue[0] = L'\0';
    }
    
    if (pwzKeyName)
    {
        if (_FindLine(pwzKeyName, it))
        {
            bReturn = TRUE;
            
            if (pwzValue)
            {
                wchar_t wzToken[MAX_LINE_LENGTH] = { 0 };
                GetTokenW(it->second.c_str(), wzToken, NULL, FALSE);
                
                StringSet recursiveVarSet;
                recursiveVarSet.insert(pwzKeyName);
                VarExpansionEx(pwzValue, wzToken, nMaxLen, recursiveVarSet);
            }
        }
        else if (pwzDefStr && pwzValue)
        {
            StringCchCopyW(pwzValue, nMaxLen, pwzDefStr);
            
            // for compatibility reasons GetRCString and GetRCLine return FALSE
            // if the default value is returned.
            //bReturn = TRUE;
        }
    }
    
    return bReturn;
}


BOOL SettingsManager::GetRCLine(LPCWSTR pwzKeyName, LPWSTR pwzValue, int nMaxLen, LPCWSTR pwzDefStr)
{
    SettingsMap::iterator it;
    BOOL bReturn = FALSE;
    
    if (pwzValue)
    {
        pwzValue[0] = L'\0';
    }
    
    if (pwzKeyName)
    {
        if (_FindLine(pwzKeyName, it))
        {
            bReturn = TRUE;
            
            if (pwzValue)
            {
                // for compatibility reasons GetRCLine expands $evars$
                StringSet recursiveVarSet;
                recursiveVarSet.insert(pwzKeyName);
                VarExpansionEx(pwzValue, it->second.c_str(),
                    nMaxLen, recursiveVarSet);
            }
        }
        else if (pwzDefStr && pwzValue)
        {
            StringCchCopyW(pwzValue, nMaxLen, pwzDefStr);
            
            // for compatibility reasons GetRCString and GetRCLine
            // return FALSE if the default value is returned.
            //bReturn = TRUE;
        }
    }
    
    return bReturn;
}


BOOL SettingsManager::GetRCBool(LPCWSTR pwzKeyName, BOOL bIfFound)
{
    SettingsMap::iterator it;
    
    if (pwzKeyName && _FindLine(pwzKeyName, it))
    {
        wchar_t wzExpanded[MAX_LINE_LENGTH] = { 0 };
        wchar_t wzToken[MAX_LINE_LENGTH] = { 0 };
        
        StringSet recursiveVarSet;
        recursiveVarSet.insert(pwzKeyName);
        VarExpansionEx(wzExpanded, it->second.c_str(),
            MAX_LINE_LENGTH, recursiveVarSet);
        
        if (GetTokenW(wzExpanded, wzToken, nullptr, FALSE))
        {
            if (_wcsicmp(wzToken, L"off") &&
                _wcsicmp(wzToken, L"false") &&
                _wcsicmp(wzToken, L"no"))
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


BOOL SettingsManager::GetRCBoolDef(LPCWSTR pwzKeyName, BOOL bDefault)
{
    SettingsMap::iterator it;
    
    if (pwzKeyName && _FindLine(pwzKeyName, it))
    {
        wchar_t wzToken[MAX_LINE_LENGTH] = { 0 };
        wchar_t wzExpanded[MAX_LINE_LENGTH] = { 0 };
        
        StringSet recursiveVarSet;
        recursiveVarSet.insert(pwzKeyName);
        VarExpansionEx(wzExpanded, it->second.c_str(),
            MAX_LINE_LENGTH, recursiveVarSet);
        
        if (GetTokenW(wzExpanded, wzToken, NULL, FALSE))
        {
            if ((_wcsicmp(wzToken, L"off") == 0) ||
                (_wcsicmp(wzToken, L"false") == 0) ||
                (_wcsicmp(wzToken, L"no") == 0))
            {
                return FALSE;
            }
        }
        
        return TRUE;
    }
    
    return bDefault;
}


__int64 SettingsManager::GetRCInt64(LPCWSTR pszKeyName, __int64 nDefault)
{
    SettingsMap::iterator it;
    __int64 nValue = nDefault;
    
    if (pszKeyName && _FindLine(pszKeyName, it))
    {
        wchar_t wzToken[MAX_LINE_LENGTH] = { 0 };
        wchar_t wzExpanded[MAX_LINE_LENGTH] = { 0 };
        
        StringSet recursiveVarSet;
        recursiveVarSet.insert(pszKeyName);
        VarExpansionEx(wzExpanded, it->second.c_str(),
            MAX_LINE_LENGTH, recursiveVarSet);
        
        if (GetTokenW(wzExpanded, wzToken, nullptr, FALSE))
        {
            nValue = _wcstoi64(wzToken, nullptr, 0);
        }
    }
    
    return nValue;
}


int SettingsManager::GetRCInt(LPCWSTR pszKeyName, int nDefault)
{
    SettingsMap::iterator it;
    int nValue = nDefault;
    
    if (pszKeyName && _FindLine(pszKeyName, it))
    {
        wchar_t wzToken[MAX_LINE_LENGTH] = { 0 };
        wchar_t wzExpanded[MAX_LINE_LENGTH] = { 0 };
        
        StringSet recursiveVarSet;
        recursiveVarSet.insert(pszKeyName);
        VarExpansionEx(wzExpanded, it->second.c_str(),
            MAX_LINE_LENGTH, recursiveVarSet);
        
        if (GetTokenW(wzExpanded, wzToken, nullptr, FALSE))
        {
            nValue = wcstol(wzToken, nullptr, 0);
        }
    }
    
    return nValue;
}


float SettingsManager::GetRCFloat(LPCWSTR pszKeyName, float fDefault)
{
    SettingsMap::iterator it;
    float fValue = fDefault;
    
    if (pszKeyName && _FindLine(pszKeyName, it))
    {
        wchar_t wzToken[MAX_LINE_LENGTH] = { 0 };
        wchar_t wzExpanded[MAX_LINE_LENGTH] = { 0 };
        
        StringSet recursiveVarSet;
        recursiveVarSet.insert(pszKeyName);
        VarExpansionEx(wzExpanded, it->second.c_str(),
            MAX_LINE_LENGTH, recursiveVarSet);
        
        if (GetTokenW(wzExpanded, wzToken, nullptr, FALSE))
        {
            fValue = (float)wcstod(wzToken, nullptr);
        }
    }
    
    return fValue;
}


double SettingsManager::GetRCDouble(LPCWSTR pszKeyName, double dDefault)
{
    SettingsMap::iterator it;
    double dValue = dDefault;
    
    if (pszKeyName && _FindLine(pszKeyName, it))
    {
        wchar_t wzToken[MAX_LINE_LENGTH] = { 0 };
        wchar_t wzExpanded[MAX_LINE_LENGTH] = { 0 };
        
        StringSet recursiveVarSet;
        recursiveVarSet.insert(pszKeyName);
        VarExpansionEx(wzExpanded, it->second.c_str(),
            MAX_LINE_LENGTH, recursiveVarSet);
        
        if (GetTokenW(wzExpanded, wzToken, nullptr, FALSE))
        {
            dValue = wcstod(wzToken, nullptr);
        }
    }
    
    return dValue;
}


COLORREF SettingsManager::GetRCColor(LPCWSTR pszKeyName, COLORREF crDefault)
{
    COLORREF crReturn = crDefault;
    SettingsMap::iterator it;
    
    if (pszKeyName && _FindLine(pszKeyName, it))
    {
        wchar_t wzBuffer[MAX_LINE_LENGTH];
        wchar_t wzFirst[MAX_LINE_LENGTH];
        wchar_t wzSecond[MAX_LINE_LENGTH];
        wchar_t wzThird[MAX_LINE_LENGTH];
        
        LPWSTR lpwzTokens[3] = { wzFirst, wzSecond, wzThird };
        
        StringSet recursiveVarSet;
        recursiveVarSet.insert(pszKeyName);
        VarExpansionEx(wzBuffer, it->second.c_str(),
            MAX_LINE_LENGTH, recursiveVarSet);
        
        int nCount = LCTokenizeW(wzBuffer, lpwzTokens, 3, nullptr);
        
        if (nCount >= 3)
        {
            int nRed, nGreen, nBlue;
            
            nRed = wcstol(wzFirst, nullptr, 10);
            nGreen = wcstol(wzSecond, nullptr, 10);
            nBlue = wcstol(wzThird, nullptr, 10);
            
            crReturn = RGB(nRed, nGreen, nBlue);
        }
        else if (nCount >= 1)
        {
            crReturn = wcstol(wzFirst, nullptr, 16);
            // convert from BGR to RGB
            crReturn = RGB(GetBValue(crReturn), GetGValue(crReturn),
                           GetRValue(crReturn));
        }
    }
    
    return crReturn;
}


BOOL SettingsManager::GetVariable(LPCWSTR pszKeyName, LPWSTR pszValue, DWORD dwLength)
{
    // using GetRCString instead of GetRCLine here, again for compatibility
    // reasons. As a side effect this strips any "" quotes around the
    // variable's value.
    return GetRCString(pszKeyName, pszValue, NULL, dwLength);
}


void SettingsManager::SetVariable(LPCWSTR pszKeyName, LPCWSTR pszValue)
{
    if ((pszKeyName) && (pszValue))
    {
        // in order for LSSetVariable to work evars must be redefinable
        SettingsMap::iterator it;
        if (_FindLine(pszKeyName, it))
        {
            it->second.assign(pszValue);
        }
        else
        {
            m_SettingsMap.insert(SettingsMap::value_type(pszKeyName, pszValue));
        }
    }
}


void SettingsManager::VarExpansionEx(LPWSTR pwzExpandedString, LPCWSTR pwzTemplate, size_t stLength)
{
    StringSet recursiveVarSet;
    VarExpansionEx(pwzExpandedString, pwzTemplate, stLength, recursiveVarSet);
}


void SettingsManager::VarExpansionEx(LPWSTR pwzExpandedString, LPCWSTR pwzTemplate, size_t stLength, const StringSet& recursiveVarSet)
{
    wchar_t wzTempExpandedString[MAX_LINE_LENGTH] = { 0 };
    LPWSTR pwzTempExpandedString = wzTempExpandedString;
    // available working length in szTempExpandedString
    DWORD cchTempExpanded = MAX_LINE_LENGTH;
    
    if ((pwzTemplate != nullptr) && (pwzExpandedString != nullptr) &&
        (cchTempExpanded > 0))
    {
        while ((*pwzTemplate != L'\0') && (cchTempExpanded > 0))
        {
            if (*pwzTemplate != L'$')
            {
                *pwzTempExpandedString = *pwzTemplate;
                ++pwzTemplate;
                ++pwzTempExpandedString;
                --cchTempExpanded;
            }
            else
            {
                //
                // This is a variable so we need to find the end of it:
                //
                ++pwzTemplate;
                
                LPCWSTR pwzVariable = pwzTemplate;
                
                while ((*pwzTemplate != L'$') && (*pwzTemplate != L'\0'))
                {
                    ++pwzTemplate;
                }
                
                bool bSucceeded = false;
                
                if (*pwzTemplate == L'\0')
                {
                    bSucceeded = SUCCEEDED(StringCchCopyNW(
                        pwzTempExpandedString, (size_t)cchTempExpanded,
                        pwzVariable, pwzTemplate - pwzVariable));
                }
                else
                {
                    //
                    // We've found the end of the variable so copy it
                    // someplace usefull:
                    //
                    wchar_t wzVariable[MAX_LINE_LENGTH];
                    
                    if (SUCCEEDED(StringCchCopyNW(wzVariable, MAX_LINE_LENGTH,
                        pwzVariable, pwzTemplate - pwzVariable)) &&
                        (wzVariable[0] != L'\0'))
                    {
                        // Check for recursive variable definitions
                        if (recursiveVarSet.count(wzVariable) > 0)
                        {
                            RESOURCE_STREX(
                                GetModuleHandle(NULL), IDS_RECURSIVEVAR,
                                resourceTextBuffer, MAX_LINE_LENGTH,
                                L"Error: Variable \"%ls\" is defined recursively.",
                                wzVariable);
                            
                            RESOURCE_MSGBOX_F(L"LiteStep", MB_ICONERROR);
                            
                            pwzExpandedString[0] = L'\0';
                            return;
                        }
                        
                        //
                        // Get the value, if we can.
                        //
                        SettingsMap::iterator it;
                        if (_FindLine(wzVariable, it))
                        {
                            StringSet newRecursiveVarSet(recursiveVarSet);
                            newRecursiveVarSet.insert(wzVariable);
                            
                            // FIXME: Should we not call GetToken here?!
                            WCHAR wzTemp[MAX_LINE_LENGTH];
                            GetTokenW(it->second.c_str(), wzTemp, NULL, FALSE);
                            
                            VarExpansionEx(pwzTempExpandedString, wzTemp,
                                (size_t)cchTempExpanded, newRecursiveVarSet);
                            
                            bSucceeded = true;
                        }
                        else if (GetEnvironmentVariableW(wzVariable,
                            pwzTempExpandedString, cchTempExpanded))
                        {
                            bSucceeded = true;
                        }
#if defined(LS_COMPAT_MATH)
                        else
                        {
                            std::wstring result;
                            
                            if (MathEvaluateString(m_SettingsMap, wzVariable,
                                result, recursiveVarSet,
                                MATH_EXCEPTION_ON_UNDEFINED |
                                MATH_VALUE_TO_COMPATIBLE_STRING))
                            {
                                StringCchCopyW(pwzTempExpandedString,
                                    (size_t)cchTempExpanded, result.c_str());
                                bSucceeded = true;
                            }
                        }
#endif // LS_COMPAT_MATH
                    }
                }
                
                //
                // If we succeeded, adjust our output buffers
                // accordingly:
                //
                if (bSucceeded)
                {
                    size_t stTempLen = wcslen(pwzTempExpandedString);
                    
                    // safe operation and cast because
                    // stTempLen < cchTempExpanded
                    cchTempExpanded -= (DWORD)stTempLen;
                    pwzTempExpandedString += stTempLen;
                }
                
                //
                // Move to the next character if we didn't run out of space:
                //
                if (*pwzTemplate != L'\0')
                {
                    ++pwzTemplate;
                }
            }
        }
        
        *pwzTempExpandedString = L'\0';
        StringCchCopyW(pwzExpandedString, stLength, wzTempExpandedString);
    }
}


LPVOID SettingsManager::LCOpen(LPCWSTR pwzPath)
{
    LPVOID pFile = nullptr;
    
    if (pwzPath == nullptr)
    {
        SettingsIterator* psiNew = new SettingsIterator(&m_SettingsMap, L"\0");
        
        if (psiNew)
        {
            m_Iterators.insert(psiNew);
            pFile = (LPVOID)psiNew;
        }
    }
    else
    {
        wchar_t wzPath[MAX_PATH] = { 0 };
        VarExpansionEx(wzPath, pwzPath, MAX_PATH);
        
        Lock lock(m_CritSection);
        
        FileMap::iterator it = m_FileMap.find(wzPath);
        
        if (it == m_FileMap.end() && PathFileExistsW(wzPath))
        {
            FileInfo* pFileInfo = new FileInfo;
            
            if (pFileInfo)
            {
                pFileInfo->m_pSettingsMap = new SettingsMap;
                pFileInfo->m_Count = 1;
                
                TRACE("Loading config file \"%ls\"", wzPath);
                
                FileParser fpParser(pFileInfo->m_pSettingsMap);
                fpParser.ParseFile(wzPath);
                
                m_FileMap.insert(FileMap::value_type(wzPath, pFileInfo));
                
                it = m_FileMap.find(wzPath);
                ASSERT(it != m_FileMap.end());
            }
        }
        
        if (it != m_FileMap.end())
        {
            SettingsIterator * psiNew =
                new SettingsIterator(it->second->m_pSettingsMap, wzPath);
            
            if (psiNew)
            {
                ++it->second->m_Count;
                m_Iterators.insert(psiNew);
                pFile = (LPVOID)psiNew;
            }
        }
    }
    
    return pFile;
}


BOOL SettingsManager::LCClose(LPVOID pFile)
{
    BOOL bReturn = FALSE;
    
    if (pFile != NULL)
    {
        Lock lock(m_CritSection);
        
        IteratorSet::iterator it = m_Iterators.find((SettingsIterator*)pFile);
        
        if (it != m_Iterators.end())
        {
            FileMap::iterator fmIt = m_FileMap.find((*it)->getPath());
            
            if (fmIt != m_FileMap.end())
            {
                if (fmIt->second->m_Count == 1)
                {
                    delete fmIt->second->m_pSettingsMap;
                    delete fmIt->second;
                    
                    m_FileMap.erase(fmIt);
                }
                else
                {
                    --fmIt->second->m_Count;
                }
            }
            
            delete (*it);
            m_Iterators.erase(it);
        }
        
        bReturn = TRUE;
    }
    
    return bReturn;
}


BOOL SettingsManager::LCReadNextConfig(LPVOID pFile, LPCWSTR pwzConfig, LPWSTR pwzValue, size_t cchValue)
{
    BOOL bReturn = FALSE;
    wchar_t wzTempValue[MAX_LINE_LENGTH];
    
    if (pFile != nullptr && pwzConfig != nullptr &&
        pwzValue != nullptr && cchValue > 0)
    {
        IteratorSet::iterator it = m_Iterators.find((SettingsIterator*)pFile);
        if (it != m_Iterators.end())
        {
            bReturn = (*it)->ReadNextConfig(pwzConfig,
                wzTempValue, MAX_LINE_LENGTH);
            
            if (bReturn)
            {
                VarExpansionEx(pwzValue, wzTempValue, cchValue);
            }
        }
    }
    
    return bReturn;
}


BOOL SettingsManager::LCReadNextCommand(LPVOID pFile, LPWSTR pwzValue, size_t cchValue)
{
    BOOL bReturn = FALSE;
    wchar_t wzTempValue[MAX_LINE_LENGTH];
    
    if (pFile != nullptr && pwzValue != nullptr && cchValue > 0)
    {
        IteratorSet::iterator it = m_Iterators.find((SettingsIterator*)pFile);
        
        if (it != m_Iterators.end())
        {
            bReturn = (*it)->ReadNextCommand(wzTempValue, MAX_LINE_LENGTH);
            
            if (bReturn)
            {
                VarExpansionEx(pwzValue, wzTempValue, cchValue);
            }
        }
    }
    
    return bReturn;
}


BOOL SettingsManager::LCReadNextLine(LPVOID pFile, LPWSTR pwzValue, size_t cchValue)
{
    BOOL bReturn = FALSE;
    wchar_t wzTempValue[MAX_LINE_LENGTH];
    
    if (pFile != nullptr && pwzValue != nullptr && cchValue > 0)
    {
        IteratorSet::iterator it = m_Iterators.find((SettingsIterator*)pFile);
        
        if (it != m_Iterators.end())
        {
            bReturn = (*it)->ReadNextLine(wzTempValue, MAX_LINE_LENGTH);
            
            if (bReturn)
            {
                VarExpansionEx(pwzValue, wzTempValue, cchValue);
            }
        }
    }
    
    return bReturn;
}
