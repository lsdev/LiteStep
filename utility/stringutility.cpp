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
#include "stringutility.h"
#include "common.h"


//
// WCSFromMBS
//
wchar_t *WCSFromMBS(const char *pszMBS)
{
    if (pszMBS != nullptr)
    {
        size_t nLen = strlen(pszMBS) + 1;
        wchar_t *pwzWCS = new wchar_t[nLen];
        MultiByteToWideChar(CP_ACP, 0, pszMBS, -1, pwzWCS, (int)nLen);
        return pwzWCS;
    }
    return nullptr;
}


//
// MBSFromWCS
//
char *MBSFromWCS(const wchar_t *pwzWCS)
{
    if (nullptr != pwzWCS)
    {
        size_t nLen = wcslen(pwzWCS) + 1;
        char *pszMBS = new char[nLen];
        WideCharToMultiByte(CP_ACP, 0, pwzWCS, -1, pszMBS, (int)nLen, "?", nullptr);
        return pszMBS;
    }
    return nullptr;
}
