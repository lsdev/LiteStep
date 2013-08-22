// Tutorial
// Copyright (C) 2006 Litestep Development Team
//
// This program is free software; you can redistribute it and/or modify it under
// the terms of the GNU General Public License as published by the Free Software
// Foundation; either version 2 of the License, or (at your option) any later
// version.
//
// This program is distributed in the hope that it will be useful, but WITHOUT
// ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
// FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License along with
// this program; if not, write to the Free Software Foundation, Inc.,
// 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.

#include "common.h"
#include "Label.h"
#include "msgcrack.h"
#include "utility.h"


// Litestep messages that we're interested in
UINT gMessages[] = { LM_GETREVID, 0 };

// Text to display
TCHAR gText[] = "Hello!";

// Label window class
#define WINDOW_CLASS "TutorialLabel"


Label::Label()
{
    // Position and Size
    mAlwaysOnTop = true;
    mVisible = true;
    mX = 0;
    mY = 0;
    mWidth = 80;
    mHeight = 40;
    
    // Window
    mWindow = NULL;
}


Label::~Label()
{
    Destroy();
}


bool Label::Create(HINSTANCE hInstance)
{
    // Create the window
    mWindow = CreateWindowEx(WS_EX_TOOLWINDOW,
        WINDOW_CLASS, NULL,
        WS_POPUP,
        mX, mY,
        mWidth, mHeight,
        NULL, NULL,
        hInstance, this);
    
    if (!mWindow)
    {
        TRACE2("%s: CreateWindowEx failed, GetLastError returns %d", MODULE_NAME, GetLastError());
        return false;
    }
    
    // Register for Litestep messages that we're interested in
    SendMessage(GetLitestepWnd(), LM_REGISTERMESSAGE, (WPARAM) mWindow, (LPARAM) gMessages);
    
    // Adjust the window's z-order, make it sticky, and show it
    SetAlwaysOnTop(mAlwaysOnTop);
    SetSticky(true);
    SetVisible(mVisible);
    
    return true;
}


void Label::Destroy()
{
    if (mWindow)
    {
        // Unregister Litestep messages
        SendMessage(GetLitestepWnd(), LM_UNREGISTERMESSAGE, (WPARAM) mWindow, (LPARAM) gMessages);
        
        // Destroy the window
        DestroyWindow(mWindow);
    }
}


void Label::DrawBackground(HDC hDC)
{
    RECT rect;
    
    // Fill background with default window color
    GetClientRect(mWindow, &rect);
    FillRect(hDC, &rect, GetSysColorBrush(COLOR_WINDOW));
}


void Label::DrawImage(HDC hDC, int x, int y)
{
    DrawIcon(hDC, x, y, LoadIcon(NULL, IDI_INFORMATION));
}


void Label::DrawText(HDC hDC, int x, int y)
{
    // Set up the DC
    int oldBkMode = SetBkMode(hDC, TRANSPARENT);
    COLORREF oldTextColor = SetTextColor(hDC, GetSysColor(COLOR_WINDOWTEXT));
    HFONT oldFont = SelectFont(hDC, GetStockFont(DEFAULT_GUI_FONT));
    
    // Draw the text
    TextOut(hDC, x, y, gText, StrLen(gText));
    
    // Restore the DC
    SetBkMode(hDC, oldBkMode);
    SetTextColor(hDC, oldTextColor);
    SelectFont(hDC, oldFont);
}


void Label::GetTextExtent(HDC hDC, int *width, int *height)
{
    SIZE size;
    
    // Select the font into the DC, then get the text's extent
    HFONT oldFont = SelectFont(hDC, GetStockFont(DEFAULT_GUI_FONT));
    GetTextExtentPoint32(hDC, gText, StrLen(gText), &size);
    SelectFont(hDC, oldFont);
    
    *width = size.cx;
    *height = size.cy;
}


int Label::OnGetRevID(LPTSTR buffer)
{
    // This is the string displayed in Litestep's about box
    StrPrintf(buffer, MAX_REVID, "%s %s", MODULE_NAME, MODULE_VERSION);
    return StrLen(buffer);
}


void Label::OnMove(int x, int y)
{
    // Keep position up to date
    mX = x;
    mY = y;
}


void Label::OnPaint()
{
    PAINTSTRUCT ps;
    HDC hDC;
    
    hDC = BeginPaint(mWindow, &ps);
    
    // Call Paint to do the real work
    Paint(hDC);
    
    EndPaint(mWindow, &ps);
}


void Label::OnSize(UINT state, int width, int height)
{
    // Keep size up to date
    mWidth = width;
    mHeight = height;
    
    // Force a repaint
    Repaint();
}


void Label::Paint(HDC hDC)
{
    // Figure out how much space we have to work with
    int x = 0;
    int y = 0;
    int w = mWidth;
    int h = mHeight;
    
    // Compute the size of the image
    int wImage = GetSystemMetrics(SM_CXICON);
    int hImage = GetSystemMetrics(SM_CYICON);
    
    // Compute the amount of space between the image and the text
    int wGap = 4;
    
    // Compute the size of the text
    int wText = 0;
    int hText = 0;
    
    GetTextExtent(hDC, &wText, &hText);
    
    // Compute the size of the content area (image and text)
    int wContent = wImage + wGap + wText;
    int hContent = max(hImage, hText);
    
    // Align the content area horizontally and vertically
    int xContent = x + (w - wContent) / 2;
    int yContent = y + (h - hContent) / 2;
    
    // Place the image and the text
    int xImage = xContent;
    int yImage = yContent + (hContent - hImage) / 2;
    int xText = xContent + wImage + wGap;
    int yText = yContent + (hContent - hText) / 2;
    
    // Paint the background
    DrawBackground(hDC);
    
    // Clip the content area
    IntersectClipRect(hDC, x, y, x + w, y + h);
    
    // Paint the image
    DrawImage(hDC, xImage, yImage);
    
    // Paint the text
    DrawText(hDC, xText, yText);
}


void Label::RegisterWindowClass(HINSTANCE hInstance)
{
    WNDCLASSEX wc;
    
    wc.cbSize = sizeof(WNDCLASSEX);
    wc.style = 0;
    wc.lpfnWndProc = WindowProcProxy;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = sizeof(Label *);
    wc.hInstance = hInstance;
    wc.hIcon = NULL;
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = NULL;
    wc.lpszMenuName = NULL;
    wc.lpszClassName = WINDOW_CLASS;
    wc.hIconSm = NULL;
    
    if (!RegisterClassEx(&wc))
    {
        TRACE2("%s: RegisterClassEx failed, GetLastError returns %d", MODULE_NAME, GetLastError());
    }
}


void Label::Repaint()
{
    InvalidateRect(mWindow, NULL, FALSE);
}


void Label::SetAlwaysOnTop(bool alwaysOnTop)
{
    // Switch from WS_POPUP to WS_CHILD so that we can change our parent
    ModifyWindowStyle(mWindow, WS_POPUP, WS_CHILD);
    
    // To be always on top we can't have a parent. To be pinned to the desktop
    // we need the desktop to be our parent.
    SetParent(mWindow, alwaysOnTop ? NULL : GetLitestepDesktopWindow());
    
    if (alwaysOnTop)
    {
        // If we're going to be always on top, switch back to being a WS_POPUP
        // window. A pinned to desktop window can remain WS_CHILD.
        ModifyWindowStyle(mWindow, WS_CHILD, WS_POPUP);
    }
    
    // Adjust the window's z-order
    SetWindowPos(mWindow, alwaysOnTop ? HWND_TOPMOST : HWND_NOTOPMOST,
        0, 0, 0, 0, SWP_NOACTIVATE | SWP_NOMOVE | SWP_NOSIZE);
    
    mAlwaysOnTop = alwaysOnTop;
}


void Label::SetSticky(bool sticky)
{
    // In order to determine whether a window is sticky, the VWM looks at the
    // window's user data field. If its a special value (the "magic dword"),
    // the window is sticky, otherwise it's not.
    SetWindowLongPtr(mWindow, GWLP_USERDATA, sticky ? MAGIC_DWORD : 0);
}


void Label::SetVisible(bool visible)
{
    // Show or hide the window
    ShowWindow(mWindow, visible ? SW_SHOWNOACTIVATE : SW_HIDE);
    mVisible = visible;
}


void Label::UnregisterWindowClass(HINSTANCE hInstance)
{
    UnregisterClass(WINDOW_CLASS, hInstance);
}


LRESULT Label::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
        DISPATCH_MSG(LM_GETREVID, wParam, lParam, OnGetRevID);
        DISPATCH_MSG(WM_MOVE, wParam, lParam, OnMove);
        DISPATCH_MSG(WM_PAINT, wParam, lParam, OnPaint);
        DISPATCH_MSG(WM_SIZE, wParam, lParam, OnSize);
    }
    
    return DefWindowProc(mWindow, message, wParam, lParam);
}


LRESULT Label::WindowProcProxy(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    // Get a 'this' pointer from the window data
    Label *label = (Label *) GetWindowLongPtr(hWnd, GWLP_THIS);
    
    if (message == WM_NCCREATE)
    {
        // WM_NCCREATE is our first chance to get our 'this' pointer, which
        // was passed in to CreateWindowEx
        label = (Label *) ((LPCREATESTRUCT) lParam)->lpCreateParams;
        
        if (label)
        {
            // Create is still inside CreateWindowEx, so mWindow hasn't been
            // assigned. Assign it here so we can use it in the WM_[NC]CREATE
            // handler.
            label->mWindow = hWnd;
            
            // Put a 'this' pointer in the window data so we can get it later
            SetWindowLongPtr(hWnd, GWLP_THIS, (LONG_PTR) label);
        }
    }
    
    if (label)
    {
        // Relay the message
        return label->WindowProc(message, wParam, lParam);
    }
    
    // Any messages sent before WM_NCCREATE wind up here
    return DefWindowProc(hWnd, message, wParam, lParam);
}
