// Clock Example
// Copyright (C) 2006 Litestep Development Team
//
// This program is free software; you can redistribute it and/or modify it under
// the terms of the GNU Lesser General Public License as published by the Free
// Software Foundation; either version 2.1 of the License, or (at your option)
// any later version.
//
// This program is distributed in the hope that it will be useful, but WITHOUT
// ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
// FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more
// details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with this program; if not, write to the Free Software Foundation, Inc.,
// 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA

#if !defined(MSGCRACK_H)
#define MSGCRACK_H


#define DISPATCH_MSG(message, wParam, lParam, fn) \
    case (message): return DISPATCH_##message((wParam), (lParam), (fn))

// void OnDestroy()
#define DISPATCH_WM_DESTROY(wParam, lParam, fn) \
    ((fn)(), 0L)

// void OnLButtonDblClk(UINT fuFlags, int x, int y)
#define DISPATCH_WM_LBUTTONDBLCLK(wParam, lParam, fn) \
    ((fn)((UINT)(wParam), (int)(short)LOWORD(lParam), (int)(short)HIWORD(lParam)), 0L)

// void OnLButtonDown(UINT fuFlags, int x, int y)
#define DISPATCH_WM_LBUTTONDOWN(wParam, lParam, fn) \
    ((fn)((UINT)(wParam), (int)(short)LOWORD(lParam), (int)(short)HIWORD(lParam)), 0L)

// void OnLButtonUp(UINT fuFlags, int x, int y)
#define DISPATCH_WM_LBUTTONUP(wParam, lParam, fn) \
    ((fn)((UINT)(wParam), (int)(short)LOWORD(lParam), (int)(short)HIWORD(lParam)), 0L)

// void OnMButtonDblClk(UINT fuFlags, int x, int y)
#define DISPATCH_WM_MBUTTONDBLCLK(wParam, lParam, fn) \
    ((fn)((UINT)(wParam), (int)(short)LOWORD(lParam), (int)(short)HIWORD(lParam)), 0L)

// void OnMButtonDown(UINT fuFlags, int x, int y)
#define DISPATCH_WM_MBUTTONDOWN(wParam, lParam, fn) \
    ((fn)((UINT)(wParam), (int)(short)LOWORD(lParam), (int)(short)HIWORD(lParam)), 0L)

// void OnMButtonUp(UINT fuFlags, int x, int y)
#define DISPATCH_WM_MBUTTONUP(wParam, lParam, fn) \
    ((fn)((UINT)(wParam), (int)(short)LOWORD(lParam), (int)(short)HIWORD(lParam)), 0L)

// void OnMouseLeave()
#define DISPATCH_WM_MOUSELEAVE(wParam, lParam, fn) \
    ((fn)(), 0L)

// void OnMouseMove(UINT fuFlags, int x, int y)
#define DISPATCH_WM_MOUSEMOVE(wParam, lParam, fn) \
    ((fn)((UINT)(wParam), (int)(short)LOWORD(lParam), (int)(short)HIWORD(lParam)), 0L)

// void OnMove(int x, int y)
#define DISPATCH_WM_MOVE(wParam, lParam, fn) \
    ((fn)((int)(short)LOWORD(lParam), (int)(short)HIWORD(lParam)), 0L)

// LRESULT OnNotify(int idCtrl, LPNMHDR pnmh)
#define DISPATCH_WM_NOTIFY(wParam, lParam, fn) \
    (LRESULT)(fn)((int)(wParam), (LPNMHDR)(lParam))

// void OnPaint()
#define DISPATCH_WM_PAINT(wParam, lParam, fn) \
    ((fn)(), 0L)

// void OnPrintClient(HDC hdc, UINT fuFlags)
#define DISPATCH_WM_PRINTCLIENT(wParam, lParam, fn) \
    ((fn)((HDC)(wParam), (UINT)(lParam)), 0L)

// void OnRButtonDblClk(UINT fuFlags, int x, int y)
#define DISPATCH_WM_RBUTTONDBLCLK(wParam, lParam, fn) \
    ((fn)((UINT)(wParam), (int)(short)LOWORD(lParam), (int)(short)HIWORD(lParam)), 0L)

// void OnRButtonDown(UINT fuFlags, int x, int y)
#define DISPATCH_WM_RBUTTONDOWN(wParam, lParam, fn) \
    ((fn)((UINT)(wParam), (int)(short)LOWORD(lParam), (int)(short)HIWORD(lParam)), 0L)

// void OnRButtonUp(UINT fuFlags, int x, int y)
#define DISPATCH_WM_RBUTTONUP(wParam, lParam, fn) \
    ((fn)((UINT)(wParam), (int)(short)LOWORD(lParam), (int)(short)HIWORD(lParam)), 0L)

// void OnSize(UINT uState, int cx, int cy)
#define DISPATCH_WM_SIZE(wParam, lParam, fn) \
    ((fn)((UINT)(wParam), (int)(short)LOWORD(lParam), (int)(short)HIWORD(lParam)), 0L)

// void OnTimer(UINT uID)
#define DISPATCH_WM_TIMER(wParam, lParam, fn) \
    ((fn)((UINT)(wParam)), 0L)


// int OnGetRevID(LPSTR pszBuffer)
#define DISPATCH_LM_GETREVID(wParam, lParam, fn) \
    (LRESULT)(int)(fn)((LPSTR)(lParam))

// BOOL OnSysTray(DWORD dwMessage, LPLSNOTIFYICONDATA plsnid)
#define DISPATCH_LM_SYSTRAY(wParam, lParam, fn) \
    (LRESULT)(BOOL)(fn)((DWORD)(wParam), (LPLSNOTIFYICONDATA)(lParam))


#endif // MSGCRACK_H
