/*
This is a part of the LiteStep Shell Source code.

Copyright (C) 1997-2005 The LiteStep Development Team

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
#include <vector>
#include "../utility/core.hpp"

ModuleManager::ModuleManager() : 
m_pILiteStep(NULL), m_hLiteStep(NULL)
{
}

ModuleManager::~ModuleManager()
{
}

HRESULT ModuleManager::Start(ILiteStep *pILiteStep)
{
    ASSERT_ISNULL(m_pILiteStep);

    HRESULT hr = E_FAIL;

	if (pILiteStep != NULL)
	{
		m_pILiteStep = pILiteStep;
		m_pILiteStep->AddRef();

        char szAppPath[MAX_PATH] = { 0 };

        if (SUCCEEDED(m_pILiteStep->get_Window((LONG*)&m_hLiteStep)) &&
            SUCCEEDED(m_pILiteStep->get_AppPath(szAppPath, MAX_PATH)))
        {
            m_sAppPath = szAppPath;

            _LoadModules();
            
            hr = S_OK;
        }
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
        m_pILiteStep = NULL;
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
	ASSERT(m_ModuleQueue.empty());

    UINT uReturn = 0;
	char szLine[MAX_LINE_LENGTH];
	
    FILE* f = LCOpen(NULL);

	if (f)
	{
		// need to use a separate queue as modules may load other modules (e.g.
        // mzscript via LoadModule) during the startup process
        ModuleQueue mqModules;

        while (LCReadNextConfig(f, "LoadModule", szLine, MAX_LINE_LENGTH))
		{
            char szToken1[MAX_LINE_LENGTH] = { 0 };
            char szToken2[MAX_LINE_LENGTH] = { 0 };
			char szToken3[MAX_LINE_LENGTH] = { 0 };

            // first buffer takes the "LoadModule" token
            LPSTR lpszBuffers[] = { NULL, szToken1, szToken2, szToken3 };
            
			if (LCTokenize(szLine, lpszBuffers, 4, NULL) >= 2)
			{
				DWORD dwFlags = 0;

				if ((stricmp(szToken2, "threaded") == 0) ||
                    (stricmp(szToken3, "threaded") == 0))
				{
					dwFlags |= LS_MODULE_THREADED;
				}
				
				if ((stricmp(szToken2, "nopump") == 0) ||
                    (stricmp(szToken3, "nopump") == 0))
				{
					dwFlags |= LS_MODULE_NOTPUMPED;
				}

				Module* pModule = _MakeModule(szToken1, dwFlags);
				
                if (pModule)
				{
					mqModules.push_back(pModule);
				}
			}
		}
		LCClose (f);

        uReturn = _StartModules(mqModules);
	}

	return uReturn;
}


BOOL ModuleManager::LoadModule(LPCSTR pszLocation, DWORD dwFlags)
{
	BOOL bReturn = FALSE;
	
	Module* pModule = _MakeModule(pszLocation, dwFlags);
	
	if (pModule)
	{
		ModuleQueue	mqModule(1, pModule);
        bReturn = (_StartModules(mqModule) == 1);
	}

	return bReturn;
}


Module* ModuleManager::_MakeModule(LPCSTR pszLocation, DWORD dwFlags)
{
	Module* pModule = NULL;

    try
    {
        pModule = new Module(pszLocation, dwFlags);
    }
    catch (...)
    {
        delete pModule;
        pModule = NULL;
    }

	return pModule;
}


UINT ModuleManager::_StartModules(ModuleQueue& mqModules)
{
	UINT uReturn = 0;

	if (mqModules.size() > 0)
	{
        std::vector<HANDLE> vecInitEvents;;
        ModuleQueue::iterator iter = mqModules.begin();
        
        while (iter != mqModules.end())
        {
            if (*iter)
            {
                if (_FindModule((*iter)->GetLocation()) == m_ModuleQueue.end())
                {
                    if ((*iter)->Init(m_hLiteStep, m_sAppPath))
                    {
                        if ((*iter)->GetInitEvent())
                        {
                            vecInitEvents.push_back((*iter)->GetInitEvent());
                        }

                        m_ModuleQueue.push_back(*iter);
                        uReturn++;

                        iter++;
                        continue;
                    }
                }
            }

            /* If we got here, then this is an invalid
             * entry, and needs erased. */
            ModuleQueue::iterator iterOld = iter++;
            
            delete *iterOld;
            mqModules.erase(iterOld);
        }
        
        // Wait for all modules to signal that they have started
        _WaitForModules(&vecInitEvents[0], vecInitEvents.size());
    }

	return uReturn;
}

void ModuleManager::_QuitModules()
{
    std::vector<HANDLE> vecQuitEvents;;
    ModuleQueue::reverse_iterator iter = m_ModuleQueue.rbegin();
    
    while (iter != m_ModuleQueue.rend())
    {
        if (*iter)
        {
            (*iter)->Quit();

            HANDLE hEvent = (*iter)->GetQuitEvent();

            if (hEvent)
            {
                vecQuitEvents.push_back(hEvent);
            }
        }
        
        ++iter;
    }
    
    _WaitForModules(&vecQuitEvents[0], vecQuitEvents.size());
    
    // Clean it all up
    iter = m_ModuleQueue.rbegin();
    while (iter != m_ModuleQueue.rend())
    {
        delete *iter;
        ++iter;
    }
    
    m_ModuleQueue.clear();
}


BOOL ModuleManager::QuitModule(HINSTANCE hModule)
{
    ModuleQueue::iterator iter = _FindModule(hModule);
    
    if (iter != m_ModuleQueue.end() && *iter)
    {
        (*iter)->Quit();

        HANDLE hQuitEvent = (*iter)->GetQuitEvent();
        
        if (hQuitEvent)
        {
            _WaitForModules(&hQuitEvent, 1);
        }
        
        delete *iter;
        m_ModuleQueue.erase(iter);
    }
	
	return TRUE;
}


BOOL ModuleManager::QuitModule(LPCSTR pszLocation)
{
    BOOL bReturn = FALSE;

    ModuleQueue::iterator iter = _FindModule(pszLocation);
    
    if (iter != m_ModuleQueue.end() && *iter)
    {
        bReturn = QuitModule((*iter)->GetInstance());
    }

    return bReturn;
}


BOOL ModuleManager::ReloadModule(HINSTANCE hModule)
{
    BOOL bReturn = FALSE;

    ModuleQueue::iterator iter = _FindModule(hModule);
    
    if (iter != m_ModuleQueue.end())
    {
        std::string sLocation = (*iter)->GetLocation();
        DWORD dwFlags = (*iter)->GetFlags();

        QuitModule(hModule);
        bReturn = LoadModule(sLocation.c_str(), dwFlags);
    }

    return bReturn;
}


ModuleQueue::iterator ModuleManager::_FindModule(LPCSTR pszLocation)
{
    return std::find_if(m_ModuleQueue.begin(), m_ModuleQueue.end(),
        IsLocationEqual(pszLocation));
}


ModuleQueue::iterator ModuleManager::_FindModule(HINSTANCE hModule)
{
    return std::find_if(m_ModuleQueue.begin(), m_ModuleQueue.end(),
        IsInstanceEqual(hModule));
}


void ModuleManager::_WaitForModules(const HANDLE* pHandles, DWORD dwCount) const
{
    DWORD dwRest = dwCount;
    
    while (dwRest)
    {
        DWORD dwWaitStatus = MsgWaitForMultipleObjects(dwCount, pHandles,
            FALSE, INFINITE, QS_ALLINPUT);
        
        if (dwWaitStatus == (WAIT_OBJECT_0 + dwCount))
        {
            MSG message;
            // if we use NULL instead of hLiteStep here.. it locks us up...
            if (PeekMessage(&message, m_hLiteStep, 0, 0, PM_REMOVE))
            {
                TranslateMessage(&message);
                DispatchMessage (&message);
            }
        }
        else if ((dwWaitStatus >= WAIT_OBJECT_0) &&
            (dwWaitStatus <= (WAIT_OBJECT_0 + dwCount - 1)))
        {
            --dwRest;
        }
    }
}


HRESULT ModuleManager::EnumModules(LSENUMMODULESPROC pfnCallback, LPARAM lParam) const
{
    HRESULT hr = S_OK;
    
    try
    {
        for (ModuleQueue::const_iterator iter = m_ModuleQueue.begin(); iter != m_ModuleQueue.end(); ++iter)
        {
            if (!pfnCallback((*iter)->GetLocation(), (*iter)->GetFlags(),
                lParam))
            {
                hr = S_FALSE;
                break;
            }
        }
    }
    catch (...)
    {
        hr = E_UNEXPECTED;
    }
    
    return hr;
}
