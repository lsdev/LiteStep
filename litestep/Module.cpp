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
#include "module.h"
#include "resource.h"
#include "../utility/macros.h"
#include "../lsapi/ThreadedBangCommand.h"
#include "../utility/safestr.h" // Always include last in cpp file

Module::Module(LPCSTR pszLoc, DWORD dwFlags)
{
	m_hThread = NULL;
	m_pInitEx = NULL;
	m_hInitEvent = NULL;
	m_hQuitEvent = NULL;
	m_pQuit = NULL;
	m_dwFlags = dwFlags;

	if (IsValidStringPtr(pszLoc))
	{
		m_szLocation = pszLoc;

		m_hInstance = LoadLibrary(pszLoc);

		if (m_hInstance)
		{
			m_pInitEx = (ModuleInitExFunc)GetProcAddress(m_hInstance, "initModuleEx");
			if (!m_pInitEx) // Might be a BC module, check for underscore
			{
				m_pInitEx = (ModuleInitExFunc)GetProcAddress(m_hInstance, "_initModuleEx");
			}
			if (!m_pInitEx)
			{
				throw MODULE_BAD_INIT;
			}

			m_pQuit = (ModuleQuitFunc)GetProcAddress(m_hInstance, "quitModule");
			if (!m_pQuit)   // Might be a BC module, check for underscore
			{
                m_pQuit = (ModuleQuitFunc)GetProcAddress(m_hInstance, "_quitModule");
			}
			if (!m_pQuit)
			{
				throw MODULE_BAD_QUIT;
			}
		}
		else
		{
			throw MODULE_BAD_PATH;
		}
	}
}

Module::~Module()
{
	if (m_dwFlags & MODULE_THREADED)
    {
        CloseHandle(m_hThread);
        CloseHandle(m_hInitEvent);
        CloseHandle(m_hQuitEvent);
    }
    
	if (m_hInstance)
	{
		FreeLibrary(m_hInstance);
		m_hInstance = NULL;
	}
}

HANDLE Module::Init(HWND hMainWindow, LPCSTR pszAppPath)
{
	if (m_hInstance)
	{
		if (m_pInitEx)
		{
			m_hMainWindow = hMainWindow;
			m_szAppPath = pszAppPath;

			if (m_dwFlags & MODULE_THREADED)
			{
				SECURITY_ATTRIBUTES sa;

				sa.nLength = sizeof(SECURITY_ATTRIBUTES);
				sa.lpSecurityDescriptor = NULL;
				sa.bInheritHandle = FALSE;

				m_hInitEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
				m_hThread = CreateThread(&sa, 0, Module::ThreadProc, this, NULL, (ULONG*) & m_dwThreadID);
			}
			else
			{
				CallInit();
			}
		}
	}

	return m_hInitEvent;
}

ULONG Module::CallInit()
{
	ULONG ulReturn = 0;

	try
	{
		m_pInitEx(m_hMainWindow, m_hInstance, m_szAppPath.c_str());
	}
	catch (...)
	{
		FreeLibrary(m_hInstance);
		m_hInstance = NULL;

		ulReturn = -1;
	}

	return ulReturn;
}

HANDLE Module::Quit()
{
	if (m_hInstance)
	{

		if (m_hThread)
		{
			m_hQuitEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
			PostThreadMessage(m_dwThreadID, WM_DESTROY, 0, (LPARAM)this);
		}
		else
		{
			if (m_pQuit)
			{
				try
				{
					m_pQuit(m_hInstance);
				}
				catch (...)
				{
					RESOURCE_MSGBOX(NULL, IDS_MODULEQUIT_ERROR,
					                "Exception while quitting module.", m_szLocation.c_str())
				}
			}
		}
	}

	return m_hQuitEvent;
}


ULONG __stdcall Module::ThreadProc(void* dllModPtr)
{
	Module * dllMod = (Module*)dllModPtr;

	dllMod->CallInit();

	SetEvent(dllMod->GetInitEvent());

	if (dllMod->HasMessagePump())
	{
		MSG msg;

		while (GetMessage(&msg, 0, 0, 0))
		{
			try
			{
				if (msg.hwnd == NULL)
				{
					// Thread message
					dllMod->HandleThreadMessage(msg);
				}
				else
				{
					// Window message
					TranslateMessage(&msg);
					DispatchMessage(&msg);
				}
			}
			catch (...)
			{
				// Quietly ignore exceptions?
				// #pragma COMPILE_WARN(Note: Need stronger exception-handling code here...restart the module or something)
			}
		}
	}

	SetEvent(dllMod->GetQuitEvent());

	return 0;
}

void Module::HandleThreadMessage(MSG &msg)
{
	switch (msg.message)
	{
		case LM_THREAD_BANGCOMMAND:
		{
			ThreadedBangCommand * pInfo = (ThreadedBangCommand*)msg.wParam;

			if (pInfo != NULL)
			{
				pInfo->Execute();
				pInfo->Release(); //check BangCommand.cpp for the reason
			}
		}
		break;

		case WM_DESTROY:
		{
			Module *dll_mod = (Module*)msg.lParam;
			if (dll_mod)
			{
				try
				{
					dll_mod->GetQuit()(dll_mod->GetInstance());
				}
				catch (...)
				{
					RESOURCE_MSGBOX(NULL, IDS_MODULEQUIT_ERROR,
					                "Exception while quitting module.", dll_mod->GetLocation())
				}

				PostQuitMessage(0);
			}

		}
		break;
	}
}
