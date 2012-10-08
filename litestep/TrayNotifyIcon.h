//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//
// This is a part of the Litestep Shell source code.
//
// Copyright (C) 1997-2012  LiteStep Development Team
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
#if !defined(TRAYNOTIFYICON_H)
#define TRAYNOTIFYICON_H

#include "../utility/core.hpp"
#include <vector>

#define TRAY_MAX_TIP_LENGTH       128
#define TRAY_MAX_INFO_LENGTH      256
#define TRAY_MAX_INFOTITLE_LENGTH 64

//
// This struct is sent to the systray module(s)
//
// NOTE: Currently, the dwState/Mask members are never passed on to the
// systray modules. dwState/Mask are handled internally to this tray service.
//
typedef struct
{
    DWORD cbSize;                                /* arbitrary  &     volatile */
    HWND hWnd;                                   /* persistent & non volatile */
    UINT uID;                                    /* persistent & non volatile */
    UINT uFlags;                                 /* persistent &     volatile */
    UINT uCallbackMessage;                       /* persistent &     volatile */
    HICON hIcon;                                 /* persistent &     volatile */
    CHAR szTip[TRAY_MAX_TIP_LENGTH];             /* persistent &     volatile */
    
    // new in 2K:
    DWORD dwState;                               /* persistent &     volatile */
    DWORD dwStateMask;                           /* arbitrary  &     volatile */
    CHAR szInfo[TRAY_MAX_INFO_LENGTH];           /* arbitrary  &     volatile */
    union
    {
        UINT uTimeout;                           /* arbitrary  &     volatile */
        UINT uVersion;                           /* arbitrary  &     volatile */
    } DUMMYUNIONNAME;
    CHAR szInfoTitle[TRAY_MAX_INFOTITLE_LENGTH]; /* arbitrary  &     volatile */
    DWORD dwInfoFlags;                           /* arbitrary  &     volatile */
    
    // new in XP:
    GUID guidItem;                               /* ???? */
    /**/
} LSNOTIFYICONDATA, *PLSNOTIFYICONDATA;
typedef const LSNOTIFYICONDATA * PCLSNOTIFYICONDATA;


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

// Vista
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
    HICON hBalloonIcon;
} NID_7W;

// sub structure common to all others
typedef struct
{
    DWORD cbSize;
    HWND hWnd;
    UINT uID;
    UINT uFlags;
    UINT uCallbackMessage;
    HICON hIcon;
} NID_XX, *PNID_XX;
typedef const NID_XX * PCNID_XX;


// size constants
#define NID_4A_SIZE     (sizeof(NID_4A)) // 9x          [=  88]
#define NID_4W_SIZE     (sizeof(NID_4W)) // NT 4.0      [= 152]
#define NID_5A_SIZE     (sizeof(NID_5A)) // IE 5 (ME?)  [= 488]
#define NID_5W_SIZE     (sizeof(NID_5W)) // IE 5 (2K)   [= 936]
#define NID_6A_SIZE     (sizeof(NID_6A)) // IE 6        [= 504]
#define NID_6W_SIZE     (sizeof(NID_6W)) // IE 6 (XP)   [= 952]
#define NID_7W_SIZE     (sizeof(NID_7W)) // Vista       [= 956]


#if !defined(NIF_STATE)
#define NIF_STATE       0x00000008
#define NIF_INFO        0x00000010
#define NIF_GUID        0x00000020 // not supported
#define NIF_REALTIME    0x00000040 // not supported
#define NIF_SHOWTIP     0x00000080 // not supported
#define NIS_HIDDEN      0x00000001
#define NIS_SHAREDICON  0x00000002
#define NIM_SETFOCUS    0x00000003 // relayed to systray module
#define NIM_SETVERSION  0x00000004 // relayed to systray module
#endif // NIF_STATE

class NotifyIcon
{
    typedef std::vector<NotifyIcon*> IcVtr;
    static IcVtr s_icVtr;
    
public:
    NotifyIcon(const NID_XX& nidSource);
    ~NotifyIcon();
    
    void Update(const NID_XX& nidSource);
    
    inline bool IsHidden() const
    {
        return ((NIF_STATE & m_uFlags) && (NIS_HIDDEN & m_dwState));
    }
    
    inline bool IsShared() const
    {
        return ((NIF_STATE & m_uFlags) && (NIS_SHAREDICON & m_dwState));
    }
    
    inline bool IsValid() const
    {
        return HasIcon() && !IsHidden();
    }
    
    inline bool HasIcon() const
    {
        return NIF_ICON == (NIF_ICON & m_uFlags);
    }
    
    inline HWND GetHwnd() const
    {
        return m_hWnd;
    }
    
    inline UINT GetuID() const
    {
        return m_uID;
    }
    
    inline void CopyLSNID(LSNOTIFYICONDATA * plsnid) const
    {
        CopyLSNID(plsnid, (UINT)-1);
    }
    
    void CopyLSNID(LSNOTIFYICONDATA * plsnid, UINT uFlagMask) const;
    
private:
    void copy_message(PCNID_XX pnidSource);
    void copy_icon(PCNID_XX pnidSource);
    void copy_tip(PCNID_XX pnidSource);
    void copy_state(PCNID_XX pnidSource);
    
    void update_state(DWORD dwState, DWORD dwMask);
    
    // Preserved Notify Icon Data members
    const HWND  m_hWnd;                          /* persistent & non volatile */
    const UINT  m_uID;                           /* persistent & non volatile */
    
    UINT  m_uFlags;                              /* persistent &     volatile */
    UINT  m_uCallbackMessage;                    /* persistent &     volatile */
    HICON m_hIcon;                               /* persistent &     volatile */
    CHAR  m_szTip[TRAY_MAX_TIP_LENGTH];          /* persistent &     volatile */
    
    DWORD m_dwState;                             /* persistent &     volatile */
    
    // Internal tracking
    HANDLE m_hOriginalIcon;
    HANDLE m_hSharedWnd;
    UINT   m_uSharedID;
    
    // not implemented
    NotifyIcon(const NotifyIcon& rhs);
    NotifyIcon& operator=(const NotifyIcon& rhs);
};

#endif //!defined(TRAYNOTIFYICON_H)
