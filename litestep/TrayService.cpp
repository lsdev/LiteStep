//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//
// This is a part of the Litestep Shell source code.
//
// Copyright (C) 1997-2007  Litestep Development Team
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
#include "TrayService.h"
#include <regstr.h>
#include <shlobj.h>
#include <algorithm>
#include "../utility/macros.h"
#include "../utility/shellhlp.h"
#include "../utility/core.hpp"
#include <docobj.h>

#if !defined(REGSTR_PATH_SHELLSERVICEOBJECTDELAYED)
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
    ASSERT(NULL == m_hTrayWnd);
    HRESULT hr = E_FAIL;
    
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
            SetWindowLongPtr(m_hTrayWnd, 0, (LONG_PTR)this);
            
            // http://forums.microsoft.com/MSDN/ShowPost.aspx?PostID=1427677&SiteID=1
            //
            // Re: TaskbarCreated message (not working in Vista)
            //
            //  > The code is still in there to broadcast the message.
            //  > SendNotifyMessage(HWND_BROADCAST, RegisterWindowMessage(TEXT("TaskbarCreated")), 0, 0);
            //
            //  >> It looks as though applications running as admin cannot receive the message.
            //  >> Presumably, this is because Explorer, which publishes the message, is not
            //  >> running as admin
            //
            //  >>> The message is blocked by User Interface Privilege Isolation, Administrative
            //  >>> applications that need to see it can allow it through by calling
            //  >>> ChangeWindowMessageFilter after making sure the necessary security precautions
            //  >>> are in place. 
            
            // tell apps to reregister their icons
            SendNotifyMessage(HWND_BROADCAST,
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
    wc.cbWndExtra = sizeof(TrayService*);
    wc.lpfnWndProc = TrayService::WindowProc;
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
            wc.lpfnWndProc = TrayService::WindowProc;
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
LRESULT CALLBACK TrayService::WindowProc(HWND hWnd, UINT uMsg,
                                               WPARAM wParam, LPARAM lParam)
{
    static TrayService* pTrayService = NULL;
    
    if (!pTrayService)
    {
        pTrayService = (TrayService*)GetWindowLongPtr(hWnd, 0);
    }
    
    if (pTrayService)
    {
        switch (uMsg)
        {
        case WM_COPYDATA:
            {
                //
                // Undocumented: This is how we can make our own system tray
                // and eventually handle app bar windows. Once a window in
                // the system has the "Shell_TrayWnd" class name, it receives
                // this message!
                //
                COPYDATASTRUCT* pcds = (COPYDATASTRUCT*)lParam;
                
                switch(pcds->dwData)
                {
                case SH_APPBAR_DATA:
                    {
                        //
                        // Application Bar Message
                        //
                        PSHELLAPPBARDATA pData = (PSHELLAPPBARDATA)pcds->lpData;
                        
                        return pTrayService->HandleAppBarMessage(pData);
                    }
                    break;
                    
                case SH_TRAY_DATA:
                    {
                        //
                        // System Tray Notification
                        //
                        PSHELLTRAYDATA pstd = (PSHELLTRAYDATA)pcds->lpData;
                        
                        return pTrayService->HandleNotification(pstd);
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
            
        default:
            break;
        }
    }

    return DefWindowProc(hWnd, uMsg, wParam, lParam);
}


//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
//
// HandleAppBarMessage
//
// Handler for all AppBar Messages
//
BOOL TrayService::HandleAppBarMessage(PSHELLAPPBARDATA pData)
{
    TRACE("Received AppBar Message %u; DontKnow: %u; NoClue: %u",
        pData->dwMessage, pData->dwDontKnow, pData->dwNoClue);
    
    //
    // Hack that could let some AppBars work
    //
    switch(pData->dwMessage)
    {
    case ABM_NEW:
    case ABM_REMOVE:
    case ABM_QUERYPOS:
    case ABM_SETPOS:
    case ABM_ACTIVATE:
    case ABM_WINDOWPOSCHANGED:
        {
            return TRUE;
        }
        break;
        
    default:
        break;
    }
    
    return FALSE;
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
HWND TrayService::SendSystemTray()
{
    removeDeadIcons();

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
// removeDeadIcons
//
// Removes all "dead" icons.  ie. Those that no longer have a valid HWND
// associated with them.
//
void TrayService::removeDeadIcons()
{
    IconVector::iterator it = m_siVector.begin();
    
    while(it != m_siVector.end())
    {
        if(IsWindow((*it)->GetHwnd()))
        {
            it++;
            continue;
        }
        
        delete *it;
        it = m_siVector.erase(it);
    }
    
    return;
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
            // Fail shared icons, unless a valid hIcon exists
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
    const HWND m_hWnd;
    const UINT m_uID;
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
