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
#include "../utility/common.h"
#include "../utility/debug.hpp"
#include "../utility/shellhlp.h"
#include "TaskbarListHandler.h"


//
// TaskbarListHandler
//
TaskbarListHandler::TaskbarListHandler() :
        m_hWndTaskband(nullptr),
        m_aWndClass(0),
        m_hLiteStep(nullptr),
        m_hInstance(nullptr),
        WM_ShellHook(0),
        m_dwLiteStepProc(0)
{
}


//
// Start
//
HRESULT TaskbarListHandler::Start(HWND hWndTray) {
    HRESULT hr = E_FAIL;
    
    m_hLiteStep = GetLitestepWnd();
    GetWindowThreadProcessId(m_hLiteStep, &m_dwLiteStepProc);
    m_hInstance = GetModuleHandle(NULL);

    WNDCLASSEX wndClass;
    ZeroMemory(&wndClass, sizeof(WNDCLASSEX));
    wndClass.cbSize = sizeof(WNDCLASSEX);
    wndClass.cbWndExtra = sizeof(TaskbarListHandler*);
    wndClass.lpszClassName = _T("taskbandHWNDClass");
    wndClass.lpfnWndProc = &TaskbarListHandler::WindowProcedureInit;
    wndClass.hInstance = m_hInstance;
    wndClass.style = CS_NOCLOSE;

    m_aWndClass = RegisterClassEx(&wndClass);
    if (m_aWndClass == 0)
    {
        hr = HrGetLastError();
    }
    else
    {
        m_hWndTaskband = CreateWindowEx(WS_EX_TOOLWINDOW, (LPCTSTR)m_aWndClass, _T("taskbandHWND"), WS_CHILD, 0, 0, 100, 100, hWndTray, nullptr, m_hInstance, this);
        if (m_hWndTaskband == nullptr)
        {
            hr = HrGetLastError();
        }
        else
        {
            WM_ShellHook = RegisterWindowMessage(_T("SHELLHOOK"));
            SetProp(hWndTray, _T("TaskbandHWND"), m_hWndTaskband);
            hr = S_OK;
        }
    }

    return hr;
}


//
// Stop
//
HRESULT TaskbarListHandler::Stop() {
    if (m_hWndTaskband != nullptr) {
        DestroyWindow(m_hWndTaskband);
    }
    
    if (m_aWndClass != 0) {
        UnregisterClass((LPCTSTR)m_aWndClass, m_hInstance);
    }

    return S_OK;
}


//
// HandleMessage
//
LRESULT WINAPI TaskbarListHandler::HandleMessage(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    DbgTraceWindowMessage("ITaskbarList", uMsg, wParam, lParam);
    switch (uMsg)
    {
        // Sent by ITaskbarList::ActivateTab and ITaskbarList::SetActiveAlt
        // ActivateTab will send WM_ShellHook(4, hwnd) immediately after this message.
    case WM_USER + 50:
        {
            // wParam = 0, lParam = hWnd
            SendMessage(m_hLiteStep, LM_TASK_MARKASACTIVE, lParam, 0);
        }
        return 0;

        // Sent by ITaskbarList2::MarkFullscreenWindow
    case WM_USER + 60:
        {
            // wParam = 1 to mark as fullscreen, 0 to remove marking
            // lParam = hWnd

            // TODO::This should be sent to the fullscreen monitor
        }
        return 0;

        // Sent by ITaskbarList3::SetProgressValue
    case WM_USER + 64:
        {
            // wParam = hwnd
            // lParam = progress, 0 = 0%, 0xFFFE = 100%
            SendMessage(m_hLiteStep, LM_TASK_SETPROGRESSVALUE, wParam, lParam);
        }
        return 0;

        // Sent by ITaskbarList3::SetProgressState
    case WM_USER + 65:
        {
            // wParam = hwnd
            // lParam = tbpFlags
            SendMessage(m_hLiteStep, LM_TASK_SETPROGRESSSTATE, wParam, lParam);
        }
        return 0;

        // Sent by ITaskbarList3::RegisterTab
    case WM_USER + 67:
        {
            // wParam = hwndTab
            // lParam = hwndMDI
            SendMessage(m_hLiteStep, LM_TASK_REGISTERTAB, wParam, lParam);
        }
        return 0;
        
        // Sent by ITaskbarList3::UnregisterTab
    case WM_USER + 68:
        {
            // wParam = hwndTab
            // lParam = 0
            SendMessage(m_hLiteStep, LM_TASK_UNREGISTERTAB, wParam, 0);
        }
        return 0;

        // Sent by ITaskbarList3::SetTabOrder
    case WM_USER + 71:
        {
            // wParam = hwndTab
            // lParam = hwndInsertBefore
            SendMessage(m_hLiteStep, LM_TASK_SETTABORDER, wParam, lParam);
        }
        return 0;

        // Sent by ITaskbarList3::SetTabActive
    case WM_USER + 72:
        {
            // wParam = hwndTab
            // lParam = 0 ???
            SendMessage(m_hLiteStep, LM_TASK_SETACTIVETAB, wParam, lParam);
        }
        return 0;

        // Mysterious message, sent by control panel windows
    case WM_USER + 75:
        {
            // wParam = hwnd of the control panel
            // lParam = 0 ?

        }
        return 0;

        // Sent by ITaskbarList3::ThumbBarAddButtons
    case WM_USER + 76:
        {
            // wParam = hwnd
            // lParam = SHSharedAlloc(cButtons, pButton) (first 4 bytes is cButtons)
            LPUINT pData = (LPUINT)SHLockShared((HANDLE)lParam, m_dwLiteStepProc);
            THUMBBUTTONLIST thumbList;
            thumbList.cButtons = pData[0];
            thumbList.pButton = (THUMBBUTTON*)&pData[1];
            SendMessage(m_hLiteStep, LM_TASK_THUMBBARADDBUTTONS, wParam, (LPARAM)&thumbList);
            SHUnlockShared(pData);
        }
        return 0;
        
        // Sent by ITaskbarList3::ThumbBarUpdateButtons
    case WM_USER + 77:
        {
            // wParam = hwnd
            // lParam = SHSharedAlloc(cButtons, pButton) (first 4 bytes is cButtons)
            LPUINT pData = (LPUINT)SHLockShared((HANDLE)lParam, m_dwLiteStepProc);
            THUMBBUTTONLIST thumbList;
            thumbList.cButtons = pData[0];
            thumbList.pButton = (THUMBBUTTON*)&pData[1];
            SendMessage(m_hLiteStep, LM_TASK_THUMBBARUPDATEBUTTONS, wParam, (LPARAM)&thumbList);
            SHUnlockShared(pData);
        }
        return 0;

        // Sent by ITaskbarList3::ThumbBarSetImageList
    case WM_USER + 78:
        {
            // wParam = hwnd
            // lParam = SHSharedAlloc(himl)
            HIMAGELIST hImageList = (HIMAGELIST)SHLockShared((HANDLE)lParam, m_dwLiteStepProc);
            SendMessage(m_hLiteStep, LM_TASK_THUMBBARSETIMAGELIST, wParam, (LPARAM)hImageList);
            if (hImageList) {
                SHUnlockShared(hImageList);
            }
        }
        return 0;

        // Sent by ITaskbarList3::SetOverlayIcon
    case WM_USER + 79:
        {
            // wParam = hwnd
            // lParam = hicon
            SendMessage(m_hLiteStep, LM_TASK_SETOVERLAYICON, wParam, lParam);
        }
        return 0;

        // Sent by ITaskbarList3::SetThumbnailTooltip
    case WM_USER + 80:
        {
            // wParam = hwnd
            // lParam = SHSharedAlloc(pszTip)
            LPWSTR pwzTip = (LPWSTR)SHLockShared((HANDLE)lParam, m_dwLiteStepProc);
            SendMessage(m_hLiteStep, LM_TASK_SETTHUMBNAILTOOLTIP, wParam, (LPARAM)pwzTip);
            if (pwzTip) {
                SHUnlockShared(pwzTip);
            }
        }
        return 0;

        // Sent by ITaskbarList3::SetThumbnailClip
    case WM_USER + 81:
        {
            // wParam = hwnd
            // lParam = SHSharedAlloc(prcClip)
            LPRECT prcClip = (LPRECT)SHLockShared((HANDLE)lParam, m_dwLiteStepProc);
            SendMessage(m_hLiteStep, LM_TASK_SETTHUMBNAILCLIP, wParam, (LPARAM)prcClip);
            if (prcClip) {
                SHUnlockShared(prcClip);
            }
        }
        return 0;

        // Sent by ITaskbarList3::SetOverlayIcon
    case WM_USER + 85:
        {
            // wParam = hwnd
            // lParam = SHSharedAlloc(pszDescription)
            LPWSTR pwzDescription = (LPWSTR)SHLockShared((HANDLE)lParam, m_dwLiteStepProc);
            SendMessage(m_hLiteStep, LM_TASK_SETOVERLAYICONDESC, wParam, (LPARAM)pwzDescription);
            if (pwzDescription) {
                SHUnlockShared(pwzDescription);
            }
        }
        return 0;

        // Sent by ITaskbarList4::SetTabProperties
    case WM_USER + 87:
        {
            // wParam = hwndTab
            // lParam = stpFlags
            SendMessage(m_hLiteStep, LM_TASK_SETTABPROPERTIES, wParam, lParam);
        }
        return 0;

    default:
        {
            // AddTab, DeleteTab, and ActivateTab will send WM_ShellHook
            // TODO::We should consider distinguishing between these and actual WM_SHELLHOOK messages
            // we may wish to remember that a certain window has been "marked" as deleted, so that we
            // don't accidentally recreate it (when activated, or on recycles).
            if (uMsg == WM_ShellHook)
            {
                return SendMessage(m_hLiteStep, uMsg, wParam, lParam);
            }
        }
    }

    return DefWindowProc(hWnd, uMsg, wParam, lParam);
}


//
// WindowProcedure
//
LRESULT WINAPI TaskbarListHandler::WindowProcedure(HWND window, UINT msg, WPARAM wParam, LPARAM lParam) {
    return ((TaskbarListHandler *)GetWindowLongPtr(window, 0))->HandleMessage(window, msg, wParam, lParam);
}


//
// WindowProcedureInit
//
LRESULT WINAPI TaskbarListHandler::WindowProcedureInit(HWND window, UINT msg, WPARAM wParam, LPARAM lParam) {
    if (msg == WM_CREATE) {
        SetWindowLongPtr(window, 0, (LONG_PTR)((LPCREATESTRUCT)lParam)->lpCreateParams);
        SetWindowLongPtr(window, GWLP_WNDPROC, (LONG_PTR)&TaskbarListHandler::WindowProcedure);
        return WindowProcedure(window, msg, wParam, lParam);
    }
    return DefWindowProc(window, msg, wParam, lParam);
}
