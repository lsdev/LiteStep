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
#include "ModuleManager.h"
#include "../utility/macros.h"
#include "resource.h"
#include "../lsapi/lsapi.h"
#include "../utility/safestr.h"

ModuleManager::ModuleManager()
{
	m_pILiteStep = NULL;
}

ModuleManager::~ModuleManager()
{
}

HRESULT ModuleManager::Start(ILiteStep *pILiteStep)
{
	HRESULT hr;

	if (pILiteStep != NULL)
	{
		m_pILiteStep = pILiteStep;
		m_pILiteStep->AddRef();

		_LoadModules();

		hr = S_OK;
	}
	else
	{
		hr = E_INVALIDARG;
	}

	return hr;
}

HRESULT ModuleManager::Stop()
{
	HRESULT hr = S_OK;

	_QuitModules();

	if (m_pILiteStep)
	{
		m_pILiteStep->Release();
	}

	return hr;
}

HRESULT ModuleManager::rStart()
{
	HRESULT hr = S_OK;

	_LoadModules();

	return hr;
}

HRESULT ModuleManager::rStop()
{
	HRESULT hr = S_OK;

	_QuitModules();

	return hr;
}


UINT ModuleManager::_LoadModules()
{
	int nReturn = 0;
	char buffer[MAX_LINE_LENGTH];
	FILE *f;
	f = LCOpen(NULL);

	if (f)
	{
		while (LCReadNextConfig(f, "LoadModule", buffer, MAX_LINE_LENGTH))
		{
			char * lpszBuffers[4];
			//char token1[MAX_PATH];
			char token2[MAX_PATH];
			char token3[MAX_PATH];
			char token4[MAX_PATH];

			lpszBuffers[0] = NULL;
			lpszBuffers[1] = token2;
			lpszBuffers[2] = token3;
			lpszBuffers[3] = token4;

			if (LCTokenize (buffer, lpszBuffers, 4, NULL) >= 1)
			{
				DWORD dwFlags = MODULE_THREADED;

				if ((stricmp(token3, "notthreaded") == 0) || (stricmp(token4, "notthreaded") == 0))
				{
					dwFlags &= ~(MODULE_THREADED);
				}
				else if ((stricmp(token3, "nopump") == 0) || (stricmp(token4, "nopump") == 0))
				{
					dwFlags |= MODULE_NOTPUMPED;
				}

				if (LoadModule(token2, dwFlags))
				{
					++nReturn;
				}
			}
		}
		LCClose (f);

		_StartModules();
	}

	return nReturn;
}


BOOL ModuleManager::LoadModule(LPCSTR pszLocation, DWORD dwFlags)
{
	Module * module = NULL;
	LPSTR pszLowerLocation = _strlwr(_strdup(pszLocation));
	BOOL bReturn = FALSE;

	ModuleMap::iterator it = m_ModuleMap.find(pszLowerLocation);
	if (it == m_ModuleMap.end())
	{
		try
		{
			// #pragma COMPILE_WARN(NOTE: Need to do threading stuff)
			module = new Module(pszLowerLocation, dwFlags);
			m_ModuleMap.insert(ModuleMap::value_type(pszLowerLocation, module));
		}
		catch (int error)
		{

			switch (error)
			{
				case MODULE_BAD_PATH:
				RESOURCE_STR(NULL, IDS_MODULENOTFOUND_ERROR,
				             "Error: Could not locate module.\nPlease check your configuration.");
				break;
				case MODULE_BAD_INIT:
				RESOURCE_STR(NULL, IDS_INITMODULEEXNOTFOUND_ERROR,
				             "Error: Could not find initModuleEx().\n\nPlease confirm that the dll is a Litestep module,\nand check with the author for updates.");
				break;
				case MODULE_BAD_QUIT:
				RESOURCE_STR(NULL, IDS_QUITMODULENOTFOUND_ERROR,
				             "Error: Could not find quitModule().\n\nPlease conirm that the dll is a Litestep module.");
				break;
			}

			if (module)
			{
				delete module;
			}

			MessageBox(NULL, resourceTextBuffer, pszLowerLocation, MB_OK | MB_ICONEXCLAMATION | MB_TOPMOST);
		}
	}

	free(pszLowerLocation);

	return bReturn;
}


void ModuleManager::_StartModules()
{
	int nModuleCount = m_ModuleMap.size();

	if (nModuleCount)
	{
		HWND hLiteStep = NULL;
		CHAR szAppPath[MAX_PATH];
		HRESULT hr;

		hr = m_pILiteStep->get_Window((LONG*)&hLiteStep);
		if (SUCCEEDED(hr))
		{
			hr = m_pILiteStep->get_AppPath(szAppPath, MAX_PATH);
			if (SUCCEEDED(hr))
			{
				HANDLE * pQuitEvents = new HANDLE[nModuleCount];
				ModuleMap::iterator iter = m_ModuleMap.begin();

				int nIndex = 0;
				while (iter != m_ModuleMap.end())
				{
					if (iter->second)
					{
						try
						{
							HANDLE hEvent = iter->second->Init(hLiteStep, szAppPath);
							if (hEvent)
							{
								pQuitEvents[nIndex++] = hEvent;
							}
						}
						catch (...)
						{
							RESOURCE_MSGBOX(NULL, IDS_MODULEINITEXCEPTION_ERROR,
											"Error: Exception during module initialization.\n\nPlease contact the module writer.",
											iter->first.c_str());
						}
					}

					iter++;
				}

				// Wait for all modules to signal that they have started before exiting this function
				nModuleCount = nIndex;
				DWORD dwWaitStatus = MsgWaitForMultipleObjects(nModuleCount, pQuitEvents, FALSE, INFINITE, QS_ALLINPUT);
				HWND hLiteStep = GetLitestepWnd();
				while (nIndex)
				{
					if (dwWaitStatus == (WAIT_OBJECT_0 + nModuleCount))
					{
						MSG message;
						// if we use NULL instead of hLiteStep here.. it locks us up...
						if (PeekMessage(&message, hLiteStep, 0, 0, PM_REMOVE) > 0)
						{
							TranslateMessage(&message);
							DispatchMessage (&message);
						}
					}
					else if ((dwWaitStatus >= WAIT_OBJECT_0) && (dwWaitStatus <= (WAIT_OBJECT_0 + nModuleCount - 1)))
					{
						nIndex--;
					}

					if (nIndex)
					{
						dwWaitStatus = MsgWaitForMultipleObjects(nModuleCount, pQuitEvents, FALSE, INFINITE, QS_ALLINPUT);
					}
				}

				delete [] pQuitEvents;
			}
		}
	}
}

void ModuleManager::_QuitModules()
{
	int nModuleCount = m_ModuleMap.size();

	if (nModuleCount)
	{
		HANDLE * pQuitEvents = new HANDLE[nModuleCount];
		ModuleMap::iterator iter = m_ModuleMap.begin();

		int nIndex = 0;
		while (iter != m_ModuleMap.end())
		{
			if (iter->second)
			{
				try
				{
					HANDLE hEvent = iter->second->Quit();
					if (hEvent)
					{
						pQuitEvents[nIndex++] = hEvent;
					}
				}
				catch (...)
				{
					// quietly swallow exceptions
					// debugging/logging code should go here
				}
			}

			iter++;
		}

		// Wait for all modules to signal that they have quit before deleting them
		nModuleCount = nIndex;
		DWORD dwWaitStatus = MsgWaitForMultipleObjects(nModuleCount, pQuitEvents, FALSE, INFINITE, QS_ALLINPUT);
		HWND hLiteStep = GetLitestepWnd();
		while (nIndex)
		{
			if (dwWaitStatus == (WAIT_OBJECT_0 + nModuleCount))
			{
				MSG message;
				// if we use NULL instead of hLiteStep here.. it locks us up...
				while (PeekMessage(&message, hLiteStep, 0, 0, PM_REMOVE) > 0)
				{
					TranslateMessage(&message);
					DispatchMessage (&message);
				}
			}
			else if ((dwWaitStatus >= WAIT_OBJECT_0) && (dwWaitStatus <= (WAIT_OBJECT_0 + nModuleCount - 1)))
			{
				nIndex--;
			}

			if (nIndex)
			{
				dwWaitStatus = MsgWaitForMultipleObjects(nModuleCount, pQuitEvents, FALSE, INFINITE, QS_ALLINPUT);
			}
		}


		delete [] pQuitEvents;

		// Clean it all up
		iter = m_ModuleMap.begin();
		while (iter != m_ModuleMap.end())
		{
			if (iter->second)
			{
				//CloseHandle(iter->second->GetThread());
				delete iter->second;
			}
			++iter;
		}
		m_ModuleMap.clear();

	}
}


BOOL ModuleManager::QuitModule(LPCSTR pszLocation)
{
	if (IsValidStringPtr(pszLocation))
	{
		LPSTR pszLowerLocation = _strlwr(_strdup(pszLocation));

		ModuleMap::iterator iter = m_ModuleMap.find(pszLowerLocation);
		free(pszLowerLocation);

		if (iter != m_ModuleMap.end())
		{
			HANDLE hQuitEvents[1];
			try
			{
				HANDLE hEvent = iter->second->Quit();
				if (hEvent)
				{
					hQuitEvents[0] = hEvent;

					DWORD dwWaitStatus = MsgWaitForMultipleObjects(1, hQuitEvents, FALSE, INFINITE, QS_ALLINPUT);
					HWND hLiteStep = GetLitestepWnd();
					while (1)
					{
						if (dwWaitStatus == (WAIT_OBJECT_0 + 1))
						{
							MSG message;
							// if we use NULL instead of hLiteStep here.. it locks us up...
							while (PeekMessage(&message, hLiteStep, 0, 0, PM_REMOVE) > 0)
							{
								TranslateMessage(&message);
								DispatchMessage (&message);
							}
						}
						else if (dwWaitStatus == WAIT_OBJECT_0)
						{
							break;
						}

						dwWaitStatus = MsgWaitForMultipleObjects(1, hQuitEvents, FALSE, INFINITE, QS_ALLINPUT);
					}
				}
				m_ModuleMap.erase(iter);
			}
			catch (...)
			{
				// quietly swallow exceptions
				// debugging/logging code should go here
			}
		}
	}

	return TRUE;
}

UINT ModuleManager::GetModuleList(LPSTR *lpszModules, DWORD dwSize)
{
	/*long lBound;
	long uBound;
	long size;
	int i;

	BSTR *outList;

	SafeArrayGetLBound(strlist, 1, &lBound);
	SafeArrayGetUBound(strlist, 1, &uBound);

	size = uBound - lBound + 1;

	SafeArrayAccessData(strlist, (void**)&outList);

	string2module::iterator iter = LoadModuleMap.begin();
	for (i = 0; i < size && iter != LoadModuleMap.end(); i++, iter++)
	{
		outList[i] = SysAllocString(iter->first.c_str());
	}

	SafeArrayUnaccessData(strlist);

	*count = i;

	return S_OK;*/ 
	return 0;
}

STDMETHODIMP ModuleManager::get_Count(long* pCount)
{
	HRESULT hr = E_INVALIDARG;

	if (pCount != NULL)
	{
		//Lock();
		*pCount = m_ModuleMap.size();
		//Unlock();

		hr = S_OK;
	}

	return hr;
}
