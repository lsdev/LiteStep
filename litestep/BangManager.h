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
#ifndef __BANGMANAGER_H
#define __BANGMANAGER_H

#include "../utility/common.h"
#include "../lsapi/BangCommand.h"
#include "../utility/criticalsection.h"
#include <map>
#include <string>

using namespace std;

struct stringicmp
{
	bool operator()(const string &s1, const string &s2) const
	{
		return (stricmp(s1.c_str(), s2.c_str()) < 0);
	}
};

typedef map<string, Bang*, stringicmp> BangMap;

class BangManager
{
private:
	BangMap bang_map;
	CRITICAL_SECTION cs;

public:
	BangManager();
	virtual ~BangManager();

	BOOL AddBangCommand(LPCSTR pszName, Bang *pbbBang);
	BOOL RemoveBangCommand(LPCSTR pszName);
	Bang* GetBangCommand(LPCSTR pszName);
	void ClearBangCommands();
	BOOL ExecuteBangCommand(LPCSTR pszName, HWND hCaller, LPCSTR pszParams);
	UINT GetBangCommandNames(LPCSTR pszNames, UINT nSize);
	STDMETHOD(get_Count)(PLONG plCount);

};

#endif // __BANGMANAGER_H
