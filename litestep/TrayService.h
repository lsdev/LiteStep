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
#include "TrayAppBar.h"
#include "../utility/common.h"
#include "../utility/IService.h"
#include <shellapi.h>
#include <vector>

typedef PVOID (WINAPI *FUNC_PVOID__HANDLE_DWORD)(HANDLE, DWORD);
typedef BOOL (WINAPI *FUNC_BOOL__PVOID)(PVOID);

// shell copy data types
#define SH_APPBAR_DATA    (0)
#define SH_TRAY_DATA      (1)
#define SH_LOADPROC_DATA  (2)

// internally posted AppBar messages
#define ABP_NOTIFYPOSCHANGED   (WM_USER+350)
#define ABP_NOTIFYSTATECHANGE  (WM_USER+351)
#define ABP_RAISEAUTOHIDEHWND  (WM_USER+360)

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
    _SHELLAPPBARDATA(APPBARDATAV1& abdsrc):abd(abdsrc) {}

    const APPBARDATAV1& abd;
    /**/
    DWORD  dwMessage;
    HANDLE hSharedMemory;
    DWORD  dwSourceProcessId;
    /**/
} SHELLAPPBARDATA, *PSHELLAPPBARDATA;

typedef std::vector<NotifyIcon*> IconVector;
typedef std::vector<AppBar*> BarVector;


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
    
    // resend all icon data
    HWND SendSystemTray() const;
    
    // Notify TrayService of full screen app change
    void NotifyRudeApp(bool bIsFullScreen) const;
    
    // Message Handler
    static LRESULT CALLBACK WindowTrayProc(HWND, UINT, WPARAM, LPARAM);
    static LRESULT CALLBACK WindowNotifyProc(HWND, UINT, WPARAM, LPARAM);
    
private:
    bool createWindows();
    void destroyWindows();

    //
    // manage COM based shell services
    //
    void loadShellServiceObjects();
    void unloadShellServiceObjects();
    
    // Handler for AppBar messages
    LRESULT HandleAppBarMessage(PSHELLAPPBARDATA psad);
    
    // Handler for system tray notifications
    BOOL HandleNotification(PSHELLTRAYDATA pstd);
    
    //
    // ABM_* Notification handlers
    //
    LRESULT barCreate(const APPBARDATAV1& abd);
    LRESULT barDestroy(const APPBARDATAV1& abd);
    LRESULT barQueryPos(PSHELLAPPBARDATA psad);
    LRESULT barSetPos(PSHELLAPPBARDATA psad);
    LRESULT barGetTaskBarState();
    LRESULT barGetTaskBarPos(PSHELLAPPBARDATA psad);
    LRESULT barActivate(const APPBARDATAV1& abd);
    LRESULT barGetAutoHide(UINT uEdge);
    LRESULT barSetAutoHide(const APPBARDATAV1& abd);
    LRESULT barPosChanged(const APPBARDATAV1& abd);
    LRESULT barSetTaskBarState(const APPBARDATAV1& abd);
    
    //
    // barSetPos and barQueryPos helpers
    //
    void modifyOverlapBar(RECT& rcDst, const RECT& rcOrg, UINT uEdge);
    void modifyNormalBar(RECT& rcDst, const RECT& rcOrg, UINT uEdge, HWND hWnd);
    void modifyBarExtent(RECT& rcDst, const RECT& rcOrg, UINT uEdge);
    void modifyBarBreadth(RECT& rcDst, const RECT& rcOrg, UINT uEdge);
    void adjustWorkArea();
    
    //
    // AppBar Un/Lock handlers for shared data
    //
    PAPPBARDATAV1 ABLock(PSHELLAPPBARDATA psad);
    void ABUnLock(PAPPBARDATAV1 pabd);
    
    //
    // findBar variants and wrappers
    //
    BarVector::iterator findBar(HWND hWnd);
    BarVector::iterator findBar(UINT uEdge, LPARAM lParam);
    bool isBar(HWND hWnd);
    bool getBar(HWND hWnd, BarVector::iterator& itAppBar);
    bool getBar(HWND hWnd, AppBar*& pBarRef);
    
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
    bool m_bWorkAreaDirty;
    RECT m_rWorkArea;
    HWND m_hNotifyWnd;
    HWND m_hTrayWnd;
    HWND m_hLiteStep;
    HINSTANCE m_hInstance;
    
    std::vector<struct IOleCommandTarget*> m_ssoList;
    IconVector m_siVector;
    BarVector m_abVector;
};

#endif //!defined(TRAYSERVICE_H)
