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
#include "core.hpp"

//
// GetShellFolderPath
//
// Given a CLSID, returns the given shell folder.
// cchPath must be atleast equal to MAX_PATH.
// Does NOT quote spaces, because often times the first thing that is done with
// the returned path is to append another string to it, which doesn't work with
// quotes.
//
bool GetShellFolderPath(int nFolder, LPTSTR ptzPath, size_t cchPath)
{
	ASSERT(cchPath >= MAX_PATH);
    ASSERT_ISWRITEDATA(ptzPath, cchPath);

	IMalloc* pMalloc;
	bool bReturn = false;

    // SHGetSpecialFolderPath is not available on Win95
    // use SHGetSpecialFolderLocation and SHGetPathFromIDList instead
    if (SUCCEEDED(SHGetMalloc(&pMalloc)))
    {
        LPITEMIDLIST pidl;

        if (SUCCEEDED(SHGetSpecialFolderLocation(NULL, nFolder, &pidl)))
        {
            bReturn = SHGetPathFromIDList(pidl, ptzPath) ? true : false;
            
            if (bReturn)
            {
                PathAddBackslashEx(ptzPath, cchPath);
            }
            
            pMalloc->Free(pidl);
        }

        pMalloc->Release();
    }
    
    return bReturn;
}


//
// PathAddBackslashEx
//
// Checked version of PathAddBackslash which also handles quoted paths
//
// Return values:  S_OK          - backslash appended
//                 S_FALSE       - path already ended with a backslash
//                 E_OUTOFMEMORY - buffer too small
//                 E_FAIL        - other failure (invalid input string)
//
HRESULT PathAddBackslashEx(LPTSTR ptzPath, size_t cchPath)
{
    ASSERT(cchPath > 0); ASSERT(cchPath <= STRSAFE_MAX_CCH);
    ASSERT_ISWRITEDATA(ptzPath, cchPath);

    HRESULT hr = E_FAIL;
    size_t cchCurrentLength = 0;

    if (SUCCEEDED(StringCchLength(ptzPath, cchPath, &cchCurrentLength)))
    {
        bool bHasQuote = false;
        LPTSTR ptzEnd = ptzPath + cchCurrentLength;
        
        if ((ptzEnd > ptzPath) && (*(ptzEnd-1) == _T('\"')))
        {
            --ptzEnd;
            bHasQuote = true;
        }

        if (ptzEnd > ptzPath)
        {
            if (*(ptzEnd-1) != _T('\\'))
            {
                if (cchPath - cchCurrentLength > 1)
                {
                    if (bHasQuote)
                    {
                        *(ptzEnd+1) = *ptzEnd;
                    }
                    
                    *ptzEnd = _T('\\');
                    
                    if (bHasQuote)
                    {
                        ++ptzEnd;
                    }
                    
                    ASSERT((ptzEnd - ptzPath) < cchPath);
                    *(ptzEnd+1) = _T('\0');
                    
                    hr = S_OK;
                }
                else
                {
                    hr = E_OUTOFMEMORY;
                }
            }
            else
            {
                hr = S_FALSE;
            }
        }
    }

    return hr;
}


//
// GetSystemString
//
bool GetSystemString(DWORD dwCode, LPTSTR ptzBuffer, size_t cchBuffer)
{
    ASSERT_ISWRITEDATA(ptzBuffer, cchBuffer);

    return (0 != FormatMessage(
        FORMAT_MESSAGE_FROM_SYSTEM |
        FORMAT_MESSAGE_IGNORE_INSERTS,
        NULL,
        dwCode,
        0,
        ptzBuffer,
        cchBuffer,
        NULL
        ));
}
