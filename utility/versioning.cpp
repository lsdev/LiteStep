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
#include "versioning.h"


static DWORD GetDllVersionWorker(HINSTANCE hDll)
{
    DWORD dwVersion = 0;

    if (hDll)
    {
        DLLGETVERSIONPROC pDllGetVersion;

		pDllGetVersion =
            (DLLGETVERSIONPROC)GetProcAddress(hDll, "DllGetVersion");

		// Because some DLLs may not implement this function, you
		// must test for it explicitly. Depending on the particular 
		// DLL, the lack of a DllGetVersion function may
		// be a useful indicator of the version.
        if (pDllGetVersion)
		{
            DLLVERSIONINFO dvi = { 0 };
            dvi.cbSize = sizeof(DLLVERSIONINFO);
			
            HRESULT hr = (*pDllGetVersion)(&dvi);

			if (SUCCEEDED(hr))
			{
				dwVersion = PACKVERSION(dvi.dwMajorVersion, dvi.dwMinorVersion);
			}
		}

        FreeLibrary(hDll);
    }

    return dwVersion;
}

//
// GetDllVersionA(LPCSTR pszDllName)
//
DWORD GetDllVersionA(LPCSTR pszDllName)
{
	return GetDllVersionWorker(LoadLibraryA(pszDllName));
}


//
// GetDllVersionW(LPCWSTR pwzDllName)
//
DWORD GetDllVersionW(LPCWSTR pwzDllName)
{
    return GetDllVersionWorker(LoadLibraryW(pwzDllName));
}
