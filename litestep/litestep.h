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
#ifndef __LITESTEP_H
#define __LITESTEP_H

#include "../utility/common.h"
#include "../utility/ILiteStep.h"
#include <vector>


// forward declarations
class IService;
class TrayService;
class DataStore;
class MessageManager;
class ModuleManager;


// RegisterShellHook flags
#define RSH_UNREGISTER	0
#define RSH_REGISTER	1
#define RSH_PROGMAN		2
#define RSH_TASKMAN		3


// Program Options
const LPCSTR szMainWindowClass = "TApplication";
const LPCSTR szMainWindowTitle = "LiteStep";

#define GWL_CLASSPOINTER	0


class CLiteStep: public ILiteStep
{
public:
	CLiteStep();
	~CLiteStep();

	HRESULT Start(LPCSTR pszAppPath, LPCSTR pszRcPath, HINSTANCE hInstance, BOOL bRunStartup);

	LRESULT ExternalWndProc (HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

	// ILiteStep
	STDMETHOD(get_Window)(/*[out, retval]*/ long* phWnd);
	STDMETHOD(get_AppPath)(/*[out, retval]*/ LPSTR pszPath, /*[in]*/ size_t cchPath);

private:

	static LRESULT CALLBACK InternalWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

	void _Recycle();

	CHAR m_szAppPath[MAX_PATH];
	CHAR m_szRcPath[MAX_PATH];

	// Application instance
	HINSTANCE m_hInstance;

	BOOL m_bAutoHideModules;
    BOOL m_bAppIsFullScreen; // = FALSE;		// Is there a full screen app active?

	// Windows
	HWND m_hMainWindow; // = NULL;

	// Shell hooks
	UINT WM_ShellHook; // = 0;

	//
	// Manager Related
	//
	ModuleManager* m_pModuleManager; // = NULL;
	DataStore* m_pDataStoreManager; // = NULL;
	MessageManager* m_pMessageManager; // = NULL;

	HRESULT _InitManagers();
	HRESULT _StartManagers();
	HRESULT _StopManagers();
	void _CleanupManagers();

	BOOL bHookManagerStarted; // = FALSE;

	//
	// Service Related
	//
	TrayService* m_pTrayService; // = NULL;
    std::vector<IService*> m_Services;
    
	HRESULT _InitServices();
	HRESULT _StartServices();
	HRESULT _StopServices();
	void _CleanupServices();
};

#endif	// __LITESTEP_H





















