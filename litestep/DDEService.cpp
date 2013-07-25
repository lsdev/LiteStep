//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//
// This is a part of the Litestep Shell source code.
//
// Copyright (C) 1997-2013  LiteStep Development Team
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
#include "DDEService.h"
#include "../utility/core.hpp"


DWORD DDEService::m_idThread;
HANDLE DDEService::m_hThread;
HANDLE DDEService::m_hStartEvent;
DDEWorker DDEService::m_DDEWorker;
HSZ DDEService::m_hszProgman;
HSZ DDEService::m_hszGroups;
HSZ DDEService::m_hszFolders;
HSZ DDEService::m_hszAppProperties;
DWORD DDEService::m_dwDDEInst;

DDEService::DDEService()
{
    m_hszProgman = NULL;
    m_hszGroups = NULL;
    m_hszFolders = NULL;
    m_hszAppProperties = NULL;
    m_dwDDEInst = 0;
    m_hStartEvent = NULL;
}

DDEService::~DDEService()
{
    // do nothing
}

DWORD WINAPI DDEService::_DDEThreadProc(LPVOID pvService)
{
    DDEService* pService = (DDEService*)pvService;
    
    bool bStarted = pService->_DoStart();
    
    SetEvent(pService->m_hStartEvent);
    
    if (bStarted)
    {
        MSG msg;
        while (GetMessage(&msg, 0, 0, 0))
        {
            // Window message
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }
    
    pService->_DoStop();
    
    return 0;
}

HRESULT DDEService::Start()
{
    HRESULT hr = E_FAIL;
    
    // If m_dwDDEInst is not NULL, then we have already been started, just
    // return S_FALSE which will still pass SUCCEEDED()
    if (!m_dwDDEInst)
    {
        if (FindWindow(_T("Progman"), NULL) == NULL)
        {
            m_hStartEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
            
            m_hThread = LSCreateThread(
                "DDEService", _DDEThreadProc, this, &m_idThread);
            
            WaitForSingleObject(m_hStartEvent, INFINITE);
            CloseHandle(m_hStartEvent);
            hr = S_OK;
        }
    }
    else
    {
        hr = S_FALSE;
    }
    
    return hr;
}


HRESULT DDEService::Stop()
{
    HRESULT hr = S_OK;
    
    if (m_hThread)
    {
        PostThreadMessage(m_idThread, WM_QUIT, 0, 0);
        
        if (WaitForSingleObject(m_hThread, 3000) == WAIT_TIMEOUT)
        {
            TerminateThread(m_hThread, 0);
            hr = S_FALSE;
        }
        
        CloseHandle(m_hThread);
    }
    
    return hr;
}


HRESULT DDEService::Recycle()
{
    return S_OK;
}


bool DDEService::_DoStart()
{
    bool bReturn = false;
    
    if (!m_dwDDEInst)
    {
        CoInitialize(NULL);
        
        UINT uInitReturn = DdeInitialize(&m_dwDDEInst, (PFNCALLBACK)DdeCallback,
            APPCLASS_STANDARD | CBF_FAIL_POKES | CBF_FAIL_SELFCONNECTIONS |
            CBF_SKIP_ALLNOTIFICATIONS, 0L);
        
        if (uInitReturn == DMLERR_NO_ERROR)
        {
            bReturn = SUCCEEDED(_RegisterDDE());
        }
    }
    
    return bReturn;
}


void DDEService::_DoStop()
{
    DdeNameService(m_dwDDEInst, 0L, 0L, DNS_UNREGISTER);
    if (m_hszProgman)
    {
        DdeFreeStringHandle(m_dwDDEInst, m_hszProgman);
    }
    if (m_hszGroups)
    {
        DdeFreeStringHandle(m_dwDDEInst, m_hszGroups);
    }
    if (m_hszFolders)
    {
        DdeFreeStringHandle(m_dwDDEInst, m_hszFolders);
    }
    if (m_hszAppProperties)
    {
        DdeFreeStringHandle(m_dwDDEInst, m_hszAppProperties);
    }
    DdeUninitialize(m_dwDDEInst);
    
    m_dwDDEInst = 0;
    
    CoUninitialize();
}


HDDEDATA CALLBACK DDEService::DdeCallback(
    UINT wType,
    UINT wFmt,
    HCONV /* hConv */,
    HSZ hszTopic,
    HSZ hszItem,
    HDDEDATA hData,
    DWORD /* lData1 */,
    DWORD /* lData2 */)
{
    HDDEDATA hReturn = (HDDEDATA)FALSE;
    
    switch (wType)
    {
    case XTYP_CONNECT:
        {
            hReturn = (HDDEDATA)TRUE;
        }
        break;
        
    case XTYP_WILDCONNECT:
        {
            HSZPAIR FAR *phszp;
            DWORD cb;
            
            if ((!hszTopic || hszTopic == m_hszProgman) &&
                (!hszItem || hszItem == m_hszProgman))
            {
                HDDEDATA hData = DdeCreateDataHandle(m_dwDDEInst, NULL,
                    2 * sizeof(HSZPAIR), 0L, 0, 0, 0);
                
                if (hData)
                {
                    phszp = (HSZPAIR FAR *)DdeAccessData(hData, &cb);
                    phszp[0].hszSvc = m_hszProgman;
                    phszp[0].hszTopic = m_hszProgman;
                    phszp[1].hszSvc = phszp[1].hszTopic = 0;
                    DdeUnaccessData(hData);
                    hReturn = hData;
                }
            }
        }
        break;
        
    case XTYP_EXECUTE:
        {
            if ((hszTopic == m_hszGroups) || (hszTopic == m_hszAppProperties))
            {
                TCHAR tzBuf[MAX_PATH];
                DdeGetData(hData, (LPBYTE)tzBuf, MAX_PATH, 0);
                tzBuf[MAX_PATH - 1] = '\0';
                if (m_DDEWorker.ParseRequest(tzBuf))
                {
                    hReturn = (HDDEDATA)DDE_FACK;
                }
            }
        }
        break;
        
    case XTYP_ADVSTART:
    case XTYP_ADVSTOP:
        {
            if (wFmt == CF_TEXT)
            {
                hReturn = (HDDEDATA)TRUE;
            }
        }
        break;
        
    case XTYP_REQUEST:
    case XTYP_ADVREQ:
        {
            if ((wFmt == CF_TEXT) && ((hszTopic == m_hszProgman) &&
                (hszItem == m_hszGroups)))
            {
                LPVOID pList = NULL;
                UINT ulLen = 0;
                
                if (m_DDEWorker.ListGroups(pList, ulLen))
                {
                    hReturn = DdeCreateDataHandle(m_dwDDEInst, (LPBYTE)pList,
                        ulLen, 0L, m_hszGroups, CF_TEXT, 0);
                    
                    HeapFree(GetProcessHeap(), 0, pList);
                }
            }
        }
        break;
        
    default:
        {
            // do nothing
        }
        break;
    }
    
    return hReturn;
}

#if defined(_UNICODE)
#define DDE_CP CP_WINUNICODE
#else // _UNICODE
#define DDE_CP CP_WINANSI
#endif // _UNICODE

HRESULT DDEService::_RegisterDDE()
{
    HRESULT hr = E_FAIL;
    
    m_hszProgman = DdeCreateStringHandle(m_dwDDEInst, _T("PROGMAN"), DDE_CP);
    if (m_hszProgman != 0L)
    {
        m_hszGroups = DdeCreateStringHandle(m_dwDDEInst, _T("Groups"), DDE_CP);
        if (m_hszGroups != 0L)
        {
            m_hszFolders = DdeCreateStringHandle(m_dwDDEInst, _T("Folders"), DDE_CP);
            if (m_hszFolders != 0L)
            {
                m_hszAppProperties = DdeCreateStringHandle(m_dwDDEInst, _T("AppProperties"), DDE_CP);
                if (m_hszAppProperties != 0L)
                {
                    if (DdeNameService(m_dwDDEInst, m_hszProgman, 0L, DNS_REGISTER) != 0L)
                    {
                        if (DdeNameService(m_dwDDEInst, m_hszFolders, 0L, DNS_REGISTER) != 0L)
                        {
                            hr = S_OK;
                        }
                    }
                }
            }
        }
    }
    
    return hr;
}
