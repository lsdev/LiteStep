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
#ifndef __CRITICALSECTION_H
#define __CRITICALSECTION_H

class CriticalSection
{
    CriticalSection(const CriticalSection& rhs);
    CriticalSection& operator=(const CriticalSection& rhs);
    
    CRITICAL_SECTION m_CritSection;
    
public:
    CriticalSection()
    {
        InitializeCriticalSection(&m_CritSection);
    }
    
    ~CriticalSection()
    {
        DeleteCriticalSection (&m_CritSection);
    }
    
    void Acquire()
    {
        EnterCriticalSection(&m_CritSection);
    }
    
    void Release()
    {
        LeaveCriticalSection(&m_CritSection);
    }
};


class Lock
{
	Lock(const Lock& rhs);
	Lock& operator=(const Lock& rhs);

	CriticalSection& m_cs;

public:
	Lock(CriticalSection& cs) : m_cs(cs)
	{
		m_cs.Acquire();
	}

	~Lock()
	{
		m_cs.Release();
	}
};

class Block
{
	Block(const Block& rhs);
	Block& operator=(const Block& rhs);

	UINT& m_Count;

public:
	Block(UINT& cnt) : m_Count(cnt)
	{
		m_Count++;
	}

	~Block()
	{
		m_Count--;
	}

	bool IsBlocked()
	{
		return m_Count > 1;
	}
};


#endif // __CRITICALSECTION_H
