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
BOOL GetShellFolderPath(int nFolder, LPSTR szPath, size_t cchPath)
{
	// LPITEMIDLIST pidl;
	// IMalloc *pMalloc;
	// HRESULT hr;
	BOOL bReturn = FALSE;

	if (IsValidStringPtr(szPath, cchPath) && (cchPath >= MAX_PATH))
	{
        bReturn = SHGetSpecialFolderPath(NULL, szPath, nFolder, FALSE);

        if (bReturn)
        {
            PathAddBackslash(szPath);
            PathQuoteSpaces(szPath);
        }
        
/*        if (SUCCEEDED(SHGetMalloc(&pMalloc)))
		{

			hr = SHGetSpecialFolderLocation(NULL, nFolder, &pidl);

			if (SUCCEEDED(hr))
			{
				bReturn = SHGetPathFromIDList(pidl, szPath);

				if (bReturn)
				{
					PathAddBackslash(szPath);
					PathQuoteSpaces(szPath);
				}

				pMalloc->Free(pidl);
				pMalloc->Release();
			}
		}*/
	}

	return bReturn;
}