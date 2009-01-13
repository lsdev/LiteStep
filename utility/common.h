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
#if !defined(COMMON_H)
#define COMMON_H

// _WINDOWS_ is used by MSVC, _WINDOWS_H is the MinGW variant
#if defined (_WINDOWS_) || defined(_WINDOWS_H)
#error Do not include Windows.h before this header
#endif


#if !defined(_WIN32_WINNT)
#define _WIN32_WINNT    0x0600
#define _WIN32_IE       0x0603
#endif

// _WINDOWS_ is used by MSVC, _WINDOWS_H is the MinGW variant
#if !defined (_WINDOWS_) && !defined(_WINDOWS_H)
#  ifndef NOMINMAX
#    define NOMINMAX
#  endif
#  define WIN32_LEAN_AND_MEAN
#  define STRICT
#  define NOCRYPT
#  include <windows.h>

// These GDI macros trigger "smaller type check" errors unless fixed like this
#  undef GetRValue
#  undef GetGValue
#  undef GetBValue
#  define GetRValue(rgb)    ((BYTE)( (rgb) & 0x0000FF))
#  define GetGValue(rgb)    ((BYTE)(((rgb) & 0x00FF00) >> 8))
#  define GetBValue(rgb)    ((BYTE)(((rgb) & 0xFF0000) >> 16))
#endif // _WINDOWS_

#include <tchar.h>

#include "../litestep/buildoptions.h"
#include "debug.hpp"
#include "../lsapi/lsapidefines.h"

// Count array elements
#define COUNTOF(array)  (sizeof(array)/sizeof(array[0]))

#endif // COMMON_H
