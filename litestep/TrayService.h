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
#include <vector>

#define TRAYSERVICE_CLASS "Shell_TrayWnd"
#define TRAYSERVICE_TITLE "Litestep Tray Manager"

typedef struct LSNOTIFYICONDATA
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
LSNOTIFYICONDATA, *PLSNOTIFYICONDATA;

struct SYSTRAYICONDATA
{
	LSNOTIFYICONDATA nid;
	BOOL bHidden;
};
typedef SYSTRAYICONDATA* PSYSTRAYICONDATA;

struct IOleCommandTarget;
typedef std::vector<IOleCommandTarget*> ShellServiceObjectVector;
typedef std::vector<SYSTRAYICONDATA*> SystrayIconVector;


// version of NOTIFYICONDATA used by 9x
typedef struct _NOTIFYICONDATAV4A
{
	DWORD cbSize;
	HWND hWnd;
	UINT uID;
	UINT uFlags;
	UINT uCallbackMessage;
	HICON hIcon;
	CHAR szTip[64];
}
NOTIFYICONDATAV4A, *PNOTIFYICONDATAV4A;
const ULONG NOTIFYICONDATAV4A_cbSize = sizeof(NOTIFYICONDATAV4A);

// version of NOTIFYICONDATA used by NT 4.0
typedef struct _NOTIFYICONDATAV4W
{
	DWORD cbSize;
	HWND hWnd;
	UINT uID;
	UINT uFlags;
	UINT uCallbackMessage;
	HICON hIcon;
	WCHAR szTip[64];
}
NOTIFYICONDATAV4W, *PNOTIFYICONDATAV4W;
const ULONG NOTIFYICONDATAV4W_cbSize = sizeof(NOTIFYICONDATAV4W);

// version of NOTIFYICONDATA used by ME
typedef struct _NOTIFYICONDATAV5A
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
	union {
		UINT uTimeout;
		UINT uVersion;
	} DUMMYUNIONNAME;
	CHAR szInfoTitle[64];
	DWORD dwInfoFlags;
}
NOTIFYICONDATAV5A, *PNOTIFYICONDATAV5A;
const ULONG NOTIFYICONDATAV5A_cbSize = sizeof(NOTIFYICONDATAV5A);

// version of NOTIFYICONDATA used by W2K
typedef struct _NOTIFYICONDATAV5W
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
	union {
		UINT uTimeout;
		UINT uVersion;
	} DUMMYUNIONNAME;
	WCHAR szInfoTitle[64];
	DWORD dwInfoFlags;
}
NOTIFYICONDATAV5W, *PNOTIFYICONDATAV5W;
const ULONG NOTIFYICONDATAV5W_cbSize = sizeof(NOTIFYICONDATAV5W);

// version of NOTIFYICONDATA used by W2K
typedef struct _NOTIFYICONDATAV6W
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
	union {
		UINT uTimeout;
		UINT uVersion;
	} DUMMYUNIONNAME;
	WCHAR szInfoTitle[64];
	DWORD dwInfoFlags;
	GUID guidItem;
}
NOTIFYICONDATAV6W, *PNOTIFYICONDATAV6W;
const ULONG NOTIFYICONDATAV6W_cbSize = sizeof(NOTIFYICONDATAV6W);

// data sent by shell via Shell_NotifyIcon -- Maduin
typedef struct SHELLTRAYDATA
{
	DWORD dwUnknown;
	DWORD dwMessage;
	union nid
	{
		NOTIFYICONDATAV4A w9X;
		NOTIFYICONDATAV4W wNT;
		NOTIFYICONDATAV5A wME;
		NOTIFYICONDATAV5W w2K;
	} nid;
}
SHELLTRAYDATA, *PSHELLTRAYDATA;


#ifndef NIF_INFO
#define NIF_INFO        0x00000010
#endif
#ifndef NIS_HIDDEN
#define NIS_HIDDEN		0x00000001
#endif
#ifndef NIS_SHAREDICON
#define NIS_SHAREDICON  0x00000002
#endif

#ifndef NIM_SETFOCUS
#define NIM_SETFOCUS    0x00000003
#endif
#ifndef NIM_SETVERSION
#define NIM_SETVERSION  0x00000004
#endif

#ifndef NIF_STATE
#define NIF_STATE       0x00000008
#endif

class TrayService: public Service<TrayService>
{
public:
	~TrayService();
	TrayService();

	HRESULT Start();
	HRESULT Stop();

    void SendSystemTray();

    static LRESULT CALLBACK WndProcTray(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	LRESULT WindowProcTray(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);


private:
	BOOL _PrepIcon(PSYSTRAYICONDATA pnidFound, PSHELLTRAYDATA pstd, BOOL bNewIcon);
	LRESULT _AddIcon(PSYSTRAYICONDATA pnidFound, PSHELLTRAYDATA pstd);
	LRESULT _ModifyIcon(PSYSTRAYICONDATA pnidFound, PSHELLTRAYDATA pstd);
	void _LoadShellServiceObjects();
	void _UnloadShellServiceObjects();

	inline BOOL _ValidIcon(DWORD dwFlags)
	{
		return ((dwFlags & NIF_MESSAGE) && (dwFlags & NIF_ICON) && (dwFlags & NIF_TIP));
	}

    SystrayIconVector::iterator _FindItem(const HWND hWnd, const UINT uID);

	HINSTANCE m_hInstance;

	HWND m_hTrayWnd;
	HWND m_hLiteStep;
	HWND m_hConnectionsTray;

	ShellServiceObjectVector m_ssoVector;
    SystrayIconVector m_siVector;
	BOOL m_bWin2000;
};

#endif //!defined TRAYSERVICE
