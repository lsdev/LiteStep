/*
This is a part of the LiteStep Shell Source code.

Copyright (C) 1997-2003,2005 The LiteStep Development Team

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

typedef int (*ModuleInitExFunc) (HWND, HINSTANCE, LPCSTR);
typedef int (*ModuleQuitFunc) (HINSTANCE);

class Module
{
private:
	HINSTANCE m_hInstance;
	HANDLE m_hThread;
	HWND m_hMainWindow;
	DWORD m_dwThreadID;

    std::basic_string<TCHAR> m_tzLocation;
    std::basic_string<TCHAR> m_tzAppPath;

	ModuleInitExFunc m_pInitEx;
	ModuleQuitFunc m_pQuit;

	DWORD m_dwFlags;

	HANDLE m_hInitEvent;
	HANDLE m_hInitCopyEvent;

public:
	Module(const std::string& sLocation, DWORD dwFlags);
	virtual ~Module();

    bool Init(HWND hMainWindow, const std::string& sAppPath);
	void Quit();

	static UINT __stdcall ThreadProc(void* dllModPtr);
	static void HandleThreadMessage(MSG &msg);

	HINSTANCE GetInstance() const
	{
		return m_hInstance;
	};

	HANDLE GetThread() const
	{
		return m_hThread;
	};

	/* Caller may NOT call CloseHandle() until the thread
	 * has an exit signal. */
	HANDLE TakeThread()
	{
		HANDLE hTemp = m_hThread;
		m_hThread = NULL;

		return hTemp;
	};

	HANDLE GetInitEvent() const
	{
		return m_hInitEvent;
	};

	/* Caller may NOT call CloseHandle() until the event
	 * has been set to signaled. */
	HANDLE TakeInitEvent()
	{
		HANDLE hTemp = m_hInitEvent;
		m_hInitEvent = NULL;

		return hTemp;
	};

	LPCTSTR GetLocation() const
	{
		return m_tzLocation.c_str();
	};

	DWORD GetFlags() const
    {
        return m_dwFlags;
    }

	ModuleQuitFunc GetQuit() const
	{
		return m_pQuit;
	};
	ModuleInitExFunc GetInitEx() const
	{
		return m_pInitEx;
	};


private:
    bool _LoadDll();
    
    int CallInit();
    void CallQuit();
};

#endif // __MODULE_H
