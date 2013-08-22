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

#if !defined(COMMON_H)
#define COMMON_H

#include <tchar.h>
#include <windows.h>
#include <windowsx.h>
#include "lsapi.h"
#include "trace.h"


// Module name and version
#define MODULE_AUTHORS "Litestep Development Team"
#define MODULE_NAME "Tutorial"
#define MODULE_VERSION "1.0"

// GetWindowLongPtr index where a module window's 'this' pointer is stored
#define GWLP_THIS 0

// Maximum string lengths
#define MAX_ENUM_NAME 64
#define MAX_REVID 64
#define MAX_TEXT 256


#endif // COMMON_H
