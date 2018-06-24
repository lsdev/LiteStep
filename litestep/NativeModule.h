//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//
// This is a part of the Litestep Shell source code.
//
// Copyright (C) 1997-2015  LiteStep Development Team
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
#if !defined(NATIVE_MODULE_H)
#define NATIVE_MODULE_H

#include "Module.h"
#include <functional>

class NativeModule : public Module
{
private:
	/** Instance handle of module's DLL */
	HINSTANCE m_hInstance;

	/** Thread handle */
	HANDLE m_hThread;

	/** LiteStep's main window handle */
	HWND m_hMainWindow;

	/** Thread ID */
	DWORD m_dwThreadID;

	/** Path to LiteStep's root directory */
	std::basic_string<WCHAR> m_wzAppPath;

	/** Pointer to <code>initModuleEx</code> function */
	std::function<int __cdecl (HWND, HINSTANCE, LPCWSTR)> m_pInit;

	/** Pointer to <code>quitModule</code> function */
	quitModuleProc m_pQuit;
	
	/**
	* Event that is triggered when a threaded module completes initialization
	*/
	HANDLE m_hInitEvent;
	HANDLE m_hInitCopyEvent;

public:
	NativeModule(const std::wstring& sLocation, DWORD dwFlags);
	virtual ~NativeModule();
	

	bool Init(HWND hMainWindow, const std::wstring& sAppPath);

	void Quit();
	
	HINSTANCE GetInstance() const
	{
		return m_hInstance;
	}

	HANDLE GetThread() const
	{
		return m_hThread;
	}

	HANDLE TakeThread()
	{
		HANDLE hTemp = m_hThread;
		m_hThread = nullptr;

		return hTemp;
	}

	HANDLE GetInitEvent() const
	{
		return m_hInitEvent;
	}

	HANDLE TakeInitEvent()
	{
		HANDLE hTemp = m_hInitEvent;
		m_hInitEvent = nullptr;

		return hTemp;
	}


	/**
	* Returns a pointer to this module's <code>quitModule</code> function.
	*/
	decltype(m_pQuit) GetQuit() const
	{
		return m_pQuit;
	}

	/**
	* Returns a pointer to this module's <code>initModuleEx</code> function.
	*/
	decltype(m_pInit) GetInit() const
	{
		return m_pInit;
	}

	/**
	* Entry point for the module's main thread.
	*
	* @param  dllModPtr  pointer to <code>this</code>
	* @return unused
	*/
	static UINT __stdcall ThreadProc(void* dllModPtr);

	/**
	* Handles messages sent to the module's main thread.
	*
	* @param  msg  message information
	*/
	static void HandleThreadMessage(MSG &msg);

private:
	/**
	* Loads this module's DLL.
	*
	* @return <code>true</code> if successful or
	*         <code>false</code> if an error occurs
	*/
	bool _LoadDll();

	/**
	* Calls this module's <code>initModuleEx</code> function.
	*
	* @return return value from <code>initModuleEx</code>
	*/
	int _CallInit();

	/**
	* Calls this module's <code>quitModule</code> function.
	*/
	void _CallQuit();

};

#endif