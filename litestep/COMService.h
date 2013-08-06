//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//
// This is a part of the Litestep Shell source code.
//
// Copyright (C) 1998 (e)
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
#if !defined(COMSERVICE_H)
#define COMSERVICE_H

#include "../utility/IService.h"
#include "COMFactory.h"
#include <thread>

class COMService : public IService
{
public:
    COMService();
    ~COMService();
    
    // IService
public:
    HRESULT Start() override;
    HRESULT Stop() override;
    HRESULT Recycle() override;

private:
    void ThreadProc();

private:
    // COM worker thread
    std::thread m_COMThread;

    // Necesary since XP does not support GetThreadId
    DWORD m_dwThreadID;
};

#endif // COMSERVICE_H
