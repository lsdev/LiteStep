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
	UINT uReturn = 0;
	char szLine[MAX_LINE_LENGTH];
	
    FILE* f = LCOpen(NULL);

	if (f)
	{
		while (LCReadNextConfig(f, "LoadModule", szLine, MAX_LINE_LENGTH))
		{
            // LCTokenize clears these buffers if necessary
            char szToken1[MAX_LINE_LENGTH];
            char szToken2[MAX_LINE_LENGTH];
			char szToken3[MAX_LINE_LENGTH];

            // first buffer takes the "LoadModule" token
            LPSTR lpszBuffers[] = { NULL, szToken1, szToken2, szToken3 };
            
			if (LCTokenize(szLine, lpszBuffers, 4, NULL) >= 2)
			{
				DWORD dwFlags = 0;

				if ((stricmp(szToken2, "threaded") == 0) ||
                    (stricmp(szToken3, "threaded") == 0))
				{
					dwFlags |= MODULE_THREADED;
				}
				
				if ((stricmp(szToken2, "nopump") == 0) ||
                    (stricmp(szToken3, "nopump") == 0))
				{
					dwFlags |= MODULE_NOTPUMPED;
				}

				if (LoadModule(szToken1, dwFlags))
				{
					++uReturn;
				}
			}
		}
		LCClose (f);

		_StartModules();
	}

	return uReturn;
}


BOOL ModuleManager::LoadModule(LPCSTR pszLocation, DWORD dwFlags)
{
	Module* pModule = NULL;
	BOOL bReturn = FALSE;

	ModuleQueue::iterator it = _FindModule(pszLocation);
	if (it == m_ModuleQueue.end())
	{
		try
		{
			pModule = new Module(pszLocation, dwFlags);
			m_ModuleQueue.push_back(pModule);
            bReturn = TRUE;
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

			if (pModule)
			{
				delete pModule;
			}

			MessageBox(NULL, resourceTextBuffer, pszLocation,
                MB_OK | MB_ICONEXCLAMATION | MB_TOPMOST | MB_SETFOREGROUND);
		}
	}

	return bReturn;
}


void ModuleManager::_StartModules()
{
	int nModuleCount = m_ModuleQueue.size();

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
				HANDLE* pInitEvents = new HANDLE[nModuleCount];
				ModuleQueue::iterator iter = m_ModuleQueue.begin();

				int nIndex = 0;
				while (iter != m_ModuleQueue.end())
				{
					if (*iter)
					{
						try
						{
							HANDLE hEvent = (*iter)->Init(hLiteStep, szAppPath);
							if (hEvent)
							{
								pInitEvents[nIndex++] = hEvent;
							}
						}
						catch (...)
						{
							RESOURCE_MSGBOX(NULL, IDS_MODULEINITEXCEPTION_ERROR,
											"Error: Exception during module initialization.\n\nPlease contact the module writer.",
											(*iter)->GetLocation());
						}
					}

					iter++;
				}

				// Wait for all modules to signal that they have started before exiting this function
				nModuleCount = nIndex;
				DWORD dwWaitStatus = MsgWaitForMultipleObjects(nModuleCount, pInitEvents, FALSE, INFINITE, QS_ALLINPUT);
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
						dwWaitStatus = MsgWaitForMultipleObjects(nModuleCount, pInitEvents, FALSE, INFINITE, QS_ALLINPUT);
					}
				}

				delete [] pInitEvents;
			}
		}
	}
}

void ModuleManager::_QuitModules()
{
	int nModuleCount = m_ModuleQueue.size();

	if (nModuleCount)
	{
		HANDLE * pQuitEvents = new HANDLE[nModuleCount];
		ModuleQueue::reverse_iterator iter = m_ModuleQueue.rbegin();

		int nIndex = 0;
		while (iter != m_ModuleQueue.rend())
		{
			if (*iter)
			{
				try
				{
					HANDLE hEvent = (*iter)->Quit();
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
		iter = m_ModuleQueue.rbegin();
		while (iter != m_ModuleQueue.rend())
		{
			if (*iter)
			{
				//CloseHandle(iter->second->GetThread());
				delete *iter;
			}
			++iter;
		}
		m_ModuleQueue.clear();

	}
}


BOOL ModuleManager::QuitModule(LPCSTR pszLocation)
{
	if (IsValidStringPtr(pszLocation))
	{
		ModuleQueue::iterator iter = _FindModule(pszLocation);

		if (iter != m_ModuleQueue.end())
		{
			HANDLE hQuitEvents[1];
			try
			{
				HANDLE hEvent = (*iter)->Quit();
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
			}
			catch (...)
			{
				// quietly swallow exceptions
				// debugging/logging code should go here
			}
			
            // better safe than sorry
            if (*iter)
            {
                delete *iter;
            }
            m_ModuleQueue.erase(iter);
		}
	}
	
	return TRUE;
}


struct ModuleManager::ModuleLookup
{
	ModuleLookup(LPCSTR pszName) : m_pszName(pszName){}
	
	bool operator() (const Module*& pModule) const
	{
		return (stricmp(m_pszName, pModule->GetLocation()) == 0);
	}
	
    private:
        LPCSTR m_pszName;
};


ModuleQueue::iterator ModuleManager::_FindModule(LPCSTR pszLocation)
{
	return find_if(m_ModuleQueue.begin(), m_ModuleQueue.end(),
		ModuleLookup(pszLocation));
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
		*pCount = m_ModuleQueue.size();
		//Unlock();

		hr = S_OK;
	}

	return hr;
}
