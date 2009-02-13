//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//
// This is a part of the Litestep Shell source code.
//
// Copyright (C) 1997-2009  LiteStep Development Team
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

#include "../lsapi/lsapi.h"
#include "../litestep/resource.h"

//
// Macros for resource strings and localization
//

#if !defined(LSRESOURCEBUFFER)
#define LSRESOURCEBUFFER
static char resourceTextBuffer[MAX_LINE_LENGTH + 1] = { 0 };
static char resourceTitleBuffer[MAX_LINE_LENGTH + 1] = { 0 };
#endif // LSRESOURCEBUFFER

#define RESOURCE_MSGBOX_F(title, extra_flags)                   \
    MessageBox(NULL, resourceTextBuffer, title,                 \
        MB_TOPMOST | extra_flags)

#define RESOURCE_MSGBOX(instance, id, deftext, title)           \
    GetResStr( (instance), (id)                                 \
        ,resourceTextBuffer, MAX_LINE_LENGTH                    \
        ,deftext);                                              \
    RESOURCE_MSGBOX_F(title, MB_OK | MB_ICONEXCLAMATION)

#define RESOURCE_MSGBOX_T(instance, id, deftext, titleid, deftitle) \
    GetResStr( (instance), (id)                                     \
        ,resourceTextBuffer, MAX_LINE_LENGTH                        \
        ,deftext );                                                 \
    GetResStr( (instance), (titleid)                                \
        ,resourceTitleBuffer, MAX_LINE_LENGTH                       \
        ,deftitle );                                                \
    RESOURCE_MSGBOX_F(resourceTitleBuffer, MB_OK | MB_ICONEXCLAMATION)

#define RESOURCE_STR(instance, id, deftext)  \
    GetResStr( (instance), (id)              \
        ,resourceTextBuffer, MAX_LINE_LENGTH \
        ,deftext )

#define RESOURCE_STREX GetResStrEx

#define RESOURCE_TITLE(instance, id, deftext) \
    GetResStr( (instance), (id)               \
        ,resourceTitleBuffer, MAX_LINE_LENGTH \
        ,deftext )

#endif // MACROS_H
