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
/****************************************************************************
****************************************************************************/
#ifndef __COMMON_H
#define __COMMON_H

#pragma warning(disable: 4786) // STL naming warnings
#pragma warning(disable: 4503) // STL naming warnings

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#ifndef STRICT
#define STRICT
#endif
#ifndef NOCRYPT
#define NOCRYPT
#endif

#ifdef _WINDOWS_
	#error WINDOWS.H already included.  Do not #include <windows.h> with common.h
#endif

#include <windows.h>

// Common includes
#include <stdlib.h>
#include <shellapi.h>
#include <shlwapi.h>
#include <basetyps.h>
#include <tchar.h>

// Include debugging macros
#ifdef _DEBUG
#define _LSDEBUG
#endif
#include "lsdebug.h"

#define INIT_STRUCT_CBSIZE(structure) memset(&structure, 0, sizeof(structure)); \
								      structure.cbSize = sizeof(structure);

#define INIT_STRUCT_DWSIZE(structure)	memset(&structure, 0, sizeof(structure)); \
                                        structure.dwSize = sizeof(structure);

#define INIT_STRUCT(structure) INIT_STRUCT_CBSIZE(structure);

#ifndef MAX_LINE_LENGTH
#define MAX_LINE_LENGTH				4096
#endif // MAX_LINE_LENGTH

#include "safeptr.h"

/*#include "..\lsapi\lsapi.h"
 
//
// Redefines windows Multimon functions to our own
// just makes it easier to convert existing code
//
#define GetSystemMetrics    LSGetSystemMetrics
#define MonitorFromWindow   LSMonitorFromWindow
#define MonitorFromRect     LSMonitorFromRect
#define MonitorFromPoint    LSMonitorFromPoint
#undef GetMonitorInfo
#define GetMonitorInfo      LSGetMonitorInfo
#define EnumDisplayMonitors LSEnumDisplayMonitors
#undef EnumDisplayDevices
#define EnumDisplayDevices  LSEnumDisplayDevices
 
#include "macros.h"*/

#endif // __COMMON_H
