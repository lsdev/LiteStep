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
#ifndef __BASE_H
#define __BASE_H

#include "../utility/common.h"

class Base
{
protected:
	ULONG __BaseRefCount;
	CRITICAL_SECTION __BaseCritSec;

	virtual ~Base()
	{
		DeleteCriticalSection(&__BaseCritSec);
		FillMemory(&__BaseCritSec, sizeof(__BaseCritSec), 0x00);
	}

	inline ULONG BaseAddRef()
	{
		InterlockedIncrement((LONG *) & __BaseRefCount);
		return (ULONG)__BaseRefCount;
	}

	inline ULONG BaseRelease()
	{
		return (ULONG)InterlockedDecrement((LONG *) & __BaseRefCount);
	}

	inline void Lock()
	{
		EnterCriticalSection(&__BaseCritSec);
	}

	inline void Unlock()
	{
		LeaveCriticalSection(&__BaseCritSec);
	}

public:

	Base()
	{
		__BaseRefCount = 1;
		InitializeCriticalSection(&__BaseCritSec);
	}
};


class CountedBase: public Base
{
public:
	ULONG AddRef()
	{
		return BaseAddRef();
	};
	ULONG Release()
	{
		ULONG cRefs = BaseRelease();

		if (cRefs == 0)
		{
			delete this;
		}

		return cRefs;
	};
};

#endif // __BASE_H
