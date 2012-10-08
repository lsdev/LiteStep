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
#include "DataStore.h"


DataStore::DataStore()
{
    // do nothing
}


DataStore::~DataStore()
{
    Clear();
}


void DataStore::Clear()
{
    DataHolderMap::iterator iter = m_dhmData.begin();
    
    while (iter != m_dhmData.end())
    {
        delete iter->second;
        ++iter;
    }
    
    m_dhmData.clear();
}


size_t DataStore::Count()
{
    return m_dhmData.size();
}


BOOL DataStore::StoreData(WORD wIdent, void *pvData, WORD wLength)
{
    BOOL bReturn = FALSE;
    
    if (pvData != NULL && wLength > 0)
    {
        DataHolderMap::iterator iter = m_dhmData.find(wIdent);
        
        if (iter == m_dhmData.end())
        {
            DataHolder* pdhData = new DataHolder(pvData, wLength);
            
            if (pdhData != NULL)
            {
                m_dhmData[wIdent] = pdhData;
                bReturn = TRUE;
            }
        }
    }
    
    return bReturn;
}


BOOL DataStore::ReleaseData(WORD wIdent, void *pvData, WORD wLength)
{
    BOOL bReturn = FALSE;
    
    if (pvData != NULL && wLength > 0)
    {
        DataHolderMap::iterator iter = m_dhmData.find(wIdent);
        
        if (iter != m_dhmData.end())
        {
            DataHolder* pdhData = iter->second;
            
            if (pdhData != NULL)
            {
                pdhData->GetData(pvData, wLength);
                delete pdhData;
                bReturn = TRUE;
            }
            
            m_dhmData.erase(iter);
        }
    }
    
    return bReturn;
}


DataHolder::DataHolder()
: m_wLength(0), m_pvData(NULL)
{
    // do nothing
}


DataHolder::DataHolder(void *pvData, WORD wLength)
{
    if (pvData == NULL || wLength == 0)
    {
        m_wLength = 0;
        m_pvData = NULL;
    }
    else
    {
        m_pvData = new BYTE[wLength];
        
        if (m_pvData != NULL)
        {
            m_wLength = wLength;
            memcpy(m_pvData, pvData, wLength);
        }
    }
}


DataHolder::~DataHolder()
{
    if (m_pvData)
    {
        delete [] m_pvData;
    }
}


int DataHolder::GetData(void *pvData, WORD wLength)
{
    int nReturn = 0;
    
    if (pvData != NULL && wLength > 0 &&
        m_pvData != NULL && m_wLength > 0)
    {
        if (wLength > m_wLength)
        {
            wLength = m_wLength;
        }
        
        memcpy(pvData, m_pvData, wLength);
        nReturn = wLength;
    }
    
    return nReturn;
}


int DataHolder::SetData(void *pvData, WORD wLength)
{
    int nReturn = 0;
    
    if ((pvData == NULL) || (wLength == 0))
    {
        if (m_pvData)
        {
            delete [] m_pvData;
            m_pvData = NULL;
            m_wLength = 0;
        }
    }
    else if (pvData != NULL && wLength > 0)
    {
        if (m_pvData == NULL)
        {
            m_pvData = new BYTE[wLength];
        }
        else
        {
            delete [] m_pvData;
            m_pvData = new BYTE[wLength];
        }
        
        if (m_pvData != NULL)
        {
            memcpy(m_pvData, pvData, wLength);
            m_wLength = wLength;
            nReturn = m_wLength;
        }
    }
    
    return nReturn;
}
