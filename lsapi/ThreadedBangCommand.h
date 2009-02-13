//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//
// This is a part of the Litestep Shell source code.
//
// Copyright (C) 1997-2009  LiteStep Development Team
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
#if !defined(THREADEDBANGCOMMAND_H)
#define THREADEDBANGCOMMAND_H

#include "../utility/base.h"
#include "../utility/core.hpp"


class ThreadedBangCommand : public CountedBase
{
public:
    ThreadedBangCommand(HWND hCaller, LPCSTR pszName, LPCSTR pszParams)
    :m_hCaller(hCaller)
    {
        ASSERT(NULL != pszName);
        
        // pszName is guaranteed to be non-NULL
        StringCchCopy(m_szName, MAX_BANGCOMMAND, pszName);
        
        if (pszParams)
        {
            StringCchCopy(m_szParams, MAX_BANGARGS, pszParams);
        }
        else
        {
            m_szParams[0] = '\0';
        }
    }
    
    void Execute()
    {
        // Cannot use ParseBangCommand here because that would expand variables
        // again - and some themes rely on the fact that they are expanded only
        // once. Besides, it would create inconsistent behavior.
        InternalExecuteBangCommand(m_hCaller, m_szName, m_szParams);
    }
    
private:
    char m_szName[MAX_BANGCOMMAND];
    char m_szParams[MAX_BANGARGS];
    HWND m_hCaller;
};

#endif // THREADEDBANGCOMMAND_H
