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


// Pointer to the module instance
Label *gLabel;


// Litestep calls this function after it loads the module. This is where the
// module should register bang commands and create windows. If initialization
// succeeds, return zero. If an error occurs, return a nonzero value.
extern "C" int initModuleEx(HWND hwndParent, HINSTANCE hInstance, const char *path)
{
    TRACE1("%s: initModuleEx", MODULE_NAME);
    
    // Ignore attempts to load the module more than once
    if (gLabel)
    {
        return 1;
    }
    
    // Register the module window class
    Label::RegisterWindowClass(hInstance);
    
    // Create the single global instance of the module
    gLabel = new Label;
    
    if (!gLabel->Create(hInstance))
    {
        return 1;
    }
    
    // Register bang commands
    AddBangCommand("!TutorialAlwaysOnTop", BangAlwaysOnTop);
    AddBangCommand("!TutorialHide", BangHide);
    AddBangCommand("!TutorialMoveBy", BangMoveBy);
    AddBangCommand("!TutorialMoveTo", BangMoveTo);
    AddBangCommand("!TutorialPinToDesktop", BangPinToDesktop);
    AddBangCommand("!TutorialResizeBy", BangResizeBy);
    AddBangCommand("!TutorialResizeTo", BangResizeTo);
    AddBangCommand("!TutorialSetImage", BangSetImage);
    AddBangCommand("!TutorialSetText", BangSetText);
    AddBangCommand("!TutorialShow", BangShow);
    AddBangCommand("!TutorialToggle", BangToggle);
    AddBangCommand("!TutorialToggleAlwaysOnTop", BangToggleAlwaysOnTop);
    
    return 0;
}


// Litestep calls this function before it unloads the module. This function
// should unregister bang commands and destroy windows.
extern "C" void quitModule(HINSTANCE hInstance)
{
    TRACE1("%s: quitModule", MODULE_NAME);
    
    // Unregister bang commands
    RemoveBangCommand("!TutorialAlwaysOnTop");
    RemoveBangCommand("!TutorialHide");
    RemoveBangCommand("!TutorialMoveBy");
    RemoveBangCommand("!TutorialMoveTo");
    RemoveBangCommand("!TutorialPinToDesktop");
    RemoveBangCommand("!TutorialResizeBy");
    RemoveBangCommand("!TutorialResizeTo");
    RemoveBangCommand("!TutorialSetImage");
    RemoveBangCommand("!TutorialSetText");
    RemoveBangCommand("!TutorialShow");
    RemoveBangCommand("!TutorialToggle");
    RemoveBangCommand("!TutorialToggleAlwaysOnTop");
    
    // Destroy the module instance
    gLabel->Destroy();
    delete gLabel;
    gLabel = NULL;
    
    // Unregister the module window class
    Label::UnregisterWindowClass(hInstance);
}
