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
#if !defined(TRAYSERVICE_H)
#define TRAYSERVICE_H

#include "TrayNotifyIcon.h"
#include "../utility/common.h"
#include "../utility/IService.h"
#include <shellapi.h>
#include <vector>

#define SH_APPBAR_DATA    (0)
#define SH_TRAY_DATA      (1)
#define SH_LOADPROC_DATA  (2)

// data sent by shell via Shell_NotifyIcon
typedef struct _SHELLTRAYDATA
{
    DWORD dwUnknown;
    DWORD dwMessage;
    NID_XX nid;
} *PSHELLTRAYDATA;

// Data sent with AppBar Message
typedef struct _SHELLAPPBARDATA
{
    APPBARDATA abd;
    DWORD dwMessage;
    DWORD dwDontKnow;
    DWORD dwNoClue;
} *PSHELLAPPBARDATA;

typedef std::vector<NotifyIcon*> IconVector;


//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
//
// TrayService
//
// This is the tray service handler. It keeps track of all systray icons and
// loads ShellService-objects. If an icon is added/removed/modified/... it
// notifies all listeners (usually the systray module) via LM_SYSTRAY.
//
class TrayService: public IService
{
public:
    ~TrayService();
    TrayService();
    
    //
    // IService methods
    //
    virtual HRESULT Start();
    virtual HRESULT Stop();
    
    // Handler for AppBar messages
    BOOL HandleAppBarMessage(PSHELLAPPBARDATA pData);

    // Handler for system tray notifications
    BOOL HandleNotification(PSHELLTRAYDATA pstd);
    
    // resend all icon data
    HWND SendSystemTray() const;
    
    // Message Handler
    static LRESULT CALLBACK WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam,
        LPARAM lParam);
    
private:
    bool createWindows();
    void destroyWindows();

    //
    // manage COM based shell services
    //
    void loadShellServiceObjects();
    void unloadShellServiceObjects();

    //
    // NIM_* Notification handlers
    //
    bool addIcon(const NID_XX& nid);        // NIM_ADD
    bool modifyIcon(const NID_XX& nid);     // NIM_MODIFY
    bool deleteIcon(const NID_XX& nid);     // NIM_DELETE
    bool setFocusIcon(const NID_XX& nid);   // NIM_SETFOCUS
    bool setVersionIcon(const NID_XX& nid); // NIM_SETVERSION

    //
    // Send icon notifications on to Litestep (thus systray modules)
    //
    bool notify(DWORD dwMessage, PCLSNOTIFYICONDATA pclsnid) const;
    bool extendNIDCopy(LSNOTIFYICONDATA& lsnid, const NID_XX& nid) const;

    //
    // findIcon variants
    //
    IconVector::iterator findIcon(HWND hWnd, UINT uId);

    inline IconVector::iterator findIcon(const NID_XX& nid)
    {
        return findIcon(nid.hWnd, nid.uID);
    }

    //
    //
    //
    bool m_bWin2000;
    HWND m_hNotifyWnd;
    HWND m_hTrayWnd;
    HWND m_hLiteStep;
    HINSTANCE m_hInstance;
    
    std::vector<struct IOleCommandTarget*> m_ssoList;
    IconVector m_siVector;
};

#endif //!defined(TRAYSERVICE_H)
