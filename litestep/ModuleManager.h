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
#include "../utility/IManager.h"
#include "../utility/stringutility.h"
#include "module.h"
#include <map>
#include <string>

using namespace std;

// might want to move these to lsapidefines.h
typedef int (*ModuleInitExFunc) (HWND, HINSTANCE, LPCSTR);
typedef int (*ModuleQuitFunc) (HINSTANCE);

typedef map<string, Module*, stringicmp> ModuleMap;

class ModuleManager: public IManager
{
public:
	ModuleManager();
	virtual ~ModuleManager();

	// IManager
	HRESULT Start(ILiteStep *ILiteStep);
	HRESULT Stop();

	HRESULT rStart();
	HRESULT rStop();

	BOOL LoadModule(LPCSTR pszLocation, DWORD dwFlags);
	BOOL QuitModule(LPCSTR pszLocation);
	UINT GetModuleList(LPSTR *lpszModules, DWORD dwSize);

	STDMETHOD(get_Count)( /*[out, retval]*/ long* pCount);

private:
	UINT _LoadModules();
	void _StartModules();
	void _QuitModules();

	ModuleMap m_ModuleMap;
	ILiteStep *m_pILiteStep;
};

#endif // __MODULEMANAGER_H
