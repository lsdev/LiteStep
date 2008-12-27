//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//
// This is a part of the Litestep Shell source code.
//
// Copyright (C) 1997-2007  Litestep Development Team
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
#if !defined(MACROS_H)
#define MACROS_H

#include "common.h"
#include "../litestep/resource.h"
#include "../lsapi/lsapi.h"

// Macros for Resource strings and localization
#if !defined(LSRESOURCEBUFFER)
#define LSRESOURCEBUFFER
static char resourceTextBuffer[MAX_LINE_LENGTH + 1];
static char resourceTitleBuffer[MAX_LINE_LENGTH + 1];
#endif // LSRESOURCEBUFFER

#define RESOURCE_MSGBOX(instance, id, deftext, title) \
    GetResStr( (instance), (id)                       \
        ,resourceTextBuffer, MAX_LINE_LENGTH          \
        ,deftext);                                    \
    MessageBox( NULL                                  \
        ,resourceTextBuffer, (title)                  \
        ,MB_OK | MB_TOPMOST | MB_ICONEXCLAMATION )

#define RESOURCE_MSGBOX_T(instance, id, deftext, titleid, deftitle) \
    GetResStr( (instance), (id)                                     \
        ,resourceTextBuffer, MAX_LINE_LENGTH                        \
        ,deftext );                                                 \
    GetResStr( (instance), (titleid)                                \
        ,resourceTitleBuffer, MAX_LINE_LENGTH                       \
        ,deftitle );                                                \
    MessageBox( NULL                                                \
        ,resourceTextBuffer, resourceTitleBuffer                    \
        ,MB_OK | MB_TOPMOST | MB_ICONEXCLAMATION )

#define RESOURCE_STR(instance, id, deftext)  \
    GetResStr( (instance), (id)              \
        ,resourceTextBuffer, MAX_LINE_LENGTH \
        ,deftext )

#define RESOURCE_STREX GetResStrEx

#define RESOURCE_TITLE(instance, id, deftext) \
    GetResStr( (instance), (id)               \
        ,resourceTitleBuffer, MAX_LINE_LENGTH \
        ,deftext )

// Code for multimonitor systems
#define SCREEN_LEFT GetSystemMetrics(SM_XVIRTUALSCREEN)
#define SCREEN_TOP GetSystemMetrics(SM_YVIRTUALSCREEN)
#define IS_MULTI_MON (GetSystemMetrics(SM_CMONITORS) > 1)
#define SCREEN_WIDTH (IS_MULTI_MON ? GetSystemMetrics(SM_CXVIRTUALSCREEN) : GetSystemMetrics(SM_CXSCREEN))
#define SCREEN_HEIGHT (IS_MULTI_MON ? GetSystemMetrics(SM_CYVIRTUALSCREEN) : GetSystemMetrics(SM_CYSCREEN))

// Converts coordinates properly, taking into account for negative values
// that are relative to the right/bottom of the screen
#define CONVERT_COORDINATE_WIDTH(x) ((x) + ((x) < 0 ? SCREEN_WIDTH : 0))
#define CONVERT_COORDINATE_HEIGHT(y) ((y) + ((y) < 0 ? SCREEN_HEIGHT : 0))
#define CONVERT_COORDINATE_X(x) (SCREEN_LEFT + CONVERT_COORDINATE_WIDTH((x)))
#define CONVERT_COORDINATE_Y(y) (SCREEN_TOP + CONVERT_COORDINATE_HEIGHT((y)))

#endif // MACROS_H
