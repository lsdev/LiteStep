/*
This is a part of the LiteStep Shell Source code.

Copyright (C) 1997-2005 The LiteStep Development Team

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

#define TRAY_MAX_TIP_LENGTH       128
#define TRAY_MAX_INFO_LENGTH      256
#define TRAY_MAX_INFOTITLE_LENGTH 64

// this is sent to the systray module(s)
typedef struct
{
	DWORD cbSize;
	HWND hWnd;
	UINT uID;
	UINT uFlags;
	UINT uCallbackMessage;
	HICON hIcon;
	CHAR szTip[TRAY_MAX_TIP_LENGTH];

	// new in 2K:
    DWORD dwState;
	DWORD dwStateMask;
    CHAR szInfo[TRAY_MAX_INFO_LENGTH];
    union
    {
        UINT uTimeout;
        UINT uVersion;
    } DUMMYUNIONNAME;
    CHAR szInfoTitle[TRAY_MAX_INFOTITLE_LENGTH];
    DWORD dwInfoFlags;

    // new in XP:
    GUID guidItem;
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
#define NIF_INFO        0x00000010
#define NIF_GUID        0x00000020 // not supported
#define NIS_HIDDEN		0x00000001
#define NIS_SHAREDICON  0x00000002
#define NIM_SETFOCUS    0x00000003 // relayed to systray module
#define NIM_SETVERSION  0x00000004 // relayed to systray module
#endif

// data sent by shell via Shell_NotifyIcon -- Maduin
typedef struct _SHELLTRAYDATA
{
	DWORD dwUnknown;
	DWORD dwMessage;
    NOTIFYICONDATA nid;
} SHELLTRAYDATA;

// Data sent with AppBar Message (from Maduin)
typedef struct _SHELLAPPBARDATA
{
    APPBARDATA abd;
    DWORD dwMessage;
    DWORD dwDontKnow;
    DWORD dwNoClue;
} SHELLAPPBARDATA;


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
    BOOL HandleAppBarMessage(SHELLAPPBARDATA* pData);

    // Handler for system tray notifications
    BOOL HandleNotification(SHELLTRAYDATA* pstd);
    
    // resend all icon data
    HWND SendSystemTray();
    
    // Message Handler
    static LRESULT CALLBACK WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam,
        LPARAM lParam);
    
private:
    bool _CreateWindows();
    void _DestroyWindows();

    //
    // manage COM based shell services
    //
    void _LoadShellServiceObjects();
    void _UnloadShellServiceObjects();

    //
    // NIM_* handlers
    //
    bool _AddIcon(const NOTIFYICONDATA& nid);
    bool _ModifyIcon(LSNOTIFYICONDATA* plnid, const NOTIFYICONDATA& nid);
    bool _RemoveIcon(IconVector::iterator itIcon);
    
    bool _CopyIconHandle(LSNOTIFYICONDATA& lnidTarget,
        const NOTIFYICONDATA& nidSource) const;
    bool _Notify(DWORD dwMessage, LSNOTIFYICONDATA* plnid);
    
    //
    // methods handling version specifics
    //
    void _CopyVersionSpecifics(LSNOTIFYICONDATA& lnidTarget,
                               const NOTIFYICONDATA& nidSource) const;

    //
    // _FindIcon variants
    //
    IconVector::iterator _FindIcon(HWND hWnd, UINT uId);
    
    inline IconVector::iterator _FindIcon(const NOTIFYICONDATA& nid)
    {
        return _FindIcon(nid.hWnd, nid.uID);
    }
    
    //
    // miscellaneous
    //
    inline bool _IsUnicode(const NOTIFYICONDATA& nid) const
    {
        return (nid.cbSize != NID_4A_SIZE && nid.cbSize != NID_5A_SIZE && 
            nid.cbSize != NID_6A_SIZE);
    }

    inline bool _IsHidden(const LSNOTIFYICONDATA& lnid) const
    {
        return ((lnid.uFlags & NIF_STATE) &&
            ((lnid.dwState & lnid.dwStateMask) & NIS_HIDDEN));
    }

    inline bool _IsShared(const LSNOTIFYICONDATA& lnid) const
    {
        return ((lnid.uFlags & NIF_STATE) &&
            ((lnid.dwState & lnid.dwStateMask) & NIS_SHAREDICON));
    }

    inline bool _IsShared(const NOTIFYICONDATA& nid) const
    {
        NID_6W* pnid = (NID_6W*)&nid;
        return ((pnid->uFlags & NIF_STATE) &&
            (pnid->cbSize != NID_4A_SIZE) && (pnid->cbSize != NID_4W_SIZE) &&
            ((pnid->dwState & pnid->dwStateMask) & NIS_SHAREDICON));
    }

    inline bool _IsValidIcon(const LSNOTIFYICONDATA* plnid) const
    {
        ASSERT_ISNOTNULL(plnid);
        return ((plnid->uFlags & NIF_ICON) && !_IsHidden(*plnid));
    }

    int _ConvertWideToAnsi(char* pszOutput, size_t cchOutput,
        const wchar_t* pwzInput, size_t cchInputMax) const;

    bool _StringCopy(LPSTR pszDest, size_t cchDest, LPCSTR pszSrc) const;
    bool _StringCopy(LPSTR pszDest, size_t cchDest, LPCWSTR pwzSrc) const;
    
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

#endif //!defined __TRAYSERVICE_H
