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
#include <algorithm>
#include "../utility/core.hpp"


const char szTrayClass[] = "Shell_TrayWnd";
const char szTrayTitle[] = "Litestep Tray Manager";


//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
//
// TrayService
//
TrayService::TrayService()
{
	m_hInstance = NULL;
	m_hTrayWnd = NULL;
	m_hLiteStep = NULL;

	m_bWin2000 = false;
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
// Returns:
//				E_FAIL			- Unknown Error
//				E_OUTOFMEMORY	- Couldn't allocate window or WNDCLASSEX
//
//				S_OK			- Success
//
HRESULT TrayService::Start()
{
    HRESULT hr = E_FAIL;
    
    m_hLiteStep = GetLitestepWnd();
    m_hInstance = GetModuleHandle(NULL);
    
    if (m_hLiteStep && m_hInstance)
    {
        OSVERSIONINFO OsVersionInfo;
        
        ZeroMemory(&OsVersionInfo, sizeof(OSVERSIONINFO));
        OsVersionInfo.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);

        GetVersionEx(&OsVersionInfo);
        
        if (OsVersionInfo.dwPlatformId == VER_PLATFORM_WIN32_NT &&
            OsVersionInfo.dwMajorVersion == 5)
        {
            m_bWin2000 = true;
        }

        //
        // Register tray notification window class
        //
        WNDCLASSEX wc;

        ZeroMemory(&wc, sizeof(WNDCLASSEX));
        wc.cbSize = sizeof(WNDCLASSEX);
		wc.cbWndExtra = 4;
		wc.lpfnWndProc = TrayService::WindowProc;
		wc.hInstance = m_hInstance;
		wc.lpszClassName = szTrayClass;
		wc.style = CS_DBLCLKS;

		if (RegisterClassEx(&wc))
		{
            // Window which receives the tray messages
			m_hTrayWnd = CreateWindowEx(
			                 WS_EX_TOPMOST | WS_EX_TOOLWINDOW,
			                 szTrayClass,
			                 szTrayTitle,
			                 WS_POPUP | WS_CLIPSIBLINGS | WS_CLIPCHILDREN,
			                 0, 0,
			                 0, 0,
			                 NULL,
			                 NULL,
			                 m_hInstance,
			                 NULL);
			if (m_hTrayWnd)
			{
				SetWindowLong(m_hTrayWnd, GWL_USERDATA, magicDWord);
				SetWindowLong(m_hTrayWnd, 0, (LONG)this);

                // tell apps to reregister their icons (see Note 6)
                PostMessage(HWND_BROADCAST,
                    RegisterWindowMessage("TaskbarCreated"), 0, 0);

                if (m_bWin2000)
				{
					_LoadShellServiceObjects();
				}
                
                hr = S_OK;
			}
			else
			{
				UnregisterClass(szTrayClass, m_hInstance);
				
                RESOURCE_MSGBOX(m_hInstance, IDS_LITESTEP_CREATEWINDOW_ERROR,
                    "Unable to create window.", szTrayTitle);

                hr = E_OUTOFMEMORY;
			}
		}
		else
		{
            RESOURCE_MSGBOX(m_hInstance, IDS_LITESTEP_REGISTERCLASS_ERROR,
                "Error registering window class.", szTrayClass);
            
            hr = E_OUTOFMEMORY;
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

    if (m_bWin2000)
    {
        _UnloadShellServiceObjects();
    }
    
    if (m_hTrayWnd)
	{
		DestroyWindow(m_hTrayWnd);
        m_hTrayWnd = NULL;

		UnregisterClass(szTrayClass, m_hInstance);
	}
    
    while (m_siVector.size())
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
// WindowProc
//
// Redirects all messages to HandleNotification or DefWindowProc
//
LRESULT CALLBACK TrayService::WindowProc(HWND hWnd, UINT uMsg,
                                               WPARAM wParam, LPARAM lParam)
{
    static TrayService* pTrayService = NULL;
    LRESULT lReturn = 0;
    
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
                        // Application Bar Message (not supported yet)
                        //
                    }
                    break;
                    
                    case 1:
                    {
                        //
                        // System Tray notification
                        //
                        SHELLTRAYDATA* pstd = (SHELLTRAYDATA*)pcds->lpData;
                        lReturn = pTrayService->HandleNotification(pstd);
                    }
                    break;
                }
            }
            break;
            
            default:
            {
                lReturn = DefWindowProc(hWnd, uMsg, wParam, lParam);
            }
            break;
        }
    }
    else
    {
        lReturn = DefWindowProc(hWnd, uMsg, wParam, lParam);
    }
    
    return lReturn;
}


//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
//
// HandleNotification
//
// Handler for all system tray notifications
//
bool TrayService::HandleNotification(SHELLTRAYDATA* pstd)
{
    bool bReturn = false;
    
    // New actions for W2K and after, we don't handle these right now
    // so we will just bail out if we receive them
    if ((pstd->dwMessage == NIM_SETFOCUS) ||
        (pstd->dwMessage == NIM_SETVERSION))
    {
        return bReturn;
    }
    
    bool bHidden = _IsHidden(&pstd->nid);

    switch (pstd->dwMessage)
    {
        case NIM_ADD:
        {
            // Don't add it if it's hidden.
            if (bHidden)
            {
                break;
            }
        }
        // No break here, see below.
        
        case NIM_MODIFY:
        {
            //
            // Some programs use Add when they mean Modify, so both messages are
            // handled here.
            // Some apps (Daemon Tools) also use NIM_MODIFY to reregister the
            // icon in response to the TaskbarCreated message.
            //
            IconVector::iterator itIcon = _FindIcon(&pstd->nid);
            
            if (itIcon == m_siVector.end())
            {
                bReturn = _AddIcon(&pstd->nid);
            }
            else if (bHidden)
            {
                bReturn = _RemoveIcon(itIcon);
            }
            else
            {
                bReturn = _ModifyIcon(*itIcon, &pstd->nid);
            }
        }
        break;
        
        case NIM_DELETE:
        {
            bReturn = _RemoveIcon(_FindIcon(&pstd->nid));
        }
        break;

        default:
        break;
    }

    return bReturn;
}


//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
//
// SendSystemTray
//
// Resend all icon data; systray modules will request this via LM_SYSTRAYREADY
// during their startup.
//
void TrayService::SendSystemTray()
{
    for (IconVector::iterator iter = m_siVector.begin();
         iter != m_siVector.end(); ++iter)
    {
        if (_IsValidIcon((*iter)->uFlags))
        {
            _Notify(NIM_ADD, *iter);
        }
    }
}


//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
//
// _AddIcon
//
bool TrayService::_AddIcon(const NOTIFYICONDATA* pnid)
{
    //
    // Return true in most cases; some apps crash if false is returned. In OOM
    // conditions or if the icon handle is invalid it is safe to return false.
    //
    bool bReturn = true;
    
    if (pnid && pnid->uFlags & NIF_MESSAGE)
    {
        //
        // Create a new item
        //
        LSNOTIFYICONDATA* plnid = new LSNOTIFYICONDATA;
        
        if (plnid)
        {
            ZeroMemory(plnid, sizeof(LSNOTIFYICONDATA));
            plnid->cbSize = sizeof(LSNOTIFYICONDATA);
            plnid->uFlags = pnid->uFlags;
            plnid->uID = pnid->uID;
            plnid->hWnd  = pnid->hWnd;
            plnid->uCallbackMessage  = pnid->uCallbackMessage;
            
            if (pnid->uFlags & NIF_ICON)
            {
                if (pnid->hIcon)
                {
                    plnid->hIcon = CopyIcon(pnid->hIcon);

                    if (!plnid->hIcon)
                    {
                        bReturn = false;
                    }
                }
                else
                {
                    plnid->uFlags &= ~NIF_ICON;
                }
            }
            
            _CopyVersionSpecifics(plnid, pnid);
            
            //
            // This needs to be stored even if there is no icon or no callback
            // message. A subsequent NIM_MODIFY could add the missing
            // information.
            //
            m_siVector.push_back(plnid);
            
            if (_IsValidIcon(plnid->uFlags))
            {
                _Notify(NIM_ADD, plnid);
            }
        }
        else
        {
            // OOM
            bReturn = false;
        }
    }

    return bReturn;
}


//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
//
// _ModifyIcon
//
bool TrayService::_ModifyIcon(LSNOTIFYICONDATA* plnid,
                              const NOTIFYICONDATA* pnid)
{
    bool bReturn = false;

    if (plnid && pnid)
    {
        plnid->uFlags |= pnid->uFlags;

        if (pnid->uFlags & NIF_MESSAGE)
        {
            plnid->uCallbackMessage = pnid->uCallbackMessage;
        }
        
        if (pnid->uFlags & NIF_ICON)
        {
            if (plnid->hIcon)
            {
                DestroyIcon(plnid->hIcon);
            }
            
            plnid->hIcon = CopyIcon(pnid->hIcon);
        }
        
        _CopyVersionSpecifics(plnid, pnid);

        if (_IsValidIcon(plnid->uFlags))
        {
            _Notify(NIM_MODIFY, plnid);
            bReturn = true;
        }
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
    bool bReturn = false;
    
    if (itIcon != m_siVector.end() && *itIcon)
    {
        _Notify(NIM_DELETE, *itIcon);
        
        if ((*itIcon)->hIcon)
        {
            DestroyIcon((*itIcon)->hIcon);
        }
        
        delete *itIcon;
        m_siVector.erase(itIcon);
        
        bReturn = true;
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
    SendMessage(m_hLiteStep, LM_SYSTRAY, dwMessage, (LPARAM)plnid);
}



//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
//
// _FindIconPredicate
//
// Predicate for std::find_if, used by _FindIcon
//
struct TrayService::_FindIconPredicate
{
    _FindIconPredicate(HWND hWnd, UINT uID) : m_hWnd(hWnd), m_uID(uID) {}
    
    bool operator() (const LSNOTIFYICONDATA*& plnid) const
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
IconVector::iterator TrayService::_FindIcon(const NOTIFYICONDATA* pnid)
{
    return std::find_if(m_siVector.begin(), m_siVector.end(),
        TrayService::_FindIconPredicate(pnid->hWnd, pnid->uID));
}


//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
//
// _IsHiddenWorker
//
// Does the actual work for _IsHidden
// MSVC has problems with member templates, thus this is at global scope
//
template <typename T>
inline bool _IsHiddenWorker(const NOTIFYICONDATA* pnidArgument)
{
    const T* pnid = (const T*)pnidArgument;
    
    return ((pnid->dwState & pnid->dwStateMask) & NIS_HIDDEN) ? true : false;
}


//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
//
// _IsHidden
//
// Determines if an icon is hidden
//
bool TrayService::_IsHidden(const NOTIFYICONDATA* pnid)
{
    bool bReturn = false;
    
    if (pnid->uFlags & NIF_STATE)
    {
        switch (pnid->cbSize)
        {
            case NID_5A_SIZE:
            case NID_6A_SIZE:
            {
                bReturn = _IsHiddenWorker<NID_5A>(pnid);
            }
            break;
            
            case NID_5W_SIZE:
            case NID_6W_SIZE:
            {
                bReturn = _IsHiddenWorker<NID_5W>(pnid);
            }
            break;
            
            default:
            break;
        }
    }
    
    return bReturn;
}


//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
//
// _CopyVersionSpecifics
//
// Setup version specific attributes
//
void TrayService::_CopyVersionSpecifics(LSNOTIFYICONDATA* plnidTarget,
                                        const NOTIFYICONDATA* pnidSource)
{
    if (plnidTarget && pnidSource)
    {
        NID_PTR pnid;
        pnid.wXP = (NID_6W*)pnidSource;

        switch (pnidSource->cbSize)
        {
            // 9x
            case NID_4A_SIZE:
            {
                if ((pnid.w9X->uFlags & NIF_TIP) && pnid.w9X->szTip)
                {
                    StringCchCopy(plnidTarget->szTip, 64, pnid.w9X->szTip);
                }
            }
            break;
            
            // NT 4.0
            case NID_4W_SIZE:
            {
                if ((pnid.NT4->uFlags & NIF_TIP) && pnid.NT4->szTip)
                {
                    UINT nChars = min(UINT(wcslen(pnid.NT4->szTip) + 1),
                        UINT(64));
                    
                    ::WideCharToMultiByte(CP_ACP, 0, pnid.NT4->szTip,
                        nChars, plnidTarget->szTip, nChars, NULL, NULL);
                    
                    plnidTarget->szTip[63] = '\0';
                }
            }
            break;
            
            // IE 5 and IE 6 (ME)
            case NID_5A_SIZE:
            case NID_6A_SIZE:
            {
                if (pnidSource->uFlags & NIF_STATE)
                {
                    plnidTarget->dwState |= pnid.IE5->dwState;
                    plnidTarget->dwStateMask |= pnid.IE5->dwStateMask;
                }

                if ((pnid.IE5->uFlags & NIF_INFO) && pnid.IE5->szInfo)
                {
                    // Emulate NIF_INFO with NIF_TIP
                    if (SUCCEEDED(StringCchCopy(
                        plnidTarget->szTip, 128, pnid.IE5->szInfo)))
                    {
                        plnidTarget->uFlags |= NIF_TIP;
                    }
                }
                else if ((pnid.IE5->uFlags & NIF_TIP) && pnid.IE5->szTip)
                {
                    StringCchCopy(plnidTarget->szTip, 128, pnid.IE5->szTip);
                }
                
            }
            break;
            
            // 2K and XP (IE 6)
            default:
            {
                if (pnidSource->uFlags & NIF_STATE)
                {
                    plnidTarget->dwState |= pnid.w2K->dwState;
                    plnidTarget->dwStateMask |= pnid.w2K->dwStateMask;
                }
                
                if ((pnid.w2K->uFlags & NIF_INFO) && pnid.w2K->szInfo)
                {
                    UINT nChars = min(UINT(wcslen(pnid.w2K->szInfo) + 1),
                        UINT(256));
                    
                    ::WideCharToMultiByte(CP_ACP, 0, pnid.w2K->szInfo,
                        nChars, plnidTarget->szTip, nChars, NULL, NULL);
                    
                    plnidTarget->szTip[255] = '\0';
                    
                    // Add the NIF_TIP flag so modules will know to use this
                    plnidTarget->uFlags |= NIF_TIP;
                }
                else if ((pnid.w2K->uFlags & NIF_TIP) && pnid.w2K->szTip)
                {
                    UINT nChars = min(UINT(wcslen(pnid.w2K->szTip) + 1),
                        UINT(128));
                    
                    ::WideCharToMultiByte(CP_ACP, 0, pnid.w2K->szTip,
                        nChars, plnidTarget->szTip, nChars, NULL, NULL);

                    plnidTarget->szTip[127] = '\0';
                }
                break;
            }
        }
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
	LONG lErrorCode;
    int nCounter = 0;
    
	lErrorCode = RegOpenKeyEx(HKEY_LOCAL_MACHINE,
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
			WCHAR wszCLSID[40];
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
    while (m_ssoList.size())
	{
        m_ssoList.back()->Exec(&CGID_ShellServiceObject, OLECMDID_SAVE,
            OLECMDEXECOPT_DODEFAULT, NULL, NULL);

		m_ssoList.back()->Release();
        m_ssoList.pop_back();
	}
}
