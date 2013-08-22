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

#include "bang.h"
#include "common.h"
#include "Label.h"


// Pointer to module instance
extern Label *gLabel;


void BangAlwaysOnTop(HWND hwndOwner, LPCTSTR args)
{
    gLabel->SetAlwaysOnTop(TRUE);
}


void BangHide(HWND hwndOwner, LPCTSTR args)
{
    gLabel->SetVisible(FALSE);
}


void BangMoveBy(HWND hwndOwner, LPCTSTR args)
{
    char xBuffer[MAX_LINE_LENGTH];
    char yBuffer[MAX_LINE_LENGTH];
    
    if (GetToken(args, xBuffer, &args, FALSE) && GetToken(args, yBuffer, &args, FALSE))
    {
        gLabel->MoveBy(atoi(xBuffer), atoi(yBuffer));
    }
}


void BangMoveTo(HWND hwndOwner, LPCTSTR args)
{
    char xBuffer[MAX_LINE_LENGTH];
    char yBuffer[MAX_LINE_LENGTH];
    
    if (GetToken(args, xBuffer, &args, FALSE) && GetToken(args, yBuffer, &args, FALSE))
    {
        gLabel->MoveTo(atoi(xBuffer), atoi(yBuffer));
    }
}


void BangPinToDesktop(HWND hwndOwner, LPCTSTR args)
{
    gLabel->SetAlwaysOnTop(FALSE);
}


void BangResizeBy(HWND hwndOwner, LPCTSTR args)
{
    char wBuffer[MAX_LINE_LENGTH];
    char hBuffer[MAX_LINE_LENGTH];
    
    if (GetToken(args, wBuffer, &args, FALSE) && GetToken(args, hBuffer, &args, FALSE))
    {
        gLabel->ResizeBy(atoi(wBuffer), atoi(hBuffer));
    }
}


void BangResizeTo(HWND hwndOwner, LPCTSTR args)
{
    char wBuffer[MAX_LINE_LENGTH];
    char hBuffer[MAX_LINE_LENGTH];
    
    if (GetToken(args, wBuffer, &args, FALSE) && GetToken(args, hBuffer, &args, FALSE))
    {
        gLabel->ResizeTo(atoi(wBuffer), atoi(hBuffer));
    }
}


void BangSetImage(HWND hwndOwner, LPCTSTR args)
{
    TCHAR buffer[MAX_LINE_LENGTH];
    
    if (GetToken(args, buffer, &args, FALSE))
    {
        gLabel->SetImage(buffer);
    }
}


void BangSetText(HWND hwndOwner, LPCTSTR args)
{
    TCHAR buffer[MAX_LINE_LENGTH];
    
    if (GetToken(args, buffer, &args, FALSE))
    {
        gLabel->SetText(buffer);
    }
}


void BangShow(HWND hwndOwner, LPCTSTR args)
{
    gLabel->SetVisible(TRUE);
}


void BangToggle(HWND hwndOwner, LPCTSTR args)
{
    gLabel->SetVisible(!gLabel->IsVisible());
}


void BangToggleAlwaysOnTop(HWND hwndOwner, LPCTSTR args)
{
    gLabel->SetAlwaysOnTop(!gLabel->IsAlwaysOnTop());
}
