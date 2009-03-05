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
#include "settingsiterator.h"
#include "settingsmanager.h"
#include "../utility/core.hpp"
#include <algorithm>

using std::string;


SettingsIterator::SettingsIterator(SettingsMap* pSettingsMap, const string& szPath)
{
    if (pSettingsMap)
    {
        m_pSettingsMap = pSettingsMap;
        m_pFileIterator = m_pSettingsMap->begin();
        
        if (szPath.length())
        {
            m_Path = szPath;
        }
    }
}


BOOL SettingsIterator::ReadNextLine(LPSTR pszValue, size_t cchValue)
{
    BOOL bReturn = FALSE;
    
    if (pszValue != NULL && cchValue > 0)
    {
        if (m_pFileIterator != m_pSettingsMap->end())
        {
            StringCchCopy(pszValue, cchValue, m_pFileIterator->first.c_str());
            StringCchCat(pszValue, cchValue, " ");
            StringCchCat(pszValue, cchValue, m_pFileIterator->second.c_str());
            ++m_pFileIterator;
            bReturn = TRUE;
        }
        else
        {
            pszValue[0] = '\0';
        }
    }
    
    return bReturn;
}


BOOL SettingsIterator::ReadNextCommand(LPSTR pszValue, size_t cchValue)
{
    BOOL bReturn = FALSE;
    
    while (!bReturn && m_pFileIterator != m_pSettingsMap->end())
    {
        if (!ispunct(*(m_pFileIterator->first.c_str())))
        {
            StringCchCopy(pszValue, cchValue, m_pFileIterator->first.c_str());
            StringCchCat(pszValue, cchValue, " ");
            StringCchCat(pszValue, cchValue, m_pFileIterator->second.c_str());
            bReturn = TRUE;
        }
        
        ++m_pFileIterator;
    }
    
    return bReturn;
}


BOOL SettingsIterator::ReadNextConfig(LPCSTR pszConfig, LPSTR pszValue, size_t cchValue)
{
    BOOL bReturn = FALSE;
    
    if (pszValue != NULL && cchValue > 0 && pszConfig != NULL)
    {
        SettingsMap::iterator itSettings;
        
        pszValue[0] = '\0';
        
#if defined(LS_COMPAT_LCREADNEXTCONFIG)
        string sConfig;
        
        if('*' != *pszConfig)
        {
            sConfig = "*";
            sConfig += pszConfig;
        }
        else
        {
            sConfig = pszConfig;
        }
        
        pszConfig = sConfig.c_str();
#endif // defined(LS_COMPAT_LCREADNEXTCONFIG)
        
        // Has ReadNextConfig been used before for pszConfig?
        IteratorMap::iterator it = m_Iterators.find(pszConfig);
        
        if (it == m_Iterators.end())
        {
            // No, so find the first item with a key of pszConfig
            itSettings = m_pSettingsMap->lower_bound(pszConfig);
            
            if (_stricmp(itSettings->first.c_str(), pszConfig) == 0)
            {
                // Save the iterator for future use and return the value
                it = (m_Iterators.insert(
                    IteratorMap::value_type(pszConfig, itSettings)
                )).first;
                
                bReturn = TRUE;
            }
        }
        else
        {
            // Yes so find the last item with a matching key
            itSettings = m_pSettingsMap->upper_bound(pszConfig);
            
            // Loop until we either find an item with a matching key or the
            // last matching item
            do
            {
                ++it->second;
            } while ((_stricmp(pszConfig, it->first.c_str()) != 0) &&
                     (it->second != itSettings));
            
            // If we found a valid item, return it
            if (it->second != itSettings)
            {
                bReturn = TRUE;
            }
        }
        
        if (bReturn)
        {
            StringCchCopy(pszValue, cchValue, it->second->first.c_str());
            StringCchCat(pszValue, cchValue, " ");
            StringCchCat(pszValue, cchValue, it->second->second.c_str());
        }
    }
    
    return bReturn;
}
