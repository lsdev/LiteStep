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
#ifndef __THREADEDBANGCOMMAND_H
#define __THREADEDBANGCOMMAND_H

#include "bangcommand.h"
#include "../utility/core.hpp"

class ThreadedBangCommand : public CountedBase
{
public:
    ThreadedBangCommand(HWND hCaller, LPCSTR pszParams, Bang* pBang) :
      m_pBang(pBang), m_hCaller(hCaller)
	{
		ASSERT_ISWRITEPTR(pBang);
        ASSERT((pszParams == NULL) || !IsBadStringPtr(pszParams, UINT_MAX));

        m_pBang->AddRef();

        if (pszParams)
		{
			StringCchCopy(m_szParams, MAX_BANGARGS, pszParams);
		}
        else
        {
            m_szParams[0] = '\0';
        }
	};

	~ThreadedBangCommand()
	{
		if (m_pBang)
		{
			m_pBang->Release();
			m_pBang = NULL;
		}
	}

	void Execute()
	{
        if (m_pBang)
		{
			m_pBang->Execute(m_hCaller, m_szParams);
		}
	};

private:
	Bang* m_pBang;

    char m_szParams[MAX_BANGARGS];
	HWND m_hCaller;
};

#endif // __THREADEDBANGCOMMAND_H
