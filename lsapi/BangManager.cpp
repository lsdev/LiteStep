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
#include "BangManager.h"
#include "../utility/debug.hpp"


BangManager::BangManager()
{
    // do nothing
}


BangManager::~BangManager()
{
    // do nothing
}


// Add a bang command to the manager
BOOL BangManager::AddBangCommand(LPCSTR pszName, Bang *pbbBang)
{
    Lock lock(m_cs);
    
    BangMap::iterator iter = bang_map.find(pszName);
    
    if (iter != bang_map.end())
    {
        iter->second->Release();
        bang_map.erase(iter);
    }
    
    bang_map.insert(BangMap::value_type(pszName, pbbBang));
    pbbBang->AddRef();
    
    return TRUE;
}


// Remove a bang command from the manager
BOOL BangManager::RemoveBangCommand(LPCSTR pszName)
{
    Lock lock(m_cs);
    BOOL bReturn = FALSE;
    
    ASSERT(pszName != NULL);
    BangMap::iterator iter = bang_map.find(pszName);
    
    if (iter != bang_map.end())
    {
        iter->second->Release();
        bang_map.erase(iter);
        
        bReturn = TRUE;
    }
    
    return bReturn;
}


// Execute named bang command, passing params, getting result
BOOL BangManager::ExecuteBangCommand(LPCSTR pszName, HWND hCaller, LPCSTR pszParams)
{
    BOOL bReturn = FALSE;
    Bang* pToExec = NULL;
    
    // Acquiring lock manually to allow manual release below
    m_cs.Acquire();
    
    BangMap::const_iterator iter = bang_map.find(pszName);
    
    if (iter != bang_map.end())
    {
        pToExec = iter->second;
        pToExec->AddRef();
    }
    
    // Release lock before executing the !bang since the BangProc might
    // (recursively) enter this function again
    m_cs.Release();
    
    if (pToExec)
    {
        pToExec->Execute(hCaller, pszParams);
        pToExec->Release();
        
        bReturn = TRUE;
    }
    
    return bReturn;
}


void BangManager::ClearBangCommands()
{
    Lock lock(m_cs);
    
    BangMap::iterator iter = bang_map.begin();
    
    while (iter != bang_map.end())
    {
        iter->second->Release();
        ++iter;
    }
    
    bang_map.clear();
}


HRESULT BangManager::EnumBangs(LSENUMBANGSV2PROC pfnCallback, LPARAM lParam) const
{
    Lock lock(m_cs);
    
    HRESULT hr = S_OK;
    
    for (BangMap::const_iterator iter = bang_map.begin();
        iter != bang_map.end(); ++iter)
    {
        HMODULE hModule = iter->second->GetModule();
        
        if (!pfnCallback(hModule, iter->first.c_str(), lParam))
        {
            hr = S_FALSE;
            break;
        }
    }
    
    return hr;
}
