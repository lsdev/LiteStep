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
#ifndef __MODULEMANAGER_H
#define __MODULEMANAGER_H

#include "../utility/common.h"
#include "module.h"
#include <map>
#include <string>

using namespace std;

typedef int (*ModuleInitExFunc) (HWND, HINSTANCE, LPCSTR);
typedef int (*ModuleQuitFunc) (HINSTANCE);

typedef map<string, class Module*> ModuleMap;

class ModuleManager
{
public:
	//static HANDLE thread_event;
	//static HANDLE load_quit_event;
	ModuleManager(HWND, LPCSTR);
	virtual ~ModuleManager();

	UINT LoadModules();
	BOOL LoadModule(LPCSTR pszLocation, DWORD dwFlags);
	BOOL QuitModules();
	BOOL QuitModule(LPCSTR pszLocation);
	UINT GetModuleList(LPSTR *lpszModules, DWORD dwSize);

	STDMETHOD(get_Count)( /*[out, retval]*/ long* pCount);

private:

	void _StartModules();

	ModuleMap m_ModuleMap;
	HWND hMainWindow;
	string appPath;
};

#endif // __MODULEMANAGER_H
