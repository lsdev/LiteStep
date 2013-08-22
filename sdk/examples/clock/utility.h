// Clock Example
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

#if !defined(UTILITY_H)
#define UTILITY_H

#include <windows.h>


/**
 * Returns a handle to Litestep's desktop window.
 */
HWND GetLitestepDesktopWindow();

/**
 * Simultaneously removes one set of styles from a window while adding another
 * set.
 *
 * @param hWnd     handle to the window whose style is modified
 * @param toRemove set of styles to remove
 * @param toAdd    set of styles to add
 */
void ModifyWindowStyle(HWND hWnd, UINT toRemove, UINT toAdd);


#endif // UTILITY_H
