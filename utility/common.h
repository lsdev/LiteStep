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

// Build options
#include "../litestep/buildoptions.h"

#ifdef _MSC_VER
#  pragma warning(disable: 4786) // STL naming warnings
#  pragma warning(disable: 4503) // STL naming warnings
#endif

// _WINDOWS_ is used by MSVC, _WINDOWS_H is the MinGW variant
#if !defined (_WINDOWS_) && !defined(_WINDOWS_H)
#  define WIN32_LEAN_AND_MEAN
#  define STRICT
#  define NOCRYPT
#  include <windows.h>
#endif // _WINDOWS_

#include <stdlib.h>
#include <tchar.h>

#include <memory>

#include "debug.hpp"
#include "../lsapi/lsapidefines.h"


#endif // __COMMON_H
