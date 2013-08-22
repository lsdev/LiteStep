// Hello Example
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
#include "trace.h"
#include "version.h"


// Litestep calls this function after it loads the module. This is where the
// module should register bang commands and create windows. If initialization
// succeeds, return zero. If an error occurs, return a nonzero value.
extern "C" int initModuleEx(HWND hwndParent, HINSTANCE hInstance, const char *path)
{
    TRACE1("%s: initModuleEx", MODULE_NAME);
    
    // Register bang commands
    AddBangCommand("!Hello", BangHello);
    
    return 0;
}


// Litestep calls this function before it unloads the module. This function
// should unregister bang commands and destroy windows.
extern "C" void quitModule(HINSTANCE hInstance)
{
    TRACE1("%s: quitModule", MODULE_NAME);
    
    // Unregister bang commands
    RemoveBangCommand("!Hello");
}
