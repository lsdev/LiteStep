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
#include <algorithm>
#include "../utility/core.hpp"

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

				Module* pModule = _MakeModule(szToken1, dwFlags);
				if (pModule)
				{
					m_ModuleQueue.push_back(pModule);
				}
			}
		}
		LCClose (f);

		uReturn = _StartModules(m_ModuleQueue);
	}

	return uReturn;
}


BOOL ModuleManager::LoadModule(LPCSTR pszLocation, DWORD dwFlags)
{
	BOOL bReturn = FALSE;
	
	// _MakeModule checks if the module is already loaded
	Module* pModule = _MakeModule(pszLocation, dwFlags);
	
	if (pModule)
	{
		ModuleQueue	mqModule(1, pModule);
        bReturn = (_StartModules(mqModule) == 1);

        if (bReturn)
        {
            m_ModuleQueue.push_back(pModule);
        }
	}

	return bReturn;
}


Module* ModuleManager::_MakeModule(LPCSTR pszLocation, DWORD dwFlags)
{
	Module* pModule = NULL;

	ModuleQueue::iterator it = _FindModule(pszLocation);
	if (it == m_ModuleQueue.end())
	{
		try
		{
			pModule = new Module(pszLocation, dwFlags);
		}
		catch (...)
		{
			if (pModule)
			{
				delete pModule;
				pModule = NULL;
			}
		}
	}

	return pModule;
}


UINT ModuleManager::_StartModules(ModuleQueue& mqModules)
{
	UINT uReturn = 0;

	if (mqModules.size() > 0)
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
                std::vector<HANDLE> vecInitEvents;;
				ModuleQueue::iterator iter = mqModules.begin();

				while (iter != mqModules.end())
				{
					if (*iter)
					{
						// delaying the LoadLibrary call is necessary to make
						// grdtransparent work
						if ((*iter)->LoadDll())
						{
							HANDLE hEvent = (*iter)->Init(hLiteStep, szAppPath);
							
                            if (hEvent)
							{
								vecInitEvents.push_back(hEvent);
							}
							++uReturn;
						}
					}

					++iter;
				}

				// Wait for all modules to signal that they have started
				size_t stModuleCount = vecInitEvents.size();
                size_t stIndex = stModuleCount;
				
                DWORD dwWaitStatus = MsgWaitForMultipleObjects(stModuleCount,
                    &vecInitEvents[0], FALSE, INFINITE, QS_ALLINPUT);

				while (stIndex)
				{
					if (dwWaitStatus == (WAIT_OBJECT_0 + stModuleCount))
					{
						MSG message;
						// if we use NULL instead of hLiteStep here.. it locks us up...
						if (PeekMessage(&message, hLiteStep, 0, 0, PM_REMOVE) > 0)
						{
							TranslateMessage(&message);
							DispatchMessage (&message);
						}
					}
					else if ((dwWaitStatus >= WAIT_OBJECT_0) &&
                        (dwWaitStatus <= (WAIT_OBJECT_0 + stModuleCount - 1)))
					{
						--stIndex;
					}

					if (stIndex)
					{
						dwWaitStatus = MsgWaitForMultipleObjects(stModuleCount,
                            &vecInitEvents[0], FALSE, INFINITE, QS_ALLINPUT);
					}
				}
			}
		}
	}

	return uReturn;
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
				HANDLE hEvent = (*iter)->Quit();
				if (hEvent)
				{
					pQuitEvents[nIndex++] = hEvent;
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


// need to put this at global scope because of mingw issues
struct ModuleLookup
{
	ModuleLookup(LPCSTR pszName) : m_pszName(pszName){}
	
	bool operator() (const Module* pModule) const
	{
		return (stricmp(m_pszName, pModule->GetLocation()) == 0);
	}
	
    private:
        LPCSTR m_pszName;
};


ModuleQueue::iterator ModuleManager::_FindModule(LPCSTR pszLocation)
{
    return std::find_if(m_ModuleQueue.begin(), m_ModuleQueue.end(),
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
