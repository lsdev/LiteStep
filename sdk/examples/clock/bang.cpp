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

#include <windows.h>
#include "lsapi.h"
#include "bang.h"
#include "Clock.h"


// Pointer to module instance
extern Clock *gClock;


void BangAlwaysOnTop(HWND hwndOwner, const char *args)
{
    gClock->SetAlwaysOnTop(TRUE);
}


void BangHide(HWND hwndOwner, const char *args)
{
    gClock->SetVisible(FALSE);
}


void BangMoveBy(HWND hwndOwner, const char *args)
{
    char xBuffer[64];
    char yBuffer[64];
    
    if (GetToken(args, xBuffer, &args, FALSE) && GetToken(args, yBuffer, &args, FALSE))
    {
        gClock->MoveBy(atoi(xBuffer), atoi(yBuffer));
    }
}


void BangMoveTo(HWND hwndOwner, const char *args)
{
    char xBuffer[64];
    char yBuffer[64];
    
    if (GetToken(args, xBuffer, &args, FALSE) && GetToken(args, yBuffer, &args, FALSE))
    {
        gClock->MoveTo(atoi(xBuffer), atoi(yBuffer));
    }
}


void BangPinToDesktop(HWND hwndOwner, const char *args)
{
    gClock->SetAlwaysOnTop(FALSE);
}


void BangResizeBy(HWND hwndOwner, const char *args)
{
    char wBuffer[64];
    char hBuffer[64];
    
    if (GetToken(args, wBuffer, &args, FALSE) && GetToken(args, hBuffer, &args, FALSE))
    {
        gClock->ResizeBy(atoi(wBuffer), atoi(hBuffer));
    }
}


void BangResizeTo(HWND hwndOwner, const char *args)
{
    char wBuffer[64];
    char hBuffer[64];
    
    if (GetToken(args, wBuffer, &args, FALSE) && GetToken(args, hBuffer, &args, FALSE))
    {
        gClock->ResizeTo(atoi(wBuffer), atoi(hBuffer));
    }
}


void BangShow(HWND hwndOwner, const char *args)
{
    gClock->SetVisible(TRUE);
}


void BangToggle(HWND hwndOwner, const char *args)
{
    gClock->SetVisible(!gClock->IsVisible());
}


void BangToggleAlwaysOnTop(HWND hwndOwner, const char *args)
{
    gClock->SetAlwaysOnTop(!gClock->IsAlwaysOnTop());
}
