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
#ifndef __TRAYSERVICE_H
#define __TRAYSERVICE_H

#include "../utility/common.h"
#include "../utility/IService.h"
#include <shellapi.h>
#include <vector>


// this is sent to the systray module(s)
typedef struct
{
	DWORD cbSize;
	HWND hWnd;
	UINT uID;
	UINT uFlags;
	UINT uCallbackMessage;
	HICON hIcon;
	CHAR szTip[256];
	DWORD dwState;
	DWORD dwStateMask;
}
LSNOTIFYICONDATA;

typedef std::vector<LSNOTIFYICONDATA*> IconVector;


//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
//
// NOTIFYICONDATA variants
//

// Win9x
typedef struct
{
    DWORD cbSize;
    HWND hWnd;
    UINT uID;
    UINT uFlags;
    UINT uCallbackMessage;
    HICON hIcon;
    CHAR szTip[64];
} NID_4A;

// NT 4.0
typedef struct
{
	DWORD cbSize;
	HWND hWnd;
	UINT uID;
	UINT uFlags;
	UINT uCallbackMessage;
	HICON hIcon;
	WCHAR szTip[64];
} NID_4W;

// IE 5 (ME?)
typedef struct
{
    DWORD cbSize;
    HWND hWnd;
    UINT uID;
    UINT uFlags;
    UINT uCallbackMessage;
    HICON hIcon;
    CHAR szTip[128];
    DWORD dwState;
    DWORD dwStateMask;
    CHAR szInfo[256];
    union
    {
        UINT uTimeout;
        UINT uVersion;
    } DUMMYUNIONNAME;
    CHAR szInfoTitle[64];
    DWORD dwInfoFlags;
} NID_5A;

// IE 5 (2K)
typedef struct
{
    DWORD cbSize;
    HWND hWnd;
    UINT uID;
    UINT uFlags;
    UINT uCallbackMessage;
    HICON hIcon;
    WCHAR szTip[128];
    DWORD dwState;
    DWORD dwStateMask;
    WCHAR szInfo[256];
    union
    {
        UINT uTimeout;
        UINT uVersion;
    } DUMMYUNIONNAME;
    WCHAR szInfoTitle[64];
    DWORD dwInfoFlags;
} NID_5W;

// IE 6
typedef struct
{
	DWORD cbSize;
	HWND hWnd;
	UINT uID;
	UINT uFlags;
	UINT uCallbackMessage;
	HICON hIcon;
	CHAR szTip[128];
	DWORD dwState;
	DWORD dwStateMask;
	CHAR szInfo[256];
	union
    {
		UINT uTimeout;
		UINT uVersion;
	} DUMMYUNIONNAME;
	CHAR szInfoTitle[64];
	DWORD dwInfoFlags;
    GUID guidItem;
} NID_6A;

// IE 6 (XP)
typedef struct
{
	DWORD cbSize;
	HWND hWnd;
	UINT uID;
	UINT uFlags;
	UINT uCallbackMessage;
	HICON hIcon;
	WCHAR szTip[128];
	DWORD dwState;
	DWORD dwStateMask;
	WCHAR szInfo[256];
	union
    {
		UINT uTimeout;
		UINT uVersion;
	} DUMMYUNIONNAME;
	WCHAR szInfoTitle[64];
	DWORD dwInfoFlags;
	GUID guidItem;
} NID_6W;


union NID_PTR
{
    NID_4A* w9X;
    NID_4W* NT4;
    NID_5A* IE5;
    NID_5W* w2K;
    NID_6A* IE6;
    NID_6W* wXP;
};


// size constants
const unsigned short NID_4A_SIZE = sizeof(NID_4A); // 9x
const unsigned short NID_4W_SIZE = sizeof(NID_4W); // NT 4.0
const unsigned short NID_5A_SIZE = sizeof(NID_5A); // IE 5 (ME?)
const unsigned short NID_5W_SIZE = sizeof(NID_5W); // IE 5 (2K)
const unsigned short NID_6A_SIZE = sizeof(NID_6A); // IE 6
const unsigned short NID_6W_SIZE = sizeof(NID_6W); // IE 6 (XP)


#ifndef NIF_STATE
#define NIF_STATE       0x00000008
#define NIF_INFO        0x00000010 // partial support: used as tooltip
#define NIF_GUID        0x00000020 // not supported
#define NIS_HIDDEN		0x00000001
#define NIS_SHAREDICON  0x00000002
#define NIM_SETFOCUS    0x00000003 // not supported
#define NIM_SETVERSION  0x00000004 // not supported
#endif

// data sent by shell via Shell_NotifyIcon -- Maduin
typedef struct _SHELLTRAYDATA
{
	DWORD dwUnknown;
	DWORD dwMessage;
    NOTIFYICONDATA nid;
} SHELLTRAYDATA;


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
    
    // Handler for system tray notifications
    bool HandleNotification(SHELLTRAYDATA* pstd);
    
    // resend all icon data
    void SendSystemTray();
    
    // Message Handler
    static LRESULT CALLBACK WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam,
        LPARAM lParam);
    
private:
    bool _AddIcon(const NOTIFYICONDATA* pnid);
    bool _ModifyIcon(LSNOTIFYICONDATA* plnid, const NOTIFYICONDATA* pnid);
    bool _RemoveIcon(IconVector::iterator itIcon);
    
    void _Notify(DWORD dwMessage, LSNOTIFYICONDATA* plnid);
    IconVector::iterator _FindIcon(const NOTIFYICONDATA* pnid);
    
    // methods handling version specifics
    bool _IsHidden(const NOTIFYICONDATA* pstd);
    void _CopyVersionSpecifics(LSNOTIFYICONDATA* plnidTarget,
        const NOTIFYICONDATA* pnidSource);

    // manage COM based shell services
    void _LoadShellServiceObjects();
    void _UnloadShellServiceObjects();

    inline bool _IsValidIcon(UINT uFlags)
    {
        return ((uFlags & NIF_MESSAGE) && (uFlags & NIF_ICON));
    }
    
    bool m_bWin2000;
    HWND m_hTrayWnd;
	HWND m_hLiteStep;
    HINSTANCE m_hInstance;
    
    std::vector<struct IOleCommandTarget*> m_ssoList;
    IconVector m_siVector;

    struct _FindIconPredicate;
};

#endif //!defined __TRAYSERVICE_H