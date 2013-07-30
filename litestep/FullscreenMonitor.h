//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//
// This is a part of the Litestep Shell source code.
//
// Copyright (C) 1998 (e)
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
#if !defined(FULLSCREENMONITOR_H)
#define FULLSCREENMONITOR_H

#include "../utility/IService.h"
#include <thread>
#include <atomic>
#include <list>
#include <unordered_set>

class FullscreenMonitor: public IService
{
public:
    FullscreenMonitor();
    ~FullscreenMonitor();
    
    HRESULT Start() override;
    HRESULT Stop() override;
    HRESULT Recycle() override;

private:
    static HMONITOR _FullScreenGetMonitorHelper(HWND hWnd);
    static BOOL CALLBACK _EnumThreadFSWnd(HWND hWnd, LPARAM lParam);
    static HMONITOR IsFullscreenWindow(HWND hwnd);
    void ThreadProc();
    void ShowModules(HMONITOR hMonitor);
    void HideModules(HMONITOR hMonitor, HWND hWnd);

    // LS thread only
private:
    std::thread m_fullscreenMonitorThread;
    
    // Shared
private:
    std::atomic<bool> m_bRun;
    std::atomic<bool> m_bReHide;
    std::atomic<bool> m_bAutoHideModules;

    // Worker thread only
private:
    std::chrono::milliseconds m_workerSleepDuration;
};

#endif // FULLSCREENMONITOR_H
