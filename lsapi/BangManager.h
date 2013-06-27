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
#if !defined(BANGMANAGER_H)
#define BANGMANAGER_H

#include "BangCommand.h"
#include "lsapidefines.h"
#include "../utility/criticalsection.h"
#include "../utility/stringutility.h"
#include <map>
#include <string>

/**
 * Manages bang commands.
 */
class BangManager
{
private:
    /** Maps bang command names to Bang objects. */
    typedef std::map<std::string, Bang*, stringicmp> BangMap;
    
    /** List of bang commands indexed by name */
    BangMap bang_map;
    
    /** Critical section for serializing access to data members */
    mutable CriticalSection m_cs;
    
    // Not implemented
    BangManager(const BangManager& rhs);
    BangManager& operator=(const BangManager& rhs);
    
public:
    /** Constructor */
    BangManager();
    
    /** Destructor */
    virtual ~BangManager();
    
    /**
     * Adds a bang command to the list.
     *
     * @param  pszName  bang command name
     * @param  pbbBang  Bang object that implements the bang command
     * @return <code>TRUE</code> if the operation succeeds or
     *         <code>FALSE</code> otherwise
     */
    BOOL AddBangCommand(LPCSTR pszName, Bang *pbbBang);
    
    /**
     * Removes a bang command from the list.
     *
     * @param   pszName  bang command name
     * @return  <code>TRUE</code> if the operation succeeds or
     *          <code>FALSE</code> otherwise
     */
    BOOL RemoveBangCommand(LPCSTR pszName);
    
    /**
     * Removes all bang commands from the list.
     */
    void ClearBangCommands();
    
    /**
     * Executes a bang command with the specified parameters.
     *
     * @param  pszName     bang command name
     * @param  hCaller     handle to owner window
     * @param  pszParams   command-line arguments
     * @return <code>TRUE</code> if the operation succeeds or
     *         <code>FALSE</code> otherwise
     */
    BOOL ExecuteBangCommand(LPCSTR pszName, HWND hCaller, LPCSTR pszParams);
    
    /**
     * Calls a callback function once for each bang command in the list.
     * Continues so long as the callback function returns <code>TRUE</code>.
     *
     * @param   pfnCallback  callback function
     * @param   lParam       parameter passed to callback function
     * @return  <code>S_OK</code> if all bang commands were enumerated,
     *          <code>S_FALSE</code> if the callback function returned
     *          <code>FALSE</code>, or an error code
     */
    HRESULT EnumBangs(LSENUMBANGSV2PROC pfnCallback, LPARAM lParam) const;
};

#endif // BANGMANAGER_H
