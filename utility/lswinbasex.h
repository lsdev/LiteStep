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
#ifndef __LSWINBASEX_H
#define __LSWINBASEX_H

#include "common.h"
#include "lswinbase.h"
#include "../lsapi/lsapi.h"

#if !defined(ASSERT)
#  if defined(_DEBUG)
#    include <crtdbg.h>
#    define ASSERT(x) _ASSERTE(x)
#  else
#    define ASSERT(x)
#  endif
#endif // !defined(ASSERT)

#ifdef END_MESSAGEPROC
#undef END_MESSAGEPROC
#define END_MESSAGEPROC default: WindowX::windowProc(message); }
#endif

class WindowX: public Window
{
public:
	WindowX(LPCSTR className);
	virtual ~WindowX();

protected:
	virtual void windowProc(Message& message);
	void onEndSession(Message& message);
	void onSysCommand(Message& message);
	void onGetRevId(Message& message);
	void onTop(bool bSetAlwaysOnTop);

	virtual LPCSTR Revision() = 0;

	// Attributes

	DWORD GetStyle() const
	{
		ASSERT(::IsWindow(hWnd));
		return (DWORD)::GetWindowLong(hWnd, GWL_STYLE);
	}

	DWORD GetExStyle() const
	{
		ASSERT(::IsWindow(hWnd));
		return (DWORD)::GetWindowLong(hWnd, GWL_EXSTYLE);
	}

	LONG GetWindowLong(int nIndex) const
	{
		ASSERT(::IsWindow(hWnd));
		return ::GetWindowLong(hWnd, nIndex);
	}

	LONG SetWindowLong(int nIndex, LONG dwNewLong)
	{
		ASSERT(::IsWindow(hWnd));
		return ::SetWindowLong(hWnd, nIndex, dwNewLong);
	}

	WORD GetWindowWord(int nIndex) const
	{
		ASSERT(::IsWindow(hWnd));
		return ::GetWindowWord(hWnd, nIndex);
	}

	WORD SetWindowWord(int nIndex, WORD wNewWord)
	{
		ASSERT(::IsWindow(hWnd));
		return ::SetWindowWord(hWnd, nIndex, wNewWord);
	}


	// Window Text Functions

	BOOL SetWindowText(LPCSTR lpszString)
	{
		ASSERT(::IsWindow(hWnd));
		return ::SetWindowText(hWnd, lpszString);
	}

	int GetWindowText(LPSTR lpszStringBuf, int nMaxCount) const
	{
		ASSERT(::IsWindow(hWnd));
		return ::GetWindowText(hWnd, lpszStringBuf, nMaxCount);
	}

	int GetWindowTextLength() const
	{
		ASSERT(::IsWindow(hWnd));
		return ::GetWindowTextLength(hWnd);
	}

	// Font Functions

	void SetFont(HFONT hFont, BOOL bRedraw = TRUE)
	{
		ASSERT(::IsWindow(hWnd));
		::SendMessage(hWnd, WM_SETFONT, (WPARAM)hFont, MAKELPARAM(bRedraw, 0));
	}

	HFONT GetFont() const
	{
		ASSERT(::IsWindow(hWnd));
		return (HFONT)::SendMessage(hWnd, WM_GETFONT, 0, 0);
	}

	// Menu Functions (non-child windows only)

	HMENU GetMenu() const
	{
		ASSERT(::IsWindow(hWnd));
		return ::GetMenu(hWnd);
	}

	BOOL SetMenu(HMENU hMenu)
	{
		ASSERT(::IsWindow(hWnd));
		return ::SetMenu(hWnd, hMenu);
	}

	BOOL DrawMenuBar()
	{
		ASSERT(::IsWindow(hWnd));
		return ::DrawMenuBar(hWnd);
	}

	HMENU GetSystemMenu(BOOL bRevert) const
	{
		ASSERT(::IsWindow(hWnd));
		return ::GetSystemMenu(hWnd, bRevert);
	}

	BOOL HiliteMenuItem(HMENU hMenu, UINT uItemHilite, UINT uHilite)
	{
		ASSERT(::IsWindow(hWnd));
		return ::HiliteMenuItem(hWnd, hMenu, uItemHilite, uHilite);
	}

	// Window Size and Position Functions

	BOOL IsIconic() const
	{
		ASSERT(::IsWindow(hWnd));
		return ::IsIconic(hWnd);
	}

	BOOL IsZoomed() const
	{
		ASSERT(::IsWindow(hWnd));
		return ::IsZoomed(hWnd);
	}

	BOOL MoveWindow(int x, int y, int nWidth, int nHeight, BOOL bRepaint = TRUE)
	{
		ASSERT(::IsWindow(hWnd));
		return ::MoveWindow(hWnd, x, y, nWidth, nHeight, bRepaint);
	}

	BOOL MoveWindow(LPCRECT lpRect, BOOL bRepaint = TRUE)
	{
		ASSERT(::IsWindow(hWnd));
		return ::MoveWindow(hWnd, lpRect->left, lpRect->top, lpRect->right - lpRect->left, lpRect->bottom - lpRect->top, bRepaint);
	}

	BOOL SetWindowPos(HWND hWndInsertAfter, int x, int y, int cx, int cy, UINT nFlags)
	{
		ASSERT(::IsWindow(hWnd));
		return ::SetWindowPos(hWnd, hWndInsertAfter, x, y, cx, cy, nFlags);
	}

	BOOL SetWindowPos(HWND hWndInsertAfter, LPCRECT lpRect, UINT nFlags)
	{
		ASSERT(::IsWindow(hWnd));
		return ::SetWindowPos(hWnd, hWndInsertAfter, lpRect->left, lpRect->top, lpRect->right - lpRect->left, lpRect->bottom - lpRect->top, nFlags);
	}

	UINT ArrangeIconicWindows()
	{
		ASSERT(::IsWindow(hWnd));
		return ::ArrangeIconicWindows(hWnd);
	}

	BOOL BringWindowToTop()
	{
		ASSERT(::IsWindow(hWnd));
		return ::BringWindowToTop(hWnd);
	}

	BOOL GetWindowRect(LPRECT lpRect) const
	{
		ASSERT(::IsWindow(hWnd));
		return ::GetWindowRect(hWnd, lpRect);
	}

	BOOL GetClientRect(LPRECT lpRect) const
	{
		ASSERT(::IsWindow(hWnd));
		return ::GetClientRect(hWnd, lpRect);
	}

	BOOL GetWindowPlacement(WINDOWPLACEMENT FAR* lpwndpl) const
	{
		ASSERT(::IsWindow(hWnd));
		return ::GetWindowPlacement(hWnd, lpwndpl);
	}

	BOOL SetWindowPlacement(const WINDOWPLACEMENT FAR* lpwndpl)
	{
		ASSERT(::IsWindow(hWnd));
		return ::SetWindowPlacement(hWnd, lpwndpl);
	}

	// Coordinate Mapping Functions

	BOOL ClientToScreen(LPPOINT lpPoint) const
	{
		ASSERT(::IsWindow(hWnd));
		return ::ClientToScreen(hWnd, lpPoint);
	}

	BOOL ClientToScreen(LPRECT lpRect) const
	{
		ASSERT(::IsWindow(hWnd));
		if (!::ClientToScreen(hWnd, (LPPOINT)lpRect))
			return FALSE;
		return ::ClientToScreen(hWnd, ((LPPOINT)lpRect) + 1);
	}

	BOOL ScreenToClient(LPPOINT lpPoint) const
	{
		ASSERT(::IsWindow(hWnd));
		return ::ScreenToClient(hWnd, lpPoint);
	}

	BOOL ScreenToClient(LPRECT lpRect) const
	{
		ASSERT(::IsWindow(hWnd));
		if (!::ScreenToClient(hWnd, (LPPOINT)lpRect))
			return FALSE;
		return ::ScreenToClient(hWnd, ((LPPOINT)lpRect) + 1);
	}

	int MapWindowPoints(HWND hWndTo, LPPOINT lpPoint, UINT nCount) const
	{
		ASSERT(::IsWindow(hWnd));
		return ::MapWindowPoints(hWnd, hWndTo, lpPoint, nCount);
	}

	int MapWindowPoints(HWND hWndTo, LPRECT lpRect) const
	{
		ASSERT(::IsWindow(hWnd));
		return ::MapWindowPoints(hWnd, hWndTo, (LPPOINT)lpRect, 2);
	}

	// Update and Painting Functions

	HDC BeginPaint(LPPAINTSTRUCT lpPaint)
	{
		ASSERT(::IsWindow(hWnd));
		return ::BeginPaint(hWnd, lpPaint);
	}

	void EndPaint(LPPAINTSTRUCT lpPaint)
	{
		ASSERT(::IsWindow(hWnd));
		::EndPaint(hWnd, lpPaint);
	}

	HDC GetDC()
	{
		ASSERT(::IsWindow(hWnd));
		return ::GetDC(hWnd);
	}

	HDC GetWindowDC()
	{
		ASSERT(::IsWindow(hWnd));
		return ::GetWindowDC(hWnd);
	}

	int ReleaseDC(HDC hDC)
	{
		ASSERT(::IsWindow(hWnd));
		return ::ReleaseDC(hWnd, hDC);
	}

	void Print(HDC hDC, DWORD dwFlags) const
	{
		ASSERT(::IsWindow(hWnd));
		::SendMessage(hWnd, WM_PRINT, (WPARAM)hDC, dwFlags);
	}

	void PrintClient(HDC hDC, DWORD dwFlags) const
	{
		ASSERT(::IsWindow(hWnd));
		::SendMessage(hWnd, WM_PRINTCLIENT, (WPARAM)hDC, dwFlags);
	}

	BOOL UpdateWindow()
	{
		ASSERT(::IsWindow(hWnd));
		return ::UpdateWindow(hWnd);
	}

	void SetRedraw(BOOL bRedraw = TRUE)
	{
		ASSERT(::IsWindow(hWnd));
		::SendMessage(hWnd, WM_SETREDRAW, (WPARAM)bRedraw, 0);
	}

	BOOL GetUpdateRect(LPRECT lpRect, BOOL bErase = FALSE)
	{
		ASSERT(::IsWindow(hWnd));
		return ::GetUpdateRect(hWnd, lpRect, bErase);
	}

	int GetUpdateRgn(HRGN hRgn, BOOL bErase = FALSE)
	{
		ASSERT(::IsWindow(hWnd));
		return ::GetUpdateRgn(hWnd, hRgn, bErase);
	}

	BOOL Invalidate(BOOL bErase = TRUE)
	{
		ASSERT(::IsWindow(hWnd));
		return ::InvalidateRect(hWnd, NULL, bErase);
	}

	BOOL InvalidateRect(LPCRECT lpRect, BOOL bErase = TRUE)
	{
		ASSERT(::IsWindow(hWnd));
		return ::InvalidateRect(hWnd, lpRect, bErase);
	}

	BOOL ValidateRect(LPCRECT lpRect)
	{
		ASSERT(::IsWindow(hWnd));
		return ::ValidateRect(hWnd, lpRect);
	}

	void InvalidateRgn(HRGN hRgn, BOOL bErase = TRUE)
	{
		ASSERT(::IsWindow(hWnd));
		::InvalidateRgn(hWnd, hRgn, bErase);
	}

	BOOL ValidateRgn(HRGN hRgn)
	{
		ASSERT(::IsWindow(hWnd));
		return ::ValidateRgn(hWnd, hRgn);
	}

	BOOL ShowWindow(int nCmdShow)
	{
		ASSERT(::IsWindow(hWnd));
		return ::ShowWindow(hWnd, nCmdShow);
	}

	BOOL IsWindowVisible() const
	{
		ASSERT(::IsWindow(hWnd));
		return ::IsWindowVisible(hWnd);
	}

	BOOL ShowOwnedPopups(BOOL bShow = TRUE)
	{
		ASSERT(::IsWindow(hWnd));
		return ::ShowOwnedPopups(hWnd, bShow);
	}

	HDC GetDCEx(HRGN hRgnClip, DWORD flags)
	{
		ASSERT(::IsWindow(hWnd));
		return ::GetDCEx(hWnd, hRgnClip, flags);
	}

	BOOL LockWindowUpdate(BOOL bLock = TRUE)
	{
		ASSERT(::IsWindow(hWnd));
		return ::LockWindowUpdate(bLock ? hWnd : NULL);
	}

	BOOL RedrawWindow(LPCRECT lpRectUpdate = NULL, HRGN hRgnUpdate = NULL, UINT flags = RDW_INVALIDATE | RDW_UPDATENOW | RDW_ERASE)
	{
		ASSERT(::IsWindow(hWnd));
		return ::RedrawWindow(hWnd, lpRectUpdate, hRgnUpdate, flags);
	}

	// Timer Functions

	UINT_PTR SetTimer(UINT nIDEvent, UINT nElapse)
	{
		ASSERT(::IsWindow(hWnd));
		return ::SetTimer(hWnd, nIDEvent, nElapse, NULL);
	}

	BOOL KillTimer(UINT nIDEvent)
	{
		ASSERT(::IsWindow(hWnd));
		return ::KillTimer(hWnd, nIDEvent);
	}

	// Window State Functions

	BOOL IsWindowEnabled() const
	{
		ASSERT(::IsWindow(hWnd));
		return ::IsWindowEnabled(hWnd);
	}

	BOOL EnableWindow(BOOL bEnable = TRUE)
	{
		ASSERT(::IsWindow(hWnd));
		return ::EnableWindow(hWnd, bEnable);
	}

	HWND SetActiveWindow()
	{
		ASSERT(::IsWindow(hWnd));
		return ::SetActiveWindow(hWnd);
	}

	HWND SetCapture()
	{
		ASSERT(::IsWindow(hWnd));
		return ::SetCapture(hWnd);
	}

	HWND SetFocus()
	{
		ASSERT(::IsWindow(hWnd));
		return ::SetFocus(hWnd);
	}

	// Dialog-Box Item Functions

	BOOL CheckDlgButton(int nIDButton, UINT nCheck)
	{
		ASSERT(::IsWindow(hWnd));
		return ::CheckDlgButton(hWnd, nIDButton, nCheck);
	}

	BOOL CheckRadioButton(int nIDFirstButton, int nIDLastButton, int nIDCheckButton)
	{
		ASSERT(::IsWindow(hWnd));
		return ::CheckRadioButton(hWnd, nIDFirstButton, nIDLastButton, nIDCheckButton);
	}

	int DlgDirList(LPSTR lpPathSpec, int nIDListBox, int nIDStaticPath, UINT nFileType)
	{
		ASSERT(::IsWindow(hWnd));
		return ::DlgDirList(hWnd, lpPathSpec, nIDListBox, nIDStaticPath, nFileType);
	}

	int DlgDirListComboBox(LPSTR lpPathSpec, int nIDComboBox, int nIDStaticPath, UINT nFileType)
	{
		ASSERT(::IsWindow(hWnd));
		return ::DlgDirListComboBox(hWnd, lpPathSpec, nIDComboBox, nIDStaticPath, nFileType);
	}

	BOOL DlgDirSelect(LPSTR lpString, int nCount, int nIDListBox)
	{
		ASSERT(::IsWindow(hWnd));
		return ::DlgDirSelectEx(hWnd, lpString, nCount, nIDListBox);
	}

	BOOL DlgDirSelectComboBox(LPSTR lpString, int nCount, int nIDComboBox)
	{
		ASSERT(::IsWindow(hWnd));
		return ::DlgDirSelectComboBoxEx(hWnd, lpString, nCount, nIDComboBox);
	}

	UINT GetDlgItemInt(int nID, BOOL* lpTrans = NULL, BOOL bSigned = TRUE) const
	{
		ASSERT(::IsWindow(hWnd));
		return ::GetDlgItemInt(hWnd, nID, lpTrans, bSigned);
	}

	UINT GetDlgItemText(int nID, LPSTR lpStr, int nMaxCount) const
	{
		ASSERT(::IsWindow(hWnd));
		return ::GetDlgItemText(hWnd, nID, lpStr, nMaxCount);
	}

	HWND GetNextDlgGroupItem(HWND hWndCtl, BOOL bPrevious = FALSE) const
	{
		ASSERT(::IsWindow(hWnd));
		return ::GetNextDlgGroupItem(hWnd, hWndCtl, bPrevious);
	}

	HWND GetNextDlgTabItem(HWND hWndCtl, BOOL bPrevious = FALSE) const
	{
		ASSERT(::IsWindow(hWnd));
		return ::GetNextDlgTabItem(hWnd, hWndCtl, bPrevious);
	}

	UINT IsDlgButtonChecked(int nIDButton) const
	{
		ASSERT(::IsWindow(hWnd));
		return ::IsDlgButtonChecked(hWnd, nIDButton);
	}

	LRESULT SendDlgItemMessage(int nID, UINT message, WPARAM wParam = 0, LPARAM lParam = 0)
	{
		ASSERT(::IsWindow(hWnd));
		return ::SendDlgItemMessage(hWnd, nID, message, wParam, lParam);
	}

	BOOL SetDlgItemInt(int nID, UINT nValue, BOOL bSigned = TRUE)
	{
		ASSERT(::IsWindow(hWnd));
		return ::SetDlgItemInt(hWnd, nID, nValue, bSigned);
	}

	BOOL SetDlgItemText(int nID, LPCSTR lpszString)
	{
		ASSERT(::IsWindow(hWnd));
		return ::SetDlgItemText(hWnd, nID, lpszString);
	}

	// Scrolling Functions

	int GetScrollPos(int nBar) const
	{
		ASSERT(::IsWindow(hWnd));
		return ::GetScrollPos(hWnd, nBar);
	}

	BOOL GetScrollRange(int nBar, LPINT lpMinPos, LPINT lpMaxPos) const
	{
		ASSERT(::IsWindow(hWnd));
		return ::GetScrollRange(hWnd, nBar, lpMinPos, lpMaxPos);
	}

	BOOL ScrollWindow(int xAmount, int yAmount, LPCRECT lpRect = NULL, LPCRECT lpClipRect = NULL)
	{
		ASSERT(::IsWindow(hWnd));
		return ::ScrollWindow(hWnd, xAmount, yAmount, lpRect, lpClipRect);
	}

	int ScrollWindowEx(int dx, int dy, LPCRECT lpRectScroll, LPCRECT lpRectClip, HRGN hRgnUpdate, LPRECT lpRectUpdate, UINT uFlags)
	{
		ASSERT(::IsWindow(hWnd));
		return ::ScrollWindowEx(hWnd, dx, dy, lpRectScroll, lpRectClip, hRgnUpdate, lpRectUpdate, uFlags);
	}

	int ScrollWindowEx(int dx, int dy, UINT uFlags, LPCRECT lpRectScroll = NULL, LPCRECT lpRectClip = NULL, HRGN hRgnUpdate = NULL, LPRECT lpRectUpdate = NULL)
	{
		ASSERT(::IsWindow(hWnd));
		return ::ScrollWindowEx(hWnd, dx, dy, lpRectScroll, lpRectClip, hRgnUpdate, lpRectUpdate, uFlags);
	}

	int SetScrollPos(int nBar, int nPos, BOOL bRedraw = TRUE)
	{
		ASSERT(::IsWindow(hWnd));
		return ::SetScrollPos(hWnd, nBar, nPos, bRedraw);
	}

	BOOL SetScrollRange(int nBar, int nMinPos, int nMaxPos, BOOL bRedraw = TRUE)
	{
		ASSERT(::IsWindow(hWnd));
		return ::SetScrollRange(hWnd, nBar, nMinPos, nMaxPos, bRedraw);
	}

	BOOL ShowScrollBar(UINT nBar, BOOL bShow = TRUE)
	{
		ASSERT(::IsWindow(hWnd));
		return ::ShowScrollBar(hWnd, nBar, bShow);
	}

	BOOL EnableScrollBar(UINT uSBFlags, UINT uArrowFlags = ESB_ENABLE_BOTH)
	{
		ASSERT(::IsWindow(hWnd));
		return ::EnableScrollBar(hWnd, uSBFlags, uArrowFlags);
	}

	// Window Access Functions

	HWND ChildWindowFromPoint(POINT point) const
	{
		ASSERT(::IsWindow(hWnd));
		return ::ChildWindowFromPoint(hWnd, point);
	}

	HWND ChildWindowFromPointEx(POINT point, UINT uFlags) const
	{
		ASSERT(::IsWindow(hWnd));
		return ::ChildWindowFromPointEx(hWnd, point, uFlags);
	}

	HWND GetTopWindow() const
	{
		ASSERT(::IsWindow(hWnd));
		return ::GetTopWindow(hWnd);
	}

	HWND GetWindow(UINT nCmd) const
	{
		ASSERT(::IsWindow(hWnd));
		return ::GetWindow(hWnd, nCmd);
	}

	HWND GetLastActivePopup() const
	{
		ASSERT(::IsWindow(hWnd));
		return ::GetLastActivePopup(hWnd);
	}

	BOOL IsChild(HWND hWnd) const
	{
		ASSERT(::IsWindow(hWnd));
		return ::IsChild(hWnd, hWnd);
	}

	HWND GetParent() const
	{
		ASSERT(::IsWindow(hWnd));
		return ::GetParent(hWnd);
	}

	HWND SetParent(HWND hWndNewParent)
	{
		ASSERT(::IsWindow(hWnd));
		return ::SetParent(hWnd, hWndNewParent);
	}

	// Window Tree Access

	int GetDlgCtrlID() const
	{
		ASSERT(::IsWindow(hWnd));
		return ::GetDlgCtrlID(hWnd);
	}

	int SetDlgCtrlID(int nID)
	{
		ASSERT(::IsWindow(hWnd));
		return (int)::SetWindowLong(hWnd, GWL_ID, nID);
	}

	HWND GetDlgItem(int nID) const
	{
		ASSERT(::IsWindow(hWnd));
		return ::GetDlgItem(hWnd, nID);
	}

	// Alert Functions

	BOOL FlashWindow(BOOL bInvert)
	{
		ASSERT(::IsWindow(hWnd));
		return ::FlashWindow(hWnd, bInvert);
	}

	int MessageBox(LPCSTR lpszText, LPCSTR lpszCaption = "", UINT nType = MB_OK)
	{
		ASSERT(::IsWindow(hWnd));
		return ::MessageBox(hWnd, lpszText, lpszCaption, nType);
	}

	// Clipboard Functions

	BOOL ChangeClipboardChain(HWND hWndNewNext)
	{
		ASSERT(::IsWindow(hWnd));
		return ::ChangeClipboardChain(hWnd, hWndNewNext);
	}

	HWND SetClipboardViewer()
	{
		ASSERT(::IsWindow(hWnd));
		return ::SetClipboardViewer(hWnd);
	}

	BOOL OpenClipboard()
	{
		ASSERT(::IsWindow(hWnd));
		return ::OpenClipboard(hWnd);
	}

	// Caret Functions

	BOOL CreateCaret(HBITMAP hBitmap)
	{
		ASSERT(::IsWindow(hWnd));
		return ::CreateCaret(hWnd, hBitmap, 0, 0);
	}

	BOOL CreateSolidCaret(int nWidth, int nHeight)
	{
		ASSERT(::IsWindow(hWnd));
		return ::CreateCaret(hWnd, (HBITMAP)0, nWidth, nHeight);
	}

	BOOL CreateGrayCaret(int nWidth, int nHeight)
	{
		ASSERT(::IsWindow(hWnd));
		return ::CreateCaret(hWnd, (HBITMAP)1, nWidth, nHeight);
	}

	BOOL HideCaret()
	{
		ASSERT(::IsWindow(hWnd));
		return ::HideCaret(hWnd);
	}

	BOOL ShowCaret()
	{
		ASSERT(::IsWindow(hWnd));
		return ::ShowCaret(hWnd);
	}

#ifdef _INC_SHELLAPI
	// Drag-Drop Functions
	void DragAcceptFiles(BOOL bAccept = TRUE)
	{
		ASSERT(::IsWindow(hWnd));
		::DragAcceptFiles(hWnd, bAccept);
	}
#endif

	// Icon Functions

	HICON SetIcon(HICON hIcon, BOOL bBigIcon = TRUE)
	{
		ASSERT(::IsWindow(hWnd));
		return (HICON)::SendMessage(hWnd, WM_SETICON, bBigIcon, (LPARAM)hIcon);
	}

	HICON GetIcon(BOOL bBigIcon = TRUE) const
	{
		ASSERT(::IsWindow(hWnd));
		return (HICON)::SendMessage(hWnd, WM_GETICON, bBigIcon, 0);
	}

	// Help Functions

	BOOL WinHelp(LPCSTR lpszHelp, UINT nCmd = HELP_CONTEXT, DWORD dwData = 0)
	{
		ASSERT(::IsWindow(hWnd));
		return ::WinHelp(hWnd, lpszHelp, nCmd, dwData);
	}

	BOOL SetWindowContextHelpId(DWORD dwContextHelpId)
	{
		ASSERT(::IsWindow(hWnd));
		return ::SetWindowContextHelpId(hWnd, dwContextHelpId);
	}

	DWORD GetWindowContextHelpId() const
	{
		ASSERT(::IsWindow(hWnd));
		return ::GetWindowContextHelpId(hWnd);
	}

	// Hot Key Functions

	int SetHotKey(WORD wVirtualKeyCode, WORD wModifiers)
	{
		ASSERT(::IsWindow(hWnd));
		return (int)::SendMessage(hWnd, WM_SETHOTKEY, MAKEWORD(wVirtualKeyCode, wModifiers), 0);
	}

	DWORD GetHotKey() const
	{
		ASSERT(::IsWindow(hWnd));
		return static_cast<DWORD>(::SendMessage(hWnd, WM_GETHOTKEY, 0, 0));
	}

protected:
	bool bAlwaysOnTop;
	HWND hDesktop;
};

#endif // __LSWINBASEX_H
