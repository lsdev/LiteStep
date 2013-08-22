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

#include <stdio.h>
#include <windows.h>
#include <windowsx.h>
#include "lsapi.h"
#include "Clock.h"
#include "msgcrack.h"
#include "trace.h"
#include "utility.h"
#include "version.h"


// Module's window class name
#define WINDOW_CLASS "LitestepClock"

// Index into the window data where we stash our 'this' pointer
#define GWLP_THIS 0

// Maximum length of a formatted time string
#define MAX_TIME 64

// Litestep messages that we want to listen for. The last element in this array
// must be a zero.
UINT gMessages[] = { LM_GETREVID, 0 };


Clock::Clock(HINSTANCE hInstance)
{
    // Read configuration first since we need it to create the window
    ReadConfig();
    
    // Register the window class
    WNDCLASSEX wc;
    
    wc.cbSize = sizeof(WNDCLASSEX);
    wc.style = 0;
    wc.lpfnWndProc = WindowProcProxy;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = sizeof(Clock *);
    wc.hInstance = hInstance;
    wc.hIcon = NULL;
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH) (COLOR_WINDOW + 1);
    wc.lpszMenuName = NULL;
    wc.lpszClassName = WINDOW_CLASS;
    wc.hIconSm = NULL;
    
    if (!RegisterClassEx(&wc))
    {
        TRACE1("RegisterClassEx failed, GetLastError returns %d", GetLastError());
    }
    
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
        TRACE1("CreateWindowEx failed, GetLastError returns %d", GetLastError());
    }
    
    // Register for Litestep messages that we're interested in
    SendMessage(GetLitestepWnd(), LM_REGISTERMESSAGE, (WPARAM) mWindow, (LPARAM) gMessages);
    
    // Adjust the window's z-order
    SetAlwaysOnTop(mAlwaysOnTop);
    
    // Make the window sticky so that the VWM won't move it
    SetSticky(TRUE);
    
    // Show the window if it should be visible
    SetVisible(mVisible);
    
    // Save the DLL instance handle for later
    mInstance = hInstance;
}


Clock::~Clock()
{
    // Unregister Litestep messages
    SendMessage(GetLitestepWnd(), LM_UNREGISTERMESSAGE, (WPARAM) mWindow, (LPARAM) gMessages);
    
    // Destroy the window
    DestroyWindow(mWindow);
    
    // Unregister the window class
    UnregisterClass(WINDOW_CLASS, mInstance);
}


void Clock::MoveBy(int x, int y)
{
    SetWindowPos(mWindow, NULL,
        mX + x, mY + y, 0, 0,
        SWP_NOACTIVATE | SWP_NOSIZE | SWP_NOZORDER);
}


void Clock::MoveTo(int x, int y)
{
    SetWindowPos(mWindow, NULL,
        x, y, 0, 0,
        SWP_NOACTIVATE | SWP_NOSIZE | SWP_NOZORDER);
}


int Clock::OnGetRevID(char *buffer)
{
    return sprintf(buffer, "%s %s (%s)", MODULE_NAME, MODULE_VERSION, MODULE_AUTHORS);
}


void Clock::OnMove(int x, int y)
{
    mX = x;
    mY = y;
}


void Clock::OnPaint()
{
    PAINTSTRUCT ps;
    HDC hDC;
    
    hDC = BeginPaint(mWindow, &ps);
    
    // Get the current time formatted as a string
    char time[MAX_TIME];
    GetTimeFormat(LOCALE_USER_DEFAULT, TIME_NOSECONDS, NULL, NULL, time, MAX_TIME);
    
    // Draw the string
    RECT rect;
    GetClientRect(mWindow, &rect);
    DrawText(hDC, time, strlen(time), &rect, DT_CENTER | DT_SINGLELINE | DT_VCENTER);
    
    EndPaint(mWindow, &ps);
}


void Clock::OnSize(UINT state, int width, int height)
{
    mWidth = width;
    mHeight = height;
    
    // Repaint
    InvalidateRect(mWindow, NULL, TRUE);
}


void Clock::OnTimer(UINT id)
{
    // Repaint
    InvalidateRect(mWindow, NULL, TRUE);
}


void Clock::ReadConfig()
{
    mAlwaysOnTop = GetRCBoolDef("ClockAlwaysOnTop", FALSE);
    mVisible = !GetRCBoolDef("ClockStartHidden", FALSE);
    mX = GetRCInt("ClockX", 0);
    mY = GetRCInt("ClockY", 0);
    mWidth = GetRCInt("ClockWidth", 0);
    mHeight = GetRCInt("ClockHeight", 0);
}


void Clock::ResizeBy(int width, int height)
{
    SetWindowPos(mWindow, NULL,
        0, 0, mWidth + width, mHeight + height,
        SWP_NOACTIVATE | SWP_NOMOVE | SWP_NOZORDER);
}


void Clock::ResizeTo(int width, int height)
{
    SetWindowPos(mWindow, NULL,
        0, 0, width, height,
        SWP_NOACTIVATE | SWP_NOMOVE | SWP_NOZORDER);
}


void Clock::SetAlwaysOnTop(BOOL alwaysOnTop)
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


void Clock::SetSticky(BOOL sticky)
{
    // In order to determine whether a window is sticky, the VWM looks at the
    // window's user data field. If its a special value (the "magic dword"),
    // the window is sticky, otherwise it's not.
    SetWindowLongPtr(mWindow, GWLP_USERDATA, sticky ? MAGIC_DWORD : 0);
}


void Clock::SetVisible(BOOL visible)
{
    // Show or hide the window
    ShowWindow(mWindow, visible ? SW_SHOWNOACTIVATE : SW_HIDE);
    mVisible = visible;
}


LRESULT Clock::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
        DISPATCH_MSG(LM_GETREVID, wParam, lParam, OnGetRevID);
        DISPATCH_MSG(WM_MOVE, wParam, lParam, OnMove);
        DISPATCH_MSG(WM_PAINT, wParam, lParam, OnPaint);
        DISPATCH_MSG(WM_SIZE, wParam, lParam, OnSize);
        DISPATCH_MSG(WM_TIMER, wParam, lParam, OnTimer);
    }
    
    return DefWindowProc(mWindow, message, wParam, lParam);
}


LRESULT Clock::WindowProcProxy(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    // Get a 'this' pointer from the window data
    Clock *clock = (Clock *) GetWindowLongPtr(hWnd, GWLP_THIS);
    
    if (message == WM_NCCREATE)
    {
        // WM_NCCREATE is our first chance to get our 'this' pointer, which
        // was passed in to CreateWindowEx
        clock = (Clock *) ((LPCREATESTRUCT) lParam)->lpCreateParams;
        
        if (clock)
        {
            // Create is still inside CreateWindowEx, so mWindow hasn't been
            // assigned. Assign it here so we can use it in the WM_[NC]CREATE
            // handler.
            clock->mWindow = hWnd;
            
            // Put a 'this' pointer in the window data so we can get it later
            SetWindowLongPtr(hWnd, GWLP_THIS, (LONG_PTR) clock);
        }
    }
    
    if (clock)
    {
        // Relay the message
        return clock->WindowProc(message, wParam, lParam);
    }
    
    // Any messages sent before WM_NCCREATE wind up here
    return DefWindowProc(hWnd, message, wParam, lParam);
}
