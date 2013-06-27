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
#if !defined(DATASTORE_H)
#define DATASTORE_H

#include "../utility/common.h"
#include <map>


/**
 * Item in the data store.
 */
class DataHolder
{
private:

    /** Size of data in bytes */
    WORD m_wLength;
    
    /** Pointer to data */
    BYTE* m_pvData;
    
public:

    /**
     * Constructs an empty DataHolder.
     */
    DataHolder();
    
    /**
     * Constructs a DataHolder containing a copy of the specified data.
     *
     * @param  pvData   pointer to data
     * @param  wLength  size of data in bytes
     */
    DataHolder(void *pvData, WORD wLength);
    
    /**
     * Destructor.
     */
    virtual ~DataHolder();
    
    /**
     * Copies the data into the supplied buffer.
     *
     * @param   pvData   buffer to receive data
     * @param   wLength  maximum number of bytes to copy
     * @return  number of bytes copied
     */
    int GetData(void *pvData, WORD wLength);
    
    /**
     * Replaces the current data with a copy of the specified data.
     *
     * @param   pvData   pointer to data
     * @param   wLength  size of data in bytes
     * @return  number of bytes copied
     */
    int SetData(void *pvData, WORD wLength);
};


/**
 * Manages module data that needs to be preserved across recycles.
 */
class DataStore
{
    /** Maps data identifiers to DataHolders */
    typedef std::map<WORD, DataHolder*> DataHolderMap;
    
    /** List of data item index by identifier */
    DataHolderMap m_dhmData;
    
public:

    /**
     * Constructor.
     */
    DataStore();
    
    /**
     * Destructor.
     */
    virtual ~DataStore();
    
    /**
     * Returns the number of items in the data store.
     */
    size_t Count();
        
    /**
     * Removes all data from the data store.
     */
    void Clear();
    
    /**
     * Retrieves a stored data item and removes it from the data store.
     *
     * @param   wIdent   data identifier
     * @param   pvData   buffer to receive data
     * @param   wLength  maximum number of bytes to copy
     * @return  <code>TRUE</code> if the operation succeeds or
     *          <code>FALSE</code> if no data item with the given
     *          identifier exists
     */
    BOOL ReleaseData(WORD wIdent, void *pvData, WORD wLength);
    
    /**
     * Adds a data item to the data store.
     *
     * @param   wIdent   data identifier
     * @param   pvData   pointer to data
     * @param   wLength  size of data in bytes
     * @return  <code>TRUE</code> if the operation succeeds or
     *          <code>FALSE</code> if another data item with the same
     *          identifier already exists
     */
    BOOL StoreData(WORD wIdent, void *pvData, WORD wLength);
};

#endif // DATASTORE_H
