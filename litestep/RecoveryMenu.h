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
#if !defined RECOVERYMENU_H
#define RECOVERYMENU_H

#include "../utility/common.h"

#define ID_HOTKEY 1

#define ID_RECYCLE 1
#define ID_QUIT 2
#define ID_TERMINATE 3
#define ID_RUN 4
#define ID_SHUTDOWN 5

LRESULT WINAPI RecoveryMenuWndProc(HWND, UINT, WPARAM, LPARAM);
DWORD WINAPI RecoveryThreadProc(LPVOID);

#endif // RECOVERYMENU_H
