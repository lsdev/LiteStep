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
#include "LabelManager.h"


// Pointer to manager
extern LabelManager *gLabelManager;


void BangAlwaysOnTop(HWND hwndOwner, LPCTSTR args)
{
    TCHAR name[MAX_LINE_LENGTH];
    
    if (GetToken(args, name, &args, FALSE))
    {
        // Find the label
        Label *label = gLabelManager->FindLabel(name);
        
        if (label)
        {
            // Make it always on top
            label->SetAlwaysOnTop(TRUE);
        }
    }
}


void BangCreate(HWND hwndOwner, LPCTSTR args)
{
    TCHAR name[MAX_LINE_LENGTH];
    
    if (GetToken(args, name, &args, FALSE))
    {
        // Create the label
        gLabelManager->CreateLabel(name);
    }
}


void BangDestroy(HWND hwndOwner, LPCTSTR args)
{
    TCHAR name[MAX_LINE_LENGTH];
    
    if (GetToken(args, name, &args, FALSE))
    {
        // Destroy the label
        gLabelManager->DestroyLabel(name);
    }
}


void BangHide(HWND hwndOwner, LPCTSTR args)
{
    TCHAR name[MAX_LINE_LENGTH];
    
    if (GetToken(args, name, &args, FALSE))
    {
        // Find the label
        Label *label = gLabelManager->FindLabel(name);
        
        if (label)
        {
            // Hide it
            label->SetVisible(FALSE);
        }
    }
}


void BangMoveBy(HWND hwndOwner, LPCTSTR args)
{
    TCHAR name[MAX_LINE_LENGTH];
    TCHAR x[MAX_LINE_LENGTH];
    TCHAR y[MAX_LINE_LENGTH];
    
    if (GetToken(args, name, &args, FALSE) && GetToken(args, x, &args, FALSE) && GetToken(args, y, &args, FALSE))
    {
        // Find the label
        Label *label = gLabelManager->FindLabel(name);
        
        if (label)
        {
            // Move it
            label->MoveBy(atoi(x), atoi(y));
        }
    }
}


void BangMoveTo(HWND hwndOwner, LPCTSTR args)
{
    TCHAR name[MAX_LINE_LENGTH];
    TCHAR x[MAX_LINE_LENGTH];
    TCHAR y[MAX_LINE_LENGTH];
    
    if (GetToken(args, name, &args, FALSE) && GetToken(args, x, &args, FALSE) && GetToken(args, y, &args, FALSE))
    {
        // Find the label
        Label *label = gLabelManager->FindLabel(name);
        
        if (label)
        {
            // Move it
            label->MoveTo(atoi(x), atoi(y));
        }
    }
}


void BangPinToDesktop(HWND hwndOwner, LPCTSTR args)
{
    TCHAR name[MAX_LINE_LENGTH];
    
    if (GetToken(args, name, &args, FALSE))
    {
        // Find the label
        Label *label = gLabelManager->FindLabel(name);
        
        if (label)
        {
            // Pin it to the desktop
            label->SetAlwaysOnTop(FALSE);
        }
    }
}


void BangResizeBy(HWND hwndOwner, LPCTSTR args)
{
    TCHAR name[MAX_LINE_LENGTH];
    TCHAR w[MAX_LINE_LENGTH];
    TCHAR h[MAX_LINE_LENGTH];
    
    if (GetToken(args, name, &args, FALSE) && GetToken(args, w, &args, FALSE) && GetToken(args, h, &args, FALSE))
    {
        // Find the label
        Label *label = gLabelManager->FindLabel(name);
        
        if (label)
        {
            // Resize it
            label->ResizeBy(atoi(w), atoi(h));
        }
    }
}


void BangResizeTo(HWND hwndOwner, LPCTSTR args)
{
    TCHAR name[MAX_LINE_LENGTH];
    TCHAR w[MAX_LINE_LENGTH];
    TCHAR h[MAX_LINE_LENGTH];
    
    if (GetToken(args, name, &args, FALSE) && GetToken(args, w, &args, FALSE) && GetToken(args, h, &args, FALSE))
    {
        // Find the label
        Label *label = gLabelManager->FindLabel(name);
        
        if (label)
        {
            // Resize it
            label->ResizeTo(atoi(w), atoi(h));
        }
    }
}


void BangSetImage(HWND hwndOwner, LPCTSTR args)
{
    TCHAR name[MAX_LINE_LENGTH];
    TCHAR imageFile[MAX_LINE_LENGTH];
    
    if (GetToken(args, name, &args, FALSE) && GetToken(args, imageFile, &args, FALSE))
    {
        // Find the label
        Label *label = gLabelManager->FindLabel(name);
        
        if (label)
        {
            // Set it's image
            label->SetImage(imageFile);
        }
    }
}


void BangSetText(HWND hwndOwner, LPCTSTR args)
{
    TCHAR name[MAX_LINE_LENGTH];
    TCHAR text[MAX_LINE_LENGTH];
    
    if (GetToken(args, name, &args, FALSE) && GetToken(args, text, &args, FALSE))
    {
        // Find the label
        Label *label = gLabelManager->FindLabel(name);
        
        if (label)
        {
            // Set it's text
            label->SetText(text);
        }
    }
}


void BangShow(HWND hwndOwner, LPCTSTR args)
{
    TCHAR name[MAX_LINE_LENGTH];
    
    if (GetToken(args, name, &args, FALSE))
    {
        // Find the label
        Label *label = gLabelManager->FindLabel(name);
        
        if (label)
        {
            // Show it
            label->SetVisible(TRUE);
        }
    }
}


void BangToggle(HWND hwndOwner, LPCTSTR args)
{
    TCHAR name[MAX_LINE_LENGTH];
    
    if (GetToken(args, name, &args, FALSE))
    {
        // Find the label
        Label *label = gLabelManager->FindLabel(name);
        
        if (label)
        {
            // Toggle it's visibility
            label->SetVisible(!label->IsVisible());
        }
    }
}


void BangToggleAlwaysOnTop(HWND hwndOwner, LPCTSTR args)
{
    TCHAR name[MAX_LINE_LENGTH];
    
    if (GetToken(args, name, &args, FALSE))
    {
        // Find the label
        Label *label = gLabelManager->FindLabel(name);
        
        if (label)
        {
            // Toggle it's z-order
            label->SetAlwaysOnTop(!label->IsAlwaysOnTop());
        }
    }
}
