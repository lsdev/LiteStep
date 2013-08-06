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
#if !defined(COMFACTORY_H)
#define COMFACTORY_H

#include "../utility/common.h"
#include <ShlObj.h>

class COMFactory : public IClassFactory
{
    // Constructor
public:
    COMFactory();

    // Destructor
private:
    virtual ~COMFactory();

public:
    // IUnknown
    ULONG STDMETHODCALLTYPE AddRef();
    STDMETHOD(QueryInterface) (REFIID, void**);
    ULONG STDMETHODCALLTYPE Release();

    // IClassFactory
    STDMETHOD(CreateInstance) (IUnknown*, REFIID, void**);
    STDMETHOD(LockServer) (BOOL);

    // Utility
    bool IsLocked();

private:
    ULONG m_uRefCount;
};

#endif // COMFACTORY_H
