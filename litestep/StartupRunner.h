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
#ifndef __STARTUPRUNNER_H
#define __STARTUPRUNNER_H

#include "../utility/common.h"

class StartupRunner
{
public:
	StartupRunner();
	virtual ~StartupRunner();

	static DWORD WINAPI Run(void* pvVoid);

private:
	static bool _IsFirstRunThisSession();
	static void _RunRegKeys(HKEY hkParent, LPCTSTR ptzSubKey, DWORD dwFlags);
	static void _RunStartupMenu();
	static void _RunFolderContents(LPCTSTR ptzPath);
};

#endif // __STARTUPRUNNER_H
