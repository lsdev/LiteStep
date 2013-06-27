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
#if !defined(CRITICALSECTION_H)
#define CRITICALSECTION_H

#include "common.h"


//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//
// CriticalSection
//
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


//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//
// Lock
//
class Lock
{
public:
    Lock(CriticalSection& cs) : m_cs(cs)
    {
        m_cs.Acquire();
    }
    
    ~Lock()
    {
        m_cs.Release();
    }
    
protected:
    CriticalSection& m_cs;
    
private:
    // not implemented
    Lock(const Lock& rhs);
    Lock& operator=(const Lock& rhs);
};


//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//
// Block
//
class Block
{
public:
    Block(UINT& cnt) : m_Count(cnt)
    {
        ++m_Count;
    }
    ~Block()
    {
        --m_Count;
    }
    bool IsBlocked()
    {
        return m_Count > 1;
    }
    
protected:
    UINT& m_Count;
    
private:
    // not implemented
    Block(const Block& rhs);
    Block& operator=(const Block& rhs);
};


#endif // CRITICALSECTION_H
