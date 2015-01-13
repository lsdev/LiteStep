//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//
// This is a part of the Litestep Shell source code.
//
// Copyright (C) 1997-2013  LiteStep Development Team
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
#if !defined(CORE_HPP)
#define CORE_HPP

#include "common.h"

//
// The "backbone" (aka essential tools).
// Mostly included by .cpp files
//

#include <shellapi.h>

#define STRICT_TYPED_ITEMIDS
#include <ShlObj.h>

// mainly used for Path helper functions
#define STRICT_CONST
#define NO_SHLWAPI_STRFCNS
#include <shlwapi.h>

#define STRSAFE_NO_DEPRECATE
#include <strsafe.h>

// Now first thing after system includes, is our fixup for
// the system headers.
#include "fixup.h"

// Add in debugging support for the rest of our files
#include "debug.hpp"

// Now items that generally most files should have.
#include "../lsapi/lsapi.h"
#include "shellhlp.h"

#endif // CORE_HPP
