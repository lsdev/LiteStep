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
#include "module.h"
#include <list>


// might want to move these to lsapidefines.h
typedef int (*ModuleInitExFunc) (HWND, HINSTANCE, LPCSTR);
typedef int (*ModuleQuitFunc) (HINSTANCE);

typedef std::list<Module*> ModuleQueue;

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
	BOOL QuitModule(HINSTANCE hModule);
    BOOL QuitModule(LPCSTR pszLocation);
    BOOL ReloadModule(HINSTANCE hModule);

    HRESULT EnumModules(LSENUMMODULESPROC pfnCallback, LPARAM lParam) const;
	
private:
	UINT _LoadModules();
	UINT _StartModules(const ModuleQueue& mqModules);
	void _QuitModules();
	
	ModuleQueue::iterator _FindModule(LPCSTR pszLocation);
    ModuleQueue::iterator _FindModule(HINSTANCE hModule);
    Module* _MakeModule(LPCSTR pszLocation, DWORD dwFlags);
    
    void _WaitForModules(const HANDLE* pHandles, DWORD dwCount, HWND hWnd) const;
    
    ModuleQueue m_ModuleQueue;
    ILiteStep *m_pILiteStep;
    
    struct IsLocationEqual
    {
        IsLocationEqual(LPCSTR pszLocation) : m_pszLocation(pszLocation){}
        
        bool operator() (const Module* pModule) const
        {
            return (lstrcmpi(m_pszLocation, pModule->GetLocation()) == 0);
        }
        
    private:
        LPCSTR m_pszLocation;
    };
    
    struct IsInstanceEqual
    {
        IsInstanceEqual(HINSTANCE hInstance) : m_hInstance(hInstance) {}
        
        bool operator() (const Module* pModule) const
        {
            return (pModule->GetInstance() == m_hInstance);
        }
        
    private:
        HINSTANCE m_hInstance;
    };
};

#endif // __MODULEMANAGER_H
