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
#include "shellhlp.h"

//
// GetShellFolderPath
//
// Given a CLSID, returns the given shell folder
// cchPath must be atleast equal to MAX_PATH
//
BOOL GetShellFolderPath(int nFolder, LPTSTR tzPath, size_t cchPath)
{
	LPITEMIDLIST pidl;
	IMalloc* pMalloc;
	BOOL bReturn = FALSE;

	if (IsValidStringPtr(tzPath, cchPath) && (cchPath >= MAX_PATH))
	{
        // SHGetSpecialFolderPath is not available on Win95
        // use SHGetSpecialFolderLocation and SHGetPathFromIDList instead
        if (SUCCEEDED(SHGetMalloc(&pMalloc)))
		{
			if (SUCCEEDED(SHGetSpecialFolderLocation(NULL, nFolder, &pidl)))
			{
				bReturn = SHGetPathFromIDList(pidl, tzPath);

				if (bReturn)
				{
					PathAddBackslash(tzPath);
					PathQuoteSpaces(tzPath);
				}

				pMalloc->Free(pidl);
				pMalloc->Release();
			}
		}
	}

    return bReturn;
}