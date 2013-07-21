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
#include <list>
#include "../lsapi/lsapi.h"
#include "../utility/debug.hpp"


//
// FullscreenMonitor
//
FullscreenMonitor::FullscreenMonitor()
{
    m_bIsRunning = false;
}


//
// ~FullscreenMonitor
//
FullscreenMonitor::~FullscreenMonitor()
{
    if (m_bIsRunning)
    {
        Stop();
    }
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
    
    DWORD dwProcessID;
    DWORD dwThreadID = GetWindowThreadProcessId(hWnd, &dwProcessID);
    
    DWORD dwLSProcessID;
    GetWindowThreadProcessId(GetLitestepWnd(), &dwLSProcessID);
    
    HMONITOR hMonFS = nullptr;
    
    if (dwProcessID != dwLSProcessID)
    {
        EnumThreadWindows(dwThreadID, _EnumThreadFSWnd, (LPARAM)&hMonFS);
    }
    
    return hMonFS;
}


//
// ThreadProc
//
void FullscreenMonitor::ThreadProc(FullscreenMonitor* pFullscreenMonitor)
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

    // How long to sleep for
    std::chrono::milliseconds sleepDuration(100);

    // List of all currently living known fullscreen windows
    std::list<FSWindow> fullscreenWindows;

    // On startup, find any existing fullscreen windows.
    EnumDesktopWindows(nullptr, [] (HWND hWnd, LPARAM lParam) -> BOOL {
        HMONITOR hMonitor = IsFullscreenWindow(hWnd);
        std::list<FSWindow> *fullscreenWindows = (std::list<FSWindow>*)lParam;
        if (hMonitor)
        {
            // If the modules on this monitor are not already hidden, hide them.
            bool bAlreadyHidden = false;
            for (FSWindow window : *fullscreenWindows)
            {
                if (window.hMonitor == hMonitor)
                {
                    bAlreadyHidden = true;
                    break;
                }
            }
            if (!bAlreadyHidden)
            {
                ParseBangCommand((HWND)hMonitor, "!HideModules", nullptr);
            }

            // And add it to the list of fullscreen windows.
            fullscreenWindows->push_back(FSWindow(hWnd, hMonitor));
        }
        return TRUE;
    }, (LPARAM)&fullscreenWindows);

    // Main Loop
    while (pFullscreenMonitor->m_bRun.load())
    {
        HWND hwndForeGround = GetForegroundWindow();
        bool bCheckedForeGround = false;

        // Re-Hide modules on all monitors
        if (pFullscreenMonitor->m_bReHide.load())
        {
            pFullscreenMonitor->m_bReHide.store(false);
            for (FSWindow window : fullscreenWindows)
            {
                ParseBangCommand((HWND)window.hMonitor, "!HideModules", nullptr);
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
                // Check if there are any other fullscreen windows on this monitor.
                __int8 nFSWindows = 0;
                for (FSWindow window : fullscreenWindows)
                {
                    if (window.hMonitor == iter->hMonitor)
                    {
                        ++nFSWindows;
                    }
                }
                // If not, show the modules on it.
                if (nFSWindows < 2)
                {
                    ParseBangCommand((HWND)iter->hMonitor, "!ShowModules", nullptr);
                }

                // Check if the window moved to another monitor (and stayed fullscreen)
                if (hNewMonitor)
                {
                    iter->hMonitor = hNewMonitor;
                    ParseBangCommand((HWND)hNewMonitor, "!HideModules", nullptr);
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
            HMONITOR hMonitor = IsFullscreenWindow(hwndForeGround);
            if (hMonitor)
            {
                // Check if we already know of a fullscreen window on this monitor.
                bool bKnownWindow = false;
                for (FSWindow window : fullscreenWindows)
                {
                    if (window.hMonitor == hMonitor)
                    {
                        bKnownWindow = true;
                        break;
                    }
                }

                // Add the window to the list of known foreground windows.
                fullscreenWindows.push_back(FSWindow(hwndForeGround, hMonitor));

                // If we didn't know about any fullscreen windows on this monitor before, hide the modules on it.
                if (!bKnownWindow)
                {
                    ParseBangCommand((HWND)hMonitor, "!HideModules", nullptr);
                }
            }
        }

        //
        std::this_thread::sleep_for(sleepDuration);
    }

    // We are stoping, if there are any current fullscreen windows, show all modules.
    if (!fullscreenWindows.empty())
    {
        ParseBangCommand(nullptr, "!ShowModules", nullptr);
    }
}


//
// IService::Start
//
HRESULT FullscreenMonitor::Start()
{
    if (!m_bIsRunning)
    {
        m_bIsRunning = true;
        m_bRun.store(true);
        m_fullscreenMonitorThread = std::thread(ThreadProc, this);
    }
    else
    {
        m_bReHide.store(true);
    }

    return S_OK;
}


//
// IService::Stop
//
HRESULT FullscreenMonitor::Stop()
{
    if (m_bIsRunning)
    {
        m_bIsRunning = false;
        m_bRun.store(false);
        m_fullscreenMonitorThread.join();
    }

    return S_OK;
}
