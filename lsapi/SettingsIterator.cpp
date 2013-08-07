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
#include "settingsiterator.h"
#include "settingsmanager.h"
#include "../utility/core.hpp"
#include <algorithm>

using std::wstring;


SettingsIterator::SettingsIterator(SettingsMap* pSettingsMap, const wstring& sPath)
{
    if (pSettingsMap)
    {
        m_pSettingsMap = pSettingsMap;
        m_pFileIterator = m_pSettingsMap->begin();
        
        if (sPath.length())
        {
            m_sPath = sPath;
        }
    }
}


BOOL SettingsIterator::ReadNextLine(LPWSTR pwzValue, size_t cchValue)
{
    BOOL bReturn = FALSE;
    
    if (pwzValue != nullptr && cchValue > 0)
    {
        if (m_pFileIterator != m_pSettingsMap->end())
        {
            StringCchCopyW(pwzValue, cchValue, m_pFileIterator->first.c_str());
            StringCchCatW(pwzValue, cchValue, L" ");
            StringCchCatW(pwzValue, cchValue, m_pFileIterator->second.sValue.c_str());
            ++m_pFileIterator;
            bReturn = TRUE;
        }
        else
        {
            pwzValue[0] = L'\0';
        }
    }
    
    return bReturn;
}


BOOL SettingsIterator::ReadNextCommand(LPWSTR pwzValue, size_t cchValue)
{
    BOOL bReturn = FALSE;
    
    while (!bReturn && m_pFileIterator != m_pSettingsMap->end())
    {
        if (!ispunct(*(m_pFileIterator->first.c_str())))
        {
            StringCchCopyW(pwzValue, cchValue, m_pFileIterator->first.c_str());
            StringCchCatW(pwzValue, cchValue, L" ");
            StringCchCatW(pwzValue, cchValue, m_pFileIterator->second.sValue.c_str());
            bReturn = TRUE;
        }
        
        ++m_pFileIterator;
    }
    
    return bReturn;
}


BOOL SettingsIterator::ReadNextConfig(LPCWSTR pwzConfig, LPWSTR pwzValue, size_t cchValue)
{
    BOOL bReturn = FALSE;
    
    if (pwzValue != nullptr && cchValue > 0 && pwzConfig != nullptr)
    {
        SettingsMap::iterator itSettings;
        
        pwzValue[0] = L'\0';
        
#if defined(LS_COMPAT_LCREADNEXTCONFIG)
        string sConfig;
        
        if(L'*' != *pwzConfig)
        {
            sConfig = L"*";
            sConfig += pwzConfig;
        }
        else
        {
            sConfig = pwzConfig;
        }
        
        pwzConfig = sConfig.c_str();
#endif // defined(LS_COMPAT_LCREADNEXTCONFIG)
        
        // Has ReadNextConfig been used before for pszConfig?
        IteratorMap::iterator it = m_Iterators.find(pwzConfig);
        
        if (it == m_Iterators.end())
        {
            // No, so find the first item with a key of pszConfig
            itSettings = m_pSettingsMap->lower_bound(pwzConfig);
            
            if (_wcsicmp(itSettings->first.c_str(), pwzConfig) == 0)
            {
                // Save the iterator for future use and return the value
                it = (m_Iterators.insert(
                    IteratorMap::value_type(pwzConfig, itSettings)
                )).first;
                
                bReturn = TRUE;
            }
        }
        else
        {
            // Yes so find the last item with a matching key
            itSettings = m_pSettingsMap->upper_bound(pwzConfig);
            
            // Loop until we either find an item with a matching key or the
            // last matching item
            do
            {
                ++it->second;
            } while ((_wcsicmp(pwzConfig, it->first.c_str()) != 0) &&
                     (it->second != itSettings));
            
            // If we found a valid item, return it
            if (it->second != itSettings)
            {
                bReturn = TRUE;
            }
        }
        
        if (bReturn)
        {
            StringCchCopyW(pwzValue, cchValue, it->second->first.c_str());
            StringCchCatW(pwzValue, cchValue, L" ");
            StringCchCatW(pwzValue, cchValue, it->second->second.sValue.c_str());
        }
    }
    
    return bReturn;
}
