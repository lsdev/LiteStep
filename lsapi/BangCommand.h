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
#ifndef __BANG_H
#define __BANG_H

#include "../utility/common.h"
#include "lsapidefines.h"
#include "../utility/base.h"
#include <string>

class Bang: public CountedBase
{
public:
	Bang(DWORD dwThread, BangCommand* pfnBang, LPCSTR pszCommand);
	Bang(DWORD dwThread, BangCommandEx* pfnBang, LPCSTR pszCommand);
	~Bang();

	void Execute(HWND hCaller, LPCSTR pszParams);


private:
	DWORD m_dwThreadID;
	bool m_bEX;
	BangCommand* m_bBang;
	BangCommandEx* m_bBangEX;
    std::string m_szCommand;
};

#endif // __BANG_H
