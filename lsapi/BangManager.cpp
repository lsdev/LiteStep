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
#include "BangManager.h"

BangManager::BangManager()
{
}

BangManager::~BangManager()
{
}


// Add a bang command to the manager
BOOL BangManager::AddBangCommand(LPCSTR pszName, Bang *pbbBang)
{
	Lock lock (cs);

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
	Lock lock (cs);
	BOOL bReturn = FALSE;

	BangMap::iterator iter = bang_map.find(pszName);

	if (iter != bang_map.end())
	{
		iter->second->Release();
		bang_map.erase(iter);

		bReturn = TRUE;
	}

	return bReturn;
}

// Retrieve a bang command from the manager
Bang* BangManager::GetBangCommand(LPCSTR pszName)
{
	Lock lock (cs);

	BangMap::const_iterator iter = bang_map.find(pszName);

	if (iter != bang_map.end())
	{
		return iter->second;
	}

	return NULL;
}

// Execute a bang command with the specified name, passing params, getting result
BOOL BangManager::ExecuteBangCommand(LPCSTR pszName, HWND hCaller, LPCSTR pszParams)
{
    // Not locking the BangManager any more due to unresolved issues. For
    // example calling a !bang which loads or unloads (threaded) modules
    // (!recycle, !reloadmodule...) would result in a deadlock if the BangManger
    // was locked here. !Bangs like !execute might also call !bangs in other
    // threads which would again be a deadlock.
    // It is not clear if this function even needs to be locked.
//	Lock lock (cs);

	BOOL bReturn = FALSE;

	BangMap::const_iterator iter = bang_map.find(pszName);

	if (iter != bang_map.end())
	{
		iter->second->Execute(hCaller, pszParams);

		bReturn = TRUE;
	}

	return bReturn;
}

void BangManager::ClearBangCommands()
{
	Lock lock (cs);

	BangMap::iterator iter = bang_map.begin();

	while (iter != bang_map.end())
	{
		iter->second->Release();
		++iter;
	}

	bang_map.clear();
}
