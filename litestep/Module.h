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
#ifndef __MODULE_H
#define __MODULE_H

#include "../utility/common.h"
#include <string>

using namespace std;

#define MODULE_BAD_PATH		0x0001
#define MODULE_BAD_INIT		0x0002
#define MODULE_BAD_QUIT		0x0004

#define MODULE_THREADED		0x0001
#define MODULE_NOTPUMPED	0x0002

typedef int (*ModuleInitExFunc) (HWND, HINSTANCE, LPCSTR);
typedef int (*ModuleQuitFunc) (HINSTANCE);

class Module
{
private:
	HINSTANCE m_hInstance;
	HANDLE m_hThread;
	HWND m_hMainWindow;
	HWND m_hThreadWnd;
	DWORD m_dwThreadID;

	string m_szLocation;
	string m_szAppPath;


	ModuleInitExFunc m_pInitEx;
	ModuleQuitFunc m_pQuit;

	DWORD m_dwFlags;

	HANDLE m_hInitEvent;
	HANDLE m_hQuitEvent;

public:
	void HandleThreadMessage(MSG &msg);

	Module(LPCSTR pszLoc, DWORD dwFlags);
	virtual ~Module();

	HANDLE Init(HWND hMainWindow, LPCSTR pszAppPath);
	HANDLE Quit();

	static ULONG __stdcall ThreadProc(void* dllModPtr);

	HINSTANCE GetInstance()
	{
		return m_hInstance;
	};

	HANDLE GetThread()
	{
		return m_hThread;
	};

	HANDLE GetInitEvent()
	{
		return m_hInitEvent;
	};

	HANDLE GetQuitEvent()
	{
		return m_hQuitEvent;
	};

	LPCSTR GetLocation()
	{
		return m_szLocation.c_str();
	};

	BOOL HasMessagePump()
	{
		return !(m_dwFlags & MODULE_NOTPUMPED);
	};

	ModuleQuitFunc GetQuit()
	{
		return m_pQuit;
	};
	ModuleInitExFunc GetInitEx()
	{
		return m_pInitEx;
	};


private:
	ULONG CallInit();
};

#endif // __MODULE_H
