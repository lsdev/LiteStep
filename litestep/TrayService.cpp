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
// NOTES // NOTES // NOTES // NOTES // NOTES // NOTES // NOTES // NOTES //
/****************************************************************************
Note 1:  The offline file sync is sent only with a valid uMessageCallback 
         (NIF_MESSAGE) (W2K)
 
Note 2:  The DUN/Networking icons sent using the NIS_HIDDEN dwState flags use 
         an invalid uID (usually 0xFFFFFFFF) (W2K/XP)
 
Note 3:  The window handling the DUN/Networking icons has a class and caption of
         "Connections Tray" (FindWindowEx(NULL, NULL, "Connections Tray, NULL))
		 (W2K/XP)
 
Note 4:  Tooltip (NIF_TIP) for the DUN/Networking icons isn't created until the
         tray icon receives its first mouseover event (W2K/XP)
 
Note 5:  The DUN/Networking Icons appear to use NIS_SHAREDICON with two
         different uIDs. This will cause two icons to display with the same
         functionality. One icon will not show a tooltip. Therefore as a
         workaround, find the "Connections Tray" window, and everytime a
         notification comes in matching that hWnd and without a NIF_TIP flag,
         post a WM_MOUSEMOVE message with the uID and uCallbackMessage to the
         "Connections Tray" window. This will cause a tooltip to be added to the
         true icon next time it is sent. The other icon will remain with out a
         tooltip and can be ignored. (XP)
 
Note 6:  RegisterWindowMessage("TaskbarCreated") should be sent with
         HWND_BROADCAST before the shell service objects are loaded (sending
         afterwards would result in a Volume icon flicker or loss because the
         shell service object would delete the icon) Also ensure the
         "TaskbarCreated" message is sent only once. (W2K/XP)

Note 7:  Volume/DUN/Networking icons are controlled by shell service objects
         (SSO). They are COM servers implementing the IOleCommandTarget
         interface. The CLSIDs are listed in 
		 "Software\\Microsoft\\Windows\\CurrentVersion\\ShellServiceObjectDelayLoad" 
         They should be enumerated and created using IID_IOleCOmmandTarget. 
		 Exec each SSO with a group id of CGID_ShellServiceObject and command id 
		 of 2 to start. Exec each SSO with a group id of CGID_ShellServiceObject 
		 and a command id of 3 to stop.
****************************************************************************/
#include "TrayService.h"
#include <regstr.h>
#include <shlobj.h>
#include "../utility/macros.h"
#include "../utility/shellhlp.h"
#include "../utility/core.hpp"

#ifdef __GNUC__ // mingw workarounds
#  include <initguid.h>
#  include <docobj.h>
#  if __MINGW32_MAJOR_VERSION < 3 // mingw 3.x headers already have this symbol
     DEFINE_GUID(CGID_ShellServiceObject, 0x000214D2, 0, 0, 0xC0,0,0,0,0,0,0,0x46);
#  endif
#endif

#ifndef REGSTR_PATH_SHELLSERVICEOBJECTDELAYED
#define REGSTR_PATH_SHELLSERVICEOBJECTDELAYED _T("Software\\Microsoft\\Windows\\CurrentVersion\\ShellServiceObjectDelayLoad")
#endif

const char szTrayClass[]   = "Shell_TrayWnd";
const char szTrayTitle[]   = "Litestep Tray Manager";
const char szNotifyClass[] = "TrayNotifyWnd";


//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
//
// TrayService
//
TrayService::TrayService() :
m_hNotifyWnd(NULL), m_hTrayWnd(NULL), m_hLiteStep(NULL),
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
    ASSERT(m_hTrayWnd == NULL);
    HRESULT hr = E_FAIL;
    
    m_hLiteStep = GetLitestepWnd();
    m_hInstance = GetModuleHandle(NULL);
    
    if (m_hLiteStep && m_hInstance)
    {
        _CreateWindows();
        
        // Our main window is enough to start up, we can do without the
        // TrayNotifyWnd if necessary.
        if (m_hTrayWnd)
        {
            SetWindowLong(m_hTrayWnd, GWL_USERDATA, magicDWord);
            SetWindowLong(m_hTrayWnd, 0, (LONG)this);
            
            // tell apps to reregister their icons (see Note 6)
            PostMessage(HWND_BROADCAST,
                RegisterWindowMessage("TaskbarCreated"), 0, 0);
            
            _LoadShellServiceObjects();
            
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

    _UnloadShellServiceObjects();

    _DestroyWindows();

    while (!m_siVector.empty())
    {
        if (m_siVector.back()->hIcon)
        {
            DestroyIcon(m_siVector.back()->hIcon);
        }

        delete m_siVector.back();
        m_siVector.pop_back();
    }

	return hr;
}


//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
//
// _CreateWindows
//
bool TrayService::_CreateWindows()
{
    bool bReturn = false;

    //
    // Register tray window class
    //
    WNDCLASSEX wc = { 0 };
    wc.cbSize = sizeof(WNDCLASSEX);
    wc.cbWndExtra = 4;
    wc.lpfnWndProc = TrayService::WindowProc;
    wc.hInstance = m_hInstance;
    wc.lpszClassName = szTrayClass;
    wc.style = CS_DBLCLKS;
    
    if (RegisterClassEx(&wc))
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

        if (m_hTrayWnd)
        {
            //
            // Register "TrayNotifyWnd" class and create window
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

                if (m_hNotifyWnd)
                {
                    bReturn = true;
                }
                else
                {
                    UnregisterClass(szNotifyClass, m_hInstance);
                    
                    RESOURCE_MSGBOX(m_hInstance, IDS_LITESTEP_CREATEWINDOW_ERROR,
                        "Unable to create window.", szNotifyClass);
                }
            }
            else
            {
                RESOURCE_MSGBOX(m_hInstance, IDS_LITESTEP_REGISTERCLASS_ERROR,
                    "Error registering window class.", szTrayClass);
            }
        }
        else
        {
            UnregisterClass(szTrayClass, m_hInstance);
            
            RESOURCE_MSGBOX(m_hInstance, IDS_LITESTEP_CREATEWINDOW_ERROR,
                "Unable to create window.", szTrayTitle);
        }
    }
    else
    {
        RESOURCE_MSGBOX(m_hInstance, IDS_LITESTEP_REGISTERCLASS_ERROR,
            "Error registering window class.", szTrayClass);
    }

    return bReturn;
}


//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
//
// _DestroyWindows
//
void TrayService::_DestroyWindows()
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
// _LoadShellServiceObjects
//
// Start the COM based shell services listed in the registry.
//
void TrayService::_LoadShellServiceObjects()
{
    HKEY hkeyServices;
    int nCounter = 0;
    
    LONG lErrorCode = RegOpenKeyEx(HKEY_LOCAL_MACHINE,
        REGSTR_PATH_SHELLSERVICEOBJECTDELAYED,
        0, KEY_READ, &hkeyServices);
    
    
    while (lErrorCode == ERROR_SUCCESS)
    {
        char szValueName[32];
        char szData[40];
        DWORD cbValueName = 32;
        DWORD cbData = 40;
        DWORD dwDataType;
        
        lErrorCode = RegEnumValue(hkeyServices, nCounter, szValueName,
            &cbValueName, 0, &dwDataType, (LPBYTE) szData, &cbData);
        
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
// _UnloadShellServiceObjects
//
// Shut down the COM based shell services.
//
//
void TrayService::_UnloadShellServiceObjects()
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
		pTrayService = (TrayService*)GetWindowLong(hWnd, 0);
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
                    case 0:
                    {
                        //
                        // Application Bar Message
                        //
                        SHELLAPPBARDATA* pData = (SHELLAPPBARDATA*)pcds->lpData;
                        
                        return pTrayService->HandleAppBarMessage(pData);
                    }
                    break;
                    
                    case 1:
                    {
                        //
                        // System Tray Notification
                        //
                        SHELLTRAYDATA* pstd = (SHELLTRAYDATA*)pcds->lpData;
                        
                        return pTrayService->HandleNotification(pstd);
                    }
                    break;

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
BOOL TrayService::HandleAppBarMessage(SHELLAPPBARDATA* pData)
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
BOOL TrayService::HandleNotification(SHELLTRAYDATA* pstd)
{
    bool bReturn = false;

    IconVector::iterator itIcon = _FindIcon(pstd->nid);

    switch (pstd->dwMessage)
    {
        case NIM_ADD:
        {
            if (itIcon == m_siVector.end())
            {
                bReturn = _AddIcon(pstd->nid);
            }
        }
        break;

        case NIM_MODIFY:
        {
            //
            // Some apps (old versions of Daemon Tools) use NIM_MODIFY to
            // reregister the icon in response to the TaskbarCreated message.
            // But treating NIM_MODIFY as NIM_ADD creates problems esp. with the
            // DUN and volume icons.
            //
            if (itIcon != m_siVector.end())
            {
                bReturn = _ModifyIcon(*itIcon, pstd->nid);
            }
        }
        break;
        
        case NIM_DELETE:
        {
            if (itIcon != m_siVector.end())
            {
                bReturn = _RemoveIcon(itIcon);
            }
        }
        break;

        default:
        {
            TRACE("NIM unknown: %u", pstd->dwMessage);
        }
        case NIM_SETFOCUS:
        case NIM_SETVERSION:
        {
            if (itIcon != m_siVector.end())
            {
                _Notify(pstd->dwMessage, *itIcon);
            }
        }
        break;
    }

    return bReturn ? TRUE : FALSE;
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
    for (IconVector::iterator iter = m_siVector.begin();
         iter != m_siVector.end(); ++iter)
    {
        if (_IsValidIcon(*iter))
        {
            _Notify(NIM_ADD, *iter);
        }
    }
    
    return m_hNotifyWnd;
}


//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
//
// _AddIcon
//
bool TrayService::_AddIcon(const NOTIFYICONDATA& nid)
{
    bool bReturn = false;

    //
    // Ignore adds if NIS_SHAREDICON is set and hIcon is NULL. This fixes the
    // "ghost" DUN icon on XP. It sends three NIM_ADDs, one of them with
    // NIS_HIDDEN and another one with hIcon == NULL. The former doesn't pass
    // the _IsValidIcon test, the latter is filtered out here. The third one is
    // displayed.
    //
    if (IsWindow(nid.hWnd) && !(_IsShared(nid) && (nid.hIcon == NULL)))
    {
        //
        // Create a new item
        //
        LSNOTIFYICONDATA* plnid = new LSNOTIFYICONDATA;

        if (plnid)
        {
            //
            // Return true in most cases; some apps crash if false is returned.
            //
            bReturn = true;

            ZeroMemory(plnid, sizeof(LSNOTIFYICONDATA));
            plnid->cbSize = sizeof(LSNOTIFYICONDATA);
            plnid->uFlags = nid.uFlags;
            plnid->uID = nid.uID;
            plnid->hWnd  = nid.hWnd;
            plnid->uCallbackMessage  = nid.uCallbackMessage;

            _CopyIconHandle(*plnid, nid);
            _CopyVersionSpecifics(*plnid, nid);

            //
            // This needs to be stored even if the icon doesn't pass the
            // _IsValidIcon test. A subsequent NIM_MODIFY could make it valid.
            //
            m_siVector.push_back(plnid);

            if (_IsValidIcon(plnid))
            {
                _Notify(NIM_ADD, plnid);
            }
        }
    }
    
    return bReturn;
}


//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
//
// _ModifyIcon
//
bool TrayService::_ModifyIcon(LSNOTIFYICONDATA* plnid,
                              const NOTIFYICONDATA& nid)
{
    bool bReturn = true;

    if (plnid)
    {
        bool bWasValid = _IsValidIcon(plnid);

        plnid->uFlags |= nid.uFlags;

        if (nid.uFlags & NIF_MESSAGE)
        {
            plnid->uCallbackMessage = nid.uCallbackMessage;
        }
        
        bReturn = _CopyIconHandle(*plnid, nid);
        _CopyVersionSpecifics(*plnid, nid);

        if (_IsValidIcon(plnid))
        {
            _Notify(bWasValid ? NIM_MODIFY : NIM_ADD, plnid);
        }
        else if (bWasValid)
        {
            _Notify(NIM_DELETE, plnid);
        }
        else
        {
            bReturn = false;
        }
    }
    else
    {
        bReturn = false;
    }

    return bReturn;
}


//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
//
// _RemoveIcon
//
// Remove an icon from the icon list
//
bool TrayService::_RemoveIcon(IconVector::iterator itIcon)
{
    if (*itIcon)
    {
        _Notify(NIM_DELETE, *itIcon);
        
        if ((*itIcon)->hIcon)
        {
            DestroyIcon((*itIcon)->hIcon);
        }
        
        delete *itIcon;
    }
    
    m_siVector.erase(itIcon);

    return true;
}


//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
//
// _CopyIconHandle
//
bool TrayService::_CopyIconHandle(LSNOTIFYICONDATA& lnidTarget,
                                  const NOTIFYICONDATA& nidSource) const
{
    bool bReturn = true;
    
    if (nidSource.uFlags & NIF_ICON)
    {
        if (lnidTarget.hIcon)
        {
            DestroyIcon(lnidTarget.hIcon);
        }
        
        lnidTarget.hIcon = CopyIcon(nidSource.hIcon);
        
        if (!lnidTarget.hIcon)
        {
            lnidTarget.uFlags &= ~NIF_ICON;
            bReturn = false;
        }
    }
    
    return bReturn;
}


//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
//
// _Notify
//
// Notify all listeners of a systray event
//
void TrayService::_Notify(DWORD dwMessage, LSNOTIFYICONDATA* plnid)
{
    ASSERT_ISREADPTR(plnid);
    SendMessage(m_hLiteStep, LM_SYSTRAY, dwMessage, (LPARAM)plnid);
}



//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
//
// FindIconPredicate
//
// Predicate for std::find_if, used by _FindIcon
// Needs to be at global scope because of mingw issues
//
struct FindIconPredicate
{
    FindIconPredicate(HWND hWnd, UINT uID) : m_hWnd(hWnd), m_uID(uID) {}
    
    bool operator() (const LSNOTIFYICONDATA* plnid) const
    {
        return (plnid->hWnd == m_hWnd && plnid->uID == m_uID);
    }
    
private:
    HWND m_hWnd;
    UINT m_uID;
};


//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
//
// _FindIcon
//
// Looks up an icon in the SystrayIconList
//
IconVector::iterator TrayService::_FindIcon(HWND hWnd, UINT uId)
{
    return std::find_if(m_siVector.begin(), m_siVector.end(),
        FindIconPredicate(hWnd, uId));
}


//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
//
// _CopyVersionSpecifics
//
// Setup version specific attributes
//
void TrayService::_CopyVersionSpecifics(LSNOTIFYICONDATA& lnidTarget,
                                        const NOTIFYICONDATA& nidSource) const
{
    if (_IsUnicode(nidSource))
    {
        NID_6W* pnid = (NID_6W*)&nidSource;

        switch (nidSource.cbSize)
        {
            default:
            case NID_6W_SIZE:
            {
                lnidTarget.guidItem = pnid->guidItem;
            }
            case NID_5W_SIZE:
            {
                if (nidSource.uFlags & NIF_STATE)
                {
                    lnidTarget.dwState |= pnid->dwState;
                    lnidTarget.dwStateMask |= pnid->dwStateMask;
                }

                if (nidSource.uFlags & NIF_INFO)
                {
                    if (_StringCopy(lnidTarget.szInfo, TRAY_MAX_INFO_LENGTH,
                        pnid->szInfo))
                    {
                        _StringCopy(lnidTarget.szInfoTitle,
                            TRAY_MAX_INFOTITLE_LENGTH, pnid->szInfoTitle);
                    }
                    else
                    {
                        pnid->uFlags &= ~NIF_INFO;
                    }
                }
            }
            case NID_4W_SIZE:
            {
                if (nidSource.uFlags & NIF_TIP)
                {
                    if (!_StringCopy(lnidTarget.szTip, TRAY_MAX_TIP_LENGTH,
                        pnid->szTip))
                    {
                        lnidTarget.uFlags &= ~NIF_TIP;
                    }
                }
            }
        }
    }
    else
    {
        NID_6A* pnid = (NID_6A*)&nidSource;

        switch (nidSource.cbSize)
        {
            default:
            case NID_6A_SIZE:
            {
                lnidTarget.guidItem = pnid->guidItem;
            }
            case NID_5A_SIZE:
            {
                if (nidSource.uFlags & NIF_STATE)
                {
                    lnidTarget.dwState |= pnid->dwState;
                    lnidTarget.dwStateMask |= pnid->dwStateMask;
                }

                if (nidSource.uFlags & NIF_INFO)
                {
                    if (_StringCopy(lnidTarget.szInfo, TRAY_MAX_INFO_LENGTH,
                        pnid->szInfo))
                    {
                        _StringCopy(lnidTarget.szInfoTitle,
                            TRAY_MAX_INFOTITLE_LENGTH, pnid->szInfoTitle);
                    }
                    else
                    {
                        pnid->uFlags &= ~NIF_INFO;
                    }
                }
            }
            case NID_4A_SIZE:
            {
                if (nidSource.uFlags & NIF_TIP)
                {
                    if (!_StringCopy(lnidTarget.szTip, TRAY_MAX_TIP_LENGTH,
                        pnid->szTip))
                    {
                        lnidTarget.uFlags &= ~NIF_TIP;
                    }
                }
            }
        }
    }
}


//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
//
// _ConvertWideToAnsi
//
int TrayService::_ConvertWideToAnsi(char* pszOutput, size_t cchOutput,
                                    const wchar_t* pwzInput, size_t cchInputMax)
                                    const
{
    ASSERT(!IsBadStringPtrW(pwzInput, cchInputMax));
    ASSERT_ISWRITEDATA(pszOutput, cchOutput);
    
    size_t cchRealInputMax = min((wcslen(pwzInput) + 1), cchInputMax);
    
    int nReturn = WideCharToMultiByte(CP_ACP, 0, pwzInput, cchRealInputMax,
        pszOutput, cchOutput, NULL, NULL);
    
    ASSERT(!IsBadStringPtr(pszOutput, cchOutput + 1));
    return nReturn;
}


//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
//
// _StringCopy
//
bool TrayService::_StringCopy(LPSTR pszDest, size_t cchDest, LPCSTR pszSrc) const
{
    ASSERT_ISWRITEDATA(pszDest, cchDest);
    ASSERT_ISSTRING(pszSrc);

    return SUCCEEDED(StringCchCopyEx(pszDest, cchDest, pszSrc, NULL, NULL,
        STRSAFE_NULL_ON_FAILURE));
}

bool TrayService::_StringCopy(LPSTR pszDest, size_t cchDest, LPCWSTR pwzSrc) const
{
    ASSERT_ISWRITEDATA(pszDest, cchDest);
    ASSERT(!IsBadStringPtrW(pwzSrc, -1));

    bool bReturn = (_ConvertWideToAnsi(pszDest, cchDest, pwzSrc, cchDest) != 0);
    
    if (!bReturn)
    {
        *pszDest = '\0';
    }

    return bReturn;
}
