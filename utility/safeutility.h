//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//
// This is a part of the Litestep Shell source code.
//
// Copyright (C) 1997-2015  LiteStep Development Team
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
#if !defined(SAFEUTILITY_H)
#define SAFEUTILITY_H

//
// Typesafe method for releasing interface pointers and setting them
// to NULL.
//
template <class T> inline LONG SafeRelease(T *&pUnk)
{
    LONG lr;

    if (pUnk != NULL)
    {
        lr = pUnk->Release();
        pUnk = nullptr;
    }
    else
    {
        lr = 0;
    }

    return lr;
}

//
// Typesafe method for deleting things created with "new" and setting
// them to NULL.
//
template <class T> inline void SafeDelete(T *&pT)
{
    if (pT != NULL)
    {
        delete pT;
        pT = nullptr;
    }
}

#endif
