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
#if !defined(TRAYAPPBAR_H)
#define TRAYAPPBAR_H

#include <shellapi.h>

#define ABE_HORIZONTAL      0x01

#define ABS_TASKBAR         0x00010000
#define ABS_OVERLAPAUTOHIDE 0x00020000
#define ABS_CLEANRECT       0x00040000

// Maintains 32bit handles on 64bit builds
typedef DWORD HANDLE32;
typedef DWORD HWND32;
typedef LONG LPARAM32;

typedef struct _APPBARDATAV1
{
    DWORD cbSize;
    HWND32 hWnd;
    UINT uCallbackMessage;
    UINT uEdge;
    RECT rc;
    LPARAM32 lParam;
} APPBARDATAV1, *PAPPBARDATAV1;

typedef struct _APPBARDATAV2
{
    DWORD cbSize;
    HWND32 hWnd;
    UINT uCallbackMessage;
    UINT uEdge;
    RECT rc;
    LPARAM32 lParam;
    DWORD dw64BitAlign;
} APPBARDATAV2, *PAPPBARDATAV2;

/* 2K */
typedef struct _APPBARMSGDATAV1
{
    APPBARDATAV1 abd;
    /**/
    DWORD  dwMessage;
    HANDLE32 hSharedMemory;
    DWORD  dwSourceProcessId;
    /**/
} APPBARMSGDATAV1, *PAPPBARMSGDATAV1;
typedef const APPBARMSGDATAV1 * PCAPPBARMSGDATAV1;

/* XP */
typedef struct _APPBARMSGDATAV2
{
    APPBARDATAV2 abd;
    /**/
    DWORD  dwMessage;
    HANDLE32 hSharedMemory;
    DWORD  dwSourceProcessId;
    /**/
    DWORD dw64BitAlign;
    /**/
} APPBARMSGDATAV2, *PAPPBARMSGDATAV2;
typedef const APPBARMSGDATAV2 * PCAPPBARMSGDATAV2;

/* Win7 */
typedef struct _APPBARMSGDATAV3
{
    APPBARDATAV2 abd;
    /**/
    DWORD  dwMessage;
    DWORD  dwPadding1;
    /**/
    HANDLE32 hSharedMemory;
    DWORD  dwPadding2;
    /**/
    DWORD  dwSourceProcessId;
    DWORD  dwPadding3;
    /**/
} APPBARMSGDATAV3, *PAPPBARMSGDATAV3;
typedef const APPBARMSGDATAV3* PCAPPBARMSGDATAV3;


class AppBar
{
public:
    AppBar(HWND hWnd, UINT uMsg)
        :m_hWnd(hWnd)
        ,m_uCallbackMessage(uMsg)
        ,m_lParam(0)
        ,m_uEdge((UINT)-1)
        ,m_hMon(NULL)
    {
        // do nothing
    }
    ~AppBar()
    {
        // do nothing
    }
    
    inline HWND hWnd() const
    {
        return m_hWnd;
    }
    
    inline LPARAM lParam() const
    {
        return m_lParam;
    }
    
    inline void lParam(LPARAM lParam)
    {
        m_lParam = lParam;
    }
    
    inline UINT uEdge() const
    {
        return m_uEdge;
    }
    
    inline void uEdge(UINT uEdge)
    {
        m_uEdge = uEdge;
    }
    
    inline HMONITOR hMon() const
    {
        return m_hMon;
    }
    
    inline void hMon(HMONITOR hMon)
    {
        m_hMon = hMon;
    }
    
    inline UINT uMsg() const
    {
        return m_uCallbackMessage;
    }
    
    inline bool IsAutoHide() const
    {
        return ABS_AUTOHIDE == (m_lParam & ABS_AUTOHIDE);
    }
    
    inline bool IsOverLap() const
    {
        return ABS_OVERLAPAUTOHIDE == (m_lParam & ABS_OVERLAPAUTOHIDE);
    }
    
    inline const RECT& GetRectRef() const
    {
        return m_rc;
    }
    
    inline RECT& GetRectRef()
    {
        return m_rc;
    }
    
private:
    const HWND m_hWnd;
    const UINT m_uCallbackMessage;
    
    LPARAM   m_lParam;
    RECT     m_rc;
    UINT     m_uEdge;
    HMONITOR m_hMon;
    
    // not implemented
    AppBar(const AppBar& rhs);
    AppBar& operator=(const AppBar& rhs);
};

#endif /* !defined(TRAYAPPBAR_H) */
