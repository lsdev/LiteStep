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
#ifndef __PNG_SUPPORT_H
#define __PNG_SUPPORT_H

#pragma include_alias("zlib.h", "../../zlib/zlib.h")
#pragma comment(linker, "/NODEFAULTLIB:libc.lib")

#include "../utility/common.h"
#include "../../libpng/png.h"

typedef struct _PNGERROR
{
	HWND Wnd;
	jmp_buf ErrorJump;
}
PNGERROR, *PPNGERROR;

void PNGErrorHandler(png_structp PngStruct, png_const_charp Message);
HBITMAP LoadFromPNG(LPCSTR pszFilename);

#endif // __PNG_SUPPORT_H
