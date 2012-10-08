//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//
// This is a part of the Litestep Shell source code.
//
// Copyright (C) 1997-2012  LiteStep Development Team
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
#include "DDEStub.h"

HMODULE DDEStub::m_hShDocVw = NULL;
DDEStub::ShellDDEFunc DDEStub::m_pShellDDEInit = NULL;

DDEStub::DDEStub()
{
    // do nothing
}

DDEStub::~DDEStub()
{
    // do nothing
}

HRESULT DDEStub::Start()
{
    HRESULT hr = S_FALSE;
    
    if (FindWindow(_T("Progman"), NULL) == NULL)
    {
        if (!m_hShDocVw)
        {
            m_hShDocVw = LoadLibrary(_T("SHDOCVW.DLL"));
            //m_hShDocVw = GetModuleHandle("shell32.dll"); // win2k
        }
        
        if (m_hShDocVw && !m_pShellDDEInit)
        {
            // Ordinal 0x00BC in shell32.dll
            // Ordinal 0x0076 in shdocvw.dll
            m_pShellDDEInit = (ShellDDEFunc)
                GetProcAddress(m_hShDocVw, (LPCSTR)((long)0x0076));
        }
        
        if (m_pShellDDEInit)
        {
            m_pShellDDEInit(TRUE);
            hr = S_OK;
        }
        else
        {
            hr = E_FAIL;
        }
    }
    
    return hr;
}

HRESULT DDEStub::Stop()
{
    if (m_pShellDDEInit)
    {
        m_pShellDDEInit(FALSE);
    }
    
    if (m_hShDocVw)
    {
        FreeLibrary(m_hShDocVw);
    }
    
    return S_OK;
}
