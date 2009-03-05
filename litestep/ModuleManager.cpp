//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//
// This is a part of the Litestep Shell source code.
//
// Copyright (C) 1997-2009  LiteStep Development Team
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
#include "ModuleManager.h"
#include "../utility/core.hpp"
#include <algorithm>
#include <vector>


ModuleManager::ModuleManager() :
    m_pILiteStep(NULL), m_hLiteStep(NULL)
{
    // do nothing
}


ModuleManager::~ModuleManager()
{
    // do nothing
}


HRESULT ModuleManager::Start(ILiteStep *pILiteStep)
{
    ASSERT(NULL == m_pILiteStep);
    
    HRESULT hr = E_FAIL;
    
    if (pILiteStep != NULL)
    {
        m_pILiteStep = pILiteStep;
        m_pILiteStep->AddRef();
        
        char szAppPath[MAX_PATH] = { 0 };
        
        m_hLiteStep = GetLitestepWnd();
        
        if (m_hLiteStep && LSGetLitestepPath(szAppPath, MAX_PATH))
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
    
    LPVOID f = LCOpen(NULL);
    
    if (f)
    {
        // need to use a separate queue as modules may load other modules (e.g.
        // mzscript via LoadModule) during the startup process
        ModuleQueue mqModules;
        
#if defined(LS_COMPAT_LSLOADMODULE)
        while (LCReadNextConfig(f, "*LSLoadModule", szLine, MAX_LINE_LENGTH))
#elif defined(LS_COMPAT_LCREADNEXTCONFIG)
        while (LCReadNextCommand(f, szLine, MAX_LINE_LENGTH))
#else
        while (LCReadNextConfig(f, "LoadModule", szLine, MAX_LINE_LENGTH))
#endif
        {
            char szCommand[MAX_RCCOMMAND] = { 0 };
            char szToken1[MAX_LINE_LENGTH] = { 0 };
            char szToken2[MAX_LINE_LENGTH] = { 0 };
            
            // first buffer takes the "LoadModule" token
            LPSTR lpszBuffers[] = { szCommand, szToken1, szToken2 };
            
            if (LCTokenize(szLine, lpszBuffers, 3, NULL) >= 2)
            {
#if defined(LS_COMPAT_LCREADNEXTCONFIG)
                if (_stricmp(szCommand, "LoadModule"))
                {
                    continue;
                }
#endif
                
                DWORD dwFlags = 0;
                
                if (_stricmp(szToken2, "threaded") == 0)
                {
                    dwFlags |= LS_MODULE_THREADED;
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
        ModuleQueue mqModule(1, pModule);
        bReturn = (_StartModules(mqModule) == 1);
    }
    
    return bReturn;
}


Module* ModuleManager::_MakeModule(LPCSTR pszLocation, DWORD dwFlags)
{
    return new Module(pszLocation, dwFlags);
}


UINT ModuleManager::_StartModules(ModuleQueue& mqModules)
{
    UINT uReturn = 0;
    
    if (mqModules.size() > 0)
    {
        std::vector<HANDLE> vecInitEvents;
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
                            // Note: We are taking ownership of the Event handle
                            //       here.  We call CloseHandle() below.
                            vecInitEvents.push_back((*iter)->TakeInitEvent());
                        }
                        
                        m_ModuleQueue.push_back(*iter);
                        ++uReturn;
                        
                        ++iter;
                        continue;
                    }
                }
            }
            
            // If we got here, then this is an invalid entry, and needs erased.
            ModuleQueue::iterator iterOld = iter++;
            
            delete *iterOld;
            mqModules.erase(iterOld);
        }
        
        // Are there any "threaded" modules?
        if (!vecInitEvents.empty())
        {
            // Wait for all modules to signal that they have started.
            _WaitForModules(&vecInitEvents[0], vecInitEvents.size());
            
            // Close the handles we have taken ownership of.
            std::for_each(
                vecInitEvents.begin(), vecInitEvents.end(), CloseHandle);
        }
    }
    
    return uReturn;
}


void ModuleManager::_QuitModules()
{
    std::vector<HANDLE> vecQuitObjects;
    ModuleQueue::reverse_iterator iter = m_ModuleQueue.rbegin();
    ModuleQueue TempQueue;
    
    // Note:
    //  Store each module in a temporary queue, so that the module may not be
    //  accessed via our main queue while it is being unloaded.  This does -not-
    //  protect us from threads, however it does hopefully add some security
    //  through obscurity from recursion.
    
    while (iter != m_ModuleQueue.rend())
    {
        TempQueue.push_back(*iter);
        
        if (*iter)
        {
            (*iter)->Quit();
            
            if ((*iter)->GetThread())
            {
                vecQuitObjects.push_back((*iter)->TakeThread());
            }
        }
        
        ++iter;
    }
    
    m_ModuleQueue.clear();
    
    if (!vecQuitObjects.empty())
    {
        _WaitForModules(&vecQuitObjects[0], vecQuitObjects.size());
        
        // Close the handles we have taken ownership of.
        std::for_each(
            vecQuitObjects.begin(), vecQuitObjects.end(), CloseHandle);
    }
    
    // Clean it all up
    iter = TempQueue.rbegin();
    while (iter != TempQueue.rend())
    {
        delete *iter;
        ++iter;
    }
}


BOOL ModuleManager::QuitModule(HINSTANCE hModule)
{
    ModuleQueue::iterator iter = _FindModule(hModule);
    
    if (iter != m_ModuleQueue.end() && *iter)
    {
        (*iter)->Quit();
        
        if ((*iter)->GetThread())
        {
            HANDLE hThread = (*iter)->TakeThread();
            
            _WaitForModules(&hThread, 1);
            
            CloseHandle(hThread);
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


void ModuleManager::_WaitForModules(const HANDLE* pHandles, size_t stCount) const
{
    std::vector<HANDLE> vWait(pHandles, pHandles+stCount);
    
    // Loop for as long as we have an object whose state is not signaled.
    while (vWait.size())
    {
        // Handle all pending messages first
        m_pILiteStep->PeekAllMsgs();
        
        // Wait for a new message to come in, or for one of our objects to
        // become signaled.
        DWORD dwWaitStatus = MsgWaitForMultipleObjects((DWORD)vWait.size(),
            &vWait[0], FALSE, INFINITE, QS_ALLINPUT);
        
        // Recreate the pObject list, in case any of the objects do not auto
        // reset their signaled state.  Otherwise, we would drop through our
        // outer loop immediately without waiting for all of our objects.
        if ((dwWaitStatus >= WAIT_OBJECT_0) &&
            (dwWaitStatus < (WAIT_OBJECT_0 + vWait.size())))
        {
            vWait.erase(vWait.begin() + (dwWaitStatus - WAIT_OBJECT_0));
        }
    }
}


HRESULT ModuleManager::EnumModules(LSENUMMODULESPROC pfnCallback, LPARAM lParam) const
{
    HRESULT hr = S_OK;
    
    for (ModuleQueue::const_iterator iter = m_ModuleQueue.begin();
        iter != m_ModuleQueue.end(); ++iter)
    {
        if (!pfnCallback((*iter)->GetLocation(), (*iter)->GetFlags(),
            lParam))
        {
            hr = S_FALSE;
            break;
        }
    }
    
    return hr;
}
