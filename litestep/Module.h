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

    std::basic_string<TCHAR> m_tzLocation;
    std::basic_string<TCHAR> m_tzAppPath;


	ModuleInitExFunc m_pInitEx;
	ModuleQuitFunc m_pQuit;

	DWORD m_dwFlags;

	HANDLE m_hInitEvent;
	HANDLE m_hQuitEvent;

public:
	void HandleThreadMessage(MSG &msg);

	Module(LPCTSTR ptzLoc, DWORD dwFlags);
	virtual ~Module();

	BOOL LoadDll();

	HANDLE Init(HWND hMainWindow, LPCTSTR ptzAppPath);
	HANDLE Quit();

	static UINT __stdcall ThreadProc(void* dllModPtr);

	HINSTANCE GetInstance() const
	{
		return m_hInstance;
	};

	HANDLE GetThread() const
	{
		return m_hThread;
	};

	HANDLE GetInitEvent() const
	{
		return m_hInitEvent;
	};

	HANDLE GetQuitEvent() const
	{
		return m_hQuitEvent;
	};

	LPCTSTR GetLocation() const
	{
		return m_tzLocation.c_str();
	};

	BOOL HasMessagePump() const
	{
		return !(m_dwFlags & MODULE_NOTPUMPED);
	};

	ModuleQuitFunc GetQuit() const
	{
		return m_pQuit;
	};
	ModuleInitExFunc GetInitEx() const
	{
		return m_pInitEx;
	};


private:
	int CallInit();
};

#endif // __MODULE_H
