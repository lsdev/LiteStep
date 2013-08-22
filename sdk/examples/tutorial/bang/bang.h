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

#if !defined(BANG_H)
#define BANG_H

#include "common.h"


// Bang command callback functions
void BangAlwaysOnTop(HWND hwndOwner, LPCTSTR args);
void BangHide(HWND hwndOwner, LPCTSTR args);
void BangMoveBy(HWND hwndOwner, LPCTSTR args);
void BangMoveTo(HWND hwndOwner, LPCTSTR args);
void BangPinToDesktop(HWND hwndOwner, LPCTSTR args);
void BangResizeBy(HWND hwndOwner, LPCTSTR args);
void BangResizeTo(HWND hwndOwner, LPCTSTR args);
void BangSetImage(HWND hwndOwner, LPCTSTR args);
void BangSetText(HWND hwndOwner, LPCTSTR args);
void BangShow(HWND hwndOwner, LPCTSTR args);
void BangToggle(HWND hwndOwner, LPCTSTR args);
void BangToggleAlwaysOnTop(HWND hwndOwner, LPCTSTR args);


#endif // BANG_H
