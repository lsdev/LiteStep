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
#include "RecoveryMenu.h"
#include "resource.h"
#include "../utility/core.hpp"


//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//
// Globals
//
#define RECOVERY_WAIT_TIMEOUT 3000

#define ID_HOTKEY 1

#define ID_RECYCLE 1
#define ID_QUIT 2
#define ID_TERMINATE 3
#define ID_RUN 4
#define ID_SHUTDOWN 5

const TCHAR szRecoveryMenuWndClass[] = _T("RecoveryMenuWndClass");


//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//
// The Menu
//
struct MenuCommands
{
    int nStringID;
    int nCommandID;
    LPCSTR pszDefText;
}
rgMenuCommands[] = \
{
     { IDS_LITESTEP_RECYCLELS,   ID_RECYCLE,   "Re&cycle LiteStep"            }
    ,{ IDS_LITESTEP_QUITLS,      ID_QUIT,      "&Quit LiteStep"               }
    ,{ IDS_LITESTEP_TERMINATELS, ID_TERMINATE, "Forcibly &Terminate LiteStep" }
    ,{ 0,                        -1,           ""                             }
    ,{ IDS_LITESTEP_RUN,         ID_RUN,       "&Run..."                      }
    ,{ 0,                        -1,           ""                             }
    ,{ IDS_LITESTEP_SHUTDOWNWIN, ID_SHUTDOWN,  "Sh&utdown Windows..."         }
};


//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//
// constructor
//
RecoveryMenu::RecoveryMenu(HINSTANCE hInstance)
: m_hInstance(hInstance)
{
    // do nothing
}


//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//
// Start
//
HRESULT RecoveryMenu::Start()
{
    HRESULT hr = E_FAIL;
    
    m_hThread = LSCreateThread("RecoveryThread",
        ThreadThunk, (LPVOID)this, &m_dwThreadId);
    
    if (m_hThread != NULL)
    {
        hr = S_OK;
    }
    else
    {
        hr = HrGetLastError();
    }
    
    return hr;
}


//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//
// Stop
//
HRESULT RecoveryMenu::Stop()
{
    HRESULT hr = E_FAIL;
    
    if (m_hThread)
    {
        PostThreadMessage(m_dwThreadId, WM_QUIT, 0, 0);
        
        // Wait until the thread is done
        DWORD dwWait = WaitForSingleObject(m_hThread, RECOVERY_WAIT_TIMEOUT);
        
        if (dwWait == WAIT_OBJECT_0)
        {
            hr = S_OK;
        }
        else if (dwWait == WAIT_TIMEOUT)
        {
            TerminateThread(m_hThread, 0);
        }
        else if (dwWait == WAIT_FAILED)
        {
            hr = HrGetLastError();
        }
        
        VERIFY(CloseHandle(m_hThread));
        m_hThread = NULL;
        m_dwThreadId = 0;
    }
    else
    {
        // Nothing to shut down
        hr = S_FALSE;
    }
    
    return hr;
}


//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//
// ThreadThunk
//
DWORD WINAPI RecoveryMenu::ThreadThunk(LPVOID pParam)
{
    RecoveryMenu* self = (RecoveryMenu*)pParam;
    return self->ThreadProc();
}


//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//
// ThreadProc
//
DWORD RecoveryMenu::ThreadProc()
{
    WNDCLASSEX wc = { 0 };
    wc.cbSize = sizeof(WNDCLASSEX);
    wc.lpfnWndProc = WindowThunk;
    wc.hInstance = m_hInstance;
    wc.lpszClassName = szRecoveryMenuWndClass;
    
    if (RegisterClassEx(&wc))
    {
        HWND hRecoveryWnd = \
            CreateWindowEx(0,
                szRecoveryMenuWndClass,
                NULL,
                WS_POPUP,
                0, 0, 0, 0,
                NULL,
                NULL,
                m_hInstance,
                (LPVOID)this);
        
        if (IsWindow(hRecoveryWnd))
        {
            MSG msg;
            
            while (GetMessage(&msg, NULL, 0, 0) > 0)
            {
                TranslateMessage(&msg);
                DispatchMessage(&msg);
            }
            
            DestroyWindow(hRecoveryWnd);
        }
        
        UnregisterClass(szRecoveryMenuWndClass, m_hInstance);
    }
    
    return 0;
}


//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//
// WindowThunk
//
LRESULT WINAPI RecoveryMenu::WindowThunk(HWND hWnd, UINT uMsg,
                                         WPARAM wParam, LPARAM lParam)
{
    LRESULT lResult = 0;
    
    if (uMsg == WM_NCCREATE)
    {
        LPCREATESTRUCT pData = (LPCREATESTRUCT)lParam;
        SetWindowLongPtr(hWnd, GWLP_USERDATA, (LONG_PTR)pData->lpCreateParams);
    }
    
    RecoveryMenu* self = (RecoveryMenu*)GetWindowLongPtr(hWnd, GWLP_USERDATA);
    
    if (self)
    {
        lResult = self->WindowProc(hWnd, uMsg, wParam, lParam);
    }
    else
    {
        lResult = DefWindowProc(hWnd, uMsg, wParam, lParam);
    }
    
    return lResult;
}


//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//
// WindowProc
//
LRESULT RecoveryMenu::WindowProc(HWND hWnd, UINT uMsg,
                                 WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
    case WM_HOTKEY:
        {
            if (wParam == ID_HOTKEY)
            {
                int nCommand = ShowMenu(hWnd);
                HandleMenuCommand(nCommand);
                return 0;
            }
        }
        break;
        
    case WM_CREATE:
        {
            RegisterHotKey(hWnd, ID_HOTKEY, MOD_CONTROL | MOD_ALT, VK_F1);
            return 0;
        }
        break;
        
    case WM_DESTROY:
        {
            UnregisterHotKey(hWnd, ID_HOTKEY);
            return 0;
        }
        break;
        
    case WM_CLOSE:
        {
            return 0;
        }
        break;
    
    default:
        {
            // do nothing
        }
        break;
    }
    
    return DefWindowProc(hWnd, uMsg, wParam, lParam);
}


//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//
// ShowMenu
//
int RecoveryMenu::ShowMenu(HWND hWnd) const
{
    HMENU hMenu = CreatePopupMenu();
    
    // populate the menu
    for (size_t i = 0; i < COUNTOF(rgMenuCommands); ++i)
    {
        if (rgMenuCommands[i].nStringID)
        {
            char szBuffer[MAX_PATH] = { 0 };
            GetResStr(m_hInstance,
                rgMenuCommands[i].nStringID, szBuffer, MAX_PATH,
                rgMenuCommands[i].pszDefText);
            
            AppendMenu(hMenu, MF_STRING, rgMenuCommands[i].nCommandID,
                szBuffer);
        }
        else
        {
            AppendMenu(hMenu, MF_SEPARATOR, 0, NULL);
        }
    }
    
    // get the current position of the mouse
    POINT pt;
    GetCursorPos(&pt);
    
    SetForegroundWindow(hWnd);
    
    int nCommand = (int)TrackPopupMenu(
        hMenu,
        TPM_LEFTBUTTON | TPM_RETURNCMD | TPM_NONOTIFY,
        pt.x, pt.y,
        0, hWnd, NULL);
    
    DestroyMenu(hMenu);
    
    return nCommand;
}


//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//
// HandleMenuCommand
//
void RecoveryMenu::HandleMenuCommand(int nCommand) const
{
    switch (nCommand)
    {
    case ID_RECYCLE:
        {
            // using PostMessage so the user can still use this menu
            // to kill LS should the message queue be blocked
            PostMessage(GetLitestepWnd(), LM_RECYCLE, LR_RECYCLE, 0);
        }
        break;
        
    case ID_QUIT:
        {
            // ditto
            PostMessage(GetLitestepWnd(), LM_RECYCLE, LR_QUIT, 0);
        }
        break;
        
    case ID_TERMINATE:
        {
            TerminateProcess(GetCurrentProcess(), 1);
        }
        break;
        
    case ID_RUN:
        {
            typedef void (WINAPI* RUNDLGPROC)(
                HWND, HICON, LPCSTR, LPCSTR, LPCSTR, UINT);
            
            RUNDLGPROC fnRunDlg = (RUNDLGPROC)GetProcAddress(
                GetModuleHandle(_T("SHELL32.DLL")), (LPCSTR)((long)0x003D));
            
            fnRunDlg(NULL, NULL, NULL, NULL, NULL, 0);
        }
        break;
        
    case ID_SHUTDOWN:
        {
            LSShutdownDialog(GetLitestepWnd());
        }
        break;
        
    default:
        {
            // do nothing
        }
        break;
    }
}
