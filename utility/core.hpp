//
//  This is a part of the LiteStep Shell source code.
//
//  Copyright (C) 1997-2003 The LiteStep Development Team.
//
//
//  This program is free software; you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation; either version 2 of the License, or
//  (at your option) any later version.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with this program; if not, write to the Free Software
//  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
//
#ifndef LS_CORE_HPP_INCLUDED
#define LS_CORE_HPP_INCLUDED

#include "common.h"

//
// The "backbone" (aka essential tools).
// Mostly included by .cpp files
//

#include <shellapi.h>

// mainly used for Path helper functions
#define NO_SHLWAPI_STRFCNS
#include <shlwapi.h>

#include "../litestep/resource.h"
#include "../lsapi/lsapi.h"
#include "localization.h"
#include "safeptr.h"
#include "safestr.h"

#endif // !defined(LS_CORE_HPP_INCLUDED)
