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
#include "FullscreenMonitor.h"
#include "../lsapi/lsapi.h"
#include "../utility/debug.hpp"


//
// FullscreenMonitor
//
FullscreenMonitor::FullscreenMonitor() :
    m_workerSleepDuration(100)
{
    m_bReHide = false;
}


//
// ~FullscreenMonitor
//
FullscreenMonitor::~FullscreenMonitor()
{
}


//
// _FullScreenGetMonitorHelper
//
HMONITOR FullscreenMonitor::_FullScreenGetMonitorHelper(HWND hWnd)
{
    if (!IsWindow(hWnd) || !IsWindowVisible(hWnd) || IsIconic(hWnd))
    {
        return nullptr;
    }
    
    HMONITOR hMonFS = MonitorFromWindow(hWnd, MONITOR_DEFAULTTONULL);
    
    if (nullptr == hMonFS)
    {
        return nullptr;
    }
    
    RECT rScreen = { 0 };
    
    if (nullptr != hMonFS)
    {
        MONITORINFO miFS;
        miFS.cbSize = sizeof(MONITORINFO);
        
        if (GetMonitorInfo(hMonFS, &miFS))
        {
            VERIFY(CopyRect(&rScreen, &miFS.rcMonitor));
        }
        else
        {
            rScreen.left = 0;
            rScreen.top = 0;
            rScreen.right = GetSystemMetrics(SM_CXSCREEN);
            rScreen.bottom = GetSystemMetrics(SM_CYSCREEN);
            
            hMonFS = MonitorFromRect(&rScreen, MONITOR_DEFAULTTOPRIMARY);
        }
    }
    
    RECT rWnd;
    VERIFY(GetClientRect(hWnd, &rWnd));
    
    LONG width = rWnd.right - rWnd.left;
    LONG height = rWnd.bottom - rWnd.top;
    
    POINT pt = { rWnd.left, rWnd.top };
    VERIFY(ClientToScreen(hWnd, &pt));
    
    rWnd.left = pt.x;
    rWnd.top = pt.y;
    rWnd.right = pt.x + width;
    rWnd.bottom = pt.y + height;
    
    // If the client area is the size of the screen, then consider it to be
    // a full screen window.
    if (EqualRect(&rScreen, &rWnd))
    {
        return hMonFS;
    }
    
    DWORD dwStyle = GetWindowLong(hWnd, GWL_STYLE);

    // Check Window Rect if WS_CAPTION or WS_THICKFRAME is part of the style.
    // As long as at least one of them is not set, then we can check if the
    // window is full screen or not: http://support.microsoft.com/kb/q179363/
    if (WS_CAPTION != (WS_CAPTION & dwStyle) ||
        WS_THICKFRAME != (WS_THICKFRAME & dwStyle))
    {
        VERIFY(GetWindowRect(hWnd, &rWnd));
        
        if (EqualRect(&rScreen, &rWnd))
        {
            return hMonFS;
        }
    }
    
    return nullptr;
}


//
// _EnumThreadFSWnd
//
BOOL CALLBACK FullscreenMonitor::_EnumThreadFSWnd(HWND hWnd, LPARAM lParam)
{
    HMONITOR hMonFS = _FullScreenGetMonitorHelper(hWnd);
    
    if (nullptr != hMonFS)
    {
        *(HMONITOR*)lParam = hMonFS;
        return FALSE;
    }
    
    return TRUE;
}


//
// IsFullscreenWindow
// 
HMONITOR FullscreenMonitor::IsFullscreenWindow(HWND hWnd)
{
    if (!IsWindow(hWnd))
    {
        return nullptr;
    }
    
    HMONITOR hMonFS = nullptr;
    EnumThreadWindows(GetWindowThreadProcessId(hWnd, nullptr), _EnumThreadFSWnd, (LPARAM)&hMonFS);
    
    return hMonFS;
}


//
// ShowModules
//
void FullscreenMonitor::ShowModules(HMONITOR hMonitor)
{
    if (m_bAutoHideModules)
    {
        ParseBangCommandW((HWND)hMonitor, L"!ShowModules", nullptr);
    }
    PostMessage(GetLitestepWnd(), LM_FULLSCREENDEACTIVATED, (WPARAM) hMonitor, 0);
}


//
// HideModules
//
void FullscreenMonitor::HideModules(HMONITOR hMonitor, HWND hWnd)
{
    if (m_bAutoHideModules)
    {
        ParseBangCommandW((HWND)hMonitor, L"!HideModules", nullptr);
    }
    PostMessage(GetLitestepWnd(), LM_FULLSCREENACTIVATED, (WPARAM) hMonitor, (LPARAM) hWnd);
}


//
// ThreadProc
//
void FullscreenMonitor::ThreadProc()
{
    //
    struct FSWindow {
        FSWindow(HWND hWnd, HMONITOR hMonitor) {
            this->hWnd = hWnd;
            this->hMonitor = hMonitor;
        }
        HWND hWnd;
        HMONITOR hMonitor;
    };

    // List of all currently living known fullscreen windows
    std::list<FSWindow> fullscreenWindows;

    // The monitors which currently have modules hidden
    std::unordered_multiset<HMONITOR> hiddenMonitors;

    DWORD dwLiteStepProcID;
    GetWindowThreadProcessId(GetLitestepWnd(), &dwLiteStepProcID);

    // On startup, find any existing fullscreen windows.
    EnumDesktopWindows(nullptr, [] (HWND hWnd, LPARAM lParam) -> BOOL
    {
        HMONITOR hMonitor = IsFullscreenWindow(hWnd);
        std::list<FSWindow> *fullscreenWindows = (std::list<FSWindow>*)lParam;
        if (hMonitor)
        {
            // And add it to the list of fullscreen windows.
            fullscreenWindows->push_back(FSWindow(hWnd, hMonitor));
        }
        return TRUE;
    }, (LPARAM)&fullscreenWindows);

    // Hide modules on any monitor we found
    for (FSWindow window : fullscreenWindows)
    {
        hiddenMonitors.insert(window.hMonitor);
        if (hiddenMonitors.count(window.hMonitor) == 1)
        {
            HideModules(window.hMonitor, window.hWnd);
        }
    }

    // Main Loop
    while (m_bRun.load())
    {
        HWND hwndForeGround = GetForegroundWindow();
        bool bCheckedForeGround = false;

        // Re-Hide modules on all monitors
        if (m_bReHide.load())
        {
            hiddenMonitors.clear();
            for (FSWindow window : fullscreenWindows)
            {
                hiddenMonitors.insert(window.hMonitor);
                if (hiddenMonitors.count(window.hMonitor) == 1)
                {
                    HideModules(window.hMonitor, window.hWnd);
                }
            }
        }

        // Verify that all currently known fullscreen windows are still fullscreen.
        std::list<FSWindow>::iterator iter = fullscreenWindows.begin();
        while (iter != fullscreenWindows.end())
        {
            HMONITOR hNewMonitor = IsFullscreenWindow(iter->hWnd);

            // If we already checked the foreground window, don't bother doing it again.
            if (iter->hWnd == hwndForeGround)
            {
                bCheckedForeGround = true;
            }

            // 
            if (hNewMonitor != iter->hMonitor)
            {
                hiddenMonitors.erase(iter->hMonitor);

                // If there are no fullscreen windows left on the monitor, show the modules
                if (hiddenMonitors.count(iter->hMonitor) == 0)
                {
                    ShowModules(iter->hMonitor);
                }

                // Check if the window moved to another monitor (and stayed fullscreen)
                if (hNewMonitor)
                {
                    iter->hMonitor = hNewMonitor;
                    hiddenMonitors.insert(hNewMonitor);
                    if (hiddenMonitors.count(hNewMonitor) == 1)
                    {
                        HideModules(hNewMonitor, iter->hWnd);
                    }
                }
                else
                {
                    iter = fullscreenWindows.erase(iter);
                    continue;
                }
            }

            ++iter;
        }

        // Check if the currently active window is a fullscreen window.
        if (!bCheckedForeGround)
        {
            // If the currently active window belongs to litestep, show all modules.
            DWORD dwProcID;
            GetWindowThreadProcessId(hwndForeGround, &dwProcID);
            if (dwProcID == dwLiteStepProcID)
            {
                if (!hiddenMonitors.empty())
                {
                    for (HMONITOR hMonitor : hiddenMonitors)
                    {
                        ShowModules(hMonitor);
                    }
                    hiddenMonitors.clear();
                    fullscreenWindows.clear();
                }
            }
            else
            {
                HMONITOR hMonitor = IsFullscreenWindow(hwndForeGround);
                if (hMonitor)
                {
                    // Add the window to the list of known foreground windows.
                    fullscreenWindows.push_back(FSWindow(hwndForeGround, hMonitor));
                    hiddenMonitors.insert(hMonitor);

                    // If we didn't know about any fullscreen windows on this monitor before, hide the modules on it.
                    if (hiddenMonitors.count(hMonitor) == 1)
                    {
                        HideModules(hMonitor, hwndForeGround);
                    }
                }
            }
        }

        // Avoid using excesive amounts of CPU time
        std::this_thread::sleep_for(m_workerSleepDuration);
    }
}


//
// IService::Start
//
HRESULT FullscreenMonitor::Start()
{
    m_bAutoHideModules = GetRCBoolW(L"LSAutoHideModules", TRUE) != FALSE;
    m_bRun.store(true);
    m_fullscreenMonitorThread = std::thread(std::bind(&FullscreenMonitor::ThreadProc, this));

    return S_OK;
}


//
// IService::Stop
//
HRESULT FullscreenMonitor::Stop()
{
    m_bRun.store(false);
    m_fullscreenMonitorThread.join();

    return S_OK;
}


//
// IService::Recycle
//
HRESULT FullscreenMonitor::Recycle()
{
    bool bAlreadyHidden = m_bAutoHideModules;
    m_bAutoHideModules = GetRCBoolW(L"LSAutoHideModules", TRUE) != FALSE;

    // We need to show the modules that were hidden before
    if (bAlreadyHidden && !m_bAutoHideModules)
    {
        // TODO::We could possibly just show the modules on monitors with fullscreen windows. 
        ParseBangCommandW(nullptr, L"!ShowModules", nullptr);
    }

    m_bReHide.store(true);

    return S_OK;
}
