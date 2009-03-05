//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//
// This is a part of the Litestep Shell source code.
//
// Copyright (C) 1997-2009  LiteStep Development Team
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


void SettingsManager::ParseFile(LPCSTR pszFileName)
{
    TRACE("Loading config file \"%s\"", pszFileName);
    
    FileParser fpParser(&m_SettingsMap);
    fpParser.ParseFile(pszFileName);
}


BOOL SettingsManager::_FindLine(LPCSTR pszName, SettingsMap::iterator &it)
{
    ASSERT(NULL != pszName);
    BOOL bReturn = FALSE;
    
    // first appearance of a setting takes effect
    it = m_SettingsMap.lower_bound(pszName);
    
    if (it != m_SettingsMap.end() && _stricmp(pszName, it->first.c_str()) == 0)
    {
        bReturn = TRUE;
    }
    
    return bReturn;
}


BOOL SettingsManager::GetRCString(LPCSTR pszKeyName, LPSTR pszValue, LPCSTR pszDefStr, int nMaxLen)
{
    SettingsMap::iterator it;
    BOOL bReturn = FALSE;
    
    if (pszValue)
    {
        pszValue[0] = '\0';
    }
    
    if (pszKeyName)
    {
        if (_FindLine(pszKeyName, it))
        {
            bReturn = TRUE;
            
            if (pszValue)
            {
                char szToken[MAX_LINE_LENGTH] = { 0 };
                GetToken(it->second.c_str(), szToken, NULL, FALSE);
                
                StringSet recursiveVarSet;
                recursiveVarSet.insert(pszKeyName);
                VarExpansionEx(pszValue, szToken, nMaxLen, recursiveVarSet);
            }
        }
        else if (pszDefStr && pszValue)
        {
            StringCchCopy(pszValue, nMaxLen, pszDefStr);
            
            // for compatibility reasons GetRCString and GetRCLine return FALSE
            // if the default value is returned.
            //bReturn = TRUE;
        }
    }
    
    return bReturn;
}


BOOL SettingsManager::GetRCLine(LPCSTR pszKeyName, LPSTR pszValue, int nMaxLen, LPCSTR pszDefStr)
{
    SettingsMap::iterator it;
    BOOL bReturn = FALSE;
    
    if (pszValue)
    {
        pszValue[0] = '\0';
    }
    
    if (pszKeyName)
    {
        if (_FindLine(pszKeyName, it))
        {
            bReturn = TRUE;
            
            if (pszValue)
            {
                // for compatibility reasons GetRCLine expands $evars$
                StringSet recursiveVarSet;
                recursiveVarSet.insert(pszKeyName);
                VarExpansionEx(pszValue, it->second.c_str(),
                    nMaxLen, recursiveVarSet);
            }
        }
        else if (pszDefStr && pszValue)
        {
            StringCchCopy(pszValue, nMaxLen, pszDefStr);
            
            // for compatibility reasons GetRCString and GetRCLine
            // return FALSE if the default value is returned.
            //bReturn = TRUE;
        }
    }
    
    return bReturn;
}


BOOL SettingsManager::GetRCBool(LPCSTR pszKeyName, BOOL bIfFound)
{
    SettingsMap::iterator it;
    
    if (pszKeyName && _FindLine(pszKeyName, it))
    {
        char szExpanded[MAX_LINE_LENGTH] = { 0 };
        char szToken[MAX_LINE_LENGTH] = { 0 };
        
        StringSet recursiveVarSet;
        recursiveVarSet.insert(pszKeyName);
        VarExpansionEx(szExpanded, it->second.c_str(),
            MAX_LINE_LENGTH, recursiveVarSet);
        
        if (GetToken(szExpanded, szToken, NULL, FALSE))
        {
            if (_stricmp(szToken, "off") &&
                _stricmp(szToken, "false") &&
                _stricmp(szToken, "no"))
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
    
    if (pszKeyName && _FindLine(pszKeyName, it))
    {
        char szToken[MAX_LINE_LENGTH] = { 0 };
        char szExpanded[MAX_LINE_LENGTH] = { 0 };
        
        StringSet recursiveVarSet;
        recursiveVarSet.insert(pszKeyName);
        VarExpansionEx(szExpanded, it->second.c_str(),
            MAX_LINE_LENGTH, recursiveVarSet);
        
        if (GetToken(szExpanded, szToken, NULL, FALSE))
        {
            if ((_stricmp(szToken, "off") == 0) ||
                (_stricmp(szToken, "false") == 0) ||
                (_stricmp(szToken, "no") == 0))
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
    int nValue = nDefault;
    
    if (pszKeyName && _FindLine(pszKeyName, it))
    {
        char szToken[MAX_LINE_LENGTH] = { 0 };
        char szExpanded[MAX_LINE_LENGTH] = { 0 };
        
        StringSet recursiveVarSet;
        recursiveVarSet.insert(pszKeyName);
        VarExpansionEx(szExpanded, it->second.c_str(),
            MAX_LINE_LENGTH, recursiveVarSet);
        
        if (GetToken(szExpanded, szToken, NULL, FALSE))
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
    
    if (pszKeyName && _FindLine(pszKeyName, it))
    {
        char szBuffer[MAX_LINE_LENGTH];
        char szFirst[MAX_LINE_LENGTH];
        char szSecond[MAX_LINE_LENGTH];
        char szThird[MAX_LINE_LENGTH];
        
        LPSTR lpszTokens[3] = { szFirst, szSecond, szThird };
        
        StringSet recursiveVarSet;
        recursiveVarSet.insert(pszKeyName);
        VarExpansionEx(szBuffer, it->second.c_str(),
            MAX_LINE_LENGTH, recursiveVarSet);
        
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
    // using GetRCString instead of GetRCLine here, again for compatibility
    // reasons. As a side effect this strips any "" quotes around the
    // variable's value.
    return GetRCString(pszKeyName, pszValue, NULL, dwLength);
}


void SettingsManager::SetVariable(LPCSTR pszKeyName, LPCSTR pszValue)
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


void SettingsManager::VarExpansionEx(LPSTR pszExpandedString, LPCSTR pszTemplate, size_t stLength)
{
    StringSet recursiveVarSet;
    VarExpansionEx(pszExpandedString, pszTemplate, stLength, recursiveVarSet);
}


void SettingsManager::VarExpansionEx(LPSTR pszExpandedString, LPCSTR pszTemplate, size_t stLength, const StringSet& recursiveVarSet)
{
    char szTempExpandedString[MAX_LINE_LENGTH] = { 0 };
    LPSTR pszTempExpandedString = szTempExpandedString;
    // available working length in szTempExpandedString
    DWORD cchTempExpanded = MAX_LINE_LENGTH;
    
    if ((pszTemplate != NULL) && (pszExpandedString != NULL) &&
        (cchTempExpanded > 0))
    {
        while ((*pszTemplate != '\0') && (cchTempExpanded > 0))
        {
            if (*pszTemplate != '$')
            {
                *pszTempExpandedString = *pszTemplate;
                ++pszTemplate;
                ++pszTempExpandedString;
                --cchTempExpanded;
            }
            else
            {
                //
                // This is a variable so we need to find the end of it:
                //
                ++pszTemplate;
                
                LPCSTR pszVariable = pszTemplate;
                
                while ((*pszTemplate != '$') && (*pszTemplate != '\0'))
                {
                    ++pszTemplate;
                }
                
                bool bSucceeded = false;
                
                if (*pszTemplate == '\0')
                {
                    bSucceeded = SUCCEEDED(StringCchCopyN(
                        pszTempExpandedString, (size_t)cchTempExpanded,
                        pszVariable, pszTemplate - pszVariable));
                }
                else
                {
                    //
                    // We've found the end of the variable so copy it
                    // someplace usefull:
                    //
                    char szVariable[MAX_LINE_LENGTH];
                    
                    if (SUCCEEDED(StringCchCopyN(szVariable, MAX_LINE_LENGTH,
                        pszVariable, pszTemplate - pszVariable)) &&
                        (szVariable[0] != '\0'))
                    {
                        // Check for recursive variable definitions
                        if (recursiveVarSet.count(szVariable) > 0)
                        {
                            RESOURCE_STREX(
                                GetModuleHandle(NULL), IDS_RECURSIVEVAR,
                                resourceTextBuffer, MAX_LINE_LENGTH,
                                "Error: Variable \"%s\" is defined recursively.",
                                szVariable);
                            
                            RESOURCE_MSGBOX_F("LiteStep", MB_ICONERROR);
                            
                            pszExpandedString[0] = '\0';
                            return;
                        }
                        
                        //
                        // Get the value, if we can.
                        //
                        SettingsMap::iterator it;
                        if (_FindLine(szVariable, it))
                        {
                            StringSet newRecursiveVarSet(recursiveVarSet);
                            newRecursiveVarSet.insert(szVariable);
                            
                            // FIXME: Should we not call GetToken here?!
                            TCHAR szTemp[MAX_LINE_LENGTH];
                            GetToken(it->second.c_str(), szTemp, NULL, FALSE);
                            
                            VarExpansionEx(pszTempExpandedString, szTemp,
                                (size_t)cchTempExpanded, newRecursiveVarSet);
                            
                            bSucceeded = true;
                        }
                        else if (GetEnvironmentVariable(szVariable,
                            pszTempExpandedString, cchTempExpanded))
                        {
                            bSucceeded = true;
                        }
#if defined(LS_COMPAT_MATH)
                        else
                        {
                            std::string result;
                            
                            if (MathEvaluateString(m_SettingsMap, szVariable,
                                result, recursiveVarSet,
                                MATH_EXCEPTION_ON_UNDEFINED |
                                MATH_VALUE_TO_COMPATIBLE_STRING))
                            {
                                StringCchCopy(pszTempExpandedString,
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
                    size_t stTempLen = strlen(pszTempExpandedString);
                    
                    // safe operation and cast because
                    // stTempLen < cchTempExpanded
                    cchTempExpanded -= (DWORD)stTempLen;
                    pszTempExpandedString += stTempLen;
                }
                
                //
                // Move to the next character if we didn't run out of space:
                //
                if (*pszTemplate != '\0')
                {
                    ++pszTemplate;
                }
            }
        }
        
        *pszTempExpandedString = '\0';
        StringCchCopy(pszExpandedString, stLength, szTempExpandedString);
    }
}


LPVOID SettingsManager::LCOpen(LPCSTR pszPath)
{
    LPVOID pFile = NULL;
    
    if (pszPath == NULL)
    {
        SettingsIterator* psiNew = new SettingsIterator(&m_SettingsMap, "\0");
        
        if (psiNew)
        {
            m_Iterators.insert(psiNew);
            pFile = (LPVOID)psiNew;
        }
    }
    else
    {
        char szPath[MAX_PATH] = { 0 };
        VarExpansionEx(szPath, pszPath, MAX_PATH);
        
        Lock lock(m_CritSection);
        
        FileMap::iterator it = m_FileMap.find(szPath);
        
        if (it == m_FileMap.end() && PathFileExists(szPath))
        {
            FileInfo* pFileInfo = new FileInfo;
            
            if (pFileInfo)
            {
                pFileInfo->m_pSettingsMap = new SettingsMap;
                pFileInfo->m_Count = 1;
                
                TRACE("Loading config file \"%s\"", szPath);
                
                FileParser fpParser(pFileInfo->m_pSettingsMap);
                fpParser.ParseFile(szPath);
                
                m_FileMap.insert(FileMap::value_type(szPath, pFileInfo));
                
                it = m_FileMap.find(szPath);
                ASSERT(it != m_FileMap.end());
            }
        }
        
        if (it != m_FileMap.end())
        {
            SettingsIterator * psiNew =
                new SettingsIterator(it->second->m_pSettingsMap, szPath);
            
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


BOOL SettingsManager::LCReadNextConfig(LPVOID pFile, LPCSTR pszConfig, LPSTR pszValue, size_t cchValue)
{
    BOOL bReturn = FALSE;
    char szTempValue[MAX_LINE_LENGTH];
    
    if (pFile != NULL && pszConfig != NULL &&
        pszValue != NULL && cchValue > 0)
    {
        IteratorSet::iterator it = m_Iterators.find((SettingsIterator*)pFile);
        if (it != m_Iterators.end())
        {
            bReturn = (*it)->ReadNextConfig(pszConfig,
                szTempValue, MAX_LINE_LENGTH);
            
            if (bReturn)
            {
                VarExpansionEx(pszValue, szTempValue, cchValue);
            }
        }
    }
    
    return bReturn;
}


BOOL SettingsManager::LCReadNextCommand(LPVOID pFile, LPSTR pszValue, size_t cchValue)
{
    BOOL bReturn = FALSE;
    char szTempValue[MAX_LINE_LENGTH];
    
    if (pFile != NULL && pszValue != NULL && cchValue > 0)
    {
        IteratorSet::iterator it = m_Iterators.find((SettingsIterator*)pFile);
        
        if (it != m_Iterators.end())
        {
            bReturn = (*it)->ReadNextCommand(szTempValue, MAX_LINE_LENGTH);
            
            if (bReturn)
            {
                VarExpansionEx(pszValue, szTempValue, cchValue);
            }
        }
    }
    
    return bReturn;
}


BOOL SettingsManager::LCReadNextLine(LPVOID pFile, LPSTR pszValue, size_t cchValue)
{
    BOOL bReturn = FALSE;
    char szTempValue[MAX_LINE_LENGTH];
    
    if (pFile != NULL && pszValue != NULL && cchValue > 0)
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
