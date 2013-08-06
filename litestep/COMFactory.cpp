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
#include "../utility/common.h"
#include "../utility/debug.hpp"
#include "../utility/shellhlp.h"
#include "COMFactory.h"
#include "DesktopWallpaper.h"
#include <Shobjidl.h>


//
// COMFactory
//
COMFactory::COMFactory()
{
    m_uRefCount = 1;
}


//
// ~COMFactory
// 
COMFactory::~COMFactory()
{
}


//
// IUnknown::AddRef
// Increments the reference count for an interface on an object.
//
ULONG COMFactory::AddRef()
{
    return InterlockedIncrement(&m_uRefCount);
}


//
// IUnknown::Release
// Decrements the reference count for an interface on an object.
//
ULONG COMFactory::Release()
{
    if (InterlockedDecrement(&m_uRefCount) == 0)
    {
        delete this;
        return 0;
    }

    return m_uRefCount;
}


//
// IUnknown::QueryInterface
// Retrieves pointers to the supported interfaces on an object.
//
HRESULT COMFactory::QueryInterface(REFIID riid, void **ppvObject)
{
    if (ppvObject == nullptr)
    {
        return E_POINTER;
    }

    if (riid == IID_IUnknown)
    {
        *ppvObject = (IUnknown*)this;
    }
    else if (riid == IID_IClassFactory)
    {
        *ppvObject = (IClassFactory*)this;
    }
    else
    {
        *ppvObject = nullptr;
        return E_NOINTERFACE;
    }

    AddRef();
    return S_OK;
}


//
// IClassFactory::CreateInstance
// Creates an instance of the specified objet
// 
HRESULT COMFactory::CreateInstance(IUnknown *pUnkOuter, REFIID riid, void **ppvObject)
{
    if (ppvObject == nullptr)
    {
        return E_POINTER;
    }

    // TODO::Aggregate?
    if (pUnkOuter != nullptr)
    {
        return CLASS_E_NOAGGREGATION;
    }
    
    if (riid == IID_IUnknown)
    {
        *ppvObject = (IUnknown*)(new DesktopWallpaper());
    }
    else if (riid == IID_IDesktopWallpaper)
    {
        *ppvObject = (IDesktopWallpaper*)(new DesktopWallpaper());
    }
    else if (riid == IID_IDesktopWallpaperPrivate)
    {
        *ppvObject = (IDesktopWallpaperPrivate*)(new DesktopWallpaper());
    }
    else
    {
        *ppvObject = nullptr;
        return E_NOINTERFACE;
    }

    // We failed to create the object.
    if (*ppvObject == nullptr)
    {
        return E_OUTOFMEMORY;
    }

    return S_OK;
}


//
// IClassFactory::LockServer
//
//
HRESULT COMFactory::LockServer(BOOL)
{
    return E_NOTIMPL;
}


//
// IClassFactory::IsLocked
// 
//
bool COMFactory::IsLocked()
{
    return false;
}
