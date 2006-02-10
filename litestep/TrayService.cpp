/*
This is a part of the LiteStep Shell Source code.

Copyright (C) 1997-2006 The LiteStep Development Team

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
#include "TrayService.h"
#include <regstr.h>
#include <shlobj.h>
#include <algorithm>
#include "../utility/macros.h"
#include "../utility/shellhlp.h"
#include "../utility/core.hpp"
#include <docobj.h>

FUNC_PVOID__HANDLE_DWORD fpSHLockShared;
FUNC_BOOL__PVOID fpSHUnlockShared;

#ifndef REGSTR_PATH_SHELLSERVICEOBJECTDELAYED
#define REGSTR_PATH_SHELLSERVICEOBJECTDELAYED _T("Software\\Microsoft\\Windows\\CurrentVersion\\ShellServiceObjectDelayLoad")
#endif

static const TCHAR szTrayClass[]   = _T("Shell_TrayWnd");
static const TCHAR szTrayTitle[]   = _T("Litestep Tray Manager");
static const TCHAR szNotifyClass[] = _T("TrayNotifyWnd");

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
//
// TrayService
//
TrayService::TrayService() :
m_bWin2000(false), m_hNotifyWnd(NULL), m_hTrayWnd(NULL), m_hLiteStep(NULL),
m_hInstance(NULL)
{
}


//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
//
// ~TrayService
//
TrayService::~TrayService()
{}


//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
//
// Start()
//
HRESULT TrayService::Start()
{
    ASSERT_ISNULL(m_hTrayWnd);
    HRESULT hr = E_FAIL;
    
    fpSHLockShared = (FUNC_PVOID__HANDLE_DWORD)GetProcAddress(
         GetModuleHandle(_T("SHELL32"))
        ,(LPCSTR)((long)0x0209)
    );
    fpSHUnlockShared = (FUNC_BOOL__PVOID)GetProcAddress(
         GetModuleHandle(_T("SHELL32"))
        ,(LPCSTR)((long)0x020A)
    );
    
    m_hLiteStep = GetLitestepWnd();
    m_hInstance = GetModuleHandle(NULL);
    
    if (m_hLiteStep && m_hInstance)
    {
        OSVERSIONINFO OsVersionInfo = { 0 };
        OsVersionInfo.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);

        GetVersionEx(&OsVersionInfo);

        if (OsVersionInfo.dwPlatformId == VER_PLATFORM_WIN32_NT &&
            OsVersionInfo.dwMajorVersion >= 5)
        {
            m_bWin2000 = true;
        }

        if (createWindows())
        {
            SetWindowLong(m_hTrayWnd, GWL_USERDATA, magicDWord);
            SetWindowLongPtr(m_hTrayWnd, 0, (LONG)this);
            
            // tell apps to reregister their icons (see Note 6)
            PostMessage(HWND_BROADCAST,
                RegisterWindowMessage(_T("TaskbarCreated")), 0, 0);
            
            if (m_bWin2000)
            {
                loadShellServiceObjects();
            }
            
            hr = S_OK;
        }
    }

    return hr;
}


//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
//
// Stop()
//
HRESULT TrayService::Stop()
{
    HRESULT hr = S_OK;

    if (m_bWin2000 && m_hTrayWnd)
    {
        unloadShellServiceObjects();
    }

    destroyWindows();

    while (!m_siVector.empty())
    {
        delete m_siVector.back();
        m_siVector.pop_back();
    }

    while (!m_abVector.empty())
    {
        delete m_abVector.back();
        m_abVector.pop_back();
    }
    
    fpSHLockShared = NULL;
    fpSHUnlockShared = NULL;
    
    return hr;
}


//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
//
// createWindows
//
bool TrayService::createWindows()
{
    bool bReturn = false;

    //
    // Register tray window class
    //
    WNDCLASSEX wc = { 0 };
    wc.cbSize = sizeof(WNDCLASSEX);
    wc.cbWndExtra = 4;
    wc.lpfnWndProc = TrayService::WindowTrayProc;
    wc.hInstance = m_hInstance;
    wc.lpszClassName = szTrayClass;
    wc.style = CS_DBLCLKS;

    if(!RegisterClassEx(&wc))
    {
        RESOURCE_MSGBOX(m_hInstance, IDS_LITESTEP_REGISTERCLASS_ERROR,
            "Error registering window class.", szTrayClass);
    }
    else
    {
        //
        // Window which receives the tray messages
        //
        m_hTrayWnd = CreateWindowEx(
            WS_EX_TOPMOST | WS_EX_TOOLWINDOW,
            szTrayClass, szTrayTitle,
            WS_POPUP,
            0, 0, 0, 0,
            NULL, NULL,
            m_hInstance,
            NULL);

        if(NULL == m_hTrayWnd)
        {
            UnregisterClass(szTrayClass, m_hInstance);

            RESOURCE_MSGBOX(m_hInstance, IDS_LITESTEP_CREATEWINDOW_ERROR,
                "Unable to create window.", szTrayTitle);
        }
        else
        {
            // Our main window is enough to start up, we can do without the
            // TrayNotifyWnd if necessary.
            bReturn = true;

            //
            // Register "TrayNotifyWnd" class and create window (see Note 8)
            //
            ZeroMemory(&wc, sizeof(wc));
            wc.cbSize = sizeof(WNDCLASSEX);
            wc.lpfnWndProc = TrayService::WindowNotifyProc;
            wc.hInstance = m_hInstance;
            wc.lpszClassName = szNotifyClass;
            wc.style = CS_DBLCLKS;

            if (RegisterClassEx(&wc))
            {
                m_hNotifyWnd = CreateWindowEx(
                    0,
                    szNotifyClass, NULL,
                    WS_CHILD,
                    0, 0, 0, 0,
                    m_hTrayWnd, NULL,
                    m_hInstance,
                    NULL);

                if (!m_hNotifyWnd)
                {
                    UnregisterClass(szNotifyClass, m_hInstance);
                }
            }
        }
    }

    return bReturn;
}


//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
//
// destroyWindows
//
void TrayService::destroyWindows()
{
    if (m_hNotifyWnd)
    {
        DestroyWindow(m_hNotifyWnd);
        m_hNotifyWnd = NULL;

        UnregisterClass(szNotifyClass, m_hInstance);
    }

    if (m_hTrayWnd)
    {
        DestroyWindow(m_hTrayWnd);
        m_hTrayWnd = NULL;
        
        UnregisterClass(szTrayClass, m_hInstance);
    }
}


//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
//
// loadShellServiceObjects
//
// Start the COM based shell services listed in the registry.
//
void TrayService::loadShellServiceObjects()
{
    HKEY hkeyServices;
    int nCounter = 0;
    
    LONG lErrorCode = RegOpenKeyEx(HKEY_LOCAL_MACHINE,
        REGSTR_PATH_SHELLSERVICEOBJECTDELAYED,
        0, KEY_READ, &hkeyServices);
    
    
    while (lErrorCode == ERROR_SUCCESS)
    {
        TCHAR szValueName[32] = { 0 };
        char szData[40] = { 0 };
        DWORD cchValueName = sizeof(szValueName) / sizeof(szValueName[0]);
        DWORD cbData = sizeof(szData);
        DWORD dwDataType;
        
        lErrorCode = RegEnumValue(hkeyServices, nCounter, szValueName,
            &cchValueName, NULL, &dwDataType, (LPBYTE)szData, &cbData);
        
        if (lErrorCode == ERROR_SUCCESS)
        {
            WCHAR wszCLSID[40] = { 0 };
            CLSID clsid;
            IOleCommandTarget* pCmdTarget = NULL;
            
            MultiByteToWideChar(CP_ACP, 0, szData, cbData, wszCLSID, 40);
            
            CLSIDFromString(wszCLSID, &clsid);

            HRESULT hr = CoCreateInstance(clsid, NULL,
                CLSCTX_INPROC_SERVER | CLSCTX_INPROC_HANDLER,
                IID_IOleCommandTarget, (void **)&pCmdTarget);
            
            if (SUCCEEDED(hr))
            {
                pCmdTarget->Exec(&CGID_ShellServiceObject, OLECMDID_NEW,
                    OLECMDEXECOPT_DODEFAULT, NULL, NULL);
                
                m_ssoList.push_back(pCmdTarget);
            }
        }
        
        ++nCounter;
    }
    
    RegCloseKey(hkeyServices);
}


//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
//
// unloadShellServiceObjects
//
// Shut down the COM based shell services.
//
//
void TrayService::unloadShellServiceObjects()
{
    while (!m_ssoList.empty())
    {
        m_ssoList.back()->Exec(&CGID_ShellServiceObject, OLECMDID_SAVE,
            OLECMDEXECOPT_DODEFAULT, NULL, NULL);
        
        m_ssoList.back()->Release();
        m_ssoList.pop_back();
    }
}


//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
//
// WindowProc
//
// Redirects all messages to the appropriate handler
//
LRESULT CALLBACK TrayService::WindowTrayProc(HWND hWnd, UINT uMsg,
                                         WPARAM wParam, LPARAM lParam)
{
    static TrayService* pTrayService = NULL;
    
    if (!pTrayService)
    {
        pTrayService = (TrayService*)GetWindowLongPtr(hWnd, 0);
    }

    if (pTrayService)
    {
        LRESULT lResult = 0;
        
        switch (uMsg)
        {
        case WM_COPYDATA:
            {
                //
                // Undocumented: This is how we can make our own system tray
                // and handle app bar windows. Once a window in the system has
                // the "Shell_TrayWnd" class name, it receives this message!
                //
                COPYDATASTRUCT* pcds = (COPYDATASTRUCT*)lParam;
                
                switch(pcds->dwData)
                {
                case SH_APPBAR_DATA:
                    {
                        TRACE("SH_APPBAR_DATA: %u", pcds->cbData);
                        
                        //
                        // Application Bar Message
                        //
                        switch (pcds->cbData)
                        {
                        case sizeof(APPBARMSGDATAV2):
                            {
                                PAPPBARMSGDATAV2 pamd = (PAPPBARMSGDATAV2)pcds->lpData;
                                
                                TRACE("APPBARMSGDATAV2: %u", pamd->abd.cbSize);
                                
                                if(sizeof(APPBARDATAV2) != pamd->abd.cbSize)
                                    break;
                                
                                SHELLAPPBARDATA sbd((APPBARDATAV1&)(pamd->abd));
                                sbd.dwMessage = pamd->dwMessage;
                                sbd.hSharedMemory = pamd->hSharedMemory;
                                sbd.dwSourceProcessId = pamd->dwSourceProcessId;
                                
                                TRACE("dwMessage: %u", pamd->dwMessage);
                                
                                lResult = pTrayService->HandleAppBarMessage(&sbd);
                            }
                            break;
                            
                        case sizeof(APPBARMSGDATAV1):
                            {
                                PAPPBARMSGDATAV1 pamd = (PAPPBARMSGDATAV1)pcds->lpData;
                                
                                TRACE("APPBARMSGDATAV1: %u", pamd->abd.cbSize);
                                
                                if(sizeof(APPBARDATAV1) != pamd->abd.cbSize)
                                    break;
                                
                                TRACE("dwMessage: %u", pamd->dwMessage);
                                
                                SHELLAPPBARDATA sbd((APPBARDATAV1&)(pamd->abd));
                                sbd.dwMessage = pamd->dwMessage;
                                sbd.hSharedMemory = pamd->hSharedMemory;
                                sbd.dwSourceProcessId = pamd->dwSourceProcessId;
                                
                                lResult = pTrayService->HandleAppBarMessage(&sbd);
                            }
                            break;
                            
                        default:
                            {
                                TRACE("Unknown APPBARMSGDATA size: %u", pcds->cbData);
                            }
                            break;
                        }
                    }
                    break;
                    
                case SH_TRAY_DATA:
                    {
                        //
                        // System Tray Notification
                        //
                        PSHELLTRAYDATA pstd = (PSHELLTRAYDATA)pcds->lpData;
                        
                        lResult = (LRESULT)pTrayService->HandleNotification(pstd);
                    }
                    break;
                    
                case SH_LOADPROC_DATA:
                default:
                    {
                        TRACE("Unsupported tray message: %u", pcds->dwData);
                    }
                    break;
                }
            }
            break;
            
        case ABP_NOTIFYPOSCHANGED:
            {
                BarVector::reverse_iterator rit;
                
                HWND hSkip = (HWND)wParam;
                AppBar* p = NULL;
                
                for(rit = pTrayService->m_abVector.rbegin(); rit != pTrayService->m_abVector.rend(); rit++)
                {
                    if(hSkip != (*rit)->hWnd())
                    {
                        if(!p || p->uEdge() != (*rit)->uEdge())
                        {
                            SendMessage(
                                 (*rit)->hWnd()
                                ,(*rit)->uMsg()
                                ,ABN_POSCHANGED
                                ,0
                            );
                        }
                    }
                    else
                    {
                        p = *rit;
                    }
                }
            }
            break;
            
        case ABP_NOTIFYSTATECHANGE:
            {
                BarVector::reverse_iterator rit;
                
                for(rit = pTrayService->m_abVector.rbegin(); rit != pTrayService->m_abVector.rend(); rit++)
                {
                    if(!(*rit)->IsOverLap())
                    {
                        SendMessage(
                             (*rit)->hWnd()
                            ,(*rit)->uMsg()
                            ,ABN_STATECHANGE
                            ,0
                        );
                    }
                }
            }
            break;
            
        case ABP_RAISEAUTOHIDEHWND:
            {
                HWND hRaise = (HWND)wParam;
                //UINT uEdge = (UINT)lParam;
                
                SetWindowPos(
                     hRaise, HWND_TOP
                    ,0,0,0,0
                    ,SWP_NOACTIVATE|SWP_NOMOVE|SWP_NOSIZE|SWP_NOOWNERZORDER
                );
            }
            break;
            
        default:
            return DefWindowProc(hWnd, uMsg, wParam, lParam);
        }
        
        return lResult;
    }

    return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
//
// WindowNotifyProc
//
// Redirects all messages to the appropriate handler
//
LRESULT CALLBACK TrayService::WindowNotifyProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
//
// HandleAppBarMessage
//
// Handler for all AppBar Messages
//
LRESULT TrayService::HandleAppBarMessage(PSHELLAPPBARDATA psad)
{
    LRESULT lResult = 0;
    
    switch(psad->dwMessage)
    {
    case ABM_NEW:
        lResult = barCreate(psad->abd);
        break;
        
    case ABM_REMOVE:
        lResult = barDestroy(psad->abd);
        break;
        
    case ABM_QUERYPOS:
        lResult = barQueryPos(psad);
        break;
        
    case ABM_SETPOS:
        lResult = barSetPos(psad);
        break;
        
    case ABM_GETSTATE:
        lResult = barGetTaskBarState();
        break;
        
    case ABM_GETTASKBARPOS:
        lResult = barGetTaskBarPos(psad);
        break;
        
    case ABM_ACTIVATE:
        lResult = barActivate(psad->abd);
        break;
        
    case ABM_GETAUTOHIDEBAR:
        lResult = barGetAutoHide(psad->abd.uEdge);
        break;
        
    case ABM_SETAUTOHIDEBAR:
        lResult = barSetAutoHide(psad->abd);
        break;
        
    case ABM_WINDOWPOSCHANGED:
        lResult = barPosChanged(psad->abd);
        break;
        
    case ABM_SETSTATE:
        lResult = barSetTaskBarState(psad->abd);
        break;
        
    default:
        TRACE("ABM unknown: %u", psad->dwMessage);
        break;
    }
    
    return lResult;
}

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
//
// NotifyRudeApp
//
// External interface to let us know a window has gone/left full screen mode
//
void TrayService::NotifyRudeApp(bool bIsFullScreen) const
{
    BarVector::const_reverse_iterator rit;
    
    for(rit = m_abVector.rbegin(); rit != m_abVector.rend(); rit++)
    {
        SendMessage(
             (*rit)->hWnd()
            ,(*rit)->uMsg()
            ,ABN_FULLSCREENAPP
            ,(LPARAM)(bIsFullScreen ? 1:0)
        );
    }
}

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
//
// barCreate
//
// Creates a new AppBar.
//
// ABM_NEW
//
// Valid APPBARDATA members:
//
//  cbSize
//  hWnd
//
LRESULT TrayService::barCreate(const APPBARDATAV1& abd)
{
    LRESULT lResult = 0;
    
    if(IsWindow(abd.hWnd) && !isBar(abd.hWnd))
    {
        m_abVector.push_back(new AppBar(abd.hWnd, abd.uCallbackMessage));
        lResult = 1;
    }
    
    return lResult;
}

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
//
// barDestroy
//
// Removes the specified AppBar.
//
// ABM_REMOVE
//
// Valid APPBARDATA members:
//
//  cbSize
//  hWnd
//
LRESULT TrayService::barDestroy(const APPBARDATAV1& abd)
{
    BarVector::iterator itBar;
    LRESULT lResult = 0;
    
    if(getBar(abd.hWnd, itBar))
    {
        lResult = 1;
        
        delete *itBar;
        m_abVector.erase(itBar);
        
        PostMessage(
             m_hTrayWnd
            ,ABP_NOTIFYPOSCHANGED
            ,(WPARAM)abd.hWnd
            ,(LPARAM)MAKELONG(0, 0)
        );
    }
    
    return lResult;
}

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
//
// barQueryPos
//
// Queries for an acceptable position for the specified appbar
//
// ABM_QUERYPOS
//
// Valid APPBARDATA members:
//
//  cbSize
//  hWnd
//  uEdge
//  rc
//
LRESULT TrayService::barQueryPos(PSHELLAPPBARDATA psad)
{
    LRESULT lResult = 0;
    PAPPBARDATAV1 pabd = ABLock(psad);
    const APPBARDATAV1& abd = psad->abd;
    
    if(pabd)
    {
        AppBar* p;
        
        if(getBar(abd.hWnd, p))
        {
            lResult = 1;
            
            p->uEdge(abd.uEdge);
            
            modifyBar(pabd->rc, abd.rc, p->uEdge());
            
            if(ABS_OVERLAPAUTOHIDE == (p->lParam() & ABS_OVERLAPAUTOHIDE))
            {
                modifyOverlapBar(pabd->rc, abd.rc, p->uEdge());
            }
            else
            {
                modifyNormalBar(pabd->rc, abd);
            }
        }
        
        ABUnLock(pabd);
    }
    
    return lResult;
}

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
//
// barSetPos
//
// Sets the position of the specified appbar
//
// ABM_SETPOS
//
// Valid APPBARDATA members:
//
//  cbSize
//  hWnd
//  uEdge
//  rc
//
LRESULT TrayService::barSetPos(PSHELLAPPBARDATA psad)
{
    LRESULT lResult = 0;
    PAPPBARDATAV1 pabd = ABLock(psad);
    const APPBARDATAV1& abd = psad->abd;
    
    if(pabd)
    {
        AppBar* p;
        
        if(getBar(abd.hWnd, p))
        {
            lResult = 1;
            
            p->uEdge(abd.uEdge);
            
            modifyBar(pabd->rc, abd.rc, p->uEdge());
            
            if(ABS_OVERLAPAUTOHIDE == (p->lParam() & ABS_OVERLAPAUTOHIDE))
            {
                modifyOverlapBar(pabd->rc, abd.rc, p->uEdge());
            }
            else
            {
                modifyNormalBar(pabd->rc, abd);
                
                if((ABS_CLEANRECT != (ABS_CLEANRECT & p->lParam()))
                   || !EqualRect(&(p->GetRectRef()), &(pabd->rc)))
                {
                    // Set Work Area
                    // TODO: Integrate work area management, and have
                    // desktop modules interact with us.
                    
                    // Notify other bars
                    PostMessage(
                         m_hTrayWnd
                        ,ABP_NOTIFYPOSCHANGED
                        ,(WPARAM)abd.hWnd
                        ,(LPARAM)MAKELONG(1, abd.uEdge) //0x00010001
                    );
                }
            }
            
            CopyRect(&(p->GetRectRef()), &(pabd->rc));
            p->lParam(p->lParam() | ABS_CLEANRECT);
        }
        
        ABUnLock(pabd);
    }
    
    return lResult;
}

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
//
// barGetTaskBarState
//
// Returns the current TaskBar state (autohide, alwaysontop)
//
// ABM_GETSTATE
//
// Valid APPBARDATA members:
//
//  cbSize
//
LRESULT TrayService::barGetTaskBarState()
{
    return 0;
}

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
//
// barGetTaskBarPos
//
// Gets the current TaskBar position
//
// ABM_GETTASKBARPOS
//
// Valid APPBARDATA members:
//
//  cbSize
//  hWnd
//
LRESULT TrayService::barGetTaskBarPos(PSHELLAPPBARDATA psad)
{
    LRESULT lResult = 0;
    PAPPBARDATAV1 pabd = ABLock(psad);
    
    if(pabd)
    {
        if(GetWindowRect(m_hNotifyWnd, &(pabd->rc)))
        {
            lResult = 1;
        }
        
        ABUnLock(pabd);
    }
    
    return lResult;
}

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
//
// barActivate
//
// Ensures that any autohide appbar has the topmost zOrder when any appbar is
// activated.
//
// ABM_ACTIVATE
//
// Valid APPBARDATA members:
//
//  cbSize
//  hWnd
//
LRESULT TrayService::barActivate(const APPBARDATAV1& abd)
{
    LRESULT lResult = 0;
    AppBar* p;
    
    if(getBar(abd.hWnd, p))
    {
        BarVector::iterator itBar = findBar(p->uEdge(), ABS_AUTOHIDE);
        
        if(itBar != m_abVector.end())
        {
            PostMessage(
                 m_hTrayWnd
                ,ABP_RAISEAUTOHIDEHWND
                ,(WPARAM)(*itBar)->hWnd()
                ,(LPARAM)(*itBar)->uEdge()
            );
        }
        
        lResult = 1;
    }
    
    return lResult;
}

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
//
// barGetAutoHide
//
// Returns the HWND of the autohide appbar on the specified screen edge
//
// ABM_GETAUTOHIDEBAR
//
// Valid APPBARDATA members:
//
//  cbSize
//  hWnd
//  uEdge
//
LRESULT TrayService::barGetAutoHide(UINT uEdge)
{
    LRESULT lResult = 0;
    BarVector::iterator itBar = findBar(uEdge, ABS_AUTOHIDE);
    
    if(itBar != m_abVector.end())
    {
        lResult = (LRESULT)(*itBar)->hWnd();
    }
    
    return lResult;
}

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
//
// barSetAutoHide
//
// Sets the specified AppBar to autohide if it already exists, otherwise
// if the specified AppBar does not exist, it creates it and sets it to as
// an overlap autohide AppBar.
//
// ABM_SETAUTOHIDEBAR
//
// Valid APPBARDATA members:
//
//  cbSize
//  hWnd
//  uEdge
//  lParam
//
LRESULT TrayService::barSetAutoHide(const APPBARDATAV1& abd)
{
    LRESULT lResult = 0;
    
    BarVector::iterator itBar = findBar(abd.hWnd);
    BarVector::iterator itAutoHideBar = findBar(abd.uEdge, ABS_AUTOHIDE);
    
    if(abd.lParam) // Set Auto Hide
    {
        lResult = 1;
        
        // Does an autohide bar already exist on this edge?
        if(itAutoHideBar != m_abVector.end())
        {
            // Return true if this appbar is the autohide bar.
            return (itBar == itAutoHideBar);
        }
        
        // if bar doesn't exist, create it and set it as an overlap bar
        if(itBar == m_abVector.end())
        {
            if(!barCreate(abd))
            {
                return 0;
            }
            
            itBar = m_abVector.end()-1;
            (*itBar)->lParam((*itBar)->lParam() | ABS_OVERLAPAUTOHIDE);
        }
        
        // set edge bar is assigned to, and set that it an autohide bar
        (*itBar)->uEdge(abd.uEdge);
        (*itBar)->lParam((*itBar)->lParam() | ABS_AUTOHIDE);
    }
    else // Clear Auto Hide
    {
        // if the bar was the auto hide bar proceed
        if(itBar != m_abVector.end() && itBar == itAutoHideBar)
        {
            lResult = 1;
            
            // if bar was overlap, destroy it
            if((*itBar)->lParam() & ABS_OVERLAPAUTOHIDE)
            {
                barDestroy(abd);
            }
            else
            {
                // otherwise clear its autohide status.
                (*itBar)->lParam((*itBar)->lParam() & ~ABS_AUTOHIDE);
            }
        }
    }
    
    return lResult;
}

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
//
// barPosChanged
//
// Ensures that any autohide appbar has the topmost zOrder when any non autohide
// appbar's position has changed.
//
// ABM_WINDOWPOSCHANGED
//
// Valid APPBARDATA members:
//
//  cbSize
//  hWnd
//
LRESULT TrayService::barPosChanged(const APPBARDATAV1& abd)
{
    LRESULT lResult = 0;
    AppBar* p;
    
    if(getBar(abd.hWnd, p) && !p->IsAutoHide())
    {
        BarVector::iterator itBar = findBar(p->uEdge(), ABS_AUTOHIDE);
        
        if(itBar != m_abVector.end())
        {
            PostMessage(
                 m_hTrayWnd
                ,ABP_RAISEAUTOHIDEHWND
                ,(WPARAM)(*itBar)->hWnd()
                ,(LPARAM)(*itBar)->uEdge()
            );
        }
        
        lResult = 1;
    }
    
    return lResult;
}

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
//
// barSetTaskBarState
//
// Sets the current TaskBar state (autohide, alwaysontop)
//
// ABM_SETSTATE
//
// Valid APPBARDATA members:
//
//  cbSize
//  hWnd
//  lParam
//
LRESULT TrayService::barSetTaskBarState(const APPBARDATAV1& abd)
{
    return 0;
}

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
//
// modifyOverlapBar
//
// Helper function to barSetPos and barQueryPos
//
void TrayService::modifyOverlapBar(RECT& rcDst, const RECT& rcSrc, UINT uEdge)
{
    switch(uEdge)
    {
    case ABE_LEFT:
        rcDst.right = rcDst.left + (rcSrc.right - rcSrc.left);
        break;
    case ABE_TOP:
        rcDst.bottom = rcDst.top + (rcSrc.bottom - rcSrc.top);
        break;
    case ABE_RIGHT:
        rcDst.left = rcDst.right - (rcSrc.right - rcSrc.left);
        break;
    case ABE_BOTTOM:
        rcDst.top = rcDst.bottom - (rcSrc.bottom - rcSrc.top);
        break;
    }
    
    return;
}

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
//
// modifyNormalBar
//
// Helper function to barSetPos and barQueryPos
//
void TrayService::modifyNormalBar(RECT& dtRect, const APPBARDATAV1& abd)
{
    BarVector::const_iterator it;
    AppBar* p;
    bool bFound = FALSE;
    
    for(it = m_abVector.begin(); it != m_abVector.end(); it++)
    {
        p = *it;
        
        // ignore overlap bars and invalid windows
        if(!p || p->IsOverLap())
        {
        }
        /* if this is our appbar, mark it found and continue */
        else if(abd.hWnd == p->hWnd())
        {
            bFound = true;
        }
        // ignore bars that haven't been initialized
        else if(ABS_CLEANRECT != (ABS_CLEANRECT & p->lParam()))
        {
        }
        /* Ignore other edge appbars for the most part */
        else if(abd.uEdge != p->uEdge())
        {
            /* Left/Right bars must resize to top/bottom bars */
            if(ABE_HORIZONTAL != (ABE_HORIZONTAL & abd.uEdge)
                &&
               ABE_HORIZONTAL == (ABE_HORIZONTAL & p->uEdge()))
            {
                RECT r; // dummy
                
                /* If our destination rectangle currently intersects
                 * then resize it */
                if(IntersectRect(&r, &(p->GetRectRef()), &dtRect))
                {
                    if(ABE_TOP == p->uEdge())
                    {
                        dtRect.top = p->GetRectRef().bottom;
                    }
                    else
                    {
                        dtRect.bottom = p->GetRectRef().top;
                    }
                }
            }
        }
        /* Keep moving the destination appbar while we are looking
         * at same edge appbars before our own.  Once we find our
         * own, then we ignore all same edge appbars after ours. */
        else if(!bFound)
        {
            switch(abd.uEdge)
            {
            case ABE_LEFT:
                dtRect.left = p->GetRectRef().right;
                break;
            case ABE_TOP:
                dtRect.top = p->GetRectRef().bottom;
                break;
            case ABE_RIGHT:
                dtRect.right = p->GetRectRef().left;
                break;
            case ABE_BOTTOM:
                dtRect.bottom = p->GetRectRef().top;
                break;
            }
        }
    }
    
    switch(abd.uEdge)
    {
    case ABE_LEFT:
        dtRect.right = dtRect.left + (abd.rc.right - abd.rc.left);
        break;
    case ABE_TOP:
        dtRect.bottom = dtRect.top + (abd.rc.bottom - abd.rc.top);
        break;
    case ABE_RIGHT:
        dtRect.left = dtRect.right - (abd.rc.right - abd.rc.left);
        break;
    case ABE_BOTTOM:
        dtRect.top = dtRect.bottom - (abd.rc.bottom - abd.rc.top);
        break;
    }
    
    return;
}

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
//
// modifyBar
//
// Helper function to barSetPos and barQueryPos
//
void TrayService::modifyBar(RECT& rcDst, const RECT& rcSrc, UINT uEdge)
{
    GetWindowRect(GetDesktopWindow(), &rcDst);
    
    /* decrease to requesting bounding rect */
    switch(uEdge)
    {
    case ABE_LEFT:
    case ABE_RIGHT:
        rcDst.top = rcSrc.top;
        rcDst.bottom = rcSrc.bottom;
        break;
    case ABE_TOP:
    case ABE_BOTTOM:
        rcDst.left = rcSrc.left;
        rcDst.right = rcSrc.right;
        break;
    }
    
    return;
}

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
//
// ABLock
//
// Calls SHLockShared API for the specified Shell AppBar memory
//
// Returns a pointer to shared memory location for an AppBarData structure
//
PAPPBARDATAV1 TrayService::ABLock(PSHELLAPPBARDATA psad)
{
    if(NULL != fpSHLockShared && NULL != psad->hSharedMemory)
    {
        return (PAPPBARDATAV1)fpSHLockShared(psad->hSharedMemory, psad->dwSourceProcessId);
    }
    
    return NULL;
}

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
//
// ABUnlock
//
// Calls SHUnlockShared API for the specified AppBarData* memory
//
void TrayService::ABUnLock(PAPPBARDATAV1 pabd)
{
    if(NULL != fpSHUnlockShared && NULL != pabd)
    {
        fpSHUnlockShared(pabd);
    }
}

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
//
// FindAppBarPredicate_hWnd
//
// Predicate for std::find_if, used by findBar variants
// Needs to be at global scope because of mingw issues
//
// Find the appbar matching the specified hWnd
//
struct FindAppBarPredicate_hWnd
{
    FindAppBarPredicate_hWnd(HWND hWnd) : m_hWnd(hWnd)
    {}
    
    bool operator() (const AppBar* pab) const
    {
        return (pab->hWnd() == m_hWnd);
    }
    
private:
    const HWND m_hWnd;
};


//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
//
// FindAppBarPredicate_MatchLParam
//
// Predicate for std::find_if, used by findBar variants
// Needs to be at global scope because of mingw issues
//
// Find the appbar on the specified edge that has a matching lParam value
//
struct FindAppBarPredicate_MatchLParam
{
    FindAppBarPredicate_MatchLParam(UINT uEdge, LPARAM lParam) : m_uEdge(uEdge), m_lParam(lParam)
    {}
    
    bool operator() (const AppBar* pab) const
    {
        return (pab->uEdge() == m_uEdge && m_lParam == (pab->lParam() & m_lParam));
    }
    
private:
    const UINT m_uEdge;
    const LPARAM m_lParam;
};

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
//
// findBar
//
// Looks up an AppBar in the AppBarList
//
BarVector::iterator TrayService::findBar(HWND hWnd)
{
    return std::find_if(m_abVector.begin(), m_abVector.end(),
                        FindAppBarPredicate_hWnd(hWnd));
}
BarVector::iterator TrayService::findBar(UINT uEdge, LPARAM lParam)
{
    return std::find_if(m_abVector.begin(), m_abVector.end(),
                        FindAppBarPredicate_MatchLParam(uEdge, lParam));
}

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
//
// isBar
//
// Determines whether the HWND is associated with any current AppBar.
//
bool TrayService::isBar(HWND hWnd)
{
    return m_abVector.end() != findBar(hWnd);
}

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
//
// getBar
//
// Looks up an BarVector iterator based on the HWND
//
// returns true if found, otherwise false.
//
bool TrayService::getBar(HWND hWnd, BarVector::iterator& itBar)
{
    bool bReturn = false;
    
    itBar = findBar(hWnd);
    
    if(itBar != m_abVector.end())
    {
        bReturn = true;
    }
    
    return bReturn;
}

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
//
// getBar
//
// Looks up an AppBar* based on the HWND
//
// Returns true if found, otherwise false.
//
bool TrayService::getBar(HWND hWnd, AppBar*& pBarRef)
{
    bool bReturn = false;
    BarVector::iterator itBar;
    
    if(getBar(hWnd, itBar))
    {
        pBarRef = *itBar;
        bReturn = true;
    }
    
    return bReturn;
}

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
//
// HandleNotification
//
// Handler for all system tray notifications
//
BOOL TrayService::HandleNotification(PSHELLTRAYDATA pstd)
{
    bool bReturn = false;

    switch (pstd->dwMessage)
    {
    case NIM_ADD:
        {
            bReturn = addIcon(pstd->nid);
        }
        break;

    case NIM_MODIFY:
        {
            bReturn = modifyIcon(pstd->nid);
        }
        break;

    case NIM_DELETE:
        {
            bReturn = deleteIcon(pstd->nid);
        }
        break;

    case NIM_SETFOCUS:
        {
            bReturn = setFocusIcon(pstd->nid);
        }
        break;

    case NIM_SETVERSION:
        {
            bReturn = setVersionIcon(pstd->nid);
        }
        break;

    default:
        {
            TRACE("NIM unknown: %u", pstd->dwMessage);
        }
        break;
    }

    return bReturn ? TRUE : FALSE;
}


//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
//
// notify
//
// Notify all listeners of a systray event
//
bool TrayService::notify(DWORD dwMessage, PCLSNOTIFYICONDATA pclsnid) const
{
    return 0 != SendMessage(m_hLiteStep, LM_SYSTRAY, dwMessage, (LPARAM)pclsnid);
}


//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
//
// SendSystemTray
//
// Resend all icon data; systray modules will request this via LM_SYSTRAYREADY
// during their startup.
//
HWND TrayService::SendSystemTray() const
{
    for (IconVector::const_iterator it = m_siVector.begin();
         it != m_siVector.end(); ++it)
    {
        if((*it)->IsValid())
        {
            LSNOTIFYICONDATA lsnid = {0};

            (*it)->CopyLSNID(&lsnid);

            notify(NIM_ADD, &lsnid);
        }
    }

    return m_hNotifyWnd;
}


//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
//
// extendNIDCopy
//
// Copies all non-persistent data that is not stored in NotifyIcon.
//
bool TrayService::extendNIDCopy(LSNOTIFYICONDATA& lsnid, const NID_XX& nid) const
{
    // Copy Info Tips
    if(nid.uFlags & NIF_INFO)
    {
        switch(nid.cbSize)
        {
        case NID_6W_SIZE:
        case NID_5W_SIZE:
            {
                NID_5W* pnid = (NID_5W*)&nid;

                int nReturn;

                nReturn = WideCharToMultiByte(CP_ACP, 0, pnid->szInfo, 256,
                    lsnid.szInfo, TRAY_MAX_INFO_LENGTH, NULL, NULL);

                if(!nReturn)
                {
                    lsnid.szInfo[0] = 0;
                }

                nReturn = WideCharToMultiByte(CP_ACP, 0, pnid->szInfoTitle, 64,
                    lsnid.szInfoTitle, TRAY_MAX_INFOTITLE_LENGTH, NULL, NULL);

                if(!nReturn)
                {
                    lsnid.szInfoTitle[0] = 0;
                }

                lsnid.dwInfoFlags = pnid->dwInfoFlags;
                lsnid.uTimeout = pnid->uTimeout;
                lsnid.uFlags |= NIF_INFO;
            }
            break;
        case NID_6A_SIZE:
        case NID_5A_SIZE:
            {
                NID_5A* pnid = (NID_5A*)&nid;

                HRESULT hr;

                hr = StringCchCopy(lsnid.szInfo, TRAY_MAX_INFO_LENGTH, pnid->szInfo);

                if(FAILED(hr))
                {
                    lsnid.szInfo[0] = 0;
                }

                hr = StringCchCopy(lsnid.szInfoTitle, TRAY_MAX_INFOTITLE_LENGTH, pnid->szInfoTitle);

                if(FAILED(hr))
                {
                    lsnid.szInfoTitle[0] = 0;
                }

                lsnid.dwInfoFlags = pnid->dwInfoFlags;
                lsnid.uTimeout = pnid->uTimeout;
                lsnid.uFlags |= NIF_INFO;
            }
            break;
        default:
            break;
        }
    }

    // Copy GUID
    if(nid.uFlags & NIF_GUID)
    {
        switch(nid.cbSize)
        {
        case NID_6W_SIZE:
            {
                NID_6W* pnid = (NID_6W*)&nid;

                lsnid.guidItem = pnid->guidItem;
                lsnid.uFlags |= NIF_GUID;
            }
            break;
        case NID_6A_SIZE:
            {
                NID_6A* pnid = (NID_6A*)&nid;

                lsnid.guidItem = pnid->guidItem;
                lsnid.uFlags |= NIF_GUID;
            }
            break;
        default:
            break;
        }
    }

    return true;
}


//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
//
// addIcon
//
bool TrayService::addIcon(const NID_XX& nid)
{
    bool bReturn = false;

    if(m_siVector.end() == findIcon(nid))
    {
        NotifyIcon * pni = new NotifyIcon(nid);

        if (pni)
        {
            /* Fail shared icons, unless a valid hIcon exists */
            if(IsWindow(pni->GetHwnd()) && (!pni->IsShared() || pni->HasIcon()))
            {
                m_siVector.push_back(pni);

                if(pni->IsValid())
                {
                    LSNOTIFYICONDATA lsnid = {0};

                    pni->CopyLSNID(&lsnid);
                    extendNIDCopy(lsnid, nid);

                    notify(NIM_ADD, &lsnid);
                }
                bReturn = true;
            }
            else
            {
                delete pni;
            }
        }
    }
    
    return bReturn;
}


//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
//
// modifyIcon
//
bool TrayService::modifyIcon(const NID_XX& nid)
{
    bool bReturn = false;

    IconVector::iterator it = findIcon(nid);

    if(m_siVector.end() != it && *it)
    {
        NotifyIcon * pni = *it;

        bool bWasValid = pni->IsValid();

        // Update stored NotifyIcon
        pni->Update(nid);

        if(pni->IsValid())
        {
            LSNOTIFYICONDATA lsnid = {0};

            if(!bWasValid)
            {
                // This is a "new" icon, send entire stored NotifyIcon
                pni->CopyLSNID(&lsnid);
                extendNIDCopy(lsnid, nid);

                notify(NIM_ADD, &lsnid);
            }
            else
            {
                // This icon already exists, just send updated flags
                pni->CopyLSNID(&lsnid, nid.uFlags);
                extendNIDCopy(lsnid, nid);

                notify(NIM_MODIFY, &lsnid);
            }
        }
        else if(bWasValid)
        {
            LSNOTIFYICONDATA lsnid = {
                 sizeof(LSNOTIFYICONDATA)
                ,pni->GetHwnd()
                ,pni->GetuID()
                ,0
            };

            // This icon is no longer visible, remove
            notify(NIM_DELETE, &lsnid);
        }

        bReturn = true;
    }

    return bReturn;
}


//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
//
// deleteIcon
//
// Remove an icon from the icon list
//
bool TrayService::deleteIcon(const NID_XX& nid)
{
    bool bReturn = false;

    IconVector::iterator it = findIcon(nid);

    if(m_siVector.end() != it)
    {
        LSNOTIFYICONDATA lsnid = {
             sizeof(LSNOTIFYICONDATA)
            ,(*it)->GetHwnd()
            ,(*it)->GetuID()
            ,0
        };

        notify(NIM_DELETE, &lsnid);

        delete *it;
        m_siVector.erase(it);

        bReturn = true;
    }

    return bReturn;
}


//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
//
// setFocusIcon
//
// Sets selection focus to the specified icon
//
bool TrayService::setFocusIcon(const NID_XX& nid)
{
    bool bReturn = false;

    IconVector::iterator it = findIcon(nid);

    if(m_siVector.end() != it)
    {
        LSNOTIFYICONDATA lsnid = {
             sizeof(LSNOTIFYICONDATA)
            ,(*it)->GetHwnd()
            ,(*it)->GetuID()
            ,0
        };

        bReturn = notify(NIM_SETFOCUS, &lsnid);
    }

    return bReturn;
}


//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
//
// setVersionIcon
//
// Sets compatibility version for specified icon
//
bool TrayService::setVersionIcon(const NID_XX& nid)
{
    bool bReturn = false;

    IconVector::iterator it = findIcon(nid);

    if(m_siVector.end() != it)
    {
        LSNOTIFYICONDATA lsnid = {
             sizeof(LSNOTIFYICONDATA)
            ,(*it)->GetHwnd()
            ,(*it)->GetuID()
            ,0
        };

        switch(nid.cbSize)
        {
        case NID_6W_SIZE:
        case NID_5W_SIZE:
            lsnid.uVersion = ((NID_5W&)nid).uVersion;
            break;

        case NID_6A_SIZE:
        case NID_5A_SIZE:
            lsnid.uVersion = ((NID_5A&)nid).uVersion;
            break;

        default:
            break;
        }

        bReturn = notify(NIM_SETVERSION, &lsnid);
    }

    return bReturn;
}


//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
//
// FindIconPredicate
//
// Predicate for std::find_if, used by findIcon
// Needs to be at global scope because of mingw issues
//
struct FindIconPredicate
{
    FindIconPredicate(HWND hWnd, UINT uID) : m_hWnd(hWnd), m_uID(uID) {}

    bool operator() (const NotifyIcon* pni) const
    {
        return (pni->GetHwnd() == m_hWnd && pni->GetuID() == m_uID);
    }

private:
    HWND m_hWnd;
    UINT m_uID;
};


//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
//
// findIcon
//
// Looks up an icon in the SystrayIconList
//
IconVector::iterator TrayService::findIcon(HWND hWnd, UINT uId)
{
    return std::find_if(m_siVector.begin(),
        m_siVector.end(), FindIconPredicate(hWnd, uId));
}

